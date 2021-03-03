//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 command-line compiler
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#define __MSVCRT_VERSION__ 0x0800

#include "elena.h"
// --------------------------------------------------------------------------
#include "elc.h"
#include "constants.h"
#include "errors.h"
#include "compilerlogic.h"
#include "linker.h"
#include "image.h"
#include "x86jitcompiler.h"
#include "amd64jitcompiler.h"
//#include "derivation.h"

//#include <stdarg.h>
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
   bool             _consoleMode;

public:
   _ELENA_::ref_t tls_directory;

   virtual void beforeLoad(_ELENA_::_JITCompiler* compiler, _ELENA_::ExecutableImage& image)
   {
      _ELENA_::Project* project = image.getProject();
      _ELENA_::_JITLoader* loader = dynamic_cast<_ELENA_::_JITLoader*>(&image);

      // compile TLS section if it is a multi-threading app
      if (project->IntSetting(_ELENA_::opThreadMax) > 1) {
         _linker->prepareTLS(image, compiler->allocateTLSVariable(loader), tls_directory);
      }
      else compiler->allocateTLSVariable(loader);

      // load GC thread table, should be allocated before static roots
      // thread table contains TLS reference
      compiler->allocateThreadTable(loader, project->IntSetting(_ELENA_::opThreadMax));

      if (_vmMode) {
         _ELENA_::MemoryDump tape;
         createTape(tape, project, _consoleMode);

         compiler->allocateVMTape(loader, tape.get(0), tape.Length());
      }
   }

   virtual void afterLoad(_ELENA_::ExecutableImage&)
   {
   }

   ImageHelper(_ELENA_::Linker* linker, bool consoleMode, bool vmMode = false)
   {
      this->_linker = linker;
      this->tls_directory = 0;
      this->_vmMode = vmMode;
      this->_consoleMode = consoleMode;
   }
};

// --- Project ---

_ELC_::Project :: Project()
   : _sources(true), _targets(true)
{
   getAppPath(appPath);
   _settings.add(_ELENA_::opAppPath, _ELENA_::IdentifierString::clonePath(appPath.c_str()));
   _settings.add(_ELENA_::opNamespace, ((_ELENA_::ident_t)"unnamed").clone());

   _tabSize = 4;
   _encoding = _ELENA_::feUTF8;
}

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   if (_ELENA_::emptystr(path))
      // !! HOTFIX
      path = "autogeneated";

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

void _ELC_::Project :: raiseError(_ELENA_::ident_t msg)
{
   _ELENA_::WideString wMsg(msg);

   print(wMsg);

   throw _ELENA_::_Exception();
}

void _ELC_::Project::printInfo(const char* msg, _ELENA_::ReferenceInfo referenceInfo)
{
   if (referenceInfo.isRelative()) {
      _ELENA_::IdentifierString fullName(referenceInfo.module->Name(), referenceInfo.referenceName);

      printInfo(msg, fullName.c_str());
   }
   else printInfo(msg, referenceInfo.referenceName);
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

void _ELC_::Project::raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path, int row, int column, _ELENA_::ident_t terminal)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);
   _ELENA_::WideString wTerminal(terminal);

   print(wMsg, (const wchar_t*)wPath, row, column, (const wchar_t*)wTerminal);
}

void _ELC_::Project::raiseWarning(int level, _ELENA_::ident_t msg, _ELENA_::ident_t path)
{
   if (!_ELENA_::test(_warningMasks, level))
      return;

   if (!indicateWarning())
      return;

   _ELENA_::WideString wMsg(msg);
   _ELENA_::WideString wPath(path);

   print(wMsg, (const wchar_t*)wPath);
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
      _wremove(path);

      // remove debug module
      path.copy(rootPath.c_str());
      _loader.nameToPath(ns, path, "dnl");
      _wremove(path);
   }
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler()
{
   return new _ELENA_::x86JITCompiler(BoolSetting(_ELENA_::opDebugMode));
}

