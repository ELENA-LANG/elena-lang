#include "elena.h"
// --------------------------------------------------------------------------
#include "x86elenavmachine.h"
//#include "evmscope.h"

#include <windows.h>

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

using namespace _ELENA_;

static x86ELENAVMMachine* _Machine = NULL;
static Path rootPath;

// --- getAppPath ---

void loadDLLPath(HMODULE hModule)
{
   TCHAR path[MAX_PATH + 1];

   ::GetModuleFileName(hModule, path, MAX_PATH);

   rootPath.copySubPath(path);
   rootPath.lower();
}

// ==== DLL entries ====

EXTERN_DLL_EXPORT void InitializeVMSTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* vmTape)
{
   FrameHeader header;
   // initialize the exception handler
   __asm {
      mov header.root_exception_struct.core_catch_frame, ebp
      mov header.root_exception_struct.core_catch_level, esp
   }
   header.root_exception_struct.core_catch_addr = (pos_t)exceptionHandler;

   // initialize the critical exception handler
   __routineProvider.InitCriticalStruct(&header.root_critical_struct, (pos_t)criticalHandler);

   //// initialize system env variable
   //_SystemEnv = systemEnv;

   // start the system
   _Machine->startSTA(&header, (SystemEnv*)systemEnv, vmTape);
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
      ident_t className = instance->getClassName(vmtAddress);
      size_t length = getlength(className);
      if (length > 0) {
         if (maxLength >= (int)length) {
            Convertor::copy(buffer, className, length, length);
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
      ref_t subj_id = instance->getSubjectRef((const char*)subjectName);

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
      ref_t action;
      int count;
      decodeMessage((ref_t)message, action, count);

      size_t used = 0;
      //if (test((ref_t)message, encodeAction(SIGNATURE_FLAG))) {
      //   ImageSection messageSection;
      //   messageSection.init(_messageSection, 0x10000); // !! dummy size

      //   ref_t verb = messageSection[action];
      //   used += manager.readSubjectName(reader, verb, buffer + used, length - used);
      //}
      //else {
         ident_t subjectName = instance->getSubject(action);
         size_t length = getlength(subjectName);
         if (length > 0) {
            if (maxLength >= (int)(length + used)) {
               Convertor::copy(buffer + used, subjectName, length, length);

               used += length;
            }
            else buffer[used] = 0;
         }
      //}

      if (count > 0) {
         size_t dummy = 10;
         String<char, 10>temp;
         temp.appendInt(count);

         buffer[used++] = '[';
         Convertor::copy(buffer + used, temp, getlength(temp), dummy);
         used += dummy;
         buffer[used++] = ']';
      }
      buffer[used] = 0;

      return used;
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
      return (void*)(instance->getMessageRef((const char*)messageName));
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

EXTERN_DLL_EXPORT void* LoadSymbol(void* referenceName)
{
   Instance* instance = _Machine->getInstance();
   if (instance == NULL)
      return 0;

   try {
      return instance->getSymbolRef((const char*)referenceName, false);
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

EXTERN_DLL_EXPORT int InterpretTape(void* tape)
{
   // !! temporal

//   Instance* instance = getCurrentInstance();
//   if (instance == NULL)
      return 0;
//
//   try {
//      return instance->interprete(tape, VM_INTERPRET_EXT);
//   }
//   catch (JITUnresolvedException& e)
//   {
//      instance->setStatus("Cannot load ", e.referenceInfo);
//
//      return 0;
//   }
//   catch(InternalError& e)
//   {
//      instance->setStatus(e.message);
//
//      return 0;
//   }
//   catch (EAbortException&)
//   {
//      return 0;
//   }
}

//EXTERN_DLL_EXPORT int EvaluateTape(void* tape)
//{
//   Instance* instance = getCurrentInstance();
//   if (instance == NULL)
//      return 0;
//
//   try {
//      _Memory* m = instance->getTargetSection(mskStatRef);
//      int bef = (*m)[8];
//
//      int r = instance->interprete(tape, VM_INTERPRET);
//
//      m = instance->getTargetSection(mskStatRef);
//      int aft = (*m)[8];
//
//      return r;
//   }
//   catch (JITUnresolvedException& e)
//   {
//      instance->setStatus("Cannot load ", e.referenceInfo);
//
//      return 0;
//   }
//   catch(InternalError& e)
//   {
//      instance->setStatus(e.message);
//
//      return 0;
//   }
//   catch (EAbortException&)
//   {
//      return 0;
//   }
//}
//
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
//   Instance* instance = getCurrentInstance();
//
   return  /*instance ? instance->getStatus() : */"Not initialized";
}

// --- initmachine ---

void initMachine(path_t rootPath)
{
   _Machine = new x86ELENAVMMachine(rootPath);
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
