//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cli.h"
#include "elfimage.h"

//#define CROSS_COMPILE_MODE 1

#include "constants.h"
#include "messages.h"
#include "linux/presenter.h"
#include "linux/pathmanager.h"

#include <stdarg.h>

using namespace elena_lang;

#if defined(__x86_64__)

#if defined(__FreeBSD__)

constexpr auto CURRENT_PLATFORM = PlatformType::FreeBSD_x86_64;

#else

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86_64;

#endif

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

#endif

class Presenter : public LinuxConsolePresenter
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

private:
   Presenter() = default;

public:
   Presenter(Presenter const&) = delete;
   void operator=(Presenter const&) = delete;

   ~Presenter() = default;
};

ustr_t getDefaultExtension(PlatformType platform)
{
   switch (platform)
   {
      case PlatformType::Win_x86:
      case PlatformType::Win_x86_64:
         return "exe";
      default:
         return nullptr;
   }
}

int compileProject(int argc, char** argv, path_t dataPath, ErrorProcessor& errorProcessor,
   path_t basePath = nullptr, ustr_t defaultProfile = nullptr)
{
   PlatformType platform = CLIHelper::definePlatform(argc, argv, CURRENT_PLATFORM);

   ProcessSettings defaultCoreSettings = CLIHelper::getProcessSettings(platform);
   JITCompilerSettings jitSettings = CLIHelper::getJITCompilerSettings(platform, &errorProcessor);

   CompilingProcess process(dataPath, getDefaultExtension(platform), "<moduleProlog>", "<prolog>", "<epilog>",
      &Presenter::getInstance(), &errorProcessor,
      VA_ALIGNMENT, defaultCoreSettings, CLIHelper::createJITCompiler);

   path_t defaultConfigPath = PathHelper::retrieveFilePath(LOCAL_DEFAULT_CONFIG);
   if (defaultConfigPath.compare(LOCAL_DEFAULT_CONFIG)) {
      // if the local config file was not found
      defaultConfigPath = DEFAULT_CONFIG;
   }

   PathString configPath(dataPath, PathHelper::retrieveFilePath(defaultConfigPath));

   return CLIHelper::compileProject(argc, argv,
      process,
      platform, jitSettings,
      Presenter::getInstance(), errorProcessor,
      dataPath, basePath, *configPath,
      defaultProfile);
}

const char* dataFileList[] = { BC_RULES_FILE, BT_RULES_FILE, SYNTAX60_FILE };

int main(int argc, char* argv[])
{
   try
   {
      PathString dataPath(PathHelper::retrievePath(dataFileList, 3, DATA_PATH));

      ErrorProcessor   errorProcessor(&Presenter::getInstance());

      CompilingProcess::greeting(&Presenter::getInstance());

      // Reading command-line arguments...
      if (argc < 2) {
         Presenter::getInstance().printLine(ELC_HELP_INFO);
         return -2;
      }
      else if (argv[argc - 1][0] != '-' && PathUtil::checkExtension(argv[argc - 1], "prjcol")) {
         return CLIHelper::compileProjectCollection(argc, argv, argv[argc - 1],            
            *dataPath, 
            CLIHelper::definePlatform(argc, argv, CURRENT_PLATFORM),
            errorProcessor, Presenter::getInstance(), compileProject);
      }
      else return compileProject(argc, argv, *dataPath, errorProcessor);
   }
   catch (CLIException)
   {
      return ERROR_RET_CODE;
   }
}
