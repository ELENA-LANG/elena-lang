//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Module scope class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "modulescope.h"

using namespace elena_lang;

// --- ModuleScope ---

inline ustr_t getVisibilityPrefix(Visibility visibility)
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

inline ref_t mapNewIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name);
}

inline ref_t mapExistingIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name, true);
}

bool ModuleScope :: isStandardOne()
{
   return module->name().compare(STANDARD_MODULE) || module->name().compare(PREDEFINED_MODULE);
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
      return module->mapReference(resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), existing);
   }
   else return module->mapReference(referenceName, existing);
}

ExternalInfo ModuleScope :: mapExternal(ustr_t dllAlias, ustr_t functionName)
{
   ExternalType type = ExternalType::Standard;

   ustr_t dllName = forwardResolver->resolveWinApi(dllAlias);
   if (!emptystr(dllName)) {
      type = ExternalType::WinApi;
   }
   else dllName = forwardResolver->resolveExternal(dllAlias);
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
   ref_t reference = 0;

   auto it = importedNs->start();
   while (!it.eof()) {
      ReferenceName fullName(*it, identifier);

      auto sectionInfo = loader->getModule(*fullName, true);
      if (sectionInfo.module && sectionInfo.reference) {
         if (sectionInfo.module != module) {
            return module->mapReference(*fullName, false);
         }
         else return reference;
      }

      ++it;
   }

   return reference;
}

ustr_t ModuleScope :: resolveWeakTemplateReference(ustr_t referenceName)
{
   ustr_t resolvedName = forwardResolver->resolveForward(referenceName);
   if (referenceName.empty()) {
      // COMPILER MAGIC : try to find a template implementation
      auto resolved = getWeakModule(referenceName, true);
      if (resolved.module != nullptr) {
         resolvedName = resolved.module->resolveReference(resolved.reference);
         if (isWeakReference(resolvedName)) {
            IdentifierString fullName(resolved.module->name(), resolvedName);

            forwardResolver->addForward(referenceName, *fullName);
            //resolvedName = forwardResolver->resolveForward(referenceName);
         }
         else forwardResolver->addForward(referenceName, resolvedName);
      }
   }

   return resolvedName;
}

ref_t ModuleScope :: importSignature(ModuleBase* referenceModule, ref_t signRef)
{
   if (!signRef)
      return 0;

   ref_t dump[ARG_COUNT];
   size_t len = referenceModule->resolveSignature(signRef, dump);
   for (size_t i = 0; i < len; i++) {
      dump[i] = importReference(referenceModule, dump[i]);
   }

   return module->mapSignature(dump, len, false);
}

ref_t ModuleScope :: importMessage(ModuleBase* referenceModule, mssg_t exportRef)
{
   if (!exportRef)
      return 0;

   pos_t paramCount = 0;
   ref_t actionRef, flags;
   decodeMessage(exportRef, actionRef, paramCount, flags);

   // signature and custom verb should be imported
   ref_t signature = 0;
   ustr_t actionName = referenceModule->resolveAction(actionRef, signature);

   actionRef = module->mapAction(actionName, importSignature(referenceModule, signature), false);

   return encodeMessage(actionRef, paramCount, flags);
}

ref_t ModuleScope :: importReference(ModuleBase* referenceModule, ustr_t referenceName)
{
   auto info = loader->getModule(ReferenceInfo(referenceModule, referenceName), true);
   if (info.reference) {
      referenceName = info.module->resolveReference(info.reference);
      if (isWeakReference(referenceName) && !isTemplateWeakReference(referenceName)) {
         IdentifierString fullName(info.module->name(), referenceName);

         return module->mapReference(*fullName);
      }
      else return module->mapReference(referenceName);
   }
   else return 0;
}

