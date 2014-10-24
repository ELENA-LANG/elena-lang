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

// --- RTHelper ---

class RTHelper
{
public:
   static void readCallStack(StreamReader& stack, size_t startPosition, size_t currentAddress, StreamWriter& output);
};

// --- RTManager ---

class RTManager
{
public:
   RTManager()
   {
   }

   virtual ~RTManager()
   {
   }
};

} // _ELENA_

#endif // rtmanH
