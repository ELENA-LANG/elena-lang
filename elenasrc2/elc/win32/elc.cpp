//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 command-line compiler
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#define __MSVCRT_VERSION__ 0x0800

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
#include "constants.h"
#include "errors.h"
#include "compiler.h"
#include "linker.h"
#include "image.h"
#include "x86jitcompiler.h"

#include <stdarg.h>
#include <windows.h>

// --- getAppPath ---

void getAppPath(_ELENA_::Path& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path);
   appPath.lower();
}

// --- Print ---

void print(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

void print(const char* str, ...)
{
   va_list argptr;
   va_start(argptr, str);

   vprintf(str, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

// --- ImageHelper ---

class ImageHelper : public _ELENA_::ExecutableImage::_Helper
{
   _ELENA_::Linker* _linker;

public:
   _ELENA_::ref_t tls_directory;

   virtual void beforeLoad(_ELENA_::_JITCompiler* compiler, _ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();

      // compile TLS section if it is a multi-threading app
      if (project->IntSetting(_ELENA_::opThreadMax) > 1) {
         _ELENA_::_JITLoader* loader = dynamic_cast<_ELENA_::_JITLoader*>(&image);

         _linker->prepareTLS(image, compiler->allocateTLSVariable(loader), tls_directory);

         // load GC thread table, should be allocated before static roots
         // thread table contains TLS reference
         compiler->allocateThreadTable(loader, project->IntSetting(_ELENA_::opThreadMax));
      }
   }

   virtual void afterLoad(_ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();

      _ELENA_::Section* debug = image.getDebugSection();

      // fix up debug section if required
      if (debug->Length() > 8) {
         debug->writeDWord(0, debug->Length());
         debug->addReference(image.getDebugEntryPoint(), 4);

         // save subject info if enabled
         _ELENA_::MemoryWriter debugWriter(debug);
         if (project->BoolSetting(_ELENA_::opDebugSubjectInfo)) {
            image.saveSubject(&debugWriter);
         }
         else debugWriter.writeDWord(0);
      }
      else debug->clear();
   }

   ImageHelper(_ELENA_::Linker* linker)
   {
      this->_linker = linker;
      this->tls_directory = 0;
   }
};

// --- Project ---

_ELC_::Project :: Project()
{
   getAppPath(appPath);
   _settings.add(_ELENA_::opAppPath, _ELENA_::IdentifierString::clonePath(appPath));
   _settings.add(_ELENA_::opNamespace, _ELENA_::StringHelper::clone("unnamed"));

   _tabSize = 4;
   _encoding = _ELENA_::feUTF8;

   // !! temporally
   _settings.add(_ELENA_::opDebugSubjectInfo, -1);
}

void _ELC_::Project :: raiseError(const char* msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);
   _ELENA_::WideString wTerminal(terminal);

   print(wMsg, (const wchar_t*)wPath, row, column, (const wchar_t*)wTerminal);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t identifier)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(identifier);

   print(wMsg, (const wchar_t*)wParam);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: printInfo(const char* msg, _ELENA_::ident_t value)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(value);

   print(wMsg, (const wchar_t*)wParam);
}

void _ELC_::Project::raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier)
{
   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wParam(identifier);

   print(wMsg, (const wchar_t*)wParam);

   if (throwExecption)
      throw _ELENA_::_Exception();
}

void _ELC_::Project::raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);
   _ELENA_::WideString wTerminal(terminal);

   print(wMsg, (const wchar_t*)wPath, row, column, (const wchar_t*)wTerminal);
}

void _ELC_::Project::raiseWarning(_ELENA_::ident_t msg, _ELENA_::ident_t path)
{
   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);

   print(wMsg, (const wchar_t*)wPath);
}

