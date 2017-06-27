//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiler.h"
#include "errors.h"
//#include <errno.h>

using namespace _ELENA_;

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- Hint constants ---
#define HINT_CLOSURE_MASK     0xC0008800

#define HINT_ROOT             0x80000000
#define HINT_ROOTSYMBOL       0xC0000000
#define HINT_NOBOXING         0x20000000
#define HINT_NOUNBOXING       0x10000000
#define HINT_EXTERNALOP       0x08000000
#define HINT_NOCONDBOXING     0x04000000
#define HINT_EXTENSION_MODE   0x02000000
#define HINT_TRY_MODE         0x01000000
#define HINT_LOOP             0x00800000
#define HINT_SWITCH           0x00400000
#define HINT_ALT_MODE         0x00200000
#define HINT_SINGLETON        0x00100000
#define HINT_NODEBUGINFO      0x00020000
//#define HINT_CLOSURE          0x00008000
#define HINT_SUBCODE_CLOSURE  0x00008800
#define HINT_RESENDEXPR       0x00000400
#define HINT_LAZY_EXPR        0x00000200
#define HINT_DYNAMIC_OBJECT   0x00000100  // indicates that the structure MUST be boxed

typedef Compiler::ObjectInfo ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef ClassInfo::Attribute Attribute;

// --- Auxiliary routines ---

inline bool isCollection(SNode node)
{
   return (node == lxExpression && node.nextNode() == lxExpression);
}

inline bool isPrimitiveRef(ref_t reference)
{
   return (int)reference < 0;
}

inline ref_t importMessage(_Module* exporter, ref_t exportRef, _Module* importer)
{
   ref_t verbId = 0;
   ref_t signRef = 0;
   int paramCount = 0;

   decodeMessage(exportRef, signRef, verbId, paramCount);

   // if it is generic message
   if (signRef == 0) {
      return exportRef;
   }

   // otherwise signature and custom verb should be imported
   if (signRef != 0) {
      ident_t subject = exporter->resolveSubject(signRef);

      signRef = importer->mapSubject(subject, false);
   }
   return encodeMessage(signRef, verbId, paramCount);
}

inline ref_t importSubject(_Module* exporter, ref_t exportRef, _Module* importer)
{
   // otherwise signature and custom verb should be imported
   if (exportRef != 0) {
      ident_t subject = exporter->resolveSubject(exportRef);

      exportRef = importer->mapSubject(subject, false);
   }
   return exportRef;
}

inline ref_t importReference(_Module* exporter, ref_t exportRef, _Module* importer)
{
   if (isPrimitiveRef(exportRef)) {
      return exportRef;
   }
   else if (exportRef) {
      ident_t reference = exporter->resolveReference(exportRef);

      return importer->mapReference(reference);
   }
   else return 0;
}

inline void findUninqueName(_Module* module, ReferenceNs& name)
{
   size_t pos = getlength(name);
   int   index = 0;
   ref_t ref = 0;
   do {
      name[pos] = 0;
      name.appendHex(index++);

      ref = module->mapReference(name, true);
   } while (ref != 0);
}

//inline void findUninqueSubject(_Module* module, ReferenceNs& name)
//{
//   size_t pos = getlength(name);
//   int   index = 0;
//   ref_t ref = 0;
//   do {
//      name[pos] = 0;
//      name.appendHex(index++);
//
//      ref = module->mapSubject(name, true);
//   } while (ref != 0);
//}

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

inline SNode findParent(SNode node, LexicalType type)
{
   while (node.type != type && node != lxNone) {
      node = node.parentNode();
   }

   return node;
}

inline bool isImportRedirect(SNode node)
{
   SNode terminal = node.firstChild(lxObjectMask);
   if (terminal == lxReference) {
      if (terminal.identifier().compare(INTERNAL_MASK, INTERNAL_MASK_LEN))
         return true;
   }
   return false;
}

inline void copyIdentifier(SyntaxWriter& writer, SNode ident)
{
   if (emptystr(ident.identifier())) {
      SNode terminal = ident.findChild(lxTerminal);
      if (terminal != lxNone) {
         writer.newNode(ident.type, terminal.identifier());
      }
      else writer.newNode(ident.type);
   }
   else writer.newNode(ident.type, ident.identifier());

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);
   writer.closeNode();
}

inline void copyOperator(SyntaxWriter& writer, SNode ident)
{
   if (emptystr(ident.identifier())) {
      SNode terminal = ident.findChild(lxTerminal);
      if (terminal != lxNone) {
         writer.newNode(lxOperator, terminal.identifier());
      }
      else writer.newNode(ident.type);
   }
   else writer.newNode(lxOperator, ident.identifier());

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);
   writer.closeNode();
}

inline int readSizeValue(SNode node, int radix)
{
   ident_t val = node.identifier();
   if (emptystr(val))
      val = node.findChild(lxTerminal).identifier();

   return val.toLong(radix);
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

// --- Compiler::ModuleScope ---

Compiler::ModuleScope :: ModuleScope(_ProjectManager* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints(INVALID_REF), extensions(NULL, freeobj)
{
   this->project = project;
   this->sourcePath = sourcePath;
   this->module = module;
   this->debugModule = debugModule;
   this->sourcePathRef = INVALID_REF;

   this->forwardsUnresolved = forwardsUnresolved;

   warnOnUnresolved = project->WarnOnUnresolved();
   warnOnWeakUnresolved = project->WarnOnWeakUnresolved();
   warningMask = project->getWarningMask();

   // cache the frequently used references
   superReference = mapReference(project->resolveForward(SUPER_FORWARD));
   intReference = mapReference(project->resolveForward(INT_FORWARD));
   longReference = mapReference(project->resolveForward(LONG_FORWARD));
   realReference = mapReference(project->resolveForward(REAL_FORWARD));
   literalReference = mapReference(project->resolveForward(STR_FORWARD));
   wideReference = mapReference(project->resolveForward(WIDESTR_FORWARD));
   charReference = mapReference(project->resolveForward(CHAR_FORWARD));
   signatureReference = mapReference(project->resolveForward(SIGNATURE_FORWARD));
   messageReference = mapReference(project->resolveForward(MESSAGE_FORWARD));
   verbReference = mapReference(project->resolveForward(VERB_FORWARD));
   arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));
   boolReference = mapReference(project->resolveForward(BOOL_FORWARD));

   paramsSubj = module->mapSubject(project->resolveForward(PARAMS_SUBJFORWARD), false);

   // HOTFIX : package section should be created if at least literal class is declated
   if (literalReference != 0) {
      packageReference = module->mapReference(ReferenceNs(module->Name(), PACKAGE_SECTION));
   }
   else packageReference = 0;

   defaultNs.add(module->Name());

   loadModuleInfo(module);

   // system module should be included by default
   if (!module->Name().compare(STANDARD_MODULE)) {
      defaultNs.add(STANDARD_MODULE);
      loadModuleInfo(project->loadModule(STANDARD_MODULE, true));
   }      
}

ref_t Compiler::ModuleScope :: getBaseLazyExpressionClass()
{
   return mapReference(project->resolveForward(LAZYEXPR_FORWARD));
}

ObjectInfo Compiler::ModuleScope :: mapObject(SNode identifier)
{
   ident_t terminal = identifier.identifier();

   if (identifier==lxReference) {
      return mapReferenceInfo(terminal, false);
   }
   else if (identifier==lxPrivate) {
      if (terminal.compare(NIL_VAR)) {
         return ObjectInfo(okNil);
      }
      else return defineObjectInfo(mapTerminal(identifier, true), true);
   }
   else if (identifier==lxIdentifier) {
      return defineObjectInfo(mapTerminal(identifier, true), true);
   }
   else return ObjectInfo();
}

ref_t Compiler::ModuleScope :: resolveIdentifier(ident_t identifier)
{
   List<ident_t>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      ReferenceNs name(*it, identifier);

      if (checkReference(name))
         return module->mapReference(name);

      it++;
   }
   return 0;
}

ref_t Compiler::ModuleScope :: mapNewSubject(ident_t terminal)
{
   IdentifierString fullName(terminal);
   fullName.append('$');

   ident_t ns = module->Name();
   if (ns.compare(STANDARD_MODULE)) {
   }
   else if (ns.compare(STANDARD_MODULE, STANDARD_MODULE_LEN)) {
      fullName.append(ns + STANDARD_MODULE_LEN + 1);
   }
   else fullName.append(ns);

   return module->mapSubject(fullName, false);
}

ref_t Compiler::ModuleScope :: resolveAttributeRef(ident_t identifier, bool explicitOnly)
{
   ref_t subj_ref = attributes.get(identifier);
   if (subj_ref != 0)
      return subj_ref;

   IdentifierString fullName(identifier);
   fullName.append('$');

   size_t tail = fullName.Length();
   List<ident_t>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      fullName.truncate(tail);

      // if it is a sytem root
      if ((*it).compare(STANDARD_MODULE)) {
      }
      else if ((*it).compare(STANDARD_MODULE, STANDARD_MODULE_LEN)) {
         fullName.append(*it + STANDARD_MODULE_LEN + 1);
      }
      else fullName.append(*it);

      subj_ref = module->mapSubject(fullName, true);
      if (subj_ref && (!explicitOnly || subjectHints.exist(subj_ref))) {
         attributes.add(identifier, subj_ref);

         return subj_ref;
      }
      it++;
   }

   return 0;
}

ref_t Compiler::ModuleScope :: mapSubject(SNode terminal, bool explicitOnly)
{
   ident_t identifier = NULL;
   if (terminal.type == lxIdentifier || terminal.type == lxPrivate || terminal.type == lxMessage/* || terminal.type == lxAttribute*/) {
      identifier = terminal.identifier();
      if (emptystr(identifier))
         identifier = terminal.findChild(lxTerminal).identifier();
   }
   else if (terminal.type == lxReference) {
      identifier = terminal.identifier();
      if (emptystr(identifier))
         identifier = terminal.findChild(lxTerminal).identifier();

      return module->mapSubject(identifier, explicitOnly);
   }
   else raiseError(errInvalidSubject, terminal);

   return resolveAttributeRef(identifier, explicitOnly);
}

ref_t Compiler::ModuleScope :: mapSubject(SNode terminal, IdentifierString& output)
{
   ident_t identifier = terminal.identifier();

   // add a namespace for the private message
   if (terminal.type == lxPrivate) {
      output.append(project->Namespace());
      output.append(identifier);

      return 0;
   }

   ref_t subjRef = mapSubject(terminal);
   if (subjRef != 0) {
      output.append(module->resolveSubject(subjRef));
   }
   else if (terminal.type != lxReference) {
      output.append(identifier);
   }
   else raiseError(errInvalidSubject, terminal);

   return subjRef;
}

ref_t Compiler::ModuleScope :: mapTerminal(SNode terminal, bool existing)
{
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   if (terminal == lxIdentifier) {
      ref_t reference = forwards.get(identifier);
      if (reference == 0) {
         if (!existing) {
            ReferenceNs name(module->Name(), identifier);

            return module->mapReference(name);
         }
         else return resolveIdentifier(identifier);
      }
      else return reference;
   }
   else if (terminal == lxPrivate) {
      ReferenceNs name(module->Name(), identifier);

      return mapReference(name, existing);
   }
   else return mapReference(identifier, existing);
}

ref_t Compiler::ModuleScope :: mapNestedExpression()
{
   // otherwise auto generate the name
   ReferenceNs name(module->Name(), INLINE_POSTFIX);

   findUninqueName(module, name);

   return module->mapReference(name);
}

bool Compiler::ModuleScope :: checkReference(ident_t referenceName)
{
   ref_t moduleRef = 0;
   _Module* module = project->resolveModule(referenceName, moduleRef, true);

   if (module == NULL || moduleRef == 0)
      return false;

   return module->mapReference(referenceName, true) != 0;
}

ObjectInfo Compiler::ModuleScope :: defineObjectInfo(ref_t reference, bool checkState)
{
   // if reference is zero the symbol is unknown
   if (reference == 0) {
      return ObjectInfo();
   }
   // check if symbol should be treated like constant one
   else if (constantHints.exist(reference)) {
      return ObjectInfo(okConstantSymbol, reference, constantHints.get(reference));
   }
   else if (checkState) {
      ClassInfo info;
      // check if the object can be treated like a constant object
      ref_t r = loadClassInfo(info, module->resolveReference(reference), true);
      if (r) {
         // if it is a stateless symbol
         if (test(info.header.flags, elStateless)) {
            return ObjectInfo(okConstantSymbol, reference, reference);
         }
         // if it is a normal class
         // then the symbol is reference to the class class
         else if (test(info.header.flags, elStandartVMT) && info.header.classRef != 0) {
            return ObjectInfo(okConstantClass, reference, info.header.classRef);
         }
      }
      else {
         // check if the object is typed expression
         SymbolExpressionInfo symbolInfo;
         // check if the object can be treated like a constant object
         r = loadSymbolExpressionInfo(symbolInfo, module->resolveReference(reference));
         if (r) {
            // if it is a constant
            if (symbolInfo.constant) {
               ref_t classRef = symbolInfo.expressionClassRef != 0 ? symbolInfo.expressionClassRef : subjectHints.get(symbolInfo.expressionTypeRef);

               if (symbolInfo.listRef != 0) {
                  return ObjectInfo(okArrayConst, symbolInfo.listRef, classRef, symbolInfo.expressionTypeRef);
               }
               else return ObjectInfo(okConstantSymbol, reference, classRef, symbolInfo.expressionTypeRef);
            }
            // if it is a typed symbol
            else if (symbolInfo.expressionTypeRef != 0) {
               return ObjectInfo(okSymbol, reference, 0, symbolInfo.expressionTypeRef);
            }
         }
      }
   }

   // otherwise it is a normal one
   return ObjectInfo(okSymbol, reference);
}

ref_t Compiler::ModuleScope :: mapReference(ident_t referenceName, bool existing)
{
   if (emptystr(referenceName))
      return 0;

   ref_t reference = 0;
   if (!isWeakReference(referenceName)) {
      if (existing) {
         // check if the reference does exist
         ref_t moduleRef = 0;
         _Module* argModule = project->resolveModule(referenceName, moduleRef);

         if (argModule != NULL && moduleRef != 0)
            reference = module->mapReference(referenceName);
      }
      else reference = module->mapReference(referenceName, existing);
   }
   else reference = module->mapReference(referenceName, existing);

   return reference;
}

ObjectInfo Compiler::ModuleScope :: mapReferenceInfo(ident_t reference, bool existing)
{
   if (reference.compare(EXTERNAL_MODULE, strlen(EXTERNAL_MODULE))) {
      char ch = reference[strlen(EXTERNAL_MODULE)];
      if (ch == '\'' || ch == 0)
         return ObjectInfo(okExternal);
   }
   // To tell apart primitive modules, the name convention is used
   else if (reference.compare(INTERNAL_MASK, INTERNAL_MASK_LEN)) {
      return ObjectInfo(okInternal, module->mapReference(reference));
   }

   ref_t referenceID = mapReference(reference, existing);

   return defineObjectInfo(referenceID);
}

void Compiler::ModuleScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly)
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
         info.value2 = importSubject(exporter, info.value2, module);

         target.fieldTypes.add(type_it.key(), info);

         type_it++;
      }

      // import method attributes
      ClassInfo::MethodInfoMap::Iterator mtype_it = copy.methodHints.start();
      while (!mtype_it.Eof()) {
         Attribute key = mtype_it.key();
         ref_t value = *mtype_it;
         if (test(key.value2, maSubjectMask)) {
            value = importSubject(exporter, value, module);
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
            importReference(exporter, (*static_it).value2, module),
            importSubject(exporter, (*static_it).value2, module));

         target.statics.add(static_it.key(), info);

         static_it++;
      }
   }
   // import class class reference
   if (target.header.classRef != 0)
      target.header.classRef = importReference(exporter, target.header.classRef, module);

   // import parent reference
   target.header.parentRef = importReference(exporter, target.header.parentRef, module);
}

_Module* Compiler::ModuleScope :: loadReferenceModule(ref_t& reference)
{
   return project->resolveModule(module->resolveReference(reference), reference);
}

ref_t Compiler::ModuleScope :: loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly)
{
   _Module* argModule;

   if (emptystr(vmtName))
      return 0;

   // load class meta data
   ref_t moduleRef = 0;
   argModule = project->resolveModule(vmtName, moduleRef, true);

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

ref_t Compiler::ModuleScope :: loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol)
{
   if (emptystr(symbol))
      return 0;

   // load class meta data
   ref_t moduleRef = 0;
   _Module* argModule = project->resolveModule(symbol, moduleRef, true);

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
      info.expressionTypeRef = importSubject(argModule, info.expressionTypeRef, module);
      info.expressionClassRef = importReference(argModule, info.expressionClassRef, module);
   }
   return moduleRef;
}

_Memory* Compiler::ModuleScope :: loadAttributeInfo(ident_t attribute/*, _Module* &argModule*/)
{
   if (emptystr(attribute))
      return NULL;

   List<ident_t>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      _Module* argModule = project->loadModule(*it, true);

      ref_t ref = argModule->mapSubject(attribute, true);
      if (ref) {
         _Memory* section = argModule->mapSection(ref | mskSyntaxTreeRef, true);
         if (section)
            return section;
      }
      it++;
   }

   //argModule = NULL;

   return NULL;
}

void Compiler::ModuleScope :: validateReference(SNode terminal, ref_t reference)
{
   // check if the reference may be resolved
   bool found = false;

   if (warnOnUnresolved && (warnOnWeakUnresolved || !isWeakReference(terminal.identifier()))) {
      int   mask = reference & mskAnyRef;
      reference &= ~mskAnyRef;

      ref_t    ref = 0;
      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);

      if (refModule != NULL) {
         found = (refModule->mapSection(ref | mask, true)!=NULL);
      }
      if (!found) {
         if (!refModule || refModule == module) {
            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module,
               terminal.findChild(lxRow).argument,
               terminal.findChild(lxCol).argument));
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnresovableLink, terminal);
      }
   }
}

void Compiler::ModuleScope :: raiseError(const char* message, SNode node)
{
   SNode terminal = findTerminalInfo(node);

   int col = terminal.findChild(lxCol).argument;
   int row = terminal.findChild(lxRow).argument;
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   raiseError(message, row, col, identifier);
}

void Compiler::ModuleScope :: raiseWarning(int level, const char* message, SNode node)
{
   SNode terminal = findTerminalInfo(node);

   int col = terminal.findChild(lxCol).argument;
   int row = terminal.findChild(lxRow).argument;
   ident_t identifier = terminal.findChild(lxTerminal).identifier();

   raiseWarning(level, message, row, col, identifier);
}

void Compiler::ModuleScope :: raiseWarning(int level, const char* message, int row, int col, ident_t terminal)
{
   if (test(warningMask, level))
      project->raiseWarning(message, sourcePath, row, col, terminal);
}

void Compiler::ModuleScope :: raiseError(const char* message, int row, int col, ident_t terminal)
{
   project->raiseError(message, sourcePath, row, col, terminal);
}

void Compiler::ModuleScope :: loadActions(_Module* extModule)
{
   if (extModule) {
      ReferenceNs sectionName(extModule->Name(), ACTION_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t mssg_ref = importMessage(extModule, metaReader.getDWord(), module);
            ref_t class_ref = importReference(extModule, metaReader.getDWord(), module);

            actionHints.add(mssg_ref, class_ref);
         }
      }
   }
}

void Compiler::ModuleScope :: loadAttributes(_Module* extModule)
{
   if (extModule) {
      //bool owner = module == extModule;

      ReferenceNs sectionName(extModule->Name(), ATTRIBUTE_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t subj_ref = importSubject(extModule, metaReader.getDWord(), module);
            ref_t class_ref = metaReader.getDWord();
            if (class_ref != INVALID_REF) {
               class_ref = importReference(extModule, class_ref, module);
            }

            subjectHints.add(subj_ref, class_ref);
         }
      }
   }
}

void Compiler::ModuleScope :: loadExtensions(_Module* extModule, bool& duplicateExtensions)
{
   if (extModule) {
      ReferenceNs sectionName(extModule->Name(), EXTENSION_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t type_ref = importSubject(extModule, metaReader.getDWord(), module);
            ref_t message = importMessage(extModule, metaReader.getDWord(), module);
            ref_t role_ref = importReference(extModule, metaReader.getDWord(), module);

            if(!extensionHints.exist(message, type_ref)) {
               extensionHints.add(message, type_ref);

               SubjectMap* typeExtensions = extensions.get(type_ref);
               if (!typeExtensions) {
                  typeExtensions = new SubjectMap();

                  extensions.add(type_ref, typeExtensions);
               }

               typeExtensions->add(message, role_ref);
            }
            else duplicateExtensions = true;
         }
      }
   }
}

void Compiler::ModuleScope :: saveSubject(ref_t attrRef, ref_t classReference, bool internalType)
{
   if (!internalType) {
      ReferenceNs sectionName(module->Name(), ATTRIBUTE_SECTION);

      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(attrRef);
      metaWriter.writeDWord(classReference);
   }

   subjectHints.add(attrRef, classReference, true);
}

bool Compiler::ModuleScope :: saveExtension(ref_t message, ref_t type, ref_t role)
{
   if (type == -1)
      type = 0;

   ReferenceNs sectionName(module->Name(), EXTENSION_SECTION);

   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

   metaWriter.writeDWord(type);
   metaWriter.writeDWord(message);
   metaWriter.writeDWord(role);

   if (!extensionHints.exist(message, type)) {
      extensionHints.add(message, type);

      SubjectMap* typeExtensions = extensions.get(type);
      if (!typeExtensions) {
         typeExtensions = new SubjectMap();

         extensions.add(type, typeExtensions);
      }

      typeExtensions->add(message, role);

      return true;
   }
   else return false;
}

void Compiler::ModuleScope :: saveAction(ref_t mssg_ref, ref_t reference)
{
   ReferenceNs sectionName(module->Name(), ACTION_SECTION);

   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

   metaWriter.writeDWord(mssg_ref);
   metaWriter.writeDWord(reference);

   actionHints.add(mssg_ref, reference);
}

ref_t Compiler::ModuleScope :: mapAttribute(SNode attribute, int& attrValue)
{
   ref_t attrRef = 0;

   SNode terminal = attribute.findChild(lxPrivate, lxIdentifier, lxExplicitAttr);
   if (terminal == lxNone)
      terminal = attribute;
   
   if (terminal == lxExplicitAttr) {
      ident_t value = terminal.findChild(lxTerminal).identifier();
   
      attrValue = value.toInt(1);
      if (attrValue >= 0x10000) {
         attrValue = attrValue >> 16;
      }
      else attrValue = -attrValue;
   }
   else attrRef = mapSubject(terminal);
   
   return attrRef;
}

// --- Compiler::SourceScope ---

Compiler::SourceScope :: SourceScope(ModuleScope* moduleScope, ref_t reference)
   : Scope(moduleScope)
{
   this->reference = reference;
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(ModuleScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   outputRef = typeRef = 0;
   constant = false;
   staticOne = false;
   preloaded = false;
}

ObjectInfo Compiler::SymbolScope :: mapTerminal(ident_t identifier)
{
   return Scope::mapTerminal(identifier);
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(ModuleScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   info.header.parentRef =   moduleScope->superReference;
   info.header.flags = elStandartVMT;
   info.header.count = 0;
   info.header.classRef = 0;
   info.header.packageRef = parent->packageReference;
   info.size = 0;
   
   extensionMode = 0;
}

ObjectInfo Compiler::ClassScope :: mapField(ident_t terminal)
{
   int offset = info.fields.get(terminal);
   if (offset >= 0) {
      ClassInfo::FieldInfo fieldInfo = info.fieldTypes.get(offset);
      if (test(info.header.flags, elStructureRole)) {
         return ObjectInfo(okFieldAddress, offset, fieldInfo.value1, fieldInfo.value2);
      }
      else return ObjectInfo(okField, offset, fieldInfo.value1, fieldInfo.value2);
   }
   else if (offset == -2 && test(info.header.flags, elDynamicRole)) {
      return ObjectInfo(okThisParam, 1, -2, info.fieldTypes.get(-1).value2);
   }
   else {
      ClassInfo::FieldInfo staticInfo = info.statics.get(terminal);
      if (staticInfo.value1 != 0) {
         return ObjectInfo(okStaticField, staticInfo.value1, 0, staticInfo.value2);
      }
      else return ObjectInfo();
   }

}

ObjectInfo Compiler::ClassScope :: mapTerminal(ident_t terminal)
{
   if (terminal.compare(SUPER_VAR)) {
      return ObjectInfo(okSuper, info.header.parentRef);
   }
   else if (terminal.compare(SELF_VAR)) {
      if (extensionMode != 0 && extensionMode != -1) {
         return ObjectInfo(okParam, (size_t)-1, 0, extensionMode);
      }
      else return ObjectInfo(okParam, (size_t)-1);
   }
   else {
      ObjectInfo fieldInfo = mapField(terminal);
      if (fieldInfo.kind != okUnknown) {
         return fieldInfo;
      }
      else return Scope::mapTerminal(terminal);
   }
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved = 0;
   this->rootToFree = 1;
   this->hints = 0;
   this->withOpenArg = false;
   this->stackSafe = this->classEmbeddable = false;
   this->generic = false;
   this->extensionMode = false;
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal)
{
   if (terminal.compare(THIS_VAR)) {
      if (extensionMode) {
         //COMPILER MAGIC : if it is an extension ; replace $self with self
         return ObjectInfo(okLocal, (ref_t)-1, ((ClassScope*)getScope(slClass))->reference);          
      }
      else if (stackSafe && classEmbeddable) {
         return ObjectInfo(okThisParam, 1, -1);
      }
      else return ObjectInfo(okThisParam, 1);
   }
   else {
      Parameter param = parameters.get(terminal);

      int local = param.offset;
      if (local >= 0) {
         if (withOpenArg && param.subj_ref == moduleScope->paramsSubj) {
            return ObjectInfo(okParams, -1 - local, 0, param.subj_ref);
         }
         else if (stackSafe && param.subj_ref != 0 && param.size != 0) {
            return ObjectInfo(okParam, -1 - local, -1, param.subj_ref);
         }
         return ObjectInfo(okParam, -1 - local, 0, param.subj_ref);
      }
      else return Scope::mapTerminal(terminal);
   }
}

// --- Compiler::ActionScope ---

Compiler::ActionScope :: ActionScope(ClassScope* parent)
   : MethodScope(parent)
{
   subCodeMode = false;
   singletonMode = false;
}

ObjectInfo Compiler::ActionScope :: mapTerminal(ident_t identifier)
{   
   if (singletonMode && identifier.compare(SELF_VAR)) {
      // COMPILER MAGIC : recognize self / $self in singleton closure
      return ObjectInfo(okParam, (size_t)-1);
   }
   if (identifier.compare(THIS_VAR)) {
      if (singletonMode) {
         // COMPILER MAGIC : recognize $self in singleton closure
         return ObjectInfo(okThisParam, 1);
      }
      // otherwise it should refer to the owner ones
      else return parent->mapTerminal(identifier);
   }
   else if (identifier.compare(CLOSURE_THIS_VAR)) {
      if (subCodeMode) {
         return parent->mapTerminal(identifier);
      }
      else return MethodScope::mapTerminal(THIS_VAR);
   }
   else if (identifier.compare(CLOSURE_SELF_VAR)) {
      if (subCodeMode) {
         return parent->mapTerminal(identifier);
      }
      else return ObjectInfo(okParam, (size_t)-1);
   }
   else if (identifier.compare(RETVAL_VAR) && subCodeMode) {
      ObjectInfo retVar = parent->mapTerminal(identifier);
      if (retVar.kind == okUnknown) {
         InlineClassScope* closure = (InlineClassScope*)getScope(Scope::slClass);

         retVar = closure->allocateRetVar();
      }

      return retVar;
   }
   else return MethodScope::mapTerminal(identifier);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SymbolScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->level = 0;
   this->saved = this->reserved = 0;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->level = 0;
   this->saved = this->reserved = 0;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->level = parent->level;
   this->saved = parent->saved;
   this->reserved = parent->reserved;
}

ObjectInfo Compiler::CodeScope :: mapTerminal(ident_t identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset) {
      if (identifier.compare(SUBJECT_VAR)) {
         return ObjectInfo(okSubject, local.offset);
      }
      else if (local.size != 0) {
         return ObjectInfo(okLocalAddress, local.offset, local.class_ref, local.subj_ref);
      }
      else return ObjectInfo(okLocal, local.offset, local.class_ref, local.subj_ref);
   }
   else return Scope::mapTerminal(identifier);
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
   : ClassScope(owner->moduleScope, reference), outers(Outer()), outerFieldTypes(ClassInfo::FieldInfo(0, 0))
{
   this->returningMode = false;
   this->closureMode = false;
   this->parent = owner;
   info.header.flags |= elNestedClass;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   Outer owner = outers.get(THIS_VAR);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();

      owner.outerObject = parent->mapTerminal(THIS_VAR);
      if (owner.outerObject.extraparam == 0)
         owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId(false);

      outers.add(THIS_VAR, owner);
      mapKey(info.fields, THIS_VAR, (int)owner.reference);
   }
   return owner;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope::mapOwner()
{
   Outer owner = outers.get(OWNER_VAR);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.outerObject = parent->mapTerminal(OWNER_VAR);
      if (owner.outerObject.kind != okUnknown) {
         owner.reference = info.fields.Count();

         if (owner.outerObject.extraparam == 0)
            owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId(false);

         outers.add(OWNER_VAR, owner);
         mapKey(info.fields, OWNER_VAR, (int)owner.reference);
      }
      else return mapSelf();
   }
   return owner;
}

ObjectInfo Compiler::InlineClassScope :: mapTerminal(ident_t identifier)
{
   if (identifier.compare(THIS_VAR)) {
      Outer owner = mapSelf();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuter, owner.reference, owner.outerObject.extraparam, owner.outerObject.type);
   }
   else if (identifier.compare(SUPER_VAR)) {
      return ObjectInfo(okSuper, info.header.parentRef);
   }
   else if (identifier.compare(OWNER_VAR)) {
      Outer owner = mapOwner();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuter, owner.reference, owner.outerObject.extraparam, owner.outerObject.type);
   }
   else if (identifier.compare(SELF_VAR) && !closureMode) {
      return ObjectInfo(okParam, (size_t)-1);
   }
   else {
      Outer outer = outers.get(identifier);

      // if object already mapped
      if (outer.reference != -1) {
         if (outer.outerObject.kind == okSuper) {
            return ObjectInfo(okSuper, outer.reference);
         }
         else return ObjectInfo(okOuter, outer.reference, 0, outer.outerObject.type);
      }
      else {
         outer.outerObject = parent->mapTerminal(identifier);
         // handle outer fields in a special way: save only self
         if (outer.outerObject.kind == okField || outer.outerObject.kind == okOuterField) {
            Outer owner = mapSelf();

            // save the outer field type if provided
            if (outer.outerObject.extraparam != 0) {
               outerFieldTypes.add(outer.outerObject.param, ClassInfo::FieldInfo(outer.outerObject.extraparam, outer.outerObject.type), true);
            }

            // map as an outer field (reference to outer object and outer object field index)
            if (outer.outerObject.kind == okOuterField) {
               return ObjectInfo(okOuterField, owner.reference, outer.outerObject.extraparam, outer.outerObject.type);
            }
            else return ObjectInfo(okOuterField, owner.reference, outer.outerObject.param, outer.outerObject.type);
         }
         // map if the object is outer one
         else if (outer.outerObject.kind == okParam || outer.outerObject.kind == okLocal
            || outer.outerObject.kind == okOuter || outer.outerObject.kind == okSuper || outer.outerObject.kind == okThisParam
            || outer.outerObject.kind == okLocalAddress)
         {
            outer.reference = info.fields.Count();

            outers.add(identifier, outer);
            mapKey(info.fields, identifier, (int)outer.reference);

            if (outer.outerObject.kind == okOuter && identifier.compare(RETVAL_VAR)) {
               // HOTFIX : quitting several clsoures
               (*outers.getIt(identifier)).preserved = true;
            }

            switch (outer.outerObject.kind) {
               case okOuterField:
               case okParam:
               case okThisParam:
                  return ObjectInfo(okOuter, outer.reference, 0, outer.outerObject.type);
               default:
                  return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam, outer.outerObject.type);
            }
         }
         // map if the object is outer one
         else if (outer.outerObject.kind == okUnknown) {
            // check if there is inherited fields
            ObjectInfo fieldInfo = mapField(identifier);
            if (fieldInfo.kind != okUnknown) {
               return fieldInfo;
            }
            else return outer.outerObject;
         }
         else return outer.outerObject;
      }
   }
}

