//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  macOS ELENA System Routines
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

#include <mach-o/dyld.h>
#include <mach-o/loader.h>

#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <ctime>
#include <cstring>
#include <unistd.h>

#include <pthread.h>

using namespace elena_lang;

class EventImpl
{
private:
   bool              _signalled;
   pthread_mutex_t   _mutex;
   pthread_cond_t    _cond;

public:
   EventImpl()
   {
      _signalled = false;
      pthread_mutex_init(&_mutex, nullptr);
      pthread_cond_init(&_cond, nullptr);
   }

   virtual ~EventImpl()
   {
      pthread_mutex_destroy(&_mutex);
      pthread_cond_destroy(&_cond);
   }

   void waitForSignal()
   {
      pthread_mutex_lock(&_mutex);
      while (!_signalled) {
         pthread_cond_wait(&_cond, &_mutex);
      }
      pthread_mutex_unlock(&_mutex);
   }

   void reset()
   {
      if (_signalled) {
         pthread_mutex_lock(&_mutex);
         _signalled = false;
         pthread_mutex_unlock(&_mutex);
      }
   }

   void signal()
   {
      pthread_mutex_lock(&_mutex);
      _signalled = true;
      pthread_mutex_unlock(&_mutex);
      pthread_cond_broadcast(&_cond);
   }
};

static uintptr_t CriticalHandler = 0;

#if defined(__APPLE__)

static void* retrieveSection(const mach_header_64* header, const char* segmentName, const char* sectionName)
{
   if (!header || header->magic != MH_MAGIC_64)
      return nullptr;

   intptr_t slide = 0;
   uint32_t imageCount = _dyld_image_count();
   for (uint32_t i = 0; i < imageCount; i++) {
      if (_dyld_get_image_header(i) == reinterpret_cast<const mach_header*>(header)) {
         slide = _dyld_get_image_vmaddr_slide(i);

         break;
      }
   }

   auto command = reinterpret_cast<const load_command*>(reinterpret_cast<uintptr_t>(header) + sizeof(mach_header_64));
   for (uint32_t i = 0; i < header->ncmds; i++) {
      if (command->cmd == LC_SEGMENT_64) {
         auto segment = reinterpret_cast<const segment_command_64*>(command);
         if (strncmp(segment->segname, segmentName, sizeof(segment->segname)) == 0) {
            auto section = reinterpret_cast<const section_64*>(segment + 1);
            for (uint32_t j = 0; j < segment->nsects; j++) {
               if (strncmp(section[j].sectname, sectionName, sizeof(section[j].sectname)) == 0) {
                  return reinterpret_cast<void*>(static_cast<uintptr_t>(section[j].addr + slide));
               }
            }
         }
      }

      command = reinterpret_cast<const load_command*>(reinterpret_cast<uintptr_t>(command) + command->cmdsize);
   }

   return nullptr;
}

#endif

void* SystemRoutineProvider::RetrieveMDataPtr(void* imageBase, pos_t imageLength)
{
   (void)imageLength;

   auto header = imageBase ? static_cast<const mach_header_64*>(imageBase) : reinterpret_cast<const mach_header_64*>(_dyld_get_image_header(0));

   void* section = retrieveSection(header, "__DATA_CONST", "__mdata");
   if (!section)
      section = retrieveSection(header, "__DATA", "__mdata");

   return section;
}

size_t SystemRoutineProvider :: AlignHeapSize(size_t size)
{
   return alignSize(size, 0x10);
}

