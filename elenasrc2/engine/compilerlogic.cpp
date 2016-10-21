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

inline bool isPrimitiveStructArrayRef(ref_t classRef)
{
   switch (classRef)
   {
      case V_INT32ARRAY:
      case V_INT16ARRAY:
      case V_INT8ARRAY:
      case V_BINARYARRAY:
         return true;
      default:
         return false;
   }
}

inline bool isPrimitiveArrayRef(ref_t classRef)
{
   return classRef == V_OBJARRAY;
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

inline bool IsInvertedOperator(int& operator_id)
{
   switch (operator_id)
   {
      case NOTEQUAL_MESSAGE_ID:
         operator_id = EQUAL_MESSAGE_ID;

         return true;
      case NOTLESS_MESSAGE_ID:
         operator_id = LESS_MESSAGE_ID;

         return true;
      case NOTGREATER_MESSAGE_ID:
         operator_id = GREATER_MESSAGE_ID;

         return true;
      default:
         return false;
   }
}

// --- CompilerLogic ---

CompilerLogic :: CompilerLogic()
{
   // int32 primitive operations
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));

   operators.add(OperatorInfo(APPEND_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   operators.add(OperatorInfo(REDUCE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(LESS_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_FLAG));

   // subject primitive operations
   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_SIGNATURE, V_SIGNATURE, lxIntOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_SIGNATURE, V_SIGNATURE, lxIntOp, V_FLAG));

   // int64 primitive operations
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_INT64));

   operators.add(OperatorInfo(APPEND_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));
   operators.add(OperatorInfo(REDUCE_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));
   operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));
   operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));
   operators.add(OperatorInfo(LESS_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, V_FLAG));

   // real64 primitive operations
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));

   operators.add(OperatorInfo(APPEND_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   operators.add(OperatorInfo(REDUCE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(LESS_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));

   // array of int32 primitive operations
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT32ARRAY, V_INT32, V_INT32, lxIntArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT32ARRAY, V_INT32, lxIntArrOp, 0));

   // array of int16 primitive operations
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT16ARRAY, V_INT32, V_INT32, lxShortArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT16ARRAY, V_INT32, lxShortArrOp, 0));

   // array of int8 primitive operations
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT8ARRAY, V_INT32, V_INT32, lxByteArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT8ARRAY, V_INT32, lxByteArrOp, 0));

   // array of object primitive operations
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_OBJARRAY, V_INT32, lxArrOp, 0));

   // array of structures primitive operations
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_BINARYARRAY, V_INT32, lxBinArrOp, 0));
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
   found = defineClassInfo(scope, info, reference);

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

bool CompilerLogic :: loadBranchingInfo(_CompilerScope& scope, _Compiler& compiler, ref_t reference)
{
   if (scope.branchingInfo.trueRef == reference || scope.branchingInfo.falseRef == reference)
      return true;

   ClassInfo info;
   scope.loadClassInfo(info, reference, true);

   if ((info.header.flags & elDebugMask) == elEnumList) {
      _Module* extModule = NULL;
      _Memory* listSection = NULL;

      while (true) {
         extModule = scope.loadReferenceModule(reference);
         listSection = extModule ? extModule->mapSection(reference | mskConstArray, true) : NULL;

         if (listSection == NULL && info.header.parentRef != 0) {
            reference = info.header.parentRef;

            scope.loadClassInfo(info, reference, true);
         }
         else break;
      }

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
            scope.branchingInfo.reference = reference;
            scope.branchingInfo.trueRef = trueRef;
            scope.branchingInfo.falseRef = falseRef;

            return true;
         }
      }
   }

   return false;
}

bool CompilerLogic :: resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference)
{
   if (!loperand)
      return false;

   if (loperand != scope.branchingInfo.reference) {
      if (!loadBranchingInfo(scope, compiler, loperand))
         return false;
   }

   reference = operatorId == IF_MESSAGE_ID ? scope.branchingInfo.trueRef : scope.branchingInfo.falseRef;

   return true;

}

