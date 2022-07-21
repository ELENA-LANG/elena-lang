//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "langcommon.h"
#include "compilerlogic.h"

using namespace elena_lang;

inline MethodHint operator | (const MethodHint& l, const MethodHint& r)
{
   return (MethodHint)((unsigned int)l | (unsigned int)r);
}

inline MethodHint operator & (const MethodHint& l, const MethodHint& r)
{
   return (MethodHint)((unsigned int)l & (unsigned int)r);
}

inline MethodHint operator & (const ref_t& l, const MethodHint& r)
{
   return (MethodHint)(l & (unsigned int)r);
}

bool testMethodHint(MethodHint hint, MethodHint mask)
{
   return test((ref_t)hint, (ref_t)mask);
}

bool testMethodHint(ref_t hint, MethodHint mask)
{
   return test(hint, (ref_t)mask);
}

struct Op
{
   int      operatorId;
   BuildKey operation;

   ref_t    loperand;
   ref_t    roperand;
   ref_t    ioperand;
   ref_t    output;
};

constexpr auto OperationLength = 15;
constexpr Op Operations[OperationLength] =
{
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_INT32, V_STRING, V_OBJECT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_OBJECT, V_STRING, V_OBJECT
   },
   {
      SET_INDEXER_OPERATOR_ID, BuildKey::DictionaryOp, V_DICTIONARY, V_DECLARATION, V_STRING, V_OBJECT
   },
   {
      NAME_OPERATOR_ID, BuildKey::DeclOp, V_DECLARATION, 0, 0, V_STRING
   },
   {
      ADD_ASSIGN_OPERATOR_ID, BuildKey::ObjArrayOp, V_OBJARRAY, V_OBJECT, 0, V_OBJECT
   },
   {
      ADD_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   {
      SUB_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   },
   //{
   //   MUL_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   //},
   //{
   //   DIV_OPERATOR_ID, BuildKey::IntOp, V_INT32, V_INT32, 0, V_INT32
   //},
   {
      EQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      LESS_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      NOTEQUAL_OPERATOR_ID, BuildKey::IntCondOp, V_INT32, V_INT32, 0, V_FLAG
   },
   {
      NOT_OPERATOR_ID, BuildKey::BoolSOp, V_FLAG, 0, 0, V_FLAG
   },
   {
      LEN_OPERATOR_ID, BuildKey::ByteArraySOp, V_INT8ARRAY, 0, 0, V_INT32
   },
   {
      IF_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, 0, V_CLOSURE
   },
   {
      ELSE_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, 0, V_CLOSURE
   },
   {
      IF_ELSE_OPERATOR_ID, BuildKey::BranchOp, V_FLAG, V_CLOSURE, V_CLOSURE, V_CLOSURE
   },
//   {
//      ArraySetOperators, 1,
//      BuildKey::ByteArrayOp, V_INT8ARRAY, V_INT8, V_INT32, 0, false
//   },
//   { {}, 0,BuildKey::ObjOp, V_OBJECT, V_OBJECT, 0, V_OBJECT, false },
//   {
//      ArrayGetOperators, 1,
//      BuildKey::ByteArrayOp, V_INT8ARRAY, V_INT32, 0, V_INT8, true
//   },
};

inline bool isPrimitiveCompatible(TypeInfo target, TypeInfo source)
{
   switch (target.typeRef) {
      case V_OBJECT:
         return !isPrimitiveRef(source.typeRef);
      default:
         return target == source;
   }
}

// --- CompilerLogic ---

bool CompilerLogic :: isValidOp(int operatorId, const int* validOperators, size_t len)
{
   for (size_t i = 0; i < len; ++i) {
      if (validOperators[i] == operatorId)
         return true;
   }

   return false;
}

