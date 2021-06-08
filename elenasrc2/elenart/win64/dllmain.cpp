#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include <windows.h>
#include "pehelper.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

#define CONFIG_PATH "elenart64.cfg"

using namespace _ELENA_;

static ELENARTMachine* _Instance = nullptr;
static void*           _SystemEnv = nullptr;

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

//EXTERN_DLL_EXPORT void InitializeMTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint)
//{
//   ProgramHeader header;
//   // initialize the exception handler
//   __asm {
//      mov header.root_exception_struct.core_catch_frame, ebp
//      mov header.root_exception_struct.core_catch_level, esp
//   }
//   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;
//
//   // initialize the critical exception handler
//   __routineProvider.InitCriticalStruct(&header.root_critical_struct, (pos_t)criticalHandler);
//
//   // initialize system env variable
//   _SystemEnv = systemEnv;
//
//   // start the system
//   _Instance->startMTA(&header, (SystemEnv*)systemEnv, entryPoint);
//}
//
//EXTERN_DLL_EXPORT int StartThread(void* systemEnv, void* exceptionHandler, void* entryPoint, int index)
//{
//   ProgramHeader header;
//   // initialize the exception handler
//   __asm {
//      mov header.root_exception_struct.core_catch_frame, ebp
//      mov header.root_exception_struct.core_catch_level, esp
//   }
//   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;
//
//   _Instance->startThread(&header, (SystemEnv*)systemEnv, entryPoint, index);
//
//   return 0;
//}
//
//EXTERN_DLL_EXPORT void OpenFrame(void* systemEnv, void* frameHeader)
//{
//   SystemRoutineProvider::OpenFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
//}
//
//EXTERN_DLL_EXPORT void CloseFrame(void* systemEnv, void* frameHeader)
//{
//   SystemRoutineProvider::CloseFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
//}

EXTERN_DLL_EXPORT void Exit(int exitCode)
{
   _Instance->Exit(exitCode);
}

//EXTERN_DLL_EXPORT void StopThread(int exitCode)
//{
//   _Instance->ExitThread((SystemEnv*)_SystemEnv, exitCode);
//}
//
//EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
//{
//   return ((ELENARTMachine*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
//}

EXTERN_DLL_EXPORT int LoadAddressInfo(uintptr_t retPoint, char* lineInfo, int length)
{
   return _Instance->loadAddressInfo(*(long long*)retPoint, lineInfo, length);
}

EXTERN_DLL_EXPORT int LoadClassName(void* object, char* buffer, int length)
{
   return _Instance->loadClassName((size_t)object, buffer, length);
}

//EXTERN_DLL_EXPORT void* EvaluateTape(void* tape)
//{
//   // !! terminator code
//   return NULL;
//}
//
//EXTERN_DLL_EXPORT void* InterpretTape(void* tape)
//{
//   // !! terminator code
//   return NULL;
//}

EXTERN_DLL_EXPORT void* GetVMLastError(void* retVal)
{
   return nullptr;
}

EXTERN_DLL_EXPORT int LoadSubjectName(void* subject, char* lineInfo, int length)
{
   return _Instance->loadSubjectName((size_t)subject, lineInfo, length);
}

EXTERN_DLL_EXPORT ref_t LoadSubject(void* subjectName)
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

EXTERN_DLL_EXPORT void* GCCollect(void* roots, size_t size)
{
   return SystemRoutineProvider::GCRoutine(((SystemEnv*)_SystemEnv)->Table, (GCRoot*)roots, size);
}

EXTERN_DLL_EXPORT void* GCCollectPerm(size_t size)
{
   return SystemRoutineProvider::GCRoutinePerm(((SystemEnv*)_SystemEnv)->Table, size, ((SystemEnv*)_SystemEnv)->GCPERMSize);
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
