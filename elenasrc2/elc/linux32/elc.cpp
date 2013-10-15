//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
//#include "errors.h"
//#include "compiler.h"
//#include "linker.h"
//#include "image.h"
//#include "win32\x86jitcompiler.h"

#include <stdarg.h>

// --- Project ---

_ELC_::Project :: Project()
{
//   getAppPath(appPath);
//   _settings.add(_ELENA_::opAppPath, _ELENA_::StringHelper::cloneWide(appPath));
//
//   _tabSize = 4;
//   _encoding = CP_OEMCP;
}

void _ELC_::Project :: printInfo(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf(_T("\n"));

   fflush(stdout);
}

void _ELC_::Project :: raiseWarning(const char* msg, ...)
{
   if (!indicateWarning())
      return;

   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf(_T("\n"));

   fflush(stdout);
}

void _ELC_::Project :: raiseError(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf(_T("\n"));

   fflush(stdout);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseErrorIf(bool throwExecption, const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf(_T("\n"));

   fflush(stdout);

   if (throwExecption)
      throw _ELENA_::_Exception();
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
   case _ELENA_::opPreloadedSubjects:
      return config.getCategoryIt(PRELOADEDSUBJECT_CATEGORY);
   default:
      return _ELENA_::ConfigCategoryIterator();
   }
}

const _text_t* _ELC_::Project :: getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
{
   switch (setting)
   {
   case _ELENA_::opEntry:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_START);
   case _ELENA_::opPackage:
      return config.getSetting(PROJECT_CATEGORY, ELC_PACKAGE);
   case _ELENA_::opGCHeapSize:
      return config.getSetting(LINKER_CATEGORY, ELC_GC_PAGESIZE);
   case _ELENA_::opGCObjectSize:
      return config.getSetting(LINKER_CATEGORY, ELC_GC_OBJSIZE);
   case _ELENA_::opYGRatio:
      return config.getSetting(LINKER_CATEGORY, ELC_YG_RATIO);
   case _ELENA_::opSizeOfStackReserv:
      return config.getSetting(LINKER_CATEGORY, ELC_STACK_RESERV);
   case _ELENA_::opSizeOfStackCommit:
      return config.getSetting(LINKER_CATEGORY, ELC_STACK_COMMIT);
   case _ELENA_::opSizeOfHeapReserv:
      return config.getSetting(LINKER_CATEGORY, ELC_HEAP_RESERV);
   case _ELENA_::opSizeOfHeapCommit:
      return config.getSetting(LINKER_CATEGORY, ELC_HEAP_COMMIT);
   case _ELENA_::opImageBase:
      return config.getSetting(LINKER_CATEGORY, ELC_YG_IMAGEBASE);
   case _ELENA_::opApplicationType:
      return config.getSetting(SYSTEM_CATEGORY, ELC_APPTYPE);
   case _ELENA_::opPlatformType:
      return config.getSetting(PROJECT_CATEGORY, ELC_PLATFORMTYPE);
   case _ELENA_::opTarget:
      return config.getSetting(PROJECT_CATEGORY, ELC_TARGET);
   case _ELENA_::opLibPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_LIB_PATH);
   case _ELENA_::opOutputPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_OUTPUT_PATH);
   case _ELENA_::opWarnOnUnresolved:
      return config.getSetting(PROJECT_CATEGORY, ELC_WARNON_UNRESOLVED);
   case _ELENA_::opWarnOnSignature:
      return config.getSetting(PROJECT_CATEGORY, ELC_WARNON_SIGNATURE);
   case _ELENA_::opDebugMode:
      return config.getSetting(PROJECT_CATEGORY, ELC_DEBUGINFO);
   case _ELENA_::opStarter:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_ENTRY);
   case _ELENA_::opVMPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_VM_PATH);
   case _ELENA_::opJITType:
      return config.getSetting(COMPILER_CATEGORY, ELC_JIT);
   case _ELENA_::opThreadMax:
      return config.getSetting(SYSTEM_CATEGORY, ELC_SYSTEM_THREADMAX);
   case _ELENA_::opL0:
      return config.getSetting(COMPILER_CATEGORY, ELC_L0);
   case _ELENA_::opL1:
      return config.getSetting(COMPILER_CATEGORY, ELC_L1);
   case _ELENA_::opL2:
      return config.getSetting(COMPILER_CATEGORY, ELC_L2);
   case _ELENA_::opTemplate:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   default:
      return NULL;
   }
}

