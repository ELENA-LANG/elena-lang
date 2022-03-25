//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include "idecommon.h"

namespace elena_lang
{
   // --- ProjectModel ---
   struct ProjectModel
   {
      IDEStatus* status;

      bool       autoRecompile;
      PathString projectPath;

      ustr_t getTarget();
      ustr_t getArguments();

      bool getDebugMode();

      ProjectModel(IDEStatus* status);
   };
}

#endif