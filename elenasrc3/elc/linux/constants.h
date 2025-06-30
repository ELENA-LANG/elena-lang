//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the platform dependant common constants of the command-line
//      compiler and a ELC project class
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace elena_lang
{
   // compiler service files
   constexpr auto LOCAL_DEFAULT_CONFIG = "./local.elc60.config";

#if defined(__FreeBSD__)

   constexpr auto DEFAULT_CONFIG = "/usr/local/etc/elena/elc60.config";
   constexpr auto DATA_PATH = "/usr/local/share/elena";

#else

   constexpr auto DEFAULT_CONFIG = "/etc/elena/elc60.config";
   constexpr auto DATA_PATH = "/usr/share/elena";

#endif

}

#endif