bool Compiler::InlineClassScope :: markAsPresaved(ObjectInfo object)
{
   if (object.kind == okOuter) {
      Map<ident_t, Outer>::Iterator it = outers.start();
      while (!it.Eof()) {
         if ((*it).reference == object.param) {
            if ((*it).outerObject.kind == okLocal || (*it).outerObject.kind == okLocalAddress) {
               (*it).preserved = true;

               return true;
            }
            else if ((*it).outerObject.kind == okOuter) {
               InlineClassScope* closure = (InlineClassScope*)parent->getScope(Scope::slClass);
               if (closure->markAsPresaved((*it).outerObject)) {
                  (*it).preserved = true;

                  return true;
               }
               else return false;
            }
            break;
         }

         it++;
      }
   }

   return false;
}

ObjectInfo Compiler::InlineClassScope :: allocateRetVar()
{
   returningMode = true;

   Outer outer;
   outer.reference = info.fields.Count();
   outer.outerObject = ObjectInfo(okNil, -1);

   outers.add(RETVAL_VAR, outer);
   mapKey(info.fields, RETVAL_VAR, (int)outer.reference);

   return ObjectInfo(okOuter, outer.reference);
}

// --- Compiler::TemplateScope ---

ref_t Compiler::TemplateScope :: mapAttribute(SNode attribute, int& attrValue)
{   
   SNode terminal = attribute.firstChild(lxTerminalMask);
   if (terminal == lxNone)
      terminal = attribute;

   int index = mapAttribute(terminal);
   if (index) {
      attrValue = index;

      return INVALID_REF;
   }
   else return moduleScope->mapAttribute(attribute, attrValue);
}

bool Compiler::TemplateScope :: isAttribute(SNode terminal)
{
   int dummy = 0;
   ref_t ref = mapAttribute(terminal, dummy);

   return (ref != 0 && isPrimitiveRef(moduleScope->subjectHints.get(ref)));
}

bool Compiler::TemplateScope :: isTypeAttribute(SNode terminal, bool followedByVerb)
{
   int dummy = 0;
   ref_t ref = mapAttribute(terminal, dummy);
   if (ref == INVALID_REF) {
      return followedByVerb;
   }
   else return mapSubject(terminal.findChild(lxIdentifier, lxPrivate)) != 0;
}

ref_t Compiler::TemplateScope :: mapTemplate(SNode terminal, int prefixCounter)
{
   int paramCounter = SyntaxTree::countChild(terminal, lxAttributeValue);
   IdentifierString attrName(terminal.findChild(lxIdentifier).findChild(lxTerminal).identifier());
   if (prefixCounter != 0) {
      attrName.append('#');
   }
   attrName.append('#');
   attrName.appendInt(paramCounter + prefixCounter);
   
   return moduleScope->resolveAttributeRef(attrName, false);
}

void Compiler::TemplateScope :: loadAttributeValues(SNode attributes, bool prefixMode)
{
   SNode current = attributes;
   // load template parameters
   while (current != lxNone) {
      if (current == lxAttributeValue) {
         SNode terminalNode = current.firstChild(lxObjectMask);
         ref_t subject = mapSubject(terminalNode);
         if (subject == 0) {
            ident_t identifier = terminalNode.identifier();
            if (emptystr(identifier))
               identifier = terminalNode.findChild(lxTerminal).identifier();

            subject = moduleScope->module->mapSubject(identifier, false);
         }

         subjects.add(subjects.Count() + 1, subject);
      }
//      else if (current == lxTypeAttr) {
//         ref_t subject = subject = moduleScope->module->mapSubject(current.identifier(), false);
//
//         subjects.add(subjects.Count() + 1, subject);
//      }
      else if (current == lxTemplateType) {
         TemplateScope* parentTemplate = (TemplateScope*)parent;
         ref_t subject = parentTemplate->subjects.get(current.argument);

         subjects.add(subjects.Count() + 1, subject);
      }
      else if (prefixMode && current == lxNameAttr)
         break;

      current = current.nextNode();
   }
}

void Compiler::TemplateScope :: loadParameters(SNode node)
{
   SNode current = node.firstChild();
   // load template parameters
   while (current != lxNone) {
      if (current == lxBaseParent) {               
         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();

         parameters.add(name, parameters.Count() + 1);
      }

      current = current.nextNode();
   }
}

void Compiler::TemplateScope :: loadFields(SNode node)
{
   SNode current = node.firstChild();
   // load template parameters
   while (current != lxNone) {
      if (current == lxAttributeValue) {
         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();

         fields.add(name, parameters.Count() + 1);
      }

      current = current.nextNode();
   }
}

int Compiler::TemplateScope :: mapAttribute(SNode terminal)
{
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   int index = parameters.get(identifier);
   if (!index) {
      if (parent != NULL) {
         return ((TemplateScope*)parent)->mapAttribute(terminal);
      }
      else return 0;
   }
   else return index;
}

int Compiler::TemplateScope :: mapIdentifier(SNode terminal)
{
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   if (type == TemplateScope::ttFieldTemplate) {
      return fields.get(identifier);
   }
   else if (type == TemplateScope::ttCodeTemplate) {
      return parameters.get(identifier);
   }
   else return 0;
}

void Compiler::TemplateScope :: copySubject(SyntaxWriter& writer, SNode terminal)
{
   int index = mapAttribute(terminal);
   if (index) {
      writer.newNode(lxTemplateParam, index);
      copyIdentifier(writer, terminal);
      writer.closeNode();
   }
   else copyIdentifier(writer, terminal);
}

void Compiler::TemplateScope :: copyIdentifier(SyntaxWriter& writer, SNode terminal)
{
   ::copyIdentifier(writer, terminal);
}

bool Compiler::TemplateScope :: generateClassName()
{
   ReferenceNs name;
   name.copy(moduleScope->module->Name());
   name.combine(moduleScope->module->resolveSubject(templateRef));

   SubjectMap::Iterator it = subjects.start();
   while (!it.Eof()) {
      name.append('@');
      name.append(moduleScope->module->resolveSubject(*it));

      it++;
   }

   reference = moduleScope->module->mapReference(name, true);
   if (!reference) {
      reference = moduleScope->module->mapReference(name, false);

      return true;
   }
   else return false;
}

// --- Compiler ---

Compiler :: Compiler(_CompilerLogic* logic)
   : _verbs(0)
{
   _optFlag = 0;

   this->_logic = logic;

   ByteCodeCompiler::loadVerbs(_verbs);
   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: writeMessageInfo(SyntaxWriter& writer, ModuleScope& scope, ref_t messageRef)
{
   ref_t subjectRef, verb;
   int paramCount;
   decodeMessage(messageRef, subjectRef, verb, paramCount);

   IdentifierString name(retrieveKey(_verbs.start(), verb, DEFAULT_STR));
   if (subjectRef != 0) {
      name.append('&');
      name.append(scope.module->resolveSubject(subjectRef));
   }
   name.append('[');
   name.appendInt(paramCount);
   name.append(']');

   writer.appendNode(lxMessageVariable, name);
}

void Compiler :: loadRules(StreamReader* optimization)
{
   _rules.load(optimization);
}

bool Compiler :: optimizeIdleBreakpoints(CommandTape& tape)
{
   return CommandTape::optimizeIdleBreakpoints(tape);
}

bool Compiler :: optimizeJumps(CommandTape& tape)
{
   return CommandTape::optimizeJumps(tape);
}

bool Compiler :: applyRules(CommandTape& tape)
{
   if (!_rules.loaded)
      return false;

   if (_rules.apply(tape)) {
      while (_rules.apply(tape));

      return true;
   }
   else return false;
}

void Compiler :: optimizeTape(CommandTape& tape)
{
   // HOTFIX : remove all breakpoints which follows jumps
   while (optimizeIdleBreakpoints(tape));

   // optimize unused and idle jumps
   while (optimizeJumps(tape));

   // optimize the code
   bool modified = false;
   while (applyRules(tape)) {
      modified = true;
   }

   if (modified) {
      optimizeTape(tape);
   }
}

pos_t Compiler :: saveSourcePath(ModuleScope&, ident_t path)
{
   return _writer.writeString(path);
}

ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object)
{
   if (object.kind == okThisParam) {
      if (object.extraparam == -2) {
         return _logic->definePrimitiveArray(*scope.moduleScope, scope.moduleScope->subjectHints.get(object.type));
      }
      else return scope.getClassRefId(false);
   }
   else return resolveObjectReference(*scope.moduleScope, object);
}


ref_t Compiler :: resolveObjectReference(ModuleScope& scope, ObjectInfo object)
{
   // if static message is sent to a class class
   switch (object.kind)
   {
      case okConstantClass:
         return object.extraparam;
      case okConstantRole:
         // if external role is provided
         return object.param;
      case okConstantSymbol:
         if (object.extraparam != 0) {
            return object.extraparam;
         }
         else return object.param;
      case okLocalAddress:
         return object.extraparam;
      case okIntConstant:
         return V_INT32;
      case okLongConstant:
         return V_INT64;
      case okRealConstant:
         return V_REAL64;
      case okCharConstant:
         return scope.charReference;
      case okLiteralConstant:
         return scope.literalReference;
      case okWideLiteralConstant:
         return scope.wideReference;
      case okSubject:
      case okSignatureConstant:
         return V_SIGNATURE;
      case okSuper:
         return object.param;
//      case okTemplateLocal:
//         return object.extraparam;
      case okParams:
         return V_ARGARRAY;
      case okExternal:
         return V_INT32;
      case okMessageConstant:
         return V_MESSAGE;
      case okNil:
         return V_NIL;
      case okField:
      case okLocal:
      case okFieldAddress:
      case okOuter:
         if (object.extraparam != 0) {
            return object.extraparam;
         }
      case okParam:
      default:
         if (object.kind == okObject && object.param != 0) {
            return object.param;
         }
         else return (object.type != 0) ? scope.subjectHints.get(object.type) : 0;
   }
}

void Compiler :: declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withThis, bool withSelf)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // declare built-in variables
   if (withThis) {
      if (scope.stackSafe && scope.classEmbeddable) {
         SNode debugNode = node.insertNode(lxBinarySelf, 1);
         debugNode.appendNode(lxClassName, scope.moduleScope->module->resolveReference(scope.getClassRef()));
      }
      else writer.appendNode(lxSelfVariable, 1);
   }

   if (withSelf)
      writer.appendNode(lxSelfVariable, -1);

   writeMessageInfo(writer, *moduleScope, scope.message);

   SNode current = node.firstChild();
   // method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter || current == lxIdentifier) {         
         ident_t name = (current == lxIdentifier) ? current.identifier() : current.findChild(lxIdentifier, lxPrivate).identifier();
         Parameter param = scope.parameters.get(name);
         if (param.offset != -1) {
            if (param.subj_ref == moduleScope->paramsSubj) {
               writer.newNode(lxParamsVariable);
            }
            else if (scope.moduleScope->subjectHints.exist(param.subj_ref, moduleScope->intReference)) {
               writer.newNode(lxIntVariable);
            }
            else if (scope.moduleScope->subjectHints.exist(param.subj_ref, moduleScope->longReference)) {
               writer.newNode(lxLongVariable);
            }
            else if (scope.moduleScope->subjectHints.exist(param.subj_ref, moduleScope->realReference)) {
               writer.newNode(lxReal64Variable);
            }
            else if (scope.stackSafe && param.subj_ref != 0) {
               ref_t classRef = scope.moduleScope->subjectHints.get(param.subj_ref);
               if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {
                  writer.newNode(lxBinaryVariable);
                  writer.appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
               }
               else writer.newNode(lxVariable);
            }
            else writer.newNode(lxVariable);

            writer.appendNode(lxLevel, -1 - param.offset);
            writer.newNode(lxIdentifier);
            writer.appendNode(lxTerminal, name);
            writer.closeNode();

            writer.closeNode();
         }
      }
      else if (current == lxSourcePath) {
         writer.appendNode(lxSourcePath, saveSourcePath(*scope.moduleScope, current.identifier()));
      }

      current = current.nextNode();
   }

   //COMPILER MAGIC : if it is an extension ; replace $self with self
   ClassScope* ownerScope = (ClassScope*)scope.getScope(Scope::slClass);
   scope.extensionMode = (ownerScope != NULL && ownerScope->extensionMode != 0);
}

void Compiler :: importCode(SyntaxWriter& writer, SNode node, ModuleScope& scope, ident_t function, ref_t message)
{
   IdentifierString virtualReference(function);
   virtualReference.append('.');

   int paramCount;
   ref_t sign_ref, verb_id;
   decodeMessage(message, sign_ref, verb_id, paramCount);

   // HOTFIX : include self as a parameter
   paramCount++;

   virtualReference.append('0' + (char)paramCount);
   virtualReference.append('#');
   virtualReference.append(0x20 + (char)verb_id);

   if (sign_ref != 0) {
      virtualReference.append('&');
      virtualReference.append(scope.module->resolveSubject(sign_ref));
   }

   ref_t reference = 0;
   _Module* api = scope.project->resolveModule(virtualReference, reference);

   _Memory* section = api != NULL ? api->mapSection(reference | mskCodeRef, true) : NULL;
   if (section != NULL) {
      writer.appendNode(lxImporting, _writer.registerImportInfo(section, api, scope.module));
   }
   else scope.raiseError(errInvalidLink, node);
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed)
{
   ModuleScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.header.classRef;
   size_t packageRefCopy = scope.info.header.packageRef;

   // get module reference
   ref_t moduleRef = 0;
   _Module* module = moduleScope->project->resolveModule(moduleScope->module->resolveReference(parentRef), moduleRef);

   if (module == NULL || moduleRef == 0)
      return irUnsuccessfull;

   // load parent meta data
   _Memory* metaData = module->mapSection(moduleRef | mskMetaRDataRef, true);
   if (metaData != NULL) {
      MemoryReader reader(metaData);
      // import references if we inheriting class from another module
      if (moduleScope->module != module) {
         ClassInfo copy;
         copy.load(&reader);

         moduleScope->importClassInfo(copy, scope.info, module, false);
      }
      else {
         scope.info.load(&reader);

         // mark all methods as inherited
         ClassInfo::MethodMap::Iterator it = scope.info.methods.start();
         while (!it.Eof()) {
            (*it) = false;

            it++;
         }
      }

      if (!ignoreSealed && test(scope.info.header.flags, elSealed))
         return irSealed;

      // restore parent and flags
      scope.info.header.parentRef = parentRef;
      scope.info.header.classRef = classClassCopy;
      scope.info.header.flags |= flagCopy;
      scope.info.header.packageRef = packageRefCopy;

      return irSuccessfull;
   }
   else return irUnsuccessfull;
}

void Compiler :: compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed)
{
   scope.info.header.parentRef = parentRef;
   InheritResult res = irSuccessfull;
   if (scope.info.header.parentRef != 0) {
      res = inheritClass(scope, scope.info.header.parentRef, ignoreSealed);
   }

   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   if (res == irInvalid) {
      scope.raiseError(errInvalidParent, baseNode);
   }
   if (res == irSealed) {
      scope.raiseError(errSealedParent, baseNode);
   }
   else if (res == irUnsuccessfull)
      scope.raiseError(errUnknownBaseClass, baseNode);
}

void Compiler :: compileParentDeclaration(SNode node, ClassScope& scope)
{
   ref_t parentRef = scope.info.header.parentRef;

   SNode identifier = node.findChild(lxIdentifier,lxPrivate,lxReference);
   if (scope.info.header.parentRef == scope.reference) {
      if (identifier != lxNone) {
         scope.raiseError(errInvalidSyntax, node);
      }
      else parentRef = 0;
   }
   else if (identifier != lxNone) {
      if (identifier == lxIdentifier || identifier == lxPrivate) {
         parentRef = scope.moduleScope->mapTerminal(identifier, true);
      }
      else parentRef = scope.moduleScope->mapReference(identifier.findChild(lxTerminal).identifier());

      if (parentRef == 0)
         scope.raiseError(errUnknownClass, identifier);
   }

   compileParentDeclaration(node, scope, parentRef);
}

void Compiler :: declareClassAttributes(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (!_logic->validateClassAttribute(value)) {
            current = lxIdle;

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else current.set(lxClassFlag, value);
      }
      else if (current == lxTypeAttr) {
         current.set(lxType, scope.moduleScope->module->mapSubject(current.identifier(), false));
      }
      current = current.nextNode();
   }
}

void Compiler :: declareSymbolAttributes(SNode node, SymbolScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (!_logic->validateSymbolAttribute(value, scope.constant, scope.staticOne, scope.preloaded))
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }
      else if (current == lxTypeAttr) {
         if (scope.typeRef == 0) {
            scope.typeRef = scope.moduleScope->module->mapSubject(current.identifier(), false);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }
      else if (current == lxClassRefAttr) {
         scope.outputRef = scope.moduleScope->module->mapReference(current.identifier(), false);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareFieldAttributes(SNode node, ClassScope& scope, ref_t& fieldType, ref_t& fieldRef, int& size)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (value > 0 && size == 0) {
            size = value;
         }
         else if (_logic->validateFieldAttribute(value) && fieldRef == 0) {
            fieldRef = current.argument;
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxTypeAttr) {
         if (fieldRef == 0) {
            fieldType = scope.moduleScope->module->mapSubject(current.identifier(), false);
            fieldRef = scope.moduleScope->subjectHints.get(fieldType);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxClassRefAttr) {
         if (fieldRef == 0) {
            fieldRef = scope.moduleScope->module->mapReference(current.identifier(), false);
         }
         else scope.raiseError(errInvalidHint, node);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareLocalAttributes(SNode node, CodeScope& scope, ObjectInfo& variable, int& size)
{
   SNode current = node.firstChild(lxAttribute);
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (value > 0 && size == 0) {
            size = value;
         }
         else if (_logic->validateLocalAttribute(value) && variable.extraparam == 0) {
            // negative value defines the target virtual class
            variable.extraparam = value;
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }
      else if (current == lxTypeAttr) {
         if (variable.extraparam == 0) {
            variable.type = scope.moduleScope->module->mapSubject(current.identifier(), false);
            variable.extraparam = scope.moduleScope->subjectHints.get(variable.type);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxClassRefAttr) {
         if (variable.extraparam == 0) {
            variable.extraparam = scope.moduleScope->module->mapReference(current.identifier(), false);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      current = current.nextNode();
   }

   if (size != 0 && variable.type != 0) {
      variable.extraparam = _logic->definePrimitiveArray(*scope.moduleScope, variable.extraparam);
   }
   
}

void Compiler :: compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   SNode targetNode = node.firstChild(lxObjectMask);

   writer.newBookmark();

   bool immMode = true;
   int localOffs = 0;
   ObjectInfo loperand;
   if (targetNode == lxExpression) {
      immMode = false;

      localOffs = scope.newLocal();

      loperand = compileExpression(writer, targetNode, scope, 0);

      writer.insertChild(0, lxLocal, localOffs);
      writer.insert(lxAssigning, 0);
      writer.closeNode();
   }

   SNode current = node.findChild(lxOption, lxElse);
   while (current == lxOption) {
      writer.newNode(lxOption);
      writer.newNode(lxExpression);

      writer.newBookmark();

      writer.appendNode(lxBreakpoint, dsStep);

      int operator_id = current.argument;

      if (!immMode) {
         writer.newNode(lxLocal, localOffs);
         writer.appendNode(lxTarget, resolveObjectReference(scope, loperand));
         writer.closeNode();         
      }
      else loperand = compileExpression(writer, targetNode, scope, 0);

      // find option value
      SNode valueNode = current.firstChild(lxObjectMask);

      ObjectInfo roperand = compileExpression(writer, valueNode.firstChild(lxObjectMask), scope, 0);

      ObjectInfo operationInfo = compileOperator(writer, node, scope, operator_id, 1, loperand, roperand, ObjectInfo());

      ObjectInfo retVal;
      compileBranchingOperand(writer, valueNode, scope, HINT_SWITCH, IF_MESSAGE_ID, operationInfo, retVal);

      writer.removeBookmark();
      writer.closeNode();
      writer.closeNode();

      current = current.nextNode();
   }

   if (current == lxElse) {
      CodeScope subScope(&scope);
      SNode thenCode = current.findSubNode(lxCode);

      writer.newNode(lxElse);

      SNode statement = thenCode.firstChild(lxObjectMask);
      if (statement.nextNode() != lxNone || statement == lxEOF) {
         compileCode(writer, thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(writer, statement, scope, 0);

      // preserve the allocated space
      scope.level = subScope.level;

      writer.closeNode();
   }

   writer.insert(lxSwitching);
   writer.closeNode();

   writer.removeBookmark();
}

void Compiler :: compileVariable(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   SNode terminal = node.findChild(lxIdentifier, lxPrivate, lxExpression);
   if (terminal == lxExpression)
      terminal = terminal.findChild(lxIdentifier, lxPrivate);

   ident_t identifier = terminal.identifier();

   if (!scope.locals.exist(identifier)) {
      LexicalType variableType = lxVariable;
      int variableArg = 0;
      int size = 0;
      ident_t className = NULL;

      ObjectInfo variable(okLocal);
      declareLocalAttributes(node, scope, variable, size);

      ClassInfo localInfo;
      bool bytearray = false;
      _logic->defineClassInfo(*scope.moduleScope, localInfo, variable.extraparam);
      if (_logic->isEmbeddableArray(localInfo)) {
         bytearray = true;
         size = size * (-((int)localInfo.size));
      }
      else if (_logic->isEmbeddable(localInfo))
         size = _logic->defineStructSize(localInfo);

      if (size > 0) {
         if (!allocateStructure(scope, size, bytearray, variable))
            scope.raiseError(errInvalidOperation, terminal);

         // make the reservation permanent
         scope.saved = scope.reserved;

         switch (localInfo.header.flags & elDebugMask)
         {
            case elDebugDWORD:
               variableType = lxIntVariable;
               break;
            case elDebugQWORD:
               variableType = lxLongVariable;
               break;
            case elDebugReal64:
               variableType = lxReal64Variable;
               break;
            case elDebugIntegers:
               variableType = lxIntsVariable;
               variableArg = size;
               break;
            case elDebugShorts:
               variableType = lxShortsVariable;
               variableArg = size;
               break;
            case elDebugBytes:
               variableType = lxBytesVariable;
               variableArg = size;
               break;
            default:
               if (isPrimitiveRef(variable.extraparam)) {
                  variableType = lxBytesVariable;
                  variableArg = size;
               }
               else {
                  variableType = lxBinaryVariable;
                  // HOTFIX : size should be provide only for dynamic variables
                  if (bytearray)
                     variableArg = size;

                  if (variable.extraparam != 0) {
                     className = scope.moduleScope->module->resolveReference(variable.extraparam);
                  }
               }
               break;
         }
      }
      else variable.param = scope.newLocal();

      writer.newNode(variableType, variableArg);

      writer.appendNode(lxLevel, variable.param);
      writer.appendNode(lxIdentifier, identifier);
      if (!emptystr(className))
         writer.appendNode(lxClassName, className);

      writer.closeNode();

      scope.mapLocal(identifier, variable.param, variable.type, variable.extraparam, size);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

void Compiler :: writeTerminalInfo(SyntaxWriter& writer, SNode terminal)
{
   SyntaxTree::copyNode(writer, lxRow, terminal);
   SyntaxTree::copyNode(writer, lxCol, terminal);
   SyntaxTree::copyNode(writer, lxLength, terminal);
   //writer.appendNode(lxTerminal, terminal.identifier());
}

inline void writeTarget(SyntaxWriter& writer, ref_t targetRef)
{
   if (targetRef)
      writer.appendNode(lxTarget, targetRef);
}

void Compiler :: writeTerminal(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ObjectInfo object, int mode)
{
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         writer.newNode(lxSymbolReference, object.param);
         break;
      case okConstantClass:
         writer.newNode(lxConstantClass, object.param);
         break;
      case okConstantSymbol:
         writer.newNode(lxConstantSymbol, object.param);
         break;
      case okLiteralConstant:
         writer.newNode(lxConstantString, object.param);
         break;
      case okWideLiteralConstant:
         writer.newNode(lxConstantWideStr, object.param);
         break;
      case okCharConstant:
         writer.newNode(lxConstantChar, object.param);
         break;
      case okIntConstant:
         writer.newNode(lxConstantInt, object.param);
         writer.appendNode(lxIntValue, object.extraparam);
         break;
      case okLongConstant:
         writer.newNode(lxConstantLong, object.param);
         break;
      case okRealConstant:
         writer.newNode(lxConstantReal, object.param);
         break;
      case okArrayConst:
         writer.newNode(lxConstantList, object.param);
         break;
      case okLocal:
      case okParam:
         if (object.extraparam == -1) {
            ref_t targetRef = resolveObjectReference(scope, object);
            writer.newNode(lxCondBoxing, _logic->defineStructSize(*scope.moduleScope, targetRef));
            writer.appendNode(lxLocal, object.param);
         }
         else writer.newNode(lxLocal, object.param);
         break;
      case okThisParam:
         if (object.extraparam == -1) {
            ref_t targetRef = resolveObjectReference(scope, object);
            writer.newNode(lxCondBoxing, _logic->defineStructSize(*scope.moduleScope, targetRef));
            writer.appendNode(lxThisLocal, object.param);
         }
         else writer.newNode(lxThisLocal, object.param);
         break;
      case okSuper:
         writer.newNode(lxLocal, 1);
         break;
      case okField:
      case okOuter:
         writer.newNode(lxField, object.param);
         break;
      case okStaticField:
         writer.newNode(lxStaticField, object.param);
         break;
      case okOuterField:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxField, object.param);
         writer.appendNode(lxResultField, object.extraparam);
         break;
      case okSubject:
         writer.newNode(lxBoxing, _logic->defineStructSize(*scope.moduleScope, scope.moduleScope->signatureReference));
         writer.appendNode(lxLocalAddress, object.param);
         writer.appendNode(lxTarget, scope.moduleScope->signatureReference);
         break;
      case okLocalAddress:
         if (!test(mode, HINT_NOBOXING) || test(mode, HINT_DYNAMIC_OBJECT)) {
            writer.newNode(lxBoxing, _logic->defineStructSize(*scope.moduleScope, object.extraparam));
            writer.appendNode(lxLocalAddress, object.param);
            if (test(mode, HINT_DYNAMIC_OBJECT))
               writer.appendNode(lxBoxingRequired);
         }
         else writer.newNode(lxLocalAddress, object.param);
         break;
      case okFieldAddress:
         if (!test(mode, HINT_NOBOXING) || test(mode, HINT_DYNAMIC_OBJECT)) {
            ref_t target = resolveObjectReference(scope, object);

            writer.newNode(lxBoxing, _logic->defineStructSize(*scope.moduleScope, target));
            writer.appendNode(lxFieldAddress, object.param);
            if (test(mode, HINT_DYNAMIC_OBJECT))
               writer.appendNode(lxBoxingRequired);
         }
         else writer.newNode(lxFieldAddress, object.param);
         break;
      case okNil:
         writer.newNode(lxNil, object.param);
         break;
      case okVerbConstant:
         writer.newNode(lxVerbConstant, object.param);
         break;
      case okMessageConstant:
         writer.newNode(lxMessageConstant, object.param);
         break;
      case okExtMessageConstant:
         writer.newNode(lxExtMessageConstant, object.param);
         break;
      case okSignatureConstant:
         writer.newNode(lxSignatureConstant, object.param);
         break;
//      case okBlockLocal:
//         terminal.set(lxBlockLocal, object.param);
//         break;
      case okParams:
         writer.newNode(lxArgBoxing, 0);
         writer.appendNode(lxBlockLocalAddr, object.param);
         writer.appendNode(lxTarget, scope.moduleScope->arrayReference);
         break;
      case okObject:
         writer.newNode(lxResult, 0);
         break;
      case okConstantRole:
         writer.newNode(lxConstantSymbol, object.param);
         break;
      case okExternal:
         return;
      case okInternal:
         writer.appendNode(lxInternalRef, object.param);
         return;
   }

   writeTarget(writer, resolveObjectReference(scope, object));

   if (!test(mode, HINT_NODEBUGINFO))
      writeTerminalInfo(writer, terminal);

   writer.closeNode();
}

ObjectInfo Compiler :: compileTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, int mode)
{
   //if (!test(mode, HINT_NODEBUGINFO))
   //   insertDebugStep(terminal, dsStep);

   ident_t token = terminal.identifier();

   ObjectInfo object;
   if (terminal==lxLiteral) {
      object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(token));
   }
   else if (terminal == lxWide) {
      object = ObjectInfo(okWideLiteralConstant, scope.moduleScope->module->mapConstant(token));
   }
   else if (terminal==lxCharacter) {
      object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(token));
   }
   else if (terminal == lxInteger) {
      String<char, 20> s;

      int integer = token.toInt();
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
   }
   else if (terminal == lxLong) {
      String<char, 30> s("_"); // special mark to tell apart from integer constant
      s.append(token, getlength(token) - 1);

      token.toULongLong(10, 1);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s));
   }
   else if (terminal == lxHexInteger) {
      String<char, 20> s;

      int integer = token.toULong(16);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
   }
   else if (terminal == lxReal) {
      String<char, 30> s(token, getlength(token) - 1);
      token.toDouble();
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // HOT FIX : to support 0r constant
      if (s.Length() == 1) {
         s.append(".0");
      }

      object = ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant((const char*)s));
   }
   else if (terminal == lxExplicitConst) {
      // try to resolve explicit constant
      size_t len = getlength(token);

      ref_t postfixRef = scope.moduleScope->module->mapSubject(token + len - 1, false);

      IdentifierString constant(token, len - 1);

      object = ObjectInfo(okExplicitConstant, scope.moduleScope->module->mapConstant(constant), postfixRef);
   }
//   else if (terminal == lxResult) {
//      object = ObjectInfo(okObject);
//   }
   else if (!emptystr(token))
      object = scope.mapObject(terminal);

   if (object.kind == okExplicitConstant) {
      // replace an explicit constant with the appropriate object
      writer.newBookmark();
      writeTerminal(writer, terminal, scope, ObjectInfo(okLiteralConstant, object.param) , mode);

      ref_t constRef = scope.moduleScope->actionHints.get(encodeMessage(object.extraparam, PRIVATE_MESSAGE_ID, 1));
      if (constRef != 0) {
         if (!convertObject(writer, *scope.moduleScope, constRef, 0, V_STRCONSTANT, object.extraparam))
            scope.raiseError(errInvalidConstant, terminal);
      }
      else scope.raiseError(errInvalidConstant, terminal);

      object = ObjectInfo(okObject, constRef);

      writer.removeBookmark();
   }
   else writeTerminal(writer, terminal, scope, object, mode);

   return object;
}

ObjectInfo Compiler :: compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

   SNode member = objectNode.findChild(lxCode, lxNestedClass, lxMessageReference, lxExpression, lxLazyExpression);
   switch (member.type)
   {
      case lxNestedClass:
      case lxLazyExpression:
         result = compileClosure(writer, member, scope, mode & HINT_CLOSURE_MASK);
         break;
      case lxCode:
         result = compileClosure(writer, objectNode, scope, mode & HINT_CLOSURE_MASK);
         break;
      case lxExpression:
         if (isCollection(member)) {
            result = compileCollection(writer, objectNode, scope);
         }
         else result = compileExpression(writer, member, scope, mode & HINT_CLOSURE_MASK);
         break;
      case lxMessageReference:
         result = compileMessageReference(writer, member, scope, mode);
         break;
      default:
         result = compileTerminal(writer, objectNode, scope, mode);
   }

   return result;
}

