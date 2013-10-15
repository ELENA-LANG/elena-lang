//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Settings class implementation
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "settings.h"

#ifdef _WIN32

#include "winapi32\wincommon.h"

#elif _LINUX32

#include "gtk-linux32/gtkcommon.h"

#endif

using namespace _GUI_;
using namespace _ELENA_;

// --- help functions ---

inline void loadSetting(const _text_t* value, bool& setting)
{
   if (value) {
      setting = _ELENA_::StringHelper::compare(value, _T("-1"));
   }
}

inline void loadSetting(const _text_t* value, size_t& setting, size_t minValue, size_t maxValue, size_t defaultValue)
{
   if (value) {
      setting = _ELENA_::StringHelper::strToInt(value);
      if (setting < minValue || setting > maxValue)
         setting = defaultValue;
   }
}

inline void loadSetting(const _text_t* value, int& setting)
{
   if (value) {
      setting = _ELENA_::StringHelper::strToInt(value);
   }
}

inline void loadSection(ConfigCategoryIterator it, Settings::PathMapping& list)
{
   while (!it.Eof()) {
      list.erase(it.key());
      list.add(it.key(), StringHelper::clone((_text_t*)*it), true);

      it++;
   }
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const _text_t* section, const _text_t* setting, bool value, bool defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const _text_t* section, const _text_t* setting, size_t value, size_t defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSetting(_ELENA_::IniConfigFile& config, const _text_t* section, const _text_t* setting, int value, int defaultValue)
{
   if (value != defaultValue)
      config.setSetting(section, setting, value);
}

inline void saveSection(_ELENA_::IniConfigFile& config, const _text_t* section, Settings::PathMapping& list)
{
   for(Settings::PathMapping::Iterator it = list.start(); !it.Eof(); it++) {
      config.setSetting(section, it.key(), *it);
   }
}

// -- Paths ---

Path Paths :: appPath = Path();

Path Paths :: defaultPath = Path();

Path Paths :: packageRoot = Path();

Path Paths :: libraryRoot = Path();

Path Paths :: lastPath = Path();

void Paths :: init(const _path_t* appPath, const _path_t* defaultPath)
{
   Paths::appPath.copy(appPath);

   Paths::defaultPath.copy(defaultPath);
   Paths::lastPath.copy(defaultPath);
}

void Paths :: setLibraryRoot(const _path_t* libraryPath)
{
   Paths::libraryRoot.copy(libraryPath);
   resolveRelativePath(libraryRoot, appPath);
   Paths::libraryRoot.lower();
}

void Paths :: resolveRelativePath(Path& path, const _path_t* rootPath)
{
   if (isPathRelative(path)) {
      Path fullPath(rootPath);
      fullPath.combine(path);

      path.copy(fullPath);
   }
   canonicalize(path);
}

void Paths :: makeRelativePath(Path& path, const _path_t* rootPath)
{
   makePathRelative(path, rootPath);
}

// --- Settings ---

size_t Settings :: scheme = 0;

size_t Settings :: tabSize = 4;

bool Settings :: tabCharUsing = false;

bool Settings :: appMaximized = true;

bool Settings :: compilerOutput = true;

bool Settings :: lastPathRemember = true;

bool Settings :: lastProjectRemember = true;

int Settings :: defaultEncoding = 0;

bool Settings :: autoDetecting = true;

bool Settings :: lineNumberVisible = true;

bool Settings :: highlightSyntax = true;

bool Settings :: highlightBrackets = true;

bool Settings :: tabWithAboveScore = true;

bool Settings :: autoRecompile = true;

bool Settings :: debugTape = false;

bool Settings :: hexNumberMode = true;

bool Settings :: testMode = false;

Path Settings :: defaultProject = Path();

List<_path_t*> Settings :: defaultFiles = List<_path_t*>(NULL, freestr);

List<_text_t*> Settings :: searchHistory = List<_text_t*>(NULL, freestr);
List<_text_t*> Settings :: replaceHistory = List<_text_t*>(NULL, freestr);

Settings::PathMapping Settings :: packageRoots = PathMapping(NULL, freestr);

Settings::PathMapping Settings :: libraryRoots = PathMapping(NULL, freestr);

void Settings :: init(const _path_t* packagePath, const _path_t* libraryPath)
{
   packageRoots.add(_T("default"), StringHelper::clone(packagePath));
   libraryRoots.add(_T("default"), StringHelper::clone(libraryPath));
}

void Settings :: load(_ELENA_::IniConfigFile& config)
{
   defaultProject.copy(config.getSetting(SETTINGS_SECTION, DEFAULT_PROJECT_SETTING));

   loadSetting(config.getSetting(SETTINGS_SECTION, TAB_USING_SETTING), tabCharUsing);
   loadSetting(config.getSetting(SETTINGS_SECTION, MAXIMIZED_SETTING), appMaximized);
   loadSetting(config.getSetting(SETTINGS_SECTION, OUTPUT_SETTING), compilerOutput);
   loadSetting(config.getSetting(SETTINGS_SECTION, PATH_REMEMBER_SETTING), lastPathRemember);
   loadSetting(config.getSetting(SETTINGS_SECTION, PROJECT_REMEMBER_SETTING), lastProjectRemember);
   loadSetting(config.getSetting(SETTINGS_SECTION, AUTO_DETECTING_SETTING), autoDetecting);
   loadSetting(config.getSetting(SETTINGS_SECTION, AUTO_RECOMPILE_SETTING), autoRecompile);
   loadSetting(config.getSetting(SETTINGS_SECTION, DEBUG_TAPE_SETTING), debugTape);

   loadSetting(config.getSetting(SETTINGS_SECTION, TAB_SIZE_SETTING), tabSize, 1, 20, 4);
   loadSetting(config.getSetting(SETTINGS_SECTION, ENCODING_SETTING), defaultEncoding);

   loadSetting(config.getSetting(SETTINGS_SECTION, LINE_NUMBERS_SETTING), lineNumberVisible);
   loadSetting(config.getSetting(SETTINGS_SECTION, HIGHLIGHT_SETTING), highlightSyntax);
   loadSetting(config.getSetting(SETTINGS_SECTION, BRACKETS_SETTING), highlightBrackets);
   loadSetting(config.getSetting(SETTINGS_SECTION, TABSCORE_SETTING), tabWithAboveScore);
   loadSetting(config.getSetting(SETTINGS_SECTION, SCHEME_SETTING), scheme, 0, 1, 0);

   loadSection(config.getCategoryIt(SRCPATH_SECTION), packageRoots);
   loadSection(config.getCategoryIt(LIBPATH_SECTION), libraryRoots);
}

void Settings :: save(_ELENA_::IniConfigFile& config)
{
   if (!defaultProject.isEmpty() && lastProjectRemember)
      config.setSetting(SETTINGS_SECTION, DEFAULT_PROJECT_SETTING, defaultProject);

   saveSetting(config, SETTINGS_SECTION, TAB_USING_SETTING, tabCharUsing, false);
   saveSetting(config, SETTINGS_SECTION, MAXIMIZED_SETTING, appMaximized, true);
   saveSetting(config, SETTINGS_SECTION, OUTPUT_SETTING, compilerOutput, true);
   saveSetting(config, SETTINGS_SECTION, PATH_REMEMBER_SETTING, lastPathRemember, true);
   saveSetting(config, SETTINGS_SECTION, PROJECT_REMEMBER_SETTING, lastProjectRemember, true);
   saveSetting(config, SETTINGS_SECTION, AUTO_DETECTING_SETTING, autoDetecting, true);
   saveSetting(config, SETTINGS_SECTION, AUTO_RECOMPILE_SETTING, autoRecompile, true);
   saveSetting(config, SETTINGS_SECTION, DEBUG_TAPE_SETTING, debugTape, false);

   saveSetting(config, SETTINGS_SECTION, LINE_NUMBERS_SETTING, lineNumberVisible, true);
   saveSetting(config, SETTINGS_SECTION, HIGHLIGHT_SETTING, highlightSyntax, true);
   saveSetting(config, SETTINGS_SECTION, BRACKETS_SETTING, highlightBrackets, true);
   saveSetting(config, SETTINGS_SECTION, TABSCORE_SETTING, tabWithAboveScore, true);
   saveSetting(config, SETTINGS_SECTION, SCHEME_SETTING, scheme, 0u);

   saveSetting(config, SETTINGS_SECTION, TAB_SIZE_SETTING, tabSize, 4u);
   saveSetting(config, SETTINGS_SECTION, ENCODING_SETTING, defaultEncoding, 0);

   saveSection(config, SRCPATH_SECTION, packageRoots);
   saveSection(config, LIBPATH_SECTION, libraryRoots);
}

void Settings :: onNewProjectTemplate()
{
   const _text_t* projectTemplate = Project::getTemplate();

   // reload package root
   const _path_t* path = packageRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = packageRoots.get(_T("default"));

   Paths::packageRoot.copy(path);
   Paths::resolveRelativePath(Paths::packageRoot, Paths::appPath);
   Paths::packageRoot.lower();

   // reload library root
   path = libraryRoots.get(projectTemplate);
   if (_ELENA_::emptystr(path))
      path = libraryRoots.get(_T("default"));

   Paths::libraryRoot.copy(path);
   Paths::resolveRelativePath(Paths::libraryRoot, Paths::appPath);
   Paths::libraryRoot.lower();
}

void Settings :: addSearchHistory(const _text_t* line)
{
   if (retrieve(searchHistory.start(), line, (const _text_t*)NULL) == NULL)
      searchHistory.add(StringHelper::clone(line));
}

void Settings :: addReplaceHistory(const _text_t* line)
{
   if (retrieve(replaceHistory.start(), line, (const _text_t*)NULL) == NULL)
      replaceHistory.add(StringHelper::clone(line));
}

// --- Projects ---

bool Project :: _changed = false;

IniConfigFile Project :: _config = IniConfigFile();
Path          Project :: _path = Path();
FileName      Project :: _name = FileName();

const _text_t* Project :: getPackage()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_PACKAGE_SETTING);
}

