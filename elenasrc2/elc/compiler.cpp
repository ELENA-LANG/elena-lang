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

#define HINT_LOOP             0x20000000
#define HINT_TRY              0x10000000
#define HINT_ALT              0x12000000
#define HINT_CATCH            0x08000000
#define HINT_STACKSAFE_CALL   0x04000000
#define HINT_DIRECT_ORDER     0x01000000     // indictates that the parameter should be stored directly in reverse order
#define HINT_HEAP_MODE        0x00400000
#define HINT_OARG_UNBOXING    0x00200000     // used to indicate unboxing open argument list
#define HINT_GENERIC_METH     0x00100000     // generic methodcompileRetExpression
#define HINT_ASSIGN_MODE      0x00080000     // indicates possible assigning operation (e.g a := a + x)
#define HINT_SELFEXTENDING    0x00040000
#define HINT_ACTION           0x00020000
#define HINT_EXTERNAL_CALL    0x00010000

//
//////#define HINT_ROOTEXPR         0x40000000

// --- Auxiliary routines ---

inline bool isCollection(DNode node)
{
   return (node == nsExpression && node.nextNode()==nsExpression);
}

inline bool isReturnExpression(DNode expr)
{
   return (expr == nsExpression && expr.nextNode() == nsNone);
}

//inline bool isSimpleExpression(DNode expr)
//{
//   if (expr == nsObject) {
//      expr = expr.nextNode();
//      if (expr == nsMessageOperation) {
//         expr = expr.nextNode();
//
//         return expr == nsNone;
//      }
//   }
//
//   return false;
//}

inline bool isSingleStatement(DNode expr)
{
   return (expr == nsExpression) && (expr.firstChild().nextNode() == nsNone);
}

inline ref_t importMessage(_Module* exporter, ref_t exportRef, _Module* importer)
{
   int verbId = 0;
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

inline int countSymbol(DNode node, Symbol symbol)
{
   int counter = 0;
   while (node != nsNone) {
      if (node == symbol)
         counter++;

      node = node.nextNode();
   }
   return counter;
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

//inline bool findSymbol(DNode node, Symbol symbol1, Symbol symbol2)
//{
//   while (node != nsNone) {
//      if (node==symbol1 || node==symbol2)
//         return true;
//
//      node = node.nextNode();
//   }
//   return false;
//}

inline DNode goToSymbol(DNode node, Symbol symbol)
{
   while (node != nsNone) {
      if (node==symbol)
         return node;

      node = node.nextNode();
   }
   return node;
}

inline DNode goToLastSymbol(DNode node, Symbol symbol)
{
   DNode last;
   while (node != nsNone) {
      if (node==symbol)
         last = node;

      node = node.nextNode();
   }
   return last;
}

//inline bool IsArgumentList(ObjectInfo object)
//{
//   return object.kind == okParams;
//}

inline bool isAssignOperation(DNode target, DNode operation)
{
   DNode lnode = operation.firstChild().firstChild();

   TerminalInfo tTerminal = target.Terminal();
   TerminalInfo lTerminal = lnode.Terminal();

   if (lTerminal.symbol == tsIdentifier && lTerminal.symbol == tsIdentifier && StringHelper::compare(lTerminal.value, tTerminal.value)) {
      if (operation.nextNode() == nsNone)
         return true;
   }

   return false;
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

inline bool isLocal(ObjectInfo info)
{
   switch (info.kind) {
   case okLocal:
   case okLocalAddress:
      return true;
   default:
      return false;
   }
}

inline bool IsExprOperator(int operator_id)
{
   switch(operator_id) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         return true;
      default:
         return false;
   }
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

// --- Compiler::ModuleScope ---

Compiler::ModuleScope::ModuleScope(Project* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints(-1), extensions(NULL, freeobj)
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

   boolType = module->mapSubject(project->resolveForward(BOOL_FORWARD), false);

   defaultNs.add(module->Name());
   
   loadTypes(module);
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

ref_t Compiler::ModuleScope :: mapSubject(TerminalInfo terminal, IdentifierString& output)
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

   return typeRef;
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

   return module->mapReference(referenceName, true);
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
   
   if (!headerOnly && argModule != module) {
      ClassInfo copy;
      copy.load(&reader, headerOnly);

      info.header = copy.header;
      info.classClassRef = copy.classClassRef;
      info.extensionTypeRef = copy.extensionTypeRef;

      // import method references and mark them as inherited
      ClassInfo::MethodMap::Iterator it = copy.methods.start();
      while (!it.Eof()) {
         info.methods.add(importMessage(argModule, it.key(), module), false);

         it++;
      }

      info.fields.add(copy.fields);

      // import field types
      ClassInfo::FieldTypeMap::Iterator type_it = copy.fieldTypes.start();
      while (!type_it.Eof()) {
         info.fieldTypes.add(type_it.key(), importSubject(argModule, *type_it, module));

         type_it++;
      }

      // import method types
      ClassInfo::MethodInfoMap::Iterator mtype_it = copy.methodHints.start();
      while (!mtype_it.Eof()) {
         MethodInfo methInfo = *mtype_it;
         methInfo.typeRef = importSubject(argModule, methInfo.typeRef, module);

         info.methodHints.add(
            importMessage(argModule, mtype_it.key(), module), methInfo);

         mtype_it++;
      }
   }
   else info.load(&reader, headerOnly);

   if (argModule != module) {
      // import class class reference
      if (info.classClassRef != 0)
         info.classClassRef = importReference(argModule, info.classClassRef, module);

      // import reference
      importReference(argModule, moduleRef, module);

      // import parent reference
      info.header.parentRef = importReference(argModule, info.header.parentRef, module);
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

int Compiler::ModuleScope :: defineStructSize(ref_t classReference)
{
   ClassInfo classInfo;
   if(loadClassInfo(classInfo, module->resolveReference(classReference), true) == 0)
      return 0;

   if (test(classInfo.header.flags, elStructureRole) && test(classInfo.header.flags, elEmbeddable))
      return classInfo.size;

   return 0;
}

int Compiler::ModuleScope :: defineTypeSize(ref_t type_ref, ref_t& classReference)
{
   if (type_ref == 0)
      return 0;

   classReference = typeHints.get(type_ref);
   if (classReference != 0) {
      return defineStructSize(classReference);
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
         MethodInfo methodInfo = info.methodHints.get(message);

         outputType = methodInfo.typeRef;

         if (test(info.header.flags, elSealed)) {
            return tpSealed | methodInfo.hint;
         }
         else if (test(info.header.flags, elClosed)) {
            return tpClosed | methodInfo.hint;
         }
         else return tpNormal | methodInfo.hint;
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
         else raiseWarning(1, wrnUnresovableLink, terminal);
      }
   }
}

void Compiler::ModuleScope :: raiseError(const char* message, TerminalInfo terminal)
{
   project->raiseError(message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: raiseWarning(int level, const char* message, TerminalInfo terminal)
{
   project->raiseWarning(level, message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: compileForwardHints(DNode hints, bool& constant)
{
   constant = false;

   while (hints == nsHint) {
      if (StringHelper::compare(hints.Terminal(), HINT_CONSTANT)) {
         constant = true;
      }
      else raiseWarning(1, wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
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
            else raiseWarning(1, wrnDuplicateExtension, terminal);
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

// --- Compiler::SourceScope ---

//Compiler::SourceScope :: SourceScope(Scope* parent)
//   : Scope(parent)
//{
//   this->reference = 0;
//}

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
      else raiseWarning(1, wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

ObjectInfo Compiler::SymbolScope :: mapObject(TerminalInfo identifier)
{
   /*if (StringHelper::compare(identifier, param) || ConstIdentifier::compare(identifier, PARAM_VAR)) {
      return ObjectInfo(otLocal, -1);
   }
   else */return Scope::mapObject(identifier);
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
      return ObjectInfo(okParam, -1);
   }
   else {
      int reference = info.fields.get(identifier);
      if (reference != -1) {
         if (test(info.header.flags, elStructureRole)) {
            int offset = reference;

            return ObjectInfo(okFieldAddress, offset, 0, info.fieldTypes.get(offset));
         }
//         else if (test(info.header.flags, elDynamicRole)) {
//            int type = getClassType();
//            if (type == elDebugArray) {
//               return ObjectInfo(okField, otArray, -1);
//            }
//            else return ObjectInfo(okUnknown);
//         }
         // otherwise it is a normal field
         else return ObjectInfo(okField, reference, 0, info.fieldTypes.get(reference));
      }
      else {
         if (identifier.symbol == tsReference && StringHelper::compare(identifier.value, moduleScope->module->resolveReference(this->reference))) {
            return ObjectInfo(okConstantClass, this->reference, info.classClassRef);
         }
         else return Scope::mapObject(identifier);
      }
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
         info.header.flags |= elDebugDWORD;

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
      else if (StringHelper::compare(terminal, HINT_LIMITED)) {
         info.header.flags |= elClosed;
      }
      else if (StringHelper::compare(terminal, HINT_STRUCT)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
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
         else raiseWarning(1, wrnUnknownHint, terminal);

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
         else raiseWarning(1, wrnUnknownHint, terminal);

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
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
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
            if (info.size <= 0)
               raiseError(wrnInvalidHint, value.Terminal());
         }
         else raiseWarning(1, wrnUnknownHint, terminal);

         info.header.flags |= (elEmbeddable | elStructureRole | elDynamicRole);
      }
      else if (StringHelper::compare(terminal, HINT_DYNAMIC)) {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole))
            raiseError(wrnInvalidHint, terminal);

         info.header.flags |= elDynamicRole;
         info.header.flags |= elDebugArray;
      }
      else if (StringHelper::compare(terminal, HINT_NONSTRUCTURE)) {
         info.header.flags |= elNonStructureRole;
      }
      else if (StringHelper::compare(terminal, HINT_STRING)) {
         info.header.flags |= elDebugLiteral;
      }
      else if (StringHelper::compare(terminal, HINT_WIDESTRING)) {
         info.header.flags |= elDebugWideLiteral;
      }
      else raiseWarning(1, wrnUnknownHint, terminal);

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
         else raiseWarning(1, wrnInvalidHint, terminal);
      }
      else raiseWarning(1, wrnUnknownHint, terminal);

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

int Compiler::MethodScope :: compileHints(DNode hints)
{
   int mode = 0;
   int hint = 0;
   ref_t type = 0;
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      if (StringHelper::compare(terminal, HINT_GENERIC)) {
         if (getSignature(message) != 0)
            raiseError(errInvalidHint, terminal);

         message = overwriteSubject(message, moduleScope->mapSubject(GENERIC_PREFIX));

         setClassFlag(elWithGenerics);

         mode |= HINT_GENERIC_METH;
      }
      else if (StringHelper::compare(terminal, HINT_TYPE)) {
         DNode value = hints.select(nsHintValue);
         TerminalInfo typeTerminal = value.Terminal();

         type = moduleScope->mapType(typeTerminal);
         if (type == 0)
            raiseError(wrnInvalidHint, terminal);
      }
      else if (StringHelper::compare(terminal, HINT_STACKSAFE)) {
         hint |= tpStackSafe;
      }
      else raiseWarning(1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }

   if (type != 0 || hint != 0) {
      ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);

      classScope->info.methodHints.exclude(message);
      classScope->info.methodHints.add(message, MethodInfo(type, hint));
   }

   return mode;
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

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->tape = &parent->tape;
   this->level = 0;
   this->saved = this->reserved = 0;
}

//Compiler::CodeScope :: CodeScope(MethodScope* parent, CodeType type)
//   : Scope(parent)
//{
//   this->tape = &((ClassScope*)parent->getScope(slClass))->tape;
//   this->level = 0;
//}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->tape = &((ClassScope*)parent->getScope(slClass))->tape;
   this->level = 0;
   this->saved = this->reserved = 0;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->tape = parent->tape;
   this->level = parent->level;
   this->saved = parent->saved;
   this->reserved = parent->reserved;
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
         else raiseWarning(1, wrnUnknownHint, terminal);
      }
      else raiseWarning(1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

//ref_t Compiler::CodeScope :: getObjectType(ObjectInfo object)
//{
//   if (object.kind == okOuterField) {
//      InlineClassScope* ownerScope = (InlineClassScope*)getScope(Scope::slClass);
//
//      return ownerScope->outerFieldTypes.get(object.extraparam);
//   }
//   else return getType(object);
//}

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
            || outer.outerObject.kind==okOuterField)
         {
            outer.reference = info.fields.Count();

            outers.add(identifier, outer);
            mapKey(info.fields, identifier.value, outer.reference);

            return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam, outer.outerObject.type);
         }
         // if inline symbol declared in symbol it treats self variable in a special way
         else if (StringHelper::compare(identifier, SELF_VAR)) {
            return ObjectInfo(okParam, -1);
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
   ByteCodeCompiler::loadVerbs(_verbs);
   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: loadRules(StreamReader* optimization)
{
   _rules.load(optimization);
}

bool Compiler :: optimizeJumps(CommandTape& tape)
{
   //if (!test(_optFlag, optJumps))
   //   return false;

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
   // optimize unsued and idle jumps
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

ref_t Compiler :: mapNestedExpression(CodeScope& scope, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // otherwise auto generate the name
   ReferenceNs name(moduleScope->module->Name(), INLINE_POSTFIX);

   findUninqueName(moduleScope->module, name);

   return moduleScope->module->mapReference(name);
}

void Compiler :: declareParameterDebugInfo(MethodScope& scope, CommandTape* tape, bool withThis, bool withSelf)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // declare method parameter debug info
   LocalMap::Iterator it = scope.parameters.start();
   while (!it.Eof()) {
      if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->paramsReference)) {
         _writer.declareLocalParamsInfo(*tape, it.key(), -1 - (*it).offset);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->intReference)) {
         _writer.declareLocalIntInfo(*tape, it.key(), -1 - (*it).offset, true);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->longReference)) {
         _writer.declareLocalLongInfo(*tape, it.key(), -1 - (*it).offset, true);
      }
      else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->realReference)) {
         _writer.declareLocalRealInfo(*tape, it.key(), -1 - (*it).offset, true);
      }
      else _writer.declareLocalInfo(*tape, it.key(), -1 - (*it).offset);

      it++;
   }
   if (withThis)
      _writer.declareSelfInfo(*tape, 1);

   if (withSelf)
      _writer.declareSelfInfo(*tape, -1);

   _writer.declareMessageInfo(*tape, _writer.writeMessage(moduleScope->debugModule, moduleScope->module, _verbs, scope.message));
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

         scope.info.header = copy.header;
         scope.info.size = copy.size;

         // import method references and mark them as inherited
         ClassInfo::MethodMap::Iterator it = copy.methods.start();
         while (!it.Eof()) {
            scope.info.methods.add(importMessage(module, it.key(), moduleScope->module), false);

            it++;
         }

         scope.info.fields.add(copy.fields);

         // import field types
         ClassInfo::FieldTypeMap::Iterator type_it = copy.fieldTypes.start();
         while (!type_it.Eof()) {
            scope.info.fieldTypes.add(type_it.key(), importSubject(module, *type_it, moduleScope->module));

            type_it++;
         }

         // import method types
         ClassInfo::MethodInfoMap::Iterator mtype_it = copy.methodHints.start();
         while (!mtype_it.Eof()) {
            MethodInfo methHint = *mtype_it;
            methHint.typeRef = importSubject(module, methHint.typeRef, moduleScope->module);

            scope.info.methodHints.add(
               importMessage(module, mtype_it.key(), moduleScope->module),
               methHint);

            mtype_it++;
         }
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
   _writer.declareSwitchBlock(*scope.tape);

   if (switchValue.kind == okAccumulator) {
      _writer.pushObject(*scope.tape, switchValue);

      switchValue.kind = okBlockLocal;
      switchValue.param = 1;
   }

   DNode option = node.firstChild();
   while (option == nsSwitchOption || option == nsBiggerSwitchOption || option == nsLessSwitchOption)  {
      _writer.declareSwitchOption(*scope.tape);

      int operator_id = EQUAL_MESSAGE_ID;
      if (option == nsBiggerSwitchOption) {
         operator_id = GREATER_MESSAGE_ID;
      }
      else if (option == nsLessSwitchOption) {
         operator_id = LESS_MESSAGE_ID;
      }

      recordDebugStep(scope, option.firstChild().FirstTerminal(), dsStep);
      openDebugExpression(scope);
      ObjectInfo optionValue = compileObject(option.firstChild(), scope, 0);
      _writer.loadObject(*scope.tape, optionValue);

      bool boxed = false;
      boxObject(scope, optionValue, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, option.firstChild().Terminal());

      _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
      _writer.pushObject(*scope.tape, switchValue);

      _writer.setMessage(*scope.tape, encodeMessage(0, operator_id, 1));
      _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
      _writer.callMethod(*scope.tape, 0, 1);

      bool mismatch = false;
      compileTypecast(scope, ObjectInfo(okAccumulator), scope.moduleScope->boolType, mismatch, boxed, 0);

      endDebugExpression(scope);

      _writer.jumpIfEqual(*scope.tape, scope.moduleScope->falseReference);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild().nextNode();

      //_writer.declareBlock(*scope.tape);

      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope, 0);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);

      _writer.endSwitchOption(*scope.tape);

      option = option.nextNode();
   }
   if (option == nsLastSwitchOption) {
      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild();

      //_writer.declareBlock(*scope.tape);

      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);
   }

   _writer.endSwitchBlock(*scope.tape);
}

