//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 OS Controller class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32CONTROLLER_H
#define WIN32CONTROLLER_H

#include "idecommon.h"
#include <windows.h>

namespace elena_lang
{
   // --- Win32RedirectInfo --
   struct Win32RedirectInfo
   {
      HANDLE hStdOut; 
      HANDLE hStdIn;
      HANDLE hStdErr;

      Win32RedirectInfo() :
         hStdOut(nullptr),
         hStdIn(nullptr),
         hStdErr(nullptr)
      {
      }
   };

   // -- Win32Controller ---
   class Win32Controller : public OSControllerBase
   {
      Win32RedirectInfo redirectInfo;

   public:
      bool execute(path_t path, path_t commandLine, path_t curDir) override;

      Win32Controller();
   };

}

#endif // WIN32CONTROLLER_H