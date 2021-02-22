//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class scope implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compilerscope.h"

using namespace _ELENA_;

typedef ClassInfo::Attribute Attribute;

inline ref_t importAction(_Module* exporter, ref_t exportRef, _Module* importer)
{
   return getAction(importMessage(exporter, encodeAction(exportRef), importer));
}

inline void findUninqueName(_Module* module, IdentifierString& name)
{
   size_t pos = getlength(name);
   int   index = 0;
   ref_t ref = 0;
   do {
      name[pos] = 0;
      name.appendHex(index++);

      ref = module->mapReference(name.c_str(), true);
   } while (ref != 0);
}

// --- CompilerScope ---

ref_t ModuleScope :: mapAnonymous(ident_t prefix)
{
   // auto generate the name
   IdentifierString name("'", prefix, INLINE_CLASSNAME);

   findUninqueName(module, name);

   return module->mapReference(name);
}

void ModuleScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly, bool inheritMode, bool ignoreFields)
{
   target.header = copy.header;
   target.size = copy.size;

   if (!headerOnly) {
      // import method references and mark them as inherited if required (inherit mode)
      auto it = copy.methods.start();
      if (inheritMode) {
         while (!it.Eof()) {
            target.methods.add(importMessage(exporter, it.key(), module), false);

            it++;
         }
      }
      else {
         while (!it.Eof()) {
            target.methods.add(importMessage(exporter, it.key(), module), *it);

            it++;
         }
      }

      // import method attributes ignoring private methods in inherit mode
      auto mtype_it = copy.methodHints.start();
      while (!mtype_it.Eof()) {
         Attribute key = mtype_it.key();
         if (!inheritMode || key.value2 != maPrivate) {
            ref_t value = *mtype_it;
            if (test(key.value2, maActionMask)) {
               value = importAction(exporter, value, module);
            }
            else if (test(key.value2, maRefefernceMask)) {
               value = importReference(exporter, value, module);
            }
            else if (test(key.value2, maMessageMask))
               value = importMessage(exporter, value, module);

            target.methodHints.add(
               Attribute(importMessage(exporter, key.value1, module), key.value2),
               value);
         }

         mtype_it++;
      }

      // import static fields
      ClassInfo::StaticFieldMap::Iterator static_it = copy.statics.start();
      while (!static_it.Eof()) {
         ClassInfo::FieldInfo info(
            isSealedStaticField((*static_it).value1) ? importReference(exporter, (*static_it).value1, module) : (*static_it).value1,
            importReference(exporter, (*static_it).value2, module));

         target.statics.add(static_it.key(), info);

         static_it++;
      }

      // import static field values
      auto staticValue_it = copy.staticValues.start();
      while (!staticValue_it.Eof()) {
         ref_t val = *staticValue_it;
         if (val != mskStatRef) {
            val = importReference(exporter, (val & ~mskAnyRef), module) | (val & mskAnyRef);
         }

         target.staticValues.add(staticValue_it.key(), val);

         staticValue_it++;
      }

      // meta attributes are not inherited
      auto attribute_it = copy.mattributes.start();
      while (!attribute_it.Eof()) {
         auto key = attribute_it.key();
         if (key.value2 != 0)
            key.value2 = importMessage(exporter, key.value2, module);

         ref_t val = *attribute_it;
         if (test(key.value1, caRefefernceMask) && val != INVALID_REF) {
            val = importReference(exporter, (val & ~mskAnyRef), module) | (val & mskAnyRef);
         }

         target.mattributes.add(key, val);
         
         attribute_it++;
      }

      if (!ignoreFields) {
         target.fields.add(copy.fields);

         // import field types
         auto type_it = copy.fieldTypes.start();
         while (!type_it.Eof()) {
            ClassInfo::FieldInfo info = *type_it;
            info.value1 = importReference(exporter, info.value1, module);
            info.value2 = importReference(exporter, info.value2, module);

            target.fieldTypes.add(type_it.key(), info);

            type_it++;
         }
      }
   }
   // import class class reference
   if (target.header.classRef != 0)
      target.header.classRef = importReference(exporter, target.header.classRef, module);

   // import parent reference
   target.header.parentRef = importReference(exporter, target.header.parentRef, module);
}

