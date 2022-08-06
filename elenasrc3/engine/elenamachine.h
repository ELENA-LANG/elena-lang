//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAMACHINE_H
#define ELENAMACHINE_H

namespace elena_lang
{
   // --- SystemSettings ---
   struct SystemSettings
   {
      int      yg_total_size;
      int      yg_committed_size;
      int      mg_total_size;
      int      mg_committed_size;
      int      page_mask;
      int      page_size_order;
   };

#pragma pack(push, 1)

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
   };

   // --- SystemEnv ---
   struct SystemEnv
   {
      size_t      stat_counter;
      GCTable*    table;
      void*       bc_invoker;
      void*       ex_handler;
      pos_t       gc_mg_size;
      pos_t       gc_yg_size;
   };

   // --- ExceptionStruct ---
   struct ExceptionStruct
   {
      uintptr_t core_catch_addr;
      uintptr_t core_catch_level;
      uintptr_t core_catch_frame;
   };

   constexpr int SizeOfExceptionStruct32 = 0x0C;
   constexpr int SizeOfExceptionStruct64 = 0x20;

   // --- _Entry ---
   struct Entry
   {
      union {
         void* address;
         int  (*evaluate)(void*, int);
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
      // NOTE : the array size if fictinal - it can contain the number of entried defined in the first field
      Entry  entries[1];  

      SymbolList()
      {
         length = 0;
      }
   };

#pragma pack(pop)

   // --- SystemRoutineProvider ---
   static class SystemRoutineProvider
   {
   public:
      static void FillSettings(SystemEnv* env, SystemSettings& settings);

      static uintptr_t NewHeap(int totalSize, int committedSize);

      static void Init(SystemEnv* env, SystemSettings settings);
      static void InitExceptionHandling(SystemEnv* env, ExceptionStruct* ex_struct);
      static void InitCriticalStruct(uintptr_t criticalHandler);

      static void InitSTA(SystemEnv* env);

      static void Exit(int exitCode);

   } __routineProvider;

}

#endif