#include "elena.h"
// --------------------------------------------------------------------------
#include "instance.h"
#include <windows.h>
#include "pehelper.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

using namespace _ELENA_;

static Instance* instance = NULL;

// ==== DLL entries ====

EXTERN_DLL_EXPORT void* Init(void* debugSection, const wchar16_t* package)
{
   if (instance) {
      if (debugSection == NULL) {
         Instance::ImageSection section;
         section.init((void*)0x400000, 0x1000);

         size_t ptr = 0;
         PEHelper::seekSection(MemoryReader(&section), ".debug", ptr);
         debugSection = (void*)ptr;
      }

      instance->init(debugSection, package);

      return instance;
   }
   else return 0;
}

EXTERN_DLL_EXPORT int LoadAddressInfo(void* instance, size_t retPoint, wchar16_t* lineInfo, int length)
{
   return ((Instance*)instance)->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadClassName(void* instance, void* object, wchar16_t* lineInfo, int length)
{
   // !! terminator code
   return 0;
}

EXTERN_DLL_EXPORT void* GetSymbolRef(void* instance, void* referenceName)
{
   // !! terminator code
   return NULL;
}

EXTERN_DLL_EXPORT void* Interpreter(void* instance, void* tape)
{
   // !! terminator code
   return NULL;
}

EXTERN_DLL_EXPORT void* GetRTLastError(void* instance, void* retVal)
{
   // !! terminator code
   return NULL;
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
      instance = new Instance();
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeobj(instance);
      break;
   }
   return TRUE;
}
