//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the ldoc common interfaces & types
//
//                                             (C)2022-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ECVCONST_H
#define ECVCONST_H

namespace elena_lang
{
   #define LDOC_REVISION_NUMBER           0x001B

   constexpr auto LDOC_GREETING           = "ELENA command line Html Documentation generator %d.%d.%d (C)2021-24 by Aleksey Rakov\n";
   constexpr auto LDOC_READING            = "Reading...\n";
   constexpr auto LDOC_GENERATING         = "Generating %s...\n";

   constexpr auto LDOC_MODULE_NOTLOADED   = "cannot load a module: %s";

   constexpr auto TITLE                   = "ELENA Standard Library 6.4: Module ";
   constexpr auto TITLE2                  = "ELENA&nbsp;Standard&nbsp;Library<br>6.3";

}

#endif
