#include <cstdio> // !! temporal

#include "elena.h"
#include "rtcommon.h"
#include "elenartmachine.h"
#include "windows/winconsts.h"
// --------------------------------------------------------------------------------
#include <windows.h>

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86;

#elif _M_X64

constexpr auto CURRENT_PLATFORM = PlatformType::Win_x86_64;

#endif

constexpr auto CONFIG_FILE = "elenart60.cfg";

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

void loadModulePath(HMODULE hModule, PathString& rootPath, bool includeName)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   if (includeName) {
      rootPath.copy(path);
   }
   else rootPath.copySubPath(path, false);

   rootPath.lower();
}

void init(HMODULE hModule)
{
   // get DLL path
   PathString rootPath;
   loadModulePath(hModule, rootPath, false);

   // get EXE path
   PathString execPath;
   loadModulePath(0, execPath, true);

   PathString configPath(CONFIG_FILE);

   // !! temporal : hard-coded constants
   machine = new ELENARTMachine(
      *rootPath, *execPath, *configPath, CURRENT_PLATFORM,
      __routineProvider.RetrieveMDataPtr(UInt32ToPtr(IMAGE_BASE), 0x1000000));
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeSTLA(SystemEnv* env, void* entry, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeSTA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   __routineProvider.InitSTAExceptionHandling(env, criricalHandler);

   machine->startApp(env, entry);
}

EXTERN_DLL_EXPORT void InitializeMTLA(SystemEnv* env, SymbolList* entry, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeMTA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   size_t index = machine->allocateThreadEntry(env);
   __routineProvider.InitMTAExceptionHandling(env, index, criricalHandler);
   __routineProvider.InitMTASignals(env, index);

   machine->startApp(env, entry);
}

EXTERN_DLL_EXPORT void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }

   __routineProvider.Exit(retVal);
}

// NOTE : arg must be unique for every separate thread
EXTERN_DLL_EXPORT void* CreateThreadLA(void* arg, void* threadProc, int stackSize, int flags)
{
   return machine->allocateThread(systemEnv, arg, threadProc, stackSize, flags);
}

EXTERN_DLL_EXPORT void InitThreadLA(SystemEnv* env, void* criricalHandler, int index)
{
   __routineProvider.InitMTAExceptionHandling(env, index, criricalHandler);
   __routineProvider.InitMTASignals(env, index);
}

EXTERN_DLL_EXPORT void StartThreadLA(SystemEnv* env, void* entryPoint, int index)
{
#ifdef DEBUG_OUTPUT
   printf("StartThreadLA.6 %x\n", (int)env);

   fflush(stdout);
#endif

   machine->startThread(env, entryPoint, index);
}

EXTERN_DLL_EXPORT void UninitThreadLA(SystemEnv* env, int index)
{
   __routineProvider.ClearMTASignals(env, index);
   machine->clearThreadEntry(env, index);
}

EXTERN_DLL_EXPORT void ExitThreadLA(int errCode)
{
   return __routineProvider.ExitThread(errCode);
}

EXTERN_DLL_EXPORT void* CollectPermGCLA(size_t size)
{
   return __routineProvider.GCRoutinePerm(systemEnv->gc_table, size);
}

EXTERN_DLL_EXPORT void* CollectGCLA(void* roots, size_t size, bool fullMode)
{
//   printf("CollectGCLA %llx %llx\n", (long long)roots, size);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size, fullMode);
}

EXTERN_DLL_EXPORT size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
}

EXTERN_DLL_EXPORT size_t LoadActionNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadActionName((mssg_t)message, buffer, length);
}

EXTERN_DLL_EXPORT size_t LoadCallStackLA(uintptr_t framePtr, uintptr_t* list, size_t length)
{
   return __routineProvider.LoadCallStack(framePtr, list, length);
}

