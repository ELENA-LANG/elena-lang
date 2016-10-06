//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class implementation.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compilerlogic.h"

using namespace _ELENA_;

typedef ClassInfo::Attribute Attribute;

inline bool isWrappable(int flags)
{
   return !test(flags, elWrapper) && test(flags, elSealed);
}

inline bool isPrimitiveArrayRef(ref_t classRef)
{
   switch (classRef)
   {
   case V_INT32ARRAY:
   case V_INT16ARRAY:
   case V_INT8ARRAY:
      return true;
   default:
      return false;
   }
}

inline ref_t definePrimitiveArrayItem(ref_t classRef)
{
   switch (classRef)
   {
      case V_INT32ARRAY:
      case V_INT16ARRAY:
      case V_INT8ARRAY:
         return V_INT32;
      default:
         return 0;
   }
}

// --- CompilerLogic ---

CompilerLogic :: CompilerLogic()
{
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(LESS_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));

   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT32ARRAY, V_INT32, V_INT32, lxIntArrOp, 0));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT16ARRAY, V_INT32, V_INT32, lxShortArrOp, 0));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT8ARRAY, V_INT32, V_INT32, lxByteArrOp, 0));
}

int CompilerLogic :: checkMethod(ClassInfo& info, ref_t message, ref_t& outputType)
{
   bool methodFound = info.methods.exist(message);

   if (methodFound) {
      int hint = info.methodHints.get(Attribute(message, maHint));
      outputType = info.methodHints.get(Attribute(message, maType));

      if ((hint & tpMask) == tpSealed) {
         return hint;
      }
      else if (test(info.header.flags, elSealed)) {
         return tpSealed | hint;
      }
      else if (test(info.header.flags, elClosed)) {
         return tpClosed | hint;
      }
      else return tpNormal | hint;
   }
   //HOTFIX : to recognize the sealed private method call
   //         hint search should be done even if the method is not declared
   else return info.methodHints.get(Attribute(message, maHint));
}

int CompilerLogic :: checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, bool& found, ref_t& outputType)
{
   ClassInfo info;
   found = scope.loadClassInfo(info, reference) != 0;

   if (found) {
      // only sealed / closed classes should be considered as found
      if (!test(info.header.flags, elClosed))
         found = false;

      return checkMethod(info, message, outputType);
   }
   else return tpUnknown;
}

int CompilerLogic :: resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t messageRef, bool& classFound, ref_t& outputType)
{
   if (classReference == 0)
      classReference = scope.superReference;

   int methodHint = classReference != 0 ? checkMethod(scope, classReference, messageRef, classFound, outputType) : 0;
   int callType = methodHint & tpMask;

   return callType;
}

int CompilerLogic :: resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result)
{
   if (loperand == 0 || roperand == 0)
      return 0;

   OperatorList::Iterator it = operators.start();
   while (!it.Eof()) {
      OperatorInfo info = *it;

      if (info.operatorId == operatorId && isCompatible(scope, info.loperand, loperand) && isCompatible(scope, info.roperand, roperand)) {
         result = info.result;

         return info.operationType;
      }
      it++;
   }

   return 0;
}

int CompilerLogic :: resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result)
{
   if (loperand == 0 || roperand == 0 || roperand2 == 0)
      return 0;

   OperatorList::Iterator it = operators.start();
   while (!it.Eof()) {
      OperatorInfo info = *it;

      if (info.operatorId == operatorId && isCompatible(scope, info.loperand, loperand) && isCompatible(scope, info.roperand, roperand)
         && isCompatible(scope, info.roperand2, roperand2)) 
      {
         result = info.result;

         return info.operationType;
      }
      it++;
   }

   return 0;
}

