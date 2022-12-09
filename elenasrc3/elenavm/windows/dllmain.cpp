#include "elena.h"
#include "vmcommon.h"
// --------------------------------------------------------------------------------
#include <windows.h>

using namespace elena_lang;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

void init()
{
}

// --- API export ---

EXTERN_DLL_EXPORT void InitializeVMSTLA(/*SystemEnv* env, SymbolList* entryList, void* criricalHandler*/)
{
}

EXTERN_DLL_EXPORT void ExitLA(int retVal)
{
}

EXTERN_DLL_EXPORT void* CollectGCLA(void* roots, size_t size)
{
   // !! temporal
   return nullptr;
}

EXTERN_DLL_EXPORT size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   // !! temporal
   return 0;
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