void _ELC_::Project :: addSource(const _path_t* path)
{
//   _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath), path);
//   fullPath.lower();
//
//   _settings.add(_ELENA_::opSources, path, _ELENA_::StringHelper::cloneWide(fullPath));
}

void _ELC_::Project :: cleanUp()
{
//   for(_ELENA_::SourceIterator it = getSourceIt() ; !it.Eof() ; it++) {
//      _ELENA_::ReferenceNs name(StrSetting(_ELENA_::opPackage));
//      _ELENA_::Path path(StrSetting(_ELENA_::opProjectPath), StrSetting(_ELENA_::opOutputPath));
//
//      name.pathToName(it.key());          // get a full name
//
//      _loader.nameToPath(name, path, _T("nl"));   // get a full path
//
//      _wremove(path);
//   }
}

void _ELC_::Project :: loadConfig(const _path_t* path, bool requiered)
{
//   ElcConfigFile config;
//   _ELENA_::Path configPath;
//
//   configPath.copyPath(path);
//
//   if (!config.load(path, getDefaultEncoding())) {
//      _ELENA_::PrintableValue value(path);
//
//      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, (const _text_t*)value);
//      return;
//   }
//
//   // load template list
//   loadCategory(config, _ELENA_::opTemplates, configPath);
//
//   // load template
//   _ELENA_::ProjectParam projectTemplate(config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE));
//   if (!projectTemplate.isEmpty()) {
//      const wchar_t* templateFile = _settings.get(_ELENA_::opTemplates, projectTemplate, DEFAULT_STR);
//      if (_ELENA_::emptystr(templateFile)) {
//         raiseErrorIf(requiered, ELC_ERR_INVALID_TEMPLATE, (const wchar16_t*)projectTemplate);
//      }
//      else loadConfig(templateFile, false);
//   }
//
//   loadConfig(config, configPath);
}

