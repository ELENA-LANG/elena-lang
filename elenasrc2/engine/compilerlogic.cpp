//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compilerlogic.h"
#include "errors.h"

using namespace _ELENA_;

typedef ClassInfo::Attribute Attribute;

inline bool isPrimitiveArrayRef(ref_t classRef)
{
   switch (classRef) {
      case V_OBJARRAY:
      case V_INT32ARRAY:
      case V_INT16ARRAY:
      case V_INT8ARRAY:
      case V_BINARYARRAY:
         return true;
      default:
         return false;
   }
}

inline bool IsInvertedOperator(int& operator_id)
{
   switch (operator_id)
   {
      case NOTEQUAL_OPERATOR_ID:
         operator_id = EQUAL_OPERATOR_ID;

         return true;
      case NOTLESS_OPERATOR_ID:
         operator_id = LESS_OPERATOR_ID;

         return true;
      case NOTGREATER_OPERATOR_ID:
         operator_id = GREATER_OPERATOR_ID;

         return true;
      default:
         return false;
   }
}

inline ident_t findSourceRef(SNode node)
{
   while (node != lxNone && node != lxNamespace) {
      if (node.compare(lxConstructor, lxStaticMethod, lxClassMethod) && node.existChild(lxSourcePath)) {
         return node.findChild(lxSourcePath).identifier();
      }

      node = node.parentNode();
   }

   return node.findChild(lxSourcePath).identifier();
}

// --- CompilerLogic Optimization Ops ---
struct EmbeddableOp
{
   int attribute;
   int paramCount;   // -1 indicates that operation should be done with the assigning target

   EmbeddableOp(int attr, int count)
   {
      this->attribute = attr;
      this->paramCount = count;
   }
};

constexpr auto EMBEDDABLEOP_MAX = /*5*/1;
EmbeddableOp embeddableOps[EMBEDDABLEOP_MAX] =
{
//   EmbeddableOp(maEmbeddableGetAt, 2/*, READ_MESSAGE_ID*/),
//   EmbeddableOp(maEmbeddableGetAt2, 3, READ_MESSAGE_ID),
//   EmbeddableOp(maEmbeddableEval, 2, EVAL_MESSAGE_ID),
//   EmbeddableOp(maEmbeddableEval2, 3, EVAL_MESSAGE_ID),
   EmbeddableOp(maEmbeddableNew, -1)
};

// --- CompilerLogic ---

CompilerLogic :: CompilerLogic()
{
   // nil
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_NIL, 0, lxNilOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_NIL, 0, lxNilOp, V_FLAG));
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, 0, V_NIL, lxNilOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, 0, V_NIL, lxNilOp, V_FLAG));

   // int32 primitive operations
   operators.add(OperatorInfo(ADD_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SUB_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(MUL_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(DIV_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(BAND_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(BOR_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(BXOR_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SHIFTR_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));
   operators.add(OperatorInfo(SHIFTL_OPERATOR_ID, V_INT32, V_INT32, lxIntOp, V_INT32));

   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(LESS_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_OPERATOR_ID, V_INT32, V_INT32, lxIntBoolOp, V_FLAG));

   operators.add(OperatorInfo(NEGATIVE_OPERATOR_ID, V_INT32, V_OBJECT, lxIntOp, V_INT32));
   operators.add(OperatorInfo(BINVERTED_OPERATOR_ID, V_INT32, V_OBJECT, lxIntOp, V_INT32));

   // subject primitive operations
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_SUBJECT, V_SUBJECT, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_SUBJECT, V_SUBJECT, lxIntBoolOp, V_FLAG));

   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_MESSAGE, V_MESSAGE, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_MESSAGE, V_MESSAGE, lxIntBoolOp, V_FLAG));

   // int64 primitive operations
   operators.add(OperatorInfo(ADD_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(SUB_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(MUL_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(DIV_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(BAND_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(BOR_OPERATOR_ID,    V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(BXOR_OPERATOR_ID,   V_INT64, V_INT64, lxLongOp, V_INT64));
   operators.add(OperatorInfo(SHIFTR_OPERATOR_ID,  V_INT64, V_INT32, lxLongOp, V_INT64));
   operators.add(OperatorInfo(SHIFTL_OPERATOR_ID, V_INT64, V_INT32, lxLongOp, V_INT64));

   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(LESS_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_OPERATOR_ID, V_INT64, V_INT64, lxLongBoolOp, V_FLAG));

   // real64 primitive operations
   operators.add(OperatorInfo(ADD_OPERATOR_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(SUB_OPERATOR_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(MUL_OPERATOR_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));
   operators.add(OperatorInfo(DIV_OPERATOR_ID, V_REAL64, V_REAL64, lxRealOp, V_REAL64));

   operators.add(OperatorInfo(ADD_OPERATOR_ID, V_REAL64, V_INT32, lxRealIntOp, V_REAL64));
   operators.add(OperatorInfo(SUB_OPERATOR_ID, V_REAL64, V_INT32, lxRealIntOp, V_REAL64));
   operators.add(OperatorInfo(MUL_OPERATOR_ID, V_REAL64, V_INT32, lxRealIntOp, V_REAL64));
   operators.add(OperatorInfo(DIV_OPERATOR_ID, V_REAL64, V_INT32, lxRealIntOp, V_REAL64));

   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));
   operators.add(OperatorInfo(LESS_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_OPERATOR_ID, V_REAL64, V_REAL64, lxRealBoolOp, V_FLAG));

   // array of int32 primitive operations
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_INT32ARRAY, V_INT32, lxIntArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_INT32ARRAY, V_INT32, V_INT32, lxIntArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_INT32ARRAY, lxIntArrOp, 0));

   // array of int16 primitive operations
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_INT16ARRAY, V_INT32, lxShortArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_INT16ARRAY, V_INT32, V_INT32, lxShortArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_INT16ARRAY, lxShortArrOp, 0));

   // array of int8 primitive operations
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_INT8ARRAY, V_INT32, lxByteArrOp, V_INT32));
   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_INT8ARRAY, V_INT32, V_INT32, lxByteArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_INT8ARRAY, lxByteArrOp, 0));

   // array of object primitive operations
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_OBJARRAY, V_INT32, lxArrOp, V_OBJECT));
   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_OBJARRAY, V_INT32, 0, lxArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_OBJARRAY, lxArrOp, 0));

   // array of structures primitive operations
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_BINARYARRAY, V_INT32, lxBinArrOp, V_BINARY));
   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_BINARYARRAY, V_INT32, 0, lxBinArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_BINARYARRAY, lxBinArrOp, 0));

   // array of arg list
   operators.add(OperatorInfo(REFER_OPERATOR_ID, V_ARGARRAY, V_INT32, lxArgArrOp, 0));
//   operators.add(OperatorInfo(SET_REFER_OPERATOR_ID, V_ARGARRAY, V_INT32, 0, lxArgArrOp, 0));
   operators.add(OperatorInfo(LEN_OPERATOR_ID, V_INT32, V_ARGARRAY, lxArgArrOp, 0));

