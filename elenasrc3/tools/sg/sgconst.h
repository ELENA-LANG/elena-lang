//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

namespace elena_lang
{

   #define SG_REVISION_NUMBER            0x0001

   constexpr auto SG_GREETING            = "ELENA command line syntax generator %d.%d.%d (C)2005-2024 by Aleksey Rakov\n";

   constexpr auto SG_HELP                = "sg-cli <syntax_file> [-cp<codepage>]\n";

   constexpr auto SG_FATAL               = "a fatal error\n";
   constexpr auto SG_FILENOTEXIST        = "error:file not found\n";
   constexpr auto SG_INVALID_RULE        = "error(%d:%d) : invalid rule structure\n";
   constexpr auto SG_AMBIGUOUS           = "error:ambiguous rule %s for '%s' terminal\n";

}

#endif