void _ELC_::Project :: setOption(const _text_t* value)
{
//   _ELENA_::ProjectParam param;
//   param.copy(value + 1);
//
//   switch (value[0]) {
//      case ELC_PRM_LIB_PATH:
//         _settings.add(_ELENA_::opLibPath, param.clone());
//         break;
//      case ELC_PRM_OUTPUT_PATH:
//         _settings.add(_ELENA_::opOutputPath, param.clone());
//         break;
//      case ELC_PRM_PACKAGE:
//         _settings.add(_ELENA_::opPackage, param.clone());
//         break;
//      case ELC_PRM_LIBRARY:
//         if (_ELENA_::StringHelper::compare(value, ELC_PRM_STANDART_LIBRARY)) {
//            _settings.add(_ELENA_::opStandart, -1);
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_STANDART_LIBRARY_DBG)) {
//            _settings.add(_ELENA_::opStandart, -2);
//         }
//         else raiseError(ELC_ERR_INVALID_OPTION, value);
//         break;
//      case ELC_PRM_EXTRA:
//         if (_ELENA_::StringHelper::compare(value, ELC_PRM_TABSIZE, 4)) {
//            _tabSize = _ELENA_::StringHelper::strToInt(param + 3);
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_CODEPAGE, 3)) {
//            _encoding = _ELENA_::StringHelper::strToInt(param + 2);
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_UNICODE)) {
//            _settings.add(_ELENA_::opOutputPath, param.clone());
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_PROJECTPATH, _ELENA_::getlength(ELC_PRM_PROJECTPATH))) {
//            _settings.add(_ELENA_::opProjectPath, param.clone(_ELENA_::getlength(ELC_PRM_PROJECTPATH) - 1));
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_OPTOFF)) {
//            _settings.add(_ELENA_::opL0, 0);
//            _settings.add(_ELENA_::opL1, 0);
//            _settings.add(_ELENA_::opL2, 0);
//         }
//         else raiseError(ELC_ERR_INVALID_OPTION, value);
//         break;
//      case ELC_PRM_WARNING:
//         if (_ELENA_::StringHelper::compare(value, ELC_W_UNRESOLVED)) {
//            _settings.add(_ELENA_::opWarnOnUnresolved, -1);
//         }
//         else if (_ELENA_::StringHelper::compare(value, ELC_W_WEAKUNRESOLVED)) {
//            _settings.add(_ELENA_::opWarnOnWeakUnresolved, -1);
//         }
//         else raiseError(ELC_ERR_INVALID_OPTION, value);
//         break;
//      case ELC_PRM_TARGET:
//         _settings.add(_ELENA_::opTarget, param.clone());
//         break;
//      //case ELC_PRM_MAP:
//      //   _settings.add(_ELENA_::opMapFile, value + 1);
//      //   break;
//      case ELC_PRM_ENTRY:
//         loadForward(STARTUP_CLASS, param.clone());
//         break;
//      case ELC_PRM_START:
//         _settings.add(_ELENA_::opEntry, param.clone());
//         break;
//      case ELC_PRM_DEBUGINFO:
//         _settings.add(_ELENA_::opDebugMode, -1);
//         break;
//      case ELC_PRM_CONFIG:
//      {
//         loadConfig(value + 1);
//
//         _ELENA_::Path projectPath;
//         projectPath.copyPath(value + 1);
//         _settings.add(_ELENA_::opProjectPath, projectPath.clone());
//
//         break;
//      }
//      default:
//         raiseError(ELC_ERR_INVALID_OPTION, value[0]);
//   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: getJITCompiler()
{
   return NULL; // !! temporal
//   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

//void setCompilerOptions(_ELC_::Project& project, _ELENA_::Compiler& compiler)
//{
//   if (project.IntSetting(_ELENA_::opL0) != 0) {
//      // !! temporal: there should be several optimization levels
//      _ELENA_::Path rulesPath(project.StrSetting(_ELENA_::opAppPath), RULES_FILE);
//      _ELENA_::FileReader rulesFile(rulesPath, _ELENA_::feRaw, false);
//      if (!rulesFile.isOpened())
//         project.raiseError(errInvalidFile, rulesPath);
//
//      compiler.loadRules(&rulesFile);
//   }
//   if (project.IntSetting(_ELENA_::opL1) != 0) {
//      compiler.setOptFlag(_ELENA_::optDirectConstant);
//   }
//   if (project.IntSetting(_ELENA_::opL2) != 0) {
//      compiler.setOptFlag(_ELENA_::optJumps);
//   }
//}

// --- Main function ---

//int launchProc;

int main(int argc, char* argv[])
{
   int    exitCode = 0;
   _ELC_::Project project;

   try {
      project.printInfo(ELC_GREETING, ELC_MAJOR_VERSION, ELC_MINOR_VERSION, ELC_BUILD_NUMBER);

      if (argc < 2) {
         // show help if no parameters proveded
         project.printInfo(ELC_HELP_INFO);
         return -3;
      }

//      // Initializing..
//      project.loadConfig(_ELENA_::Path(project.appPath, DEFAULT_CONFIG), false);
//
//      // Initializing..
//      for (int i = 1 ; i < argc ; i++) {
//         if (argv[i][0]=='-') {
//            project.setOption(argv[i] + 1);
//         }
//         else project.addSource(argv[i]);
//      }
//      project.initLoader();
//
//      // Cleaning up
//      project.printInfo(_T("Cleaning up..."));
//      project.cleanUp();
//
//      // Compiling..
//      project.printInfo(ELC_COMPILING);
//
//      _ELENA_::Path syntaxPath(project.StrSetting(_ELENA_::opAppPath), SYNTAX_FILE);
//      _ELENA_::FileReader syntaxFile(syntaxPath, _ELENA_::feRaw, false);
//      if (!syntaxFile.isOpened())
//         project.raiseError(errInvalidFile, (const _path_t*)syntaxPath);
//
//      // compile normal project
//      bool result = false;
//      if (!project.BoolSetting(_ELENA_::opStandart)) {
//         // load core module
//         project.loadModule(STANDARD_MODULE, false);
//
//         _ELENA_::Compiler compiler(&syntaxFile);
//
//         setCompilerOptions(project, compiler);
//
//         result = compiler.run(project);
//      }
//      // there is a special routine for a default module
//      else {
//         _ELENA_::StdCompiler compiler(&syntaxFile, project.IntSetting(_ELENA_::opStandart)==-2);
//
//         setCompilerOptions(project, compiler);
//
//         result = compiler.run(project);
//      }
//
//      if (result)
//         project.printInfo(ELC_SUCCESSFUL_COMPILATION);
//      else {
//         exitCode = -1;
//         project.printInfo(ELC_WARNING_COMPILATION);
//      }
//
//      // Linking..
//      if (project.IntSetting(_ELENA_::opPlatformType) == _ELENA_::ptStandalone) {
//         project.printInfo(ELC_LINKING);
//
//         _ELENA_::ExecutableImage image(&project, project.getJITCompiler());
//         _ELENA_::Linker linker(&project);
//         // check if we need to create TLS table
//         if (image.getTLSSection()->Length() > 0) {
//            void* directory = image.resolveReference(TLS_KEY, _ELENA_::mskNativeRDataRef);
//
//            linker.run(image, (ref_t)directory & ~_ELENA_::mskAnyRef);
//         }
//         else linker.run(image, -1);
//
//         project.printInfo(ELC_SUCCESSFUL_LINKING);
//      }
//      else if (project.IntSetting(_ELENA_::opPlatformType) == _ELENA_::ptElenaVM) {
//         project.printInfo(ELC_LINKING);
//
//         if (_ELENA_::emptystr(project.StrSetting(_ELENA_::opVMPath)))
//            project.raiseError(ELC_WRN_MISSING_VMPATH);
//
//         _ELENA_::VirtualMachineClientImage image(
//            &project, project.getJITCompiler(), project.StrSetting(_ELENA_::opAppPath));
//
//         _ELENA_::Linker linker(&project);
//         linker.run(image, -1);
//
//         project.printInfo(ELC_SUCCESSFUL_LINKING);
//      }
   }
   catch(_ELENA_::InternalError& e) {
      project.printInfo(ELC_INTERNAL_ERROR, e.message);
      exitCode = -2;

      project.cleanUp();
   }
//   catch(_ELENA_::JITUnresolvedException& ex)
//   {
//      _ELENA_::PrintableValue value(ex.reference);
//
//      project.printInfo(errUnresovableLink, (const _text_t*)value);
//      project.printInfo(ELC_UNSUCCESSFUL);
//      exitCode = -2;
//
//      project.cleanUp();
//   }
//   catch(_ELENA_::JITConstantExpectedException& ex)
//   {
//      _ELENA_::PrintableValue value(ex.reference);
//
//      project.printInfo(errConstantExpectedLink, (const _text_t*)value);
//      project.printInfo(ELC_UNSUCCESSFUL);
//      exitCode = -2;
//
//      project.cleanUp();
//   }
   catch(_ELENA_::_Exception&) {
      project.printInfo(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   return exitCode;
}
