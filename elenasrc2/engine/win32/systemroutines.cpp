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

inline void Init(SystemEnv* env)
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

   env->Table->gc_signal = 0 ;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env, FrameHeader* frameHeader)
{
   Init(env);

   // setting current exception handler (only for STA)
   env->Table->gc_et_current = &frameHeader->root_exception_struct;
   env->Table->gc_stack_frame = 0;
}

void SystemRoutineProvider :: InitMTA(SystemEnv* env, FrameHeader* frameHeader)
{
   Init(env);
   InitTLSEntry(*env->TLSIndex, frameHeader, env->ThreadTable);

   // set the thread table size
   env->ThreadTable[-1] = env->MaxThread;
}

void SystemRoutineProvider :: InitTLSEntry(pos_t tlsIndex, FrameHeader* frameHeader, pos_t* threadTable)
{
   TLSEntry* entry = nullptr;

   // ; GCXT: assign tls entry
   __asm {
      mov  ebx, tlsIndex
      mov  ecx, fs:[2Ch]
      mov  edx, [ecx + ebx * 4]
      mov  entry, edx
   }

   entry->tls_flags = 0;
   entry->tls_sync_event = ::CreateEvent(0, -1, 0, 0);
   entry->tls_et_current =  &frameHeader->root_exception_struct;

   threadTable[0] = (pos_t)entry;
}

void SystemRoutineProvider :: InitCriticalStruct(CriticalStruct* header, pos_t criticalHandler)
{
   pos_t previousHeader = 0;

   // ; set SEH handler / frame / stack pointers
   __asm {
      mov  eax, header
      mov  ecx, fs:[0]
      mov  previousHeader, ecx
      mov  fs : [0], eax
   }

   header->previousStruct = previousHeader;
   header->handler = criticalHandler;
}

bool SystemRoutineProvider :: NewThread(SystemEnv* env)
{
   //   push eax

   //   // ; GCXT
   //   mov  edx, data : %THREAD_TABLE
   //   mov  esi, data : %CORE_GC_TABLE + tt_lock
   //   mov  ecx, [edx - 4]

   void* tt_ptr = &env->Table->tt_ptr;
   __asm {
      mov  esi, tt_ptr

      labWait :
      // ; set lock
      xor  eax, eax
      mov  edx, 1
      lock cmpxchg dword ptr[esi], edx
      jnz  short labWait
   }

   bool valid = false;
   if (env->Table->tt_ptr != 0) {
      env->Table->tt_ptr--;

      valid = true;
   }
   
   // ; free lock
   __asm {
      mov  esi, tt_ptr

      // ; could we use mov [esi], 0 instead?
      mov  edx, -1
      lock xadd[esi], edx
   }

   if (valid) {
      //   sub  ebx, 1
      //   mov  esi, data : %THREAD_TABLE
      //   lea  esi, [esi + ebx * 4]

      //   // ; assign tls entry
      //   mov  ebx, [data:%CORE_TLS_INDEX]
      //   push 0

      //   mov  eax, fs:[2Ch]
      //   push 0

      //   mov  eax, [eax + ebx * 4]
      //   push 0FFFFFFFFh //-1

      //   mov[esi], eax               // ; save tls entry
      //   push 0
      //   mov  esi, eax

      //   call extern 'dlls'kernel32.CreateEventW

      //   // ; initialize thread entry
      //   mov[esi + tls_sync_event], eax     // ; set thread event handle

      //   mov[esi + tls_flags], 0              // ; init thread flags  

      //   pop  eax
      //   pop  edx                             // ; put frame end and move procedure returning address

      //   xor  ebx, ebx
      //   push ebp
      //   push ebx
      //   push ebx

      //   // ; set stack frame pointer  
      //   // ; mov  ebp, esp 
      //   // ; mov  [esi + tls_stack_frame], ebx
      //   // ; mov  [esi + tls_stack_bottom], esp

      //   // ; restore return pointer
      //   push edx

      //   mov  ebx, code : "$native'coreapi'core_thread_handler"
      //   //;  call code : % INIT_ET

      //   ret

      //   lErr :
      //   add esp, 4
      //   ret
   }

   return valid;
}

void SystemRoutineProvider :: Exit(pos_t exitCode)
{
   ExitProcess(exitCode);
}
