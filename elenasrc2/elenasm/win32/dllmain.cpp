#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"

#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

_ELENA_::Map<int, _ELENA_::Session*> Sessions(NULL, _ELENA_::freeobj);

void newSession(int processId)
{
   Sessions.add(processId, new _ELENA_::Session());
}

void freeSession(int processId)
{
//   Sessions.erase(processId);
}

// === dll entries ===

EXTERN_DLL_EXPORT int TranslateLVMTape(const wchar16_t* script)
{
   _ELENA_::Session* session = Sessions.get(::GetCurrentProcessId());
   
   return session->translate(script);
}

EXTERN_DLL_EXPORT int TranslateLVMFile(const wchar16_t* path, int encoding, bool autoDetect)
{
   _ELENA_::Session* session = Sessions.get(::GetCurrentProcessId());
   
   return session->translate(path, encoding, autoDetect);
}

EXTERN_DLL_EXPORT const wchar16_t* GetLSMStatus()
{
   _ELENA_::Session* session = Sessions.get(::GetCurrentProcessId());

   return session ? session->getLastError() : NULL;
}

// --- dllmain ---

extern "C"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
               )
{
   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
      newSession(::GetCurrentProcessId());
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeSession(::GetCurrentProcessId());
      break;
   }
   return TRUE;
}
