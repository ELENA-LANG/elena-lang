//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the common Debugger interfaces
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef debuggingH
#define debuggingH

#include "elena.h"

namespace _ELENA_
{

// --- Controller ---

class _Controller
{
public:
   virtual void debugThread() = 0;

   virtual ~_Controller() {}   
};

// --- Breakpoint ---

struct Breakpoint
{
   size_t           row;
   IdentifierString source;
   IdentifierString module;
   void*            param;

   Breakpoint()
   {
      param = NULL;
   }
   Breakpoint(const wchar_t* module, const wchar_t* source, size_t row, void* param)
   {
      this->module.copy(module);
      this->source.copy(source);
      this->row = row;
      this->param = param;
   }
};

}
#endif // debuggingH
