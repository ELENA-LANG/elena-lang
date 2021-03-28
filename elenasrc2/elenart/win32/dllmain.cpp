#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include <windows.h>
#include "pehelper.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#define CONFIG_PATH "elenart.cfg"

using namespace _ELENA_;

static ELENARTMachine* _Instance = NULL;
static void*           _SystemEnv = NULL;

EXTERN_DLL_EXPORT void PrepareEM(void* args)
{

}

EXTERN_DLL_EXPORT void InitializeSTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint,
   ProgramHeader* header)
{
   header->root_exception_struct.core_catch_addr = (uintptr_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct((uintptr_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   // start the system
   _Instance->startSTA(header, (SystemEnv*)systemEnv, entryPoint);
}

EXTERN_DLL_EXPORT void InitializeMTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint,
   ProgramHeader* header)
{
   header->root_exception_struct.core_catch_addr = (uintptr_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct((uintptr_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   // start the system
   _Instance->startMTA(header, (SystemEnv*)systemEnv, entryPoint);
}

EXTERN_DLL_EXPORT int StartThread(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint,
   int index, ProgramHeader* header)
{
   header->root_exception_struct.core_catch_addr = (uintptr_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct((uintptr_t)criticalHandler);

   _Instance->startThread(header, (SystemEnv*)systemEnv, entryPoint, index);

   return 0;
}

EXTERN_DLL_EXPORT void OpenFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::OpenFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

EXTERN_DLL_EXPORT void CloseFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::CloseFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

EXTERN_DLL_EXPORT void Exit(int exitCode)
{
   _Instance->Exit(exitCode);
}

EXTERN_DLL_EXPORT void StopThread(int exitCode)
{
   _Instance->ExitThread((SystemEnv*)_SystemEnv, exitCode);
}

EXTERN_DLL_EXPORT void* GCCollect(void* roots, size_t size)
{
   return SystemRoutineProvider::GCRoutine(((SystemEnv*)_SystemEnv)->Table, (GCRoot*)roots, size);
}

EXTERN_DLL_EXPORT void* GCCollectPerm(size_t size)
{
   return SystemRoutineProvider::GCRoutinePerm(((SystemEnv*)_SystemEnv)->Table, size, ((SystemEnv*)_SystemEnv)->GCPERMSize);
}

EXTERN_DLL_EXPORT void GCSignalStop(int handle)
{
   SystemRoutineProvider::GCSignalStop(handle);
}

EXTERN_DLL_EXPORT void GCSignalClear(int handle)
{
   SystemRoutineProvider::GCSignalClear(handle);
}

EXTERN_DLL_EXPORT void GCWaitForSignal(int handle)
{
   SystemRoutineProvider::GCWaitForSignal(handle);
}

EXTERN_DLL_EXPORT void GCWaitForSignals(int count, int* handles)
{
   SystemRoutineProvider::GCWaitForSignals(count, handles);
}

void loadModulePath(HMODULE hModule, Path& rootPath, bool includeName)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   if (includeName) {
      rootPath.copy(path);
   }
   else rootPath.copySubPath(path);
   rootPath.lower();
}

// ==== DLL entries ====

void init(HMODULE hModule)
{
   // get DLL path
   Path rootPath;
   loadModulePath(hModule, rootPath, false);

   // get EXE path
   Path execPath;
   loadModulePath(0, execPath, true);

   _Instance = new ELENARTMachine(rootPath.c_str(), execPath.c_str());

   void* messageSection = nullptr;
   void* mattributeSection = nullptr;
   ELENARTMachine::ImageSection section;
   section.init((void*)0x400000, 0x1000);

   size_t ptr = 0;
   PEHelper::seekSection(MemoryReader(&section), ".mdata", ptr);
   messageSection = (void*)ptr;

   PEHelper::seekSection(MemoryReader(&section), ".adata", ptr);
   mattributeSection = (void*)ptr;

   Path configPath(CONFIG_PATH);
   _Instance->init(messageSection, mattributeSection, configPath.c_str());
}

EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, lvaddr_t* buffer, size_t maxLength)
{
   return ((ELENARTMachine*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
}

EXTERN_DLL_EXPORT int LoadAddressInfo(size_t retPoint, char* lineInfo, int length)
{
   return _Instance->loadAddressInfo(retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadClassName(void* object, char* buffer, int length)
{
   return _Instance->loadClassName((size_t)object, buffer, length);
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
   return _Instance->loadSubjectName((size_t)subject, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadSubject(void* subjectName)
{
   return _Instance->loadSubject((const char*)subjectName);
}

EXTERN_DLL_EXPORT int LoadMessageName(void* message, char* lineInfo, int length)
{
   return _Instance->loadMessageName((ref_t)message, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadMessage(void* messageName)
{
   return _Instance->loadMessage((const char*)messageName);
}

EXTERN_DLL_EXPORT void* LoadClassByString(void* systemEnv, void* referenceName)
{
   return (void*)_Instance->loadMetaAttribute((const char*)referenceName, caSerializable);
}

EXTERN_DLL_EXPORT void* LoadClassByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length)
{
   if (length < 0x100) {
      IdentifierString str((const char*)referenceName, index, length);

      return LoadClassByString(systemEnv, (void*)str.c_str());
   }
   else {
      DynamicString<char> str((const char*)referenceName, index, length);

      return LoadClassByString(systemEnv, (void*)str.str());
   }
}

/// <summary>
/// Fills the passed dispatch list with references to extension message overload list
/// </summary>
/// <param name="moduleList">List of imported modules separated by semicolon</param>
/// <param name="message">Extension message</param>
/// <param name="output">Dispatch list</param>
/// <returns></returns>
EXTERN_DLL_EXPORT int LoadExtensionDispatcher(const char* moduleList, void* message, void* output)
{
   return _Instance->loadExtensionDispatcher(moduleList, (ref_t)message, output);
}

EXTERN_DLL_EXPORT void* LoadSymbolByString(void* systemEnv, void* referenceName)
{
   return (void*)_Instance->loadMetaAttribute((const char*)referenceName, caSymbolSerializable);
}

EXTERN_DLL_EXPORT void* LoadSymbolByString2(void* systemEnv, void* ns, void* referenceName)
{
   ReferenceNs str((const char*)ns, (const char*)referenceName);

   return LoadSymbolByString(systemEnv, (void*)str.c_str());
}

EXTERN_DLL_EXPORT void* LoadSymbolByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length)
{
   if (length < 0x100) {
      IdentifierString str((const char*)referenceName, index, length);

      return LoadSymbolByString(systemEnv, (void*)str.c_str());
   }
   else {
      DynamicString<char> str((const char*)referenceName, index, length);

      return LoadSymbolByString(systemEnv, (void*)str.str());
   }
}

/// <summary>
/// Creates a dynamic class inheriting the given VMT
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
EXTERN_DLL_EXPORT void* Inherit(const char* name, void* classPtr, void* handler)
{
   void* basePtr = (void*)SystemRoutineProvider::GetParent(classPtr);
   size_t classLen = SystemRoutineProvider::GetLength(classPtr);
   size_t baseLen = SystemRoutineProvider::GetLength(basePtr);
   int flags = SystemRoutineProvider::GetFlags(classPtr);

   return (void*)_Instance->inherit((SystemEnv*)_SystemEnv, name, (VMTEntry*)classPtr, (VMTEntry*)basePtr,
      classLen, baseLen, (pos_t*)&handler, 1, flags);
}

/// <summary>
/// Returns the signature member type at the specified position
/// </summary>
/// <param name="message">A strong-typed message</param>
/// <param name="index">A signature member index</param>
/// <param name="output">Signature tyoe</param>
/// <returns></returns>
EXTERN_DLL_EXPORT void* LoadSignatureMember(void* message, int index)
{
   return (void*)_Instance->loadSignatureMember((mssg_t)message, index);
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
         freeobj(_Instance);
         break;
   }
   return TRUE;
}
