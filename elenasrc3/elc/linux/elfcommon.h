//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains Common ELF types
//                                              (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFCOMMON_H
#define ELFCOMMON_H

namespace elena_lang
{
   constexpr auto SECTION_ALIGNMENT  = 0x1000;
   constexpr auto FILE_ALIGNMENT     = 0x0010;

   constexpr auto elfDynamicOffset   = 1;
   constexpr auto elfDynamicVAddress = 2;
   constexpr auto elfDynamicSize     = 3;
   constexpr auto elfTLSSize         = 5;

}

#endif