ObjectInfo Compiler :: compileMessageReference(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   SNode terminal = node.findChild(lxPrivate, lxIdentifier, lxLiteral);
   IdentifierString signature;
   ref_t verb_id = 0;
   int paramCount = -1;
   ref_t extensionRef = 0;
   if (terminal == lxIdentifier || terminal == lxPrivate) {
      ident_t name = terminal.identifier();
      verb_id = _verbs.get(name);
      if (verb_id == 0) {
         signature.copy(name);
      }
   }
   else {
      ident_t message = terminal.identifier();

      int subject = 0;
      int param = 0;
      bool firstSubj = true;
      for (size_t i = 0; i < getlength(message); i++) {
         if (message[i] == '&') {
            if (firstSubj) {
               signature.copy(message + subject, i - subject);
               verb_id = _verbs.get(signature);
               if (verb_id != 0) {
                  subject = i + 1;
               }
               firstSubj = false;
            }
         }
         else if (message[i] == '.' && extensionRef == 0) {
            signature.copy(message + subject, i - subject);
            subject = i + 1;

            extensionRef = scope.moduleScope->resolveIdentifier(signature);
            if (extensionRef == 0)
               scope.raiseError(errInvalidSubject, terminal);
         }
         else if (message[i] == '[') {
            int len = getlength(message);
            if (message[i+1] == ']') {
               paramCount = OPEN_ARG_COUNT;
            }
            else if (message[len - 1] == ']') {
               signature.copy(message + i + 1, len - i - 2);
               paramCount = signature.ident().toInt();
               if (paramCount >= OPEN_ARG_COUNT)
                  scope.raiseError(errInvalidSubject, terminal);
            }
            else scope.raiseError(errInvalidSubject, terminal);

            param = i;
            break;
         }
         else if (message[i] >= 65 || (message[i] >= 48 && message[i] <= 57)) {
         }
         else scope.raiseError(errInvalidSubject, terminal);
      }

      if (param != 0) {
         signature.copy(message + subject, param - subject);
      }
      else signature.copy(message + subject);

      if (subject == 0 && paramCount != -1) {
         verb_id = _verbs.get(signature);
         if (verb_id != 0) {
            signature.clear();
         }
      }
   }

   if (verb_id == 0 && paramCount != -1) {
      if (paramCount == 0) {
         verb_id = GET_MESSAGE_ID;
      }
      else verb_id = EVAL_MESSAGE_ID;
   }

   ObjectInfo retVal;
   IdentifierString message;
   if (extensionRef != 0) {
      if (verb_id == 0) {
         scope.raiseError(errInvalidSubject, terminal);
      }

      message.append(scope.moduleScope->module->resolveReference(extensionRef));
      message.append('.');
   }

   if (paramCount == -1) {
      message.append('0');
   }
   else message.append('0' + (char)paramCount);
   message.append('#');
   if (verb_id != 0) {
      message.append((char)(0x20 + verb_id));
   }
   else message.append(0x20);

   if (!emptystr(signature)) {
      message.append('&');
      message.append(signature);
   }

   if (verb_id != 0) {
      if (extensionRef != 0) {
         retVal.kind = okExtMessageConstant;
      }
      else if (paramCount == -1 && emptystr(signature)) {
         retVal.kind = okVerbConstant;
      }
      else retVal.kind = okMessageConstant;
   }
   else retVal.kind = okSignatureConstant;

   retVal.param = scope.moduleScope->module->mapReference(message);

   writeTerminal(writer, node, scope, retVal, mode);

   return retVal;
}

ref_t Compiler :: mapMessage(SNode node, CodeScope& scope, size_t& paramCount)
{
   bool   first = true;
   ref_t  verb_id = 0;

   IdentifierString signature;
   SNode arg = node.findChild(lxMessage);
   if (arg.argument != 0)
      return arg.argument;

   SNode name = arg.findChild(lxPrivate, lxIdentifier, lxReference);
   //HOTFIX : if generated by a script
   if (name == lxNone)
      name = arg;

   if (name == lxNone)
      scope.raiseError(errInvalidOperation, node);

   verb_id = _verbs.get(name.identifier());
   if (verb_id == 0) {
      ref_t id = scope.mapSubject(name, signature);

      if (arg.nextNode() != lxNone) {
         // HOT FIX : strong types cannot be used as a custom verb with a parameter
         if (scope.moduleScope->subjectHints.exist(id))
            scope.raiseError(errStrongTypeNotAllowed, arg);

         first = false;
      }
   }

   arg = arg.nextNode();

   paramCount = 0;
   // if message has generic argument list
   while (test(arg.type, lxObjectMask)) {
      if (paramCount < OPEN_ARG_COUNT)
         paramCount++;

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxPrivate, lxIdentifier, lxReference);
      //HOTFIX : if generated by a script
      if (subject == lxNone)
         subject = arg;

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subjRef = scope.mapSubject(subject, signature);
//      arg.setArgument(subjRef);

      // skip an argument
      arg = arg.nextNode();

      //// HOTFIX : if it was already compiled as open argument list
      //if (arg.type == lxIdle && scope.moduleScope->subjectHints.exist(subjRef, scope.moduleScope->paramsReference)) {
      //   paramCount += OPEN_ARG_COUNT;

      //   break;
      //}
      /*else */if (test(arg.type, lxObjectMask)) {
         // if it is an open argument list
         if (arg.nextNode() == lxNone && subjRef == scope.moduleScope->paramsSubj) {
            // if open argument list virtual subject is used - replace it with []
            if (paramCount > 0)
               scope.raiseError(errInvalidOperation, node);

            paramCount = OPEN_ARG_COUNT;
            signature.truncate(signature.ident().findLast('&', 0));

            break;
         }
         else {
            paramCount++;
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errInvalidOperation, node);

            arg = arg.nextNode();
         }
      }
   }

   if (verb_id == 0) {
      verb_id = paramCount > 0 ? EVAL_MESSAGE_ID : GET_MESSAGE_ID;
   }

   // if signature is presented
   ref_t sign_id = 0;
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   // create a message id
   return encodeMessage(sign_id, verb_id, paramCount);
}

ref_t Compiler :: mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo object)
{
   // check typed extension if the type available
   ref_t type = 0;
   ref_t extRef = 0;

   if (object.type != 0 && scope.moduleScope->extensionHints.exist(messageRef, object.type)) {
      type = object.type;
   }
   else {
      if (scope.moduleScope->extensionHints.exist(messageRef)) {
         ref_t classRef = resolveObjectReference(scope, object);
         if (_logic->isPrimitiveRef(classRef)) {
            classRef = _logic->resolvePrimitiveReference(*scope.moduleScope, classRef);
         }

         // if class reference available - select the possible type
         if (classRef != 0) {
            SubjectMap::Iterator it = scope.moduleScope->extensionHints.start();
            while (!it.Eof()) {
               if (it.key() == messageRef) {
                  if (_logic->isCompatibleWithType(*scope.moduleScope, classRef, *it)) {
                     type = *it;

                     break;
                  }
               }

               it++;
            }
         }
      }
   }

   if (type != 0) {
      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(type);

      if (typeExtensions)
         extRef = typeExtensions->get(messageRef);
   }

   // check generic extension
   if (extRef == 0) {
      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(0);

      if (typeExtensions)
         extRef = typeExtensions->get(messageRef);
   }

   return extRef;
}

void Compiler :: compileBranchingNodes(SyntaxWriter& writer, SNode thenBody, CodeScope& scope, ref_t ifReference, bool loopMode, bool switchMode)
{   
   if (loopMode) {
      writer.newNode(lxElse, ifReference);

      compileBranching(writer, thenBody.findSubNode(lxCode), scope);
      writer.closeNode();
   }
   else {
      SNode thenCode = thenBody.findSubNode(lxCode);

      writer.newNode(lxIf, ifReference);
      compileBranching(writer, thenCode, scope);
      writer.closeNode();

      // HOTFIX : switch mode - ignore else
      if (!switchMode) {
         SNode elseCode = thenBody.firstChild().nextNode();
         if (elseCode != lxNone) {
            writer.newNode(lxElse, 0);
            compileBranching(writer, elseCode, scope);
            writer.closeNode();
         }
      }
   }
}

void Compiler :: compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal)
{
   bool loopMode = test(mode, HINT_LOOP);
   bool switchMode = test(mode, HINT_SWITCH);

   // HOTFIX : in loop expression, else node is used to be similar with branching code
   // because of optimization rules
   ref_t original_id = operator_id;
   if (loopMode) {
      operator_id = operator_id == IF_MESSAGE_ID ? IFNOT_MESSAGE_ID : IF_MESSAGE_ID;
   }

   ref_t ifReference = 0;
   ref_t resolved_operator_id = operator_id;
   // try to resolve the branching operator directly
   if (_logic->resolveBranchOperation(*scope.moduleScope, *this, resolved_operator_id, resolveObjectReference(scope, loperand), ifReference)) {
      // good luck : we can implement branching directly
      compileBranchingNodes(writer, roperandNode, scope, ifReference, loopMode, switchMode);

      writer.insert(loopMode ? lxLooping : lxBranching, switchMode ? -1 : 0);
      writer.closeNode();
   }
   else {
      operator_id = original_id;

      // bad luck : we have to create closure      
      compileObject(writer, roperandNode, scope, HINT_SUBCODE_CLOSURE);

      SNode roperand2Node = roperandNode.firstChild() == lxExpression ? roperandNode.findChild(lxCode) : SNode();
      if (roperand2Node != lxNone) {
         // HOTFIX : else sub code is located in the first expression, while if one - in second layer expression
         compileClosure(writer, roperandNode, scope, HINT_SUBCODE_CLOSURE);

         //compileObject(writer, roperand2Node, scope, HINT_SUBCODE_CLOSURE);

         retVal = compileMessage(writer, roperandNode, scope, loperand, encodeMessage(0, operator_id, 2), 0);
      }
      else retVal = compileMessage(writer, roperandNode, scope, loperand, encodeMessage(0, operator_id, 1), 0);

      if (loopMode) {
         writer.insert(lxLooping);
         writer.closeNode();
      }
   }
}

ObjectInfo Compiler :: compileBranchingOperator(SyntaxWriter& writer, SNode& node, CodeScope& scope, int mode, int operator_id)
{
   ObjectInfo retVal(okObject);

   writer.newBookmark();

   SNode loperandNode = node.firstChild(lxObjectMask);
   ObjectInfo loperand = compileExpression(writer, loperandNode, scope, 0);

   SNode roperandNode = loperandNode.nextNode(lxObjectMask);

   compileBranchingOperand(writer, roperandNode, scope, mode, operator_id, loperand, retVal);

   writer.removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2)
{
   ObjectInfo retVal;

   ref_t loperandRef = resolveObjectReference(scope, loperand);
   ref_t roperandRef = resolveObjectReference(scope, roperand);
   ref_t roperand2Ref = 0;
   ref_t resultClassRef = 0;
   int operationType = 0;

   if (roperand2.kind != okUnknown) {
      roperand2Ref = resolveObjectReference(scope, roperand2);

      operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, roperand2Ref, resultClassRef);
   }
   else operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, resultClassRef);

   // HOTFIX : primitive operations can be implemented only in the method
   // because the symbol implementations do not open a new stack frame
   if (operationType != 0 && resultClassRef != V_FLAG && scope.getScope(Scope::slMethod) == NULL) {
      operationType = 0;
   }

   bool assignMode = false;
   if (operationType != 0) {
      if (loperand.kind == okField || loperand.kind == okOuter) {
         // HOTFIX : for fields replace assigning operators with assigning expression
         switch (operator_id) {
            case APPEND_MESSAGE_ID:
               assignMode = true;
               operator_id = ADD_MESSAGE_ID;
               break;
            case REDUCE_MESSAGE_ID:
               assignMode = true;
               operator_id = SUB_MESSAGE_ID;
               break;
            default:
               break;
         }
         if (assignMode) {
            // once again resolve the primitive operator
            operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, resultClassRef);

            if (loperand.kind == okOuter) {
               InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);

               if (!closure->markAsPresaved(loperand))
                  scope.raiseError(errInvalidOperation, node);
            }
         }
      }
      // if it is a primitive operation
      _logic->injectOperation(writer, *scope.moduleScope, *this, operator_id, operationType, resultClassRef, loperand.type);

      retVal = assignResult(writer, scope, resultClassRef);
   }
   // if not , replace with appropriate method call
   else retVal = compileMessage(writer, node, scope, loperand, _logic->defineOperatorMessage(*scope.moduleScope, operator_id, paramCount, loperandRef, roperandRef, roperand2Ref), HINT_NODEBUGINFO);

   if (assignMode) {
      if (loperand.kind == okField || loperand.kind == okOuter) {
         writer.insertChild(0, lxField, loperand.param);
      }
      writer.insert(lxAssigning);
      writer.closeNode();
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int, int operator_id)
{
   ObjectInfo retVal(okObject);
   int paramCount = 1;

   writer.newBookmark();
   writer.appendNode(lxOperatorAttr);

   ObjectInfo loperand;
   ObjectInfo roperand;
   ObjectInfo roperand2;
   if (operator_id == SET_REFER_MESSAGE_ID) {
      SNode exprNode = node.firstChild(lxObjectMask);

      // HOTFIX : inject the third parameter to the first two ones
      SNode thirdNode = exprNode.nextNode(lxObjectMask);
      SyntaxTree::copyNodeSafe(exprNode.nextNode(lxObjectMask), exprNode.appendNode(lxExpression), true);

      exprNode.nextNode(lxObjectMask) = lxIdle;

      SNode loperandNode = exprNode.firstChild(lxObjectMask);
      loperand = compileExpression(writer, loperandNode, scope, 0);

      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
      roperand = compileExpression(writer, roperandNode, scope, 0);
      roperand2 = compileExpression(writer, roperandNode.nextNode(lxObjectMask), scope, 0);

      node = exprNode;

      paramCount++;
   }
   else {
      SNode loperandNode = node.firstChild(lxObjectMask);
      loperand = compileExpression(writer, loperandNode, scope, 0);

      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
      /*if (roperandNode == lxLocal) {
         // HOTFIX : to compile switch statement
         roperand = ObjectInfo(okLocal, roperandNode.argument);
      }
      else */roperand = compileExpression(writer, roperandNode, scope, 0);
   }

   retVal = compileOperator(writer, node, scope, operator_id, paramCount, loperand, roperand, roperand2);

   writer.removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   SNode operatorNode = node.findChild(lxOperator);
   int operator_id = operatorNode.argument != 0 ? operatorNode.argument : _operators.get(operatorNode.identifier());

   // if it is a new operator
   if (operator_id == -1) {
      return compileNewOperator(writer, node, scope/*, mode*/);
   }
   // if it is branching operators
   else if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
      return compileBranchingOperator(writer, node, scope, mode, operator_id);
   }
   else return compileOperator(writer, node, scope, mode, operator_id);
}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode)
{
   ObjectInfo retVal(okObject);

   LexicalType operation = lxCalling;
   int argument = messageRef;

   int signRef = getSignature(messageRef);
   int paramCount = getParamCount(messageRef);

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target);
   bool dispatchCall = false;
   _CompilerLogic::ChechMethodInfo result;
   int callType = _logic->resolveCallType(*scope.moduleScope, classReference, messageRef, result);
   if (result.found) {
      retVal.param = result.outputReference;
   }

   if (target.kind == okThisParam && callType == tpPrivate) {
      messageRef = overwriteVerb(messageRef, PRIVATE_MESSAGE_ID);

      callType = tpSealed;
   }
   else if (classReference == scope.moduleScope->signatureReference) {
      dispatchCall = test(mode, HINT_EXTENSION_MODE);
   }
   else if (classReference == scope.moduleScope->messageReference) {
      dispatchCall = test(mode, HINT_EXTENSION_MODE);
   }
   else if (target.kind == okSuper) {
      // parent methods are always sealed
      callType = tpSealed;
   }

   if (dispatchCall) {
      operation = lxDirectCalling;
      argument = encodeVerb(DISPATCH_MESSAGE_ID);

      writer.appendNode(lxOvreriddenMessage, messageRef);
   }
   else if (callType == tpClosed || callType == tpSealed) {
      operation = callType == tpClosed ? lxSDirctCalling : lxDirectCalling;
      argument = result.withOpenArgDispatcher ? overwriteParamCount(messageRef, OPEN_ARG_COUNT) : messageRef;

      if (result.stackSafe)
         writer.appendNode(lxStacksafeAttr);

      if (result.embeddable)
         writer.appendNode(lxEmbeddableAttr);
   }
//   else {
//      // if the sealed / closed class found and the message is not supported - warn the programmer and raise an exception
//      if (result.found && !result.withCustomDispatcher && callType == tpUnknown)
//         node.appendNode(lxNotFoundAttr);
//   }

   if (classReference)
      writer.appendNode(lxCallTarget, classReference);
   
   if (result.outputReference)
      writer.appendNode(lxTarget, result.outputReference);

   if (!test(mode, HINT_NODEBUGINFO)) {
      // set a breakpoint
      writer.newNode(lxBreakpoint, dsStep);

      SNode messageNode = node.findChild(lxMessage).findChild(lxIdentifier, lxPrivate);
      if (messageNode != lxNone) {
         writeTerminalInfo(writer, messageNode);
      }

      writer.closeNode();
   }   

   // define the message target if required
   if (target.kind == okConstantRole || target.kind == okSubject) {
      writer.newNode(lxOverridden);
      writer.newNode(lxExpression);
      writeTerminal(writer, node, scope, target, 0);
      writer.closeNode();
      writer.closeNode();
   }

   // inserting calling expression
   writer.insert(operation, argument);
   writer.closeNode();   

   // the result of get&type message should be typed
   if (retVal.param == 0 && paramCount == 0 && getVerb(messageRef) == GET_MESSAGE_ID) {
      if (scope.moduleScope->subjectHints.exist(signRef)) {
         retVal.param = scope.moduleScope->subjectHints.get(signRef);
         retVal.type = signRef;
      }
   }

   return retVal;
}

bool Compiler :: convertObject(SyntaxWriter& writer, ModuleScope& scope, ref_t targetRef, ref_t targetType, ref_t sourceRef, ref_t sourceType)
{
   if (!_logic->isCompatible(scope, (targetRef == 0 && targetType != 0) ? scope.subjectHints.get(targetType) : targetRef, sourceRef)) {
      // if it can be boxed / implicitly converted
      if (!_logic->injectImplicitConversion(writer, scope, *this, targetRef, sourceRef, sourceType))
         return typecastObject(writer, targetType);
   }
   return true;
}

bool Compiler :: typecastObject(SyntaxWriter& writer, ref_t targetType)
{
   if (targetType != 0) {
      writer.insert(lxCalling, encodeMessage(targetType, GET_MESSAGE_ID, 0));
      writer.closeNode();

      return true;
   }
   else return false;
}

ObjectInfo Compiler :: compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   ObjectInfo target;

   int paramMode = 0;

   SNode arg = node.firstChild();
   if (test(mode, HINT_RESENDEXPR) && (arg == lxMessage && arg.nextNode() != lxMessage)) {
      arg = arg.nextNode();
   }

   // compile the message target and generic argument list
   while (arg != lxMessage && arg != lxNone) {
      if (test(arg.type, lxObjectMask)) {
         if (target.kind == okUnknown) {
            // HOTFIX : to handle alt expression
            if (arg.type == lxLocal) {
               target = ObjectInfo(okLocal, arg.argument);
               writeTerminal(writer, arg, scope, target, 0);
            }
            // HOTFIX : to handle resend expression
            else if (arg.type == lxResult) {
               target = ObjectInfo(okObject);
            }
            else target = compileExpression(writer, arg, scope, paramMode);

            // HOTFIX : recognize external operation
            if (target.kind == okExternal) {
               paramMode |= HINT_EXTERNALOP;
            }

            // HOTFIX : skip the prime message
            arg = arg.nextNode();

            if (test(mode, HINT_RESENDEXPR)) {
               // HOTFIX : support several generic arguments for try mode
               if (arg == lxExpression)
                  continue;
            }
            else if (arg == lxExtension)
               // HOTFIX : skip the extension node
               arg = arg.nextNode();
         }
         else compileExpression(writer, arg, scope, paramMode);
      }

      arg = arg.nextNode();
   }

   // resend expression starts with the message, so it should be skipped
   if (test(mode, HINT_RESENDEXPR))
      arg = arg.nextNode();

   // if message has named argument list
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxPrivate, lxIdentifier, lxReference);
      //HOTFIX : if generated by a script
      if (subject == lxNone)
         subject = arg;

      ref_t subjectRef = scope.mapSubject(subject);