BuildKey CompilerLogic :: resolveOp(ModuleScopeBase& scope, int operatorId, ref_t* arguments, size_t length,
   ref_t& outputRef)
{
   for(size_t i = 0; i < OperationLength; i++) {
      if (Operations[i].operatorId == operatorId) {
         bool compatible = isCompatible(scope, { Operations[i].loperand }, { arguments[0] }, false);
         compatible = compatible && (length <= 1 || isCompatible(scope, { Operations[i].roperand }, { arguments[1] }, false));
         compatible = compatible && (length <= 2 || isCompatible(scope, { Operations[i].ioperand }, { arguments[2] }, false));
         if (compatible) {
            outputRef = Operations[i].output;

            return Operations[i].operation;
         }
      }
   }

   return BuildKey::None;
}

BuildKey CompilerLogic :: resolveNewOp(ModuleScopeBase& scope, ref_t loperand, ref_t* signatures, pos_t signatureLen)
{
   if (signatureLen == 1 &&
      isCompatible(scope, { V_INT32 }, { signatures[0] }, false))
   {
      ClassInfo info;
      if (defineClassInfo(scope, info, loperand, true)) {
         return test(info.header.flags, elDynamicRole) ? BuildKey::NewArrayOp : BuildKey::None;
      }
   }

   return BuildKey::None;
}

bool CompilerLogic :: validateTemplateAttribute(ref_t attribute, Visibility& visibility, TemplateType& type)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_INLINE:
         type = TemplateType::Inline;
         break;
      default:
      {
         ref_t dummy = 0;
         return validateClassAttribute(attribute, dummy, visibility);
      }
   }

   return true;
}

