//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Engine
//             Linux Shared Library Declaration
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELT_ELENASM_H
#define ELT_ELENASM_H

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
   DLL_PUBLIC void* InterpretFileSMLA(const char* pathStr, int encoding, bool autoDetect);
   DLL_PUBLIC void* InterpretScriptSMLA(const char* script);
   DLL_PUBLIC size_t GetStatusSMLA(char* buffer, size_t maxLength);
   DLL_PUBLIC void ReleaseSMLA(void* tape);
}

#endif // ELENART_H_INCLUDED
