//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                              (C)2009-2016, by Alexei Rakov
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
   DLL_PUBLIC int ReadCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   DLL_PUBLIC int LoadAddressInfo(void* retPoint, char* lineInfo, size_t length);

   DLL_PUBLIC int LoadClassName(void* object, char* lineInfo, size_t length);

   DLL_PUBLIC void* LoadSymbol(void* referenceName);

   DLL_PUBLIC void* Interpreter(void* tape);

   DLL_PUBLIC void* GetVMLastError(void* retVal);

   DLL_PUBLIC int LoadSubjectName(void* subject, char* lineInfo, size_t length);

   DLL_PUBLIC int LoadMessageName(void* subject, char* lineInfo, size_t length);

   DLL_PUBLIC void* LoadSubject(void* subjectName);
}

#endif // ELENART_H_INCLUDED
