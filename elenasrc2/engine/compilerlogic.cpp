//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class implementation.
//
//                                              (C)2005-2017, by Alexei Rakov
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

// --- CompilerLogic Optimization Ops ---
struct EmbeddableOp
{
   int attribute;
   int paramCount;
   int verb;

   EmbeddableOp(int attr, int count, int verb)
   {
      this->attribute = attr;
      this->paramCount = count;
      this->verb = verb;
   }
};
#define EMBEDDABLEOP_MAX 4
EmbeddableOp embeddableOps[EMBEDDABLEOP_MAX] =
{
   EmbeddableOp(maEmbeddableGetAt, 2, READ_MESSAGE_ID),
   EmbeddableOp(maEmbeddableGetAt2, 3, READ_MESSAGE_ID),
   EmbeddableOp(maEmbeddableEval, 2, EVAL_MESSAGE_ID),
   EmbeddableOp(maEmbeddableEval2, 3, EVAL_MESSAGE_ID)
};

// --- CompilerLogic ---

CompilerLogic :: CompilerLogic()
{
   // nil
   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_NIL, 0, lxNilOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_NIL, 0, lxNilOp, V_FLAG));

   // int32 primitive operations
   operators.add(OperatorInfo(ADD_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SUB_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(MUL_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(DIV_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(AND_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(OR_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(XOR_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(WRITE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, V_INT32));

   //operators.add(OperatorInfo(APPEND_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   //operators.add(OperatorInfo(REDUCE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   //operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));
   //operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_INT32, V_INT32, lxIntOp, 0));

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
   operators.add(OperatorInfo(ADD_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(SUB_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(MUL_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(DIV_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(AND_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(OR_MESSAGE_ID,    V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(XOR_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(READ_MESSAGE_ID,  V_INT64, V_INT32, lxLongOp, V_INT64));
   operators.add(OperatorInfo(WRITE_MESSAGE_ID, V_INT64, V_INT32, lxLongOp, V_INT64));

   //operators.add(OperatorInfo(APPEND_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, 0));
   //operators.add(OperatorInfo(REDUCE_MESSAGE_ID,   V_INT64, V_INT64, lxLongOp, 0));
   //operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));
   //operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_INT64, V_INT64, lxLongOp, 0));

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

   //operators.add(OperatorInfo(APPEND_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   //operators.add(OperatorInfo(REDUCE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   //operators.add(OperatorInfo(INCREASE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));
   //operators.add(OperatorInfo(SEPARATE_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, 0));

   operators.add(OperatorInfo(EQUAL_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(LESS_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_MESSAGE_ID, V_REAL64, V_REAL64, lxRealOp, V_FLAG));

   // array of int32 primitive operations
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_INT32ARRAY, V_INT32, lxIntArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT32ARRAY, V_INT32, V_INT32, lxIntArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT32ARRAY, V_INT32, lxIntArrOp, 0));

   // array of int16 primitive operations
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_INT16ARRAY, V_INT32, lxShortArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT16ARRAY, V_INT32, V_INT32, lxShortArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT16ARRAY, V_INT32, lxShortArrOp, 0));

   // array of int8 primitive operations
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_INT8ARRAY, V_INT32, lxByteArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_INT8ARRAY, V_INT32, V_INT32, lxByteArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_INT8ARRAY, V_INT32, lxByteArrOp, 0));

   // array of object primitive operations
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_OBJARRAY, V_INT32, lxArrOp, V_OBJECT));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_OBJARRAY, V_INT32, 0, lxArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_OBJARRAY, V_INT32, lxArrOp, 0));

   // array of structures primitive operations
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_BINARYARRAY, V_INT32, lxBinArrOp, V_BINARY));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_BINARYARRAY, V_INT32, 0, lxBinArrOp, 0));
   operators.add(OperatorInfo(READ_MESSAGE_ID, V_BINARYARRAY, V_INT32, lxBinArrOp, 0));

   // array of arg list
   operators.add(OperatorInfo(REFER_MESSAGE_ID, V_ARGARRAY, V_INT32, lxArgArrOp, 0));
   operators.add(OperatorInfo(SET_REFER_MESSAGE_ID, V_ARGARRAY, V_INT32, 0, lxArgArrOp, 0));
   //operators.add(OperatorInfo(READ_MESSAGE_ID, V_OBJARRAY, V_INT32, lxArrOp, 0));
}

