//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPILERLOGIC_H
#define COMPILERLOGIC_H

#include "buildtree.h"
#include "clicommon.h"

namespace elena_lang
{
   struct CheckMethodResult
   {
      mssg_t      message;
      ref_t       kind;
      ref_t       outputRef;
      ref_t       constRef;
      Visibility  visibility;
      bool        stackSafe;
      bool        withVariadicDispatcher;
      bool        withCustomDispatcher;
   };

   struct TypeAttributes
   {
      bool variadicOne;
      bool variableOne;
      bool byRefOne;
      bool outRefOne;
      bool mssgNameLiteral;
      bool newOp;
      bool classOne;
      bool typecastOne;

      bool isNonempty() const
      {
         return variableOne || variadicOne || byRefOne || outRefOne || mssgNameLiteral || newOp || typecastOne;
      }
   };

   typedef CachedList<Pair<mssg_t, ref_t>, 10> VirtualMethods;

   inline pos_t OpHashRule(int id)
   {
      return id;
   }

   // --- CompilerLogic ---
   class CompilerLogic
   {
   public:
      struct Op
      {
         int      operatorId;
         BuildKey operation;

         ref_t    loperand;
         ref_t    roperand;
         ref_t    ioperand;
         ref_t    output;
      };

      typedef HashTable<int, Op, OpHashRule, MAX_OPERATOR_ID> OperationMap;

   private:
      OperationMap _operations;

      ref_t generateOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags, ClassInfo::MethodMap& methods, 
         mssg_t message, void* param, ref_t(*resolve)(void*, ref_t));

      bool isSignatureCompatible(ModuleScopeBase& scope, ref_t targetSignature, ref_t* sourceSignatures, size_t sourceLen);
      bool isSignatureCompatible(ModuleScopeBase& scope, ModuleBase* targetModule, ref_t targetSignature, 
         ref_t* sourceSignatures, size_t sourceLen);

      void setSignatureStacksafe(ModuleScopeBase& scope, ModuleBase* targetModule,
         ref_t targetSignature, int& stackSafeAttr);

      void loadOperations();

   public:
      BuildKey resolveOp(ModuleScopeBase& scope, int operatorId, ref_t* arguments, size_t length, ref_t& outputRef);
      BuildKey resolveNewOp(ModuleScopeBase& scope, ref_t loperand, ref_t* arguments, pos_t length);