//   //operators.add(OperatorInfo(READ_MESSAGE_ID, V_OBJARRAY, V_INT32, lxArrOp, 0));

   // boolean primitive operations
   operators.add(OperatorInfo(AND_OPERATOR_ID, V_FLAG, V_FLAG, 0, lxBoolOp, V_FLAG));
   operators.add(OperatorInfo(OR_OPERATOR_ID, V_FLAG, V_FLAG, 0, lxBoolOp, V_FLAG));
   operators.add(OperatorInfo(INVERTED_OPERATOR_ID, V_FLAG, V_OBJECT, 0, lxBoolOp, V_FLAG));

   // 32bit pointer primitive operations
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_PTR32, V_PTR32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_PTR32, V_PTR32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_PTR32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_PTR32, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(ADD_OPERATOR_ID, V_PTR32, V_INT32, lxIntOp, V_PTR32));
   operators.add(OperatorInfo(SUB_OPERATOR_ID, V_PTR32, V_INT32, lxIntOp, V_PTR32));

   // 64bit pointer primitive operations
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_PTR64, V_PTR64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_PTR64, V_PTR64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_PTR64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_PTR64, V_INT64, lxLongBoolOp, V_FLAG));
   operators.add(OperatorInfo(ADD_OPERATOR_ID, V_PTR64, V_INT32, lxLongOp, V_PTR64));
   operators.add(OperatorInfo(SUB_OPERATOR_ID, V_PTR64, V_INT32, lxLongOp, V_PTR64));

   // dword primitive operations
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(EQUAL_OPERATOR_ID, V_DWORD, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTEQUAL_OPERATOR_ID, V_DWORD, V_INT32, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(LESS_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTLESS_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(GREATER_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
   operators.add(OperatorInfo(NOTGREATER_OPERATOR_ID, V_DWORD, V_DWORD, lxIntBoolOp, V_FLAG));
}

int CompilerLogic :: checkMethod(ClassInfo& info, mssg_t message, ChechMethodInfo& result, bool resolveProtected)
{
   bool methodFound = info.methods.exist(message);

   if (methodFound) {
      int hint = info.methodHints.get(Attribute(message, maHint));
      if ((hint & tpMask) == tpPrivate) {
         // recognize the private message
         message |= STATIC_MESSAGE;

         hint = info.methodHints.get(Attribute(message, maHint));
      }

      result.outputReference = info.methodHints.get(Attribute(message, maReference));
      result.constRef = info.methodHints.get(Attribute(message, maConstant));

      result.embeddable = test(hint, tpEmbeddable);
//      result.function = test(hint, tpFunction);
//      result.dynamicRequired = test(hint, tpDynamic);

      if ((hint & tpMask) == tpSealed || (hint & tpMask) == tpPrivate) {
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
   else {
      //HOTFIX : to recognize the predefined messages
      result.outputReference = info.methodHints.get(Attribute(message, maReference));

      if (resolveProtected) {
         result.protectedRef = info.methodHints.get(Attribute(message, maProtected));
      }

      //HOTFIX : to recognize the sealed private method call
      //         hint search should be done even if the method is not declared
      return info.methodHints.get(Attribute(message, maHint));
   }
}

//ref_t CompilerLogic :: resolveArrayElement(_ModuleScope& scope, ref_t reference)
//{
//   ClassInfo info;
//   if (defineClassInfo(scope, info, reference)) {
//      return info.fieldTypes.get(-1).value2;
//   }
//   else return 0;
//}

int CompilerLogic :: checkMethod(_ModuleScope& scope, ref_t reference, mssg_t message,
   ChechMethodInfo& result, bool resolveProtected)
{
   ClassInfo info;
   result.found = reference && defineClassInfo(scope, info, reference);

   if (result.found) {
      if (testany(info.header.flags, elClosed | elClassClass))
         result.directResolved = true;

      if (testany(info.header.flags, elWithVariadics)) {
         result.withVariadicDispatcher = true;
      }
      else if (test(info.header.flags, elWithCustomDispatcher))
         result.withCustomDispatcher = true;

      int hint = checkMethod(info, message, result, resolveProtected);

      result.withEmbeddableRet = info.methodHints.get(Attribute(message, maEmbeddableRet)) != 0;

      return hint;
   }
   else return tpUnknown;
}

mssg_t CompilerLogic :: resolveSingleMultiDisp(_ModuleScope& scope, ref_t reference, mssg_t message)
{
   if (!reference)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, reference)) {
      return info.methodHints.get(Attribute(message, maSingleMultiDisp));
   }
   else return 0;
}

int CompilerLogic :: resolveCallType(_ModuleScope& scope, ref_t& classReference, mssg_t messageRef, ChechMethodInfo& result)
{
   int methodHint = checkMethod(scope, classReference != 0 ? classReference : scope.superReference, messageRef, result, false);
   int callType = methodHint & tpMask;

   result.stackSafe = test(methodHint, tpStackSafe);

//   if (test(messageRef, SPECIAL_MESSAGE)) {
//      // HOTFIX : calling closure
//      result.closure = true;
//   }

   return callType;
}

int CompilerLogic :: resolveOperationType(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result)
{
   if ((loperand == 0 && roperand != V_NIL) || (roperand == 0 && loperand != V_NIL))
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
         else if (info.roperand == V_NIL) {
            if (roperand == V_NIL) {
               result = info.result;

               return info.operationType;
            }
         }
         else if (info.loperand == V_FLAG && info.roperand == V_FLAG) {
            if (isBoolean(scope, loperand) && isBoolean(scope, roperand)) {
               result = info.result;

               return info.operationType;
            }
         }
         else if (info.loperand == V_FLAG && info.roperand == V_OBJECT) {
            if (isBoolean(scope, loperand) && roperand == V_OBJECT) {
               result = info.result;

               return info.operationType;
            }
         }
         else if (isCompatible(scope, info.loperand, loperand, false)
            && isCompatible(scope, info.roperand, roperand, false))
         {
            result = info.result;

            return info.operationType;
         }
      }

      it++;
   }

   return 0;
}

bool CompilerLogic :: resolveBranchOperation(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t& reference)
{
   if (!loperand)
      return false;

   if (!isCompatible(scope, scope.branchingInfo.reference, loperand, true)) {
      return false;
   }

   reference = operatorId == IF_OPERATOR_ID ? scope.branchingInfo.trueRef : scope.branchingInfo.falseRef;

   return true;
}

int CompilerLogic :: resolveNewOperationType(_ModuleScope& scope, ref_t loperand, ref_t roperand)
{
   if (isCompatible(scope, V_INT32, roperand, true)) {
      ClassInfo info;
      if (defineClassInfo(scope, info, loperand, true)) {
         return test(info.header.flags, elDynamicRole) ? lxNewArrOp : 0;
      }
   }

   return 0;
}

inline bool isPrimitiveCompatible(ref_t targetRef, ref_t sourceRef)
{
   switch (targetRef) {
      case V_PTR32:
         return sourceRef == V_INT32;
      case V_PTR64:
         return sourceRef == V_INT64;
      case V_DWORD:
         return sourceRef == V_INT32 || sourceRef == V_PTR32 || sourceRef == V_MESSAGE || sourceRef == V_SUBJECT;
      case V_QWORD:
         return sourceRef == V_INT64 || sourceRef == V_PTR64;
      default:
         return false;
   }
}

bool CompilerLogic :: isCompatible(_ModuleScope& scope, ref_t targetRef, ref_t sourceRef, bool ignoreNils)
{
   if ((!targetRef || targetRef == scope.superReference) && !isPrimitiveRef(sourceRef))
      return true;

   if (sourceRef == V_NIL) {
      // nil is compatible with a super class for the message dispatching
      // and with all types for all other cases
      if (!ignoreNils || targetRef == scope.superReference)
         return true;
   }

   if (isPrimitiveRef(targetRef) && isPrimitiveCompatible(targetRef, sourceRef))
      return true;

   // !! temporal for the debugging
   ClassInfo tmp;
   defineClassInfo(scope, tmp, targetRef);

   while (sourceRef != 0) {
      if (targetRef != sourceRef) {
         ClassInfo info;
         if (!defineClassInfo(scope, info, sourceRef))
            return false;

         if (test(info.header.flags, elTemplatebased) && !isPrimitiveRef(targetRef)) {
            // HOTFIX : resolve weak reference before checking compability
            targetRef = scope.resolveWeakTemplateReferenceID(targetRef);
            info.header.parentRef = scope.resolveWeakTemplateReferenceID(info.header.parentRef);
            if (targetRef == sourceRef) {
               return true;
            }
         }

         // if it is a structure wrapper
         if (isPrimitiveRef(targetRef) && test(info.header.flags, elWrapper)) {
            ClassInfo::FieldInfo inner = info.fieldTypes.get(0);
            if (isCompatible(scope, targetRef, inner.value1, ignoreNils))
               return true;
         }

         if (test(info.header.flags, elClassClass)) {
            // class class can be compatible only with itself and the super class
            sourceRef = scope.superReference;
         }
         else sourceRef = info.header.parentRef;
      }
      else return true;
   }

   return false;
}

bool CompilerLogic :: isEmbeddableArray(ClassInfo& info)
{
   return test(info.header.flags, elDynamicRole | elStructureRole | elWrapper);
}

bool CompilerLogic :: isVariable(_ModuleScope& scope, ref_t classReference)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, classReference))
      return false;

   return isVariable(info);
}

bool CompilerLogic :: isVariable(ClassInfo& info)
{
   return test(info.header.flags, elWrapper) && !test(info.header.flags, elReadOnlyRole);
}

//bool CompilerLogic :: isSealedOrClosed(ClassInfo& info)
//{
//   return test(info.header.flags, elSealed | elClosed);
//}
//
//bool CompilerLogic :: isArray(_ModuleScope& scope, ref_t classReference)
//{
//   ClassInfo info;
//   if (!defineClassInfo(scope, info, classReference))
//      return false;
//
//   return isArray(info);
//}
//
//bool CompilerLogic :: isArray (ClassInfo& info)
//{
//   return test(info.header.flags, elDynamicRole);
//}

bool CompilerLogic :: isValidType(_ModuleScope& scope, ref_t classReference, bool ignoreUndeclared, bool allowRole)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, classReference, true))
      return ignoreUndeclared;

   return isValidType(info, allowRole);
}

bool CompilerLogic :: doesClassExist(_ModuleScope& scope, ref_t targetRef)
{
   if (!targetRef)
      return false;

   ClassInfo info;
   return defineClassInfo(scope, info, targetRef, true);
}

bool CompilerLogic :: validateAutoType(_ModuleScope& scope, ref_t& reference)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, reference))
      return false;

   while (isRole(info)) {
      reference = info.header.parentRef;

      if (!defineClassInfo(scope, info, reference))
         return false;
   }

   return true;
}

bool CompilerLogic :: isValidType(ClassInfo& info, bool allowRole)
{
   return allowRole || !testany(info.header.flags, elRole);
}

bool CompilerLogic :: isEmbeddable(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole) && !test(info.header.flags, elDynamicRole);
}

bool CompilerLogic :: isStacksafeArg(ClassInfo& info)
{
   if (test(info.header.flags, elDynamicRole)) {
      return isEmbeddableArray(info);
   }
   else return isEmbeddable(info);
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
}

bool CompilerLogic :: isAbstract(ClassInfo& info)
{
   return test(info.header.flags, elAbstract);
}

bool CompilerLogic :: isMethodAbstract(ClassInfo& info, mssg_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpAbstract);
}

//bool CompilerLogic :: isMethodYieldable(ClassInfo& info, mssg_t message)
//{
//   return test(info.methodHints.get(Attribute(message, maHint)), tpYieldable);
//}

bool CompilerLogic :: isMethodEmbeddable(ClassInfo& info, mssg_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpEmbeddable);
}

bool CompilerLogic :: isMethodPrivate(ClassInfo& info, mssg_t message)
{
   return (info.methodHints.get(Attribute(message, maHint)) & tpMask) == tpPrivate;
}