bool CompilerLogic :: validateSymbolAttribute(ref_t attribute, Visibility& visibility, bool& constant, bool& isStatic)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_SYMBOLEXPR:
         break;
      case V_CONST:
         constant = true;
         break;
      case V_STATIC:
         isStatic = true;
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateClassAttribute(ref_t attribute, ref_t& flags, Visibility& visibility)
{
   switch (attribute) {
      case V_PUBLIC:
         visibility = Visibility::Public;
         break;
      case V_PRIVATE:
         visibility = Visibility::Private;
         break;
      case V_CLASS:
         break;
      case V_STRUCT:
         flags |= elStructureRole;
         break;
      case V_CONST:
         flags |= elReadOnlyRole;
         return true;
      case V_SINGLETON:
         flags = elRole | elSealed | elStateless;
         break;
      case V_LIMITED:
         flags = (elClosed | elAbstract | elNoCustomDispatcher);
         break;
      case V_ABSTRACT:
         flags = elAbstract;
         break;
      case V_SEALED:
         flags = elSealed;
         break;
      case V_EXTENSION:
         flags = elExtension;
         break;
      case 0:
         // ignore idle
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateFieldAttribute(ref_t attribute, FieldAttributes& attrs)
{
   switch (attribute) {
      case V_FIELD:
         break;
      case V_INTBINARY:
      case V_WORDBINARY:
         attrs.typeInfo.typeRef = attribute;
         break;
      case V_STRINGOBJ:
         attrs.inlineArray = true;
         break;
      case V_EMBEDDABLE:
         attrs.isEmbeddable = true;
         break;
      case V_CONST:
         attrs.isConstant = true;
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateMethodAttribute(ref_t attribute, ref_t& hint, bool& explicitMode)
{
   switch (attribute) {
      case 0:
      case V_PUBLIC:
         break;
      case V_PRIVATE:
         hint = (ref_t)MethodHint::Private | (ref_t)MethodHint::Sealed;
         break;
      case V_PROTECTED:
         hint = (ref_t)MethodHint::Protected;
         break;
      case V_INTERNAL:
         hint = (ref_t)MethodHint::Internal;
         break;
      case V_METHOD:
         explicitMode = true;
         break;
      case V_STATIC:
         hint = (ref_t)MethodHint::Static;
         break;
      case V_DISPATCHER:
         explicitMode = true;
         hint = (ref_t)MethodHint::Dispatcher;
         break;
      case V_CONSTRUCTOR:
         explicitMode = true;
         hint = (ref_t)MethodHint::Constructor;
         break;
      case V_ABSTRACT:
         hint = (ref_t)MethodHint::Abstract;
         break;
      case V_GETACCESSOR:
         hint = (ref_t)MethodHint::GetAccessor;
         break;
      case V_SETACCESSOR:
         hint = (ref_t)MethodHint::SetAccessor;
         break;
      case V_CONST:
         hint = (ref_t)MethodHint::Constant | (ref_t)MethodHint::Sealed;
         return true;
      case V_FUNCTION:
         hint = (ref_t)MethodHint::Function;
         return true;
      case V_PREDEFINED:
         hint = (ref_t)MethodHint::Predefined;
         return true;
      case V_CONVERSION:
         hint = (ref_t)MethodHint::Conversion;
         return true;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateImplicitMethodAttribute(ref_t attribute, ref_t& hint)
{
   bool dummy = false;
   switch (attribute) {
      case V_METHOD:
      case V_DISPATCHER:
      case V_CONSTRUCTOR:
      case V_FUNCTION:
      case V_CONVERSION:
         return validateMethodAttribute(attribute, hint, dummy);
      default:
         return false;
   }
}

bool CompilerLogic :: validateDictionaryAttribute(ref_t attribute, TypeInfo& dictionaryTypeInfo)
{
   switch (attribute) {
      case V_STRINGOBJ:
         dictionaryTypeInfo.typeRef = V_STRINGOBJ;
         dictionaryTypeInfo.elementRef = V_SYMBOL;
         return true;
      case V_SYMBOL:
         dictionaryTypeInfo.typeRef = V_DICTIONARY;
         dictionaryTypeInfo.elementRef = V_SYMBOL;
         return true;
      //case V_DECLOBJ:
      //   dictionaryType = V_DECLATTRIBUTES;
      //   return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attrs)
{
   switch(attrValue) {
      case V_FORWARD:
         attrs |= ExpressionAttribute::Forward;
         return true;
      case V_INTERN:
         attrs |= ExpressionAttribute::Intern;
         return true;
      case V_VARIABLE:
         attrs |= ExpressionAttribute::NewVariable;
         return true;
      case V_EXTERN:
         attrs |= ExpressionAttribute::Extern;
         return true;
      case V_NEWOP:
         attrs |= ExpressionAttribute::NewOp;
         return true;
      case V_CONVERSION:
         attrs |= ExpressionAttribute::CastOp;
         return true;
      case V_WRAPPER:
         attrs |= ExpressionAttribute::RefOp;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateArgumentAttribute(ref_t attrValue, bool& byRefArg)
{
   switch (attrValue) {
      case V_WRAPPER:
         byRefArg = true;
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: validateMessage(ModuleScopeBase& scope, ref_t hints, mssg_t message)
{
   bool dispatchOne = message == scope.buildins.dispatch_message;
   if (testany((int)hints, (int)(MethodHint::Constructor | MethodHint::Static))) {
      if (dispatchOne)
         return false;
   }

   // const attribute can be applied only to a get-property
   if (testMethodHint(hints, MethodHint::Constant) 
      && ((message & PREFIX_MESSAGE_MASK) != PROPERTY_MESSAGE && getArgCount(message) > 1))
   {
      return false;
   }

   return true;
}

void CompilerLogic :: validateClassDeclaration(ModuleScopeBase& scope, ClassInfo& info, 
   bool& emptyStructure, bool& disptacherNotAllowed)
{
   // a structure class should contain fields
   if (test(info.header.flags, elStructureRole) && info.size == 0)
      emptyStructure = true;

   // interface class cannot have a custom dispatcher method
   if (test(info.header.flags, elNoCustomDispatcher)) {
      auto dispatchInfo = info.methods.get(scope.buildins.dispatch_message);

      disptacherNotAllowed = !dispatchInfo.inherited;
   }
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
}

bool CompilerLogic :: isEmbeddableArray(ModuleScopeBase& scope, ref_t reference)
{
   ClassInfo info;
   if (defineClassInfo(scope, info, reference, true)) {
      return isEmbeddableArray(info);
   }

   return false;
}

bool CompilerLogic :: isEmbeddableArray(ClassInfo& info)
{
   return test(info.header.flags, elDynamicRole | elStructureRole | elWrapper);
}

bool CompilerLogic :: isEmbeddableStruct(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole)
      && !test(info.header.flags, elDynamicRole);
}

bool CompilerLogic :: isEmbeddable(ModuleScopeBase& scope, ref_t reference)
{
   ClassInfo info;
   if (defineClassInfo(scope, info, reference, true)) {
      return isEmbeddable(info);
   }

   return false;
}

bool CompilerLogic :: isEmbeddable(ClassInfo& info)
{
   if (test(info.header.flags, elDynamicRole)) {
      return isEmbeddableArray(info);
   }
   else return isEmbeddableStruct(info);
}

bool CompilerLogic :: isMultiMethod(ClassInfo& info, MethodInfo& methodInfo)
{
   return test(methodInfo.hints, (ref_t)MethodHint::Multimethod);
}

void CompilerLogic :: tweakClassFlags(ref_t classRef, ClassInfo& info, bool classClassMode)
{
   if (classClassMode) {
      // class class is always stateless and final
      info.header.flags |= elStateless;
      info.header.flags |= elSealed;
   }

   if (test(info.header.flags, elNestedClass)) {
      // stateless inline class
      if (info.fields.count() == 0 && !test(info.header.flags, elStructureRole)) {
         info.header.flags |= elStateless;

         // stateless inline class is its own class class
         info.header.classRef = classRef;
      }
      else info.header.flags &= ~elStateless;
   }

   if (test(info.header.flags, elExtension))
      info.header.flags |= elSealed;
}

void CompilerLogic :: tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef)
{
   
}

void CompilerLogic :: writeTypeMapEntry(MemoryBase* section, ustr_t key, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(2);
   writer.writeRef(reference);
}

bool CompilerLogic :: readTypeMap(ModuleBase* extModule, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope)
{
   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      reader.readString(key);
      int type = reader.getDWord();

      if (type == 2) {
         ref_t reference = reader.getRef();
         if (scope->module != extModule) {
            reference = scope->importReference(extModule, reference);
         }

         map.add(*key, reference);
      }
      else return false;
   }

   return true;
}

//void CompilerLogic :: writeDeclDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference)
//{
//   MemoryWriter writer(section);
//   writer.writeString(key);
//   writer.writeDWord(3);
//   writer.writeRef(reference);
//}
//
//bool CompilerLogic :: readDeclDictionary(ModuleBase* extModule, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope)
//{
//   IdentifierString key;
//
//   MemoryReader reader(section);
//   while (!reader.eof()) {
//      reader.readString(key);
//      int type = reader.getDWord();
//
//      if (type == 3) {
//         ref_t reference = reader.getRef();
//         if (scope->module != extModule) {
//            reference = scope->importReference(extModule, reference);
//         }
//
//         map.add(*key, reference);
//      }
//      else return false;
//   }
//
//   return true;
//}

void CompilerLogic :: writeAttributeMapEntry(MemoryBase* section, ustr_t key, int value)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(1);
   writer.writeDWord(value);
}

bool CompilerLogic :: readAttributeMap(MemoryBase* section, ReferenceMap& map)
{
   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      reader.readString(key);
      int type = reader.getDWord();

      if (type == 1) {
         ref_t value = reader.getRef();

         map.add(*key, value);
      }
      else return false;
   }

   return true;
}

void CompilerLogic :: writeArrayEntry(MemoryBase* section, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeRef(reference);
}

void CompilerLogic :: writeExtMessageEntry(MemoryBase* section, ref_t extRef, mssg_t message, mssg_t strongMessage)
{
   MemoryWriter writer(section);
   writer.writeRef(extRef);
   writer.writeRef(message);
   writer.writeRef(strongMessage);
}

bool CompilerLogic :: readExtMessageEntry(ModuleBase* extModule, MemoryBase* section, ExtensionMap& map, ModuleScopeBase* scope)
{
   bool importMode = extModule != scope->module;

   IdentifierString key;

   MemoryReader reader(section);
   while (!reader.eof()) {
      ref_t extRef = reader.getRef();
      if (importMode)
         extRef = scope->importReference(extModule, extRef);

      mssg_t message = reader.getRef();
      if (importMode)
         message = scope->importMessage(extModule, message);

      mssg_t strongMessage = reader.getRef();
      if (importMode)
         strongMessage = scope->importMessage(extModule, strongMessage);

      map.add(message, { extRef, strongMessage });
   }

   return true;
}

bool CompilerLogic :: defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference, 
   bool headerOnly, bool fieldsOnly)
{
   if (isPrimitiveRef(reference) && !headerOnly) {
      scope.loadClassInfo(info, scope.buildins.superReference);
   }

   switch (reference)
   {
      case V_INT32:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugDWORD | */elStructureRole | elReadOnlyRole;
         info.size = 4;
         break;
      case V_INT8:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugDWORD | */elStructureRole | elReadOnlyRole;
         info.size = 1;
         break;
      case V_INT8ARRAY:
         info.header.parentRef = scope.buildins.superReference;
         info.header.flags = /*elDebugBytes | */elStructureRole | elDynamicRole | elWrapper;
         info.size = -1;
         break;
      default:
         if (reference != 0) {
            if (!scope.loadClassInfo(info, reference, headerOnly, fieldsOnly))
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

SizeInfo CompilerLogic :: defineStructSize(ClassInfo& info)
{
   SizeInfo sizeInfo = { /*!test(info.header.flags, elReadOnlyRole)*/};

   if (isEmbeddableStruct(info)) {
      sizeInfo.size = info.size;

      return sizeInfo;
   }
   else if (isEmbeddableArray(info)) {
      sizeInfo.size = info.size;

      return sizeInfo;
   }

   return {};
}

SizeInfo CompilerLogic :: defineStructSize(ModuleScopeBase& scope, ref_t reference)
{
   auto sizeInfo = scope.cachedSizes.get(reference);
   if (!sizeInfo.size) {
      ClassInfo classInfo;
      if (defineClassInfo(scope, classInfo, reference)) {
         sizeInfo = defineStructSize(classInfo);

         scope.cachedSizes.add(reference, sizeInfo);

         return sizeInfo;
      }
      else return { 0 };
   }
   else return sizeInfo;
}

ref_t CompilerLogic :: definePrimitiveArray(ModuleScopeBase& scope, ref_t elementRef, bool structOne)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, elementRef, true))
      return 0;

   if (isEmbeddableStruct(info) && structOne) {
      if (isCompatible(scope, { V_INT8 }, { elementRef }, true) && info.size == 1)
         return V_INT8ARRAY;

      //if (isCompatible(scope, V_INT32, elementRef, true)) {
      //   switch (info.size) {
      //      case 4:
      //         return V_INT32ARRAY;
      //      case 2:
      //         return V_INT16ARRAY;
      //      default:
      //         break;
      //   }
      //}
      return V_BINARYARRAY;
   }
   else return V_OBJARRAY;
}

bool CompilerLogic :: isCompatible(ModuleScopeBase& scope, TypeInfo targetInfo, TypeInfo sourceInfo, bool ignoreNils)
{
   //if ((!targetRef || targetRef == scope.buildins.superReference) && !isPrimitiveRef(sourceRef))
   //   return true;

   switch (sourceInfo.typeRef) {
      case V_NIL:
         // nil is compatible with a super class for the message dispatching
         // and with all types for all other cases
         if (!ignoreNils || targetInfo.typeRef == scope.buildins.superReference)
            return true;

         break;
      case V_STRING:
         if (targetInfo == sourceInfo) {
            return true;
         }
         else return isCompatible(scope, targetInfo, { scope.buildins.literalReference }, ignoreNils);
         break;
      case V_FLAG:
         return isCompatible(scope, targetInfo, { scope.branchingInfo.typeRef }, ignoreNils);
         break;
      default:
         if (targetInfo.typeRef == V_FLAG) {
            if (targetInfo == sourceInfo) {
               return true;
            }
            else return isCompatible(scope, { scope.branchingInfo.typeRef }, sourceInfo, ignoreNils);
         }
         break;
   }

   if (targetInfo.isPrimitive() && isPrimitiveCompatible(targetInfo, sourceInfo))
      return true;

   while (sourceInfo.typeRef != 0) {
      if (targetInfo != sourceInfo) {
         ClassInfo info;
         if (sourceInfo.isPrimitive() || !defineClassInfo(scope, info, sourceInfo.typeRef))
            return false;

         // if it is a structure wrapper
         if (targetInfo.isPrimitive() && test(info.header.flags, elWrapper)) {
            if (info.fields.count() > 0) {
               auto inner = *info.fields.start();
               if (isCompatible(scope, targetInfo, inner.typeInfo, ignoreNils))
                  return true;
            }
         }

         if (test(info.header.flags, elClassClass)) {
            // class class can be compatible only with itself and the super class
            sourceInfo = { scope.buildins.superReference };
         }
         else sourceInfo = { info.header.parentRef };
      }
      else return true;
   }

   return false;
}

inline ref_t getSignature(ModuleScopeBase& scope, mssg_t message)
{
   ref_t actionRef = getAction(message);
   ref_t signRef = 0;
   scope.module->resolveAction(actionRef, signRef);

   return signRef;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, ModuleBase* targetModule, ref_t targetSignature, 
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

      if (!isCompatible(scope, { scope.importReference(targetModule, targetSign) }, { sourceSignatures[i] }, true))
         return false;
   }

   return true;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, ref_t targetSignature, 
   ref_t* sourceSignatures, size_t sourceLen)
{
   ref_t targetSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(targetSignature, targetSignatures);
   if (sourceLen == 0 && len == 0)
      return true;

   if (len < 1)
      return false;

   for (size_t i = 0; i < sourceLen; i++) {
      ref_t targetSign = i < len ? targetSignatures[i] : targetSignatures[len - 1];
      if (!isCompatible(scope, { targetSign }, { sourceSignatures[i] }, true))
         return false;
   }

   return true;
}

bool CompilerLogic :: isSignatureCompatible(ModuleScopeBase& scope, mssg_t targetMessage, mssg_t sourceMessage)
{
   ref_t sourceSignatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(getSignature(scope, sourceMessage), sourceSignatures);

   return isSignatureCompatible(scope, getSignature(scope, targetMessage), sourceSignatures, len);
}

bool CompilerLogic :: isMessageCompatibleWithSignature(ModuleScopeBase& scope, mssg_t targetMessage, 
   ref_t* sourceSignature, size_t len)
{
   ref_t targetSignRef = getSignature(scope, targetMessage);

   if (isSignatureCompatible(scope, targetSignRef, sourceSignature, len)) {
      return true;
   }
   else return false;
}

ref_t CompilerLogic :: getClassClassRef(ModuleScopeBase& scope, ref_t targetRef)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef, true))
      return 0;

   return info.header.classRef;
}

mssg_t CompilerLogic :: resolveMultimethod(ModuleScopeBase& scope, mssg_t weakMessage, ref_t targetRef, ref_t implicitSignatureRef)
{
   if (!targetRef)
      return 0;

   ClassInfo info;
   if (defineClassInfo(scope, info, targetRef)) {
      if (!implicitSignatureRef)
         return 0;

      ref_t signatures[ARG_COUNT];
      size_t signatureLen = scope.module->resolveSignature(implicitSignatureRef, signatures);

      ref_t listRef = info.attributes.get({ weakMessage, ClassAttribute::OverloadList });
      if (listRef) {
         auto sectionInfo = scope.getSection(scope.module->resolveReference(listRef), mskConstArray, true);
         if (!sectionInfo.section || sectionInfo.section->length() < 4)
            return 0;

         MemoryReader reader(sectionInfo.section);
         pos_t position = sectionInfo.section->length() - 4;
         mssg_t foundMessage = 0;
         while (position != 0) {
            reader.seek(position - 8);
            mssg_t argMessage = reader.getRef();
            ref_t argSign = 0;
            sectionInfo.module->resolveAction(getAction(argMessage), argSign);

            if (sectionInfo.module == scope.module) {
               if (isSignatureCompatible(scope, argSign, signatures, signatureLen)) {
                  foundMessage = argMessage;
               }
            }
            else {
               if (isSignatureCompatible(scope, sectionInfo.module, 
                  argSign, signatures, signatureLen)) 
               {
                  foundMessage = scope.importMessage(sectionInfo.module, argMessage);
               }
            }

            position -= 8;
         }
         return foundMessage;
      }
   }

   return 0;
}

ref_t CompilerLogic :: retrieveImplicitConstructor(ModuleScopeBase& scope, ref_t targetRef, ref_t signRef, pos_t signLen)
{
   ref_t classClassRef = getClassClassRef(scope, targetRef);
   mssg_t messageRef = overwriteArgCount(scope.buildins.constructor_message, signLen);

   // try to resolve implicit multi-method
   mssg_t resolvedMessage = resolveMultimethod(scope, messageRef, classClassRef, signRef);
   if (resolvedMessage)
      return resolvedMessage;

   return 0;
}

ConversionRoutine CompilerLogic :: retrieveConversionRoutine(ModuleScopeBase& scope, ref_t targetRef, 
   TypeInfo sourceInfo)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef))
      return { };

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper) && !test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = false;
      compatible = isCompatible(scope, inner.typeInfo, sourceInfo, false);

      if (compatible)
         return { ConversionResult::BoxingRequired };
   }

   // COMPILE MAGIC : trying to typecast primitive array
   if (isEmbeddableArray(scope, sourceInfo.typeRef) && test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = isCompatible(scope, { inner.typeInfo.elementRef }, { sourceInfo.elementRef }, false);
      if (compatible)
         return { ConversionResult::BoxingRequired };
   }

   // if there is a implicit conversion routine
   if (!sourceInfo.isPrimitive()) {
      ref_t signRef = scope.module->mapSignature(&sourceInfo.typeRef, 1, false);
      mssg_t messageRef = retrieveImplicitConstructor(scope, targetRef, signRef, 1);
      if (messageRef)
         return { ConversionResult::Conversion, messageRef };
   }

   return {};
}

