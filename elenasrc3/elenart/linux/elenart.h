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
   DLL_PUBLIC char** environ;

   DLL_PUBLIC void InitializeSTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC void InitializeMTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC void* CollectGCLA(void* roots, size_t size);
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
}

#endif // ELENART_H_INCLUDED
