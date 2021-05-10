#include "elena.h"
// --------------------------------------------------------------------------
#include "elenavm.h"
#include "x86elenavmachine.h"

using namespace _ELENA_;

static x86ELENAVMMachine* _Machine = nullptr;
//static Path rootPath;
static void* _SystemEnv = nullptr;
static void* _Args = nullptr;

// --- initmachine ---

void initMachine(path_t rootPath)
{
   _Machine = new x86ELENAVMMachine(rootPath);

 /*  if (::IsDebuggerPresent()) {
      _Machine->getInstance()->setDebugMode();
   }*/
}

//// --- freeMachine ---
//
//void freeMachine()
//{
//   freeobj(_Machine);
//   _Machine = nullptr;
//}

//// --- getAppPath ---
//
//void loadDLLPath(HMODULE hModule)
//{
//   TCHAR path[MAX_PATH + 1];
//
//   ::GetModuleFileName(hModule, path, MAX_PATH);
//
//   rootPath.copySubPath(path);
//   rootPath.lower();
//}

// ==== DLL entries ====

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

void InitializeVMSTA(void* sehTable, void* systemEnv, void* exceptionHandler, void* criticalHandler, void* vmTape,
   ProgramHeader* header)
{
   header->root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   // initialize the critical exception handler
   if (criticalHandler != nullptr)
      __routineProvider.InitCriticalStruct((pos_t)criticalHandler);

   // initialize system env variable
   _SystemEnv = systemEnv;

   if (_Machine == nullptr)
      initMachine("/usr/lib/elena");

   // start the system
   _Machine->startSTA(header, (SystemEnv*)systemEnv, sehTable, vmTape);
}

void OpenFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::OpenFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

void CloseFrame(void* systemEnv, void* frameHeader)
{
   SystemRoutineProvider::CloseFrame((SystemEnv*)systemEnv, (FrameHeader*)frameHeader);
}

void Exit(int exitCode)
{
   _Machine->Exit(exitCode);
}

// !! temporally

//EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
//{
//   return 0; // !! temporally
//}

int LoadAddressInfo(void* retPoint, char* buffer, size_t maxLength)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      if (instance->loadAddressInfo(retPoint, buffer, maxLength)) {
         return maxLength;
      }
      else return 0;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

int LoadClassName(void* vmtAddress, char* buffer, int maxLength)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      size_t length = maxLength;

      int packagePtr = *(int*)((int)vmtAddress - 24);
      int namePtr = *(int*)((int)vmtAddress - 20);

      char* name = (char*)namePtr;
      char* ns = ((char**)packagePtr)[0];

      size_t ns_len = length;
      if (!ident_t(ns).copyTo(buffer, ns_len))
         return 0;

      maxLength -= ns_len;
      if (!ident_t(name).copyTo(buffer + ns_len, length))
         return 0;

      return length + ns_len;


      //ident_t className = instance->getClassName(vmtAddress);
      //size_t length = getlength(className);
      //if (length > 0) {
      //   if (maxLength >= (int)length) {
      //      Convertor::copy(buffer, className, length, length);
      //   }
      //   else buffer[0] = 0;
      //}

      //return length;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

int LoadSubjectName(void* subjectRef, char* buffer, int maxLength)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      size_t subj_id = (size_t)subjectRef;

      ident_t subjectName = instance->getSubject(subj_id);
      size_t length = getlength(subjectName);
      if (length > 0) {
         if (maxLength >= (int)length) {
            Convertor::copy(buffer, subjectName, length, length);
         }
         else buffer[0] = 0;
      }

      return length;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

void* LoadSubject(void* subjectName)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      ref_t subj_id = instance->getSubjectRef((SystemEnv*)_SystemEnv, (const char*)subjectName);

      return (void*)subj_id;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

int LoadMessageName(void* message, char* buffer, int maxLength)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      return instance->loadMessageName((mssg_t)message, buffer, maxLength);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

void* LoadMessage(void* messageName)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      return (void*)(instance->getMessageRef((SystemEnv*)_SystemEnv, (const char*)messageName));
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

void* LoadSymbolByString(void* systemEnv, void* referenceName)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      return (void*)instance->getSymbolRef((SystemEnv*)systemEnv, (const char*)referenceName, false);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
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

void* LoadClassByString(void* systemEnv, void* referenceName)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      return (void*)instance->getClassVMTRef((SystemEnv*)systemEnv, (const char*)referenceName, false);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch (InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
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

int LoadExtensionDispatcher(const char* moduleList, void* message, void* output)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   return instance->loadExtensionDispatcher((SystemEnv*)_SystemEnv, moduleList, (mssg_t)message, output);
}

int EvaluateTape(void* systemEnv, void* sehTable, void* tape)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      int retVal = instance->interprete((SystemEnv*)systemEnv, sehTable, tape, false);

      return retVal;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.referenceInfo);

      return 0;
   }
   catch(InternalError& e)
   {
      instance->setStatus(e.message);

      return 0;
   }
   catch (EAbortException&)
   {
      return 0;
   }
}

//EXTERN_DLL_EXPORT size_t SetDebugMode()
//{
//   Instance* instance = getCurrentInstance();
//   if (instance == NULL)
//      return 0;
//
//   instance->setDebugMode();
//
//   return (size_t)instance->loadDebugSection();
//}

const char* GetVMLastError()
{
   Instance* instance = _Machine->getInstance();

   return  instance ? instance->getStatus() : "Not initialized";
}

void* GCCollect(void* roots, size_t size)
{
   return SystemRoutineProvider::GCRoutine(((SystemEnv*)_SystemEnv)->Table, (GCRoot*)roots, size);
}

void* GCCollectPerm(size_t size)
{
   return SystemRoutineProvider::GCRoutinePerm(((SystemEnv*)_SystemEnv)->Table, size, ((SystemEnv*)_SystemEnv)->GCPERMSize);
}
