//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux ELENA System Routines
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"
#include "linux/elfhelper.h"

#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <ctime>

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
      _pthread_mutex_init(&_mutex, nullptr);
      _pthread_cond_init(&_cond, nullptr);
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

void* SystemRoutineProvider::RetrieveMDataPtr(void* imageBase, pos_t imageLength)
{
   ImageSection header(imageBase, imageLength);
   MemoryReader reader(&header);
   addr_t addr = 0;
   if (ELFHelper::seekRODataSegment(reader, addr)) {
      return (void*)addr;
   }

   return nullptr;
}

size_t SystemRoutineProvider :: AlignHeapSize(size_t size)
{
   return alignSize(size, 0x10);
}

uintptr_t SystemRoutineProvider :: NewHeap(size_t totalSize, size_t committedSize)
{
   void* allocPtr = mmap(nullptr, totalSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (allocPtr == (void*)INVALID_REF) {
      ::exit(errno);
   }

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandHeap(void* allocPtr, size_t newSize)
{
#if defined(__FreeBSD__)

   void* r = mmap(allocPtr, newSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

#else

   void* r = mremap(allocPtr, newSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

#endif

   //assert(r == allocPtr);

   return !r ? 0 : (uintptr_t)r;
}

uintptr_t SystemRoutineProvider :: ExpandPerm(void* allocPtr, size_t newSize)
{
#if defined(__FreeBSD__)

   void* r = mmap(allocPtr, newSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

#else

   void* r = mremap(allocPtr, newSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

#endif

   //assert(r == allocPtr);

   return !r ? 0 : (uintptr_t)allocPtr;
}

void* SystemRoutineProvider :: CreateThread(size_t tt_index, int stackSize, int flags, void* threadProc)
{
   pthread_t th;
   pthread_create(&th, nullptr, threadProc, tt_index);

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

#if defined(__FreeBSD__)

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
      for (size_t i = 0; i < coun; i++) {
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
