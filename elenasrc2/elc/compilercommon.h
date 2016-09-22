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

// virtual objects
#define V_INT32 (size_t)-11

namespace _ELENA_
{

typedef Map<ref_t, ref_t> ClassMap;

enum MethodHint
{
   tpMask       = 0x0F,

   tpUnknown    = 0x00,
      tpSealed     = 0x01,
      tpClosed     = 0x02,
      tpNormal     = 0x03,
//      tpDispatcher = 0x04,
//      tpPrivate    = 0x05,
//      tpStackSafe  = 0x10,
//      tpEmbeddable = 0x20,
//      tpGeneric    = 0x40,
//      tpAction     = 0x80,
};


// --- _CompileScope ---

struct _CompilerScope
{
   // list of typified classes which may need get&type message
   ClassMap typifiedClasses;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
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
   // retrieve the class info / size
   virtual void defineClassInfo(ClassInfo& info, ref_t reference) = 0;
   virtual size_t defineStructSize(ref_t reference) = 0;

   // retrieve the call type
   virtual int resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t message, bool& classFound, ref_t& outputType) = 0;

   // check if the classes is compatible
   virtual bool isCompatible(ref_t targetRef, ref_t sourceRef) = 0;

   // auto generate virtual methods
   virtual void injectVirtualMethods(SNode node, _CompilerScope& scope, _Compiler& compiler) = 0;
};
   
}  // _ELENA_

#endif // compilerCommonH