//      arg.setArgument(subjectRef);

      arg = arg.nextNode();

      // compile an argument
      if (test(arg.type, lxObjectMask)) {
         // if it is an open argument list
         if (arg.nextNode() != lxMessage && subjectRef == scope.moduleScope->paramsSubj) {
            if (arg == lxExpression) {
               SNode argListNode = arg.firstChild();
               while (argListNode != lxNone) {
                  compileExpression(writer, argListNode, scope, paramMode);

                  argListNode = argListNode.nextNode();
               }
            }
            else {
               writer.newBookmark();
               ObjectInfo argListParam = compileExpression(writer, arg, scope, paramMode);
               if (argListParam.kind == okParams) {
                  writer.insert(lxArgUnboxing);
                  writer.closeNode();
               }
               writer.removeBookmark();
            }
         }
         else {
            writer.newBookmark();

            int exprMode = paramMode;
            ref_t subjClassRef = subjectRef != 0 ? scope.moduleScope->subjectHints.get(subjectRef) : 0;
            // HOTFIX : generic parameter object MUST be boxed
            if (subjClassRef == 0)
               exprMode |= HINT_DYNAMIC_OBJECT;

            ObjectInfo param = compileExpression(writer, arg, scope, exprMode);
            if (subjectRef != 0)
               if (!convertObject(writer, *scope.moduleScope, subjClassRef, subjectRef, resolveObjectReference(scope, param), param.type))
                  scope.raiseError(errInvalidOperation, arg);

            // HOTFIX : externall operation arguments should be inside expression node
            if (test(paramMode, HINT_EXTERNALOP)) {
               writer.appendNode(lxExtArgumentType, subjectRef);

               writer.insert(lxExpression);
               writer.closeNode();
            }

            writer.removeBookmark();

            arg = arg.nextNode();
         }
      }
   }

   return target;
}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   writer.newBookmark();

   size_t paramCount = 0;
   ObjectInfo retVal;
   ObjectInfo target = compileMessageParameters(writer, node, scope, mode & HINT_RESENDEXPR);

   if (test(mode, HINT_TRY_MODE)) {
      writer.insertChild(0, lxResult, 0);

      target = ObjectInfo(okObject);
   }
   else if (test(mode, HINT_ALT_MODE)) {
      writer.insertChild(0, lxCurrent, 0);

      target = ObjectInfo(okObject);
   }
   else if (test(mode, HINT_RESENDEXPR)) {
      writer.insertChild(0, lxThisLocal, 1);

      target = ObjectInfo(okThisParam, 1);
   }      

   //   bool externalMode = false;
   if (target.kind == okExternal) {
      retVal = compileExternalCall(writer, node, scope);
   }
   else {
      ref_t  messageRef = mapMessage(node, scope, paramCount/*, argsUnboxing*/);

      if (target.kind == okInternal) {
         retVal = compileInternalCall(writer, node, scope, messageRef, target);
      }
      else {
         ref_t extensionRef = mapExtension(scope, messageRef, target);

         if (extensionRef != 0) {
            //HOTFIX: A proper method should have a precedence over an extension one
            if (checkMethod(*scope.moduleScope, resolveObjectReference(scope, target), messageRef) == tpUnknown) {
               target = ObjectInfo(okConstantRole, extensionRef, 0, target.type);
            }
         }

         retVal = compileMessage(writer, node, scope, target, messageRef, mode);
      }
   }

   writer.removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   writer.newBookmark();

   ObjectInfo retVal;
   LexicalType operationType = lxAssigning;
   int operand = 0;

   SNode exprNode = node;
   SNode operation = node.findChild(lxMessage, lxExpression, lxAssign);
   if (operation == lxExpression) {
      exprNode = operation;
      operation = exprNode.findChild(lxMessage, lxOperator);
   }
   
   if (operation == lxMessage) {
      // try to figure out if it is property assignging
      SNode firstToken = exprNode.firstChild(lxObjectMask);
      ObjectInfo tokenInfo = scope.mapObject(firstToken);
      //ref_t attrRef = scope.mapSubject(firstToken);
      if (tokenInfo.kind != okUnknown) {
         // if it is shorthand property settings
         SNode name = operation.findChild(lxIdentifier, lxPrivate);
         ref_t subject = scope.mapSubject(name);
         //HOTFIX : support lexical subjects
         if (subject == 0)
            subject = scope.moduleScope->module->mapSubject(name.identifier(), false);

         ref_t messageRef = encodeMessage(subject, SET_MESSAGE_ID, 1);

         // compile target
         // NOTE : compileMessageParameters does not compile the parameter, it'll be done in the next statement
         ObjectInfo target = compileMessageParameters(writer, exprNode, scope);

         // compile the parameter
         SNode sourceNode = exprNode.nextNode(lxObjectMask);
         ObjectInfo source = compileExpression(writer, sourceNode, scope, HINT_DYNAMIC_OBJECT);

         retVal = compileMessage(writer, node, scope, target, messageRef, HINT_NODEBUGINFO);

         operationType = lxNone;
      }
   //   else if (_logic->recognizeNewLocal(exprNode)) {
   //      // if it is variable declaration
   //      declareVariable(writer, exprNode, scope);
   //      declareExpression(writer, node, scope, 0);

   //      operationType = lxNone;
   //   }
      else scope.raiseError(errUnknownObject, firstToken);
   }
   // if it setat operator
   else if (operation == lxOperator) {
      retVal = compileOperator(writer, node, scope, mode, SET_REFER_MESSAGE_ID);

      operationType = lxNone;
   }
   else {
      SNode targetNode = node.firstChild(lxObjectMask);
      retVal = compileExpression(writer, targetNode, scope, mode | HINT_NOBOXING);

      ref_t targetRef = resolveObjectReference(scope, retVal);
      if (retVal.kind == okLocalAddress) {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef);
         if (size != 0) {
            operand = size;
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else if (retVal.kind == okFieldAddress) {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef);
         if (size != 0) {
            operand = size;
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else if (retVal.kind == okLocal || retVal.kind == okField || retVal.kind == okOuterField || retVal.kind == okStaticField) {
      }
      else if (/*retVal.kind == okParam || */retVal.kind == okOuter) {
//         // Compiler magic : allowing to assign byref / variable parameter
//         if (_logic->isVariable(*scope.moduleScope, targetRef)) {
//            _logic->injectVariableAssigning(writer, *scope.moduleScope, *this, targetRef, retVal.type, operand, retVal.kind == okParam);
//
//            retVal.kind = (retVal.kind == okParam) ? okParamField : okOuterField;
//         }
         // Compiler magic : allowing to assign outer local variables
//         else if (retVal.kind == okOuter) {
            InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);

            if (!closure->markAsPresaved(retVal))
               scope.raiseError(errInvalidOperation, node);
//         }
//         else scope.raiseError(errInvalidOperation, node);
      }
      else scope.raiseError(errInvalidOperation, node);

      writer.newBookmark();

      SNode sourceNode = targetNode.nextNode(lxObjectMask);
      ObjectInfo source = compileAssigningExpression(writer, sourceNode, scope);

//      // assigning primitive array
//      if (_logic->isPrimitiveArray(targetRef)) {
//         // HOTFIX : allowing to declare the primitive array 
//         if (source.kind == okIntConstant && source.extraparam == 0) {
//            operationType = lxIdle;
//         }
//         else scope.raiseError(errInvalidOperation, node);
//      }
      /*else */if (!convertObject(writer, *scope.moduleScope, targetRef, retVal.type, resolveObjectReference(scope, source), source.type))
         scope.raiseError(errInvalidOperation, node);

      writer.removeBookmark();
   }

   if (operationType != lxNone) {
      writer.insert(operationType, operand);
      writer.closeNode();
   }

   writer.removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ref_t extensionRef = 0;
   ref_t extensionType = 0;

   writer.newBookmark();

   ModuleScope* moduleScope = scope.moduleScope;
   ObjectInfo   role;

   SNode roleNode = node.findChild(lxExtension);
   // check if the extension can be used as a static role (it is constant)
   SNode roleTerminal = roleNode.firstChild(lxTerminalMask);
   if (roleTerminal != lxNone) {
      int flags = 0;

      role = scope.mapObject(roleTerminal);
      if (role.kind == okSymbol || role.kind == okConstantSymbol) {
         ref_t classRef = role.kind == okConstantSymbol ? role.extraparam : role.param;

         // if the symbol is used inside itself
         if (classRef == scope.getClassRefId()) {
            flags = scope.getClassFlags();
         }
         // otherwise
         else {
            ClassInfo roleClass;
            moduleScope->loadClassInfo(roleClass, moduleScope->module->resolveReference(classRef));

            flags = roleClass.header.flags;
            //HOTFIX : typecast the extension target if required
            if (test(flags, elExtension) && roleClass.fieldTypes.exist(-1)) {
               extensionRef = roleClass.fieldTypes.get(-1).value1;
               extensionType = roleClass.fieldTypes.get(-1).value2;
            }
         }
      }
      // if the symbol VMT can be used as an external role
      if (test(flags, elStateless)) {
         role = ObjectInfo(okConstantRole, role.param);
      }
   }

   // if it is a generic role
   if (role.kind != okConstantRole && role.kind != okSubject) {
      writer.newNode(lxOverridden);
      role = compileExpression(writer, roleNode, scope, 0);
      writer.closeNode();
   }
   
   ObjectInfo retVal = compileExtensionMessage(writer, node, scope, role, extensionRef, extensionType);

   writer.removeBookmark();

   return retVal;
}

// NOTE : targetRef refers to the type for the typified extension method
ObjectInfo Compiler :: compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo role, ref_t targetRef, ref_t targetType)
{
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount);
   ObjectInfo object = compileMessageParameters(writer, node, scope, HINT_EXTENSION_MODE);

   if (targetRef != 0) {
      convertObject(writer, *scope.moduleScope, targetRef, targetType, resolveObjectReference(scope, object), object.type);
   }

   return compileMessage(writer, node, scope, role, messageRef, HINT_EXTENSION_MODE);
}

bool Compiler :: declareActionScope(SNode& node, ClassScope& scope, SNode argNode, ActionScope& methodScope, int mode/*, bool alreadyDeclared*/)
{
   bool lazyExpression = test(mode, HINT_LAZY_EXPR);

   methodScope.message = encodeVerb(EVAL_MESSAGE_ID);

   if (argNode != lxNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope);

      //node = node.select(nsSubCode);
   }

   ref_t parentRef = scope.info.header.parentRef;
   if (lazyExpression) {
      parentRef = scope.moduleScope->getBaseLazyExpressionClass();
   }
   else {
      ref_t actionRef = scope.moduleScope->actionHints.get(methodScope.message);
      if (actionRef)
         parentRef = actionRef;
   }

   compileParentDeclaration(SNode(), scope, parentRef);

   return lazyExpression;
}

void Compiler :: compileAction(SNode node, ClassScope& scope, SNode argNode, int mode)
{
   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   writer.newNode(lxClass, scope.reference);

   ActionScope methodScope(&scope);
   bool lazyExpression = declareActionScope(node, scope, argNode, methodScope, mode);   

   scope.include(methodScope.message);

   // HOTFIX : if the clousre emulates code brackets
   if (test(mode, HINT_SUBCODE_CLOSURE))
      methodScope.subCodeMode = true;

//   if (test(mode, HINT_SINGLETON))
//      methodScope.singletonMode = true;

   // if it is single expression
   if (!lazyExpression) {
      initialize(scope, methodScope);

      compileActionMethod(writer, node, methodScope);
   }
   else compileLazyExpressionMethod(writer, node, methodScope);

   generateClassDeclaration(SNode(), scope, false);

   writer.closeNode();

   scope.save();

   generateClassImplementation(expressionTree.readRoot(), scope/*, test(scope.info.header.flags, elClosed)*/);
}

void Compiler :: compileNestedVMT(SNode node, InlineClassScope& scope)
{
   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   // check if the class was already compiled
   if (!node.argument) {
      compileParentDeclaration(node, scope);

      declareVMT(node, scope);
      generateClassDeclaration(node, scope, false, true);

      scope.save();
   }
   else scope.moduleScope->loadClassInfo(scope.info, scope.moduleScope->module->resolveReference(node.argument), false);

   writer.newNode(lxClass, scope.reference);

   compileVMT(writer, node, scope);

   writer.closeNode();

   // set flags once again
   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info, false);
   scope.save();

   generateClassImplementation(expressionTree.readRoot(), scope);
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope)
{
   if (test(scope.info.header.flags, elStateless)) {
      writer.appendNode(lxConstantSymbol, scope.reference);
      //ownerScope.writer->appendNode(lxTarget, scope.reference);

      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, scope.reference, scope.reference/*, scope.moduleScope->defineType(scope.reference)*/);
   }
   else if (test(scope.info.header.flags, elDynamicRole)) {
      scope.raiseError(errInvalidInlineClass, node);

      // idle return
      return ObjectInfo();
   }
   else {
      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         writer.newNode(lxStruct, scope.info.size);
         writer.appendNode(lxTarget, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node);
      }
      else {
         // dynamic normal symbol
         writer.newNode(lxNested, scope.info.fields.Count());
         writer.appendNode(lxTarget, scope.reference);
      }

      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      //int toFree = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         //SNode member = node.appendNode().appendNode(lxIdle);

         writer.newNode((*outer_it).preserved ? lxOuterMember : lxMember, (*outer_it).reference);
         writeTerminal(writer, node, ownerScope, info, 0);
         writer.closeNode();

         outer_it++;
      }

      if (scope.returningMode) {
         // injecting returning code if required
         InlineClassScope::Outer retVal = scope.outers.get(RETVAL_VAR);

         writer.newNode(lxCode);
         writer.newNode(lxExpression);
         writer.newNode(lxBranching);

         writer.newNode(lxExpression);
         writer.appendNode(lxCurrent);
         writer.appendNode(lxResultField, retVal.reference); // !! current field
         writer.closeNode();

         writer.newNode(lxIfNot, -1);
         writer.newNode(lxCode);
         writer.newNode(lxReturning);
         writer.appendNode(lxResult);
         writer.closeNode();
         writer.closeNode();
         writer.closeNode();

         writer.closeNode();
         writer.closeNode();
         writer.closeNode();
      }

      ref_t implicitConstructor = encodeVerb(PRIVATE_MESSAGE_ID);
      if (scope.info.methods.exist(implicitConstructor, true)) {
         // if implicit constructor is declared - it should be automatically called
         writer.appendNode(lxOvreriddenMessage, implicitConstructor);
      }

      writer.closeNode();

      return ObjectInfo(okObject, scope.reference);
   }
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode)
{
   ref_t nestedRef = 0;
   bool singleton = false;
   if (test(mode, HINT_ROOTSYMBOL)) {
      SymbolScope* owner = (SymbolScope*)ownerScope.getScope(Scope::slSymbol);
      if (owner) {
         nestedRef = owner->reference;
         // HOTFIX : symbol should refer to self and $self for singleton closure
         singleton = node.existChild(lxCode);
      }         
   }
   if (!nestedRef)
      nestedRef = ownerScope.moduleScope->mapNestedExpression();

   InlineClassScope scope(&ownerScope, nestedRef);

   // if it is a lazy expression / multi-statement closure without parameters
   SNode argNode = node.firstChild();
   if (node == lxLazyExpression) {
      scope.closureMode = true;

      compileAction(node, scope, SNode(), HINT_LAZY_EXPR);
   }
   else if (argNode == lxCode) {
      scope.closureMode = true;

      compileAction(node, scope, SNode(), singleton ? mode | HINT_SINGLETON : mode);
   }
   else if (node.existChild(lxCode)) {
      scope.closureMode = true;

      SNode codeNode = node.findChild(lxCode);

      // if it is a closure / lambda function with a parameter
      int actionMode = mode/* | HINT_CLOSURE*/;
      if (singleton)
         actionMode |= HINT_SINGLETON;

      compileAction(node, scope, node.findChild(lxIdentifier, lxPrivate, lxMethodParameter, lxClosureMessage), actionMode);

      // HOTFIX : hide code node because it is no longer required
      codeNode = lxIdle;
   }
   // if it is a nested class
   else compileNestedVMT(node, scope);

   return compileClosure(writer, node, ownerScope, scope);
}

ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ref_t parentRef = scope.moduleScope->arrayReference;
   SNode parentNode = node.findChild(lxIdentifier, lxPrivate, lxReference);
   if (parentNode != lxNone) {
      parentRef = scope.moduleScope->mapTerminal(parentNode, true);
      if (parentRef == 0)
         scope.raiseError(errUnknownObject, node);
   }

   return compileCollection(writer, node, scope, parentRef);
}

ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t vmtReference)
{
   if (vmtReference == 0)
      vmtReference = scope.moduleScope->superReference;

   int counter = 0;

   writer.newBookmark();

   // all collection memebers should be created before the collection itself
   SNode current = node.findChild(lxExpression);
   while (current != lxNone) {
      writer.newNode(lxMember, counter);
      compileExpression(writer, current, scope, 0);
      writer.closeNode();

      current = current.nextNode();
      counter++;
   }

   writer.appendNode(lxTarget, vmtReference);
   writer.insert(lxNested, counter);
   writer.closeNode();

   writer.removeBookmark();

   return ObjectInfo(okObject, vmtReference);
}

ObjectInfo Compiler :: compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   bool typecasting = false;
   bool converting = false;
   ref_t subjectRef = 0;
   ref_t targetRef = 0;
   if (test(mode, HINT_ROOT)) {
      // type cast returning value if required
      int paramCount;
      ref_t verb;
      decodeMessage(scope.getMessageID(), subjectRef, verb, paramCount);
      if (classScope->info.methodHints.exist(Attribute(scope.getMessageID(), maReference))) {
         targetRef = classScope->info.methodHints.get(Attribute(scope.getMessageID(), maReference));
         subjectRef = classScope->info.methodHints.get(Attribute(scope.getMessageID(), maType));
         converting = true;
      }
      else if (verb == GET_MESSAGE_ID && paramCount == 0) {
         typecasting = true;
         targetRef = scope.moduleScope->subjectHints.get(subjectRef);
      }
   }

   writer.newBookmark();

   ObjectInfo info = compileExpression(writer, node, scope, mode);

   if (converting || typecasting)
      convertObject(writer, *scope.moduleScope, targetRef, subjectRef, resolveObjectReference(scope, info), info.type);

   // HOTFIX : implementing closure exit
   if (test(mode, HINT_ROOT)) {
      ObjectInfo retVar = scope.mapTerminal(RETVAL_VAR);
      if (retVar.kind != okUnknown) {
         writer.insertChild(0, lxField, retVar.param);

         writer.insert(lxAssigning);
         writer.closeNode();
      }
   }

   writer.removeBookmark();

   return info;
}

ObjectInfo Compiler :: compileNewOperator(SyntaxWriter& writer, SNode node, CodeScope& scope/*, int mode*/)
{
   ObjectInfo retVal(okObject);

   if (node.nextNode() != lxNone)
      scope.raiseError(errInvalidOperation, node);

   SNode objectNode = node.firstChild(lxTerminalMask);

   //if (objectNode.nextNode(lxObjectMask))

   retVal.type = scope.mapSubject(objectNode);
   if (retVal.type != 0 && scope.moduleScope->subjectHints.get(retVal.type) == 0)
      retVal.type = 0; // HOTFIX : ignore weak types

   ref_t loperand = scope.moduleScope->subjectHints.get(retVal.type);
   ref_t roperand = resolveObjectReference(scope, compileExpression(writer, objectNode.nextNode(lxObjectMask), scope, 0));
   ref_t targetRef = 0;
   int operationType = _logic->resolveNewOperationType(*scope.moduleScope, loperand, roperand, targetRef);

   if (operationType != 0) {
      // if it is a primitive operation
      _logic->injectNewOperation(writer, *scope.moduleScope, operationType, retVal.type, targetRef);

      retVal = assignResult(writer, scope, targetRef, retVal.type);
   }
   else scope.raiseError(errInvalidOperation, node);

   return retVal;
}

void Compiler :: compileTrying(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   writer.newBookmark();

   bool catchNode = false;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         compileExpression(writer, current, scope, catchNode ? HINT_TRY_MODE | HINT_RESENDEXPR : 0);

         catchNode = true;
      }

      current = current.nextNode();
   }

   writer.insert(lxTrying);
   writer.closeNode();

   writer.removeBookmark();
}

void Compiler :: compileAltOperation(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   // extract the expression target
   SNode firstExpr = node.firstChild(lxObjectMask);
   SNode targetNode = firstExpr.firstChild(lxObjectMask);

   writer.newBookmark();

   // inject a temporal variable
   int tempLocal = scope.newLocal();
   writer.newNode(lxAssigning);
   writer.appendNode(lxLocal, tempLocal);
   compileExpression(writer, targetNode, scope, 0);
   writer.closeNode();

   targetNode.set(lxLocal, tempLocal);

   writer.newBookmark();

   bool catchNode = false;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxExprMask)) {
         compileExpression(writer, current, scope, catchNode ? HINT_ALT_MODE | HINT_RESENDEXPR : 0);

         catchNode = true;
      }

      current = current.nextNode();
   }

   writer.insert(lxAlt);
   writer.closeNode();

   writer.removeBookmark();

   // inject a nested expression
   writer.insert(lxAltExpression);
   writer.closeNode();

   writer.removeBookmark();
}

ObjectInfo Compiler :: compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   ObjectInfo objectInfo;
   if (node == lxAlt) {
      compileAltOperation(writer, node, scope);

      objectInfo = ObjectInfo(okObject);
   }
   else if (node == lxTrying) {
      compileTrying(writer, node, scope);

      objectInfo = ObjectInfo(okObject);
   }
   else {
      SNode current = node.findChild(lxAssign, lxExtension, lxMessage, lxOperator, lxSwitching);
      switch (current.type) {
         case lxAssign:
            objectInfo = compileAssigning(writer, node, scope, mode);
            break;
         case lxMessage:
            objectInfo = compileMessage(writer, node, scope, mode);
            break;
         case lxSwitching:
            compileSwitch(writer, current, scope);

            objectInfo = ObjectInfo(okObject);
            break;
         case lxExtension:
            objectInfo = compileExtension(writer, node, scope);
            break;
         case lxOperator:
            objectInfo = compileOperator(writer, node, scope, mode);
            break;
         default:
            current = node.firstChild(lxObjectMask);
            SNode nextChild = current.nextNode();

            if ((current == lxExpression || current == lxAlt || current == lxTrying) && nextChild == lxNone) {
               // if it is a nested expression
               objectInfo = compileExpression(writer, current, scope, mode);
            }
            else if (test(current.type, lxTerminalMask) && nextChild == lxNone) {
               objectInfo = compileObject(writer, current, scope, mode);
            }
            else objectInfo = compileObject(writer, node, scope, mode);
      }
   }
   return objectInfo;
}

ObjectInfo Compiler :: compileAssigningExpression(SyntaxWriter& writer, SNode assigning, CodeScope& scope)
{
   writer.newNode(lxExpression);
   //writer.appendNode(lxBreakpoint, dsStep);

   ObjectInfo objectInfo = compileExpression(writer, assigning, scope, 0);

   writer.closeNode();

   return objectInfo;
}

ObjectInfo Compiler :: compileBranching(SyntaxWriter& writer, SNode thenCode, CodeScope& scope)
{
   CodeScope subScope(&scope);

   writer.newNode(lxCode);

   SNode expr = thenCode.firstChild(lxObjectMask);
   if (expr == lxEOF || expr.nextNode() != lxNone) {
      compileCode(writer, thenCode, subScope);
   }
   // if it is inline action
   else compileRetExpression(writer, expr, scope, 0);

   // preserve the allocated space
   scope.level = subScope.level;

   writer.closeNode();

   return ObjectInfo(okObject);
}

void Compiler :: compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   // find inner expression
   SNode expr = node;
   while (expr.findChild(lxMessage, lxAssign, lxOperator) == lxNone) {
      expr = expr.findChild(lxExpression);
   }

   compileExpression(writer, expr, scope, HINT_LOOP);
}

ObjectInfo Compiler :: compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxExpression:
            writer.newNode(lxExpression);
            writer.appendNode(lxBreakpoint, dsStep);
            compileExpression(writer, current, scope, HINT_ROOT);
            writer.closeNode();
            break;
         case lxLoop:
            writer.newNode(lxExpression);
            writer.appendNode(lxBreakpoint, dsStep);
            compileLoop(writer, current, scope);
            writer.closeNode();
            break;
         case lxReturning:
         {
            needVirtualEnd = false;

            writer.newNode(lxReturning);
            writer.appendNode(lxBreakpoint, dsStep);
            retVal = compileRetExpression(writer, current, scope, HINT_ROOT);
            writer.closeNode();

            break;
         }
         case lxVariable:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileVariable(writer, current, scope);
            break;
         case lxExtern:
            writer.newNode(lxExternFrame);
            compileCode(writer, current.findSubNode(lxCode), scope);
            writer.closeNode();
            break;
         case lxEOF:
            needVirtualEnd = false;
            writer.newNode(lxBreakpoint, dsEOP);
            writeTerminalInfo(writer, current);
            writer.closeNode();
            break;
      }

      scope.freeSpace();

      current = current.nextNode();
   }

   if (needVirtualEnd) {
      writer.appendNode(lxBreakpoint, dsVirtualEnd);
   }

   return retVal;
}

void Compiler :: compileExternalArguments(SNode node, ModuleScope& moduleScope, WarningScope& warningScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         optimizeExpressionTree(current, moduleScope, warningScope, HINT_NOBOXING);

         ref_t argType = current.findChild(lxExtArgumentType).argument;

         ref_t classReference = moduleScope.subjectHints.get(argType);
         if (classReference) {
            ClassInfo classInfo;
            _logic->defineClassInfo(moduleScope, classInfo, classReference);

            ref_t primitiveRef = _logic->retrievePrimitiveReference(moduleScope, classInfo);
            switch (primitiveRef) {
               case V_INT32:
               case V_SIGNATURE:
               case V_MESSAGE:
               case V_VERB:
                  current.set(_logic->isVariable(classInfo) ? lxExtArgument : lxIntExtArgument, 0);
                  break;
               case V_SYMBOL:
               {
                  current.set(lxExtInteranlRef, 0);
                  // HOTFIX : ignore call operation
                  SNode callNode = current.findChild(lxCalling);
                  callNode.set(lxExpression, 0);
                  break;
               }
               default:
                  if (test(classInfo.header.flags, elStructureRole)) {
                     current.set(lxExtArgument, 0);
                  }
                  else if (test(classInfo.header.flags, elWrapper)) {
                     //HOTFIX : allow to pass a normal object
                     current.set(lxExtArgument, 0);
                  }
                  else moduleScope.raiseError(errInvalidOperation, current);
                  break;
            }
         }
         else moduleScope.raiseError(errInvalidOperation, current);
      }

      current = current.nextNode();
   }
}

ObjectInfo Compiler :: compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ObjectInfo retVal(okExternal);

   ModuleScope* moduleScope = scope.moduleScope;

//   bool rootMode = test(mode, HINT_ROOT);
   bool stdCall = false;
   bool apiCall = false;

   SNode targetNode = node.firstChild(lxTerminalMask);
   // HOTFIX : comment out dll reference
   targetNode = lxIdle;

   //SNode messageNode = node.findChild(lxMessage);
   writer.appendNode(lxBreakpoint, dsAtomicStep);

   ident_t dllAlias = targetNode.identifier();
   ident_t functionName = node.findChild(lxMessage).firstChild(lxTerminalMask).identifier();

   ident_t dllName = NULL;
   if (dllAlias.compare(EXTERNAL_MODULE)) {
      // if run time dll is used
      dllName = RTDLL_FORWARD;

      if (functionName.compare(COREAPI_MASK, COREAPI_MASK_LEN))
         apiCall = true;
   }
   else dllName = moduleScope->project->resolveExternalAlias(dllAlias + strlen(EXTERNAL_MODULE) + 1, stdCall);

   // legacy : if dll is not mapped, use the name directly
   if (emptystr(dllName))
      dllName = dllAlias + strlen(EXTERNAL_MODULE) + 1;

   ReferenceNs name;
   if (!apiCall) {
      name.copy(DLL_NAMESPACE);
      name.combine(dllName);
      name.append(".");
      name.append(functionName);
   }
   else {
      name.copy(NATIVE_MODULE);
      name.combine(CORE_MODULE);
      name.combine(functionName);
   }

   ref_t reference = moduleScope->module->mapReference(name);

//   if (!rootMode)
//      scope.writer->appendNode(lxTarget, -1);

   // To tell apart coreapi calls, the name convention is used
   if (apiCall) {
      writer.insert(lxCoreAPICall, reference);
   }
   else writer.insert(stdCall ? lxStdExternalCall : lxExternalCall, reference);
   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ObjectInfo routine)
{
   ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   int paramCount;
   ref_t sign_ref, verb_id;
   decodeMessage(message, sign_ref, verb_id, paramCount);

   virtualReference.append('0' + (char)paramCount);
   virtualReference.append('#');
   virtualReference.append((char)(0x20 + verb_id));

   if (sign_ref != 0) {
      virtualReference.append('&');
      virtualReference.append(moduleScope->module->resolveSubject(sign_ref));
   }

   writer.insert(lxInternalCall, moduleScope->module->mapReference(virtualReference));
   writer.closeNode();

   SNode targetNode = node.firstChild(lxTerminalMask);
   // HOTFIX : comment out dll reference
   targetNode = lxIdle;

   return ObjectInfo(okObject);
}

