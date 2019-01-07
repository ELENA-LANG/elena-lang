//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                              (C)2005-2019, by Alexei Rakov
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

//   int checkMethod(ClassInfo& info, ref_t message)
//   {
//      ChechMethodInfo dummy;
//      return checkMethod(info, message, dummy);
//   }   

   OperatorList operators;

   bool isSignatureCompatible(_ModuleScope& scope, ref_t targetMessage, ref_t sourceMessage);
   bool isSignatureCompatible(_ModuleScope& scope, ref_t targetSignature, ref_t* sourceSignatures);
   bool isSignatureCompatible(_ModuleScope& scope, _Module* targetModule, ref_t targetSignature, ref_t* sourceSignatures);

   void setSignatureStacksafe(_ModuleScope& scope, ref_t targetSignature, int& stackSafeAttr);
   void setSignatureStacksafe(_ModuleScope& scope, _Module* targetModule, ref_t targetSignature, int& stackSafeAttr);

//   bool loadBranchingInfo(_CompilerScope& scope, ref_t reference);
   bool injectImplicitConstructor(SyntaxWriter& writer, _ModuleScope& scope, _Compiler& compiler, ClassInfo& info, ref_t targetRef/*, ref_t elementRef*/, ref_t* signatures, int paramCount);

   ref_t resolveImplicitConstructor(_ModuleScope& scope, ref_t targetRef, ref_t* signatures, int paramCount, int& stackSafeAttr);
   ref_t resolveImplicitConstructor(_ModuleScope& scope, ClassInfo& info, ref_t* signatures, int signatureLen, int& stackSafeAttr);

   ref_t getClassClassRef(_ModuleScope& scope, ref_t reference);

   bool isBoolean(_ModuleScope& scope, ref_t reference);

public:
   virtual int checkMethod(_ModuleScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result);
   virtual int checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result);

   virtual bool defineClassInfo(_ModuleScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false);

   virtual int defineStructSize(_ModuleScope& scope, ref_t reference, ref_t elementRef)
   {
      bool dummy = false;
      return defineStructSizeVariable(scope, reference, elementRef, dummy);
   }
   virtual int defineStructSizeVariable(_ModuleScope& scope, ref_t reference, ref_t elementRef, bool& variable);
   virtual int defineStructSize(ClassInfo& info, bool& variable);

   virtual ref_t retrievePrimitiveReference(_ModuleScope& scope, ClassInfo& info);

   virtual int resolveCallType(_ModuleScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result);
   virtual int resolveOperationType(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result);
   virtual int resolveOperationType(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result);
   virtual int resolveNewOperationType(_ModuleScope& scope, ref_t loperand, ref_t roperand);
   virtual bool resolveBranchOperation(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t& reference);
   virtual ref_t definePrimitiveArray(_ModuleScope& scope, ref_t elementRef);

   virtual bool isCompatible(_ModuleScope& scope, ref_t targetRef, ref_t sourceRef);
////   virtual bool isPrimitiveArray(ref_t reference);
//   virtual bool isPrimitiveRef(ref_t reference)
//   {
//      return (int)reference < 0;
//   }
   virtual ref_t resolvePrimitive(ClassInfo& info, ref_t& element);
   virtual bool isWrapper(ClassInfo& info)
   {
      return test(info.header.flags, elWrapper);
   }
   virtual bool isEmbeddableArray(ClassInfo& info);
   virtual bool isVariable(_ModuleScope& scope, ref_t targetRef);
   virtual bool isVariable(ClassInfo& info);
   virtual bool isEmbeddable(ClassInfo& info);
   virtual bool isEmbeddable(_ModuleScope& scope, ref_t reference)
   {
      ClassInfo info;
      if(!defineClassInfo(scope, info, reference, true))
         return false;

      return isEmbeddable(info);
   }
   virtual bool isStacksafeArg(ClassInfo& info);
   virtual bool isStacksafeArg(_ModuleScope& scope, ref_t reference)
   {
      ClassInfo info;
      if (!defineClassInfo(scope, info, reference, true))
         return false;

      return isStacksafeArg(info);
   }
   virtual bool isRole(ClassInfo& info);
   virtual bool isAbstract(ClassInfo& info);
   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message);
   virtual bool isMethodGeneric(ClassInfo& info, ref_t message);
   virtual bool isMethodAbstract(ClassInfo& info, ref_t message);
   virtual bool isMethodInternal(ClassInfo& info, ref_t message);
   virtual bool isMethodPrivate(ClassInfo& info, ref_t message);
   virtual bool isMultiMethod(ClassInfo& info, ref_t message);
   virtual bool isClosure(ClassInfo& info, ref_t message);