void Compiler :: compileAssignment(DNode node, CodeScope& scope, ObjectInfo object)
{
   if (object.kind == okLocal || object.kind == okField || object.kind == okOuterField) {
      _writer.saveObject(*scope.tape, object);
   }
   else if ((object.kind == okOuter)) {
      scope.raiseWarning(2, wrnOuterAssignment, node.Terminal());
      _writer.saveObject(*scope.tape, object);
   }
   else if (object.kind == okUnknown) {
      scope.raiseError(errUnknownObject, node.Terminal());
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileContentAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo, ObjectInfo object)
{
   if (variableInfo.kind == okLocal || variableInfo.kind == okFieldAddress || variableInfo.kind == okLocalAddress) {
      ref_t classReference = 0;
      int size = 0;
      if (variableInfo.kind == okLocalAddress) {
         classReference = variableInfo.extraparam;

         size = scope.moduleScope->defineStructSize(classReference);
      }
      else size = scope.moduleScope->defineTypeSize(variableInfo.type, classReference);

      if (size <= 0)
         scope.raiseError(errInvalidOperation, node.Terminal());

      if (object.kind == okIndexAccumulator) {
         if (size == 4) {
            _writer.saveInt(*scope.tape, variableInfo);
         }
         else if ((size == 2 || size == 1) && variableInfo.kind == okLocal) {
            _writer.saveInt(*scope.tape, variableInfo);
         }
         // HOTFIX: assign a float-point numeric result 
         else if ((size == 8) && classReference == scope.moduleScope->realReference) {
            _writer.saveReal(*scope.tape, variableInfo);
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
      else {
         if (size == 4) {
            _writer.assignInt(*scope.tape, variableInfo);
         }
         else if (size == 2) {
            _writer.assignShort(*scope.tape, variableInfo);
         }
         else if (size == 1) {
            _writer.assignByte(*scope.tape, variableInfo);
         }
         else if (size == 8) {
            _writer.assignLong(*scope.tape, variableInfo);
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileVariable(DNode node, CodeScope& scope, DNode hints)
{
   if (!scope.locals.exist(node.Terminal())) {
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

         if (flags == elDebugDWORD) {
            _writer.declareLocalIntInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else if (flags == elDebugQWORD) {
            _writer.declareLocalLongInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else if (flags == elDebugReal64) {
            _writer.declareLocalRealInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else if (flags == elDebugBytes) {
            _writer.declareLocalByteArrayInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else if (flags == elDebugShorts) {
            _writer.declareLocalShortArrayInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else if (flags == elDebugIntegers) {
            _writer.declareLocalIntArrayInfo(*scope.tape, node.Terminal(), variable.param, false);
         }
         else _writer.declareLocalInfo(*scope.tape, node.Terminal(), variable.param);
      }
      else {
         int level = scope.newLocal();

         _writer.declareVariable(*scope.tape, 0);
         _writer.declareLocalInfo(*scope.tape, node.Terminal(), level);

         variable.param = level;
      }

      DNode assigning = node.firstChild();
      if (assigning != nsNone)
         compileAssigningExpression(node, assigning, scope, variable);

      if (variable.kind == okLocal) {
         scope.mapLocal(node.Terminal(), variable.param, type);
      }
      else scope.mapLocal(node.Terminal(), variable.param, variable.extraparam, true);
   }
   else scope.raiseError(errDuplicatedLocal, node.Terminal());
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
   else if (!emptystr(terminal)) {
      object = scope.mapObject(terminal);
   }

   bool dummy = false;
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         break;
      //case okExternal:
      //   // external call cannot be used inside symbol
      //   if (test(mode, HINT_ROOT))
      //      scope.raiseError(errInvalidSymbolExpr, node.Terminal());
      //   break;
      case okInternal:
         if (!test(mode, HINT_EXTERNAL_CALL) && node.nextNode() != nsMessageOperation)
            scope.raiseError(errInvalidOperation, node.Terminal());
         break;
   }

   return object;
}

ObjectInfo Compiler :: compileObject(DNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

   DNode member = objectNode.firstChild();
   switch (member)
   {
      //case nsRetStatement:
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
         else result = compileExpression(member, scope, 0);
         break;
      case nsMessageReference:
         result = compileMessageReference(member, scope, mode);
         break;
      default:
         result = compileTerminal(objectNode, scope, mode);
   }

   return result;
}

ObjectInfo Compiler :: compileMessageReference(DNode node, CodeScope& scope, int mode)
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

         message.append(verb_id + 0x20);
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
         else message.append(verb_id + 0x20);
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
         message[0] = message[0] + count;
      }
   }

   retVal.param = scope.moduleScope->module->mapReference(message);

   return retVal;
}

ObjectInfo Compiler :: loadObject(CodeScope& scope, ObjectInfo& object)
{
   if (object.kind == okFieldAddress) {
      if (object.param == 0) {
         object.kind = okParam;
         object.param = 1;
      }
      else object = boxStructureField(scope, object, ObjectInfo(okThisParam, 1), 0);
   }

   _writer.loadObject(*scope.tape, object);

   return ObjectInfo(okAccumulator, 0, 0, object.type);
}

ObjectInfo Compiler::saveObject(CodeScope& scope, ObjectInfo& object, int offset)
{
   if (object.kind == okFieldAddress) {
      if (object.param == 0) {
         object.kind = okParam;
         object.param = 1;
      }
      else object = boxStructureField(scope, object, ObjectInfo(okThisParam, 1), 0);
   }

   if (offset != 0) {
      _writer.loadObject(*scope.tape, object);
      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, offset));
   }
   else _writer.pushObject(*scope.tape, object);

   return ObjectInfo(okCurrent, offset, 0, object.type);
}

bool Compiler :: checkIfBoxingRequired(CodeScope& scope, ObjectInfo object, ref_t argType, int mode)
{
   //if (test(mode, HINT_DIRECT_CALL) && resolveStrongType(type_ref)) {
   //   return false;
   //}
   if ((object.kind == okLocal || object.kind == okParam || object.kind == okThisParam) && object.extraparam == -1) {
      int size = scope.moduleScope->defineTypeSize(object.type);

      return (size != 0 && !test(mode, HINT_STACKSAFE_CALL));
   }
   else if (object.kind == okLocalAddress) {
      return !test(mode, HINT_STACKSAFE_CALL);
   }
   else if (object.kind == okSubject) {
      return true;
   }
   else if (object.kind == okIndexAccumulator || object.kind == okFieldAddress) {
      return true;
   }
   else if (object.kind == okParams) {
      if (test(mode, HINT_STACKSAFE_CALL) && argType != 0 && scope.moduleScope->typeHints.get(argType) == scope.moduleScope->paramsReference) {
         return false;
      }
      else return true;
   }
   else return false;
}

ObjectInfo Compiler :: boxObject(CodeScope& scope, ObjectInfo object, bool& boxed)
{
   if (object.kind == okFieldAddress) {
      if (object.param != 0) {
         object = ObjectInfo(okParam, 1, scope.isStackSafe() ? -1 : 0, object.type);
      }
      else object = boxStructureField(scope, object, ObjectInfo(okThisParam, 1));
   }

   // NOTE: boxing should be applied only for the typed local parameter
   if (object.kind == okParams) {
      boxed = true;

      _writer.boxArgList(*scope.tape, scope.moduleScope->mapReference(scope.moduleScope->project->resolveForward(ARRAY_FORWARD)) | mskVMTRef);
   }
   else if (object.kind == okSubject) {
      boxed = true;

      _writer.loadObject(*scope.tape, ObjectInfo(okLocalAddress, object.param));
      _writer.boxObject(*scope.tape, 4, scope.moduleScope->mapReference(scope.moduleScope->project->resolveForward(SIGNATURE_FORWARD)) | mskVMTRef);

      return ObjectInfo(okAccumulator, scope.moduleScope->signatureReference);
   }
   else if (object.kind == okIndexAccumulator) {
      boxed = true;

      allocateStructure(scope, 0, object);
      _writer.saveInt(*scope.tape, object);
      ref_t wrapperRef = scope.moduleScope->intReference;
      _writer.boxObject(*scope.tape, 4, wrapperRef | mskVMTRef, true);
   }
   else if (object.kind == okLocalAddress) {
      int size = scope.moduleScope->defineStructSize(object.extraparam);
      if (size != 0) {
         boxed = true;

         _writer.boxObject(*scope.tape, size, object.extraparam | mskVMTRef, true);
      }
   }
   else if ((object.kind == okParam || object.kind == okThisParam) && object.type != 0 && object.extraparam == -1)
   {
      ref_t classReference = 0;
      int size = scope.moduleScope->defineTypeSize(object.type, classReference);
      if (size != 0) {
         boxed = true;

         _writer.boxObject(*scope.tape, size, classReference | mskVMTRef);
      }
   }

   return object;
}

ObjectInfo Compiler :: boxStructureField(CodeScope& scope, ObjectInfo field, ObjectInfo thisParam, int mode)
{
   bool presavedAcc = thisParam.kind == okAccumulator;

   int offset = field.param;
   ref_t classReference = 0;
   ref_t type = field.type;
   int size = scope.moduleScope->defineTypeSize(field.type, classReference);

   if (test(mode, HINT_HEAP_MODE) || size > 8) {
      if (presavedAcc)
         _writer.pushObject(*scope.tape, thisParam);

      // otherwise create a dynamic object and copy the content
      _writer.newStructure(*scope.tape, size, classReference);
      _writer.loadBase(*scope.tape, ObjectInfo(okAccumulator));

      if (presavedAcc) {
         _writer.popObject(*scope.tape, thisParam);
      }
      else _writer.loadObject(*scope.tape, thisParam);

      field = ObjectInfo(okAccumulator);
   }
   else {
      allocateStructure(scope, 0, field, presavedAcc);

      _writer.loadBase(*scope.tape, field);
      _writer.loadObject(*scope.tape, thisParam);
   }

   if (size == 2) {
      _writer.copyShort(*scope.tape, offset);
   }
   else if (size == 4) {
      _writer.copyInt(*scope.tape, offset);
   }
   else if (size == 8) {
      _writer.copyStructure(*scope.tape, offset, size);
      _writer.loadObject(*scope.tape, ObjectInfo(okBase));
   }
   else if (size > 0) {
      _writer.copyStructure(*scope.tape, field.param, size);
      _writer.loadObject(*scope.tape, ObjectInfo(okBase));
   }

   return ObjectInfo(okAccumulator, field.extraparam, 0, type);
}

void Compiler :: compileMessageParameter(DNode& arg, TerminalInfo& subject, CodeScope& scope, ref_t type_ref, int mode, size_t& count)
{
   if (arg == nsMessageParameter) {
      count++;

      ObjectInfo param = compileObject(arg.firstChild(), scope, 0);

      loadObject(scope, param);

      // if type is mismatch - typecast
      bool mismatch = false;
      bool boxed = false;
      compileTypecast(scope, param, type_ref, mismatch, boxed, mode);

      if (checkIfBoxingRequired(scope, param, type_ref, mode) && !boxed)
         boxObject(scope, param, boxed);

      if (mismatch)
         scope.raiseWarning(2, wrnTypeMismatch, arg.FirstTerminal());

      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, arg.firstChild().Terminal());

      if (test(mode, HINT_DIRECT_ORDER)) {
         _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
      }
      else _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

      arg = arg.nextNode();
   }
}

ref_t Compiler :: mapMessage(DNode node, CodeScope& scope, size_t& count, int& mode)
{
   bool             simpleParameters = true;

   TerminalInfo     verb = node.Terminal();
   IdentifierString signature;
   bool             first = true;

   ref_t            verb_id = _verbs.get(verb.value);
   int              paramCount = 0;
   DNode            arg = node.firstChild();

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

   // if message has generic argument list
   while (arg == nsMessageParameter) {
      count++;
      paramCount++;

      if (arg.firstChild().firstChild() != nsNone)
         simpleParameters = false;

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      // if it is an open argument list

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
            // if a generic argument is used with an open argument list
            simpleParameters = false;

            // check if argument list should be unboxed
            DNode param = arg.firstChild();

            if (arg.firstChild().nextNode() == nsNone && scope.mapObject(arg.firstChild().Terminal()).kind == okParams) {
               mode |= HINT_OARG_UNBOXING;
            }
            else {
               // terminator
               count += 1;

               while (arg != nsNone) {
                  count++;

                  arg = arg.nextNode();
               }
            }
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > 0x0F)
               scope.raiseError(errNotApplicable, subject);
         }
         else {
            count++;
            paramCount++;

            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            if (arg.firstChild().firstChild() != nsNone)
               simpleParameters = false;

            arg = arg.nextNode();
         }
      }
   }

   // if signature is presented
   ref_t sign_id = 0;
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   if (simpleParameters)
      mode |= HINT_DIRECT_ORDER;

   // create a message id
   return encodeMessage(sign_id, verb_id, paramCount);
}

void Compiler :: compileDirectMessageParameters(DNode arg, CodeScope& scope, int mode)
{
   if (arg == nsMessageParameter) {
      compileDirectMessageParameters(arg.nextNode(), scope, mode);

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode);

      // box the object if required
      if (checkIfBoxingRequired(scope, param, 0, 0)) {
         _writer.loadObject(*scope.tape, param);

         bool boxed = false;
         boxObject(scope, param, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, arg.firstChild().FirstTerminal());

         _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
      }
      else _writer.pushObject(*scope.tape, param);
   }
   else if (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      arg = arg.nextNode();

      // skip an argument without a parameter value
      while (arg == nsSubjectArg) {
         subject = arg.Terminal();

         arg = arg.nextNode();
      }

      int paramMode = mode & (HINT_STACKSAFE_CALL | HINT_DIRECT_ORDER);

      ref_t type_ref = scope.moduleScope->mapType(subject);

      compileDirectMessageParameters(arg.nextNode(), scope, mode);

      size_t dummy = 0;
      compileMessageParameter(arg, subject, scope, type_ref, paramMode, dummy);
   }
}

