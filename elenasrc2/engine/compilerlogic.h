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
   struct OperatorInfo
   {
      int         operatorId;

      ref_t       loperand;
      ref_t       roperand;
      LexicalType operationType;
      ref_t       result;

      OperatorInfo()
      {
         operatorId = 0;
         loperand = roperand = result = 0;
         operationType = lxNone;
      }
      OperatorInfo(int operatorId, ref_t loperand, ref_t roperand, LexicalType type, ref_t result)
      {
         this->operatorId = operatorId;
         this->loperand = loperand;
         this->roperand = roperand;
         this->operationType = type;
         this->result = result;
      }
   };

   typedef List<OperatorInfo> OperatorList;

   int checkMethod(ClassInfo& info, ref_t message, ref_t& outputType);
   int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, bool& found, ref_t& outputType);

   OperatorList operators;

public:
   virtual void defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference);
   virtual size_t defineStructSize(_CompilerScope& scope, ref_t reference);
   virtual size_t defineStructSize(ClassInfo& info);

   virtual int resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t message, bool& classFound, ref_t& outputType);
   virtual int resolveOperationType(int operatorId, ref_t loperand, ref_t roperand, ref_t& result);
   virtual bool resolveBranchOperation(int operatorId, ref_t& reference);

   virtual bool isCompatible(ref_t targetRef, ref_t sourceRef);
   virtual bool isPrimitiveRef(ref_t reference)
   {
      return (int)reference < 0;
   }
   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef);
   virtual bool isVariable(ClassInfo& info);
   virtual bool isEmbeddable(ClassInfo& info);

   virtual void injectVirtualMethods(SNode node, _CompilerScope& scope, _Compiler& compiler);

   virtual void tweakInlineClassFlags(ref_t classRef, ClassInfo& info);

   CompilerLogic();
};

} // _ELENA_

#endif // compilerLogicH