bool CompilerLogic :: checkMethod(ClassInfo& info, mssg_t message, CheckMethodResult& result)
{
   bool methodFound = info.methods.exist(message);
   if (methodFound) {
      MethodInfo methodInfo = info.methods.get(message);

      result.message = message;
      result.outputRef = methodInfo.outputRef;
      if (test(methodInfo.hints, (ref_t)MethodHint::Private)) {
         result.visibility = Visibility::Private;
      }
      else if (test(methodInfo.hints, (ref_t)MethodHint::Protected)) {
         result.visibility = Visibility::Protected;
      }
      else if (test(methodInfo.hints, (ref_t)MethodHint::Internal)) {
         result.visibility = Visibility::Internal;
      }
      else result.visibility = Visibility::Public;

      result.kind = methodInfo.hints & (ref_t)MethodHint::Mask;
      if (result.kind == (ref_t)MethodHint::Normal) {
         // check if the normal method can be called directly / semi-directly
         if (test(info.header.flags, elSealed)) {
            result.kind = (ref_t)MethodHint::Sealed; // mark it as sealed - because the class is sealed
         }
         else if (test(info.header.flags, elClosed)) {
            result.kind = (ref_t)MethodHint::Virtual; // mark it as virtual - because the class is closed
         }
      }

      switch ((MethodHint)result.kind) {
         case MethodHint::Virtual:
         case MethodHint::Sealed:
            result.stackSafe = true;
            break;
         default:
            result.stackSafe = false;
            break;
      }

      if (test(methodInfo.hints, (ref_t)MethodHint::Constant)) {
         result.constRef = info.attributes.get({ message, ClassAttribute::ConstantMethod });
      }

      return true;
   }
   else return false;
}

