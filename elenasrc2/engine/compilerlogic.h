//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerLogicH
#define compilerLogicH

#include "elena.h"
#include "compilercommon.h"

namespace _ELENA_
{

class CompilerLogic : public _CompilerLogic
{
//   struct OperatorInfo
//   {
//      int         operatorId;
//
//      ref_t       loperand;
//      ref_t       roperand;
//      ref_t       roperand2;
//      LexicalType operationType;
//      ref_t       result;
//
//      OperatorInfo()
//      {
//         operatorId = 0;
//         loperand = roperand = result = roperand2 = 0;
//         operationType = lxNone;
//      }
//      OperatorInfo(int operatorId, ref_t loperand, ref_t roperand, LexicalType type, ref_t result)
//      {
//         this->operatorId = operatorId;
//         this->loperand = loperand;
//         this->roperand = roperand;
//         this->operationType = type;
//         this->result = result;
//         this->roperand2 = 0;
//      }
//      OperatorInfo(int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, LexicalType type, ref_t result)
//      {
//         this->operatorId = operatorId;
//         this->loperand = loperand;
//         this->roperand = roperand;
//         this->roperand2 = roperand2;
//         this->operationType = type;
//         this->result = result;
//      }
//   };
//
//   typedef List<OperatorInfo> OperatorList;
//
//   int checkMethod(ClassInfo& info, ref_t message)
//   {
//      ChechMethodInfo dummy;
//      return checkMethod(info, message, dummy);
//   }   
//
//   OperatorList operators;
//
//   bool isSignatureCompatible(_CompilerScope& scope, ref_t targetAction, ref_t sourceAction);
//   bool loadBranchingInfo(_CompilerScope& scope, _Compiler& compiler, ref_t reference);
//   bool injectImplicitConstructor(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ClassInfo& info, ref_t targetRef, ref_t elementRef, ref_t actionRef, int paramCount);
//
//   bool isBoolean(_CompilerScope& scope, _Compiler& compiler, ref_t reference);

public:
   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result);
   virtual int checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result);

   virtual bool defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false);

//   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t elementRef)
//   {
//      bool dummy = false;
//      return defineStructSizeVariable(scope, reference, elementRef, dummy);
//   }
//   virtual int defineStructSizeVariable(_CompilerScope& scope, ref_t reference, ref_t elementRef, bool& variable);
//   virtual int defineStructSize(ClassInfo& info, bool& variableS);
//
//   virtual ref_t retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info);

   virtual int resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result);
//   virtual int resolveOperationType(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t roperand, ref_t& result);
//   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result);
//   virtual int resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result);
//   virtual bool resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference);
//   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef);
//   virtual ref_t resolvePrimitiveReference(_CompilerScope& scope, ref_t reference);
//
//   virtual bool isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef);
//////   virtual bool isPrimitiveArray(ref_t reference);
//   virtual bool isPrimitiveRef(ref_t reference)
//   {
//      return (int)reference < 0;
//   }
//   virtual bool isEmbeddableArray(ClassInfo& info);
//   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef);
//   virtual bool isVariable(ClassInfo& info);
//   virtual bool isEmbeddable(ClassInfo& info);
//   virtual bool isEmbeddable(_CompilerScope& scope, ref_t reference)
//   {
//      ClassInfo info;
//      defineClassInfo(scope, info, reference, true);
//
//      return isEmbeddable(info);
//   }
   virtual bool isRole(ClassInfo& info);
//   virtual bool isAbstract(ClassInfo& info);
//   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message);
//   virtual bool isMethodGeneric(ClassInfo& info, ref_t message);
//   virtual bool isMultiMethod(ClassInfo& info, ref_t message);
//   virtual bool isClosure(ClassInfo& info, ref_t message);
//   virtual bool isReadonly(ClassInfo& info);
//   virtual bool isReadonly(_CompilerScope& scope, ref_t reference)
//   {
//      ClassInfo info;
//      defineClassInfo(scope, info, reference, true);
//
//      return isReadonly(info);
//   }
//
//   virtual void injectVirtualCode(_CompilerScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler, bool closed);
//   virtual void injectVirtualMultimethods(_CompilerScope& scope, SNode node, ClassInfo& info, _Compiler& compiler, List<ref_t>& implicitMultimethods, LexicalType methodType);
//   virtual void injectOperation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference, ref_t elementRef);
//   virtual bool injectImplicitConversion(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t elementRef);
//   virtual bool injectImplicitConstructor(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t actionRef, int paramCount);
//   //
//   virtual bool injectImplicitCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef);
//   virtual void injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t targetRef, ref_t elementRef);
////   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode);
//   virtual void injectOverloadList(_CompilerScope& scope, ClassInfo& info, _Compiler& compiler, ref_t classRef);

   virtual void tweakClassFlags(_CompilerScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode);
//   virtual bool tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info);

   virtual bool validateClassAttribute(int& attrValue);
//   virtual bool validateMethodAttribute(int& attrValue);
//   virtual bool validateFieldAttribute(int& attrValue, bool& isSealed, bool& isConstant);
//   virtual bool validateLocalAttribute(int& attrValue);
//   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne);
//////   virtual bool validateWarningAttribute(int& attrValue);
//   virtual bool validateMessage(ref_t message, bool isClassClass);
////
////   virtual bool validateClassFlag(ClassInfo& info, int flag);
//
//   virtual bool isDefaultConstructorEnabled(ClassInfo& info)
//   {
//      return (info.header.flags & elDebugMask) != elEnumList;
//   }
//
//   bool recognizeEmbeddableOp(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t verb, ref_t& subject);
//   bool recognizeEmbeddableOp2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t verb, ref_t& subject);
//
//   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject);
//   virtual bool recognizeEmbeddableGetAt(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject);
//   virtual bool recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject);
//   virtual bool recognizeEmbeddableEval(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject);
//   virtual bool recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningRef, ref_t& subject);
//   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne);
//   virtual bool recognizeEmbeddableMessageCall(SNode node, ref_t& messageRef);
//
//   virtual bool optimizeEmbeddable(SNode node, _CompilerScope& scope);
//   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node);
//   virtual bool optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node);
//   virtual void optimizeBranchingOp(_CompilerScope& scope, SNode node);
//
//   virtual bool validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool unboxingExpected);
//
//////   virtual void optimizeDuplicateBoxing(SNode node);
//
//   virtual ref_t resolveMultimethod(_CompilerScope& scope, ref_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef);
//   virtual void verifyMultimethods(_CompilerScope& scope, SNode node, ClassInfo& info, List<ref_t>& implicitMultimethods);

   CompilerLogic();
};

} // _ELENA_

#endif // compilerLogicH