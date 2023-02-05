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
   #define ELC_REVISION_NUMBER               0x01A7

   // --- Information messages ---
   constexpr auto ELC_GREETING               = "ELENA Command-line compiler %d.%d.%d (C)2005-2023 by Aleksey Rakov\n";
   constexpr auto ELC_STARTING               = "Project: %s, Platform: %s, Target type: %s";
   constexpr auto ELC_CLEANING               = "Cleaning up";
   constexpr auto ELC_LINKING                = "Linking..\n";

   constexpr auto ELC_SUCCESSFUL_COMPILATION = "\nSuccessfully compiled\n";
   constexpr auto ELC_SUCCESSFUL_LINKING     = "Successfully linked\n";
   constexpr auto ELC_UNSUCCESSFUL           = "Compiled with errors\n";

   constexpr auto ELC_PARSING_FILE           = "\nParsing %s";
   constexpr auto ELC_COMPILING_MODULE       = "\nCompiling %s";

   constexpr auto ELC_HELP_INFO              = "elena-cli {source-file+}";

   constexpr auto SYNTAX_FILE                = "syntax60.dat";

   constexpr auto VA_ALIGNMENT               = 0x08;

   inline ustr_t getTargetTypeName(PlatformType type, PlatformType system)
   {
      if (system == PlatformType::VMClient) {
         switch (type) {
            case PlatformType::Console:
               return VM_CONSOLE_KEY;
            case PlatformType::Library:
               return LIBRARY_KEY;
            default:
               return "undefined";
         }
      }
      else {
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
}

#endif
