//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//      This file contains the platform dependant common constants of the command-line
//      compiler and a ELC project class
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace elena_lang
{
   // compiler service files
   constexpr auto LOCAL_DEFAULT_CONFIG = "./local.elc60.config";
   constexpr auto DEFAULT_CONFIG = "/etc/elena/elc60.config";
   constexpr auto DATA_PATH = "/usr/share/elena";

}

#endif