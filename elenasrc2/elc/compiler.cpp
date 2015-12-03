//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiler.h"
#include "errors.h"
#include <errno.h>

using namespace _ELENA_;

// --- Hint constants ---
#define HINT_MASK             0xFFFF0000

#define HINT_ROOT             0x80000000
#define HINT_NOBOXING         0x40000000
#define HINT_ALT              0x12000000
#define HINT_ACTION           0x00020000
#define HINT_ALTBOXING        0x00010000

typedef Compiler::ObjectInfo ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef Compiler::ObjectKind ObjectKind;
typedef ClassInfo::Attribute Attribute;

// --- Auxiliary routines ---

inline bool isCollection(DNode node)
{
   return (node == nsExpression && node.nextNode()==nsExpression);
}

inline bool isReturnExpression(DNode expr)
{
   return (expr == nsExpression && expr.nextNode() == nsNone);
}

inline bool isSingleStatement(DNode expr)
{
   return (expr == nsExpression) && (expr.firstChild().nextNode() == nsNone);
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
   if (exportRef) {
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

// skip the hints and return the first hint node or none
inline DNode skipHints(DNode& node)
{
   DNode hints;
   if (node == nsHint)
      hints = node;

   while (node == nsHint)
      node = node.nextNode();

   return hints;
}

inline bool findSymbol(DNode node, Symbol symbol)
{
   while (node != nsNone) {
      if (node==symbol)
         return true;

      node = node.nextNode();
   }
   return false;
}

inline DNode goToSymbol(DNode node, Symbol symbol)
{
   while (node != nsNone) {
      if (node==symbol)
         return node;

      node = node.nextNode();
   }
   return node;
}

inline bool isImportRedirect(DNode node)
{
   if (node.firstChild() == nsNone) {
      TerminalInfo terminal = node.Terminal();
      if (terminal.symbol == tsReference) {
         if (StringHelper::compare(terminal.value, INTERNAL_MODULE, strlen(INTERNAL_MODULE)) && terminal.value[strlen(INTERNAL_MODULE)]=='\'')
            return true;
      }
   }
   return false;
}

inline bool IsVarOperator(int operator_id)
{
   switch (operator_id) {
   case WRITE_MESSAGE_ID:
   case APPEND_MESSAGE_ID:
   case REDUCE_MESSAGE_ID:
   case INCREASE_MESSAGE_ID:
   case SEPARATE_MESSAGE_ID:
      return true;
   default:
      return false;
   }
}

inline bool IsCompOperator(int operator_id)
{
   switch(operator_id) {
      case EQUAL_MESSAGE_ID:
      case NOTEQUAL_MESSAGE_ID:
      case LESS_MESSAGE_ID:
      case NOTLESS_MESSAGE_ID:
      case GREATER_MESSAGE_ID:
      case NOTGREATER_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsReferOperator(int operator_id)
{
   return operator_id == REFER_MESSAGE_ID || operator_id == SET_REFER_MESSAGE_ID;
}

inline bool IsDoubleOperator(int operator_id)
{
   return operator_id == SET_REFER_MESSAGE_ID;
}

inline bool IsInvertedOperator(int& operator_id)
{
   if (operator_id == NOTEQUAL_MESSAGE_ID) {
      operator_id = EQUAL_MESSAGE_ID;

      return true;
   }
   else if (operator_id == NOTLESS_MESSAGE_ID) {
      operator_id = LESS_MESSAGE_ID;

      return true;
   }
   else if (operator_id == NOTGREATER_MESSAGE_ID) {
      operator_id = GREATER_MESSAGE_ID;

      return true;
   }
   else return false;
}

void appendCoordinate(SyntaxWriter* writer, TerminalInfo terminal)
{
   writer->appendNode(lxCol, terminal.Col());
   writer->appendNode(lxRow, terminal.Row());
}

// --- Compiler::ModuleScope ---

Compiler::ModuleScope::ModuleScope(Project* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints((ref_t)-1), extensions(NULL, freeobj)
{
   this->project = project;
   this->sourcePath = sourcePath;
   this->module = module;
   this->debugModule = debugModule;

   this->forwardsUnresolved = forwardsUnresolved;
   this->sourcePathRef = 0;

   warnOnUnresolved = project->BoolSetting(opWarnOnUnresolved);
   warnOnWeakUnresolved = project->BoolSetting(opWarnOnWeakUnresolved);

   // cache the frequently used references
   superReference = mapReference(project->resolveForward(SUPER_FORWARD));
   intReference = mapReference(project->resolveForward(INT_FORWARD));
   longReference = mapReference(project->resolveForward(LONG_FORWARD));
   realReference = mapReference(project->resolveForward(REAL_FORWARD));
   literalReference = mapReference(project->resolveForward(WSTR_FORWARD));
   charReference = mapReference(project->resolveForward(WCHAR_FORWARD));
   signatureReference = mapReference(project->resolveForward(SIGNATURE_FORWARD));
   paramsReference = mapReference(project->resolveForward(PARAMS_FORWARD));
   trueReference = mapReference(project->resolveForward(TRUE_FORWARD));
   falseReference = mapReference(project->resolveForward(FALSE_FORWARD));
   arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));

   boolType = module->mapSubject(project->resolveForward(BOOLTYPE_FORWARD), false);

   defaultNs.add(module->Name());

   loadTypes(module);
   loadExtensions(TerminalInfo(), module);
}

ref_t Compiler::ModuleScope :: getBaseFunctionClass(int paramCount)
{
   if (paramCount == 0) {
      return mapReference(project->resolveForward(FUNCX_FORWARD));
   }
   else {
      IdentifierString className(project->resolveForward(FUNCX_FORWARD));
      className.appendInt(paramCount);

      return mapReference(className);
   }
}

ref_t Compiler::ModuleScope :: getBaseIndexFunctionClass(int paramCount)
{
   if (paramCount > 0) {
      IdentifierString className(project->resolveForward(NFUNCX_FORWARD));
      className.appendInt(paramCount);

      return mapReference(className);
   }
   else return 0;
}

ref_t Compiler::ModuleScope :: getBaseLazyExpressionClass()
{
   return mapReference(project->resolveForward(LAZYEXPR_FORWARD));
}

ObjectInfo Compiler::ModuleScope :: mapObject(TerminalInfo identifier)
{
   if (identifier==tsReference) {
      return mapReferenceInfo(identifier, false);
   }
   else if (identifier==tsPrivate) {
      if (StringHelper::compare(identifier.value, NIL_VAR)) {
         return ObjectInfo(okNil);
      }
      else return defineObjectInfo(mapTerminal(identifier, true), true);
   }
   else if (identifier==tsIdentifier) {
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

ref_t Compiler::ModuleScope :: mapNewType(ident_t terminal)
{
   IdentifierString fullName(terminal);
   fullName.append('$');

   ident_t ns = module->Name();
   if (StringHelper::compare(ns, STANDARD_MODULE)) {
   }
   else if (StringHelper::compare(ns, STANDARD_MODULE, STANDARD_MODULE_LEN)) {
      fullName.append(ns + STANDARD_MODULE_LEN + 1);
   }
   else fullName.append(ns);

   return module->mapSubject(fullName, false);
}

ref_t Compiler::ModuleScope :: mapType(TerminalInfo terminal)
{
   ident_t identifier = NULL;
   if (terminal.symbol == tsIdentifier || terminal.symbol == tsPrivate) {
      identifier = terminal.value;
   }
   else raiseError(errInvalidSubject, terminal);

   ref_t subj_ref = types.get(identifier);
   if (subj_ref != 0)
      return subj_ref;

   IdentifierString fullName(identifier);
   fullName.append('$');

   size_t tail = fullName.Length();
   List<ident_t>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      fullName.truncate(tail);

      // if it is a sytem root
      if (StringHelper::compare(*it, STANDARD_MODULE)) {
      }
      else if (StringHelper::compare(*it, STANDARD_MODULE, STANDARD_MODULE_LEN)) {
         fullName.append(*it + STANDARD_MODULE_LEN + 1);
      }
      else fullName.append(*it);

      subj_ref = module->mapSubject(fullName, true);
      if (subj_ref && typeHints.exist(subj_ref)) {
         types.add(terminal, subj_ref);

         return subj_ref;
      }
      it++;
   }

   return 0;
}

ref_t Compiler::ModuleScope :: mapSubject(TerminalInfo terminal, IdentifierString& output, bool strongOnly)
{
   // add a namespace for the private message
   if (terminal.symbol == tsPrivate) {
      output.append(project->StrSetting(opNamespace));
      output.append(terminal.value);

      return 0;
   }

   ref_t typeRef = mapType(terminal);
   if (typeRef != 0) {
      output.append(module->resolveSubject(typeRef));
   }
   else if (terminal.symbol != tsReference){
      typeRef = module->mapSubject(terminal, false);

      output.append(terminal.value);
   }
   else raiseError(errInvalidSubject, terminal);

   if (strongOnly) {
      if (typeHints.exist(typeRef)) {
         return typeRef;
      }
      else return 0;
   }
   else return typeRef;
}

ref_t Compiler::ModuleScope :: mapTerminal(TerminalInfo terminal, bool existing)
{
   if (terminal == tsIdentifier) {
      ref_t reference = forwards.get(terminal);
      if (reference == 0) {
         if (!existing) {
            ReferenceNs name(module->Name(), terminal);

            return module->mapReference(name);
         }
         else return resolveIdentifier(terminal);
      }
      else return reference;
   }
   else if (terminal == tsPrivate) {
      ReferenceNs name(module->Name(), terminal);

      return mapReference(name, existing);
   }
   else return mapReference(terminal, existing);
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
         else if (test(info.header.flags, elStandartVMT) && info.classClassRef != 0) {
            return ObjectInfo(okConstantClass, reference, info.classClassRef);
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
               return ObjectInfo(okConstantSymbol, reference, typeHints.get(symbolInfo.expressionTypeRef), symbolInfo.expressionTypeRef);
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
   if (StringHelper::compare(reference, EXTERNAL_MODULE, strlen(EXTERNAL_MODULE)) && reference[strlen(EXTERNAL_MODULE)]=='\'') {
      return ObjectInfo(okExternal);
   }
   else if (StringHelper::compare(reference, INTERNAL_MODULE, strlen(INTERNAL_MODULE)) && reference[strlen(INTERNAL_MODULE)] == '\'') {
      ReferenceNs fullName(project->resolveForward(IMPORT_FORWARD), reference + strlen(INTERNAL_MODULE) + 1);

      return ObjectInfo(okInternal, module->mapReference(fullName));
   }
   else {
      ref_t referenceID = mapReference(reference, existing);

      return defineObjectInfo(referenceID);
   }
}

void Compiler::ModuleScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly)
{
   target.header = copy.header;
   target.classClassRef = copy.classClassRef;
   target.extensionTypeRef = copy.extensionTypeRef;
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
         target.fieldTypes.add(type_it.key(), importSubject(exporter, *type_it, module));

         type_it++;
      }

      // import method types
      ClassInfo::MethodInfoMap::Iterator mtype_it = copy.methodHints.start();
      while (!mtype_it.Eof()) {
         Attribute key = mtype_it.key();
         ref_t value = *mtype_it;
         if (test(key.value2, maTypeMask))
            value = importSubject(exporter, value, module);

         target.methodHints.add(
            Attribute(importMessage(exporter, key.value1, module), key.value2),
            value);

         mtype_it++;
      }
   }
   // import class class reference
   if (target.classClassRef != 0)
      target.classClassRef = importReference(exporter, target.classClassRef, module);


   // import parent reference
   target.header.parentRef = importReference(exporter, target.header.parentRef, module);
}

ref_t Compiler::ModuleScope :: loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly)
{
   _Module* argModule;

   if (emptystr(vmtName))
      return 0;

   // load class meta data
   ref_t moduleRef = 0;
   argModule = project->resolveModule(vmtName, moduleRef);

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
   _Module* argModule = project->resolveModule(symbol, moduleRef);

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
   }
   return moduleRef;
}

int Compiler::ModuleScope::defineStructSize(ref_t classReference, bool& variable)
{
   ClassInfo classInfo;
   if(loadClassInfo(classInfo, module->resolveReference(classReference), true) == 0)
      return 0;

   if (test(classInfo.header.flags, elStructureRole) && test(classInfo.header.flags, elEmbeddable)) {
      variable = !test(classInfo.header.flags, elReadOnlyRole);

      return classInfo.size;
   }

   return 0;
}

int Compiler::ModuleScope :: defineTypeSize(ref_t type_ref, ref_t& classReference, bool& variable)
{
   if (type_ref == 0)
      return 0;

   classReference = typeHints.get(type_ref);
   if (classReference != 0) {
      return defineStructSize(classReference, variable);
   }
   else return 0;
}

int Compiler::ModuleScope :: getClassFlags(ref_t reference)
{
   if (reference == 0)
      return 0;

   ClassInfo classInfo;
   if(loadClassInfo(classInfo, module->resolveReference(reference), true) == 0)
      return 0;

   return classInfo.header.flags;
}

int Compiler::ModuleScope :: checkMethod(ref_t reference, ref_t message, bool& found, ref_t& outputType)
{
   ClassInfo info;
   found = loadClassInfo(info, module->resolveReference(reference)) != 0;

   if (found) {
      bool methodFound = info.methods.exist(message);

      if (methodFound) {
         int hint = info.methodHints.get(Attribute(message, maHint));
         outputType = info.methodHints.get(Attribute(message, maType));

         if ((hint & tpMask) == tpSealed) {
            return hint;
         }
         else if (test(info.header.flags, elSealed)) {
            return tpSealed | hint;
         }
         else if (test(info.header.flags, elClosed)) {
            return tpClosed | hint;
         }
         else return tpNormal | hint;
      }
   }

   return tpUnknown;
}

void Compiler::ModuleScope :: validateReference(TerminalInfo terminal, ref_t reference)
{
   // check if the reference may be resolved
   bool found = false;

   if (warnOnUnresolved && (warnOnWeakUnresolved || !isWeakReference(terminal))) {
      int   mask = reference & mskAnyRef;
      reference &= ~mskAnyRef;

      ref_t    ref = 0;
      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);

      if (refModule != NULL) {
         found = (refModule->mapSection(ref | mask, true)!=NULL);
      }
      if (!found) {
         if (!refModule || refModule == module) {
            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module, terminal.Row(), terminal.Col()));
         }
         else raiseWarning(wrnUnresovableLink, terminal);
      }
   }
}

void Compiler::ModuleScope :: raiseError(const char* message, TerminalInfo terminal)
{
   project->raiseError(message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: raiseWarning(const char* message, TerminalInfo terminal)
{
   project->raiseWarning(message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: raiseWarning(const char* message, int row, int col)
{
   project->raiseWarning(message, sourcePath, row, col, DEFAULT_STR);
}

void Compiler::ModuleScope :: loadTypes(_Module* extModule)
{
   if (extModule) {
      ReferenceNs sectionName(extModule->Name(), TYPE_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t subj_ref = importSubject(extModule, metaReader.getDWord(), module);

            ref_t class_ref = importReference(extModule, metaReader.getDWord(), module);

            typeHints.add(subj_ref, class_ref);
         }
      }
   }
}

void Compiler::ModuleScope :: loadExtensions(TerminalInfo terminal, _Module* extModule)
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
            else raiseWarning(wrnDuplicateExtension, terminal);
         }
      }
   }
}

void Compiler::ModuleScope :: saveType(ref_t type_ref, ref_t classReference, bool internalType)
{
   if (!internalType) {
      ReferenceNs sectionName(module->Name(), TYPE_SECTION);

      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(type_ref);
      metaWriter.writeDWord(classReference);
   }

   typeHints.add(type_ref, classReference, true);
}

bool Compiler::ModuleScope :: saveExtension(ref_t message, ref_t type, ref_t role)
{
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

bool Compiler::ModuleScope :: checkIfCompatible(ref_t typeRef, ref_t classRef)
{
   ClassInfo sourceInfo;

   if (typeHints.exist(typeRef, classRef))
      return true;

   // if source class inherites / is target class
   while (classRef != 0) {
      if (loadClassInfo(sourceInfo, module->resolveReference(classRef), true) == 0)
         break;

      if (typeHints.exist(typeRef, classRef))
         return true;

      classRef = sourceInfo.header.parentRef;
   }

   return false;
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
   typeRef = 0;
   constant = false;
}

void Compiler::SymbolScope :: compileHints(DNode hints)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (StringHelper::compare(terminal, HINT_CONSTANT)) {
         constant = true;
      }
      else if (StringHelper::compare(terminal, HINT_TYPE)) {
         DNode value = hints.select(nsHintValue);
         TerminalInfo typeTerminal = value.Terminal();

         typeRef = moduleScope->mapType(typeTerminal);
         if (typeRef == 0)
            raiseError(wrnInvalidHint, terminal);
      }
      else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

ObjectInfo Compiler::SymbolScope :: mapObject(TerminalInfo identifier)
{
   return Scope::mapObject(identifier);
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(ModuleScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   info.header.parentRef =   moduleScope->superReference;
   info.header.flags = elStandartVMT;
   info.header.count = 0;
   info.size = 0;
   info.classClassRef = 0;
   info.extensionTypeRef = 0;
}

ObjectInfo Compiler::ClassScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, SUPER_VAR)) {
      return ObjectInfo(okSuper, info.header.parentRef);
   }
   else if (StringHelper::compare(identifier, SELF_VAR)) {
      return ObjectInfo(okParam, (size_t)-1);
   }
   else {
      int reference = info.fields.get(identifier);
      if (reference != -1) {
         if (test(info.header.flags, elStructureRole)) {
            int offset = reference;

            return ObjectInfo(okFieldAddress, offset, 0, info.fieldTypes.get(offset));
         }
         // otherwise it is a normal field
         else return ObjectInfo(okField, reference, 0, info.fieldTypes.get(reference));
      }
      else return Scope::mapObject(identifier);
   }
}

