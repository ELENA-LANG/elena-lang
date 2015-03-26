//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Output class implementation
//                                              (C)2005-2015, by Alexei Rakov
//                                              (C)2001, by Jeff Lee
//---------------------------------------------------------------------------

#include "winoutput.h"
#include "winideconst.h"
//#include "idesettings.h"

using namespace _GUI_;
using namespace _ELENA_;

inline void destroyHandle(HANDLE& hObject)
{
   if (hObject != NULL) {
      ::CloseHandle(hObject);
      hObject = NULL;
   }
}

// --- Output ---

Output :: Output(Control* owner, Control* receptor)
   : Control(0, 0, 40, 40)
{
   _hStdoutRead = NULL;
   _hProcess = NULL;
   _hEvtStop = NULL;
   _hThread = NULL;
   _dwThreadId = 0;

   _postponedAction = 0;

   _instance = owner->_getInstance();
   _receptor = receptor->getHandle();

   _handle = ::CreateWindowEx(
      0, _T("edit"), _T("output"),
      WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY,
      _left, _top, _width, _height, owner->getHandle(), NULL, _instance, (LPVOID)this);

}

Output :: ~Output()
{
   close();
}

void Output :: close()
{
   if (_hThread != NULL) {
      if (::GetCurrentThreadId() != _dwThreadId) {
         ::SetEvent(_hEvtStop);
         if (::WaitForSingleObject(_hThread, 5000) == WAIT_TIMEOUT) {
            ::TerminateThread(_hThread, (DWORD)-2);
         }
      }
      destroyHandle(_hThread);
   }
   _dwThreadId = 0;
   destroyHandle(_hStdoutRead);
   destroyHandle(_hEvtStop);
   destroyHandle(_hProcess);

   _postponedAction = 0;
}

bool Output :: execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, HANDLE hStdOut)
{
   PROCESS_INFORMATION pi;
   STARTUPINFO si;

   ::ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.hStdOutput = hStdOut;
   si.hStdInput = NULL;
   si.hStdError = NULL;
   si.wShowWindow = SW_HIDE;
   si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

   if (!::CreateProcess(path, (wchar_t*)cmdLine, NULL, NULL, TRUE,
                        CREATE_NEW_CONSOLE, NULL, curDir, &si,&pi))
   {
      return false;
   }
   _hProcess = pi.hProcess;
   ::CloseHandle(pi.hThread);

   return true;
}

bool Output :: execute(const wchar_t* path, const wchar_t* cmdLine, const wchar_t* curDir, int postponedAction)
{
   HANDLE hStdoutReadTmp = NULL;			// parent stdout read handle
   HANDLE hStdoutWrite = NULL;				// child stdout write handle
   SECURITY_ATTRIBUTES sa;

   clear();
   close();

   _postponedAction = postponedAction;

   // Set up the security attributes struct.
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = NULL;
   sa.bInheritHandle = TRUE;

   bool bOk = false;
   while (1) {                                         // executes only one time!!
      // Create a child stdout pipe.
      if (!::CreatePipe(&hStdoutReadTmp, &hStdoutWrite, &sa, 0))
         break;

      // Create new stdout read handle and the stdin write handle.
      // Set the inheritance properties to FALSE. Otherwise, the child
      // inherits the these handles; resulting in non-closeable
      // handles to the pipes being created.
      if (!::DuplicateHandle(::GetCurrentProcess(), hStdoutReadTmp, ::GetCurrentProcess(), &_hStdoutRead,	0, FALSE, DUPLICATE_SAME_ACCESS))
         break;

      destroyHandle(hStdoutReadTmp);

      if (!execute(path, cmdLine, curDir, hStdoutWrite))
         break;

      destroyHandle(hStdoutWrite);

      // Launch a thread to receive output from the child process.
      _hEvtStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
      _hThread = ::CreateThread(NULL, 0, OutputThread, this, 0, &_dwThreadId);
      if (!_hThread)
         break;

      bOk = true;
      break;
   }
   if (!bOk) {
      destroyHandle(hStdoutReadTmp);
      destroyHandle(hStdoutWrite);
      close();
   }
   return bOk;
}

