//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PROJECT_H
#define PROJECT_H

#include "elena.h"
#include "idecommon.h"

namespace elena_lang
{
#ifdef _MSC_VER

#define DEFAULT_FONTNAME "Courier New"

#elif __GNUG__

#define DEFAULT_FONTNAME "Monospace"

#endif


   constexpr auto ROOT_NODE                     = "configuration";
   constexpr auto MAXIMIZED_SETTINGS            = "configuration/settings/maximized";
   constexpr auto LASTPATH_SETTINGS             = "configuration/settings/last_path";
   constexpr auto LASTPROJECT_SETTINGS          = "configuration/settings/last_project";
   constexpr auto FONTSIZE_SETTINGS             = "configuration/settings/font_size";
   constexpr auto FONTNAME_SETTINGS             = "configuration/settings/font_name";
   constexpr auto SCHEME_SETTINGS               = "configuration/settings/scheme";
   constexpr auto HIGHLIGHTSYNTAX_SETTINGS      = "configuration/settings/highlight";
   constexpr auto PERSISTENT_CONSOLE_SETTINGS   = "configuration/settings/persist_output";
   constexpr auto LINENUMBERS_SETTINGS          = "configuration/settings/line_number";
   constexpr auto AUTO_RECOMPILE_SETTING        = "configuration/settings/autocomp";
   constexpr auto AUTO_SAVE_SETTING             = "configuration/settings/autosave";

   constexpr auto RECENTFILES_SETTINGS          = "configuration/recent_files/*";
   constexpr auto RECENTFILE_SETTINGS           = "configuration/recent_files/path";
   constexpr auto RECENTPROJECTS_SETTINGS       = "configuration/recent_projects/path";

   constexpr auto FLAG_UNDEFINED                = 1;

   // --- Map types ---
   typedef List<path_t, freepath>            ProjectPaths;
   typedef List<Breakpoint*, freeobj>        Breakpoints;
   typedef List<ustr_t, freeUStr>            StringList;

   typedef Map<ustr_t, path_t, allocUStr, freeUStr, freepath> ReferencePathMap;

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

      ReferencePathMap referencePaths;

      bool              withPersistentConsole;

      bool              singleSourceProject;
      bool              autoRecompile;
      bool              empty;
      bool              started;
      bool              notSaved;
      int               strictType; // 1 - default, 0 - off, -1 - on
      PathString        name;
      PathString        projectFile;
      PathString        projectPath;
      PathString        outputPath;
      PathString        debugArguments;

      IdentifierString  package;
      IdentifierString  target;
      IdentifierString  templateName;
      IdentifierString  profile;
      IdentifierString  options;

      ProjectPaths      sources;
      ProjectPaths      addedSources;
      ProjectPaths      removeSources;

      ProjectPaths      lastOpenFiles;
      ProjectPaths      lastOpenProjects;

      Breakpoints       breakpoints;

      StringList        projectTypeList;
      StringList        profileList;

      ustr_t getTarget();
      path_t getArguments();

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