void Compiler::ClassScope :: compileClassHints(DNode hints)
{
   // define class flags
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (StringHelper::compare(terminal, HINT_GROUP)) {
         info.header.flags |= elGroup;
      }
      else if (StringHelper::compare(terminal, HINT_MESSAGE)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.size = 4;
         info.header.flags |= elDebugDWORD;

         info.header.flags |= (elStructureRole | elMessage | elEmbeddable);
      }
      else if (StringHelper::compare(terminal, HINT_SYMBOL)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.size = 4;
         info.header.flags |= elDebugReference;

         info.header.flags |= (elStructureRole | elSymbol);
      }
      else if (StringHelper::compare(terminal, HINT_SIGNATURE)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.size = 4;
         info.header.flags |= elDebugSubject;

         info.header.flags |= (elStructureRole | elSignature | elEmbeddable);
      }
      else if (StringHelper::compare(terminal, HINT_EXTENSION)) {
         info.header.flags |= elExtension;
         info.header.flags |= elSealed;      // extension should be sealed
         DNode value = hints.select(nsHintValue);
         if (value != nsNone) {
            info.extensionTypeRef = moduleScope->mapType(value.Terminal());
            if (info.extensionTypeRef == 0)
               raiseError(errUnknownSubject, value.Terminal());
         }
      }
      else if (StringHelper::compare(terminal, HINT_SEALED)) {
         info.header.flags |= elSealed;
      }
      else if (StringHelper::compare(terminal, HINT_CONSTANT)) {
         info.header.flags |= elReadOnlyRole;
      }
      else if (StringHelper::compare(terminal, HINT_LIMITED)) {
         info.header.flags |= elClosed;
      }
      else if (StringHelper::compare(terminal, HINT_STRUCT)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole | elWrapper))
            raiseError(wrnInvalidHint, terminal);

         info.header.flags |= elStructureRole;

         DNode option = hints.select(nsHintValue);
         if (option != nsNone) {
            if (StringHelper::compare(option.Terminal(), HINT_EMBEDDABLE)) {
               info.header.flags |= elEmbeddable;
            }
            else raiseError(wrnInvalidHint, terminal);
         }
      }
      else if (StringHelper::compare(terminal, HINT_INTEGER_NUMBER)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
         if (sizeValue.symbol == tsInteger) {
            info.size = StringHelper::strToInt(sizeValue.value);
            // !! HOTFIX : allow only 1,2,4 or 8
            if (info.size == 1 || info.size == 2 || info.size == 4) {
               info.header.flags |= elDebugDWORD;
            }
            else if (info.size == 8) {
               info.header.flags |= elDebugQWORD;
            }
            else raiseError(wrnInvalidHint, terminal);

            info.header.flags |= elReadOnlyRole;
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

         info.header.flags |= (elEmbeddable | elStructureRole);
      }
      else if (StringHelper::compare(terminal, HINT_FLOAT_NUMBER)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
         if (sizeValue.symbol == tsInteger) {
            info.size = StringHelper::strToInt(sizeValue.value);
            // !! HOTFIX : allow only 8
            if (info.size == 8) {
               info.header.flags |= elDebugReal64;
            }
            else raiseError(wrnInvalidHint, terminal);

            info.header.flags |= elReadOnlyRole;
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

         info.header.flags |= (elEmbeddable | elStructureRole);
      }
      else if (StringHelper::compare(terminal, HINT_POINTER)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.size = 4;
         info.header.flags |= elDebugPTR;
         info.header.flags |= (elEmbeddable | elStructureRole);
      }
      else if (StringHelper::compare(terminal, HINT_BINARY)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole | elWrapper))
            raiseError(wrnInvalidHint, terminal);

         TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
         if (sizeValue.symbol == tsInteger) {
            info.size = -StringHelper::strToInt(sizeValue.value);
            if (info.size == -1) {
               info.header.flags |= elDebugBytes;
            }
            else if (info.size == -2) {
               info.header.flags |= elDebugShorts;
            }
            else if (info.size == -4) {
               info.header.flags |= elDebugIntegers;
            }
            else raiseError(wrnInvalidHint, terminal);
         }
         else if (sizeValue.symbol == tsIdentifier) {
            DNode value = hints.select(nsHintValue);
            size_t type = moduleScope->mapType(value.Terminal());
            if (type == 0)
               raiseError(errUnknownSubject, value.Terminal());

            info.fieldTypes.add(-1, type);
            info.size = -moduleScope->defineTypeSize(type);
            if (info.size == -4) {
               info.header.flags |= elDebugIntegers;
            }
            else if ((int)info.size > 0)
               raiseError(wrnInvalidHint, value.Terminal());
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

         info.header.flags |= (elEmbeddable | elStructureRole | elDynamicRole);
      }
      else if (StringHelper::compare(terminal, HINT_XDYNAMIC) || StringHelper::compare(terminal, HINT_DYNAMIC)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole | elWrapper))
            raiseError(wrnInvalidHint, terminal);

         info.header.flags |= elDynamicRole;
         info.header.flags |= elDebugArray;
         if (StringHelper::compare(terminal, HINT_DYNAMIC)) {
            DNode value = hints.select(nsHintValue);
            if (value != nsNone) {
               size_t type = moduleScope->mapType(value.Terminal());
               if (type == 0)
                  raiseError(errUnknownSubject, value.Terminal());

               info.fieldTypes.add(-1, type);
            }
         }
      }
      else if (StringHelper::compare(terminal, HINT_NONSTRUCTURE)) {
         info.header.flags |= elNonStructureRole;
      }
      else if (StringHelper::compare(terminal, HINT_STRING)) {
         info.header.flags |= elDebugLiteral;
         info.header.flags |= elStructureRole;
      }
      else if (StringHelper::compare(terminal, HINT_WIDESTRING)) {
         info.header.flags |= elDebugWideLiteral;
         info.header.flags |= elStructureRole;
      }
      else if (StringHelper::compare(terminal, HINT_VARIABLE)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.header.flags |= elWrapper;
      }
      else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }

}

void Compiler::ClassScope :: compileFieldHints(DNode hints, int& size, ref_t& type)
{
   type = 0;

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (StringHelper::compare(terminal, HINT_TYPE)) {
         DNode value = hints.select(nsHintValue);
         if (value!=nsNone && type == 0) {
            TerminalInfo typeTerminal = value.Terminal();

            type = moduleScope->mapType(typeTerminal);
            if (type == 0)
               raiseError(errInvalidHint, terminal);

            size = moduleScope->defineTypeSize(type);
         }
         else raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
      }
      else if (StringHelper::compare(terminal, HINT_SIZE)) {
         if (size < 0) {
            TerminalInfo sizeValue = hints.firstChild().Terminal();
            if (size < 0 && sizeValue.symbol == tsInteger) {
               size = -size;

               size = StringHelper::strToInt(sizeValue.value) * size;
            }
            else if (size < 0 && sizeValue.symbol == tsHexInteger) {
               size = -size;

               size = StringHelper::strToLong(sizeValue.value, 16) * size;
            }
            else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      }
      else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved = 0;
   this->rootToFree = 1;
   this->withOpenArg = false;
   this->stackSafe = false;

   //NOTE : tape has to be overridden in the constructor
   this->tape = &parent->tape;
}

bool Compiler::MethodScope :: include()
{
   ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   ClassInfo::MethodMap::Iterator it = classScope->info.methods.getIt(message);
   if (it.Eof()) {
      classScope->info.methods.add(message, true);

      return true;
   }
   else {
      (*it) = true;

      return false;
   }
}

ObjectInfo Compiler::MethodScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, THIS_VAR)) {
      return ObjectInfo(okThisParam, 1, stackSafe ? -1 : 0);
   }
   else if (StringHelper::compare(identifier, SELF_VAR)) {
      ObjectInfo retVal = parent->mapObject(identifier);
      retVal.extraparam = stackSafe ? -1 : 0;

      return retVal;
   }
   else {
      Parameter param = parameters.get(identifier);

      int local = param.offset;
      if (local >= 0) {
         if (withOpenArg && moduleScope->typeHints.exist(param.sign_ref, moduleScope->paramsReference)) {
            return ObjectInfo(okParams, -1 - local, 0, param.sign_ref);
         }
         else return ObjectInfo(okParam, -1 - local, stackSafe ? -1 : 0, param.sign_ref);
      }
      else {
         ObjectInfo retVal = Scope::mapObject(identifier);

         return retVal;
      }
   }
}

void Compiler::MethodScope ::compileWarningHints(DNode hints)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      if (StringHelper::compare(terminal, HINT_SUPPRESS_WARNINGS)) {
         DNode value = hints.select(nsHintValue);
         TerminalInfo level = value.Terminal();
         if (StringHelper::compare(level, "w2")) {
            warningMask = WARNING_MASK_1;
         }
         else if (StringHelper::compare(level, "w3")) {
            warningMask = WARNING_MASK_2;
         }
         else raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
      }

      hints = hints.nextNode();
   }
}

void Compiler::MethodScope :: compileHints(DNode hints)
{
   ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);

   ref_t outputType = 0;
   bool hintChanged = false;
   int hint = classScope->info.methodHints.get(Attribute(message, maHint));

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      if (StringHelper::compare(terminal, HINT_GENERIC)) {
         setClassFlag(elWithGenerics);

         hint |= tpGeneric;
         hintChanged = true;
      }
      else if (StringHelper::compare(terminal, HINT_TYPE)) {
         DNode value = hints.select(nsHintValue);
         TerminalInfo typeTerminal = value.Terminal();

         outputType = moduleScope->mapType(typeTerminal);
         if (outputType == 0)
            raiseError(wrnInvalidHint, terminal);
      }
      else if (StringHelper::compare(terminal, HINT_STACKSAFE)) {
         hint |= tpStackSafe;
         hintChanged = true;
      }
      else if (StringHelper::compare(terminal, HINT_EMBEDDABLE)) {
         hint |= tpEmbeddable;
         hint |= tpSealed;
         hintChanged = true;
      }
      else if (StringHelper::compare(terminal, HINT_SEALED)) {
         hint |= tpSealed;
         hintChanged = true;
      }
      else if (StringHelper::compare(terminal, HINT_SUPPRESS_WARNINGS)) {
         // HOTFIX : ignore for the first pass
         // should be recognized on the second pass
      }
      else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }

   if (outputType != 0) {
      classScope->info.methodHints.exclude(Attribute(message, maType));
      classScope->info.methodHints.add(Attribute(message, maType), outputType);
   }

   if (hintChanged) {
      classScope->info.methodHints.exclude(Attribute(message, maHint));
      classScope->info.methodHints.add(Attribute(message, maHint), hint);
   }
}

// --- Compiler::ActionScope ---

Compiler::ActionScope :: ActionScope(ClassScope* parent)
   : MethodScope(parent)
{
}

ObjectInfo Compiler::ActionScope :: mapObject(TerminalInfo identifier)
{
   // action does not support this variable
   if (StringHelper::compare(identifier, THIS_VAR)) {
      return parent->mapObject(identifier);
   }
   else return MethodScope::mapObject(identifier);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SymbolScope* parent, SyntaxWriter* writer)
   : Scope(parent), locals(Parameter(0))
{
   this->writer = writer;
   this->level = 0;
   this->saved = this->reserved = 0;
   this->rootBookmark = -1;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent, SyntaxWriter* writer)
   : Scope(parent), locals(Parameter(0))
{
   this->writer = writer;
   this->level = 0;
   this->saved = this->reserved = 0;
   this->rootBookmark = -1;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->writer = parent->writer;
   this->level = parent->level;
   this->saved = parent->saved;
   this->reserved = parent->reserved;
   this->rootBookmark = -1;
}

ObjectInfo Compiler::CodeScope :: mapObject(TerminalInfo identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset) {
      if (StringHelper::compare(identifier, SUBJECT_VAR)) {
         return ObjectInfo(okSubject, local.offset);
      }
      else if (local.stackAllocated) {
         return ObjectInfo(okLocalAddress, local.offset, local.class_ref);
      }
      else return ObjectInfo(okLocal, local.offset, 0, local.sign_ref);
   }
   else return Scope::mapObject(identifier);
}

void Compiler::CodeScope :: compileLocalHints(DNode hints, ref_t& type, int& size, ref_t& classReference)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (StringHelper::compare(terminal, HINT_TYPE)) {
         TerminalInfo typeValue = hints.firstChild().Terminal();

         type = moduleScope->mapType(typeValue);
         if (!type)
            raiseError(errUnknownSubject, typeValue);

         size = moduleScope->defineTypeSize(type, classReference);
      }
      else if (StringHelper::compare(terminal, HINT_SIZE)) {
         int itemSize = moduleScope->defineTypeSize(type);

         TerminalInfo sizeValue = hints.firstChild().Terminal();
         if (itemSize < 0 && sizeValue.symbol == tsInteger) {
            itemSize = -itemSize;

            size = StringHelper::strToInt(sizeValue.value) * itemSize;
         }
         else if (itemSize < 0 && sizeValue.symbol == tsHexInteger) {
            itemSize = -itemSize;

            size = StringHelper::strToLong(sizeValue.value, 16) * itemSize;
         }
         else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      }
      else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
   : ClassScope(owner->moduleScope, reference), outers(Outer())
{
   this->parent = owner;
   info.header.flags |= elNestedClass;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   String<ident_c, 10> thisVar(THIS_VAR);

   Outer owner = outers.get(thisVar);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();
      owner.outerObject.kind = okThisParam;
      owner.outerObject.param = 1;

      outers.add(thisVar, owner);
      mapKey(info.fields, thisVar, owner.reference);
   }
   return owner;
}

ObjectInfo Compiler::InlineClassScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, THIS_VAR)) {
      Outer owner = mapSelf();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuter, owner.reference);
   }
   else {
      Outer outer = outers.get(identifier);

	  // if object already mapped
      if (outer.reference!=-1) {
         if (outer.outerObject.kind == okSuper) {
            return ObjectInfo(okSuper, outer.reference);
         }
         else return ObjectInfo(okOuter, outer.reference, 0, outer.outerObject.type);
      }
      else {
         outer.outerObject = parent->mapObject(identifier);
         // handle outer fields in a special way: save only self
         if (outer.outerObject.kind==okField) {
            Outer owner = mapSelf();

            // save the outer field type if provided
            if (outer.outerObject.extraparam != 0) {
               outerFieldTypes.add(outer.outerObject.param, outer.outerObject.extraparam, true);
            }

            // map as an outer field (reference to outer object and outer object field index)
            return ObjectInfo(okOuterField, owner.reference, outer.outerObject.param, outer.outerObject.type);
         }
         // map if the object is outer one
         else if (outer.outerObject.kind==okParam || outer.outerObject.kind==okLocal || outer.outerObject.kind==okField
            || outer.outerObject.kind==okOuter || outer.outerObject.kind==okSuper || outer.outerObject.kind == okThisParam
            || outer.outerObject.kind == okOuterField || outer.outerObject.kind == okLocalAddress)
         {
            outer.reference = info.fields.Count();

            outers.add(identifier, outer);
            mapKey(info.fields, identifier.value, outer.reference);

            return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam, outer.outerObject.type);
         }
         // if inline symbol declared in symbol it treats self variable in a special way
         else if (StringHelper::compare(identifier, SELF_VAR)) {
            return ObjectInfo(okParam, (size_t)-1);
         }
         else if (outer.outerObject.kind == okUnknown) {
            // check if there is inherited fields
            outer.reference = info.fields.get(identifier);
            if (outer.reference != -1) {
               return ObjectInfo(okField, outer.reference);
            }
            else return outer.outerObject;
         }
         else return outer.outerObject;
      }
   }
}

// --- Compiler ---

Compiler :: Compiler(StreamReader* syntax)
   : _parser(syntax), _verbs(0)
{
   _optFlag = 0;

   ByteCodeCompiler::loadVerbs(_verbs);
   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: appendObjectInfo(CodeScope& scope, ObjectInfo object)
{
   if (object.type != 0) {
      scope.writer->appendNode(lxType, object.type);
   }
   ref_t objectRef = resolveObjectReference(scope, object);
   if (objectRef != 0) {
      scope.writer->appendNode(lxTarget, objectRef);
   }
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

ref_t Compiler :: mapNestedExpression(CodeScope& scope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // otherwise auto generate the name
   ReferenceNs name(moduleScope->module->Name(), INLINE_POSTFIX);

   findUninqueName(moduleScope->module, name);

   return moduleScope->module->mapReference(name);
}

bool Compiler :: checkIfCompatible(CodeScope& scope, ref_t typeRef, ObjectInfo object)
{
   if (object.type == typeRef) {
      return true;
   }
   // NOTE : $nil is compatible to any type
   else if (object.kind == okNil) {
      return true;
   }
   else if (object.kind == okIntConstant) {
      int flags = scope.moduleScope->getClassFlags(scope.moduleScope->typeHints.get(typeRef));

      return (flags & elDebugMask) == elDebugDWORD;
   }
   else return scope.moduleScope->checkIfCompatible(typeRef, resolveObjectReference(scope, object));
}

ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object)
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
         return scope.moduleScope->intReference;
      case okLongConstant:
         return scope.moduleScope->longReference;
      case okRealConstant:
         return scope.moduleScope->realReference;
      case okLiteralConstant:
         return scope.moduleScope->literalReference;
      case okCharConstant:
         return scope.moduleScope->charReference;
      case okThisParam:
         return scope.getClassRefId(false);
      case okSubject:
      case okSubjectDispatcher:
      case okSignatureConstant:
         return scope.moduleScope->signatureReference;
      case okSuper:
         return object.param;
      case okParams:
         return scope.moduleScope->paramsReference;
      default:
         if (object.kind == okObject && object.param != 0) {
            return object.param;
         }
         else return object.type != 0 ? scope.moduleScope->typeHints.get(object.type) : 0;
   }
}