_ELENA_::ConfigCategoryIterator _ELC_::Project :: getCategory(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
{
   switch (setting)
   {
   case _ELENA_::opTemplates:
      return config.getCategoryIt(TEMPLATE_CATEGORY);
   case _ELENA_::opPrimitives:
      return config.getCategoryIt(PRIMITIVE_CATEGORY);
   case _ELENA_::opSources:
      return config.getCategoryIt(SOURCE_CATEGORY);
   case _ELENA_::opForwards:
      return config.getCategoryIt(FORWARD_CATEGORY);
   case _ELENA_::opExternals:
      return config.getCategoryIt(EXTERNALS_CATEGORY);
   case _ELENA_::opWinAPI:
      return config.getCategoryIt(WINAPI_CATEGORY);
   default:
      return _ELENA_::ConfigCategoryIterator();
   }
}

_ELENA_::ident_t _ELC_::Project::getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
{
   switch (setting)
   {
   case _ELENA_::opEntry:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_ENTRY);
   case _ELENA_::opNamespace:
      return config.getSetting(PROJECT_CATEGORY, ELC_NAMESPACE);
   case _ELENA_::opGCMGSize:
      return config.getSetting(LINKER_CATEGORY, ELC_MG_SIZE);
   case _ELENA_::opGCYGSize:
      return config.getSetting(LINKER_CATEGORY, ELC_YG_SIZE);
//   case _ELENA_::opSizeOfStackReserv:
//      return config.getSetting(LINKER_CATEGORY, ELC_STACK_RESERV);
//   case _ELENA_::opSizeOfStackCommit:
//      return config.getSetting(LINKER_CATEGORY, ELC_STACK_COMMIT);
//   case _ELENA_::opSizeOfHeapReserv:
//      return config.getSetting(LINKER_CATEGORY, ELC_HEAP_RESERV);
//   case _ELENA_::opSizeOfHeapCommit:
//      return config.getSetting(LINKER_CATEGORY, ELC_HEAP_COMMIT);
//   case _ELENA_::opImageBase:
//      return config.getSetting(LINKER_CATEGORY, ELC_YG_IMAGEBASE);
   case _ELENA_::opPlatform:
      return config.getSetting(SYSTEM_CATEGORY, ELC_PLATFORMTYPE);
   case _ELENA_::opTarget:
      return config.getSetting(PROJECT_CATEGORY, ELC_TARGET);
   case _ELENA_::opLibPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_LIB_PATH);
   case _ELENA_::opOutputPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_OUTPUT_PATH);
   case _ELENA_::opWarnOnUnresolved:
      return config.getSetting(PROJECT_CATEGORY, ELC_WARNON_UNRESOLVED);
//   case _ELENA_::opWarnOnSignature:
//      return config.getSetting(PROJECT_CATEGORY, ELC_WARNON_SIGNATURE);
   case _ELENA_::opDebugMode:
      return config.getSetting(PROJECT_CATEGORY, ELC_DEBUGINFO);
   case _ELENA_::opDebugSubjectInfo:
      return config.getSetting(PROJECT_CATEGORY, ELC_SUBJECTINFO);
   case _ELENA_::opThreadMax:
      return config.getSetting(SYSTEM_CATEGORY, ELC_SYSTEM_THREADMAX);
   case _ELENA_::opL0:
      return config.getSetting(COMPILER_CATEGORY, ELC_L0);
   case _ELENA_::opL1:
      return config.getSetting(COMPILER_CATEGORY, ELC_L1);
//   case _ELENA_::opL2:
//      return config.getSetting(COMPILER_CATEGORY, ELC_L2);
   case _ELENA_::opTemplate:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   default:
      return NULL;
   }
}

void _ELC_::Project :: addSource(_ELENA_::path_t path)
{
   _ELENA_::Path fullPath;
   _ELENA_::Path::loadPath(fullPath, StrSetting(_ELENA_::opProjectPath));
   fullPath.combine(path);

   _ELENA_::ident_c name[IDENTIFIER_LEN];
   _ELENA_::Path::savePath(path, name, IDENTIFIER_LEN);

   _settings.add(_ELENA_::opSources, name, _ELENA_::IdentifierString::clonePath(fullPath));
}

void _ELC_::Project :: cleanUp()
{
   _ELENA_::Path rootPath;
   _ELENA_::Path::loadPath(rootPath, StrSetting(_ELENA_::opProjectPath));
   _ELENA_::Path::combinePath(rootPath, StrSetting(_ELENA_::opOutputPath));

   for(_ELENA_::SourceIterator it = getSourceIt() ; !it.Eof() ; it++) {
      _ELENA_::Path path;
      _ELENA_::Path::loadSubPath(path, it.key());

      _ELENA_::ReferenceNs name(StrSetting(_ELENA_::opNamespace));
      name.pathToName(path);          // get a full name

      // remove module
      path.copy(rootPath);
      _loader.nameToPath(name, path, "nl");
      _wremove(path);

      // remove debug module
      path.copy(rootPath);
      _loader.nameToPath(name, path, "dnl");
      _wremove(path);
   }
}

