//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Engine
//             Linux Shared Library Declaration
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELT_ELENAVM_H
#define ELT_ELENAVM_H

#include "common.h"
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
   DLL_PUBLIC int InitializeVMSTLA(elena_lang::SystemEnv* env, void* tape, const char* criricalHandlerReference);
   DLL_PUBLIC int EvaluateVMLA(void* tape);

}

#endif // ELENART_H_INCLUDED
