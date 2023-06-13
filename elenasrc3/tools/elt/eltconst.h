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
   #define ELT_REVISION_NUMBER               0x0001

   constexpr auto ELT_GREETING               = "ELENA command line VM terminal %d.%d.%d (C)2021-23 by Aleksey Rakov\n";

   constexpr auto COMMAMD_TEMPLATE           = "command60.es";
   constexpr auto ELT_CONFIG                 = "~\\elt60.es";

   constexpr auto ELT_STARTUP_FAILED         = "ELENA VM has failed to start";
   constexpr auto ELT_SCRIPT_FAILED          = "\nFailed:%s";
}

#endif
