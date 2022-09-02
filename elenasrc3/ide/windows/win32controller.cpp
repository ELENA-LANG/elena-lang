//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Controller class and its helpers implementation
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "windows/win32controller.h"

using namespace elena_lang;

inline void destroyHandle(HANDLE& hObject)
{
   if (hObject != NULL) {
      ::CloseHandle(hObject);
      hObject = NULL;
   }
}

// --- Win32Process ---

Win32Process :: Win32Process(int waitTime) :
   _hThread(nullptr),
   _hEvtStop(nullptr),
   _dwThreadId(0),
   _dwWaitTime(waitTime),
   _hStdinWrite(nullptr),
   _hStdoutRead(nullptr),
   _hChildProcess(nullptr)
{
}

Win32Process :: ~Win32Process()
{
   close();
}

void Win32Process :: close()
{
   if (_hThread != NULL) {
      if (::GetCurrentThreadId() != _dwThreadId) {
         ::SetEvent(_hEvtStop);
         if (::WaitForSingleObject(_hThread, 5000) == WAIT_TIMEOUT) {
            writeStdError("The redir thread is dead\r\n");
            ::TerminateThread(_hThread, (DWORD)-2);
         }
      }
      destroyHandle(_hThread);
   }
   _dwThreadId = 0;
   destroyHandle(_hStdoutRead);
   destroyHandle(_hStdinWrite);
   destroyHandle(_hEvtStop);
   destroyHandle(_hChildProcess);
}

int Win32Process :: redirectStdout()
{
   for (;;)
   {
      DWORD dwAvail = 0;
      if (!::PeekNamedPipe(_hStdoutRead, NULL, 0, NULL,
         &dwAvail, NULL))			// error
         break;

      if (!dwAvail)					// not data available
         return 1;

      char buffer[256];
      DWORD dwRead = 0;
      if (!::ReadFile(_hStdoutRead, buffer, min(255, dwAvail),
         &dwRead, NULL) || !dwRead)	// error, the child might ended
         break;

      buffer[dwRead] = 0;
      writeStdOut(buffer);
   }

   DWORD dwError = ::GetLastError();
   if (dwError == ERROR_BROKEN_PIPE ||	// pipe has been ended
      dwError == ERROR_NO_DATA)		// pipe closing in progress
   {
      return 0;	// child process ended
   }

   writeStdError("Read stdout pipe error\r\n");
   return -1;		// os error
}

DWORD WINAPI Win32Process :: OutputThread(LPVOID lpvThreadParam)
{
   HANDLE aHandles[2];
   int nRet;
   Win32Process* process = (Win32Process*)lpvThreadParam;

   aHandles[0] = process->_hChildProcess;
   aHandles[1] = process->_hEvtStop;

   for (;;)
   {
      // redirect stdout till there's no more data.
      nRet = process->redirectStdout();
      if (nRet <= 0)
         break;

      // check if the child process has terminated.
      DWORD dwRc = ::WaitForMultipleObjects(
         2, aHandles, FALSE, process->_dwWaitTime);
      if (WAIT_OBJECT_0 == dwRc)		// the child process ended
      {
         nRet = process->redirectStdout();
         if (nRet > 0)
            nRet = 0;
         break;
      }
      if (WAIT_OBJECT_0 + 1 == dwRc)	// m_hEvtStop was signalled
      {
         nRet = 1;	// cancelled
         break;
      }
   }

   DWORD error = 0;
   GetExitCodeProcess(aHandles[0], &error);
   process->afterExecution(error);

   // close handles
   process->close();
   return nRet;
}

bool Win32Process :: start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir,
   HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr)
{
   PROCESS_INFORMATION pi;
   STARTUPINFO si;

   ::ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.hStdOutput = hStdOut;
   si.hStdInput = hStdIn;
   si.hStdError = hStdErr;
   si.wShowWindow = SW_HIDE;
   si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

   // Note that dwFlags must include STARTF_USESHOWWINDOW if we
   // use the wShowWindow flags. This also assumes that the
   // CreateProcess() call will use CREATE_NEW_CONSOLE.

   // Launch the child process.
   if (!::CreateProcess(path, (wchar_t*)cmdLine, NULL, NULL, TRUE,
      CREATE_NEW_CONSOLE, NULL, curDir, &si, &pi))
   {
      return false;
   }
   _hChildProcess = pi.hProcess;
   ::CloseHandle(pi.hThread);

   return true;
}

