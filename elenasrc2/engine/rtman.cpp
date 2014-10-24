//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the base class implementing ELENA RTManager.
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------
#include "rtman.h"

using namespace _ELENA_;

// --- RTHelper ---

void RTHelper :: readCallStack(StreamReader& stack, size_t startPosition, size_t currentAddress, StreamWriter& output)
{
   size_t position = startPosition;
   size_t ret = 0;

   output.writeDWord(currentAddress);

   do {
      stack.seek(position);
      position = stack.getDWord();
      ret = stack.getDWord();
      if (position == 0 && ret != 0) {
         position = ret;
      }
      else if (ret != 0) {
         output.writeDWord(ret);
      }
   } while (position != 0);
}