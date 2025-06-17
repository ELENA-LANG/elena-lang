//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler interface code implementation
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cli.h"
#include "project.h"
#include "langcommon.h"

#if defined (__unix__) || defined(CROSS_COMPILE_MODE)

#if defined(__x86_64__) || defined(_M_X64)

#include "linux/elflinker32.h"
#include "linux/elflinker64.h"
#include "linux/elfimage.h"

#include "x86compiler.h"
#include "x86_64compiler.h"

#elif defined(__i386__) || defined (_M_IX86)

#include "linux/elflinker32.h"
#include "linux/elfimage.h"

#include "x86compiler.h"

#elif defined(__aarch64__)

#include "linux/elfarmlinker64.h"
#include "linux/arm64compiler.h"

#include "linux/elfarmimage.h"

#elif defined(__PPC64__)

#include "linux/elfppclinker64.h"
#include "linux/elfppcimage.h"

#include "ppc64compiler.h"

#endif

#endif

#if (defined(_WIN32) || defined(__WIN32__)) || defined(CROSS_COMPILE_MODE)

#include "windows/ntimage.h"

#if defined(__x86_64__) || defined(_M_X64)

#include "windows/ntlinker32.h"
#include "windows/ntlinker64.h"
#include "x86compiler.h"
#include "x86_64compiler.h"

#elif defined(__i386__) || defined (_M_IX86)

#include "windows/ntlinker32.h"
#include "x86compiler.h"

#endif

#endif

//#define TIME_RECORDING 1

using namespace elena_lang;

// --- CommandHelper ---

JITCompilerSettings CLIHelper :: getJITCompilerSettings(PlatformType platform, ErrorProcessorBase* errorProcessor)
{
   switch (platform) {
#if defined(__x86_64__) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
      case PlatformType::Win_x86_64:
         return X86_64JITCompiler::getSettings();
#endif
#if defined(__i386__) || defined (_M_IX86) || defined(__x86_64__) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
      case PlatformType::Win_x86:
      case PlatformType::Linux_x86:
         return X86JITCompiler::getSettings();
#endif
#if defined(__PPC64__)
      case PlatformType::Linux_PPC64le:
         return PPC64leJITCompiler::getSettings();
#endif
#if defined(__aarch64__)
      case PlatformType::Linux_ARM64:
         return ARM64JITCompiler::getSettings();
#endif
      default:
         errorProcessor->raiseError(errNotSupportedPlatform);
         return {};
   }
}