int Compiler :: allocateStructure(bool bytearray, int& allocatedSize, int& reserved)
{
   if (bytearray) {
      // plus space for size
      allocatedSize = ((allocatedSize + 3) >> 2) + 2;
   }
   else allocatedSize = (allocatedSize + 3) >> 2;

   int retVal = reserved;
   reserved += allocatedSize;

   // the offset should include frame header offset
   retVal = -2 - retVal;

   // reserve place for byte array header if required
   if (bytearray)
      retVal -= 2;

   return retVal;
}

bool Compiler :: allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand)
{
   if (size <= 0)
      return false;

   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
   if (methodScope == NULL)
      return false;

   int offset = allocateStructure(bytearray, size, scope.reserved);

   // if it is not enough place to allocate
   if (methodScope->reserved < scope.reserved) {
      methodScope->reserved += size;
   }

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
}

ref_t Compiler :: declareInlineArgumentList(SNode arg, MethodScope& scope)
{
   IdentifierString signature;

   ref_t sign_id = 0;

   // if method has generic (unnamed) argument list
   while (arg == lxMethodParameter || arg == lxIdentifier || arg == lxPrivate) {
      SNode terminalNode = arg;
      if (terminalNode == lxMethodParameter) {
         terminalNode = terminalNode.findChild(lxIdentifier, lxPrivate);
      }

      ident_t terminal = terminalNode.identifier();
      int index = 1 + scope.parameters.Count();
      scope.parameters.add(terminal, Parameter(index));

      arg = arg.nextNode();
   }
   bool first = true;
   while (arg == lxMessage || arg == lxClosureMessage) {
      SNode subject = arg.findChild(lxIdentifier, lxPrivate);

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.mapSubject(subject, signature);

      // declare method parameter
      arg = arg.nextNode();

      if (arg == lxMethodParameter) {
         ident_t name = arg.findChild(lxIdentifier, lxPrivate).identifier();

         // !! check duplicates
         if (scope.parameters.exist(name))
            scope.raiseError(errDuplicatedLocal, arg);

         int index = 1 + scope.parameters.Count();
         ref_t paramRef = scope.moduleScope->subjectHints.get(subj_ref);
         int size = subj_ref != 0 ? _logic->defineStructSize(*scope.moduleScope,
            paramRef, 0, true) : 0;

         scope.parameters.add(name, Parameter(index, subj_ref, paramRef, size));

         arg = arg.nextNode();
      }
   }

   if (!emptystr(signature))
      sign_id = scope.moduleScope->module->mapSubject(signature, false);

   return encodeMessage(sign_id, EVAL_MESSAGE_ID, scope.parameters.Count());
}

inline SNode findTerminal(SNode node)
{
   SNode ident = node.findChild(lxIdentifier, lxPrivate);
   if (ident == lxNone)
      ident = node;

   return ident;
}

void Compiler :: declareArgumentList(SNode node, MethodScope& scope)
{
   IdentifierString signature;
   ref_t verb_id = 0;
   ref_t sign_id = 0;

   SNode verb = node.findChild(lxIdentifier, lxPrivate, lxReference);
   SNode arg = node.findChild(lxMethodParameter, lxMessage);
   if (verb == lxNone) {
      if (arg == lxMessage) {
         verb = arg;
         arg = verb.nextNode();

         verb_id = _verbs.get(verb.identifier());
         if (verb_id == 0) {
            sign_id = scope.mapSubject(verb, signature);
         }
      }
   }
   else {
      verb_id = _verbs.get(verb.identifier());
      if (verb_id == 0) {
         sign_id = scope.mapSubject(verb, signature);
      }
   }

   bool first = signature.Length() == 0;
   int paramCount = 0;
   // if method has generic (unnamed) argument list
   while (arg == lxMethodParameter) {
      int index = 1 + scope.parameters.Count();

      ident_t terminal = findTerminal(arg).identifier();
      if (scope.parameters.exist(terminal))
         scope.raiseError(errDuplicatedLocal, arg);

      // if it is typified argument
      if (verb_id == 0 && sign_id != 0 && paramCount == 0 && arg.nextNode() != lxMethodParameter) {
         ref_t paramRef = scope.moduleScope->subjectHints.get(sign_id);
         int size = sign_id != 0 ? _logic->defineStructSize(*scope.moduleScope,
            paramRef, 0, true) : 0;

         scope.parameters.add(terminal, Parameter(index, sign_id, paramRef, size));
      }
      else scope.parameters.add(terminal, Parameter(index));

      paramCount++;

      arg = arg.nextNode();
   }

   // if method has named argument list
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxIdentifier, lxPrivate, lxReference);
      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.mapSubject(subject, signature);

      arg = arg.nextNode();

      if (arg == lxMethodParameter) {
         ident_t name = arg.findChild(lxIdentifier, lxPrivate).identifier();

         if (scope.parameters.exist(name))
            scope.raiseError(errDuplicatedLocal, arg);

         int index = 1 + scope.parameters.Count();

         // if it is an open argument type
         if (subj_ref == scope.moduleScope->paramsSubj) {
            scope.parameters.add(name, Parameter(index, subj_ref));

            // the generic arguments should be free by the method exit
            scope.rootToFree += paramCount;
            scope.withOpenArg = true;

            // to indicate open argument list
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > OPEN_ARG_COUNT)
               scope.raiseError(errNotApplicable, arg);

            // to clear virtual open argument subject
            signature.truncate(signature.ident().findLast('&', 0));
         }
         else {
            paramCount++;
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            ref_t paramRef = scope.moduleScope->subjectHints.get(subj_ref);
            int size = subj_ref != 0 ? _logic->defineStructSize(*scope.moduleScope,
               paramRef, 0, true) : 0;

            scope.parameters.add(name, Parameter(index, subj_ref, paramRef, size));

            arg = arg.nextNode();
         }
      }
   }

   // HOTFIX : do not overrwrite the message on the second pass
   if (scope.message == 0) {
      if (test(scope.hints, tpSealed | tpGeneric)) {
         if (paramCount == OPEN_ARG_COUNT) {
            // if it is a generic open argument handler - eval verb should be used
            if (verb_id == 0)
               verb_id = EVAL_MESSAGE_ID;
         }
         else {
            if (!emptystr(signature))
               scope.raiseError(errInvalidHint, verb);

            signature.copy(GENERIC_PREFIX);
         }
      }
      if (verb_id == 0)
         verb_id = paramCount > 0 ? EVAL_MESSAGE_ID : GET_MESSAGE_ID;

      // if signature is presented
      if (!emptystr(signature)) {
         sign_id = scope.moduleScope->module->mapSubject(signature, false);
      }

      if (test(scope.hints, tpSealed | tpConversion)) {
         if (verb_id == EVAL_MESSAGE_ID && paramCount == 1) {
            verb_id = PRIVATE_MESSAGE_ID;
         }
         else if (verb_id == GET_MESSAGE_ID && paramCount == 0 && sign_id != 0 && test(scope.getClassFlags(false), elNestedClass)) {
            // if it is an implicit nested constructor
            sign_id = 0;
            verb_id = PRIVATE_MESSAGE_ID;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      if (test(scope.hints, tpSealed) && verb == lxPrivate) {
         verb_id = PRIVATE_MESSAGE_ID;
      }

      scope.message = encodeMessage(sign_id, verb_id, paramCount);
   }
}

void Compiler :: compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withGenericMethods, bool withOpenArgGenerics)
{
   writer.newNode(lxClassMethod, scope.message);

   CodeScope codeScope(&scope);

   if (isImportRedirect(node)) {
      importCode(writer, node, *scope.moduleScope, node.findChild(lxReference).identifier(), scope.message);
   }
   else {
      writer.newNode(lxDispatching);

      // if it is generic handler with redirect statement / redirect statement
      if (node != lxNone && node.firstChild(lxObjectMask) != lxNone) {
         // !! temporally
         if (withOpenArgGenerics)
            scope.raiseError(errInvalidOperation, node);

         if (withGenericMethods) {
            writer.appendNode(lxDispatching, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));
         }

         compileDispatchExpression(writer, node, codeScope);
      }
      // if it is generic handler without redirect statement
      else if (withGenericMethods) {
         // !! temporally
         if (withOpenArgGenerics)
            scope.raiseError(errInvalidOperation, node);

         writer.newNode(lxResending);

         writer.appendNode(lxMessage, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));

         writer.newNode(lxTarget, scope.moduleScope->superReference);
         writer.appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
         writer.closeNode();

         writer.closeNode();
      }
      // if it is open arg generic without redirect statement
      else if (withOpenArgGenerics) {
         writer.newNode(lxResending);

         writer.appendNode(lxMessage, encodeMessage(0, DISPATCH_MESSAGE_ID, OPEN_ARG_COUNT));

         writer.newNode(lxTarget, scope.moduleScope->superReference);
         writer.appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
         writer.closeNode();

         writer.closeNode();
      }

      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileActionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   declareParameterDebugInfo(writer, node, scope, false, true);

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning);
//   if (body == lxReturning) {
//      // HOTFIX : if it is an returning expression, inject returning node
//      SNode expr = body.findChild(lxExpression);
//      expr = lxReturning;
//   }

   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains previous $self value
   codeScope.level++;

   compileCode(writer, body == lxReturning ? node : body, codeScope);

   writer.closeNode();

   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - 1);  // allocate the space for the local variables excluding "this" one

   writer.closeNode();
}

void Compiler :: compileLazyExpressionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   declareParameterDebugInfo(writer, node, scope, false, false);

   CodeScope codeScope(&scope);

   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains previous $self value
   codeScope.level++;

   compileRetExpression(writer, node.findChild(lxExpression), codeScope, 0);

   writer.closeNode();

   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - 1);  // allocate the space for the local variables excluding "this" one

   writer.closeNode();
}

void Compiler :: compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(writer, node, *scope.moduleScope, node.findChild(lxReference).identifier(), scope.getMessageID());
   }
   else {
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      // try to implement light-weight resend operation
      ObjectInfo target;
      if (isSingleStatement(node)) {
         target = scope.mapObject(node.firstChild(lxTerminalMask));
      }

      if (target.kind == okConstantSymbol || target.kind == okField) {
         writer.newNode(lxResending, methodScope->message);
         writer.newNode(lxExpression);
         if (target.kind == okField) {
            writer.appendNode(lxResultField, target.param);
         }
         else writer.appendNode(lxConstantSymbol, target.param);

         writer.closeNode();
         writer.closeNode();
      }
      else {
         writer.newNode(lxResending, methodScope->message);
         writer.newNode(lxNewFrame);

         target = compileExpression(writer, node, scope, 0);

         writer.closeNode();
         writer.closeNode();
      }
   }
}

void Compiler :: compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
{
   SNode expr = node.findChild(lxExpression);

   ModuleScope* moduleScope = scope.moduleScope;
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // find where the target constructor is declared in the current class
   size_t count = 0;
   ref_t messageRef = mapMessage(expr, scope, count);

   ref_t classRef = classClassScope.reference;
   bool found = false;

   // find where the target constructor is declared in the current class
   // but it is not equal to the current method
   if (methodScope->message != messageRef && classClassScope.info.methods.exist(messageRef)) {
      found = true;
   }
   // otherwise search in the parent class constructors
   else {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
      ref_t parent = classScope->info.header.parentRef;
      ClassInfo info;
      while (parent != 0) {
         moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(parent));

         if (checkMethod(*moduleScope, info.header.classRef, messageRef) != tpUnknown) {
            classRef = info.header.classRef;
            found = true;

            break;
         }
         else parent = info.header.parentRef;
      }
   }
   if (found) {
      if ((count != 0 && methodScope->parameters.Count() != 0) || node.existChild(lxCode)) {
         withFrame = true;

         // new stack frame
         // stack already contains $self value
         writer.newNode(lxNewFrame);
         scope.level++;
      }
      else writer.newNode(lxExpression);

      writer.newBookmark();

      if (withFrame) {
         writer.appendNode(lxThisLocal, 1);
      }
      else writer.appendNode(lxResult);

      writer.appendNode(lxCallTarget, classRef);

      compileMessageParameters(writer, expr, scope, HINT_RESENDEXPR);

      compileMessage(writer, expr, scope, ObjectInfo(okObject, classRef), messageRef, HINT_RESENDEXPR);

      writer.removeBookmark();
      
      if (withFrame) {
         // HOT FIX : inject saving of the created object
         SNode codeNode = node.findChild(lxCode);
         if (codeNode != lxNone) {
            writer.newNode(lxAssigning);
            writer.appendNode(lxLocal, 1);
            writer.appendNode(lxResult);
            writer.closeNode();
         }
      }
      else writer.closeNode();
   }
   else scope.raiseError(errUnknownMessage, node);
}

void Compiler :: compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(writer, node, *scope.moduleScope, node.findChild(lxReference).identifier(), scope.getMessageID());
   }
   else scope.raiseError(errInvalidOperation, node);
}

void Compiler :: compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains current $self reference
   scope.level++;

   writer.newNode(lxExpression);
   //writer.appendNode(lxThisLocal, 1);
   compileMessage(writer, node.firstChild(lxObjectMask), scope, HINT_RESENDEXPR);
   writer.closeNode();

   scope.freeSpace();

   writer.closeNode();
}

void Compiler :: compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   declareParameterDebugInfo(writer, node, scope, true, test(scope.getClassFlags(), elRole));

   int paramCount = getParamCount(scope.message);
   int preallocated = 0;
   
   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   // check if it is a resend
   if (body == lxResendExpression) {
      compileResendExpression(writer, body, codeScope);
      preallocated = 1;
   }
   // check if it is a dispatch
   else if (body == lxDispatchCode) {
      compileDispatchExpression(writer, body, codeScope);
   }
   else {
      writer.newNode(lxNewFrame, scope.generic ? -1 : 0);
   
      // new stack frame
      // stack already contains current $self reference
      // the original message should be restored if it is a generic method
      codeScope.level++;
      // declare the current subject for a generic method
      if (scope.generic) {
         codeScope.level++;
         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0, V_MESSAGE, 0);
      }
   
      preallocated = codeScope.level;
   
      ObjectInfo retVal = compileCode(writer, body == lxReturning ? node : body, codeScope);
   
      // if the method returns itself
      if(retVal.kind == okUnknown) {
         // adding the code loading $self
         writer.newNode(lxExpression);
         writer.newBookmark();
         writer.appendNode(lxLocal, 1);

         ref_t resultRef = scope.getReturningRef(false);
         ref_t resultType = scope.getReturningType(false);
         if (resultRef != 0) {
//            // HOTFIX : copy EOP coordinates
//            SNode eop = body.lastChild().prevNode();
//            if (eop != lxNone)
//               SyntaxTree::copyNode(eop, localNode);
   
            if (!convertObject(writer, *codeScope.moduleScope, resultRef, resultType, resolveObjectReference(codeScope, ObjectInfo(okThisParam)), 0))
               scope.raiseError(errInvalidOperation, node);

            //boxObject(writer, node, codeScope, ObjectInfo(okThisParam), resultRef, resultType);            
         }

         writer.removeBookmark();
         writer.closeNode();
      }

      writer.closeNode();
   }
   
   writer.appendNode(lxParamCount, paramCount + scope.rootToFree);
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - preallocated);  // allocate the space for the local variables excluding preallocated ones ("$this", "$message")

   writer.closeNode();
}

void Compiler :: compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   writer.newNode(lxClassMethod, scope.message);

   SNode attrNode = node.findChild(lxEmbeddableMssg);
   if (attrNode != lxNone) {
      writer.appendNode(attrNode.type, attrNode.argument);
   }

   declareParameterDebugInfo(writer, node, scope, true, false);

   CodeScope codeScope(&scope);

   bool retExpr = false;
   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();
   int preallocated = 0;

   SNode bodyNode = node.findChild(lxResendExpression, lxCode, lxReturning, lxDispatchCode);
   if (bodyNode == lxDispatchCode) {
      compileConstructorDispatchExpression(writer, bodyNode, codeScope);

      writer.closeNode();
      return;
   }
   else if (bodyNode == lxResendExpression) {
      compileConstructorResendExpression(writer, bodyNode, codeScope, classClassScope, withFrame);

      bodyNode = bodyNode.findChild(lxCode);
   }
   else if (bodyNode == lxReturning) {
      retExpr = true;
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(encodeVerb(NEWOBJECT_MESSAGE_ID))) {
      writer.appendNode(lxCalling, -1);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node);

   if (bodyNode != lxNone) {
      if (!withFrame) {
         withFrame = true;

         writer.newNode(lxNewFrame);

         // new stack frame
         // stack already contains $self value
         codeScope.level++;
      }

      if (retExpr) {
//         recordDebugStep(codeScope, bodyNode.firstChild().FirstTerminal(), dsStep);

         writer.newNode(lxReturning);         
         writer.newBookmark();

         ObjectInfo retVal = compileRetExpression(writer, bodyNode, codeScope, /*HINT_CONSTRUCTOR_EPXR*/0);
         convertObject(writer, *codeScope.moduleScope, codeScope.getClassRefId(), 0, resolveObjectReference(codeScope, retVal), retVal.type);

         writer.removeBookmark();
         writer.closeNode();
      }
      else {
         preallocated = codeScope.level;

         compileCode(writer, bodyNode, codeScope);

         // HOT FIX : returning the created object
         writer.appendNode(lxLocal, 1);
      }
   }

   if (withFrame)
      writer.closeNode();

   writer.appendNode(lxParamCount, getParamCount(scope.message) + 1);
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - preallocated);  // allocate the space for the local variables excluding preallocated ones ("$this", "$message")

   writer.closeNode();
}

void Compiler :: compileDefaultConstructor(SyntaxWriter& writer, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   if (test(classScope->info.header.flags, elStructureRole)) {
      if (!test(classScope->info.header.flags, elDynamicRole)) {
         writer.newNode(lxCreatingStruct, classScope->info.size);
         writer.appendNode(lxTarget, classScope->reference);
         writer.closeNode();
      }
   }
   else if (!test(classScope->info.header.flags, elDynamicRole)) {
      writer.newNode(lxCreatingClass, classScope->info.fields.Count());
      writer.appendNode(lxTarget, classScope->reference);
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   if (test(classScope->info.header.flags, elStructureRole)) {
      writer.newNode(lxCreatingStruct, classScope->info.size);
      writer.appendNode(lxTarget, classScope->reference);
      writer.closeNode();
   }
   else {
      writer.newNode(lxCreatingClass, -1);
      writer.appendNode(lxTarget, classScope->reference);
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxClassMethod:
         {
            MethodScope methodScope(&scope);            
            methodScope.message = current.argument;

            // if it is a dispatch handler
            if (methodScope.message == encodeVerb(DISPATCH_MESSAGE_ID)) {
               //if (test(scope.info.header.flags, elRole))
               //   scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               initialize(scope, methodScope);

               compileDispatcher(writer, current.findChild(lxDispatchCode), methodScope, 
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithArgGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(current, methodScope);

               initialize(scope, methodScope);

               compileMethod(writer, current, methodScope);
            }
            break;
         }
      }

      current = current.nextNode();
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (testany(scope.info.header.flags, elWithGenerics | elWithArgGenerics) && scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), false)) {
      MethodScope methodScope(&scope);
      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);

      scope.include(methodScope.message);

      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);

      compileDispatcher(writer, SNode(), methodScope,
         test(scope.info.header.flags, elWithGenerics),
         test(scope.info.header.flags, elWithArgGenerics));

      // overwrite the class info
      scope.save();
   }
}

void Compiler :: compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   // add virtual constructor
   if (classClassScope.info.methods.exist(encodeVerb(NEWOBJECT_MESSAGE_ID), true)) {
      MethodScope methodScope(&classScope);
      methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

      if (test(classScope.info.header.flags, elDynamicRole)) {
         compileDynamicDefaultConstructor(writer, methodScope);
      }
      else compileDefaultConstructor(writer, methodScope);
   }

   SNode current = node.firstChild();

   while (current != lxNone) {
      switch (current) {
         case lxConstructor:
         {
            MethodScope methodScope(&classScope);
            methodScope.message = current.argument;

            declareArgumentList(current, methodScope);

            initialize(classClassScope, methodScope);

            compileConstructor(writer, current, methodScope, classClassScope);
            break;
         }
      }

      current = current.nextNode();
   }
}

inline int countFields(SNode node)
{
   int counter = 0;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassField) {
         counter++;
      }

      current = current.nextNode();
   }

   return counter;
}
void Compiler :: generateClassFields(SNode node, ClassScope& scope, bool singleField)
{
   SNode current = node.firstChild();

   while (current != lxNone) {
      if (current == lxClassField) {
         ref_t fieldRef = 0;
         ref_t fieldType = 0;
         int sizeHint = 0;
         declareFieldAttributes(current, scope, fieldType, fieldRef, sizeHint);

         generateClassField(scope, current, fieldType, fieldRef, sizeHint, singleField);
      }
      current = current.nextNode();
   }
}


void Compiler :: compileSymbolCode(ClassScope& scope)
{
   CommandTape tape;

   // creates implicit symbol
   SymbolScope symbolScope(scope.moduleScope, scope.reference);

   _writer.generateSymbol(tape, symbolScope.reference, lxConstantClass, scope.reference);

   // create byte code sections
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compilePreloadedCode(SymbolScope& scope)
{
   _Module* module = scope.moduleScope->module;

   ReferenceNs sectionName(module->Name(), INITIALIZER_SECTION);

   CommandTape tape;
   _writer.generateInitializer(tape, module->mapReference(sectionName), lxSymbolReference, scope.reference);

   // create byte code sections
   _writer.save(tape, module, NULL, 0);
}

void Compiler :: compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   bool withDefaultConstructor = _logic->isDefaultConstructorEnabled(classScope.info);
   // if no construtors are defined inherits the default one
   if (!node.existChild(lxConstructor) && withDefaultConstructor) {
      if (classScope.info.header.parentRef == 0)
         classScope.raiseError(errNoConstructorDefined, node.findChild(lxIdentifier, lxPrivate));

      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.moduleScope->superReference));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }

   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef, true);

   generateClassDeclaration(node, classClassScope, true);

   // generate constructor attributes
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.start();
   while (!it.Eof()) {
      int hints = classClassScope.info.methodHints.get(Attribute(it.key(), maHint));
      if (test(hints, tpConstructor)) {
         classClassScope.info.methodHints.exclude(Attribute(it.key(), maReference));
         classClassScope.info.methodHints.add(Attribute(it.key(), maReference), classScope.reference);
      }
   
      it++;
   }
   
   // save declaration
   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

   writer.newNode(lxClass, classClassScope.reference);
   compileClassVMT(writer, node, classClassScope, classScope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), classClassScope);
}

void Compiler :: initialize(ClassScope& scope, MethodScope& methodScope)
{
   methodScope.stackSafe = _logic->isMethodStacksafe(scope.info, methodScope.message);
   methodScope.classEmbeddable = _logic->isEmbeddable(scope.info);
   methodScope.withOpenArg = isOpenArg(methodScope.message);
   if (!methodScope.withOpenArg) {
      // HOTFIX : generic with open argument list is compiled differently
      methodScope.generic = _logic->isMethodGeneric(scope.info, methodScope.message);
   }   
}

void Compiler :: declareVMT(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         MethodScope methodScope(&scope);

         declareMethodAttributes(current, methodScope);

         declareArgumentList(current, methodScope);
         current.setArgument(methodScope.message);

         if (test(methodScope.hints, tpConstructor))
            current = lxConstructor;

         if (!_logic->validateMessage(methodScope.message, false))
            scope.raiseError(errIllegalMethod, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: generateClassFlags(ClassScope& scope, SNode root)
{
   ref_t extensionTypeRef = 0;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         scope.info.header.flags |= current.argument;
      }
      else if (current == lxType) {
         extensionTypeRef = current.argument;
      }

      current = current.nextNode();
   }

   // check if extension is qualified
   if (extensionTypeRef != 0) {
      if (test(scope.info.header.flags, elExtension)) {
         scope.extensionMode = extensionTypeRef;

         scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(0, scope.extensionMode));
      }
      else scope.raiseError(errInvalidHint, root);
   }
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxTree::Node current, ref_t typeRef, ref_t classRef, int sizeHint, bool singleField)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int flags = scope.info.header.flags;
   int offset = 0;
   ident_t terminal = current.findChild(lxIdentifier, lxPrivate).identifier();

//   if (!classRef && typeAttr) {
//      classRef = moduleScope->attributeHints.get(typeAttr);
//   }

   // a role cannot have fields
   if (test(flags, elStateless))
      scope.raiseError(errIllegalField, current);

   int size = (classRef != 0) ? _logic->defineStructSize(*moduleScope, classRef) : 0;
   bool fieldArray = false;
   if (sizeHint != 0) {
      if (isPrimitiveRef(classRef) && (size == sizeHint || (classRef == V_INT32 && sizeHint <= size))) {
         // for primitive types size should be specified
         size = sizeHint;
      }
      else if (size > 0) {
         size *= sizeHint;

         // HOTFIX : to recognize the fixed length array
         fieldArray = true; 
         classRef = _logic->definePrimitiveArray(*scope.moduleScope, classRef);
      }
      else scope.raiseError(errIllegalField, current);
   }

   if (test(flags, elWrapper) && scope.info.fields.Count() > 0) {
      // wrapper may have only one field
      scope.raiseError(errIllegalField, current);
   }
   // if it is a primitive data wrapper
   else if (isPrimitiveRef(classRef) && !fieldArray) {
      if (testany(flags, elNonStructureRole | elDynamicRole))
         scope.raiseError(errIllegalField, current);

      if (test(flags, elStructureRole)) {
         scope.info.fields.add(terminal, scope.info.size);
         scope.info.size += size;
      }
      else scope.raiseError(errIllegalField, current);

      if (!_logic->tweakPrimitiveClassFlags(classRef, scope.info))
         scope.raiseError(errIllegalField, current);
   }
   // a class with a dynamic length structure must have no fields
   else if (test(scope.info.header.flags, elDynamicRole)) {
      if (scope.info.size == 0 && scope.info.fields.Count() == 0) {
         // compiler magic : turn a field declaration into an array or string one
         if (size != 0) {
            scope.info.header.flags |= elStructureRole;
            scope.info.size = -size;
         }

         scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(classRef, typeRef));
         scope.info.fields.add(terminal, -2);
      }
      else scope.raiseError(errIllegalField, current);
   }
   else {
      if (scope.info.fields.exist(terminal))
         scope.raiseError(errDuplicatedField, current);

      // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
      if (!test(scope.info.header.flags, elNonStructureRole) && singleField
         && test(scope.info.header.flags, elSealed) && size != 0 && scope.info.fields.Count() == 0)
      {
         scope.info.header.flags |= elStructureRole;
         scope.info.size = size;

         //if (size < 0) {
         //   scope.info.header.flags |= elDynamicRole;
         //}

         scope.info.fields.add(terminal, 0);
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeRef));
      }
      // if it is a structure field
      else if (test(scope.info.header.flags, elStructureRole)) {
         if (size <= 0)
            scope.raiseError(errIllegalField, current);

         if (scope.info.size != 0 && scope.info.fields.Count() == 0)
            scope.raiseError(errIllegalField, current);

         offset = scope.info.size;
         scope.info.size += size;

         scope.info.fields.add(terminal, offset);
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeRef));
      }
      // if it is a normal field
      else {
         // primitive / virtual classes cannot be declared
         if (size != 0 && _logic->isPrimitiveRef(classRef))
            scope.raiseError(errIllegalField, current);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.Count();
         scope.info.fields.add(terminal, offset);

         if (typeRef != 0 || classRef != 0)
            scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeRef));
      }
   }
}