bool CompilerLogic :: isMixinMethod(ClassInfo& info, mssg_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpMixin);
}

bool CompilerLogic :: isMethodGeneric(ClassInfo& info, mssg_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpGeneric);
}

bool CompilerLogic :: isMultiMethod(ClassInfo& info, mssg_t message)
{
   return test(info.methodHints.get(Attribute(message, maHint)), tpMultimethod);
}

bool CompilerLogic :: isMultiMethod(_ModuleScope& scope, ref_t reference, mssg_t message)
{
   ClassInfo info;
   defineClassInfo(scope, info, reference, true);

   return isMultiMethod(info, message);
}

inline ident_t resolveActionName(_Module* module, mssg_t message)
{
   ref_t signRef = 0;
   return module->resolveAction(getAction(message), signRef);
}

ref_t CompilerLogic :: generateOverloadList(_ModuleScope& scope, _Compiler& compiler, mssg_t message,
   ClassInfo::CategoryInfoMap& methodHints, void* param, ref_t(*resolve)(void*,ref_t), int flags)
{
   // create a new overload list
   ref_t listRef = scope.mapAnonymous(resolveActionName(scope.module, message));

   // sort the overloadlist
   int defaultOne[0x20];
   int* list = defaultOne;
   size_t capcity = 0x20;
   size_t len = 0;
   for (auto h_it = methodHints.start(); !h_it.Eof(); h_it++) {
      if (h_it.key().value2 == maMultimethod && *h_it == message) {
         if (len == capcity) {
            int* new_list = new int[capcity + 0x10];
            memmove(new_list, list, capcity * 4);
            list = new_list;
            capcity += 0x10;
         }

         mssg_t omsg = h_it.key().value1;
         list[len] = omsg;
         for (size_t i = 0; i < len; i++) {
            if (isSignatureCompatible(scope, omsg, list[i])) {
               memmove((void*)((size_t)list + (i + 1) * 4), (void*)((size_t)list + i * 4), (len - i) * 4);
               list[i] = omsg;
               break;
            }
         }
         len++;
      }
   }

   // fill the overloadlist
   for (size_t i = 0; i < len; i++) {
      ref_t classRef = resolve(param, list[i]);

      if (test(flags, elSealed) || test(message, STATIC_MESSAGE)) {
         compiler.generateSealedOverloadListMember(scope, listRef, list[i], classRef);
      }
      else if (test(flags, elClosed)) {
         compiler.generateClosedOverloadListMember(scope, listRef, list[i], classRef);
      }
      else compiler.generateOverloadListMember(scope, listRef, list[i]);
   }

   if (capcity > 0x20)
      delete[] list;

   return listRef;
}

ref_t paramFeedback(void* param, ref_t)
{
#if defined(__LP64__)
   size_t val = (size_t)val;

   return (ref_t)val;
#else
   return (ref_t)param;
#endif
}

void CompilerLogic :: injectOverloadList(_ModuleScope& scope, ClassInfo& info, _Compiler& compiler, ref_t classRef)
{
   for (auto it = info.methods.start(); !it.Eof(); it++) {
      if (*it && isMultiMethod(info, it.key())) {
         mssg_t message = it.key();

         // create a new overload list
         ref_t listRef = generateOverloadList(scope, compiler, message, info.methodHints, (void*)classRef,
            paramFeedback, info.header.flags);

         info.methodHints.exclude(Attribute(message, maOverloadlist));
         info.methodHints.add(Attribute(message, maOverloadlist), listRef);
         if (test(message, STATIC_MESSAGE)) {
            info.methodHints.exclude(Attribute(message & ~STATIC_MESSAGE, maOverloadlist));
            info.methodHints.add(Attribute(message & ~STATIC_MESSAGE, maOverloadlist), listRef);
         }
      }
   }
}

//inline int countFields(SNode node)
//{
//   int counter = 0;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxClassField) {
//         counter++;
//      }
//
//      current = current.nextNode();
//   }
//
//   return counter;
//}

void CompilerLogic :: injectVirtualFields(_ModuleScope&, SNode node, ref_t, ClassInfo& info, _Compiler& compiler)
{
   // generate yield fields
   if (test(info.header.flags, elWithYieldable)) {
      int index = info.fields.Count();

      //// HOTFIX : take into account newly declared fields as well
      //index += countFields(node);

      //SNode current = node.firstChild();
      //while (current != lxNone) {
      //   if (current == lxClassMethod && SyntaxTree::existChild(current, lxAttribute, tpYieldable)) {

      //      // NOTE : field initialization MUST be declared after the yield method declaration
      //      info.methodHints.add(ClassInfo::Attribute(current.argument, maYieldContext), index);
      //      compiler.injectVirtualField(node, lxYieldContext, current.argument, index);

      //      info.methodHints.add(ClassInfo::Attribute(current.argument, maYieldLocals), ++index);
      //      compiler.injectVirtualField(node, lxYieldLocals, current.argument, index);
      //   }

      //   current = current.nextNode();
      //}
   }

//   // generate enumeration list static field
//   if ((info.header.flags & elDebugMask) == elEnumList && !test(info.header.flags, elNestedClass)) {
//      compiler.injectVirtualStaticConstField(scope, node, ENUM_VAR, classRef);
//   }
}

void CompilerLogic :: injectVirtualCode(_ModuleScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler, bool closed)
{
   if (test(info.header.flags, elClassClass)) {
   }
   else if (!test(info.header.flags, elNestedClass) && !test(info.header.flags, elRole))
   {
      if (test(info.header.flags, elDynamicRole)) {
         // HOTFIX : remove auto generated default constructor for a dynamic object
         SNode methNode = SyntaxTree::goToChild(node, lxConstructor, scope.constructor_message);
         if (methNode != lxNone && methNode.existChild(lxAutogenerated))
            methNode = lxIdle;
      }

      // skip class classes, extensions and singletons
      if (classRef != scope.superReference && !closed) {
         // auto generate cast$<type> message for explicitly declared classes
         ref_t signRef = scope.module->mapSignature(&classRef, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         compiler.injectVirtualReturningMethod(scope, node, encodeMessage(actionRef, 1, CONVERSION_MESSAGE), SELF_VAR, classRef);
      }

      if (test(info.header.flags, elStructureRole)) {
         List<mssg_t> generatedConstructors;
         bool found = 0;
         SNode current = node.firstChild();
         while (current != lxNone) {
            if (current == lxConstructor) {
               SNode attr = current.firstChild();
               while (attr != lxNone) {
                  if (attr == lxAttribute) {
                     if (attr.argument == tpEmbeddable) {
                        generatedConstructors.add(current.argument);

                        current.set(lxClassMethod, current.argument | STATIC_MESSAGE);
                        attr.setArgument(tpPrivate | tpSealed);

                        found = true;
                        break;
                     }
                  }
                  else break;

                  attr = attr.nextNode();
               }
            }
            current = current.nextNode();
         }

         if (found) {
            for (auto it = generatedConstructors.start(); !it.Eof(); it++) {
               mssg_t message = *it;

               compiler.injectEmbeddableConstructor(node, message, message | STATIC_MESSAGE);
            }
         }
      }
   }
}

void CompilerLogic :: injectVirtualMultimethods(_ModuleScope& scope, SNode node, _Compiler& compiler,
   List<mssg_t>& implicitMultimethods, LexicalType methodType, ClassInfo& info)
{
   // generate implicit mutli methods
   for (auto it = implicitMultimethods.start(); !it.Eof(); it++) {
      compiler.injectVirtualMultimethod(scope, node, *it, methodType, info);
   }
}

bool isEmbeddableDispatcher(_ModuleScope& scope, SNode current)
{
   SNode attr = current.firstChild();
   bool embeddable = false;
   bool implicit = true;
   while (attr != lxNone) {
      if (attr == lxAttribute) {
         switch (attr.argument) {
            case V_EMBEDDABLE:
            {
               SNode dispatch = current.findChild(lxDispatchCode);

               embeddable = isSingleStatement(dispatch);
               break;
            }
            case V_METHOD:
            case V_CONSTRUCTOR:
            case V_DISPATCHER:
               implicit = false;
               break;
         }
      }
      else if (attr == lxNameAttr && embeddable && implicit) {
         if (scope.attributes.get(attr.firstChild(lxTerminalMask).identifier()) == V_DISPATCHER) {
            return true;
         }
         else break;
      }

      attr = attr.nextNode();
   }

   return false;
}

bool CompilerLogic :: isWithEmbeddableDispatcher(_ModuleScope& scope, SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod && current.existChild(lxDispatchCode)) {
         if (isEmbeddableDispatcher(scope, current)) {
            return true;
         }
      }
      current = current.nextNode();
   }

   return false;
}

void CompilerLogic :: injectInterfaceDispatch(_ModuleScope& scope, _Compiler& compiler, SNode node, ref_t parentRef)
{
   SNode current = node.firstChild();
   SNode dispatchMethodNode;
   SNode dispatchNode;
   while (current != lxNone) {
      if (current == lxClassMethod && current.existChild(lxDispatchCode)) {
         if (isEmbeddableDispatcher(scope, current)) {
            dispatchMethodNode = current;
            dispatchNode = current.findChild(lxDispatchCode).firstSubNodeMask();
         }

      }
      current = current.nextNode();
   }

   if (!isSingleStatement(dispatchNode))
      scope.raiseError(errInvalidSyntax, findSourceRef(node), dispatchNode);

   ClassInfo info;
   scope.loadClassInfo(info, parentRef);
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      if (it.key().value2 == maHint && test(*it, tpAbstract)) {
         compiler.injectVirtualDispatchMethod(scope, node, it.key().value1, dispatchNode.type, dispatchNode.identifier());
      }
   }

   dispatchMethodNode = lxIdle;
}