int CompilerLogic :: resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result)
{
   if (isCompatible(scope, V_INT32, roperand)) {
      result = definePrimitiveArray(scope, loperand);
      if (result != 0)
         return lxNewOp;
   }

   return 0;
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

   if (sourceRef == V_NIL)
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

void CompilerLogic :: injectOperation(SNode node, _CompilerScope& scope, _Compiler& compiler, int operator_id, int operationType, ref_t& reference, int size)
{
   bool inverting = IsInvertedOperator(operator_id);

   SNode operationNode = node.injectNode((LexicalType)operationType, operator_id);
   if (size != 0) {
      // HOTFIX : inject an item size for the binary array operations
      operationNode.appendNode(lxSize, size);
   }

   if (reference == V_FLAG) {      
      if (!scope.branchingInfo.reference) {
         // HOTFIX : resolve boolean symbols
         ref_t dummy;
         resolveBranchOperation(scope, compiler, IF_MESSAGE_ID, scope.boolReference, dummy);
      }

      reference = scope.branchingInfo.reference;
      if (inverting) {
         operationNode.appendNode(lxIfValue, scope.branchingInfo.falseRef);
         operationNode.appendNode(lxElseValue, scope.branchingInfo.trueRef);
      }
      else {
         operationNode.appendNode(lxIfValue, scope.branchingInfo.trueRef);
         operationNode.appendNode(lxElseValue, scope.branchingInfo.falseRef);
      }
   }
}

bool CompilerLogic :: isReadonly(ClassInfo& info)
{
   return test(info.header.flags, elReadOnlyRole);
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
            isReadonly(info) ? lxBoxing : lxUnboxing,
            test(info.header.flags, elStructureRole) ? info.size : 0, targetRef);

         return true;
      }
   }

   // HOT FIX : trying to typecast primitive structure array
   if (isPrimitiveStructArrayRef(sourceRef) && test(info.header.flags, elStructureRole | elDynamicRole)) {
      ClassInfo sourceInfo;      
      if (sourceRef == V_BINARYARRAY && sourceType != 0) {
         // HOTFIX : for binary array of structures - sourceType  contains the element size
         ref_t elementRef = scope.attributeHints.get(sourceType);

         defineClassInfo(scope, sourceInfo, elementRef, true);
         if (-sourceInfo.size == info.size && isCompatible(scope, elementRef, info.fieldTypes.get(-1).value1)) {
            compiler.injectBoxing(scope, node,
               test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, info.size, targetRef);

            return true;
         }
      }
      else {
         defineClassInfo(scope, sourceInfo, sourceRef, true);
         if (sourceInfo.size == info.size && isCompatible(scope, definePrimitiveArrayItem(sourceRef), info.fieldTypes.get(-1).value1)) {
            compiler.injectBoxing(scope, node,
               test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, info.size, targetRef);

            return true;
         }
      }
   }

   // HOTFIX : trying to typecast primitive array
   if (isPrimitiveArrayRef(sourceRef) && test(info.header.flags, elDynamicRole | elNonStructureRole)) {
      ClassInfo sourceInfo;
      defineClassInfo(scope, sourceInfo, sourceRef, true);

      if (isCompatible(scope, sourceType, info.fieldTypes.get(-1).value1)) {
         compiler.injectBoxing(scope, node,
            test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, 0, targetRef);

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
         else if (test(info.header.flags, elDynamicRole)) {
            return false;
         }
         else compiler.injectConverting(node, lxDirectCalling, implicitMessage, lxCreatingClass, info.fields.Count(), targetRef);

         return true;
      }
   }

   return false;
}

void CompilerLogic :: injectNewOperation(SNode node, _CompilerScope& scope, int operation, ref_t elementType, ref_t targetRef)
{
   SNode operationNode = node.injectNode((LexicalType)operation, targetRef);

   int size = defineStructSize(scope, targetRef, elementType, false);
   if (size != 0)
      operationNode.appendNode(lxSize, size);
}

bool CompilerLogic :: defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly)
{
   if (isPrimitiveRef(reference) && !headerOnly) {
      scope.loadClassInfo(info, scope.superReference);
   }

   switch (reference)
   {
      case V_INT32:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugDWORD | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_INT64:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugQWORD | elStructureRole | elEmbeddable;
         info.size = 8;
         break;
      case V_REAL64:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugReal64 | elStructureRole | elEmbeddable;
         info.size = 8;
         break;
      case V_PTR32:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugPTR | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_SIGNATURE:
      case V_VERB:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugSubject | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_MESSAGE:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugMessage | elStructureRole | elEmbeddable;
         info.size = 4;
         break;
      case V_INT32ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugIntegers | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -4;
         break;
      case V_INT16ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugShorts | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -2;
         break;
      case V_INT8ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugBytes | elStructureRole | elDynamicRole | elEmbeddable;
         info.size = -1;
         break;
      case V_OBJARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugArray | elDynamicRole;
         info.size = 0;
         break;
      case V_BINARYARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDynamicRole | elStructureRole;
         info.size = -1;
         break;
      default:
         if (reference != 0) {
            if (!scope.loadClassInfo(info, reference, headerOnly))
               return false;
         }
         else {
            info.header.parentRef = 0;
            info.header.flags = 0;
            info.size = 0;
         }
         break;
   }

//      else if (isPrimitiveRef(classRef)) {
//         else if (classRef == -3) {
//            scope.moduleScope->loadClassInfo(localInfo, scope.moduleScope->subjectHints.get(type), true);
//            size = size * localInfo.size;
//            bytearray = true;
//         }
//      }

   return true;
}