const _text_t* Project :: getTemplate()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_TEMPLATE_SETTING);
}

const _text_t* Project :: getOptions()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_COMPILER_OPTIONS);
}

const _text_t* Project :: getTarget()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_EXECUTABLE_SETTING);
}

const _path_t* Project :: getOutputPath()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_OUTPUT_SETTING);
}

const _path_t* Project :: getVMPath()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_VMPATH_SETTING);
}

const _path_t* Project :: getArguments()
{
   return _config.getSetting(IDE_PROJECT_SECTION, IDE_ARGUMENT_SETTING);
}

int Project :: getDebugMode()
{
   const _text_t* mode = _config.getSetting(IDE_PROJECT_SECTION, IDE_DEBUGINFO_SETTING);
   if (_ELENA_::StringHelper::compare(mode, _T("-1"))) {
      return -1;
   }
   else if (_ELENA_::StringHelper::compare(mode, _T("-2"))) {
      return -2;
   }
   else return 0;
}

bool Project :: getBoolSetting(const _text_t* name)
{
   const _text_t* value = _config.getSetting(IDE_PROJECT_SECTION, name);

   return _ELENA_::StringHelper::compare(value, _T("-1"));
}

void Project :: setSectionOption(const _text_t* option, const _text_t* value)
{
   if (!_ELENA_::emptystr(value)) {
      _config.setSetting(IDE_PROJECT_SECTION, option, value);
   }
   else _config.clear(IDE_PROJECT_SECTION, option);

   _changed = true;
}