ref_t ModuleScope :: loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol)
{
   _Module* argModule = NULL;

   if (emptystr(symbol))
      return 0;

   // load class meta data
   ref_t moduleRef = 0;
   if (isWeakReference(symbol)) {
      // if it is a weak reference - do not need to resolve the module
      argModule = module;
      moduleRef = module->mapReference(symbol);
   }
   else argModule = project->resolveModule(symbol, moduleRef, true);

   if (argModule == NULL || moduleRef == 0)
      return 0;

   // load argument VMT meta data
   _Memory* metaData = argModule->mapSection(moduleRef | mskMetaRDataRef, true);
   if (metaData == NULL || metaData->Length() != sizeof(SymbolExpressionInfo))
      return 0;

   MemoryReader reader(metaData);

   info.load(&reader);

   if (argModule != module) {
      // import type
      info.exprRef = importReference(argModule, info.exprRef, module);
      info.typeRef = importReference(argModule, info.typeRef, module);
   }
   return moduleRef;
}

ref_t ModuleScope :: resolveWeakTemplateReferenceID(ref_t reference)
{
   ident_t vmtName = module->resolveReference(reference);
   if (isTemplateWeakReference(vmtName)) {
      ident_t resolvedName = resolveWeakTemplateReference(vmtName + TEMPLATE_PREFIX_NS_LEN);

      if (NamespaceName::compare(resolvedName, module->Name())) {
         return module->mapReference(resolvedName + getlength(module->Name()));
      }
      else return module->mapReference(resolvedName);
   }
   else return reference;
}

ref_t ModuleScope :: loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly)
{
   _Module* argModule = NULL;

   if (emptystr(vmtName))
      return 0;

   if (isTemplateWeakReference(vmtName)) {
      // COMPILER MAGIC : try to find a template
      ref_t ref = loadClassInfo(info, resolveWeakTemplateReference(vmtName + TEMPLATE_PREFIX_NS_LEN), headerOnly);
      if (ref != 0 && info.header.classRef != 0) {
         if (module->resolveReference(info.header.classRef).endsWith(CLASSCLASS_POSTFIX)) {
            // HOTFIX : class class ref should be template weak reference as well
            IdentifierString classClassName(vmtName, CLASSCLASS_POSTFIX);

            info.header.classRef = module->mapReference(classClassName);
         }
      }

      return ref;
   }
   else {
      // load class meta data
      ref_t moduleRef = 0;
      if (isWeakReference(vmtName)) {
         // if it is a weak reference - do not need to resolve the module
         argModule = module;
         moduleRef = module->mapReference(vmtName);
      }
      else argModule = project->resolveModule(vmtName, moduleRef, true);

      if (argModule == NULL || moduleRef == 0)
         return 0;

      // load argument VMT meta data
      _Memory* metaData = argModule->mapSection(moduleRef | mskMetaRDataRef, true);
      if (metaData == NULL || metaData->Length() == sizeof(SymbolExpressionInfo))
         return 0;

      MemoryReader reader(metaData);

      if (argModule != module) {
         ClassInfo copy;
         copy.load(&reader, headerOnly);

         importClassInfo(copy, info, argModule, headerOnly, false, false);
      }
      else info.load(&reader, headerOnly);

      if (argModule != module) {
         // import reference
         importReference(argModule, moduleRef, module);
      }
      return moduleRef;
   }
}

inline ident_t getPrefix(Visibility visibility)
{
   if (visibility == Visibility::Private) {
      return PRIVATE_PREFIX_NS;
   }
   else if (visibility == Visibility::Internal) {
      return INTERNAL_PREFIX_NS;
   }
   else return "'";
}

inline ref_t mapNewIdentifier(_Module* module, ident_t identifier, Visibility visibility)
{
   ident_t prefix = getPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(name);
}

ref_t ModuleScope :: mapNewIdentifier(ident_t ns, ident_t identifier, Visibility visibility)
{
   if (!emptystr(ns)) {
      ReferenceNs nameWithNs(ns, identifier);

      return ::mapNewIdentifier(module, nameWithNs.c_str(), visibility);
   }
   else return ::mapNewIdentifier(module, identifier, visibility);
}