int CompilerLogic :: defineStructSize(_CompilerScope& scope, ref_t reference, ref_t elementType, bool embeddableOnly)
{
   if (reference == V_BINARYARRAY && elementType != 0) {
      // HOTFIX : binary array of structures
      ref_t elementRef = scope.attributeHints.get(elementType);

      return -defineStructSize(scope, elementRef, 0, false);
   }
   else {
      ClassInfo classInfo;
      defineClassInfo(scope, classInfo, reference);

      return defineStructSize(classInfo, embeddableOnly);
   }
}

int CompilerLogic :: defineStructSize(ClassInfo& info, bool embeddableOnly)
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

   if (test(info.header.flags, elExtension)) {
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

   // adjust array
   if (test(info.header.flags, elDynamicRole) && !testany(info.header.flags, elStructureRole | elNonStructureRole)) {
      info.header.flags |= elNonStructureRole;

      if ((info.header.flags & elDebugMask) == 0) {
         info.header.flags |= elDebugArray;
      }
   }

   // adjust binary array
   if (test(info.header.flags, elDynamicRole | elStructureRole)) {
      if ((info.header.flags & elDebugMask) == 0) {
         info.header.flags |= elDebugBytes;
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
      case V_EXTENSION:
         attrValue = elExtension;
         return true;
      case V_NOSTRUCT:
         attrValue = elNonStructureRole;
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
      case V_SEALED:
         attrValue = tpSealed;
         return true;
      case V_ACTION:
         attrValue = tpAction;
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
      case V_INT64:
         attrValue = lxQWordAttr;
         return true;
      case V_REAL64:
         attrValue = lxRealAttr;
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
         case lxQWordAttr:
            info.header.flags |= (elDebugQWORD | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_INT64, 0));
            return true;
         case lxRealAttr:
            info.header.flags |= (elDebugReal64 | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_REAL64, 0));
            return true;
         case lxPtrAttr:
            info.header.flags |= (elDebugPTR | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_PTR32, 0));
            return info.size == 4;
         case lxSignatureAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper | elSignature);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_SIGNATURE, 0));
            return info.size == 4;
         case lxVerbAttr:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_VERB, 0));
            return info.size == 4;
         case lxMessageAttr:
            info.header.flags |= (elDebugMessage | elReadOnlyRole | elWrapper | elMessage);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_MESSAGE, 0));
            return info.size == 4;
            //            case -7:
            //               scope.info.header.flags |= (elDebugReference | elReadOnlyRole | elSymbol);
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
      case V_INT64:
         return firstNonZero(scope.longReference, scope.superReference);
      case V_REAL64:
         return firstNonZero(scope.realReference, scope.superReference);
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

   if (isEmbeddable(info)) {
      if (isCompatible(scope, V_INT32, elementRef)) {
         switch (info.size) {
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
      return V_BINARYARRAY;
   }
   else return V_OBJARRAY;

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
   if (returningType != 0 && defineStructSize(scope, scope.attributeHints.get(returningType), 0, true) > 0) {
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

bool CompilerLogic :: optimizeEmbeddableBoxing(_CompilerScope& scope, _Compiler& compiler, SNode node, ref_t targetRef)
{
   SNode exprNode = node.findSubNodeMask(lxObjectMask);

   bool variable = !isReadonly(scope, targetRef);

   if (exprNode == lxFieldAddress && exprNode.argument > 0/* && !test(mode, HINT_ASSIGNING)*/) {
      compiler.injectLocalBoxing(exprNode, node.argument);

      node = variable ? lxLocalUnboxing : lxExpression;
      
      return true;
   }

   return false;
}