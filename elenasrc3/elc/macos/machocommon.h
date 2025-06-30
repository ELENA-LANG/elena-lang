//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains Common MachO types
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

   constexpr auto __PAGEZERO_SEGMENT = "__PAGEZERO";
   constexpr auto __TEXT_SEGMENT = "__TEXT";
   constexpr auto __DATA_SEGMENT = "__DATA";

   constexpr int Flags_NoUndefs = 0x000001;
   constexpr int Flags_DyldLink = 0x000004;
   constexpr int Flags_TwoLevel = 0x000080;
   
   constexpr int Command_Segment_64 = 0x00000019;

   struct Command
   {
      uint32_t commandType;

      uint32_t commandSize;
   };

   typedef List<Command*, freeobj> Commands;

   typedef vm_prot_t int;

   constexpr int PROT_R = 1;
   constexpr int PROT_W = 2;
   constexpr int PROT_X = 4;
}

#endif