//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA Core constants
//
//                                              (C)2021-2022, by Aleksey Rakov
//------------------------------------------------------------------------------

#ifndef CORE_H
#define CORE_H

namespace elena_lang
{
   // --- 32bit ELENA Object constants ---
   constexpr auto gcPageSize32               = 0x0010;                // a heap page size constant
   constexpr auto gcPageSizeOrder32          = 4;
   constexpr auto gcPageSizeOrderMinus2_32   = 2;
   constexpr auto gcPageMask32               = 0x0FFFFFFF0;

   constexpr auto elVMTClassOffset32         = 0x0010;                // a VMT32 class offset
   constexpr auto elObjectOffset32           = 0x0008;                // object header / offset constant

   constexpr int elStructMask32              = 0x800000;

   // --- 64bit ELENA Object constants ---
   constexpr int gcPageSize64                = 0x0020;                // a heap page size constant
   constexpr auto gcPageSizeOrder64          = 5;
   constexpr auto gcPageSizeOrderMinus2_64   = 3;
   constexpr auto gcPageMask64               = 0x0FFFFFFE0;

   constexpr auto elVMTClassOffset64         = 0x0020;                // a VMT64 class offset
   constexpr auto elObjectOffset64           = 0x0010;                // object header / offset constant

   constexpr int elStructMask64              = 0x40000000;

   // --- ELENA CORE built-in routines
   constexpr ref_t INVOKER                   = 0x10001;
   constexpr ref_t GC_ALLOC                  = 0x10002;

   constexpr ref_t CORE_TOC                  = 0x20001;
   constexpr ref_t SYSTEM_ENV                = 0x20002;
   constexpr ref_t CORE_GC_TABLE             = 0x20003;
   constexpr ref_t VOIDOBJ                   = 0x2000D;
   constexpr ref_t VOIDPTR                   = 0x2000E;

#pragma pack(push, 1)
   // --- VMTHeader ---
   constexpr auto VMTHeader32ParentRefOffs = 0;
   constexpr auto VMTHeader32ClassRefOffs = 8;
   struct VMTHeader32
   {
      pos_t    parentRef;
      ref_t    flags;
      ref_t    classRef;
      pos_t    count;
   };

   // --- VMTEntry32 ---
   struct VMTEntry32
   {
      mssg_t message;
      pos_t  address;
   };

   constexpr auto VMTHeader64ParentRefOffs = 0;
   constexpr auto VMTHeader64ClassRefOffs = 16;
   struct VMTHeader64
   {
      pos64_t  parentRef;
      ref64_t  flags;
      ref64_t  classRef;
      pos64_t  count;
   };

   struct VMTEntry64
   {
      mssg64_t message;
      pos64_t  address;
   };

#pragma pack(pop)

}

#endif
