//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                         DLL Main Entry
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include <unistd.h>
#include <sys/uio.h>

#include "elenart.h"
#include "elenartmachine.h"
#include "linux/lnxconsts.h"

using namespace elena_lang;

#if defined(__FreeBSD__)

#define ROOT_PATH          "/usr/local/lib/elena"

#define CONFIG_PATH        "/usr/local/etc/elena/elenart60.config"

#else

#define ROOT_PATH          "/usr/lib/elena"

#define CONFIG_PATH        "/etc/elena/elenart60.config"

#endif


#if defined(__x86_64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86_64;

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

#endif // defined

//#define DEBUG_OUTPUT 1

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

static int    __argc = 0;
static char** __argv = nullptr;

void getSelfPath(PathString& rootPath)
{
   char buff[FILENAME_MAX];
   size_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
   if (len != NOTFOUND_POS) {
      buff[len] = 0;
      rootPath.copy(buff);
   }
   /* handle error condition */
}

void init()
{
   PathString execPath;
   getSelfPath(execPath);

   machine = new ELENARTMachine(
      ROOT_PATH, *execPath, CONFIG_PATH, CURRENT_PLATFORM,
      __routineProvider.RetrieveMDataPtr((void*)IMAGE_BASE, 0x1000000));
}

void InitializeSTLA(SystemEnv* env, SymbolList* entryList, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeSTA.6 %llx,%llx,%llx\n", (long long)env, (long long)entryList, (long long)criricalHandler);
   fflush(stdout);
#endif

   if (machine == nullptr)
      init();

   __routineProvider.InitSTAExceptionHandling(env, criricalHandler);

   machine->startApp(env, entryList);
}

void InitializeMTLA(SystemEnv* env, SymbolList* entryList, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeMTA.6 %llx,%llx,%llx\n", (long long)env, (long long)entryList, (long long)criricalHandler);
   fflush(stdout);
#endif

   if (machine == nullptr)
      init();

   size_t index = machine->allocateThreadEntry(env);
   __routineProvider.InitMTAExceptionHandling(env, index, criricalHandler);
   __routineProvider.InitMTASignals(env, index);

   machine->startApp(env, entryList);
}

// ; TEMPORARY DIAGNOSTIC, remove before the PR.
// ; audits the frame chain of every thread at the moment the collector receives the roots,
// ; which is where a corrupt chain becomes a corrupt scan. The fault handler in
// ; lnxroutines.cpp only sees the thread that faulted, and only after the damage
static size_t MTAAuditCollection = 0;
static uintptr_t MTAAuditFrame[0x200] = {};
static size_t MTAAuditStuck[0x200] = {};
static uintptr_t MTAAuditLink[0x200] = {};

static bool MTAAuditRead(uintptr_t address, void* target, size_t length)
{
   iovec local { target, length };
   iovec remote { (void*)address, length };

   return process_vm_readv(getpid(), &local, 1, &remote, 1, 0) == (ssize_t)length;
}

