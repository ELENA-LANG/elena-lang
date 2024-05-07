//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common types
//
//                                             (C)2021-2024, by Aleksey Rakov
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
   public:
      addr_t execute(SystemEnv* env, void* symbolListEntry);
      addr_t execute(SystemEnv* env, void* threadEntry, void* threadFunc);

      ELENAMachine() = default;

      virtual ~ELENAMachine() = default;
   };

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

      static void Init(SystemEnv* env, SystemSettings settings);
      static void InitSTAExceptionHandling(SystemEnv* env, void* criticalHandler);
      static void InitMTAExceptionHandling(SystemEnv* env, size_t index, void* criticalHandler);
      static void InitMTASignals(SystemEnv* env, size_t index);
      static void InitCriticalStruct(uintptr_t criticalDispatcher);
      static void InitSTA(SystemEnv* env);

      static long long GenerateSeed();
      static void InitRandomSeed(SeedStruct& seed, long long seedNumber);
      static unsigned int GetRandomNumber(SeedStruct& seed);

      static void* CreateThread(size_t tt_index, int flags, void* threadProc);

      static void GCSignalClear(void* handle);
      static void GCSignalStop(void* handle);
      static void GCWaitForSignal(void* handle);
      static void GCWaitForSignals(size_t count, void* handles);

      static bool CopyResult(addr_t value, char* output, size_t maxLength, size_t& copied);

      static size_t LoadMessages(MemoryBase* msection, void* classPtr, mssg_t* output, size_t skip,
         size_t maxLength, bool vmMode);

      static bool CheckMessage(MemoryBase* msection, void* classPtr, mssg_t message);

      static void RaiseError(int code);

      static void Exit(int exitCode);

   } __routineProvider;

}

#endif