bool CompilerLogic :: resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference)
{
   if (!loperand)
      return false;

   if (loperand != scope.branchingInfo.reference) {
      ClassInfo info;
      scope.loadClassInfo(info, loperand, true);

      if ((info.header.flags & elDebugMask) == elEnumList) {
         ref_t listRef = loperand;
         _Module* extModule = scope.loadReferenceModule(listRef);
         _Memory* listSection = extModule ? extModule->mapSection(listRef | mskConstArray, true) : NULL;         
         if (listSection) {
            MemoryReader reader(listSection);
            
            ref_t trueRef = 0, falseRef = 0;
            while (!reader.Eof()) {
               ref_t memberRef = compiler.readEnumListMember(scope, extModule, reader);

               ClassInfo memberInfo;
               scope.loadClassInfo(memberInfo, memberRef);
               int attribute = checkMethod(memberInfo, encodeMessage(0, IF_MESSAGE_ID, 1));
               if (attribute == (tpIfBranch | tpSealed)) {
                  trueRef = memberRef;
               }
               else if (attribute == (tpIfNotBranch | tpSealed)) {
                  falseRef = memberRef;
               }
            }

            if (trueRef && falseRef) {
               scope.branchingInfo.reference = loperand;
               scope.branchingInfo.trueRef = trueRef;
               scope.branchingInfo.falseRef = falseRef;
            }
         }         
      }
   }

   if (loperand == scope.branchingInfo.reference) {
      reference = operatorId == IF_MESSAGE_ID ? scope.branchingInfo.trueRef : scope.branchingInfo.falseRef;

      return true;
   }
   else return false;
}

inline bool isPrimitiveCompatible(ref_t targetRef, ref_t sourceRef)
{
   switch (sourceRef)
   {
      case V_PTR32:
         return targetRef == V_INT32;
      default:
         return false;
   }
}

inline bool isPrimitiveArrayCompatible(ref_t targetRef, ref_t sourceRef)
{
   switch (sourceRef)
   {
   case V_PTR32:
      return targetRef == V_INT32;
   default:
      return false;
   }
}

bool CompilerLogic :: isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef)
{
   if (!targetRef)
      return true;

   if (isPrimitiveCompatible(targetRef, sourceRef))
      return true;

   while (sourceRef != 0) {
      if (targetRef != sourceRef) {
         ClassInfo info;
         defineClassInfo(scope, info, sourceRef);

         // if it is a structure wrapper
         if (isPrimitiveRef(targetRef) && test(info.header.flags, elStructureWrapper)) {
            ClassInfo::FieldInfo inner = info.fieldTypes.get(0);
            if (isCompatible(scope, targetRef, inner.value1))
               return true;
         }

         sourceRef = info.header.parentRef;
      }
      else return true;
   }

   return false;
}

bool CompilerLogic :: isEmbeddableArray(ClassInfo& info)
{
   return test(info.header.flags, elDynamicRole | elEmbeddable | elStructureRole);
}

bool CompilerLogic :: isVariable(_CompilerScope& scope, ref_t classReference)
{
   ClassInfo info;
   defineClassInfo(scope, info, classReference);

   return isVariable(info);
}

bool CompilerLogic :: isVariable(ClassInfo& info)
{
   return test(info.header.flags, elWrapper) && !test(info.header.flags, elReadOnlyRole);
}

bool CompilerLogic :: isEmbeddable(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole | elEmbeddable) && !test(info.header.flags, elDynamicRole);
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
}

bool CompilerLogic :: isMethodStacksafe(ClassInfo& info, ref_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpStackSafe);
}

bool CompilerLogic :: isMethodGeneric(ClassInfo& info, ref_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpGeneric);
}

void CompilerLogic :: injectVirtualCode(SNode node, _CompilerScope& scope, ClassInfo& info, _Compiler& compiler)
{
   SNode templateNode = node.appendNode(lxTemplate);

   // auto generate get&type message if required
   ClassMap::Iterator c_it = scope.typifiedClasses.getIt(node.argument);
   while (!c_it.Eof()) {
      if (c_it.key() == node.argument) {
         SNode methodNode = templateNode.appendNode(lxClassMethod, encodeMessage(*c_it, GET_MESSAGE_ID, 0));

         compiler.injectVirtualReturningMethod(methodNode, THIS_VAR);
      }
      c_it++;
   }

   // generate enumeration list
   if ((info.header.flags & elDebugMask) == elEnumList && test(info.header.flags, elNestedClass)) {
      compiler.generateEnumListMember(scope, info.header.parentRef, node.argument);
   }
}

