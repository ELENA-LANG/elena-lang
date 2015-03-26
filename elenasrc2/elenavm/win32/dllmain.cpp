#include "elena.h"
// --------------------------------------------------------------------------
#include "x86elenamachine.h"
//#include "evmscope.h"

#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

using namespace _ELENA_;

x86ELENAMachine* machine = NULL;
Path rootPath;

// --- getAppPath ---

void loadDLLPath(HMODULE hModule)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);
   rootPath.lower();
}

Instance* getCurrentInstance()
{
   Instance* instance = machine ? machine->getInstance(::GetCurrentProcessId()) : NULL;

   return instance;
}

// ==== DLL entries ====

EXTERN_DLL_EXPORT int Interpret(void* tape)
{
   //getchar();

   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      return instance->interprete(tape, VM_INTERPRET_EXT);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

      return 0;
   }
   catch(InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException& e)
   {
      return 0;
   }
}

EXTERN_DLL_EXPORT int Evaluate(void* tape)
{
   //getchar();

   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      return instance->interprete(tape, VM_INTERPRET);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

      return 0;
   }
   catch(InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException& e)
   {
      return 0;
   }
}

EXTERN_DLL_EXPORT size_t SetDebugMode()
{
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   instance->setDebugMode();

   return (size_t)instance->loadDebugSection();
}

EXTERN_DLL_EXPORT ident_t GetLVMStatus()
{
   Instance* instance = machine ? machine->getInstance(::GetCurrentProcessId()) : NULL;

   return  instance ? instance->getStatus() : "Not initialized";
}

// --- initmachine ---

void initMachine(path_t rootPath)
{
   machine = new x86ELENAMachine(rootPath);
}

// --- createInstace ---

void createInstance(DWORD processId)
{
   x86Instance* instance = new x86Instance(machine);

   machine->newInstance(processId, instance);
}

// --- freeinstace ---

void freeInstance(DWORD processId)
{
   machine->deleteInstance(processId);

//   if (machine->getInstanceCount() == 0) {
//      freeobj(machine);
//      machine = NULL;
//   }
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
      if (!machine) {
         loadDLLPath(hModule);
         initMachine(rootPath);
      }
      createInstance(::GetCurrentProcessId());
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeInstance(::GetCurrentProcessId());
      break;
   }
   return TRUE;
}
