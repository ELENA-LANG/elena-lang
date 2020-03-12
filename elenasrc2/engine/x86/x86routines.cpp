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
const pos_t page_ceil = 0x17;
const pos_t page_align_mask = 0x000FFFF0;

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

constexpr int elObjectOffset = 0x0008;

struct ObjectPage
{
   int   size;
   void* vmtPtr;
   int   body[2];
};

inline ObjectPage* getObjectPage(size_t objptr)
{
   return (ObjectPage*)objptr - elObjectOffset;
}

inline int getSize(size_t objptr)
{
   return getObjectPage(objptr)->size;
}

inline void* getVMTPtr(size_t objptr)
{
   return getObjectPage(objptr)->vmtPtr;
}

inline void setVMTPtr(size_t objptr, void* ptr)
{
   getObjectPage(objptr)->vmtPtr = ptr;
}

inline void orSize(size_t objptr, int mask)
{
   getObjectPage(objptr)->size |= mask;
}

inline void CopyObectData(size_t bytesToCopy, void* dst, void* src)
{
   bytesToCopy += 3;
   bytesToCopy &= 0xFFFFC;

#ifdef _WIN32
   __asm {
         mov  esi, src
         mov  edi, dst
         mov  ecx, bytesToCopy
      labYGCopyData :
         mov  eax, [esi]
         sub  ecx, 4
         mov[edi], eax
         lea  esi, [esi + 4]
         lea  edi, [edi + 4]
         jnz  short labYGCopyData
   }
#endif

}

inline void CollectYG(GCRoot* root, size_t start, size_t end, ObjectPage*& shadowHeap)
{
   size_t* ptr = (size_t*)root->stackPtr;
   size_t  size = root->size;
   size_t  new_ptr = 0;
   GCRoot  current;

   // ; collect roots
   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {
         current.stackPtrAddr = *ptr;

         // ; check if it was collected
         current.size = getSize((size_t)current.stackPtr);
         if (current.size >= 0) {
            // ; copy object size
            shadowHeap->size = current.size;

            // ; copy object vmt
            shadowHeap->vmtPtr = getVMTPtr(current.stackPtrAddr);

            // ; mark as collected
            orSize(current.stackPtrAddr, 0x80000000);

            // ; reserve shadow YG
            new_ptr = (size_t)shadowHeap + elObjectOffset;

            shadowHeap = (ObjectPage*)((size_t)shadowHeap + (current.size + page_ceil) & page_align_mask);

            // ; update reference 
            *ptr = new_ptr;

            // ; get object size
            current.size &= 0x8FFFFF;
            
            //      // ; save ESI
            //      push esi
            //      mov  esi, eax
            //
            // ; save new reference
            setVMTPtr(current.stackPtrAddr, (void*)new_ptr);
            
            if (current.size < 0x800000) {
               // ; check if the object has fields
               CollectYG(&current, start, end, shadowHeap);
            }
            else if (current.size != 0x800000) {
               CopyObectData(current.size, (void*)new_ptr, current.stackPtr);
            }
         }
         else {
            //   // ; update reference
            //   mov  edi, [eax - elVMTOffset]
            //   mov[esi], edi
            current.stackPtr = getVMTPtr(current.stackPtrAddr);
         }
      }
      ptr++;
      size -= 4;
   }
}

