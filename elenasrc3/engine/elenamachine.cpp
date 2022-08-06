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

void SystemRoutineProvider :: Init(SystemEnv* env, SystemSettings settings)
{
   int page_mask = settings.page_mask;

   // ; allocate memory heap
   env->table->gc_header = NewHeap(settings.yg_total_size,  settings.yg_committed_size);

   uintptr_t mg_ptr = NewHeap(settings.mg_total_size, settings.mg_committed_size);
   env->table->gc_start = mg_ptr;

   // ; initialize yg
   env->table->gc_yg_current = env->table->gc_yg_start = mg_ptr;

   // ; initialize gc end
   env->table->gc_end = mg_ptr + align(env->gc_mg_size, 128);

   // ; initialize gc shadow
   mg_ptr += (env->gc_yg_size & page_mask);
   env->table->gc_shadow = env->table->gc_yg_end = mg_ptr;

   // ; initialize gc mg
   mg_ptr += (env->gc_yg_size & page_mask);
   env->table->gc_shadow_end = env->table->gc_mg_start = env->table->gc_mg_current = mg_ptr;

   // ; initialize wbar start
   env->table->gc_mg_wbar = ((mg_ptr - env->table->gc_start) >> settings.page_size_order) + env->table->gc_header;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env)
{
   SystemSettings settings;
   FillSettings(env, settings);

   Init(env, settings);
}
