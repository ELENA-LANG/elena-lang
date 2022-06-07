//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Amd64 ELENA System Routines
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace elena_lang;

constexpr int page_size_order_minus2   = gcPageSizeOrderMinus2_64;
constexpr int page_mask                = gcPageMask64;
constexpr int page_size_order          = gcPageSizeOrder64;

void SystemRoutineProvider :: FillSettings(SystemEnv* env, SystemSettings& settings)
{
   settings.YGTotalSize = 0x8000000;
   settings.YGCommittedSize = align(env->GCMGSize, 128) >> page_size_order_minus2;
   settings.MGTotalSize = 0x20000000;
   settings.MGCommittedSize = align(env->GCMGSize, 128);

   settings.PageMask = page_mask;
   settings.PageSizeOrder = page_size_order;
}