int CompilerLogic :: checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result)
{
   bool methodFound = info.methods.exist(message);

   if (methodFound) {
      int hint = info.methodHints.get(Attribute(message, maHint));
      result.outputReference = info.methodHints.get(Attribute(message, maReference));

      result.embeddable = test(hint, tpEmbeddable);

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

int CompilerLogic :: checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result)
{
   ClassInfo info;
   result.found = defineClassInfo(scope, info, reference);

   if (result.found) {
      if (!test(info.header.flags, elClosed))
         result.closed = false;

      if (test(info.header.flags, elWithCustomDispatcher))
         result.withCustomDispatcher = true;

      int hint = checkMethod(info, message, result);
      if (hint == tpUnknown && test(info.header.flags, elWithArgGenerics)) {
         hint = checkMethod(info, overwriteParamCount(message, OPEN_ARG_COUNT), result);
         if (hint != tpUnknown)
            result.withOpenArgDispatcher = true;
      }

      return hint;
   }
   else return tpUnknown;
}

int CompilerLogic :: resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t messageRef, ChechMethodInfo& result)
{
   if (isPrimitiveRef(classReference)) {
      classReference = resolvePrimitiveReference(scope, classReference);
   }

   int methodHint = checkMethod(scope, classReference != 0 ? classReference : scope.superReference, messageRef, result);
   int callType = methodHint & tpMask;
   if (callType == tpClosed || callType == tpSealed) {
      result.stackSafe = test(methodHint, tpStackSafe);
   }      

   return callType;
}

int CompilerLogic :: resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result)
{
   if (loperand == 0 || (roperand == 0 && loperand != V_NIL))
      return 0;

   OperatorList::Iterator it = operators.start();
   while (!it.Eof()) {
      OperatorInfo info = *it;

      if (info.operatorId == operatorId) {
         if (info.loperand == V_NIL) {
            if (loperand == V_NIL) {
               result = info.result;

               return info.operationType;
            }
         }
         else if (isCompatible(scope, info.loperand, loperand) && isCompatible(scope, info.roperand, roperand)) {
            result = info.result;

            return info.operationType;
         }
      }

      it++;
   }

   return 0;
}

