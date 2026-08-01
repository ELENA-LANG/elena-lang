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

#if defined(MTA_DIAG) && defined(__x86_64__) && !defined(__FreeBSD__) && !defined(__APPLE__)

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/uio.h>

#endif

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

#if defined(MTA_DIAG) && defined(__x86_64__) && !defined(__FreeBSD__) && !defined(__APPLE__)

struct MTADiagMapping
{
   uintptr_t start;
   uintptr_t end;
   char perms[5];
   char line[384];
};

static bool MTADiagRead(uintptr_t address, void* target, size_t length)
{
   iovec local { target, length };
   iovec remote { (void*)address, length };

   return process_vm_readv(getpid(), &local, 1, &remote, 1, 0) == (ssize_t)length;
}

static int MTADiagHex(char ch)
{
   if (ch >= '0' && ch <= '9')
      return ch - '0';
   if (ch >= 'a' && ch <= 'f')
      return ch - 'a' + 10;
   if (ch >= 'A' && ch <= 'F')
      return ch - 'A' + 10;

   return -1;
}

static bool MTADiagHexValue(const char*& source, uintptr_t& value)
{
   int digit = MTADiagHex(*source);
   if (digit < 0)
      return false;

   value = 0;
   while ((digit = MTADiagHex(*source)) >= 0) {
      value = (value << 4) | (uintptr_t)digit;
      source++;
   }

   return true;
}

static bool MTADiagParseMapping(const char* line, uintptr_t address, MTADiagMapping& mapping)
{
   const char* source = line;
   uintptr_t start = 0;
   uintptr_t end = 0;
   if (!MTADiagHexValue(source, start) || *source != '-')
      return false;

   source++;
   if (!MTADiagHexValue(source, end) || address < start || address >= end)
      return false;

   while (*source == ' ')
      source++;

   mapping.start = start;
   mapping.end = end;
   for (size_t i = 0; i < 4; i++)
      mapping.perms[i] = source[i];
   mapping.perms[4] = 0;
   std::strncpy(mapping.line, line, sizeof(mapping.line) - 1);
   mapping.line[sizeof(mapping.line) - 1] = 0;

   return true;
}

static bool MTADiagFindMapping(uintptr_t address, MTADiagMapping& mapping)
{
   int file = ::open("/proc/self/maps", O_RDONLY);
   if (file < 0)
      return false;

   char buffer[4096];
   char line[384];
   size_t lineLength = 0;
   ssize_t readCount = 0;
   while ((readCount = ::read(file, buffer, sizeof(buffer))) > 0) {
      for (ssize_t i = 0; i < readCount; i++) {
         if (buffer[i] == '\n') {
            line[lineLength] = 0;
            if (MTADiagParseMapping(line, address, mapping)) {
               ::close(file);

               return true;
            }
            lineLength = 0;
         }
         else if (lineLength + 1 < sizeof(line)) {
            line[lineLength++] = buffer[i];
         }
      }
   }

   ::close(file);

   return false;
}

static const char* MTADiagClassify(uintptr_t address, const MTADiagMapping* ownerMapping,
   MTADiagMapping& mapping)
{
   if (!MTADiagFindMapping(address, mapping))
      return "unmapped";

   if (ownerMapping && mapping.start == ownerMapping->start && mapping.end == ownerMapping->end)
      return "owner-stack";

   if (mapping.perms[3] == 's')
      return "elena-heap";

   return "other-map";
}

static void MTADiagOne(size_t step, const char* field, uintptr_t container, uintptr_t segment,
   const MTADiagMapping* ownerMapping)
{
   MTADiagMapping mapping {};
   const char* location = MTADiagClassify(container, ownerMapping, mapping);
   if (mapping.end) {
      dprintf(STDERR_FILENO,
         "[mta-one] step=%zu field=%s container=%p segment=%p class=%s map=%s\n",
         step, field, (void*)container, (void*)segment, location, mapping.line);
   }
   else {
      dprintf(STDERR_FILENO,
         "[mta-one] step=%zu field=%s container=%p segment=%p class=%s\n",
         step, field, (void*)container, (void*)segment, location);
   }
}

