//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  x86 ELENA System Routines
//
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace elena_lang;

constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_32;
constexpr int page_mask              = gcPageMask32;
constexpr int page_size_order        = gcPageSizeOrder32;

void SystemRoutineProvider :: FillSettings(SystemEnv* env, SystemSettings& settings)
{
   settings.yg_total_size = 0x8000000;
   settings.yg_committed_size = align(env->gc_mg_size, 128) >> page_size_order_minus2;
   settings.mg_total_size = 0x20000000;
   settings.mg_committed_size = align(env->gc_mg_size, 128);

   settings.page_mask = page_mask;
   settings.page_size_order = page_size_order;

   settings.perm_total_size = 0x20000000;
}