int CompilerLogic :: resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result)
{
   if (loperand == 0 || roperand == 0 || (roperand2 == 0 && loperand != V_OBJARRAY))
      return 0;

   OperatorList::Iterator it = operators.start();
   while (!it.Eof()) {
      OperatorInfo info = *it;

      if (info.operatorId == operatorId) {
         if (info.loperand == V_NIL) {
            // skip operation with NIL
         }
         else if (isCompatible(scope, info.loperand, loperand) && isCompatible(scope, info.roperand, roperand)
            && isCompatible(scope, info.roperand2, roperand2)) 
         {
            result = info.result;

            return info.operationType;
         }

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
      case V_INT32:
         return targetRef == V_PTR32;
      default:
         return false;
   }
}

//inline bool isPrimitiveArrayCompatible(ref_t targetRef, ref_t sourceRef)
//{
//   switch (sourceRef)
//   {
//      case V_PTR32:
//         return targetRef == V_INT32;
//      default:
//         return false;
//   }
//}

bool CompilerLogic :: isCompatibleWithType(_CompilerScope& scope, ref_t targetRef, ref_t type)
{
   while (targetRef != 0) {
      if (scope.subjectHints.exist(type, targetRef))
         return true;

      ClassInfo info;
      defineClassInfo(scope, info, targetRef);
      targetRef = info.header.parentRef;
   }

   return false;
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

void CompilerLogic :: injectVirtualCode(_CompilerScope& scope, ref_t classRef, ClassInfo& info, _Compiler& compiler)
{
//   SNode templateNode = node.appendNode(lxTemplate);

   // generate enumeration list
   if ((info.header.flags & elDebugMask) == elEnumList && test(info.header.flags, elNestedClass)) {
      compiler.generateEnumListMember(scope, info.header.parentRef, classRef);
   }
}

void CompilerLogic :: injectOperation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, int operator_id, int operationType, ref_t& reference, ref_t elementType)
{
   int size = 0;
   if (operationType == lxBinArrOp) {
      // HOTFIX : define an item size for the binary array operations
      size = -defineStructSize(scope, V_BINARYARRAY, elementType);
   }

   if (reference == V_BINARY && elementType != 0) {
      reference = scope.subjectHints.get(elementType);
   }
   else if (reference == V_OBJECT && elementType != 0) {
      reference = scope.subjectHints.get(elementType);
   }

   bool inverting = IsInvertedOperator(operator_id);

   if (reference == V_FLAG) {      
      if (!scope.branchingInfo.reference) {
         // HOTFIX : resolve boolean symbols
         ref_t dummy;
         resolveBranchOperation(scope, compiler, IF_MESSAGE_ID, scope.boolReference, dummy);
      }

      reference = scope.branchingInfo.reference;
      if (inverting) {
         writer.appendNode(lxIfValue, scope.branchingInfo.falseRef);
         writer.appendNode(lxElseValue, scope.branchingInfo.trueRef);
      }
      else {
         writer.appendNode(lxIfValue, scope.branchingInfo.trueRef);
         writer.appendNode(lxElseValue, scope.branchingInfo.falseRef);
      }
   }

   if (size != 0) {
      // HOTFIX : inject an item size for the binary array operations
      writer.appendNode(lxSize, size);
   }

   writer.insert((LexicalType)operationType, operator_id);
   writer.closeNode();
}

bool CompilerLogic :: isReadonly(ClassInfo& info)
{
   return test(info.header.flags, elReadOnlyRole);
}

bool CompilerLogic :: injectImplicitConversion(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t sourceType)
{
   ClassInfo info;
   defineClassInfo(scope, info, targetRef);   

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper)) {
      ClassInfo::FieldInfo inner = info.fieldTypes.get(0);

      bool compatible = false;
      if (test(info.header.flags, elStructureWrapper)) {
         if (isPrimitiveRef(sourceRef)) {
            compatible = isCompatible(scope, sourceRef, inner.value1);
         }
         // HOTFIX : the size should be taken into account as well (e.g. byte and int both V_INT32)
         else compatible = isCompatible(scope, inner.value1, sourceRef) && info.size == defineStructSize(scope, sourceRef);
      }
      else compatible = isCompatible(scope, inner.value1, sourceRef);

      if (compatible) {
         compiler.injectBoxing(writer, scope, 
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
         ref_t elementRef = scope.subjectHints.get(sourceType);

         defineClassInfo(scope, sourceInfo, elementRef, true);
         if (-sourceInfo.size == info.size && isCompatible(scope, elementRef, info.fieldTypes.get(-1).value1)) {
            compiler.injectBoxing(writer, scope, 
               test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, info.size, targetRef);

            return true;
         }
      }
      else {
         defineClassInfo(scope, sourceInfo, sourceRef, true);
         if (sourceInfo.size == info.size && isCompatible(scope, definePrimitiveArrayItem(sourceRef), info.fieldTypes.get(-1).value1)) {
            compiler.injectBoxing(writer, scope,
               test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, info.size, targetRef);

            return true;
         }
      }
   }

   // HOTFIX : trying to typecast primitive array
   if (isPrimitiveArrayRef(sourceRef) && test(info.header.flags, elDynamicRole | elNonStructureRole)) {
      ClassInfo sourceInfo;
      defineClassInfo(scope, sourceInfo, sourceRef, true);

      ref_t elementRef = scope.subjectHints.get(sourceType);

      if (isCompatible(scope, elementRef, info.fieldTypes.get(-1).value1)) {
         compiler.injectBoxing(writer, scope,
            test(info.header.flags, elReadOnlyRole) ? lxBoxing : lxUnboxing, 0, targetRef);

         return true;
      }
   }

   // check if there are implicit constructors
   if (test(info.header.flags, elSealed)) {
      if (isPrimitiveRef(sourceRef))
         // HOTFIX : recognize primitive data except of a constant literal
         if (sourceRef != V_STRCONSTANT)
            sourceRef = resolvePrimitiveReference(scope, sourceRef);

   //   if (sourceType != 0) {
   //      // if the source type is defined we are lucky
   //      int implicitMessage = encodeMessage(sourceType, PRIVATE_MESSAGE_ID, 1);
   //      if (info.methods.exist(implicitMessage)) {
   //         if (test(info.header.flags, elStructureRole)) {
   //            compiler.injectConverting(node, lxDirectCalling, implicitMessage, lxCreatingStruct, info.size, targetRef);
   //         }
   //         else if (test(info.header.flags, elDynamicRole)) {
   //            return false;
   //         }
   //         else compiler.injectConverting(node, lxDirectCalling, implicitMessage, lxCreatingClass, info.fields.Count(), targetRef);

   //         return true;
   //      }
   //   }
   //   else {
         // otherwise we have to go through the list
         ClassInfo::MethodMap::Iterator it = info.methods.start();
         while (!it.Eof()) {
            pos_t implicitMessage = it.key();
            if (getVerb(implicitMessage) == PRIVATE_MESSAGE_ID && getParamCount(implicitMessage) == 1) {
               ref_t subj = getSignature(implicitMessage);
               bool compatible = false;
               if (sourceRef == V_STRCONSTANT) {
                  // try to resolve explicit constant conversion routine
                  ident_t signature = scope.module->resolveSubject(subj);
                  size_t index = signature.find('&');
                  if (index != NOTFOUND_POS) {
                     IdentifierString postfix(signature, index);
                     ref_t postfixRef = scope.module->mapSubject(postfix, true);
                     if (sourceType == postfixRef) {
                        ref_t subjRef = scope.subjectHints.get(scope.module->mapSubject(signature + index + 1, false));

                        compatible = subjRef != 0 && isCompatible(scope, subjRef, scope.literalReference);
                     }
                  }
               }
               else {
                  ref_t subjRef = scope.subjectHints.get(subj);

                  compatible = subjRef != 0 && isCompatible(scope, subjRef, sourceRef);
               }

               if (compatible) {
                  bool stackSafe = test(info.methodHints.get(Attribute(implicitMessage, maHint)), tpStackSafe);
                  if (test(info.header.flags, elStructureRole)) {
                     compiler.injectConverting(writer, lxDirectCalling, implicitMessage, lxCreatingStruct, info.size, targetRef, stackSafe);
                  }
                  else if (test(info.header.flags, elDynamicRole)) {
                     return false;
                  }
                  else compiler.injectConverting(writer, lxDirectCalling, implicitMessage, lxCreatingClass, info.fields.Count(), targetRef, stackSafe);

                  return true;
               }
            }

            it++;
         }
   //   }
   }

   return false;
}