inline ref_t mapExistingIdentifier(_Module* module, ident_t identifier, Visibility visibility)
{
   ident_t prefix = getPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(name, true);
}

ref_t ModuleScope :: resolveImplicitIdentifier(ident_t ns, ident_t identifier, Visibility visibility)
{
   if (!emptystr(ns)) {
      ReferenceNs nameWithNs(ns, identifier);

      return ::mapExistingIdentifier(module, nameWithNs.c_str(), visibility);
   }
   else return ::mapExistingIdentifier(module, identifier, visibility);
}

ref_t ModuleScope :: mapFullReference(ident_t referenceName, bool existing)
{
   if (emptystr(referenceName))
      return 0;

   ref_t reference = 0;
   if (existing && !isTemplateWeakReference(referenceName)) {
      // check if the reference does exist
      ref_t moduleRef = 0;
      _Module* argModule = project->resolveModule(referenceName, moduleRef);
      if (argModule != NULL && moduleRef != 0) {
         if (argModule != module) {
            reference = module->mapReference(referenceName);
         }
         else reference = moduleRef;
      }
   }
   else reference = module->mapReference(referenceName, existing);

   return reference;
}

void ModuleScope :: saveAttribute(ident_t name, ref_t attr)
{
   if (attr) {
      ReferenceNs sectionName("'", ATTRIBUTE_SECTION);
      MemoryWriter metaWriter(module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(attr);
      metaWriter.writeLiteral(name);
   }
}

ref_t ModuleScope :: mapWeakReference(ident_t referenceName, bool existing)
{
   if (isTemplateWeakReference(referenceName)) {
      // COMPILER MAGIC : try to find a template implementation
      return module->mapReference(resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), existing);
   }
   else return module->mapReference(referenceName, existing);
}

_Module* ModuleScope :: loadReferenceModule(ident_t referenceName, ref_t& reference)
{
   if (isTemplateWeakReference(referenceName)) {
      // COMPILER MAGIC : try to find a template implementation
      return loadReferenceModule(resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN), reference);
   }
   else {
      if (isWeakReference(referenceName)) {
         reference = module->mapReference(referenceName, true);

         return reference ? module : NULL;
      }

      _Module* extModule = project->resolveModule(referenceName, reference);

      return reference ? extModule : NULL;
   }
}

ref_t ModuleScope :: mapTemplateClass(ident_t ns, ident_t templateName, bool& alreadyDeclared)
{
   ReferenceNs forwardName;
   // NOTE : the nested namespace is not included into the weak name
   forwardName.append(TEMPLATE_PREFIX_NS);

   forwardName.append(templateName);

   if (emptystr(project->resolveForward(templateName))) {
      ReferenceNs fullName(module->Name());
      if (!emptystr(ns))
         fullName.combine(ns);

      fullName.combine(templateName);

      project->addForward(templateName, fullName);

      mapNewIdentifier(ns, templateName, Visibility::Public);

      alreadyDeclared = false;
   }
   else alreadyDeclared = true;

   return module->mapReference(forwardName);
}

ident_t ModuleScope:: resolveWeakTemplateReference(ident_t referenceName)
{
   ident_t resolvedName = project->resolveForward(referenceName);
   if (emptystr(resolvedName)) {
      // COMPILER MAGIC : try to find a template implementation
      ref_t resolvedRef = 0;
      _Module* refModule = project->resolveWeakModule(referenceName, resolvedRef, true);
      if (refModule != nullptr) {
         resolvedName = refModule->resolveReference(resolvedRef);
         if (isWeakReference(resolvedName)) {
            IdentifierString fullName(refModule->Name(), resolvedName);

            project->addForward(referenceName, fullName.c_str());

            resolvedName = project->resolveForward(referenceName);
         }
         else project->addForward(referenceName, resolvedName);
      }
   }

   return resolvedName;
}

ref_t ModuleScope :: resolveImportedIdentifier(ident_t identifier, IdentifierList* importedNs)
{
   ref_t reference = 0;

   auto it = importedNs->start();
   while (!it.Eof()) {
      ReferenceNs fullName(*it, identifier);
   
      reference = 0;
      _Module* ext_module = project->resolveModule(fullName, reference, true);
      if (ext_module && reference) {
         if (ext_module != module) {
            return module->mapReference(fullName.c_str(), false);
         }
         else return reference;
      }
   
      it++;
   }

   return reference;
}

