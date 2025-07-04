//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  GC System Routines
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace elena_lang;

#if _M_IX86 || __i386__

constexpr int LOCK_FLAG          = 0x01000000;

constexpr int elObjectOffset     = elObjectOffset32;
constexpr int gcCollectedMask    = 0x80000000;

constexpr int page_ceil          = gcPageCeil32;
constexpr int size_ceil          = elSizeCeil32;

constexpr int page_align_mask    = 0x007FFFF0;

constexpr int page_size          = gcPageSize32;
constexpr int page_size_order    = gcPageSizeOrder32;
constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_32;

constexpr int size_mask          = elObjectSizeMask32;
constexpr int struct_mask        = elStructMask32;

constexpr int heap_inc           = 0x2A000;
constexpr int heapheader_inc     = 0x0A800;

typedef ObjectPage32    ObjectPage;

#else

constexpr size_t LOCK_FLAG       = 0x00000000;

constexpr int elObjectOffset     = elObjectOffset64;
constexpr int gcCollectedMask    = 0x80000000;

constexpr int page_ceil          = gcPageCeil64;
constexpr int size_ceil          = elSizeCeil64;

constexpr int page_align_mask    = 0x3FFFFFE0;

constexpr int page_size          = gcPageSize64;
constexpr int page_size_order    = gcPageSizeOrder64;
constexpr int page_size_order_minus2 = gcPageSizeOrderMinus2_64;

constexpr int size_mask          = elObjectSizeMask64;
constexpr int struct_mask        = elStructMask64;

constexpr int heap_inc           = 0x54000;
constexpr int heapheader_inc     = 0x15000;

typedef ObjectPage64    ObjectPage;

#endif

static size_t minorCollections = 0;
static size_t majorCollections = 0;

inline ObjectPage* getObjectPage(uintptr_t objptr)
{
   return (ObjectPage*)(objptr - elObjectOffset);
}

inline uintptr_t getObjectPtr(uintptr_t pagePtr)
{
   return pagePtr + elObjectOffset;
}

inline void* getVMTPtr(uintptr_t objptr)
{
   return (void*)getObjectPage(objptr)->vmtPtr;
}

inline int getSize(uintptr_t objptr)
{
   return getObjectPage(objptr)->size;
}

inline void orSize(uintptr_t objptr, int mask)
{
   getObjectPage(objptr)->size |= mask;
}

inline void CopyObjectData(size_t bytesToCopy, void* dst, void* src)
{
   bytesToCopy += (sizeof(uintptr_t) - 1);
   bytesToCopy &= size_ceil;

   memcpy(dst, src, bytesToCopy);
}

inline void MoveObject(size_t bytesToCopy, void* dst, void* src)
{
   memmove(dst, src, bytesToCopy);
}

inline void YGCollect(GCRoot* root, size_t start, size_t end, ObjectPage*& shadowHeap, void* src)
{
   uintptr_t* ptr = (uintptr_t*)root->stack_ptr;
   uintptr_t  size = root->size;
   uintptr_t  new_ptr = 0;
   GCRoot     current;

   //printf("YGCollect %llx,%llx\n", (long long)ptr, (long long)size);

   // ; collect roots
   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {
         current.stack_ptr_addr = *ptr;

         ObjectPage* currentPage = getObjectPage(current.stack_ptr_addr);

         // ; check if it was collected
         current.size = currentPage->size;
         if (!(current.size & gcCollectedMask)) {
            // ; copy object size
            shadowHeap->size = (int)current.size;

            // ; copy object vmt
            shadowHeap->vmtPtr = currentPage->vmtPtr;

#if defined _M_X64 || __x86_64__ || __PPC64__ || __aarch64__
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
               YGCollect(&current, start, end, shadowHeap, current.stack_ptr);
            }
            else if (current.size != struct_mask) {
               CopyObjectData(current.size, (void*)new_ptr, current.stack_ptr);
            }
         }
         else {
            // ; update reference
            *ptr = (size_t)getVMTPtr(current.stack_ptr_addr);
         }
      }
      ptr++;
      size -= sizeof(uintptr_t);
   }

   // ; copy object to shadow YG
   if (src) {
      void* dst = getVMTPtr((intptr_t)src);
      CopyObjectData(root->size, dst, src);
   }
}

