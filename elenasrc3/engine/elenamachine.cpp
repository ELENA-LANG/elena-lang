//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Machine common routines implementation
//
//                                             (C)2021-2024, by Aleksey Rakov
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
constexpr int gcPageSize = gcPageSize32;
constexpr int elObjectOffset = elObjectOffset32;

#else

typedef VMTHeader64  VMTHeader;
typedef VMTEntry64   VMTEntry;

constexpr int elVMTClassOffset = elVMTClassOffset64;
constexpr int gcPageSize = gcPageSize64;
constexpr int elObjectOffset = elObjectOffset64;

#endif

inline uintptr_t RetrieveStaticField(uintptr_t ptr, int index)
{
   uintptr_t str = *(uintptr_t*)(ptr - sizeof(VMTHeader) + index * sizeof(uintptr_t));

   return str;
}

inline uintptr_t RetrieveVMT(uintptr_t ptr)
{
   return *(uintptr_t*)(ptr - elObjectOffset);
}

// --- ELENAMachine ---

uintptr_t ELENAMachine :: createPermString(SystemEnv* env, ustr_t s, uintptr_t classPtr)
{
   size_t nameLen = getlength(s) + 1;
   uintptr_t nameAddr = (uintptr_t)SystemRoutineProvider::GCRoutinePerm(env->gc_table, align(nameLen, gcPageSize));

   StrConvertor::copy((char*)nameAddr, s.str(), nameLen, nameLen);

   ObjectPage32* header = (ObjectPage32*)(nameAddr - elObjectOffset);
   header->vmtPtr = classPtr;
   header->size = nameLen;

   return nameAddr;
}

addr_t ELENAMachine :: inherit(SystemEnv* env, void* srcVMTPtr, int staticLen, int nameIndex, 
   addr_t* addresses, size_t length)
{
   assert(nameIndex < 0);

   void* baseVMTPtr = (void*)SystemRoutineProvider::GetParent(srcVMTPtr);
   size_t srcLength = SystemRoutineProvider::GetVMTLength(srcVMTPtr);
   size_t baseLength = SystemRoutineProvider::GetVMTLength(baseVMTPtr);
   int flags = SystemRoutineProvider::GetFlags(srcVMTPtr);

   static int autoIndex = 0;

   uintptr_t namePtr = RetrieveStaticField((uintptr_t)srcVMTPtr, nameIndex);
   uintptr_t stringVMT = RetrieveVMT(namePtr);

   IdentifierString dynamicName("proxy$");
   if (namePtr) {
      dynamicName.append((const char*)namePtr);
   }
   else dynamicName.appendInt(++autoIndex);

   addr_t addr = _generatedClasses.get(*dynamicName);
   if (addr)
      return addr;

   // NOTE : probably better to create a custom package, but for a moment we can simply copy it
   uintptr_t nameAddr = createPermString(env, *dynamicName, stringVMT);

   size_t size = (srcLength * sizeof(VMTEntry)) + sizeof(VMTHeader) + elObjectOffset32 + staticLen * sizeof(uintptr_t);
   addr_t ptr = (addr_t)SystemRoutineProvider::GCRoutinePerm(env->gc_table, align(size, gcPageSize));

   // HOTFIX : copy build-in static variables
   uintptr_t* staticFields = (uintptr_t*)(ptr + staticLen * sizeof(uintptr_t));
   for (int i = 1; i <= staticLen; i++) {
      staticFields[-i] = RetrieveStaticField((uintptr_t)srcVMTPtr, -i);
   }
   staticFields[nameIndex] = nameAddr;

   VMTHeader* header = (VMTHeader*)(ptr + staticLen * sizeof(uintptr_t));
   VMTEntry* entries = (VMTEntry*)(ptr + staticLen * sizeof(uintptr_t) + sizeof(VMTHeader));

   size_t i = 0;
   size_t j = 0;
   size_t addr_i = 0;

   VMTEntry* base = (VMTEntry*)baseVMTPtr;
   VMTEntry* src = (VMTEntry*)srcVMTPtr;
   while (i < srcLength) {
      if (base[j].message == src[i].message) {
         entries[i] = src[i];
         if (j < baseLength)
            j++;
      }
      else if (src[i].address) {
         // if the method is not abstract
         entries[i] = src[i];
      }
      else {
         entries[i].message = src[i].message;
         entries[i].address = addresses[addr_i];

         addr_i += (addr_i < (length - 1) ? 1 : 0);
      }

      i++;
   }

   header->parentRef = (addr_t)src;
   header->count = srcLength;
   header->flags = flags;
   header->classRef = (addr_t)base;

   _generatedClasses.add(*dynamicName, (addr_t)entries);

   return (addr_t)entries;
}

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

size_t SystemRoutineProvider :: GetVMTLength(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset);

   return header->count;
}

addr_t SystemRoutineProvider::GetParent(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset);

   return header->parentRef;
}

int SystemRoutineProvider :: GetFlags(void* classPtr)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset);

   return header->flags;
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
         mssg_t weakMessage = (mssg_t)((VMTEntry*)classPtr)[i].message;
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

bool SystemRoutineProvider :: CheckMessage(MemoryBase* msection, void* classPtr, mssg_t message)
{
   RTManager manager(msection, nullptr);

   VMTHeader* header = (VMTHeader*)((uintptr_t)classPtr - elVMTClassOffset);
   size_t counter = 0;
   // NOTE : skip the dispatcher
   for (pos_t i = 1; i < header->count; i++) {
      if (((VMTEntry*)classPtr)[i].message == message)
         return true;
   }
   return false;
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
