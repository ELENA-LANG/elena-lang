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
   constexpr auto MAXIMIZED_SETTINGS   = "configuration/settings/maximized";
   constexpr auto FONTSIZE_SETTINGS    = "configuration/settings/font_size";
   constexpr auto SCHEME_SETTINGS      = "configuration/settings/scheme";

   constexpr auto RECENTFILES_SETTINGS = "configuration/recent_files/*";

   // --- Map types ---
   typedef List<path_t, freepath> ProjectPaths;

   // --- ProjectModel ---
   struct ProjectModel
   {
   private:
      IDEStatus* status;

   public:
      struct Paths
      {
         PathString libraryRoot;
         PathString lastPath;
         PathString librarySourceRoot;
         PathString appPath;
         PathString compilerPath;
      } paths;

      bool           singleSourceProject;
      bool           autoRecompile;
      PathString     name;
      PathString     projectFile;
      PathString     projectPath;
      PathString     outputPath;
      ProjectPaths   sources;

      ProjectPaths   lastOpenFiles;

      ustr_t getTarget();
      ustr_t getArguments();
      ustr_t getPackage();

      path_t getOutputPath();

      bool getDebugMode();

      IDEStatus getStatus() const
      {
         return *status;
      }

      ProjectModel(IDEStatus* status);
   };
}

#endif