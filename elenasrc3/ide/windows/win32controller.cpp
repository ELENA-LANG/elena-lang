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

// --- Win32Controller ---

Win32Controller :: Win32Controller()
{
}

bool Win32Controller :: execute(path_t path, path_t cmdLine, path_t curDir)
{
   PROCESS_INFORMATION pi;
   STARTUPINFO si;

   ::ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb = sizeof(STARTUPINFO);
   si.hStdOutput = redirectInfo.hStdOut;
   si.hStdInput = redirectInfo.hStdIn;
   si.hStdError = redirectInfo.hStdErr;
   si.wShowWindow = SW_HIDE;
   si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

   // Note that dwFlags must include STARTF_USESHOWWINDOW if we
   // use the wShowWindow flags. This also assumes that the
   // CreateProcess() call will use CREATE_NEW_CONSOLE.

   // Launch the child process.
   if (!::CreateProcess(path, (wchar_t*)cmdLine.str(), nullptr, nullptr, TRUE,
      CREATE_NEW_CONSOLE, nullptr, curDir, &si, &pi))
   {
      return false;
   }
   //_hChildProcess = pi.hProcess;
   ::CloseHandle(pi.hThread);

   return true;
}