void _ELC_ :: Project::loadConfig(_ELENA_::path_t path, bool root, bool requiered)
{
   ElcConfigFile config;
   _ELENA_::Path configPath;

   configPath.copySubPath(path);

   if (!config.load(path, getDefaultEncoding())) {
      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, _ELENA_:: IdentifierString(path));
      return;
   }

   // load template list
   if (root)
      loadCategory(config, _ELENA_::opTemplates, configPath);

   // load template
   _ELENA_::ident_t projectTemplate = config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   if (!_ELENA_::emptystr(projectTemplate)) {
      _ELENA_::ident_t templateFile = _settings.get(_ELENA_::opTemplates, projectTemplate, DEFAULT_STR);
      if (!_ELENA_::emptystr(templateFile)) {
         _ELENA_::Path templatePath;
         _ELENA_::Path::loadPath(templatePath, templateFile);

         loadConfig(templatePath, false, false);
      }
      else raiseErrorIf(requiered, ELC_ERR_INVALID_TEMPLATE, projectTemplate);
   }

   loadConfig(config, configPath);
}

void _ELC_::Project :: setOption(const wchar_t* value)
{
   _ELENA_::IdentifierString valueName;
   _ELENA_::Path::savePath(value, valueName, IDENTIFIER_LEN);

   switch ((char)value[0]) {
      case ELC_PRM_LIB_PATH:
         _settings.add(_ELENA_::opLibPath, _ELENA_::StringHelper::clone(valueName + 1));
         break;
      case ELC_PRM_OUTPUT_PATH:
         _settings.add(_ELENA_::opOutputPath, _ELENA_::StringHelper::clone(valueName + 1));
         break;
      case ELC_PRM_EXTRA:
         if (_ELENA_::StringHelper::compare(valueName, ELC_PRM_TABSIZE, 4)) {
            _tabSize = _ELENA_::StringHelper::strToInt(value + 4);
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_PRM_CODEPAGE, 3)) {
            _encoding = _ELENA_::StringHelper::strToInt(value + 3);
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_PRM_PROJECTPATH, _ELENA_::getlength(ELC_PRM_PROJECTPATH))) {
            _settings.add(_ELENA_::opProjectPath, _ELENA_::StringHelper::clone(valueName + _ELENA_::getlength(ELC_PRM_PROJECTPATH)));
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_PRM_OPTOFF)) {
            _settings.add(_ELENA_::opL0, 0);
            _settings.add(_ELENA_::opL1, 0);
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_PRM_OPT1OFF)) {
            _settings.add(_ELENA_::opL1, 0);
         }
         else raiseError(ELC_ERR_INVALID_OPTION, valueName);
         break;
      case ELC_PRM_WARNING:
         if (_ELENA_::StringHelper::compare(valueName, ELC_W_UNRESOLVED)) {
            _settings.add(_ELENA_::opWarnOnUnresolved, -1);
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_W_WEAKUNRESOLVED)) {
            _settings.add(_ELENA_::opWarnOnWeakUnresolved, -1);
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_W_LEVEL1)) {
            _warningMasks = _ELENA_::WARNING_MASK_1;
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_W_LEVEL2)) {
            _warningMasks = _ELENA_::WARNING_MASK_2;
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_W_LEVEL3)) {
            _warningMasks = _ELENA_::WARNING_MASK_3;
         }
         else if (_ELENA_::StringHelper::compare(valueName, ELC_W_OFF)) {
            _warningMasks = 0;
         }
         else raiseError(ELC_ERR_INVALID_OPTION, valueName);
         break;
      case ELC_PRM_TARGET:
         _settings.add(_ELENA_::opTarget, _ELENA_::StringHelper::clone(valueName + 1));
         break;
      case ELC_PRM_START:
         _settings.add(_ELENA_::opEntry, _ELENA_::StringHelper::clone(valueName + 1));
         break;
      case ELC_PRM_DEBUGINFO:
         if (_ELENA_::StringHelper::compare(valueName, ELC_SUBJECTINFO)) {
            _settings.add(_ELENA_::opDebugSubjectInfo, -1);
         }
         _settings.add(_ELENA_::opDebugMode, -1);
         break;
      case ELC_PRM_CONFIG:
      {
         projectName.copy(valueName + 1);

         loadConfig(value + 1);

         _ELENA_::Path projectPath;
         projectPath.copySubPath(value + 1);
         _settings.add(_ELENA_::opProjectPath, _ELENA_::IdentifierString::clonePath(projectPath));

         break;
      }
      default:
         raiseError(ELC_ERR_INVALID_OPTION, valueName);
   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

