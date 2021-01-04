//--------------------------------------NO-----------------------------------
//		E L E N A   P r o j e c t:  x86 ELENA System Routines
//
//                                              (C)2020-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace _ELENA_;

constexpr int page_size = gcPageSize32;
constexpr int page_size_order = gcPageSizeOrder32;
constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_32;
constexpr int page_ceil = gcPageCeil32;
constexpr int page_mask = gcPageMask32;
constexpr int struct_mask = elStructMask32;

const pos_t page_align_mask = 0x000FFFF0; // !! temp

void SystemRoutineProvider :: Init(SystemEnv* env)
{
   // ; initialize static roots
 //  mov  ecx, [data : %CORE_STAT_COUNT]
 //  mov  edi, data : %CORE_STATICROOT
 //  shr  ecx, 2
 //  xor  eax, eax
 //  rep  stos
   memset(env->StatRoots, 0, env->StatLength);

   // set the GC root counter
   env->Table->gc_rootcount = env->StatLength;
   env->Table->gc_roots = (uintptr_t)env->StatRoots;

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
   uintptr_t mg_ptr = NewHeap(0x20000000, align(env->GCMGSize, 128));
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

inline void __vectorcall entryCriticalSection(void* tt_lock)
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

inline void __vectorcall leaveCriticalSection(void* tt_lock)
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

constexpr int elObjectOffset = 0x0008;

struct ObjectPage
{
   void* vmtPtr;
   int   size;
   int   body[2];
};

inline uintptr_t __vectorcall getObjectPtr(uintptr_t pagePtr)
{
   return pagePtr + elObjectOffset;
}

inline ObjectPage* __vectorcall getObjectPage(uintptr_t objptr)
{
   return (ObjectPage*)(objptr - elObjectOffset);
}

inline int __vectorcall getSize(uintptr_t objptr)
{
   return getObjectPage(objptr)->size;
}

inline void* __vectorcall getVMTPtr(uintptr_t objptr)
{
   return getObjectPage(objptr)->vmtPtr;
}

inline void __vectorcall setVMTPtr(uintptr_t objptr, void* ptr)
{
   getObjectPage(objptr)->vmtPtr = ptr;
}

inline void __vectorcall orSize(uintptr_t objptr, int mask)
{
   getObjectPage(objptr)->size |= mask;
}

inline void __vectorcall CopyObject(size_t bytesToCopy, void* dst, void* src)
{
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

inline void __vectorcall CopyObjectData(size_t bytesToCopy, void* dst, void* src)
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

void __vectorcall YGCollect(GCRoot* root, size_t start, size_t end, ObjectPage*& shadowHeap)
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
         if (!(current.size & 0x80000000)) {
            // ; copy object size
            shadowHeap->size = current.size;

            // ; copy object vmt
            shadowHeap->vmtPtr = getVMTPtr(current.stackPtrAddr);

            // ; mark as collected
            orSize(current.stackPtrAddr, 0x80000000);

            // ; reserve shadow YG
            new_ptr = (size_t)shadowHeap + elObjectOffset;

            shadowHeap = (ObjectPage*)((size_t)shadowHeap + ((current.size + page_ceil) & page_align_mask));

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
               YGCollect(&current, start, end, shadowHeap);
            }
            else if (current.size != 0x800000) {
               CopyObjectData(current.size, (void*)new_ptr, current.stackPtr);
            }
         }
         else {
            //   // ; update reference
            //   mov  edi, [eax - elVMTOffset]
            //   mov[esi], edi
            *ptr = (size_t)getVMTPtr(current.stackPtrAddr);
         }
      }
      ptr++;
      size -= 4;
   }
}

void __vectorcall MGCollect(GCRoot* root, size_t start, size_t end)
{
   size_t* ptr = (size_t*)root->stackPtr;
   size_t  size = root->size;

   GCRoot  current;

   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {
         current.stackPtrAddr = *ptr;

         // ; check if it was collected
         current.size = getSize((size_t)current.stackPtr);
         if (!(current.size & 0x80000000)) {
            // ; mark as collected
            // or [eax - elSizeOffset], 080000000h
            orSize(current.stackPtrAddr, 0x80000000);

            // ; check if the object has fields
            if (current.size < 0x800000) {
               MGCollect(&current, start, end);
            }
         }
      }
      ptr++;
      size -= 4;
   }
}

inline void __vectorcall FixObject(GCTable* table, GCRoot* roots, size_t start, size_t end)
{
   size_t* ptr = (size_t*)roots->stackPtr;
   size_t  size = roots->size;

   GCRoot  current;

   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {

         ObjectPage* pagePtr = getObjectPage((uintptr_t)(*ptr));
         uintptr_t mappings = table->gc_header + (((uintptr_t)pagePtr - table->gc_start) << page_size_order_minus2);

         *ptr = *(uintptr_t*)mappings;

         // ; make sure the object was not already fixed
         if (pagePtr->size & 0x80000000) {
            pagePtr->size = pagePtr->size & 0x7FFFFFFF;

            if (!test(pagePtr->size, struct_mask)) {
               current.stackPtrAddr = *ptr;
               current.size = pagePtr->size;

               FixObject(table, &current, start, end);
            }
         }
      }
      ptr++;
      size -= 4;
   }
}