void Project :: setTarget(const _text_t* target)
{
   setSectionOption(IDE_EXECUTABLE_SETTING, target);
}

void Project :: setArguments(const _text_t* arguments)
{
   setSectionOption(IDE_ARGUMENT_SETTING, arguments);
}

void Project :: setTemplate(const _text_t* templateName)
{
   setSectionOption(IDE_TEMPLATE_SETTING, templateName);
}

void Project :: setOutputPath(const _path_t* path)
{
   setSectionOption(IDE_OUTPUT_SETTING, path);
}

void Project :: setVMPath(const _path_t* path)
{
   setSectionOption(IDE_VMPATH_SETTING, path);
}

void Project :: setOptions(const _text_t* options)
{
   setSectionOption(IDE_COMPILER_OPTIONS, options);
}

void Project :: setPackage(const _text_t* package)
{
   setSectionOption(IDE_PACKAGE_SETTING, package);
}

void Project:: setDebugMode(int mode)
{
   if (mode != 0) {
      _config.setSetting(IDE_PROJECT_SECTION, IDE_DEBUGINFO_SETTING, mode);
   }
   else _config.clear(IDE_PROJECT_SECTION, IDE_DEBUGINFO_SETTING);

   _changed = true;
}

void Project :: setBoolSetting(const _text_t* key, bool value)
{
   setSectionOption(key, value ? _T("-1") : _T("0"));
}

