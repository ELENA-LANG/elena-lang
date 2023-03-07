#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptmachine.h"
#include <windows.h>

using namespace elena_lang;

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

static ScriptEngine* engine = nullptr;

void init(HMODULE hModule)
{
   engine = new ScriptEngine();
}

EXTERN_DLL_EXPORT int NewScope()
{
   if (engine) {
      return engine->newScope();
   }
   else return -1;
}


BOOL APIENTRY DllMain(HMODULE hModule,
   DWORD  ul_reason_for_call,
   LPVOID lpReserved
   )
{
   switch (ul_reason_for_call)
   {
      case DLL_PROCESS_ATTACH:
         init(hModule);
         return TRUE;
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
         return TRUE;
      case DLL_PROCESS_DETACH:
         break;
   }
   return TRUE;
}
