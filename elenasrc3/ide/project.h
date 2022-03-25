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

      ProjectModel(IDEStatus* status)
      {
         this->status = status;

         this->autoRecompile = true;
      }
   };
}

#endif