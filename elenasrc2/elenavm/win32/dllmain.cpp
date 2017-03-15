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
   Instance* instance = machine ? machine->getInstance() : NULL;

   return instance;
}

// ==== DLL entries ====

EXTERN_DLL_EXPORT int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   return 0; // !! temporally
}

EXTERN_DLL_EXPORT int LoadAddressInfo(void* retPoint, char* buffer, size_t maxLength)
{
   Instance* instance = getCurrentInstance();
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
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
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
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      size_t verb_id, subj_id;
      int param_count;
      decodeMessage((size_t)subjectRef, subj_id, verb_id, param_count);

      ident_t subjectName = instance->getSubject((ref_t)subj_id);
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
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      ref_t subj_id = instance->getSubjectRef((const char*)subjectName);

      return (void*)(MESSAGE_MASK | encodeMessage(subj_id, 0, 0));
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      size_t verb_id, subj_id;
      int param_count;
      decodeMessage((size_t)message, subj_id, verb_id, param_count);

      ident_t verbName = instance->getVerb(verb_id);
      size_t used = getlength(verbName);
      Convertor::copy(buffer, verbName, used, used);

      if (subj_id > 0) {
         buffer[used++] = '&';

         ident_t subjectName = instance->getSubject((ref_t)subj_id);
         size_t length = getlength(subjectName) ;
         if (length > 0) {
            if (maxLength >= (int)(length + used)) {
               Convertor::copy(buffer + used, subjectName, length, length);

               used += length;
            }
            else buffer[used] = 0;
         }
      }

      if (param_count > 0) {
         buffer[used++] = '[';
         Convertor::intToStr(param_count, buffer + used, 10);
         used = getlength(buffer);
         buffer[used++] = ']';
      }
      buffer[used] = 0;

      return used;
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      return (void*)(MESSAGE_MASK | instance->getMessageRef((const char*)messageName));
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

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
   Instance* instance = getCurrentInstance();
   if (instance == NULL)
      return 0;

   try {
      return instance->getSymbolRef((const char*)referenceName, false);
   }
   catch (JITUnresolvedException& e)
   {
      instance->setStatus("Cannot load ", e.reference);

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
   catch (EAbortException&)
   {
      return 0;
   }
}

EXTERN_DLL_EXPORT int EvaluateTape(void* tape)
{
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
   catch (EAbortException&)
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

EXTERN_DLL_EXPORT const char* GetVMLastError()
{
   Instance* instance = getCurrentInstance();

   return  instance ? instance->getStatus() : "Not initialized";
}

// --- initmachine ---

void initMachine(path_t rootPath)
{
   machine = new x86ELENAMachine(rootPath);
}

// --- createInstace ---

void createInstance()
{
   machine->newInstance(new x86Instance(machine));
}

// --- freeinstace ---

void freeInstance()
{
   machine->deleteInstance();

   freeobj(machine);
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
         initMachine(rootPath.c_str());
      }
      createInstance();
      return TRUE;
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      return TRUE;
   case DLL_PROCESS_DETACH:
      freeInstance();
      break;
   }
   return TRUE;
}