//   virtual bool isDispatcher(ClassInfo& info, ref_t message);
   virtual bool isReadonly(ClassInfo& info);
   virtual bool isReadonly(_ModuleScope& scope, ref_t reference)
   {
      ClassInfo info;
      defineClassInfo(scope, info, reference, true);

      return isReadonly(info);
   }
   virtual bool isWithEmbeddableDispatcher(_ModuleScope& scope, SNode node);

   virtual void injectVirtualCode(_ModuleScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler, bool closed);
//   virtual void injectVirtualFields(_CompilerScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler);
   virtual void injectVirtualMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, _Compiler& compiler, List<ref_t>& implicitMultimethods, LexicalType methodType);
   virtual void injectOperation(SyntaxWriter& writer, _ModuleScope& scope, int operatorId, int operation, ref_t& reference, ref_t elementRef);
   virtual bool injectImplicitConversion(SyntaxWriter& writer, _ModuleScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, 
      ref_t elementRef, ident_t ns);
   //virtual bool injectImplicitConstructor(SyntaxWriter& writer, _ModuleScope& scope, _Compiler& compiler, ref_t targetRef, ref_t signRef);
   virtual ref_t resolveImplicitConstructor(_ModuleScope& scope, ref_t targetRef, ref_t signRef, int paramCount, int& stackSafeAttr);

//   virtual bool injectDefaultCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t classClassRef);
//   virtual bool injectImplicitCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef);
   virtual void injectNewOperation(SyntaxWriter& writer, _ModuleScope& scope, int operation, ref_t targetRef, ref_t elementRef);
//////   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode);
   virtual void injectOverloadList(_ModuleScope& scope, ClassInfo& info, _Compiler& compiler, ref_t classRef);

   virtual void injectInterfaceDisaptch(_ModuleScope& scope, _Compiler& compiler, SNode node, ref_t parentRef);

   virtual void tweakClassFlags(_ModuleScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode);
   virtual void tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info);

   virtual bool validateClassAttribute(int& attrValue);
   virtual bool validateMethodAttribute(int& attrValue, bool& explicitMode, bool& templateMode);
   virtual bool validateImplicitMethodAttribute(int& attrValue);
   virtual bool validateFieldAttribute(int& attrValue, bool& isSealed, bool& isConstant, bool& isEmbeddable);
   virtual bool validateExpressionAttribute(int attrValue, ExpressionAttributes& attributes);
   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne);
//////   virtual bool validateWarningAttribute(int& attrValue);
   virtual bool validateMessage(_ModuleScope& scope, ref_t message, bool isClassClass);
   virtual bool validateArgumentAttribute(int attrValue, bool& byRefArg, bool& paramsArg, bool& templateArg);

////   virtual bool validateClassFlag(ClassInfo& info, int flag);
   virtual void validateClassDeclaration(_ModuleScope& scope, ClassInfo& info, bool& withAbstractMethods, bool& disptacherNotAllowed, bool& emptyStructure);

   virtual bool isDefaultConstructorEnabled(ClassInfo& info)
   {
      return !test(info.header.flags, elDynamicRole);
   }

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
   virtual void optimizeBranchingOp(_ModuleScope& scope, SNode node);

   virtual bool validateBoxing(_ModuleScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool unboxingExpected, bool dynamicRequired);

////   virtual void optimizeDuplicateBoxing(SNode node);

   virtual ref_t resolveMultimethod(_ModuleScope& scope, ref_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef, int& stackSafeAttr);
   virtual void verifyMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, List<ref_t>& implicitMultimethods);
   virtual ref_t resolveExtensionTemplate(ident_t pattern);

   CompilerLogic();
};

} // _ELENA_

#endif // compilerLogicH