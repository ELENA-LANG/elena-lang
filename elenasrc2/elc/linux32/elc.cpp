//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
#include "constants.h"
#include "errors.h"
#include "compiler.h"
//#include "linker.h"
//#include "image.h"
//#include "win32\x86jitcompiler.h"

#include <stdarg.h>

// --- Project ---

void print(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

_ELC_::Project :: Project()
{
   appPath.copy(DATA_PATH);

//   getAppPath(appPath);
   _settings.add(_ELENA_::opAppPath, _ELENA_::StringHelper::clone(appPath));
   _settings.add(_ELENA_::opNamespace, _ELENA_::StringHelper::clone("unnamed"));

   _tabSize = 4;
   _encoding = _ELENA_::feUTF8;
}

void _ELC_::Project :: raiseError(const char* msg, const tchar_t* path, int row, int column, const wchar16_t* wTerminal)
{
   _ELENA_::UTF8String s(wTerminal);

   print(msg, path, row, column, s);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(const char* msg)
{
   print(msg);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(const char* msg, const char* value)
{
   print(msg, value);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(const char* msg, const wchar16_t* wValue)
{
   _ELENA_::UTF8String s(wValue);

   print(msg, s);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: printInfo(const char* msg, const char* value)
{
   print(msg, value);
}

void _ELC_::Project :: printInfo(const char* msg, const wchar16_t* wValue)
{
   _ELENA_::UTF8String s(wValue);

   print(msg, s);
}

//void _ELC_::Project :: printInfo(const wchar16_t* msg)
//{
//   print(msg);
//}

void _ELC_::Project :: raiseErrorIf(bool throwExecption, const char* msg, const tchar_t* path)
{
   print(msg, path);

   if (throwExecption)
      throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseWarning(const char* msg, const tchar_t* path, int row, int column, const wchar16_t* wTerminal)
{
   if (!indicateWarning())
      return;

   _ELENA_::UTF8String s(wTerminal);

   print(msg, path, row, column, s);
}

void _ELC_::Project :: raiseWarning(const char* msg, const tchar_t* path)
{
   if (!indicateWarning())
      return;

   print(msg, path);
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
   default:
      return _ELENA_::ConfigCategoryIterator();
   }
}

const char* _ELC_::Project :: getOption(_ELENA_::_ConfigFile& config, _ELENA_::ProjectSetting setting)
{
   switch (setting)
   {
   case _ELENA_::opEntry:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_ENTRY);
   case _ELENA_::opNamespace:
      return config.getSetting(PROJECT_CATEGORY, ELC_NAMESPACE);
   case _ELENA_::opGCMGSize:
      return config.getSetting(LINKER_CATEGORY, ELC_MG_SIZE);
   case _ELENA_::opGCObjectSize:
      return config.getSetting(LINKER_CATEGORY, ELC_GC_OBJSIZE);
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
   case _ELENA_::opEmbeddedSymbolMode:
      return config.getSetting(PROJECT_CATEGORY, ELC_SYMBOLINFO);
//   case _ELENA_::opStarter:
//      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_ENTRY);
   case _ELENA_::opVMPath:
      return config.getSetting(PROJECT_CATEGORY, ELC_VM_PATH);
   case _ELENA_::opThreadMax:
      return config.getSetting(SYSTEM_CATEGORY, ELC_SYSTEM_THREADMAX);
   case _ELENA_::opL0:
      return config.getSetting(COMPILER_CATEGORY, ELC_L0);
//   case _ELENA_::opL1:
//      return config.getSetting(COMPILER_CATEGORY, ELC_L1);
//   case _ELENA_::opL2:
//      return config.getSetting(COMPILER_CATEGORY, ELC_L2);
   case _ELENA_::opL3:
      return config.getSetting(COMPILER_CATEGORY, ELC_L3);
   case _ELENA_::opTemplate:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   default:
      return NULL;
   }
}

void _ELC_::Project :: addSource(const tchar_t* path)
{
   _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath), path);
   fullPath.lower();

   _sources.add(path, _ELENA_::StringHelper::clone(fullPath));
}

void _ELC_::Project :: cleanUp()
{
//   _ELENA_::Path rootPath(StrSetting(_ELENA_::opProjectPath), StrSetting(_ELENA_::opOutputPath));
//
//   for(_ELENA_::SourceIterator it = getSourceIt() ; !it.Eof() ; it++) {
//      _ELENA_::Path path;
//      path.copyPath(it.key());
//
//      _ELENA_::ReferenceNs name(StrSetting(_ELENA_::opNamespace));
//      name.pathToName(path);          // get a full name
//
//      // remove module
//      path.copy(rootPath);
//      _loader.nameToPath(name, path, _T("nl"));
//      _wremove(path);
//
//      // remove debug module
//      path.copy(rootPath);
//      _loader.nameToPath(name, path, _T("dnl"));
//      _wremove(path);
//   }
}

void _ELC_::Project :: loadConfig(const tchar_t* path, bool root, bool requiered)
{
   ElcConfigFile config;
   _ELENA_::Path configPath;

   configPath.copyPath(path);

   if (!config.load(path, getDefaultEncoding())) {
      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, path);
      return;
   }

   // load template list
   if (root)
      loadCategory(config, _ELENA_::opTemplates, configPath);

   // load template
   _ELENA_::ProjectParam projectTemplate(config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE));
   if (!projectTemplate.isEmpty()) {
      const char* templateFile = _settings.get(_ELENA_::opTemplates, projectTemplate, (const char*)NULL);
      if (_ELENA_::emptystr(templateFile)) {
        _ELENA_::String<char, 255> str(projectTemplate);

         raiseErrorIf(requiered, ELC_ERR_INVALID_TEMPLATE, (const char*)str);
      }
      else loadConfig(templateFile, false, false);
   }

   loadConfig(config, configPath);
}

