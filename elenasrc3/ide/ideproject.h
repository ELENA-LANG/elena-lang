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

      struct Paths
      {
         PathString libraryRoot;
         PathString lastPath;
         PathString librarySourceRoot;
      } paths;

      bool       autoRecompile;
      PathString projectPath;
      PathString outputPath;

      ustr_t getTarget();
      ustr_t getArguments();
      ustr_t getPackage();

      path_t getOutputPath();

      bool getDebugMode();

      ProjectModel(IDEStatus* status);
   };
}

#endif