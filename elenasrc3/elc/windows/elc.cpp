//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 / win64 command-line compiler
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <cstdarg>
#include <windows.h>

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiling.h"
#include "project.h"
#ifdef _M_IX86
#include "ntlinker32.h"
#elif _M_X64
#include "ntlinker64.h"
#endif
#include "ntimage.h"
#include "cliconst.h"
#include "langcommon.h"
#include "x86compiler.h"
#include "x86_64compiler.h"

#include "messages.h"
#include "constants.h"

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto DEFAULT_STACKALIGNMENT     = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86;

constexpr int DEFAULT_MGSIZE              = 344064;
constexpr int DEFAULT_YGSIZE              = 86016;

constexpr int MINIMAL_ARG_LIST            = 1;

typedef Win32NtLinker             WinLinker;
typedef Win32NtImageFormatter     WinImageFormatter;

#elif _M_X64

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86_64;

constexpr int DEFAULT_MGSIZE              = 688128;
constexpr int DEFAULT_YGSIZE              = 204800;

constexpr int MINIMAL_ARG_LIST            = 2;

typedef Win64NtLinker             WinLinker;
typedef Win64NtImageFormatter     WinImageFormatter;

#endif

void print(const wchar_t* wstr, ...)
{
   va_list argptr;
   va_start(argptr, wstr);

   vwprintf(wstr, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

class Presenter : public PresenterBase
{
public:
   ustr_t getMessage(int code)
   {
      for (size_t i = 0; i < MessageLength; i++) {
         if (Messages[i].value1 == code)
            return Messages[i].value2;
      }

      return errMsgUnrecognizedError;
   }

   static Presenter& getInstance()
   {
      static Presenter instance;

      return instance;
   }

   void print(ustr_t msg, ustr_t arg) override
   {
      WideMessage wstr(msg);
      WideMessage warg(arg);

      ::print(wstr.str(), warg.str());
   }
   void print(ustr_t msg, ustr_t arg1, ustr_t arg2) override
   {
      WideMessage wstr(msg);
      WideMessage warg1(arg1);
      WideMessage warg2(arg2);

      ::print(wstr.str(), warg1.str(), warg2.str());
   }
   void print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) override
   {
      WideMessage wstr(msg);
      WideMessage warg1(arg1);
      WideMessage warg2(arg2);
      WideMessage warg3(arg3);

      ::print(wstr.str(), warg1.str(), warg2.str(), warg3.str());
   }
   void print(ustr_t msg, int arg1, int arg2, int arg3) override
   {
      WideMessage wstr(msg);

      ::print(wstr.str(), arg1, arg2, arg3);
   }
   void printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) override
   {
      WideMessage wstr(msg);
      WideMessage warg4(arg4);

      ::print(wstr.str(), arg1.str(), arg2, arg3, warg4.str());
   }
   void print(ustr_t msg, int arg1, int arg2) override
   {
      WideMessage wstr(msg);

      ::print(wstr.str(), arg1, arg2);
   }
   void printPath(ustr_t msg, path_t arg) override
   {
      WideMessage wstr(msg);

      ::print(wstr.str(), arg.str());
   }
   void print(ustr_t msg) override
   {
      WideMessage wstr(msg);

      ::print(wstr.str());
   }
   void print(ustr_t msg, ustr_t path, int col, int row, ustr_t s) override
   {
      WideMessage wstr(msg);
      WideMessage wpath(path);
      WideMessage ws(s);

      ::print(wstr.str(), wpath.str(), row, col, ws.str());
   }

private:
   Presenter() = default;

public:
   Presenter(Presenter const&) = delete;
   void operator=(Presenter const&) = delete;

   ~Presenter() override = default;
};

// --- getAppPath ---

void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(nullptr, path, MAX_PATH);

   appPath.copySubPath(path);
   appPath.lower();
}

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
      case PlatformType::Win_x86:
         return new X86JITCompiler();
      case PlatformType::Win_x86_64:
         return new X86_64JITCompiler();
      default:
         return nullptr;
   }
}

int main()
{
   try
   {
      PathString appPath;
      getAppPath(appPath);

      JITSettings defaultCoreSettings = { DEFAULT_MGSIZE, DEFAULT_YGSIZE };
      ErrorProcessor   errorProcessor(&Presenter::getInstance());
      Project          project(*appPath, CURRENT_PLATFORM, &Presenter::getInstance());
      WinLinker        linker(&errorProcessor, &WinImageFormatter::getInstance(&project));
      CompilingProcess process(appPath, &Presenter::getInstance(), &errorProcessor,
         VA_ALIGNMENT, defaultCoreSettings, createJITCompiler);

      process.greeting();

      // Initializing...
      PathString configPath(*appPath, DEFAULT_CONFIG);
      project.loadConfig(*configPath, false/*, true, false*/);

      // Reading command-line arguments...
      int argc;
      wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

      if (argc < 2) {
         Presenter::getInstance().print(ELC_HELP_INFO);
         return -3;
      }

      for (int i = 1; i < argc; i++) {
         if (PathUtil::checkExtension(argv[i], "prj")) {
            PathString path(argv[i]);

            if (!project.loadProject(*path)) {
               return -1;
            }
         }
         else {
            FileNameString fileName(argv[i]);
            IdentifierString ns(*fileName);

            project.addSource(*ns, argv[i]);
         }
      }

      // Building...
      return process.build(project, linker, 
         DEFAULT_STACKALIGNMENT, 
         DEFAULT_RAW_STACKALIGNMENT,
         MINIMAL_ARG_LIST);
   }
   catch (CLIException)
   {
      return -2;
   }
}
