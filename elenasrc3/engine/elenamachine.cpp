//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common routines implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "rtmanager.h"

using namespace elena_lang;

#if _M_IX86 || __i386__

typedef VMTHeader32  VMTHeader;
typedef VMTEntry32   VMTEntry;

constexpr int elVMTClassOffset = elVMTClassOffset32;

#else

typedef VMTHeader64  VMTHeader;
typedef VMTEntry64   VMTEntry;

constexpr int elVMTClassOffset = elVMTClassOffset64;

#endif

// --- SystemRoutineProvider ---

void SystemRoutineProvider :: InitSTAExceptionHandling(SystemEnv* env, void* criticalHandler)
{
   // inti App criticaL handler
   env->th_single_content->eh_critical = (uintptr_t)criticalHandler;

   // inti OS criticaL handler
   InitCriticalStruct((uintptr_t)env->veh_handler);
}

void SystemRoutineProvider :: InitMTAExceptionHandling(SystemEnv* env, size_t index, void* criticalHandler)
{
   env->th_table->slots[index].content->eh_critical = (uintptr_t)criticalHandler;

   if (index == 0)
      InitCriticalStruct((uintptr_t)env->veh_handler);
}

void SystemRoutineProvider :: Init(SystemEnv* env, SystemSettings settings)
{
   int page_mask = settings.page_mask;

   // ; allocate memory heap
   env->gc_table->gc_header = NewHeap(settings.yg_total_size,  settings.yg_committed_size);

   uintptr_t mg_ptr = NewHeap(settings.mg_total_size, settings.mg_committed_size);
   env->gc_table->gc_start = mg_ptr;

   // ; initialize yg
   env->gc_table->gc_yg_current = env->gc_table->gc_yg_start = mg_ptr;

   // ; initialize gc end
   env->gc_table->gc_end = mg_ptr + align(env->gc_mg_size, 128);

   // ; initialize gc shadow
   mg_ptr += (env->gc_yg_size & page_mask);
   env->gc_table->gc_shadow = env->gc_table->gc_yg_end = mg_ptr;

   // ; initialize gc mg
   mg_ptr += (env->gc_yg_size & page_mask);
   env->gc_table->gc_shadow_end = env->gc_table->gc_mg_start = env->gc_table->gc_mg_current = mg_ptr;

   // ; initialize wbar start
   env->gc_table->gc_mg_wbar = ((mg_ptr - env->gc_table->gc_start) >> settings.page_size_order) + env->gc_table->gc_header;

   // ; initialize but not commit perm space
   int perm_size = align(settings.perm_total_size, 128);
   env->gc_table->gc_perm_start = env->gc_table->gc_perm_current = NewHeap(perm_size, 0);
   env->gc_table->gc_perm_end = env->gc_table->gc_perm_start;
}

void SystemRoutineProvider :: InitSTA(SystemEnv* env)
{
   SystemSettings settings;
   FillSettings(env, settings);

   Init(env, settings);
}

inline uintptr_t getContent(uintptr_t ptr)
{
   return *(uintptr_t*)ptr;
}

size_t SystemRoutineProvider :: LoadCallStack(uintptr_t framePtr, uintptr_t* list, size_t totalLength)
{
   size_t length = 0;
   uintptr_t current = framePtr;
   while (length < totalLength) {
      uintptr_t retAddress = getContent(current + sizeof(uintptr_t));
      if (getContent(current) != 0) {
         list[length++] = retAddress;

         current = getContent(current);
      }
      else if (retAddress) {
         current = retAddress;
      }
      else break;
   }

   return length;
}

void SystemRoutineProvider :: InitRandomSeed(SeedStruct& seed, long long seedNumber)
{
   unsigned int low = (unsigned int)(seedNumber & 0xFFFFFFFF);
   unsigned int hi = (unsigned int)(seedNumber >> 32);

   seed.z1 = low;
   seed.z2 = hi;
   seed.z3 = low * (low & 0xFF);
   seed.z3 = hi * (low & 0xFF00);
}

unsigned int SystemRoutineProvider :: GetRandomNumber(SeedStruct& seed)
{
   unsigned int b;
   b = ((seed.z1 << 6) ^ seed.z1) >> 13;
   seed.z1 = ((seed.z1 & 4294967294U) << 18) ^ b;
   b = ((seed.z2 << 2) ^ seed.z2) >> 27;
   seed.z2 = ((seed.z2 & 4294967288U) << 2) ^ b;
   b = ((seed.z3 << 13) ^ seed.z3) >> 21;
   seed.z3 = ((seed.z3 & 4294967280U) << 7) ^ b;
   b = ((seed.z4 << 3) ^ seed.z4) >> 12;
   seed.z4 = ((seed.z4 & 4294967168U) << 13) ^ b;

   return (seed.z1 ^ seed.z2 ^ seed.z3 ^ seed.z4);
}

size_t SystemRoutineProvider :: LoadMessages(MemoryBase* msection, void* classPtr, mssg_t* output, size_t skip, 
   size_t maxLength, bool vmMode)
{
   RTManager manager(msection, nullptr);

   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset);
   size_t counter = 0;
   // NOTE : skip the dispatcher
   for (pos_t i = 1; i < header->count; i++) {
      if (skip != 0) {
         skip--;
      }
      else if (counter < maxLength) {
         mssg_t weakMessage = ((VMTEntry*)classPtr)[i].message;
         bool duplicate = false;
         for (size_t i = 0; i < counter; i++) {
            if (output[i] == weakMessage) {
               duplicate = true;
               break;
            }              
         }
         if (!duplicate) {
            output[counter] = manager.loadWeakMessage(weakMessage, vmMode);
            counter++;
         }         
      }
      else break;
   }

   return counter;
}

// --- ELENAMachine ---

addr_t ELENAMachine :: execute(SystemEnv* env, void* entryAddress)
{
   Entry entry;
   entry.address = env->bc_invoker;

   // executing the program   

   addr_t retVal = 0;
   try
   {
      retVal = entry.evaluate(entryAddress, nullptr);
   }
   catch (InternalError&)
   {
      //_instance->printInfo("EAbortException");

      retVal = 0;
   }

   return retVal;
}

addr_t ELENAMachine::execute(SystemEnv* env, void* threadEntry, void* threadFunc)
{
   Entry entry;
   entry.address = env->bc_invoker;

   addr_t retVal = entry.evaluate(threadEntry, threadFunc);

   return retVal;
}
