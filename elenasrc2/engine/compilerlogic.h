//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                              (C)2005-2017, by Alexei Rakov
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
      ref_t       roperand2;
      LexicalType operationType;
      ref_t       result;

      OperatorInfo()
      {
         operatorId = 0;
         loperand = roperand = result = roperand2 = 0;
         operationType = lxNone;
      }
      OperatorInfo(int operatorId, ref_t loperand, ref_t roperand, LexicalType type, ref_t result)
      {
         this->operatorId = operatorId;
         this->loperand = loperand;
         this->roperand = roperand;
         this->operationType = type;
         this->result = result;
         this->roperand2 = 0;
      }
      OperatorInfo(int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, LexicalType type, ref_t result)
      {
         this->operatorId = operatorId;
         this->loperand = loperand;
         this->roperand = roperand;
         this->roperand2 = roperand2;
         this->operationType = type;
         this->result = result;
      }
   };

   typedef List<OperatorInfo> OperatorList;

   int checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result);
   int checkMethod(ClassInfo& info, ref_t message)
   {
      ChechMethodInfo dummy;
      return checkMethod(info, message, dummy);
   }   

   OperatorList operators;

   bool loadBranchingInfo(_CompilerScope& scope, _Compiler& compiler, ref_t reference);

public:
   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result);

   virtual bool defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false);
   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t type = 0, bool embeddableOnly = false);
   virtual int defineStructSize(ClassInfo& info, bool embeddableOnly);

   virtual ref_t retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info);

   virtual int resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result);
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result);
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result);
   virtual int resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result);
   virtual bool resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference);
   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef);
   virtual ref_t resolvePrimitiveReference(_CompilerScope& scope, ref_t reference);

   virtual bool isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef);
   virtual bool isCompatibleWithType(_CompilerScope& scope, ref_t targetRef, ref_t type);
//   virtual bool isPrimitiveArray(ref_t reference);
   virtual bool isPrimitiveRef(ref_t reference)
   {
      return (int)reference < 0;
   }
   virtual bool isEmbeddableArray(ClassInfo& info);
   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef);
   virtual bool isVariable(ClassInfo& info);
   virtual bool isEmbeddable(ClassInfo& info);
   virtual bool isEmbeddable(_CompilerScope& scope, ref_t reference)
   {
      ClassInfo info;
      defineClassInfo(scope, info, reference, true);

      return isEmbeddable(info);
   }
   virtual bool isRole(ClassInfo& info);
   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message);
   virtual bool isMethodGeneric(ClassInfo& info, ref_t message);
   virtual bool isReadonly(ClassInfo& info);
   virtual bool isReadonly(_CompilerScope& scope, ref_t reference)
   {
      ClassInfo info;
      defineClassInfo(scope, info, reference, true);

      return isReadonly(info);
   }

   virtual void injectVirtualCode(_CompilerScope& scope, ref_t classRef, ClassInfo& info, _Compiler& compiler);
   virtual void injectOperation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference, ref_t type);
   virtual bool injectImplicitConversion(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t sourceType);
   virtual void injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t elementType, ref_t targetRef);
//   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode);

   virtual void tweakClassFlags(_CompilerScope& scope, ref_t classRef, ClassInfo& info, bool classClassMode);
   virtual bool tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info);

   virtual bool validateClassAttribute(int& attrValue);
   virtual bool validateMethodAttribute(int& attrValue);
   virtual bool validateFieldAttribute(int& attrValue);
   virtual bool validateLocalAttribute(int& attrValue);
   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne);
   virtual bool validateDeclarationAttribute(int attrValue, DeclarationAttr& declType);
//   virtual bool validateWarningAttribute(int& attrValue);
   virtual bool validateMessage(ref_t message, bool isClassClass);
//
//   virtual bool validateClassFlag(ClassInfo& info, int flag);

   virtual bool isDefaultConstructorEnabled(ClassInfo& info)
   {
      return (info.header.flags & elDebugMask) != elEnumList;
   }

   bool recognizeEmbeddableOp(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t verb, ref_t& subject);
   bool recognizeEmbeddableOp2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t verb, ref_t& subject);

   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject);
   virtual bool recognizeEmbeddableGetAt(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject);
   virtual bool recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject);
   virtual bool recognizeEmbeddableEval(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject);
   virtual bool recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject);
   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne);

   virtual bool optimizeEmbeddable(SNode node, _CompilerScope& scope);
   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node);
   virtual bool optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node);

   virtual bool validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool assingingMode);

////   virtual void optimizeDuplicateBoxing(SNode node);

   virtual ref_t defineOperatorMessage(_CompilerScope& scope, ref_t operatorId, int paramCount, ref_t loperand, ref_t roperand, ref_t roperand2);

//   virtual bool recognizeNestedScope(SNode& node);
//   virtual bool recognizeScope(SNode& node);
//   virtual bool recognizeNewLocal(SNode& node);
//   virtual bool recognizeNewField(SNode& node);

   CompilerLogic();
};

} // _ELENA_

#endif // compilerLogicH