void CompilerLogic :: injectOperation(SNode node, _CompilerScope& scope, _Compiler& compiler, int operator_id, int operationType, ref_t& reference)
{
   SNode operationNode = node.injectNode((LexicalType)operationType, operator_id);

   if (reference == V_FLAG) {      
      if (!scope.branchingInfo.reference) {
         // HOTFIX : resolve boolean symbols
         ref_t dummy;
         resolveBranchOperation(scope, compiler, IF_MESSAGE_ID, scope.boolReference, dummy);
      }

      reference = scope.branchingInfo.reference;
      operationNode.appendNode(lxIfValue, scope.branchingInfo.trueRef);
      operationNode.appendNode(lxElseValue, scope.branchingInfo.falseRef);
   }
}

bool CompilerLogic :: injectImplicitConversion(SNode node, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t sourceType)
{
   ClassInfo info;
   defineClassInfo(scope, info, targetRef);   

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper)) {
      ClassInfo::FieldInfo inner = info.fieldTypes.get(0);

      bool compatible = false;
      if (test(info.header.flags, elStructureWrapper) && isPrimitiveRef(sourceRef)) {
         compatible = isCompatible(scope, sourceRef, inner.value1);
      }
      else compatible = isCompatible(scope, inner.value1, sourceRef);

      if (compatible) {
         compiler.injectBoxing(scope, node, 
            test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, 
            test(info.header.flags, elStructureRole) ? info.size : 0, targetRef);

         return true;
      }
   }

   // HOT FIX : trying to typecast primitive structure array
   if (isPrimitiveArrayRef(sourceRef) && test(info.header.flags, elStructureRole | elDynamicRole)) {
      ClassInfo sourceInfo;
      defineClassInfo(scope, sourceInfo, sourceRef, true);

      if (sourceInfo.size == info.size && isCompatible(scope, definePrimitiveArrayItem(sourceRef), info.fieldTypes.get(-1).value1)) {
         compiler.injectBoxing(scope, node,
            test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, info.size, targetRef);

         return true;
      }
   }

   // check if there are implicit constructors
   if (test(info.header.flags, elSealed) && sourceType != 0) {
      int implicitMessage = encodeMessage(sourceType, PRIVATE_MESSAGE_ID, 1);
      if (info.methods.exist(implicitMessage)) {
         if (test(info.header.flags, elStructureRole)) {
            compiler.injectConverting(node, lxDirectCalling, implicitMessage, lxCreatingStruct, info.size, targetRef);
         }
         else compiler.injectConverting(node, lxDirectCalling, implicitMessage, lxCreatingClass, info.fields.Count(), targetRef);

         return true;
      }
   }

   return false;
}

void CompilerLogic :: defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly)
{
//      if (isTemplateRef(classRef)) {
//         variable.kind = okTemplateLocal;
//      }
   switch (reference)
   {
      case V_INT32:
         info.header.parentRef = 0;
         info.header.flags = elDebugDWORD | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_PTR32:
         info.header.parentRef = 0;
         info.header.flags = elDebugPTR | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_SIGNATURE:
      case V_VERB:
         info.header.parentRef = 0;
         info.header.flags = elDebugSubject | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_MESSAGE:
         info.header.parentRef = 0;
         info.header.flags = elDebugMessage | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_INT32ARRAY:
         info.header.parentRef = 0;
         info.header.flags = elDebugIntegers | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -4;
         break;
      case V_INT16ARRAY:
         info.header.parentRef = 0;
         info.header.flags = elDebugShorts | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -2;
         break;
      case V_INT8ARRAY:
         info.header.parentRef = 0;
         info.header.flags = elDebugBytes | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -1;
         break;
      default:
         if (reference != 0) {
            scope.loadClassInfo(info, reference, headerOnly);
         }      
         break;
   }
//      else if (isPrimitiveRef(classRef)) {
//         else if (classRef == -2) {
//            localInfo.header.flags = elDebugQWORD;
//         }
//         else if (classRef == -4) {
//            localInfo.header.flags = elDebugReal64;
//         }
//         else if (classRef == -3) {
//            scope.moduleScope->loadClassInfo(localInfo, scope.moduleScope->subjectHints.get(type), true);
//            size = size * localInfo.size;
//            bytearray = true;
//         }
//      }
}