inline void CollectMG2YGRoots(GCTable* table, ObjectPage* &shadowPtr)
{
   intptr_t wbar_count = (table->gc_mg_current - table->gc_mg_start) >> page_size_order;
   uintptr_t wb_current = table->gc_mg_wbar;
   uintptr_t mg_current = table->gc_mg_start;

   GCRoot wb_root;
   while (wbar_count > 0) {
      long long card = *(long long*)wb_current;
      if (card) {
         if (testanyLong(card, 0xFFULL)) {
            wb_root.stack_ptr_addr = mg_current + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00ULL)) {
            wb_root.stack_ptr_addr = mg_current + page_size + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF0000ULL)) {
            wb_root.stack_ptr_addr = mg_current + (page_size << 1) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF000000ULL)) {
            wb_root.stack_ptr_addr = mg_current + (page_size * 3) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00000000ULL)) {
            wb_root.stack_ptr_addr = mg_current + (page_size << 2) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF0000000000ULL)) {
            wb_root.stack_ptr_addr = mg_current + ((page_size << 2) + page_size) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF000000000000ULL)) {
            wb_root.stack_ptr_addr = mg_current + (page_size * 6) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
         if (testanyLong(card, 0xFF00000000000000ULL)) {
            wb_root.stack_ptr_addr = mg_current + (page_size * 7) + elObjectOffset;
            wb_root.size = getSize(wb_root.stack_ptr_addr) & ~LOCK_FLAG;

            YGCollect(&wb_root, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
         }
      }

      mg_current += (page_size << 3);
      wb_current += 8;
      wbar_count -= 8;
   }
}

inline void CollectPermYGRoots(GCTable* table, ObjectPage*& shadowPtr)
{
   GCRoot     currentRoot;

   addr_t current = table->gc_perm_start;
   addr_t end = table->gc_perm_current;
   while (current < end) {
      addr_t objectPtr = getObjectPtr(current);
      ObjectPage* currentPage = getObjectPage(objectPtr);

      currentRoot.size = currentPage->size;
      currentRoot.stack_ptr_addr = objectPtr;

      int object_size = ((currentPage->size + page_ceil) & page_align_mask);
      if (currentRoot.size < struct_mask) {
         // ; check if the object has fields
         YGCollect(&currentRoot, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);
      }

      current += object_size;
   }
}

