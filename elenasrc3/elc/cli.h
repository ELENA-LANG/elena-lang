//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler interface code declaration
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLI_H
#define CLI_H

#include "cliconst.h"
#include "clicommon.h"
#include "compiling.h"

namespace elena_lang
{
   // --- CLIHelper -- 

   class CLIHelper
   {
   public:
      static PlatformType definePlatform(int argc, path_c** argv, PlatformType defaultPlatform)
      {
#if defined(CROSS_COMPILE_MODE)
         for (int i = 1; i < argc; i++) {
            IdentifierString arg(argv[i]);

            if ((*arg).compare(WIN32_PLATFORM_OPTION)) {
#if defined(__x86_64__) || defined(__i386__)
               return PlatformType::Win_x86;
#else
               return PlatformType::None;
#endif
            }
            else if ((*arg).compare(WIN64_PLATFORM_OPTION)) {
#if defined(__x86_64__) || defined(__i386__)
               return PlatformType::Win_x86_64;
#else
               return PlatformType::None;
#endif
            }
            else if ((*arg).compare(LNX64_PLATFORM_OPTION)) {
#if defined(__x86_64__) || defined(_M_X64)
               return PlatformType::Linux_x86_64;
#else
               return PlatformType::None;
#endif
            }
         }
#endif
         return defaultPlatform;
      }

      static ProcessSettings getProcessSettings(PlatformType platform)
      {
         switch (platform) {
#if defined(__i386__) || defined(__x86_64__) || defined (_M_IX86) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
            case PlatformType::Linux_x86:
            case PlatformType::Win_x86:
               return { 344064, 86016, 0x200000, 1, true, true };
#endif
#if defined(__x86_64__) || defined (_M_X64) || defined(CROSS_COMPILE_MODE)
            case PlatformType::Linux_x86_64:
            case PlatformType::FreeBSD_x86_64:
            case PlatformType::Win_x86_64:
               return { 688128, 204800, 0x200000, 1, true, true };
#endif
#if defined(__PPC64__)
            case PlatformType::Linux_PPC64le:
               return { 688128, 204800, 0x200000, 1, true, true };
#endif
#if defined(__aarch64__)
            case PlatformType::Linux_ARM64:
               return { 688128, 204800, 0x200000, 1, true, true };
#endif
         default:
            return {};
         }
      }

      static JITCompilerSettings getJITCompilerSettings(PlatformType platform, ErrorProcessorBase* errorProcessor);
      static JITCompilerBase* createJITCompiler(PlatformType platform);
      static LinkerBase* createLinker(PlatformType platform, Project* project, ErrorProcessorBase* errorProcessor);

      static int compileProjectCollection(int argc, path_c** argv, path_t path, path_t appPath,
         PlatformType platform,
         ErrorProcessor& errorProcessor, PresenterBase& presenter,
         int(*compileSingleProject)(int, path_c**, path_t, ErrorProcessor&, path_t, ustr_t));

      static int compileProject(int argc, path_c** argv,
         CompilingProcess& process, 
         PlatformType platform, JITCompilerSettings& jitSettings,
         PresenterBase& presenter, ErrorProcessor& errorProcessor,
         path_t dataPath, path_t basePath, path_t configPath,
         ustr_t defaultProfile);

      static void handleOption(path_c* arg, IdentifierString& profile, Project& project, CompilingProcess& process,
         ErrorProcessor& errorProcessor, path_t dataPath, bool& cleanMode);
   };

}

#endif