//void Compiler :: generateClassStaticField(ClassScope& scope, SNode current)
//{
//   _Module* module = scope.moduleScope->module;
//
//   ident_t terminal = current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();
//   ref_t typeHint = current.findChild(lxType).argument;
//
//   if (scope.info.statics.exist(terminal))
//      scope.raiseError(errDuplicatedField, current);
//
//   // generate static reference
//   ReferenceNs name(module->resolveReference(scope.reference));
//   name.append(STATICFIELD_POSTFIX);
//
//   findUninqueName(module, name);
//
//   scope.info.statics.add(terminal, ClassInfo::FieldInfo(module->mapReference(name), typeHint));
//}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, ref_t message)
{
   ref_t outputType = scope.info.methodHints.get(Attribute(message, maType));
   ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));
   bool hintChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         //if (current.argument == tpSealed && node.existChild(lxPrivate)) {
         //   //HOTFIX : private sealed method should be marked appropriately
         //   hint |= tpPrivate;
         //}
         //else {
            hint |= current.argument;

            if (current.argument == tpAction)
               scope.moduleScope->saveAction(message, scope.reference);

            ////HOTFIX : overwrite the message for the generic one
            //if (hint == tpGeneric) {
            //   message = overwriteSubject(message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));
            //}
         //}

         hintChanged = true;
      }
      else if (current == lxTypeAttr) {
         if (outputType == 0 && outputRef == 0) {
            outputType = scope.moduleScope->module->mapSubject(current.identifier(), false);
            scope.info.methodHints.add(Attribute(message, maType), outputType);
            scope.info.methodHints.add(Attribute(message, maReference), scope.moduleScope->subjectHints.get(outputType));
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnTypeAlreadyDeclared, node);
      }
      else if (current == lxClassRefAttr) {
         if (outputType == 0 && outputRef == 0) {
            outputRef = scope.moduleScope->module->mapReference(current.identifier(), false);

            scope.info.methodHints.add(Attribute(message, maReference), outputRef);
         }
         else scope.raiseError(errInvalidHint, node);
      }
//      else if (current == lxClassMethodOpt) {
//         SNode mssgAttr = SyntaxTree::findChild(current, lxMessage);
//         if (mssgAttr != lxNone) {
//            scope.info.methodHints.add(Attribute(message, current.argument), getSignature(mssgAttr.argument));
//         }
//      }
      current = current.nextNode();
   }
   
   if (getVerb(message) == PRIVATE_MESSAGE_ID) {
      // if it is private message set private hint and save it as EVAL one
      hintChanged = true;
      hint |= tpPrivate;

      scope.info.methodHints.add(Attribute(overwriteVerb(message, EVAL_MESSAGE_ID), maHint), hint);

      // if it is an explicit constant conversion
      if (getSignature(message) != 0) {
         ident_t signature = scope.moduleScope->module->resolveSubject(getSignature(message));
         size_t pos = signature.find('&');
         if (test(hint, tpConversion) && pos != NOTFOUND_POS && getParamCount(message) == 1) {
            // add it to the action list
            IdentifierString postfix(signature, pos);

            scope.moduleScope->saveAction(encodeMessage(scope.moduleScope->module->mapSubject(postfix, false), PRIVATE_MESSAGE_ID, 1), scope.reference);
         }
      }
   }

   if (hintChanged) {
      scope.info.methodHints.exclude(Attribute(message, maHint));
      scope.info.methodHints.add(Attribute(message, maHint), hint);
   }
}

void Compiler :: generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed)
{
   ref_t message = current.argument;

   if (scope.info.methods.exist(message, true) && hideDuplicates) {
      // HOTFIX : check if duplicate template methods are declared
      SNode prev = current.prevNode();
      while (prev != lxNone) {
         if (prev == lxClassMethod && prev.argument == message)
            scope.raiseError(errDuplicatedMethod, current);

         prev = prev.prevNode();
      }

      current = lxIdle;

      return;
   }

   generateMethodAttributes(scope, current, message);

   int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(message, maHint));
   if (isOpenArg(message)) {
      if (_logic->isMethodGeneric(scope.info, message)) {
         scope.info.header.flags |= elWithArgGenerics;
      }         
   }
   else if (_logic->isMethodGeneric(scope.info, message)) {
      scope.info.header.flags |= elWithGenerics;
   }

   // check if there is no duplicate method
   if (scope.info.methods.exist(message, true)) {
      scope.raiseError(errDuplicatedMethod, current);
   }
   else {
      bool privateOne = getVerb(message) == PRIVATE_MESSAGE_ID;
      bool included = scope.include(message);
      bool sealedMethod = (methodHints & tpMask) == tpSealed;
      // if the class is closed, no new methods can be declared
      // except private sealed ones (which are declared outside the class VMT)
      if (included && closed && !privateOne)
         scope.raiseError(errClosedParent, findParent(current, lxClass));

      // if the method is sealed, it cannot be overridden
      if (!included && sealedMethod)
         scope.raiseError(errClosedMethod, findParent(current, lxClass));

      // save extensions if required ; private method should be ignored
      if (test(scope.info.header.flags, elExtension) && !test(methodHints, tpPrivate)) {
         scope.moduleScope->saveExtension(message, scope.extensionMode, scope.reference);
         if (isOpenArg(message) && _logic->isMethodGeneric(scope.info, message)) {
            // if it is an extension with open argument list generic handler
            // creates the references for all possible number of parameters
            for (int i = 1; i < 15; i++) {
               scope.moduleScope->saveExtension(overwriteParamCount(message, i), scope.extensionMode, scope.reference);
            }
         }
      }
   }
}

void Compiler :: generateMethodDeclarations(SNode root, ClassScope& scope, bool closed, bool classClassMode)
{
   bool templateMethods = false;

   // first pass - ignore template based methods
   SNode current = root.firstChild();
   while (current != lxNone) {
      if ((current == lxClassMethod && !classClassMode) || (classClassMode && current == lxConstructor)) {
         if (!classClassMode) {
            if (!current.existChild(lxTemplate)) {
               generateMethodDeclaration(current, scope, false, closed);
            }
            else templateMethods = true;
         }
         else generateMethodDeclaration(current, scope, false, closed);
      }
      current = current.nextNode();
   }

   if (templateMethods) {
      // second pass - do not include overwritten template-based methods
      current = root.firstChild();
      while (current != lxNone) {
         if (current == lxClassMethod && current.existChild(lxTemplate)) {
            generateMethodDeclaration(current, scope, true, closed);
         }
         current = current.nextNode();
      }
   }
}

void Compiler :: generateClassDeclaration(SNode node, ClassScope& scope, bool classClassMode, bool closureDeclarationMode)
{
   bool closed = test(scope.info.header.flags, elClosed);

   if (classClassMode) {
      if (_logic->isDefaultConstructorEnabled(scope.info)) {
         scope.include(encodeVerb(NEWOBJECT_MESSAGE_ID));
      }
   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, node);

      if (test(scope.info.header.flags, elExtension)) {
         scope.extensionMode = scope.info.fieldTypes.get(-1).value2;
         if (scope.extensionMode == 0)
            scope.extensionMode = INVALID_REF;
      }

      // generate fields
      generateClassFields(node, scope, countFields(node) == 1);
   }

   _logic->injectVirtualCode(*scope.moduleScope, node, scope.reference, scope.info, *this);

   // generate methods
   generateMethodDeclarations(node, scope/*, false*/, closed, classClassMode);

   // do not set flags for closure declaration - they will be set later
   if(!closureDeclarationMode)
      _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info, classClassMode);
}

void Compiler :: declareMethodAttributes(SNode node, MethodScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateMethodAttribute(value)) {
            scope.hints |= value;

            current.setArgument(value);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: compileClassDeclaration(SNode node, ClassScope& scope)
{
   SNode baseNode = node.findChild(lxBaseParent);
   if (baseNode!=lxNone) {
      compileParentDeclaration(baseNode, scope);
   }
   else compileParentDeclaration(SNode(), scope);

   declareClassAttributes(node, scope);
   //compileFieldDeclarations(node, scope);

   declareVMT(node, scope);

   generateClassDeclaration(node, scope, false);

   // if it cannot be initiated
   if (_logic->isRole(scope.info)) {
      // class is its own class class
      scope.info.header.classRef = scope.reference;
   }
   else {
      // define class class name
      IdentifierString classClassName(scope.moduleScope->module->resolveReference(scope.reference));
      classClassName.append(CLASSCLASS_POSTFIX);

      scope.info.header.classRef = scope.moduleScope->module->mapReference(classClassName);
   }

   // if it is a super class validate it
   if (scope.info.header.parentRef == 0 && scope.reference == scope.moduleScope->superReference) {
      if (!scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID)))
         scope.raiseError(errNoDispatcher, node.findChild(lxIdentifier, lxPrivate));
   }

   // save declaration
   scope.save();

   // compile class class if it available
   if (scope.info.header.classRef != scope.reference) {
      ClassScope classClassScope(scope.moduleScope, scope.info.header.classRef);

      compileClassClassDeclaration(node, classClassScope, scope);
   }
}

void Compiler :: generateClassImplementation(SNode node, ClassScope& scope)
{
   WarningScope warningScope(scope.moduleScope->warningMask);

   optimizeClassTree(node, scope, warningScope);

   CommandTape tape;
   _writer.generateClass(tape, node);

   // optimize
   optimizeTape(tape);

   //// create byte code sections
   //scope.save();
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule,
      scope.moduleScope->sourcePathRef);
}

void Compiler :: compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

   if (test(scope.info.header.flags, elExtension)) {
      scope.extensionMode = scope.info.fieldTypes.get(-1).value2;
      if (scope.extensionMode == 0)
         scope.extensionMode = INVALID_REF;
   }

   writer.newNode(lxClass, node.argument);
   compileVMT(writer, node, scope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (scope.extensionMode == 0)
      compileSymbolCode(scope);
}

void Compiler :: compileSymbolDeclaration(SNode node, SymbolScope& scope)
{
   declareSymbolAttributes(node, scope);
   
   if ((scope.constant || scope.typeRef != 0 || scope.outputRef != 0) && scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, true) == false) {
      SymbolExpressionInfo info;
      info.expressionTypeRef = scope.typeRef;
      info.expressionClassRef = scope.outputRef;
      info.constant = scope.constant;
      
      // save class meta data
      MemoryWriter metaWriter(scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, false), 0);
      info.save(&metaWriter);
   }
}

bool Compiler :: compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal)
{
   if (retVal.kind == okIntConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      size_t value = module->resolveConstant(retVal.param).toULong(16);

      dataWriter.writeDWord(value);

      dataWriter.Memory()->addReference(scope.moduleScope->intReference | mskVMTRef, (ref_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, V_INT32);
   }
   else if (retVal.kind == okLongConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      long long value = module->resolveConstant(retVal.param).toULongLong(10, 1);

      dataWriter.write(&value, 8u);

      dataWriter.Memory()->addReference(scope.moduleScope->longReference | mskVMTRef, (ref_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->longReference);
   }
   else if (retVal.kind == okRealConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      double value = module->resolveConstant(retVal.param).toDouble();

      dataWriter.write(&value, 8u);

      dataWriter.Memory()->addReference(scope.moduleScope->realReference | mskVMTRef, (ref_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->realReference);
   }
   else if (retVal.kind == okLiteralConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      ident_t value = module->resolveConstant(retVal.param);

      dataWriter.writeLiteral(value, getlength(value) + 1);

      dataWriter.Memory()->addReference(scope.moduleScope->literalReference | mskVMTRef, (size_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->literalReference);
   }
   else if (retVal.kind == okWideLiteralConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      WideString wideValue(module->resolveConstant(retVal.param));

      dataWriter.writeLiteral(wideValue, getlength(wideValue) + 1);

      dataWriter.Memory()->addReference(scope.moduleScope->wideReference | mskVMTRef, (size_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->wideReference);
   }
   else if (retVal.kind == okCharConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      ident_t value = module->resolveConstant(retVal.param);

      dataWriter.writeLiteral(value, getlength(value));

      dataWriter.Memory()->addReference(scope.moduleScope->charReference | mskVMTRef, (ref_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->charReference);
   }
   else if (retVal.kind == okObject) {
      SNode root = node.findSubNodeMask(lxObjectMask);

      if (root == lxConstantList) {
         SymbolExpressionInfo info;
         info.expressionTypeRef = scope.typeRef;
         info.expressionClassRef = scope.outputRef;
         info.constant = scope.constant;
         info.listRef = root.argument;

         // save class meta data
         MemoryWriter metaWriter(scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, false), 0);
         info.save(&metaWriter);

         return true;
      }
      else return false;
   }
   else return false;

   return true;
}

void Compiler :: compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

   declareSymbolAttributes(node, scope);

   bool isStatic = scope.staticOne;

   SNode expression = node.findChild(lxExpression);
   
   CodeScope codeScope(&scope);

   writer.newNode(lxSymbol, node.argument);
   writer.newNode(lxExpression);
   writer.appendNode(lxBreakpoint, dsStep);
   writer.newBookmark();
   ObjectInfo retVal = compileExpression(writer, expression, codeScope, isSingleStatement(expression) ? HINT_ROOTSYMBOL : 0);
   if (scope.typeRef != 0) {
      ModuleScope* moduleScope = scope.moduleScope;

      convertObject(writer, *moduleScope, moduleScope->subjectHints.get(scope.typeRef), scope.typeRef, resolveObjectReference(codeScope, retVal), retVal.type);
   }
   else if (scope.outputRef != 0) {
      ModuleScope* moduleScope = scope.moduleScope;

      convertObject(writer, *moduleScope, scope.outputRef, scope.typeRef, resolveObjectReference(codeScope, retVal), retVal.type);
   }
   writer.removeBookmark();
   writer.closeNode();
   writer.closeNode();

   optimizeSymbolTree(expressionTree.readRoot(), scope, scope.moduleScope->warningMask);
   node.refresh();

   // create constant if required
   if (scope.constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression);

      if (!compileSymbolConstant(expressionTree.readRoot(), scope, retVal))
         scope.raiseError(errInvalidOperation, expression);
   }

   if (scope.preloaded) {
      compilePreloadedCode(scope);
   }

   CommandTape tape;
   _writer.generateSymbol(tape, expressionTree.readRoot(), isStatic);

   // optimize
   optimizeTape(tape);

   // create byte code sections
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule,
      scope.moduleScope->sourcePathRef);
}

// NOTE : targetType is used for binary arrays
ObjectInfo Compiler :: assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t targetType)
{
   ObjectInfo retVal(okObject, targetRef, 0, targetType);

   int size = _logic->defineStructSize(*scope.moduleScope, targetRef, targetType);
   if (size != 0) {
      if (allocateStructure(scope, size, false, retVal)) {
         retVal.extraparam = targetRef;

         writer.insertChild(0, lxLocalAddress, retVal.param);
         writer.appendNode(lxTempAttr);
         writer.insert(lxAssigning, size);
         writer.closeNode();         
      }
      else if (size > 0) {
         retVal.kind = okObject;
         retVal.param = targetRef;

         writer.appendNode(lxTarget, targetRef);
         writer.insert(lxCreatingStruct, size);
         writer.closeNode();

         writer.insert(lxAssigning, size);
         writer.closeNode();
      }

      writer.appendNode(lxTarget, targetRef);
      writer.appendNode(lxBoxableAttr);
      writer.insert(lxBoxing, size);
      writer.closeNode();

      return retVal;
   }
   else return retVal;
}

ref_t Compiler :: optimizeExtCall(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   //SNode parentNode = node.parentNode();
   //while (parentNode == lxExpression)
   //   parentNode = parentNode.parentNode();

   //if (parentNode == lxAssigning) {
   //   if (parentNode.argument != 4) {
   //      boxPrimitive(scope, node, -1, warningMask, mode);
   //   }
   //}
   //else if (parentNode == lxTypecasting) {
   //   boxPrimitive(scope, node, -1, warningMask, mode);
   //}

   compileExternalArguments(node, scope, warningScope);

   return V_INT32;
}

ref_t Compiler :: optimizeInternalCall(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   optimizeExpressionTree(node, scope, warningScope, HINT_NOBOXING);

   return V_INT32;
}

ref_t Compiler :: optimizeArgUnboxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int)
{
   optimizeExpressionTree(node, scope, warningScope, HINT_NOBOXING);

   return 0;
}

int Compiler :: allocateStructure(SNode node, int& size)
{
   // finding method's reserved attribute
   SNode methodNode = node.parentNode();
   while (methodNode != lxClassMethod)
      methodNode = methodNode.parentNode();

   SNode reserveNode = methodNode.findChild(lxReserved);
   int reserved = reserveNode.argument;

   // allocating space
   int offset = allocateStructure(false, size, reserved);

   // HOT FIX : size should be in bytes
   size *= 4;

   reserveNode.setArgument(reserved);

   return offset;
}

ref_t Compiler :: optimizeNestedExpression(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   // check if the nested collection can be treated like constant one
   bool constant = true;
   ref_t memberCounter = 0;
   SNode current = node.firstChild();
   while (constant && current != lxNone) {
      if (current == lxMember) {
         SNode object = current.findSubNodeMask(lxObjectMask);
         switch (object.type) {
            case lxConstantChar:
            case lxConstantClass:
            case lxConstantInt:
            case lxConstantLong:
            case lxConstantList:
            case lxConstantReal:
            case lxConstantString:
            case lxConstantWideStr:
            case lxConstantSymbol:
               break;
            default:
               constant = false;
               optimizeExpression(current, scope, warningScope);
               break;
         }
         memberCounter++;
      }
      else if (current == lxOuterMember) {
         // nested class with outer member must not be constant
         constant = false;

         optimizeExpression(current, scope, warningScope);
      }
      else if (current == lxOvreriddenMessage) {
         constant = false;
      }
      current = current.nextNode();
   }

   if (node.argument != memberCounter)
      constant = false;

   // replace with constant array if possible
   if (constant) {
      ref_t reference = scope.mapNestedExpression();

      node = lxConstantList;
      node.setArgument(reference | mskConstArray);

      _writer.generateConstantList(node, scope.module, reference);
   }

   return node.findChild(lxTarget).argument;
}

ref_t Compiler :: optimizeMessageCall(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   int mode = 0;

   if (node.existChild(lxStacksafeAttr)) {
      mode |= HINT_NOBOXING;
   }

   if (node.existChild(lxEmbeddableAttr)) {
      if (!_logic->optimizeEmbeddable(node, scope))
         node.appendNode(lxEmbeddable);
   }

   optimizeExpressionTree(node, scope, warningScope, mode);

   return node.findChild(lxTarget).argument;
}

ref_t Compiler :: optimizeAssigning(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   //ref_t targetRef = node.findChild(lxTarget).argument;
   SNode targetNode = node.firstChild(lxObjectMask);
   SNode sourceNode = targetNode.nextNode(lxObjectMask);

   ref_t sourceRef = optimizeExpression(sourceNode, scope, warningScope,node.argument != 0 ? HINT_NOBOXING | HINT_NOUNBOXING : HINT_NOUNBOXING);

   if (node.argument != 0) {
      SNode intValue = node.findSubNode(lxConstantInt);
      if (intValue != lxNone && node.argument == 4) {
         // direct operation with numeric constants
         node.set(lxIntOp, SET_MESSAGE_ID);
      }
      else {
         SNode subNode = node.findSubNode(lxDirectCalling, lxSDirctCalling, lxAssigning);
         if (subNode == lxAssigning) {
            bool tempAttr = subNode.existChild(lxTempAttr);

            // assignment operation
            SNode operationNode = subNode.findChild(lxIntOp, lxRealOp, lxLongOp, lxIntArrOp, lxByteArrOp, lxShortArrOp);
            if (operationNode != lxNone) {
               SNode larg = operationNode.findSubNodeMask(lxObjectMask);
               SNode target = node.firstChild(lxObjectMask);
               // if it is an operation with the same target
               if (larg.type == target.type && larg.argument == target.argument) {
                  // remove an extra assignment
                  larg = subNode.findSubNodeMask(lxObjectMask);
      
                  larg = target.type;
                  larg.setArgument(target.argument);
                  node = lxExpression;
                  target = lxIdle;
               }      
               // if it is an operation with an extra temporal variable
               else if ((node.argument == subNode.argument || operationNode == lxByteArrOp || operationNode == lxShortArrOp) && tempAttr) {
                  larg = subNode.findSubNodeMask(lxObjectMask);
      
                  if (larg.type == targetNode.type && larg.argument == targetNode.argument) {
                     // remove an extra assignment
                     subNode = lxExpression;
                     larg = lxIdle;
                  }
               }
            }
            else if (tempAttr && subNode.argument == node.argument) {
               SNode larg = subNode.firstChild(lxObjectMask);
               if (larg == lxLocalAddress) {
                  // remove an extra assignment
                  subNode = lxExpression;
                  larg = lxIdle;
               }
            }
         }
         else if (subNode != lxNone) {
            if (subNode.existChild(lxEmbeddable)) {
               if (!_logic->optimizeEmbeddableGet(scope, *this, node)) {
                  _logic->optimizeEmbeddableOp(scope, *this, node);
               }
            }
            else if (subNode.existChild(lxBoxableAttr) && subNode.existChild(lxStacksafeAttr)) {
               SNode createNode = subNode.findChild(lxCreatingStruct);
               if (createNode != lxNone && targetNode == lxLocalAddress) {
                  // if it is implicit conversion
                  createNode.set(targetNode.type, targetNode.argument);

                  node = lxExpression;
                  targetNode = lxIdle;
               }
            }
         }
      }
   }

   return sourceRef;
}

ref_t Compiler :: optimizeBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
{
   if (node == lxCondBoxing && test(mode, HINT_NOCONDBOXING))
      node = lxBoxing;

   if (node == lxUnboxing && test(mode, HINT_NOUNBOXING))
      node = lxBoxing;

   ref_t targetRef = node.findChild(lxTarget).argument;
   ref_t sourceRef = 0;
   bool boxing = !test(mode, HINT_NOBOXING);

   // HOTFIX : override the stacksafe attribute if the object must be boxed
   if (!boxing && node.existChild(lxBoxingRequired))
      boxing = true;

   SNode sourceNode = node.findSubNodeMask(lxObjectMask);
   if (sourceNode == lxBoxing) {
      SNode newNode = sourceNode.findSubNodeMask(lxObjectMask);
      if (newNode == lxNewOp) {
         // HOTFIX : boxing inside boxing
         sourceNode = lxExpression;

         sourceNode = newNode;
      }
   }

   if (sourceNode == lxNewOp) {
      // HOTFIX : set correct target for the new operator
      sourceNode.setArgument(targetRef);
   
      optimizeExpression(sourceNode, scope, warningScope, HINT_NOBOXING);

      boxing = false;
   }
   else {
      // for boxing stack allocated / embeddable variables - source is the same as target
      if ((sourceNode == lxLocalAddress || sourceNode == lxFieldAddress || sourceNode == lxLocal || sourceNode == lxThisLocal) && node.argument != 0) {
         sourceRef = targetRef;
      }
      // HOTFIX : do not box constant classes
      else if (sourceNode == lxConstantInt && targetRef == scope.intReference) {
         boxing = false;
      }
      else if (sourceNode == lxConstantSymbol && targetRef == scope.intReference) {
         boxing = false;
      }
      else sourceRef = optimizeExpression(sourceNode, scope, warningScope, HINT_NOBOXING);

      // adjust primitive target
      if (_logic->isPrimitiveRef(targetRef) && boxing) {
         targetRef = _logic->resolvePrimitiveReference(scope, targetRef);
         node.findChild(lxTarget).setArgument(targetRef);
      }

      if (!_logic->validateBoxing(scope, *this, node, targetRef, sourceRef)) {
         scope.raiseError(errIllegalOperation, node);
      }
   }

   if (!boxing && node != lxLocalUnboxing) {
      node = lxExpression;
   }

   return targetRef; 
}

ref_t Compiler :: optimizeArgBoxing(SNode node, ModuleScope& scope, WarningScope&, int mode)
{
   bool boxing = !test(mode, HINT_NOBOXING);
   if (!boxing)
      node = lxExpression;

   return scope.arrayReference;
}

ref_t Compiler :: optimizeSymbol(SNode& node, ModuleScope& scope, WarningScope&)
{
   ObjectInfo result = scope.defineObjectInfo(node.argument, true);
   switch (result.kind) {
      case okConstantClass:
         node = lxConstantClass;
         break;
      case okConstantSymbol:
         node = lxConstantSymbol;
         break;
   }
   
   return resolveObjectReference(scope, result);
}

ref_t Compiler :: optimizeOp(SNode current, ModuleScope& scope, WarningScope& warningScope)
{
   SNode loperand = current.firstChild(lxObjectMask);
   optimizeExpression(loperand, scope, warningScope, HINT_NOBOXING);

   SNode roperand = loperand.nextNode(lxObjectMask);
   optimizeExpression(roperand, scope, warningScope, HINT_NOBOXING);

   SNode roperand2 = roperand.nextNode(lxObjectMask);
   if (roperand2 != lxNone)
      optimizeExpression(roperand2, scope, warningScope, HINT_NOBOXING);

   switch (current) {
      case lxIntOp:
      case lxByteArrOp:
      case lxIntArrOp:
      case lxShortArrOp:
         return V_INT32;
      case lxLongOp:
         return V_INT64;
      case lxRealOp:
         return V_REAL64;
      case lxBinArrOp:
         return V_BINARY;
      default:
         return 0;
   }
}

ref_t Compiler :: optimizeExpression(SNode current, ModuleScope& scope, WarningScope& warningScope, int mode)
{
   switch (current.type) {
      case lxCalling:
      case lxDirectCalling:
      case lxSDirctCalling:
         return optimizeMessageCall(current, scope, warningScope);
      case lxExpression:
      case lxReturning:
         return optimizeExpression(current.firstChild(lxObjectMask), scope, warningScope, mode);
      case lxAltExpression:
      case lxBranching:
      case lxTrying:
         optimizeExpressionTree(current, scope, warningScope);
         return 0;
      case lxBoxing:
      case lxCondBoxing:
      case lxUnboxing:
         return optimizeBoxing(current, scope, warningScope, mode);
      case lxArgBoxing:
         return optimizeArgBoxing(current, scope, warningScope, mode);
      case lxArgUnboxing:
         return optimizeArgUnboxing(current, scope, warningScope, mode);
      case lxAssigning:
         return optimizeAssigning(current, scope, warningScope);
      case lxSymbolReference:
         return optimizeSymbol(current, scope, warningScope);
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
      case lxIntArrOp:
      case lxShortArrOp:
      case lxByteArrOp:
      case lxArrOp:
      case lxBinArrOp:
      case lxNewOp:
      case lxArgArrOp:
         return optimizeOp(current, scope, warningScope);
      case lxInternalCall:
         return optimizeInternalCall(current, scope, warningScope);
      case lxStdExternalCall:
      case lxExternalCall:
      case lxCoreAPICall:
         return optimizeExtCall(current, scope, warningScope);
      case lxLooping:
      case lxSwitching:
      case lxOption:
      case lxElse:
      //case lxCode:
      //case lxIf:
      //case lxExternFrame:
         optimizeExpressionTree(current, scope, warningScope);
         return 0;
      case lxNested:
         return optimizeNestedExpression(current, scope, warningScope);
      default:
         return current.findChild(lxTarget).argument;
   }
}

void Compiler :: optimizeExpressionTree(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxLooping:
         case lxElse:
         case lxCode:
         case lxIf:
         case lxExternFrame:
         case lxBranching:
            optimizeExpressionTree(current, scope, warningScope);
            break;
         default:
            if (test(current.type, lxObjectMask)) {
               optimizeExpression(current, scope, warningScope, mode);
            }
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: optimizeCode(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxReturning:
            optimizeExpressionTree(current, scope, warningScope, HINT_NOUNBOXING | HINT_NOCONDBOXING);
            break;
         case lxExpression:
         case lxExternFrame:
         case lxDirectCalling:
         case lxSDirctCalling:
         case lxCalling:
            optimizeExpressionTree(current, scope, warningScope);
            break;
      }
      current = current.nextNode();
   }
}

void Compiler :: optimizeMethod(SNode node, ModuleScope& scope, WarningScope& warningScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxNewFrame) {
         optimizeCode(current, scope, warningScope);
      }
      current = current.nextNode();
   }
}

