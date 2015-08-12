//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Settings class implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "settings.h"

#ifdef _WIN32

#include "winapi32\wincommon.h"

#elif _LINUX32

#include "gtk-linux32/gtkcommon.h"

#endif

using namespace _GUI_;
using namespace _ELENA_;

//typedef String<char, 255> ParamString;

// --- help functions ---

inline void loadSetting(const char* value, bool& setting)
{
   if (value) {
      setting = _ELENA_::StringHelper::compare(value, "-1");
   }
}

inline void loadSetting(const char* value, size_t& setting, size_t minValue, size_t maxValue, size_t defaultValue)
{
   if (value) {
      setting = _ELENA_::StringHelper::strToInt(value);
      if (setting < minValue || setting > maxValue)
         setting = defaultValue;
   }
}

inline void loadSetting(const char* value, int& setting)
{
   if (value) {
      setting = _ELENA_::StringHelper::strToInt(value);
   }
}

inline void loadSection(ConfigCategoryIterator it, PathMapping& list)
{
   Path value;
   while (!it.Eof()) {
      Path::loadPath(value, (const char*)(*it));

      list.erase(it.key());
      list.add(it.key(), value.clone(), true);

      it++;
   }
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const char* section, const char* setting, bool value, bool defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const char* section, const char* setting, size_t value, size_t defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const char* section, const char* setting, int value, int defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSection(_ELENA_::IniConfigFile& config, const char* section, PathMapping& list)
{
   for(PathMapping::Iterator it = list.start(); !it.Eof(); it++) {
      IdentifierString value(*it);

      config.setSetting(section, it.key(), value);
   }
}

// -- Paths ---

void Paths :: init(Model* model, path_t appPath, const path_t defaultPath)
{
   model->paths.appPath.copy(appPath);

   model->paths.defaultPath.copy(defaultPath);
   model->paths.lastPath.copy(defaultPath);
}

void Paths::setLibraryRoot(Model* model, path_t libraryPath)
{
   model->paths.libraryRoot.copy(libraryPath);
   resolveRelativePath(model->paths.libraryRoot, model->paths.appPath);
   model->paths.libraryRoot.lower();
}

void Paths :: resolveRelativePath(Path& path, path_t rootPath)
{
   if (isPathRelative(path)) {
      Path fullPath(rootPath);
      fullPath.combine(path);

      path.copy(fullPath);
   }
   canonicalize(path);
}

void Paths :: makeRelativePath(Path& path, path_t rootPath)
{
   makePathRelative(path, rootPath);
}

// --- Settings ---

void Settings :: init(Model* model, path_t packagePath, path_t libraryPath)
{
   model->packageRoots.add("default", StringHelper::clone(packagePath));
   model->libraryRoots.add("default", StringHelper::clone(libraryPath));
}

void Settings :: load(Model* model, _ELENA_::IniConfigFile& config)
{
   Path::loadPath(model->defaultProject, config.getSetting(SETTINGS_SECTION, DEFAULT_PROJECT_SETTING));

   loadSetting(config.getSetting(SETTINGS_SECTION, TAB_USING_SETTING), model->tabCharUsing);
   loadSetting(config.getSetting(SETTINGS_SECTION, MAXIMIZED_SETTING), model->appMaximized);
   loadSetting(config.getSetting(SETTINGS_SECTION, OUTPUT_SETTING), model->compilerOutput);
   loadSetting(config.getSetting(SETTINGS_SECTION, PROJECTVIEW_SETTING), model->projectView);
   loadSetting(config.getSetting(SETTINGS_SECTION, CALLSTACK_SETTING), model->callStack);
   loadSetting(config.getSetting(SETTINGS_SECTION, MESSAGES_SETTING), model->messages);
   loadSetting(config.getSetting(SETTINGS_SECTION, PATH_REMEMBER_SETTING), model->lastPathRemember);
   loadSetting(config.getSetting(SETTINGS_SECTION, PROJECT_REMEMBER_SETTING), model->lastProjectRemember);
   loadSetting(config.getSetting(SETTINGS_SECTION, AUTO_DETECTING_SETTING), model->autoDetecting);
   loadSetting(config.getSetting(SETTINGS_SECTION, AUTO_RECOMPILE_SETTING), model->autoRecompile);
   loadSetting(config.getSetting(SETTINGS_SECTION, AUTO_PROJECT_LOAD_SETTING), model->autoProjectLoad);
   //loadSetting(config.getSetting(SETTINGS_SECTION, DEBUG_TAPE_SETTING), model->debugTape);

   loadSetting(config.getSetting(SETTINGS_SECTION, TAB_SIZE_SETTING), model->tabSize, 1, 20, 4);
   loadSetting(config.getSetting(SETTINGS_SECTION, ENCODING_SETTING), model->defaultEncoding);

   loadSetting(config.getSetting(SETTINGS_SECTION, LINE_NUMBERS_SETTING), model->lineNumberVisible);
   loadSetting(config.getSetting(SETTINGS_SECTION, HIGHLIGHT_SETTING), model->highlightSyntax);
   loadSetting(config.getSetting(SETTINGS_SECTION, BRACKETS_SETTING), model->highlightBrackets);
   loadSetting(config.getSetting(SETTINGS_SECTION, TABSCORE_SETTING), model->tabWithAboveScore);
   loadSetting(config.getSetting(SETTINGS_SECTION, SCHEME_SETTING), model->scheme, 0, 1, 0);
   loadSetting(config.getSetting(SETTINGS_SECTION, FONTSIZE_SETTING), model->font_size, 8, 24, 10);

   loadSection(config.getCategoryIt(SRCPATH_SECTION), model->packageRoots);
   loadSection(config.getCategoryIt(LIBPATH_SECTION), model->libraryRoots);
}

void Settings :: save(Model* model, _ELENA_::IniConfigFile& config)
{
   if (!model->defaultProject.isEmpty() && model->lastProjectRemember)
      config.setSetting(SETTINGS_SECTION, DEFAULT_PROJECT_SETTING, IdentifierString(model->defaultProject));

   saveSetting(config, SETTINGS_SECTION, TAB_USING_SETTING, model->tabCharUsing, false);
   saveSetting(config, SETTINGS_SECTION, MAXIMIZED_SETTING, model->appMaximized, true);
   saveSetting(config, SETTINGS_SECTION, OUTPUT_SETTING, model->compilerOutput, true);
   saveSetting(config, SETTINGS_SECTION, PROJECTVIEW_SETTING, model->projectView, true);
   saveSetting(config, SETTINGS_SECTION, CALLSTACK_SETTING, model->callStack, true);
   saveSetting(config, SETTINGS_SECTION, MESSAGES_SETTING, model->messages, true);
   saveSetting(config, SETTINGS_SECTION, PATH_REMEMBER_SETTING, model->lastPathRemember, true);
   saveSetting(config, SETTINGS_SECTION, PROJECT_REMEMBER_SETTING, model->lastProjectRemember, true);
   saveSetting(config, SETTINGS_SECTION, AUTO_PROJECT_LOAD_SETTING, model->autoProjectLoad, true);
   saveSetting(config, SETTINGS_SECTION, AUTO_DETECTING_SETTING, model->autoDetecting, true);
   saveSetting(config, SETTINGS_SECTION, AUTO_RECOMPILE_SETTING, model->autoRecompile, true);
   //saveSetting(config, SETTINGS_SECTION, DEBUG_TAPE_SETTING, model->debugTape, false);

   saveSetting(config, SETTINGS_SECTION, LINE_NUMBERS_SETTING, model->lineNumberVisible, true);
   saveSetting(config, SETTINGS_SECTION, HIGHLIGHT_SETTING, model->highlightSyntax, true);
   saveSetting(config, SETTINGS_SECTION, BRACKETS_SETTING, model->highlightBrackets, true);
   saveSetting(config, SETTINGS_SECTION, TABSCORE_SETTING, model->tabWithAboveScore, true);
   saveSetting(config, SETTINGS_SECTION, SCHEME_SETTING, model->scheme, 0u);
   saveSetting(config, SETTINGS_SECTION, FONTSIZE_SETTING, model->font_size, 10u);

   saveSetting(config, SETTINGS_SECTION, TAB_SIZE_SETTING, model->tabSize, 4u);
   saveSetting(config, SETTINGS_SECTION, ENCODING_SETTING, model->defaultEncoding, 0);

   saveSection(config, SRCPATH_SECTION, model->packageRoots);
   saveSection(config, LIBPATH_SECTION, model->libraryRoots);
}

void Settings :: onNewProjectTemplate(Model* model, _ProjectManager* project)
{
   const char* projectTemplate = project->getTemplate();

   // reload package root
   path_t path = model->packageRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = model->packageRoots.get("default");

   model->paths.packageRoot.copy(path);
   Paths::resolveRelativePath(model->paths.packageRoot, model->paths.appPath);

   // reload library root
   path = model->libraryRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = model->libraryRoots.get("default");

   model->paths.libraryRoot.copy(path);
   Paths::resolveRelativePath(model->paths.libraryRoot, model->paths.appPath);
}

void Settings :: addSearchHistory(Model* model, text_t line)
{
   if (retrieve(model->searchHistory.start(), (text_c*)line, (text_c*)NULL) == NULL)
      model->searchHistory.add(StringHelper::clone(line));
}

void Settings :: addReplaceHistory(Model* model, text_t line)
{
   if (retrieve(model->replaceHistory.start(), line, (text_c*)NULL) == NULL)
      model->replaceHistory.add(StringHelper::clone(line));
}

