//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//      This header contains the declaration of the base class implementing
//      ELENA RT manager.
//                                              (C)2005-2014, by Alexei Rakov
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

   size_t readCallStack(StreamReader& stack, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   bool readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, 
      const wchar16_t* &symbol, const wchar16_t* &method, const wchar16_t* &path, int& row);

   bool readAddressInfo(StreamReader& debug, size_t retAddress, _LibraryManager* manager, wchar16_t* buffer, size_t& length);
};


} // _ELENA_

#endif // rtmanH
