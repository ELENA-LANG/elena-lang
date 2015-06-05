//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//             Linux Shared Library Declaration
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ELENART_H_INCLUDED
#define ELENART_H_INCLUDED

extern "C"
{
   void* Init(void* debugSection, const char* package);

   int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   int LoadAddressInfo(void* instance, int retPoint, char* lineInfo, int length);

   int LoadClassName(void* instance, void* object, char* lineInfo, int length);

   void* GetSymbolRef(void* instance, void* referenceName);

   void* Interpreter(void* instance, void* tape);

   void* GetRTLastError(void* instance, void* retVal);

   int LoadSubjectName(void* instance, void* subject, char* lineInfo, int length);

   void* LoadSubject(void* instance, void* subjectName);
}

#endif // ELENART_H_INCLUDED