EXTERN_DLL_EXPORT size_t LoadAddressInfoLM(size_t retPoint, char* lineInfo, size_t length)
{
   return machine->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT addr_t LoadSymbolByStringLA(const char* symbolName)
{
   return machine->loadSymbol(symbolName);
}

EXTERN_DLL_EXPORT addr_t LoadClassByStringLA(const char* symbolName)
{
   return machine->loadClassReference(symbolName);
}

EXTERN_DLL_EXPORT addr_t LoadClassByBufferLA(void* referenceName, size_t index, size_t length)
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

EXTERN_DLL_EXPORT addr_t LoadSymbolByString2LA(const char* ns, const char* symbolName)
{
   ReferenceName fullName(ns, symbolName);

   return machine->loadSymbol(*fullName);
}

EXTERN_DLL_EXPORT mssg_t LoadMessageLA(const char* messageName)
{
   return machine->loadMessage(messageName);
}

EXTERN_DLL_EXPORT mssg_t LoadActionLA(const char* actionName)
{
   return machine->loadAction(actionName);
}

/// <summary>
/// Fills the passed dispatch list with references to extension message overload list
/// </summary>
/// <param name="moduleList">List of imported modules separated by semicolon</param>
/// <param name="message">Extension message</param>
/// <param name="output">Dispatch list</param>
/// <returns></returns>
EXTERN_DLL_EXPORT int LoadExtensionDispatcherLA(const char* moduleList, mssg_t message, void* output)
{
   return machine->loadExtensionDispatcher(moduleList, message, output);
}

EXTERN_DLL_EXPORT size_t LoadClassMessagesLA(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   return machine->loadClassMessages(classPtr, output, skip, maxLength);
}

EXTERN_DLL_EXPORT bool CheckClassMessageLA(void* classPtr, mssg_t message)
{
   return machine->checkClassMessage(classPtr, message);
}

EXTERN_DLL_EXPORT void GetRandomSeedLA(SeedStruct& seed)
{
   machine->initRandomSeed(seed);
}

EXTERN_DLL_EXPORT unsigned int GetRandomIntLA(SeedStruct& seed)
{
   return machine->getRandomNumber(seed);
}

EXTERN_DLL_EXPORT void SignalStopGCLA(void* handle)
{
   SystemRoutineProvider::GCSignalStop(handle);
}

EXTERN_DLL_EXPORT void WaitForSignalGCLA(void* handle)
{
   SystemRoutineProvider::GCWaitForSignal(handle);
}

EXTERN_DLL_EXPORT void SignalClearGCLA(void* handle)
{
   SystemRoutineProvider::GCSignalClear(handle);
}

EXTERN_DLL_EXPORT void WaitForSignalsGCLA(size_t count, void* handles)
{
   SystemRoutineProvider::GCWaitForSignals(count, handles);
}

/// <summary>
/// Inject an interface VMT into a dynamic proxy class
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
EXTERN_DLL_EXPORT void* InjectProxyTypeLA(void* target, void* type, int staticLength, int nameIndex)
{
   return (void*)machine->injectType(systemEnv, target, type, staticLength, nameIndex);
}

/// <summary>
/// Returns the signature list
/// </summary>
/// <param name="message">A strong-typed message</param>
/// <param name="output">Signature tyoe</param>
/// <returns>the total length</returns>
EXTERN_DLL_EXPORT int LoadSignatureLA(mssg_t message, addr_t* output, int maximalLength)
{
   return machine->loadSignature(message, output, maximalLength);
}

EXTERN_DLL_EXPORT void GetGCStatisticsLA(GCStatistics* statistics)
{
   SystemRoutineProvider::CalcGCStatistics(systemEnv, statistics);
}

EXTERN_DLL_EXPORT void ResetGCStatisticsLA()
{
   SystemRoutineProvider::ResetGCStatistics();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
       case DLL_PROCESS_ATTACH:
          init(hModule);
          break;
       case DLL_THREAD_ATTACH:
       case DLL_THREAD_DETACH:
       case DLL_PROCESS_DETACH:
           break;
       default:
          // to make compiler happy
          break;
    }
    return TRUE;
}