void CompilerLogic :: injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t elementType, ref_t targetRef)
{
   int size = defineStructSize(scope, targetRef, elementType, false);
   if (size != 0)
      writer.appendNode(lxSize, size);

   writer.insert((LexicalType)operation, targetRef);
   writer.closeNode();
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
      case V_EXTMESSAGE:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugMessage | elStructureRole | elEmbeddable;
         info.size = 8;
         break;
      case V_SYMBOL:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugReference | elStructureRole | elEmbeddable;
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

   return true;
}

int CompilerLogic :: defineStructSize(_CompilerScope& scope, ref_t reference, ref_t elementType, bool embeddableOnly)
{
   if (reference == V_BINARYARRAY && elementType != 0) {
      // HOTFIX : binary array of structures
      ref_t elementRef = scope.subjectHints.get(elementType);

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

void CompilerLogic :: tweakClassFlags(_CompilerScope& scope, ref_t classRef, ClassInfo& info, bool classClassMode)
{
   if (classClassMode) {
      // class class is always stateless and sealed
      info.header.flags |= elStateless;
      info.header.flags |= elSealed;
   }

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
         ref_t itemRef = info.fieldTypes.get(-1).value1;
         if (isCompatible(scope, V_INT32, itemRef)) {
            switch (info.size) {
               case -4:
                  info.header.flags |= elDebugIntegers;
                  break;
               case -2:
                  info.header.flags |= elDebugShorts;
                  break;
               case -1:
               default:
                  info.header.flags |= elDebugBytes;
                  break;
            }
         }
         else info.header.flags |= elDebugBytes;
      }
   }

   // adjust objects with custom dispatch handler
   if (info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), true) && classRef != scope.superReference) {
      info.header.flags |= elWithCustomDispatcher;
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
         attrValue = elEmbeddable;
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
      case V_GROUP:
         attrValue = elGroup;
         return true;
      case V_TAPEGROUP:
         attrValue = elTapeGroup;
         return true;
      case V_CLASS:
         attrValue = 0;
         return true;
      case V_SINGLETON:
         attrValue = elRole | elNestedClass;
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
      case V_CONSTRUCTOR:
         attrValue = tpConstructor;
         return true;
      case V_CONVERSION:
         attrValue = tpConversion;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateDeclarationAttribute(int attrValue, DeclarationAttr& declType)
{
   DeclarationAttr attr = daNone;
   switch ((size_t)attrValue) {
      case V_TYPETEMPL:
         attr = daType;
         break;
      case V_CLASS:
      case V_STRUCT:
      case V_STRING:
         attr = daClass;
         break;
      case V_TEMPLATE:
         attr = daTemplate;
         break;
      case V_FIELD:
         attr = daField;
         break;
      case V_METHOD:
         attr = daMethod;
         break;
      case V_LOOP:
         attr = daLoop;
         break;
      case V_IMPORT:
         attr = daImport;
         break;
      case V_EXTERN:
         attr = daExtern;
         break;
      default:
         return true;
   }

   declType = (DeclarationAttr)(declType | attr);
   return true;
}

bool CompilerLogic :: validateFieldAttribute(int& attrValue)
{
   switch ((size_t)attrValue)
   {
//      case V_STATIC:
//         attrValue = lxStaticAttr;
//         return true;
      case V_INT32:
         attrValue = 0;
         return true;
      case V_INT64:
         attrValue = 0;
         return true;
      case V_REAL64:
         attrValue = 0;
         return true;
      case V_PTR32:
         attrValue = 0;
         return true;
      case V_SIGNATURE:
         attrValue = 0;
         return true;
      case V_SYMBOL:
         attrValue = 0;
         return true;
      case V_MESSAGE:
         attrValue = 0;
         return true;
      case V_EXTMESSAGE:
         attrValue = 0;
         return true;
      case V_VERB:
         attrValue = 0;
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
   else if (attrValue == (int)V_VARIABLE) {
      attrValue = 0;

      return true;
   }
   else return false;
}

bool CompilerLogic :: validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne)
{
   if (attrValue == (int)V_CONST) {
      constant = true;

      return true;
   }
   else if (attrValue == (int)V_SYMBOLEXPR) {
      return true;
   }
   else if (attrValue == (int)V_STATIC) {
      staticOne = true;

      return true;
   }
   else if (attrValue == (int)V_PRELOADED) {
      preloadedOne = true;

      return true;
   }
   else return false;
}

////bool CompilerLogic :: validateWarningAttribute(int& attrValue)
////{
////   switch ((size_t)attrValue)
////   {
////      case V_WARNING1:
////         attrValue = WARNING_MASK_0;
////         return true;
////      case V_WARNING2:
////         attrValue = WARNING_MASK_1;
////         return true;
////      case V_WARNING3:
////         attrValue = WARNING_MASK_2;
////         return true;
////      default:
////         return false;
////   }
////}

bool CompilerLogic :: tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info)
{
   // if it is a primitive field
   if (info.fields.Count() == 1) {
      switch (classRef) {
         case V_INT32:
            info.header.flags |= (elDebugDWORD | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_INT32, 0));
            return true;
         case V_INT64:
            info.header.flags |= (elDebugQWORD | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_INT64, 0));
            return true;
         case V_REAL64:
            info.header.flags |= (elDebugReal64 | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_REAL64, 0));
            return true;
         case V_PTR32:
            info.header.flags |= (elDebugPTR | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_PTR32, 0));
            return info.size == 4;
         case V_SIGNATURE:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper | elSignature);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_SIGNATURE, 0));
            return info.size == 4;
         case V_VERB:
            info.header.flags |= (elDebugSubject | elReadOnlyRole | elWrapper);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_VERB, 0));
            return info.size == 4;
         case V_MESSAGE:
            info.header.flags |= (elDebugMessage | elReadOnlyRole | elWrapper | elMessage);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_MESSAGE, 0));
            return info.size == 4;
         case V_EXTMESSAGE:
            info.header.flags |= (elDebugMessage | elReadOnlyRole | elWrapper | elExtMessage);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_EXTMESSAGE, 0));
            return info.size == 8;
         case V_SYMBOL:
            info.header.flags |= (elDebugReference | elReadOnlyRole | elWrapper | elSymbol);
            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_SYMBOL, 0));
            return info.size == 4;
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
      case V_ARGARRAY:
         return firstNonZero(scope.arrayReference, scope.superReference);
      default:
         return scope.superReference;
   }
}

