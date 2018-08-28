//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                              (C)2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace _ELENA_;

void SystemRoutineProvider :: Prepare()
{

}

// %CORE_STAT_COUNT, CORE_STATICROOT, CORE_GC_SIZE, CORE_GC_TABLE

void SystemRoutineProvider :: InitSTA(SystemEnv* env)
{

  //// ; initialize fpu
  //finit

  // ; initialize static roots
//  mov  ecx, [data : %CORE_STAT_COUNT]
//  mov  edi, data : %CORE_STATICROOT
//  shr  ecx, 2
//  xor  eax, eax
//  rep  stos
   memset(env->StatRoots, 0, env->StatLength << 2);

//  mov  ecx, 8000000h // ; 10000000h
//  mov  ebx, [data : %CORE_GC_SIZE]
//  and  ebx, 0FFFFFF80h     // ; align to 128
//  shr  ebx, page_size_order_minus2
//  call code : %NEW_HEAP
//  mov  [data : %CORE_GC_TABLE + gc_header], eax
//
//  mov  ecx, 20000000h // ; 40000000h
//  mov  ebx, [data : %CORE_GC_SIZE]
//  and  ebx, 0FFFFFF80h     // align to 128
//  call code : %NEW_HEAP
//  mov  [data : %CORE_GC_TABLE + gc_start], eax
//
//  // ; initialize yg
//  mov  [data : %CORE_GC_TABLE + gc_start], eax
//  mov  [data : %CORE_GC_TABLE + gc_yg_start], eax
//  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax
//
//  // ; initialize gc end
//  mov  ecx, [data : %CORE_GC_SIZE]
//  and  ecx, 0FFFFFF80h     // ; align to 128
//  add  ecx, eax
//  mov  [data : %CORE_GC_TABLE + gc_end], ecx
//
//  // ; initialize gc shadow
//  mov  ecx, [data : %CORE_GC_SIZE + gcs_YGSize]
//  and  ecx, page_mask
//  add  eax, ecx
//  mov  [data : %CORE_GC_TABLE + gc_yg_end], eax
//  mov  [data : %CORE_GC_TABLE + gc_shadow], eax
//
//  // ; initialize gc mg
//  add  eax, ecx
//  mov  [data : %CORE_GC_TABLE + gc_shadow_end], eax
//  mov  [data : %CORE_GC_TABLE + gc_mg_start], eax
//  mov  [data : %CORE_GC_TABLE + gc_mg_current], eax
//
//  // ; initialize wbar start
//  mov  edx, eax
//  sub  edx, [data : %CORE_GC_TABLE + gc_start]
//  shr  edx, page_size_order
//  add  edx, [data : %CORE_GC_TABLE + gc_header]
//  mov  [data : %CORE_GC_TABLE + gc_mg_wbar], edx

}

void SystemRoutineProvider :: NewFrame()
{

}

void SystemRoutineProvider :: Exit()
{

}