static void MTADiagWalk(ThreadContent* contentAddress, const ThreadContent& content,
   uintptr_t faultSegment)
{
   MTADiagMapping ownerMapping {};
   MTADiagMapping* owner = nullptr;
   if (MTADiagFindMapping(content.tt_stack_root, ownerMapping)
      || MTADiagFindMapping(content.tt_stack_frame, ownerMapping))
   {
      owner = &ownerMapping;
      dprintf(STDERR_FILENO, "[mta-owner] root=%p map=%s\n",
         (void*)content.tt_stack_root, ownerMapping.line);
   }

   uintptr_t node = content.tt_stack_frame;
   uintptr_t segment = node;
   uintptr_t source = (uintptr_t)contentAddress + offsetof(ThreadContent, tt_stack_frame);
   const char* sourceField = "head";

   for (size_t step = 0; step < 128 && node; step++) {
      if (node == 1) {
         MTADiagOne(step, sourceField, source, segment, owner);
         break;
      }

      uintptr_t link = 0;
      if (!MTADiagRead(node, &link, sizeof(link))) {
         MTADiagMapping mapping {};
         const char* location = MTADiagClassify(node, owner, mapping);
         dprintf(STDERR_FILENO,
            "[mta-chain] step=%zu segment=%p fault_segment=%d node=%p read=EFAULT class=%s\n",
            step, (void*)segment, segment == faultSegment, (void*)node, location);
         break;
      }

      MTADiagMapping mapping {};
      const char* location = MTADiagClassify(node, owner, mapping);
      dprintf(STDERR_FILENO,
         "[mta-chain] step=%zu segment=%p fault_segment=%d node=%p link=%p class=%s\n",
         step, (void*)segment, segment == faultSegment, (void*)node, (void*)link, location);

      source = node;
      sourceField = "link";
      if (link == 1) {
         MTADiagOne(step, sourceField, source, segment, owner);
         break;
      }
      if (link) {
         node = link;
         continue;
      }

      uintptr_t continuationAddress = node + sizeof(uintptr_t);
      uintptr_t continuation = 0;
      if (!MTADiagRead(continuationAddress, &continuation, sizeof(continuation))) {
         dprintf(STDERR_FILENO,
            "[mta-chain] step=%zu terminator=%p continuation_read=EFAULT\n",
            step, (void*)node);
         break;
      }

      dprintf(STDERR_FILENO,
         "[mta-chain] step=%zu terminator=%p continuation=%p\n",
         step, (void*)node, (void*)continuation);
      source = continuationAddress;
      sourceField = "continuation";
      if (continuation == 1) {
         MTADiagOne(step, sourceField, source, segment, owner);
         break;
      }

      node = continuation;
      segment = continuation;
   }
}