void ModuleScope :: compile(SyntaxTree& derivationTree, ident_t greeting, ExtensionMap* outerExtensionList)
{
   // declare classes / symbols based on the derivation tree
   bool repeatMode = true;
   bool idle = false;
   bool nothingToCompile = true;

   compiler->declareModuleIdentifiers(derivationTree, *this);

   while (repeatMode && !idle) {
      repeatMode = false;
      idle = !compiler->declareModule(derivationTree, *this, false, repeatMode, outerExtensionList);
      if (idle && repeatMode) {
         repeatMode = false;
         // if the last declaration was not successful, force it last time 
         idle = !compiler->declareModule(derivationTree, *this, true, repeatMode, outerExtensionList);
      }

      nothingToCompile &= idle;
   }
   
   if (!nothingToCompile) {
      // compile classes / symbols if not idle 
      compiler->compileModule(derivationTree, *this, greeting, outerExtensionList);
   }
}

void ModuleScope :: generateStatementCode(SyntaxWriter& output, ref_t reference, List<SNode>& parameters)
{
   SyntaxTree templateTree;

   TemplateGenerator transformer;
   SyntaxWriter writer(templateTree);
   writer.newNode(lxRoot);
   transformer.generateTemplateCode(writer, *this, reference, parameters);
   writer.closeNode();

   SyntaxTree::copyNode(output, templateTree.readRoot());
}

void ModuleScope :: importClassTemplate(SyntaxWriter& output, ref_t reference, List<SNode>& parameters)
{
   SyntaxTree templateTree;

   TemplateGenerator transformer;
   SyntaxWriter writer(templateTree);
   writer.newNode(lxRoot);
   transformer.generateTemplate(writer, *this, reference, parameters, false, true);
   writer.closeNode();

   transformer.importClass(output, templateTree.readRoot());
}

inline void copyTemplateSourceInfo(SyntaxWriter& writer, List<SNode>& parameters)
{
   for (auto it = parameters.start(); !it.Eof(); it++) {
      SNode node = *it;

      if (node == lxTemplateSource) {
         writer.newNode(lxTemplateSource);

         SyntaxTree::copyNode(writer, lxRow, node);
         SyntaxTree::copyNode(writer, lxCol, node);
         SyntaxTree::copyNode(writer, lxLength, node);

         SNode sourceNode = node.findChild(lxSourcePath);
         if (sourceNode != lxNone)
            writer.appendNode(lxSourcePath, sourceNode.identifier());

         writer.closeNode();
      }
   }
}

void ModuleScope :: generateTemplateProperty(SyntaxWriter& output, ref_t reference, List<SNode>& parameters, 
   int bookmark, bool inlineMode)
{
   SyntaxTree templateTree;

   TemplateGenerator transformer;
   SyntaxWriter writer(templateTree);
   writer.newNode(lxRoot);

   copyTemplateSourceInfo(writer, parameters);

   transformer.generateTemplateProperty(writer, *this, reference, parameters, bookmark, inlineMode);
   writer.closeNode();

   SyntaxTree::copyNode(output, templateTree.readRoot());
}

ref_t ModuleScope :: generateTemplate(ref_t reference, List<SNode>& parameters, ident_t ns, bool declarationMode, 
   ExtensionMap* outerExtensionList)
{
   SyntaxTree templateTree;

   TemplateGenerator transformer;
   SyntaxWriter writer(templateTree);
   writer.newNode(lxRoot);
   
   copyTemplateSourceInfo(writer, parameters);

   writer.newNode(lxNamespace, ns ? ns : "");

   ref_t generatedReference = 0;

   if (declarationMode) {
      generatedReference = transformer.declareTemplate(writer, *this, reference, parameters);

      writer.closeNode();
      writer.closeNode();
   }
   else {
      generatedReference = transformer.generateTemplate(writer, *this, reference, parameters, true, false);

      writer.closeNode();
      writer.closeNode();

      if (generatedReference) {
         IdentifierString path;
         path.copy("compiling ");
         path.append(resolveFullName(generatedReference));
         path.append(" template...");
         //writer.insertChild(0, lxSourcePath, path.c_str());

         try
         {
            compile(templateTree, path.c_str(), outerExtensionList);
         }
         catch (_Exception&)
         {
            return 0;
         }
      }
   }

   return generatedReference;
}