void Compiler :: compilePresavedMessageParameters(DNode arg, CodeScope& scope, int mode, size_t& stackToFree)
{
   size_t count = 0;

   // if message has generic argument list
   while (arg == nsMessageParameter) {
      count++;

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode);
      _writer.loadObject(*scope.tape, param);

      // box the object if required
      bool boxed = false;
      boxObject(scope, param, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, arg.firstChild().Terminal());

      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      arg = arg.nextNode();

      // skip an argument without a parameter value
      while (arg == nsSubjectArg) {
         subject = arg.Terminal();

         arg = arg.nextNode();
      }

      int paramMode = mode & HINT_STACKSAFE_CALL;
      ref_t type_ref = scope.moduleScope->mapType(subject);

      if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(type_ref, scope.moduleScope->paramsReference)) {
         if (!scope.moduleScope->typeHints.exist(type_ref, scope.moduleScope->paramsReference))
            scope.raiseError(errNotApplicable, subject);

         stackToFree = 1;
         while (arg != nsNone) {
            count++;
            stackToFree++;

            ObjectInfo retVal = compileExpression(arg, scope, 0);
            _writer.loadObject(*scope.tape, retVal);

            // box object if required
            bool boxed = false;
            boxObject(scope, retVal, boxed);
            if (boxed)
               scope.raiseWarning(4, wrnBoxingCheck, arg.FirstTerminal());

            _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

            arg = arg.nextNode();
         }
      }
      else compileMessageParameter(arg, subject, scope, type_ref, paramMode, count);
   }
}

void Compiler :: compileUnboxedMessageParameters(DNode node, CodeScope& scope, int mode, int count, size_t& stackToFree)
{
   // unbox the argument list
   DNode args = goToLastSymbol(node, nsMessageParameter);
   ObjectInfo list = compileObject(args.firstChild(), scope, mode);
   _writer.loadObject(*scope.tape, list);

   // unbox the argument list and return the object saved before the list
   _writer.unboxArgList(*scope.tape);

   // reserve the place for target and generic message parameters if available and save the target
   _writer.declareArgumentList(*scope.tape, count + 1);
   _writer.saveObject(*scope.tape, ObjectInfo(okCurrent));

   count = 0;
   // if message has generic argument list
   DNode arg = node;
   while (arg == nsMessageParameter && arg != args) {
      count++;

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode);
      _writer.loadObject(*scope.tape, param);

      // box the object if required
      bool boxed = false;
      boxObject(scope, param, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, arg.firstChild().FirstTerminal());

      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

      arg = arg.nextNode();
   }

   // indicate that the stack to be freed cannot be defined at compile-time
   stackToFree = (size_t)-1;
}

ref_t Compiler :: compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo& object, int& mode, size_t& spaceToRelease)
{
   size_t count = 0;
   ref_t messageRef = mapMessage(node, scope, count, mode);

   ref_t extensionRef = mapExtension(scope, messageRef, object);
   int methodHint = extensionRef != 0 ? scope.moduleScope->checkMethod(extensionRef, messageRef) : defineMethodHint(scope, object, messageRef);

   compileMessageParameters(node, scope, object, methodHint, count, mode, spaceToRelease);

   if (extensionRef != 0) {
      object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
   }

   return messageRef;
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
   else if ((object.kind == okAccumulator && object.param != 0) || (object.kind == okConstantSymbol && object.extraparam != 0)) {
      if (scope.moduleScope->extensionHints.exist(messageRef)) {
         ref_t classRef = object.kind == okAccumulator ? object.param : object.extraparam;

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

int Compiler :: defineMethodHint(CodeScope& scope, ObjectInfo object, ref_t messageRef)
{
   // use dynamic extension if exists
   int methodHint = 0;

   if (object.kind == okConstantSymbol || object.kind == okLocalAddress) {
      methodHint = scope.moduleScope->checkMethod(object.extraparam, messageRef);
   }
   else if (object.kind == okSubject) {
      methodHint = scope.moduleScope->checkMethod(scope.moduleScope->signatureReference, messageRef);
   }
   else if (object.kind == okConstantClass) {
      // class is always sealed
      methodHint = (scope.moduleScope->checkMethod(object.extraparam, messageRef) & ~tpMask) | tpSealed;
   }
   else if ((object.kind == okAccumulator || object.kind == okConstantRole) && object.param != 0) {
      methodHint = scope.moduleScope->checkMethod(object.param, messageRef);
   }
   else if (object.kind == okThisParam) {
      methodHint = scope.moduleScope->checkMethod(scope.getClassRefId(), messageRef);
   }
   else if (object.kind == okSuper) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

      // super class is always sealed
      methodHint = (scope.moduleScope->checkMethod(classScope->info.header.parentRef, messageRef) & ~tpMask) | tpSealed;
   }
   else methodHint = scope.moduleScope->checkMethod(scope.moduleScope->typeHints.get(object.type), messageRef);

   return methodHint;
}

void Compiler :: compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo object, int methodHint, int count, int& mode, size_t& spaceToRelease)
{
   // if the target is in register(i.e. it is the result of the previous operation), direct message compilation is not possible
   if (object.kind == okAccumulator) {
      mode &= ~HINT_DIRECT_ORDER;
   }
   else if (count == 1 && !test(mode, HINT_OARG_UNBOXING)) {
      mode |= HINT_DIRECT_ORDER;
   }

   if (methodHint == (tpClosed | tpStackSafe) || methodHint == (tpSealed | tpStackSafe))
      mode |= HINT_STACKSAFE_CALL;

   // if only simple arguments are used we could directly save parameters
   if (test(mode, HINT_DIRECT_ORDER)) {
      compileDirectMessageParameters(node.firstChild(), scope, mode);

      _writer.loadObject(*scope.tape, object);

      if (checkIfBoxingRequired(scope, object, object.type, mode)) {
         // if the object is stack allocated - box it
         bool boxed = false;
         boxObject(scope, object, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());
      }

      _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
   }
   // if open argument list should be unboxed
   else if (test(mode, HINT_OARG_UNBOXING)) {
      // save the target
      _writer.loadObject(*scope.tape, object);

      // box object if required
      bool boxed = false;
      object = boxObject(scope, object, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());

      _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));

      compileUnboxedMessageParameters(node.firstChild(), scope, mode & ~(HINT_OARG_UNBOXING), count, spaceToRelease);
   }
   // otherwise the space should be allocated first,
   // to garantee the correct order of parameter evaluation
   else {
      _writer.declareArgumentList(*scope.tape, count + 1);
      _writer.loadObject(*scope.tape, object);

      if (checkIfBoxingRequired(scope, object, object.type, mode)) {
         //scope.raiseWarning(wrnBoxingCheck, node.Terminal());

         // if the object is stack allocated - box it
         bool boxed = false;
         boxObject(scope, object, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());
      }

      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent));

      compilePresavedMessageParameters(node.firstChild(), scope, mode, spaceToRelease);
   }
}

ObjectInfo Compiler :: compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   _writer.loadObject(*scope.tape, object);

   DNode elsePart = node.select(nsElseOperation);
   if (elsePart != nsNone) {
      _writer.declareThenElseBlock(*scope.tape);
      compileBranching(node, scope, object, operator_id, 0);
      _writer.declareElseBlock(*scope.tape);
      compileBranching(elsePart, scope, object, 0, 0); // for optimization, the condition is checked only once
      _writer.endThenBlock(*scope.tape);
   }
   else if (test(mode, HINT_LOOP)) {
      compileBranching(node, scope, object, operator_id, HINT_LOOP);
      _writer.jump(*scope.tape, true);
   }
   else {
      _writer.declareThenBlock(*scope.tape);
      compileBranching(node, scope, object, operator_id, 0);
      _writer.endThenBlock(*scope.tape);
   }

   return ObjectInfo(okAccumulator, 0);
}

int Compiler :: mapInlineOperandType(ModuleScope& moduleScope, ObjectInfo operand)
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
   else if (operand.kind == okAccumulator && operand.param != 0) {
      return moduleScope.getClassFlags(operand.param) & elDebugMask;
   }
   else if (operand.kind == okUnknown) {
      return 0;
   }
   else if (operand.kind == okConstantSymbol || operand.kind == okLocalAddress) {
      return moduleScope.getClassFlags(operand.extraparam) & elDebugMask;
   }
   else return moduleScope.getClassFlags(moduleScope.typeHints.get(operand.type)) & elDebugMask;
}

int Compiler :: mapInlineTargetOperandType(ModuleScope& moduleScope, ObjectInfo operand)
{
   int flags = 0;

   if (operand.kind == okAccumulator && operand.param != 0) {
      flags = moduleScope.getClassFlags(operand.param);
   }
   else flags = moduleScope.getClassFlags(moduleScope.typeHints.get(operand.type));

   // read only classes cannot be used for variable operations
   if (test(flags, elReadOnlyRole))
      flags = 0;

   return flags & elDebugMask;
}

bool Compiler::compileInlineArithmeticOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int lflag = mapInlineOperandType(*moduleScope, loperand);
   int rflag = mapInlineOperandType(*moduleScope, roperand);

   if (lflag == 0 || lflag != rflag) {
      return false;
   }

   bool assignMode = test(mode, HINT_ASSIGN_MODE);

   // check
   if (lflag == elDebugDWORD) {
      result.param = moduleScope->intReference;
   }
   else if (lflag == elDebugQWORD) {
      result.param = moduleScope->longReference;
   }
   else if (operator_id == AND_MESSAGE_ID || operator_id == OR_MESSAGE_ID || operator_id == XOR_MESSAGE_ID) {
      return false;
   }
   else if (lflag == elDebugReal64) {
      result.param = moduleScope->realReference;
   }
   else return false;

   //result.extraparam =

   if (assignMode) {
      _writer.popObject(*scope.tape, ObjectInfo(okBase));
      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

      result.kind = okIdle;
   }
   else {
      allocateStructure(scope, 0, result);
      _writer.loadBase(*scope.tape, result);
      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

      if (lflag == elDebugDWORD) {
         _writer.assignInt(*scope.tape, ObjectInfo(okBase));
      }
      else _writer.assignLong(*scope.tape, ObjectInfo(okBase));

      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));
   }

   if (lflag == elDebugDWORD) {
      _writer.doIntOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugQWORD) {
      _writer.doLongOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugReal64) {
      _writer.doRealOperation(*scope.tape, operator_id);
   }

   _writer.loadObject(*scope.tape, ObjectInfo(okBase));

   return true;
}

bool Compiler::compileInlineVarArithmeticOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int lflag = mapInlineTargetOperandType(*moduleScope, loperand);
   int rflag = mapInlineOperandType(*moduleScope, roperand);

   if (lflag == 0 || lflag != rflag) {
      return false;
   }

   // check
   if (lflag == elDebugDWORD) {
      result.param = moduleScope->intReference;
   }
   else if (lflag == elDebugQWORD) {
      result.param = moduleScope->longReference;
   }
   else if (lflag == elDebugReal64) {
      result.param = moduleScope->realReference;
   }
   else return false;

   _writer.popObject(*scope.tape, ObjectInfo(okBase));
   _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

   result.kind = okIdle;

   if (lflag == elDebugDWORD) {
      _writer.doIntOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugQWORD) {
      _writer.doLongOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugReal64) {
      _writer.doRealOperation(*scope.tape, operator_id);
   }

   _writer.loadObject(*scope.tape, ObjectInfo(okBase));

   return true;
}

bool Compiler :: compileInlineComparisionOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, bool& invertMode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int lflag = mapInlineOperandType(*moduleScope, loperand);
   int rflag = mapInlineOperandType(*moduleScope, roperand);

   if (lflag == 0 || lflag != rflag) {
      return false;
   }

   if (operator_id == GREATER_MESSAGE_ID) {
      _writer.popObject(*scope.tape, ObjectInfo(okBase));
      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

      operator_id = LESS_MESSAGE_ID;
   }
   else {
      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));
      _writer.popObject(*scope.tape, ObjectInfo(okBase));
   }

   if (lflag == elDebugDWORD) {
      _writer.doIntOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugQWORD) {
      _writer.doLongOperation(*scope.tape, operator_id);
   }
   else if (lflag == elDebugReal64) {
      _writer.doRealOperation(*scope.tape, operator_id);
   }
   //else if (lflag == elDebugChars) {
   //   _writer.doLiteralOperation(*scope.tape, operator_id);
   //}
   else return false;

   if (invertMode) {
       _writer.selectByIndex(*scope.tape, scope.moduleScope->trueReference, scope.moduleScope->falseReference);

       invertMode = false;
   }
   else _writer.selectByIndex(*scope.tape, scope.moduleScope->falseReference, scope.moduleScope->trueReference);

   result.type = moduleScope->boolType;

   return true;
}

bool Compiler::compileInlineReferOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2, ObjectInfo& result)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int lflag = mapInlineOperandType(*moduleScope, loperand);
   int rflag = mapInlineOperandType(*moduleScope, roperand);
   int rflag2 = mapInlineOperandType(*moduleScope, roperand2);

   if (operator_id == SET_REFER_MESSAGE_ID) {
      if (lflag == elDebugIntegers && rflag == elDebugDWORD && rflag2 == elDebugDWORD) {
         _writer.popObject(*scope.tape, ObjectInfo(okBase));
         _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

         _writer.doIntArrayOperation(*scope.tape, operator_id);
      }
      //else if (lflag == elDebugChars && rflag == elDebugDWORD && rflag2 == elDebugDWORD) {
      //   _writer.popObject(*scope.tape, ObjectInfo(okBase));
      //   _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

      //   _writer.doCharArrayOperation(*scope.tape, operator_id);
      //}
      else return false;
   }
   else {
      if (lflag == elDebugIntegers && rflag == elDebugDWORD) {
         result.param = moduleScope->intReference;

         allocateStructure(scope, 0, result);
         _writer.loadBase(*scope.tape, result);
         _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

         _writer.doIntArrayOperation(*scope.tape, operator_id);
      }
      else if (loperand.kind == okParams && rflag == elDebugDWORD) {
         _writer.popObject(*scope.tape, ObjectInfo(okBase));
         _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

         _writer.doArrayOperation(*scope.tape, operator_id);
      }
      //else if (lflag == elDebugChars && rflag == elDebugDWORD) {
      //   result.param = moduleScope->charReference;

      //   allocateStructure(scope, 0, result);
      //   _writer.loadBase(*scope.tape, result);
      //   _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

      //   _writer.doCharArrayOperation(*scope.tape, operator_id);
      //}
      else return false;
   }

   ////if (lflag == elDebugIntegers && rflag == elDebugDWORD) {
   ////   if (operator_id == SET_REFER_MESSAGE_ID && rflag2 == elDebugDWORD) {
   ////   }
   ////   else return false;

   ////   if (rflag2 == 0 || rflag2 == elDebugDWORD) {

   ////   }
   ////}

   ////if (loperand.kind != okParams || rflag != elDebugDWORD)
   ////   return false;

   ////_writer.popObject(*scope.tape, ObjectInfo(okBase));
   ////_writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

   ////_writer.doArrayOperation(*scope.tape, operator_id);

   return true;
}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   ObjectInfo retVal(okAccumulator);

   TerminalInfo operator_name = node.Terminal();
   int operator_id = _operators.get(operator_name);

   // if it is branching operators
   if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
      return compileBranchingOperator(node, scope, object, mode, operator_id);
   }

   recordDebugStep(scope, operator_name, dsStep);
   openDebugExpression(scope);

   // HOTFIX : recognize SET_REFER_MESSAGE_ID
   if (operator_id == REFER_MESSAGE_ID && node.nextNode() == nsAssigning)
      operator_id = SET_REFER_MESSAGE_ID;

   bool dblOperator = IsDoubleOperator(operator_id);
   bool notOperator = IsInvertedOperator(operator_id);

   ObjectInfo operand;
   ObjectInfo operand2;

   // if it is comparing with nil
   if (object.kind == okNil) {
      // if operation with $nil
      operand = compileExpression(node, scope, 0);

      _writer.loadObject(*scope.tape, operand);

      if (notOperator) {
         _writer.selectByAcc(*scope.tape, scope.moduleScope->trueReference, scope.moduleScope->falseReference);
      }
      else _writer.selectByAcc(*scope.tape, scope.moduleScope->falseReference, scope.moduleScope->trueReference);
      
      return ObjectInfo(okAccumulator, 0, 0, scope.moduleScope->boolType);
   }

   // if the operation parameters can be compiled directly
   if (!dblOperator && object.kind != okAccumulator) {
      operand = compileExpression(node, scope, 0);

      saveObject(scope, operand, 0);
      saveObject(scope, object, 0);
   }
   else {
      _writer.declareArgumentList(*scope.tape, dblOperator ? 3 : 2);

      saveObject(scope, object, 0);

      if (dblOperator) {
         operand2 = compileExpression(node.nextNode().firstChild(), scope, 0);
         saveObject(scope, operand2, 2);
      }

      operand = compileExpression(node, scope, 0);
      saveObject(scope, operand2, 1);
   }

   //recordStep(scope, node.Terminal(), dsProcedureStep);

   if (IsExprOperator(operator_id) && compileInlineArithmeticOperator(scope, operator_id, object, operand, retVal, mode)) {
      // if inline arithmetic operation is implemented
      // do nothing
   }
   else if (IsVarOperator(operator_id) && compileInlineVarArithmeticOperator(scope, operator_id, object, operand, retVal, mode)) {
      // if inline referring operation is implemented
      // do nothing
   }
   else if (IsCompOperator(operator_id) && compileInlineComparisionOperator(scope, operator_id, object, operand, retVal, notOperator)) {
      // if inline comparision operation is implemented
      // do nothing
   }
   else if (IsReferOperator(operator_id) && compileInlineReferOperator(scope, operator_id, object, operand, operand2, retVal)) {
      // if inline referring operation is implemented
      // do nothing
   }
   else {
      // box operands if necessary
      if (checkIfBoxingRequired(scope, object, 0, 0)) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));

         bool boxed = false;
         object = boxObject(scope, object, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, node.Terminal());

         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 0));
      }
      if (checkIfBoxingRequired(scope, operand, 0, 0)) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 1));

         bool boxed = false;
         operand = boxObject(scope, operand, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, node.firstChild().Terminal());

         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 1));
      }
      if (operand2.kind != okUnknown && checkIfBoxingRequired(scope, operand2, 0, 0)) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 2));

         bool boxed = false;
         operand2 = boxObject(scope, operand2, boxed);
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, node.nextNode().firstChild().FirstTerminal());

         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 2));
      }

      int message_id = encodeMessage(0, operator_id, dblOperator ? 2 : 1);

      compileMessage(node, scope, object, message_id, 0);
   }

   if (notOperator) {
      ModuleScope* moduleScope = scope.moduleScope;

      bool mismatch = false;
      bool boxed = false;
      compileTypecast(scope, retVal, scope.moduleScope->boolType, mismatch, boxed, 0);

      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.nextNode().firstChild().FirstTerminal());

      _writer.invertBool(*scope.tape, moduleScope->trueReference, moduleScope->falseReference);

      retVal.extraparam = scope.moduleScope->boolType;
   }

   if (dblOperator)
      node = node.nextNode();

   endDebugExpression(scope);

   return retVal;
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode)
{
   ObjectInfo retVal(okAccumulator);

   if (messageRef == 0)
      scope.raiseError(errUnknownMessage, node.Terminal());

   int signRef = getSignature(messageRef);
   int paramCount = getParamCount(messageRef);
   bool catchMode = test(mode, HINT_CATCH);

   _writer.setMessage(*scope.tape, messageRef);

//   bool directMode = test(_optFlag, optDirectConstant);

   //recordStep(scope, node.Terminal(), dsProcedureStep);

   if (catchMode) {
      _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
      _writer.callMethod(*scope.tape, 0, paramCount);
   }
   else if (object.kind == okRole) {
      _writer.callRoleMessage(*scope.tape, paramCount);
   }
   else {
      ref_t classReference = 0;
      bool directCall = false;
      bool itselfCall = false;
      bool dispatchCall = false;
      // if static message is sent to a class class
      if (object.kind == okConstantClass) {
         retVal.param = object.param;

         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
         classReference = object.extraparam;
         directCall = true;
      }
      // if external role is provided
      else if (object.kind == okConstantRole) {
         classReference = object.param;
         directCall = true;

         if (test(mode, HINT_SELFEXTENDING) || classReference == scope.getClassRefId())
            itselfCall = true;
      }
      else if (object.kind == okConstantSymbol) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));

         if (object.extraparam != 0) {
            classReference = object.extraparam;
         }
         else classReference = object.param;

         directCall = true;
      }
      else if (object.kind == okAccumulator && object.param != 0) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));

         classReference = object.param;
         directCall = true;
      }
      else if (object.kind == okLocalAddress) {
         classReference = object.extraparam;
         directCall = true;
      }
      // if message sent to the class parent
      else if (object.kind == okSuper) {
         _writer.loadObject(*scope.tape, ObjectInfo(okThisParam, 1));

         classReference = object.param;
         directCall = true;
      }
      // if message sent to the subject variable
      else if (object.kind == okSubject) {
         classReference = scope.moduleScope->signatureReference;
         dispatchCall = true;
      }
      // if message sent to the $self
      else if (object.kind == okThisParam) {
         itselfCall = true;

         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
         classReference = scope.getClassRefId(false);
      }
      else if (object.kind == okOuterField) {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
      }
      else {
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));

         classReference = object.type != 0 ? scope.moduleScope->typeHints.get(object.type) : 0;
      }

      if (classReference != 0) {
         // check if the message is supported
         bool classFound = false;
         int methodHint = tpNormal;
         if (itselfCall) {
            if (test(scope.getClassFlags(), elClosed)) {
               ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
               if (classScope->info.methods.exist(messageRef)) {
                  methodHint = test(classScope->info.header.flags, elSealed) ? tpSealed : tpClosed;

                  retVal.type = classScope->info.methodHints.get(messageRef).typeRef;
               }
            }
            classFound = true;
         }
         else methodHint = scope.moduleScope->checkMethod(classReference, messageRef, classFound, retVal.type) & tpMask;

         if ((directCall && methodHint != tpUnknown) || methodHint == tpSealed) {
            _writer.callResolvedMethod(*scope.tape, classReference, messageRef);
         }
         else if (methodHint == tpClosed) {
            _writer.callVMTResolvedMethod(*scope.tape, classReference, messageRef);
         }
         else if (dispatchCall) {
            _writer.callResolvedMethod(*scope.tape, classReference, encodeVerb(DISPATCH_MESSAGE_ID));
         }
         else {
            retVal.extraparam = 0;

            bool boxed = false;
            boxObject(scope, object, boxed);
            if (boxed)
               scope.raiseWarning(4, wrnBoxingCheck, node.Terminal());

            // if the class found and the message is not supported - warn the programmer and raise an exception
            if (classFound && methodHint == tpUnknown) {
               scope.raiseWarning(1, wrnUnknownMessage, node.FirstTerminal());
            }
            _writer.callMethod(*scope.tape, 0, paramCount);
         }
      }
      else _writer.callMethod(*scope.tape, 0, paramCount);
   }

   // the result of get&type message should be typed
   if (paramCount == 0 && getVerb(messageRef) == GET_MESSAGE_ID) {
      if (scope.moduleScope->typeHints.exist(signRef)) {
         return ObjectInfo(okAccumulator, 0, 0, signRef);
      }
   }
   return retVal;
}