SectionInfo ModuleScope :: getSection(ustr_t referenceName, ref_t mask, bool silentMode)
{
   if (isForwardReference(referenceName)) {
      referenceName = forwardResolver->resolveForward(referenceName);
   }

   if (isWeakReference(referenceName)) {
      return loader->getSection(ReferenceInfo(module, referenceName), mask, silentMode);
   }
   else return loader->getSection(ReferenceInfo(referenceName), mask, silentMode);
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
      referenceName = forwardResolver->resolveForward(referenceName);
   }

   if (isWeakReference(referenceName)) {
      return loader->getModule(ReferenceInfo(module, referenceName), silentMode);
   }
   else return loader->getModule(ReferenceInfo(referenceName), silentMode);
}

ModuleInfo ModuleScope :: getWeakModule(ustr_t referenceName, bool silentMode)
{
   if (isForwardReference(referenceName)) {
      referenceName = forwardResolver->resolveForward(referenceName);
   }

   return loader->getWeakModule(referenceName, silentMode);
}

ref_t ModuleScope :: loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly)
{
   if (referenceName.empty())
      return 0;

   if (isTemplateWeakReference(referenceName)) {
      // COMPILER MAGIC : try to find a template
      ref_t ref = loadClassInfo(info, resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), headerOnly);
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

      if (moduleInfo.unassigned())
         return 0;

      // load argument VMT meta data
      MemoryBase* metaData = moduleInfo.module->mapSection(moduleInfo.reference | mskMetaClassInfoRef, true);
      if (!metaData)
         return 0;

      MemoryReader reader(metaData);
      if (moduleInfo.module != module) {
         ClassInfo copy;
         copy.load(&reader, headerOnly);

         importClassInfo(copy, info, moduleInfo.module, headerOnly, false/*, false*/);

         // import reference
         importReference(moduleInfo.module, moduleInfo.reference);
      }
      else info.load(&reader, headerOnly);

      return moduleInfo.reference;
   }
}

void ModuleScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, ModuleBase* exporter, bool headerOnly, bool inheritMode)
{
   target.header = copy.header;
   target.size = copy.size;

   if (!headerOnly) {
      // import method references and mark them as inherited if required (inherit mode)
      for (auto it = copy.methods.start(); !it.eof(); ++it) {
         MethodInfo info = *it;

         if (inheritMode)
            info.inherited = true;

         if (info.outputRef)
            info.outputRef = importReference(exporter, info.outputRef);

         target.methods.add(importMessage(exporter, it.key()), info);
      }

      for (auto it = copy.fields.start(); !it.eof(); ++it) {
         FieldInfo info = *it;

         if (info.typeRef && !isPrimitiveRef(info.typeRef))
            info.typeRef = importReference(exporter, info.typeRef);

         target.fields.add(it.key(), info);
      }
   }

   // import class class reference
   if (target.header.classRef != 0)
      target.header.classRef = importReference(exporter, target.header.classRef);

   // import parent reference
   if (target.header.parentRef)
      target.header.parentRef = importReference(exporter, target.header.parentRef);

}

void ModuleScope :: saveListMember(ustr_t name, ustr_t memberName)
{
   // HOTFIX : do not include itself
   IdentifierString sectionName("'", name);

   MemoryBase* section = module->mapSection(
      module->mapReference(*sectionName, false) | mskStrMetaArrayRef, 
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

   if (!ns.empty())
      saveListMember(NAMESPACES_SECTION, ns);
}

bool ModuleScope :: includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion)
{
   // check if the namespace exists
   ReferenceName virtualRef(name, NAMESPACE_REF);
   ref_t dummyRef = 0;

   auto sectionInfo = loader->getModule(ReferenceInfo(module, *virtualRef), true);
   if (sectionInfo.module && sectionInfo.reference && sectionInfo.module != module) {
      ustr_t value = importedNs.retrieve<ustr_t>(name, [](ustr_t name, ustr_t current)
         {
            return current == name;
         });
      if (value == nullptr) {
         importedNs.add(name.clone());

         saveListMember(IMPORTS_SECTION, sectionInfo.module->name());

         return true;
      }
      else duplicateInclusion = true;
   }
   return false;
}
