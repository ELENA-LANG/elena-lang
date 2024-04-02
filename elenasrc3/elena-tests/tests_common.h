//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Common declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include "compiler.h"

namespace elena_lang 
{
   // --- TestModuleScope ---
   class TestModuleScope : public ModuleScopeBase
   {
   public:
      bool isStandardOne() override;
      bool withValidation() override;

      ref_t mapAnonymous(ustr_t prefix) override;

      ref_t mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
      ref_t mapTemplateIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility, bool& alreadyDeclared,
         bool declarationMode) override;

      ref_t mapFullReference(ustr_t referenceName, bool existing) override;
      ref_t mapWeakReference(ustr_t referenceName, bool existing) override;

      ExternalInfo mapExternal(ustr_t dllAlias, ustr_t functionName) override;

      ref_t resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
      ref_t resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs) override;

      ref_t resolveWeakTemplateReferenceID(ref_t reference) override;

      ustr_t resolveFullName(ref_t reference) override
      {
         ustr_t referenceName = module->resolveReference(reference);
         /*if (isTemplateWeakReference(referenceName)) {
            return resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN);
         }
         else */return referenceName;
      }

      SectionInfo getSection(ustr_t referenceName, ref_t mask, bool silentMode) override;
      MemoryBase* mapSection(ref_t reference, bool existing) override;

      ModuleInfo getModule(ustr_t referenceName, bool silentMode) override;
      ModuleInfo getWeakModule(ustr_t referenceName, bool silentMode) override;

      ref_t loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly, bool fieldsOnly) override;
      ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly, bool fieldsOnly) override
      {
         return loadClassInfo(info, module->resolveReference(reference), headerOnly, fieldsOnly);
      }

      ref_t loadSymbolInfo(SymbolInfo& info, ustr_t referenceName) override;
      ref_t loadSymbolInfo(SymbolInfo& info, ref_t reference) override
      {
         return loadSymbolInfo(info, module->resolveReference(reference));
      }

      void newNamespace(ustr_t name) override;
      bool includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion) override;

      bool isDeclared(ref_t reference) override;

      Visibility retrieveVisibility(ref_t reference) override;

      TestModuleScope(bool tapeOptMode, bool threadFriendly);
   };

   class TestTemplateProssesor : public TemplateProssesorBase
   {
   public:
      ref_t generateClassTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t templateRef,
         List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList) override;

      bool importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;

      static TemplateProssesorBase* getInstance()
      {
         static TestTemplateProssesor instance;

         return &instance;
      }
   };

   // --- CompilerEnvironment ---
   class CompilerEnvironment
   {
   public:
      ModuleScopeBase* createModuleScope(bool tapeOptMode, bool threadFriendly);

      Compiler* createCompiler();

      CompilerEnvironment();
   };
}

#endif // TESTS_COMMON_H