void CompilerLogic :: verifyMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, List<mssg_t>& implicitMultimethods)
{
   // HOTFIX : Make sure the multi-method methods have the same output type as generic one
   bool needVerification = false;
   for (auto it = implicitMultimethods.start(); !it.Eof(); it++) {
      mssg_t message = *it;

      ref_t outputRef = info.methodHints.get(Attribute(message, maReference));
      if (outputRef != 0) {
         // Bad luck we have to verify all overloaded methods
         needVerification = true;
         break;
      }
   }

   if (!needVerification)
      return;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         mssg_t multiMethod = info.methodHints.get(Attribute(current.argument, maMultimethod));
         if (multiMethod != lxNone) {
            ref_t outputRefMulti = info.methodHints.get(Attribute(multiMethod, maReference));
            if (outputRefMulti != 0) {
               ref_t outputRef = info.methodHints.get(Attribute(current.argument, maReference));
               if (outputRef == 0) {
                  scope.raiseError(errNotCompatibleMulti, findSourceRef(current), current.findChild(lxNameAttr).firstChild(lxTerminalMask));
               }
               else if (!isCompatible(scope, outputRefMulti, outputRef, true)) {
                  scope.raiseError(errNotCompatibleMulti, findSourceRef(current), current.findChild(lxNameAttr).firstChild(lxTerminalMask));
               }
            }
         }
      }
      current = current.nextNode();
   }
}

bool CompilerLogic :: isBoolean(_ModuleScope& scope, ref_t reference)
{
   return isCompatible(scope, scope.branchingInfo.reference, reference, true);
}

void CompilerLogic :: injectOperation(SNode& node, _CompileScope& scope, _Compiler& compiler, int operator_id,
   int operationType, ref_t& reference, ref_t elementRef, int tempLocal)
{
   int size = 0;
   if (operationType == lxBinArrOp) {
      // HOTFIX : define an item size for the binary array operations
      size = -defineStructSize(*scope.moduleScope, V_BINARYARRAY, elementRef);
   }

   if (reference == V_BINARY && elementRef != 0) {
      reference = elementRef;
   }
   else if (reference == V_OBJECT && elementRef != 0) {
      reference = elementRef;
   }

   bool inverting = IsInvertedOperator(operator_id);

   if (reference == V_FLAG) {
      if (!scope.moduleScope->branchingInfo.reference) {
         // HOTFIX : resolve boolean symbols
         ref_t dummy;
         resolveBranchOperation(*scope.moduleScope, IF_OPERATOR_ID, scope.moduleScope->branchingInfo.reference, dummy);
      }

      reference = scope.moduleScope->branchingInfo.reference;
      if (inverting) {
         node.appendNode(lxIfValue, scope.moduleScope->branchingInfo.falseRef);
         node.appendNode(lxElseValue, scope.moduleScope->branchingInfo.trueRef);
      }
      else {
         node.appendNode(lxIfValue, scope.moduleScope->branchingInfo.trueRef);
         node.appendNode(lxElseValue, scope.moduleScope->branchingInfo.falseRef);
      }
   }

   if (size != 0) {
      // HOTFIX : inject an item size for the binary array operations
      node.appendNode(lxSize, size);
   }

   if (IsExprOperator(operator_id) && operationType != lxBoolOp) {
      size = defineStructSize(*scope.moduleScope, reference, elementRef);

      compiler.injectExprOperation(node, size, tempLocal, (LexicalType)operationType, operator_id);
   }
   else node.set((LexicalType)operationType, operator_id);
}

bool CompilerLogic :: isReadonly(ClassInfo& info)
{
   return test(info.header.flags, elReadOnlyRole);
}

inline ref_t getSignature(_ModuleScope& scope, mssg_t message)
{
   ref_t actionRef = getAction(message);
   ref_t signRef = 0;
   scope.module->resolveAction(actionRef, signRef);

   return signRef;
}

bool CompilerLogic :: isSignatureCompatible(_ModuleScope& scope, mssg_t targetMessage, mssg_t sourceMessage)
{
   ref_t sourceSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(getSignature(scope, sourceMessage), sourceSignatures);

   return isSignatureCompatible(scope, getSignature(scope, targetMessage), sourceSignatures, len);
}

bool CompilerLogic :: isSignatureCompatible(_ModuleScope& scope, ref_t targetSignature, ref_t* sourceSignatures, size_t sourceLen)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(targetSignature, targetSignatures);
   if (sourceLen == 0 && len == 0)
      return true;

   if (len < 1)
      return false;

   for (size_t i = 0; i < sourceLen; i++) {
      ref_t targetSign = i < len ? targetSignatures[i] : targetSignatures[len - 1];
      if (!isCompatible(scope, targetSign, sourceSignatures[i], true))
         return false;
   }

   return true;
}

bool CompilerLogic :: isSignatureCompatible(_ModuleScope& scope, _Module* targetModule, ref_t targetSignature,
   ref_t* sourceSignatures, size_t sourceLen)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = targetModule->resolveSignature(targetSignature, targetSignatures);

   if (sourceLen == 0 && len == 0)
      return true;

   if (len < 1)
      return false;

   for (size_t i = 0; i < sourceLen; i++) {
      ref_t targetSign = i < len ? targetSignatures[i] : targetSignatures[len - 1];

      if (!isCompatible(scope, importReference(targetModule, targetSign, scope.module), sourceSignatures[i], true))
         return false;
   }

   return true;
}

int CompilerLogic :: defineStackSafeAttrs(_ModuleScope& scope, mssg_t message)
{
   ref_t signRef = getSignature(scope, message);
   int stackSafeAttr = 0;
   setSignatureStacksafe(scope, signRef, stackSafeAttr);

   return stackSafeAttr;
}

bool CompilerLogic :: isMessageCompatibleWithSignature(_ModuleScope& scope, ref_t targetRef, mssg_t targetMessage,
   ref_t* sourceSignatures, size_t len, int& stackSafeAttr)
{
   ref_t targetSignRef = getSignature(scope, targetMessage);

   if (isSignatureCompatible(scope, targetSignRef, sourceSignatures, len)) {
      if (isStacksafeArg(scope, targetRef))
         stackSafeAttr |= 1;

      setSignatureStacksafe(scope, targetSignRef, stackSafeAttr);

      return true;
   }
   else return false;
}

void CompilerLogic :: setSignatureStacksafe(_ModuleScope& scope, ref_t targetSignature, int& stackSafeAttr)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(targetSignature, targetSignatures);
   if (len <= 0)
      return;

   int flag = 1;
   for (size_t i = 0; i < len; i++) {
      flag <<= 1;

      if (isStacksafeArg(scope, targetSignatures[i]))
         stackSafeAttr |= flag;
   }
}

void CompilerLogic :: setSignatureStacksafe(_ModuleScope& scope, _Module* targetModule, ref_t targetSignature, int& stackSafeAttr)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = targetModule->resolveSignature(targetSignature, targetSignatures);
   if (len <= 0)
      return;

   int flag = 1;
   for (size_t i = 0; i < len; i++) {
      flag <<= 1;

      if (isStacksafeArg(scope, importReference(targetModule, targetSignatures[i], scope.module)))
         stackSafeAttr |= flag;
   }
}

bool CompilerLogic :: isMethodEmbeddable(_ModuleScope& scope, ref_t reference, mssg_t message)
{
   ClassInfo info;
   if (defineClassInfo(scope, info, reference)) {
      return isMethodEmbeddable(info, message);
   }
   else return false;
}

ConversionInfo CompilerLogic :: injectImplicitConstructor(_ModuleScope& scope, ClassInfo& info,
   ref_t targetRef, ref_t* signatures, size_t signLen)
{
   ref_t signRef = scope.module->mapSignature(signatures, signLen, false);

   int stackSafeAttr = 0;
   mssg_t messageRef = resolveImplicitConstructor(scope, targetRef, signRef, signLen, stackSafeAttr, true);
   if (messageRef) {
      bool embeddableAttr = isMethodEmbeddable(scope, info.header.classRef, messageRef);

      return ConversionInfo(ConversionResult::crConverted, messageRef, info.header.classRef, stackSafeAttr, embeddableAttr);
   }
   else return ConversionInfo();
}

//bool CompilerLogic :: injectConstantConstructor(SNode& node, _ModuleScope& scope, _Compiler& compiler, ref_t targetRef, mssg_t messageRef)
//{
//   int stackSafeAttr = 0;
//   setSignatureStacksafe(scope, scope.literalReference, stackSafeAttr);
//
//   compiler.injectConverting(node, lxDirectCalling, messageRef, lxClassSymbol, targetRef, getClassClassRef(scope, targetRef), stackSafeAttr, false);
//
//   return true;
//}

ref_t CompilerLogic :: getClassClassRef(_ModuleScope& scope, ref_t targetRef)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef, true))
      return 0;

   return info.header.classRef;
}

