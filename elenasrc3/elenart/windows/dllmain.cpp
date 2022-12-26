#include <cstdio> // !! temporal

#include "elena.h"
#include "rtcommon.h"
#include "elenartmachine.h"
#include "windows/winconsts.h"
// --------------------------------------------------------------------------------
#include <windows.h>

using namespace elena_lang;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

void init()
{
   // !! temporal : hard-coded constants
   machine = new ELENARTMachine(__routineProvider.RetrieveMDataPtr((void*)IMAGE_BASE, 0x1000000));
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeSTLA(SystemEnv* env, SymbolList* entry, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeSTA.6 %x,%x\n", (int)env, (int)criricalHandler);

   fflush(stdout);
#endif

   __routineProvider.InitExceptionHandling(env, criricalHandler);

   machine->startSTA(env, entry);
}

EXTERN_DLL_EXPORT void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }

   __routineProvider.Exit(retVal);
}

EXTERN_DLL_EXPORT void* CollectGCLA(void* roots, size_t size)
{
//   printf("CollectGCLA %llx %llx\n", (long long)roots, size);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size);
}

EXTERN_DLL_EXPORT size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
       case DLL_PROCESS_ATTACH:
          init();
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

