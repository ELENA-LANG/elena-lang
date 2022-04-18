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

struct Op
{
   BuildKey operation;

   ref_t    loperand;
   ref_t    roperand;
   ref_t    ioperand;
};

constexpr auto OperationLength = 4;
constexpr Op Operations[OperationLength] =
{
   { BuildKey::StrDictionaryOp, V_DICTIONARY, V_INT32, V_STRING },
   { BuildKey::AttrDictionaryOp, V_OBJATTRIBUTES, V_OBJECT, V_STRING },
   { BuildKey::ObjArrayOp, V_OBJARRAY, V_OBJECT, 0 },
   { BuildKey::ObjOp, V_OBJECT, V_OBJECT, 0 }
};

inline bool isPrimitiveCompatible(ref_t targetRef, ref_t sourceRef)
{
   switch (targetRef) {
      default:
         return targetRef == sourceRef;
   }
}

// --- CompilerLogic ---

bool CompilerLogic :: isValidObjOp(int operatorId)
{
   switch (operatorId) {
      default:
         return false;
   }
}

bool CompilerLogic :: isValidObjArrayOp(int operatorId)
{
   switch (operatorId) {
      case ADD_ASSIGN_OPERATOR_ID:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: isValidStrDictionaryOp(int operatorId)
{
   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic::isValidAttrDictionaryOp(int operatorId)
{
   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         return true;
      default:
         return false;
   }
}

bool CompilerLogic :: isValidOp(int operatorId, BuildKey op)
{
   switch (op) {
      case BuildKey::ObjOp:
         return isValidObjOp(operatorId);
      case BuildKey::StrDictionaryOp:
         return isValidStrDictionaryOp(operatorId);
      case BuildKey::ObjArrayOp:
         return isValidObjArrayOp(operatorId);
      case BuildKey::AttrDictionaryOp:
         return isValidAttrDictionaryOp(operatorId);
      default:
         return false;
   }
}

BuildKey CompilerLogic :: resolveOp(int operatorId, ref_t* arguments, size_t length)
{
   for(size_t i = 0; i < OperationLength; i++) {
      if (arguments[0] == Operations[i].loperand && arguments[1] == Operations[i].roperand) {
         if ((length == 2) || (arguments[2] == Operations[i].ioperand)) {
            if (isValidOp(operatorId, Operations[i].operation))
               return Operations[i].operation;
         }
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
         return false;
   }

   return true;
}

bool CompilerLogic :: validateSymbolAttribute(ref_t attribute, Visibility& visibility)
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
         attrs.typeRef = V_INTBINARY;
         break;
      case V_STRINGOBJ:
         attrs.inlineArray = true;
         break;
      case V_EMBEDDABLE:
         attrs.isEmbeddable = true;
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
         return validateMethodAttribute(attribute, hint, dummy);
      default:
         return false;
   }
}

bool CompilerLogic :: validateDictionaryAttribute(ref_t attribute, ref_t& dictionaryType)
{
   switch (attribute) {
      case V_STRINGOBJ:
         dictionaryType = V_STRINGOBJ;
         return true;
      case V_SYMBOL:
         dictionaryType = V_OBJATTRIBUTES;
         return true;
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
      default:
         return false;
   }
}

bool CompilerLogic :: validateMessage(mssg_t message)
{
   return true;
}

void CompilerLogic :: validateClassDeclaration(ClassInfo& info, bool& emptyStructure)
{
   // a structure class should contain fields
   if (test(info.header.flags, elStructureRole) && info.size == 0)
      emptyStructure = true;
}

bool CompilerLogic :: isRole(ClassInfo& info)
{
   return test(info.header.flags, elRole);
}

bool CompilerLogic :: isEmbeddableStruct(ClassInfo& info)
{
   return test(info.header.flags, elStructureRole) && !test(info.header.flags, elDynamicRole);
}

void CompilerLogic :: tweakClassFlags(ClassInfo& info, bool classClassMode)
{
   if (classClassMode) {
      // class class is always stateless and final
      info.header.flags |= elStateless;
      info.header.flags |= elSealed;
   }
}

void CompilerLogic :: tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef)
{
   
}

void CompilerLogic :: writeAttrDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(2);
   writer.writeRef(reference);
}