ref_t CompilerLogic :: retrievePrimitiveReference(_CompilerScope&, ClassInfo& info)
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

////bool CompilerLogic :: validateClassFlag(ClassInfo& info, int flag)
////{
////   if (test(flag, elDynamicRole) && info.fields.Count() != 0)
////      return false;
////
////   return true;
////}

bool CompilerLogic :: recognizeEmbeddableGet(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject)
{
   if (returningType != 0 && defineStructSize(scope, scope.subjectHints.get(returningType), 0, true) > 0) {
      root = root.findChild(lxNewFrame);

      if (root.existChild(lxReturning)) {
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
            SNodePattern(lxThisLocal, lxLocal));

         if (target == lxLocal && target.argument == -1 && extensionRef != 0) {            
            if (message.findChild(lxCallTarget).argument != extensionRef)
               return false;
         }
         else if (target != lxThisLocal || target.argument != 1)
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

         SNode ret = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxReturning),
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

bool CompilerLogic :: recognizeEmbeddableOp(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t verb, ref_t& subject)
{
   if (returningType != 0 && defineStructSize(scope, scope.subjectHints.get(returningType), 0, true) > 0) {
      root = root.findChild(lxNewFrame);

      if (root.existChild(lxReturning))
      {
         SNode message = SyntaxTree::findPattern(root, 2,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling));

         // if it is read&subject&var[2] message
         if (getParamCount(message.argument) != 2 || getVerb(message.argument) != (ref_t)verb)
            return false;

         // check if it is operation with $self
         SNode target = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxThisLocal, lxLocal));

         if (target == lxLocal && target.argument == -1 && extensionRef != 0) {
            if (message.findChild(lxCallTarget).argument != extensionRef)
               return false;
         }
         else if (target != lxThisLocal || target.argument != 1)
            return false;

         // check if the index is used
         SNode indexArg = target.nextNode(lxObjectMask);

         if (indexArg == lxExpression)
            indexArg = indexArg.firstChild(lxObjectMask);

         if (indexArg.type != lxLocal || indexArg.argument != (ref_t)-2)
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

         SNode ret = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxReturning),
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

