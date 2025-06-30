//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the platform dependant common constants of the command-line
//      compiler and a ELC project class
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACOS_CONSTANTS_H
#define MACOS_CONSTANTS_H

namespace elena_lang
{
   // compiler service files
   constexpr auto LOCAL_DEFAULT_CONFIG = "./local.elc60.config";

   constexpr auto DEFAULT_CONFIG = "/etc/elena/elc60.config";
   constexpr auto DATA_PATH = "/usr/share/elena";

}

#endif // MACOS_CONSTANTS_H