static void MTADiagFault(int sig, siginfo_t* si, ucontext_t* u)
{
   uintptr_t rip = u->uc_mcontext.gregs[REG_RIP];
   uintptr_t rax = u->uc_mcontext.gregs[REG_RAX];
   uintptr_t rbx = u->uc_mcontext.gregs[REG_RBX];
   uintptr_t rcx = u->uc_mcontext.gregs[REG_RCX];
   uintptr_t rsi = u->uc_mcontext.gregs[REG_RSI];
   uintptr_t r8 = u->uc_mcontext.gregs[REG_R8];
   dprintf(STDERR_FILENO,
      "[mta-signal] sig=%d si_addr=%p si_code=%d rip=%p rax=%p rbx=%zu rsi=%p rcx=%p r8=%p\n",
      sig, si->si_addr, si->si_code, (void*)rip, (void*)rax, (size_t)rbx,
      (void*)rsi, (void*)rcx, (void*)r8);

   Dl_info signalInfo {};
   bool signalResolved = dladdr((void*)rip, &signalInfo) != 0;
   dprintf(STDERR_FILENO, "[mta-signal-image] image=%s symbol=%s offset=%p\n",
      signalResolved && signalInfo.dli_fname ? signalInfo.dli_fname : "?",
      signalResolved && signalInfo.dli_sname ? signalInfo.dli_sname : "?",
      signalResolved && signalInfo.dli_saddr ? (void*)(rip - (uintptr_t)signalInfo.dli_saddr) : nullptr);

   if (sig != SIGSEGV || rip < 0x40046B || rip > 0x400497 || rbx >= 0x200)
   {
      return;
   }

   Dl_info info {};
   bool resolved = dladdr((void*)rip, &info) != 0;
   dprintf(STDERR_FILENO,
      "[mta-fault] sig=%d si_addr=%p si_code=%d rip=%p image=%s symbol=%s rax=%p rbx=%zu rsi=%p rcx=%p r8=%p\n",
      sig, si->si_addr, si->si_code, (void*)rip,
      resolved && info.dli_fname ? info.dli_fname : "?",
      resolved && info.dli_sname ? info.dli_sname : "?",
      (void*)rax, (size_t)rbx, (void*)rsi, (void*)rcx, (void*)r8);

   ThreadSlot slot {};
   if (!MTADiagRead(r8, &slot, sizeof(slot))) {
      dprintf(STDERR_FILENO, "[mta-slot] address=%p index=%zu read=EFAULT\n",
         (void*)r8, (size_t)rbx);
      return;
   }

   dprintf(STDERR_FILENO, "[mta-slot] address=%p index=%zu content=%p arg=%p\n",
      (void*)r8, (size_t)rbx, (void*)slot.content, slot.arg);

   ThreadContent content {};
   if (!MTADiagRead((uintptr_t)slot.content, &content, sizeof(content))) {
      dprintf(STDERR_FILENO, "[mta-content] address=%p read=EFAULT\n", (void*)slot.content);
      return;
   }

   dprintf(STDERR_FILENO,
      "[mta-content] address=%p critical=%p current=%p head=%p event=%p flags=%zu root=%p\n",
      (void*)slot.content, (void*)content.eh_critical, (void*)content.eh_current,
      (void*)content.tt_stack_frame, content.tt_sync_event, content.tt_flags,
      (void*)content.tt_stack_root);
   MTADiagWalk(slot.content, content, rcx);
}

#endif

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


#if defined(MTA_DIAG)

   MTADiagFault(sig, si, u);

#endif

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


}

void SystemRoutineProvider::ClearMTASignals(SystemEnv* env, size_t index)
{
   EventImpl* event = (EventImpl*)env->th_table->slots[index].content->tt_sync_event;


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

void SystemRoutineProvider::GCSignalClear(void* handle)
{

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


   // ; wake on every collection end rather than only when nothing runs : with the storm
   // ; of back to back collections the gc_signal == 0 window is too short to observe and
   // ; the sleepers starve. Leaving into a running collection is safe, the thread still
   // ; carries tt_flags = 1 and its frame stays frozen until the resume passes gc_lock
   size_t generation = CollectionGeneration;
   while (CollectionGeneration == generation && table->gc_signal != 0)
      pthread_cond_wait(&CollectionCond, &CollectionMutex);


   pthread_mutex_unlock(&CollectionMutex);
}

void SystemRoutineProvider::GCSignalCollectionEnd()
{
   // ; the collector clears gc_signal before getting here, outside this mutex. Taking the
   // ; mutex orders that store against the waiters : whoever is inside the wait re-checks
   // ; gc_signal with the mutex held and either sees the zero or gets the broadcast
   pthread_mutex_lock(&CollectionMutex);
   CollectionGeneration++;
   pthread_mutex_unlock(&CollectionMutex);

   pthread_cond_broadcast(&CollectionCond);
}
