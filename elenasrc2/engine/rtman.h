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
   static void readCallStack(StreamReader& stack, lvaddr_t framePosition, lvaddr_t currentAddress, StreamWriter& output);

   bool readAddressInfo(StreamReader& debug, uintptr_t retAddress, _LibraryManager* manager,
      ident_t &symbol, ident_t &method, ident_t &path, int& row);

   pos_t readCallStack(StreamReader& stack, lvaddr_t framePosition, lvaddr_t currentAddress, lvaddr_t startLevel, lvaddr_t* buffer, pos_t maxLength);

   lvaddr_t readAddressInfo(StreamReader& debug, uintptr_t retAddress, _LibraryManager* manager, char* buffer, size_t maxLength);

   lvaddr_t readClassName(StreamReader& debug, lvaddr_t vmtAddress, char* buffer, size_t maxLength);
   //size_t readSubjectName(StreamReader& debug, size_t subjectRef, char* buffer, size_t maxLength);

   lvaddr_t loadMetaAttribute(StreamReader& section, ident_t name, int category, size_t len);
   pos_t loadSubject(StreamReader& section, ident_t name);

   RTManager()
   {
   }
};

} // _ELENA_

#endif // rtmanH
