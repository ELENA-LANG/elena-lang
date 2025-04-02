//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains Common ELF types
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOCOMMON_H
#define MACHOCOMMON_H

namespace elena_lang
{
   constexpr auto SECTION_ALIGNMENT = 0x1000;
   constexpr auto FILE_ALIGNMENT = 0x0010;

   constexpr unsigned long MH_MAGIC_64 = 0xFEEDFACF;

   constexpr unsigned long FILE_EXECUTABLE = 2;

   enum class CPUType : int32_t
   {
      None     = 0,
      AARCH64  = 0x1000000C;
   };

   enum class CPUSubType : int32_t
   {
      None    = 0,
      ARM_ALL = 0
   };

   struct Command
   {
      uint32_t commandType;
      uint32_t commandSize;
   };

   typedef List<Command*, freeobj> Commands;
}

#endif