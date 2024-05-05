//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Module scope class implementation.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "modulescope.h"
#include "bytecode.h"
#include "compilerlogic.h"

using namespace elena_lang;

// --- ModuleScope ---

inline ref_t mapNewIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = CompilerLogic::getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name);
}

inline ref_t mapExistingIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = CompilerLogic::getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name, true);
}

bool ModuleScope :: isStandardOne()
{
   return test(hints, mhStandart);
}

bool ModuleScope :: withValidation()
{
   return !test(hints, mhNoValidation);
}

inline void findUninqueName(ModuleBase* module, IdentifierString& name)
{
   size_t pos = name.length();
   int   index = 0;
   ref_t ref = 0;
   do {
      name[pos] = 0;
      name.appendUInt(index++, 16);

      ref = module->mapReference(*name, true);
   } while (ref != 0);
}

ref_t ModuleScope :: mapAnonymous(ustr_t prefix)
{
   // auto generate the name
   IdentifierString name("'", prefix, INLINE_CLASSNAME);

   findUninqueName(module, name);

   return module->mapReference(*name);
}

ref_t ModuleScope :: mapTemplateIdentifier(ustr_t ns, ustr_t templateName, Visibility visibility, bool& alreadyDeclared, bool declarationMode)
{
   IdentifierString forwardName(TEMPLATE_PREFIX_NS, templateName);

   if (!declarationMode) {
      if (forwardResolver->resolveForward(templateName).empty()) {
         ReferenceName fullName(module->name());
         if (!ns.empty())
            fullName.combine(ns);

         fullName.combine(templateName);

         forwardResolver->addForward(templateName, *fullName);

         mapNewIdentifier(ns, templateName, visibility);

         alreadyDeclared = false;
      }
      else alreadyDeclared = true;
   }

   return module->mapReference(*forwardName);
}

ref_t ModuleScope :: mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   if (!ns.empty()) {
      ReferenceName fullName(ns, identifier);

      return ::mapNewIdentifier(module, *fullName, visibility);
   }
   else return ::mapNewIdentifier(module, identifier, visibility);
}

ref_t ModuleScope :: mapFullReference(ustr_t referenceName, bool existing)
{
   if (emptystr(referenceName))
      return 0;

   ref_t reference = 0;
   if (existing && !isTemplateWeakReference(referenceName)) {
      // check if the reference does exist
      auto moduleInfo = getModule(referenceName, true);
      if (moduleInfo.module != nullptr && moduleInfo.reference != 0) {
         if (moduleInfo.module != module) {
            reference = module->mapReference(referenceName);
         }
         else reference = moduleInfo.reference;
      }
   }
   else reference = module->mapReference(referenceName, existing);

   return reference;
}

ref_t ModuleScope :: mapWeakReference(ustr_t referenceName, bool existing)
{
   if (isTemplateWeakReference(referenceName)) {
      // COMPILER MAGIC : try to find a template implementation
      return mapFullReference(resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), existing);
   }
   else return mapFullReference(referenceName, existing);
}

ExternalInfo ModuleScope :: mapExternal(ustr_t dllAlias, ustr_t functionName)
{
   ExternalType type = ExternalType::Standard;

   ustr_t dllName = forwardResolver->resolveWinApi(dllAlias);
   if (!emptystr(dllName)) {
      type = ExternalType::WinApi;
   }
   else if (!dllAlias.compare(RT_FORWARD)) {
      dllName = forwardResolver->resolveExternal(dllAlias);
   }
   if (dllName.empty())
      dllName = dllAlias;

   ReferenceName referenceName(dllName);
   referenceName.append(".");
   referenceName.append(functionName);

   return { type, module->mapReference(*referenceName) };
}

ref_t ModuleScope :: resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   if (!ns.empty()) {
      ReferenceName fullName(ns, identifier);

      return ::mapExistingIdentifier(module, *fullName, visibility);
   }
   else return ::mapExistingIdentifier(module, identifier, visibility);
}

