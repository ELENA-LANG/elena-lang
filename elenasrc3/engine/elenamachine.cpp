//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common routines implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace elena_lang;

// --- SystemRoutineProvider ---

void SystemRoutineProvider :: InitExceptionHandling(SystemEnv* env, void* criticalHandler)
{
   // inti App criticaL handler
   env->eh_table->eh_critical = (uintptr_t)criticalHandler;

   // inti OS criticaL handler
   InitCriticalStruct((uintptr_t)env->veh_handler);
}

void SystemRoutineProvider :: Init(SystemEnv* env, SystemSettings settings)
{
   int page_mask = settings.page_mask;

   // ; allocate memory heap
   env->gc_table->gc_header = NewHeap(settings.yg_total_size,  settings.yg_committed_size);

   uintptr_t mg_ptr = NewHeap(settings.mg_total_size, settings.mg_committed_size);
   env->gc_table->gc_start = mg_ptr;

   // ; initialize yg
   env->gc_table->gc_yg_current = env->gc_table->gc_yg_start = mg_ptr;

   // ; initialize gc end
   env->gc_table->gc_end = mg_ptr + align(env->gc_mg_size, 128);

   // ; initialize gc shadow
   mg_ptr += (env->gc_yg_size & page_mask);
   env->gc_table->gc_shadow = env->gc_table->gc_yg_end = mg_ptr;

   // ; initialize gc mg
   mg_ptr += (env->gc_yg_size & page_mask);
   env->gc_table->gc_shadow_end = env->gc_table->gc_mg_start = env->gc_table->gc_mg_current = mg_ptr;

   // ; initialize wbar start
   env->gc_table->gc_mg_wbar = ((mg_ptr - env->gc_table->gc_start) >> settings.page_size_order) + env->gc_table->gc_header;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env)
{
   SystemSettings settings;
   FillSettings(env, settings);

   Init(env, settings);
}

// --- ELENAMachine ---

int ELENAMachine :: execute(SystemEnv* env, void* symbolListEntry)
{
   Entry entry;
   entry.address = env->bc_invoker;

   // executing the program

   int retVal = 0;
   try
   {
      retVal = entry.evaluate(symbolListEntry, 0);
   }
   catch (InternalError&)
   {
      //_instance->printInfo("EAbortException");

      retVal = -1;
   }

   return retVal;
}
