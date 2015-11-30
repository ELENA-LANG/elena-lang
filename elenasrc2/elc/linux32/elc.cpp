//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

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

// --- ImageHelper ---

class ImageHelper : public _ELENA_::ExecutableImage::_Helper
{
   virtual void beforeLoad(_ELENA_::_JITCompiler* compiler, _ELENA_::ExecutableImage& image)
   {
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

public:
   ImageHelper()
   {
   }
};

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

   // !! temporally
   _settings.add(_ELENA_::opDebugSubjectInfo, -1);
}

void _ELC_::Project :: raiseError(const char* msg, const char* path, int row, int column, const char* s)
{
   print(msg, path, row, column, s);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(const char* msg, const char* value)
{
   print(msg, value);

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: printInfo(const char* msg, const char* s)
{
   print(msg, s);
}

void _ELC_::Project :: raiseErrorIf(bool throwExecption, const char* msg, const char* path)
{
   print(msg, path);

   if (throwExecption)
      throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseWarning(const char* msg, const char* path, int row, int column, const char* s)
{
   if (!indicateWarning())
      return;

   print(msg, path, row, column, s);
}

void _ELC_::Project :: raiseWarning(const char* msg, const char* path)
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
   case _ELENA_::opExternals:
      return config.getCategoryIt(EXTERNALS_CATEGORY);
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
   case _ELENA_::opThreadMax:
      return config.getSetting(SYSTEM_CATEGORY, ELC_SYSTEM_THREADMAX);
   case _ELENA_::opL0:
      return config.getSetting(COMPILER_CATEGORY, ELC_L0);
   case _ELENA_::opL1:
      return config.getSetting(COMPILER_CATEGORY, ELC_L1);
   case _ELENA_::opTemplate:
      return config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   default:
      return NULL;
   }
}

void _ELC_::Project :: addSource(const char* path)
{
   _ELENA_::Path fullPath(StrSetting(_ELENA_::opProjectPath));
   fullPath.combine(path);
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

void _ELC_::Project :: loadConfig(const char* path, bool root, bool requiered)
{
   ElcConfigFile config;
   _ELENA_::Path configPath;

   configPath.copySubPath(path);

   if (!config.load(path, getDefaultEncoding())) {
      raiseErrorIf(requiered, ELC_ERR_INVALID_PATH, path);
      return;
   }

   // load template list
   if (root)
      loadCategory(config, _ELENA_::opTemplates, configPath);

   // load template
   const char* projectTemplate = config.getSetting(PROJECT_CATEGORY, ELC_PROJECT_TEMPLATE);
   if (!_ELENA_::emptystr(projectTemplate)) {
      const char* templateFile = _settings.get(_ELENA_::opTemplates, projectTemplate, (const char*)NULL);
      if (_ELENA_::emptystr(templateFile)) {
        _ELENA_::String<char, 255> str(projectTemplate);

         raiseErrorIf(requiered, ELC_ERR_INVALID_TEMPLATE, (const char*)str);
      }
      else loadConfig(templateFile, false, false);
   }

   loadConfig(config, configPath);
}

void _ELC_::Project :: setOption(const char* value)
{
   switch ((char)value[0]) {
      case ELC_PRM_LIB_PATH:
         _settings.add(_ELENA_::opLibPath, _ELENA_::StringHelper::clone(value + 1));
         break;
      case ELC_PRM_OUTPUT_PATH:
         _settings.add(_ELENA_::opOutputPath, _ELENA_::StringHelper::clone(value + 1));
         break;
      case ELC_PRM_EXTRA:
         if (_ELENA_::StringHelper::compare(value, ELC_PRM_TABSIZE, 4)) {
            _tabSize = _ELENA_::StringHelper::strToInt(value + 4);
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_PROJECTPATH, _ELENA_::getlength(ELC_PRM_PROJECTPATH))) {
            _settings.add(_ELENA_::opProjectPath, _ELENA_::StringHelper::clone(value + _ELENA_::getlength(ELC_PRM_PROJECTPATH)));
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_OPTOFF)) {
            _settings.add(_ELENA_::opL0, 0);
            _settings.add(_ELENA_::opL1, 0);
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_PRM_OPT1OFF)) {
            _settings.add(_ELENA_::opL1, 0);
         }
         else raiseError(ELC_ERR_INVALID_OPTION, value);
         break;
      case ELC_PRM_WARNING:
         if (_ELENA_::StringHelper::compare(value, ELC_W_UNRESOLVED)) {
            _settings.add(_ELENA_::opWarnOnUnresolved, -1);
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_W_WEAKUNRESOLVED)) {
            _settings.add(_ELENA_::opWarnOnWeakUnresolved, -1);
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_W_LEVEL1)) {
            _warningMasks |= _ELENA_::WARNING_MASK_1;
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_W_LEVEL2)) {
            _warningMasks |= _ELENA_::WARNING_MASK_2;
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_W_LEVEL3)) {
            _warningMasks |= _ELENA_::WARNING_MASK_3;
         }
         else if (_ELENA_::StringHelper::compare(value, ELC_W_OFF)) {
            _warningMasks = 0;
         }
         break;
      case ELC_PRM_TARGET:
         _settings.add(_ELENA_::opTarget, _ELENA_::StringHelper::clone(value + 1));
         break;
      case ELC_PRM_START:
         _settings.add(_ELENA_::opEntry, _ELENA_::StringHelper::clone(value + 1));
         break;
      case ELC_PRM_DEBUGINFO:
         _settings.add(_ELENA_::opDebugMode, -1);
         break;
      case ELC_PRM_CONFIG:
      {
         projectName.copy(value + 1);

         loadConfig(value + 1);

         _ELENA_::Path projectPath;
         projectPath.copySubPath(value + 1);
         _settings.add(_ELENA_::opProjectPath, projectPath.clone());

         break;
      }
      default:
         raiseError(ELC_ERR_INVALID_OPTION, value);
   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

void setCompilerOptions(_ELC_::Project& project, _ELENA_::Compiler& compiler)
{
   if (project.IntSetting(_ELENA_::opL0, -1) != 0) {
      _ELENA_::Path rulesPath(RULES_FILE);
      _ELENA_::FileReader rulesFile(rulesPath, _ELENA_::feRaw, false);
      if (!rulesFile.isOpened()) {
         project.raiseWarning(errInvalidFile, rulesPath);
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
   if (platform == _ELENA_::ptLinux32Console) {
      return ELC_LINUX32CONSOLE;
   }
   else if (platform == _ELENA_::ptLibrary) {
      return ELC_LIBRARY;
   }
   else return ELC_UNKNOWN;
}

int main(int argc, char* argv[])
{
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
      project.loadConfig(_ELENA_::Path(/*project.appPath, */DEFAULT_CONFIG), true, false);

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

//      // Cleaning up
//      print("Cleaning up...");
//      project.cleanUp();

      // Compiling..
      print(ELC_COMPILING);

      _ELENA_::Path syntaxPath(SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath, _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseError(errInvalidFile, syntaxPath);

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
      if (platform == _ELENA_::ptLinux32Console) {
         print(ELC_LINKING);

         ImageHelper helper;
         _ELENA_::ExecutableImage image(&project, project.createJITCompiler(), helper);
         _ELENA_::I386Linker32 linker;
         linker.run(project, image/*, -1*/);

         print(ELC_SUCCESSFUL_LINKING);
      }
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
   catch(_ELENA_::InternalError& e) {
      print(ELC_INTERNAL_ERROR, e.message);
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

//      project.cleanUp();
   }
   return exitCode;
}