void Compiler :: declareParameterDebugInfo(MethodScope& scope, bool withThis, bool withSelf)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // declare method parameter debug info
   LocalMap::Iterator it = scope.parameters.start();
   while (!it.Eof()) {
      if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->paramsReference)) {
         _writer.declareLocalParamsInfo(*scope.tape, it.key(), -1 - (*it).offset);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->intReference)) {
         _writer.declareLocalIntInfo(*scope.tape, it.key(), -1 - (*it).offset, true);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->longReference)) {
         _writer.declareLocalLongInfo(*scope.tape, it.key(), -1 - (*it).offset, true);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->realReference)) {
         _writer.declareLocalRealInfo(*scope.tape, it.key(), -1 - (*it).offset, true);
      }
      else _writer.declareLocalInfo(*scope.tape, it.key(), -1 - (*it).offset);

      it++;
   }
   if (withThis)
      _writer.declareSelfInfo(*scope.tape, 1);

   if (withSelf)
      _writer.declareSelfInfo(*scope.tape, -1);

   _writer.declareMessageInfo(*scope.tape, _writer.writeMessage(moduleScope->debugModule, moduleScope->module, _verbs, scope.message));
}

void Compiler :: importCode(DNode node, ModuleScope& scope, CommandTape* tape, ident_t referenceProperName)
{
   ReferenceNs fullName(scope.project->resolveForward(IMPORT_FORWARD), referenceProperName + strlen(INTERNAL_MODULE) + 1);

   ByteCodeIterator it = tape->end();

   ref_t reference = 0;
   _Module* api = scope.project->resolveModule(fullName, reference);

   _Memory* section = api != NULL ? api->mapSection(reference | mskCodeRef, true) : NULL;
   if (section == NULL) {
      scope.raiseError(errInvalidLink, node.Terminal());
   }
   else tape->import(section, true);

   // goes to the first imported command
   it++;

   // import references
   while (!it.Eof()) {
      CommandTape::import(*it, api, scope.module);
      it++;
   }
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed)
{
   ModuleScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.classClassRef;

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
      scope.info.classClassRef = classClassCopy;
      scope.info.header.flags |= flagCopy;

      return irSuccessfull;
   }
   else return irUnsuccessfull;
}

void Compiler :: compileParentDeclaration(DNode node, ClassScope& scope)
{
   // base class system'object must not to have a parent
   ref_t parentRef = scope.info.header.parentRef;
   if (scope.info.header.parentRef == scope.reference) {
      if (node.Terminal() != nsNone)
         scope.raiseError(errInvalidSyntax, node.Terminal());

      parentRef = 0;
   }
   // if the class has an implicit parent
   else if (node.Terminal() != nsNone) {
      TerminalInfo identifier = node.Terminal();
      if (identifier == tsIdentifier || identifier == tsPrivate) {
         parentRef = scope.moduleScope->mapTerminal(node.Terminal(), true);
      }
      else parentRef = scope.moduleScope->mapReference(identifier);

      if (parentRef == 0)
         scope.raiseError(errUnknownClass, node.Terminal());
   }
   InheritResult res = compileParentDeclaration(parentRef, scope);
   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   if (res == irInvalid) {
      scope.raiseError(errInvalidParent, node.Terminal());
   }
   if (res == irSealed) {
      scope.raiseError(errSealedParent, node.Terminal());
   }
   else if (res == irUnsuccessfull)
      scope.raiseError(node != nsNone ? errUnknownClass : errUnknownBaseClass, node.Terminal());
}

Compiler::InheritResult Compiler :: compileParentDeclaration(ref_t parentRef, ClassScope& scope, bool ignoreSealed)
{
   scope.info.header.parentRef = parentRef;
   if (scope.info.header.parentRef != 0) {
      return inheritClass(scope, scope.info.header.parentRef, ignoreSealed);
   }
   else return irSuccessfull;
}

void Compiler :: compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue)
{
   if (switchValue.kind == okObject) {
      scope.writer->insert(lxVariable);
      scope.writer->insert(lxSwitching);
      scope.writer->closeNode();

      switchValue.kind = okBlockLocal;
      switchValue.param = 1;
   }
   else scope.writer->insert(lxSwitching);

   DNode option = node.firstChild();
   while (option == nsSwitchOption || option == nsBiggerSwitchOption || option == nsLessSwitchOption)  {
      scope.writer->newNode(lxOption);
      recordDebugStep(scope, option.firstChild().FirstTerminal(), dsStep);

      //      _writer.declareSwitchOption(*scope.tape);

      int operator_id = EQUAL_MESSAGE_ID;
      if (option == nsBiggerSwitchOption) {
         operator_id = GREATER_MESSAGE_ID;
      }
      else if (option == nsLessSwitchOption) {
         operator_id = LESS_MESSAGE_ID;
      }

      scope.writer->newBookmark();

      writeTerminal(TerminalInfo(), scope, switchValue);

      DNode operand = option.firstChild();
      ObjectInfo result = compileOperator(operand, scope, switchValue, 0, operator_id);
      if (!checkIfCompatible(scope, scope.moduleScope->boolType, result)) {
         scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));

         appendCoordinate(scope.writer, node.FirstTerminal());

         scope.writer->closeNode();
      }

      scope.writer->removeBookmark();

      scope.writer->newNode(lxElse, scope.moduleScope->falseReference);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild().nextNode();

      //_writer.declareBlock(*scope.tape);

      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);

      scope.writer->closeNode();

      scope.writer->closeNode();

      option = option.nextNode();
   }
   if (option == nsLastSwitchOption) {
      scope.writer->newNode(lxElse);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild();

      //_writer.declareBlock(*scope.tape);

      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);

      scope.writer->closeNode();
   }

   scope.writer->closeNode();
}

void Compiler :: compileVariable(DNode node, CodeScope& scope, DNode hints)
{
   TerminalInfo terminal = node.Terminal();

   if (!scope.locals.exist(terminal)) {
      ref_t type = 0;
      ref_t classReference = 0;
      int size = 0;
      scope.compileLocalHints(hints, type, size, classReference);

      ObjectInfo variable(okLocal, 0, 0, type);
      if (size > 0) {
         int flags = scope.moduleScope->getClassFlags(classReference) & elDebugMask;

         if (!allocateStructure(scope, size, variable))
            scope.raiseError(errInvalidOperation, node.Terminal());

         // make the reservation permanent
         scope.saved = scope.reserved;

         switch (flags & elDebugMask)
         {
            case elDebugDWORD:
               scope.writer->newNode(lxIntVariable, 0);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            case elDebugQWORD:
               scope.writer->newNode(lxLongVariable, 0);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            case elDebugReal64:
               scope.writer->newNode(lxReal64Variable, 0);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            case elDebugBytes:
               scope.writer->newNode(lxBytesVariable, size);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            case elDebugShorts:
               scope.writer->newNode(lxShortsVariable, size);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            case elDebugIntegers:
               scope.writer->newNode(lxIntsVariable, size);
               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->closeNode();
               break;
            default:
               // HOTFIX : size should be provide only for dynamic variables
               if (test(flags, elDynamicRole)) {
                  scope.writer->newNode(lxBinaryVariable, size);
               }
               else scope.writer->newNode(lxBinaryVariable);

               scope.writer->appendNode(lxTerminal, (int)terminal.value);
               scope.writer->appendNode(lxLevel, variable.param);
               scope.writer->appendNode(lxClassName, (int)scope.moduleScope->module->resolveReference(classReference));
               scope.writer->closeNode();
               break;
         }
      }
      else {
         int level = scope.newLocal();

         scope.writer->newNode(lxVariable, 0);
         scope.writer->appendNode(lxTerminal, (int)terminal.value);
         scope.writer->appendNode(lxLevel, level);
         scope.writer->closeNode();

         variable.param = level;

         size = 0; // to indicate assigning by ref
      }

      DNode assigning = node.firstChild();
      if (assigning == nsAssigning) {
         scope.writer->newNode(lxAssigning, size);
         writeTerminal(terminal, scope, variable);

         compileAssigningExpression(node, assigning, scope, variable, /*size > 0 ? HINT_INITIALIZING : */0);

         scope.writer->closeNode();
      }

      if (variable.kind == okLocal) {
         scope.mapLocal(terminal, variable.param, type);
      }
      else scope.mapLocal(node.Terminal(), variable.param, variable.extraparam, true);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

void Compiler :: writeTerminal(TerminalInfo terminal, CodeScope& scope, ObjectInfo object)
{
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         scope.writer->newNode(lxSymbol, object.param);
         break;
      case okConstantClass:
         scope.writer->newNode(lxConstantClass, object.param);
         break;
      case okConstantSymbol:
         scope.writer->newNode(lxConstantSymbol, object.param);
         break;
      case okLiteralConstant:
         scope.writer->newNode(lxConstantString, object.param);
         break;
      case okCharConstant:
         scope.writer->newNode(lxConstantChar, object.param);
         break;
      case okIntConstant:
         scope.writer->newNode(lxConstantInt, object.param);
         break;
      case okLongConstant:
         scope.writer->newNode(lxConstantLong, object.param);
         break;
      case okRealConstant:
         scope.writer->newNode(lxConstantReal, object.param);
         break;
      case okLocal:
      case okParam:
      case okThisParam:
         scope.writer->newNode(lxLocal, object.param);
         break;
      case okSuper:
         scope.writer->newNode(lxLocal, 1);
         break;
      case okField:
      case okOuter:
         scope.writer->newNode(lxField, object.param);
         break;
      case okOuterField:
         scope.writer->newNode(lxExpression);
         scope.writer->appendNode(lxField, object.param);
         scope.writer->appendNode(lxResultField, object.extraparam);
         break;
      case okLocalAddress:
         scope.writer->newNode(lxLocalAddress, object.param);
         break;
      case okFieldAddress:
         scope.writer->newNode(lxFieldAddress, object.param);
         break;
      case okNil:
         scope.writer->newNode(lxNil, object.param);
         break;
      case okVerbConstant:
         scope.writer->newNode(lxVerbConstant, object.param);
         break;
      case okMessageConstant:
         scope.writer->newNode(lxMessageConstant, object.param);
         break;
      case okSignatureConstant:
         scope.writer->newNode(lxSignatureConstant, object.param);
         break;
      case okSubject:
      case okSubjectDispatcher:
         scope.writer->newNode(lxLocalAddress, object.param);
         break;
      case okBlockLocal:
         scope.writer->newNode(lxBlockLocal, object.param);
         break;
      case okParams:
         scope.writer->newNode(lxBlockLocalAddr, object.param);
         break;
      case okObject:
         scope.writer->newNode(lxResult);
         break;
      case okConstantRole:
         scope.writer->newNode(lxConstantSymbol, object.param);
         break;
      case okExternal:
      case okInternal:
         // HOTFIX : external / internal node will be declared later
         return;
   }

   appendObjectInfo(scope, object);

   scope.writer->closeNode();
}

ObjectInfo Compiler :: compileTerminal(DNode node, CodeScope& scope, int mode)
{
   TerminalInfo terminal = node.Terminal();

   ObjectInfo object;
   if (terminal==tsLiteral) {
      object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(terminal));
   }
   else if (terminal==tsCharacter) {
      object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(terminal));
   }
   else if (terminal == tsInteger) {
      String<ident_c, 20> s(terminal.value, getlength(terminal.value));

      long integer = s.toInt();
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.clear();
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant(s));
   }
   else if (terminal == tsLong) {
      String<ident_c, 30> s("_"); // special mark to tell apart from integer constant
      s.append(terminal.value, getlength(terminal.value) - 1);

      long long integer = StringHelper::strToLongLong(s + 1, 10);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant(s));
   }
   else if (terminal == tsHexInteger) {
      String<ident_c, 20> s(terminal.value, getlength(terminal.value) - 1);

      long integer = s.toULong(16);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.clear();
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant(s));
   }
   else if (terminal == tsReal) {
      String<ident_c, 30> s(terminal.value, getlength(terminal.value) - 1);
      double number = StringHelper::strToDouble(s);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // HOT FIX : to support 0r constant
      if (s.Length() == 1) {
         s.append(".0");
      }

      object = ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant(s));
   }
   else if (!emptystr(terminal))
      object = scope.mapObject(terminal);

   writeTerminal(terminal, scope, object);

   return object;
}

ObjectInfo Compiler :: compileObject(DNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

   DNode member = objectNode.firstChild();
   switch (member)
   {
      case nsRetStatement:
      case nsNestedClass:
         if (objectNode.Terminal() != nsNone) {
            result = compileNestedExpression(objectNode, scope, 0);
            break;
         }
      case nsSubCode:
      case nsSubjectArg:
      case nsMethodParameter:
         result = compileNestedExpression(member, scope, 0);
         break;
      case nsInlineExpression:
         result = compileNestedExpression(objectNode, scope, HINT_ACTION);
         break;
      case nsExpression:
         if (isCollection(member)) {
            TerminalInfo parentInfo = objectNode.Terminal();
            // if the parent class is defined
            if (parentInfo == tsIdentifier || parentInfo == tsReference || parentInfo == tsPrivate) {
               ref_t vmtReference = scope.moduleScope->mapTerminal(parentInfo, true);
               if (vmtReference == 0)
                  scope.raiseError(errUnknownObject, parentInfo);

               result = compileCollection(member, scope, 0, vmtReference);
            }
            else result = compileCollection(member, scope, 0);
         }
         else result = compileExpression(member, scope, 0, HINT_NOBOXING);
         break;
      case nsMessageReference:
         result = compileMessageReference(member, scope);
         break;
      default:
         result = compileTerminal(objectNode, scope, mode);
   }

   return result;
}

ObjectInfo Compiler :: compileMessageReference(DNode node, CodeScope& scope)
{
   DNode arg = node.firstChild();

   IdentifierString message;

   // reserve place for param counter
   message.append('0');
   message.append('#');

   ObjectInfo retVal;
   if (arg == nsNone) {
      ref_t verb_id = _verbs.get(node.Terminal().value);
      if (verb_id != 0) {
         retVal.kind = okVerbConstant;

         message.append((char)verb_id + 0x20);
      }
      else {
         retVal.kind = okSignatureConstant;

         message.append(0x20);
         message.append('&');

         scope.moduleScope->mapSubject(node.Terminal(), message);
      }
   }
   else {
      int count = 0;
      if (!findSymbol(arg, nsSizeValue)) {
         retVal.kind = okSignatureConstant;

         message.append(0x20);
         message.append('&');
         scope.moduleScope->mapSubject(arg.Terminal(), message);
      }
      else {
         retVal.kind = okMessageConstant;

         ref_t verb_id = _verbs.get(arg.Terminal());
         if (verb_id == 0) {
            message.append(EVAL_MESSAGE_ID + 0x20);
            message.append('&');
            scope.moduleScope->mapSubject(arg.Terminal(), message);
         }
         else message.append((char)verb_id + 0x20);
      }

      arg = arg.nextNode();

      // if method has argument list
      while (arg == nsSubjectArg) {
         TerminalInfo subject = arg.Terminal();

         message.append('&');
         ref_t subjRef = scope.moduleScope->mapSubject(subject, message);
         if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
            count = OPEN_ARG_COUNT;
         }

         arg = arg.nextNode();
      }

      if (arg == nsSizeValue) {
         TerminalInfo size = arg.Terminal();
         if (size == tsInteger) {
            count += StringHelper::strToInt(size.value);
            if (count > 0x0F)
               scope.raiseError(errNotApplicable, size);
         }
         else scope.raiseError(errInvalidOperation, size);

         // define the number of parameters
         message[0] = message[0] + (char)count;
      }
   }

   retVal.param = scope.moduleScope->module->mapReference(message);

   writeTerminal(TerminalInfo(), scope, retVal);

   return retVal;
}

bool Compiler :: writeBoxing(TerminalInfo terminal, CodeScope& scope, ObjectInfo& object, ref_t targetTypeRef, int mode)
{
   if (test(mode, HINT_NOBOXING))
      return false;

   ModuleScope* moduleScope = scope.moduleScope;

   int size = 0;
   ref_t classRef = 0;
   bool unboxRequired = false;
   if (object.type != 0) {
      classRef = moduleScope->typeHints.get(object.type);
   }
   else classRef = object.extraparam;

   ClassInfo sourceInfo;
   if (classRef != 0)
      moduleScope->loadClassInfo(sourceInfo, scope.moduleScope->module->resolveReference(classRef), false);

   if (test(sourceInfo.header.flags, elStructureRole) && test(sourceInfo.header.flags, elEmbeddable))
      size = sourceInfo.size;

   LexicalType boxing = lxNone;
   if (targetTypeRef != 0) {
      ref_t targetClassReference = moduleScope->typeHints.get(targetTypeRef);
      ClassInfo targetInfo;
      moduleScope->loadClassInfo(targetInfo, scope.moduleScope->module->resolveReference(targetClassReference), false);

      // if the target is structure
      if (test(targetInfo.header.flags, elStructureRole)) {
         // NOTE : compiler magic!
         // if the source is structure
         if (test(sourceInfo.header.flags, elStructureRole)) {
            // if target is source wrapper (i.e. target is a source container)
            if (test(targetInfo.header.flags, elStructureWrapper | elEmbeddable) && moduleScope->typeHints.exist(targetInfo.fieldTypes.get(0), classRef)) {
               boxing = lxBoxing;

               classRef = targetClassReference;
               object.type = targetTypeRef;
            }
            // if source is target wrapper (i.e. source is a target container)
            // virtually copy the value into the stack allocated local
            else if (test(sourceInfo.header.flags, elStructureWrapper) && moduleScope->typeHints.exist(sourceInfo.fieldTypes.get(0), targetClassReference)) {
               boxing = lxBoxing;

               classRef = targetClassReference;
               object.type = targetTypeRef;
            }
         }

         if (!test(targetInfo.header.flags, elReadOnlyRole))
            unboxRequired = (object.kind == okLocalAddress || object.kind == okFieldAddress);
      }
      // NOTE : compiler magic!
      // if the target is generic wrapper (container) and the object is a local
      else if (test(targetInfo.header.flags, elWrapper)) {
         classRef = targetClassReference;
         boxing = lxBoxing;
         size = 0;
         unboxRequired = (object.kind == okLocal || object.kind == okField);
      }
   }

   if (object.kind == okLocalAddress) {
      boxing = lxBoxing;
   }
   else if (object.kind == okParams) {
      boxing = lxArgBoxing;
      classRef = scope.moduleScope->paramsReference;
      size = -1;
   }
   else if ((object.kind == okLocal || object.kind == okParam || object.kind == okThisParam) && object.extraparam == -1 && size != 0) {
      boxing = lxCondBoxing;
   }
   else if (object.kind == okFieldAddress) {
      if (object.param > 0) {
         allocateStructure(scope, 0, object);
         scope.writer->insertChild(lxLocalAddress, object.param);
         scope.writer->insert(lxAssigning, size);
         scope.writer->closeNode();
      }
      else boxing = lxBoxing;
   }
   else if (object.kind == okSubject) {
      boxing = lxBoxing;
      size = 4;
      classRef = scope.moduleScope->signatureReference;
   }

   if (boxing != lxNone) {
      scope.writer->insert(unboxRequired ? lxUnboxing : boxing, size);
      scope.writer->appendNode(lxTarget, classRef);
      appendCoordinate(scope.writer, terminal);

      if (unboxRequired) {
         if (test(mode, HINT_ALTBOXING)) {
            int level = scope.newLocal();
            scope.writer->insertChild(scope.rootBookmark, lxVariable, 0);
            scope.writer->appendNode(lxTempLocal, level);
         }
      }

      scope.writer->closeNode();

      object.kind = okObject;
      object.param = classRef;

      return true;
   }
   else return false;
}

