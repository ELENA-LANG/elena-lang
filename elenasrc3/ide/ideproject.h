//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include "elena.h"
#include "idecommon.h"

namespace elena_lang
{
   constexpr auto ROOT_NODE            = "configuration";
   constexpr auto MAXIMIZED_SETTINGS   = "configuration/settings/maximized";
   constexpr auto FONTSIZE_SETTINGS    = "configuration/settings/font_size";
   constexpr auto SCHEME_SETTINGS      = "configuration/settings/scheme";

   constexpr auto RECENTFILES_SETTINGS = "configuration/recent_files/*";
   constexpr auto RECENTFILE_SETTINGS  = "configuration/recent_files/path";

   // --- Map types ---
   typedef List<path_t, freepath>      ProjectPaths;
   typedef List<Breakpoint*, freeobj>  Breakpoints;
   typedef List<ustr_t, freeUStr>      StringList;

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
         PathString vmTerminalPath;
         PathString configPath;
      } paths;

      bool              singleSourceProject;
      bool              autoRecompile;
      bool              empty;
      bool              started;
      bool              notSaved;
      PathString        name;
      PathString        projectFile;
      PathString        projectPath;
      PathString        outputPath;
      PathString        debugArguments;

      IdentifierString  package;
      IdentifierString  target;
      IdentifierString  templateName;
      IdentifierString  options;

      ProjectPaths      sources;

      ProjectPaths      lastOpenFiles;

      Breakpoints       breakpoints;

      StringList        projectTypeList;

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