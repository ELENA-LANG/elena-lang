//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the Linux command-line compiler
//
//                                             (C)2021-2025, by Aleksey Rakov
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

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

typedef ElfAmd64Linker           LinuxLinker;
typedef ElfAmd64ImageFormatter   LinuxImageFormatter;

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

constexpr int MINIMAL_ARG_LIST            = 1;

constexpr auto DEFAULT_STACKALIGNMENT     = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 16;

typedef ElfI386Linker            LinuxLinker;
typedef ElfI386ImageFormatter    LinuxImageFormatter;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

typedef ElfPPC64leLinker         LinuxLinker;
typedef ElfPPC64leImageFormatter LinuxImageFormatter;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

constexpr int MINIMAL_ARG_LIST            = 2;

constexpr auto DEFAULT_STACKALIGNMENT     = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

typedef ElfARM64Linker         LinuxLinker;
typedef ElfARM64ImageFormatter LinuxImageFormatter;

#endif

constexpr int DEFAULT_MGSIZE = 688128;
constexpr int DEFAULT_YGSIZE = 204800;
constexpr int DEFAULT_STACKRESERV = 0x100000;

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

JITCompilerBase* createJITCompiler(PlatformType platform)
{
   switch (platform) {
#if defined(__i386__) || defined(__x86_64__)
      case PlatformType::Linux_x86:
         return new X86JITCompiler();
#endif
#if defined(__x86_64__)
      case PlatformType::Linux_x86_64:
      case PlatformType::FreeBSD_x86_64:
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

void handleOption(char* arg, IdentifierString& profile, Project& project, CompilingProcess& process,
   ErrorProcessor& errorProcessor, path_t dataPath, bool& cleanMode)
{
   switch (arg[1]) {
      case 't':
      {
         IdentifierString configName(arg + 2);

         if(!project.loadConfigByName(dataPath, *configName, true))
            errorProcessor.info(wrnInvalidConfig, *configName);;
         break;
      }
      case 'p':
         project.setBasePath(arg + 2);
         break;
      default:
      {
         IdentifierString argStr(arg);

         CommandHelper::handleOption(*argStr, profile, project, process, errorProcessor, cleanMode);
         break;
      }
   }
}

PlatformType definePlatform(PlatformType defaultPlatform) 
{
   return defaultPlatform;
}

LinkerBase* createLinker(PlatformType defaultPlatform, Project* project, ErrorProcessorBase* errorProcessor)
{
   return new LinuxLinker(errorProcessor, project);
}

int compileProject(int argc, char** argv, path_t dataPath, ErrorProcessor& errorProcessor,
   path_t basePath = nullptr, ustr_t defaultProfile = nullptr)
{
   PlatformType platform = definePlatform(CURRENT_PLATFORM);
   bool cleanMode = false;

   JITSettings      defaultCoreSettings = { DEFAULT_MGSIZE, DEFAULT_YGSIZE, DEFAULT_STACKRESERV, 1, true, true };
   CompilingProcess process(dataPath, nullptr, "<moduleProlog>", "<prolog>", "<epilog>",
      &Presenter::getInstance(), &errorProcessor,
      VA_ALIGNMENT, defaultCoreSettings, createJITCompiler);

   Project          project(dataPath, platform, &Presenter::getInstance());
   LinkerBase*      linker = createLinker(platform, &project, &errorProcessor);

   // Initializing...
   path_t defaultConfigPath = PathHelper::retrieveFilePath(LOCAL_DEFAULT_CONFIG);
   if (defaultConfigPath.compare(LOCAL_DEFAULT_CONFIG)) {
      // if the local config file was not found
      defaultConfigPath = DEFAULT_CONFIG;
   }

   PathString configPath(dataPath, PathHelper::retrieveFilePath(defaultConfigPath));
   project.loadConfig(*configPath, nullptr, false);

   IdentifierString profile(defaultProfile);
   for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         handleOption(argv[i], profile, project, process,
            errorProcessor, dataPath, cleanMode);
      }
      else if (PathUtil::checkExtension(argv[i], "prj")) {
         PathString path(argv[i]);
         if (!project.loadProject(*path, *profile)) {
            return ERROR_RET_CODE;
         }

         if (profile.empty() && project.availableProfileList.count() != 0) {
            IdentifierString profileList;
            for (auto it = project.availableProfileList.start(); !it.eof(); ++it) {
               if (profileList.length() != 0)
                  profileList.append(", ");

               profileList.append(*it);
            }

            Presenter::getInstance().printLine(ELC_PROFILE_WARNING, *profileList);
         }
      }
      else if (PathUtil::checkExtension(argv[i], "prjcol")) {
         Presenter::getInstance().printLine(ELC_PRJ_COLLECTION_WARNING);
         return -2;
      }
      else {
         FileNameString fileName(argv[i]);

         project.addSource(*fileName, argv[i], nullptr, nullptr, true);
      }
   }

   if (!basePath.empty())
      project.setBasePath(basePath);

   if (cleanMode) {
      return process.clean(project);
   }
   else {
      // Building...
      return process.build(project, *linker,
         DEFAULT_STACKALIGNMENT,
         DEFAULT_RAW_STACKALIGNMENT,
         DEFAULT_EHTABLE_ENTRY_SIZE,
         MINIMAL_ARG_LIST,
         *profile);
   }
}

int compileProjectCollection(int argc, char** argv, path_t path, path_t dataPath,
   ErrorProcessor& errorProcessor)
{
   Presenter* presenter = &Presenter::getInstance();

   int retVal = 0;
   ProjectCollection collection;

   if (!collection.load(path)) {
      presenter->printPath(presenter->getMessage(wrnInvalidConfig), path);

      return ERROR_RET_CODE;
   }

   for (auto it = collection.projectSpecs.start(); !it.eof(); ++it) {
      auto spec = *it;

      size_t destLen = FILENAME_MAX;
      char projectPath[FILENAME_MAX];
      StrConvertor::copy(projectPath, spec->path.str(), spec->path.length(), destLen);
      projectPath[destLen] = 0;

      argv[argc - 1] = projectPath;
      presenter->printPath(ELC_COMPILING_PROJECT, projectPath);

      int result = compileProject(argc, argv, dataPath, errorProcessor, spec->basePath, spec->profile);
      if (result == ERROR_RET_CODE) {
         return ERROR_RET_CODE;
      }
      else if (result == WARNING_RET_CODE) {
         retVal = WARNING_RET_CODE;
      }
   }

   return retVal;
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
         return compileProjectCollection(argc, argv, argv[argc - 1],
            *dataPath, errorProcessor);
      }
      else return compileProject(argc, argv, *dataPath, errorProcessor);
   }
   catch (CLIException)
   {
      return ERROR_RET_CODE;
   }
}
