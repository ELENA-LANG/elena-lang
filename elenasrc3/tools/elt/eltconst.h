//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the elt common interfaces & types
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELTCONST_H
#define ELTCONST_H

namespace elena_lang
{
   #define ELT_REVISION_NUMBER               0x0006

   constexpr auto ELT_GREETING               = "ELENA command line VM terminal %d.%d.%d (C)2021-25 by Aleksey Rakov\n";

   constexpr auto REPL_TEMPLATE_NAME         = "repl";
   constexpr auto MULTILINE_TEMPLATE_NAME    = "multiline";
   constexpr auto GETVAR_TEMPLATE_NAME       = "get_var";
   constexpr auto SETVAR_TEMPLATE_NAME       = "set_var";

#if (defined(_WIN32) || defined(__WIN32__))

   constexpr auto ELT_CONFIG                 = "~\\elt60.es";
   constexpr auto ELT_GRAMMAR_CONFIG         = "~\\scripts\\grammar60.es";
   constexpr auto ELT_LSCRIPT_CONFIG         = "~\\scripts\\lscript60.es";

#elif defined(__FreeBSD__)

   constexpr auto ELT_CONFIG_PATH            = "/usr/local/etc/elena";
   constexpr auto ELT_CONFIG                 = "/usr/local/etc/elena/elt60.es";
   constexpr auto ELT_GRAMMAR_CONFIG         = "/usr/local/etc/elena/scripts/grammar60.es";
   constexpr auto ELT_LSCRIPT_CONFIG         = "/usr/local/etc/elena/scripts/lscript60.es";

#else

   constexpr auto ELT_CONFIG_PATH            = "/etc/elena";
   constexpr auto ELT_CONFIG                 = "/etc/elena/elt60.es";
   constexpr auto ELT_GRAMMAR_CONFIG         = "/etc/elena/scripts/grammar60.es";
   constexpr auto ELT_LSCRIPT_CONFIG         = "/etc/elena/scripts/lscript60.es";

#endif

   constexpr auto ELT_STARTUP_FAILED         = "ELENAVM: Start has failed";
   constexpr auto ELT_CODE_FAILED            = "ELENAVM: Operation has failed";
   constexpr auto ELT_SCRIPT_FAILED          = "\nFailed:%s";

   constexpr auto ELT_EXCEPTION_HANDLER      = "system'core_routines'critical_exception_handler";

   constexpr auto ELT_LOADING_TEMPLATE       = "Loading %s template\n";
   constexpr auto ELT_CANNOT_LOAD_TEMPLATE   = "Cannot load %s template\n";
}

#endif
