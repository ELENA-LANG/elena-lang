//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA SM Engine
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
   DLL_PUBLIC void* InterpretScript(_ELENA_::ident_t script);

   DLL_PUBLIC void* InterpretScopeScript(int scope_id, _ELENA_::ident_t script);

   DLL_PUBLIC void* InterpretFile(const char* pathStr, int encoding, bool autoDetect);

   DLL_PUBLIC void* InterpretScopeFile(int scope_id, const char* pathStr, int encoding, bool autoDetect);

   DLL_PUBLIC void Release(void* tape);

   DLL_PUBLIC int GetStatus(char* buffer, int maxLength);

   DLL_PUBLIC int NewScope();
}

#endif // ELENART_H_INCLUDED
