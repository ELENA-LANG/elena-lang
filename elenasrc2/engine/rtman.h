//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA RT manager.
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef rtmanH
#define rtmanH 1

namespace _ELENA_
{

// --- RTManager ---

class RTManager
{
public:
   static void readCallStack(StreamReader& stack, size_t framePosition, size_t currentAddress, StreamWriter& output);

   bool readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, 
      ident_t &symbol, ident_t &method, ident_t &path, int& row);

   size_t readCallStack(StreamReader& stack, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   size_t readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, char* buffer, size_t maxLength);

   size_t readClassName(StreamReader& debug, size_t vmtAddress, char* buffer, size_t maxLength);
   //size_t readSubjectName(StreamReader& debug, size_t subjectRef, char* buffer, size_t maxLength);

   void* loadMetaAttribute(StreamReader& section, ident_t name, int category, size_t len);
   void* loadSubject(StreamReader& section, ident_t name);

   RTManager()
   {
   }
};

} // _ELENA_

#endif // rtmanH
