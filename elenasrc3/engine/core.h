//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA Core constants
//
//                                              (C)2021-2023, by Aleksey Rakov
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
   constexpr int  gcPageCeil32               = 0x17;

   constexpr auto elVMTClassOffset32         = 0x0010;                // a VMT32 class offset
   constexpr auto elObjectOffset32           = 0x0008;                // object header / offset constant

   constexpr int  elStructMask32             = 0x800000;
   constexpr int  elObjectSizeMask32         = 0xFFFFFF;
   constexpr int  elSizeCeil32               = 0x0FFFFC;

   constexpr int  elVMTFlagOffset32          = 0x0C;

   // --- 64bit ELENA Object constants ---
   constexpr int  gcPageSize64               = 0x0020;                // a heap page size constant
   constexpr auto gcPageSizeOrder64          = 5;
   constexpr auto gcPageSizeOrderMinus2_64   = 2;
   constexpr auto gcPageMask64               = 0x0FFFFFFE0;
   constexpr int  gcPageCeil64               = 0x2F;

   constexpr auto elVMTClassOffset64         = 0x0020;                // a VMT64 class offset
   constexpr auto elObjectOffset64           = 0x0010;                // object header / offset constant

   constexpr int  elStructMask64             = 0x40000000;
   constexpr int  elObjectSizeMask64         = 0x7FFFFFFF;
   constexpr int  elSizeCeil64               = 0x3FFFFFF8;

   constexpr int  elVMTFlagOffset64          = 0x18;

   // --- ELENA CORE built-in routines
   constexpr ref_t INVOKER                   = 0x10001;
   constexpr ref_t GC_ALLOC                  = 0x10002;
   constexpr ref_t EXCEPTION_HANDLER         = 0x10003;
   constexpr ref_t GC_COLLECT                = 0x10004;
   constexpr ref_t GC_ALLOCPERM              = 0x10005;
   constexpr ref_t PREPARE                   = 0x10006;
   constexpr ref_t THREAD_WAIT               = 0x10007;

   constexpr ref_t CORE_TOC                  = 0x20001;
   constexpr ref_t SYSTEM_ENV                = 0x20002;
   constexpr ref_t CORE_GC_TABLE             = 0x20003;
   constexpr ref_t CORE_TLS_INDEX            = 0x20004;
   constexpr ref_t CORE_SINGLE_CONTENT       = 0x2000B;
   constexpr ref_t VOIDOBJ                   = 0x2000D;
   constexpr ref_t VOIDPTR                   = 0x2000E;
   constexpr ref_t CORE_THREAD_TABLE         = 0x2000F;

   // ELENA run-time exceptions
   constexpr int ELENA_ERR_CRITICAL          = 0x100;
   constexpr int ELENA_ERR_ACCESS_VIOLATION  = 0x101;
   constexpr int ELENA_ERR_DIVIDE_BY_ZERO    = 0x102;
   constexpr int ELENA_ERR_OUT_OF_MEMORY     = 0x103;
   constexpr int ELENA_ERR_OUT_OF_PERMMEMORY = 0x104;

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

   // --- GCTable ---
   struct GCTable
   {
      uintptr_t   gc_header;
      uintptr_t   gc_start;
      uintptr_t   gc_yg_start;
      uintptr_t   gc_yg_current;
      uintptr_t   gc_yg_end;
      uintptr_t   gc_shadow;
      uintptr_t   gc_shadow_end;
      uintptr_t   gc_mg_start;
      uintptr_t   gc_mg_current;
      uintptr_t   gc_end;
      uintptr_t   gc_mg_wbar;

      uintptr_t   gc_perm_start;
      uintptr_t   gc_perm_end;
      uintptr_t   gc_perm_current;

      size_t      gc_lock;             // NOTE : used only for MTA 
      size_t      gc_signal;           // NOTE : used only for MTA 
   };

   // --- GCRoot ---
   struct GCRoot
   {
      size_t size;
      union
      {
         void*     stack_ptr;
         uintptr_t stack_ptr_addr;
      };
   };

   // --- ObjectPage ---
   struct ObjectPage32
   {
      uintptr_t vmtPtr;
      int       size;
      int       body[2];
   };

   struct ObjectPage64
   {
      uintptr_t vmtPtr;
      int       lock_flag;
      int       size;
      int       body[4];
   };

   // --- ExceptionStruct ---
   struct ExceptionStruct
   {
      uintptr_t prev_et_struct;
      uintptr_t core_catch_addr;
      uintptr_t core_catch_level;
      uintptr_t core_catch_frame;
   };

   // --- ThreadContent ---
   struct ThreadContent
   {
      uintptr_t        eh_critical;
      ExceptionStruct* eh_current;
      uintptr_t        tt_stack_frame;
      void*            tt_sync_event;
      size_t           tt_flags;
      uintptr_t        tt_stack_root;
   };

   // --- ThreadTable ---
   struct ThreadSlot
   {
      ThreadContent* content;
      void*          arg;
   };

   struct ThreadTable
   {
      size_t       counter;
      ThreadSlot   slots[0x200]; // it is the maximal size, in reality it is depend on project settings
   };

   // --- SystemEnv ---
   struct SystemEnv
   {
      size_t            stat_counter;
      GCTable*          gc_table;
      ThreadContent*    th_single_content;  // NOTE : used only for STA
      ThreadTable*      th_table;           // NOTE : used only for MTA 
      void*             bc_invoker;
      void*             veh_handler;
      pos_t             gc_mg_size;
      pos_t             gc_yg_size;
      pos_t             threadCounter;
   };

   constexpr int SizeOfExceptionStruct32 = 0x10;
   constexpr int SizeOfExceptionStruct64 = 0x20;

   // --- _Entry ---
   struct Entry
   {
      union {
         void* address;
         int  (*evaluate)(void*, void*);
      };

      Entry()
      {
         address = nullptr;
      }
   };

   // --- SymbolList ---
   struct SymbolList
   {
      size_t length;
      // NOTE : the array size is not valud - the actual number of entries defined in the first field
      Entry  entries[1];

      SymbolList()
      {
         length = 0;
      }
   };

   struct SeedStruct
   {
      int z1;
      int z2;
      int z3;
      int z4;
   };

#pragma pack(pop)

}

#endif
