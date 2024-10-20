//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the main body of the win32 / win64 command-line compiler
//
//                                             (C)2021-2024, by Aleksey Rakov
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
constexpr int DEFAULT_SACKRESERV          = 0x200000;

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
constexpr int DEFAULT_SACKRESERV          = 0x200000;

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

void handleOption(wchar_t* arg, IdentifierString& profile, Project& project, CompilingProcess& process,
   ErrorProcessor& errorProcessor, path_t appPath, bool& cleanMode)
{
   switch (arg[1]) {
      case 'e':
         if (wstr_t(arg).compare(L"-el5")) {
            project.setSyntaxVersion(SyntaxVersion::L5);
         }
         else if (wstr_t(arg).compare(L"-el6")) {
            project.setSyntaxVersion(SyntaxVersion::L6);
         }
         break;
      case 'f':
      {
         IdentifierString setting(arg + 2);
         process.addForward(*setting);

         break;
      }
      case 'l':
      {
         IdentifierString setting(arg + 2);
         profile.copy(*setting);
         break;
      }
      case 'm':
         project.addBoolSetting(ProjectOption::MappingOutputMode, true);
         break;
      case 'o':
         if (arg[2] == '0') {
            project.addIntSetting(ProjectOption::OptimizationMode, optNone);
         }
         else if (arg[2] == '1') {
            project.addIntSetting(ProjectOption::OptimizationMode, optLow);
         }
         else if (arg[2] == '2') {
            project.addIntSetting(ProjectOption::OptimizationMode, optMiddle);
         }
         else if (arg[2] == '3') {
            project.addIntSetting(ProjectOption::OptimizationMode, optHigh);
         }
         break;
      case 'p':
         project.setBasePath(arg + 2);
         break;
      case 'r':
         cleanMode = true;
         break;
      case 's':
      {
         IdentifierString setting(arg + 2);
         if (setting.compare("stackReserv:", 0, 12)) {
            ustr_t valStr = *setting + 12;
            int val = StrConvertor::toInt(valStr, 10);
            project.addIntSetting(ProjectOption::StackReserved, val);
         }
         break;
      }
      case 't':
      {
         IdentifierString configName(arg + 2);

         if(!project.loadConfigByName(appPath, *configName, true))
            errorProcessor.info(wrnInvalidConfig, *configName);
         break;
      }
      case 'v':
         process.setVerboseOn();
         break;
      case 'w':
         if (arg[2] == '0') {
            errorProcessor.setWarningLevel(WarningLevel::Level0);
         }
         else if (arg[2] == '1') {
            errorProcessor.setWarningLevel(WarningLevel::Level1);
         }
         else if (arg[2] == '2') {
            errorProcessor.setWarningLevel(WarningLevel::Level2);
         }
         else if (arg[2] == '3') {
            errorProcessor.setWarningLevel(WarningLevel::Level3);
         }
         break;
      case 'x':
         if (arg[2] == 'b') {
            project.addBoolSetting(ProjectOption::ConditionalBoxing, arg[3] != '-');
         }
         else if (arg[2] == 'e') {
            project.addBoolSetting(ProjectOption::EvaluateOp, arg[3] != '-');
         }
         else if (arg[2] == 'j') {
            project.addBoolSetting(ProjectOption::WithJumpAlignment, arg[3] != '-');
         }
         else if (arg[2] == 'm') {
            project.addBoolSetting(ProjectOption::ModuleExtensionAutoLoad, arg[3] != '-');
         }
         else if (arg[2] == 'p') {
            project.addBoolSetting(ProjectOption::GenerateParamNameInfo, arg[3] != '-');
         }
         else if (arg[2] == 's') {
            project.addBoolSetting(ProjectOption::StrictTypeEnforcing, arg[3] != '-');
         }
         break;
      default:
         break;
   }
}

int compileProject(int argc, wchar_t** argv, path_t appPath, ErrorProcessor& errorProcessor, 
   CompilingProcess& process)
{
   bool cleanMode = false;

   Project          project(appPath, CURRENT_PLATFORM, &Presenter::getInstance());
   WinLinker        linker(&errorProcessor, &WinImageFormatter::getInstance(&project));

   // Initializing...
   PathString configPath(appPath, DEFAULT_CONFIG);
   project.loadConfig(*configPath, nullptr, false);

   IdentifierString profile;
   for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
         handleOption(argv[i], profile, project, process,
            errorProcessor, appPath, cleanMode);
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
         IdentifierString ns(*fileName);
         project.addSource(*ns, argv[i], nullptr, nullptr);
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

int compileProjectCollection(int argc, wchar_t** argv, path_t path, path_t appPath,
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
      wchar_t projectPath[FILENAME_MAX];
      StrConvertor::copy(projectPath, (*it).str(), (*it).length(), destLen);
      projectPath[destLen] = 0;

      argv[argc - 1] = projectPath;
      presenter->printPath(ELC_COMPILING_PROJECT, projectPath);

      int result = compileProject(argc, argv, appPath, errorProcessor, process);
      if (result == ERROR_RET_CODE) {
         return ERROR_RET_CODE;
      }
      else if (result == WARNING_RET_CODE) {
         retVal = WARNING_RET_CODE;
      }
   }

   return retVal;
}

int main()
{
   try
   {
      PathString appPath;
      getAppPath(appPath);
      
      JITSettings      defaultCoreSettings = { DEFAULT_MGSIZE, DEFAULT_YGSIZE, DEFAULT_SACKRESERV, 1, true, false };
      ErrorProcessor   errorProcessor(&Presenter::getInstance());
      CompilingProcess process(*appPath, L"exe", L"<moduleProlog>", L"<prolog>", L"<epilog>",
         &Presenter::getInstance(), &errorProcessor,
         VA_ALIGNMENT, defaultCoreSettings, createJITCompiler);

      process.greeting();

      // Reading command-line arguments...
      int argc;
      wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

      if (argc < 2) {
         Presenter::getInstance().printLine(ELC_HELP_INFO);
         return -2;
      }
      else if (argv[argc - 1][0] != '-' && PathUtil::checkExtension(argv[argc - 1], "prjcol")) {
         return compileProjectCollection(argc, argv, argv[argc - 1], 
            *appPath, errorProcessor, process);
      }
      else return compileProject(argc, argv, *appPath, errorProcessor, process);
   }
   catch (CLIException)
   {
      return ERROR_RET_CODE;
   }
}
