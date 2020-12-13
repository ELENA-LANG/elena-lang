//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Settings class implementation
//                                              (C)2005-2018, by Alexei Rakov
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
      setting = _ELENA_::ident_t(value).compare("-1");
   }
}

inline void loadSetting(const char* value, int& setting, int minValue, int maxValue, int defaultValue)
{
   if (value) {
      setting = _ELENA_::ident_t(value).toInt();
      if (setting < minValue || setting > maxValue)
         setting = defaultValue;
   }
}

inline void loadSetting(const char* value, int& setting)
{
   if (value) {
      setting = _ELENA_::ident_t(value).toInt();
   }
}

inline void loadSection(_ELENA_::XmlConfigFile& config, const char* key, PathMapping& list)
{
   Path value;

   _ConfigFile::Nodes nodes;
   config.select(key, nodes);
   for (auto it = nodes.start(); !it.Eof(); it++) {
      ident_t key = (*it).Attribute("key");
      value.copy((*it).Content());

      list.erase(key);
      list.add(key, value.clone(), true);
   }
}

inline void saveSetting(_ELENA_::XmlConfigFile& config, const char* setting, bool value, bool defaultValue)
{
   if (value != defaultValue)
      config.setSetting(setting, value);
}

inline void saveSetting(_ELENA_::XmlConfigFile& config, const char* setting, size_t value, unsigned int defaultValue)
{
   if (value != defaultValue)
      config.setSetting(setting, value);
}

inline void saveSetting(_ELENA_::XmlConfigFile& config, const char* setting, int value, int defaultValue)
{
   if (value != defaultValue)
      config.setSetting(setting, value);
}

