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
      return checkMethod(info, message, outputType);
   }
   else return tpUnknown;
}

int CompilerLogic :: resolveCallType(_CompilerScope& scope, ref_t classReference, ref_t messageRef, bool& classFound, ref_t& outputType)
{
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

bool CompilerLogic :: resolveBranchOperation(int operatorId, ref_t& reference)
{
   return false;
}

bool CompilerLogic :: isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef)
{
   while (sourceRef != 0) {
      if (targetRef != sourceRef) {
         ClassInfo info;
         defineClassInfo(scope, info, sourceRef);

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

void CompilerLogic :: injectVirtualMethods(SNode node, _CompilerScope& scope, _Compiler& compiler)
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
}

void CompilerLogic :: defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference)
{
//      if (isTemplateRef(classRef)) {
//         variable.kind = okTemplateLocal;
//      }
   switch (reference)
   {
      case V_INT32:
         info.header.flags = elDebugDWORD | elStructureRole;
         info.size = 4;
         break;
      default:
         if (reference != 0) {
            scope.loadClassInfo(info, reference, true);
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

void CompilerLogic :: tweakInlineClassFlags(ref_t classRef, ClassInfo& info)
{
   // stateless inline class
   if (info.fields.Count() == 0 && !test(info.header.flags, elStructureRole)) {
      info.header.flags |= elStateless;

      // stateless inline class is its own class class
      info.header.classRef = classRef;
   }
   else info.header.flags &= ~elStateless;

   // nested class is sealed if it has no parent
   if (!test(info.header.flags, elClosed))
      info.header.flags |= elSealed;
}

bool CompilerLogic :: validateClassAttribute(int& attrValue)
{
   return attrValue > 0;
}

bool CompilerLogic :: validateMethodAttribute(int& attrValue)
{
   return false;
}

bool CompilerLogic :: validateFieldAttribute(int& attrValue)
{
   return false;
}

bool CompilerLogic :: validateLocalAttribute(int& attrValue)
{
   return false;
}
