//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler common interfaces.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerCommonH
#define compilerCommonH

#include "elena.h"
#include "syntaxtree.h"

namespace _ELENA_
{

typedef Map<ref_t, ref_t> ClassMap;

// --- _CompileScope ---

struct _CompilerScope
{
   // list of typified classes which may need get&type message
   ClassMap typifiedClasses;
};

// --- _Compiler ---

class _Compiler
{
public:
   virtual void injectVirtualReturningMethod(SNode node, ident_t variable) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   // check if the classes is compatible
   virtual bool isCompatible(ref_t targetRef, ref_t sourceRef) = 0;

   // auto generate virtual methods
   virtual void injectVirtualMethods(SNode node, _CompilerScope& scope, _Compiler& compiler) = 0;
};
   
}  // _ELENA_

#endif // compilerCommonH