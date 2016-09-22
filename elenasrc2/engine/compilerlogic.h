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
#include "compilercommon.h"

namespace _ELENA_
{

class CompilerLogic : public _CompilerLogic
{
   int checkMethod(ClassInfo& info, ref_t message, ref_t& outputType);
   int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, bool& found, ref_t& outputType);

public:
   virtual void defineClassInfo(ClassInfo& info, ref_t reference); 
   virtual size_t defineStructSize(ref_t reference);

   virtual int resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t message, bool& classFound, ref_t& outputType);

   virtual bool isCompatible(ref_t targetRef, ref_t sourceRef);

   virtual void injectVirtualMethods(SNode node, _CompilerScope& scope, _Compiler& compiler);
};

} // _ELENA_

#endif // compilerLogicH