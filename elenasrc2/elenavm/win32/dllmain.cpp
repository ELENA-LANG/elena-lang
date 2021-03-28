#include "elena.h"
// --------------------------------------------------------------------------
#include "x86elenavmachine.h"
//#include "evmscope.h"

#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

using namespace _ELENA_;

static x86ELENAVMMachine* _Machine = nullptr;
static Path rootPath;
static void* _SystemEnv = nullptr;

constexpr size_t cnNameOffset = sizeof(VMTHeader) + sizeof(uintptr_t);
constexpr size_t cnPackageOffset = cnNameOffset + sizeof(uintptr_t);

// --- getAppPath ---

void loadDLLPath(HMODULE hModule)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);
   rootPath.lower();
}

// ==== DLL entries ====

EXTERN_DLL_EXPORT void PrepareEM(void* args)
{

}

EXTERN_DLL_EXPORT void InitializeVMSTA(void* sehTable, void* systemEnv, void* exceptionHandler, void* criticalHandler, void* vmTape, 
   ProgramHeader* header)
{
   _SystemEnv = systemEnv;

   header->root_exception_struct.core_catch_addr = (uintptr_t)exceptionHandler;

   // initialize the critical exception handler
   if (criticalHandler != nullptr)
      __routineProvider.InitCriticalStruct((uintptr_t)criticalHandler);

   //// initialize system env variable
   //_SystemEnv = systemEnv;

   // start the system
   _Machine->startSTA(header, (SystemEnv*)systemEnv, sehTable, vmTape);
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
   _Machine->Exit(exitCode);
}

// !! temporally

//EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
//{
//   return 0; // !! temporally
//}

EXTERN_DLL_EXPORT int LoadAddressInfo(void* retPoint, char* buffer, size_t maxLength)
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

EXTERN_DLL_EXPORT int LoadClassName(void* vmtAddress, char* buffer, int maxLength)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      size_t length = maxLength;

      uintptr_t packagePtr = *(uintptr_t*)((lvaddr_t)vmtAddress - cnPackageOffset);
      uintptr_t namePtr = *(uintptr_t*)((lvaddr_t)vmtAddress - cnNameOffset);

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

EXTERN_DLL_EXPORT int LoadSubjectName(void* subjectRef, char* buffer, int maxLength)
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

EXTERN_DLL_EXPORT void* LoadSubject(void* subjectName)
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

EXTERN_DLL_EXPORT int LoadMessageName(void* message, char* buffer, int maxLength)
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

EXTERN_DLL_EXPORT void* LoadMessage(void* messageName)
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

EXTERN_DLL_EXPORT void* LoadSymbolByString(void* systemEnv, void* referenceName)
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

EXTERN_DLL_EXPORT void* LoadClassByString(void* systemEnv, void* referenceName)
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

EXTERN_DLL_EXPORT int EvaluateTape(void* systemEnv, void* sehTable, void* tape)
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

EXTERN_DLL_EXPORT const char* GetVMLastError()
{
   Instance* instance = _Machine->getInstance();

   return  instance ? instance->getStatus() : "Not initialized";
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
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   return instance->loadExtensionDispatcher((SystemEnv*)_SystemEnv, moduleList, (mssg_t)message, output);
}

EXTERN_DLL_EXPORT void* GCCollect(void* roots, size_t size)
{
   return SystemRoutineProvider::GCRoutine(((SystemEnv*)_SystemEnv)->Table, (GCRoot*)roots, size);
}

EXTERN_DLL_EXPORT void* GCCollectPerm(size_t size)
{
   return SystemRoutineProvider::GCRoutinePerm(((SystemEnv*)_SystemEnv)->Table, size, ((SystemEnv*)_SystemEnv)->GCPERMSize);
}

/// <summary>
/// Creates a dynamic class inheriting the given VMT
/// </summary>
/// <returns>a reference to dynamically created VMT</returns>
EXTERN_DLL_EXPORT void* Inherit(const char* name, void* classPtr, void* handler)
{
   return nullptr; // !! currently is not implemented

   //void* basePtr = (void*)SystemRoutineProvider::GetParent(classPtr);
   //size_t classLen = SystemRoutineProvider::GetLength(classPtr);
   //size_t baseLen = SystemRoutineProvider::GetLength(basePtr);
   //int flags = SystemRoutineProvider::GetFlags(classPtr);

   //return (void*)_Instance->inherit((SystemEnv*)_SystemEnv, name, (VMTEntry*)classPtr, (VMTEntry*)basePtr,
   //   classLen, baseLen, (pos_t*)&handler, 1, flags);
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
   return nullptr; // !! currently is not implemented

//   return (void*)_Instance->loadSignatureMember((mssg_t)message, index);
}

// --- initmachine ---

void initMachine(path_t rootPath)
{
   _Machine = new x86ELENAVMMachine(rootPath);

   if (::IsDebuggerPresent()) {
      _Machine->getInstance()->setDebugMode();
   }
}

// --- freeMachine ---

void freeMachine()
{
   freeobj(_Machine);
   _Machine = nullptr;
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
         loadDLLPath(hModule);
         initMachine(rootPath.c_str());
         return TRUE;
      }
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
         return TRUE;
      case DLL_PROCESS_DETACH:
         freeMachine();
         break;
   }
   return TRUE;
}
