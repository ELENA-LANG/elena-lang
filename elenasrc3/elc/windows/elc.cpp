//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 / win64 command-line compiler
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

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
#include "windows/presenter.h"

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto DEFAULT_STACKALIGNMENT     = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 16;

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86;

constexpr int DEFAULT_MGSIZE              = 344064;
constexpr int DEFAULT_YGSIZE              = 86016;

constexpr int MINIMAL_ARG_LIST            = 1;

typedef Win32NtLinker             WinLinker;
typedef Win32NtImageFormatter     WinImageFormatter;

#elif _M_X64

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86_64;

constexpr int DEFAULT_MGSIZE              = 688128;
constexpr int DEFAULT_YGSIZE              = 204800;

constexpr int MINIMAL_ARG_LIST            = 2;

typedef Win64NtLinker             WinLinker;
typedef Win64NtImageFormatter     WinImageFormatter;

#endif

// --- Presenter ---

class Presenter : public WinConsolePresenter
{
private:
   Presenter() = default;

public:
   ustr_t getMessage(int code) override
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

   Presenter(Presenter const&) = delete;
   void operator=(Presenter const&) = delete;

   ~Presenter() override = default;
};

// --- getAppPath ---

void getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(nullptr, path, MAX_PATH);

   appPath.copySubPath(path, false);
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
      bool cleanMode = false;

      PathString appPath;
      getAppPath(appPath);

      JITSettings defaultCoreSettings = { DEFAULT_MGSIZE, DEFAULT_YGSIZE, 1, true };
      ErrorProcessor   errorProcessor(&Presenter::getInstance());
      Project          project(*appPath, CURRENT_PLATFORM, &Presenter::getInstance());
      WinLinker        linker(&errorProcessor, &WinImageFormatter::getInstance(&project));
      CompilingProcess process(appPath, L"<prolog>", L"<epilog>", 
         &Presenter::getInstance(), &errorProcessor,
         VA_ALIGNMENT, defaultCoreSettings, createJITCompiler);

      process.greeting();

      // Initializing...
      PathString configPath(*appPath, DEFAULT_CONFIG);
      project.loadConfig(*configPath, false/*, true, false*/);

      // Reading command-line arguments...
      int argc;
      wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

      if (argc < 2) {
         Presenter::getInstance().printLine(ELC_HELP_INFO);
         return -2;
      }

      for (int i = 1; i < argc; i++) {
         if (argv[i][0] == '-') {
            switch (argv[i][1]) {
               case 'm':
                  project.addBoolSetting(ProjectOption::MappingOutputMode, true);
                  break;
               case 'o':
                  if (argv[i][2] == '0') {
                     project.addIntSetting(ProjectOption::OptimizationMode, optNone);
                  }
                  else if (argv[i][2] == '1') {
                     project.addIntSetting(ProjectOption::OptimizationMode, optLow);
                  }
                  else if (argv[i][2] == '2') {
                     project.addIntSetting(ProjectOption::OptimizationMode, optMiddle);
                  }
                  break;
               case 'r':
                  cleanMode = true;
                  break;
               case 't':
               {
                  IdentifierString configName(argv[i] + 2);

                  project.loadConfigByName(*appPath, *configName, true);
                  break;
               }
               case 'p':
                  project.setBasePath(argv[i] + 2);
                  break;
               case 'w':
                  if (argv[i][2] == '0') {
                     errorProcessor.setWarningLevel(WarningLevel::Level0);
                  }
                  else if (argv[i][2] == '1') {
                     errorProcessor.setWarningLevel(WarningLevel::Level1);
                  }
                  else if (argv[i][2] == '2') {
                     errorProcessor.setWarningLevel(WarningLevel::Level2);
                  }
                  else if (argv[i][2] == '3') {
                     errorProcessor.setWarningLevel(WarningLevel::Level3);
                  }
                  break;
               case 'x':
                  if (argv[i][2] == 'p') {
                     project.addBoolSetting(ProjectOption::GenerateParamNameInfo, argv[i][3] != '-');
                  }
                  else if (argv[i][2] == 'b') {
                     project.addBoolSetting(ProjectOption::ConditionalBoxing, argv[i][3] != '-');
                  }
                  break;
               default:
                  break;
            }
         }
         else if (PathUtil::checkExtension(argv[i], "prj")) {
            PathString path(argv[i]);

            if (!project.loadProject(*path)) {
               return ERROR_RET_CODE;
            }
         }
         else {
            FileNameString fileName(argv[i]);
            IdentifierString ns(*fileName);
            project.addSource(*ns, argv[i], nullptr);
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
            DEFAULT_EHTABLE_ENTRY_SIZE,
            MINIMAL_ARG_LIST);
      }
   }
   catch (CLIException)
   {
      return ERROR_RET_CODE;
   }
}
