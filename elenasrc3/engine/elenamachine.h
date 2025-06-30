//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAMACHINE_H
#define ELENAMACHINE_H

#include "core.h"

namespace elena_lang
{
   // --- SystemSettings ---
   struct SystemSettings
   {
      int      yg_total_size;
      int      yg_committed_size;
      int      mg_total_size;
      int      mg_committed_size;
      int      page_mask;
      int      page_size_order;
      int      perm_total_size;
   };

   class ImageSection : public MemoryBase
   {
      void* _section;
      pos_t _length;

   public:
      pos_t length() const override
      {
         return _length;
      }

      void* get(pos_t position) const override
      {
         if (position < _length) {
            return (char*)_section + position;
         }
         return nullptr;
      }

      bool read(pos_t position, void* s, pos_t length) const override
      {
         if (position < _length && _length >= position + length) {
            memcpy(s, (unsigned char*)_section + position, length);

            return true;
         }
         else return false;
      }

      bool write(pos_t position, const void* s, pos_t length) override
      {
         // write operations are not supported
         return false;
      }

      bool insert(pos_t position, const void* s, pos_t length) override
      {
         // write operations are not supported
         return false;
      }

      void trim(pos_t position) override
      {
         _length = position;
      }

      ImageSection(void* section, pos_t length)
         : _section(section), _length(length)
      {

      }
   };

   // --- ELENAMachine ---
   class ELENAMachine
   {
   protected:
      AddressMap  _generatedClasses;

      uintptr_t createPermString(SystemEnv* env, ustr_t s, uintptr_t classPtr);
      uintptr_t createPermVMT(SystemEnv* env, size_t size);

   public:
      addr_t injectType(SystemEnv* env, void* proxy, void* srcVMTPtr, int staticLen, int nameIndex/*, addr_t* addresses, size_t length*/);

      addr_t execute(void* symbolListEntry);
      addr_t execute(void* symbolListEntry, void* arg);

      ELENAMachine()
         : _generatedClasses(0)
      {

      }

      virtual ~ELENAMachine() = default;
   };

   DISABLE_WARNING_PUSH
   DISABLE_WARNING_UNUSEDVARIABLE

   // --- SystemRoutineProvider ---
   static class SystemRoutineProvider
   {
   public:
      void* RetrieveMDataPtr(void* imageBase, pos_t imageLength);

      static void FillSettings(SystemEnv* env, SystemSettings& settings);

      static size_t AlignHeapSize(size_t size);

      static uintptr_t NewHeap(size_t totalSize, size_t committedSize);
      static uintptr_t ExpandHeap(void* allocPtr, size_t newSize);
      static uintptr_t ExpandPerm(void* allocPtr, size_t newSize);

      static void* GCRoutine(GCTable* table, GCRoot* roots, size_t size, bool fullMode);
      static void* GCRoutinePerm(GCTable* table, size_t size);

      static size_t LoadCallStack(uintptr_t framePtr, uintptr_t* list, size_t length);

      static void InitGC(SystemEnv* env, SystemSettings settings);
      static void InitSTAExceptionHandling(SystemEnv* env, void* criticalHandler);
      static void InitMTAExceptionHandling(SystemEnv* env, size_t index, void* criticalHandler);
      static void InitMTASignals(SystemEnv* env, size_t index);
      static void ClearMTASignals(SystemEnv* env, size_t index);
      static void InitCriticalStruct(uintptr_t criticalDispatcher);
      static void InitApp(SystemEnv* env);

      static long long GenerateSeed();
      static void InitRandomSeed(SeedStruct& seed, long long seedNumber);
      static unsigned int GetRandomNumber(SeedStruct& seed);

      static void* CreateThread(size_t tt_index, int stackSize, int flags, void* threadProc);

      static void StopThread();

      static void GCSignalClear(void* handle);
      static void GCSignalStop(void* handle);
      static void GCWaitForSignal(void* handle);
      static void GCWaitForSignals(size_t count, void* handles);

      static bool CopyResult(addr_t value, char* output, size_t maxLength, size_t& copied);

      static size_t LoadMessages(MemoryBase* msection, void* classPtr, mssg_t* output, size_t skip,
         size_t maxLength, bool vmMode);
      static size_t GetVMTLength(void* classPtr);
      static addr_t GetClass(void* ptr);
      static addr_t GetParent(void* classPtr);
      static pos_t GetFlags(void* classPtr);
      static bool overrideClass(void* ptr, void* classPtr);

      static bool CheckMessage(MemoryBase* msection, void* classPtr, mssg_t message);

      static void RaiseError(int code);

      static void CalcGCStatistics(SystemEnv* systemEnv, GCStatistics* statistics);
      static void ResetGCStatistics();

      static void Exit(int exitCode);
      static void ExitThread(int exitCode);

   } __routineProvider;

   DISABLE_WARNING_POP
}

#endif