bool CompilerLogic :: checkMethod(ModuleScopeBase& scope, ref_t classRef, mssg_t message, CheckMethodResult& result)
{
   ClassInfo info;
   if (classRef && defineClassInfo(scope, info, classRef)) {
      return checkMethod(info, message, result);
   }
   else return false;
}

bool CompilerLogic :: resolveCallType(ModuleScopeBase& scope, ref_t classRef, mssg_t message, 
   CheckMethodResult& result)
{
   if (!classRef)
      classRef = scope.buildins.superReference;

   ClassInfo info;
   if (defineClassInfo(scope, info, classRef)) {
      if (!checkMethod(info, message, result)) {
         if (checkMethod(info, message | STATIC_MESSAGE, result)) {
            result.visibility = Visibility::Private;

            return true;
         }
         mssg_t protectedMessage = info.attributes.get({ message, ClassAttribute::ProtectedAlias });
         if (protectedMessage) {
            if(checkMethod(info, protectedMessage, result)) {
               result.visibility = Visibility::Protected;
               return true;
            }
         }
         mssg_t internalMessage = info.attributes.get({ message, ClassAttribute::InternalAlias });
         if (internalMessage) {
            if (checkMethod(info, internalMessage, result)) {
               result.visibility = Visibility::Internal;
               return true;
            }
         }
      }
      else return true;      
   }

   return false;
}

