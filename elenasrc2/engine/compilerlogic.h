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
   int checkMethod(ClassInfo& info, ref_t message)
   {
      ref_t dummy;
      return checkMethod(info, message, dummy);
   }
   int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, bool& found, ref_t& outputType);

   OperatorList operators;

public:
   virtual void defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference);
   virtual size_t defineStructSize(_CompilerScope& scope, ref_t reference);
   virtual size_t defineStructSize(ClassInfo& info);

   virtual int resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t message, bool& classFound, ref_t& outputType);
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result);
   virtual bool resolveBranchOperation(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t& reference);

   virtual bool isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef);
   virtual bool isPrimitiveRef(ref_t reference)
   {
      return (int)reference < 0;
   }
   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef);
   virtual bool isVariable(ClassInfo& info);
   virtual bool isEmbeddable(ClassInfo& info);
   virtual bool isRole(ClassInfo& info);

   virtual void injectVirtualCode(SNode node, _CompilerScope& scope, ClassInfo& info, _Compiler& compiler);

   virtual void tweakClassFlags(ref_t classRef, ClassInfo& info);

   virtual bool validateClassAttribute(int& attrValue);
   virtual bool validateMethodAttribute(int& attrValue);
   virtual bool validateFieldAttribute(int& attrValue);
   virtual bool validateLocalAttribute(int& attrValue);

   virtual bool isDefaultConstructorEnabled(ClassInfo& info)
   {
      return (info.header.flags & elDebugMask) != elEnumList;
   }

   CompilerLogic();
};

} // _ELENA_

#endif // compilerLogicH