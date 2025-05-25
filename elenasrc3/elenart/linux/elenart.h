//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                             (C)2022-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENART_H_INCLUDED
#define ELENART_H_INCLUDED

#include "elenamachine.h"
#include "core.h"

#if __GNUG__

 #define DLL_PUBLIC __attribute__ ((visibility ("default")))
 #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))

#else

 #define DLL_PUBLIC
 #define DLL_LOCAL

#endif

extern "C"
{
#ifdef __FreeBSD__
   // temporal solution to work on FreeBSD
   DLL_PUBLIC char** environ = nullptr;
   DLL_PUBLIC const char* __progname = "elena-prog";
#endif

   DLL_PUBLIC void InitializeSTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC void InitializeMTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC void* CollectGCLA(void* roots, size_t size);
   DLL_PUBLIC void* CollectPermGCLA(size_t size);
   DLL_PUBLIC size_t LoadMessageNameLA(size_t message, char* buffer, size_t length);
   DLL_PUBLIC size_t LoadCallStackLA(uintptr_t framePtr, uintptr_t* list, size_t length);
   DLL_PUBLIC size_t LoadAddressInfoLM(size_t retPoint, char* lineInfo, size_t length);
   DLL_PUBLIC elena_lang::addr_t LoadSymbolByStringLA(const char* symbolName);
   DLL_PUBLIC elena_lang::addr_t LoadClassByStringLA(const char* symbolName);
   DLL_PUBLIC elena_lang::addr_t LoadSymbolByString2LA(const char* ns, const char* symbolName);
   DLL_PUBLIC elena_lang::mssg_t LoadMessageLA(const char* messageName);
   DLL_PUBLIC void* InjectProxyTypeLA(void* target, void* type, int staticLength, int nameIndex);
   DLL_PUBLIC int LoadSignatureLA(elena_lang::mssg_t message, elena_lang::addr_t* output, int maximalLength);
   DLL_PUBLIC unsigned int GetRandomIntLA(elena_lang::SeedStruct& seed);
   DLL_PUBLIC void GetRandomSeedLA(elena_lang::SeedStruct& seed);
   DLL_PUBLIC void PrepareLA(uintptr_t arg);
   DLL_PUBLIC int GetArgCLA();
   DLL_PUBLIC int GetArgLA(int index, char* buffer, int length);
   DLL_PUBLIC void ExitLA(int retVal);
   DLL_PUBLIC void GetGCStatisticsLA(elena_lang::GCStatistics* statistics);
   DLL_PUBLIC void ResetGCStatisticsLA();
   DLL_PUBLIC int LoadExtensionDispatcherLA(const char* moduleList, elena_lang::mssg_t message, void* output);
   DLL_PUBLIC size_t LoadActionNameLA(size_t message, char* buffer, size_t length);
   DLL_PUBLIC elena_lang::addr_t LoadClassByBufferLA(void* referenceName, size_t index, size_t length);
   DLL_PUBLIC elena_lang::mssg_t LoadActionLA(const char* actionName);
   DLL_PUBLIC size_t LoadClassMessagesLA(void* classPtr, elena_lang::mssg_t* output, size_t skip, size_t maxLength);
   DLL_PUBLIC bool CheckClassMessageLA(void* classPtr, elena_lang::mssg_t message);
   DLL_PUBLIC void* CreateThreadLA(void* arg, void* threadProc, int stackSize, int flags);
   DLL_PUBLIC void InitThreadLA(elena_lang::SystemEnv* env, void* criricalHandler, int index);
   DLL_PUBLIC void StartThreadLA(elena_lang::SystemEnv* env, void* entryPoint, int index);
   DLL_PUBLIC void UninitThreadLA(elena_lang::SystemEnv* env, int index);
   DLL_PUBLIC void ExitThreadLA(int errCode);
   DLL_PUBLIC void SignalStopGCLA(void* handle);
   DLL_PUBLIC WaitForSignalGCLA(void* handle);
   DLL_PUBLIC SignalClearGCLA(void* handle);
   DLL_PUBLIC void WaitForSignalsGCLA(size_t count, void* handles);
}

#endif // ELENART_H_INCLUDED
