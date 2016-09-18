//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerLogicH
#define compilerLogicH

#include "elena.h"

namespace _ELENA_
{

class CompilerLogic : public _CompilerLogic
{
public:
   virtual bool isCompatible(ref_t targetRef, ref_t sourceRef);
};

} // _ELENA_

#endif // compilerLogicH