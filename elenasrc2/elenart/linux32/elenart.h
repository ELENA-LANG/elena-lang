//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ELENART_H_INCLUDED
#define ELENART_H_INCLUDED

#if _LINUX32
 #define DLL_PUBLIC __attribute__ ((visibility ("default")))
 #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
 #define DLL_PUBLIC
 #define DLL_LOCAL
#endif

extern "C"
{
   DLL_PUBLIC void* Init(void* debugSection, const char* package);

   DLL_PUBLIC int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   DLL_PUBLIC int LoadAddressInfo(void* instance, int retPoint, char* lineInfo, int length);

   DLL_PUBLIC int LoadClassName(void* instance, void* object, char* lineInfo, int length);

   DLL_PUBLIC void* GetSymbolRef(void* instance, void* referenceName);

   DLL_PUBLIC void* Interpreter(void* instance, void* tape);

   DLL_PUBLIC void* GetRTLastError(void* instance, void* retVal);

   DLL_PUBLIC int LoadSubjectName(void* instance, void* subject, char* lineInfo, int length);

   DLL_PUBLIC void* LoadSubject(void* instance, void* subjectName);
}

#endif // ELENART_H_INCLUDED
