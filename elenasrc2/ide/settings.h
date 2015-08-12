//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Settings class header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef settingsH
#define settingsH

#include "elena.h"
#include "config.h"
#include "idecommon.h"

#define SETTINGS_SECTION         "settings"
#define SRCPATH_SECTION          "srcpath"     // template source paths
#define LIBPATH_SECTION          "libpath"     // template library paths
#define RECENTFILES_SECTION      "recent_files"
#define RECENTRPOJECTS_SECTION   "recent_projects"

#define TAB_SIZE_SETTING         "tabsize"
#define ENCODING_SETTING         "encoding"
#define TAB_USING_SETTING        "tabusing"
#define MAXIMIZED_SETTING        "app_maximized"
#define PATH_REMEMBER_SETTING    "remeber_path"
#define PROJECT_REMEMBER_SETTING "remeber_project"
#define AUTO_DETECTING_SETTING   "encoding_detecting"
#define LINE_NUMBERS_SETTING     "linenumbers"
#define HIGHLIGHT_SETTING        "highlightsyntax"
#define BRACKETS_SETTING         "highlightbrackets"
#define SCHEME_SETTING           "scheme"
#define TABSCORE_SETTING         "tabscore"
#define OUTPUT_SETTING           "compileroutput"
#define PROJECTVIEW_SETTING      "projectview"
#define CALLSTACK_SETTING        "callstack"
#define MESSAGES_SETTING         "messages"
#define AUTO_RECOMPILE_SETTING   "autocomp"
#define AUTO_PROJECT_LOAD_SETTING "autoload"
#define DEBUG_TAPE_SETTING       "debugtape"
#define FONTSIZE_SETTING         "font_size"

#define DEFAULT_PROJECT_SETTING  "defaultproject"

namespace _GUI_
{

// --- Paths ---

class Paths
{
public:
   static void init(Model* model, _ELENA_::path_t appPath, _ELENA_::path_t defaultPath);

   static void setLibraryRoot(Model* model, _ELENA_::path_t libraryPath);

   static void resolveRelativePath(_ELENA_::Path& path, _ELENA_::path_t rootPath);

   static void makeRelativePath(_ELENA_::Path& path, _ELENA_::path_t rootPath);
};

// --- Settings ---

struct Settings
{
   static void init(Model* model, _ELENA_::path_t packagePath, _ELENA_::path_t libraryPath);
   static void load(Model* model, _ELENA_::IniConfigFile& config);
   static void save(Model* model, _ELENA_::IniConfigFile& config);

   static void onNewProjectTemplate(Model* model, _ProjectManager* project);

   static void addSearchHistory(Model* model, text_t line);
   static void addReplaceHistory(Model* model, text_t line);

   static void addPackagePath(Model* model, const char* projectTemplate, _ELENA_::path_t path)
   {
      model->packageRoots.erase(projectTemplate);
      model->packageRoots.add(projectTemplate, _ELENA_::StringHelper::clone(path));
   }

   static void addLibraryPath(Model* model, const char* projectTemplate, _ELENA_::path_t path)
   {
      model->libraryRoots.erase(projectTemplate);
      model->libraryRoots.add(projectTemplate, _ELENA_::StringHelper::clone(path));
   }
};

} // _GUI_

#endif // idesettingsH