void Project :: refresh()
{
   const _text_t* projectTemplate = getTemplate();

   // reload package root
   const _path_t* templatePath = Settings::packageRoots.get(projectTemplate);
   if (_ELENA_::emptystr(templatePath))
      templatePath = Settings::packageRoots.get(_T("default"));

   Paths::packageRoot.copy(templatePath);
   Paths::resolveRelativePath(Paths::packageRoot, Paths::appPath);
   Paths::packageRoot.lower();

   // reload library root
   templatePath = Settings::libraryRoots.get(projectTemplate);
   if (_ELENA_::emptystr(templatePath))
      templatePath = Settings::libraryRoots.get(_T("default"));

   Paths::libraryRoot.copy(templatePath);
   Paths::resolveRelativePath(Paths::libraryRoot, Paths::appPath);
   Paths::libraryRoot.lower();
}

bool Project :: open(const _path_t* path)
{
   _config.clear();

   int encoding = Settings::defaultEncoding;
   if (encoding == feUTF16)   // !! HOTFIX: it is not possible to open Ansi files if Unicode is a default encoding
      encoding = 0;

   if (!_config.load(path, encoding))
      return false;

   rename(path);
   refresh();

   _changed = false;
   return true;
}

void Project :: reset()
{
   _config.clear();

   _name.clear();
   _path.clear();

   refresh();

   // should be the last to prevent being marked as changed
   _changed = false;
}

void Project :: save()
{
   Path cfgPath(_path, _name);
   cfgPath.appendExtension(_T("prj"));

   _config.save(cfgPath, Settings::defaultEncoding);

   _changed = false;
}

void Project :: rename(const _path_t* path)
{
   _name.copyName(path);
   _path.copyPath(path);
   _path.lower();

   Paths::resolveRelativePath(_path);

   _changed = true;
}

void Project :: retrieveName(_ELENA_::Path& path, _ELENA_::ReferenceNs & name)
{
   const _path_t* root = _path;
   int rootLength = getlength(root);

   Path fullPath;
   fullPath.copyPath(path);
   Paths::resolveRelativePath(fullPath, root);
   fullPath.lower();

   if (!emptystr(root) && StringHelper::compare(fullPath, root, rootLength)) {
      name.copy(getPackage());
      if (getlength(fullPath) > rootLength)
         name.pathToName(fullPath + rootLength + 1);

      path.copy(path + rootLength + 1);
   }
   else {
      root = Paths::packageRoot;
      rootLength = getlength(root);

      if (!emptystr(root) && StringHelper::compare(fullPath, root, rootLength)) {
         name.pathToName(fullPath + rootLength + 1);

         // skip the root path
         path.copySubPath(path + rootLength + 1);
      }
      else {
         FileName fileName(fullPath);

         name.copy(fileName);
      }
   }
}

bool Project :: isIncluded(const _path_t* path)
{
   Path relPath(path);
   Paths::makeRelativePath(relPath, _path);

   ConfigCategoryIterator it = SourceFiles();
   while (!it.Eof()) {
      if (StringHelper::compare(relPath, it.key())) {
         return true;
      }
      it++;
   }
   return false;
}

void Project :: includeSource(const _path_t* path)
{
   Path relPath(path);
   Paths::makeRelativePath(relPath, _path);

   _config.setSetting(IDE_FILES_SECTION, StringHelper::clone(relPath), DEFAULT_STR);

   _changed = true;
}

void Project :: excludeSource(const _path_t* path)
{
   Path relPath(path);
   Paths::makeRelativePath(relPath, _path);

   _config.clear(IDE_FILES_SECTION, relPath);

   _changed = true;
}

void Project :: clearForwards()
{
   _config.clear(IDE_FORWARDS_SECTION);

   _changed = true;
}

void Project :: addForward(const _text_t* name, const _text_t* reference)
{
   _config.setSetting(IDE_FORWARDS_SECTION, name, reference);

   _changed = true;
}

void Project :: retrievePath(const wchar16_t* name, _ELENA_::Path & path, const _path_t* extension)
{
   const _text_t* package = getPackage();

   // if it is the root package
   if (StringHelper::compare(name, package)) {
      path.copy(_path);
      path.combine(getOutputPath());
      path.nameToPath(name, extension);
   }
   // if the class belongs to the project package
   else if (StringHelper::compare(name, package, getlength(package)) && (name[getlength(package)] == '\'')) {
      path.copy(_path);
      path.combine(getOutputPath());

      path.nameToPath(name, extension);
   }
   else {
      // if file doesn't exist use package root
      path.copy(Paths::libraryRoot);

      path.nameToPath(name, extension);
   }
}

