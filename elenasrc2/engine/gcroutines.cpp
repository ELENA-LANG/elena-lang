//--------------------------------------NO-----------------------------------
//		E L E N A   P r o j e c t:  x86 ELENA GC Routines
//
//                                              (C)2020-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

using namespace _ELENA_;

#ifdef _WIN64

constexpr int page_size = gcPageSize64;
constexpr int page_size_order = gcPageSizeOrder64;
constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_64;
constexpr int page_ceil = gcPageCeil64;
constexpr int page_mask = gcPageMask64;
constexpr int struct_mask = elStructMask64;
constexpr int size_ceil = elSizeCeil64;
constexpr int size_mask = elObjectSizeMask64;

constexpr int page_align_mask = 0x3FFFFFE0;
constexpr int heap_inc = 0x54000;
constexpr int heapheader_inc = 0x0A800;

constexpr int elObjectOffset = elObjectOffset32;

typedef ObjectPage64 ObjectPage;

#else

constexpr int page_size = gcPageSize32;
constexpr int page_size_order = gcPageSizeOrder32;
constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_32;
constexpr int page_ceil = gcPageCeil32;
constexpr int page_mask = gcPageMask32;
constexpr int struct_mask = elStructMask32;
constexpr int size_ceil = elSizeCeil32;
constexpr int size_mask = elObjectSizeMask32;

constexpr int page_align_mask = 0x000FFFF0; // !! temp - is 7FFFF0 correct value?
constexpr int heap_inc = 0x2A000;
constexpr int heapheader_inc = 0x0A800;

constexpr int elObjectOffset = elObjectOffset32;

typedef ObjectPage32 ObjectPage;

#endif

constexpr int gcCollectedMask = 0x80000000;

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

inline void __vectorcall andSize(uintptr_t objptr, int mask)
{
   getObjectPage(objptr)->size &= mask;
}

inline void __vectorcall MoveObject(size_t bytesToCopy, void* dst, void* src)
{
   memmove(dst, src, bytesToCopy);
}

inline void __vectorcall CopyObjectData(size_t bytesToCopy, void* dst, void* src)
{
   bytesToCopy += 3;
   bytesToCopy &= size_ceil;

   memcpy(dst, src, bytesToCopy);
}

void __vectorcall YGCollect(GCRoot* root, size_t start, size_t end, ObjectPage*& shadowHeap, void* src)
{
   size_t* ptr = (size_t*)root->stackPtr;
   size_t  size = root->size;
   intptr_t new_ptr = 0;
   GCRoot  current;

   // ; collect roots
   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {
         current.stackPtrAddr = *ptr;

         // ; check if it was collected
         current.size = getSize((size_t)current.stackPtr);
         if (!(current.size & gcCollectedMask)) {
            // ; copy object size
            shadowHeap->size = current.size;

            // ; copy object vmt
            shadowHeap->vmtPtr = getVMTPtr(current.stackPtrAddr);

            // ; mark as collected
            orSize(current.stackPtrAddr, gcCollectedMask);

            // ; reserve shadow YG
            new_ptr = (size_t)shadowHeap + elObjectOffset;

            shadowHeap = (ObjectPage*)((size_t)shadowHeap + ((current.size + page_ceil) & page_align_mask));

            // ; update reference 
            *ptr = new_ptr;

            // ; get object size
            current.size &= size_mask;

            // ; save new reference
            setVMTPtr(current.stackPtrAddr, (void*)new_ptr);

            if (current.size < struct_mask) {
               // ; check if the object has fields
               YGCollect(&current, start, end, shadowHeap, current.stackPtr);
            }
            else if (current.size != struct_mask) {
               CopyObjectData(current.size, (void*)new_ptr, current.stackPtr);
            }
         }
         else {
            // ; update reference
            *ptr = (size_t)getVMTPtr(current.stackPtrAddr);
         }
      }
      ptr++;
      size -= sizeof(intptr_t);
   }

   // ; copy object to shadow YG
   if (src) {
      void* dst = getVMTPtr((intptr_t)src);
      CopyObjectData(root->size, dst, src);
   }
}