bool CompilerLogic :: readAttrDictionary(ModuleBase* extModule, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope)
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

void CompilerLogic :: writeDictionaryEntry(MemoryBase* section, ustr_t key, int value)
{
   MemoryWriter writer(section);
   writer.writeString(key);
   writer.writeDWord(1);
   writer.writeDWord(value);
}

bool CompilerLogic :: readDictionary(MemoryBase* section, ReferenceMap& map)
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

void CompilerLogic :: injectVirtualCode(CompilerBase* compiler, SyntaxNode classNode, ModuleScopeBase* scope, 
   ref_t classRef, ClassInfo& classInfo)
{
   if (test(classInfo.header.flags, elClassClass)) {
      
   }
   else if (/*!test(classInfo.header.flags, elNestedClass) && */!test(classInfo.header.flags, elRole)) {
      // skip class classes, extensions and singletons
      if (classRef != scope->buildins.superReference && !test(classInfo.header.flags, elClosed)) {
         // auto generate cast$<type> message for explicitly declared classes
         ref_t signRef = scope->module->mapSignature(&classRef, 1, false);
         ref_t actionRef = scope->module->mapAction(CAST_MESSAGE, signRef, false);

         compiler->injectVirtualReturningMethod(scope, classNode, 
            encodeMessage(actionRef, 1, CONVERSION_MESSAGE), 
            *scope->selfVar, classRef);
      }
   }
}

bool CompilerLogic :: defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference, bool headerOnly)
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

SizeInfo CompilerLogic :: defineStructSize(ClassInfo& info)
{
   SizeInfo sizeInfo = { /*!test(info.header.flags, elReadOnlyRole)*/};

   if (isEmbeddableStruct(info)) {
      sizeInfo.size = info.size;

      return sizeInfo;
   }
   //else if (isEmbeddableArray(info)) {
   //   return info.size;
   //}

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
      //if (isCompatible(scope, V_INT32, elementRef, true)) {
      //   switch (info.size) {
      //      case 4:
      //         return V_INT32ARRAY;
      //      case 2:
      //         return V_INT16ARRAY;
      //      case 1:
      //         return V_INT8ARRAY;
      //      default:
      //         break;
      //   }
      //}
      return V_BINARYARRAY;
   }
   else return V_OBJARRAY;
}

bool CompilerLogic :: isCompatible(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef)
{
   if ((!targetRef || targetRef == scope.buildins.superReference) && !isPrimitiveRef(sourceRef))
      return true;

   if (isPrimitiveRef(targetRef) && isPrimitiveCompatible(targetRef, sourceRef))
      return true;

   while (sourceRef != 0) {
      if (targetRef != sourceRef) {
         ClassInfo info;
         if (!defineClassInfo(scope, info, sourceRef))
            return false;

         // if it is a structure wrapper
         if (isPrimitiveRef(targetRef) && test(info.header.flags, elWrapper)) {
            auto inner = *info.fields.start();
            if (isCompatible(scope, targetRef, inner.typeRef/*, ignoreNils*/))
               return true;
         }

         if (test(info.header.flags, elClassClass)) {
            // class class can be compatible only with itself and the super class
            sourceRef = scope.buildins.superReference;
         }
         else sourceRef = info.header.parentRef;

      }
      else return true;
   }

   return false;
}

ConversionRoutine CompilerLogic :: retrieveConversionRoutine(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef)
{
   ClassInfo info;
   if (!defineClassInfo(scope, info, targetRef))
      return { };

   // if the target class is wrapper around the source
   if (test(info.header.flags, elWrapper) && !test(info.header.flags, elDynamicRole)) {
      auto inner = *info.fields.start();

      bool compatible = false;
      compatible = isCompatible(scope, inner.typeRef, sourceRef);

      if (compatible)
         return { ConversionResult::BoxingRequired };
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
      if (!result.kind)
         result.kind = (ref_t)MethodHint::Normal;

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
}

bool CompilerLogic :: resolveCallType(ModuleScopeBase& scope, ref_t classRef, mssg_t message, 
   CheckMethodResult& result)
{
   ClassInfo info;
   if (classRef && defineClassInfo(scope, info, classRef)) {
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
