//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
#include "constants.h"
#include "errors.h"
#include "compilerlogic.h"
#include "linker.h"
#include "image.h"
#include "x86jitcompiler.h"

#include <stdarg.h>

// --- ImageHelper ---

class ImageHelper : public _ELENA_::ExecutableImage::_Helper
{
   //_ELENA_::Linker* _linker;
   bool             _consoleMode;

public:
   virtual void beforeLoad(_ELENA_::_JITCompiler* compiler, _ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();
      _ELENA_::_JITLoader* loader = dynamic_cast<_ELENA_::_JITLoader*>(&image);

      // compile TLS section if it is a multi-threading app
      /*if (project->IntSetting(_ELENA_::opThreadMax) > 1) {
         _linker->prepareTLS(image, compiler->allocateTLSVariable(loader), tls_directory);
      }
      else */compiler->allocateTLSVariable(loader);

      // load GC thread table, should be allocated before static roots
      // thread table contains TLS reference
      compiler->allocateThreadTable(loader, project->IntSetting(_ELENA_::opThreadMax));

      if (_vmMode) {
         _ELENA_::MemoryDump tape;
         createTape(tape, project, _consoleMode);

         compiler->allocateVMTape(loader, tape.get(0), tape.Length());
      }
   }

   virtual void afterLoad(_ELENA_::ExecutableImage& image)
   {
   }

public:
   ImageHelper(/*_ELENA_::Linker* linker, */bool consoleMode, bool vmMode = false)
   {
      //this->_linker = linker;
      //this->tls_directory = 0;
      this->_vmMode = vmMode;
      this->_consoleMode = consoleMode;
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
   : _sources(true), _targets(true)
{
   appPath.copy(DATA_PATH);

//   getAppPath(appPath);
   _settings.add(_ELENA_::opAppPath, _ELENA_::StrFactory::clone(appPath));
   _settings.add(_ELENA_::opNamespace, _ELENA_::StrFactory::clone("unnamed"));

   _tabSize = 4;
   _encoding = _ELENA_::feUTF8;
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t s)
{
   print(msg.c_str(), (const char*)path, row, column, s.c_str());

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t value)
{
   print(msg.c_str(), value.c_str());

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg)
{
   print(msg.c_str());

   throw _ELENA_::_Exception();
}

void _ELC_::Project :: printInfo(const char* msg, _ELENA_::ident_t s)
{
   print(msg, s.c_str());
}

void _ELC_::Project :: printInfo(const char* msg, _ELENA_::ReferenceInfo referenceInfo)
{
   if (referenceInfo.isRelative()) {
      _ELENA_::IdentifierString fullName(referenceInfo.module->Name(), referenceInfo.referenceName);

      printInfo(msg, fullName.c_str());
   }
   else printInfo(msg, referenceInfo.referenceName);
}

void _ELC_::Project :: raiseErrorIf(bool throwExecption, _ELENA_::ident_t msg, _ELENA_::ident_t identifier)
{
   print(msg.c_str(), identifier.c_str());

   if (throwExecption)
      throw _ELENA_::_Exception();
}

void _ELC_::Project :: raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t s)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   print(msg.c_str(), path.c_str(), row, column, s.c_str());
}

void _ELC_::Project :: raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   print(msg.c_str(), path.c_str());
}

void _ELC_::Project :: cleanUp()
{
   _ELENA_::Path rootPath(StrSetting(_ELENA_::opProjectPath), StrSetting(_ELENA_::opOutputPath));

   for (_ELENA_::SourceIterator it = _sources.start(); !it.Eof(); it++) {
      _ELENA_::ident_t ns = _sources.get(it.key(), ELC_NAMESPACE_KEY, DEFAULT_STR);

      _ELENA_::Path path;

      // remove module
      path.copy(rootPath.c_str());
      _loader.nameToPath(ns, path, "nl");
      remove(path);

      // remove debug module
      path.copy(rootPath.c_str());
      _loader.nameToPath(ns, path, "dnl");
      remove(path);
   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

//_ELENA_::_JITCompiler* _ELC_::Project::createJITCompiler64()
//{
//   return /*new _ELENA_::AMD64JITCompiler(BoolSetting(_ELENA_::opDebugMode))*/NULL;
//}

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

inline void loadDefaultProjectIfRequired(_ELC_::Project& project, _ELENA_::IdentifierString& defaultName, bool& withoutProject)
{
   if (withoutProject) {
      project.loadDefaultConfig(defaultName.c_str());
      withoutProject = false;
   }
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
      project.loadConfig(DEFAULT_CONFIG, true, false);

      // Initializing..
      bool withoutProject = true;
      _ELENA_::IdentifierString defaultName;

      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0]=='-') {
            loadDefaultProjectIfRequired(project, defaultName, withoutProject);
            project.setOption(argv[i] + 1, withoutProject);
         }
         else if (_ELENA_::Path::checkExtension(argv[i], "project")) {
            withoutProject = false;
            project.loadProject(argv[i]);
         }
         else {
            _ELENA_::FileName fileName(argv[i]);
            if (defaultName.Length() == 0) {
               defaultName.copy(fileName.c_str());

               project.addSource(argv[i], defaultName.c_str());
            }
            project.addSource(argv[i], fileName.c_str());
         }
      }

      loadDefaultProjectIfRequired(project, defaultName, withoutProject);

      project.initLoader();

      int platform = project.IntSetting(_ELENA_::opPlatform);

      // Greetings
      print(ELC_STARTING, (const char*)project.projectName, showPlatform(platform));

      // Cleaning up
      print("Cleaning up...");
      project.cleanUp();

      // Compiling..
      print(ELC_COMPILING);

      _ELENA_::Path syntaxPath(SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath.c_str(), _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseError(errInvalidFile, syntaxPath.c_str());

      // compile normal project
      bool result = false;
      _ELENA_::CompilerLogic elenaLogic;
      _ELENA_::Compiler compiler(&elenaLogic);
      _ELENA_::Parser parser(&syntaxFile);
      project.setCompilerOptions(compiler);

      result = project.compileSources(compiler, parser);

      if (result)
         print(ELC_SUCCESSFUL_COMPILATION);
      else {
         exitCode = -1;
         print(ELC_WARNING_COMPILATION);
      }

      // Linking..
      if (platform == _ELENA_::ptLinux32Console) {
         print(ELC_LINKING);

         _ELENA_::I386Linker linker;
         ImageHelper helper(true);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);
         linker.run(project, image/*, -1*/);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptLinux64Console) {
         print(ELC_LINKING);

         _ELENA_::AMD64Linker linker;
         ImageHelper helper(true);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);
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
      else if (project.IntSetting(_ELENA_::opPlatform) == _ELENA_::ptVMLinux32Console) {
         print(ELC_LINKING);

         _ELENA_::I386Linker linker;
         ImageHelper helper(/*&linker, */true, true);
         _ELENA_::ExecutableImage image(false, &project, project.createJITCompiler(), helper);

         linker.run(project, image/*, -1*/);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptLibrary) {
         // no linking for the library
      }
      else print(ELC_UNKNOWN_PLATFORM);
   }
   catch(_ELENA_::InternalError& e) {
      print(ELC_INTERNAL_ERROR, e.message);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITUnresolvedException& ex)
   {
      project.printInfo(errUnresovableLink, ex.referenceInfo);
      print(ELC_UNSUCCESSFUL);
      exitCode = -2;

      project.cleanUp();
   }
   catch(_ELENA_::JITConstantExpectedException& ex)
   {
      project.printInfo(errConstantExpectedLink, ex.referenceInfo);
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