void CompilerLogic :: verifyMultimethods()
{
   
}

inline ustr_t resolveActionName(ModuleBase* module, mssg_t message)
{
   ref_t signRef = 0;
   return module->resolveAction(getAction(message), signRef);
}

ref_t CompilerLogic :: generateOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags, ClassInfo::MethodMap& methods, 
   mssg_t message, void* param, ref_t(*resolve)(void*, ref_t))
{
   // create a new overload list
   ref_t listRef = scope.mapAnonymous(resolveActionName(scope.module, message));

   // sort the overloadlist
   CachedList<mssg_t, 0x20> list;
   for (auto m_it = methods.start(); !m_it.eof(); ++m_it) {
      auto methodInfo = *m_it;
      if (methodInfo.multiMethod == message) {
         bool added = false;
         mssg_t omsg = m_it.key();
         pos_t len = list.count_pos();
         for (pos_t i = 0; i < len; i++) {
            if (isSignatureCompatible(scope, omsg, list[i])) {
               list.insert(i, omsg);
               added = true;
               break;
            }
         }
         if (!added)
            list.add(omsg);
      }
   }

   // fill the overloadlist
   for (size_t i = 0; i < list.count(); i++) {
      ref_t classRef = resolve(param, list[i]);

      if (test(flags, elSealed) || test(message, STATIC_MESSAGE)) {
         compiler->generateOverloadListMember(scope, listRef, classRef, list[i], MethodHint::Sealed);
      }
      else if (test(flags, elClosed)) {
         compiler->generateOverloadListMember(scope, listRef, classRef, list[i], MethodHint::Virtual);
      }
      else compiler->generateOverloadListMember(scope, listRef, classRef, list[i], MethodHint::Normal);
   }

   return listRef;
}

ref_t paramFeedback(void* param, ref_t)
{
#if defined(__LP64__)
   size_t val = (size_t)param;

   return (ref_t)val;
#else
   return (ref_t)param;
#endif
}

void CompilerLogic::injectMethodOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags,
   mssg_t message, ClassInfo::MethodMap& methods, ClassAttributes& attributes,
   void* param, ref_t(*resolve)(void*, ref_t))
{
   ref_t listRef = generateOverloadList(compiler, scope, flags, methods, message, param, resolve);

   ClassAttributeKey key = { message, ClassAttribute::OverloadList };
   attributes.exclude(key);
   attributes.add(key, listRef);
}

void CompilerLogic :: injectOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ClassInfo& info, ref_t classRef)
{
   for (auto it = info.methods.start(); !it.eof(); ++it) {
      auto methodInfo = *it;
      if (!methodInfo.inherited && isMultiMethod(info, methodInfo)) {
         // create a new overload list
         mssg_t message = it.key();

         injectMethodOverloadList(compiler, scope, info.header.flags, message, 
            info.methods, info.attributes, (void*)classRef, paramFeedback);
      }
   }
}

