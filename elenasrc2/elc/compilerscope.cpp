//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class scope implementation.
//
//                                              (C)2005-2018, by Alexei Rakov
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

inline ref_t importReference(_Module* exporter, ref_t exportRef, _Module* importer)
{
   //if (isPrimitiveRef(exportRef)) {
   //   return exportRef;
   //}
   /*else */if (exportRef) {
      ident_t reference = exporter->resolveReference(exportRef);

      return importer->mapReference(reference);
   }
   else return 0;
}

SNode findTerminalInfo(SNode node)
{
   if (node.existChild(lxRow))
      return node;

   SNode current = node.firstChild();
   while (current != lxNone) {
      SNode terminalNode = findTerminalInfo(current);
      if (terminalNode != lxNone)
         return terminalNode;

      current = current.nextNode();
   }

   return current;
}

// --- CompilerScope ---

void CompilerScope :: raiseError(const char* message, ident_t sourcePath, SNode node)
{
   SNode terminal = findTerminalInfo(node);
   
   int col = terminal.findChild(lxCol).argument;
   int row = terminal.findChild(lxRow).argument;
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.identifier();
   
   project->raiseError(message, sourcePath, row, col, identifier);
}

void CompilerScope :: raiseWarning(int level, const char* message, ident_t sourcePath, SNode node)
{
   SNode terminal = findTerminalInfo(node);

   int col = terminal.findChild(lxCol).argument;
   int row = terminal.findChild(lxRow).argument;
   ident_t identifier = terminal.identifier();
   //if (emptystr(identifier))
   //   identifier = terminal.findChild(lxTerminal).identifier();

   if (test(warningMask, level))
      project->raiseWarning(message, sourcePath, row, col, identifier);
}

void CompilerScope :: raiseWarning(int level, const char* message, ident_t sourcePath)
{
   if (test(warningMask, level))
      project->raiseWarning(message, sourcePath);
}

void CompilerScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly)
{
   target.header = copy.header;
   target.size = copy.size;

   if (!headerOnly) {
      // import method references and mark them as inherited
      ClassInfo::MethodMap::Iterator it = copy.methods.start();
      while (!it.Eof()) {
         target.methods.add(importMessage(exporter, it.key(), module), false);

         it++;
      }

      target.fields.add(copy.fields);

      // import field types
      ClassInfo::FieldTypeMap::Iterator type_it = copy.fieldTypes.start();
      while (!type_it.Eof()) {
         ClassInfo::FieldInfo info = *type_it;
         info.value1 = importReference(exporter, info.value1, module);
         info.value2 = importReference(exporter, info.value2, module);

         target.fieldTypes.add(type_it.key(), info);

         type_it++;
      }

      // import method attributes
      ClassInfo::MethodInfoMap::Iterator mtype_it = copy.methodHints.start();
      while (!mtype_it.Eof()) {
         Attribute key = mtype_it.key();
         ref_t value = *mtype_it;
         if (test(key.value2, maActionMask)) {
            value = importAction(exporter, value, module);
         }
         else if (test(key.value2, maRefefernceMask))
            value = importReference(exporter, value, module);

         target.methodHints.add(
            Attribute(importMessage(exporter, key.value1, module), key.value2),
            value);

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
   }
   // import class class reference
   if (target.header.classRef != 0)
      target.header.classRef = importReference(exporter, target.header.classRef, module);

   // import parent reference
   target.header.parentRef = importReference(exporter, target.header.parentRef, module);
}

ref_t CompilerScope :: loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly)
{
   _Module* argModule;

   if (emptystr(vmtName))
      return 0;

   if (isTemplateWeakReference(vmtName)) {
      // COMPILER MAGIC : try to find a template implementation
      return loadClassInfo(info, resolveWeakTemplateReference(vmtName + TEMPLATE_PREFIX_NS_LEN), headerOnly);
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

         importClassInfo(copy, info, argModule, headerOnly);
      }
      else info.load(&reader, headerOnly);

      if (argModule != module) {
         // import reference
         importReference(argModule, moduleRef, module);
      }
      return moduleRef;
   }
}

inline ref_t mapNewIdentifier(_Module* module, ident_t identifier, bool privateOne)
{
   ident_t prefix = privateOne ? PRIVATE_PREFIX_NS : "'";

   IdentifierString name(prefix, identifier);

   return module->mapReference(name);
}

ref_t CompilerScope :: mapNewIdentifier(ident_t ns, ident_t identifier, bool privateOne)
{
   if (!emptystr(ns)) {
      ReferenceNs nameWithNs(ns, identifier);

      return ::mapNewIdentifier(module, nameWithNs.c_str(), privateOne);

   }
   else return ::mapNewIdentifier(module, identifier, privateOne);
}

//ref_t CompilerScope :: mapNewTerminal(SNode terminal, bool privateOne)
//{
//   ident_t identifier = terminal.identifier();
//   switch (terminal) {
//      case lxIdentifier:
//      case lxReference:
//         return mapNewIdentifier(identifier, privateOne);
////         return mapIdentifier(identifier, existing);
//      //   return module->mapReference(identifier, false);
//      default:
//         return 0;
//   }
//   //ident_t identifier = terminal.identifier();
//   //if (terminal == lxIdentifier) {
//      //      ref_t reference = forwards.get(identifier);
//      //      if (reference == 0) {
//      //         if (!existing) {
//      //            ReferenceNs name(module->Name(), identifier);
//      //
//     // return module->mapReference(/*name*/identifier);
//      //         }
//      //         else return resolveImplicitIdentifier(identifier);
//      //      }
//      //      else return reference;
//   //}
//   ////   else if (terminal == lxPrivate) {
//   ////      ReferenceNs name(module->Name(), "#");
//   ////      name.append(identifier.c_str() + 1);
//   ////
//   ////      return mapReference(name, existing);
//   ////   }
//   //else return /*mapReference(identifier, existing)*/0;
//}