void _ELC_::Project :: setOption(const tchar_t* value)
{
//   switch ((char)value[0]) {
//      case ELC_PRM_LIB_PATH:
//         _settings.add(_ELENA_::opLibPath, _ELENA_::StringHelper::clone(value + 1));
//         break;
//      case ELC_PRM_OUTPUT_PATH:
//         _settings.add(_ELENA_::opOutputPath, _ELENA_::StringHelper::clone(value + 1));
//         break;
////      case ELC_PRM_PACKAGE:
////         _settings.add(_ELENA_::opPackage, _ELENA_::StringHelper::clone(value + 1));
////         break;
////      case ELC_PRM_LIBRARY:
////         if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_STANDART_LIBRARY)) {
////            _settings.add(_ELENA_::opStandart, -1);
////         }
////         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_STANDART_LIBRARY_DBG)) {
////            _settings.add(_ELENA_::opStandart, -2);
////         }
////         else raiseError(ELC_ERR_INVALID_OPTION, value);
////         break;
//      case ELC_PRM_EXTRA:
//         if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_TABSIZE, 4)) {
//            _tabSize = _ELENA_::StringHelper::strToInt(value + 4);
//         }
//         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_CODEPAGE, 3)) {
//            _encoding = _ELENA_::StringHelper::strToInt(value + 3);
//         }
//////         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_UNICODE)) {
//////            _settings.add(_ELENA_::opOutputPath, _ELENA_::StringHelper::clone(value + 1));
//////         }
//         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_PROJECTPATH, _ELENA_::getlength(ELC_PRM_PROJECTPATH))) {
//            _settings.add(_ELENA_::opProjectPath, _ELENA_::StringHelper::clone(value + _ELENA_::getlength(ELC_PRM_PROJECTPATH)));
//         }
//         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_OPTOFF)) {
//            _settings.add(_ELENA_::opL0, 0);
////            _settings.add(_ELENA_::opL1, 0);
////            _settings.add(_ELENA_::opL2, 0);
//            _settings.add(_ELENA_::opL3, 0);
//         }
//         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_PRM_SYMBOLEMBEDOFF)) {
//            _settings.add(_ELENA_::opEmbeddedSymbolMode, 0);
//         }
////         else raiseError(ELC_ERR_INVALID_OPTION, value);
//         break;
//      case ELC_PRM_WARNING:
//         if (_ELENA_::ConstantIdentifier::compare(value, ELC_W_UNRESOLVED)) {
//            _settings.add(_ELENA_::opWarnOnUnresolved, -1);
//         }
//         else if (_ELENA_::ConstantIdentifier::compare(value, ELC_W_WEAKUNRESOLVED)) {
//            _settings.add(_ELENA_::opWarnOnWeakUnresolved, -1);
//         }
//         else raiseError(ELC_ERR_INVALID_OPTION, value);
//         break;
//      case ELC_PRM_TARGET:
//         _settings.add(_ELENA_::opTarget, _ELENA_::StringHelper::clone(value + 1));
//         break;
//////      //case ELC_PRM_MAP:
//////      //   _settings.add(_ELENA_::opMapFile, value + 1);
//////      //   break;
//////      case ELC_PRM_ENTRY:
//////         loadForward(_ELENA_::ConstantIdentifier(STARTUP_CLASS), _ELENA_::StringHelper::clone(value + 1));
//////         break;
////      case ELC_PRM_START:
////         _settings.add(_ELENA_::opEntry, _ELENA_::StringHelper::clone(value + 1));
////         break;
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
//         raiseError(ELC_ERR_INVALID_OPTION, value);
//   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return NULL; //!! new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode), BoolSetting(_ELENA_::opEmbeddedSymbolMode));
}

