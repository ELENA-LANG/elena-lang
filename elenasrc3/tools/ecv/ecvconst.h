//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the ecv common interfaces & types
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ECVCONST_H
#define ECVCONST_H

namespace elena_lang
{
   #define ECV_REVISION_NUMBER               0x004E

   constexpr auto ECV_GREETING = "ELENA command line ByteCode Viewer %d.%d.%d (C)2021-23 by Aleksey Rakov\n";

   constexpr auto ECV_MODULE_NOTLOADED = "cannot load a module: %s";
   constexpr auto ECV_MODULE_LOADED    = "module %s loaded\n";
   constexpr auto ECV_SYMBOL_NOTFOUND  = "Symbol not found: %s";
   constexpr auto ECV_CLASS_NOTFOUND   = "Class not found: %s";

}

#endif
