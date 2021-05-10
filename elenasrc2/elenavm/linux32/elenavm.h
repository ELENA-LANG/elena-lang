//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elenamachine.h"

#ifndef ELENART_H_INCLUDED
#define ELENART_H_INCLUDED

#if _LINUX
 #define DLL_PUBLIC __attribute__ ((visibility ("default")))
 #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
 #define DLL_PUBLIC
 #define DLL_LOCAL
#endif

extern "C"
{
   DLL_PUBLIC void PrepareEM(void* args);

   DLL_PUBLIC void InitializeVMSTA(void* sehTable, void* systemEnv, void* exceptionHandler, void* criticalHandler, void* vmTape,
                                   _ELENA_::ProgramHeader* header);

   //DLL_PUBLIC void InitializeMTA(void* systemEnv, void* exceptionHandler, void* criticalHandler, void* entryPoint, _ELENA_::ProgramHeader* header);

   //DLL_PUBLIC int StartThread(void* systemEnv, void* exceptionHandler, void* entryPoint, int index);

   DLL_PUBLIC void OpenFrame(void* systemEnv, void* frameHeader);

   DLL_PUBLIC void CloseFrame(void* systemEnv, void* frameHeader);

   DLL_PUBLIC void Exit(int exitCode);

   DLL_PUBLIC void StopThread(int exitCode);

   DLL_PUBLIC void* GCCollect(void* roots, size_t size);

   DLL_PUBLIC int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   DLL_PUBLIC int LoadAddressInfo(void* retPoint, char* buffer, size_t maxLength);

   DLL_PUBLIC int LoadClassName(void* object, char* buffer, int length);

   DLL_PUBLIC void* EvaluateTape(void* tape);

   DLL_PUBLIC void* InterpretTape(void* tape);

   DLL_PUBLIC const char* GetVMLastError();

   DLL_PUBLIC int LoadSubjectName(void* subject, char* lineInfo, int length);

   DLL_PUBLIC void* LoadSubject(void* subjectName);

   DLL_PUBLIC int LoadMessageName(void* message, char* lineInfo, int length);

   DLL_PUBLIC void* LoadMessage(void* messageName);

   DLL_PUBLIC void* LoadClassByString(void* systemEnv, void* referenceName);

   DLL_PUBLIC void* LoadClassByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length);

   DLL_PUBLIC void* LoadSymbolByString(void* systemEnv, void* referenceName);

   DLL_PUBLIC void* LoadSymbolByString2(void* systemEnv, void* ns, void* referenceName);

   DLL_PUBLIC void* LoadSymbolByBuffer(void* systemEnv, void* referenceName, size_t index, size_t length);

   DLL_PUBLIC int LoadExtensionDispatcher(const char* moduleList, void* message, void* output);

   DLL_PUBLIC void* GCCollect(void* roots, size_t size);

   DLL_PUBLIC void* GCCollectPerm(size_t size);

   // == Linux specific routines ==
   DLL_PUBLIC int l_core_getargc();

   DLL_PUBLIC int l_core_getarg(int index, char* buffer, int length);
}

#endif // ELENART_H_INCLUDED