ref_t Compiler :: mapMessage(DNode node, CodeScope& scope, size_t& paramCount, bool& argsUnboxing)
{
   bool   first = true;
   ref_t  verb_id = 0;
   DNode  arg;

   IdentifierString signature;
   TerminalInfo     verb = node.Terminal();
   // check if it is a short-cut eval message
   if (node == nsMessageParameter) {
      arg = node;

      verb_id = EVAL_MESSAGE_ID;
   }
   else {
      arg = node.firstChild();

      verb_id = _verbs.get(verb.value);
      if (verb_id == 0) {
         size_t id = scope.moduleScope->mapSubject(verb, signature);

         // if followed by argument list - it is EVAL verb
         if (arg != nsNone) {
            // HOT FIX : strong types cannot be used as a custom verb with a parameter
            if (scope.moduleScope->typeHints.exist(id))
               scope.raiseError(errStrongTypeNotAllowed, verb);

            verb_id = EVAL_MESSAGE_ID;

            first = false;
         }
         // otherwise it is GET message
         else verb_id = GET_MESSAGE_ID;
      }
   }

   paramCount = 0;
   // if message has generic argument list
   while (arg == nsMessageParameter) {
      paramCount++;

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subjRef = scope.moduleScope->mapSubject(subject, signature);

      arg = arg.nextNode();

      // skip an argument
      if (arg == nsMessageParameter) {
         // if it is an open argument list
         if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > 0x0F)
               scope.raiseError(errNotApplicable, subject);

            ObjectInfo argListParam = scope.mapObject(arg.firstChild().Terminal());
            // HOTFIX : set flag if the argument list has to be unboxed
            if (arg.firstChild().nextNode() == nsNone && argListParam.kind == okParams) {
               argsUnboxing = true;
            }
         }
         else {
            paramCount++;

            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            arg = arg.nextNode();
         }
      }
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
   // if class reference available - select the possible type
   else {
      if (scope.moduleScope->extensionHints.exist(messageRef)) {
         ref_t classRef = resolveObjectReference(scope, object);
         SubjectMap::Iterator it = scope.moduleScope->extensionHints.start();
         while (!it.Eof()) {
            if (it.key() == messageRef) {
               if (scope.moduleScope->typeHints.exist(*it, classRef)) {
                  type = *it;

                  break;
               }
            }

            it++;
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

ObjectInfo Compiler :: compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   if (!checkIfCompatible(scope, scope.moduleScope->boolType, object)) {
      scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));

      appendCoordinate(scope.writer, node.FirstTerminal());

      scope.writer->closeNode();
   }

   DNode elsePart = node.select(nsElseOperation);
   if (elsePart != nsNone) {
      scope.writer->newNode(lxIf, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);

      compileBranching(node, scope/*, object, operator_id, 0*/);

      scope.writer->closeNode();
      scope.writer->newNode(lxElse);

      compileBranching(elsePart, scope); // for optimization, the condition is checked only once

      scope.writer->closeNode();
   }
   else {
      scope.writer->newNode(lxIf, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);

      compileBranching(node, scope);

      scope.writer->closeNode();
   }

   scope.writer->insert(lxBranching);
   scope.writer->closeNode();

   return ObjectInfo(okObject);
}

int Compiler ::mapOperandType(CodeScope& scope, ObjectInfo operand)
{
   if (operand.kind == okIntConstant) {
      return elDebugDWORD;
   }
   else if (operand.kind == okLongConstant) {
      return elDebugQWORD;
   }
   else if (operand.kind == okRealConstant) {
      return elDebugReal64;
   }
   else if (operand.kind == okSubject) {
      return elDebugSubject;
   }
   else return scope.moduleScope->getClassFlags(resolveObjectReference(scope, operand)) & elDebugMask;
}

int Compiler :: mapVarOperandType(CodeScope& scope, ObjectInfo operand)
{
   int flags = scope.moduleScope->getClassFlags(resolveObjectReference(scope, operand));

   // read only classes cannot be used for variable operations
   if (test(flags, elReadOnlyRole))
      flags = 0;

   return flags & elDebugMask;
}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   ModuleScope* moduleScope = scope.moduleScope;

   ObjectInfo retVal(okObject);

   // HOTFIX : recognize SET_REFER_MESSAGE_ID
   if (operator_id == REFER_MESSAGE_ID && node.nextNode() == nsAssigning)
      operator_id = SET_REFER_MESSAGE_ID;

   bool dblOperator = IsDoubleOperator(operator_id);
   bool notOperator = IsInvertedOperator(operator_id);

   ObjectInfo operand = compileExpression(node, scope, 0, 0);

   // try to implement the primitive operation directly
   LexicalType primitiveOp = lxNone;
   size_t size = 0;

   // if it is comparing with nil
   if (object.kind == okNil && operator_id == EQUAL_MESSAGE_ID) {
      primitiveOp = lxNilOp;
   }
   // HOTFIX : primitive operations can be implemented only in the method
   // because the symbol implementations do not open a new stack frame
   else if (scope.getScope(Scope::slMethod) != NULL) {
      int lflag, rflag;

      if (IsVarOperator(operator_id)) {
         lflag = mapVarOperandType(scope, object);
      }
      else lflag = mapOperandType(scope, object);

      rflag = mapOperandType(scope, operand);

      if (lflag == rflag) {
         if (lflag == elDebugDWORD && (IsExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
            if (IsExprOperator(operator_id))
               retVal.param = resolveObjectReference(scope, object);

            primitiveOp = lxIntOp;
            size = 4;
         }
         else if (lflag == elDebugQWORD && (IsExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
            if (IsExprOperator(operator_id))
               retVal.param = moduleScope->longReference;

            primitiveOp = lxLongOp;
            size = 8;
         }
         else if (lflag == elDebugReal64 && (IsRealExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
            if (IsExprOperator(operator_id))
               retVal.param = moduleScope->realReference;

            primitiveOp = lxRealOp;
            size = 8;
         }
         else if (lflag == elDebugSubject && IsCompOperator(operator_id)) {
            primitiveOp = lxIntOp;
            size = 4;
         }
      }
      else if (IsReferOperator(operator_id)) {
         if (lflag == elDebugIntegers && rflag == elDebugDWORD) {
            if (operator_id == SET_REFER_MESSAGE_ID) {
               ObjectInfo operand2 = compileExpression(node.nextNode().firstChild(), scope, 0, 0);

               if (mapOperandType(scope, operand2) == elDebugDWORD) {
                  primitiveOp = lxIntArrOp;
               }
            }
            else {
               size = 4;
               retVal.param = moduleScope->intReference;
               primitiveOp = lxIntArrOp;
            }
         }
         else if (lflag == elDebugArray && rflag == elDebugDWORD) {
            // check if it is typed array
            ref_t classReference = resolveObjectReference(scope, object);
            ref_t type = 0;
            if (classReference != 0) {
               ClassInfo info;
               scope.moduleScope->loadClassInfo(info, scope.moduleScope->module->resolveReference(classReference), false);
               type = info.fieldTypes.get(-1);
            }

            if (operator_id == SET_REFER_MESSAGE_ID) {
               ObjectInfo operand2 = compileExpression(node.nextNode().firstChild(), scope, type, 0);
            }
            else retVal.type = type;

            primitiveOp = lxArrOp;
         }
      }
   }

   if (primitiveOp != lxNone) {
      scope.writer->insert(primitiveOp, operator_id);

      if (IsCompOperator(operator_id)) {
         if (notOperator) {
            scope.writer->appendNode(lxIfValue, scope.moduleScope->falseReference);
            scope.writer->appendNode(lxElseValue, scope.moduleScope->trueReference);
         }
         else {
            scope.writer->appendNode(lxIfValue, scope.moduleScope->trueReference);
            scope.writer->appendNode(lxElseValue, scope.moduleScope->falseReference);
         }
      }

      scope.writer->closeNode();

      if (retVal.param != 0) {
         allocateStructure(scope, 0, retVal);

         scope.writer->insertChild(lxLocalAddress, retVal.param);
         scope.writer->insert(lxAssigning, size);
         scope.writer->closeNode();
      }

      if (IsCompOperator(operator_id))
         retVal.type = moduleScope->boolType;
   }
   else {
      if (operator_id == SET_REFER_MESSAGE_ID)
         compileExpression(node.nextNode().firstChild(), scope, 0, 0);

      int message_id = encodeMessage(0, operator_id, dblOperator ? 2 : 1);

      // otherwise operation is replaced with a normal message call
      retVal = compileMessage(node, scope, object, message_id, mode/* | HINT_INLINE*/);

      if (notOperator) {
         scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
         scope.writer->closeNode();

         scope.writer->insert(lxBoolOp, NOT_MESSAGE_ID);
         scope.writer->appendNode(lxIfValue, scope.moduleScope->trueReference);
         scope.writer->appendNode(lxElseValue, scope.moduleScope->falseReference);
         scope.writer->closeNode();

         retVal.type = scope.moduleScope->boolType;
      }
   }

   if (dblOperator)
      node = node.nextNode();

   return retVal;
}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   TerminalInfo operator_name = node.Terminal();
   int operator_id = _operators.get(operator_name);

   // if it is branching operators
   if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
      return compileBranchingOperator(node, scope, object, mode, operator_id);
   }

   return compileOperator(node, scope, object, mode, operator_id);
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode)
{
   ObjectInfo retVal(okObject);

   int signRef = getSignature(messageRef);
   int paramCount = getParamCount(messageRef);

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target);
   bool classFound = false;
   bool dispatchCall = false;
   int methodHint = classReference != 0 ? scope.moduleScope->checkMethod(classReference, messageRef, classFound, retVal.type) : 0;
   int callType = methodHint & tpMask;

   if (target.kind == okConstantClass) {
      retVal.param = target.param;

      // constructors are always sealed
      callType = tpSealed;
   }
   else if (classReference == scope.moduleScope->signatureReference) {
      dispatchCall = true;
   }
   else if (target.kind == okSuper) {
      // parent methods are always sealed
      callType = tpSealed;
   }

   if (dispatchCall) {
      scope.writer->insert(lxDirectCalling, encodeVerb(DISPATCH_MESSAGE_ID));

      scope.writer->appendNode(lxMessage, messageRef);
      scope.writer->appendNode(lxTarget, classReference);
   }
   else if (callType == tpClosed) {
      scope.writer->insert(lxSDirctCalling, messageRef);

      scope.writer->appendNode(lxTarget, classReference);
      if (test(methodHint, tpStackSafe))
         scope.writer->appendNode(lxStacksafe);
   }
   else if (callType == tpSealed) {
      scope.writer->insert(lxDirectCalling, messageRef);

      scope.writer->appendNode(lxTarget, classReference);
      if (test(methodHint, tpStackSafe))
         scope.writer->appendNode(lxStacksafe);
      if (test(methodHint, tpEmbeddable))
         scope.writer->appendNode(lxEmbeddable);
   }
   else {
      scope.writer->insert(lxCalling, messageRef);

      // if the class found and the message is not supported - warn the programmer and raise an exception
      if (classFound && callType == tpUnknown) {
         scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.FirstTerminal());
      }
   }

   // the result of get&type message should be typed
   if (paramCount == 0 && getVerb(messageRef) == GET_MESSAGE_ID && scope.moduleScope->typeHints.exist(signRef)) {
      retVal.type = signRef;
   }

   recordDebugStep(scope, node.Terminal(), dsStep);

   appendObjectInfo(scope, retVal);

   // define the message target if required
   if (target.kind == okConstantRole || target.kind == okSubjectDispatcher) {
      scope.writer->newNode(lxOverridden);
      writeTerminal(TerminalInfo(), scope, target);
      scope.writer->closeNode();
   }

   scope.writer->closeNode();

   return retVal;
}

ref_t Compiler :: compileMessageParameters(DNode node, CodeScope& scope/*, bool stacksafe*/)
{
   bool argsUnboxing = false;
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount, argsUnboxing);

   int paramMode = 0;
   // HOTFIX : if open argument list has to be unboxed
   // alternative boxing routine should be used (using a temporal variable)
   if (argsUnboxing)
      paramMode |= HINT_ALTBOXING;

   DNode arg;
   if (node == nsMessageParameter) {
      arg = node;
   }
   else arg = node.firstChild();

   // if message has generic argument list
   while (arg == nsMessageParameter) {
      compileExpression(arg.firstChild(), scope, 0, paramMode);

      paramCount++;

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      ref_t subjRef = scope.moduleScope->mapType(subject);

      arg = arg.nextNode();

      // skip an argument
      if (arg == nsMessageParameter) {
         // if it is an open argument list
         if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
            // check if argument list should be unboxed
            DNode param = arg.firstChild();

            ObjectInfo argListParam = scope.mapObject(arg.firstChild().Terminal());
            if (arg.firstChild().nextNode() == nsNone && argListParam.kind == okParams) {
               scope.writer->newNode(lxArgUnboxing);
               writeTerminal(arg.firstChild().Terminal(), scope, argListParam);
               scope.writer->closeNode();
            }
            else {
               while (arg != nsNone) {
                  compileExpression(arg.firstChild(), scope, 0, paramMode);

                  arg = arg.nextNode();
               }

               // terminator
               writeTerminal(TerminalInfo(), scope, ObjectInfo(okNil));
            }
         }
         else {
            compileExpression(arg.firstChild(), scope, subjRef, paramMode);

            arg = arg.nextNode();
         }
      }
   }

   return messageRef;
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object)
{
   ref_t messageRef = compileMessageParameters(node, scope);
   ref_t extensionRef = mapExtension(scope, messageRef, object);

   if (extensionRef != 0) {
      //HOTFIX: A proper method should have a precedence over an extension one
      if (scope.moduleScope->checkMethod(resolveObjectReference(scope, object), messageRef) == tpUnknown) {
         object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
      }
   }

   ObjectInfo retVal = compileMessage(node, scope, object, messageRef, 0);

   return retVal;
}

ObjectInfo Compiler :: compileOperations(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   ObjectInfo currentObject = object;

   DNode member = node.nextNode();

   bool externalMode = false;
   if (object.kind == okExternal) {
      currentObject = compileExternalCall(member, scope, node.Terminal(), mode);

      externalMode = true;
      member = member.nextNode();

      if (member != nsNone)
         scope.raiseError(errInvalidOperation, node.Terminal());
   }
   else if (object.kind == okInternal) {
      currentObject = compileInternalCall(member, scope, object);

      member = member.nextNode();
   }

   while (member != nsNone) {
      if (member == nsAssigning) {
         ref_t classReference = 0;
         int size = 0;
         if (object.kind == okLocalAddress) {
            classReference = object.extraparam;

            size = scope.moduleScope->defineStructSize(classReference);
         }
         else if (object.kind == okFieldAddress) {
            size = scope.moduleScope->defineTypeSize(object.type, classReference);
         }
         else if (object.kind == okLocal || object.kind == okField || object.kind == okOuterField) {

         }
         else scope.raiseError(errInvalidOperation, node.Terminal());

         currentObject = compileAssigningExpression(node, member, scope, currentObject);

         if (size >= 0) {
            scope.writer->insert(lxAssigning, size);
            scope.writer->closeNode();
         }
      }
      else if (member == nsMessageOperation) {
         currentObject = compileMessage(member, scope, currentObject);
      }
      else if (member == nsMessageParameter) {
         currentObject = compileMessage(member, scope, currentObject);

         // skip all except the last message parameter
         while (member.nextNode() == nsMessageParameter)
            member = member.nextNode();
      }
      else if (member == nsExtension) {
         currentObject = compileExtension(member, scope, currentObject, mode);
      }
      else if (member == nsL3Operation || member == nsL4Operation || member == nsL5Operation || member == nsL6Operation
         || member == nsL7Operation || member == nsL0Operation)
      {
         currentObject = compileOperator(member, scope, currentObject, mode);
         // HOTFIX : if the primitive operation is followed by another operation
         // the result may be boxed
         if (member.nextNode() != nsNone && currentObject.kind == okLocalAddress)
            writeBoxing(node.FirstTerminal(), scope, currentObject, 0, 0);
      }
      else if (member == nsAltMessageOperation) {
         scope.writer->newBookmark();

         scope.writer->appendNode(lxCurrent);

         currentObject = compileMessage(member, scope, ObjectInfo(okObject));

         scope.writer->removeBookmark();
      }
      else if (member == nsCatchMessageOperation) {
         scope.writer->newBookmark();

         scope.writer->appendNode(lxResult);

         currentObject = compileMessage(member, scope, ObjectInfo(okObject));

         scope.writer->removeBookmark();
      }
      else if (member == nsSwitching) {
         compileSwitch(member, scope, currentObject);

         currentObject = ObjectInfo(okObject);
      }

      member = member.nextNode();
   }

   return currentObject;
}

