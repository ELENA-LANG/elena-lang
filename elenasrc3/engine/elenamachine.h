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
      int      YGTotalSize;
      int      YGCommittedSize;
      int      MGTotalSize;
      int      MGCommittedSize;
      int      PageMask;
      int      PageSizeOrder;
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
      size_t      StatCounter;
      GCTable*    Table;
      void*       BCInvoker;
      pos_t       GCMGSize;
      pos_t       GCYGSize;
   };

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

      static void InitSTA(SystemEnv* env);

      static void Exit(int exitCode);

   } __routineProvider;

}

#endif