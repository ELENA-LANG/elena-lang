#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include <windows.h>
#include "pehelper.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#define CONFIG_PATH "elenart.cfg"

using namespace _ELENA_;

static ELENARTMachine* Instance = NULL;

EXTERN_DLL_EXPORT void InitializeSTA(void* systeEnv, void* entryPoint)
{
   Instance->startSTA(systeEnv, entryPoint);
}

// !!

void loadDLLPath(HMODULE hModule, Path& rootPath)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);
   rootPath.lower();
}

// ==== DLL entries ====

void init(HMODULE hModule)
{
   Path rootPath;
   loadDLLPath(hModule, rootPath);

   Instance = new ELENARTMachine(rootPath.c_str());

   void* debugSection = NULL;
   void* messageSection = NULL;
   ELENARTMachine::ImageSection section;
   section.init((void*)0x400000, 0x1000);

   size_t ptr = 0;
   PEHelper::seekSection(MemoryReader(&section), ".debug", ptr);
   debugSection = (void*)ptr;

   PEHelper::seekSection(MemoryReader(&section), ".mdata", ptr);
   messageSection = (void*)ptr;

   Path configPath(CONFIG_PATH);
   Instance->init(debugSection, messageSection, configPath.c_str());
}

EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   return ((ELENARTMachine*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
}

EXTERN_DLL_EXPORT int LoadAddressInfo(size_t retPoint, char* lineInfo, int length)
{
   return Instance->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadClassName(void* object, char* buffer, int length)
{
   return Instance->loadClassName((size_t)object, buffer, length);
}

EXTERN_DLL_EXPORT void* EvaluateTape(void* tape)
{
   // !! terminator code
   return NULL;
}

EXTERN_DLL_EXPORT void* InterpretTape(void* tape)
{
   // !! terminator code
   return NULL;
}

EXTERN_DLL_EXPORT void* GetVMLastError(void* retVal)
{
   return NULL;
}

EXTERN_DLL_EXPORT int LoadSubjectName(void* subject, char* lineInfo, int length)
{
   return Instance->loadSubjectName((size_t)subject, lineInfo, length);
}

EXTERN_DLL_EXPORT void* LoadSubject(void* subjectName)
{
   return Instance->loadSubject((const char*)subjectName);
}

EXTERN_DLL_EXPORT int LoadMessageName(void* subject, char* lineInfo, int length)
{
   return Instance->loadMessageName((size_t)subject, lineInfo, length);
}

EXTERN_DLL_EXPORT void* LoadMessage(void* messageName)
{
   return Instance->loadMessage((const char*)messageName);
}

EXTERN_DLL_EXPORT void* LoadSymbol(void* referenceName)
{
   return Instance->loadSymbol((const char*)referenceName);
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
      init(hModule);
      return TRUE;
   }
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeobj(Instance);
      break;
   }
   return TRUE;
}