static void MTAAuditChains(SystemEnv* env)
{
   ThreadTable* table = env->th_table;
   if (!table)
      return;

   size_t collection = ++MTAAuditCollection;
   size_t counter = table->counter;
   if (counter > 0x200)
      counter = 0x200;

   for (size_t i = 0; i < counter; i++) {
      ThreadContent* content = table->slots[i].content;
      if (!content)
         continue;

      uintptr_t frame = content->tt_stack_frame;
      size_t flags = content->tt_flags;

      // ; a thread that leaves an excluded region through the exception unwind never runs
      // ; the include, so the flag stays one and the head keeps pointing at a node the
      // ; unwind has already dropped. A legitimate excluded region is short : it does not
      // ; survive several consecutive collections with the very same head
      if (flags == 1 && frame && frame == MTAAuditFrame[i]) {
         MTAAuditStuck[i]++;
      }
      else MTAAuditStuck[i] = 0;

      MTAAuditFrame[i] = frame;

      if (!frame)
         continue;

      uintptr_t link = 0;
      bool readable = MTAAuditRead(frame, &link, sizeof(link));

      const char* verdict = nullptr;
      if (!readable) {
         verdict = "head-unreadable";
      }
      else if (link == 1 || (link & (sizeof(uintptr_t) - 1)) != 0) {
         verdict = "link-not-a-pointer";
      }
      else if (link && link < frame) {
         // ; a descending link makes rsi - rcx negative, read as a huge size
         verdict = "link-descends";
      }
      else if (link && content->tt_stack_root
         && (link < frame || link > content->tt_stack_root))
      {
         // ; every link of a chain has to sit in this thread own stack, between the head
         // ; and the root published at startup. A link outside it is not a frame at all
         verdict = "link-off-stack";
      }
      else if (MTAAuditStuck[i] >= 3 && link != MTAAuditLink[i]) {
         // ; the head did not move but the word it points at did. A thread parked inside a
         // ; legitimate excluded region holds a frozen frame : this node is being written
         // ; over, so the head describes stack the thread has already dropped
         verdict = "head-overwritten";
      }

      if (MTAAuditStuck[i] == 0)
         MTAAuditLink[i] = link;

      if (verdict) {
         dprintf(STDERR_FILENO,
            "[mta-audit] collect=%zu slot=%zu flags=%zu frame=%p link=%p root=%p stuck=%zu %s\n",
            collection, i, flags, (void*)frame, (void*)link,
            (void*)content->tt_stack_root, MTAAuditStuck[i], verdict);
      }
   }
}

void* CollectGCLA(void* roots, size_t size, bool fullMode)
{
   MTAAuditChains(systemEnv);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size, fullMode);
}

void* CollectPermGCLA(size_t size)
{
   return __routineProvider.GCRoutinePerm(systemEnv->gc_table, size);
}

size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length, (message & PREFIX_MESSAGE_MASK) == CONVERSION_MESSAGE);
}

size_t LoadStrongMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length, true);
}

size_t LoadCallStackLA(uintptr_t framePtr, uintptr_t* list, size_t length)
{
   return __routineProvider.LoadCallStack(framePtr, list, length);
}

size_t LoadAddressInfoLM(size_t retPoint, char* lineInfo, size_t length)
{
   return machine->loadAddressInfo(retPoint, lineInfo, length);
}

addr_t LoadSymbolByStringLA(const char* symbolName)
{
   return machine->loadSymbol(symbolName);
}

addr_t LoadClassByStringLA(const char* symbolName)
{
   return machine->loadClassReference(symbolName);
}

addr_t LoadSymbolByString2LA(const char* ns, const char* symbolName)
{
   ReferenceName fullName(ns, symbolName);

   return machine->loadSymbol(*fullName);
}

mssg_t LoadMessageLA(const char* messageName)
{
   return machine->loadMessage(messageName);
}

/// <summary>
/// Inject an interface VMT into a dynamic proxy class
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
void* InjectProxyTypeLA(void* target, void* type, int staticLength, int nameIndex)
{
#ifdef DEBUG_OUTPUT
   printf("InjectProxyTypeLA %llx %llx %x %x\n", (long long)target, (long long)type, staticLength, nameIndex);
#endif

   return (void*)machine->injectType(systemEnv, target, type, staticLength, nameIndex);
}

/// <summary>
/// Returns the signature list
/// </summary>
/// <param name="message">A strong-typed message</param>
/// <param name="output">Signature tyoe</param>
/// <returns>the total length</returns>
int LoadSignatureLA(mssg_t message, addr_t* output, int maximalLength)
{
   return machine->loadSignature(message, output, maximalLength);
}

void GetRandomSeedLA(SeedStruct& seed)
{
   machine->initRandomSeed(seed);
}

unsigned int GetRandomIntLA(SeedStruct& seed)
{
   return machine->getRandomNumber(seed);
}

int GetArgCLA()
{
   return __argc;
}

int GetArgLA(int index, char* buffer, int length)
{
   if (index < 0 || index >= __argc) {
      buffer[0] = 0;

      return 0;
   }

   for (int i = 0; i < length; i++) {
      char tmp = __argv[index][i];

      buffer[i] = tmp;

      if (!tmp) {
         return i;
      }
   }

   return length;
}