JITCompilerBase* CLIHelper :: createJITCompiler(PlatformType platform)
{
   switch (platform) {
#if defined(__x86_64__) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
      case PlatformType::Win_x86_64:
      case PlatformType::FreeBSD_x86_64:
      case PlatformType::Linux_x86_64:
         return new X86_64JITCompiler();
#endif
#if defined(__i386__) || defined (_M_IX86) || defined(__x86_64__) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
      case PlatformType::Win_x86:
      case PlatformType::Linux_x86:
         return new X86JITCompiler();
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

LinkerBase* CLIHelper :: createLinker(PlatformType platform, Project* project, ErrorProcessorBase* errorProcessor)
{
   switch (platform) {
#if defined(_WIN32) || defined(__WIN32__) || defined(CROSS_COMPILE_MODE)

#if defined(__x86_64__) || defined (_M_X64)
      case PlatformType::Win_x86_64:
         return new Win64NtLinker(errorProcessor, &Win64NtImageFormatter::getInstance(project));
#endif
#if defined(__i386__) || defined (_M_IX86)
      case PlatformType::Win_x86:
         return new Win32NtLinker(errorProcessor, &Win32NtImageFormatter::getInstance(project));
#endif

#endif

#if defined(__unix__) || defined(CROSS_COMPILE_MODE)

#if defined(__x86_64__) || defined (_M_X64)

   case PlatformType::FreeBSD_x86_64:
      return new ElfFreeBSDAmd64Linker(errorProcessor, &ElfFreeBSDAmd64ImageFormatter::getInstance(project));
   case PlatformType::Linux_x86_64:
      return new ElfAmd64Linker(errorProcessor, &ElfAmd64ImageFormatter::getInstance(project));

#elif defined(__i386__) || defined (_M_IX86)

   case PlatformType::Linux_x86:
      return new ElfI386Linker(errorProcessor, &ElfI386ImageFormatter::getInstance(project));

#elif defined(__aarch64__)

   case PlatformType::Linux_ARM64:
      return new ElfARM64Linker(errorProcessor, &ElfARM64ImageFormatter::getInstance(project));

#elif defined(__PPC64__)

   case PlatformType::Linux_PPC64le:
      return new ElfPPC64leLinker(errorProcessor, &ElfPPC64leImageFormatter::getInstance(project));

#endif

#endif
   default:
      errorProcessor->raiseError(errNotSupportedPlatform);
      return nullptr;
   }
}

void CLIHelper :: handleOption(path_c* arg, IdentifierString& profile, Project& project, CompilingProcess& process,
   ErrorProcessor& errorProcessor, path_t dataPath, bool& cleanMode)
{
   switch (arg[1]) {
      case 't':
      {
         IdentifierString configName(arg + 2);

         if (!project.loadConfigByName(dataPath, *configName, true))
            errorProcessor.info(wrnInvalidConfig, *configName);;
         break;
      }
      case 'p':
         project.setBasePath(arg + 2);
         break;
      case 'e':
      {
         IdentifierString argStr(arg);
         if (argStr.compare("-el5")) {
            project.setSyntaxVersion(SyntaxVersion::L5);
         }
         else if (argStr.compare("-el6")) {
            project.setSyntaxVersion(SyntaxVersion::L6);
         }
         else if (argStr.compare("-el7")) {
            project.setSyntaxVersion(SyntaxVersion::L7);
         }
         break;
      }
      case 'f':
      {
         IdentifierString argStr(arg + 2);
         process.addForward(*argStr);
         break;
      }
      case 'l':
      {
         IdentifierString argStr(arg + 2);
         profile.copy(*argStr);
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
      case 'r':
         cleanMode = true;
         break;
      case 's':
      {
         IdentifierString setting(arg + 2);
         if ((*setting).compareSub("stackReserv:", 0, 12)) {
            ustr_t valStr = *setting + 12;
            int val = StrConvertor::toInt(valStr, 10);
            project.addIntSetting(ProjectOption::StackReserved, val);
         }
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
         else if (arg[2] == 'n') {
            project.addBoolSetting(ProjectOption::NullableTypeWarning, arg[3] != '-');
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
         else if (arg[2] == 't') {
            // ignore platform specification
         }
         break;
      case '-':
      {
         IdentifierString argStr(arg);
         if (argStr.compare("--tracing")) {
            project.addBoolSetting(ProjectOption::TracingMode, true);
         }
         break;
      }
      default:
         break;
   }
}

int CLIHelper :: compileProject(int argc, path_c** argv,
   CompilingProcess& process,
   PlatformType platform, JITCompilerSettings& jitSettings,
   PresenterBase& presenter, ErrorProcessor& errorProcessor,
   path_t dataPath, path_t basePath, path_t configPath,
   ustr_t defaultProfile)
{
   // try to specify supported cross-compile platform
   if (platform == PlatformType::None)
      errorProcessor.raiseError(errNotSupportedPlatform);

   Project          project(dataPath, platform, &presenter);
   LinkerBase* linker = createLinker(platform, &project, &errorProcessor);
   if (!linker)
      errorProcessor.raiseError(errNotSupportedPlatform);

   bool cleanMode = false;

   // Initializing...
   project.loadConfig(configPath, nullptr, false);

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

            presenter.printLine(ELC_PROFILE_WARNING, *profileList);
         }
      }
      else if (PathUtil::checkExtension(argv[i], "prjcol")) {
         presenter.printLine(ELC_PRJ_COLLECTION_WARNING);
         return -2;
      }
      else {
         FileNameString fileName(argv[i]);
         IdentifierString ns(*fileName);
         project.addSource(*ns, argv[i], nullptr, nullptr, true);
      }
   }

   if (!basePath.empty())
      project.setBasePath(basePath);

   if (cleanMode) {
      return process.clean(project);
   }
   else {
#ifdef TIME_RECORDING
      clock_t start, finish;
      start = clock();
#endif
      // Building...
      int retVal = process.build(project, *linker, jitSettings,
         *profile);

#ifdef TIME_RECORDING
      finish = clock();

      double duration = (double)(finish - start) / CLOCKS_PER_SEC;
      printf("The compilation took %2.3f seconds\n", duration);
#endif

      return retVal;
   }
}

int CLIHelper :: compileProjectCollection(int argc, path_c** argv, path_t path, path_t appPath,
   ErrorProcessor& errorProcessor, PresenterBase& presenter,
   int(*compileSingleProject)(int, path_c**, path_t, ErrorProcessor&, path_t, ustr_t))
{
   int retVal = 0;
   ProjectCollection collection;

   if (!collection.load(path)) {
      presenter.printPath(presenter.getMessage(wrnInvalidConfig), path);

      return ERROR_RET_CODE;
   }

   for (auto it = collection.projectSpecs.start(); !it.eof(); ++it) {
      auto spec = *it;

      size_t destLen = FILENAME_MAX;
      path_c projectPath[FILENAME_MAX];
      StrConvertor::copy(projectPath, spec->path.str(), spec->path.length(), destLen);
      projectPath[destLen] = 0;

      argv[argc - 1] = projectPath;
      presenter.printPath(ELC_COMPILING_PROJECT, projectPath);

      int result = compileSingleProject(argc, argv, appPath, errorProcessor, spec->basePath, spec->profile);
      if (result == ERROR_RET_CODE) {
         return ERROR_RET_CODE;
      }
      else if (result == WARNING_RET_CODE) {
         retVal = WARNING_RET_CODE;
      }
   }

   return retVal;
}
