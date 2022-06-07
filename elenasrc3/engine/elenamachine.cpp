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
   int page_mask = settings.PageMask;

   // ; allocate memory heap
   env->Table->gc_header = NewHeap(settings.YGTotalSize,  settings.YGCommittedSize);

   uintptr_t mg_ptr = NewHeap(settings.MGTotalSize, settings.MGCommittedSize);
   env->Table->gc_start = mg_ptr;

   // ; initialize yg
   env->Table->gc_yg_current = env->Table->gc_yg_start = mg_ptr;

   // ; initialize gc end
   env->Table->gc_end = mg_ptr + align(env->GCMGSize, 128);

   // ; initialize gc shadow
   mg_ptr += (env->GCYGSize & page_mask);
   env->Table->gc_shadow = env->Table->gc_yg_end = mg_ptr;

   // ; initialize gc mg
   mg_ptr += (env->GCYGSize & page_mask);
   env->Table->gc_shadow_end = env->Table->gc_mg_start = env->Table->gc_mg_current = mg_ptr;

   // ; initialize wbar start
   env->Table->gc_mg_wbar = ((mg_ptr - env->Table->gc_start) >> settings.PageSizeOrder) + env->Table->gc_header;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env)
{
   SystemSettings settings;
   FillSettings(env, settings);

   Init(env, settings);
}
