//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler logic class.
//
//                                             (C)2021-2022, by Aleksey Rakov
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
   };

   // --- CompilerLogic ---
   class CompilerLogic
   {
      ref_t generateOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags, ClassInfo::MethodMap& methods, 
         mssg_t message, void* param, ref_t(*resolve)(void*, ref_t));

      bool isSignatureCompatible(ModuleScopeBase& scope, ref_t targetSignature, ref_t* sourceSignatures, size_t sourceLen);

   public:
      BuildKey resolveOp(ModuleScopeBase& scope, int operatorId, ref_t* arguments, size_t length, ref_t& outputRef, bool& needToAlloc);

      bool defineClassInfo(ModuleScopeBase& scope, ClassInfo& info, ref_t reference, bool headerOnly = false, bool fieldsOnly = false);

      SizeInfo defineStructSize(ClassInfo& info);
      SizeInfo defineStructSize(ModuleScopeBase& scope, ref_t reference);
      ref_t definePrimitiveArray(ModuleScopeBase& scope, ref_t elementRef, bool structOne);

      bool validateTemplateAttribute(ref_t attribute, Visibility& visibility, TemplateType& type);
      bool validateSymbolAttribute(ref_t attribute, Visibility& visibility, bool& constant, bool& isStatic);
      bool validateClassAttribute(ref_t attribute, ref_t& flags, Visibility& visibility);
      bool validateFieldAttribute(ref_t attribute, FieldAttributes& attrs);
      bool validateMethodAttribute(ref_t attribute, ref_t& hint, bool& explicitMode);
      bool validateImplicitMethodAttribute(ref_t attribute, ref_t& hint);
      bool validateDictionaryAttribute(ref_t attribute, ref_t& dictionaryType);
      bool validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attrs);

      bool isRole(ClassInfo& info);

      bool isEmbeddableArray(ClassInfo& info);
      bool isEmbeddableArray(ModuleScopeBase& scope, ref_t reference);

      bool isEmbeddableStruct(ClassInfo& info);
      bool isMultiMethod(ClassInfo& info, MethodInfo& methodInfo);

      bool isValidOp(int operatorId, const int* validOperators, size_t len);

      void tweakClassFlags(ref_t classRef, ClassInfo& info, bool classClassMode);
      void tweakPrimitiveClassFlags(ClassInfo& info, ref_t classRef);

      bool validateMessage(ModuleScopeBase& scope, ref_t hints, mssg_t message);
      void validateClassDeclaration(ModuleScopeBase& scope, ClassInfo& info,
         bool& emptyStructure, bool& disptacherNotAllowed);

      void writeDictionaryEntry(MemoryBase* section, ustr_t key, int value);
      bool readDictionary(MemoryBase* section, ReferenceMap& map);

      void writeArrayEntry(MemoryBase* section, ref_t reference);

      void writeAttrDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference);
      bool readAttrDictionary(ModuleBase* module, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope);

      void writeDeclDictionaryEntry(MemoryBase* section, ustr_t key, ref_t reference);
      bool readDeclDictionary(ModuleBase* module, MemoryBase* section, ReferenceMap& map, ModuleScopeBase* scope);

      void writeExtMessageEntry(MemoryBase* section, ref_t extRef, mssg_t message, mssg_t strongMessage);
      bool readExtMessageEntry(ModuleBase* module, MemoryBase* section, ExtensionMap& map, ModuleScopeBase* scope);

      bool isCompatible(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef, bool ignoreNils);

      bool isSignatureCompatible(ModuleScopeBase& scope, mssg_t targetMessage, mssg_t sourceMessage);
      bool isMessageCompatibleWithSignature(ModuleScopeBase& scope, mssg_t targetMessage,
         ref_t* sourceSignature, size_t len);

      ConversionRoutine retrieveConversionRoutine(ModuleScopeBase& scope, ref_t targetRef, ref_t sourceRef);

      bool checkMethod(ClassInfo& info, mssg_t message, CheckMethodResult& result);
      bool checkMethod(ModuleScopeBase& scope, ref_t reference, mssg_t message, CheckMethodResult& result);

      bool resolveCallType(ModuleScopeBase& scope, ref_t classRef, mssg_t message, 
         CheckMethodResult& result);

      void injectOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ClassInfo& info, ref_t classRef);
      void injectMethodOverloadList(CompilerBase* compiler, ModuleScopeBase& scope, ref_t flags, 
         mssg_t message, ClassInfo::MethodMap& methods, ClassAttributes& attributes,
         void* param, ref_t(*resolve)(void*, ref_t));

      void verifyMultimethods();

      static CompilerLogic* getInstance()
      {
         static CompilerLogic instance;

         return &instance;
      }
   };

}

#endif // COMPILERLOGIC_H