void Compiler :: releaseOpenArguments(CodeScope& scope, size_t spaceToRelease)
{
   // it should be removed from the stack
   if (spaceToRelease == (size_t)-1) {
      _writer.releaseArgList(*scope.tape);
      _writer.releaseObject(*scope.tape);
   }
   else _writer.releaseObject(*scope.tape, spaceToRelease);
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   size_t spaceToRelease = 0;
   ref_t messageRef = compileMessageParameters(node, scope, object, mode, spaceToRelease);

   recordDebugStep(scope, node.Terminal(), dsStep);
   openDebugExpression(scope);

   ObjectInfo retVal = compileMessage(node, scope, object, messageRef, mode);

   endDebugExpression(scope);

   if (spaceToRelease > 0) {
      // if open argument list is used
      releaseOpenArguments(scope, spaceToRelease);
   }

   return retVal;
}

ObjectInfo Compiler :: compileEvalMessage(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   size_t count = countSymbol(node, nsMessageParameter);

   _writer.declareArgumentList(*scope.tape, count + 1);
   _writer.loadObject(*scope.tape, object);

   bool boxed;
   boxObject(scope, object, boxed);
   if (boxed)
      scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());

   _writer.saveObject(*scope.tape, ObjectInfo(okCurrent));

   size_t stackToFree = 0;
   compilePresavedMessageParameters(node, scope, mode, stackToFree);

   // skip all except the last message parameter
   while (node.nextNode() == nsMessageParameter)
      node = node.nextNode();

   return compileMessage(node, scope, object, encodeMessage(0, EVAL_MESSAGE_ID, count), mode);
}

ObjectInfo Compiler :: compileOperations(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   ObjectInfo currentObject = object;

   DNode member = node.nextNode();

   if (object.kind == okExternal) {
      recordDebugStep(scope, member.Terminal(), dsAtomicStep);
      currentObject = compileExternalCall(member, scope, node.Terminal(), mode);
      if (test(mode, HINT_TRY)) {
         // skip error handling for the external operation
         mode &= ~HINT_TRY;

         member = member.nextNode();
      }
      member = member.nextNode();
   }
   else if (object.kind == okInternal) {
      currentObject = compileInternalCall(member, scope, object, mode);

      member = member.nextNode();
   }

   bool catchMode = false;
   bool altMode = false;
   if (test(mode, HINT_TRY)) {
      if (test(mode, HINT_ALT))
         _writer.pushObject(*scope.tape, currentObject);

      _writer.declareTry(*scope.tape);

      mode &= ~HINT_ALT;
   }

   while (member != nsNone) {
      //_writer.declareBlock(*scope.tape);

      if (member == nsExtension) {
         currentObject = compileExtension(member, scope, currentObject, mode);

         continue;
      }
      else if (member==nsMessageOperation) {
         currentObject = compileMessage(member, scope, currentObject, mode);
      }
      else if (member==nsMessageParameter) {
         currentObject = compileEvalMessage(member, scope, currentObject, mode);
      }
      else if (member == nsSwitching) {
         compileSwitch(member, scope, currentObject);

         currentObject = ObjectInfo(okAccumulator);
      }
      else if (member == nsAssigning) {
         currentObject = compileAssigningExpression(node, member, scope, currentObject);
      }
      else if (member == nsAltMessageOperation) {
         if (!altMode) {
            _writer.declareAlt(*scope.tape);
            altMode = true;
         }
         _writer.loadObject(*scope.tape, ObjectInfo(okCurrent));
         currentObject = compileMessage(member, scope, ObjectInfo(okAccumulator), mode);
      }
      else if (member == nsCatchMessageOperation) {
         if (!catchMode) {
            _writer.declareCatch(*scope.tape);
            catchMode = true;
         }
         currentObject = compileMessage(member, scope, ObjectInfo(okAccumulator), mode | HINT_CATCH);
      }
      else if (member == nsL3Operation || member == nsL4Operation || member == nsL5Operation || member == nsL6Operation
         || member == nsL7Operation || member == nsL0Operation)
      {
         currentObject = compileOperator(member, scope, currentObject, mode);
      }

      //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

      member = member.nextNode();
   }

   if (catchMode) {
      _writer.endCatch(*scope.tape);
   }
   else if (altMode) {
      _writer.endAlt(*scope.tape);
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

            mode |= HINT_SELFEXTENDING;
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
         // it should be used directly
      }
      // if the symbol VMT can be used as an external role
      else if (test(flags, elStateless)) {
         role = ObjectInfo(okConstantRole, role.param);
      }
      else role = ObjectInfo(okRole);
   }
   else role = ObjectInfo(okRole);

   // override standard message compiling routine
   node = node.nextNode();

   //if (role.kind == okConstantRole)
   //   mode |= HINT_KNOWN_CALL;

   while (node==nsMessageOperation) {
      object = compileExtensionMessage(node, roleNode, scope, object, role, mode);

      node = node.nextNode();
   }

   return object;
}

ObjectInfo Compiler :: compileExtensionMessage(DNode& node, DNode& roleNode, CodeScope& scope, ObjectInfo object, ObjectInfo role, int mode)
{
   size_t spaceToRelease = 0;

   int paramMode = mode & ~HINT_SELFEXTENDING;

   size_t count = 0;
   ref_t messageRef = mapMessage(node, scope, count, mode);

   compileMessageParameters(node, scope, object, defineMethodHint(scope, role, messageRef), count, paramMode, spaceToRelease);

   mode |= paramMode;

   ObjectInfo retVal(okAccumulator);
   // if it is a not a constant or subject variable, compile a role
   if (role.kind != okConstantSymbol && role.kind != okSubject)
      _writer.loadObject(*scope.tape, compileObject(roleNode, scope, mode));

   // send message to the role
   retVal = compileMessage(node, scope, role, messageRef, mode);

   if (spaceToRelease > 0)
      _writer.releaseObject(*scope.tape, spaceToRelease);

   return retVal;
}

bool Compiler :: declareActionScope(DNode& node, ClassScope& scope, DNode argNode, ActionScope& methodScope, bool alreadyDeclared)
{
   bool lazyExpression = isReturnExpression(node.firstChild());
   //   bool stackSafeFunc = false;

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

   return lazyExpression;
}

void Compiler :: compileAction(DNode node, ClassScope& scope, DNode argNode, bool alreadyDeclared)
{
   _writer.declareClass(scope.tape, scope.reference);

   ActionScope methodScope(&scope);
   bool lazyExpression = declareActionScope(node, scope, argNode, methodScope, alreadyDeclared);

   // if it is single expression
   if (!lazyExpression) {
      compileActionMethod(node, methodScope, /*stackSafeFunc ? HINT_STACKSAFE_METH : */0);
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
   _writer.compile(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
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
   _writer.compile(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
   }
   else {
      bool dummy = false;
      int presaved = 0;

      // unbox all typed variables
      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         if (checkIfBoxingRequired(ownerScope, info, 0, mode)) {
            _writer.loadObject(*ownerScope.tape, info);
            boxObject(ownerScope, info, dummy);
            _writer.pushObject(*ownerScope.tape, ObjectInfo(okAccumulator));
            presaved++;
         }

         outer_it++;
      }

      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         _writer.newStructure(*ownerScope.tape, scope.info.size, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node.Terminal());
      }
      // dynamic normal symbol
      else _writer.newObject(*ownerScope.tape, scope.info.fields.Count(), scope.reference);

      _writer.loadBase(*ownerScope.tape, ObjectInfo(okAccumulator));

      outer_it = scope.outers.start();
      int toFree = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         //NOTE: info should be either fields or locals
         if (info.kind == okOuterField) {
            _writer.loadObject(*ownerScope.tape, info);
            _writer.saveBase(*ownerScope.tape, ObjectInfo(okAccumulator), (*outer_it).reference);
         }
         else if (info.kind == okParam || info.kind == okLocal || info.kind == okField || info.kind == okFieldAddress) {
            if (checkIfBoxingRequired(ownerScope, info, 0, mode)) {
               _writer.saveBase(*ownerScope.tape, ObjectInfo(okCurrent, --presaved), (*outer_it).reference);
               toFree++;
            }
            else _writer.saveBase(*ownerScope.tape, info, (*outer_it).reference);
         }
         else _writer.saveBase(*ownerScope.tape, info, (*outer_it).reference);

         outer_it++;
      }

      _writer.releaseObject(*ownerScope.tape, toFree);
      _writer.loadObject(*ownerScope.tape, ObjectInfo(okBase));

      return ObjectInfo(okAccumulator, scope.reference);
   }
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, int mode)
{
//   recordStep(ownerScope, node.Terminal(), dsStep);

   InlineClassScope scope(&ownerScope, mapNestedExpression(ownerScope, mode));

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
   return compileCollection(objectNode, scope, mode, scope.moduleScope->mapReference(scope.moduleScope->project->resolveForward(ARRAY_FORWARD)));
}

ObjectInfo Compiler :: compileCollection(DNode node, CodeScope& scope, int mode, ref_t vmtReference)
{
   int counter = 0;

   // all collection memebers should be created before the collection itself
   while (node != nsNone) {
      bool boxed = false;
      ObjectInfo current = compileExpression(node, scope, mode);
      current = boxObject(scope, current, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());

      _writer.pushObject(*scope.tape, current);

      node = node.nextNode();
      counter++;
   }

   // create the collection
   _writer.newObject(*scope.tape, counter, vmtReference);

   _writer.loadBase(*scope.tape, ObjectInfo(okAccumulator));

   // assign the members
   while (counter > 0) {
      _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));
      _writer.saveBase(*scope.tape, ObjectInfo(okAccumulator), counter - 1);

      counter--;
   }

   _writer.loadObject(*scope.tape, ObjectInfo(okBase));

   return ObjectInfo(okAccumulator);
}

ObjectInfo Compiler :: compileTypecast(CodeScope& scope, ObjectInfo object, ref_t target_type, bool& mismatch, bool& boxed, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   if (target_type == 0)
      return object;

   ref_t source_type = object.type;
   ref_t sourceClassReference = 0;
   if (object.kind == okConstantClass || object.kind == okLocalAddress) {
      sourceClassReference = object.extraparam;
   }
   else if (object.kind == okConstantSymbol) {
      sourceClassReference = object.extraparam;
   }
   else if (object.kind == okThisParam && (source_type == 0)) {
      sourceClassReference = scope.getClassRefId(false);
   }

   if (source_type == 0) {
      if (object.param != 0 && object.kind == okAccumulator) {
         sourceClassReference = object.param;
      }
      else if (object.extraparam != 0 && object.kind == okConstantSymbol) {
         sourceClassReference = object.extraparam;
      }
   }

   if (sourceClassReference == 0 && source_type != 0) {
      sourceClassReference = scope.moduleScope->typeHints.get(source_type);
   }

   if (target_type != source_type) {
      object.type = target_type;

      if (moduleScope->typeHints.exist(target_type)) {
         // typecast literal constant
         if (object.kind == okLiteralConstant && moduleScope->typeHints.exist(target_type, moduleScope->literalReference)) {
            return object;
         }

         ref_t targetClassReference = moduleScope->typeHints.get(target_type);
         ClassInfo targetInfo;
         moduleScope->loadClassInfo(targetInfo, scope.moduleScope->module->resolveReference(targetClassReference), false);

         ClassInfo sourceInfo;
         if (sourceClassReference != 0)
            moduleScope->loadClassInfo(sourceInfo, scope.moduleScope->module->resolveReference(sourceClassReference), false);

         // if the target is structure
         if (test(targetInfo.header.flags, elStructureRole)) {
            // typecast numeric constant
            if (object.kind == okIntConstant && (targetInfo.header.flags & elDebugMask) == elDebugDWORD) {
               return object;
            }
            else if (object.kind == okLongConstant && (targetInfo.header.flags & elDebugMask) == elDebugQWORD) {
               return object;
            }
            else if (object.kind == okRealConstant && (targetInfo.header.flags & elDebugMask) == elDebugReal64) {
               return object;
            }
            else if (object.kind == okCharConstant && moduleScope->typeHints.exist(target_type, moduleScope->charReference)) {
               return object;
            }
            else if (object.kind == okSignatureConstant && moduleScope->typeHints.exist(target_type, moduleScope->signatureReference)) {
               return ObjectInfo(okAccumulator, 0, target_type);
            }
//            else if (object.kind == okVerbConstant && test(targetInfo.header.flags, elMessage)) {
//               return ObjectInfo(okAccumulator, 0, target_type);
//            }

            // if the source is structure
            if (test(sourceInfo.header.flags, elStructureRole)) {
               // if source is target wrapper
               if (test(sourceInfo.header.flags, elStructureWrapper) && moduleScope->typeHints.exist(sourceInfo.fieldTypes.get(0), targetClassReference)) {
                  ObjectInfo primitive(okLocal, 0, 0, target_type);
                  allocateStructure(scope, 0, primitive);

                  compileContentAssignment(DNode(), scope, primitive, object);

                  return primitive;
               }
               // if target is a source wrapper
               if(isLocal(object) && test(targetInfo.header.flags, elStructureWrapper) && moduleScope->typeHints.exist(targetInfo.fieldTypes.get(0), sourceClassReference)) {
                  if (test(targetInfo.header.flags, elEmbeddable)) {

                     return object;
                  }
               }
            }
         }

         // pass $nil directly
         if (object.kind == okNil)
            return object;

         // if source class inherites / is target class
         while (sourceClassReference != 0) {
            if (moduleScope->typeHints.exist(target_type, sourceClassReference))
               return object;

            sourceClassReference = sourceInfo.header.parentRef;

            if (moduleScope->loadClassInfo(sourceInfo, moduleScope->module->resolveReference(sourceClassReference), true) == 0)
               break;
         }

         // if type mismatch
         // call typecast method
         mismatch = true;

         // the parameter should be boxed before
         boxObject(scope, object, boxed);

         _writer.setMessage(*scope.tape, encodeMessage(target_type, GET_MESSAGE_ID, 0));
         _writer.typecast(*scope.tape);

         return ObjectInfo(okAccumulator, 0, 0, target_type);
      }
   }
   
   return object;
}