ObjectInfo Compiler :: compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;
   ObjectInfo   role;

   DNode roleNode = node.firstChild();
   // check if the extension can be used as a static role (it is constant)
   if (roleNode.firstChild() == nsNone) {
      int flags = 0;

      role = scope.mapObject(roleNode.Terminal());
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
         }
      }
      if (role.kind == okSubject) {
         // if subject variable is used
         role = ObjectInfo(okSubjectDispatcher, role.param);
      }
      // if the symbol VMT can be used as an external role
      else if (test(flags, elStateless)) {
         role = ObjectInfo(okConstantRole, role.param);
      }
   }

   // if it is a generic role
   if (role.kind != okSubjectDispatcher && role.kind != okConstantRole) {
      scope.writer->newNode(lxOverridden);
      role = compileExpression(roleNode, scope, 0, 0);
      scope.writer->closeNode();
   }

   // override standard message compiling routine
   node = node.nextNode();

   return compileExtensionMessage(node, scope, object, role/*, HINT_EXTENSION_MODE*/);
}

ObjectInfo Compiler :: compileExtensionMessage(DNode node, CodeScope& scope, ObjectInfo object, ObjectInfo role/*, int mode*/)
{
   ref_t messageRef = compileMessageParameters(node, scope);

   ObjectInfo retVal = compileMessage(node, scope, role, messageRef, 0);

   return retVal;
}

bool Compiler :: declareActionScope(DNode& node, ClassScope& scope, DNode argNode, ActionScope& methodScope, bool alreadyDeclared)
{
   bool lazyExpression = isReturnExpression(node.firstChild());

   methodScope.message = encodeVerb(EVAL_MESSAGE_ID);

   if (argNode != nsNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope);

      node = node.select(nsSubCode);
   }

   if (!alreadyDeclared) {
      ref_t parentRef = scope.info.header.parentRef;
      if (lazyExpression) {
         parentRef = scope.moduleScope->getBaseLazyExpressionClass();
      }
      else if (getSignature(methodScope.message) == 0) {
         parentRef = scope.moduleScope->getBaseFunctionClass(getParamCount(methodScope.message));
      }
      else {
         // check if it is nfunc
         ref_t nfuncRef = scope.moduleScope->getBaseIndexFunctionClass(getParamCount(methodScope.message));
         if (nfuncRef != 0 && scope.moduleScope->checkMethod(nfuncRef, methodScope.message) != tpUnknown) {
            parentRef = nfuncRef;
         }
      }

      InheritResult res = compileParentDeclaration(parentRef, scope, true);
      if (res == irInvalid) {
         scope.raiseError(errInvalidParent, node.Terminal());
      }
      else if (res == irUnsuccessfull)
         scope.raiseError(/*node != nsNone ? errUnknownClass : */errUnknownBaseClass, node.Terminal());
   }

   // HOT FIX : mark action as stack safe if the hint was declared in the parent class
   methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);

   return lazyExpression;
}

void Compiler :: compileAction(DNode node, ClassScope& scope, DNode argNode, bool alreadyDeclared)
{
   _writer.declareClass(scope.tape, scope.reference);

   ActionScope methodScope(&scope);
   bool lazyExpression = declareActionScope(node, scope, argNode, methodScope, alreadyDeclared);

   // if it is single expression
   if (!lazyExpression) {
      compileActionMethod(node, methodScope);
   }
   else compileLazyExpressionMethod(node.firstChild(), methodScope);

   _writer.endClass(scope.tape);

   // stateless inline class
   if (scope.info.fields.Count()==0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;
   }
   else scope.info.header.flags &= ~elStateless;

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save();
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compileNestedVMT(DNode node, InlineClassScope& scope)
{
   _writer.declareClass(scope.tape, scope.reference);

   DNode member = node.firstChild();

   declareVMT(member, scope, nsMethod, test(scope.info.header.flags, elClosed));

   // nested class is sealed if it has no parent
   if (!test(scope.info.header.flags, elClosed))
      scope.info.header.flags |= elSealed;

   compileVMT(member, scope);

   _writer.endClass(scope.tape);

   // stateless inline class
   if (scope.info.fields.Count()==0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;
   }
   else scope.info.header.flags &= ~elStateless;

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save();
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      ownerScope.writer->appendNode(lxConstantSymbol, scope.reference);

      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
   }
   else {
      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         ownerScope.writer->newNode(lxStruct, scope.info.size);
         ownerScope.writer->appendNode(lxTarget, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node.Terminal());
      }
      else {
         // dynamic normal symbol
         ownerScope.writer->newNode(lxNested, scope.info.fields.Count());
         ownerScope.writer->appendNode(lxTarget, scope.reference);
      }

      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      //int toFree = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         ownerScope.writer->newNode(lxMember, (*outer_it).reference);
         ownerScope.writer->newBookmark();

         writeTerminal(TerminalInfo(), ownerScope, info);
         writeBoxing(node.FirstTerminal(), ownerScope, info, 0, 0);

         ownerScope.writer->removeBookmark();
         ownerScope.writer->closeNode();

         outer_it++;
      }

      ownerScope.writer->closeNode();

      return ObjectInfo(okObject, scope.reference);
   }
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, int mode)
{
   InlineClassScope scope(&ownerScope, mapNestedExpression(ownerScope));

   // if it is an action code block
   if (node == nsSubCode) {
      compileAction(node, scope, DNode());
   }
   // if it is an action code block
   else if (node == nsMethodParameter || node == nsSubjectArg) {
      compileAction(goToSymbol(node, nsInlineExpression), scope, node);
   }
   // if it is a shortcut action code block
   else if (node == nsObject && test(mode, HINT_ACTION)) {
      compileAction(node.firstChild(), scope, node);
   }
   // if it is inherited nested class
   else if (node.Terminal() != nsNone) {
	   // inherit parent
      compileParentDeclaration(node, scope);

      compileNestedVMT(node.firstChild(), scope);
   }
   // if it is normal nested class
   else {
      compileParentDeclaration(DNode(), scope);

      compileNestedVMT(node, scope);
   }
   return compileNestedExpression(node, ownerScope, scope, mode);
}

ObjectInfo Compiler :: compileCollection(DNode objectNode, CodeScope& scope, int mode)
{
   return compileCollection(objectNode, scope, mode, scope.moduleScope->arrayReference);
}

ObjectInfo Compiler :: compileCollection(DNode node, CodeScope& scope, int mode, ref_t vmtReference)
{
   int counter = 0;

   scope.writer->newBookmark();

   // all collection memebers should be created before the collection itself
   while (node != nsNone) {

      scope.writer->newNode(lxMember, counter);

      ObjectInfo current = compileExpression(node, scope, 0, mode);

      scope.writer->closeNode();

      node = node.nextNode();
      counter++;
   }

   scope.writer->insert(lxNested, counter);
   scope.writer->appendNode(lxTarget, vmtReference);
   scope.writer->closeNode();

   scope.writer->removeBookmark();

   return ObjectInfo(okObject);
}

ObjectInfo Compiler :: compileRetExpression(DNode node, CodeScope& scope, int mode)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   bool typecasting = false;
   ref_t subj = 0;
   if (test(mode, HINT_ROOT)) {
      // type cast returning value if required
      int paramCount;
      ref_t verb;
      decodeMessage(scope.getMessageID(), subj, verb, paramCount);
      if (verb == GET_MESSAGE_ID && paramCount == 0) {
         // if the class is compatible with the type
         // use alternative typecasting routine
         if (scope.moduleScope->checkIfCompatible(subj, scope.getClassRefId())) {
            scope.writer->newBookmark();
            typecasting = true;

            subj = 0;
         }
      }
      else if (classScope->info.methodHints.exist(Attribute(scope.getMessageID(), maType))) {
         subj = classScope->info.methodHints.get(Attribute(scope.getMessageID(), maType));
      }
      else subj = 0;

      // typecasting should be applied only for the strong type
      if (!scope.moduleScope->typeHints.exist(subj)) {
         subj = 0;
      }
   }

   ObjectInfo info = compileExpression(node, scope, subj, mode);

   if (typecasting) {
      // if the type class returns itself, no need to typecast the result
      if (info.kind != okThisParam) {
         scope.writer->insert(lxTypecasting, encodeMessage(subj, GET_MESSAGE_ID, 0));
      }
      scope.writer->removeBookmark();
   }

   scope.freeSpace();

   return ObjectInfo(okObject, 0, 0, subj);
}

ObjectInfo Compiler :: compileExpression(DNode node, CodeScope& scope, ref_t targetType, int mode)
{
   bool tryMode = false;
   bool altMode = false;

   if (findSymbol(node.firstChild(), nsCatchMessageOperation)) {
      scope.writer->newNode(lxTrying);
      tryMode = true;
   }
   else if (findSymbol(node.firstChild(), nsAltMessageOperation)) {
      // for alt mode the target object should be presaved
      scope.writer->newNode(lxExpression);
      scope.writer->newNode(lxVariable);
      compileExpression(node.firstChild(), scope, 0, 0);
      scope.writer->closeNode();

      scope.writer->newNode(lxAlt);
      altMode = true;
   }

   scope.writer->newBookmark();

   ObjectInfo objectInfo;
   if (node != nsObject) {
      DNode member = node.firstChild();

      if (member.nextNode() != nsNone) {
         if (member == nsObject) {
            if (!altMode) {
               objectInfo = compileObject(member, scope, mode);

               // skip boxing for assigning target
               // HOTFIX : the target should be boxed despite HINT_NOBOXING if it's followed by an operation
               if (!findSymbol(member, nsAssigning))
                  writeBoxing(node.FirstTerminal(), scope, objectInfo, 0, 0);
            }
            else scope.writer->appendNode(lxResult);
         }
         if (member != nsNone) {
            objectInfo = compileOperations(member, scope, objectInfo, mode);
         }
      }
      else objectInfo = compileObject(member, scope, mode);
   }
   else objectInfo = compileObject(node, scope, mode);

   writeBoxing(node.FirstTerminal(), scope, objectInfo, targetType, mode);

   if (targetType != 0) {
      if (!checkIfCompatible(scope, targetType, objectInfo)) {
         scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));

         appendCoordinate(scope.writer, node.FirstTerminal());

         scope.writer->closeNode();
      }
   }

   scope.writer->removeBookmark();

   if (tryMode || altMode) {
      scope.writer->closeNode(); // close try / alt

      if (altMode) {
         scope.writer->appendNode(lxReleasing, 1);
         scope.writer->closeNode(); // close expression
      }
   }

   return objectInfo;
}

ObjectInfo Compiler :: compileAssigningExpression(DNode node, DNode assigning, CodeScope& scope, ObjectInfo target, int mode)
{
   // if primitive data operation can be used
   if (target.kind == okLocalAddress || target.kind == okFieldAddress) {
      ObjectInfo info = compileExpression(assigning.firstChild(), scope, target.type, 0);
   }
   else {
      ref_t targetType = 0;
      if (target.kind == okLocal || target.kind == okField || target.kind == okOuterField) {
         if (target.type != 0) {
            targetType = target.type;
         }
      }
      else if (target.kind == okUnknown) {
         scope.raiseError(errUnknownObject, node.Terminal());
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());

      ObjectInfo info = compileExpression(assigning.firstChild(), scope, targetType, 0);
   }

   return ObjectInfo(okObject);
}

ObjectInfo Compiler :: compileBranching(DNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodeMode*/)
{
   CodeScope subScope(&scope);

   DNode thenCode = thenNode.firstChild();

   DNode expr = thenCode.firstChild();
   if (expr == nsCodeEnd || expr.nextNode() != nsNone) {
      compileCode(thenCode, subScope);

      if (subScope.level > scope.level) {
         scope.writer->appendNode(lxReleasing, subScope.level - scope.level);
      }
   }
   // if it is inline action
   else compileRetExpression(expr, scope, 0);

   return ObjectInfo(okObject);
}

void Compiler :: compileThrow(DNode node, CodeScope& scope, int mode)
{
   scope.writer->newNode(lxThrowing);

   ObjectInfo object = compileExpression(node.firstChild(), scope, 0, mode);

   scope.writer->closeNode();
}

void Compiler :: compileLoop(DNode node, CodeScope& scope)
{
   DNode expr = node.firstChild().firstChild();

   // if it is while-do loop
   if (expr.nextNode() == nsL7Operation) {
      scope.writer->newNode(lxLooping);

      DNode loopNode = expr.nextNode();

      ObjectInfo cond = compileExpression(expr, scope, scope.moduleScope->boolType, 0);

      int operator_id = _operators.get(loopNode.Terminal());

      scope.writer->newNode(lxIf, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);
      compileBranching(loopNode, scope/*, cond, _operators.get(loopNode.Terminal()), HINT_LOOP*/);
      scope.writer->closeNode();

      scope.writer->closeNode();
   }
   // if it is repeat loop
   else {
      scope.writer->newNode(lxLooping, scope.moduleScope->trueReference);

      ObjectInfo retVal = compileExpression(expr, scope, scope.moduleScope->boolType, 0);

      scope.writer->closeNode();
   }
}

void Compiler :: compileTry(DNode node, CodeScope& scope)
{
//   scope.writer->newNode(lxTrying);
//
//   // implement try expression
//   compileExpression(node.firstChild(), scope, 0, 0);
//
////   // implement finally block
////   _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
////   compileCode(goToSymbol(node.firstChild(), nsSubCode), scope);
////   _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));
//
//   DNode catchNode = goToSymbol(node.firstChild(), nsCatchMessageOperation);
//   if (catchNode != nsNone) {
//      scope.writer->newBookmark();
//
//      scope.writer->appendNode(lxResult);
//
//      // implement catch message
//      compileMessage(catchNode, scope, ObjectInfo(okObject));
//
//      scope.writer->removeBookmark();
//   }
////   // or throw the exception further
////   else _writer.throwCurrent(*scope.tape);
//
//   scope.writer->closeNode();
//
//   // implement finally block
//   compileCode(goToSymbol(node.firstChild(), nsSubCode), scope);
}

void Compiler :: compileLock(DNode node, CodeScope& scope)
{
   scope.writer->newNode(lxLocking);

   // implement the expression to be locked
   ObjectInfo object = compileExpression(node.firstChild(), scope, 0, 0);

   scope.writer->newNode(lxBody);

   // implement critical section
   compileCode(goToSymbol(node.firstChild(), nsSubCode), scope);

   scope.writer->closeNode();
   scope.writer->closeNode();
}

ObjectInfo Compiler :: compileCode(DNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   DNode statement = node.firstChild();

   // make a root bookmark for temporal variable allocating
   scope.rootBookmark = scope.writer->newBookmark();

   // skip subject argument
   while (statement == nsSubjectArg || statement == nsMethodParameter)
      statement= statement.nextNode();

   while (statement != nsNone) {
      DNode hints = skipHints(statement);

      //_writer.declareStatement(*scope.tape);

      switch(statement) {
         case nsExpression:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            scope.writer->newNode(lxExpression);
            compileExpression(statement, scope, 0, HINT_ROOT);
            scope.writer->closeNode();
            break;
         case nsThrow:
            compileThrow(statement, scope, 0);
            break;
         case nsLoop:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            //scope.writer->newNode(lxExpression);
            compileLoop(statement, scope);
            //scope.writer->closeNode();
            break;
         case nsTry:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileTry(statement, scope);
            break;
         case nsLock:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileLock(statement, scope);
            break;
         case nsRetStatement:
         {
            needVirtualEnd = false;
            recordDebugStep(scope, statement.firstChild().FirstTerminal(), dsStep);

            scope.writer->newNode(lxReturning);
            retVal = compileRetExpression(statement.firstChild(), scope, HINT_ROOT);
            scope.writer->closeNode();
            scope.freeSpace();

            break;
         }
         case nsVariable:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileVariable(statement, scope, hints);
            break;
         case nsCodeEnd:
            needVirtualEnd = false;
            recordDebugStep(scope, statement.Terminal(), dsEOP);
            break;
      }

      scope.freeSpace();

      statement = statement.nextNode();
   }

   if (needVirtualEnd) {
      recordDebugVirtualStep(scope, dsVirtualEnd);
   }

   scope.rootBookmark = -1;
   scope.writer->removeBookmark();

   return retVal;
}

void Compiler :: compileExternalArguments(DNode arg, CodeScope& scope/*, ExternalScope& externalScope*/)
{
   ModuleScope* moduleScope = scope.moduleScope;

   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();

      ref_t subject = moduleScope->mapType(terminal);
      ref_t classReference = moduleScope->typeHints.get(subject);
      int flags = 0;
      ClassInfo classInfo;
      if (moduleScope->loadClassInfo(classInfo, moduleScope->module->resolveReference(classReference), true) == 0)
         scope.raiseError(errInvalidOperation, terminal);

      if (classInfo.size == 0)
         scope.raiseError(errInvalidOperation, terminal);

      flags = classInfo.header.flags;

      LexicalType argType = lxNone;
      // if it is an integer number pass it directly
      switch (flags & elDebugMask) {
         case elDebugDWORD:
         case elDebugPTR:
            argType = test(flags, elReadOnlyRole) ? lxIntExtArgument : lxExtArgument;
            break;
         default:
            argType = lxExtArgument;
            break;
      }

      scope.writer->newNode(argType);

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
         ObjectInfo info = compileExpression(arg.firstChild(), scope, subject, 0);

         arg = arg.nextNode();
      }
      else scope.raiseError(errInvalidOperation, terminal);

      scope.writer->closeNode();
   }
}