size_t CompilerLogic :: defineStructSize(_CompilerScope& scope, ref_t reference, bool embeddableOnly)
{
   ClassInfo classInfo;
   defineClassInfo(scope, classInfo, reference);

   return defineStructSize(classInfo, embeddableOnly);
}

size_t CompilerLogic :: defineStructSize(ClassInfo& info, bool embeddableOnly)
{
   //   variable = !test(classInfo.header.flags, elReadOnlyRole);
   
   if (test(info.header.flags, elStructureRole)) {
      if (!embeddableOnly || isEmbeddable(info))
         return info.size;
   }

   return 0;
}

void CompilerLogic :: tweakClassFlags(_CompilerScope& scope, ref_t classRef, ClassInfo& info)
{
   if (test(info.header.flags, elNestedClass)) {
      // stateless inline class
      if (info.fields.Count() == 0 && !test(info.header.flags, elStructureRole)) {
         info.header.flags |= elStateless;

         // stateless inline class is its own class class
         info.header.classRef = classRef;
      }
      else info.header.flags &= ~elStateless;

      // nested class is sealed
      info.header.flags |= elSealed;
   }

   // verify if the class may be a wrapper
   if (isWrappable(info.header.flags) && info.fields.Count() == 1 &&
      test(info.methodHints.get(Attribute(encodeVerb(DISPATCH_MESSAGE_ID), maHint)), tpEmbeddable))
   {
      if (test(info.header.flags, elStructureRole)) {
         ClassInfo::FieldInfo field = *info.fieldTypes.start();

         ClassInfo fieldInfo;
         defineClassInfo(scope, fieldInfo, field.value1, true);
         if (isEmbeddable(fieldInfo)) {
            // wrapper around embeddable object should be marked as embeddable wrapper
            info.header.flags |= elEmbeddableWrapper;

            if ((info.header.flags & elDebugMask) == 0)
               info.header.flags |= fieldInfo.header.flags & elDebugMask;
         }
      }
      else info.header.flags |= elWrapper;
   }

   // adjust literal wrapper
   if ((info.header.flags & elDebugMask) == elDebugLiteral) {
      info.header.flags &= ~elDebugMask;
      if (info.size == -2) {
         info.header.flags |= elDebugWideLiteral;
      }
      else if (info.size == -1) {
         info.header.flags |= elDebugLiteral;
      }
   }
}

