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

struct Op
{
   BuildKey operation;

   ref_t    loperand;
   ref_t    roperand;
   ref_t    ioperand;
};

constexpr auto OperationLength = 3;
constexpr Op Operations[OperationLength] =
{
   { BuildKey::StrDictionaryOp, V_DICTIONARY, V_INT32, V_STRING },
   { BuildKey::ObjArrayOp, V_OBJARRAY, V_OBJECT, 0 },
   { BuildKey::ObjOp, V_OBJECT, V_OBJECT, 0 }
};

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

bool CompilerLogic :: isValidOp(int operatorId, BuildKey op)
{
   switch (op) {
      case BuildKey::ObjOp:
         return isValidObjOp(operatorId);
      case BuildKey::StrDictionaryOp:
         return isValidStrDictionaryOp(operatorId);
      case BuildKey::ObjArrayOp:
         return isValidObjArrayOp(operatorId);
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
      case V_EMBEDDABLE:
         attrs.isEmbeddable = true;
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic :: validateMethodAttribute(ref_t attribute, MethodHint& hint, bool& explicitMode)
{
   switch (attribute) {
      case 0:
         break;
      case V_METHOD:
         explicitMode = true;
         break;
      case V_STATIC:
         hint = MethodHint::Static;
         break;
      case V_DISPATCHER:
         explicitMode = true;
         hint = MethodHint::Dispatcher;
         break;
      case V_CONSTRUCTOR:
         explicitMode = true;
         hint = MethodHint::Constructor;
         break;
      default:
         return false;
   }

   return true;
}

bool CompilerLogic::validateImplicitMethodAttribute(ref_t attribute, MethodHint& hint)
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
   if (attribute == V_STRINGOBJ) {
      dictionaryType = V_STRINGOBJ;

      return true;
   }
   else return false;
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
   return test(info.header.flags, elStructureRole)/* && !test(info.header.flags, elDynamicRole)*/;
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

//void CompilerLogic :: injectVirtualCode(ClassInfo& classInfo)
//{
//   if (test(classInfo.header.flags, elClassClass)) {
//      
//   }
//   else {
//      
//   }
//}

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