bool CompilerLogic :: recognizeEmbeddableOp2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t verb, ref_t& subject)
{
   if (returningType != 0 && defineStructSize(scope, scope.subjectHints.get(returningType), 0, true) > 0) {
      root = root.findChild(lxNewFrame);

      if (root.existChild(lxReturning))
      {
         SNode message = SyntaxTree::findPattern(root, 2,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling));

         // if it is read&index1&index2&var[2] message
         if (getParamCount(message.argument) != 3 || getVerb(message.argument) != verb)
            return false;

         // check if it is operation with $self
         SNode target = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxThisLocal, lxLocal));

         if (target == lxLocal && target.argument == -1 && extensionRef != 0) {
            if (message.findChild(lxCallTarget).argument != extensionRef)
               return false;
         }
         else if (target != lxThisLocal || target.argument != 1)
            return false;

         // check if the index is used
         SNode index1Arg = target.nextNode(lxObjectMask);
         SNode index2Arg = index1Arg.nextNode(lxObjectMask);

         if (index1Arg == lxExpression)
            index1Arg = index1Arg.firstChild(lxObjectMask);

         if (index2Arg == lxExpression)
            index2Arg = index2Arg.firstChild(lxObjectMask);

         if (index1Arg.type != lxLocal || index1Arg.argument != (ref_t)-2)
            return false;

         if (index2Arg.type != lxLocal || index2Arg.argument != (ref_t)-3)
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

         SNode ret = SyntaxTree::findPattern(root, 3,
            SNodePattern(lxReturning),
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

bool CompilerLogic :: recognizeEmbeddableGetAt(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject)
{
   return recognizeEmbeddableOp(scope, root, extensionRef,  returningType, READ_MESSAGE_ID, subject);
}

bool CompilerLogic :: recognizeEmbeddableEval(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject)
{
   return recognizeEmbeddableOp(scope, root, extensionRef, returningType, EVAL_MESSAGE_ID, subject);
}

bool CompilerLogic :: recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject)
{
   return recognizeEmbeddableOp2(scope, root, extensionRef, returningType, READ_MESSAGE_ID, subject);
}

bool CompilerLogic :: recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject)
{
   return recognizeEmbeddableOp2(scope, root, extensionRef, returningType, EVAL_MESSAGE_ID, subject);
}

