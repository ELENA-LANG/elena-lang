//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  PPC64le ELENA System Routines
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace elena_lang;

constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_64;
constexpr int page_mask = gcPageMask64;
constexpr int page_size_order = gcPageSizeOrder64;

void SystemRoutineProvider::FillSettings(SystemEnv* env, SystemSettings& settings)
{
   settings.yg_total_size = 0x8000000;
   settings.yg_committed_ize = align(env->gc_mg_size, 128) >> page_size_order_minus2;
   settings.mg_total_size = 0x20000000;
   settings.mg_committed_size = align(env->gc_mg_size, 128);

   settings.page_mask = page_mask;
   settings.page_size_order = page_size_order;
}