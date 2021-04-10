//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include "elenart.h"
#include "elenartmachine.h"
#include "linux32/elfhelper.h"
#include <unistd.h>

#define ROOT_PATH          "/usr/lib/elena"
#define CONFIG_PATH        "/etc/elena/elenart.config"
#define IMAGE_BASE         0x08048000

using namespace _ELENA_;

static ELENARTMachine* _Instance = NULL;
static void* _SystemEnv = NULL;
static void* _Args = NULL;

void getSelfPath(Path& rootPath)
{
   char buff[PATH_MAX];
   size_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
   if (len != -1) {
      buff[len] = 0;
      rootPath.copy(buff);
   }
   /* handle error condition */
}

//inline void printInfo(int n)
//{
//   printf("%x\n", n);
//   fflush(stdout);
//}

void init()
{
   // get EXE path
   Path execPath;
   getSelfPath(execPath);

   _Instance = new ELENARTMachine(ROOT_PATH, execPath.c_str());

   void* messageSection = nullptr;
   void* mattributeSection = nullptr;
   ELENARTMachine::ImageSection section;
   section.init((void*)IMAGE_BASE, 0x1000000);

   size_t ptr = 0;
   MemoryReader reader(&section);

   ELFHelper::seekRODataSegment(reader, ptr);

   mattributeSection = (void*)ptr;

   reader.seek(ptr - IMAGE_BASE);
   size_t maSectionSize =  align(reader.getDWord(), 4);

   if(reader.seek(ptr - IMAGE_BASE + maSectionSize + 4)) {
      messageSection = (void*)(IMAGE_BASE + reader.Position());

      //printInfo((int)mattributeSection);
      //printInfo((int)messageSection);
      //printInfo((int)maSectionSize);

      _Instance->init(messageSection, mattributeSection, CONFIG_PATH);
   }
   //else printInfo(0);
}

// == Linux specific routines ==

int l_core_getargc()
{
   int* ptr = (int*)_Args;

   return *ptr;
}

int l_core_getarg(int index, char* buffer, int length)
{
   if (index <= 0)
      return 0;

   const char** args = (const char**)_Args;

   for (int i = 0; i < length; i++) {
      char tmp = args[index][i];

      buffer[i] = tmp;

      if (!tmp) {
         return i;
      }
   }

   return length;
}

// == ELENA run-time routines ==

/// Is used to initialize command argument list reference
void PrepareEM(void* args)
{
   _Args = args;
}

void InitializeSTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint, ProgramHeader* header)
{
   header->root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct((pos_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   if (_Instance == nullptr)
      init();

   // start the system
   _Instance->startSTA(header, (SystemEnv*)systemEnv, entryPoint);

   ::exit(0);
}

void InitializeMTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint, ProgramHeader* header)
{
//   header->root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;
//
//   // initialize the critical exception handler
//   __routineProvider.InitCriticalStruct(&header->root_critical_struct, (pos_t)criticalHandler);
//
//   // initialize system env variable
//   _SystemEnv = systemEnv;
//
//   if (_Instance == nullptr)
//      init();
//
//   // start the system
//   _Instance->startMTA(header, (SystemEnv*)systemEnv, entryPoint);
}

int StartThread(void* systemEnv, void* exceptionHandler, void* entryPoint, int index)
{
//   ProgramHeader header;
//   // initialize the exception handler
////   asm(
////      "mov header.root_exception_struct.core_catch_frame, ebp\n\t"
////      "mov header.root_exception_struct.core_catch_level, esp"
////   );
//
//   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;
//
//   _Instance->startThread(&header, (SystemEnv*)systemEnv, entryPoint, index);

   return 0;
}

void OpenFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::OpenFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

void CloseFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::CloseFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

void* GCCollect(void* roots, size_t size)
{
   return nullptr;
}

void Exit(int exitCode)
{
   _Instance->Exit(exitCode);
}

void StopThread(int exitCode)
{
//   _Instance->ExitThread((SystemEnv*)_SystemEnv, exitCode);
}

int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, void* buffer, size_t maxLength)
{
   return ((ELENARTMachine*)instance)->readCallStack(framePosition, currentAddress, startLevel, (lvaddr_t*)buffer, maxLength);
}

int LoadAddressInfo(size_t retPoint, char* lineInfo, int length)
{
   return _Instance->loadAddressInfo(retPoint, lineInfo, length);
}

int LoadClassName(void* object, char* buffer, int length)
{
   return _Instance->loadClassName((size_t)object, buffer, length);
}

void* EvaluateTape(void* tape)
{
   // !! terminator code
   return NULL;
}

void* InterpretTape(void* tape)
{
   // !! terminator code
   return NULL;
}

void* GetVMLastError(void* retVal)
{
   return NULL;
}

int LoadSubjectName(void* subject, char* lineInfo, int length)
{
   return _Instance->loadSubjectName((size_t)subject, lineInfo, length);
}

int LoadSubject(void* subjectName)
{
   return _Instance->loadSubject((const char*)subjectName);
}

int LoadMessageName(void* message, char* lineInfo, int length)
{
   return _Instance->loadMessageName((ref_t)message, lineInfo, length);
}

int LoadMessage(void* messageName)
{
   return _Instance->loadMessage((const char*)messageName);
}

void* LoadClassByString(void* systemEnv, void* referenceName)
{
   return (void*)_Instance->loadMetaAttribute((const char*)referenceName, caSerializable);
}

int LoadExtensionDispatcher(const char* moduleList, void* message, void* output)
{
   return _Instance->loadExtensionDispatcher(moduleList, (ref_t)message, output);
}

void* LoadClassByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length)
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

void* LoadSymbolByString(void* systemEnv, void* referenceName)
{
   return (void*)_Instance->loadMetaAttribute((const char*)referenceName, caSymbolSerializable);
}

void* LoadSymbolByString2(void* systemEnv, void* ns, void* referenceName)
{
   ReferenceNs str((const char*)ns, (const char*)referenceName);

   return LoadSymbolByString(systemEnv, (void*)str.c_str());
}

void* LoadSymbolByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length)
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