bool CompilerLogic :: recognizeEmbeddableIdle(SNode methodNode, bool extensionOne)
{
   SNode object = SyntaxTree::findPattern(methodNode, 3,
      SNodePattern(lxNewFrame),
      SNodePattern(lxReturning),
      SNodePattern(extensionOne ? lxLocal : lxThisLocal));

   return extensionOne ? (object == lxLocal && object.argument == -1) : (object == lxThisLocal && object.argument == 1);
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

bool CompilerLogic :: optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node)
{
   SNode callNode = node.findSubNode(lxDirectCalling, lxSDirctCalling);
   SNode callTarget = callNode.findChild(lxCallTarget);

   ClassInfo info;
   defineClassInfo(scope, info, callTarget.argument);

   for (int i = 0; i < EMBEDDABLEOP_MAX; i++) {
      EmbeddableOp op = embeddableOps[i];
      ref_t subject = info.methodHints.get(Attribute(callNode.argument, op.attribute));
      // if it is possible to replace get&subject operation with eval&subject2:local
      if (subject != 0) {
         compiler.injectEmbeddableOp(node, callNode, subject, op.paramCount, op.verb);

         return true;
      }
   }

   return false;
}

bool CompilerLogic :: validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool assingingMode)
{
   SNode exprNode = node.findSubNodeMask(lxObjectMask);   

   if (targetRef == sourceRef || isCompatible(scope, targetRef, sourceRef)) {
      if (exprNode.type != lxLocalAddress || exprNode.type != lxFieldAddress) {
      }
      else node = lxExpression;
   }
   else if (sourceRef == V_NIL) {
      // NIL reference is never boxed
      node = lxExpression;
   }
   else if (isPrimitiveRef(sourceRef) && (isCompatible(scope, targetRef, resolvePrimitiveReference(scope, sourceRef)) || sourceRef == V_INT32)) {
      //HOTFIX : allowing numeric constant direct boxing
   }
   else if (node.existChild(lxBoxableAttr)) {
      // HOTFIX : if the object was explicitly boxed
   }
   else return false;

   bool localBoxing = false;
   bool variable = false;
   if (exprNode == lxFieldAddress && exprNode.argument > 0 && !assingingMode) {
      variable = !isReadonly(scope, targetRef);
      localBoxing = true;
   }
   else if (exprNode == lxFieldAddress && node.argument < 4 && node.argument > 0) {
      variable = !isReadonly(scope, targetRef) && !assingingMode;
      localBoxing = true;
   }
   else if (exprNode == lxExternalCall || exprNode == lxStdExternalCall) {
      // the result of external operation should be boxed locally, unboxing is not required (similar to assigning)
      localBoxing = true;
   }

   if (localBoxing) {
      variable = !isReadonly(scope, targetRef);

      compiler.injectLocalBoxing(exprNode, node.argument);

      node = variable ? lxLocalUnboxing : lxExpression;
   }

   return true;
}

//void CompilerLogic :: injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode)
//{
//   ClassInfo info;
//   defineClassInfo(scope, info, targetRef);
//
//   operand = defineStructSize(info, false);
//   
//   if (paramMode) {
//      if (operand == 0) {
//         //HOTFIX : allowing to assign a reference variable
//         // replace the parameter with the field expression
//         compiler.injectFieldExpression(writer);
//      }
//
//      type = info.fieldTypes.get(0).value2;
//      targetRef = info.fieldTypes.get(0).value1;
//   }
//}

bool CompilerLogic :: optimizeEmbeddable(SNode node, _CompilerScope& scope)
{
   // check if it is a virtual call
   if (node == lxDirectCalling && getVerb(node.argument) == GET_MESSAGE_ID && getParamCount(node.argument) == 0) {
      SNode callTarget = node.findChild(lxCallTarget);

      ClassInfo info;
      defineClassInfo(scope, info, callTarget.argument);
      if (info.methodHints.get(Attribute(node.argument, maEmbeddableIdle)) == -1) {
         // if it is an idle call, remove it
         node = lxExpression;

         return true;
      }
   }

   return false;
}

//inline bool seekDuplicateBoxing(SNode& current, SNode target)
//{
//   current = current.nextNode();
//
//   while (current != lxNone) {
//      if (current == lxBoxing) {
//         SNode duplicate = current.findSubNodeMask(lxObjectMask);
//         if (duplicate.type == target.type && duplicate.argument == (ref_t)target.type) {
//            return true;
//         }
//      }
//
//      current = current.nextNode();
//   }
//
//   return false;
//}