inline void saveSection(_ELENA_::XmlConfigFile& config, const char* section, PathMapping& list)
{
   for(PathMapping::Iterator it = list.start(); !it.Eof(); it++) {
      IdentifierString value(*it);

   //   config.setSetting(section, it.key(), value);
      config.appendSetting(section, it.key(), value.c_str());
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
   resolveRelativePath(model->paths.libraryRoot, model->paths.appPath.c_str());
   model->paths.libraryRoot.lower();
}

void Paths :: resolveRelativePath(Path& path, path_t rootPath)
{
   if (isPathRelative(path)) {
      Path fullPath(rootPath);
      fullPath.combine(path.c_str());

      path.copy(fullPath.c_str());
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
   model->packageRoots.add("default", packagePath.clone());
   model->libraryRoots.add("default", libraryPath.clone());
}

void Settings :: load(Model* model, _ELENA_::XmlConfigFile& config)
{
   model->defaultProject.copy(config.getSetting(DEFAULT_PROJECT_SETTING));

   loadSetting(config.getSetting(TAB_USING_SETTING), model->tabCharUsing);
   loadSetting(config.getSetting(MAXIMIZED_SETTING), model->appMaximized);
   loadSetting(config.getSetting(OUTPUT_SETTING), model->compilerOutput);
   loadSetting(config.getSetting(PROJECTVIEW_SETTING), model->projectView);
   loadSetting(config.getSetting(CALLSTACK_SETTING), model->callStack);
   loadSetting(config.getSetting(MESSAGES_SETTING), model->messages);
   loadSetting(config.getSetting(PATH_REMEMBER_SETTING), model->lastPathRemember);
   loadSetting(config.getSetting(PROJECT_REMEMBER_SETTING), model->lastProjectRemember);
   loadSetting(config.getSetting(AUTO_DETECTING_SETTING), model->autoDetecting);
   loadSetting(config.getSetting(AUTO_RECOMPILE_SETTING), model->autoRecompile);
   loadSetting(config.getSetting(AUTO_PROJECT_LOAD_SETTING), model->autoProjectLoad);
   loadSetting(config.getSetting(PROJECT_SAVEBOM_SETTING), model->saveWithBOM);
   //loadSetting(config.getSetting(SETTINGS_SECTION, DEBUG_TAPE_SETTING), model->debugTape);

   loadSetting(config.getSetting(TAB_SIZE_SETTING), model->tabSize, 1, 20, 4);
   loadSetting(config.getSetting(ENCODING_SETTING), model->defaultEncoding);

   loadSetting(config.getSetting(LINE_NUMBERS_SETTING), model->lineNumberVisible);
   loadSetting(config.getSetting(HIGHLIGHT_SETTING), model->highlightSyntax);
   loadSetting(config.getSetting(BRACKETS_SETTING), model->highlightBrackets);
   loadSetting(config.getSetting(TABSCORE_SETTING), model->tabWithAboveScore);
   loadSetting(config.getSetting(SCHEME_SETTING), model->scheme, 0, 1, 0);
   loadSetting(config.getSetting(FONTSIZE_SETTING), model->font_size, 8, 24, 10);

   loadSection(config, SRCPATH_SECTION, model->packageRoots);
   loadSection(config, LIBPATH_SECTION, model->libraryRoots);
}

void Settings :: save(Model* model, _ELENA_::XmlConfigFile& config)
{
   if (!model->defaultProject.isEmpty() && model->lastProjectRemember)
      config.setSetting(DEFAULT_PROJECT_SETTING, IdentifierString(model->defaultProject));

   saveSetting(config, TAB_USING_SETTING, model->tabCharUsing, false);
   saveSetting(config, MAXIMIZED_SETTING, model->appMaximized, true);
   saveSetting(config, OUTPUT_SETTING, model->compilerOutput, true);
   saveSetting(config, PROJECTVIEW_SETTING, model->projectView, true);
   saveSetting(config, CALLSTACK_SETTING, model->callStack, true);
   saveSetting(config, MESSAGES_SETTING, model->messages, true);
   saveSetting(config, PATH_REMEMBER_SETTING, model->lastPathRemember, true);
   saveSetting(config, PROJECT_REMEMBER_SETTING, model->lastProjectRemember, true);
   saveSetting(config, AUTO_PROJECT_LOAD_SETTING, model->autoProjectLoad, false);
   saveSetting(config, AUTO_DETECTING_SETTING, model->autoDetecting, true);
   saveSetting(config, AUTO_RECOMPILE_SETTING, model->autoRecompile, true);
   saveSetting(config, PROJECT_SAVEBOM_SETTING, model->saveWithBOM, false);
   //saveSetting(config, SETTINGS_SECTION, DEBUG_TAPE_SETTING, model->debugTape, false);

   saveSetting(config, LINE_NUMBERS_SETTING, model->lineNumberVisible, true);
   saveSetting(config, HIGHLIGHT_SETTING, model->highlightSyntax, true);
   saveSetting(config, BRACKETS_SETTING, model->highlightBrackets, true);
   saveSetting(config, TABSCORE_SETTING, model->tabWithAboveScore, true);
   saveSetting(config, SCHEME_SETTING, model->scheme, 0);
   saveSetting(config, FONTSIZE_SETTING, model->font_size, 10);

   saveSetting(config, TAB_SIZE_SETTING, model->tabSize, 4);
   saveSetting(config, ENCODING_SETTING, model->defaultEncoding, 0);

   saveSection(config, SRCPATH_SECTION_NAME, model->packageRoots);
   saveSection(config, LIBPATH_SECTION_NAME, model->libraryRoots);
}

void Settings :: onNewProjectTemplate(Model* model, _GUI_::_ProjectManager* project)
{
   const char* projectTemplate = project->getTemplate();

   // reload package root
   path_t path = model->packageRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = model->packageRoots.get("default");

   model->paths.packageRoot.copy(path);
   Paths::resolveRelativePath(model->paths.packageRoot, model->paths.appPath.c_str());

   // reload library root
   path = model->libraryRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = model->libraryRoots.get("default");

   model->paths.libraryRoot.copy(path);
   Paths::resolveRelativePath(model->paths.libraryRoot, model->paths.appPath.c_str());
}

void Settings :: addSearchHistory(Model* model, text_t line)
{
   if (retrieve(model->searchHistory.start(), (text_c*)line, (text_c*)NULL) == NULL)
      model->searchHistory.add(text_str(line).clone());
}

void Settings :: addReplaceHistory(Model* model, text_t line)
{
   if (retrieve(model->replaceHistory.start(), line, (text_c*)NULL) == NULL)
      model->replaceHistory.add(text_str(line).clone());
}

