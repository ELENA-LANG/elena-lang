//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  x86 ELENA System Routines
//
//                                              (C)2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace _ELENA_;

const pos_t page_size = 0x10;
const pos_t page_size_order = 0x04;
const pos_t page_size_order_minus2 = 0x02;
const pos_t page_mask = 0x0FFFFFFF0;

void SystemRoutineProvider :: Init(SystemEnv* env)
{
#ifdef _WIN32
   // ; initialize fpu
   __asm {
      finit
   }
#endif

   // ; initialize static roots
 //  mov  ecx, [data : %CORE_STAT_COUNT]
 //  mov  edi, data : %CORE_STATICROOT
 //  shr  ecx, 2
 //  xor  eax, eax
 //  rep  stos
   memset(env->StatRoots, 0, env->StatLength);

   // set the GC root counter
   env->Table->gc_rootcount = env->StatLength;
   env->Table->gc_roots = (pos_t)env->StatRoots;

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

   env->Table->gc_signal = 0;
}

inline void entryCriticalSection(void* tt_lock)
{
#ifdef _WIN32
   __asm {
      mov  esi, tt_lock

      labWait :
      // ; set lock
      xor eax, eax
         mov  edx, 1
         lock cmpxchg dword ptr[esi], edx
         jnz  short labWait
   }
#endif
}

inline void leaveCriticalSection(void* tt_lock)
{
#ifdef _WIN32
   // ; free lock
   __asm {
      mov  esi, tt_lock

      // ; could we use mov [esi], 0 instead?
      mov  edx, -1
      lock xadd[esi], edx
   }
#endif
}

bool SystemRoutineProvider :: NewThread(SystemEnv* env, ProgramHeader* frameHeader)
{
   entryCriticalSection(&env->Table->tt_lock);

   bool valid = false;
   pos_t threadIndex = 0;
   for (pos_t i = 0; i < env->MaxThread; i++) {
      if (env->ThreadTable[i] == 0) {
         threadIndex = i;
         valid = true;
         break;
      }
   }

   if (valid) {
      InitTLSEntry(threadIndex, *env->TLSIndex, frameHeader, env->ThreadTable);
   }

   leaveCriticalSection(&env->Table->tt_lock);

   return valid;
}

void SystemRoutineProvider :: ExitThread(SystemEnv* env, pos_t exitCode, bool withExit)
{
   entryCriticalSection(&env->Table->tt_lock);

   TLSEntry* entry = GetTLSEntry(*env->TLSIndex);

   env->ThreadTable[entry->tls_threadindex] = 0;

   leaveCriticalSection(&env->Table->tt_lock);

   CloseThreadHandle(entry, withExit, exitCode);
}

int SystemRoutineProvider :: Execute(void* address, FrameHeader* framePtr)
{
#ifdef _WIN32
   int retVal = 0;
   int prevFrame = framePtr->previousFrame;
   int resrv = framePtr->reserved;

   __asm {
      mov  eax, address

      push esi
      push edi
      mov  esi, prevFrame
      mov  edi, resrv
      push ecx
      push ebx
      push ebp
      push esi
      push edi
      mov  ebp, esp
      call eax
      add  esp, 8
      pop  ebp
      pop  ebx
      pop  ecx
      pop  edi
      pop  esi

      mov  retVal, eax
   }

   return retVal;
#endif
}
