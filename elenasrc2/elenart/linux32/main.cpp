//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include "elenart.h"
#include "instance.h"
#include "linux32/elfhelper.h"

#define ROOT_PATH          "/usr/lib/elena"
#define CONFIG_PATH        "/etc/elena/elenart.config"
#define IMAGE_BASE         0x08048000

using namespace _ELENA_;

static ELENARTMachine* _Instance = NULL;
static void* _SystemEnv = NULL;

EXTERN_DLL_EXPORT void InitializeSTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint)
{
   ProgramHeader header;
   // initialize the exception handler
   __asm {
      mov header.root_exception_struct.core_catch_frame, ebp
      mov header.root_exception_struct.core_catch_level, esp
   }
   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct(&header.root_critical_struct, (pos_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   // start the system
   _Instance->startSTA(&header, (SystemEnv*)systemEnv, entryPoint);
}

EXTERN_DLL_EXPORT void InitializeMTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint)
{
   ProgramHeader header;
   // initialize the exception handler
   __asm {
      mov header.root_exception_struct.core_catch_frame, ebp
      mov header.root_exception_struct.core_catch_level, esp
   }
   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct(&header.root_critical_struct, (pos_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   // start the system
   _Instance->startMTA(&header, (SystemEnv*)systemEnv, entryPoint);
}

EXTERN_DLL_EXPORT int StartThread(void* systemEnv, void* exceptionHandler, void* entryPoint, int index)
{
   ProgramHeader header;
   // initialize the exception handler
   __asm {
      mov header.root_exception_struct.core_catch_frame, ebp
      mov header.root_exception_struct.core_catch_level, esp
   }
   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   _Instance->startThread(&header, (SystemEnv*)systemEnv, entryPoint, index);

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
   //// get DLL path
   //Path rootPath;
   //loadModulePath(hModule, rootPath, false);

   //// get EXE path
   //Path execPath;
   //loadModulePath(0, execPath, true);

   _Instance = new ELENARTMachine(rootPath.c_str(), execPath.c_str());

   void* messageSection = nullptr;
   void* mattributeSection = nullptr;
   ELENARTMachine::ImageSection section;
   section.init((void*)IMAGE_BASE, 0x1000);

   PEHelper::seekSection(MemoryReader(&section), ".mdata", ptr);
   messageSection = (void*)ptr;

   PEHelper::seekSection(MemoryReader(&section), ".adata", ptr);
   mattributeSection = (void*)ptr;

   _Instance->init(messageSection, mattributeSection, CONFIG_PATH);
}

EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
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

EXTERN_DLL_EXPORT void* LoadSubject(void* subjectName)
{
   return _Instance->loadSubject((const char*)subjectName);
}

EXTERN_DLL_EXPORT int LoadMessageName(void* message, char* lineInfo, int length)
{
   return _Instance->loadMessageName((ref_t)message, lineInfo, length);
}

EXTERN_DLL_EXPORT void* LoadMessage(void* messageName)
{
   return _Instance->loadMessage((const char*)messageName);
}

EXTERN_DLL_EXPORT void* LoadClassByString(void* systemEnv, void* referenceName)
{
   throw InternalError("Not yet implemented"); // !! temporal 

   //return _Instance->loadMetaAttribute((const char*)referenceName, caSerializable);
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
   throw InternalError("Not yet implemented"); // !! temporal 

   //return _Instance->loadMetaAttribute((const char*)referenceName, caSymbolSerializable);
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