      bool defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference, bool headerOnly = false, bool fieldsOnly = false);

      ref_t getClassClassRef(ModuleScopeBase& scope, ref_t targetRef);

      void setSignatureStacksafe(ModuleScopeBase& scope, ref_t targetSignature, int& stackSafeAttr);

      SizeInfo defineStructSize(ClassInfo& info);
      SizeInfo defineStructSize(ModuleScopeBase& scope, ref_t reference);
      ref_t definePrimitiveArray(ModuleScopeBase& scope, ref_t elementRef, bool structOne);

      bool validateTemplateAttribute(ref_t attribute, Visibility& visibility, TemplateType& type);
      bool validateSymbolAttribute(ref_t attribute, Visibility& visibility, bool& constant, bool& isStatic);
      bool validateClassAttribute(ref_t attribute, ref_t& flags, Visibility& visibility);
      bool validateFieldAttribute(ref_t attribute, FieldAttributes& attrs);
      bool validateMethodAttribute(ref_t attribute, ref_t& hint, bool& explicitMode);
      bool validateImplicitMethodAttribute(ref_t attribute, ref_t& hint);
      bool validateDictionaryAttribute(ref_t attribute, TypeInfo& dictionaryTypeInfo, bool& superMode);
      bool validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attrs);
      bool validateArgumentAttribute(ref_t attrValue, TypeAttributes& attributes);
      bool validateTypeScopeAttribute(ref_t attrValue, TypeAttributes& attributes);
      bool validateResendAttribute(ref_t attrValue, bool& superMode);

      bool validateAutoType(ModuleScopeBase& scope, ref_t& reference);

      bool isTryDispatchAllowed(ModuleScopeBase& scope, mssg_t message);
      mssg_t defineTryDispatcher(ModuleScopeBase& scope, mssg_t message);
      ref_t defineByRefSignature(ModuleScopeBase& scope, ref_t signRef, ref_t resultRef);

      bool isRole(ClassInfo& info);
      bool isAbstract(ClassInfo& info);
      bool isReadOnly(ClassInfo& info);
      bool withVariadicsMethods(ClassInfo& info);

      bool isDynamic(ClassInfo& info);
      bool isEmbeddableArray(ClassInfo& info);
      bool isEmbeddableArray(ModuleScopeBase& scope, ref_t reference);

      bool isEmbeddableStruct(ClassInfo& info);
      bool isEmbeddableStruct(ModuleScopeBase& scope, ref_t reference);

      bool isEmbeddableAndReadOnly(ModuleScopeBase& scope, ref_t reference);
      bool isEmbeddableAndReadOnly(ClassInfo& info);
      bool isEmbeddable(ModuleScopeBase& scope, ref_t reference);
      bool isEmbeddable(ClassInfo& info);

      bool isWrapper(ModuleScopeBase& scope, ref_t reference);
      bool isWrapper(ClassInfo& info);

      bool isStacksafeArg(ModuleScopeBase& scope, ref_t reference);
      bool isStacksafeArg(ClassInfo& info);

      bool isMultiMethod(ClassInfo& info, MethodInfo& methodInfo);

      bool isValidOp(int operatorId, const int* validOperators, size_t len);

      bool isNumericType(ModuleScopeBase& scope, ref_t& reference);

      ref_t retrievePrimitiveType(ModuleScopeBase& scope, ref_t reference);

      void tweakClassFlags(ModuleScopeBase& scope, ref_t classRef, ClassInfo& info, bool classClassMode);
      void tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef);

      bool validateMessage(ModuleScopeBase& scope, ref_t hints, mssg_t message);
      void validateClassDeclaration(ModuleScopeBase& scope, ErrorProcessorBase* errorProcessor, ClassInfo& info,
         bool& emptyStructure, bool& dispatcherNotAllowed, bool& withAbstractMethods, mssg_t& mixedUpVariadicMessage);

      void writeAttributeMapEntry(MemoryBase* section, ustr_t key, int value);
      void writeAttributeMapEntry(MemoryBase* section, ustr_t key, ustr_t value);

      void writeArrayEntry(MemoryBase* section, ref_t reference);
      void writeArrayReference(MemoryBase* section, ref_t reference);

      void writeTypeMapEntry(MemoryBase* section, ustr_t key, ref_t reference);

      //void writeDeclDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference);
      //bool readDeclDictionary(ModuleBase* module, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope);

      void writeExtMessageEntry(MemoryBase* section, ref_t extRef, mssg_t message, mssg_t strongMessage);
      void writeExtMessageEntry(MemoryBase* section, mssg_t message, ustr_t pattern);
      bool readExtMessageEntry(ModuleBase* module, MemoryBase* section, ExtensionMap& map, 
         ExtensionTemplateMap& extensionTemplates, ModuleScopeBase* scope);

      bool isCompatible(ModuleScopeBase& scope, TypeInfo targetInfo, TypeInfo sourceInfo, bool ignoreNils);
      bool isPrimitiveCompatible(ModuleScopeBase& scope, TypeInfo target, TypeInfo source);
      bool isTemplateCompatible(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef, bool weakCompatible);

      bool isSignatureCompatible(ModuleScopeBase& scope, mssg_t targetMessage, mssg_t sourceMessage);
      bool isMessageCompatibleWithSignature(ModuleScopeBase& scope, ref_t targetRef, 
         mssg_t targetMessage, ref_t* sourceSignature, size_t len, int& stackSafeAttr);

      mssg_t retrieveImplicitConstructor(ModuleScopeBase& scope, ref_t targetRef, ref_t signRef, 
         pos_t signLen, int& stackSafeAttrs);

      mssg_t retrieveDynamicConvertor(ModuleScopeBase& scope, ref_t targetRef);

      ConversionRoutine retrieveConversionRoutine(CompilerBase* compiler, ModuleScopeBase& scope, ustr_t ns, 
         ref_t targetRef, TypeInfo sourceInfo);

      bool checkMethod(ClassInfo& info, mssg_t message, CheckMethodResult& result);
      bool checkMethod(ModuleScopeBase& scope, ref_t reference, mssg_t message, CheckMethodResult& result);

      // check if internal / protected / private / public message is declared
      bool isMessageSupported(ClassInfo& info, mssg_t message, CheckMethodResult& result);
      bool isMessageSupported(ClassInfo& info, mssg_t message);

      bool resolveCallType(ModuleScopeBase& scope, ref_t classRef, mssg_t message, 
         CheckMethodResult& result);

      mssg_t resolveSingleDispatch(ModuleScopeBase& scope, ref_t reference, ref_t weakMessage);

      void injectOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ClassInfo& info, ref_t classRef);
      void injectMethodOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags, 
         mssg_t message, ClassInfo::MethodMap& methods, ClassAttributes& attributes,
         void* param, ref_t(*resolve)(void*, ref_t), ClassAttribute attribute);

      bool isNeedVerification(ClassInfo& info, VirtualMethodList& implicitMultimethods);
      bool verifyMultimethod(ModuleScopeBase& scope, ClassInfo& info, mssg_t message);

      bool validateDispatcherType(ClassInfo& classInfo);

      mssg_t resolveMultimethod(ModuleScopeBase& scope, mssg_t weakMessage, ref_t targetRef, 
         ref_t implicitSignatureRef, int& stackSafeAttr, bool selfCall);

      virtual ref_t resolveExtensionTemplate(ModuleScopeBase& scope, CompilerBase* compiler, ustr_t pattern,
         ref_t signatureRef, ustr_t ns, ExtensionMap* outerExtensionList);

      bool isValidType(ClassInfo& info, bool allowRole);
      bool isValidType(ModuleScopeBase& scope, ref_t classReference, bool ignoreUndeclared, bool allowRole);

      pos_t definePadding(ModuleScopeBase& scope, pos_t offset, pos_t size);

      static bool isPrimitiveArrRef(ref_t reference)
      {
         switch (reference) {
            case V_OBJARRAY:
            case V_INT32ARRAY:
            case V_INT16ARRAY:
            case V_INT8ARRAY:
            case V_FLOAT64ARRAY:
            case V_BINARYARRAY:
               return true;
            default:
               return false;
         }
      }

      bool isLessAccessible(ModuleScopeBase& scope, Visibility sourceVisibility, ref_t targetRef);

      void generateVirtualDispatchMethod(ModuleScopeBase& scope, ref_t parentRef, VirtualMethods& methods);

      CompilerLogic()
         : _operations({})
      {
         loadOperations();
      }

      static bool readAttributeMap(MemoryBase* section, ReferenceMap& map);
      static bool readTypeMap(ModuleBase* module, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope);

      static bool loadMetaData(ModuleScopeBase* moduleScope, ustr_t aliasName, ustr_t nsName);
      static bool clearMetaData(ModuleScopeBase* moduleScope, ustr_t name);

      static Visibility getVisibility(ustr_t name)
      {
         if (name.findStr(PRIVATE_PREFIX_NS) != NOTFOUND_POS)
            return Visibility::Private;

         if (name.findStr(INTERNAL_PREFIX_NS) != NOTFOUND_POS)
            return Visibility::Internal;

         return Visibility::Public;
      }

      static ustr_t getVisibilityPrefix(Visibility visibility)
      {
         switch (visibility) {
            case Visibility::Internal:
               return INTERNAL_PREFIX_NS;
            case Visibility::Private:
               return PRIVATE_PREFIX_NS;
            default:
               return "'";
         }
      }

      static ref_t loadClassInfo(ClassInfo& info, ModuleInfo& moduleInfo, 
         ModuleBase* target, bool headerOnly, bool fieldsOnly);

      static void importClassInfo(ClassInfo& copy, ClassInfo& target, ModuleBase* exporter, 
         ModuleBase* importer, bool headerOnly, bool inheritMode/*,bool ignoreFields*/);

      static CompilerLogic* getInstance()
      {
         static CompilerLogic instance;

         return &instance;
      }
   };

}

#endif // COMPILERLOGIC_H