ref_t ModuleScope :: resolveClosure(ref_t closureMessage, ref_t outputRef, ident_t ns)
{
   ref_t signRef = 0;
   module->resolveAction(getAction(closureMessage), signRef);

   int paramCount = getArgCount(closureMessage);

   IdentifierString closureName(module->resolveReference(closureTemplateReference));
   if (signRef == 0) {
      if (paramCount > 0) {
         closureName.appendInt(paramCount);
      }

      if (isWeakReference(closureName)) {
         return module->mapReference(closureName, true);
      }
      else return mapFullReference(closureName, true);
   }
   else {   
      ref_t signatures[ARG_COUNT];
      size_t signLen = module->resolveSignature(signRef, signatures);

      List<SNode> parameters;
      SyntaxTree dummyTree;
      SyntaxWriter dummyWriter(dummyTree);
      dummyWriter.newNode(lxRoot);
      
      for (size_t i = 0; i < signLen; i++) {
         dummyWriter.appendNode(lxType, signatures[i]);
      }
      if (outputRef) {
         dummyWriter.appendNode(lxType, outputRef);
      }
      // if the output signature is not provided - use the super class
      else dummyWriter.appendNode(lxType, superReference);

      dummyWriter.closeNode();

      SNode paramNode = dummyTree.readRoot().firstChild();
      while (paramNode != lxNone) {
         parameters.add(paramNode);

         paramNode = paramNode.nextNode();
      }

      closureName.append('#');
      closureName.appendInt(paramCount + 1);

      ref_t templateReference = 0;
      if (isWeakReference(closureName)) {
         templateReference = module->mapReference(closureName, true);
      }
      else templateReference = mapFullReference(closureName, true);

      if (templateReference) {
         return generateTemplate(templateReference, parameters, ns, false, nullptr);
      }
      else return superReference;
   }
}

void ModuleScope :: saveListMember(ident_t name, ident_t memberName)
{
   // HOTFIX : do not include itself
   IdentifierString sectionName("'", name);

   _Memory* section = module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false);

   // check if the module alread included
   MemoryReader metaReader(section);
   while (!metaReader.Eof()) {
      ident_t s = metaReader.getLiteral(DEFAULT_STR);
      if (s.compare(memberName))
         return;
   }

   // otherwise add it to the list
   MemoryWriter metaWriter(section);

   metaWriter.writeLiteral(memberName.c_str());
}

void ModuleScope :: saveIncludedModule(_Module* extModule)
{
   // HOTFIX : do not include itself
   if (module == extModule)
      return;

   saveListMember(IMPORTS_SECTION, extModule->Name());
}

void ModuleScope :: declareNamespace(ident_t ns)
{
   IdentifierString virtualRef("'");
   if (!emptystr(ns)) {
      virtualRef.append(ns);
      virtualRef.append("'");
   }
   virtualRef.append(NAMESPACE_REF);

   module->mapReference(virtualRef.c_str(), false);
   if (debugModule)
      // HOTFIX : save the namespace in the debug module as well
      debugModule->mapReference(virtualRef.c_str(), false);

   if (!emptystr(ns))
      saveListMember(NAMESPACES_SECTION, ns);
}

bool ModuleScope :: includeNamespace(IdentifierList& importedNs, ident_t name, bool& duplicateInclusion)
{
   // check if the namespace exists
   ReferenceNs virtualRef(name, NAMESPACE_REF);
   ref_t dummyRef = 0;
   _Module* extModule = project->resolveModule(virtualRef, dummyRef, true);
   if (extModule && dummyRef) {
      ident_t value = retrieve(importedNs.start(), name, NULL);
      if (value == NULL) {
         importedNs.add(name.clone());

         saveIncludedModule(extModule);

         return true;
      }
      else duplicateInclusion = true;
   }
   return false;
}