//ref_t CompilerScope :: mapIdentifier(ident_t identifier, bool existing)
//{
//   if (!existing) {
//      if (isWeakReference(identifier)) {
//         if (identifier.findLast('\'') == 0) {
//            return module->mapReference(identifier + 1);
//         }
//         else return module->mapReference(identifier);
//      }
//      else return module->mapReference(identifier);
//   }
//   else return 0; // !! temporal
//}

ref_t CompilerScope :: mapFullReference(ident_t referenceName, bool existing)
{
   if (emptystr(referenceName))
      return 0;

   ref_t reference = 0;
   if (existing) {
      // check if the reference does exist
      ref_t moduleRef = 0;
      _Module* argModule = project->resolveModule(referenceName, moduleRef);

      if (argModule != NULL && moduleRef != 0)
         reference = module->mapReference(referenceName);
   }
   else reference = module->mapReference(referenceName, existing);

   return reference;
}

void CompilerScope :: saveAttribute(ident_t name, ref_t attr)
{
   if (attr) {
      ReferenceNs sectionName("'", ATTRIBUTE_SECTION);
      MemoryWriter metaWriter(module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(attr);
      metaWriter.writeLiteral(name);
   }
}

_Module* CompilerScope :: loadReferenceModule(ident_t referenceName, ref_t& reference)
{
   if (isWeakReference(referenceName)) {
      reference = module->mapReference(referenceName, true);

      return reference ? module : NULL;
   }

   _Module* extModule = project->resolveModule(referenceName, reference);

   return reference ? extModule : NULL;
}

ref_t CompilerScope :: mapTemplateClass(ident_t ns, ident_t templateName, bool& alreadyDeclared)
{
   ReferenceNs forwardName;
   forwardName.append(TEMPLATE_PREFIX_NS);
   if (!emptystr(ns)) {
      forwardName.append(ns);
      forwardName.append('\'');
   }      

   forwardName.append(templateName);

   if (emptystr(project->resolveForward(templateName))) {
      ReferenceNs fullName(module->Name());
      if (!emptystr(ns))
         fullName.combine(ns);

      fullName.combine(templateName);

      project->addForward(templateName, fullName);

      mapNewIdentifier(ns, templateName, false);

      alreadyDeclared = false;
   }
   else alreadyDeclared = true;

   return module->mapReference(forwardName);
}

ident_t CompilerScope:: resolveWeakTemplateReference(ident_t referenceName)
{
   ident_t resolvedName = project->resolveForward(referenceName);
   if (emptystr(resolvedName)) {
      // COMPILER MAGIC : try to find a template implementation
      ref_t resolvedRef = 0;
      _Module* refModule = project->resolveWeakModule(referenceName, resolvedRef, true);
      if (refModule != nullptr) {
         resolvedName = refModule->resolveReference(resolvedRef);

         project->addForward(referenceName, resolvedName);
      }
   }

   return resolvedName;
}

inline ref_t resolveImplicitIdentifier(bool referenceOne, ident_t identifier, _Module* module, _ProjectManager* project, IdentifierList& importedNs)
{
   ref_t reference = 0;
   if (!referenceOne) {
      // check private ones
      IdentifierString privateOne(PRIVATE_PREFIX_NS, identifier);

      reference = module->mapReference(privateOne.c_str(), true);
      if (reference) {
         return reference;
      }

      // check imported references
      IdentifierString name("'", identifier);
      List<ident_t>::Iterator it = importedNs.start();
      while (!it.Eof()) {
         _Module* ext_module = project->loadModule(*it, true);
         if (ext_module) {
            reference = ext_module->mapReference(name.c_str(), true);
            if (reference) {
               if (ext_module != module) {
                  IdentifierString fullName(ext_module->Name(), name.c_str());

                  return module->mapReference(fullName.c_str(), false);
               }
               else return reference;
            }
         }

         it++;
      }

      return 0;
   }
   else {
      IdentifierString relativeName("'", identifier);

      return module->mapReference(relativeName.c_str(), true);
   }
}

ref_t CompilerScope :: resolveImplicitIdentifier(ident_t ns, ident_t identifier, bool referenceOne, IdentifierList& importedNs)
{
   ref_t reference = 0;
   if (isWeakReference(identifier)) {
      return module->mapReference(identifier, true);
   }
   else if (!emptystr(ns)) {
      // try to resovle an identifier in all nested namespaces
      ReferenceNs nameWithNs(ns, identifier);
      while (true) {
         reference = ::resolveImplicitIdentifier(referenceOne, nameWithNs.c_str(), module, project, importedNs);
         if (reference) {
            return reference;
         }
         nameWithNs.trimProperName();
         if (!emptystr(nameWithNs)) {
            nameWithNs.trimProperName();
            if (emptystr(nameWithNs)) {
               nameWithNs.copy(identifier);
            }
            else nameWithNs.combine(identifier);
         }
         else break;
      }

      return 0;
   }
   else {
      // try to resovle an identifier in the current namespace
      return ::resolveImplicitIdentifier(referenceOne, identifier, module, project, importedNs);
   }   
}
