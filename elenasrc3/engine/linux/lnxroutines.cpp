//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux ELENA System Routines
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

#if defined (__unix__)

#include "linux/elfhelper.h"

#endif

#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <ctime>
#include <unistd.h>

#include <pthread.h>
#include <sys/syscall.h>

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
      pthread_mutex_lock(&_mutex);
      if(_signalled)
         _signalled = false;
      pthread_mutex_unlock(&_mutex);
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

// ; the event belongs to the slot, not to the thread that borrows it : its address is
// ; published in gc_signal and in the collector wait list, and other threads dereference
// ; it after gc_lock is released, so freeing it at thread exit is a use after free
static EventImpl** ThreadEvents = nullptr;

void* SystemRoutineProvider::RetrieveMDataPtr(void* imageBase, pos_t imageLength)
{
#if defined(__APPLE__)

#else

   ImageSection header(imageBase, imageLength);
   MemoryReader reader(&header);
   addr_t addr = 0;
   if (ELFHelper::seekRODataSegment(reader, addr)) {
      return (void*)addr;
   }

#endif

   return nullptr;
}

size_t SystemRoutineProvider :: AlignHeapSize(size_t size)
{
   return alignSize(size, 0x10);
}

// Commits a range inside the reservation NewHeap made. mmap already maps it all
// read/write, so mprotect is really here to check the range : it returns ENOMEM once
// it leaves the mapping, which is the out of memory condition the callers look for.
// The old code passed mmap's argument list to mremap, so it always failed with EINVAL
// and returned MAP_FAILED - non-zero, hence read as success.
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

   // the old test used INVALID_REF, a 32 bit ref_t that never matches MAP_FAILED
   // on a 64 bit target, so a failed mmap went unnoticed
   if (allocPtr == MAP_FAILED) {
      ::exit(errno);
   }

   // commit the initial range, as the Windows version does after reserving
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

   fprintf(stderr, "[%ld] !! signal %d at rip=%llx addr=%p\n", (long)syscall(SYS_gettid),
      sig, (unsigned long long)u->uc_mcontext.gregs[REG_RIP], si ? si->si_addr : nullptr);

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

// ; ==== temporary deadlock tracing, to be removed ====
#include <sys/syscall.h>
#define GC_TRACE(...) fprintf(stderr, __VA_ARGS__)
static long gcTid() { return (long)syscall(SYS_gettid); }

static size_t ThreadEventCount = 0;

// ; gc_signal must always hold the sync event of the collecting thread. Catch the moment
// ; it holds anything else and name the field it matches, to find the writer
static void gcCheckSignal(SystemEnv* env, const char* where)
{
   size_t sig = (size_t)env->gc_table->gc_signal;
   if (!sig || !ThreadEvents)
      return;

   for (size_t i = 0; i < ThreadEventCount; i++) {
      if ((size_t)ThreadEvents[i] == sig)
         return;
   }

   GCTable* t = env->gc_table;
   GC_TRACE("[%ld] !! gc_signal=%zx is no event (%s)\n", gcTid(), sig, where);
   GC_TRACE("     header=%zx start=%zx yg_start=%zx yg_current=%zx yg_end=%zx\n",
      (size_t)t->gc_header, (size_t)t->gc_start, (size_t)t->gc_yg_start,
      (size_t)t->gc_yg_current, (size_t)t->gc_yg_end);
   GC_TRACE("     shadow=%zx shadow_end=%zx mg_start=%zx mg_current=%zx end=%zx wbar=%zx\n",
      (size_t)t->gc_shadow, (size_t)t->gc_shadow_end, (size_t)t->gc_mg_start,
      (size_t)t->gc_mg_current, (size_t)t->gc_end, (size_t)t->gc_mg_wbar);
   GC_TRACE("     perm_start=%zx perm_end=%zx perm_current=%zx lock=%zx\n",
      (size_t)t->gc_perm_start, (size_t)t->gc_perm_end, (size_t)t->gc_perm_current,
      (size_t)t->gc_lock);
}
// ; ====

void SystemRoutineProvider::InitMTASignals(SystemEnv* env, size_t index)
{
   // ; serialised by the system 6 / system 7 pair around InitThreadLA
   if (!ThreadEvents)
      ThreadEvents = (EventImpl**)::calloc(env->threadCounter, sizeof(EventImpl*));

   EventImpl* event = ThreadEvents[index];
   if (!event) {
      event = new EventImpl();
      ThreadEvents[index] = event;
   }
   else event->reset();

   env->th_table->slots[index].content->tt_sync_event = (void*)event;
   env->th_table->slots[index].content->tt_flags = 0;

   // ; system 3 publishes the slot without touching the frame chain, and the C library
   // ; hands a recycled stack and TLS block to a new thread : the entry would carry the
   // ; frame pointer of the dead thread that used the block, and the root scan would walk
   // ; a chain that no longer exists. The first safe point publishes the real one
   env->th_table->slots[index].content->tt_stack_frame = 0;

   if (index + 1 > ThreadEventCount)
      ThreadEventCount = index + 1;

   GC_TRACE("[%ld] init slot=%zu ev=%p\n", gcTid(), index, (void*)event);
   gcCheckSignal(env, "init");
}