ObjectInfo Compiler :: compileExternalCall(DNode node, CodeScope& scope, ident_t dllAlias, int mode)
{
   ObjectInfo retVal(okExternal);

   ModuleScope* moduleScope = scope.moduleScope;

   bool rootMode = test(mode, HINT_ROOT);
   bool stdCall = false;
   ident_t dllName = moduleScope->project->resolveExternalAlias(dllAlias + strlen(EXTERNAL_MODULE) + 1, stdCall);
   // legacy : if dll is not mapped, use the name directly
   if (emptystr(dllName))
      dllName = dllAlias + strlen(EXTERNAL_MODULE) + 1;

   ReferenceNs name(DLL_NAMESPACE);
   name.combine(dllName);
   name.append(".");
   name.append(node.Terminal());

   ref_t reference = moduleScope->module->mapReference(name);

   // save the operation result into temporal variable
   if (!rootMode) {
      scope.writer->newNode(lxAssigning, 4);

      allocateStructure(scope, 0, retVal);
      scope.writer->appendNode(lxLocalAddress, retVal.param);
   }

   scope.writer->newNode(stdCall ? lxStdExternalCall : lxExternalCall, reference);

   compileExternalArguments(node.firstChild(), scope);

   scope.writer->closeNode();

   if (!rootMode)
      scope.writer->closeNode(); // lxAssigning

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(DNode node, CodeScope& scope, ObjectInfo routine)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // only eval message is allowed
   TerminalInfo     verb = node.Terminal();
   if (_verbs.get(verb) != EVAL_MESSAGE_ID)
      scope.raiseError(errInvalidOperation, verb);

   scope.writer->newNode(lxInternalCall, routine.param);

   DNode arg = node.firstChild();

   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();
      ref_t type = moduleScope->mapType(terminal);

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
         compileExpression(arg.firstChild(), scope, type, 0);
      }
      else scope.raiseError(errInvalidOperation, terminal);

      arg = arg.nextNode();
   }

   scope.writer->closeNode();

   return ObjectInfo(okObject);
}

void Compiler :: reserveSpace(CodeScope& scope, int size)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // if it is not enough place to allocate
   // !! it should be refactored : code generation should start after the syntax tree is built
   if (methodScope->reserved < scope.reserved) {
      ByteCodeIterator allocStatement = methodScope->tape->find(bcOpen);
      // reserve place for stack allocated object
      (*allocStatement).argument += size;

      // if stack was not allocated before
      // update method enter code
      if (methodScope->reserved == 0) {
         // to include new frame header
         (*allocStatement).argument += 2;

         _writer.insertStackAlloc(allocStatement, *methodScope->tape, size);
      }
      // otherwise update the size
      else _writer.updateStackAlloc(allocStatement, size);

      methodScope->reserved += size;
   }
}

bool Compiler :: allocateStructure(CodeScope& scope, int dynamicSize, ObjectInfo& exprOperand/*, bool presavedAccumulator*/)
{
   bool bytearray = false;
   int size = 0;
   ref_t classReference = 0;
   if (exprOperand.kind == okObject && exprOperand.param != 0) {
      classReference = exprOperand.param;
      size = scope.moduleScope->defineStructSize(classReference);
   }
   else if (exprOperand.kind == okExternal && exprOperand.type == 0) {
      // typecast index to int if no type provided
      classReference = scope.moduleScope->intReference;
   }
   else size = scope.moduleScope->defineTypeSize(exprOperand.type, classReference);

   if (size < 0) {
      bytearray = true;

      // plus space for size
      size = ((dynamicSize + 3) >> 2) + 2;
   }
   else if (exprOperand.kind == okExternal) {
      size = 1;
   }
   else if (size == 0) {
      return false;
   }
   else size = (size + 3) >> 2;

   if (size > 0) {
      exprOperand.kind = okLocalAddress;
      exprOperand.param = scope.newSpace(size);
      exprOperand.extraparam = classReference;

      // allocate
      reserveSpace(scope, size);

      // reserve place for byte array header if required
      if (bytearray) {
         exprOperand.param -= 2;
      }

      return true;
   }
   else return false;
}

ref_t Compiler :: declareInlineArgumentList(DNode arg, MethodScope& scope)
{
   IdentifierString signature;

   ref_t sign_id = 0;

   // if method has generic (unnamed) argument list
   while (arg == nsMethodParameter || arg == nsObject) {
      TerminalInfo paramName = arg.Terminal();
      int index = 1 + scope.parameters.Count();
      scope.parameters.add(paramName, Parameter(index));

      arg = arg.nextNode();
   }
   bool first = true;
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.moduleScope->mapSubject(subject, signature);

      // declare method parameter
      arg = arg.nextNode();

      if (arg == nsMethodParameter) {
         // !! check duplicates
         if (scope.parameters.exist(arg.Terminal()))
            scope.raiseError(errDuplicatedLocal, arg.Terminal());

         int index = 1 + scope.parameters.Count();
         scope.parameters.add(arg.Terminal(), Parameter(index, subj_ref));

         arg = arg.nextNode();
      }
   }

   if (!emptystr(signature))
      sign_id = scope.moduleScope->module->mapSubject(signature, false);

   return encodeMessage(sign_id, EVAL_MESSAGE_ID, scope.parameters.Count());
}

void Compiler :: declareArgumentList(DNode node, MethodScope& scope, DNode hints)
{
   IdentifierString signature;
   ref_t verb_id = 0;
   ref_t sign_id = 0;
   bool first = true;

   TerminalInfo verb = node.Terminal();
   if (node != nsDefaultGeneric) {
	   verb_id = _verbs.get(verb.value);

	   // if it is a generic verb, make sure no parameters are provided
	   if (verb_id == DISPATCH_MESSAGE_ID) {
		   scope.raiseError(errInvalidOperation, verb);
	   }
	   else if (verb_id == 0) {
		   scope.moduleScope->mapSubject(verb, signature);
	   }
   }

   DNode arg = node.firstChild();
   if (verb_id == 0) {
      // if followed by argument list - it is a EVAL verb
      if (arg == nsSubjectArg || arg == nsMethodParameter) {
         verb_id = EVAL_MESSAGE_ID;
         first = false;
      }
      // otherwise it is GET message
      else verb_id = GET_MESSAGE_ID;
   }

   int paramCount = 0;
   // if method has generic (unnamed) argument list
   while (arg == nsMethodParameter) {
      int index = 1 + scope.parameters.Count();

      if (scope.parameters.exist(arg.Terminal()))
         scope.raiseError(errDuplicatedLocal, arg.Terminal());

      scope.parameters.add(arg.Terminal(), Parameter(index));
      paramCount++;

      arg = arg.nextNode();
   }

   // if method has named argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.moduleScope->mapSubject(subject, signature);

      arg = arg.nextNode();

      if (arg == nsMethodParameter) {
         if (scope.parameters.exist(arg.Terminal()))
            scope.raiseError(errDuplicatedLocal, arg.Terminal());

         int index = 1 + scope.parameters.Count();

         // if it is an open argument type
         if (scope.moduleScope->typeHints.exist(subj_ref, scope.moduleScope->paramsReference)) {
            scope.parameters.add(arg.Terminal(), Parameter(index, subj_ref));

            // the generic arguments should be free by the method exit
            scope.rootToFree += paramCount;
            scope.withOpenArg = true;

            // to indicate open argument list
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > 0xF)
               scope.raiseError(errNotApplicable, arg.Terminal());
         }
         else {
            paramCount++;
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            scope.parameters.add(arg.Terminal(), Parameter(index, subj_ref));

            arg = arg.nextNode();
         }
      }
   }

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      if (StringHelper::compare(terminal, HINT_GENERIC)) {
         if (!emptystr(signature))
            scope.raiseError(errInvalidHint, terminal);

         signature.copy(GENERIC_PREFIX);
      }

      hints = hints.nextNode();
   }

   // if signature is presented
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   scope.message = encodeMessage(sign_id, verb_id, paramCount);
}

void Compiler :: compileDispatcher(DNode node, MethodScope& scope, bool withGenericMethods)
{
   SyntaxWriter writer(&scope.syntaxTree);
   CodeScope codeScope(&scope, &writer);

   CommandTape* tape = scope.tape;

   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   _writer.declareIdleMethod(*tape, scope.message);

   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, tape, node.Terminal());
   }
   else {
      _writer.doGenericHandler(*tape);

      // if it is generic handler with redirect statement / redirect statement
      if (node != nsNone) {
         if (withGenericMethods) {
            _writer.pushObject(*tape, lxCurrentMessage);
            _writer.setSubject(*tape, encodeMessage(codeScope.moduleScope->mapSubject(GENERIC_PREFIX), 0, 0));
            _writer.doGenericHandler(*tape);
            _writer.popObject(*tape, lxCurrentMessage);
         }
         compileDispatchExpression(node, codeScope, tape);
      }
      // if it is generic handler only
      else if (withGenericMethods) {
         _writer.pushObject(*tape, lxCurrentMessage);
         _writer.setSubject(*tape, encodeMessage(codeScope.moduleScope->mapSubject(GENERIC_PREFIX), 0, 0));
         _writer.resendResolvedMethod(*tape, scope.moduleScope->superReference, encodeVerb(DISPATCH_MESSAGE_ID));
      }
   }

   _writer.endIdleMethod(*tape);
}

void Compiler :: compileActionMethod(DNode node, MethodScope& scope)
{
   // check if the method is inhreited and update vmt size accordingly
   if(scope.include() && test(scope.getClassFlag(), elClosed))
      scope.raiseError(errClosedParent, node.Terminal());

   SyntaxWriter writer(&scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot);

   CodeScope codeScope(&scope, &writer);

   // new stack frame
   // stack already contains previous $self value
   _writer.declareMethod(*scope.tape, scope.message, false);
   codeScope.level++;

   declareParameterDebugInfo(scope, false, true);

   if (isReturnExpression(node.firstChild())) {
      compileRetExpression(node.firstChild(), codeScope, HINT_ROOT);
   }
   else if (node == nsInlineExpression) {
      // !! this check should be removed, as it is no longer used
      compileCode(node.firstChild(), codeScope);
   }
   else compileCode(node, codeScope);

   // NOTE : close root node
   writer.closeNode();

   analizeSyntaxTree(&scope, scope.syntaxTree);
   _writer.generateTree(*scope.tape, scope.syntaxTree);

   _writer.endMethod(*scope.tape, scope.parameters.Count() + 1, scope.reserved);
}

void Compiler :: compileLazyExpressionMethod(DNode node, MethodScope& scope)
{
   CommandTape* tape = scope.tape;

   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   // stack already contains previous $self value
   SyntaxWriter writer(&scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot);

   CodeScope codeScope(&scope, &writer);

   // new stack frame
   // stack already contains previous $self value
   _writer.declareMethod(*tape, scope.message, false);
   codeScope.level++;

   declareParameterDebugInfo(scope, false, false);

   compileRetExpression(node, codeScope, 0);

   // NOTE : close root node
   writer.closeNode();

   analizeSyntaxTree(&scope, scope.syntaxTree);
   _writer.generateTree(*tape, scope.syntaxTree);

   _writer.endMethod(*tape, scope.parameters.Count() + 1, scope.reserved);
}

void Compiler :: compileDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   _writer.declareMethod(*tape, methodScope->message, false, false);

   // try to implement light-weight resend operation
   ObjectInfo target;
   if (node.firstChild() == nsNone && node.nextNode() == nsNone) {
      target = scope.mapObject(node.Terminal());
   }

   if (target.kind == okConstantSymbol || target.kind == okField) {
      if (target.kind == okField) {
         _writer.loadObject(*tape, lxResultField, target.param);
      }
      else _writer.loadObject(*tape, lxConstantSymbol, target.param);

      _writer.resend(*tape);
   }
   else {
      // NOTE : top expression is required for propery translation
      scope.writer->newNode(lxRoot);

      scope.writer->newNode(lxResending, methodScope->message);

      ObjectInfo target = compileExpression(node, scope, 0, 0);

      scope.writer->closeNode();

      // NOTE : close root node
      scope.writer->closeNode();

      analizeSyntaxTree(&scope, methodScope->syntaxTree);
      _writer.generateTree(*tape, methodScope->syntaxTree);
   }

   _writer.endMethod(*tape, getParamCount(methodScope->message) + 1, methodScope->reserved, false);
}

void Compiler :: compileConstructorResendExpression(DNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
{
   ModuleScope* moduleScope = scope.moduleScope;
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // find where the target constructor is declared in the current class
   size_t count = 0;
   ref_t messageRef = mapMessage(node, scope, count);
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

         if (moduleScope->checkMethod(info.classClassRef, messageRef) != tpUnknown) {
            classRef = info.classClassRef;
            found = true;

            break;
         }
         else parent = info.header.parentRef;
      }
   }
   if (found) {
      if (count != 0 && methodScope->parameters.Count() != 0) {
         withFrame = true;

         // new stack frame
         // stack already contains $self value
         _writer.newFrame(classClassScope.tape);
         scope.level++;
      }

      scope.writer->newBookmark();

      if (withFrame) {
         writeTerminal(TerminalInfo(), scope, ObjectInfo(okThisParam, 1));
         compileExtensionMessage(node, scope, ObjectInfo(okThisParam, 1), ObjectInfo(okConstantClass, 0, classRef));
      }
      else {
         writeTerminal(TerminalInfo(), scope, ObjectInfo(okObject));
         compileExtensionMessage(node, scope, ObjectInfo(okObject), ObjectInfo(okConstantClass, 0, classRef));

         // HOT FIX : save the created object
         scope.writer->newNode(lxAssigning);
         scope.writer->appendNode(lxCurrent, 1);
         scope.writer->appendNode(lxResult);
         scope.writer->closeNode();
      }

      scope.writer->removeBookmark();
   }
   else scope.raiseError(errUnknownMessage, node.Terminal());
}

void Compiler :: compileConstructorDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape)
{
   if (node.firstChild() == nsNone) {
      ObjectInfo info = scope.mapObject(node.Terminal());
      // if it is an internal routine
      if (info.kind == okInternal) {
         importCode(node, *scope.moduleScope, tape, node.Terminal());

         // NOTE : import code already contains quit command, so do not call "endMethod"
         _writer.endIdleMethod(*tape);
      }
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileResendExpression(DNode node, CodeScope& scope, CommandTape* tape)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // new stack frame
   // stack already contains current $self reference
   _writer.declareMethod(*tape, methodScope->message, false, true);
   scope.level++;

   scope.writer->newBookmark();
   writeTerminal(TerminalInfo(), scope, ObjectInfo(okThisParam, 1));

   compileMessage(node, scope, ObjectInfo(okThisParam, 1));
   scope.freeSpace();

   scope.writer->removeBookmark();
}

void Compiler :: compileImportCode(DNode node, CodeScope& codeScope, ref_t message, ident_t function, CommandTape* tape)
{
   _writer.declareIdleMethod(*tape, message);
   importCode(node, *codeScope.moduleScope, tape, function);
   _writer.endIdleMethod(*tape);
}

void Compiler :: compileMethod(DNode node, MethodScope& scope, bool genericMethod)
{
   int paramCount = getParamCount(scope.message);

   SyntaxWriter writer(&scope.syntaxTree);

   CodeScope codeScope(&scope, &writer);

   CommandTape* tape = scope.tape;

   // save extensions if any
   if (test(codeScope.getClassFlags(false), elExtension)) {
      codeScope.moduleScope->saveExtension(scope.message, codeScope.getExtensionType(), codeScope.getClassRefId());
   }

   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   // check if it is a resend
   if (resendBody != nsNone) {
      // NOTE : top expression is required for propery translation
      writer.newNode(lxRoot);

      compileResendExpression(resendBody.firstChild(), codeScope, tape);

      // NOTE : close root node
      writer.closeNode();

      analizeSyntaxTree(&scope, scope.syntaxTree);
      _writer.generateTree(*tape, scope.syntaxTree);

      _writer.endMethod(*tape, getParamCount(scope.message) + 1, scope.reserved, true);
   }
   // check if it is a dispatch
   else if (dispatchBody != nsNone) {
      if (isImportRedirect(dispatchBody.firstChild())) {
         compileImportCode(dispatchBody.firstChild(), codeScope, scope.message, dispatchBody.firstChild().Terminal(), tape);
      }
      else compileDispatchExpression(dispatchBody.firstChild(), codeScope, tape);
   }
   else {
      // NOTE : top expression is required for propery translation
      writer.newNode(lxRoot);

      // new stack frame
      // stack already contains current $self reference
      // the original message should be restored if it is a generic method
      _writer.declareMethod(*tape, scope.message, genericMethod);
      codeScope.level++;
      // declare the current subject for a generic method
      if (genericMethod) {
         _writer.saveSubject(*tape);
         codeScope.level++;
         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0);
      }

      declareParameterDebugInfo(scope, true, test(codeScope.getClassFlags(), elRole));

      DNode body = node.select(nsSubCode);
      // if method body is a returning expression
      if (body==nsNone) {
         compileCode(node, codeScope);
      }
      // if method body is a set of statements
      else {
         ObjectInfo retVal = compileCode(body, codeScope);

         // if the method returns itself
         if(retVal.kind == okUnknown) {
            ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
            ref_t typeHint = scope.getReturningType();

            if (typeHint != 0) {
               writer.newNode(lxTypecasting, encodeMessage(typeHint, GET_MESSAGE_ID, 0));
               writer.appendNode(lxLocal, 1);
               appendCoordinate(&writer, goToSymbol(body.firstChild(), nsCodeEnd).Terminal());
               writer.closeNode();
            }
            else writer.appendNode(lxLocal, 1);
         }
      }

      int stackToFree = paramCount + scope.rootToFree;

      // NOTE : close root node
      writer.closeNode();

      analizeSyntaxTree(&scope, scope.syntaxTree);
      if (scope.isEmbeddable()) {
         defineEmbeddableAttributes(scope, scope.syntaxTree);
      }
      _writer.generateTree(*tape, scope.syntaxTree);

      _writer.endMethod(*tape, stackToFree, scope.reserved);
   }
}

void Compiler :: compileEmbeddableConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope)
{
   ref_t originalSubjRef = getSignature(scope.message);

   IdentifierString signature(scope.moduleScope->module->resolveSubject(originalSubjRef));
   signature.append(EMBEDDED_PREFIX);

   ref_t embedddedMethodRef = overwriteSubject(scope.message, scope.moduleScope->module->mapSubject(signature, false));

   classClassScope.info.methodHints.add(Attribute(scope.message, maEmbeddedInit), getSignature(embedddedMethodRef));

   // compile an embedded constructor
   // HOTFIX: embedded constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;
   scope.message = embedddedMethodRef;
   scope.include();
   compileMethod(node, scope, false);

   // compile a constructor calling the embedded method
   scope.message = overwriteSubject(scope.message, originalSubjRef);
   compileConstructor(DNode(), scope, classClassScope, embedddedMethodRef);
}