DWORD WINAPI Output :: OutputThread(LPVOID lpvThreadParam)
{
   HANDLE aHandles[2];
   int nRet;
   Output* output = (Output*)lpvThreadParam;

   ::SendMessage(output->_handle, EM_SETREADONLY, FALSE, 1);

   aHandles[0] = output->_hProcess;
   aHandles[1] = output->_hEvtStop;

   for (;;) {
      // redirect stdout till there's no more data.
      nRet = output->redirectStdout();
      if (nRet <= 0)
         break;

      // check if the child process has terminated.
      DWORD dwRc = ::WaitForMultipleObjects(2, aHandles, FALSE, 100);
      if (WAIT_OBJECT_0 == dwRc) {
         nRet = output->redirectStdout();
         if (nRet > 0)
            nRet = 0;

         break;
      }
      if (WAIT_OBJECT_0 + 1 == dwRc) {
         nRet = 1;	// cancelled
         break;
      }
   }
   ::SendMessage(output->_handle, EM_SETREADONLY, TRUE, 1);

   DWORD error = 0;
   GetExitCodeProcess(output->_hProcess, &error);

   if (error==0) {
      output->_notify(output->_receptor, IDM_COMPILER_SUCCESSFUL, output->_postponedAction);
   }
   else if ((int)error==-1) {
      output->_notify(output->_receptor, IDM_COMPILER_WITHWARNING);
   }
   else output->_notify(output->_receptor, IDM_COMPILER_UNSUCCESSFUL, output->_postponedAction);

   output->close();
   return nRet;
}

int Output :: redirectStdout()
{
   wchar_t szOutput[256];
   for (;;) {
      DWORD dwAvail = 0;
      if (!::PeekNamedPipe(_hStdoutRead, NULL, 0, NULL, &dwAvail, NULL))
         break;

      if (!dwAvail)
         return 1;

      DWORD dwRead = 0;
      if (!::ReadFile(_hStdoutRead, (char*)szOutput, (dwAvail > 254 ? 254 : dwAvail),
         &dwRead, NULL) || !dwRead)
      {
         break;
      }
      // !! temporal
      //if (!Settings::unicodeELC) {
         int j;
         char* s = (char*)szOutput;
         for (int i = dwRead - 1 ; i >= 0 ; i--) {
            j = i << 1;
            s[j] = s[i];
            s[j+1] = 0;
         }
      //}
      //else dwRead = (dwRead >> 1);
      szOutput[dwRead] = 0;

      for (size_t i = 0 ; i < dwRead; i++) {
         if (szOutput[i]!=0x0A) {
            ::SendMessage(_handle, WM_CHAR, szOutput[i], 1);
         }
      }
   }
   DWORD dwError = ::GetLastError();
   if (dwError == ERROR_BROKEN_PIPE || dwError == ERROR_NO_DATA) {
      return 0;
   }
   return -1;
}

wchar_t* Output :: getOutput()
{
   int length = (int)SendMessage(_handle, WM_GETTEXTLENGTH, 0, 0);

   if (length > 0) {
      wchar_t* buffer = StringHelper::allocate(length + 1, (const wchar_t*)NULL);

      SendMessage(_handle, WM_GETTEXT, length + 1, (LPARAM)buffer);

      return buffer;
   }
   else return NULL;
}

/*
void Output :: resize()
{
   ::MoveWindow(_self, _left, _top, _width - 10, _height - 8, TRUE);

   refreshClient();
}*/

void Output :: clear()
{
   ::SendMessage(_handle, EM_SETREADONLY, FALSE, 1);
   ::SendMessage(_handle, EM_SETSEL, 0, -1);
   ::SendMessage(_handle, WM_CLEAR, 0, 0);
   ::SendMessage(_handle, EM_SETREADONLY, TRUE, 1);
}
