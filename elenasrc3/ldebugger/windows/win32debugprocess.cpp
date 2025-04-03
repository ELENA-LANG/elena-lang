//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger class implementation
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "win32debugprocess.h"

using namespace elena_lang;

bool Win32DebugProcess :: startProcess(const wchar_t* exePath/*, const wchar_t* cmdLine, const wchar_t* appPath,
   StartUpSettings& startUpSettings*/)
{
//   DynamicString<path_c> pathsEnv;
//
//   PROCESS_INFORMATION pi = { nullptr, nullptr, 0, 0 };
//   STARTUPINFO         si;
//   PathString          currentPath;
//   DWORD               flags = DEBUG_PROCESS;
//
//   currentPath.copySubPath(exePath, false);
//
//   memset(&si, 0, sizeof(si));
//
//   si.dwFlags = STARTF_USESHOWWINDOW;
//   si.wShowWindow = SW_SHOWNORMAL;
//
//   if (startUpSettings.withExplicitConsole) {
//      AllocConsole();
//
//      needToFreeConsole = true;
//   }
//   else flags |= CREATE_NEW_CONSOLE;
//
//   pos_t trimPos = INVALID_POS;
//   if (startUpSettings.includeAppPath2Paths) {
//      flags |= CREATE_UNICODE_ENVIRONMENT;
//
//      pathsEnv.allocate(4096);
//
//      int dwRet = GetEnvironmentVariable(_T("PATH"), (LPWSTR)pathsEnv.str(), 4096);
//      if (dwRet && !isIncluded(pathsEnv.str(), appPath)) {
//         trimPos = pathsEnv.length_pos();
//
//         if (!pathsEnv.empty() && pathsEnv[pathsEnv.length() - 1] != ';')
//            pathsEnv.append(';');
//         pathsEnv.append(appPath);
//
//         SetEnvironmentVariable(_T("PATH"), pathsEnv.str());
//      }
//   }
//
//   bool retVal = CreateProcess(
//      exePath,
//      (wchar_t*)cmdLine,
//      nullptr,
//      nullptr,
//      FALSE,
//      flags,
//      nullptr,
//      currentPath.str(), &si, &pi);
//
//   if (trimPos != NOTFOUND_POS) {
//      // rolling back changes to ENVIRONENT if required
//      pathsEnv.trim(trimPos);
//
//      SetEnvironmentVariable(_T("PATH"), pathsEnv.str());
//   }
//
//   if (!retVal) {
      return false;
//   }
//
//   dwDebugeeProcessId = pi.dwProcessId;
//
//   if (pi.hProcess)
//      CloseHandle(pi.hProcess);
//
//   if (pi.hThread)
//      CloseHandle(pi.hThread);
//
//   started = true;
//   //exception.code = 0;
//   needToHandle = false;
//
//   return true;
}