//void CompilerLogic :: optimizeDuplicateBoxing(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxBoxing) {
//         SNode target = current.findSubNodeMask(lxObjectMask);
//         SNode next = current;
//         while (seekDuplicateBoxing(next, target)) {
//
//         }
//      }
//   }
//}

// defineOperatorMessage tries to find the best match for the operator
ref_t CompilerLogic :: defineOperatorMessage(_CompilerScope& scope, ref_t operatorId, int paramCount, ref_t loperand, ref_t roperand, ref_t roperand2)
{
   ref_t foundSubjRef = 0;

   if (loperand != 0 && roperand != 0 && (paramCount == 1 || roperand2 != 0)) {
      ClassInfo info;
      defineClassInfo(scope, info, loperand);

      // search for appropriate methods
      ClassInfo::MethodMap::Iterator it = info.methods.start();
      while (!it.Eof()) {
         ref_t message = it.key();

         ref_t messageSubj = getSignature(message);
         if (getVerb(message) == operatorId && getParamCount(message) == paramCount && messageSubj != 0) {
            if (paramCount == 2) {
               // if the signature contains the two subjects
               ident_t signature(scope.module->resolveSubject(messageSubj));
               size_t index = signature.find('&');
               if (index != NOTFOUND_POS) {
                  IdentifierString subj1(signature, index);
                  IdentifierString subj2(signature.c_str() + index + 1);

                  ref_t subj1Ref = scope.module->mapSubject(subj1, true);
                  ref_t subj2Ref = scope.module->mapSubject(subj2, true);

                  if (isCompatible(scope, roperand, scope.subjectHints.get(subj1Ref)) && isCompatible(scope, roperand2, scope.subjectHints.get(subj2Ref))) {
                     foundSubjRef = messageSubj;

                     break;
                  }
               }
            }
            else if (isCompatible(scope, roperand, scope.subjectHints.get(messageSubj))) {
               foundSubjRef = messageSubj;

               break;
            }
         }

         it++;
      }
   }

   return encodeMessage(foundSubjRef, operatorId, paramCount);
}

bool CompilerLogic :: validateMessage(ref_t message, bool isClassClass)
{
   bool dispatchOne = getVerb(message) == DISPATCH_MESSAGE_ID;

   if (isClassClass && dispatchOne) {
      return false;
   }
   else if (!isClassClass && dispatchOne && (getSignature(message) != 0 || getParamCount(message) != 0)) {
      return false;
   }
   else return true;
}

//bool CompilerLogic :: isPrimitiveArray(ref_t reference)
//{
//   return isPrimitiveStructArrayRef(reference) | isPrimitiveArrayRef(reference);
//}
//
////bool CompilerLogic :: recognizeNewLocal(SNode& node)
////{
////   SNode firstToken = node.firstChild(lxObjectMask);
////   if (firstToken == lxIdentifier) {
////      firstToken = lxAttribute;
////
////      SNode identifier = firstToken.nextNode();
////      if (identifier == lxMessage) {
////         identifier = lxExpression;
////
////         return true;
////      }
////
////      //SNode token = identifier.findChild(lxIdentifier, lxPrivate);
////      //if (token != lxNone) {
////      //   identifier = token.type;
////
////      //   SyntaxTree::copyNode(token, identifier);
////      //   token = lxIdle;
////
////      //   return true;
////      //}
////   }
////   return false;
////}
////
////bool CompilerLogic :: recognizeNewField(SNode& node)
////{
////   SNode body = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
////   if (body == lxNone) {
////      if (setTokenIdentifier(node.lastChild())) {
////         node = lxClassField;
////
////         return true;
////      }
////   }
////   return false;
////}
////
////bool CompilerLogic :: recognizeNestedScope(SNode& node)
////{
////   SNode current = node.firstChild();
////   while (current != lxNone) { 
////      if (current == lxScope) {
////         SNode codeNode = current.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
////         if (codeNode != lxNone) {
////            if (setIdentifier(codeNode)) {
////               current = lxClassMethod;
////
////               // !! HOTFIX : the node should be once again found
////               codeNode = current.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
////
////               if (codeNode == lxExpression)
////                  codeNode = lxReturning;
////            }
////         }
////         else if (setTokenIdentifier(current.lastChild())) {
////            current = lxClassField;
////         }
////      }
////
////      current = current.nextNode();
////   }
////
////   return true;
////}
