//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENART_H_INCLUDED
#define ELENART_H_INCLUDED

#include "elenamachine.h"

#if __GNUG__
 #define DLL_PUBLIC __attribute__ ((visibility ("default")))
 #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
 #define DLL_PUBLIC
 #define DLL_LOCAL
#endif

extern "C"
{
   DLL_PUBLIC void InitializeSTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC void* CollectGCLA(void* roots, size_t size);
   DLL_PUBLIC void InitializeSTLA(elena_lang::SystemEnv* env, elena_lang::SymbolList* entryList, void* criricalHandler);
   DLL_PUBLIC size_t LoadMessageNameLA(size_t message, char* buffer, size_t length);
   DLL_PUBLIC void ExitLA(int retVal);
}

#endif // ELENART_H_INCLUDED
