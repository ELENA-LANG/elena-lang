//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 / win64 command-line compiler
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <windows.h>

#include "elena.h"
// --------------------------------------------------------------------------
#ifdef _M_IX86
#include "ntlinker32.h"
#elif _M_X64
#include "ntlinker64.h"
#endif
#include "ntimage.h"
#include "cli.h"
#include "langcommon.h"
#include "x86compiler.h"
#include "x86_64compiler.h"

#include "messages.h"
#include "constants.h"
#include "windows/presenter.h"

#include <time.h>

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86;

#elif _M_X64

constexpr auto CURRENT_PLATFORM           = PlatformType::Win_x86_64;

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

path_t getDefaultExtension(PlatformType platform)
{
   switch (platform)
   {
      case PlatformType::Win_x86:
      case PlatformType::Win_x86_64:
         return L"exe";
      default:
         return nullptr;
   }
}

int compileProject(int argc, path_c** argv, path_t appPath, ErrorProcessor& errorProcessor, 
   path_t basePath = nullptr, ustr_t defaultProfile = nullptr)
{
   PlatformType platform = CLIHelper::definePlatform(argc, argv, CURRENT_PLATFORM);
   JITCompilerSettings jitSettings = CLIHelper::getJITCompilerSettings(platform, &errorProcessor);

   ProcessSettings defaultCoreSettings = CLIHelper::getProcessSettings(platform);

   CompilingProcess process(appPath, getDefaultExtension(platform), L"<moduleProlog>", L"<prolog>", L"<epilog>",
      &Presenter::getInstance(), &errorProcessor,
      VA_ALIGNMENT, defaultCoreSettings, CLIHelper::createJITCompiler);

   PathString configPath(appPath, DEFAULT_CONFIG);

   return CLIHelper::compileProject(argc, argv,
      process, 
      platform, jitSettings, 
      Presenter::getInstance(), errorProcessor,
      appPath, basePath, *configPath,
      defaultProfile);
}

int main()
{
   try
   {
      PathString appPath;
      getAppPath(appPath);
      
      ErrorProcessor   errorProcessor(&Presenter::getInstance());

      CompilingProcess::greeting(&Presenter::getInstance());

      // Reading command-line arguments...
      int argc;
      wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

      int retVal = 0;
      if (argc < 2) {
         Presenter::getInstance().printLine(ELC_HELP_INFO);
         return -2;
      }
      else if (argv[argc - 1][0] != '-' && PathUtil::checkExtension(argv[argc - 1], "prjcol")) {
         retVal = CLIHelper::compileProjectCollection(argc, argv, argv[argc - 1],            
            *appPath, 
            CLIHelper::definePlatform(argc, argv, CURRENT_PLATFORM),
            errorProcessor, Presenter::getInstance(), compileProject);
      }
      else retVal = compileProject(argc, argv, *appPath, errorProcessor);

      return retVal;
   }
   catch (CLIException)
   {
      return ERROR_RET_CODE;
   }
}
