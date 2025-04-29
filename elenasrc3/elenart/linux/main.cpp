//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include <unistd.h>

#include "elenart.h"
#include "elenartmachine.h"
#include "linux/lnxconsts.h"

using namespace elena_lang;

#define ROOT_PATH          "/usr/lib/elena"
#define CONFIG_PATH        "/etc/elena/elenart60.config"

#if defined(__x86_64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86_64;

#elif defined(__i386__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_x86;

#elif defined(__PPC64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_PPC64le;

#elif defined(__aarch64__)

constexpr auto CURRENT_PLATFORM           = PlatformType::Linux_ARM64;

#endif // defined

extern char** environ;

//#define DEBUG_OUTPUT 1

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

static int    __argc = 0;
static char** __argv = nullptr;

void getSelfPath(PathString& rootPath)
{
   char buff[FILENAME_MAX];
   size_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
   if (len != -1) {
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

void* CollectGCLA(void* roots, size_t size)
{
   //printf("CollectGCLA %llx %llx\n", (long long)roots, size);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size, false);
}

size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
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
}

void ExitLA(int retVal)
{
   if (retVal) {
      printf("\nAborted:%x\n", retVal);
      fflush(stdout);
   }
   __routineProvider.Exit(retVal);
}
