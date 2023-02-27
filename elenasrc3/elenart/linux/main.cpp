//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                             (C)2021-2023, by Aleksey Rakov
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

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

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

   __routineProvider.InitExceptionHandling(env, criricalHandler);

   machine->startSTA(env, entryList);
}

void* CollectGCLA(void* roots, size_t size)
{
//   printf("CollectGCLA %llx %llx\n", (long long)roots, size);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size);
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

void ExitLA(int retVal)
{
   if (retVal) {
      printf("\nAborted:%x\n", retVal);
      fflush(stdout);
   }
   __routineProvider.Exit(retVal);
}
