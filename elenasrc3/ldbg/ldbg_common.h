//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains LDBG comment declaration
//
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LDBG_COMMON
#define LDBG_COMMON

#if defined(_MSC_VER)

#include "windows\winevents.h"

#else

#endif

namespace elena_lang
{
   constexpr auto LDBG_EVENT_COUNT = 1;
   constexpr auto LDBF_CONFIGURED = 0;

   typedef EventManager<int, LDBG_EVENT_COUNT> DPAEventManager;
}

#endif // LDBG_COMMON