void SystemRoutineProvider::ClearMTASignals(SystemEnv* env, size_t index)
{
   EventImpl* event = (EventImpl*)env->th_table->slots[index].content->tt_sync_event;

   GC_TRACE("[%ld] clear slot=%zu ev=%p sig=%zx\n", gcTid(), index, (void*)event,
      (size_t)env->gc_table->gc_signal);
   gcCheckSignal(env, "clear");

   // ; a collection may have listed this thread just before it reached the teardown, and
   // ; it will never signal again : the snop ahead of system 6 is not atomic with what
   // ; follows. Release the collector here, under the system 6 / system 7 pair
   if (env->gc_table->gc_signal && event)
      event->signal();

   // ; the event is kept alive for the slot, see InitMTASignals
   env->th_table->slots[index].content->tt_sync_event = nullptr;
   env->th_table->slots[index].content->tt_flags = 0;
}

void SystemRoutineProvider::GCSignalStop(void* handle)
{
   GC_TRACE("[%ld] stop-signal ev=%p\n", gcTid(), handle);

   ((EventImpl*)handle)->signal();
}

void SystemRoutineProvider::GCWaitForSignals(size_t count, void* handles)
{
   if (count > 0) {
      EventImpl** events = (EventImpl**)handles;
      for (size_t i = 0; i < count; i++) {
         GC_TRACE("[%ld] wait-for ev=%p (%zu/%zu)\n", gcTid(), (void*)events[i], i + 1, count);
         if (ThreadEvents) {
            bool known = false;
            for (size_t k = 0; k < ThreadEventCount; k++)
               if (ThreadEvents[k] == events[i]) known = true;
            if (!known)
               GC_TRACE("[%ld] !! waiting on unknown handle %p\n", gcTid(), (void*)events[i]);
         }
         events[i]->waitForSignal();
      }
      GC_TRACE("[%ld] wait-for done\n", gcTid());
   }
}

void SystemRoutineProvider::GCSignalClear(void* handle)
{
   GC_TRACE("[%ld] reset ev=%p\n", gcTid(), handle);

   ((EventImpl*)handle)->reset();
}

// ; A stopped thread used to wait on the personal event of the collector, published in
// ; gc_signal. The table scan of every collection resets every event in the table, that
// ; one included, and its owner may never collect again. The barrier below waits on the
// ; collection state instead, so there is no object left for a third party to reset
static pthread_mutex_t CollectionMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  CollectionCond = PTHREAD_COND_INITIALIZER;

// ; never reset. The wait compares against a value read just before sleeping and ends on
// ; the first increment, so a wrap would only matter if it came full circle while one
// ; thread sat parked, and unsigned wrap is defined anyway
static size_t          CollectionGeneration = 0;

void SystemRoutineProvider::GCWaitForCollection(GCTable* table)
{
   pthread_mutex_lock(&CollectionMutex);

   GC_TRACE("[%ld] park gen=%zu sig=%zx\n", gcTid(), CollectionGeneration, (size_t)table->gc_signal);

   // ; wake on every collection end rather than only when nothing runs : with the storm
   // ; of back to back collections the gc_signal == 0 window is too short to observe and
   // ; the sleepers starve. Leaving into a running collection is safe, the thread still
   // ; carries tt_flags = 1 and its frame stays frozen until the resume passes gc_lock
   size_t generation = CollectionGeneration;
   while (CollectionGeneration == generation && table->gc_signal != 0)
      pthread_cond_wait(&CollectionCond, &CollectionMutex);

   GC_TRACE("[%ld] resume gen=%zu sig=%zx\n", gcTid(), CollectionGeneration, (size_t)table->gc_signal);

   pthread_mutex_unlock(&CollectionMutex);
}

void SystemRoutineProvider::GCSignalCollectionEnd()
{
   // ; the collector clears gc_signal before getting here, outside this mutex. Taking the
   // ; mutex orders that store against the waiters : whoever is inside the wait re-checks
   // ; gc_signal with the mutex held and either sees the zero or gets the broadcast
   pthread_mutex_lock(&CollectionMutex);
   CollectionGeneration++;
   GC_TRACE("[%ld] collection-end gen=%zu\n", gcTid(), CollectionGeneration);
   pthread_mutex_unlock(&CollectionMutex);

   pthread_cond_broadcast(&CollectionCond);
}
