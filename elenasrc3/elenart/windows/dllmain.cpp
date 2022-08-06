#include <cstdio> // !! temporal

#include "elena.h"
#include "rtcommon.h"
#include "elenartmachine.h"
// --------------------------------------------------------------------------------
#include <windows.h>

using namespace elena_lang;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

static ELENARTMachine* machine;

void init()
{
   machine = new ELENARTMachine();
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeSTA(SystemEnv* env, SymbolList* entryList, ExceptionStruct* ex_struct)
{
   printf("InitializeSTA.4 %x,%x,%x\n", (int)env, (int)entryList, (int)ex_struct);
   fflush(stdout);

   machine->startSTA(env, entryList);
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

