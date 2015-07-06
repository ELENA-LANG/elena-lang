#include "elena.h"
// --------------------------------------------------------------------------
#include "instance.h"
#include <windows.h>
#include "pehelper.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#define CONFIG_PATH "elc.cfg"

using namespace _ELENA_;

static Instance* instance = NULL;

void loadDLLPath(HMODULE hModule, Path& rootPath)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);
   rootPath.lower();
}

// ==== DLL entries ====

EXTERN_DLL_EXPORT void* Init(void* debugSection, ident_t package)
{
   if (instance) {
      if (debugSection == NULL) {
         Instance::ImageSection section;
         section.init((void*)0x400000, 0x1000);

         size_t ptr = 0;
         PEHelper::seekSection(MemoryReader(&section), ".debug", ptr);
         debugSection = (void*)ptr;
      }

      Path configPath;
      Path::loadPath(configPath, CONFIG_PATH);
      instance->init(debugSection, package, configPath);

      return instance;
   }
   else return 0;
}

EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   return ((Instance*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
}

EXTERN_DLL_EXPORT int LoadAddressInfo(void* instance, size_t retPoint, ident_c* lineInfo, int length)
{
   return ((Instance*)instance)->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadClassName(void* instance, void* object, ident_c* buffer, int length)
{
   return ((Instance*)instance)->loadClassName((size_t)object, buffer, length);
}

EXTERN_DLL_EXPORT void* GetSymbolRef(void* instance, void* referenceName)
{
   return ((Instance*)instance)->loadSymbol((ident_t)referenceName);
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

EXTERN_DLL_EXPORT int LoadSubjectName(void* instance, void* subject, ident_c* lineInfo, int length)
{
   return ((Instance*)instance)->loadSubjectName((size_t)subject, lineInfo, length);
}

EXTERN_DLL_EXPORT void* LoadSubject(void* instance, void* subjectName)
{
   return ((Instance*)instance)->loadSubject((ident_t)subjectName);
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
   {
      Path rootPath;
      loadDLLPath(hModule, rootPath);

      instance = new Instance(rootPath);
      return TRUE;
   }
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeobj(instance);
      break;
   }
   return TRUE;
}