inline void __vectorcall FullCollect(GCTable* table, GCRoot* roots)
{
   uintptr_t mg_end = table->gc_mg_current;

   // ; ====== Major Collection ====
      
   // ; collect roots
   while (roots->stackPtr) {
      //   ; mark both yg and mg objects
      MGCollect(roots, table->gc_yg_start, mg_end);

      roots++;
   }

   // ; == compact mg ==

   uintptr_t mappings = table->gc_header + ((table->gc_mg_start - table->gc_start) << page_size_order_minus2);

   // ; skip the permanent part
   ObjectPage* mgPtr = (ObjectPage*)table->gc_mg_start;
   while (mgPtr->size < 0) {
      *(uintptr_t*)mappings = getObjectPtr((uintptr_t)mgPtr);

      mgPtr = (ObjectPage*)((size_t)mgPtr + ((mgPtr->size + page_ceil) & page_align_mask));
      mappings += (mgPtr->size << page_size_order_minus2);
   }

   // ; compact	
   ObjectPage* newPtr = mgPtr;
   while ((uintptr_t)mgPtr < mg_end) {
      int object_size = ((mgPtr->size + page_ceil) & page_align_mask);

      if (mgPtr->size < 0) {
         *(uintptr_t*)mappings = getObjectPtr((uintptr_t)newPtr);

         // ; copy page
         CopyObject(object_size, newPtr, mgPtr);
         newPtr = (ObjectPage*)((size_t)newPtr + object_size);
      }

      mgPtr = (ObjectPage*)((size_t)mgPtr + object_size);
      mappings += (mgPtr->size << page_size_order_minus2);
   }

   // ; promote yg
   ObjectPage* ygPtr = (ObjectPage*)table->gc_yg_start;
   mappings = table->gc_header + ((table->gc_yg_start - table->gc_start) << page_size_order_minus2);

   uintptr_t yg_end = table->gc_yg_current;
   while ((uintptr_t)ygPtr < yg_end) {
      int object_size = ((mgPtr->size + page_ceil) & page_align_mask);

      if (ygPtr->size < 0) {
         *(uintptr_t*)mappings = getObjectPtr((uintptr_t)newPtr);

         // ; copy page
         CopyObject(object_size, newPtr, ygPtr);
         newPtr = (ObjectPage*)((size_t)newPtr + object_size);
      }

      ygPtr = (ObjectPage*)((size_t)ygPtr + object_size);
      mappings += (mgPtr->size << page_size_order_minus2);
   }

   // ; set mg_current, clear yg and survive
   table->gc_mg_current = (uintptr_t)newPtr;
   table->gc_yg_current = table->gc_yg_start;

   // ; fix roots
   while (roots->stackPtr) {
      //   ; mark both yg and mg objects
      FixObject(table, roots, table->gc_yg_start, mg_end);

      roots++;
   }

   // ; clear WBar
   memset((void*)table->gc_mg_wbar, 0, table->gc_end - table->gc_mg_start);
}

void* SystemRoutineProvider::GCRoutine(GCTable* table, GCRoot* roots, size_t size)
{
   ObjectPage* shadowPtr = (ObjectPage*)table->gc_shadow;
   while (roots->stackPtr) {
      YGCollect(roots, table->gc_yg_start, table->gc_yg_end, shadowPtr);

      roots++;
   }

   // ; save gc_yg_current to mark  objects
   table->gc_yg_current = (uintptr_t)shadowPtr;

   // ; switch main YG heap with a shadow one
   uintptr_t tmp = table->gc_yg_start;
   table->gc_yg_start = table->gc_shadow;
   table->gc_shadow = tmp;
   tmp = table->gc_yg_end;
   table->gc_yg_end = table->gc_shadow_end;
   table->gc_shadow_end = tmp;

   if (table->gc_yg_end - table->gc_yg_current < size) {
      // ; expand MG if required to promote YG
      if (table->gc_end - table->gc_mg_current < table->gc_yg_current - table->gc_yg_start) {
         ExpandHeap((void*)table->gc_end, 0x2A000);
         ExpandHeap((void*)(table->gc_header + ((table->gc_end - table->gc_start) >> page_size_order_minus2)), 0x0A800);

         table->gc_end += 0x2A000;
      }

      FullCollect(table, roots);

      uintptr_t allocated = table->gc_yg_current;
      if (table->gc_yg_end - table->gc_yg_current < size) {
         // ; try to allocate in the mg
         while (table->gc_end - table->gc_mg_current < size) {
            // ; bad luck, we have to expand GC

            ExpandHeap((void*)table->gc_end, 0x2A000);
            ExpandHeap((void*)(table->gc_header + ((table->gc_end - table->gc_start) >> page_size_order_minus2)), 0x0A800);

            table->gc_end += 0x2A000;
         }

         uintptr_t allocated = table->gc_mg_current;
         table->gc_mg_current += size;

         *(char*)(table->gc_start + ((allocated - table->gc_start) >> page_size_order)) = 1;

         return (void*)getObjectPtr(allocated);
      }
      else {
         uintptr_t allocated = table->gc_yg_current;

         table->gc_yg_current += size;

         return (void*)getObjectPtr(allocated);
      }
   }
   else {
      uintptr_t allocated = table->gc_yg_current;

      table->gc_yg_current += size;

      return (void*)getObjectPtr(allocated);
   }
}