void MGCollect(GCRoot* root, size_t start, size_t end)
{
   size_t* ptr = (size_t*)root->stack_ptr;
   size_t  size = root->size & ~LOCK_FLAG;

   GCRoot  current;

   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {

         current.stack_ptr_addr = *ptr;

         // ; check if it was collected
         current.size = getSize((size_t)current.stack_ptr);
         if (!(current.size & gcCollectedMask)) {
            // ; mark as collected
            orSize(current.stack_ptr_addr, gcCollectedMask);

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

inline void FixObject(GCTable* table, GCRoot* roots, size_t start, size_t end)
{
   uintptr_t* ptr = (uintptr_t*)roots->stack_ptr;
   size_t  size = roots->size & ~LOCK_FLAG;

   GCRoot  current;

   while (size > 0) {
      // ; check if it valid reference
      if (*ptr >= start && *ptr < end) {

         ObjectPage* pagePtr = getObjectPage(*ptr);
         uintptr_t mappings = table->gc_header + (((uintptr_t)pagePtr - table->gc_start) >> page_size_order_minus2);

         // ; replace old reference with a new one
         uintptr_t newPtr = *(uintptr_t*)mappings;
         *ptr = newPtr;
         pagePtr = getObjectPage(newPtr);

         // ; make sure the object was not already fixed
         if (pagePtr->size < 0) {
            pagePtr->size = pagePtr->size & 0x7FFFFFFF;

            if (!test(pagePtr->size, struct_mask)) {
               current.stack_ptr_addr = newPtr;
               current.size = pagePtr->size;

               FixObject(table, &current, start, end);
            }
         }
      }
      ptr++;
      size -= sizeof(intptr_t);
   }
}

inline void FullCollect(GCTable* table, GCRoot* roots)
{
   majorCollections++;

   uintptr_t yg_start = table->gc_yg_start;
   uintptr_t mg_end = table->gc_mg_current;

   // ; ====== Major Collection ====

   // ; collect roots
   GCRoot* current = roots;
   while (current->stack_ptr) {
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
   while (roots->stack_ptr) {
      //   ; mark both yg and mg objects
      FixObject(table, roots, yg_start, mg_end);

      roots++;
   }

   // ; clear WBar
   size_t size = (table->gc_end - table->gc_mg_start) >> page_size_order;
   memset((void*)table->gc_mg_wbar, 0, size);
}

void* SystemRoutineProvider::GCRoutine(GCTable* table, GCRoot* roots, size_t size, bool fullMode)
{
   //printf("GCRoutine %llx,%llx\n", (long long)roots, (long long)size);

   // ; collect yg roots
   ObjectPage* shadowPtr = (ObjectPage*)table->gc_shadow;
   // ; collect roots 
   GCRoot* current = roots;
   while (current->stack_ptr) {
      YGCollect(current, table->gc_yg_start, table->gc_yg_end, shadowPtr, nullptr);

      current++;
   }

   // ; collect mg -> yg roots
   CollectMG2YGRoots(table, shadowPtr);

   // ; collect perm yg roots
   if (table->gc_perm_current > table->gc_perm_end)
      CollectPermYGRoots(table, shadowPtr);

   // ; save gc_yg_current to mark  objects
   table->gc_yg_current = (uintptr_t)shadowPtr;

   // ; switch main YG heap with a shadow one
   uintptr_t tmp = table->gc_yg_start;
   table->gc_yg_start = table->gc_shadow;
   table->gc_shadow = tmp;
   tmp = table->gc_yg_end;
   table->gc_yg_end = table->gc_shadow_end;
   table->gc_shadow_end = tmp;

   minorCollections++;

   if ((table->gc_yg_end - table->gc_yg_current < size) || fullMode) {
      // ; expand MG if required to promote YG
      while (table->gc_end - table->gc_mg_current < table->gc_yg_current - table->gc_yg_start) {
         size_t inc = AlignHeapSize(heap_inc);
         //size_t header_inc = AlignHeapSize(heap_inc >> page_size_order_minus2);

         ExpandHeap((void*)table->gc_end, inc);
         ExpandHeap((void*)(table->gc_header + ((table->gc_end - table->gc_start) >> page_size_order_minus2)), heapheader_inc);

         table->gc_end += inc;
      }

      FullCollect(table, roots);

      if (size == 0)
         return nullptr;

      if (table->gc_yg_end - table->gc_yg_current < size) {
         // ; try to allocate in the mg
         while (table->gc_end - table->gc_mg_current < size) {
            // ; bad luck, we have to expand GC
            size_t inc = AlignHeapSize(size - (table->gc_end - table->gc_mg_current));
            //size_t header_inc = AlignHeapSize(inc >> page_size_order_minus2);

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
   else if (size > 0) {
      uintptr_t allocated = table->gc_yg_current;

      table->gc_yg_current += size;

      return (void*)getObjectPtr(allocated);
   }

   return nullptr;
}

void* SystemRoutineProvider :: GCRoutinePerm(GCTable* table, size_t size)
{
   if (table->gc_perm_current + size > table->gc_perm_end) {
      size_t permSize = AlignHeapSize(size);

      // allocate a perm space
      ExpandPerm((void*)table->gc_perm_end, permSize);

      table->gc_perm_end += permSize;
   }

   uintptr_t allocated = table->gc_perm_current;

   table->gc_perm_current += size;

   return (void*)getObjectPtr(allocated);
}

bool SystemRoutineProvider :: CopyResult(addr_t value, char* output, size_t maxLength, size_t& copied)
{
   size_t size = getSize(value);
   if (size < struct_mask) {
      // ; if the object has fields
      return false;
   }
   else if (size != struct_mask) {
      size_t bytesToCopy = size;
      bytesToCopy += (sizeof(uintptr_t) - 1);
      bytesToCopy &= size_ceil;
      if (bytesToCopy > maxLength)
         return false;

      CopyObjectData(bytesToCopy, output, (void*)value);

      copied = bytesToCopy;
   }
   else copied = 0;

   return true;
}

void SystemRoutineProvider :: CalcGCStatistics(SystemEnv* systemEnv, GCStatistics* statistics)
{
   auto table = systemEnv->gc_table;

   statistics->ygInfo.allocated = (unsigned int)(table->gc_yg_current - table->gc_yg_start);
   statistics->ygInfo.free = (unsigned int)(table->gc_yg_end - table->gc_yg_current);

   statistics->mgInfo.allocated = (unsigned int)(table->gc_mg_current - table->gc_mg_start);
   statistics->mgInfo.free = (unsigned int)(table->gc_end - table->gc_mg_current);

   statistics->permInfo.allocated = (unsigned int)(table->gc_perm_current - table->gc_perm_start);
   statistics->permInfo.free = (unsigned int)(table->gc_perm_end - table->gc_perm_current);

   statistics->minorCollections = (unsigned int)minorCollections;
   statistics->majorCollections = (unsigned int)majorCollections;
}

void SystemRoutineProvider :: ResetGCStatistics()
{
   minorCollections = 0;
   majorCollections = 0;
}