void PrepareLA(uintptr_t arg)
{
   __argc = *(int*)arg;

#if defined __PPC64__

   uintptr_t argptr = *(uintptr_t*)(arg + sizeof(uintptr_t));
   __argv = (char**)argptr;

#else

   __argv = (char**)(arg + sizeof(uintptr_t));

#endif

#if defined __FreeBSD__

   __progname = __argv[0];

#endif 
}

void ExitLA(int retVal)
{
   if (retVal) {
      printf("\nAborted:%x\n", retVal);
      fflush(stdout);
   }
   __routineProvider.Exit(retVal);
}

void GetGCStatisticsLA(GCStatistics* statistics)
{
   SystemRoutineProvider::CalcGCStatistics(systemEnv, statistics);
}

void ResetGCStatisticsLA()
{
   SystemRoutineProvider::ResetGCStatistics();
}

/// <summary>
/// Fills the passed dispatch list with references to extension message overload list
/// </summary>
/// <param name="moduleList">List of imported modules separated by semicolon</param>
/// <param name="message">Extension message</param>
/// <param name="output">Dispatch list</param>
/// <returns></returns>
int LoadExtensionDispatcherLA(const char* moduleList, mssg_t message, void* output)
{
   return machine->loadExtensionDispatcher(moduleList, message, output);
}

size_t LoadActionNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadActionName((mssg_t)message, buffer, length);
}

addr_t LoadClassByBufferLA(void* referenceName, size_t index, size_t length)
{
   if (length < 0x100) {
      IdentifierString str((const char*)referenceName + index, length);

      return LoadClassByStringLA(*str);
   }
   else {
      DynamicString<char> str((const char*)referenceName, index, length);

      return LoadClassByStringLA(str.str());
   }
}

mssg_t LoadActionLA(const char* actionName)
{
   return machine->loadAction(actionName);
}

size_t LoadClassMessagesLA(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   return machine->loadClassMessages(classPtr, output, skip, maxLength);
}

void* LoadMessageOutputLA(void* classPtr, mssg_t message)
{
   return machine->loadClassMessageOutput(classPtr, message);
}

bool CheckClassMessageLA(void* classPtr, mssg_t message)
{
   return machine->checkClassMessage(classPtr, message);
}

mssg_t LoadStrongMessageLA(const char* messageName)
{
   return machine->loadStrongMessage(messageName);
}

void* CreateThreadLA(void* arg, void* threadProc, int stackSize, int flags)
{
   return machine->allocateThread(systemEnv, arg, threadProc, stackSize, flags);
}

void InitThreadLA(SystemEnv* env, void* criricalHandler, int index)
{
   __routineProvider.InitMTAExceptionHandling(env, index, criricalHandler);
   __routineProvider.InitMTASignals(env, index);
}

void StartThreadLA(SystemEnv* env, void* entryPoint, int index)
{
#ifdef DEBUG_OUTPUT
   printf("StartThreadLA.6 %x\n", (int)env);

   fflush(stdout);
#endif

   machine->startThread(env, entryPoint, index);
}

void UninitThreadLA(SystemEnv* env, int index)
{
   __routineProvider.ClearMTASignals(env, index);
   machine->clearThreadEntry(env, index);
}

void ExitThreadLA(int errCode)
{
   return __routineProvider.ExitThread(errCode);
}

void SignalStopGCLA(void* handle)
{
   SystemRoutineProvider::GCSignalStop(handle);
}

void SignalClearGCLA(void* handle)
{
   SystemRoutineProvider::GCSignalClear(handle);
}

void WaitForSignalsGCLA(size_t count, void* handles)
{
   SystemRoutineProvider::GCWaitForSignals(count, handles);
}

// ; the collection barrier, see the comment in engine/linux/lnxroutines.cpp. No handle
// ; is passed : the condition is the collection state
void WaitForCollectionGCLA()
{
   SystemRoutineProvider::GCWaitForCollection(systemEnv->gc_table);
}

void SignalCollectionEndGCLA()
{
   SystemRoutineProvider::GCSignalCollectionEnd();
}
