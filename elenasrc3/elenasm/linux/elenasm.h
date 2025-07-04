//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA SM Engine
//             Linux Shared Library Declaration
//                                             (C)2022-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENASM_H_INCLUDED
#define ELENASM_H_INCLUDED

#if __GNUG__

#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))

#else

#define DLL_PUBLIC
#define DLL_LOCAL

#endif

extern "C"
{
   DLL_PUBLIC int NewScopeSMLA();

   DLL_PUBLIC void* InterpretFileSMLA(const char* pathStr, int encoding, bool autoDetect);

   DLL_PUBLIC void* InterpretScopeFileSMLA(int scope_id, const char* pathStr, int encoding, bool autoDetect);
   
   DLL_PUBLIC void* InterpretScopeScriptSMLA(int scope_id, const char* script);

   DLL_PUBLIC void* InterpretScriptSMLA(const char* script);

   DLL_PUBLIC int GetLengthSMLA(int id);

   DLL_PUBLIC void ReleaseSMLA(int id);

   DLL_PUBLIC size_t GetStatusSMLA(char* buffer, size_t maxLength);

   DLL_PUBLIC void ClearStackSMLA();
}

#endif // ELENASM_H_INCLUDED
