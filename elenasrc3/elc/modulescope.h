//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Module scope class declaration.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MODULESCOPE_H
#define MODULESCOPE_H

#include "clicommon.h"

namespace elena_lang
{

// --- ModuleScope ---
class ModuleScope : public ModuleScopeBase
{
   LibraryLoaderBase*   loader;
   ForwardResolverBase* forwardResolver;

   void saveListMember(ustr_t name, ustr_t memberName);

public:
   bool isStandardOne() override;

   ref_t importSignature(ModuleBase* referenceModule, ref_t signRef) override;
   ref_t importMessage(ModuleBase* referenceModule, mssg_t message) override;
   ref_t importReference(ModuleBase* referenceModule, ustr_t referenceName) override;
   ref_t importReference(ModuleBase* referenceModule, ref_t reference) override
   {
      return importReference(referenceModule, referenceModule->resolveReference(reference));
   }
   ref_t importReferenceWithMask(ModuleBase* referenceModule, ref_t reference);
   ref_t importConstant(ModuleBase* referenceModule, ref_t reference);
   ref_t importMessageConstant(ModuleBase* referenceModule, ref_t reference);

   ref_t mapAnonymous(ustr_t prefix) override;

   ref_t mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
   ref_t mapTemplateIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility, bool& alreadyDeclared) override;

   ref_t mapFullReference(ustr_t referenceName, bool existing) override;
   ref_t mapWeakReference(ustr_t referenceName, bool existing) override;

   ExternalInfo mapExternal(ustr_t dllAlias, ustr_t functionName) override;

   ref_t resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
   ref_t resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs) override;

   ustr_t resolveWeakTemplateReference(ustr_t referenceName);

   ustr_t resolveFullName(ref_t reference) override
   {
      ustr_t referenceName = module->resolveReference(reference);
      if (isTemplateWeakReference(referenceName)) {
         return resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else return referenceName;
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

   void importClassInfo(ClassInfo& copy, ClassInfo& target, ModuleBase* exporter, bool headerOnly, bool inheritMode/*,
      bool ignoreFields*/) override;

   void newNamespace(ustr_t name) override;
   bool includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion) override;

   bool isDeclared(ref_t reference) override;

   ModuleScope(LibraryLoaderBase* loader, 
      ForwardResolverBase* forwardResolver, 
      ModuleBase* module,
      ModuleBase* debugModule,
      pos_t stackAlingment,
      pos_t rawStackAlingment,
      pos_t ehTableEntrySize,
      int minimalArgList)
      : ModuleScopeBase(module, debugModule, stackAlingment, rawStackAlingment, ehTableEntrySize, minimalArgList, false)
   {
      this->loader = loader;
      this->forwardResolver = forwardResolver;
   }
};

}

#endif