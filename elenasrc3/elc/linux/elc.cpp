//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cliconst.h"
#include "compiling.h"
#include "project.h"
#include "elfimage.h"

#if defined(__x86_64__)

#include "elflinker32.h"
#include "elflinker64.h"
#include "x86compiler.h"
#include "x86_64compiler.h"

#elif defined(__i386__)

#include "elflinker32.h"
#include "x86compiler.h"

#elif defined(__PPC64__)

#include "elfppclinker64.h"
#include "ppc64compiler.h"
#include "elfppcimage.h"

#elif defined(__aarch64__)

#include "elfarmlinker64.h"
#include "arm64compiler.h"
#include "elfarmimage.h"

#endif

#include "constants.h"
#include "messages.h"

#include <stdarg.h>

using namespace elena_lang;

#if defined(__x86_64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86_64;

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;

typedef ElfAmd64Linker           LinuxLinker;
typedef ElfAmd64ImageFormatter   LinuxImageFormatter;

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

constexpr int MINIMAL_ARG_LIST            = 1;

constexpr auto DEFAULT_STACKALIGNMENT     = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;


typedef ElfI386Linker            LinuxLinker;
typedef ElfI386ImageFormatter    LinuxImageFormatter;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;


typedef ElfPPC64leLinker         LinuxLinker;
typedef ElfPPC64leImageFormatter LinuxImageFormatter;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;

typedef ElfARM64Linker         LinuxLinker;
typedef ElfARM64ImageFormatter LinuxImageFormatter;

#endif

constexpr int DEFAULT_MGSIZE = 688128;
constexpr int DEFAULT_YGSIZE = 204800;

void print(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

class Presenter : public PresenterBase
{
public:
   ustr_t getMessage(int code)
   {
      for(size_t i = 0; i < MessageLength; i++) {
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
      ::print(msg.str(), arg.str());
   }
   void print(ustr_t msg, ustr_t arg1, ustr_t arg2) override
   {
      ::print(msg.str(), arg1.str(), arg2.str());
   }
   void print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) override
   {
      ::print(msg.str(), arg1.str(), arg2.str(), arg3.str());
   }
   void print(ustr_t msg, int arg1, int arg2, int arg3) override
   {
      ::print(msg.str(), arg1, arg2, arg3);
   }
   void printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) override
   {
      ::print(msg.str(), arg1.str(), arg2, arg3, arg4.str());
   }
   void print(ustr_t msg, int arg1, int arg2) override
   {
      ::print(msg.str(), arg1, arg2);
   }
   void print(ustr_t msg, int arg1) override
   {
      ::print(msg.str(), arg1);
   }
   void printPath(ustr_t msg, path_t arg)
   {
      ::print(msg.str(), arg.str());
   }
   void print(ustr_t msg)
   {
      ::print(msg.str());
   }
   void print(ustr_t msg, ustr_t path, int col, int row, ustr_t s) override
   {
      ::print(msg.str(), path.str(), row, col, s.str());
   }

private:
   Presenter() = default;

public:
   Presenter(Presenter const&) = delete;
   void operator=(Presenter const&) = delete;

   ~Presenter() = default;
};

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
#if defined(__i386__) || defined(__x86_64__)
      case PlatformType::Linux_x86:
         return new X86JITCompiler();
#endif
#if defined(__x86_64__)
      case PlatformType::Linux_x86_64:
         return new X86_64JITCompiler();
#endif
#if defined(__PPC64__)
      case PlatformType::Linux_PPC64le:
         return new PPC64leJITCompiler();
#endif
#if defined(__aarch64__)
      case PlatformType::Linux_ARM64:
         return new ARM64JITCompiler();
#endif
      default:
         return nullptr;
   }
}

int main(int argc, char* argv[])
{
   try
   {
      bool cleanMode = false;

      PathString dataPath(DATA_PATH);

      JITSettings      defaultCoreSettings = { DEFAULT_MGSIZE, DEFAULT_YGSIZE };
      ErrorProcessor   errorProcessor(&Presenter::getInstance());
      Project          project(*dataPath, CURRENT_PLATFORM, &Presenter::getInstance());
      LinuxLinker      linker(&errorProcessor, &LinuxImageFormatter::getInstance(&project));
      CompilingProcess process(dataPath, &Presenter::getInstance(), &errorProcessor,
         VA_ALIGNMENT, defaultCoreSettings, createJITCompiler);

      process.greeting();

      // Initializing...
      PathString configPath(*dataPath, DEFAULT_CONFIG);
      project.loadConfig(*configPath, false/*, true, false*/);

       // Reading command-line arguments...
      if (argc < 2) {
         Presenter::getInstance().print(ELC_HELP_INFO);
         return -3;
      }

      for (int i = 1; i < argc; i++) {
         if (argv[i][0] == '-') {
            switch (argv[i][1]) {
               case 'm':
                  project.addBoolSetting(ProjectOption::MappingOutputMode, true);
                  break;
               case 'r':
                  cleanMode = true;
                  break;
               default:
                  break;
            }
         }
         else if (PathUtil::checkExtension(argv[i], "project")) {
            PathString path(argv[i]);

            if (!project.loadProject(*path)) {
               errorProcessor.raisePathError(errProjectAlreadyLoaded, *path);
            }
         }
         else {
            FileNameString fileName(argv[i]);

            project.addSource(*fileName, argv[i]);
         }
      }

      if (cleanMode) {
         return process.clean(project);
      }
      else {
         // Building...
         return process.build(project, linker,
            DEFAULT_STACKALIGNMENT,
            DEFAULT_RAW_STACKALIGNMENT,
            MINIMAL_ARG_LIST);
      }
   }
   catch (CLIException e)
   {
      return -2;
   }


}