//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Settings class header
//                                              (C)2005-2013, by Alexei Rakov
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
#define AUTO_RECOMPILE_SETTING   "autocomp"
#define DEBUG_TAPE_SETTING       "debugtape"
#define FONTSIZE_SETTING         "font_size"

#define DEFAULT_PROJECT_SETTING  "defaultproject"

namespace _GUI_
{

// --- Paths ---

struct Paths
{
   static _ELENA_::Path appPath;
   static _ELENA_::Path defaultPath;
   static _ELENA_::Path packageRoot;
   static _ELENA_::Path libraryRoot;
   static _ELENA_::Path lastPath;

   static void init(const _path_t* appPath, const _path_t* defaultPath);

   static void setLibraryRoot(const _path_t* libraryPath);

   static void resolveRelativePath(_ELENA_::Path& path, const _path_t* rootPath);
   static void resolveRelativePath(_ELENA_::Path& path)
   {
      resolveRelativePath(path, defaultPath);
   }

   static void makeRelativePath(_ELENA_::Path& path, const _path_t* rootPath);
};

// --- Settings ---

struct Settings
{
   typedef _ELENA_::Map<const char*, _path_t*> PathMapping;

   static size_t tabSize;
   static int    defaultEncoding;

   static bool hexNumberMode;
   static bool testMode;
   static bool tabCharUsing;
   static bool appMaximized;
   static bool compilerOutput;
   static bool lastPathRemember;
   static bool lastProjectRemember;
   static bool autoDetecting;
   static bool lineNumberVisible;
   static bool highlightSyntax;
   static bool highlightBrackets;
   static bool tabWithAboveScore;
   static bool autoRecompile;
   static bool debugTape;

   static size_t scheme;
   static size_t font_size;

   static _ELENA_::Path defaultProject;
   static _ELENA_::List<_path_t*> defaultFiles;

   static _ELENA_::List<_text_t*> searchHistory;
   static _ELENA_::List<_text_t*> replaceHistory;

   static PathMapping packageRoots;
   static PathMapping libraryRoots;

   static void init(const _path_t* packagePath, const _path_t* libraryPath);
   static void load(_ELENA_::IniConfigFile& config);
   static void save(_ELENA_::IniConfigFile& config);

   static void onNewProjectTemplate();

   static void addSearchHistory(const _text_t* line);
   static void addReplaceHistory(const _text_t* line);

   static void addPackagePath(const char* projectTemplate, const _path_t* path)
   {
      packageRoots.erase(projectTemplate);
      packageRoots.add(projectTemplate, _ELENA_::StringHelper::clone(path));
   }

   static void addLibraryPath(const char* projectTemplate, const _path_t* path)
   {
      libraryRoots.erase(projectTemplate);
      libraryRoots.add(projectTemplate, _ELENA_::StringHelper::clone(path));
   }
};

// --- Project ---

struct Project
{
private:
   static bool _changed;

   static _ELENA_::FileName      _name;
   static _ELENA_::Path          _path;
   static _ELENA_::IniConfigFile _config;

public:
   static bool isChanged() { return _changed; }
   static bool isUnnamed() { return _name.isEmpty(); }

   static const _text_t* getName() { return _name; }
   static const _path_t* getPath() { return _path; }
   static const char* getPackage();
   static const char* getTemplate();
   static const char* getOptions();
   static const char* getTarget();
   static const char* getOutputPath();
   static const char* getVMPath();
   static const char* getArguments();

   static int getDebugMode();

   static bool getBoolSetting(const char* name);

   static _ELENA_::ConfigCategoryIterator SourceFiles()
   {
      return _config.getCategoryIt(IDE_FILES_SECTION);
   }

   static _ELENA_::ConfigCategoryIterator Forwards()
   {
      return _config.getCategoryIt(IDE_FORWARDS_SECTION);
   }

   static void setSectionOption(const char* option, const char* value);
   static void setBoolSetting(const char* key, bool value);

   static void setTarget(const char* target);
   static void setArguments(const char* target);
   static void setOutputPath(const char* path);
   static void setVMPath(const char* path);
   static void setOptions(const char* options);
   static void setPackage(const char* package);
   static void setTemplate(const char* target);
   static void setDebugMode(int mode);

   static bool open(const _path_t* path);
   static void refresh();
   static void reset();
   static void save();

   static void rename(const _path_t* path);

   static void retrieveName(_ELENA_::Path& path, _ELENA_::ReferenceNs & name);

   static bool isIncluded(const _path_t* path);
   static void includeSource(const _path_t* path);
   static void excludeSource(const _path_t* path);

   static void clearForwards();
   static void addForward(const _text_t* name, const _text_t* reference);

   static void retrievePath(const wchar16_t* reference, _ELENA_::Path & path, const _path_t* extension);
};

} // _GUI_

#endif // idesettingsH
