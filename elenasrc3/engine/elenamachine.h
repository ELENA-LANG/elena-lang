//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAMACHINE_H
#define ELENAMACHINE_H

#include "core.h"

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

   // --- SystemRoutineProvider ---
   static class SystemRoutineProvider
   {
   public:
      static void FillSettings(SystemEnv* env, SystemSettings& settings);

      static size_t AlignHeapSize(size_t size);

      static uintptr_t NewHeap(size_t totalSize, size_t committedSize);
      static uintptr_t ExpandHeap(void* allocPtr, size_t newSize);

      static void* GCRoutine(GCTable* table, GCRoot* roots, size_t size);

      static void Init(SystemEnv* env, SystemSettings settings);
      static void InitExceptionHandling(SystemEnv* env, void* criticalHandler);
      static void InitCriticalStruct(uintptr_t criticalDispatcher);
      static void InitSTA(SystemEnv* env);

      static void RaiseError(int code);

      static void Exit(int exitCode);

   } __routineProvider;

}

#endif