void Compiler :: compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef)
{
   SyntaxWriter writer(&scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot);

   CodeScope codeScope(&scope, &writer);

   // HOTFIX: constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;

   DNode body = node.select(nsSubCode);
   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   _writer.declareMethod(classClassScope.tape, scope.message, false, false);

   bool withFrame = false;

   if (resendBody != nsNone) {
      compileConstructorResendExpression(resendBody.firstChild(), codeScope, classClassScope, withFrame);

      // HOTFIX : generate the code
      writer.closeNode();

      analizeSyntaxTree(&scope, scope.syntaxTree);
      _writer.generateTree(classClassScope.tape, scope.syntaxTree);

      // HOTFIX : clear writer and open the node once again
      writer.clear();
      writer.newNode(lxRoot);
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (!test(codeScope.getClassFlags(), elDynamicRole)) {
      // HOTFIX: -1 indicates the stack is not consumed by the constructor
      _writer.callMethod(classClassScope.tape, 1, -1);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else if (dispatchBody == nsNone)
      scope.raiseError(errIllegalConstructor, node.Terminal());

   if (dispatchBody != nsNone) {
      compileConstructorDispatchExpression(dispatchBody.firstChild(), codeScope, &classClassScope.tape);
      return;
   }
   // if the constructor has a body
   else if (body != nsNone) {
      if (!withFrame) {
         withFrame = true;

         // new stack frame
         // stack already contains $self value
         _writer.newFrame(classClassScope.tape);
         codeScope.level++;
      }
      else _writer.saveObject(classClassScope.tape, lxLocal, 1);

      declareParameterDebugInfo(scope, true, false);

      compileCode(body, codeScope);

      codeScope.writer->appendNode(lxLocal, 1);
   }
   // if the constructor should call embeddable method
   else if (embeddedMethodRef != 0) {
      _writer.saveObject(classClassScope.tape, lxCurrent, 1);
      _writer.resendResolvedMethod(classClassScope.tape, classClassScope.reference, embeddedMethodRef);
   }

   // NOTE : close root node
   writer.closeNode();

   analizeSyntaxTree(&scope, scope.syntaxTree);
   _writer.generateTree(classClassScope.tape, scope.syntaxTree);

   _writer.endMethod(classClassScope.tape, getParamCount(scope.message) + 1, scope.reserved, withFrame);
}

void Compiler :: compileDefaultConstructor(MethodScope& scope, ClassScope& classClassScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   // HOTFIX: constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;

   _writer.declareIdleMethod(classClassScope.tape, scope.message);

   if (test(classScope->info.header.flags, elStructureRole)) {
      if (!test(classScope->info.header.flags, elDynamicRole)) {
         _writer.newStructure(classClassScope.tape, classScope->info.size, classScope->reference);
      }
   }
   else if (!test(classScope->info.header.flags, elDynamicRole)) {
      _writer.newObject(classClassScope.tape, classScope->info.fields.Count(), classScope->reference);
      size_t fieldCount = classScope->info.fields.Count();
      if (fieldCount > 0) {
         _writer.initObject(classClassScope.tape, fieldCount, lxNil);
      }
   }

   _writer.exitMethod(classClassScope.tape, 0, 0, false);

   _writer.endIdleMethod(classClassScope.tape);
}

void Compiler :: compileDynamicDefaultConstructor(MethodScope& scope, ClassScope& classClassScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   // HOTFIX: constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;

   _writer.declareIdleMethod(classClassScope.tape, scope.message);

   if (test(classScope->info.header.flags, elStructureRole)) {
      _writer.loadObject(classClassScope.tape, lxConstantClass, classScope->reference);
      switch(classScope->info.size) {
         case -1:
            _writer.newDynamicStructure(classClassScope.tape, 1);
            break;
         case -2:
            _writer.newDynamicWStructure(classClassScope.tape);
            break;
         case -4:
            _writer.newDynamicNStructure(classClassScope.tape);
            break;
         default:
            _writer.newDynamicStructure(classClassScope.tape, -((int)classScope->info.size));
            break;
      }
   }
   else {
      _writer.loadObject(classClassScope.tape, lxConstantClass, classScope->reference);
      _writer.newDynamicObject(classClassScope.tape);
   }

   _writer.exitMethod(classClassScope.tape, 0, 0, false);

   _writer.endIdleMethod(classClassScope.tape);
}

void Compiler :: compileVMT(DNode member, ClassScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      switch(member) {
         case nsMethod:
         {
            MethodScope methodScope(&scope);
            methodScope.compileWarningHints(hints);

            // if it is a dispatch handler
            if (member.firstChild() == nsDispatchHandler) {
               if (test(scope.info.header.flags, elRole))
                  scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
               methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);

               compileDispatcher(member.firstChild().firstChild(), methodScope, test(scope.info.header.flags, elWithGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(member, methodScope, hints);

               int hint = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
               methodScope.stackSafe = test(hint, tpStackSafe);

               compileMethod(member, methodScope, test(hint, tpGeneric));
            }
            break;
         }
         case nsDefaultGeneric:
         {
            MethodScope methodScope(&scope);
            declareArgumentList(member, methodScope, hints);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->mapSubject(GENERIC_PREFIX));

            // mark as having generic methods
            scope.info.header.flags |= elWithGenerics;

            compileMethod(member, methodScope, true);
            break;
         }
      }
      member = member.nextNode();
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (test(scope.info.header.flags, elWithGenerics) && scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), false)) {
      MethodScope methodScope(&scope);
      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);

      compileDispatcher(DNode(), methodScope, true);
   }
}

void Compiler :: compileFieldDeclarations(DNode& member, ClassScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member==nsField) {
         // a role cannot have fields
         if (test(scope.info.header.flags, elStateless))
            scope.raiseError(errIllegalField, member.Terminal());

         // a class with a dynamic length structure must have no fields
         if (test(scope.info.header.flags, elDynamicRole))
            scope.raiseError(errIllegalField, member.Terminal());

         if (scope.info.fields.exist(member.Terminal()))
            scope.raiseError(errDuplicatedField, member.Terminal());

         int sizeValue = 0;
         ref_t typeRef = 0;
         scope.compileFieldHints(hints, sizeValue, typeRef);

         // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
         if (test(scope.info.header.flags, elStructureRole) && !findSymbol(member.nextNode(), nsField)
            && test(scope.info.header.flags, elSealed) && sizeValue != 0 && scope.info.fields.Count() == 0)
         {
            scope.info.header.flags |= elStructureWrapper;
            scope.info.size = sizeValue;

            if (sizeValue < 0) {
                scope.info.header.flags |= elDynamicRole;
            }

            scope.info.fields.add(member.Terminal(), 0);
            scope.info.fieldTypes.add(0, typeRef);
         }
         // if it is a structure field
         else if (test(scope.info.header.flags, elStructureRole)) {
            if (sizeValue <= 0)
               scope.raiseError(errIllegalField, member.Terminal());

            if (scope.info.size != 0 && scope.info.fields.Count() == 0)
               scope.raiseError(errIllegalField, member.Terminal());

            int offset = scope.info.size;
            scope.info.size += sizeValue;

            scope.info.fields.add(member.Terminal(), offset);
            scope.info.fieldTypes.add(offset, typeRef);
         }
         // if it is a normal field
         else {
            scope.info.header.flags |= elNonStructureRole;

            int offset = scope.info.fields.Count();
            scope.info.fields.add(member.Terminal(), offset);

            if (typeRef != 0)
               scope.info.fieldTypes.add(offset, typeRef);

            // byref variable may have only one field
            if (test(scope.info.header.flags, elWrapper)) {
               if (scope.info.fields.Count() > 1)
                  scope.raiseError(errIllegalField, member.Terminal());
            }
         }
      }
      else {
         // due to current syntax we need to reset hints back, otherwise they will be skipped
         if (hints != nsNone)
            member = hints;

         break;
      }
      member = member.nextNode();
   }

   // mark the class as a wrapper if it is appropriate
   if (test(scope.info.header.flags, elStructureRole | elSealed | elEmbeddable) && (scope.info.fieldTypes.Count() == 1) && scope.info.size > 0) {
      int type = scope.info.header.flags & elDebugMask;
      int fieldType = scope.moduleScope->getClassFlags(scope.moduleScope->typeHints.get(*scope.info.fieldTypes.start())) & elDebugMask;
      if (type == 0 && fieldType != 0) {
         scope.info.header.flags |= elStructureWrapper;
         scope.info.header.flags |= fieldType;
      }
   }
}

void Compiler :: compileSymbolCode(ClassScope& scope)
{
   CommandTape tape;

   // creates implicit symbol
   SymbolScope symbolScope(scope.moduleScope, scope.reference);

   _writer.declareSymbol(tape, symbolScope.reference);
   _writer.loadObject(tape, lxConstantClass, scope.reference);
   _writer.endSymbol(tape);

   // create byte code sections
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   // if no construtors are defined inherits the parent one
   if (!findSymbol(node.firstChild(), nsConstructor)) {
      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.info.header.parentRef));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }

   InheritResult res = inheritClass(classClassScope, classClassScope.info.header.parentRef, false);
   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   if (res == irInvalid) {
      classClassScope.raiseError(errInvalidParent, node.Terminal());
   }
   else if (res == irSealed) {
      classClassScope.raiseError(errSealedParent, node.Terminal());
   }
   else if (res == irUnsuccessfull)
      classClassScope.raiseError(node != nsNone ? errUnknownClass : errUnknownBaseClass, node.Terminal());

   // class class is always stateless
   classClassScope.info.header.flags |= elStateless;

   DNode member = node.firstChild();
   declareVMT(member, classClassScope, nsConstructor, false);

   // add virtual constructor
   MethodScope defaultScope(&classClassScope);
   defaultScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);
   defaultScope.include();

   // save declaration
   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(DNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   _writer.declareClass(classClassScope.tape, classClassScope.reference);

   DNode member = node.firstChild();
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member == nsConstructor) {
         MethodScope methodScope(&classScope);

         declareArgumentList(member, methodScope, hints);
         int hint = classClassScope.info.methodHints.get(Attribute(methodScope.message, maHint));
         methodScope.stackSafe = test(hint, tpStackSafe);

         // if the constructor is stack safe, embeddable and the class is an embeddable structure
         // the special method should be compiled
         if (methodScope.stackSafe && test(hint, tpEmbeddable) && test(classScope.info.header.flags, elStructureRole | elEmbeddable)) {
            // make sure the constructor has no redirect / dispatch statements
            if (node.select(nsResendExpression) != nsNone || node.select(nsDispatchExpression))
               methodScope.raiseError(errInvalidOperation, member.Terminal());

            compileEmbeddableConstructor(member, methodScope, classClassScope);
         }
         else compileConstructor(member, methodScope, classClassScope);
      }
      member = member.nextNode();
   }

   // create a virtual constructor
   MethodScope methodScope(&classScope);
   methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

   if (test(classScope.info.header.flags, elDynamicRole)) {
      compileDynamicDefaultConstructor(methodScope, classClassScope);
   }
   else compileDefaultConstructor(methodScope, classClassScope);

   _writer.endClass(classClassScope.tape);

   // optimize
   optimizeTape(classClassScope.tape);

   // create byte code sections
   classClassScope.save();
   _writer.save(classClassScope.tape, classClassScope.moduleScope->module, classClassScope.moduleScope->debugModule, classClassScope.moduleScope->sourcePathRef);
}

void Compiler :: declareVMT(DNode member, ClassScope& scope, Symbol methodSymbol, bool closed)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member == methodSymbol || member == nsDefaultGeneric) {
         MethodScope methodScope(&scope);

         if (member.firstChild() == nsDispatchHandler) {
            methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
         }
         else if (member == nsDefaultGeneric) {
            declareArgumentList(member, methodScope, hints);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->mapSubject(GENERIC_PREFIX));

            // mark as having generic methods
            scope.info.header.flags |= elWithGenerics;
         }
         else declareArgumentList(member, methodScope, hints);

         methodScope.compileHints(hints);

         // check if there is no duplicate method
         if (scope.info.methods.exist(methodScope.message, true))
            scope.raiseError(errDuplicatedMethod, member.Terminal());

         bool included = methodScope.include();
         bool sealedMethod = methodScope.isSealed();
         // if the class is closed, no new methods can be declared
         if (included && closed)
            scope.raiseError(errClosedParent, member.Terminal());

         // if the method is sealed, it cannot be overridden
         if (!included && sealedMethod)
            scope.raiseError(errClosedMethod, member.Terminal());

      }
      member = member.nextNode();
   }
}

void Compiler :: compileClassDeclaration(DNode node, ClassScope& scope, DNode hints)
{
   DNode member = node.firstChild();
   if (member==nsBaseClass) {
      compileParentDeclaration(member, scope);

      member = member.nextNode();
   }
   else compileParentDeclaration(DNode(), scope);

   int flagCopy = scope.info.header.flags;
   scope.compileClassHints(hints);

   compileFieldDeclarations(member, scope);

   declareVMT(member, scope, nsMethod, test(flagCopy, elClosed));

   // if it is a role
   if (test(scope.info.header.flags, elRole)) {
      // class is its own class class
      scope.info.classClassRef = scope.reference;
   }
   else {
      // define class class name
      IdentifierString classClassName(scope.moduleScope->module->resolveReference(scope.reference));
      classClassName.append(CLASSCLASS_POSTFIX);

      scope.info.classClassRef = scope.moduleScope->module->mapReference(classClassName);
   }

   // save declaration
   scope.save();
}

void Compiler :: compileClassImplementation(DNode node, ClassScope& scope)
{
   _writer.declareClass(scope.tape, scope.reference);

   DNode member = node.firstChild();
   compileVMT(member, scope);

   _writer.endClass(scope.tape);

   // compile explicit symbol
   compileSymbolCode(scope);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save();
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: declareSingletonClass(DNode node, ClassScope& scope, bool closed)
{
   // nested class is sealed if it has no parent
   if (!test(scope.info.header.flags, elClosed))
      scope.info.header.flags |= elSealed;

   // singleton is always stateless
   scope.info.header.flags |= elStateless;

   declareVMT(node.firstChild(), scope, nsMethod, closed);

   scope.save();
}

void Compiler :: declareSingletonAction(ClassScope& scope, ActionScope& methodScope)
{
   // singleton is always stateless
   scope.info.header.flags |= elStateless;

   methodScope.include();

   scope.save();
}

void Compiler::compileSingletonClass(DNode node, ClassScope& scope)
{
   _writer.declareClass(scope.tape, scope.reference);

   DNode member = node.firstChild();

   compileVMT(member, scope);

   _writer.endClass(scope.tape);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints)
{
   bool singleton = false;

   scope.compileHints(hints);

   DNode expression = node.firstChild();
   // if it is a singleton
   if (isSingleStatement(expression)) {
      DNode objNode = expression.firstChild().firstChild();
      if (objNode == nsNestedClass) {
         DNode classNode = expression.firstChild();

         ClassScope classScope(scope.moduleScope, scope.reference);

         if (classNode.Terminal() != nsNone) {
            // inherit parent
            compileParentDeclaration(classNode, classScope);
            if (classScope.info.fields.Count() > 0 || testany(classScope.info.header.flags, elStructureRole | elDynamicRole))
               scope.raiseError(errInvalidSymbolExpr, expression.Terminal());

            declareSingletonClass(classNode.firstChild(), classScope, test(classScope.info.header.flags, elClosed));
            singleton = true;
         }
         // if it is normal nested class
         else {
            classScope.info.header.flags |= elSealed;

            compileParentDeclaration(DNode(), classScope);

            declareSingletonClass(classNode.firstChild(), classScope, false);
            singleton = true;
         }
      }
      else if (objNode == nsSubCode) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         ActionScope methodScope(&classScope);

         declareActionScope(objNode, classScope, DNode(), methodScope, false);

         declareSingletonAction(classScope, methodScope);
         singleton = true;
      }
      else if (objNode == nsInlineExpression) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         ActionScope methodScope(&classScope);

         declareActionScope(objNode, classScope, expression.firstChild(), methodScope, false);

         declareSingletonAction(classScope, methodScope);
         singleton = true;
      }
      else if (objNode == nsSubjectArg || objNode == nsMethodParameter) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         ActionScope methodScope(&classScope);

         declareActionScope(objNode, classScope, objNode, methodScope, false);

         declareSingletonAction(classScope, methodScope);
         singleton = true;
      }
   }

   if (!singleton && (scope.typeRef != 0 || scope.constant)) {
      SymbolExpressionInfo info;
      info.expressionTypeRef = scope.typeRef;
      info.constant = scope.constant;

      // save class meta data
      MemoryWriter metaWriter(scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, false), 0);
      info.save(&metaWriter);
   }
}