inline void FullCollect()
{
   throw InternalError("Not yet implemented"); // !! temporal

   //      labFullCollect :
   //   // ; ====== Major Collection ====
   //   // ; save the stack restore-point
   //   push ebp
   //
   //      // ; expand MG if required
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_mg_current]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_current]
   //      sub  edx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      cmp  ecx, edx
   //      ja   labSkipExpand
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_end]
   //      mov  ecx, 2A000h
   //      call code : % EXPAND_HEAP
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_header]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_start]
   //      shr  ecx, page_size_order_minus2
   //      add  eax, ecx
   //      mov  ecx, 0A800h
   //      call code : % EXPAND_HEAP
   //
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, 15000h
   //      mov[data:% CORE_GC_TABLE + gc_end], ecx
   //
   //      labSkipExpand :
   //
   //   // ; mark both yg and mg objects
   //   mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_mg_current]
   //
   //      // ; collect roots
   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //
   //      labMGCollectFrame:
   //   push eax
   //      call labCollectMG
   //      pop  eax
   //      lea  eax, [eax + 8]
   //      mov  esi, [eax + 4]
   //      test esi, esi
   //      mov  ecx, [eax]
   //      jnz short labMGCollectFrame
   //
   //      // ; compact mg
   //      mov  esi, [data:% CORE_GC_TABLE + gc_mg_start]
   //      mov  edi, esi
   //      sub  edi, [data:% CORE_GC_TABLE + gc_start]
   //      shr  edi, page_size_order_minus2
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //
   //      // ; skip the permanent part
   //      labMGSkipNext:
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labMGSkipEnd
   //      add  ecx, page_ceil
   //      mov  eax, esi
   //      and ecx, page_align_mask
   //      lea  eax, [eax + elObjectOffset]
   //      add  esi, ecx
   //      mov[edi], eax
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jb   short labMGSkipNext
   //      // ; !! undefined behaviour
   //      xor ecx, ecx
   //
   //      labMGSkipEnd :
   //   mov  ebp, esi
   //
   //      // ; compact	
   //      labMGCompactNext :
   //   add  ecx, page_ceil
   //      and ecx, page_align_mask
   //      add  esi, ecx
   //
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jae  short labMGCompactEnd
   //
   //      labMGCompactNext2 :
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labMGCompactNext
   //      add  ecx, page_ceil
   //      mov  eax, ebp
   //      and ecx, page_align_mask
   //      lea  eax, [eax + elObjectOffset]
   //      mov[edi], eax
   //      mov  eax, ecx
   //      shr  eax, page_size_order_minus2
   //      add  edi, eax
   //
   //      labMGCopy :
   //   mov  eax, [esi]
   //      mov[ebp], eax
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      lea  ebp, [ebp + 4]
   //      jnz  short labMGCopy
   //      cmp  esi, edx
   //      jb   short labMGCompactNext2
   //      labMGCompactEnd :
   //
   //   // ; promote yg
   //   mov  ebx, [data:% CORE_GC_TABLE + gc_end]
   //      mov  esi, [data:% CORE_GC_TABLE + gc_yg_start]
   //      sub  ebx, ebp
   //      mov  edi, esi
   //      sub  edi, [data:% CORE_GC_TABLE + gc_start]
   //      shr  edi, page_size_order_minus2
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_current]
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //      jmp  short labYGPromNext2
   //
   //      labYGPromNext :
   //   add  ecx, page_ceil
   //      and ecx, page_align_mask
   //      add  esi, ecx
   //      shr  ecx, page_size_order_minus2
   //      add  edi, ecx
   //      cmp  esi, edx
   //      jae  short labYGPromEnd
   //      labYGPromNext2 :
   //   mov  ecx, [esi]
   //      test ecx, ecx
   //      jns  short labYGPromNext
   //      add  ecx, page_ceil
   //      mov  eax, ebp
   //      and ecx, page_align_mask
   //      // ; raise an exception if it is not enough memory to promote object
   //      lea  eax, [eax + elObjectOffset]
   //      sub  ebx, ecx
   //      js   short labError
   //      mov[edi], eax
   //      mov  eax, ecx
   //      shr  eax, page_size_order_minus2
   //      add  edi, eax
   //      labYGProm :
   //   mov  eax, [esi]
   //      sub  ecx, 4
   //      mov[ebp], eax
   //      lea  esi, [esi + 4]
   //      lea  ebp, [ebp + 4]
   //      jnz  short labYGProm
   //      cmp  esi, edx
   //      jb   short labYGPromNext2
   //      labYGPromEnd :
   //
   //   // ; get previous heap end
   //   mov  edx, [data:% CORE_GC_TABLE + gc_mg_current]
   //
   //      // ; set mg_current, clear yg and survive
   //      mov[data:% CORE_GC_TABLE + gc_mg_current], ebp
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], eax
   //
   //      // ; fix roots
   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  ebp, [data:% CORE_GC_TABLE + gc_start]
   //
   //      labFixRoot:
   //   push eax
   //      call labFixObject
   //      pop  eax
   //      lea  eax, [eax + 8]
   //      mov  esi, [eax + 4]
   //      test esi, esi
   //      mov  ecx, [eax]
   //      jnz  short labFixRoot
   //
   //      // ; clear WBar
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      mov  edi, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_mg_start]
   //      xor eax, eax
   //      shr  ecx, page_size_order
   //      rep  stos
   //
   //      // ; free root set
   //      mov  esp, [esp]
   //      pop  ecx
   //      pop  ebp
   //
   //      // ; allocate
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_current]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_end]
   //      add  ecx, eax
   //      cmp  ecx, edx
   //      jae  labBigAlloc
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ecx
   //      lea  ebx, [eax + elObjectOffset]
   //      ret
   //
   //      labError :
   //   // ; restore stack
   //   mov  esp, [esp]
   //      pop  ecx
   //      pop  ebp
   //
   //      labError2 :
   //   mov  ebx, 17h
   //      call code : % BREAK
   //      ret
   //
   //      // ; bad luck, we have to expand GC
   //      labBigAlloc2 :
   //   push ecx
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_end]
   //      mov  ecx, 2A000h
   //      call code : % EXPAND_HEAP
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_header]
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_start]
   //      shr  ecx, page_size_order_minus2
   //      add  eax, ecx
   //      mov  ecx, 0A800h
   //      call code : % EXPAND_HEAP
   //
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, 15000h
   //      mov[data:% CORE_GC_TABLE + gc_end], ecx
   //
   //      pop  ecx
   //
   //      labBigAlloc :
   //   // ; try to allocate in the mg
   //   sub  ecx, eax
   //      cmp  ecx, 800000h
   //      jae  labError2
   //
   //      mov  eax, [data:% CORE_GC_TABLE + gc_mg_current]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_end]
   //      add  ecx, eax
   //      cmp  ecx, edx
   //      jae  labBigAlloc2
   //      mov[data:% CORE_GC_TABLE + gc_mg_current], ecx
   //      lea  ebx, [eax + elObjectOffset]
   //
   //      mov  ecx, ebx
   //      mov  esi, [data:% CORE_GC_TABLE + gc_header]
   //      sub  ecx, [data:% CORE_GC_TABLE + gc_start]
   //      shr  ecx, page_size_order
   //      mov  byte ptr[ecx + esi], 1
   //
   //      ret
   //



   //      // ---- start collecting: esi => ebp, [ebx, edx] ; ecx - count ---
   //      labCollectMG :
   //
   //   lea  ecx, [ecx + 4]
   //      push 0
   //      lea  esi, [esi - 4]
   //      push 0
   //
   //      labMGNext :
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      jz   short labMGResume
   //
   //      labMGCheck :
   //   mov  eax, [esi]
   //
   //      // ; check if it valid reference
   //      mov  edi, eax
   //      cmp  edi, ebx
   //      setb al
   //      cmp  edx, edi
   //      setb ah
   //      test eax, 0FFFFh
   //      mov  eax, edi
   //      jnz  labMGNext
   //
   //      // ; check if it was collected
   //      mov  edi, [eax - elSizeOffset]
   //      test edi, edi
   //      js   short labMGNext
   //
   //      // ; mark as collected
   //      or [eax - elSizeOffset], 080000000h
   //
   //      cmp  edi, 0800000h
   //      jae  short labMGNext
   //
   //      // ; save previous ecx field
   //      push ecx
   //
   //      // ; get object size
   //      mov  ecx, [eax - elSizeOffset]
   //
   //      // ; save ESI
   //      push esi
   //      and ecx, 0FFFFFh
   //
   //      mov  esi, eax
   //
   //      // ; collect object fields if it has them
   //      jmp   short labMGCheck
   //
   //      labMGResume :
   //   pop  esi
   //      pop  ecx
   //      test esi, esi
   //      jnz  short labMGNext
   //
   //      nop
   //      labMGEnd :
   //   ret
   //
   //      labFixObject :
   //
   //   lea  ecx, [ecx + 4]
   //      push 0
   //      lea  esi, [esi - 4]
   //      push 0
   //
   //      labFixNext :
   //      sub  ecx, 4
   //      lea  esi, [esi + 4]
   //      jz   short labFixResume
   //
   //      labFixCheck :
   //   mov  eax, [esi]
   //
   //      // ; check if it valid reference
   //      mov  edi, eax
   //      cmp  edi, ebx
   //      setb al
   //      cmp  edx, edi
   //      setb ah
   //      test eax, 0FFFFh
   //      mov  eax, edi
   //      jnz  labFixNext
   //
   //      lea  edi, [eax - elObjectOffset]
   //
   //      sub  edi, ebp
   //      shr  edi, page_size_order_minus2
   //      add  edi, [data:% CORE_GC_TABLE + gc_header]
   //
   //      mov  eax, [edi]
   //      mov[esi], eax
   //
   //      // ; make sure the object was not already fixed
   //      mov  edi, [eax - elSizeOffset]
   //      test edi, edi
   //      jns  short labFixNext
   //
   //      and edi, 7FFFFFFFh
   //      mov[eax - elSizeOffset], edi
   //
   //      cmp  edi, 0800000h
   //      jae  short labFixNext
   //
   //      // ; save previous ecx field
   //      push ecx
   //
   //      // ; get object size
   //      mov  ecx, [eax - elSizeOffset]
   //      and ecx, 0FFFFFh
   //
   //      // ; save ESI
   //      push esi
   //      mov  esi, eax
   //
   //      // ; collect object fields if it has them
   //      jmp   short labFixCheck
   //
   //      labFixResume :
   //   pop  esi
   //      pop  ecx
   //      test esi, esi
   //      jnz  short labFixNext
   //      nop
   //
   //      ret
   //      push ecx
   //      push ecx
   //
   //      // ; save static roots
   //      mov  esi, data :% CORE_STATICROOT
   //      mov  ecx, [data:% CORE_GC_TABLE + gc_rootcount]
   //      push esi
   //      push ecx
   //
   //      // ; collect frames
   //      mov  eax, [data:% CORE_GC_TABLE + gc_stack_frame]
   //      mov  ecx, eax
   //
   //      labYGNextFrame :
   //   mov  esi, eax
   //      mov  eax, [esi]
   //      test eax, eax
   //      jnz  short labYGNextFrame
   //
   //      push ecx
   //      sub  ecx, esi
   //      neg  ecx
   //      push ecx
   //
   //      mov  eax, [esi + 4]
   //      test eax, eax
   //      mov  ecx, eax
   //      jnz  short labYGNextFrame
   //
   //      // === Minor collection ===
   //      mov[ebp - 4], esp      // ; save position for roots
   //
   //      // ; save mg -> yg roots 
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_mg_current]
   //      mov  edi, [data:% CORE_GC_TABLE + gc_mg_start]
   //      sub  ebx, edi                                        // ; we need to check only MG region
   //      jz   labWBEnd                                        // ; skip if it is zero
   //      mov  esi, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //      shr  ebx, page_size_order
   //      // ; lea  edi, [edi + elObjectOffset]
   //
   //      labWBNext :
   //   cmp[esi], 0
   //      lea  esi, [esi + 4]
   //      jnz  short labWBMark
   //      sub  ebx, 4
   //      ja   short labWBNext
   //      nop
   //      nop
   //      jmp  short labWBEnd
   //
   //      labWBMark :
   //   lea  eax, [esi - 4]
   //      sub  eax, [data:% CORE_GC_TABLE + gc_mg_wbar]
   //      mov  edx, [esi - 4]
   //      shl  eax, page_size_order
   //      lea  eax, [edi + eax + elObjectOffset]
   //
   //      test edx, 0FFh
   //      jz   short labWBMark2
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //
   //      labWBMark2 :
   //   lea  eax, [eax + page_size]
   //      test edx, 0FF00h
   //      jz   short labWBMark3
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //
   //      labWBMark3 :
   //   lea  eax, [eax + page_size]
   //      test edx, 0FF0000h
   //      jz   short labWBMark4
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //
   //      labWBMark4 :
   //   lea  eax, [eax + page_size]
   //      test edx, 0FF000000h
   //      jz   short labWBNext
   //      mov  ecx, [eax - elSizeOffset]
   //      push eax
   //      and ecx, 0FFFFFh
   //      push ecx
   //      jmp  short labWBNext
   //
   //      labWBEnd :
   //   // ; save the stack restore-point
   //   push ebp



   //      // ; init registers
   //      mov  ebx, [data:% CORE_GC_TABLE + gc_yg_start]
   //      mov  edx, [data:% CORE_GC_TABLE + gc_yg_end]
   //      mov  ebp, [data:% CORE_GC_TABLE + gc_shadow]
   //
   //      
   //      lea  eax, [esp + 4]
   //      mov  ecx, [eax]
   //      mov  esi, [esp + 8]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ebp
   //
   //      labCollectFrame :
   //   push eax

}

