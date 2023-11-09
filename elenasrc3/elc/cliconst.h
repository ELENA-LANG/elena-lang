//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

#include "langcommon.h"

namespace elena_lang
{
   #define ELC_REVISION_NUMBER               0x0221

#if defined _M_IX86 || _M_X64

   #define ERROR_RET_CODE     -2
   #define WARNING_RET_CODE   -1

#elif defined __i386__ || __x86_64__ || __PPC64__ || __aarch64__

   #define ERROR_RET_CODE     2
   #define WARNING_RET_CODE   1

#endif

   // --- Information messages ---
   constexpr auto ELC_GREETING               = "ELENA Command-line compiler %d.%d.%d (C)2005-2023 by Aleksey Rakov\n";
   constexpr auto ELC_STARTING               = "Project: %s, Platform: %s, Target type: %s";
   constexpr auto ELC_CLEANING               = "Cleaning up";
   constexpr auto ELC_LINKING                = "Linking..\n";

   constexpr auto ELC_SUCCESSFUL_COMPILATION = "\nSuccessfully compiled\n";
   constexpr auto ELC_SUCCESSFUL_LINKING     = "Successfully linked\n";
   constexpr auto ELC_UNSUCCESSFUL           = "Compiled with errors\n";

   constexpr auto ELC_PARSING_FILE           = "Parsing %s";
   constexpr auto ELC_COMPILING_MODULE       = "Compiling %s";
   constexpr auto ELC_COMPILING_TEMPLATE     = "\nCompiling %s";

   constexpr auto ELC_SAVING_MODULE          = "\nsaving %s\n";

   constexpr auto ELC_HELP_INFO              = "elena-cli {-key} {source-file+ | project-file}\nkeys: m - turning on address mapping output\n      r - clean the compilation output\n      t{template name} - loading the project template\n      p - set the base path\n      wX- turns off warnings with level X=1,2,4\n      xp[-] - turning on/off a generation the parameter meta info\n      xb[-] - turning on/off a conditional boxing";

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