void Compiler :: optimizeClassTree(SNode node, ClassScope& scope, WarningScope& warningScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
//         SNode mask = current.findChild(lxWarningMask);
//         if (mask != lxNone)
//            warningScope.warningMask = mask.argument;

         optimizeMethod(current, *scope.moduleScope, warningScope);

         if (test(_optFlag, 1)) {
            if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
               defineEmbeddableAttributes(scope, current);
            }
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: optimizeSymbolTree(SNode node, SourceScope& scope, int warningMask)
{
   WarningScope warningScope(warningMask);

   SNode current = node.firstChild();
   while (current != lxNone) {
      /*if (current == lxWarningMask) {
         warningMask = current.argument;
      }
      else */if (test(current.type, lxExprMask)) {
         optimizeExpressionTree(current, *scope.moduleScope, warningScope, HINT_NOUNBOXING);
      }

      current = current.nextNode();
   }
}

void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
{
   // Optimization : var = get&subject => eval&subject&var[1]
   ref_t type = 0;
   ref_t returnType = classScope.info.methodHints.get(ClassInfo::Attribute(methodNode.argument, maType));
   if (_logic->recognizeEmbeddableGet(*classScope.moduleScope, methodNode, classScope.extensionMode != 0 ? classScope.reference : 0, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGet), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }
   // Optimization : var = getAt&int => read&int&subject&var[2]
   else if (_logic->recognizeEmbeddableGetAt(*classScope.moduleScope, methodNode, classScope.extensionMode != 0 ? classScope.reference : 0, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGetAt), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }
   // Optimization : var = getAt&int => read&int&subject&var[2]
   else if (_logic->recognizeEmbeddableGetAt2(*classScope.moduleScope, methodNode, classScope.extensionMode != 0 ? classScope.reference : 0, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGetAt2), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }
   // Optimization : var = eval&subj&int => eval&subj&var[2]
   else if (_logic->recognizeEmbeddableEval(*classScope.moduleScope, methodNode, classScope.extensionMode != 0 ? classScope.reference : 0, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableEval), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }
   // Optimization : var = eval&int&int => evald&int&subject&var[2]
   else if (_logic->recognizeEmbeddableEval2(*classScope.moduleScope, methodNode, classScope.extensionMode != 0 ? classScope.reference : 0, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableEval2), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }

   // Optimization : subject'get = self / $self
   if (_logic->recognizeEmbeddableIdle(methodNode, classScope.extensionMode != 0)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableIdle), INVALID_REF);

      classScope.save();
   }

   // Optimization : embeddable constructor call
   ref_t message = 0;
   if (_logic->recognizeEmbeddableMessageCall(methodNode, message)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableNew), message);

      classScope.save();
   }
}

void Compiler :: compileForward(SNode ns, ModuleScope& scope)
{
   ident_t shortcut = ns.findChild(lxIdentifier, lxReference).identifier();
   ident_t reference = ns.findChild(lxForward).findChild(lxIdentifier, lxReference).identifier();

   if (!scope.defineForward(shortcut, reference))
      scope.raiseError(errDuplicatedDefinition, ns);
}

void Compiler :: compileIncludeModule(SNode ns, ModuleScope& scope)
{
   ident_t name = ns.findChild(lxIdentifier, lxReference).findChild(lxTerminal).identifier();
   if (name.compare(STANDARD_MODULE))
      // system module is included automatically - nothing to do in this case
      return;

   // check if the module exists
   _Module* module = scope.project->loadModule(name, true);
   if (module) {
      ident_t value = retrieve(scope.defaultNs.start(), name, NULL);
      if (value == NULL) {
         scope.defaultNs.add(module->Name());

         bool duplicateExtensions = false;
         scope.loadModuleInfo(module, duplicateExtensions);
         if (duplicateExtensions)
            scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, ns);
      }
   }
   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, ns);
}

void Compiler :: declareSubject(SyntaxWriter& writer, SNode member, ModuleScope& scope, SyntaxTree& autogenerated)
{
   SNode name = member.findChild(lxIdentifier, lxPrivate);

   bool internalSubject = name == lxPrivate;
   bool invalid = true;

   // map a full type name
   ref_t subjRef = scope.mapNewSubject(name.findChild(lxTerminal).identifier());
   ref_t classRef = 0;

   SNode classNode = member.nextNode().findChild(lxBaseParent);
   if (classNode != lxNone) {
      SNode option = classNode.findChild(lxAttributeValue);
      if (option != lxNone) {
         TemplateScope templateScope(&scope);
         templateScope.templateRef = templateScope.mapTemplate(classNode);
         if (templateScope.templateRef != 0) {
            classRef = scope.subjectHints.get(templateScope.templateRef);
            if (classRef == INVALID_REF) {
               templateScope.loadAttributeValues(classNode.firstChild()/*, false*/);
               templateScope.autogeneratedTree = &autogenerated;

               SyntaxTree buffer;
               SyntaxWriter bufferWriter(buffer);
               if (generateTemplate(bufferWriter, templateScope, true/*, true*/)) {
                  //SyntaxWriter writer(autogenerated);
      
                  SyntaxTree::moveNodes(writer, buffer);
      
                  classRef = templateScope.reference;
      
                  invalid = false;
               }
            }
         }
      }
      else {
         SNode terminal = classNode.findChild(lxPrivate, lxIdentifier, lxReference);

         classRef = scope.mapTerminal(terminal);

         invalid = false;

      }
   }
   else invalid = false;

   if (!invalid) {
      scope.saveSubject(subjRef, classRef, internalSubject);
   }
   else scope.raiseError(errInvalidHint, name);
}

bool Compiler :: validate(_ProjectManager& project, _Module* module, int reference)
{
   int   mask = reference & mskAnyRef;
   ref_t extReference = 0;
   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
   _Module* extModule = project.resolveModule(refName, extReference, true);

   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
}

void Compiler :: validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project)
{
   for (List<Unresolved>::Iterator it = unresolveds.start() ; !it.Eof() ; it++) {
      if (!validate(project, (*it).module, (*it).reference)) {
         ident_t refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);

         project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
      }
   }
}

inline void addPackageItem(SyntaxWriter& writer, _Module* module, ident_t str)
{
   writer.newNode(lxMember);
   if (!emptystr(str)) {
      writer.appendNode(lxConstantString, module->mapConstant(str));
   }
   else writer.appendNode(lxNil);
   writer.closeNode();
}

inline ref_t mapForwardRef(_Module* module, _ProjectManager& project, ident_t forward)
{
   ident_t name = project.resolveForward(forward);

   return emptystr(name) ? 0 : module->mapReference(name);
}

void Compiler :: createPackageInfo(_Module* module, _ProjectManager& project)
{
   ReferenceNs sectionName(module->Name(), PACKAGE_SECTION);
   ref_t reference = module->mapReference(sectionName);
   ref_t vmtReference = mapForwardRef(module, project, SUPER_FORWARD);
   if (vmtReference == 0)
      return;

   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxConstantList, reference);
   writer.appendNode(lxTarget, vmtReference);

   // namespace
   addPackageItem(writer, module, module->Name());

   // package name
   addPackageItem(writer, module, project.getManinfestName());

   // package version
   addPackageItem(writer, module, project.getManinfestVersion());

   // package author
   addPackageItem(writer, module, project.getManinfestAuthor());

   writer.closeNode();

   _writer.generateConstantList(tree.readRoot(), module, reference);
}

void Compiler :: compileImplementations(SNode node, ModuleScope& scope)
{
   SyntaxTree expressionTree; // expression tree is reused

   // second pass - implementation
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxClass:
         {
            //ident_t name = scope.module->resolveReference(current.argument);

            // compile class
            ClassScope classScope(&scope, current.argument);
            scope.loadClassInfo(classScope.info, scope.module->resolveReference(current.argument), false);

            compileClassImplementation(expressionTree, current, classScope);
   
            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef);
               scope.loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);
   
               compileClassClassImplementation(expressionTree, current, classClassScope, classScope);
            }
            break;
         }
         case lxSymbol:
         {
            SymbolScope symbolScope(&scope, current.argument);
            compileSymbolImplementation(expressionTree, current, symbolScope);
            break;
         }
      }
      current = current.nextNode();
   }
}

void Compiler :: compileDeclarations(SNode node, ModuleScope& scope)
{
   SNode current = node.firstChild();

   //   if (scope.superReference == 0)
   //      scope.raiseError(errNotDefinedBaseClass, node.firstChild().firstChild(lxTerminalMask));

   // first pass - declaration
   while (current != lxNone) {
      SNode name = current.findChild(lxIdentifier, lxPrivate, lxReference);

      switch (current) {
         case lxInclude:
            compileForward(current, scope);
            break;
         case lxClass:
         {
            current.setArgument(/*name == lxNone ? scope.mapNestedExpression() : */scope.mapTerminal(name));

            ClassScope classScope(&scope, current.argument);
      
            // check for duplicate declaration
            if (scope.module->mapSection(classScope.reference | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);
                  
            scope.module->mapSection(classScope.reference | mskSymbolRef, false);
                  
            // build class expression tree
            compileClassDeclaration(current, classScope);
            break;
         }
         case lxSymbol:
         {
            if (name == lxNone) {
               // HOTFIX : for script generated anonymous symbol
               current.setArgument(scope.mapNestedExpression());
            }
            else current.setArgument(scope.mapTerminal(name));

            SymbolScope symbolScope(&scope, current.argument);
            
            // check for duplicate declaration
            if (scope.module->mapSection(symbolScope.reference | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);
                        
            scope.module->mapSection(symbolScope.reference | mskSymbolRef, false);
            
            // declare symbol
            compileSymbolDeclaration(current, symbolScope);
            break;
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: compileSyntaxTree(SyntaxTree& syntaxTree, ModuleScope& scope)
{
   // HOTFIX : the module path should be presaved in debug section
   scope.sourcePathRef = _writer.writeSourcePath(scope.debugModule, scope.sourcePath);
   
   // HOTFIX : the module path should be the first saved string
   _writer.clear();
   _writer.writeString(scope.sourcePath);

   // declare 
   compileDeclarations(syntaxTree.readRoot(), scope);

   // compile
   compileImplementations(syntaxTree.readRoot(), scope);
}

void Compiler :: compileSyntaxTree(_ProjectManager& project, ident_t file, SyntaxTree& syntaxTree, ModuleInfo& info, Unresolveds& unresolveds)
{
   ModuleScope scope(&project, file, info.codeModule, info.debugModule, &unresolveds);
   
   project.printInfo("%s", file);

   compileSyntaxTree(syntaxTree, scope);
}

inline SNode findLastAttribute(SNode current)
{
   SNode lastAttribute;
   while (current == lxAttribute) {
      lastAttribute = current;
      current = current.nextNode();
   }

   return lastAttribute;
}

inline bool setIdentifier(SNode current)
{
   SNode lastAttribute = findLastAttribute(current);

   if (lastAttribute == lxAttribute) {
      lastAttribute = lxNameAttr;

      current.refresh();

      return true;
   }
   else return false;
}

void Compiler :: generateMessageTree(SyntaxWriter& writer, SNode node, TemplateScope& scope/*, bool operationMode*/)
{
//   if (operationMode)
//      writer.newBookmark();

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxObject:
         case lxMessageParameter:
            generateExpressionTree(writer, current, scope, false);
            break;
         case lxExtern:
            generateCodeTree(writer, current, scope);
            break;
         case lxCode:
            generateCodeTree(writer, current, scope);
            if (scope.type == TemplateScope::ttCodeTemplate) {
               if (scope.parameters.Count() == 2) {
                  if (scope.codeNode == lxNone) {
                     writer.insert(lxTemplateParam);
                     scope.codeNode = current;
                  }
                  else {
                     writer.insert(lxTemplateParam, 3);
                     scope.codeNode = SNode();
                  }
               }
               else writer.insert(lxTemplateParam);
               writer.closeNode();
            }

            writer.insert(lxExpression);
            writer.closeNode();
            break;
         case lxMessage:
            writer.newNode(lxMessage);
            scope.copySubject(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
            break;
         case lxIdentifier:
         case lxPrivate:
         case lxReference:
            writer.newNode(lxMessage);
            scope.copySubject(writer, current);
            writer.closeNode();
            break;
      }
      current = current.nextNode();
   }
   
//   if (operationMode) {
//      writer.removeBookmark();
//   }
}

inline void copyAutogeneratedClass(SyntaxTree& sourceTree, SyntaxTree& destionationTree)
{
   SyntaxWriter writer(destionationTree);

   SyntaxTree::moveNodes(writer, sourceTree, lxClass);
}

void Compiler :: generateVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   // check if the first token is attribute
   SNode current = node.firstChild();
   SNode attr = node.findChild(lxIdentifier, lxPrivate);
   int dummy = 0;
   ref_t attrRef = (attr != lxPrivate) ? scope.mapAttribute(attr, dummy/*, true*/) : 0;
   //HOTFIX : there should be at leat two attribute
   if (attrRef != 0 && attr.nextNode() != lxAssigning) {
      // HOTFIX : set already recognized attribute value if it is not a template parameter
      if (attrRef != INVALID_REF) {
         attr.setArgument(attrRef);
      }

      while (current != lxAssigning) {
         current = lxAttribute;

         current = current.nextNode();
      }

      writer.newNode(lxVariable);

      setIdentifier(node.firstChild());
      SNode ident = node.findChild(lxNameAttr);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);

      generateAttributes(bufferWriter, SNode(), scope, node.firstChild()/*, true*/);
      SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxTypeAttr, lxClassRefAttr, lxTemplateType);

//      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));

      writer.closeNode();

      writer.newNode(lxExpression);

      copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));
      writer.appendNode(lxAssign);
      generateExpressionTree(writer, current, scope, false);

      writer.closeNode();
   }
   else generateExpressionTree(writer, node, scope);
}

void Compiler :: generateArrayVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   // check if the first token is attribute
   SNode attr = node.findChild(lxIdentifier, lxPrivate);
   int dummy = 0;
   ref_t attrRef = (attr != lxPrivate) ? scope.mapAttribute(attr, dummy/*, true*/) : 0;
   if (!attrRef)
      scope.raiseError(errInvalidHint, attr);

   // HOTFIX : set already recognized attribute value
   attr.setArgument(attrRef);
   SNode current = node.firstChild();
   while (current != lxNone) {
      current = lxAttribute;

      current = current.nextNode();
   }

   writer.newNode(lxVariable);

   setIdentifier(node.findChild(lxAttribute));
   SNode ident = node.findChild(lxNameAttr);

   generateAttributes(writer, SNode(), scope, node.firstChild()/*, true*/);

   copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));

   writer.closeNode();
}


void Compiler :: generateTemplateVariableTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, bool templateMode)
{
   SNode attr = node.findChild(lxIdentifier, lxPrivate);

   SNode current = node.firstChild();
   while (current != lxNone) {
      current = lxAttribute;

      current = current.nextNode();
   }

   setIdentifier(node.findChild(lxAttribute));
   SNode ident = node.findChild(lxNameAttr);

   int prefixCounter = SyntaxTree::countChild(ident, lxAttributeValue);
   IdentifierString attrName(node.findChild(lxAttribute).findChild(lxTerminal).identifier());
   attrName.append('#');
   attrName.appendInt(prefixCounter);

   ref_t attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
   if (!attrRef || scope.moduleScope->subjectHints.get(attrRef) != INVALID_REF)
      scope.raiseError(errInvalidHint, node);

   // if it is an autogenerated class
   if (templateMode) {
      // template in template should be copied "as is" (resolving all references)
      writer.newNode(lxTemplateVar, -1);
      writer.appendNode(lxReference, scope.moduleScope->module->resolveSubject(attrRef));
      copyAttributeTree(writer, ident.findChild(lxAttributeValue), scope);
      copyIdentifier(writer, ident.firstChild());
      writer.closeNode();
   }
   else {
      TemplateScope templateScope(&scope, attrRef);
      templateScope.loadAttributeValues(ident.findChild(lxAttributeValue));

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);

      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      writer.newNode(lxVariable);
      writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
      copyIdentifier(writer, ident.firstChild());
      writer.closeNode();
   }
}

void Compiler :: generateCodeTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   // check if the first token is attribute
   SNode loperand = node.firstChild();
   SNode attr = node.findChild(lxIdentifier);
   if (attr != lxNone) {
      IdentifierString attrName(attr.findChild(lxTerminal).identifier());
      attrName.append('#');
      attrName.appendInt(SyntaxTree::countChild(node, lxCode, lxNestedClass));

      ref_t attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
      if (attrRef != 0) {
         ref_t classRef = scope.moduleScope->subjectHints.get(attrRef);
         if (classRef == INVALID_REF) {
            TemplateScope templateScope(&scope, attrRef);
            templateScope.exprNode = node.findChild(lxExpression);
            templateScope.codeNode = node.findChild(lxCode);
            templateScope.nestedNode = node.findChild(lxNestedClass);
            if (templateScope.nestedNode == lxNone || templateScope.codeNode != lxNone) {
               // if there is else code block
               templateScope.elseNode = templateScope.codeNode.nextNode();
            }
            
            templateScope.type = TemplateScope::ttCodeTemplate;
            
            if (!generateTemplateCode(writer, templateScope))
               scope.raiseError(errInvalidHint, node);
         }

         return;
      }
   }

   generateExpressionTree(writer, node, scope);
}

inline bool isTerminal(LexicalType type)
{
   switch (type)
   {
      case lxIdentifier:
      case lxPrivate:
      case lxInteger:
      case lxHexInteger:
      case lxLong:
      case lxReal:
      case lxLiteral:
      case lxReference:
      case lxCharacter:
      case lxWide:
      case lxExplicitConst:
         return true;
      default:
         return false;
   }
}

void Compiler :: generateSwitchTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxSwitchOption:
         case lxBiggerSwitchOption:
         case lxLessSwitchOption:
            if (current.type == lxBiggerSwitchOption) {
               writer.newNode(lxOption, GREATER_MESSAGE_ID);
            }
            else if (current.type == lxLessSwitchOption) {
               writer.newNode(lxOption, LESS_MESSAGE_ID);
            }
            else writer.newNode(lxOption, EQUAL_MESSAGE_ID);
            generateExpressionTree(writer, current, scope, false);
            writer.closeNode();
            break;
         case lxLastSwitchOption:
            writer.newNode(lxElse);
            generateExpressionTree(writer, current, scope, false);
            writer.closeNode();
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: generateObjectTree(SyntaxWriter& writer, SNode current, TemplateScope& scope/*, int mode*/)
{
   switch (current.type) {
      case lxAssigning:
         writer.appendNode(lxAssign);
         generateExpressionTree(writer, current, scope, false);
         break;
      case lxSwitching:
         generateSwitchTree(writer, current, scope);
         writer.insert(lxSwitching);
         writer.closeNode();
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxOperator:
         copyOperator(writer, current.firstChild());
         generateExpressionTree(writer, current, scope, false);
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxCatchOperation:
      case lxAltOperation:
         if (scope.type == TemplateScope::ttCodeTemplate && scope.templateRef == 0) {
            // HOTFIX : for try-catch template
            scope.codeNode = SNode();
         }
         writer.newBookmark();
      case lxMessage:
         if (current.argument == -1 && current.nextNode() == lxMethodParameter) {
            writer.newNode(lxClosureMessage);
            copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
            writer.closeNode();
         }
         else {
            generateMessageTree(writer, current, scope/*, false*/);

            writer.insert(lxExpression);
            writer.closeNode();
         }
         if (current == lxCatchOperation) {
            writer.removeBookmark();
            writer.insert(lxTrying);
            writer.closeNode();
         }
         else if (current == lxAltOperation) {
            writer.removeBookmark();
            writer.insert(lxAlt);
            writer.closeNode();
         }
         break;
      case lxExtension:
         writer.newNode(current.type, current.argument);
         generateExpressionTree(writer, current, scope, false);
         writer.closeNode();
         break;
      case lxExpression:
         generateExpressionTree(writer, current, scope, false);
         break;
      case lxMessageReference:
      case lxLazyExpression:
         writer.newNode(lxExpression);
         writer.newNode(current.type);
         if (current == lxLazyExpression) {
            generateExpressionTree(writer, current, scope, false);
         }
         else copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate, lxLiteral));
         writer.closeNode();
         writer.closeNode();
         break;
      case lxObject:
         generateExpressionTree(writer, current, scope, false);
         break;
      case lxNestedClass:
         if (scope.type == TemplateScope::ttCodeTemplate) {
            writer.insert(lxTemplateParam, 2);
            writer.closeNode();
         }
         else {
            generateScopeMembers(current, scope);

            generateClassTree(writer, current, scope, SNode(), -1);
         }
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxCode:
         generateCodeTree(writer, current, scope);
         if (scope.type == TemplateScope::ttCodeTemplate) {
            if (scope.parameters.Count() == 2) {
               if (scope.codeNode == lxNone) {
                  writer.insert(lxTemplateParam);
                  scope.codeNode = current;
               }
               else {
                  writer.insert(lxTemplateParam, 3);
                  scope.codeNode = SNode();
               }
            }
            else writer.insert(lxTemplateParam);
            writer.closeNode();
         }
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxMethodParameter:
         writer.newNode(lxMethodParameter);
         copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
         writer.closeNode();
         break;
      default:
      {
         if (isTerminal(current.type)) {
            if (scope.type == TemplateScope::ttFieldTemplate) {
               int index = scope.mapIdentifier(current);
               if (index != 0) {
                  writer.newNode(lxTemplateField, index);
                  copyIdentifier(writer, current);
                  writer.closeNode();
               }
               else copyIdentifier(writer, current);
            }
            else if (scope.type == TemplateScope::ttCodeTemplate && scope.mapIdentifier(current)) {
               writer.newNode(lxTemplateParam, 1);
               copyIdentifier(writer, current);
               writer.closeNode();
            }
            else copyIdentifier(writer, current);
         }               
         break;
      }
   }
   //
   //   if (mode == 1 && exprCounter > 1) {
   //      _writer.insert(lxExpression);
   //      _writer.closeNode();
   //   }
}

void Compiler :: generateExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, bool explicitOne)
{
   writer.newBookmark();

   SNode current = node.firstChild();
   bool identifierMode = current.type == lxIdentifier;
   bool listMode = false;
   // check if it is new operator
   if (identifierMode && current.nextNode() == lxExpression && current.nextNode().nextNode() != lxExpression) {
      scope.copySubject(writer, current);

      writer.appendNode(lxOperator, -1);
      generateExpressionTree(writer, current.nextNode(), scope, false);
      writer.insert(lxExpression);
      writer.closeNode();
   }
   else {
      while (current != lxNone) {
         if (current == lxExpression) {
            if (current.nextNode() == lxExpression)
               listMode = true;

            if (identifierMode) {
               generateExpressionTree(writer, current, scope, listMode);
            }
            else if (listMode) {
               generateExpressionTree(writer, current, scope, true);
            }
            else generateObjectTree(writer, current, scope);
         }
         else if (listMode && (current == lxMessage || current == lxOperator)) {
            // HOTFIX : if it is an operation with a collection
            listMode = false;
            writer.insert(lxExpression);
            writer.closeNode();

            generateObjectTree(writer, current, scope);
         }
         else generateObjectTree(writer, current, scope);

         current = current.nextNode();
      }
   }

   if (listMode) {
      writer.insert(lxExpression);
      writer.closeNode();
   }

   if (explicitOne) {
      writer.insert(node.type);
      writer.closeNode();
   }

   writer.removeBookmark();
}

void Compiler :: generateSymbolTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   writer.newNode(lxSymbol);

   generateAttributes(writer, node, scope, attributes/*, false*/);

   generateExpressionTree(writer, node.findChild(lxExpression), scope);

   writer.closeNode();
}

inline bool isArrayDeclaration(SNode node)
{
   SNode current = node.findChild(lxMessage);

   return current.existChild(lxSize);
}

inline bool isTemplateDeclaration(SNode node)
{
   SNode current = node.findChild(lxMessage);

   return current.existChild(lxAttributeValue);
}

void Compiler :: generateCodeTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   writer.newNode(node.type, node.argument);

   bool withBreakpoint = (node == lxReturning || node == lxResendExpression);
   if (withBreakpoint)
      writer.newBookmark();

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression || current == lxReturning) {
         if (current.existChild(lxAssigning)) {
            generateVariableTree(writer, current, scope);
         }
         else if (isArrayDeclaration(current)) {
            generateArrayVariableTree(writer, current, scope);
         }
         else if (isTemplateDeclaration(current)) {
            generateTemplateVariableTree(writer, current, scope, scope.reference == INVALID_REF);
         }
         else if (current.existChild(lxCode) || current.existChild(lxNestedClass)) {
            generateCodeTemplateTree(writer, current, scope);
         }
         else generateExpressionTree(writer, current, scope);
      }
      else if (current == lxEOF) {
         writer.newNode(lxEOF);

         SNode terminal = current.firstChild();
         SyntaxTree::copyNode(writer, lxRow, terminal);
         SyntaxTree::copyNode(writer, lxCol, terminal);
         SyntaxTree::copyNode(writer, lxLength, terminal);
         writer.closeNode();
      }
      else if (current == lxLoop || current == lxCode || current == lxExtern) {
         generateCodeTree(writer, current, scope);
      }
      else generateObjectTree(writer, current, scope);

      current = current.nextNode();
   }

   if (withBreakpoint)
      writer.removeBookmark();

   writer.closeNode();
}

void Compiler :: copyMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SyntaxTree& buffer)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope/*, true*/);

      current = current.nextNode();
   }

   writer.closeNode();

   SyntaxTree::moveNodes(writer, buffer, lxClassMethod);
}