void setCompilerOptions(_ELC_::Project& project, _ELENA_::Compiler& compiler)
{
//   if (project.IntSetting(_ELENA_::opL0, -1) != 0) {
//      // !! temporal: there should be several optimization levels
//      _ELENA_::Path rulesPath(project.StrSetting(_ELENA_::opAppPath), RULES_FILE);
//      _ELENA_::FileReader rulesFile(rulesPath, _ELENA_::feRaw, false);
//      if (!rulesFile.isOpened()) {
//         project.raiseWarning(errInvalidFile, rulesPath);
//      }
//      else compiler.loadRules(&rulesFile);
//   }
////   if (project.IntSetting(_ELENA_::opL1) != 0) {
////      compiler.setOptFlag(_ELENA_::optDirectConstant);
////   }
////   if (project.IntSetting(_ELENA_::opL2) != 0) {
////      compiler.setOptFlag(_ELENA_::optJumps);
////   }
//   if (project.IntSetting(_ELENA_::opL3, -1) != 0) {
//      compiler.setOptFlag(_ELENA_::optIdleFrame);
//   }
}

// --- Main function ---

int main(int argc, char* argv[])
{
   int    exitCode = 0;
   _ELC_::Project project;

   try {
      print(ELC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELC_BUILD_NUMBER);

      if (argc < 2) {
         // show help if no parameters proveded
         print(ELC_HELP_INFO);
         return -3;
      }

      // Initializing..
      project.loadConfig(_ELENA_::Path(/*project.appPath, */DEFAULT_CONFIG), true, false);

      // Initializing..
      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0]=='-') {
            project.setOption(argv[i] + 1);
         }
         else project.addSource(argv[i]);
      }
//      project.initLoader();

//      // Cleaning up
//      print("Cleaning up...");
//      project.cleanUp();

      // Compiling..
      print(ELC_COMPILING);

      _ELENA_::Path syntaxPath(project.StrSetting(_ELENA_::opAppPath), SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath, _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseError(errInvalidFile, (const tchar_t*)syntaxPath);

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

//      // Linking..
//      if (project.IntSetting(_ELENA_::opPlatform) == _ELENA_::ptWin32Console) {
//         print(ELC_LINKING);
//
//         _ELENA_::ExecutableImage image(&project, project.createJITCompiler());
//         _ELENA_::Linker linker;
//         linker.run(project, image, -1);
//
//         print(ELC_SUCCESSFUL_LINKING);
//      }
//      if (project.IntSetting(_ELENA_::opPlatform) == _ELENA_::ptWin32ConsoleMT) {
//         print(ELC_LINKING);
//
//         _ELENA_::ExecutableImage image(&project, project.createJITCompiler());
//         _ELENA_::Linker linker;
//
//         void* directory = image.resolveReference(_ELENA_::ConstantIdentifier(TLS_KEY), _ELENA_::mskNativeRDataRef);
//
//         linker.run(project, image, (ref_t)directory & ~_ELENA_::mskAnyRef);
//
//         print(ELC_SUCCESSFUL_LINKING);
//      }
//      else if (project.IntSetting(_ELENA_::opPlatform) == _ELENA_::ptVMWin32Console) {
//         print(ELC_LINKING);
//
//         if (_ELENA_::emptystr(project.StrSetting(_ELENA_::opVMPath)))
//            project.raiseError(ELC_WRN_MISSING_VMPATH);
//
//         _ELENA_::VirtualMachineClientImage image(
//            &project, project.createJITCompiler(), project.StrSetting(_ELENA_::opAppPath));
//
//         _ELENA_::Linker linker;
//         linker.run(project, image, -1);
//
//         print(ELC_SUCCESSFUL_LINKING);
//      }
   }
//   catch(_ELENA_::InternalError& e) {
//      print(ELC_INTERNAL_ERROR, e.message);
//      exitCode = -2;
//
//      project.cleanUp();
//   }
//   catch(_ELENA_::JITUnresolvedException& ex)
//   {
//      project.printInfo(errUnresovableLink, ex.reference);
//      print(ELC_UNSUCCESSFUL);
//      exitCode = -2;
//
//      project.cleanUp();
//   }
//   catch(_ELENA_::JITConstantExpectedException& ex)
//   {
//      project.printInfo(errConstantExpectedLink, ex.reference);
//      print(ELC_UNSUCCESSFUL);
//      exitCode = -2;
//
//      project.cleanUp();
//   }
   catch(_ELENA_::_Exception&) {
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

//      project.cleanUp();
   }
   return exitCode;
}
