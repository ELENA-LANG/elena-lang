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

constexpr int elObjectOffset = elObjectOffset64;

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
   return (void*)getObjectPage(objptr)->vmtPtr;
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
   bytesToCopy += (sizeof(uintptr_t) - 1);
   bytesToCopy &= size_ceil;

   memcpy(dst, src, bytesToCopy);
}

void __vectorcall YGCollect(GCRoot* root, size_t start, size_t end, ObjectPage*& shadowHeap, void* src)
{
   uintptr_t* ptr = (size_t*)root->stackPtr;
   uintptr_t  size = root->size;
   uintptr_t  new_ptr = 0;
   GCRoot     current;

   // ; collect roots
   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {
         current.stackPtrAddr = *ptr;

         ObjectPage* currentPage = getObjectPage(current.stackPtrAddr);

         // ; check if it was collected
         current.size = currentPage->size;
         if (!(current.size & gcCollectedMask)) {
            // ; copy object size
            shadowHeap->size = (int)current.size;

            // ; copy object vmt
            shadowHeap->vmtPtr = currentPage->vmtPtr;

#if _WIN64
            shadowHeap->lock_flag = currentPage->lock_flag;
#endif

            // ; mark as collected
            currentPage->size |= gcCollectedMask;

            // ; reserve shadow YG
            new_ptr = (uintptr_t)shadowHeap + elObjectOffset;

            shadowHeap = (ObjectPage*)((uintptr_t)shadowHeap + ((current.size + page_ceil) & page_align_mask));

            // ; update reference 
            *ptr = new_ptr;

            // ; get object size
            current.size &= size_mask;

            // ; save new reference
            currentPage->vmtPtr = new_ptr;

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
         if (!(current.size & gcCollectedMask)) {
            // ; mark as collected
            orSize(current.stackPtrAddr, gcCollectedMask);

            // ; check if the object has fields
            if (current.size < struct_mask) {
               MGCollect(&current, start, end);
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
      size -= sizeof(intptr_t);
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
      //   ; mark both yg and mg objects
      MGCollect(current, yg_start, mg_end);

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
      FixObject(table, roots, yg_start, mg_end);

      roots++;
   }

   // ; clear WBar
   size_t size = (table->gc_end - table->gc_mg_start) >> page_size_order;
   memset((void*)table->gc_mg_wbar, 0, size);
}

void* SystemRoutineProvider::GCRoutine(GCTable* table, GCRoot* roots, size_t size)
{
   ObjectPage* shadowPtr = (ObjectPage*)table->gc_shadow;
   // ; collect roots 
   GCRoot* current = roots;
   while (current->stackPtr) {
      YGCollect(current, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);

      current++;
   }

   // ; collect mg -> yg roots 
   int wbar_count = (table->gc_mg_current - table->gc_mg_start) >> page_size_order;
   uintptr_t wb_current = table->gc_mg_wbar;
   uintptr_t mg_current = table->gc_mg_start;

   GCRoot wb_root;
   while (wbar_count > 0) {
      long long card = *(long long*)wb_current;
      if (card) {
         if (testanyLong(card, 0xFFULL)) {
            wb_root.stackPtrAddr = mg_current + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00ULL)) {
            wb_root.stackPtrAddr = mg_current + page_size + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF0000ULL)) {
            wb_root.stackPtrAddr = mg_current + (page_size << 1) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF000000ULL)) {
            wb_root.stackPtrAddr = mg_current + (page_size * 3) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00000000ULL)) {
            wb_root.stackPtrAddr = mg_current + (page_size << 2) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF0000000000ULL)) {
            wb_root.stackPtrAddr = mg_current + ((page_size << 2) + page_size) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF000000000000ULL)) {
            wb_root.stackPtrAddr = mg_current + (page_size * 6) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00000000000000ULL)) {
            wb_root.stackPtrAddr = mg_current + (page_size * 7) + elObjectOffset;
            wb_root.size = getSize(wb_root.stackPtrAddr);

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
      }
      
      mg_current += (page_size << 3);
      wb_current += 8;
      wbar_count -= 8;
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

         //*(char*)(table->gc_header + ((allocated - table->gc_start) >> page_size_order)) = 1;

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

void* SystemRoutineProvider :: GCRoutinePerm(GCTable* table, size_t size, size_t permSize)
{
   if (table->gc_perm_start == table->gc_perm_end) {
      // allocate a perm space
      ExpandPerm((void*)table->gc_perm_start, permSize);

      table->gc_perm_end += permSize;
   }
   else if (table->gc_perm_current + size > table->gc_perm_end) {
      RaiseError(ELENA_ERR_OUT_OF_PERMMEMORY);
   }
   
   uintptr_t allocated = table->gc_perm_current;

   table->gc_perm_current += size;

   return (void*)getObjectPtr(allocated);
}
