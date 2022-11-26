//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

namespace elena_lang
{
   #define ELC_REVISION_NUMBER               0x0161

   // --- Information messages ---
   constexpr auto ELC_GREETING               = "ELENA Command-line compiler %d.%d.%d (C)2005-2022 by Aleksey Rakov\n";
   constexpr auto ELC_STARTING               = "Project: %s, Platform: %s, Target type: %s";
   constexpr auto ELC_CLEANING               = "Cleaning up";
   constexpr auto ELC_LINKING                = "Linking..\n";

   constexpr auto ELC_SUCCESSFUL_COMPILATION = "\nSuccessfully compiled\n";
   constexpr auto ELC_SUCCESSFUL_LINKING     = "Successfully linked\n";
   constexpr auto ELC_UNSUCCESSFUL           = "Compiled with errors\n";

   constexpr auto ELC_PARSING_FILE           = "\nParsing %s";
   constexpr auto ELC_COMPILING_MODULE       = "\nCompiling %s";

   constexpr auto ELC_HELP_INFO              = "elena-cli {source-file+}";

   // --- Configuration xpaths ---
   constexpr auto WIN_X86_KEY                = "Win_x86";
   constexpr auto WIN_X86_64_KEY             = "Win_x64";
   constexpr auto LINUX_X86_KEY              = "Linux_I386";
   constexpr auto LINUX_X86_64_KEY           = "Linux_AMD64";
   constexpr auto LINUX_PPC64le_KEY          = "Linux_PPC64le";
   constexpr auto LINUX_ARM64_KEY            = "Linux_ARM64";
   constexpr auto LIBRARY_KEY                = "Library";
   constexpr auto CONSOLE_KEY                = "STA Console";

   constexpr auto CONFIG_ROOT                = "configuration";
   constexpr auto PLATFORM_CATEGORY          = "configuration/platform";
   constexpr auto TEMPLATE_CATEGORY          = "templates/*";
   constexpr auto PRIMITIVE_CATEGORY         = "primitives/*";
   constexpr auto FORWARD_CATEGORY           = "forwards/*";
   constexpr auto EXTERNAL_CATEGORY          = "externals/*";
   constexpr auto WINAPI_CATEGORY            = "winapi/*";
   constexpr auto REFERENCE_CATEGORY         = "references/*";
   constexpr auto MODULE_CATEGORY            = "files/*";
   constexpr auto FILE_CATEGORY              = "include/*";

   constexpr auto LIB_PATH                   = "project/libpath";
   constexpr auto OUTPUT_PATH                = "project/output";
   constexpr auto TARGET_PATH                = "project/executable";
   constexpr auto PROJECT_TEMPLATE           = "project/template";
   constexpr auto NAMESPACE_KEY              = "project/namespace";
   constexpr auto DEBUGMODE_PATH             = "project/debuginfo";
   constexpr auto FILE_PROLOG                = "project/prolog";
   constexpr auto FILE_EPILOG                = "project/epilog";

   constexpr auto PLATFORMTYPE_KEY           = "system/platform";

   constexpr auto MGSIZE_PATH                = "linker/mgsize";
   constexpr auto YGSIZE_PATH                = "linker/ygsize";

   constexpr auto SYNTAX_FILE                = "syntax60.dat";

   constexpr auto VA_ALIGNMENT               = 0x08;

   inline ustr_t getPlatformName(PlatformType type)
   {
      switch (type) {
         case PlatformType::Win_x86:
            return WIN_X86_KEY;
         case PlatformType::Win_x86_64:
            return WIN_X86_64_KEY;
         case PlatformType::Linux_x86:
            return LINUX_X86_KEY;
         case PlatformType::Linux_x86_64:
            return LINUX_X86_64_KEY;
         case PlatformType::Linux_PPC64le:
            return LINUX_PPC64le_KEY;
         case PlatformType::Linux_ARM64:
            return LINUX_ARM64_KEY;
         default:
            return nullptr;
      }
   }

   inline ustr_t getTargetTypeName(PlatformType type)
   {
      switch (type) {
         case PlatformType::Console:
            return CONSOLE_KEY;
         case PlatformType::Library:
            return LIBRARY_KEY;
         default:
            return "undefined";
      }
   }
}

#endif
