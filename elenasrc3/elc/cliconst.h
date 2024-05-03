//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

#include "langcommon.h"

namespace elena_lang
{
   #define ELC_REVISION_NUMBER               0x0269

#if defined _M_IX86 || _M_X64

   #define ERROR_RET_CODE     -2
   #define WARNING_RET_CODE   -1

#elif defined __i386__ || __x86_64__ || __PPC64__ || __aarch64__

   #define ERROR_RET_CODE     2
   #define WARNING_RET_CODE   1

#endif

   // --- Information messages ---
   constexpr auto ELC_GREETING               = "ELENA Command-line compiler %d.%d.%d (C)2005-2024 by Aleksey Rakov\n";
   constexpr auto ELC_STARTING               = "Project: %s, Platform: %s, Target type: %s";
   constexpr auto ELC_PROFILE_INFO           = "Project profile: %s";
   constexpr auto ELC_CLEANING               = "Cleaning up";
   constexpr auto ELC_LINKING                = "Linking..\n";

   constexpr auto ELC_SUCCESSFUL_COMPILATION = "\nSuccessfully compiled\n";
   constexpr auto ELC_SUCCESSFUL_LINKING     = "Successfully linked\n";
   constexpr auto ELC_UNSUCCESSFUL           = "Compiled with errors\n";

   constexpr auto ELC_PARSING_FILE           = "Parsing %s";
   constexpr auto ELC_COMPILING_MODULE       = "Compiling %s";
   constexpr auto ELC_COMPILING_TEMPLATE     = "\nCompiling %s";

   constexpr auto ELC_SAVING_MODULE          = "\nsaving %s\n";

   constexpr auto ELC_PROFILE_WARNING        = "\nWARNING - Please select one of available profiles:%s\n";

   constexpr auto ELC_HELP_INFO              = "elena-cli {-key} {source-file+ | project-file}\nkeys:\n   -f{fwd=reference}   - add a forward\n   -l{profile name}    - selecct a profile\n   -m                  - turning on address mapping output\n   -o{0 | 1 | 2}       - set the optimization level\n   -p                  - set the base path\n   -r                  - clean the compilation output\n   -s{ stackReserv:n } - set the linker option - stack reserved\n   -t{ template name } - load the project template\n   -v                  - turn on a verbose output mode\n  - w{ 0 | 1 | 2 | 3 } - set the minimal warnings level to X = { 0 | 1 | 2 | 3 }\n   -xb[-]              - turn on / off a conditional boxing\n   -xe[-]              - turn on / off a compile-time expression evaluation\n   -xm[-]              - turn on / off auto loading module extension list\n   -xp[-]              - turn on / off generation of the parameter meta info";

   constexpr auto SYNTAX_FILE                = "syntax60.dat";
   constexpr auto BC_RULES_FILE              = "bc_rules60.dat";
   constexpr auto BT_RULES_FILE              = "bt_rules60.dat";

   constexpr auto VA_ALIGNMENT               = 0x08;

   inline ustr_t getTargetTypeName(PlatformType type, PlatformType system)
   {
      if (system == PlatformType::VMClient) {
         switch (type) {
            case PlatformType::Console:
               return VM_CONSOLE_KEY;
            case PlatformType::Library:
               return LIBRARY_KEY;
            case PlatformType::GUI_App:
               return VM_GUI_KEY;
            default:
               return "undefined";
         }
      }
      else {
         switch (type) {
            case PlatformType::Console:
               return CONSOLE_KEY;
            case PlatformType::MTA_Console:
               return MT_CONSOLE_KEY;
            case PlatformType::Library:
               return LIBRARY_KEY;
            case PlatformType::GUI_App:
               return GUI_KEY;
            default:
               return "undefined";
         }
      }
   }
}

#endif
