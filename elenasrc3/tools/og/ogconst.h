//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

namespace elena_lang
{
   #define SG_REVISION_NUMBER            0x000D

   constexpr auto OG_GREETING            = "ELENA command line optimization rule set generator %d.%d.%d (C)2023 by Aleksey Rakov\n";

   constexpr auto OG_HELP                = "og-cli <optimization_file>\n";

   constexpr auto OG_FATAL               = "a fatal error\n";
   constexpr auto OG_INVALID_OPCODE      = "(%d, %d): Invalid token\n";

   constexpr auto OG_FILENOTEXIST        = "error:file not found\n";
   constexpr auto OG_INVALID_RULE        = "error(%d:%d) : invalid rule structure\n";
}

#endif