void Compiler::copyTreeNode(SyntaxWriter& writer, SNode current, TemplateScope& scope/*, bool methodMode*/)
{
   if (test(current.type, lxTerminalMask | lxObjectMask)) {
      scope.copyIdentifier(writer, current);
   }
   else if (current == lxTemplate) {
      writer.appendNode(lxTemplate, scope.templateRef);
   }
   else if (current == lxTemplateParam) {
      if (scope.type == TemplateScope::ttCodeTemplate && current.argument == 1) {
         // if it is a code template parameter
         TemplateScope* parentScope = (TemplateScope*)scope.parent;

         generateExpressionTree(writer, scope.exprNode, *parentScope, true);
      }
      else if (scope.type == TemplateScope::ttCodeTemplate && current.argument == 0) {
         // if it is a code template parameter
         TemplateScope* parentScope = (TemplateScope*)scope.parent;

         generateCodeTree(writer, scope.codeNode, *parentScope);
      }
      else if (scope.type == TemplateScope::ttCodeTemplate && current.argument == 3) {
         TemplateScope* parentScope = (TemplateScope*)scope.parent;

         // if it is an else code template parameter
         SNode subParam = current.findSubNode(lxTemplateParam);
         if (subParam == lxTemplateParam && subParam.argument == 0) {
            // HOTFIX : insert if-else code
            generateCodeTree(writer, scope.codeNode, *parentScope);
         }

         generateCodeTree(writer, scope.elseNode, *parentScope);
      }
      else if (scope.type == TemplateScope::ttCodeTemplate && current.argument == 2) {
         // if it is a code template parameter
         TemplateScope* parentScope = (TemplateScope*)scope.parent;

         writer.newBookmark();
         generateObjectTree(writer, scope.nestedNode, *parentScope);
         writer.removeBookmark();
      }
      else {
         // if it is a template parameter
         ref_t subjRef = scope.subjects.get(current.argument);
         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
         if (subjName.find('$') != NOTFOUND_POS) {
            writer.newNode(lxReference, subjName);
         }
         else writer.newNode(lxIdentifier, subjName);

         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
   }
   else if (current == lxTemplateField && current.argument >= 0) {
      ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();

      writer.newNode(lxIdentifier, fieldName);

      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else if (current == lxTemplateType) {
      ref_t subjRef = scope.subjects.get(current.argument);
      ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
      writer.newNode(lxTypeAttr, subjName);

      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else if (current == lxTypeAttr || current == lxClassRefAttr) {
      writer.appendNode(current.type, current.identifier());
   }
   else if (current == lxTemplateVar && current.argument == INVALID_REF) {
      ref_t attrRef = scope.moduleScope->module->mapSubject(current.findChild(lxReference).identifier(), false);

      TemplateScope templateScope(&scope, attrRef);
      templateScope.loadAttributeValues(current.firstChild()/*, true*/);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);
      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      writer.newNode(lxVariable);
      writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
      copyIdentifier(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else copyExpressionTree(writer, current, scope);
}

void Compiler :: copyExpressionTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   if (node.strArgument != -1) {
      writer.newNode(node.type, node.identifier());
   }
   else writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

void Compiler :: copyFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SyntaxTree& buffer)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIdentifier || current == lxPrivate || current == lxReference) {
         copyIdentifier(writer, current);
      }
//      else if (current == lxTemplateParam) {
//         ref_t subjRef = scope.subjects.get(current.argument);
//         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
//         if (subjName.find('$') != NOTFOUND_POS) {
//            writer.newNode(lxReference, subjName);
//         }
//         else writer.newNode(lxIdentifier, subjName);
//
//         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//         writer.closeNode();
//      }
      else if (current == lxTemplateField && current.argument >= 0) {
         ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();

         writer.newNode(lxIdentifier, fieldName);

         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else if (current == lxTemplateType) {
         ref_t subjRef = scope.subjects.get(current.argument);
         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
         writer.newNode(lxTypeAttr, subjName);

         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else if (current == lxTypeAttr || current == lxClassRefAttr) {
         writer.appendNode(current.type, current.identifier());
      }

      current = current.nextNode();
   }

   writer.closeNode();

   SyntaxTree::moveNodes(writer, buffer, lxClassMethod, lxClassField);
}

void Compiler :: copyTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributeValues/*, bool variableMode, bool embeddableMode*/)
{
   scope.loadAttributeValues(attributeValues/*, variableMode*/);

   if (generateTemplate(writer, scope, false/*, embeddableMode*/)) {
      //if (/*variableMode && */scope.reference != 0)
      //   writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(scope.reference));
   }
   else scope.raiseError(errInvalidHint, node);
}

bool Compiler :: generateTemplate(SyntaxWriter& writer, TemplateScope& scope, bool declaringClass/*, bool embeddableMode*/)
{
   _Memory* body = scope.moduleScope->loadAttributeInfo(scope.templateRef);
   if (body == NULL)
      return false;
   
   SyntaxTree templateTree(body);
   
   if (declaringClass) {
      // HOTFIX : exiting if the class was already declared in this module
      if (!scope.generateClassName())
         return true;

      writer.newNode(lxClass, -1);
      writer.appendNode(lxReference, scope.moduleScope->module->resolveReference(scope.reference));
      writer.appendNode(lxAttribute, V_SEALED);
   }

   SyntaxTree buffer;

   SNode current = templateTree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (current.argument == INVALID_REF) {
            TemplateScope templateScope(&scope, scope.moduleScope->module->mapSubject(current.findChild(lxReference).identifier(), false));

            copyTemplateTree(writer, current, templateScope, current.firstChild()/*, false, embeddableMode*/);
         }
         else if (current.argument == V_TEMPLATE && scope.type != TemplateScope::ttAttrTemplate) {
            // ignore template attributes
         }
         else if (current.argument == V_FIELD && scope.type != TemplateScope::ttAttrTemplate) {
            // ignore template attributes
         }
         else if (current.argument == V_METHOD && scope.type != TemplateScope::ttAttrTemplate) {
            if (scope.type == TemplateScope::Type::ttFieldTemplate) {
               // HOTFIX : is it is a method template, consider the field name as a message subject
               scope.type = TemplateScope::Type::ttMethodTemplate;

               ForwardMap::Iterator it = scope.fields.start();
               while (!it.Eof()) {
                  scope.subjects.add(scope.subjects.Count() + 1, scope.moduleScope->module->mapSubject(it.key(), false));

                  it++;
               }

               //scope.subjects.add()
            }
         }
         else writer.appendNode(current.type, current.argument);
      }
//      else if (current == lxTemplateType) {
//         ref_t subjRef = scope.subjects.get(current.argument);
//         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
//         writer.newNode(lxTypeAttr, subjName);
//
//         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//         writer.closeNode();
//      }
      else if (current == lxClassMethod) {
         copyMethodTree(writer, current, scope, buffer);
      }
      else if (current == lxClassField) {
         copyFieldTree(writer, current, scope, buffer);
      }
      else if (current == lxTemplateField && current.argument == INVALID_REF) {
         ref_t attrRef = scope.moduleScope->module->mapSubject(current.findChild(lxReference).identifier(), false);
            
         TemplateScope templateScope(&scope, attrRef);
         templateScope.loadAttributeValues(current.firstChild()/*, true*/);

         SyntaxWriter bufferWriter(buffer);
         generateTemplate(bufferWriter, templateScope, true);

         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

         writer.newNode(lxClassField);
         writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
         if (scope.type == TemplateScope::ttFieldTemplate) {
            SNode field = current.findChild(lxTemplateField);
            if (field != lxNone) {
               ident_t fieldName = retrieveIt(scope.fields.start(), field.argument).key();

               writer.newNode(lxIdentifier, fieldName);
               SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
               writer.closeNode();
            }
            else copyIdentifier(writer, current.findChild(lxIdentifier));
         }
         else copyIdentifier(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      current = current.nextNode();
   }

   if (declaringClass) {
      writer.closeNode();
   }
   
   //if (scope.autogeneratedTree && declaringClass)
   //   copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

   return true;
}

bool Compiler :: generateTemplateCode(SyntaxWriter& writer, TemplateScope& scope)
{
   _Memory* body = scope.moduleScope->loadAttributeInfo(scope.templateRef);
   if (body == NULL)
      return false;

   SyntaxTree templateTree(body);

   scope.loadAttributeValues(templateTree.readRoot()/*, false*/);

   SNode current = templateTree.readRoot().findChild(lxCode).firstChild();
   while (current != lxNone) {
      if (current.type == lxLoop || current.type == lxExpression || current.type == lxExtern)
         copyExpressionTree(writer, current, scope);

      current = current.nextNode();
   }

   return true;
}

void Compiler :: copyAttributeTree(SyntaxWriter& writer, SNode node, TemplateScope& scope)
{
   // copy coordinate
   SNode ident = node.findChild(lxIdentifier);
   if (ident == lxNone)
      ident = node.findChild(lxTerminal);

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);

   SNode current = node;
   // validare template parameters
   while (current != lxNone) {
      if (current == lxAttributeValue) {
         SNode terminalNode = current.firstChild(lxObjectMask);
         int attrValue = 0;
         ref_t attrRef = scope.mapAttribute(current, attrValue);
         if (attrRef == INVALID_REF) {
            writer.appendNode(lxTemplateType, attrValue);
         }
         else if (attrValue != 0) {
            writer.appendNode(lxAttribute, attrValue);
         }
         else if (attrRef != 0) {
            writer.appendNode(lxTypeAttr, scope.moduleScope->module->resolveSubject(attrRef));
         }
         else {
            writer.newNode(lxAttributeValue);
            copyIdentifier(writer, terminalNode);
            writer.closeNode();
         }
      }
      else if (current == lxNameAttr)
         break;

      current = current.nextNode();
   }
}

void Compiler :: generateAttributes(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   SNode current = attributes;
   while (current == lxAttribute || current == lxAttributeDecl) {
      int attrValue = 0;
      ref_t attrRef = current.argument;
      if (!attrRef)
         attrRef = scope.mapAttribute(current, attrValue/*, variableMode*/);

      if (attrRef == INVALID_REF) {
         writer.appendNode(lxTemplateType, attrValue);
      }
      else if (attrValue != 0) {
         writer.appendNode(lxAttribute, attrValue);
      }
      else if (attrRef != 0) {
         ref_t classRef = scope.moduleScope->subjectHints.get(attrRef);
         if (classRef == INVALID_REF) {
               TemplateScope templateScope(&scope, attrRef);
               if (scope.type == TemplateScope::ttAttrTemplate)
                  templateScope.type = scope.type;

               copyTemplateTree(writer, current, templateScope, current.firstChild());
         }
         else if (_logic->isPrimitiveRef(classRef)) {
            writer.appendNode(lxAttribute, classRef);
         }
         else {
            writer.appendNode(lxTypeAttr, scope.moduleScope->module->resolveSubject(attrRef));
         }            
      }
      else scope.raiseError(errInvalidHint, current);

      current = current.nextNode();
   }

   if (current == lxNameAttr && current.existChild(lxSize)) {
      SNode sizeNode = current.findChild(lxSize).findChild(lxInteger, lxHexInteger);
      if (sizeNode != lxNone) {
         writer.appendNode(lxAttribute, readSizeValue(sizeNode, sizeNode == lxHexInteger ? 16 : 10));
      }
      else scope.raiseError(errInvalidHint, current);
   }

   if (node != lxNone) {
      SNode nameNode = current == lxNameAttr ? current.findChild(lxIdentifier, lxPrivate) : node.findChild(lxIdentifier, lxPrivate);
      if (nameNode != lxNone)
         scope.copySubject(writer, nameNode);
   }
}

void Compiler :: generateMethodTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   SyntaxWriter bufferWriter(buffer);

   writer.newNode(lxClassMethod);
   if (templateMode) {
      writer.appendNode(lxSourcePath, scope.sourcePath);
      writer.appendNode(lxTemplate, scope.templateRef);
   }      

   /*if (node == lxDefaultGeneric) {
      if (node.existChild(lxMethodParameter)) {
         writer.appendNode(lxIdentifier, EVAL_MESSAGE);
      }
      else writer.appendNode(lxIdentifier, GET_MESSAGE);

      writer.appendNode(lxAttribute, V_SEALED);
      writer.appendNode(lxAttribute, V_GENERIC);
   }
   else */generateAttributes(bufferWriter, node, scope, attributes/*, false, true*/);

   // copy attributes
   SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxTypeAttr, lxClassRefAttr, lxTemplateType);

   // copy method signature
   SNode current = goToNode(attributes, lxNameAttr);
   while (current == lxNameAttr || current == lxMessage) {
      if (current == lxMessage) {
         writer.newNode(lxMessage);
         scope.copySubject(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   // copy method arguments
   current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMethodParameter || current == lxMessage) {
         writer.newNode(current.type, current.argument);
         if (current == lxMessage) {
            scope.copySubject(writer, current.firstChild(lxTerminalMask));
         }
         else copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   SNode bodyNode = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (bodyNode != lxNone) {
      if (templateMode)
         scope.reference = INVALID_REF;

      generateCodeTree(writer, bodyNode, scope);
   }

   writer.closeNode();

   // copy methods
   SyntaxTree::moveNodes(writer, buffer, lxClassMethod);
}

void Compiler :: generateFieldTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   SyntaxWriter bufferWriter(buffer);

   writer.newNode(lxClassField, templateMode ? -1 : 0);

   if (scope.type == TemplateScope::Type::ttFieldTemplate) {
      SNode name = goToNode(attributes, lxNameAttr).findChild(lxIdentifier, lxPrivate);

      scope.fields.add(name.findChild(lxTerminal).identifier(), scope.fields.Count() + 1);

      writer.newNode(lxTemplateField, scope.fields.Count());
      copyIdentifier(writer, name);
      writer.closeNode();

      generateAttributes(bufferWriter, SNode(), scope, attributes/*, false, true*/);
   }
   else generateAttributes(bufferWriter, node, scope, attributes/*, false, true*/);

   // copy attributes
   SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxTypeAttr, lxClassRefAttr, lxTemplateType);

   SNode bodyNode = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (bodyNode != lxNone) {
      generateCodeTree(writer, bodyNode, scope);
   }

   writer.closeNode();

   // copy methods
   SyntaxTree::moveNodes(writer, buffer, lxClassMethod, lxClassField);
}

void Compiler :: generateFieldTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   if (node == lxClassField && node.argument == INVALID_REF) {
      SNode ident = goToNode(attributes, lxNameAttr);

      int prefixCounter = SyntaxTree::countChild(node, lxAttributeValue);
      IdentifierString attrName(attributes.findChild(lxIdentifier).findChild(lxTerminal).identifier());
      attrName.append('#');
      attrName.appendInt(prefixCounter);

      ref_t attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
      if (!attrRef || scope.moduleScope->subjectHints.get(attrRef) != INVALID_REF)
         scope.raiseError(errInvalidHint, node);

      // if it is an autogenerated class
      if (templateMode) {
         // template in template should be copied "as is" (resolving all references)
         writer.newNode(lxTemplateField, -1);
         writer.appendNode(lxReference, scope.moduleScope->module->resolveSubject(attrRef));
         copyAttributeTree(writer, node.firstChild(), scope);
         if (scope.type == TemplateScope::ttFieldTemplate) {
            SNode name = ident.findChild(lxIdentifier, lxPrivate);

            scope.fields.add(name.findChild(lxTerminal).identifier(), scope.fields.Count() + 1);

            writer.newNode(lxTemplateField, scope.fields.Count());
            copyIdentifier(writer, name);
            writer.closeNode();
         }
         else copyIdentifier(writer, ident.findChild(lxPrivate, lxIdentifier));
         writer.closeNode();
      }
      else {
         TemplateScope templateScope(&scope, attrRef);
         templateScope.loadAttributeValues(node.firstChild());

         SyntaxWriter bufferWriter(buffer);
         generateTemplate(bufferWriter, templateScope, true);

         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

         writer.newNode(lxClassField);
         writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
         copyIdentifier(writer, ident.findChild(lxPrivate, lxIdentifier));
         writer.closeNode();
      }
   }
   else {
      SNode baseNode = node.findChild(lxBaseParent);
      // if it is field / method template
      int prefixCounter = 0;
      SNode current = attributes;
      while (current == lxAttribute) {
         current = lxAttributeValue;
         prefixCounter++;

         current = current.nextNode();
      }
      attributes.refresh();

      ref_t attrRef = scope.mapTemplate(node.findChild(lxBaseParent), prefixCounter);
      if (!attrRef || scope.moduleScope->subjectHints.get(attrRef) != INVALID_REF)
         scope.raiseError(errInvalidHint, baseNode);

      TemplateScope templateScope(&scope, attrRef);
      templateScope.type = TemplateScope::Type::ttFieldTemplate;
      templateScope.loadFields(node.findChild(lxBaseParent));

      copyTemplateTree(writer, node, templateScope, attributes);
   }
}

void Compiler :: generateTemplateTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   SyntaxTree buffer;

   generateAttributes(writer, node, scope, attributes/*, false*/);

   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute || current == lxNameAttr) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxClassMethod) {
         generateMethodTree(writer, current, scope, subAttributes, buffer, true);
         subAttributes = SNode();
      }
      else if (current == lxClassField) {
         if (current.argument == INVALID_REF) {
            generateFieldTemplateTree(writer, current, scope, subAttributes, buffer, true);
         }
         else generateFieldTree(writer, current, scope, subAttributes, buffer, true);
         subAttributes = SNode();
      }
      else if (current == lxFieldTemplate) {
         generateFieldTemplateTree(writer, current, scope, subAttributes, buffer, true);
         subAttributes = SNode();
      }
      else if (current == lxCode) {
         scope.type = TemplateScope::ttCodeTemplate;

         generateCodeTree(writer, current, scope);
      }

      current = current.nextNode();
   }
}

void Compiler :: generateClassTree(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes, int nested)
{
   SyntaxTree buffer;

   if (!nested) {
      writer.newNode(lxClass);

      generateAttributes(writer, node, scope, attributes/*, false*/);
   }

   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute || current == lxNameAttr) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxBaseParent) {
         if (current.existChild(lxAttributeValue)) {
            ref_t attrRef = scope.mapTemplate(current);

            TemplateScope templateScope(&scope, attrRef);
            copyTemplateTree(writer, current, templateScope, current.firstChild());
         }
         else {
            writer.newNode(lxBaseParent);
            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
         }
      }
      else if (current == lxClassMethod) {
         generateMethodTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }
      else if (current == lxClassField) {
         if (current.argument == INVALID_REF) {
            generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
         }
         else generateFieldTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }
      else if (current == lxFieldTemplate) {
         generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }

      current = current.nextNode();
   }

   if (nested == -1)
      writer.insert(lxNestedClass);

   writer.closeNode();

   SyntaxTree::moveNodes(writer, buffer, lxClass);
}

inline bool isAttribute(_CompilerScope& scope, ref_t subjRef)
{
   return (subjRef != 0 && isPrimitiveRef(scope.subjectHints.get(subjRef)));
}

bool Compiler :: generateMethodScope(SNode node, TemplateScope& scope, SNode attributes)
{
   SNode current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (current != lxNone) {
      // try to resolve the message name
      SNode lastAttr = findLastAttribute(attributes);

      // HOTFIX : recognize generic attribute      
      if (!scope.isAttribute(lastAttr)) {
         bool isVerb = _verbs.exist(lastAttr.findChild(lxIdentifier).findChild(lxTerminal).identifier());

         current = lastAttr.prevNode();
         // HOTFIX : recognize attribute / type
         if (current == lxAttribute && !scope.isAttribute(current) && !scope.isTypeAttribute(current, isVerb)) {
            lastAttr = lxMessage;
            lastAttr = current;
         }

         // mark the last message as a name
         lastAttr = lxNameAttr;
      }
      
      node = lxClassMethod;

      // !! HOTFIX : the node should be once again found
      current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);

      if (current == lxExpression)
         current = lxReturning;

      return true;
   }
   return false;
}

void Compiler :: generateScopeMembers(SNode node, TemplateScope& scope)
{
   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxScope) {
         if (!generateMethodScope(current, scope, subAttributes)) {
            // recognize the field template if available
            SNode fieldTemplate = current.findChild(lxBaseParent);
            if (fieldTemplate != lxNone) {
               current = lxFieldTemplate;
            }
            else if (setIdentifier(subAttributes)) {
               current = lxClassField;
               
               if (current.existChild(lxAttributeValue)) {
                  current.setArgument(-1);
               }
            }
         }
         subAttributes = SNode();
      }

      current = current.nextNode();
   }
}

bool Compiler :: generateSingletonScope(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   SNode expr = node.findChild(lxExpression);
   SNode object = expr.findChild(lxObject);
   SNode closureNode = object.findChild(lxNestedClass);
   if (closureNode != lxNone && isSingleStatement(expr)) {
      generateScopeMembers(closureNode, scope);

      SNode terminal = object.firstChild(lxTerminalMask);

      writer.newNode(lxClass);
      writer.appendNode(lxAttribute, V_SINGLETON);

      if (terminal != lxNone) {
         writer.newNode(lxBaseParent);
         copyIdentifier(writer, terminal);
         writer.closeNode();
      }

      generateAttributes(writer, node, scope, attributes/*, false*/);

      // NOTE : generateClassTree closes the class node and copies auto generated classes after it
      generateClassTree(writer, closureNode, scope, SNode(), -2);

      return true;
   }
   else return false;
}

bool Compiler :: generateDeclaration(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   ModuleScope* moduleScope = scope.moduleScope;

   SyntaxTree tree;
   SyntaxWriter bufferWriter(tree);

   bufferWriter.newNode(lxTemplate);

   scope.type = TemplateScope::ttAttrTemplate;
   generateAttributes(bufferWriter, node, scope, attributes/*, false*/);

   bufferWriter.closeNode();

   // recognize the declaration type

   DeclarationAttr declType = daNone;
   SNode current = tree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (!_logic->validateDeclarationAttribute(current.argument, declType))
            scope.raiseError(errInvalidHint, attributes);
      }

      current = current.nextNode();
   }

   if (declType == daType) {
      declareSubject(writer, goToNode(attributes, lxNameAttr), *moduleScope, *scope.autogeneratedTree);

      return true;
   }
   if (declType == daImport) {
      SNode name = goToNode(attributes, lxNameAttr);

      compileIncludeModule(name, *scope.moduleScope);

      return true;
   }
   else if (test(declType, daTemplate)) {
      node = lxTemplate;

      SNode name = goToNode(attributes, lxNameAttr);

      int count = SyntaxTree::countChild(node, lxBaseParent);

      IdentifierString templateName(name.findChild(lxIdentifier).findChild(lxTerminal).identifier());
      if (test(declType, daField)) {
         templateName.append("#");

         scope.type = TemplateScope::Type::ttFieldTemplate;
         count++; // HOTFIX : to include the field itself
      }
      else if (test(declType, daMethod)) {
         templateName.append("#");

         scope.type = TemplateScope::ttMethodTemplate;
      }
      else if (node.existChild(lxExpression)) {
         scope.type = TemplateScope::ttCodeTemplate;

         // HOTFIX : mark the expression as a code
         SNode exprNode = node.findChild(lxExpression);
         exprNode.injectNode(lxExpression);
         if (test(declType, daLoop)) {
            exprNode.injectNode(lxLoop);
         }
         else if (test(declType, daExtern)) {
            exprNode.injectNode(lxExtern);
         }

         exprNode = lxCode;
      }

      templateName.append('#');
      templateName.appendInt(count);

      ref_t templateRef = moduleScope->mapNewSubject(templateName);

      // check for duplicate declaration
      if (moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, true))
         scope.raiseError(errDuplicatedSymbol, name);

      saveTemplate(moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, false), 
         node, *moduleScope, attributes, scope.type/*, autogenerated*/);

      moduleScope->saveSubject(templateRef, INVALID_REF, false);

      return true;
   }
   else if (test(declType, daClass)) {
      return false;
   }
   else return false;
}

void Compiler :: generateScope(SyntaxWriter& writer, SNode node, TemplateScope& scope, SNode attributes)
{
   SNode body = node.findChild(lxExpression);
   if (body == lxExpression) {
      // if it could be compiled as a symbol
      if (setIdentifier(attributes)) {
         attributes.refresh();

         // check if it is a code template
         if (!generateDeclaration(writer, node, scope, attributes)) {
            // check if it could be compiled as a singleton
            if (!generateSingletonScope(writer, node, scope, attributes)) {
               node = lxSymbol;
               attributes.refresh();

               generateSymbolTree(writer, node, scope, attributes);
            }
         }
      }
   }
   else {
      // it is is a template
      if (node == lxTemplate) {
         generateScopeMembers(node, scope);

         generateTemplateTree(writer, node, scope, attributes);
      }
      else {
         setIdentifier(attributes);
         // try to recognize general declaration
         if (!generateDeclaration(writer, node, scope, attributes)) {
            // otherwise it will be compiled as a class
            node = lxClass;
            attributes.refresh();

            generateScopeMembers(node, scope);

            generateClassTree(writer, node, scope, attributes);
         }
      }
   }
}

void Compiler :: saveTemplate(_Memory* target, SNode node, ModuleScope& scope, SNode attributes, TemplateScope::Type type/*, SyntaxTree& autogenerated*/)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxTemplate);

   // HOTFIX : save the template source path
   IdentifierString fullPath(scope.module->Name());
   fullPath.append('\'');
   fullPath.append(scope.sourcePath);

   TemplateScope rootScope(&scope);
   //rootScope.autogeneratedTree = &autogenerated;
   rootScope.loadParameters(node);
   rootScope.sourcePath = fullPath;
   rootScope.type = type;

   generateScope(writer, node, rootScope, attributes);

   writer.closeNode();

   SyntaxTree::saveNode(tree.readRoot(), target);
}

void Compiler :: generateNewAttribute(SNode node, ModuleScope& scope, SNode attributes)
{
   SNode name = node.findChild(lxIdentifier);

   ref_t templateRef = scope.mapNewSubject(name.findChild(lxTerminal).identifier());

   //// check for duplicate declaration
   //if (scope.module->mapSection(templateRef | mskSyntaxTreeRef, true))
   //   scope.raiseError(errDuplicatedSymbol, name);

   SyntaxTree tree;
   SyntaxWriter writer(tree);
   
   writer.newNode(lxTemplate);
   
   // HOTFIX : save the template source path
   IdentifierString fullPath(scope.module->Name());
   fullPath.append('\'');
   fullPath.append(scope.sourcePath);
   
   TemplateScope rootScope(&scope);
   //   rootScope.autogeneratedTree = &autogenerated;
   //   rootScope.loadParameters(node);
   rootScope.sourcePath = fullPath;
   
   generateScope(writer, node, rootScope, attributes);
   
   writer.closeNode();   
   
   SNode attr = tree.readRoot().findChild(lxAttribute);
   if (SyntaxTree::countChild(tree.readRoot(), lxAttribute) != 1 || !_logic->isPrimitiveRef(attr.argument)) {
      SyntaxTree::saveNode(tree.readRoot(), scope.module->mapSection(templateRef | mskSyntaxTreeRef, false));

      //saveTemplate(scope.module->mapSection(templateRef | mskSyntaxTreeRef, false), node, scope, attributes/*, autogenerated*/);

      scope.saveSubject(templateRef, INVALID_REF, false);
   }
   // if there are only one attribute it could be saved directly
   else scope.saveSubject(templateRef, attr.argument, false);
}

void Compiler :: generateSyntaxTree(SyntaxWriter& writer, SNode node, ModuleScope& scope, SyntaxTree& autogenerated)
{
   SNode attributes;
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxAttribute:
         case lxAttributeDecl:
            if (attributes == lxNone) {
               attributes = current;
            }
            break;
         case lxScope:
         {
            TemplateScope rootScope(&scope);
//            rootScope.templateRef = INVALID_REF;
            rootScope.autogeneratedTree = &autogenerated;
            generateScope(writer, current, rootScope, attributes);
            attributes = SNode();
            break;
         }
         case lxTemplate:
            generateNewAttribute(current, scope, attributes);
            attributes = SNode();
            break;
      }
      current = current.nextNode();
   }
}

void Compiler :: compileModule(_ProjectManager& project, ident_t file, SyntaxTree& derivationTree, ModuleInfo& info, Unresolveds& unresolveds)
{
   ModuleScope scope(&project, file, info.codeModule, info.debugModule, &unresolveds);

   project.printInfo("%s", file);

   // prepare syntax tree
   SyntaxTree autogeneratedTree;
   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot);
   generateSyntaxTree(writer, derivationTree.readRoot(), scope, autogeneratedTree);

   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);

   writer.closeNode();

   // compile syntax tree
   compileSyntaxTree(syntaxTree, scope);
}

ModuleInfo Compiler :: createModule(ident_t name, _ProjectManager& project, bool withDebugInfo)
{
   ModuleInfo info;
   info.codeModule = project.createModule(name);
   if (withDebugInfo)
      info.debugModule = project.createDebugModule(name);

   createPackageInfo(info.codeModule, project);

   return info;
}

void Compiler :: generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef)
{
   MemoryWriter metaWriter(scope.module->mapSection(enumRef | mskConstArray, false));

   metaWriter.writeDWord(memberRef | mskConstantRef);
}

ref_t Compiler :: readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader)
{
   ref_t memberRef = reader.getDWord() & ~mskAnyRef;

   return importReference(extModule, memberRef, scope.module);
}

void Compiler :: injectBoxing(SyntaxWriter& writer, _CompilerScope&, LexicalType boxingType, int argument, ref_t targetClassRef)
{
   writer.appendNode(lxBoxableAttr);
   writer.appendNode(lxTarget, targetClassRef);
   writer.insert(boxingType, argument);
   writer.closeNode();
}

void Compiler :: injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, bool stacksafe)
{
   writer.insertChildren(0, createOp, createArg, lxTarget, targetClassRef);

   writer.appendNode(lxCallTarget, targetClassRef);
   writer.appendNode(lxBoxableAttr);
   if (stacksafe)
      writer.appendNode(lxStacksafeAttr);

   writer.insert(convertOp, convertArg);
   writer.closeNode();
}

void Compiler :: injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject)
{
   // removing assinging operation
   assignNode = lxExpression;

   // move assigning target into the call node
   SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
   if (assignTarget != lxNone) {
      callNode.appendNode(assignTarget.type, assignTarget.argument);
      assignTarget = lxIdle;
      callNode.setArgument(encodeMessage(subject, EVAL_MESSAGE_ID, 1));
   }
}

void Compiler :: injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb)
{
   SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));

   if (paramCount == -1 && verb == 0) {
      assignNode.set(callNode.type, subject);
      callNode = lxIdle;

      SNode targetNode = assignTarget.findChild(lxTarget);
      assignNode.appendNode(lxCallTarget, targetNode.argument);
   }
   else {
      // removing assinging operation
      assignNode = lxExpression;

      // move assigning target into the call node
      
      if (assignTarget != lxNone) {
         callNode.appendNode(assignTarget.type, assignTarget.argument);
         assignTarget = lxIdle;
         callNode.setArgument(encodeMessage(subject, verb, paramCount));
      }
   }
}

void Compiler :: injectLocalBoxing(SNode node, int size)
{
   //HOTFIX : using size variable copy to prevent aligning
   int dummy = size;
   int offset = allocateStructure(node, dummy);

   // allocate place for the local copy
   node.injectNode(node.type, node.argument);

   node.set(lxAssigning, size);

   node.insertNode(lxLocalAddress, offset);
   node.insertNode(lxTempAttr, 0);
}

void Compiler :: injectFieldExpression(SyntaxWriter& writer)
{
   writer.appendNode(lxResultField);

   writer.insert(lxFieldExpression);
   writer.closeNode();
}

void Compiler :: injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t embeddedMessageRef)
{
   SNode methNode = classNode.appendNode(lxConstructor, message);
   methNode.appendNode(lxAttribute, tpEmbeddable);
   methNode.appendNode(lxEmbeddableMssg, embeddedMessageRef);
   methNode.appendNode(lxAttribute, tpConstructor);
   SNode codeNode = methNode.appendNode(lxCode);
   SNode exprNode = codeNode.appendNode(lxExpression);
   exprNode.appendNode(lxIdentifier, THIS_VAR);
   exprNode.appendNode(lxMessage, embeddedMessageRef);
}