ref_t ModuleScope :: resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs)
{
   auto it = importedNs->start();
   while (!it.eof()) {
      ReferenceName fullName(*it, identifier);

      auto sectionInfo = loader->getModule(*fullName, true);
      if (sectionInfo.module && sectionInfo.reference) {
         if (sectionInfo.module != module) {
            return module->mapReference(*fullName, false);
         }
         else return sectionInfo.reference;
      }

      ++it;
   }

   return 0;
}

ref_t ModuleScope :: resolveWeakTemplateReferenceID(ref_t reference)
{
   ustr_t referenceName = module->resolveReference(reference);
   if (isTemplateWeakReference(referenceName)) {
      ustr_t resolvedTemplateReferenceName = resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN);
      if (resolvedTemplateReferenceName.empty())
         return reference;

      if (NamespaceString::compareNs(resolvedTemplateReferenceName, module->name(), module->name().length())) {
         return module->mapReference(resolvedTemplateReferenceName + getlength(module->name()));
      }
      else return module->mapReference(resolvedTemplateReferenceName);
   }
   else return reference;
}

ustr_t ModuleScope :: resolveWeakTemplateReference(ustr_t referenceName)
{
   ustr_t resolvedName = forwardResolver->resolveForward(referenceName);
   if (resolvedName.empty()) {
      // COMPILER MAGIC : try to find a template implementation
      auto resolved = getWeakModule(referenceName, true);
      if (resolved.module != nullptr) {
         resolvedName = resolved.module->resolveReference(resolved.reference);
         if (isWeakReference(resolvedName)) {
            IdentifierString fullName(resolved.module->name(), resolvedName);

            forwardResolver->addForward(referenceName, *fullName);
            resolvedName = forwardResolver->resolveForward(referenceName);
         }
         else forwardResolver->addForward(referenceName, resolvedName);
      }
   }

   return resolvedName;
}

SectionInfo ModuleScope :: getSection(ustr_t referenceName, ref_t mask, bool silentMode)
{
   if (isForwardReference(referenceName)) {
      referenceName = forwardResolver->resolveForward(referenceName + getlength(FORWARD_PREFIX_NS));
   }

   if (isWeakReference(referenceName)) {
      return loader->getSection(ReferenceInfo(module, referenceName), mask, 0, silentMode);
   }
   else return loader->getSection(ReferenceInfo(referenceName), mask, 0, silentMode);
}

MemoryBase* ModuleScope :: mapSection(ref_t reference, bool existing)
{
   ref_t mask = reference & mskAnyRef;

   return module->mapSection(
      mapWeakReference(
         module->resolveReference(reference & ~mskAnyRef), existing) | mask, existing);
}

ModuleInfo ModuleScope :: getModule(ustr_t referenceName, bool silentMode)
{
   if (isForwardReference(referenceName)) {
      referenceName = forwardResolver->resolveForward(referenceName + getlength(FORWARD_PREFIX_NS));
   }

   if (isWeakReference(referenceName)) {
      return loader->getModule(ReferenceInfo(module, referenceName), silentMode);
   }
   else return loader->getModule(ReferenceInfo(referenceName), silentMode);
}

ModuleInfo ModuleScope :: getWeakModule(ustr_t referenceName, bool silentMode)
{
   if (isForwardReference(referenceName)) {
      referenceName = forwardResolver->resolveForward(referenceName + getlength(FORWARD_PREFIX_NS));
   }

   return loader->getWeakModule(referenceName, silentMode);
}

ref_t ModuleScope :: loadSymbolInfo(SymbolInfo& info, ustr_t referenceName)
{
   if (referenceName.empty())
      return 0;

   ModuleInfo moduleInfo;
   if (isWeakReference(referenceName)) {
      // if it is a weak reference - do not need to resolve the module
      moduleInfo.module = module;
      moduleInfo.reference = module->mapReference(referenceName);
   }
   else moduleInfo = getModule(referenceName, true);

   if (moduleInfo.unassigned())
      return 0;

   // load argument VMT meta data
   MemoryBase* metaData = moduleInfo.module->mapSection(moduleInfo.reference | mskMetaSymbolInfoRef, true);
   if (!metaData)
      return 0;

   MemoryReader reader(metaData);
   if (moduleInfo.module != module) {
      SymbolInfo copy;
      copy.load(&reader);

      info.symbolType = copy.symbolType;
      info.typeRef = ImportHelper::importReference(moduleInfo.module, copy.typeRef, module);
      info.valueRef = ImportHelper::importReference(moduleInfo.module, copy.valueRef, module);
   }
   else info.load(&reader);

   return moduleInfo.reference;
}