// NOTE : commits a range inside the reservation NewHeap made. mmap already maps it all
//        read/write, so mprotect is really here to check the range : it returns ENOMEM
//        once it leaves the mapping, which is the out of memory condition the callers
//        look for. The old code called mmap without MAP_FIXED, so the kernel was free to
//        place the range anywhere and the heap never actually grew.
static uintptr_t commitRange(void* allocPtr, size_t newSize)
{
   // mprotect requires a page aligned address, while the heap is only 16 byte aligned
   uintptr_t pageSize = (uintptr_t)sysconf(_SC_PAGESIZE);
   uintptr_t start = (uintptr_t)allocPtr & ~(pageSize - 1);
   size_t    length = ((uintptr_t)allocPtr + newSize) - start;

   if (mprotect((void*)start, length, PROT_READ | PROT_WRITE) != 0)
      return 0;

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: NewHeap(size_t totalSize, size_t committedSize)
{
   void* allocPtr = mmap(nullptr, totalSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   // NOTE : the old test used INVALID_REF, a 32 bit ref_t that never matches MAP_FAILED
   //        on a 64 bit target, so a failed mmap went unnoticed
   if (allocPtr == MAP_FAILED) {
      ::exit(errno);
   }

   // NOTE : commit the initial range, as the Windows version does after reserving
   if (committedSize && !commitRange(allocPtr, committedSize)) {
      ::exit(errno);
   }

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandHeap(void* allocPtr, size_t newSize)
{
   return commitRange(allocPtr, newSize);
}

uintptr_t SystemRoutineProvider :: ExpandPerm(void* allocPtr, size_t newSize)
{
   return commitRange(allocPtr, newSize);
}

typedef void*(*thread_proc_t)(void*);

void* SystemRoutineProvider :: CreateThread(size_t tt_index, int stackSize, int flags, void* threadProc)
{
   pthread_t th;
   pthread_create(&th, nullptr, (thread_proc_t)threadProc, (void*)tt_index);

   return (void*)th;
}

void SystemRoutineProvider::ExitThread(int exitCode)
{
   pthread_exit((void*)exitCode);
}

void SystemRoutineProvider :: RaiseError(int code)
{
   ::raise(code);
}

void SystemRoutineProvider :: Exit(int exitCode)
{
   ::exit(exitCode);
}

#if defined(__i386__)

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

#if defined(__FreeBSD__) || defined(__APPLE__)

   switch (sig) {
   case SIGFPE:
      u->uc_mcontext.mc_edx = u->uc_mcontext.mc_eip;
      u->uc_mcontext.mc_eax = ELENA_ERR_DIVIDE_BY_ZERO;
      u->uc_mcontext.mc_eip = CriticalHandler;
      break;
   case SIGSEGV:
      u->uc_mcontext.mc_edx = u->uc_mcontext.mc_eip;
      u->uc_mcontext.mc_eax = ELENA_ERR_ACCESS_VIOLATION;
      u->uc_mcontext.mc_eip = CriticalHandler;
      break;
   default:
      u->uc_mcontext.mc_edx = u->uc_mcontext.mc_eip;
      u->uc_mcontext.mc_eax = ELENA_ERR_CRITICAL;
      u->uc_mcontext.mc_eip = CriticalHandler;
      break;
   }

#else

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
      default:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
   }

#endif
}

#elif __x86_64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

#if defined(__FreeBSD__)

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.mc_rdx = u->uc_mcontext.mc_rip;
         u->uc_mcontext.mc_rax = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.mc_rip = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.mc_rdx = u->uc_mcontext.mc_rip;
         u->uc_mcontext.mc_rax = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.mc_rip = CriticalHandler;
         break;
      default:
         u->uc_mcontext.mc_rdx = u->uc_mcontext.mc_rip;
         u->uc_mcontext.mc_rax = ELENA_ERR_CRITICAL;
         u->uc_mcontext.mc_rip = CriticalHandler;
         break;
   }

#elif defined(__APPLE__)

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext->__ss.__rdx = u->uc_mcontext->__ss.__rip;
         u->uc_mcontext->__ss.__rax = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext->__ss.__rip = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext->__ss.__rdx = u->uc_mcontext->__ss.__rip;
         u->uc_mcontext->__ss.__rax = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext->__ss.__rip = CriticalHandler;
         break;
      default:
         u->uc_mcontext->__ss.__rdx = u->uc_mcontext->__ss.__rip;
         u->uc_mcontext->__ss.__rax = ELENA_ERR_CRITICAL;
         u->uc_mcontext->__ss.__rip = CriticalHandler;
         break;
   }

#else

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
         u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
         u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
         break;
      default:
         u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
         u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
         break;
   }

#endif
}

#elif __aarch64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