void Compiler :: compileSymbolImplementation(DNode node, SymbolScope& scope, DNode hints, bool isStatic)
{
   scope.compileHints(hints);

   ObjectInfo retVal;
   DNode expression = node.firstChild();
   // if it is a singleton
   if (isSingleStatement(expression)) {
      DNode classNode = expression.firstChild().firstChild();
      if (classNode == nsNestedClass) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         if (classNode.Terminal() != nsNone) {
            compileSingletonClass(classNode.firstChild(), classScope);
         }
         // if it is normal nested class
         else compileSingletonClass(classNode, classScope);

         if (test(classScope.info.header.flags, elStateless)) {
            // if it is a stateless singleton
            retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
         }
      }
      else if (classNode == nsSubCode) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(classNode, classScope, DNode(), true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
      else if (classNode == nsInlineExpression) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(classNode, classScope, expression.firstChild(), true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
      else if (classNode == nsSubjectArg || classNode == nsMethodParameter) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(goToSymbol(classNode, nsInlineExpression), classScope, classNode, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
   }

   // compile symbol into byte codes
   if (isStatic) {
      _writer.declareStaticSymbol(scope.tape, scope.reference);
   }
   else _writer.declareSymbol(scope.tape, scope.reference);

   SyntaxWriter writer(&scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot);

   CodeScope codeScope(&scope, &writer);
   if (retVal.kind == okUnknown) {
      // compile symbol body, if it is not a singleton
      recordDebugStep(codeScope, expression.FirstTerminal(), dsStep);
      writer.newNode(lxExpression);
      retVal = compileExpression(expression, codeScope, scope.typeRef, 0);
      writer.closeNode();
   }
   else writeTerminal(node.FirstTerminal(), codeScope, retVal);

   // create constant if required
   if (scope.constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression.FirstTerminal());

      // expression cannot be constant
      if (retVal.kind == okObject)
         scope.raiseError(errInvalidOperation, expression.FirstTerminal());

      if (retVal.kind == okIntConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         size_t value = StringHelper::strToULong(module->resolveConstant(retVal.param), 16);

         dataWriter.writeDWord(value);

         dataWriter.Memory()->addReference(scope.moduleScope->intReference | mskVMTRef, (ref_t) - 4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->intReference);
      }
      else if (retVal.kind == okLongConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         long value = StringHelper::strToLongLong(module->resolveConstant(retVal.param) + 1, 10);

         dataWriter.write(&value, 8);

         dataWriter.Memory()->addReference(scope.moduleScope->longReference | mskVMTRef, (ref_t)-4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->longReference);
      }
      else if (retVal.kind == okRealConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         double value = StringHelper::strToDouble(module->resolveConstant(retVal.param));

         dataWriter.write(&value, 8);

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
      else if (retVal.kind == okCharConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         ident_t value = module->resolveConstant(retVal.param);

         dataWriter.writeLiteral(value, getlength(value));

         dataWriter.Memory()->addReference(scope.moduleScope->charReference | mskVMTRef, (ref_t)-4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->charReference);
      }
      else scope.raiseError(errInvalidOperation, expression.FirstTerminal());
   }

   // NOTE : close root node
   writer.closeNode();

   analizeSyntaxTree(&scope, scope.syntaxTree);
   _writer.generateTree(scope.tape, scope.syntaxTree);

   if (isStatic) {
      _writer.endStaticSymbol(scope.tape, scope.reference);
   }
   else _writer.endSymbol(scope.tape);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: optimizeExtCall(ModuleScope& scope, SyntaxTree::Node node)
{
   SyntaxTree::Node arg = node.firstChild();
   while (arg != lxNone) {
      if (arg == lxIntExtArgument || arg == lxExtArgument) {
         SyntaxTree::Node member = arg.firstChild();
         while (member != lxNone) {
            // if boxing used for external call
            // remove it
            if (member == lxBoxing || member == lxCondBoxing || member == lxUnboxing) {
               member = lxExpression;
            }

            member = member.nextNode();
         }
      }
      arg = arg.nextNode();
   }
}

void Compiler :: optimizeInternalCall(ModuleScope& scope, SyntaxTree::Node node)
{
   SyntaxTree::Node arg = node.firstChild();
   while (arg != lxNone) {
      // if boxing used for external call
      // remove it
      if (arg == lxBoxing || arg == lxCondBoxing || arg == lxUnboxing) {
         arg = lxExpression;
      }

      arg = arg.nextNode();
   }
}

void Compiler :: optimizeDirectCall(ModuleScope& scope, SyntaxTree::Node node)
{
   bool stackSafe = SyntaxTree::existChild(node, lxStacksafe);
   if (stackSafe) {
      SyntaxTree::Node member = node.firstChild();
      while (member != lxNone) {
         // if boxing used for direct stack safe call
         // remove it
         if (member == lxBoxing || member == lxCondBoxing || member == lxArgBoxing || member == lxUnboxing) {
            member = lxExpression;
         }

         member = member.nextNode();
      }
   }
}

void Compiler :: optimizeOp(ModuleScope& scope, SyntaxTree::Node node)
{
   SyntaxTree::Node member = node.firstChild();
   while (member != lxNone) {
      // if boxing used for primitive operation
      // remove it
      if (member == lxBoxing || member == lxCondBoxing || member == lxArgBoxing || member == lxUnboxing) {
         member = lxExpression;
      }

      member = member.nextNode();
   }
}

void Compiler::optimizeEmbeddableCall(ModuleScope& scope, SyntaxTree::Node& assignNode, SyntaxTree::Node& callNode)
{
   SyntaxTree::Node callTarget = SyntaxTree::findChild(callNode, lxTarget);

   ClassInfo info;
   scope.loadClassInfo(info, callTarget.argument);

   ref_t subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddableGet));
   // if it is possible to replace get&subject operation with eval&subject2:local
   if (subject != 0) {
      // removing assinging operation
      assignNode = lxExpression;

      // move assigning target into the call node
      SyntaxTree::Node assignTarget = assignNode.findPattern(SyntaxTree::NodePattern(lxLocalAddress));
      callNode.appendNode(assignTarget.type, assignTarget.argument);
      assignTarget = lxExpression;
      callNode.setArgument(encodeMessage(subject, EVAL_MESSAGE_ID, 1));
   }

   subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddedInit));
   // if it is possible to replace constructor call with embeddaded initialization without creating a temporal dynamic object
   if (subject != 0) {
      // move assigning target into the call node
      SyntaxTree::Node assignTarget = assignNode.findPattern(SyntaxTree::NodePattern(lxLocalAddress));
      SyntaxTree::Node callTarget = callNode.findPattern(SyntaxTree::NodePattern(lxConstantClass));
      if (callTarget != lxNone) {
         // removing assinging operation
         assignNode = lxExpression;

         // move assigning target into the call node
         callTarget = assignTarget.type;
         callTarget.setArgument(assignTarget.argument);
         assignTarget = lxExpression;

         callNode.setArgument(overwriteSubject(callNode.argument, subject));
      }
   }
}

void Compiler :: optimizeAssigning(ModuleScope& scope, SyntaxTree::Node node)
{
   // assigning (local address boxing) => assigning (local address expression)
   if (node.argument != 0) {
      SyntaxTree::Node boxing = SyntaxTree::findChild(node, lxBoxing, lxCondBoxing, lxUnboxing);
      if (boxing != lxNone && boxing.argument == node.argument) {
         boxing = lxExpression;
      }

      SyntaxTree::Node directCall = SyntaxTree::findChild(node, lxDirectCalling);
      if (directCall != lxNone && SyntaxTree::existChild(directCall, lxEmbeddable)) {
         optimizeEmbeddableCall(scope, node, directCall);
      }
   }
}

void Compiler :: analizeBoxing(Scope* scope, SyntaxTree::Node node)
{
   SyntaxTree::Node row = SyntaxTree::findChild(node, lxRow);
   SyntaxTree::Node col = SyntaxTree::findChild(node, lxCol);
   if (col != lxNone && row != lxNone) {
      scope->raiseWarning(WARNING_LEVEL_3, wrnBoxingCheck, row.argument, col.argument);
   }
}

void Compiler :: analizeTypecast(Scope* scope, SyntaxTree::Node node)
{
   SyntaxTree::Node row = SyntaxTree::findChild(node, lxRow);
   SyntaxTree::Node col = SyntaxTree::findChild(node, lxCol);
   if (col != lxNone && row != lxNone)
      scope->raiseWarning(WARNING_LEVEL_2, wrnTypeMismatch, row.argument, col.argument);
}

void Compiler :: analizeSyntaxExpression(Scope* scope, SyntaxTree::Node node)
{
   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      switch (current.type)
      {
         case lxAssigning:
            optimizeAssigning(*scope->moduleScope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxTypecasting:
            analizeTypecast(scope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxBoxing:
         case lxCondBoxing:
         case lxArgBoxing:
         case lxUnboxing:
            analizeBoxing(scope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxDirectCalling:
         case lxSDirctCalling:
            optimizeDirectCall(*scope->moduleScope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxIntOp:
         case lxLongOp:
         case lxRealOp:
         case lxIntArrOp:
         case lxArrOp:
            optimizeOp(*scope->moduleScope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxStdExternalCall:
         case lxExternalCall:
            optimizeExtCall(*scope->moduleScope, current);
            analizeSyntaxExpression(scope, current);
            break;
         case lxInternalCall:
            optimizeInternalCall(*scope->moduleScope, current);
            analizeSyntaxExpression(scope, current);
            break;
         default:
            if (test(current.type, lxExpressionMask)/* && current.nextNode() != lxAssigning*/) {
               analizeSyntaxExpression(scope, current);
            }
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: analizeSyntaxTree(Scope* scope, MemoryDump& dump)
{
   if (!test(_optFlag, 1))
      return;

   SyntaxTree reader(&dump);

   analizeSyntaxExpression(scope, reader.readRoot());
}

bool Compiler :: recognizeEmbeddableGet(MethodScope& scope, SyntaxTree& tree, SyntaxTree::Node root, ref_t& subject)
{
   ref_t returnType = scope.getReturningType();
   if (returnType != 0 && scope.moduleScope->defineTypeSize(returnType) > 0) {
      if (tree.matchPattern(root, lxObjectMask, 2, SyntaxTree::NodePattern(lxExpression), SyntaxTree::NodePattern(lxReturning))) {
         SyntaxTree::Node message = tree.findPattern(root, 2,
            SyntaxTree::NodePattern(lxExpression),
            SyntaxTree::NodePattern(lxDirectCalling, lxSDirctCalling));

         // if it is eval&subject2:var[1] message
         if (getParamCount(message.argument) != 1)
            return false;

         // check if it is operation with $self
         SyntaxTree::Node target = tree.findPattern(root, 3,
            SyntaxTree::NodePattern(lxExpression),
            SyntaxTree::NodePattern(lxDirectCalling, lxSDirctCalling),
            SyntaxTree::NodePattern(lxLocal));

         if (target == lxNone || target.argument != 1)
            return false;

         // check if the argument is returned
         SyntaxTree::Node arg = tree.findPattern(root, 4,
            SyntaxTree::NodePattern(lxExpression),
            SyntaxTree::NodePattern(lxDirectCalling, lxSDirctCalling),
            SyntaxTree::NodePattern(lxExpression),
            SyntaxTree::NodePattern(lxLocalAddress));

         SyntaxTree::Node ret = tree.findPattern(root, 3,
            SyntaxTree::NodePattern(lxReturning),
            SyntaxTree::NodePattern(lxBoxing),
            SyntaxTree::NodePattern(lxLocalAddress));

         if (arg != lxNone && ret != lxNone && arg.argument == ret.argument) {
            subject = getSignature(message.argument);

            return true;
         }
      }
   }

   return false;
}

void Compiler :: defineEmbeddableAttributes(MethodScope& scope, MemoryDump& dump)
{
   SyntaxTree tree(&dump);
   SyntaxTree::Node root = tree.readRoot();

   ClassScope* classScope = ((ClassScope*)scope.parent);

   // Optimization : var = get&subject => eval&subject2:var[1]
   ref_t type = 0;
   if (recognizeEmbeddableGet(scope, tree, root, type)) {
      classScope->info.methodHints.add(Attribute(scope.message, maEmbeddableGet), type);
   }
}

void Compiler :: compileIncludeModule(DNode node, ModuleScope& scope/*, DNode hints*/)
{
//   if (hints != nsNone)
//      scope.raiseWarning(1, wrnUnknownHint, hints.Terminal());

   TerminalInfo ns = node.Terminal();

   // check if the module exists
   _Module* module = scope.project->loadModule(ns, true);
   if (!module)
      scope.raiseWarning(wrnUnknownModule, ns);

   ident_t value = retrieve(scope.defaultNs.start(), ns, NULL);
   if (value == NULL) {
      scope.defaultNs.add(ns.value);

      // load types
      scope.loadTypes(module);

      // load extensions
      scope.loadExtensions(ns, module);
   }
}

void Compiler :: compileType(DNode& member, ModuleScope& scope, DNode hints)
{
   bool internalType = member.Terminal().symbol == tsPrivate;

   // map a full type name
   ref_t typeRef = scope.mapNewType(member.Terminal());

   bool  weak = true;
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (StringHelper::compare(terminal, HINT_WRAPPER)) {
         weak = false;

         TerminalInfo roleValue = hints.select(nsHintValue).Terminal();
         ref_t classRef = scope.mapTerminal(roleValue);

         scope.validateReference(roleValue, classRef);

         scope.saveType(typeRef, classRef, internalType);
      }
      else scope.raiseWarning(wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }

   if (weak)
      scope.saveType(typeRef, 0, internalType);
}

void Compiler::compileDeclarations(DNode member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      TerminalInfo name = member.Terminal();

      switch (member) {
         case nsType:
            compileType(member, scope, hints);
            break;
         case nsClass:
         {
            ref_t reference = scope.mapTerminal(name);

            // check for duplicate declaration
            if (scope.module->mapSection(reference | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(reference | mskSymbolRef, false);

            // compile class
            ClassScope classScope(&scope, reference);
            compileClassDeclaration(member, classScope, hints);

            // compile class class if it available
            if (classScope.info.classClassRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.classClassRef);
               compileClassClassDeclaration(member, classClassScope, classScope);
            }

            break;
         }
         case nsSymbol:
         case nsStatic:
         {
            ref_t reference = scope.mapTerminal(name);

            // check for duplicate declaration
            if (scope.module->mapSection(reference | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(reference | mskSymbolRef, false);

            SymbolScope symbolScope(&scope, reference);
            compileSymbolDeclaration(member, symbolScope, hints);
            break;
         }
      }
      member = member.nextNode();
   }
}

void Compiler::compileImplementations(DNode member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      TerminalInfo name = member.Terminal();

      switch (member) {
         case nsClass:
         {
            ref_t reference = scope.mapTerminal(name);

            // compile class
            ClassScope classScope(&scope, reference);
            scope.loadClassInfo(classScope.info, scope.module->resolveReference(reference), false);
            compileClassImplementation(member, classScope);

            // compile class class if it available
            if (classScope.info.classClassRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.classClassRef);
               scope.loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);

               compileClassClassImplementation(member, classClassScope, classScope);
            }
            break;
         }
         case nsSymbol:
         case nsStatic:
         {
            ref_t reference = scope.mapTerminal(name);

            SymbolScope symbolScope(&scope, reference);
            compileSymbolImplementation(member, symbolScope, hints, (member == nsStatic));
            break;
         }
      }
      member = member.nextNode();
   }
}

void Compiler :: compileIncludeSection(DNode& member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      switch (member) {
         case nsInclude:
            // NOTE: obsolete, used for backward compatibility
            //       should be removed in 2.1.x
            compileIncludeModule(member, scope/*, hints*/);
            break;
         case nsImport:
            compileIncludeModule(member, scope/*, hints*/);
            break;
         default:
            // due to current syntax we need to reset hints back, otherwise they will be skipped
            if (hints != nsNone)
               member = hints;

            return;
      }
      member = member.nextNode();
   }
}

void Compiler :: compileModule(DNode node, ModuleScope& scope)
{
   DNode member = node.firstChild();

   compileIncludeSection(member, scope);

   // first pass - declaration
   compileDeclarations(member, scope);

   // second pass - implementation
   compileImplementations(member, scope);
}

bool Compiler :: validate(Project& project, _Module* module, int reference)
{
   int   mask = reference & mskAnyRef;
   ref_t extReference = 0;
   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
   _Module* extModule = project.resolveModule(refName, extReference, true);

   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
}

void Compiler :: validateUnresolved(Unresolveds& unresolveds, Project& project)
{
   for (List<Unresolved>::Iterator it = unresolveds.start() ; !it.Eof() ; it++) {
      if (!validate(project, (*it).module, (*it).reference)) {
         ident_t refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);

         project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
      }
   }
}

void Compiler :: compile(ident_t source, MemoryDump* buffer, ModuleScope& scope)
{
   Path path;
   Path::loadPath(path, source);

   // parse
   TextFileReader sourceFile(path, scope.project->getDefaultEncoding(), true);
   if (!sourceFile.isOpened())
      scope.project->raiseError(errInvalidFile, source);

   buffer->clear();
   MemoryWriter bufWriter(buffer);
   DerivationWriter writer(&bufWriter);
   _parser.parse(&sourceFile, &writer, scope.project->getTabSize());

   // compile
   MemoryReader bufReader(buffer);
   DerivationReader reader(&bufReader);

   compileModule(reader.readRoot(), scope);
}

bool Compiler :: run(Project& project)
{
   bool withDebugInfo = project.BoolSetting(opDebugMode);
   Map<ident_t, ModuleInfo> modules(ModuleInfo(NULL, NULL));

   MemoryDump  buffer;                // temporal derivation buffer
   Unresolveds unresolveds(Unresolved(), NULL);

   Path modulePath;
   ReferenceNs name(project.StrSetting(opNamespace));
   int rootLength = name.Length();
   for (SourceIterator it = project.getSourceIt(); !it.Eof(); it++) {
      try {
         // build module namespace
         Path::loadSubPath(modulePath, it.key());
         name.truncate(rootLength);
         name.pathToName(modulePath);

         // create or update module
         ModuleInfo info = modules.get(name);
         if (info.codeModule == NULL) {
            info.codeModule = project.createModule(name);
            if (withDebugInfo)
               info.debugModule = project.createDebugModule(name);

            modules.add(name, info);
         }

         ModuleScope scope(&project, it.key(), info.codeModule, info.debugModule, &unresolveds);
         scope.sourcePathRef = _writer.writeSourcePath(info.debugModule, scope.sourcePath);

         project.printInfo("%s", it.key());

         // compile source
         compile(*it, &buffer, scope);
      }
      catch (LineTooLong& e)
      {
         project.raiseError(errLineTooLong, it.key(), e.row, 1);
      }
      catch (InvalidChar& e)
      {
         size_t destLength = 6;

         String<ident_c, 6> symbol;
         StringHelper::copy(symbol, (_ELENA_::unic_c*)&e.ch, 1, destLength);

         project.raiseError(errInvalidChar, it.key(), e.row, e.column, symbol);
      }
      catch (SyntaxError& e)
      {
         project.raiseError(e.error, it.key(), e.row, e.column, e.token);
      }
   }

   Map<ident_t, ModuleInfo>::Iterator it = modules.start();
   while (!it.Eof()) {
      ModuleInfo info = *it;

      project.saveModule(info.codeModule, "nl");

      if (info.debugModule)
         project.saveModule(info.debugModule, "dnl");

      it++;
   }

   // validate the unresolved forward refereces if unresolved reference warning is enabled
   validateUnresolved(unresolveds, project);

   return !project.HasWarnings();
}