mssg_t CompilerLogic :: resolveImplicitConstructor(_ModuleScope& scope, ref_t targetRef, ref_t signRef, size_t signLen, int& stackSafeAttr, bool ignoreMultimethod)
{
   ref_t classClassRef = getClassClassRef(scope, targetRef);
   ref_t actionRef = scope.module->mapAction(CONSTRUCTOR_MESSAGE, 0, false);
   mssg_t messageRef = encodeMessage(actionRef, signLen + 1, 0);
   if (signRef != 0) {
      // try to resolve implicit multi-method
      mssg_t resolvedMessage = resolveMultimethod(scope, messageRef, classClassRef, signRef, stackSafeAttr, true);
      if (resolvedMessage)
         return resolvedMessage;
   }

   ClassInfo classClassinfo;
   if (!defineClassInfo(scope, classClassinfo, classClassRef))
      return 0;

   if (classClassinfo.methods.exist(messageRef)) {
      if (ignoreMultimethod) {
         int hints = classClassinfo.methodHints.get(Attribute(messageRef, maHint));
         if (test(hints, tpMultimethod))
            return 0;
      }

      return messageRef;
   }
   else if (classClassinfo.methods.exist(encodeMessage(actionRef, 1, VARIADIC_MESSAGE))) {
      // if exists an inplicit message with variadic argument list
      return encodeMessage(actionRef, 1, VARIADIC_MESSAGE);
   }

   return 0;
}

ConversionInfo CompilerLogic :: injectImplicitConversion(_CompileScope& scope, _Compiler& compiler, ref_t targetRef,
   ref_t sourceRef, ref_t elementRef/*, bool noUnboxing, int fixedSize*/)
{
   ClassInfo info;
   if (!defineClassInfo(*scope.moduleScope, info, targetRef))
      return ConversionInfo(ConversionResult::crUncompatible);

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper) && !test(info.header.flags, elDynamicRole)) {
      ClassInfo::FieldInfo inner = info.fieldTypes.get(0);

      bool compatible = false;
      if (test(info.header.flags, elStructureWrapper)) {
         if (isPrimitiveRef(sourceRef)) {
            compatible = isCompatible(*scope.moduleScope, inner.value1, sourceRef, true);
         }
         // HOTFIX : the size should be taken into account as well (e.g. byte and int both V_INT32)
         else compatible = isCompatible(*scope.moduleScope, inner.value1, sourceRef, true)
            && info.size == defineStructSize(*scope.moduleScope, sourceRef, 0u);
      }
      else compatible = isCompatible(*scope.moduleScope, inner.value1, sourceRef, true);

      if (compatible) {
         return ConversionInfo(ConversionResult::crBoxingRequired);
      }
   }

   // COMPILE MAGIC : trying to typecast primitive array
   if (isPrimitiveArrayRef(sourceRef) && test(info.header.flags, elDynamicRole)) {
//      ref_t boxingArg = isEmbeddable(scope, elementRef) ? - 1 : 0;

      if (isCompatible(*scope.moduleScope, info.fieldTypes.get(-1).value2, elementRef, true)) {
         //int size = info.size;
         //// if we boxing fixed-sized array field into an array
         //if (size < 0 && fixedSize > 0)
         //   size = fixedSize;

         return ConversionInfo(ConversionResult::crBoxingRequired);
      }
   }

   // HOTFIX : recognize primitive data except of a constant literal
   if (isPrimitiveRef(sourceRef)/* && sourceRef != V_STRCONSTANT*/)
      sourceRef = compiler.resolvePrimitiveReference(scope, sourceRef, elementRef, false);

   return injectImplicitConstructor(*scope.moduleScope, info, targetRef, /*elementRef, */&sourceRef, 1);
}

//void CompilerLogic :: injectNewOperation(SNode& node, _ModuleScope& scope, int operation, ref_t targetRef, ref_t elementRef)
//{
//   node.refresh();
//   if (node != lxExpression)
//      node.injectAndReplaceNode(lxExpression);
//
//   int size = defineStructSize(scope, targetRef, elementRef);
//   if (size != 0)
//      node.appendNode(lxSize, size);
//
//   node.set((LexicalType)operation, targetRef);
//}

bool CompilerLogic :: defineClassInfo(_ModuleScope& scope, ClassInfo& info, ref_t reference, bool headerOnly)
{
   if (isPrimitiveRef(reference) && !headerOnly) {
      scope.loadClassInfo(info, scope.superReference);
   }

   switch (reference)
   {
      case V_INT32:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugDWORD | elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_INT64:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugQWORD | elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_REAL64:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugReal64 | elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_PTR32:
         info.header.parentRef = scope.superReference;
         info.header.flags = elStructureRole;
         info.size = 4;
         break;
      case V_PTR64:
         info.header.parentRef = scope.superReference;
         info.header.flags = elStructureRole;
         info.size = 8;
         break;
      case V_DWORD:
         info.header.parentRef = scope.superReference;
         info.header.flags = elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_QWORD:
         info.header.parentRef = scope.superReference;
         info.header.flags = elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_SUBJECT:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugSubject | elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_MESSAGE:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugMessage | elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_EXTMESSAGE:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugMessage | elStructureRole | elReadOnlyRole;
         info.size = 8;
         break;
      case V_SYMBOL:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugReference | elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_INT32ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugIntegers | elStructureRole | elDynamicRole | elWrapper;
         info.size = -4;
         break;
      case V_INT16ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugShorts | elStructureRole | elDynamicRole | elWrapper;
         info.size = -2;
         break;
      case V_INT8ARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugBytes | elStructureRole | elDynamicRole | elWrapper;
         info.size = -1;
         break;
      case V_OBJARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDebugArray | elDynamicRole;
         info.size = 0;
         break;
      case V_BINARYARRAY:
         info.header.parentRef = scope.superReference;
         info.header.flags = elDynamicRole | elStructureRole | elWrapper;
         info.size = -1;
         break;
      case V_AUTO:
         break;
      default:
         if (reference != 0) {
            if (!scope.loadClassInfo(info, reference, headerOnly))
               return false;
         }
         else {
            info.header.parentRef = 0;
            info.header.flags = 0;
            //info.size = 0;
         }
         break;
   }

   return true;
}

int CompilerLogic :: defineStructSizeVariable(_ModuleScope& scope, ref_t reference, ref_t elementRef, bool& variable)
{
   if (reference == V_BINARYARRAY && elementRef != 0) {
      variable = true;

      return -defineStructSizeVariable(scope, elementRef, 0, variable);
   }
   else if (reference == V_WRAPPER && elementRef != 0) {
      return defineStructSizeVariable(scope, elementRef, 0, variable);
   }
   else if (reference == V_OBJARRAY && elementRef != 0) {
      return -defineStructSizeVariable(scope, elementRef, 0, variable);
   }
   else if (reference == V_BINARY && elementRef != 0) {
      return defineStructSizeVariable(scope, elementRef, 0, variable);
   }
   else if (reference == V_INT32ARRAY) {
      variable = true;

      return -4;
   }
   else if (reference == V_INT16ARRAY) {
      variable = true;

      return -2;
   }
   else if (reference == V_INT8ARRAY) {
      variable = true;

      return -1;
   }
   else {
      auto sizeInfo = scope.cachedSizes.get(reference);
      if (!sizeInfo.value1) {
         ClassInfo classInfo;
         if (defineClassInfo(scope, classInfo, reference)) {
            sizeInfo.value1 = defineStructSize(classInfo, variable);
            sizeInfo.value2 = variable;

            scope.cachedSizes.add(reference, sizeInfo);

            return sizeInfo.value1;
         }
         else return 0;
      }
      else {
         variable = sizeInfo.value2;

         return sizeInfo.value1;
      }
   }
}

int CompilerLogic :: defineStructSize(ClassInfo& info, bool& variable)
{
   variable = !test(info.header.flags, elReadOnlyRole);

   if (isEmbeddable(info)) {
      return info.size;
   }
   else if (isEmbeddableArray(info)) {
      return info.size;
   }

   return 0;
}

