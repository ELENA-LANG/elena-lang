//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the elt common interfaces & types
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTCONST_H
#define ELTCONST_H

namespace elena_lang
{
   #define ELT_REVISION_NUMBER               0x0006

   constexpr auto ELT_GREETING               = "ELENA command line VM terminal %d.%d.%d (C)2021-23 by Aleksey Rakov\n";

   constexpr auto COMMAMD_TEMPLATE           = "command60.es";
   constexpr auto ELT_CONFIG                 = "~\\elt60.es";
   constexpr auto ELT_GRAMMAR_CONFIG         = "~\\scripts\\grammar60.es";
   constexpr auto ELT_TSCRIPT_CONFIG         = "~\\scripts\\tscript60.es";

   constexpr auto ELT_STARTUP_FAILED         = "ELENAVM: Start has failed";
   constexpr auto ELT_CODE_FAILED            = "ELENAVM: Operation has failed";
   constexpr auto ELT_SCRIPT_FAILED          = "\nFailed:%s";
}

#endif