bool CompilerLogic :: validateClassAttribute(int& attrValue)
{
   switch ((size_t)attrValue)
   {
      case V_SEALED:
         attrValue = elSealed;
         return true;
      case V_LIMITED:
         attrValue = elClosed;
         return true;
      case V_STRUCT:
         attrValue = elStructureRole;
         return true;
      case V_ENUMLIST:
         attrValue = elStateless | elEnumList | elClosed;
         return true;
      case V_EMBEDDABLE:
         attrValue = elStructureRole | elEmbeddable;
         return true;
      case V_DYNAMIC:
         attrValue = elDynamicRole;
         return true;
      case V_STRING:
         attrValue = elDebugLiteral;
         return true;
      case V_CONST:
         attrValue = elReadOnlyRole;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateMethodAttribute(int& attrValue)
{
   switch ((size_t)attrValue)
   {
      case V_IFBRANCH:
         attrValue = tpIfBranch;
         return true;
      case V_IFNOTBRANCH:
         attrValue = tpIfNotBranch;
         return true;
      case V_STATCKSAFE:
         attrValue = tpStackSafe;
         return true;
      case V_EMBEDDABLE:
         attrValue = tpEmbeddable;
         return true;
      case V_GENERIC:
         attrValue = tpGeneric;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateFieldAttribute(int& attrValue)
{
   switch ((size_t)attrValue)
   {
      case V_STATIC:
         attrValue = lxStaticAttr;
         return true;
      case V_INT32:
         attrValue = lxDWordAttr;
         return true;
      case V_PTR32:
         attrValue = lxPtrAttr;
         return true;
      case V_SIGNATURE:
         attrValue = lxSignatureAttr;
         return true;
      case V_MESSAGE:
         attrValue = lxMessageAttr;
         return true;
      case V_VERB:
         attrValue = lxVerbAttr;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateLocalAttribute(int& attrValue)
{
   if (attrValue == (int)V_INT32) {
      return true;
   }
   else return false;
}

bool CompilerLogic :: validateSymbolAttribute(int& attrValue)
{
   if (attrValue == (int)V_CONST) {
      attrValue = lxConstAttr;

      return true;
   }
   else return false;
}

bool CompilerLogic :: validateWarningAttribute(int& attrValue)
{
   switch ((size_t)attrValue)
   {
      case V_WARNING1:
         attrValue = WARNING_MASK_0;
         return true;
      case V_WARNING2:
         attrValue = WARNING_MASK_1;
         return true;
      case V_WARNING3:
         attrValue = WARNING_MASK_2;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: tweakPrimitiveClassFlags(LexicalType attr, ClassInfo& info)
{
   // if it is a primitive field
   if (info.fields.Count() == 1) {
      switch (attr) {
         case lxDWordAttr:
            info.header.flags |= (elDebugDWORD | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_INT32, 0));
            return true;
         case lxPtrAttr:
            info.header.flags |= (elDebugPTR | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_PTR32, 0));
            return info.size == 4;
         case lxSignatureAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_SIGNATURE, 0));
            return info.size == 4;
         case lxVerbAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_VERB, 0));
            return info.size == 4;
         case lxMessageAttr:
            info.header.flags |= (elDebugMessage | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_MESSAGE, 0));
            return info.size == 4;
            //            case -2:
            //               scope.info.header.flags |= (elDebugQWORD | elReadOnlyRole);
            //               break;
            //            case -4:
            //               scope.info.header.flags |= (elDebugReal64 | elReadOnlyRole);
            //               break;
            //            case -7:
            //               scope.info.header.flags |= (elDebugReference | elReadOnlyRole | elSymbol);
            //               break;
            //            case -8:
            //               scope.info.header.flags |= (elDebugSubject | elReadOnlyRole | elSignature);
            //               break;
            //            case -9:
            //               scope.info.header.flags |= (elDebugMessage | elReadOnlyRole | elMessage);
            //               break;
            //            case -10:
            //               scope.info.header.flags |= (elDebugMessage | elReadOnlyRole | elExtMessage);
            //               break;
         default:
            break;
      }
   }

   return false;
}

inline ref_t firstNonZero(ref_t ref1, ref_t ref2)
{
   return ref1 ? ref1 : ref2;
}

ref_t CompilerLogic :: resolvePrimitiveReference(_CompilerScope& scope, ref_t reference)
{
   switch (reference) {
      case V_INT32:
         return firstNonZero(scope.intReference, scope.superReference);
      case V_SIGNATURE:
         return firstNonZero(scope.signatureReference, scope.superReference);
      case V_MESSAGE:
         return firstNonZero(scope.messageReference, scope.superReference);
      case V_VERB:
         return firstNonZero(scope.verbReference, scope.superReference);
      default:
         return scope.superReference;
   }
}

ref_t CompilerLogic :: retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info)
{
   if (test(info.header.flags, elStructureWrapper)) {
      ClassInfo::FieldInfo field = info.fieldTypes.get(0);
      if (isPrimitiveRef(field.value1))
         return field.value1;
   }

   return 0;
}

