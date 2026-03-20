//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the ldoc common interfaces & types
//
//                                             (C)2022-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ECVCONST_H
#define ECVCONST_H

namespace elena_lang
{
   #define LDOC_REVISION_NUMBER           0x0001

   constexpr auto LDOC_GREETING           = "ELENA command line Html Documentation generator %d.%d.%d (C)2021-26 by Aleksey Rakov\n";
   constexpr auto LDOC_READING            = "Reading...\n";
   constexpr auto LDOC_GENERATING         = "Generating %s...\n";
   constexpr auto LDOC_INDEX_GENERATING   = "Generating indexes for %s...\n";

   constexpr auto LDOC_MODULE_NOTLOADED   = "cannot load a module: %s";

   constexpr auto TITLE                   = "ELENA Standard Library 7.0: Module ";
   constexpr auto TITLE2                  = "ELENA&nbsp;Standard&nbsp;Library<br>7.0";
   constexpr auto INDEX_TITLE             = "ELENA Standard Library 7.0";

}

#endif
