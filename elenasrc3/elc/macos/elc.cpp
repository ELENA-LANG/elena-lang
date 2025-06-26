//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the macOS command-line compiler
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cli.h"
#include "machoimage.h"

#if defined(__aarch64__)

//#include "elfarmlinker64.h"
//#include "arm64compiler.h"
//#include "elfarmimage.h"

#endif

#include "constants.h"
#include "messages.h"
#include "linux/presenter.h"
//#include "linux/pathmanager.h"

#include <stdarg.h>

using namespace elena_lang;

#if defined(__x86_64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::MacOS_x86_64;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::MacOS_ARM64;

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

JITCompilerBase* createJITCompiler(LibraryLoaderBase* loader, PlatformType platform)
{
   switch (platform) {
#if defined(__aarch64__)
      case PlatformType::MacOS_ARM64:
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

         project.loadConfigByName(dataPath, *configName, true);
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

int compileProject(int argc, char** argv, path_t dataPath, ErrorProcessor& errorProcessor,
   CompilingProcess& process)
{
   bool cleanMode = false;

   Project          project(dataPath, CURRENT_PLATFORM, &Presenter::getInstance());
   MacOSLinker      linker(&errorProcessor, &MacOSImageFormatter::getInstance(&project));

   // Initializing...
   path_t defaultConfigPath /*= PathHelper::retrieveFilePath(LOCAL_DEFAULT_CONFIG)*/;
//   if (defaultConfigPath.compare(LOCAL_DEFAULT_CONFIG)) {
      // if the local config file was not found
      defaultConfigPath = DEFAULT_CONFIG;
//   }

   PathString configPath(dataPath, /*PathHelper::retrieveFilePath(*/defaultConfigPath/*)*/);
   project.loadConfig(*configPath, nullptr, false);

   IdentifierString profile;
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

         project.addSource(*fileName, argv[i], nullptr, nullptr);
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
         MINIMAL_ARG_LIST,
         *profile);
   }
}

int compileProjectCollection(int argc, char** argv, path_t path, path_t dataPath,
   ErrorProcessor& errorProcessor, CompilingProcess& process)
{
   Presenter* presenter = &Presenter::getInstance();

   int retVal = 0;
   ProjectCollection collection;

   if (!collection.load(path)) {
      presenter->printPath(presenter->getMessage(wrnInvalidConfig), path);

      return ERROR_RET_CODE;
   }

   for (auto it = collection.paths.start(); !it.eof(); ++it) {
      size_t destLen = FILENAME_MAX;
      char projectPath[FILENAME_MAX];
      StrConvertor::copy(projectPath, (*it).str(), (*it).length(), destLen);
      projectPath[destLen] = 0;

      argv[argc - 1] = projectPath;
      presenter->printPath(ELC_COMPILING_PROJECT, projectPath);

      int result = compileProject(argc, argv, dataPath, errorProcessor, process);
      if (result == ERROR_RET_CODE) {
         return ERROR_RET_CODE;
      }
      else if (result == WARNING_RET_CODE) {
         retVal = WARNING_RET_CODE;
      }
   }

   return retVal;
}

//const char* dataFileList[] = { BC_RULES_FILE, BT_RULES_FILE, SYNTAX60_FILE };

int main(int argc, char* argv[])
{
   try
   {
      PathString dataPath(/*PathHelper::retrievePath(dataFileList, 3, */DATA_PATH/*)*/);

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