ref_t CompilerLogic :: definePrimitiveArray(_CompilerScope& scope, ref_t elementRef)
{
   ClassInfo info;
   defineClassInfo(scope, info, elementRef, true);

   if (test(info.header.flags, elStructureWrapper)) {
      if (isCompatible(scope, V_INT32, elementRef)) {
         switch (info.size)
         {
            case 4:
               return V_INT32ARRAY;
            case 2:
               return V_INT16ARRAY;
            case 1:
               return V_INT8ARRAY;
            default:
               break;
         }
      }
   }

   return 0;
}

bool CompilerLogic :: validateClassFlag(ClassInfo& info, int flag)
{
   if (test(flag, elDynamicRole) && info.fields.Count() != 0)
      return false;

   return true;
}

bool CompilerLogic :: recognizeEmbeddableGet(_CompilerScope& scope, SNode root, ref_t returningType, ref_t& subject)
{
   if (returningType != 0 && defineStructSize(scope, scope.attributeHints.get(returningType), true) > 0) {
      root = root.findChild(lxNewFrame);

      if (SyntaxTree::matchPattern(root, lxObjectMask, 2,
         SNodePattern(lxExpression),
         SNodePattern(lxReturning)))
      {
         SNode message = SyntaxTree::findPattern(root, 2,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling));

         // if it is eval&subject2:var[1] message
         if (getParamCount(message.argument) != 1)
            return false;

         // check if it is operation with $self
         SNode target = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxThisLocal));

         //// if the target was optimized
         //if (target == lxExpression) {
         //   target = SyntaxTree::findChild(target, lxLocal);
         //}

         if (target == lxNone || target.argument != 1)
            return false;

         // check if the argument is returned
         SNode arg = SyntaxTree::findPattern(root, 4,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxExpression),
            SNodePattern(lxLocalAddress));

         if (arg == lxNone) {
            arg = SyntaxTree::findPattern(root, 5,
               SNodePattern(lxExpression),
               SNodePattern(lxDirectCalling, lxSDirctCalling),
               SNodePattern(lxExpression),
               SNodePattern(lxExpression),
               SNodePattern(lxLocalAddress));
         }

         SNode ret = SyntaxTree::findPattern(root, 4,
            SNodePattern(lxReturning),
            SNodePattern(lxExpression),
            SNodePattern(lxBoxing),
            SNodePattern(lxLocalAddress));

         if (arg != lxNone && ret != lxNone && arg.argument == ret.argument) {
            subject = getSignature(message.argument);

            return true;
         }
      }
   }

   return false;
}

bool CompilerLogic :: recognizeEmbeddableIdle(SNode methodNode)
{
   SNode object = SyntaxTree::findPattern(methodNode, 4,
      SNodePattern(lxNewFrame),
      SNodePattern(lxReturning),
      SNodePattern(lxExpression),
      SNodePattern(lxLocal));

   if (object == lxNone) {
      object = SyntaxTree::findPattern(methodNode, 3,
         SNodePattern(lxNewFrame),
         SNodePattern(lxReturning),
         SNodePattern(lxLocal));
   }

   return (object == lxLocal && object.argument == -1);
}

bool CompilerLogic :: optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node)
{
   SNode callNode = node.findSubNode(lxDirectCalling, lxSDirctCalling);
   SNode callTarget = callNode.findChild(lxCallTarget);

   ClassInfo info;
   defineClassInfo(scope, info, callTarget.argument);

   ref_t subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddableGet));
   // if it is possible to replace get&subject operation with eval&subject2:local
   if (subject != 0) {
      compiler.injectEmbeddableGet(node, callNode, subject);

      return true;
   }
   else return false;
}