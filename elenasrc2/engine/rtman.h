//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA RT manager.
//                                              (C)2005-2015, by Alexei Rakov
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

   size_t readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, ident_c* buffer, size_t maxLength);

   size_t readClassName(StreamReader& debug, size_t vmtAddress, ident_c* buffer, size_t maxLength);
   size_t readSubjectName(StreamReader& debug, size_t subjectRef, ident_c* buffer, size_t maxLength);

   void* loadSymbol(StreamReader& debug, ident_t name);
   void* loadSubject(StreamReader& debug, ident_t name);
};

} // _ELENA_

#endif // rtmanH