ObjectInfo Compiler :: compileRetExpression(DNode node, CodeScope& scope, int mode)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   ObjectInfo info = compileExpression(node, scope, mode);

   _writer.loadObject(*scope.tape, info);

   // type cast returning value if required
   int verb, paramCount;
   ref_t subj;
   decodeMessage(scope.getMessageID(), subj, verb, paramCount);
   if (verb == GET_MESSAGE_ID && paramCount == 0) {
   }
   else if (classScope->info.methodHints.exist(scope.getMessageID())) {
      subj = classScope->info.methodHints.get(scope.getMessageID()).typeRef;
   }
   else subj = 0;

   if (subj != 0 && scope.moduleScope->typeHints.get(subj) > 0) {
      bool mismatch = false;
      bool boxed = false;
      compileTypecast(scope, info, subj, mismatch, boxed, 0);
      if (mismatch)
         scope.raiseWarning(2, wrnTypeMismatch, node.FirstTerminal());
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());
   }
   else {
      // box object if required
      bool boxed = false;
      info = boxObject(scope, info, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());
   }

   scope.freeSpace();

   //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   return ObjectInfo(okAccumulator, 0, 0, subj);
}

ObjectInfo Compiler :: compileExpression(DNode node, CodeScope& scope, int mode)
{
   DNode member = node.firstChild();

   ObjectInfo objectInfo;
   if (member==nsObject) {
      objectInfo = compileObject(member, scope, mode);
   }
   if (member != nsNone) {
      if (findSymbol(member, nsCatchMessageOperation)) {
         objectInfo = compileOperations(member, scope, objectInfo, (mode | HINT_TRY));
      }
      else if (findSymbol(member, nsAltMessageOperation)) {
         objectInfo = compileOperations(member, scope, objectInfo, (mode | HINT_ALT));
      }
      else objectInfo = compileOperations(member, scope, objectInfo, mode);
   }

   return objectInfo;
}

ObjectInfo Compiler :: compileAssigningExpression(DNode node, DNode assigning, CodeScope& scope, ObjectInfo target)
{
   // if primitive data operation can be used   
   if (target.kind == okLocalAddress || target.kind == okFieldAddress) {
      // if it is an assignment operation (e.g. a := a + b <=> a += b)
      // excluding structure fields (except the first one)
      int assignMode = ((target.kind != okFieldAddress || target.param == 0) && isAssignOperation(node, assigning)) ? HINT_ASSIGN_MODE : 0;

      ObjectInfo info = compileExpression(assigning.firstChild(), scope, assignMode);

      if (info.kind == okIndexAccumulator) {
         // if it is a primitive operation
         compileContentAssignment(node, scope, target, info);
      }
      else if (info.kind == okIdle) {
         // if assigning was already done - do nothing
      }
      else {
         _writer.loadObject(*scope.tape, info);

         compileContentAssignment(node, scope, target, info);
      }
   }
   else {
      ObjectInfo info = compileExpression(assigning.firstChild(), scope, 0);

      _writer.loadObject(*scope.tape, info);

      bool boxed = false;
      info = boxObject(scope, info, boxed);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, assigning.firstChild().FirstTerminal());

      bool mismatch = false;
      compileTypecast(scope, info, target.type, mismatch, boxed, 0);
      if (mismatch)
         scope.raiseWarning(2, wrnTypeMismatch, node.Terminal());

      compileAssignment(node, scope, target);
   }

   return ObjectInfo(okAccumulator);
}

ObjectInfo Compiler :: compileBranching(DNode thenNode, CodeScope& scope, ObjectInfo target, int verb, int subCodeMode)
{
   // execute the block if the condition
   // is true / false
   if (verb == IF_MESSAGE_ID || verb == IFNOT_MESSAGE_ID) {
      ref_t valueRef = (verb == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference;

      bool mismatch = false;
      bool boxed = false;
      compileTypecast(scope, target, scope.moduleScope->boolType, mismatch, boxed, 0);

      _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

      _writer.jumpIfNotEqual(*scope.tape, valueRef);
   }

   //_writer.declareBlock(*scope.tape);

   CodeScope subScope(&scope);

   DNode thenCode = thenNode.firstChild();
   if (thenCode.firstChild().nextNode() != nsNone) {
      compileCode(thenCode, subScope, subCodeMode & ~HINT_LOOP);

      if (test(subCodeMode, HINT_LOOP) && subScope.level > scope.level) {
         _writer.releaseObject(*scope.tape, subScope.level - scope.level);
      }
   }
   // if it is inline action
   else compileRetExpression(thenCode.firstChild(), scope, 0);

   return ObjectInfo(okAccumulator);
}

void Compiler :: compileThrow(DNode node, CodeScope& scope, int mode)
{
   ObjectInfo object = compileExpression(node.firstChild(), scope, mode);

   //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   _writer.pushObject(*scope.tape, object);
   _writer.throwCurrent(*scope.tape);
}

void Compiler :: compileLoop(DNode node, CodeScope& scope, int mode)
{
   _writer.declareLoop(*scope.tape/*, true*/);

   DNode expr = node.firstChild().firstChild();

   // if it is while-do loop
   if (expr.nextNode() == nsL7Operation) {
      DNode loopNode = expr.nextNode();

      openDebugExpression(scope);
      ObjectInfo cond = compileObject(expr, scope, 0);

      //remove last virtual breakpoint
      //_writer.removeLastBreakpoint(*scope.tape);

      // get the current value
      _writer.loadObject(*scope.tape, cond);

      compileBranching(loopNode, scope, cond, _operators.get(loopNode.Terminal()), HINT_LOOP);

      endDebugExpression(scope);
      _writer.endLoop(*scope.tape);
   }
   // if it is repeat loop
   else {
      ObjectInfo retVal = compileExpression(expr, scope, 0);

      //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

      bool mismatch = false;
      bool boxed = false;
      compileTypecast(scope, retVal, scope.moduleScope->boolType, mismatch, boxed, 0);
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, expr.FirstTerminal());

      _writer.endLoop(*scope.tape, scope.moduleScope->trueReference);
   }
   //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);
}

ObjectInfo Compiler :: compileCode(DNode node, CodeScope& scope, int mode)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   DNode statement = node.firstChild();

   // skip subject argument
   while (statement == nsSubjectArg || statement == nsMethodParameter)
      statement= statement.nextNode();

   while (statement != nsNone) {
      DNode hints = skipHints(statement);

      //_writer.declareStatement(*scope.tape);

      switch(statement) {
         case nsExpression:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            openDebugExpression(scope);
            compileExpression(statement, scope, 0);
            endDebugExpression(scope);
            break;
         case nsThrow:
            compileThrow(statement, scope, 0);
            break;
         case nsLoop:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileLoop(statement, scope, HINT_LOOP);
            break;
         case nsRetStatement:
         {
            needVirtualEnd = false;
            recordDebugStep(scope, statement.firstChild().FirstTerminal(), dsStep);
            openDebugExpression(scope);
            retVal = compileRetExpression(statement.firstChild(), scope, 0);
            endDebugExpression(scope);
            scope.freeSpace();

            _writer.gotoEnd(*scope.tape, baFirstLabel);
            break;
         }
         case nsVariable:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            openDebugExpression(scope);
            compileVariable(statement, scope, hints);
            endDebugExpression(scope);
            break;
         case nsCodeEnd:
            needVirtualEnd = false;
            recordDebugStep(scope, statement.Terminal(), dsEOP);
            break;
      }
      scope.freeSpace();

      statement = statement.nextNode();
   }

   if (needVirtualEnd)
      _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   return retVal;
}

void Compiler :: compileExternalArguments(DNode arg, CodeScope& scope, ExternalScope& externalScope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();

      ExternalScope::ParamInfo param;
      param.subject = moduleScope->mapType(terminal);

      ref_t classReference = moduleScope->typeHints.get(param.subject);
      int flags = 0;
      // HOTFIX: problem with using a strong type inside its wrapper
      if (scope.getClassRefId() == classReference) {
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

         if (classScope->info.size == 0)
            scope.raiseError(errInvalidOperation, terminal);

         flags = classScope->info.header.flags;
      }
      else {
         ClassInfo classInfo;
         if (moduleScope->loadClassInfo(classInfo, moduleScope->module->resolveReference(classReference), true) == 0)
            scope.raiseError(errInvalidOperation, terminal);

         if (classInfo.size == 0)
            scope.raiseError(errInvalidOperation, terminal);

         flags = classInfo.header.flags;
      }
      // if it is an integer number pass it directly
      if ((flags & elDebugMask) == elDebugDWORD) {
         param.size = 4;
         if (!test(flags, elReadOnlyRole))
            param.out = true;
      }
      else if ((flags & elDebugMask) == elDebugPTR) {
         param.size = 4;
      }
      else if ((flags & elDebugMask) == elDebugReference) {
         param.size = -2;
      }
      else param.size = -1;

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
         param.info = compileObject(arg.firstChild(), scope, HINT_EXTERNAL_CALL);
         if (param.info.kind == okThisParam && moduleScope->typeHints.exist(param.subject, scope.getClassRefId())) {
            param.info.extraparam = param.subject;
         }

         if (param.size == -2 && param.info.kind == okInternal) {
         }
         else if ((param.size == 4 && param.info.kind == okIntConstant)/* || (param.subject == intPtrType && param.info.kind == okSymbolReference)*/) {
            // if direct pass is possible
         }
         else if (param.size == 4 && param.info.kind == okFieldAddress && param.subject == param.info.type) {
            // if direct pass is possible
         }
         else {
            bool mismatch = false;
            bool boxed = false;

            _writer.loadObject(*scope.tape, param.info);
            if (param.info.kind == okFieldAddress) {
               param.info = boxStructureField(scope, param.info, ObjectInfo(okThisParam, 1));
            }

            param.info = compileTypecast(scope, param.info, param.subject, mismatch, boxed, 0);
            if (mismatch)
               scope.raiseWarning(2, wrnTypeMismatch, arg.firstChild().FirstTerminal());
            if (boxed)
               scope.raiseWarning(4, wrnBoxingCheck, arg.firstChild().FirstTerminal());

            _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));

            param.info.kind = okBlockLocal;
            param.info.param = ++externalScope.frameSize;
         }

         arg = arg.nextNode();
      }
      else scope.raiseError(errInvalidOperation, terminal);

      externalScope.operands.push(param);
   }
}

void Compiler :: saveExternalParameters(CodeScope& scope, ExternalScope& externalScope)
{
   ModuleScope* moduleScope = scope.moduleScope;

//   ref_t actionType = moduleScope->getActionType();

   // save function parameters
   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   while (!out_it.Eof()) {
      // if it is output parameter
      if ((*out_it).out) {
         _writer.pushObject(*scope.tape, (*out_it).info);
      }
      else {
         if ((*out_it).size == 4) {
            if ((*out_it).info.kind == okIntConstant) {
               int value = StringHelper::strToULong(moduleScope->module->resolveConstant((*out_it).info.param), 16);

               externalScope.frameSize++;
               _writer.declareVariable(*scope.tape, value);
            }
            else if ((*out_it).info.kind == okFieldAddress && (*out_it).subject == (*out_it).info.type) {
               _writer.loadObject(*scope.tape, ObjectInfo(okThisParam, 1));
               _writer.loadInt(*scope.tape, (*out_it).info);
               _writer.pushObject(*scope.tape, ObjectInfo(okIndexAccumulator));
            }
            else {
               _writer.loadObject(*scope.tape, (*out_it).info);
               _writer.pushObject(*scope.tape, ObjectInfo(okAccField, 0));
            }
         }
         // if it is an internal reference
         else if ((*out_it).size == -2) {
            _writer.loadSymbolReference(*scope.tape, (*out_it).info.param);
            _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
         }
         else _writer.pushObject(*scope.tape, (*out_it).info);
      }

      out_it++;
   }
}

ObjectInfo Compiler :: compileExternalCall(DNode node, CodeScope& scope, ident_t dllAlias, int mode)
{
   ObjectInfo retVal(okIndexAccumulator);

   ModuleScope* moduleScope = scope.moduleScope;

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

   // compile argument list
   ExternalScope externalScope;

   _writer.declareExternalBlock(*scope.tape);

   compileExternalArguments(node.firstChild(), scope, externalScope);

   // exclude stack if necessary
   _writer.excludeFrame(*scope.tape);

   // save function parameters
   saveExternalParameters(scope, externalScope);

   // call the function
   _writer.callExternal(*scope.tape, reference, externalScope.frameSize);

   if (!stdCall)
      _writer.releaseObject(*scope.tape, externalScope.operands.Count());

   //// indicate that the result is 0 or -1
   //if (test(mode, HINT_LOOP))
   //   retVal.extraparam = scope.moduleScope->intSubject;

   // error handling should follow the function call immediately
   if (test(mode, HINT_TRY))
      compilePrimitiveCatch(node.nextNode(), scope);

  _writer.endExternalBlock(*scope.tape);

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(DNode node, CodeScope& scope, ObjectInfo routine, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // only eval message is allowed
   TerminalInfo     verb = node.Terminal();
   if (_verbs.get(verb) != EVAL_MESSAGE_ID)
      scope.raiseError(errInvalidOperation, verb);

   DNode arg = node.firstChild();
   int count = countSymbol(arg, nsMessageParameter);
   _writer.declareArgumentList(*scope.tape, count);

   int index = 0;
   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();
      ref_t type = moduleScope->mapType(terminal);

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
         ObjectInfo info = compileObject(arg.firstChild(), scope, 0);
         _writer.loadObject(*scope.tape, info);

         bool mismatch = false;
         bool boxed = false;
         compileTypecast(scope, info, type, mismatch, boxed, 0);
         if (mismatch)
            scope.raiseWarning(2, wrnTypeMismatch, arg.FirstTerminal());
         if (boxed)
            scope.raiseWarning(4, wrnBoxingCheck, arg.FirstTerminal());

         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, index));
         index++;
      }
      else scope.raiseError(errInvalidOperation, terminal);

      arg = arg.nextNode();
   }

   _writer.loadObject(*scope.tape, routine);
   _writer.freeVirtualStack(*scope.tape, count);

   return ObjectInfo(okAccumulator);
}