void setCompilerOptions(_ELC_::Project& project, _ELENA_::Compiler& compiler)
{
   if (project.IntSetting(_ELENA_::opL0, -1) != 0) {
      _ELENA_::Path rulesPath;
      _ELENA_::Path::loadPath(rulesPath, project.StrSetting(_ELENA_::opAppPath));
      _ELENA_::Path::combinePath(rulesPath, RULES_FILE);

      _ELENA_::FileReader rulesFile(rulesPath, _ELENA_::feRaw, false);
      if (!rulesFile.isOpened()) {
         project.raiseWarning(errInvalidFile, RULES_FILE);
      }
      else compiler.loadRules(&rulesFile);
   }
   if (project.IntSetting(_ELENA_::opL1, -1) != 0) {
      compiler.turnOnOptimiation(1);
   }
}

// --- Main function ---

const char* showPlatform(int platform)
{
   if (platform == _ELENA_::ptWin32Console) {
      return ELC_WIN32CONSOLE;
   }
   else if (platform == _ELENA_::ptWin32ConsoleX) {
      return ELC_WIN32CONSOLEX;
   }
   else if (platform == _ELENA_::ptVMWin32Console) {
      return ELC_WIN32VMCONSOLEX;
   }
   else if (platform == _ELENA_::ptWin32GUI) {
      return ELC_WIN32GUI;
   }
   else if (platform == _ELENA_::ptWin32GUIX) {
      return ELC_WIN32GUIX;
   }
   else if (platform == _ELENA_::ptLibrary) {
      return ELC_LIBRARY;
   }
   else return ELC_UNKNOWN;
}

int main()
{
   int argc;
   wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

   int    exitCode = 0;
   _ELC_::Project project;

   try {
      print(ELC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELC_REVISION_NUMBER);

      if (argc < 2) {
         // show help if no parameters proveded
         print(ELC_HELP_INFO);
         return -3;
      }

      // Initializing..
      _ELENA_::Path configPath(project.appPath);
      _ELENA_::Path::combinePath(configPath, DEFAULT_CONFIG);

      project.loadConfig(configPath, true, false);

      // Initializing..
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0]=='-') {
            project.setOption(argv[i] + 1);
         }
         else project.addSource(argv[i]);
      }
      project.initLoader();

      int platform = project.IntSetting(_ELENA_::opPlatform);

      // Greetings
      print(ELC_STARTING, (_ELENA_::ident_t)project.projectName, showPlatform(platform));

      // Cleaning up
      print("Cleaning up...");
      project.cleanUp();

      // Compiling..
      print(ELC_COMPILING);

      _ELENA_::Path syntaxPath;
      _ELENA_::Path::loadPath(syntaxPath, project.StrSetting(_ELENA_::opAppPath));
      _ELENA_::Path::combinePath(syntaxPath, SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath, _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseErrorIf(true, errInvalidFile, SYNTAX_FILE);

      // compile normal project
      bool result = false;
      _ELENA_::Compiler compiler(&syntaxFile);
      setCompilerOptions(project, compiler);

      result = compiler.run(project);

      if (result)
         print(ELC_SUCCESSFUL_COMPILATION);
      else {
         exitCode = -1;
         print(ELC_WARNING_COMPILATION);
      }

      // Linking..
      if (platform == _ELENA_::ptWin32Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32ConsoleX) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);

         linker.run(project, image, helper.tls_directory);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptVMWin32Console) {
         print(ELC_LINKING);

         _ELENA_::VirtualMachineClientImage image(
            &project, project.createJITCompiler());

         _ELENA_::Linker linker;
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32GUI) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t) - 1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32GUIX) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker);
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         linker.run(project, image, helper.tls_directory);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptLibrary) {
         // no linking for the library
      }
      else print(ELC_UNKNOWN_PLATFORM);
   }
   catch(_ELENA_::InternalError& e) {
      print(_ELENA_::WideString(ELC_INTERNAL_ERROR), (const wchar_t*)_ELENA_::WideString(e.message));
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITUnresolvedException& ex)
   {
      project.printInfo(errUnresovableLink, ex.reference);
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITConstantExpectedException& ex)
   {
      project.printInfo(errConstantExpectedLink, ex.reference);
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::_Exception&) {
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   return exitCode;
}