bool Win32Process :: start(path_t path, path_t cmdLine, path_t curDir, bool readOnly)
{
   HANDLE hStdoutReadTmp;              // parent stdout read handle
   HANDLE hStdoutWrite, hStderrWrite;  // child stdout write handle
   HANDLE hStdinWriteTmp;				   // parent stdin write handle
   HANDLE hStdinRead;                  // child stdin read handle
   SECURITY_ATTRIBUTES sa;

   close();

   // Set up the security attributes struct.
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;

   bool bOk = false;

   while (1) {                                         // executes only one time!!
      // Create a child stdout pipe.
      if (!::CreatePipe(&hStdoutReadTmp, &hStdoutWrite, &sa, 0))
         break;

      // Create a duplicate of the stdout write handle for the std
      // error write handle. This is necessary in case the child
      // application closes one of its std output handles.
      if (!::DuplicateHandle(::GetCurrentProcess(), hStdoutWrite, ::GetCurrentProcess(), &hStderrWrite,
         0, TRUE, DUPLICATE_SAME_ACCESS))
      {
         break;
      }

      // Create new stdout read handle and the stdin write handle.
      // Set the inheritance properties to FALSE. Otherwise, the child
      // inherits the these handles; resulting in non-closeable
      // handles to the pipes being created.
      if (!::DuplicateHandle(::GetCurrentProcess(), hStdoutReadTmp, ::GetCurrentProcess(), &_hStdoutRead,
         0, FALSE, DUPLICATE_SAME_ACCESS))
      {
         break;
      }

      if (!readOnly) {
         // Create a child stdin pipe.
         if (!::CreatePipe(&hStdinRead, &hStdinWriteTmp, &sa, 0))
            break;

         if (!::DuplicateHandle(::GetCurrentProcess(), hStdinWriteTmp, ::GetCurrentProcess(), &_hStdinWrite,
            0, FALSE,			// make it uninheritable.
            DUPLICATE_SAME_ACCESS))
         {
            break;
         }
      }
      else {
         hStdinRead = NULL;
         hStdinWriteTmp = NULL;
      }

      // Close inheritable copies of the handles we do not want to
      // be inherited.
      destroyHandle(hStdoutReadTmp);
      destroyHandle(hStdinWriteTmp);

      if (!start(path, cmdLine, curDir, hStdoutWrite, hStdinRead, hStderrWrite))
         break;

      destroyHandle(hStdoutWrite);
      destroyHandle(hStdinRead);
      destroyHandle(hStderrWrite);

      // Launch a thread to receive output from the child process.
      _hEvtStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
      _hThread = ::CreateThread(NULL, 0, OutputThread, this, 0, &_dwThreadId);
      if (!_hThread)
         break;

      bOk = true;
      break;
   }
   if (!bOk) {
      DWORD err = ::GetLastError();
      char message[40];
      ::sprintf(message, "Redirect console error: %x\r\n", err);
      writeStdError(message);
      destroyHandle(hStdoutReadTmp);
      destroyHandle(hStdoutWrite);
      destroyHandle(hStderrWrite);
      destroyHandle(hStdinWriteTmp);
      destroyHandle(hStdinRead);
      ::SetLastError(err);
      close();
   }
   return bOk;
}

void Win32Process :: writeStdOut(const char* output)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onOutput(output);
   }
}

void Win32Process :: writeStdError(const char* error)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->onErrorOutput(error);
   }
}

void Win32Process :: afterExecution(DWORD exitCode)
{
   for (auto it = _listeners.start(); !it.eof(); ++it) {
      (*it)->afterExecution(exitCode);
   }
}

void Win32Process :: stop(int exitCode)
{
   if (_hChildProcess) {
      TerminateProcess(_hChildProcess, exitCode);

      WaitForSingleObject(_hChildProcess, INFINITE);
   }
}

void Win32Process :: flush(char* buffer, size_t length)
{
   DWORD dwWritten;
   ::WriteFile(_hStdinWrite, buffer, length, &dwWritten, NULL);
}
