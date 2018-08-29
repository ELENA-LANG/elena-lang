//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                              (C)2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>

using namespace _ELENA_;

const pos_t page_size              = 0x10;
const pos_t page_size_order        = 0x04;
const pos_t page_size_order_minus2 = 0x02;
const pos_t page_mask              = 0x0FFFFFFF0;

void SystemRoutineProvider :: Prepare()
{

}

inline pos_t NewHeap(int totalSize, int committedSize)
{
   // reserve
   void* allocPtr = VirtualAlloc(nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE);

   // allocate
   VirtualAlloc(allocPtr, committedSize, MEM_COMMIT, PAGE_READWRITE);

   return (pos_t)allocPtr;
}

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
              
   // ; allocate memory heap
//  mov  ecx, 8000000h // ; 10000000h
//  mov  ebx, [data : %CORE_GC_SIZE]
//  and  ebx, 0FFFFFF80h     // ; align to 128
//  shr  ebx, page_size_order_minus2
//  call code : %NEW_HEAP
//  mov  [data : %CORE_GC_TABLE + gc_header], eax
   env->Table->gc_header = NewHeap(0x8000000, align(env->GCMGSize, 128) >> page_size_order_minus2);

//  mov  ecx, 20000000h // ; 40000000h
//  mov  ebx, [data : %CORE_GC_SIZE]
//  and  ebx, 0FFFFFF80h     // align to 128
//  call code : %NEW_HEAP
//  mov  [data : %CORE_GC_TABLE + gc_start], eax
   pos_t mg_ptr = NewHeap(0x20000000, align(env->GCMGSize, 128));
   env->Table->gc_start = mg_ptr;

  // ; initialize yg
//  mov  [data : %CORE_GC_TABLE + gc_yg_start], eax
//  mov  [data : %CORE_GC_TABLE + gc_yg_current], eax
   env->Table->gc_yg_current = env->Table->gc_yg_start = mg_ptr;
   
  // ; initialize gc end
//  mov  ecx, [data : %CORE_GC_SIZE]
//  and  ecx, 0FFFFFF80h     // ; align to 128
//  add  ecx, eax
//  mov  [data : %CORE_GC_TABLE + gc_end], ecx
   env->Table->gc_end = mg_ptr + align(env->GCMGSize, 128);

  // ; initialize gc shadow
//  mov  ecx, [data : %CORE_GC_SIZE + gcs_YGSize]
//  and  ecx, page_mask
//  add  eax, ecx
//  mov  [data : %CORE_GC_TABLE + gc_yg_end], eax
//  mov  [data : %CORE_GC_TABLE + gc_shadow], eax
   mg_ptr += (env->GCYGSize & page_mask);
   env->Table->gc_shadow = env->Table->gc_yg_end = mg_ptr;

  // ; initialize gc mg
//  add  eax, ecx
//  mov  [data : %CORE_GC_TABLE + gc_shadow_end], eax
//  mov  [data : %CORE_GC_TABLE + gc_mg_start], eax
//  mov  [data : %CORE_GC_TABLE + gc_mg_current], eax
   mg_ptr += (env->GCYGSize & page_mask);
   env->Table->gc_shadow_end = env->Table->gc_mg_start = env->Table->gc_mg_current = mg_ptr;

  // ; initialize wbar start
//  mov  edx, eax
//  sub  edx, [data : %CORE_GC_TABLE + gc_start]
//  shr  edx, page_size_order
//  add  edx, [data : %CORE_GC_TABLE + gc_header]
//  mov  [data : %CORE_GC_TABLE + gc_mg_wbar], edx
   env->Table->gc_mg_wbar = ((mg_ptr - env->Table->gc_start) >> page_size_order) + env->Table->gc_header;
}

void SystemRoutineProvider :: NewFrame()
{
  //// ; put frame end and move procedure returning address
  //pop  edx           

  //// ; set SEH handler
  //mov  ebx, code : "$native'coreapi'seh_handler"
  //push ebx
  //mov  ecx, fs:[0]
  //push ecx
  //mov  fs:[0], esp

  //xor  ebx, ebx

  //push ebp
  //push ebx                      
  //push ebx

  //// ; set stack frame pointer / bottom stack pointer
  //mov  ebp, esp 
  //mov  [data : %CORE_GC_TABLE + gc_stack_bottom], esp
  //
  //push edx
  //mov  [data : %CORE_GC_TABLE + gc_ext_stack_frame], ebx

  //mov  ebx, code : "$native'coreapi'default_handler"
  //call code : % INIT_ET

  //ret

}

void SystemRoutineProvider :: Exit()
{

}