void __vectorcall MGCollect(GCRoot* root, size_t start, size_t end, int b)
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
         if (!(current.size & gcCollectedMask)) {
            // ; mark as collected
            orSize(current.stackPtrAddr, gcCollectedMask);

            // ; check if the object has fields
            if (current.size < struct_mask) {
               MGCollect(&current, start, end, b);
            }
         }
      }
      ptr++;
      size -= sizeof(intptr_t);
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
         uintptr_t mappings = table->gc_header + (((uintptr_t)pagePtr - table->gc_start) >> page_size_order_minus2);

         // ; replace old reference with a new one
         uintptr_t newPtr = *(uintptr_t*)mappings;
         *ptr = newPtr;
         pagePtr = getObjectPage(newPtr);

         // ; make sure the object was not already fixed
         if (pagePtr->size < 0) {
            pagePtr->size = pagePtr->size & 0x7FFFFFFF;

            if (!test(pagePtr->size, struct_mask)) {
               current.stackPtrAddr = newPtr;
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
   uintptr_t yg_start = table->gc_yg_start;
   uintptr_t mg_end = table->gc_mg_current;

   // ; ====== Major Collection ====

   // ; collect roots
   GCRoot* current = roots;
   while (current->stackPtr) {
      if (current->stackPtrAddr >= yg_start && current->stackPtrAddr < mg_end) {
         // HOTFIX : mark WB objects as collected
         orSize(current->stackPtrAddr, gcCollectedMask);
      }

      //   ; mark both yg and mg objects
      MGCollect(current, yg_start, mg_end, table->gc_start);

      current++;
   }

   // ; == compact mg ==

   uintptr_t mappings = table->gc_header + ((table->gc_mg_start - table->gc_start) >> page_size_order_minus2);

   // ; skip the permanent part
   ObjectPage* mgPtr = (ObjectPage*)table->gc_mg_start;
   while (mgPtr->size < 0) {
      int object_size = ((mgPtr->size + page_ceil) & page_align_mask);

      *(uintptr_t*)mappings = getObjectPtr((uintptr_t)mgPtr);

      mgPtr = (ObjectPage*)((size_t)mgPtr + object_size);
      mappings += (object_size >> page_size_order_minus2);
   }

   // ; compact	
   ObjectPage* newPtr = mgPtr;
   while ((uintptr_t)mgPtr < mg_end) {
      int object_size = ((mgPtr->size + page_ceil) & page_align_mask);

      if (mgPtr->size < 0) {
         *(uintptr_t*)mappings = getObjectPtr((uintptr_t)newPtr);

         // ; copy page
         MoveObject(object_size, newPtr, mgPtr);
         newPtr = (ObjectPage*)((size_t)newPtr + object_size);
      }

      mgPtr = (ObjectPage*)((size_t)mgPtr + object_size);
      mappings += (object_size >> page_size_order_minus2);
   }

   // ; promote yg
   ObjectPage* ygPtr = (ObjectPage*)table->gc_yg_start;
   mappings = table->gc_header + ((table->gc_yg_start - table->gc_start) >> page_size_order_minus2);

   uintptr_t yg_end = table->gc_yg_current;
   while ((uintptr_t)ygPtr < yg_end) {
      int object_size = ((ygPtr->size + page_ceil) & page_align_mask);

      if (ygPtr->size < 0) {
         // ; copy page
         MoveObject(object_size, newPtr, ygPtr);

         *(uintptr_t*)mappings = getObjectPtr((uintptr_t)newPtr);

         newPtr = (ObjectPage*)((size_t)newPtr + object_size);
      }

      ygPtr = (ObjectPage*)((size_t)ygPtr + object_size);
      mappings += (object_size >> page_size_order_minus2);
   }

   // ; set mg_current, clear yg and survive
   table->gc_mg_current = (uintptr_t)newPtr;
   table->gc_yg_current = table->gc_yg_start;

   // ; fix roots
   while (roots->stackPtr) {
      //   ; mark both yg and mg objects
      if (roots->stackPtrAddr >= yg_start && roots->stackPtrAddr < mg_end) {
         ObjectPage* pagePtr = getObjectPage(roots->stackPtrAddr);
         uintptr_t mappings = table->gc_header + (((uintptr_t)pagePtr - table->gc_start) >> page_size_order_minus2);

         // replace old reference with a new one if it is a valid mg object
         uintptr_t newPtr = *(uintptr_t*)mappings;
         roots->stackPtrAddr = newPtr;

         // ; make sure the object was not already fixed
         if (getSize(newPtr) < 0) {
            andSize(newPtr, 0x7FFFFFFF);

            FixObject(table, roots, yg_start, mg_end);
         }
      }
      else FixObject(table, roots, yg_start, mg_end);

      roots++;
   }

   // ; clear WBar
   int size = (table->gc_end - table->gc_mg_start) >> page_size_order;
   memset((void*)table->gc_mg_wbar, 0, size);
}

void* SystemRoutineProvider::GCRoutine(GCTable* table, GCRoot* roots, size_t size)
{
   ObjectPage* shadowPtr = (ObjectPage*)table->gc_shadow;
   GCRoot* current = roots;
   while (current->stackPtr) {
      YGCollect(current, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);

      current++;
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
      while (table->gc_end - table->gc_mg_current < table->gc_yg_current - table->gc_yg_start) {
         int inc = AlignHeapSize(heap_inc);
         int header_inc= AlignHeapSize(heap_inc >> page_size_order_minus2);

         ExpandHeap((void*)table->gc_end, inc);
         ExpandHeap((void*)(table->gc_header + ((table->gc_end - table->gc_start) >> page_size_order_minus2)), heapheader_inc);

         table->gc_end += inc;
      }

      FullCollect(table, roots);

      uintptr_t allocated = table->gc_yg_current;
      if (table->gc_yg_end - table->gc_yg_current < size) {
         // ; try to allocate in the mg
         while (table->gc_end - table->gc_mg_current < size) {
            // ; bad luck, we have to expand GC
            int inc = AlignHeapSize(size - (table->gc_end - table->gc_mg_current));
            int header_inc = AlignHeapSize(inc >> page_size_order_minus2);

            if (ExpandHeap((void*)table->gc_end, inc)) {
               ExpandHeap((void*)(table->gc_header + ((inc) >> page_size_order_minus2)), heapheader_inc);
            }
            else RaiseError(ELENA_ERR_OUT_OF_MEMORY);

            table->gc_end += inc;
         }

         uintptr_t allocated = getObjectPtr(table->gc_mg_current);
         table->gc_mg_current += size;

         *(char*)(table->gc_header + ((allocated - table->gc_start) >> page_size_order)) = 1;

         return (void*)allocated;
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


