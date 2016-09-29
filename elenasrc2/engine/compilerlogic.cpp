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

// --- CompilerLogic ---

CompilerLogic :: CompilerLogic()
{
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
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

         //// HOTFIX : if one of the operands is not primitive use it as a output result
         //if (isPrimitiveRef(result)) {

         //}

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

bool CompilerLogic :: isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef)
{
   if (!targetRef)
      return true;

   while (sourceRef != 0) {
      if (targetRef != sourceRef) {
         ClassInfo info;
         defineClassInfo(scope, info, sourceRef);

         // if it is a structure wrapper
         if (test(info.header.flags, elStructureWrapper)) {
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

bool CompilerLogic :: isVariable(_CompilerScope& scope, ref_t classReference)
{
   ClassInfo info;
   defineClassInfo(scope, info, classReference);

   return isVariable(info);
}

bool CompilerLogic :: isVariable(ClassInfo& info)
{
   return test(info.header.flags, elWrapper);
}

bool CompilerLogic :: isEmbeddable(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole | elEmbeddable);
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
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

bool CompilerLogic :: injectImplicitConversion(SNode node, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef)
{
   ClassInfo info;
   defineClassInfo(scope, info, targetRef);   

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper)) {      
      ClassInfo::FieldInfo inner = info.fieldTypes.get(0);
      if (isCompatible(scope, inner.value1, sourceRef)) {         
         compiler.injectBoxing(node, lxBoxing, test(info.header.flags, elStructureRole) ? info.size : 0, targetRef);

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
         info.header.flags = elDebugDWORD | elStructureRole;
         info.size = 4;
         break;
      case V_SIGNATURE:
      case V_VERB:
         info.header.parentRef = 0;
         info.header.flags = elDebugSubject | elStructureRole;
         info.size = 4;
         break;
      case V_MESSAGE:
         info.header.parentRef = 0;
         info.header.flags = elDebugMessage | elStructureRole;
         info.size = 4;
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

size_t CompilerLogic :: defineStructSize(_CompilerScope& scope, ref_t reference)
{
   ClassInfo classInfo;
   defineClassInfo(scope, classInfo, reference);

   return defineStructSize(classInfo);
}

size_t CompilerLogic :: defineStructSize(ClassInfo& info)
{
   //   variable = !test(classInfo.header.flags, elReadOnlyRole);
   //
   if (/*!embeddableOnly && */test(info.header.flags, elStructureRole)) {
      return info.size;
   }
   //   else if (isEmbeddable(classInfo)) {
   //      return classInfo.size;
   //   }
   //
   return 0;
}

void CompilerLogic :: tweakClassFlags(ref_t classRef, ClassInfo& info)
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
}

bool CompilerLogic :: validateClassAttribute(int& attrValue)
{
   return attrValue > 0;
}

bool CompilerLogic :: validateMethodAttribute(int& attrValue)
{
   if (attrValue == (int)V_IFBRANCH) {
      attrValue = tpIfBranch;

      return true;
   }
   else if (attrValue == (int)V_IFNOTBRANCH) {
      attrValue = tpIfNotBranch;

      return true;
   }
   else if (attrValue == (int)V_STATCKSAFE) {
      attrValue = tpStackSafe;

      return true;
   }
   else return false;
}

bool CompilerLogic :: validateFieldAttribute(int& attrValue)
{
   if (attrValue == (int)V_STATIC) {
      attrValue = lxStaticAttr;

      return true;
   }
   else if (attrValue == (int)V_INT32) {
      attrValue = lxDWordAttr;

      return true;
   }
   else if (attrValue == (int)V_SIGNATURE) {
      attrValue = lxSignatureAttr;

      return true;
   }
   else if (attrValue == (int)V_MESSAGE) {
      attrValue = lxMessageAttr;

      return true;
   }
   else if (attrValue == (int)V_VERB) {
      attrValue = lxVerbAttr;

      return true;
   }
   else return false;
}

bool CompilerLogic :: validateLocalAttribute(int& attrValue)
{
   if (attrValue == (int)V_INT32) {
      return true;
   }
   else return false;
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
         case lxSignatureAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_SIGNATURE, 0));
            return true;
         case lxVerbAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_VERB, 0));
            return true;
         case lxMessageAttr:
            info.header.flags |= (elDebugMessage | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_MESSAGE, 0));
            return true;
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
         return firstNonZero(scope.intReference, scope.signatureReference);
      case V_MESSAGE:
         return firstNonZero(scope.intReference, scope.messageReference);
      case V_VERB:
         return firstNonZero(scope.intReference, scope.verbReference);
      default:
         return scope.superReference;
   }
}