void Compiler :: reserveSpace(CodeScope& scope, int size)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // if it is not enough place to allocate
   if (methodScope->reserved < scope.reserved) {
      ByteCodeIterator allocStatement = scope.tape->find(bcOpen);
      // reserve place for stack allocated object
      (*allocStatement).argument += size;

      // if stack was not allocated before
      // update method enter code
      if (methodScope->reserved == 0) {
         // to include new frame header
         (*allocStatement).argument += 2;

         _writer.insertStackAlloc(allocStatement, *scope.tape, size);
      }
      // otherwise update the size
      else _writer.updateStackAlloc(allocStatement, *scope.tape, size);

      methodScope->reserved += size;
   }
}

bool Compiler :: allocateStructure(CodeScope& scope, int dynamicSize, ObjectInfo& exprOperand, bool presavedAccumulator)
{
   bool bytearray = false;
   int size = 0;
   ref_t classReference = 0;
   if (exprOperand.kind == okAccumulator && exprOperand.param != 0) {
      classReference = exprOperand.param;
      size = scope.moduleScope->defineStructSize(classReference);
   }
   else size = scope.moduleScope->defineTypeSize(exprOperand.type, classReference);

   if (size == 0)
      return false;

   if (size < 0) {
      bytearray = true;

      // plus space for size
      size = ((dynamicSize + 3) >> 2) + 2;
   }
   else if (exprOperand.kind == okIndexAccumulator) {
      size = 1;
   }
   else size = (size + 3) >> 2;

   if (size > 0) {
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      exprOperand.kind = okLocalAddress;
      exprOperand.param = scope.newSpace(size);
      exprOperand.extraparam = classReference;

      // allocate
      reserveSpace(scope, size);

      // reserve place for byte array header if required
      if (bytearray) {
         if (presavedAccumulator)
            _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));

         _writer.loadObject(*scope.tape, exprOperand);
         _writer.saveIntConstant(*scope.tape, -dynamicSize);

         if (presavedAccumulator)
            _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));

         exprOperand.param -= 2;
      }

      return true;
   }
   else return false;
}

//inline void copySubject(_Module* module, ReferenceNs& signature, ref_t type)
//{
//   signature.append(module->resolveSubject(type));
//}

//inline bool IsVarOperation(int operator_id)
//{
//   switch (operator_id) {
//      case WRITE_MESSAGE_ID:
//      case APPEND_MESSAGE_ID:
//      case REDUCE_MESSAGE_ID:
//      case SEPARATE_MESSAGE_ID:
//      case INCREASE_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}

ObjectInfo Compiler :: compilePrimitiveCatch(DNode node, CodeScope& scope)
{
   _writer.declarePrimitiveCatch(*scope.tape);

   size_t size = 0;
   ref_t message = mapMessage(node, scope, size);
   if (message == encodeMessage(0, RAISE_MESSAGE_ID, 1)) {
      compileThrow(node.firstChild().firstChild().firstChild(), scope, 0);
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());

   _writer.endPrimitiveCatch(*scope.tape);

   return ObjectInfo(okIndexAccumulator);
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

void Compiler :: declareArgumentList(DNode node, MethodScope& scope)
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

   // if signature is presented
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   scope.message = encodeMessage(sign_id, verb_id, paramCount);
}

void Compiler :: compileDispatcher(DNode node, MethodScope& scope, bool withGenericMethods)
{
   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   CodeScope codeScope(&scope);

   _writer.declareIdleMethod(*codeScope.tape, scope.message);

   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, codeScope.tape, node.Terminal());
   }
   else {
      _writer.doGenericHandler(*codeScope.tape);

      // if it is generic handler with redirect statement / redirect statement
      if (node != nsNone) {
         if (withGenericMethods) {
            _writer.pushObject(*codeScope.tape, ObjectInfo(okExtraRegister));
            _writer.setSubject(*codeScope.tape, encodeMessage(codeScope.moduleScope->mapSubject(GENERIC_PREFIX), 0, 0));
            _writer.doGenericHandler(*codeScope.tape);
            _writer.popObject(*codeScope.tape, ObjectInfo(okExtraRegister));
         }
         DNode nextNode = node.nextNode();

         // !! currently only simple construction is supported
         if (node == nsObject && node.firstChild() == nsNone && nextNode == nsNone) {
            TerminalInfo target = node.Terminal();

            ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
            // if the target is a data field
            if (test(classScope->info.header.flags, elStructureRole) && classScope->info.fields.exist(target)) {
               int offset = classScope->info.fields.get(target);

               _writer.pushObject(*codeScope.tape, ObjectInfo(okExtraRegister));

               boxStructureField(codeScope, 
                  ObjectInfo(okFieldAddress, offset, 0, classScope->info.fieldTypes.get(offset)), ObjectInfo(okAccumulator), HINT_HEAP_MODE);

               _writer.popObject(*codeScope.tape, ObjectInfo(okExtraRegister));

               _writer.resend(*codeScope.tape, ObjectInfo(okAccumulator), 0);
            }
            else _writer.resend(*codeScope.tape, compileTerminal(node, codeScope, 0), 0);
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
      // if it is generic handler only
      else if (withGenericMethods) {
         _writer.pushObject(*codeScope.tape, ObjectInfo(okExtraRegister));
         _writer.setSubject(*codeScope.tape, encodeMessage(codeScope.moduleScope->mapSubject(GENERIC_PREFIX), 0, 0));
         _writer.resendResolvedMethod(*codeScope.tape, scope.moduleScope->superReference, encodeVerb(DISPATCH_MESSAGE_ID));
      }
   }

   _writer.endIdleMethod(*codeScope.tape);
}

void Compiler :: compileActionMethod(DNode node, MethodScope& scope, int mode)
{
   // check if the method is inhreited and update vmt size accordingly
   if(scope.include() && test(scope.getClassFlag(), elClosed))
      scope.raiseError(errClosedParent, node.Terminal());

   CodeScope codeScope(&scope);

   // new stack frame
   // stack already contains previous $self value
   _writer.declareMethod(*codeScope.tape, scope.message, false);
   codeScope.level++;

   declareParameterDebugInfo(scope, codeScope.tape, false, true);

   if (isReturnExpression(node.firstChild())) {
      compileRetExpression(node.firstChild(), codeScope, 0);
   }
   else if (node == nsInlineExpression) {
      // !! this check should be removed, as it is no longer used
      compileCode(node.firstChild(), codeScope);
   }
   else compileCode(node, codeScope);

   _writer.endMethod(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);
}

void Compiler :: compileLazyExpressionMethod(DNode node, MethodScope& scope)
{
   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   // stack already contains previous $self value
   CodeScope codeScope(&scope);

   // new stack frame
   // stack already contains previous $self value
   _writer.declareMethod(*codeScope.tape, scope.message, false);
   codeScope.level++;

   declareParameterDebugInfo(scope, codeScope.tape, false, false);

   compileRetExpression(node, codeScope, 0);

   _writer.endMethod(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);
}

void Compiler :: compileDispatchExpression(DNode node, CodeScope& scope)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // try to implement light-weight resend operation
   if (node.firstChild() == nsNone && node.nextNode() == nsNone) {
      ObjectInfo target = compileTerminal(node, scope, 0);
      if (target.kind == okConstantSymbol || target.kind == okField) {
         _writer.declareMethod(*scope.tape, methodScope->message, false, false);

         if (target.kind == okField) {
            _writer.loadObject(*scope.tape, ObjectInfo(okAccField, target.param));
         }
         else _writer.loadObject(*scope.tape, target);

         _writer.resend(*scope.tape);

         _writer.endMethod(*scope.tape, getParamCount(methodScope->message) + 1, methodScope->reserved, false);

         return;
      }
   }

   // new stack frame
   // stack already contains previous $self value
   _writer.declareMethod(*scope.tape, methodScope->message, false, true);
   scope.level++;

   // copy arguments
   int param_count = getParamCount(methodScope->message);
   while (param_count > 0) {
      _writer.pushObject(*scope.tape, ObjectInfo(okParam, -1 - param_count));
      param_count--;
   }

   _writer.pushObject(*scope.tape, ObjectInfo(okParam, -1));
   _writer.pushObject(*scope.tape, ObjectInfo(okExtraRegister));

   ObjectInfo target = compileObject(node, scope, 0);

   bool boxed = false;
   boxObject(scope, target, boxed);
   if (boxed)
      scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());      

   _writer.loadObject(*scope.tape, target);

   _writer.popObject(*scope.tape, ObjectInfo(okExtraRegister));

   _writer.callRoleMessage(*scope.tape, getParamCount(methodScope->message));

   _writer.endMethod(*scope.tape, getParamCount(methodScope->message) + 1, methodScope->reserved, true);
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
         _writer.newFrame(*scope.tape);
         scope.level++;
      }

      DNode dummy;
      if (withFrame) {
         compileExtensionMessage(node, dummy, scope, ObjectInfo(okThisParam, 1), ObjectInfo(okConstantSymbol, classRef, classRef), HINT_STACKSAFE_CALL);
      }
      else compileExtensionMessage(node, dummy, scope, ObjectInfo(okAccumulator), ObjectInfo(okConstantSymbol, classRef, classRef), HINT_STACKSAFE_CALL);
   }
   else scope.raiseError(errUnknownMessage, node.Terminal());
}