void CompilerLogic :: tweakClassFlags(_ModuleScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode)
{
   if (classClassMode) {
      // class class is always stateless and final
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

   if (test(info.header.flags, elDynamicRole | elStructureRole)) {
      if (classRef == scope.literalReference) {
         // recognize string constant
         if (info.size == -1) {
            info.header.flags |= elDebugLiteral;
         }
      }
      else if (classRef == scope.wideReference) {
         // recognize wide string constant
         if (info.size == -2) {
            info.header.flags |= elDebugWideLiteral;
         }
      }
   }

   // adjust array
   if (test(info.header.flags, elDynamicRole) && !test(info.header.flags, elStructureRole)/* | elNonStructureRole*/) {
      info.header.flags |= elNonStructureRole;

      if ((info.header.flags & elDebugMask) == 0) {
         info.header.flags |= elDebugArray;
      }
   }

   // adjust binary array
   if (test(info.header.flags, elDynamicRole | elStructureRole)) {
      if ((info.header.flags & elDebugMask) == 0) {
         ref_t itemRef = info.fieldTypes.get(-1).value1;
         switch (itemRef) {
            case V_INT32ARRAY:
               info.header.flags |= elDebugIntegers;
               break;
            case V_INT16ARRAY:
               info.header.flags |= elDebugShorts;
               break;
            case V_INT8ARRAY:
               info.header.flags |= elDebugBytes;
               break;
            default:
               info.header.flags |= elDebugBytes;
               break;
         }
      }
   }

   // adjust objects with custom dispatch handler
   if (info.methods.exist(scope.dispatch_message, true) && classRef != scope.superReference) {
      info.header.flags |= elWithCustomDispatcher;
   }

   // generate operation list if required
   injectOverloadList(scope, info, compiler, classRef);
}

bool CompilerLogic :: validateArgumentAttribute(int attrValue, bool& byRefArg, bool& paramsArg)
{
   switch ((size_t)attrValue) {
      case V_WRAPPER:
         if (!byRefArg) {
            byRefArg = true;
            return true;
         }
         else return false;
      case V_ARGARRAY:
         if (!paramsArg) {
            paramsArg = true;
            return true;
         }
         else return false;
      case V_VARIABLE:
         return true;
   }
   return false;
}

bool CompilerLogic :: validateNsAttribute(int attrValue, Visibility& visibility)
{
   switch ((size_t)attrValue)
   {
      case 0:
         return true;
      case V_PUBLIC:
         visibility = Visibility::Public;
         return true;
      case V_INTERNAL:
         visibility = Visibility::Internal;
         return true;
      case V_PRIVATE:
         visibility = Visibility::Private;
         return true;
      case V_NAMESPACE:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateClassAttribute(int& attrValue, Visibility& visibility)
{
   switch ((size_t)attrValue)
   {
      case 0:
         return true;
      case V_SEALED:
         attrValue = elSealed;
         return true;
      case V_ABSTRACT:
         attrValue = elAbstract;
         return true;
      case V_LIMITED:
         attrValue = (elClosed | elAbstract | elNoCustomDispatcher);
         return true;
      case V_TEMPLATEBASED:
         attrValue = elTemplatebased;
         return true;
      case V_CLOSED:
         attrValue = elClosed;
         return true;
      case V_STRUCT:
         attrValue = elStructureRole;
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
      case V_MIXIN:
         attrValue = elGroup;
         return true;
      case V_PUBLIC:
         attrValue = 0;
         visibility = Visibility::Public;
         return true;
      case V_INTERNAL:
         attrValue = 0;
         visibility = Visibility::Internal;
         return true;
      case V_PRIVATE:
         attrValue = 0;
         visibility = Visibility::Private;
         return true;
      case V_CLASS:
         attrValue = 0;
         return true;
      case V_SINGLETON:
         attrValue = elRole | elSealed | elStateless/* | elNestedClass*/;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateImplicitMethodAttribute(int& attrValue, bool complexName)
{
   bool dummy = false;
   switch ((size_t)attrValue)
   {
      case V_METHOD:
      case V_CONSTRUCTOR:
      case V_DISPATCHER:
      case V_CONVERSION:
      case V_GENERIC:
      case V_FUNCTION:
         return validateMethodAttribute(attrValue, dummy);
      case V_GETACCESSOR:
      case V_SETACCESSOR:
         if (complexName) {
            return validateMethodAttribute(attrValue, dummy);
         }
         else return false;
      default:
         return false;
   }
}

bool CompilerLogic :: validateMethodAttribute(int& attrValue, bool& explicitMode)
{
   switch ((size_t)attrValue)
   {
      case V_EMBEDDABLE:
         attrValue = tpEmbeddable;
         return true;
      case V_GENERIC:
         attrValue = (tpGeneric | tpSealed);
         return true;
      case V_MIXIN:
         attrValue = tpMixin;
         return true;
      case V_PRIVATE:
         attrValue = (tpPrivate | tpSealed);
         return true;
      case V_PUBLIC:
         attrValue = 0;
         return true;
      case V_INTERNAL:
         attrValue = tpInternal;
         return true;
      case V_PROTECTED:
         attrValue = tpProtected;
         return true;
      case V_SEALED:
         attrValue = tpSealed;
         return true;
      case V_FUNCTION:
         attrValue = tpFunction;
         explicitMode = true;
         return true;
      case V_CONSTRUCTOR:
         attrValue = tpConstructor | tpInitializer;
         explicitMode = true;
         return true;
      case V_CONVERSION:
         attrValue = tpConversion;
         return true;
      case V_INITIALIZER:
         attrValue = (tpFunction | tpPrivate | tpInitializer);
         return true;
      case V_METHOD:
         attrValue = 0;
         explicitMode = true;
         return true;
      case V_STATIC:
         attrValue = tpStatic;
         return true;
      case V_ABSTRACT:
         attrValue = tpAbstract;
         return true;
      case V_PREDEFINED:
         attrValue = tpPredefined;
         return true;
      case V_DISPATCHER:
         attrValue = tpDispatcher;
         explicitMode = true;
         return true;
      case V_GETACCESSOR:
         attrValue = tpGetAccessor;
         return true;
      case V_SETACCESSOR:
         attrValue = tpSetAccessor;
         return true;
      case V_SCRIPTSELFMODE:
         attrValue = tpTargetSelf;
         return true;
      case V_YIELDABLE:
         attrValue = tpYieldable;
         return true;
      case V_CONST:
         attrValue = tpConstant;
         return true;
      case V_MULTIRETVAL:
         attrValue = tpMultiRetVal | tpEmbeddable;
         return true;
      case 0:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateFieldAttribute(int& attrValue, FieldAttributes& attrs)
{
   switch ((size_t)attrValue)
   {
      case V_EMBEDDABLE:
         attrs.isEmbeddable = true;
         attrValue = -1;
         return true;
      case V_STATIC:
         attrValue = lxStaticAttr;
         return true;
//      case V_SEALED:
//         attrValue = -1;
//         attrs.isSealedAttr = true;
//         return true;
      case V_CONST:
         attrValue = -1;
         attrs.isConstAttr = true;
         return true;
//      case V_ATTRIBUTE:
//         attrValue = -1;
//         attrs.isClassAttr = true;
//         attrs.isConstAttr = true;
//         return true;
      case V_FLOAT:
      case V_BINARY:
      case V_INTBINARY:
      case V_PTRBINARY:
      case V_STRING:
      case V_MESSAGE:
      case V_SUBJECT:
      case V_EXTMESSAGE:
      case V_SYMBOL:
         attrValue = 0;
         return true;
//      case 0:
//         // ignore idle attribute
      case V_FIELD:
         attrValue = -1;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attributes, bool& newVariable)
{
   switch (attrValue) {
      case 0:
         // HOTFIX : recognize idle attributes
         return true;
//      //case V_AUTOSIZE:
//      //   attributes.include(EAttr::eaAutoSize);
//      //   return true;
      case V_VARIABLE:
      case V_AUTO:
         newVariable = true;
         return true;
      case V_CONVERSION:
         attributes.include(EAttr::eaCast);
         return true;
      case V_NEWOP:
         attributes.include(EAttr::eaNewOp);
         return true;
      case V_FORWARD:
         attributes.include(EAttr::eaForward);
         return true;
      case V_EXTERN:
         attributes.include(EAttr::eaExtern);
         return true;
      case V_WRAPPER:
         attributes.include(EAttr::eaRef);
         return true;
     case V_ARGARRAY:
         attributes.include(EAttr::eaParams);
   		return true;
     case V_INTERN:
         attributes.include(EAttr::eaIntern);
         return true;
      case V_LOOP:
         attributes.include(EAttr::eaLoop);
         return true;
      case V_MEMBER:
         attributes.include(EAttr::eaMember);
         return true;
      case V_META:
         attributes.include(EAttr::eaMetaField);
         return true;
      case V_SUBJECT:
         attributes.include(EAttr::eaSubj);
         return true;
      case V_MESSAGE:
         attributes.include(EAttr::eaMssg);
         return true;
      case V_CLASS:
         attributes.include(EAttr::eaClass);
         return true;
      case V_WEAKOP:
         attributes.include(EAttr::eaDirectCall);
         return true;
//      case V_LAZY:
//         attributes.include(EAttr::eaLazy);
//         return true;
//      case V_INLINEARG:
//         attributes.include(EAttr::eaInlineArg);
//         return true;
      case V_TYPEOF:
         attributes.include(EAttr::eaTypeOfOp);
         return true;
      case V_IGNOREDUPLICATE:
         attributes.include(EAttr::eaIgnoreDuplicates);
         return true;
//      case V_YIELD:
//         attributes.include(EAttr::eaYieldExpr);
//         return true;
      case V_NODEBUGINFO:
         attributes.include(EAttr::eaNoDebugInfo);
         return true;
//      case V_PREVIOUS:
//         attributes.include(EAttr::eaPreviousScope);
//         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne,
   Visibility& visibility)
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
   else if (attrValue == (int)V_PUBLIC) {
      visibility = Visibility::Public;

      return true;
   }
   else if (attrValue == (int)V_PRIVATE) {
      visibility = Visibility::Private;

      return true;
   }
   else if (attrValue == (int)V_INTERNAL) {
      visibility = Visibility::Internal;

      return true;
   }
   else return attrValue == 0;
}

void CompilerLogic :: tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info)
{
   // if it is a primitive field
   if (info.fields.Count() == 1) {
      switch (classRef) {
         case V_DWORD:
         case V_INT32:
            info.header.flags |= elDebugDWORD;
            break;
         case V_PTR32:
         case V_PTR64:
            info.header.flags |= elDebugPTR;
            break;
         case V_INT64:
            info.header.flags |= elDebugQWORD;
            break;
         case V_REAL64:
            info.header.flags |= elDebugReal64;
            break;
////         case V_PTR:
////            info.header.flags |= (elDebugPTR | elWrapper);
////            info.fieldTypes.add(0, ClassInfo::FieldInfo(V_PTR, 0));
////            return info.size == 4;
         case V_SUBJECT:
            info.header.flags |= elDebugSubject | elSubject;
            break;
         case V_MESSAGE:
            info.header.flags |= (elDebugMessage | elMessage);
            break;
         case V_EXTMESSAGE:
            info.header.flags |= (elDebugMessage | elExtMessage);
            break;
         case V_SYMBOL:
            info.header.flags |= (elDebugReference | elSymbol);
            break;
         default:
            break;
      }
   }
}

ref_t CompilerLogic :: definePrimitiveArray(_ModuleScope& scope, ref_t elementRef, bool structOne)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, elementRef, true))
      return 0;

   if (isEmbeddable(info) && structOne) {
      if (isCompatible(scope, V_INT32, elementRef, true)) {
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
}

void CompilerLogic :: validateClassDeclaration(_ModuleScope& scope, ClassInfo& info, bool& withAbstractMethods,
   bool& disptacherNotAllowed, bool& emptyStructure)
{
   if (!isAbstract(info)) {
      for (auto it = info.methodHints.start(); !it.Eof(); it++) {
         auto key = it.key();
         if (key.value2 == maHint && test(*it, tpAbstract)) {
            scope.printMessageInfo(infoAbstractMetod, key.value1);

            withAbstractMethods = true;
         }
      }
   }

   // interface class cannot have a custom dispatcher method
   if (test(info.header.flags, elNoCustomDispatcher) && info.methods.exist(scope.dispatch_message, true))
      disptacherNotAllowed = true;

   // a structure class should contain fields
   if (test(info.header.flags, elStructureRole) && info.size == 0)
      emptyStructure = true;
}

bool CompilerLogic :: recognizeEmbeddableIdle(SNode methodNode, bool extensionOne)
{
   SNode frameNode = methodNode.findChild(lxNewFrame);
   SNode firstExpr = frameNode.firstChild(lxObjectMask);
   if (firstExpr == lxSeqExpression && firstExpr.firstChild() == lxNone)
      firstExpr = firstExpr.nextNode(lxObjectMask);

   if (firstExpr == lxReturning) {
      SNode retNode = firstExpr.findSubNodeMask(lxObjectMask);

      return extensionOne ? (retNode == lxLocal && retNode.argument == -1) : (retNode == lxSelfLocal && retNode.argument == 1);
   }
   else return false;
}

bool CompilerLogic :: recognizeEmbeddableMessageCall(SNode methodNode, mssg_t& messageRef)
{
   SNode attr = methodNode.findChild(lxEmbeddableMssg);
   if (attr != lxNone) {
      messageRef = attr.argument;

      return true;
   }
   else return false;
}

//inline bool isMethodTree(SNode node)
//{
//   while (!node.compare(lxClassMethod, lxNone))
//      node = node.parentNode();
//
//   return (node != lxNone);
//}

mssg_t CompilerLogic :: resolveEmbeddableRetMessage(_CompileScope& scope, _Compiler& compiler, ref_t target,
   mssg_t message, ref_t expectedRef)
{
   ClassInfo info;
   if (!defineClassInfo(*scope.moduleScope, info, target))
      return 0;

   // take the main embeddable method
   Attribute key(message, maEmbeddableRet);
   mssg_t byRefMessageRef = info.methodHints.get(key);
   if (!byRefMessageRef)
      return 0;

   ref_t signatures[ARG_COUNT];
   size_t len = scope.moduleScope->module->resolveSignature(getSignature(*scope.moduleScope, byRefMessageRef), signatures);

   ref_t byRefArg = compiler.resolvePrimitiveReference(scope, V_WRAPPER, expectedRef, false);

   if (signatures[len - 1] == byRefArg)
      return byRefMessageRef;

   // COMPILER MAGIC : try to find alternative-returning method
   signatures[len - 1] = byRefArg;

   ref_t signRef = scope.moduleScope->module->mapSignature(signatures, len, true);
   if (!signRef)
      return 0;

   ref_t dummy = 0, actionRef = 0, flags = 0;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);
   ident_t name = scope.moduleScope->module->resolveAction(actionRef, dummy);

   byRefMessageRef = encodeMessage(scope.moduleScope->module->mapAction(name, signRef, true),
      argCount + 1, flags);

   if (info.methods.exist(byRefMessageRef))
      return byRefMessageRef;

   return 0;
}

////bool CompilerLogic :: optimizeReturningStructure(_ModuleScope& scope, _Compiler& compiler, SNode node, bool argMode)
////{
////   // validate if it is a method
////   if (!isMethodTree(node))
////      return false;
////
////   SNode callNode = node.findSubNode(lxDirectCalling, lxSDirectCalling, lxCalling_0);
////   if (callNode == lxNone)
////      return false;
////
////   SNode callTarget = callNode.findChild(lxCallTarget);
////
////   ClassInfo info;
////   if (!defineClassInfo(scope, info, callTarget.argument))
////      return false;
////
////   ref_t messageRef = info.methodHints.get(Attribute(callNode.argument, maEmbeddableRet));
////   if (messageRef != 0) {
////      if (argMode) {
////         // allocate & inject temporal variable
////         ref_t argRef = info.methodHints.get(Attribute(callNode.argument, maReference));
////         int size = defineStructSize(scope, argRef, 0);
////         SNode tempLocal = compiler.injectTempLocal(callNode, size, false);
////
////         // inject byref arg
////         callNode.setArgument(messageRef);
////
////         // inject sequence expr
////         callNode.injectNode(callNode.type, callNode.argument);
////         callNode.set(lxSeqExpression, 0);
////         callNode.appendNode(tempLocal.type, tempLocal.argument);
////      }
////      else compiler.injectEmbeddableRet(node, callNode, messageRef);
////
////      return true;
////   }
////   else return false;
////}

bool CompilerLogic :: optimizeEmbeddableOp(_ModuleScope& scope, _Compiler& compiler, SNode node)
{
   SNode callNode = node.findSubNode(lxDirectCalling, lxSDirectCalling);
   SNode assignNode = callNode.nextNode();
   SNode copyNode = assignNode.nextNode();
   SNode callTarget = callNode.findChild(lxCallTarget);

   ClassInfo info;
   if(!defineClassInfo(scope, info, callTarget.argument))
      return false;

   for (int i = 0; i < EMBEDDABLEOP_MAX; i++) {
      EmbeddableOp op = embeddableOps[i];
      ref_t subject = info.methodHints.get(Attribute(callNode.argument, op.attribute));

      //ref_t initConstructor = encodeMessage(INIT_MESSAGE_ID, 0) | SPECIAL_MESSAGE;

      // if it is possible to replace get&subject operation with eval&subject2:local
      if (subject != 0) {
         return compiler.injectEmbeddableOp(scope, assignNode, callNode, copyNode, subject, op.paramCount);
      }
   }

   return false;
}

bool CompilerLogic :: optimizeEmbeddable(SNode node, _ModuleScope& scope)
{
   // check if it is a virtual call
   if (node == lxDirectCalling && getArgCount(node.argument) == 1) {
      SNode callTarget = node.findChild(lxCallTarget);

      ClassInfo info;
      if (defineClassInfo(scope, info, callTarget.argument)
         && info.methodHints.get(Attribute(node.argument, maEmbeddableIdle)) == INVALID_REF)
      {
         // if it is an idle call, remove it
         node = lxExpression;

         return true;
      }
   }

   return false;
}

bool CompilerLogic :: optimizeBranchingOp(_ModuleScope&, SNode node)
{
   SNode intOpNode = node.parentNode();
//   while (intOpNode == lxExpression)
//      intOpNode = intOpNode.parentNode();

   SNode lnode = intOpNode.findSubNodeMask(lxObjectMask);

   SNode assignNode = intOpNode.nextNode(lxObjectMask);
   SNode opNode = assignNode.nextNode(lxObjectMask);

//   while (opNode == lxExpression)
//      opNode = opNode.parentNode();

   int operation_id = intOpNode.argument;
   if (intOpNode != lxNone && opNode == lxBranching) {
      int arg = node.findChild(lxIntValue).argument;

      SNode ifNode = opNode.findChild(lxIf, lxElse);
      if (ifNode != lxNone) {
         SNode trueNode = intOpNode.findChild(ifNode == lxIf ? lxIfValue : lxElseValue);

         if (lnode == lxConstantInt) {
            // if the numeric constant is the first operand
            if (intOpNode.argument == LESS_OPERATOR_ID) {
               operation_id = GREATER_OPERATOR_ID;
            }
            else if (intOpNode.argument == GREATER_OPERATOR_ID) {
               operation_id = LESS_OPERATOR_ID;
            }
         }

         SNode targetNode = opNode.firstChild(lxObjectMask);
         if (targetNode != lxLocal || assignNode != lxAssigning)
            return false;

         if (operation_id == EQUAL_OPERATOR_ID) {
            if (trueNode.argument == ifNode.argument) {
               ifNode.set(lxIfN, arg);
            }
            else if (trueNode.argument != ifNode.argument) {
               ifNode.set(lxIfNotN, arg);
            }
            else return false;
         }
         else if (operation_id == LESS_OPERATOR_ID) {
            if (trueNode.argument == ifNode.argument) {
               ifNode.set(lxLessN, arg);
            }
            else if (trueNode.argument != ifNode.argument) {
               ifNode.set(lxNotLessN, arg);
            }
            else return false;
         }
         else if (operation_id == GREATER_OPERATOR_ID) {
            if (trueNode.argument == ifNode.argument) {
               ifNode.set(lxGreaterN, arg);
            }
            else if (trueNode.argument != ifNode.argument) {
               ifNode.set(lxNotGreaterN, arg);
            }
            else return false;
         }
         else return false;

         intOpNode.argument = operation_id;

         targetNode.set(lxResult, 0);

         // comment out constant
         node = lxIdle;

         assignNode = lxIdle;
         intOpNode = lxExpression;

         return true;
      }
   }
   return false;
}

inline mssg_t resolveNonpublic(ClassInfo& info, _Module* module, mssg_t publicMessage, ref_t& visibility)
{
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      Attribute key = it.key();
      if (key.value1 == publicMessage && (key.value2 == maPrivate || key.value2 == maProtected || key.value2 == maInternal)) {
         visibility = key.value2;

         // get multi method
         pos_t argCount = 0;
         ref_t actionRef = 0, flags = 0, signRef = 0;
         decodeMessage(*it, actionRef, argCount, flags);

         ident_t actionStr = module->resolveAction(actionRef, signRef);

         if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
            // COMPILER MAGIC : for variadic message - use the most general message
            ref_t genericActionRef = module->mapAction(actionStr, 0, false);
            mssg_t genericMessage = encodeMessage(genericActionRef, 2, flags);

            return genericMessage;
         }
         else if (signRef) {
            ref_t genericActionRef = module->mapAction(actionStr, 0, false);
            mssg_t genericMessage = encodeMessage(genericActionRef, argCount, flags);

            return genericMessage;
         }
         else return *it;
      }
   }

   return 0;
}

mssg_t CompilerLogic :: resolveMultimethod(_ModuleScope& scope, mssg_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef,
   int& stackSafeAttr, bool selfCall)
{
   if (!targetRef)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, targetRef)) {
      if (isStacksafeArg(info))
         stackSafeAttr |= 1;

      // check if it is non public message
      ref_t type = 0;
      mssg_t nonPublicMultiMessage = resolveNonpublic(info, scope.module, multiMessage, type);
      if (nonPublicMultiMessage != 0) {
         // if it is an internal
         mssg_t resolved = 0;
         if (type == maInternal && scope.isInteralOp(targetRef)) {
            resolved = resolveMultimethod(scope, nonPublicMultiMessage, targetRef, implicitSignatureRef, stackSafeAttr, selfCall);
            if (!resolved) {
               return nonPublicMultiMessage;
            }
            else return resolved;
         }
         else if ((type == maPrivate || type == maProtected) && selfCall) {
            resolved = resolveMultimethod(scope, nonPublicMultiMessage, targetRef, implicitSignatureRef, stackSafeAttr, selfCall);
            if (!resolved) {
               return nonPublicMultiMessage;
            }
            else return resolved;
         }
      }

      if (!implicitSignatureRef)
         return 0;

      ref_t signatures[ARG_COUNT];
      size_t signatureLen = scope.module->resolveSignature(implicitSignatureRef, signatures);

      ref_t listRef = info.methodHints.get(Attribute(multiMessage, maOverloadlist));
      if (listRef == 0 && isMethodPrivate(info, multiMessage))
         listRef = info.methodHints.get(Attribute(multiMessage | STATIC_MESSAGE, maOverloadlist));

      if (listRef) {
         _Module* argModule = scope.loadReferenceModule(listRef, listRef);

         _Memory* section = argModule->mapSection(listRef | mskRDataRef, true);
         if (!section || section->Length() < 4)
            return 0;

         MemoryReader reader(section);
         pos_t position = section->Length() - 4;
         mssg_t foundMessage = 0;
         while (position != 0) {
            reader.seek(position - 8);
            mssg_t argMessage = reader.getDWord();
            ref_t argSign = 0;
            argModule->resolveAction(getAction(argMessage), argSign);

            if (argModule == scope.module) {
               if (isSignatureCompatible(scope, argSign, signatures, signatureLen)) {
                  setSignatureStacksafe(scope, argSign, stackSafeAttr);

                  foundMessage = argMessage;
               }
            }
            else {
               if (isSignatureCompatible(scope, argModule, argSign, signatures, signatureLen)) {
                  setSignatureStacksafe(scope, argModule, argSign, stackSafeAttr);

                  foundMessage = importMessage(argModule, argMessage, scope.module);
               }
            }

            position -= 8;
         }

         return foundMessage;
      }
      else if (getSignature(scope, multiMessage) == implicitSignatureRef) {
         setSignatureStacksafe(scope, implicitSignatureRef, stackSafeAttr);

         return multiMessage;
      }
   }

   return 0;
}

inline size_t readSignatureMember(ident_t signature, size_t index)
{
   int level = 0;
   size_t len = getlength(signature);
   for (size_t i = index; i < len; i++) {
      if (signature[i] == '&') {
         if (level == 0) {
            return i;
         }
         else level--;
      }
      else if (signature[i] == '#') {
         String<char, 5> tmp;
         size_t numEnd = signature.find(i, '&', NOTFOUND_POS);
         tmp.copy(signature.c_str() + i + 1, numEnd - i - 1);
         level += ident_t(tmp).toInt();
      }
   }

   return len;
}

inline void decodeClassName(IdentifierString& signature)
{
   ident_t ident = signature.ident();

   if (ident.startsWith(TEMPLATE_PREFIX_NS_ENCODED)) {
      // if it is encodeded weak reference - decode only the prefix
      signature[0] = '\'';
      signature[strlen(TEMPLATE_PREFIX_NS_ENCODED) - 1] = '\'';
   }
   else if (ident.startsWith(TEMPLATE_PREFIX_NS)) {
      // if it is weak reference - do nothing
   }
   else signature.replaceAll('@', '\'', 0);
}

ref_t CompilerLogic :: resolveExtensionTemplate(_ModuleScope& scope, _Compiler& compiler, ident_t pattern,
   ref_t implicitSignatureRef, ident_t ns, ExtensionMap* outerExtensionList)
{
   size_t argumentLen = 0;
   ref_t parameters[ARG_COUNT] = { 0 };
   ref_t signatures[ARG_COUNT];
   /*size_t signatureLen = */scope.module->resolveSignature(implicitSignatureRef, signatures);

   // matching pattern with the provided signature
   size_t i = pattern.find('.') + 2;

   IdentifierString templateName(pattern, i - 2);
   ref_t templateRef = scope.mapFullReference(templateName.ident(), true);

   size_t len = getlength(pattern);
   bool matched = true;
   size_t signIndex = 0;
   while (matched && i < len) {
      if (pattern[i] == '{') {
         size_t end = pattern.find(i, '}', 0);

         String<char, 5> tmp;
         tmp.copy(pattern + i + 1, end - i - 1);

         size_t index = ident_t(tmp).toInt();

         parameters[index - 1] = signatures[signIndex];
         if (argumentLen < index)
            argumentLen = index;

         i = end + 2;
      }
      else {
         size_t end = pattern.find(i, '/', getlength(pattern));
         IdentifierString argType;
         argType.copy(pattern + i, end - i);

         if (argType.ident().find('{') != NOTFOUND_POS) {
            ref_t argRef = signatures[signIndex];
            // bad luck : if it is a template based argument
            ident_t signType;
            while (argRef) {
               // try to find the template based signature argument
               signType = scope.module->resolveReference(argRef);
               if (!isTemplateWeakReference(signType)) {
                  ClassInfo info;
                  defineClassInfo(scope, info, argRef, true);
                  argRef = info.header.parentRef;
               }
               else break;
            }

            if (argRef) {
               size_t argLen = getlength(argType);
               size_t start = 0;
               size_t argIndex = argType.ident().find('{');
               while (argIndex < argLen && matched) {
                  if (argType.ident().compare(signType, start, argIndex - start)) {
                     size_t paramEnd = argType.ident().find(argIndex, '}', 0);

                     String<char, 5> tmp;
                     tmp.copy(argType.c_str() + argIndex + 1, paramEnd - argIndex - 1);

                     IdentifierString templateArg;
                     size_t nextArg = readSignatureMember(signType, argIndex - start);
                     templateArg.copy(signType + argIndex - start, nextArg - argIndex + start);
                     decodeClassName(templateArg);

                     signType = signType + nextArg + 1;

                     size_t index = ident_t(tmp).toInt();
                     ref_t templateArgRef = scope.mapFullReference(templateArg);
                     if (!parameters[index - 1]) {
                        parameters[index - 1] = templateArgRef;
                     }
                     else if (parameters[index - 1] != templateArgRef) {
                        matched = false;
                        break;
                     }

                     if (argumentLen < index)
                        argumentLen = index;

                     start = paramEnd + 2;
                     argIndex = argType.ident().find(start, '{', argLen);
                  }
                  else matched = false;
               }

               if (matched && start < argLen) {
                  // validate the rest part
                  matched = argType.ident().compare(signType, start, argIndex - start);
               }
            }
            else matched = false;
         }
         else {
            ref_t argRef = scope.mapFullReference(argType.ident(), true);
            matched = isCompatible(scope, argRef, signatures[signIndex], true);
         }

         i = end + 1;
      }

      signIndex++;
   }

   if (matched) {
      return compiler.generateExtensionTemplate(scope, templateRef, argumentLen, parameters, ns, outerExtensionList);
   }

   return 0;
}

bool CompilerLogic :: validateMessage(_ModuleScope& scope, mssg_t message, int hints)
{
   bool dispatchOne = message == scope.dispatch_message;
   if (testany(hints, tpConstructor | tpStatic)) {
      if (dispatchOne)
         return false;
   }
   //else {
      //else if (!isClassClass && dispatchOne && getParamCount(message) != 0) {
      //   return false;
      //}
   //}

   // const attribute can be applied only to a get-property
   if (test(hints, tpConstant) && ((message & PREFIX_MESSAGE_MASK) != PROPERTY_MESSAGE && getArgCount(message) > 1))
      return false;

   return true;
}