_ELENA_::_JITCompiler* _ELC_::Project :: createJITCompiler64()
{
   return new _ELENA_::I64JITCompiler(BoolSetting(_ELENA_::opDebugMode), false);
}

// --- Main function ---

const char* showPlatform(int platform)
{
   if (platform == _ELENA_::ptWin32Console) {
      return ELC_WIN32CONSOLE;
   }
   else if (platform == _ELENA_::ptWin64Console) {
      return ELC_WIN64CONSOLE;
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
   //else if (platform == _ELENA_::ptVMWin32GUI) {
   //   return ELC_WIN32VMGUI;
   //}
   //else if (platform == _ELENA_::ptWin32GUIX) {
   //   return ELC_WIN32GUIX;
   //}
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
      _ELENA_::Path configPath(project.appPath.c_str(), DEFAULT_CONFIG);
      project.loadConfig(configPath.c_str(), true, false);

      // Initializing..
      bool withoutProject = true;
      _ELENA_::IdentifierString defaultName;

      for (int i = 1 ; i < argc ; i++) {
         if (argv[i][0]=='-') {
            loadDefaultProjectIfRequired(project, defaultName, withoutProject);
            project.setOption(argv[i] + 1, withoutProject);
         }
         else if (_ELENA_::Path::checkExtension(argv[i], "prj")) {
            withoutProject = false;
            project.loadProject(argv[i]);
         }
         else {
            _ELENA_::FileName fileName(argv[i]);
            if (defaultName.Length() == 0) {
               defaultName.copyWideStr(fileName.c_str());

               project.addSource(argv[i], defaultName.c_str());
            }
            else project.addSource(argv[i], fileName.c_str());
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

      _ELENA_::Path syntaxPath(project.StrSetting(_ELENA_::opAppPath), SYNTAX_FILE);
      _ELENA_::FileReader syntaxFile(syntaxPath.c_str(), _ELENA_::feRaw, false);
      if (!syntaxFile.isOpened())
         project.raiseErrorIf(true, errInvalidFile, SYNTAX_FILE);

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
      if (platform == _ELENA_::ptWin32Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker, true);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t)-1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin64Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker(true);
         ImageHelper helper(&linker, true);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler64(), helper);
         linker.run(project, image, (_ELENA_::ref_t)-1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32ConsoleX) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker, true);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);

         linker.run(project, image, helper.tls_directory);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptVMWin32Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker, true, true);
         _ELENA_::ExecutableImage image(false, &project, project.createJITCompiler(), helper);

         linker.run(project, image, (_ELENA_::ref_t)-1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptVMWin64Console) {
         print(ELC_LINKING);

         _ELENA_::Linker linker(true);
         ImageHelper helper(&linker, true, true);
         _ELENA_::ExecutableImage image(false, &project, project.createJITCompiler64(), helper);

         linker.run(project, image, (_ELENA_::ref_t)-1);

         print(ELC_SUCCESSFUL_LINKING);
      }
      else if (platform == _ELENA_::ptWin32GUI) {
         print(ELC_LINKING);

         _ELENA_::Linker linker;
         ImageHelper helper(&linker, false);
         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);
         linker.run(project, image, (_ELENA_::ref_t)-1);

         print(ELC_SUCCESSFUL_LINKING);
      }
//      else if (platform == _ELENA_::ptWin32GUIX) {
//         print(ELC_LINKING);
//
//         _ELENA_::Linker linker;
//         ImageHelper helper(&linker, false);
//         _ELENA_::ExecutableImage image(true, &project, project.createJITCompiler(), helper);
//         linker.run(project, image, helper.tls_directory);
//
//         print(ELC_SUCCESSFUL_LINKING);
//      }
//      //else if (platform == _ELENA_::ptVMWin32GUI) {
//      //   print(ELC_LINKING);
//
//      //   _ELENA_::VirtualMachineClientImage image(
//      //      &project, project.createJITCompiler(), true);
//
//      //   _ELENA_::Linker linker;
//      //   linker.run(project, image, (_ELENA_::ref_t) - 1);
//
//      //   print(ELC_SUCCESSFUL_LINKING);
//      //}
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