ref_t ModuleScope :: loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly, bool fieldsOnly)
{
   if (referenceName.empty())
      return 0;

   if (isTemplateWeakReference(referenceName)) {
      // COMPILER MAGIC : try to find a template
      ref_t ref = loadClassInfo(info, resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), headerOnly, fieldsOnly);
      if (ref != 0 && info.header.classRef != 0) {
         if (module->resolveReference(info.header.classRef).endsWith(CLASSCLASS_POSTFIX)) {
            // HOTFIX : class class ref should be template weak reference as well
            IdentifierString classClassName(referenceName, CLASSCLASS_POSTFIX);

            info.header.classRef = module->mapReference(*classClassName);
         }
      }

      return ref;
   }
   else {
      ModuleInfo moduleInfo;
      if (isWeakReference(referenceName)) {
         // if it is a weak reference - do not need to resolve the module
         moduleInfo.module = module;
         moduleInfo.reference = module->mapReference(referenceName);
      }
      else moduleInfo = getModule(referenceName, true);

      return CompilerLogic::loadClassInfo(info, moduleInfo, module, headerOnly, fieldsOnly);
   }
}

void ModuleScope :: saveListMember(ustr_t name, ustr_t memberName)
{
   // HOTFIX : do not include itself
   IdentifierString sectionName("'", name);

   MemoryBase* section = module->mapSection(
      module->mapReference(*sectionName, false) | mskLiteralListRef,
      false);

   // check if the module alread included
   MemoryReader metaReader(section);
   while (!metaReader.eof()) {
      ustr_t s = metaReader.getString(DEFAULT_STR);
      if (s.compare(memberName))
         return;
   }

   // otherwise add it to the list
   MemoryWriter metaWriter(section);

   metaWriter.writeString(memberName);
}

void ModuleScope :: newNamespace(ustr_t ns)
{
   IdentifierString virtualRef("'");
   if (!ns.empty()) {
      virtualRef.append(ns);
      virtualRef.append("'");
   }
   virtualRef.append(NAMESPACE_REF);

   module->mapReference(*virtualRef, false);
   if (debugModule)
      // HOTFIX : save the namespace in the debug module as well
      debugModule->mapReference(*virtualRef, false);

   if (!ns.empty())
      saveListMember(NAMESPACES_SECTION, ns);
}

bool ModuleScope :: includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion)
{
   // check if the namespace exists
   ReferenceName virtualRef(name, NAMESPACE_REF);
   auto sectionInfo = loader->getModule(ReferenceInfo(module, *virtualRef), true);
   if (sectionInfo.module && sectionInfo.reference) {
      ustr_t value = importedNs.retrieve<ustr_t>(name, [](ustr_t name, ustr_t current)
         {
            return current == name;
         });
      if (value == nullptr) {
         importedNs.add(name.clone());

         if (sectionInfo.module != module)
            saveListMember(IMPORTS_SECTION, sectionInfo.module->name());

         return true;
      }
      else duplicateInclusion = true;
   }
   return false;
}

bool ModuleScope :: isDeclared(ref_t reference)
{
   return mapSection(reference | mskMetaClassInfoRef, true) != nullptr;
}

bool ModuleScope :: isSymbolDeclared(ref_t reference)
{
   return mapSection(reference | mskMetaSymbolInfoRef, true) != nullptr;
}

Visibility ModuleScope :: retrieveVisibility(ref_t reference)
{
   ustr_t referenceName = module->resolveReference(reference);

   return CompilerLogic::getVisibility(referenceName);
}