void Compiler :: compileConstructorDispatchExpression(DNode node, CodeScope& scope, ClassScope& classClassScope)
{
   if (node.firstChild() == nsNone) {
      ObjectInfo info = scope.mapObject(node.Terminal());
      // if it is an internal routine
      if (info.kind == okInternal) {
         importCode(node, *scope.moduleScope, scope.tape, node.Terminal());
      }
      // if it is a wrapper dispatching
      else if (info.kind == okLocal && info.param == 1 && info.type != 0 && test(scope.getClassFlags(), elStructureWrapper)) {
         // load a data class class
         ref_t classReference = scope.moduleScope->typeHints.get(info.type);

         ClassInfo info;
         // check if the object can be treated like a constant object
         ref_t r = scope.moduleScope->loadClassInfo(info, scope.moduleScope->module->resolveReference(classReference), true);
         if (r) {
            _writer.resendResolvedMethod(*scope.tape, info.classClassRef, scope.getMessageID());
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileResendExpression(DNode node, CodeScope& scope)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // new stack frame
   // stack already contains current $self reference
   _writer.declareMethod(*scope.tape, methodScope->message, false, true);
   scope.level++;

   compileMessage(node, scope, ObjectInfo(okThisParam, 1/*, methodScope->getClassType()*/), 0);
   scope.freeSpace();

   //_writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   _writer.endMethod(*scope.tape, getParamCount(methodScope->message) + 1, methodScope->reserved, true);
}

void Compiler :: compileImportMethod(DNode node, ClassScope& scope, ref_t message, ident_t function)
{
   MethodScope methodScope(&scope);
   methodScope.message = message;

   // check if the method is inhreited and update vmt size accordingly
   if(methodScope.include() && test(methodScope.getClassFlag(), elClosed))
      scope.raiseError(errClosedParent, node.Terminal());

   CodeScope codeScope(&methodScope);

   compileImportMethod(node, codeScope, message, function, 0);
}

void Compiler :: compileImportMethod(DNode node, CodeScope& codeScope, ref_t message, ident_t function, int mode)
{
   _writer.declareIdleMethod(*codeScope.tape, message);
   importCode(node, *codeScope.moduleScope, codeScope.tape, function);
   _writer.endIdleMethod(*codeScope.tape);
}

void Compiler :: compileMethod(DNode node, MethodScope& scope, int mode)
{
   int paramCount = getParamCount(scope.message);

   CodeScope codeScope(&scope);

   // save extensions if any
   if (test(codeScope.getClassFlags(false), elExtension)) {
      codeScope.moduleScope->saveExtension(scope.message, codeScope.getExtensionType(), codeScope.getClassRefId());
   }

   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   // check if it is a resend
   if (resendBody != nsNone) {
      compileResendExpression(resendBody.firstChild(), codeScope);
   }
   // check if it is a dispatch
   else if (dispatchBody != nsNone) {
      if (isImportRedirect(dispatchBody.firstChild())) {
         compileImportMethod(dispatchBody.firstChild(), codeScope, scope.message, dispatchBody.firstChild().Terminal(), mode);
      }
      else compileDispatchExpression(dispatchBody.firstChild(), codeScope);
   }
   else {
      // new stack frame
      // stack already contains current $self reference
      // the original message should be restored if it is a generic method
      _writer.declareMethod(*codeScope.tape, scope.message, test(mode, HINT_GENERIC_METH));
      codeScope.level++;
      // declare the current subject for a generic method
      if (test(mode, HINT_GENERIC_METH)) {
         _writer.copySubject(*codeScope.tape);
         _writer.pushObject(*codeScope.tape, ObjectInfo(okIndexAccumulator));
         codeScope.level++;
         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0);
      }

      declareParameterDebugInfo(scope, codeScope.tape, true, test(codeScope.getClassFlags(), elRole));

      DNode body = node.select(nsSubCode);
      // if method body is a returning expression
      if (body==nsNone) {
         compileCode(node, codeScope);
      }
      // if method body is a set of statements
      else {
         ObjectInfo retVal = compileCode(body, codeScope);

         if(retVal.kind == okUnknown) {
            _writer.loadObject(*codeScope.tape, ObjectInfo(okThisParam, 1));

            ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
            bool mismatch = false;
            bool boxed = false;
            compileTypecast(codeScope, ObjectInfo(okAccumulator), classScope->info.methodHints.get(scope.message).typeRef, mismatch, boxed, 0);
            if (mismatch)
               scope.raiseWarning(2, wrnTypeMismatch, goToSymbol(body.firstChild(), nsCodeEnd).Terminal());
            if (boxed)
               scope.raiseWarning(4, wrnBoxingCheck, goToSymbol(body.firstChild(), nsCodeEnd).Terminal());
         }
      }

      int stackToFree = paramCount + scope.rootToFree;

   //   if (scope.testMode(MethodScope::modLock)) {
   //      _writer.endSyncMethod(*codeScope.tape, -1);
   //   }
      _writer.endMethod(*codeScope.tape, stackToFree, scope.reserved);
   }

//   // critical section entry if sync hint declared
//   if (scope.testMode(MethodScope::modLock)) {
//      ownerScope->info.header.flags |= elWithLocker;
//
//      _writer.tryLock(*codeScope.tape, 1);
//   }
//   // safe point (no need in extra one because lock already has one safe point)
//   else if (scope.testMode(MethodScope::modSafePoint))
//      _writer.declareSafePoint(*codeScope.tape);
}

void Compiler :: compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, int mode)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   CodeScope codeScope(&scope);

   // HOTFIX: constructor is declared in class class but should be executed if the class instance
   codeScope.tape = &classClassScope.tape;

   DNode body = node.select(nsSubCode);
   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   _writer.declareMethod(*codeScope.tape, scope.message, false, false);

   bool withFrame = false;
   if (resendBody != nsNone) {
      compileConstructorResendExpression(resendBody.firstChild(), codeScope, classClassScope, withFrame);
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (!test(codeScope.getClassFlags(), elDynamicRole)) {
      // HOTFIX: -1 indicates the stack is not consumed by the constructor
      _writer.callMethod(*codeScope.tape, 1, -1);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else if (dispatchBody == nsNone)
      scope.raiseError(errIllegalConstructor, node.Terminal());

   if (dispatchBody != nsNone) {
      compileConstructorDispatchExpression(dispatchBody.firstChild(), codeScope, classClassScope);
      _writer.endIdleMethod(*codeScope.tape);
      // NOTE : import code already contains quit command, so do not call "endMethod"
      return;
   }
   else if (body != nsNone) {
      if (!withFrame) {
         withFrame = true;

         // new stack frame
         // stack already contains $self value
         _writer.newFrame(*codeScope.tape);
         codeScope.level++;
      }
      else _writer.saveObject(*codeScope.tape, ObjectInfo(okThisParam, 1));

      declareParameterDebugInfo(scope, codeScope.tape, true, false);

      compileCode(body, codeScope);

      _writer.loadObject(*codeScope.tape, ObjectInfo(okThisParam, 1));
   }

   _writer.endMethod(*codeScope.tape, getParamCount(scope.message) + 1, scope.reserved, withFrame);
}

void Compiler :: compileDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   CodeScope codeScope(&scope);

   // compile constructor hints
   //int mode = scope.compileHints(hints);

   // HOTFIX: constructor is declared in class class but should be executed if the class instance
   codeScope.tape = &classClassScope.tape;

   _writer.declareIdleMethod(*codeScope.tape, scope.message);

   if (test(classScope->info.header.flags, elStructureRole)) {
      if (!test(classScope->info.header.flags, elDynamicRole)) {
         _writer.newStructure(*codeScope.tape, classScope->info.size, classScope->reference);
      }
   }
   else if (!test(classScope->info.header.flags, elDynamicRole)) {
      _writer.newObject(*codeScope.tape, classScope->info.fields.Count(), classScope->reference);
      size_t fieldCount = classScope->info.fields.Count();
      if (fieldCount > 0) {
         _writer.loadBase(*codeScope.tape, ObjectInfo(okAccumulator));
         _writer.loadObject(*codeScope.tape, ObjectInfo(okNil));
         _writer.initBase(*codeScope.tape, fieldCount);
         _writer.loadObject(*codeScope.tape, ObjectInfo(okBase));
      }
   }

   _writer.exitMethod(*codeScope.tape, 0, 0, false);

   _writer.endIdleMethod(*codeScope.tape);
}

void Compiler :: compileDynamicDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   CodeScope codeScope(&scope);

   // compile constructor hints
   //int mode = scope.compileHints(hints);

   // HOTFIX: constructor is declared in class class but should be executed if the class instance
   codeScope.tape = &classClassScope.tape;

   _writer.declareIdleMethod(*codeScope.tape, scope.message);

   if (test(classScope->info.header.flags, elStructureRole)) {
      _writer.loadObject(*codeScope.tape, ObjectInfo(okConstantClass, classScope->reference));
      switch(classScope->info.size) {
         case -1:
            _writer.newDynamicStructure(*codeScope.tape, 1);
            break;
         case -2:
            _writer.newDynamicWStructure(*codeScope.tape);
            break;
         case -4:
            _writer.newDynamicNStructure(*codeScope.tape);
            break;
         default:
            _writer.newDynamicStructure(*codeScope.tape, -classScope->info.size);
            break;
      }
   }
   else {
      _writer.loadObject(*codeScope.tape, ObjectInfo(okConstantClass, classScope->reference));
      _writer.newDynamicObject(*codeScope.tape);
   }

   _writer.exitMethod(*codeScope.tape, 0, 0, false);

   _writer.endIdleMethod(*codeScope.tape);
}

void Compiler :: compileVMT(DNode member, ClassScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      switch(member) {
         case nsMethod:
         {
            MethodScope methodScope(&scope);

            // if it is a dispatch handler
            if (member.firstChild() == nsDispatchHandler) {
               if (test(scope.info.header.flags, elRole))
                  scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
               methodScope.stackSafe = test(scope.info.methodHints.get(methodScope.message).hint, tpStackSafe);

               compileDispatcher(member.firstChild().firstChild(), methodScope, test(scope.info.header.flags, elWithGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(member, methodScope);
               methodScope.stackSafe = test(scope.info.methodHints.get(methodScope.message).hint, tpStackSafe);

               compileMethod(member, methodScope, methodScope.compileHints(hints));
            }
            break;
         }
         case nsDefaultGeneric:
         {
            MethodScope methodScope(&scope);
            declareArgumentList(member, methodScope);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->mapSubject(GENERIC_PREFIX));

            // mark as having generic methods
            scope.info.header.flags |= elWithGenerics;

            compileMethod(member, methodScope, HINT_GENERIC_METH);
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
         if (!testany(scope.info.header.flags, elStructureRole | elNonStructureRole | elDynamicRole) && !findSymbol(member.nextNode(), nsField)
            && test(scope.info.header.flags, elSealed) && sizeValue != 0)
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
   // creates implicit symbol
   SymbolScope symbolScope(scope.moduleScope, scope.reference);

   _writer.declareSymbol(symbolScope.tape, symbolScope.reference);
   _writer.loadObject(symbolScope.tape, ObjectInfo(okConstantClass, scope.reference));
   _writer.endSymbol(symbolScope.tape);

   // create byte code sections
   _writer.compile(symbolScope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
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

         declareArgumentList(member, methodScope);
         methodScope.stackSafe = test(classClassScope.info.methodHints.get(methodScope.message).hint, tpStackSafe);

         compileConstructor(member, methodScope, classClassScope, methodScope.compileHints(hints));
      }
      member = member.nextNode();
   }

   // create a virtual constructor
   MethodScope methodScope(&classScope);
   methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

   if (test(classScope.info.header.flags, elDynamicRole)) {
      compileDynamicDefaultConstructor(DNode(), methodScope, classClassScope, DNode());
   }
   else compileDefaultConstructor(DNode(), methodScope, classClassScope, DNode());

   _writer.endClass(classClassScope.tape);

   // optimize
   optimizeTape(classClassScope.tape);

   // create byte code sections
   _writer.compile(classClassScope.tape, classClassScope.moduleScope->module, classClassScope.moduleScope->debugModule, classClassScope.moduleScope->sourcePathRef);
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
            declareArgumentList(member, methodScope);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->mapSubject(GENERIC_PREFIX));

            // mark as having generic methods
            scope.info.header.flags |= elWithGenerics;
         }
         else declareArgumentList(member, methodScope);

         methodScope.compileHints(hints);

         // check if there is no duplicate method
         if (scope.info.methods.exist(methodScope.message, true))
            scope.raiseError(errDuplicatedMethod, member.Terminal());

         if (methodScope.include() && closed)
            scope.raiseError(errClosedParent, member.Terminal());
         
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

void Compiler::compileClassImplementation(DNode node, ClassScope& scope)
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
   _writer.compile(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
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

void Compiler :: declareSingletonAction(DNode node, ClassScope& scope, ActionScope& methodScope)
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
   _writer.compile(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints, bool isStatic)
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

         declareSingletonAction(objNode, classScope, methodScope);
         singleton = true;
      }
      else if (objNode == nsInlineExpression) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         ActionScope methodScope(&classScope);

         declareActionScope(objNode, classScope, expression.firstChild(), methodScope, false);

         declareSingletonAction(objNode, classScope, methodScope);
         singleton = true;
      }
      else if (objNode == nsSubjectArg || objNode == nsMethodParameter) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         ActionScope methodScope(&classScope);

         declareActionScope(objNode, classScope, objNode, methodScope, false);

         declareSingletonAction(objNode, classScope, methodScope);
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

   CodeScope codeScope(&scope);
   if (retVal.kind == okUnknown) {
      // compile symbol body

      recordDebugStep(codeScope, expression.FirstTerminal(), dsStep);
      openDebugExpression(codeScope);
      retVal = compileExpression(expression, codeScope, 0);
      endDebugExpression(codeScope);
   }
   _writer.loadObject(*codeScope.tape, retVal);
   
   // create constant if required
   if (scope.constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression.FirstTerminal());

      // expression cannot be constant
      if (retVal.kind == okAccumulator)
         scope.raiseError(errInvalidOperation, expression.FirstTerminal());

      if (retVal.kind == okIntConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         size_t value = StringHelper::strToULong(module->resolveConstant(retVal.param), 16);

         dataWriter.writeDWord(value);

         dataWriter.Memory()->addReference(scope.moduleScope->intReference | mskVMTRef, -4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->intReference);
      }
      else if (retVal.kind == okLongConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         long value = StringHelper::strToLongLong(module->resolveConstant(retVal.param) + 1, 10);

         dataWriter.write(&value, 8);

         dataWriter.Memory()->addReference(scope.moduleScope->longReference | mskVMTRef, -4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->longReference);
      }
      else if (retVal.kind == okRealConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         double value = StringHelper::strToDouble(module->resolveConstant(retVal.param));

         dataWriter.write(&value, 8);

         dataWriter.Memory()->addReference(scope.moduleScope->realReference | mskVMTRef, -4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->realReference);
      }
      else if (retVal.kind == okLiteralConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         ident_t value = module->resolveConstant(retVal.param);

         dataWriter.writeLiteral(value, getlength(value) + 1);

         dataWriter.Memory()->addReference(scope.moduleScope->literalReference | mskVMTRef, -4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->literalReference);
      }
      else if (retVal.kind == okCharConstant) {
         _Module* module = scope.moduleScope->module;
         MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

         ident_t value = module->resolveConstant(retVal.param);

         dataWriter.writeLiteral(value, getlength(value));

         dataWriter.Memory()->addReference(scope.moduleScope->charReference | mskVMTRef, -4);

         scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->charReference);
      }
      else scope.raiseError(errInvalidOperation, expression.FirstTerminal());
   }

   if (scope.typeRef != 0) {
      bool mismatch = false;
      bool boxed = false;
      compileTypecast(codeScope, retVal, scope.typeRef, mismatch, boxed, 0);
      if (mismatch)
         scope.raiseWarning(2, wrnTypeMismatch, node.FirstTerminal());
      if (boxed)
         scope.raiseWarning(4, wrnBoxingCheck, node.FirstTerminal());
   }

   if (isStatic) {
      // HOTFIX : contains no symbol ending tag, to correctly place an expression end debug symbol
      _writer.exitStaticSymbol(scope.tape, scope.reference);
   }

   TerminalInfo eop = node.lastNode().Terminal();
//   if (eop != nsNone) {
//      recordStep(codeScope, eop, dsVirtualEnd);
//   }
//   else _writer.declareBreakpoint(scope.tape, 0, 0, 0, dsVirtualEnd);

   _writer.endSymbol(scope.tape);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.compile(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, scope.moduleScope->sourcePathRef);
}

void Compiler :: compileIncludeModule(DNode node, ModuleScope& scope, DNode hints)
{
   if (hints != nsNone)
      scope.raiseWarning(1, wrnUnknownHint, hints.Terminal());

   TerminalInfo ns = node.Terminal();

   // check if the module exists
   _Module* module = scope.project->loadModule(ns, true);
   if (!module)
      scope.raiseWarning(1, wrnUnknownModule, ns);

   ident_t value = retrieve(scope.defaultNs.start(), ns, NULL);
   if (value == NULL) {
      scope.defaultNs.add(ns.value);

      // load types
      scope.loadTypes(module);

      // load extensions
      scope.loadExtensions(ns, module);
   }
}

void Compiler :: compileForward(DNode node, ModuleScope& scope, DNode hints)
{
   bool constant = false;
   scope.compileForwardHints(hints, constant);

   TerminalInfo shortcut = node.Terminal();

   if (!scope.defineForward(shortcut.value, node.firstChild().Terminal().value, constant))
      scope.raiseError(errDuplicatedDefinition, shortcut);
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
      else scope.raiseWarning(1, wrnUnknownHint, hints.Terminal());

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
            compileSymbolDeclaration(member, symbolScope, hints, (member == nsStatic));
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
            if (member.firstChild() == nsForward) {
               compileForward(member, scope, hints);
            }
            else compileIncludeModule(member, scope, hints);
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

         project.raiseWarning(1, wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
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
