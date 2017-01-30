//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Redirect Class
//                                              (C)2005-2017, by Alexei Rakov
//                                              (C)2001, by Jeff Lee
//---------------------------------------------------------------------------

#include "winredirect.h"

using namespace _GUI_;

inline void destroyHandle(HANDLE& hObject)
{
   if (hObject != NULL) {
      ::CloseHandle(hObject);
      hObject = NULL;
   }
}

// --- Redirector ---

Redirector :: Redirector(bool readOnly, int waitTime)
{
   _hStdinWrite = NULL;
   _hStdoutRead = NULL;
   _hChildProcess = NULL,
   _hThread = NULL,
   _hEvtStop = NULL;
   _dwThreadId = 0;
   _dwWaitTime = waitTime;
   _readOnly = readOnly;

   _offset = 0;
}

Redirector :: ~Redirector()
{
   close();
}

bool Redirector :: start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir)
{
   HANDLE hStdoutReadTmp;				// parent stdout read handle
   HANDLE hStdoutWrite, hStderrWrite;	// child stdout write handle
   HANDLE hStdinWriteTmp;				// parent stdin write handle
   HANDLE hStdinRead;					// child stdin read handle
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

      if (!_readOnly) {
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

void Redirector :: stop(int exitCode)
{
   if (_hChildProcess) {
      TerminateProcess(_hChildProcess, exitCode);

      WaitForSingleObject(_hChildProcess, INFINITE);
   }      
}

void Redirector :: close()
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

bool Redirector :: start(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir,
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

void Redirector :: flush()
{
   DWORD dwWritten;
   ::WriteFile(_hStdinWrite, _buffer, (int)_offset, &dwWritten, NULL);

   //!! should be moved if dwWritten != _offset
   _offset = 0;
}

bool Redirector :: write(const char* line, size_t length)
{
   if (!_hStdinWrite)
      return false;

   if (_offset + length < BUF_SIZE) {
      memcpy(_buffer + _offset, line, length);
      _offset += length;

      if (line[length - 1] == '\n')
         flush();

      return true;
   }
   else return false;
}

bool Redirector :: write(wchar_t ch)
{
   if (ch == 8) {
      if (_offset > 0) {
         _offset--;

         return true;
      }
      else return false;
   }
   else return write((const char*)&ch, 1);
}

int Redirector :: redirectStdout()
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

DWORD WINAPI Redirector :: OutputThread(LPVOID lpvThreadParam)
{
   HANDLE aHandles[2];
   int nRet;
   Redirector* redirector = (Redirector*)lpvThreadParam;

   aHandles[0] = redirector->_hChildProcess;
   aHandles[1] = redirector->_hEvtStop;

   for (;;)
   {
      // redirect stdout till there's no more data.
      nRet = redirector->redirectStdout();
      if (nRet <= 0)
         break;

      // check if the child process has terminated.
      DWORD dwRc = ::WaitForMultipleObjects(
         2, aHandles, FALSE, redirector->_dwWaitTime);
      if (WAIT_OBJECT_0 == dwRc)		// the child process ended
      {
         nRet = redirector->redirectStdout();
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
   redirector->afterExecution(error);

   // close handles
   redirector->close();
   return nRet;
}

// --- WindowRedirector ---

WindowRedirector :: WindowRedirector(RedirectorListener* target, bool readOnly, int waitTime)
   : Redirector(readOnly, waitTime)
{
   _target = target;
}

void WindowRedirector :: writeStdOut(const char* output)
{
   if(_target)
      _target->onOutput(output);
}

void WindowRedirector :: writeStdError(const char* error)
{
   if (_target)
      _target->onOutput(error);
}

void WindowRedirector :: afterExecution(DWORD exitCode)
{
   if (_target)
      _target->afterExecution(exitCode);
}