void SystemRoutineProvider::GCRoutine(GCTable* table, GCRoot* roots, size_t size)
{
   //labYGCollect:
   //   // ; restore ecx
   //   sub  ecx, eax
   //
   //      // ; save registers
   //      push ebp
   //
   //      // ; lock frame
   //      mov[data:% CORE_GC_TABLE + gc_stack_frame], esp
   //
   //      push ecx
   //
   //      // ; create set of roots
   //      mov  ebp, esp
   //      xor ecx, ecx
   //      push ecx        // ; reserve place      

   ObjectPage* shadowPtr = (ObjectPage*)table->gc_shadow;
   while (roots->stackPtr) {
      CollectYG(roots, table->gc_yg_start, table->gc_yg_end, shadowPtr);

      roots++;
   }

   // ; save gc_yg_current to mark  objects
   table->gc_yg_current = (pos_t)shadowPtr;

   // ; switch main YG heap with a shadow one
   pos_t tmp = table->gc_yg_start;
   table->gc_yg_start = table->gc_shadow;
   table->gc_shadow = tmp;
   tmp = table->gc_yg_end;
   table->gc_yg_end = table->gc_shadow_end;
   table->gc_shadow_end = tmp;

   if (table->gc_yg_end - table->gc_yg_current < size) {
      FullCollect();
   }
   //
   //      // ; free root set
   //      mov  esp, ebp
   //
   //      // ; restore registers
   //      pop  ecx
   //      pop  ebp
   //
   //      // ; try to allocate once again
   //      mov  eax, [data:% CORE_GC_TABLE + gc_yg_current]
   //      add  ecx, eax
   //      lea  ebx, [eax + elObjectOffset]
   //      mov[data:% CORE_GC_TABLE + gc_yg_current], ecx
   //      ret
   //
}