#if defined(__APPLE__)

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext->__ss.__x[9] = __darwin_arm_thread_state64_get_pc(u->uc_mcontext->__ss);
         u->uc_mcontext->__ss.__x[0] = ELENA_ERR_DIVIDE_BY_ZERO;
         __darwin_arm_thread_state64_set_pc_fptr(u->uc_mcontext->__ss, (void*)CriticalHandler);
         break;
      case SIGSEGV:
         u->uc_mcontext->__ss.__x[9] = __darwin_arm_thread_state64_get_pc(u->uc_mcontext->__ss);
         u->uc_mcontext->__ss.__x[0] = ELENA_ERR_ACCESS_VIOLATION;
         __darwin_arm_thread_state64_set_pc_fptr(u->uc_mcontext->__ss, (void*)CriticalHandler);
         break;
      default:
         u->uc_mcontext->__ss.__x[9] = __darwin_arm_thread_state64_get_pc(u->uc_mcontext->__ss);
         u->uc_mcontext->__ss.__x[0] = ELENA_ERR_CRITICAL;
         __darwin_arm_thread_state64_set_pc_fptr(u->uc_mcontext->__ss, (void*)CriticalHandler);
         break;
   }

#else

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.pc = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.pc = CriticalHandler;
         break;
      default:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.pc = CriticalHandler;
         break;
   }

#endif
}

#elif __PPC64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.gp_regs[14] = u->uc_mcontext.gp_regs[32];
         u->uc_mcontext.gp_regs[3] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.gp_regs[32] = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.gp_regs[14] = u->uc_mcontext.gp_regs[32];
         u->uc_mcontext.gp_regs[3] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.gp_regs[32] = CriticalHandler;
         break;
      default:
         u->uc_mcontext.gp_regs[14] = u->uc_mcontext.gp_regs[32];
         u->uc_mcontext.gp_regs[3] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.gp_regs[32] = CriticalHandler;
         break;
   }
}

#endif

void SystemRoutineProvider :: InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sigemptyset(&sa.sa_mask);
   sa.sa_sigaction = ELENASignalHandler;
   if (sigaction(SIGSEGV, &sa, NULL) == -1)
      throw InternalError(errAborted);
}


long long SystemRoutineProvider :: GenerateSeed()
{
   time_t t = time(nullptr);

   long long seed = (long int)t;

   return seed;
}

void SystemRoutineProvider::InitMTASignals(SystemEnv* env, size_t index)
{
   EventImpl* event = new EventImpl();

   env->th_table->slots[index].content->tt_sync_event = (void*)event;
   env->th_table->slots[index].content->tt_flags = 0;
}

void SystemRoutineProvider::ClearMTASignals(SystemEnv* env, size_t index)
{
   EventImpl* event = (EventImpl*)env->th_table->slots[index].content->tt_sync_event;

   delete event;

   env->th_table->slots[index].content->tt_sync_event = nullptr;
   env->th_table->slots[index].content->tt_flags = 0;
}

void SystemRoutineProvider::GCSignalStop(void* handle)
{
   ((EventImpl*)handle)->signal();
}

void SystemRoutineProvider::GCWaitForSignals(size_t count, void* handles)
{
   if (count > 0) {
      EventImpl** events = (EventImpl**)handles;
      for (size_t i = 0; i < count; i++) {
         events[i]->waitForSignal();
      }
   }
}

void SystemRoutineProvider::GCWaitForSignal(void* handle)
{
   ((EventImpl*)handle)->waitForSignal();
}

void SystemRoutineProvider::GCSignalClear(void* handle)
{
   ((EventImpl*)handle)->reset();
}

// ; see the comment in linux/lnxroutines.cpp : the barrier waits on gc_signal itself
// ; rather than on the personal event of whichever thread is collecting
static pthread_mutex_t CollectionMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  CollectionCond = PTHREAD_COND_INITIALIZER;
static size_t          CollectionGeneration = 0;

void SystemRoutineProvider::GCWaitForCollection(GCTable* table)
{
   pthread_mutex_lock(&CollectionMutex);

   size_t generation = CollectionGeneration;
   while (CollectionGeneration == generation && table->gc_signal != 0)
      pthread_cond_wait(&CollectionCond, &CollectionMutex);

   pthread_mutex_unlock(&CollectionMutex);
}

void SystemRoutineProvider::GCSignalCollectionEnd()
{
   pthread_mutex_lock(&CollectionMutex);
   CollectionGeneration++;
   pthread_mutex_unlock(&CollectionMutex);

   pthread_cond_broadcast(&CollectionCond);
}
