//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 OS Controller class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32CONTROLLER_H
#define WIN32CONTROLLER_H

#include "idecommon.h"

namespace elena_lang
{
   // -- Win32Controller ---
   class Win32Controller : public OSControllerBase
   {
   public:
      bool execute(path_t path, path_t commandLine, path_t curDir) override;
   };

}

#endif // WIN32CONTROLLER_H