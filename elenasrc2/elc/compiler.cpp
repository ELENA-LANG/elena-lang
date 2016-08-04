//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2016, by Alexei Rakov
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
#define HINT_NOUNBOXING       0x20000000
#define HINT_EXTERNALOP       0x10000000
#define HINT_NOCONDBOXING     0x08000000
#define HINT_EXTENSION_MODE   0x04000000
#define HINT_ACTION           0x00020000
#define HINT_ALTBOXING        0x00010000
#define HINT_CLOSURE          0x00008000
#define HINT_ASSIGNING        0x00004000
#define HINT_CONSTRUCTOR_EPXR 0x00002000
#define HINT_VIRTUAL_FIELD    0x00001000

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
   return ((expr == nsExpression || expr == nsRootExpression) && expr.nextNode() == nsNone);
}

inline bool isSingleStatement(DNode expr)
{
   return (expr == nsExpression) && (expr.firstChild().nextNode() == nsNone);
}

inline bool isSingleObject(DNode expr)
{
   return (expr == nsObject) && (expr.firstChild().nextNode() == nsNone);
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

inline ref_t importConstant(_Module* exporter, ref_t exportRef, _Module* importer)
{
   if (exportRef) {
      ident_t reference = exporter->resolveConstant(exportRef);

      return importer->mapConstant(reference);
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

inline void findUninqueSubject(_Module* module, ReferenceNs& name)
{
   size_t pos = getlength(name);
   int   index = 0;
   ref_t ref = 0;
   do {
      name[pos] = 0;
      name.appendHex(index++);

      ref = module->mapSubject(name, true);
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

inline bool countSymbol(DNode node, Symbol symbol)
{
   int counter = 0;
   DNode current = node.firstChild();
   while (current != nsNone) {
      if (current == symbol)
         counter++;

      current = current.nextNode();
   }
   return counter;
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

inline DNode goToSymbol(DNode node, Symbol symbol1, Symbol symbol2)
{
   while (node != nsNone) {
      if (node == symbol1 || node == symbol2)
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
         if (StringHelper::compare(terminal.value, INTERNAL_MASK, INTERNAL_MASK_LEN))
            return true;
      }
   }
   return false;
}

inline bool IsVarOperator(int operator_id)
{
   switch (operator_id) {
      case APPEND_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsNumericOperator(int operator_id)
{
   switch (operator_id) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsBitwiseOperator(int operator_id)
{
   switch (operator_id) {
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}


inline bool IsShiftOperator(int operator_id)
{
   switch (operator_id) {
      case READ_MESSAGE_ID:
      case WRITE_MESSAGE_ID:
      case XOR_MESSAGE_ID:
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

inline bool isPrimitiveRef(ref_t reference)
{
   return (int)reference < 0;
}

inline bool isTemplateRef(ref_t reference)
{
   return reference == -6;
}

// returns true if the stack allocated object described by the flag may be passed directly - be stacksafe
inline bool isStacksafe(int flags)
{
   return test(flags, elStructureRole | elEmbeddable) | test(flags, elStructureRole | elEmbeddableWrapper);
}

inline bool isEmbeddable(int flags)
{
   return test(flags, elStructureRole | elEmbeddable);
}

inline bool isEmbeddable(ClassInfo& localInfo)
{
   if (isEmbeddable(localInfo.header.flags)) {
      return true;
   }

   return false;
}

inline bool isWrappable(int flags)
{
   return !test(flags, elWrapper) && test(flags, elSealed);
}

inline bool isDWORD(int flags)
{
   return (isEmbeddable(flags) && (flags & elDebugMask) == elDebugDWORD);
}

inline bool isPTR(int flags)
{
   return (isEmbeddable(flags) && (flags & elDebugMask) == elDebugPTR);
}

void appendTerminalInfo(SyntaxWriter* writer, TerminalInfo terminal)
{
   writer->appendNode(lxCol, terminal.Col());
   writer->appendNode(lxRow, terminal.Row());
   writer->appendNode(lxLength, terminal.length);
   writer->appendNode(lxTerminal, terminal.value);
}

inline int importTemplateSubject(_Module* sour, _Module* dest, ref_t sign_ref, Compiler::TemplateInfo& info)
{
   if (sign_ref == 0)
      return 0;

   ident_t signature = sour->resolveSubject(sign_ref);

   // if the target subject should be overridden
   int index = StringHelper::find(signature, TARGET_POSTFIX);
   if (index >= 0) {
      IdentifierString newSignature;
      while (index >= 0) {
         newSignature.append(signature, index);
         int param_index = signature[index + strlen(TARGET_POSTFIX)] - '0';
         newSignature.append(dest->resolveSubject(info.parameters.get(param_index)));

         signature += index + getlength(TARGET_POSTFIX);
         index = StringHelper::find(signature, TARGET_POSTFIX);
      }
      newSignature.append(signature + 1);

      return dest->mapSubject(newSignature, false);
   }
   else return importSubject(sour, sign_ref, dest);
}

// --- Compiler::ModuleScope ---

Compiler::ModuleScope::ModuleScope(Project* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints((ref_t)-1), extensions(NULL, freeobj)
{
   this->project = project;
   this->sourcePath = sourcePath;
   this->module = module;
   this->debugModule = debugModule;
   this->sourcePathRef = -1;

   this->forwardsUnresolved = forwardsUnresolved;

   warnOnUnresolved = project->BoolSetting(opWarnOnUnresolved);
   warnOnWeakUnresolved = project->BoolSetting(opWarnOnWeakUnresolved);
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
   paramsReference = mapReference(project->resolveForward(PARAMS_FORWARD));
   trueReference = mapReference(project->resolveForward(TRUE_FORWARD));
   falseReference = mapReference(project->resolveForward(FALSE_FORWARD));
   arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));

   // cache the frequently used subjects / hints
   sealedHint = module->mapSubject(HINT_SEALED, false);
   integerHint = module->mapSubject(HINT_INTEGER_NUMBER, false);
   realHint = module->mapSubject(HINT_FLOAT_NUMBER, false);
   literalHint = module->mapSubject(HINT_STRING, false);
   limitedHint = module->mapSubject(HINT_LIMITED, false);
   signHint = module->mapSubject(HINT_SIGNATURE, false);
   extMssgHint = module->mapSubject(HINT_EXT_MESSAGE, false);
   mssgHint = module->mapSubject(HINT_MESSAGE, false);
   stackHint = module->mapSubject(HINT_STACKSAFE, false);
   warnHint = module->mapSubject(HINT_SUPPRESS_WARNINGS, false);
   dynamicHint = module->mapSubject(HINT_DYNAMIC, false);
   constHint = module->mapSubject(HINT_CONSTANT, false);
   structHint = module->mapSubject(HINT_STRUCT, false);
   structOfHint = module->mapSubject(HINT_STRUCTOF, false);
   embedHint = module->mapSubject(HINT_EMBEDDABLE, false);
   ptrHint = module->mapSubject(HINT_POINTER, false);
   boolType = module->mapSubject(project->resolveForward(BOOLTYPE_FORWARD), false);
   extensionHint = module->mapSubject(HINT_EXTENSION, false);
   extensionOfHint = module->mapSubject(HINT_EXTENSIONOF, false);
   genericHint = module->mapSubject(HINT_GENERIC, false);
   actionHint = module->mapSubject(HINT_ACTION_CLASS, false);
   nonstructHint = module->mapSubject(HINT_NONSTRUCTURE, false);
   symbolHint = module->mapSubject(HINT_SYMBOL, false);
   groupHint = module->mapSubject(HINT_GROUP, false);

   defaultNs.add(module->Name());

   loadModuleInfo(module);

   // create a root node for template tree
   SyntaxTree::Writer writer(templates);
   writer.appendNode(lxRoot);
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

ref_t Compiler::ModuleScope :: mapNewSubject(ident_t terminal)
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

ref_t Compiler::ModuleScope :: resolveSubjectRef(ident_t identifier, bool explicitOnly)
{
   ref_t subj_ref = subjects.get(identifier);
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
      if (subj_ref && (!explicitOnly || subjectHints.exist(subj_ref))) {
         subjects.add(identifier, subj_ref);

         return subj_ref;
      }
      it++;
   }

   return 0;
}

ref_t Compiler::ModuleScope :: mapSubject(TerminalInfo terminal, bool explicitOnly)
{
   ident_t identifier = NULL;
   if (terminal.symbol == tsIdentifier || terminal.symbol == tsPrivate) {
      identifier = terminal.value;
   }
   else raiseError(errInvalidSubject, terminal);

   return resolveSubjectRef(terminal, explicitOnly);
}

ref_t Compiler::ModuleScope :: mapSubject(TerminalInfo terminal, IdentifierString& output)
{
   // add a namespace for the private message
   if (terminal.symbol == tsPrivate) {
      output.append(project->StrSetting(opNamespace));
      output.append(terminal.value);

      return 0;
   }

   ref_t subjRef = mapSubject(terminal);
   if (subjRef != 0) {
      output.append(module->resolveSubject(subjRef));
   }
   else if (terminal.symbol != tsReference) {
      output.append(terminal.value);
   }
   else raiseError(errInvalidSubject, terminal);

   return subjRef;
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

ref_t Compiler::ModuleScope :: mapNestedExpression()
{
   // otherwise auto generate the name
   ReferenceNs name(module->Name(), INLINE_POSTFIX);

   findUninqueName(module, name);

   return module->mapReference(name);
}

ref_t Compiler::ModuleScope :: mapNestedTemplate()
{
   // otherwise auto generate the name
   ReferenceNs name(module->Name(), INLINE_POSTFIX);

   findUninqueSubject(module, name);

   return module->mapSubject(name, false);
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
               return ObjectInfo(okConstantSymbol, reference, subjectHints.get(symbolInfo.expressionTypeRef), symbolInfo.expressionTypeRef);
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
   if (StringHelper::compare(reference, EXTERNAL_MODULE, strlen(EXTERNAL_MODULE))) {
      char ch = reference[strlen(EXTERNAL_MODULE)];
      if (ch == '\'' || ch == 0)
         return ObjectInfo(okExternal);
   }
   // To tell apart primitive modules, the name convention is used
   else if (StringHelper::compare(reference, INTERNAL_MASK, INTERNAL_MASK_LEN)) {
      return ObjectInfo(okInternal, module->mapReference(reference));
   }

   ref_t referenceID = mapReference(reference, existing);

   return defineObjectInfo(referenceID);
}

void Compiler::ModuleScope :: importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly)
{
   target.header = copy.header;
   target.classClassRef = copy.classClassRef;
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
         if (test(key.value2, maSubjectMask))
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

_Memory* Compiler::ModuleScope :: loadTemplateInfo(ident_t symbol, _Module* &argModule)
{
   if (emptystr(symbol))
      return NULL;

   List<ident_t>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      argModule = project->loadModule(*it, true);

      ref_t ref = argModule->mapSubject(symbol, true);
      if (ref) {
         _Memory* section = argModule->mapSection(ref | mskSyntaxTreeRef, true);
         if (section)
            return section;
      }
      it++;
   }

   argModule = NULL;

   return NULL;
}

//bool Compiler::ModuleScope :: recognizePrimitive(ident_t name, ident_t value, size_t& roleMask, int& size)
//{
//   if (StringHelper::compare(name, HINT_INTEGER_NUMBER)) {
//      if (StringHelper::compare(value, "1")) {         
//         roleMask = elDebugDWORD;
//         size = 1;
//      }
//      else if (StringHelper::compare(value, "2")) {
//         roleMask = elDebugDWORD;
//         size = 2;
//      }
//      else if (StringHelper::compare(value, "4")) {
//         roleMask = elDebugDWORD;
//         size = 4;
//      }
//      else if (StringHelper::compare(value, "8")) {
//         roleMask = elDebugQWORD;
//         size = 8;
//      }
//      else return false;
//
//      return true;
//   }
//
//   return false;
//}

int Compiler::ModuleScope :: defineStructSizeEx(ref_t classReference, bool& variable, bool embeddableOnly)
{
   ClassInfo classInfo;
   if(loadClassInfo(classInfo, module->resolveReference(classReference), true) == 0)
      return 0;

   variable = !test(classInfo.header.flags, elReadOnlyRole);

   if (!embeddableOnly && test(classInfo.header.flags, elStructureRole)) {
      return classInfo.size;
   }
   else if (isEmbeddable(classInfo)) {
      return classInfo.size;
   }

   return 0;
}

int Compiler::ModuleScope :: defineSubjectSizeEx(ref_t type_ref, bool& variable, bool embeddableOnly)
{
   if (type_ref == 0)
      return 0;

   ref_t classReference = subjectHints.get(type_ref);
   if (classReference != 0) {
      return defineStructSizeEx(classReference, variable, embeddableOnly);
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

int Compiler::ModuleScope :: checkMethod(ClassInfo& info, ref_t message, ref_t& outputType)
{
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
   //HOTFIX : to recognize the sealed private method call
   //         hint search should be done even if the method is not declared
   else return info.methodHints.get(Attribute(message, maHint));;
}

int Compiler::ModuleScope :: checkMethod(ref_t reference, ref_t message, bool& found, ref_t& outputType)
{
   ClassInfo info;
   found = loadClassInfo(info, module->resolveReference(reference)) != 0;

   if (found) {
      return checkMethod(info, message, outputType);
   }
   else return tpUnknown;
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
         else raiseWarning(WARNING_LEVEL_1, wrnUnresovableLink, terminal);
      }
   }
}

void Compiler::ModuleScope :: raiseError(const char* message, TerminalInfo terminal)
{
   raiseError(message, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: raiseWarning(int level, const char* message, TerminalInfo terminal)
{
   raiseWarning(level, message, terminal.Row(), terminal.Col(), terminal.value);
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

void Compiler::ModuleScope :: loadSubjects(_Module* extModule)
{
   if (extModule) {
      bool owner = module == extModule;

      ReferenceNs sectionName(extModule->Name(), TYPE_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t subj_ref = importSubject(extModule, metaReader.getDWord(), module);
            ref_t class_ref = metaReader.getDWord();
            if (class_ref == -1) {
               // HOTFIX : ignore template declaration, only reference name should be loaded
            }
            else {
               class_ref = importReference(extModule, class_ref, module);

               subjectHints.add(subj_ref, class_ref);

               if (owner && class_ref != 0)
                  typifiedClasses.add(class_ref, subj_ref);
            }
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
            else raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, terminal);
         }
      }
   }
}

void Compiler::ModuleScope :: saveSubject(ref_t type_ref, ref_t classReference, bool internalType)
{
   if (!internalType) {
      ReferenceNs sectionName(module->Name(), TYPE_SECTION);

      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(type_ref);
      metaWriter.writeDWord(classReference);
   }

   if (classReference != 0)
      typifiedClasses.add(classReference, type_ref);

   subjectHints.add(type_ref, classReference, true);
}

void Compiler::ModuleScope :: saveTemplate(ref_t template_ref)
{
   ReferenceNs sectionName(module->Name(), TYPE_SECTION);

   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

   metaWriter.writeDWord(template_ref);
   metaWriter.writeDWord(-1); // -1 indicates that it is a template declaration
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

//ref_t Compiler::ModuleScope :: defineType(ref_t classRef)
//{
//   while (classRef != 0) {
//      ref_t type = retrieveKey(typeHints.start(), classRef, 0);
//
//      if (type != 0)
//         return type;
//
//      ClassInfo sourceInfo;
//      if (loadClassInfo(sourceInfo, module->resolveReference(classRef), true) == 0)
//         break;
//
//      classRef = sourceInfo.header.parentRef;
//   }
//
//   return 0;
//}

bool Compiler::ModuleScope :: checkIfCompatible(ref_t typeRef, ref_t classRef)
{
   ClassInfo sourceInfo;

   if (subjectHints.exist(typeRef, classRef))
      return true;

   // if source class inherites / is target class
   while (classRef != 0) {
      if (loadClassInfo(sourceInfo, module->resolveReference(classRef), true) == 0)
         break;

      if (subjectHints.exist(typeRef, classRef))
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

   syntaxTree.writeString(parent->sourcePath);
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

   extensionMode = 0;

   syntaxTree.writeString(parent->sourcePath);
}

ObjectInfo Compiler::ClassScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, SUPER_VAR)) {
      return ObjectInfo(okSuper, info.header.parentRef);
   }
   else if (StringHelper::compare(identifier, SELF_VAR)) {
      if (extensionMode != 0 && extensionMode != -1) {
         return ObjectInfo(okParam, (size_t)-1, 0, extensionMode);
      }
      else return ObjectInfo(okParam, (size_t)-1);
   }
   else {
      int reference = info.fields.get(identifier);
      if (reference != -1) {
         if (test(info.header.flags, elStructureRole)) {
            int offset = reference;
            ref_t type = info.fieldTypes.get(offset);
            if (type == 0) {
               // if it is a primitive field
               switch (info.header.flags & elDebugMask) {
                  case elDebugDWORD:
                     return ObjectInfo(okFieldAddress, offset, -1);
                  case elDebugQWORD:
                     return ObjectInfo(okFieldAddress, offset, -2);
                  case elDebugReal64:
                     return ObjectInfo(okFieldAddress, offset, -3);
               }
            }
            return ObjectInfo(okFieldAddress, offset, 0, type);
         }
         // otherwise it is a normal field
         else return ObjectInfo(okField, reference, 0, info.fieldTypes.get(reference));
      }
     else return Scope::mapObject(identifier);
   }
}

void Compiler::ClassScope :: compileClassHint(SNode hint)
{
   switch (hint.type)
   {
      case lxClassFlag:
         info.header.flags |= hint.argument;
         break;
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
   this->privat = false;
   this->generic = false;

   //NOTE : tape has to be overridden in the constructor
   this->tape = &parent->tape;
}

ObjectInfo Compiler::MethodScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, THIS_VAR)) {
      if (stackSafe && test(getClassFlags(), elStructureRole)) {
         return ObjectInfo(okThisParam, 1, -1);
      }
      else return ObjectInfo(okThisParam, 1);
   }
   else if (StringHelper::compare(identifier, METHOD_SELF_VAR)) {
      return ObjectInfo(okParam, (size_t)-1);
   }
   else {
      Parameter param = parameters.get(identifier);

      int local = param.offset;
      if (local >= 0) {
         if (withOpenArg && moduleScope->subjectHints.exist(param.subj_ref, moduleScope->paramsReference)) {
            return ObjectInfo(okParams, -1 - local, 0, param.subj_ref);
         }
         else if (stackSafe && param.subj_ref != 0) {
            // HOTFIX : only embeddable parameter / embeddable wrapper should be boxed in stacksafe method
            if (isEmbeddable(moduleScope->getClassFlags(moduleScope->subjectHints.get(param.subj_ref)))) {
               return ObjectInfo(okParam, -1 - local, -1, param.subj_ref);
            }
         }
         return ObjectInfo(okParam, -1 - local, 0, param.subj_ref);
      }
      else {
         ObjectInfo retVal = Scope::mapObject(identifier);
         if (stackSafe && retVal.kind == okParam && retVal.param == -1 && retVal.type != 0) {
            if (isEmbeddable(moduleScope->getClassFlags(moduleScope->subjectHints.get(retVal.type)))) {
               return ObjectInfo(okParam, retVal.param, -1, retVal.type);
            }
            else return retVal;
         }

         return retVal;
      }
   }
}

// --- Compiler::ActionScope ---

Compiler::ActionScope :: ActionScope(ClassScope* parent)
   : MethodScope(parent)
{
}

ObjectInfo Compiler::ActionScope :: mapObject(TerminalInfo identifier)
{
   // HOTFIX : self / $self : closure should refer to the owner ones
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
   //this->rootBookmark = -1;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent, SyntaxWriter* writer)
   : Scope(parent), locals(Parameter(0))
{
   this->writer = writer;
   this->level = 0;
   this->saved = this->reserved = 0;
   //this->rootBookmark = -1;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->writer = parent->writer;
   this->level = parent->level;
   this->saved = parent->saved;
   this->reserved = parent->reserved;
   //this->rootBookmark = -1;
}

ObjectInfo Compiler::CodeScope :: mapObject(TerminalInfo identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset) {
      if (StringHelper::compare(identifier, SUBJECT_VAR)) {
         return ObjectInfo(okSubject, local.offset);
      }
      else if (isTemplateRef(local.class_ref)) {
         return ObjectInfo(okTemplateLocal, local.offset, local.class_ref, local.subj_ref);
      }
      else if (local.size != 0) {
         return ObjectInfo(okLocalAddress, local.offset, local.class_ref, local.subj_ref);
      }
      else return ObjectInfo(okLocal, local.offset, local.class_ref, local.subj_ref);
   }
   else return Scope::mapObject(identifier);
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
   : ClassScope(owner->moduleScope, reference), outers(Outer())
{
   this->parent = owner;
   info.header.flags |= elNestedClass;
   templateMode = false;
   templateRef = 0;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   String<ident_c, 10> thisVar(THIS_VAR);

   Outer owner = outers.get(thisVar);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();

      TerminalInfo identifier;
      identifier.symbol = tsPrivate;
      identifier.value = THIS_VAR;

      owner.outerObject = parent->mapObject(identifier);
      if (owner.outerObject.extraparam == 0)
         owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId();

      // Compiler magic : if the owner is a template - switch to template closure mode
      if (isTemplateRef(owner.outerObject.extraparam)) {
         templateMode = true;
      }

      outers.add(thisVar, owner);
      mapKey(info.fields, thisVar, owner.reference);
   }
   return owner;
}

ObjectInfo Compiler::InlineClassScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, THIS_VAR) || StringHelper::compare(identifier, OWNER_VAR)) {
      Outer owner = mapSelf();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuter, owner.reference, owner.outerObject.extraparam);
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
         outer.outerObject = parent->mapObject(identifier);
         // handle outer fields in a special way: save only self
         if (outer.outerObject.kind == okField) {
            Outer owner = mapSelf();

            // save the outer field type if provided
            if (outer.outerObject.extraparam != 0) {
               outerFieldTypes.add(outer.outerObject.param, outer.outerObject.extraparam, true);
            }

            // map as an outer field (reference to outer object and outer object field index)
            return ObjectInfo(okOuterField, owner.reference, outer.outerObject.param, outer.outerObject.type);
         }
         // map if the object is outer one
         else if (outer.outerObject.kind == okParam || outer.outerObject.kind == okLocal || outer.outerObject.kind == okField
            || outer.outerObject.kind == okOuter || outer.outerObject.kind == okSuper || outer.outerObject.kind == okThisParam
            || outer.outerObject.kind == okOuterField || outer.outerObject.kind == okLocalAddress)
         {
            outer.reference = info.fields.Count();

            outers.add(identifier, outer);
            mapKey(info.fields, identifier.value, outer.reference);

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
            break;
         }

         it++;
      }
   }

   return false;
}

// --- Compiler::TemplateScope ---

Compiler::TemplateScope :: TemplateScope(ModuleScope* parent, ref_t reference)
   : ClassScope(parent, 0)
{
   // NOTE : reference is defined in subject namespace, so templateRef should be initialized and used
   // proper reference is 0 in this case
   this->templateRef = reference;
   this->reference = -6; // indicating template

   type = ttNone;

   // HOT FIX : overwrite source path to provide explicit namespace
   _Memory* strings = syntaxTree.Strings();
   strings->trim(0);

   MemoryWriter writer(strings);
   writer.writeLiteral(parent->module->Name());
   writer.seek(writer.Position() - 1);
   writer.writeChar('\'');
   writer.writeLiteral(parent->sourcePath);
}

ObjectInfo Compiler::TemplateScope :: mapObject(TerminalInfo identifier)
{
   int index = info.fields.get(identifier);

   if (index >= 0) {
      return ObjectInfo(okTemplateTarget, index, 0, info.fieldTypes.get(index));
   }
   else return ClassScope::mapObject(identifier);
}

bool Compiler::TemplateScope :: validateTemplate(ref_t reference)
{
   _Module* extModule = NULL;
   _Memory* section = moduleScope->loadTemplateInfo(reference, extModule);
   if (!section)
      return false;

   // HOTFIX : inherite template fields
   SyntaxTree tree(section);
   SNode current = tree.readRoot();
   current = current.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateField) {
         SNode typeNode = SyntaxTree::findChild(current, lxTemplateFieldType);

         info.fields.add(SyntaxTree::findChild(current, lxTerminal).identifier(), current.argument);
         if (typeNode.argument != 0)
            info.fieldTypes.add(current.argument, typeNode.argument);
      }

      current = current.nextNode();
   }

   return true;
}

// --- Compiler ---

Compiler :: Compiler(StreamReader* syntax)
   : _parser(syntax), _verbs(0)
{
   _optFlag = 0;

   ByteCodeCompiler::loadVerbs(_verbs);
   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: writeMessage(ModuleScope& scope, SyntaxWriter& writer, ref_t messageRef)
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

void Compiler :: appendObjectInfo(CodeScope& scope, ObjectInfo object)
{
   if (object.type != 0 && object.kind != okTemplateLocal) {
      scope.writer->appendNode(lxType, object.type);
   }

   ref_t objectRef = resolveObjectReference(scope, object);
   if (isTemplateRef(objectRef)) {
      if (object.kind == okTemplateLocal) {
         TemplateScope* templateScope = (TemplateScope*)scope.getScope(Scope::slTemplate);

         ident_t templateFullName = scope.moduleScope->module->resolveSubject(object.type);
         int index = StringHelper::find(templateFullName, '@');

         IdentifierString name(templateFullName, index);
         //HOTFIX : virtual variable
         scope.writer->newNode(lxTemplateType, scope.moduleScope->module->mapSubject(name, true));
         while (index < getlength(templateFullName)) {
            int pos = index + 1;
            index = StringHelper::find(templateFullName + pos, '@', getlength(templateFullName));
            IdentifierString param(templateFullName + pos, index - pos);

            scope.writer->appendNode(lxTemplateParam, templateScope->parameters.get(param));
         }
         scope.writer->closeNode();
      }
      else scope.writer->appendNode(lxNestedTemplateOwner);
   }
   else if (objectRef != 0) {
      scope.writer->appendNode(lxTarget, objectRef);
   }
   else if (object.type == 0) {
      if (object.kind == okFieldAddress && object.param == 0) {
         int flags = scope.getClassFlags();
         switch (flags & elDebugMask) {
            case elDebugDWORD:
               scope.writer->appendNode(lxTarget, -1);
               break;
            case elDebugQWORD:
               scope.writer->appendNode(lxTarget, -2);
               break;
            case elDebugReal64:
               scope.writer->appendNode(lxTarget, -4);
               break;
            case elDebugSubject:
               scope.writer->appendNode(lxTarget, -8);
               break;
         }
      }
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

bool Compiler :: checkIfCompatible(ModuleScope& scope, ref_t typeRef, SyntaxTree::Node node)
{
   ref_t nodeType = SyntaxTree::findChild(node, lxType).argument;   
   ref_t nodeRef = SyntaxTree::findChild(node, lxTarget).argument;

   if (nodeType == typeRef) {
      return true;
   }
   else if (isPrimitiveRef(nodeRef)) {
      int flags = scope.getClassFlags(scope.subjectHints.get(typeRef));
      if (!isEmbeddable(flags))
         return false;

      switch (nodeRef) {
         case -1:
            return isDWORD(flags) || isPTR(flags);
         case -2:
            return (flags & elDebugMask) == elDebugQWORD;
         case -4:
            return (flags & elDebugMask) == elDebugReal64;
         default:
            return false;
      }
   }
   else if (node == lxNil) {
      return true;
   }
   else if (node == lxConstantInt) {
      int flags = scope.getClassFlags(scope.subjectHints.get(typeRef));

      return isEmbeddable(flags) && (isDWORD(flags) || isPTR(flags));
   }
   else if (node == lxConstantReal) {
      int flags = scope.getClassFlags(scope.subjectHints.get(typeRef));

      return isEmbeddable(flags) && (flags & elDebugMask) == elDebugReal64;
   }
   else if (node == lxConstantLong) {
      int flags = scope.getClassFlags(scope.subjectHints.get(typeRef));

      return isEmbeddable(flags) && (flags & elDebugMask) == elDebugQWORD;
   }
   else return scope.checkIfCompatible(typeRef, nodeRef);
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
      case okWideLiteralConstant:
         return scope.moduleScope->wideReference;
      case okCharConstant:
         return scope.moduleScope->charReference;
      case okThisParam:
         return scope.getClassRefId(false);
      case okSubject:
      case okSignatureConstant:
         return scope.moduleScope->signatureReference;
      case okSuper:
         return object.param;
      case okTemplateLocal:
         return object.extraparam;
      case okParams:
         return scope.moduleScope->paramsReference;
      case okExternal:
         return -1; // NOTE : -1 means primitve int32
      case okMessageConstant:
         return scope.moduleScope->messageReference;
      default:
         if (object.kind == okObject && object.param != 0) {
            return object.param;
         }
         else if ((object.kind == okLocal || object.kind == okOuter) && object.extraparam > 0) {
            return object.extraparam;
         }
         else return object.type != 0 ? scope.moduleScope->subjectHints.get(object.type) : 0;
   }
}

void Compiler :: declareParameterDebugInfo(MethodScope& scope, SyntaxWriter& writer, bool withThis, bool withSelf)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // declare method parameter debug info
   LocalMap::Iterator it = scope.parameters.start();
   while (!it.Eof()) {
      if (scope.moduleScope->subjectHints.exist((*it).subj_ref, moduleScope->paramsReference)) {
         writer.newNode(lxParamsVariable);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.closeNode();
      }
      else if (scope.moduleScope->subjectHints.exist((*it).subj_ref, moduleScope->intReference)) {
         writer.newNode(lxIntVariable);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }
      else if (scope.moduleScope->subjectHints.exist((*it).subj_ref, moduleScope->longReference)) {
         writer.newNode(lxLongVariable);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }
      else if (scope.moduleScope->subjectHints.exist((*it).subj_ref, moduleScope->realReference)) {
         writer.newNode(lxReal64Variable);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }
      else if (scope.stackSafe && (*it).subj_ref != 0) {
         ref_t classRef = scope.moduleScope->subjectHints.get((*it).subj_ref);
         if (classRef != 0 && isEmbeddable(scope.moduleScope->getClassFlags(classRef))) {
            writer.newNode(lxBinaryVariable);
            writer.appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
         }
         else writer.newNode(lxVariable, -1);
         
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }
      else {
         writer.newNode(lxVariable, -1);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }

      it++;
   }
   if (withThis) {
      if (scope.stackSafe && isEmbeddable(scope.getClassFlags())) {
         writer.newNode(lxBinarySelf, 1);
         writer.appendNode(lxClassName, scope.moduleScope->module->resolveReference(scope.getClassRef()));
         writer.closeNode();
      }
      else writer.appendNode(lxSelfVariable, 1);
   }
      
   if (withSelf)
      writer.appendNode(lxSelfVariable, -1);

   writeMessage(*scope.moduleScope, writer, scope.message);
}

void Compiler :: importCode(DNode node, ModuleScope& scope, SyntaxWriter& writer, ident_t referenceProperName, ref_t message)
{
   IdentifierString virtualReference(referenceProperName);
   virtualReference.append('.');

   int paramCount;
   ref_t sign_ref, verb_id;
   decodeMessage(message, sign_ref, verb_id, paramCount);

   // HOTFIX : include self as a parameter
   paramCount++;

   virtualReference.append('0' + paramCount);
   virtualReference.append('#');
   virtualReference.append(0x20 + verb_id);

   if (sign_ref != 0) {
      virtualReference.append('&');
      virtualReference.append(scope.module->resolveSubject(sign_ref));
   }

   ref_t reference = 0;
   _Module* api = scope.project->resolveModule(virtualReference, reference);

   _Memory* section = api != NULL ? api->mapSection(reference | mskCodeRef, true) : NULL;
   if (section == NULL) {
      scope.raiseError(errInvalidLink, node.Terminal());
   }
   else writer.appendNode(lxImporting, _writer.registerImportInfo(section, api, scope.module));
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

void Compiler :: compileParentDeclaration(DNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed)
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
      scope.raiseError(errInvalidParent, baseNode.Terminal());
   }
   if (res == irSealed) {
      scope.raiseError(errSealedParent, baseNode.Terminal());
   }
   else if (res == irUnsuccessfull)
      scope.raiseError(errUnknownBaseClass, baseNode.Terminal());
}

void Compiler :: compileParentDeclaration(DNode node, ClassScope& scope)
{
   ref_t parentRef = scope.info.header.parentRef;

   TerminalInfo identifier = node.Terminal();
   if (scope.info.header.parentRef == scope.reference) {
      if (identifier != nsNone) {
         scope.raiseError(errInvalidSyntax, identifier);
      }
      else parentRef = 0;
   }
   else if (identifier != nsNone) {
      if (identifier == tsIdentifier || identifier == tsPrivate) {
         parentRef = scope.moduleScope->mapTerminal(node.Terminal(), true);
      }
      else parentRef = scope.moduleScope->mapReference(identifier);

      if (parentRef == 0)
         scope.raiseError(errUnknownClass, identifier);
   }

   compileParentDeclaration(node, scope, parentRef);
}

bool Compiler :: declareTemplateInfo(DNode hint, ClassScope& scope, ref_t hintRef, ref_t messageSubject)
{
   ModuleScope* moduleScope = scope.moduleScope;

   if (!scope.validateTemplate(hintRef))
      return false;

   SyntaxTree::Writer writer(moduleScope->templates, true);

   writer.newNode(lxClass, scope.reference);
   writer.newNode(lxTemplate, hintRef);
   appendTerminalInfo(&writer, hint.Terminal());

   DNode paramNode = hint.firstChild();
   while (paramNode != nsNone) {
      if (paramNode == nsHintValue) {
         TerminalInfo param = paramNode.Terminal();
         if (param == tsIdentifier) {
            ref_t subject = scope.mapSubject(param, false);
            if (subject == 0)
               subject = moduleScope->module->mapSubject(param, false);

            writer.appendNode(lxTemplateSubject, subject);
         }
         else scope.raiseError(errInvalidHintValue, param);
      }
      paramNode = paramNode.nextNode();
   }

   if (messageSubject != 0)
      writer.appendNode(lxTemplateSubject, messageSubject);   

   writer.closeNode();
   writer.closeNode();

   writer.closeNode(); //HOTFIX : close the root node

   return true;
}

void Compiler :: declareFieldTemplateInfo(SyntaxTree::Node node, ClassScope& scope, ref_t hintRef, int fieldOffset)
{
   ModuleScope* moduleScope = scope.moduleScope;

   //if (!scope.validateTemplate(hintRef))
   //   return false;

   SyntaxTree::Writer writer(moduleScope->templates, true);

   writer.newNode(lxClass, scope.reference);
   writer.newNode(lxTemplate, hintRef);
   //appendTerminalInfo(&writer, hint.Terminal());

   SNode attr = node.firstChild();
   TemplateInfo templateInfo;
   while (attr != lxNone) {
      if (attr == lxTemplateSubject) {
         int index = 1 + templateInfo.parameters.Count();

         templateInfo.parameters.add(index, attr.argument);
         writer.appendNode(lxTemplateSubject, attr.argument);
      }
      attr = attr.nextNode();
   }

   writer.appendNode(lxTemplateField, fieldOffset);

   writer.closeNode();
   writer.closeNode();

   writer.closeNode(); //HOTFIX : close the root node

   //HOTFIX : declare field template methods
   _Module* extModule = NULL;
   _Memory* section = moduleScope->loadTemplateInfo(hintRef, extModule);
   if (!section)
      return;

   SyntaxWriter classWriter(*node.Tree(), true);

   SyntaxTree tree(section);
   SNode current = tree.readRoot();
   current = current.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(extModule, moduleScope->module, getSignature(current.argument), templateInfo));

         classWriter.newNode(lxTemplateMethod, messageRef);
         SNode attr = current.firstChild();
         while (attr != lxNone) {
            if (attr == lxClassMethodAttr) {
               classWriter.appendNode(lxClassMethodAttr, attr.argument);
            }
            else if (attr == lxType) {
               classWriter.appendNode(lxType, importTemplateSubject(extModule, moduleScope->module, attr.argument, templateInfo));
            }

            attr = attr.nextNode();
         }
         classWriter.closeNode();
      }

      current = current.nextNode();
   }
   classWriter.closeNode(); // close root
}

void Compiler :: importTemplateInfo(SyntaxTree::Node node, ModuleScope& scope, ref_t ownerRef, _Module* templateModule, TemplateInfo& info)
{
   SyntaxTree::Writer writer(scope.templates, true);

   writer.newNode(lxClass, ownerRef);
   writer.newNode(lxTemplate, importSubject(templateModule, node.argument, scope.module));
   writer.appendNode(lxCol, info.sourceCol);
   writer.appendNode(lxRow, info.sourceRow);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateSubject) {
         writer.appendNode(current.type, importTemplateSubject(templateModule, scope.module, current.argument, info));
      }

      current = current.nextNode();
   }

   writer.closeNode();
   writer.closeNode();

   writer.closeNode(); //HOTFIX : close the root node
}

void Compiler :: declareTemplateParameters(DNode hint, ModuleScope& scope, RoleMap& parameters)
{
   DNode paramNode = hint.firstChild();
   while (paramNode != nsNone) {
      if (paramNode == nsHintValue) {
         TerminalInfo param = paramNode.Terminal();
         if (param == tsIdentifier) {
            ref_t subject = scope.mapSubject(param, false);
            if (subject == 0)
               subject = scope.module->mapSubject(param, false);

            parameters.add(parameters.Count() + 1, subject);
         }
         else scope.raiseError(errInvalidHintValue, param);
      }
      paramNode = paramNode.nextNode();
   }
}

void Compiler :: readTemplateInfo(SNode node, TemplateInfo& info)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplate) {
         info.templateRef = current.argument;

         readTemplateInfo(current, info);
      }
      else if (current == lxTemplateSubject) {
         int index = 1 + info.parameters.Count();

         info.parameters.add(index, current.argument);
      }
      else if (current == lxTemplateField) {
         info.targetOffset = current.argument;
      }
      else if (current == lxCol) {
         info.sourceCol = current.argument;
      }
      else if (current == lxRow) {
         info.sourceRow = current.argument;
      }

      current = current.nextNode();
   }
}

void Compiler :: copyTemplateInfo(SyntaxTree::Node node, SyntaxTree::Writer& writer)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      writer.newNode(current.type, current.argument);
      copyTemplateInfo(current, writer);
      writer.closeNode();

      current = current.nextNode();
   }
}

ref_t Compiler :: mapHint(DNode hint, ModuleScope& scope)
{
   ref_t hintRef = 0;

   TerminalInfo terminal = hint.Terminal();

   hintRef = scope.mapSubject(terminal);
   if (hintRef == 0) {
      int count = countSymbol(hint, nsHintValue);
      IdentifierString hintName(terminal);

      hintName.append('#');
      hintName.appendInt(count);

      hintRef = scope.resolveSubjectRef(hintName, false);
   }

   return hintRef;
}

bool Compiler :: compileClassHint(DNode hint, SyntaxWriter& writer, ClassScope& scope, bool directiveOnly)
{
   ModuleScope* moduleScope = scope.moduleScope;
   ref_t hintRef = mapHint(hint, *moduleScope);

   TerminalInfo terminal = hint.Terminal();

   // if it is a class modifier
   if (hintRef == moduleScope->sealedHint) {
      writer.appendNode(lxClassFlag, elSealed);

      return true;
   }
   else if (hintRef == moduleScope->dynamicHint) {
      writer.appendNode(lxClassFlag, elDynamicRole);

      return true;
   }
   else if (hintRef == moduleScope->limitedHint) {
      writer.appendNode(lxClassFlag, elClosed);

      return true;
   }
   if (hintRef == moduleScope->groupHint) {
      writer.appendNode(lxClassFlag, elGroup);

      return true;
   }
   else if (hintRef == moduleScope->literalHint) {
      writer.appendNode(lxClassFlag, elDebugLiteral); // NOTE : template importer should tweak it depending on character size
      writer.appendNode(lxClassFlag, elStructureRole | elDynamicRole);

      return true;
   }
   else if (hintRef == moduleScope->constHint) {
      writer.appendNode(lxClassFlag, elReadOnlyRole);

      return true;
   }
   else if (hintRef == moduleScope->extensionHint) {
      scope.extensionMode = -1;

      writer.appendNode(lxClassFlag, elExtension);
      writer.appendNode(lxClassFlag, elSealed);    // extension should be sealed

      return true;
   }
   else if (hintRef == moduleScope->extensionOfHint) {
      DNode value = hint.select(nsHintValue);
      scope.extensionMode = scope.moduleScope->mapSubject(value.Terminal());
      if (!scope.extensionMode)
         scope.raiseError(errUnknownSubject, value.Terminal());

      writer.appendNode(lxClassFlag, elExtension);
      writer.appendNode(lxClassFlag, elSealed);    // extension should be sealed

      return true;
   }
   else if (hintRef == moduleScope->structHint || hintRef == moduleScope->structOfHint) {
      writer.appendNode(lxClassFlag, elStructureRole);

      if (hintRef == moduleScope->structOfHint) {
         DNode option = hint.select(nsHintValue);
         ref_t optionRef = mapHint(option, *moduleScope);
         if (optionRef == moduleScope->embedHint) {
            writer.appendNode(lxClassFlag, elEmbeddable);

            return true;
         }
         else if (optionRef == moduleScope->ptrHint) {
            writer.appendNode(lxClassFlag, elPointer);

            return true;
         }
         else scope.raiseError(wrnInvalidHint, terminal);
      }
      else return true;
   }
   else if (hintRef == moduleScope->nonstructHint) {
      writer.appendNode(lxClassFlag, elNonStructureRole);

      return true;
   }
   else if (hintRef != 0) {
      return declareTemplateInfo(hint, scope, hintRef);
   }

   return false;
}

void Compiler :: compileClassHints(DNode hints, SyntaxWriter& writer, ClassScope& scope)
{
   // define class flags
   while (hints == nsHint) {
      if (!compileClassHint(hints, writer, scope, false))
         scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

void Compiler :: compileSymbolHints(DNode hints, SymbolScope& scope, bool silentMode)
{
   while (hints == nsHint) {
      ref_t hintRef = mapHint(hints, *scope.moduleScope);
      if (hintRef == scope.moduleScope->constHint) {
         scope.constant = true;
      }
      else if (scope.moduleScope->subjectHints.get(hintRef) != 0) {
         scope.typeRef = hintRef;
      }
      else if (!silentMode)
         scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

void Compiler :: compileSingletonHints(DNode hints, SyntaxWriter& writer, ClassScope& scope)
{
   while (hints == nsHint) {
      ref_t hintRef = mapHint(hints, *scope.moduleScope);

      TerminalInfo terminal = hints.Terminal();

      if (hintRef != 0) {
         declareTemplateInfo(hints, scope, hintRef);
      }
      else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

void Compiler :: compileTemplateHints(DNode hints, SyntaxWriter& writer, TemplateScope& scope)
{
   // define class flags
   while (hints == nsHint) {
      if (!compileClassHint(hints, writer, scope, true))
         scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

void Compiler :: compileFieldHints(DNode hints, SyntaxWriter& writer, ClassScope& scope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      ref_t hintRef = mapHint(hints, *scope.moduleScope);
      if (hintRef != 0) {
         if (hintRef == moduleScope->integerHint) {
            TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
            if (sizeValue.symbol == tsInteger) {
               int size = StringHelper::strToInt(sizeValue.value);

               // !! HOTFIX : allow only 1,2,4 or 8
               if (size == 1 || size == 2 || size == 4) {
                  writer.appendNode(lxTarget, -1);
               }
               else if (size == 8) {
                  writer.appendNode(lxTarget, -2);
               }
               else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

               writer.appendNode(lxSize, size);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());
         }
         else if (hintRef == moduleScope->realHint) {
            TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
            if (sizeValue.symbol == tsInteger) {
               int size = StringHelper::strToInt(sizeValue.value);

               // !! HOTFIX : allow only 8
               if (size == 8) {
                  writer.appendNode(lxTarget, -4);
               }
               else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

               writer.appendNode(lxSize, size);
            }
         }
         else if (hintRef == moduleScope->symbolHint) {
            writer.appendNode(lxTarget, -7);
            writer.appendNode(lxSize, 4);
         }
         else if (hintRef == moduleScope->signHint) {
            writer.appendNode(lxTarget, -8);
            writer.appendNode(lxSize, 4);
         }
         else if (hintRef == moduleScope->mssgHint) {
            writer.appendNode(lxTarget, -9);
            writer.appendNode(lxSize, 4);
         }
         else if (hintRef == moduleScope->extMssgHint) {
            writer.appendNode(lxTarget, -10);
            writer.appendNode(lxSize, 8);
         }
         else if(scope.moduleScope->subjectHints.exist(hintRef)) {
            writer.appendNode(lxType, hintRef);
         }
         else if (hintRef != 0) {
            writer.newNode(lxTemplate, hintRef);
            appendTerminalInfo(&writer, terminal);
            DNode paramNode = hints.firstChild();
            while (paramNode != nsNone) {
               if (paramNode == nsHintValue) {
                  TerminalInfo param = paramNode.Terminal();
                  if (param == tsIdentifier) {
                     ref_t subject = scope.mapSubject(param, false);
                     if (subject == 0)
                        subject = moduleScope->module->mapSubject(param, false);

                     writer.appendNode(lxTemplateSubject, subject);
                  }
                  else scope.raiseError(errInvalidHintValue, param);
               }
               paramNode = paramNode.nextNode();
            }
            writer.closeNode();
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());
      }
      else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

void Compiler :: compileMethodHints(DNode hints, SyntaxWriter& writer, MethodScope& scope, bool warningsOnly)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
   ModuleScope* moduleScope = scope.moduleScope;

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      //HOTFIX : if it is a virtual subject
      if (!warningsOnly && hints.firstChild() != nsHintValue && scope.isVirtualSubject(terminal)) {
         writer.appendNode(lxType, scope.mapSubject(terminal));
      }
      else {
         ref_t hintRef = mapHint(hints, *moduleScope);

         if (hintRef == moduleScope->warnHint) {
            DNode value = hints.select(nsHintValue);
            TerminalInfo level = value.Terminal();
            if (StringHelper::compare(level, "w2")) {
               writer.appendNode(lxWarningMask, WARNING_MASK_1);
            }
            else if (StringHelper::compare(level, "w3")) {
               writer.appendNode(lxWarningMask, WARNING_MASK_2);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
         }
         else if (warningsOnly) {
            // ignore other hints for implementation stage
         }
         else if (moduleScope->subjectHints.exist(hintRef)) {
            writer.appendNode(lxType, hintRef);
         }
         else if (hintRef == moduleScope->genericHint) {
            writer.appendNode(lxClassMethodAttr, tpGeneric);

            scope.generic = true;
         }
         else if (hintRef == moduleScope->sealedHint) {
            writer.appendNode(lxClassMethodAttr, scope.privat ? tpPrivate : tpSealed);
         }
         else if (hintRef == moduleScope->stackHint) {
            writer.appendNode(lxClassMethodAttr, tpStackSafe);
         }
         else if (hintRef == moduleScope->embedHint) {
            writer.appendNode(lxClassMethodAttr, tpEmbeddable);
         }
         else if (hintRef == moduleScope->actionHint) {
            writer.appendNode(lxActionAttr);
         }
         else if (hintRef != 0) {
            // Method templates can be applied only for methods with custom verbs
            if (getVerb(scope.message) == EVAL_MESSAGE_ID && getSignature(scope.message) != 0) {
               ident_t signature = moduleScope->module->resolveSubject(getSignature(scope.message));
               IdentifierString customVerb(signature, StringHelper::find(signature, '&', getlength(signature)));
               ref_t messageSubject = moduleScope->module->mapSubject(customVerb, false);

               declareTemplateInfo(hints, *classScope, hintRef, messageSubject);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      }
      hints = hints.nextNode();
   }
}

void Compiler :: compileLocalHints(DNode hints, CodeScope& scope, ref_t& type, ref_t& classRef, int& size)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      ref_t hintRef = mapHint(hints, *scope.moduleScope);

      if (hintRef != 0) {
         if (hintRef == scope.moduleScope->integerHint) {
            TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
            if (sizeValue.symbol == tsInteger) {
               size = StringHelper::strToInt(sizeValue.value);
               // !! HOTFIX : allow only 1,2,4 or 8
               if (size == 1 || size == 2 || size == 4) {
                  classRef = -1;
               }
               else if (size == 8) {
                  classRef = -2;
               }
               else scope.raiseError(errInvalidHint, terminal);
            }
            else scope.raiseError(errInvalidHint, terminal);
         }
         else if (scope.moduleScope->subjectHints.exist(hintRef)) {
            if (type == 0 && classRef == 0) {
               type = hintRef;

               TerminalInfo target = hints.firstChild().Terminal();
               if (target != nsNone) {
                  if (target.symbol == tsInteger) {
                     size = StringHelper::strToInt(target);

                     classRef = -3; // NOTE : -3 means an array of type
                  }
                  else scope.raiseError(errInvalidHint, terminal);
               }
               else classRef = scope.moduleScope->subjectHints.get(type);
            }
            else scope.raiseError(errInvalidHint, terminal);
         }
         else {
            if (type != 0)
               scope.raiseError(errInvalidHint, terminal);

            TemplateInfo templateInfo;
            templateInfo.templateRef = hintRef;
            templateInfo.sourceCol = terminal.Col();
            templateInfo.sourceRow = terminal.Row();

            declareTemplateParameters(hints, *scope.moduleScope, templateInfo.parameters);

            ReferenceNs name(scope.moduleScope->module->resolveSubject(hintRef));
            RoleMap::Iterator it = templateInfo.parameters.start();
            while (!it.Eof()) {
               name.append('@');
               name.append(scope.moduleScope->module->resolveSubject(*it));

               it++;
            }

            //HOTFIX: validate if the subjects are virtual
            if (scope.isVirtualSubject(hints.firstChild().Terminal())) {
               classRef = -6;
               type = scope.moduleScope->module->mapSubject(name, false);
            }
            else {
               classRef = generateTemplate(*scope.moduleScope, templateInfo, scope.moduleScope->resolveIdentifier(name));
               if (classRef == 0)
                  scope.raiseError(errInvalidHint, terminal);
            }
         }
      }
      else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

      hints = hints.nextNode();
   }
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
      scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
      appendTerminalInfo(scope.writer, node.FirstTerminal());
      scope.writer->closeNode();

      scope.writer->removeBookmark();

      scope.writer->newNode(lxElse, scope.moduleScope->falseReference);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild().nextNode();

      //_writer.declareBlock(*scope.tape);

      DNode statement = thenCode.firstChild();
      if (statement.nextNode() != nsNone || statement == nsCodeEnd) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(statement, scope, 0);

      scope.writer->closeNode();

      scope.writer->closeNode();

      option = option.nextNode();
   }
   if (option == nsLastSwitchOption) {
      scope.writer->newNode(lxElse);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild();

      //_writer.declareBlock(*scope.tape);

      DNode statement = thenCode.firstChild();
      if (statement.nextNode() != nsNone || statement == nsCodeEnd) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(statement, scope, 0);

      scope.writer->closeNode();
   }

   scope.writer->closeNode();
}

void Compiler :: compileVariable(DNode node, CodeScope& scope, DNode hints)
{
   TerminalInfo terminal = node.Terminal();

   if (!scope.locals.exist(terminal)) {
      ref_t type = 0;
      ref_t classRef = 0;
      int size = 0;
      compileLocalHints(hints, scope, type, classRef, size);

      ObjectInfo variable(okLocal, 0, classRef, type);

      ClassInfo localInfo;
      bool bytearray = false;
      if (isTemplateRef(classRef)) {
         variable.kind = okTemplateLocal;
      }
      else if (isPrimitiveRef(classRef)) {
         if (classRef == -1) {
            localInfo.header.flags = elDebugDWORD;
         }
         else if (classRef == -2) {
            localInfo.header.flags = elDebugQWORD;
         }
         else if (classRef == -4) {
            localInfo.header.flags = elDebugReal64;
         }
         else if (classRef == -3) {
            scope.moduleScope->loadClassInfo(localInfo, scope.moduleScope->subjectHints.get(type), true);
            size = size * localInfo.size;
            bytearray = true;
         }
      }
      else if (classRef != 0) {
         scope.moduleScope->loadClassInfo(localInfo, classRef, true);

         if (isEmbeddable(localInfo))
            size = localInfo.size;
      }      

      if (size > 0) {
         if (!allocateStructure(scope, size, localInfo.header.flags, bytearray, variable))
            scope.raiseError(errInvalidOperation, node.Terminal());

         // make the reservation permanent
         scope.saved = scope.reserved;

         if (bytearray) {
            switch (localInfo.header.flags & elDebugMask)
            {
               case elDebugDWORD:
                  if (localInfo.size == 4) {
                     scope.writer->newNode(lxIntsVariable, size);
                     scope.writer->appendNode(lxTerminal, terminal.value);
                     scope.writer->appendNode(lxLevel, variable.param);
                     scope.writer->closeNode();
                  }
                  else if (localInfo.size == 2) {
                     scope.writer->newNode(lxShortsVariable, size);
                     scope.writer->appendNode(lxTerminal, terminal.value);
                     scope.writer->appendNode(lxLevel, variable.param);
                     scope.writer->closeNode();
                  }
                  else if (localInfo.size == 1) {
                     scope.writer->newNode(lxBytesVariable, size);
                     scope.writer->appendNode(lxTerminal, terminal.value);
                     scope.writer->appendNode(lxLevel, variable.param);
                     scope.writer->closeNode();
                  }
                  break;
   //            case elDebugQWORD:
   //               break;
   //            default:
   //               // HOTFIX : size should be provide only for dynamic variables
   //               scope.writer->newNode(lxBinaryVariable, size);
   //               scope.writer->appendNode(lxTerminal, terminal.value);
   //               scope.writer->appendNode(lxLevel, variable.param);
   //               
   //               //if (type != 0) {
   //               //   ref_t classRef = scope.moduleScope->typeHints.get(type);
   //               //
   //               //   scope.writer->appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
   //               //}
   //               
   //               scope.writer->closeNode();
   //               break;
            }
         }
         else {
            switch (localInfo.header.flags & elDebugMask)
            {
               case elDebugDWORD:
                  scope.writer->newNode(lxIntVariable);
                  scope.writer->appendNode(lxTerminal, terminal.value);
                  scope.writer->appendNode(lxLevel, variable.param);
                  scope.writer->closeNode();
                  break;
               case elDebugQWORD:
                  scope.writer->newNode(lxLongVariable);
                  scope.writer->appendNode(lxTerminal, terminal.value);
                  scope.writer->appendNode(lxLevel, variable.param);
                  scope.writer->closeNode();
                  break;
               case elDebugReal64:
                  scope.writer->newNode(lxReal64Variable);
                  scope.writer->appendNode(lxTerminal, terminal.value);
                  scope.writer->appendNode(lxLevel, variable.param);
                  scope.writer->closeNode();
                  break;
               default:
                  scope.writer->newNode(lxBinaryVariable);
                  scope.writer->appendNode(lxTerminal, terminal.value);
                  scope.writer->appendNode(lxLevel, variable.param);

                  if (type != 0) {
                     ref_t classRef = scope.moduleScope->subjectHints.get(type);
                  
                     scope.writer->appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
                  }

                  scope.writer->closeNode();
                  break;
            }
         }
      }
      else {
         int level = scope.newLocal();

         scope.writer->newNode(lxVariable);
         scope.writer->appendNode(lxTerminal, terminal.value);
         scope.writer->appendNode(lxLevel, level);
         scope.writer->closeNode();

         variable.param = level;

         size = 0; // to indicate assigning by ref
      }

      DNode assigning = node.firstChild();
      if (assigning == nsAssigning) {
         scope.writer->newNode(lxAssigning, size);
         writeTerminal(terminal, scope, variable);

         compileAssigningExpression(node, assigning, scope, variable);

         scope.writer->closeNode();
      }

      scope.mapLocal(node.Terminal(), variable.param, type, classRef, size);
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
      case okWideLiteralConstant:
         scope.writer->newNode(lxConstantWideStr, object.param);
         break;
      case okCharConstant:
         scope.writer->newNode(lxConstantChar, object.param);
         break;
      case okIntConstant:
         scope.writer->newNode(lxConstantInt, object.param);

         scope.writer->appendNode(lxValue, 
            StringHelper::strToULong(scope.moduleScope->module->resolveConstant(object.param), 16));

         break;
      case okLongConstant:
         scope.writer->newNode(lxConstantLong, object.param);
         break;
      case okRealConstant:
         scope.writer->newNode(lxConstantReal, object.param);
         break;
      case okTemplateLocal:
         scope.writer->newNode(lxLocal, object.param);
         break;
      case okLocal:
      case okParam:
         if (object.extraparam == -1) {
            scope.writer->newNode(lxCondBoxing);
            scope.writer->appendNode(lxLocal, object.param);
         }
         else scope.writer->newNode(lxLocal, object.param);
         break;
      case okThisParam:
         if (object.extraparam == -1) {
            scope.writer->newNode(lxCondBoxing);
            scope.writer->appendNode(lxThisLocal, object.param);
         }
         else scope.writer->newNode(lxThisLocal, object.param);         
         break;
      case okSuper:
         scope.writer->newNode(lxLocal, 1);
         break;
      case okField:
      case okOuter:
         scope.writer->newNode(lxField, object.param);
         break;
      case okOuterField:
         scope.writer->newNode(lxFieldExpression);
         scope.writer->appendNode(lxField, object.param);
         scope.writer->appendNode(lxResultField, object.extraparam);
         break;
      case okLocalAddress:
         scope.writer->newNode(lxBoxing);
         scope.writer->appendNode(lxLocalAddress, object.param);
         break;
      case okFieldAddress:
         scope.writer->newNode(lxBoxing);
         scope.writer->appendNode(lxFieldAddress, object.param);
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
      case okExtMessageConstant:
         scope.writer->newNode(lxExtMessageConstant, object.param);
         break;
      case okSignatureConstant:
         scope.writer->newNode(lxSignatureConstant, object.param);
         break;
      case okSubject:
         scope.writer->newNode(lxLocalAddress, object.param);
         break;
      case okBlockLocal:
         scope.writer->newNode(lxBlockLocal, object.param);
         break;
      case okParams:
         scope.writer->newNode(lxArgBoxing);
         scope.writer->appendNode(lxBlockLocalAddr, object.param);
         break;
      case okObject:
         scope.writer->newNode(lxResult);
         break;
      case okConstantRole:
         scope.writer->newNode(lxConstantSymbol, object.param);
         break;
      case okTemplateTarget:
         scope.writer->newNode(lxTemplateTarget, object.param);
         // HOTFIX : tempalte type is not an actual type, so it should be saved in special way and cleared after
         scope.writer->appendNode(lxTemplateFieldType, object.type);
         object.type = 0;
         break;
      case okExternal:
      case okInternal:
         // HOTFIX : external / internal node will be declared later
         return;
   }

   appendObjectInfo(scope, object);
   if (terminal != nsNone)
      appendTerminalInfo(scope.writer, terminal);

   scope.writer->closeNode();
}

ObjectInfo Compiler :: compileTerminal(DNode node, CodeScope& scope)
{
   TerminalInfo terminal = node.Terminal();

   ObjectInfo object;
   if (terminal==tsLiteral) {
      object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(terminal));
   }
   else if (terminal == tsWide) {
      object = ObjectInfo(okWideLiteralConstant, scope.moduleScope->module->mapConstant(terminal));
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
      case nsNestedClass:
      case nsRetStatement:
         if (objectNode.Terminal() != nsNone) {
            result = compileClosure(objectNode, scope, 0);
            break;
         }
      case nsSubCode:
      case nsSubjectArg:
      case nsMethodParameter:
         result = compileClosure(member, scope, 0);
         break;
      case nsInlineClosure:
         result = compileClosure(member.firstChild(), scope, HINT_CLOSURE);
         break;
      case nsInlineExpression:
         result = compileClosure(objectNode, scope, HINT_ACTION);
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
         result = compileTerminal(objectNode, scope);
   }

   return result;
}

ObjectInfo Compiler :: compileMessageReference(DNode node, CodeScope& scope)
{
   DNode arg = node.firstChild();

   TerminalInfo terminal = node.Terminal();
   IdentifierString signature;
   ref_t verb_id = 0;
   int paramCount = -1;
   ref_t extensionRef = 0;
   if (terminal == tsIdentifier) {
      verb_id = _verbs.get(terminal.value);
      if (verb_id == 0) {
         signature.copy(terminal.value);
      }
   }
   else {
      ident_t message = terminal.value;

      int subject = 0;
      int param = 0;
      for (int i = 0; i < getlength(message); i++) {
         if (message[i] == '&' && subject == 0) {
            signature.copy(message, i);
            verb_id = _verbs.get(signature);
            if (verb_id != 0) {
               subject = i + 1;
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
            if (message[i+1] == ']') {
               //HOT FIX : support open argument list
               paramCount = OPEN_ARG_COUNT;
            }
            else if (message[getlength(message) - 1] == ']') {
               signature.copy(message + i + 1, getlength(message) - i - 2);
               paramCount = StringHelper::strToInt(signature);
               if (paramCount > 12)
                  scope.raiseError(errInvalidSubject, terminal);
            }
            else scope.raiseError(errInvalidSubject, terminal);

            param = i;
         }
         else if (message[i] >= 65 || (message[i] >= 48 && message[i] <= 57)) {
         }
         else if (message[i] == ']' && i == (getlength(message) - 1)) {
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

      if (paramCount == OPEN_ARG_COUNT) {
         // HOT FIX : support open argument list
         ref_t openArgType = retrieveKey(scope.moduleScope->subjectHints.start(), scope.moduleScope->paramsReference, 0);
         if (!emptystr(signature))
            signature.append('&');

         signature.append(scope.moduleScope->module->resolveSubject(openArgType));
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
   else message.append('0' + paramCount);
   message.append('#');
   if (verb_id != 0) {
      message.append(0x20 + verb_id);
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

   writeTerminal(TerminalInfo(), scope, retVal);

   return retVal;
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
         ref_t id = scope.mapSubject(verb, signature);

         // if followed by argument list - it is EVAL verb
         if (arg != nsNone) {
            // HOT FIX : strong types cannot be used as a custom verb with a parameter
            if (scope.moduleScope->subjectHints.exist(id))
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

      ref_t subjRef = scope.mapSubject(subject, signature);

      arg = arg.nextNode();

      // skip an argument
      if (arg == nsMessageParameter) {
         // if it is an open argument list
         if (arg.nextNode() != nsSubjectArg && scope.moduleScope->subjectHints.exist(subjRef, scope.moduleScope->paramsReference)) {
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
   else {
      if (scope.moduleScope->extensionHints.exist(messageRef)) {
         ref_t classRef = resolveObjectReference(scope, object);
         // if class reference available - select the possible type
         if (classRef != 0) {
            SubjectMap::Iterator it = scope.moduleScope->extensionHints.start();
            while (!it.Eof()) {
               if (it.key() == messageRef) {
                  if (scope.moduleScope->subjectHints.exist(*it, classRef)) {
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

ObjectInfo Compiler :: compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
   appendTerminalInfo(scope.writer, node.FirstTerminal());
   scope.writer->closeNode();

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

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   ObjectInfo retVal(okObject);

   // HOTFIX : recognize SET_REFER_MESSAGE_ID
   if (operator_id == REFER_MESSAGE_ID && node.nextNode() == nsAssigning)
      operator_id = SET_REFER_MESSAGE_ID;

   bool dblOperator = IsDoubleOperator(operator_id);
   bool notOperator = IsInvertedOperator(operator_id);

   ObjectInfo operand = compileExpression(node, scope, 0, 0);
   if (dblOperator)
      compileExpression(node.nextNode().firstChild(), scope, 0, 0);

   recordDebugStep(scope, node.Terminal(), dsStep);

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

   if (object.kind == okNil && operator_id == EQUAL_MESSAGE_ID) {
      scope.writer->insert(lxNilOp, operator_id);
      // HOT FIX : the result of comparision with $nil is always bool
      scope.writer->appendNode(lxType, scope.moduleScope->boolType);
   }
   // HOTFIX : primitive operations can be implemented only in the method
   // because the symbol implementations do not open a new stack frame
   else if (scope.getScope(Scope::slMethod) == NULL && !IsCompOperator(operator_id)) {
      scope.writer->insert(lxCalling, encodeMessage(0, operator_id, dblOperator ? 2 : 1));
   }
   else  scope.writer->insert(lxOp, operator_id);

   appendObjectInfo(scope, retVal);
   appendTerminalInfo(scope.writer, node.FirstTerminal());

   scope.writer->closeNode();

   if (IsCompOperator(operator_id) && object.kind != okNil) {
      if (notOperator) {
         scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
         scope.writer->closeNode();

         scope.writer->insert(lxBoolOp, NOT_MESSAGE_ID);
         scope.writer->appendNode(lxIfValue, scope.moduleScope->trueReference);
         scope.writer->appendNode(lxElseValue, scope.moduleScope->falseReference);
         scope.writer->appendNode(lxType, scope.moduleScope->boolType);
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
   //bool templateCall = false;
   int methodHint = classReference != 0 ? scope.moduleScope->checkMethod(classReference, messageRef, classFound, retVal.type) : 0;
   int callType = methodHint & tpMask;

   if (target.kind == okConstantClass) {
      retVal.param = target.param;

      // constructors are always sealed
      callType = tpSealed;
   }
   else if (target.kind == okThisParam && callType == tpPrivate) {
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
   //else if (target.kind == okTemplateTarget) {
   //   templateCall = true;
   //}

   if (dispatchCall) {
      scope.writer->insert(lxDirectCalling, encodeVerb(DISPATCH_MESSAGE_ID));

      scope.writer->appendNode(lxMessage, messageRef);
      scope.writer->appendNode(lxCallTarget, classReference);
      scope.writer->appendNode(lxStacksafe);
   }
   else if (callType == tpClosed || callType == tpSealed) {
      scope.writer->insert(callType == tpClosed ? lxSDirctCalling : lxDirectCalling, messageRef);

      scope.writer->appendNode(lxCallTarget, classReference);
      if (test(methodHint, tpStackSafe))
         scope.writer->appendNode(lxStacksafe);
      if (test(methodHint, tpEmbeddable))
         scope.writer->appendNode(lxEmbeddable);
   }
   //else if (templateCall) {
   //   scope.writer->insert(lxTemplateCalling, messageRef);
   //}
   else {
      scope.writer->insert(lxCalling, messageRef);

      // if the class found and the message is not supported - warn the programmer and raise an exception
      if (classFound && callType == tpUnknown) {
         scope.writer->appendNode(lxCallTarget, classReference);

         //scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.FirstTerminal());
      }
   }

   // the result of get&type message should be typed
   if (paramCount == 0 && getVerb(messageRef) == GET_MESSAGE_ID && scope.moduleScope->subjectHints.exist(signRef)) {
      retVal.type = signRef;
   }

   recordDebugStep(scope, node.Terminal(), dsStep);

   appendObjectInfo(scope, retVal);
   appendTerminalInfo(scope.writer, node.FirstTerminal());

   // define the message target if required
   if (target.kind == okConstantRole) {
      scope.writer->newNode(lxOverridden);
      writeTerminal(TerminalInfo(), scope, target);
      scope.writer->closeNode();
   }

   scope.writer->closeNode();

   return retVal;
}

ref_t Compiler :: compileMessageParameters(DNode node, CodeScope& scope)
{
   bool argsUnboxing = false;
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount, argsUnboxing);

   int paramMode = 0;
   //// HOTFIX : if open argument list has to be unboxed
   //// alternative boxing routine should be used (using a temporal variable)
   //if (argsUnboxing)
   //   paramMode |= HINT_ALTBOXING;

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

      ref_t subjRef = scope.mapSubject(subject);

      arg = arg.nextNode();

      // skip an argument
      if (arg == nsMessageParameter) {
         // if it is an open argument list
         if (arg.nextNode() != nsSubjectArg && scope.moduleScope->subjectHints.exist(subjRef, scope.moduleScope->paramsReference)) {
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

   return compileMessage(node, scope, object, messageRef, 0);
}

ObjectInfo Compiler :: compileAssigning(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   int assignMode = 0;

   DNode member = node.nextNode();

   // if it setat operator
   if (member == nsL0Operation) {
      return compileOperations(node, scope, object, mode);
   }
   else if (member == nsMessageOperation) {
      // if it is shorthand property settings
      DNode arg = member.firstChild();
      if (arg != nsNone || member.nextNode() != nsAssigning)
         scope.raiseError(errInvalidSyntax, member.FirstTerminal());

      ref_t subject = scope.moduleScope->mapSubject(member.Terminal());
      //HOTFIX : support lexical subjects
      if (subject == 0)
         subject = scope.moduleScope->module->mapSubject(member.Terminal(), false);

      ref_t messageRef = encodeMessage(subject, SET_MESSAGE_ID, 1);

      ref_t extensionRef = mapExtension(scope, messageRef, object);

      if (extensionRef != 0) {
         //HOTFIX: A proper method should have a precedence over an extension one
         if (scope.moduleScope->checkMethod(resolveObjectReference(scope, object), messageRef) == tpUnknown) {
            object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
         }
      }

      if (scope.moduleScope->subjectHints.exist(subject)) {
         compileExpression(member.nextNode().firstChild(), scope, subject, 0);
      }
      else compileExpression(member.nextNode().firstChild(), scope, 0, 0);

      return compileMessage(member, scope, object, messageRef, 0);
   }
   else {
      ObjectInfo currentObject = object;

      LexicalType assignType = lxAssigning;
      ref_t classReference = 0;
      int size = 0;
      if (object.kind == okLocalAddress) {
         classReference = object.extraparam;

         if (isPrimitiveRef(classReference)) {
            if (classReference == -1 || classReference == -2) {
               size = scope.moduleScope->defineSubjectSize(object.type);
            }
         }
         else size = scope.moduleScope->defineStructSize(classReference);
      }
      else if (object.kind == okFieldAddress) {
         size = scope.moduleScope->defineSubjectSize(object.type);
      }
      else if (object.kind == okLocal || object.kind == okField || object.kind == okOuterField) {

      }
      else if (object.kind == okParam || object.kind == okOuter) {
         // Compiler magic : allowing to assign byref / variable parameter
         classReference = scope.moduleScope->subjectHints.get(object.type);
         ClassInfo info;
         scope.moduleScope->loadClassInfo(info, classReference);
         if (test(info.header.flags, elWrapper)) {
            size = info.size;
            currentObject.kind = (object.kind == okParam) ? okParamField : okOuterField;
         }
         // Compiler magic : allowing to assign outer local variables
         else if (object.kind == okOuter) {
            InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);

            if (!closure->markAsPresaved(object))
               scope.raiseError(errInvalidOperation, node.Terminal());
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
      else if (object.kind == okTemplateTarget) {
         // if it is a template field
         // treates it like a normal field
         currentObject.kind = okField;
         // HOTFIX : provide virtual typecasting for template field
         if (currentObject.type == 0)
            assignMode |= HINT_VIRTUAL_FIELD;
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());

      currentObject = compileAssigningExpression(node, member, scope, currentObject, assignMode);

      if (size >= 0) {
         scope.writer->insert(assignType, size);
         scope.writer->closeNode();
      }

      return currentObject;
   }   
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
      if (member == nsMessageOperation) {
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
            //HOTFIX : typecast the extension target if required
            if (test(flags, elExtension) && roleClass.fieldTypes.exist(-1)) {
               scope.writer->insert(lxTypecasting, encodeMessage(roleClass.fieldTypes.get(-1), GET_MESSAGE_ID, 0));
               scope.writer->closeNode();
            }
         }
      }
      // if the symbol VMT can be used as an external role
      if (test(flags, elStateless)) {
         role = ObjectInfo(okConstantRole, role.param);
      }
   }

   // if it is a generic role
   if (role.kind != okConstantRole) {
      scope.writer->newNode(lxOverridden);
      role = compileExpression(roleNode, scope, 0, 0);
      scope.writer->closeNode();
   }

   // override standard message compiling routine
   node = node.nextNode();

   return compileExtensionMessage(node, scope, object, role);
}

ObjectInfo Compiler :: compileExtensionMessage(DNode node, CodeScope& scope, ObjectInfo object, ObjectInfo role)
{
   ref_t messageRef = compileMessageParameters(node, scope);

   ObjectInfo retVal = compileMessage(node, scope, role, messageRef, HINT_EXTENSION_MODE);

   return retVal;
}

bool Compiler :: declareActionScope(DNode& node, ClassScope& scope, DNode argNode, SyntaxWriter& writer, ActionScope& methodScope, int mode, bool alreadyDeclared)
{
   bool lazyExpression = !test(mode, HINT_CLOSURE) && isReturnExpression(node.firstChild());

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
      else {
         ref_t actionRef = scope.moduleScope->actionHints.get(methodScope.message);
         if (actionRef)
            parentRef = actionRef;
      }

      compileParentDeclaration(DNode(), scope, parentRef);
   }

   // HOT FIX : mark action as stack safe if the hint was declared in the parent class
   methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);   

   return lazyExpression;
}

void Compiler :: compileAction(DNode node, ClassScope& scope, DNode argNode, int mode, bool alreadyDeclared)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   ActionScope methodScope(&scope);
   bool lazyExpression = declareActionScope(node, scope, argNode, writer, methodScope, mode, alreadyDeclared);

   writer.newNode(lxClassMethod, methodScope.message);
   writer.appendNode(lxSourcePath); // the source path is first string

   // if it is single expression
   if (!lazyExpression) {
      compileActionMethod(node, writer, methodScope);
   }
   else compileLazyExpressionMethod(node.firstChild(), writer, methodScope);

   writer.closeNode();  // closing method

   //HOTFIX : recognize if it is nested template action
   //!!should be refactored
   if (scope.getScope(Scope::slOwnerClass) != &scope && ((InlineClassScope*)&scope)->templateMode) {
      InlineClassScope* inlineScope = (InlineClassScope*)&scope;

      // import fields
      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = inlineScope->outers.start();
      while (!outer_it.Eof()) {
         writer.newNode(lxTemplateField, (*outer_it).reference);
         writer.appendNode(lxTerminal, outer_it.key());
         writer.closeNode();

         outer_it++;
      }

      writer.closeNode();

      inlineScope->templateRef = scope.moduleScope->mapNestedTemplate();

      _Memory* section = scope.moduleScope->module->mapSection(inlineScope->templateRef | mskSyntaxTreeRef, false);

      scope.syntaxTree.save(section);
   }
   else {
      writer.closeNode();

      if (!alreadyDeclared)
         generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));

      generateClassImplementation(scope);
   }
}

void Compiler :: compileNestedVMT(DNode node, DNode parent, InlineClassScope& scope)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   compileParentDeclaration(parent, scope);

   DNode member = node.firstChild();

   // nested class is sealed if it has no parent
   if (!test(scope.info.header.flags, elClosed))
      writer.appendNode(lxClassFlag, elSealed);

   compileVMT(member, writer, scope);

   if (scope.templateMode) {
      // import fields
      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      while (!outer_it.Eof()) {
         writer.newNode(lxTemplateField, (*outer_it).reference);
         writer.appendNode(lxTerminal, outer_it.key());
         writer.closeNode();

         outer_it++;
      }

      writer.closeNode();

      scope.templateRef = scope.moduleScope->mapNestedTemplate();

      _Memory* section = scope.moduleScope->module->mapSection(scope.templateRef | mskSyntaxTreeRef, false);

      scope.syntaxTree.save(section);
   }
   else {
      writer.closeNode();

      generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));
      generateClassImplementation(scope);
   }
}

ObjectInfo Compiler :: compileClosure(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      ownerScope.writer->newNode(lxConstantSymbol, scope.reference);
      ownerScope.writer->appendNode(lxTarget, scope.reference);
      ownerScope.writer->closeNode();

      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, scope.reference, scope.reference/*, scope.moduleScope->defineType(scope.reference)*/);
   }
   else {
      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         ownerScope.writer->newNode(lxStruct, scope.info.size);
         ownerScope.writer->appendNode(lxTarget, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node.Terminal());
      }
      else if (scope.templateMode) {
         ownerScope.writer->newNode(lxNestedTemplate, scope.info.fields.Count());
         ownerScope.writer->appendNode(lxTemplate, scope.templateRef);
         ownerScope.writer->appendNode(lxNestedTemplateParent, scope.info.header.parentRef);
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

         ownerScope.writer->newNode((*outer_it).preserved ? lxOuterMember : lxMember, (*outer_it).reference);
         ownerScope.writer->newBookmark();

         writeTerminal(TerminalInfo(), ownerScope, info);

         ownerScope.writer->removeBookmark();
         ownerScope.writer->closeNode();

         outer_it++;
      }

      ownerScope.writer->closeNode();

      return ObjectInfo(okObject, scope.reference);
   }
}

ObjectInfo Compiler :: compileClosure(DNode node, CodeScope& ownerScope, int mode)
{
   InlineClassScope scope(&ownerScope, ownerScope.moduleScope->mapNestedExpression());

   // if it is a lazy expression / multi-statement closure without parameters
   if (node == nsSubCode || node == nsInlineClosure) {
      compileAction(node, scope, DNode(), mode);
   }
   // if it is a closure / labda function with a parameter
   else if (node == nsObject && testany(mode, HINT_ACTION | HINT_CLOSURE)) {
      compileAction(node.firstChild(), scope, node, mode);
   }
   // if it is an action code block
   else if (node == nsMethodParameter || node == nsSubjectArg) {
      compileAction(goToSymbol(node, nsInlineExpression), scope, node, 0);
   }
   // if it is inherited nested class
   else if (node.Terminal() != nsNone) {
	   // inherit parent
      compileNestedVMT(node.firstChild(), node, scope);
   }
   // if it is normal nested class
   else compileNestedVMT(node, DNode(), scope);

   return compileClosure(node, ownerScope, scope, mode);
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
      if (!scope.moduleScope->subjectHints.exist(subj)) {
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

      info = ObjectInfo(okObject, 0, 0, subj);
   }

   scope.freeSpace();

   return info;
}

ObjectInfo Compiler :: compileNewOperator(DNode node, CodeScope& scope, int mode)
{
   ObjectInfo retVal(okObject);
   scope.writer->newBookmark();

   // Compiler magic : if the argument is the number and the object is a strong subject
   ref_t subject = scope.mapSubject(node.Terminal());

   //compileExpression(node.nextNode().firstChild(), scope, 0, 0);
   compileObject(node.nextNode(), scope, 0);

   int flags = subject != 0 ? scope.moduleScope->getClassFlags(scope.moduleScope->subjectHints.get(subject)) : 0;

   // HOTFIX : provide the expression result
   scope.writer->insert(lxTypecasting, encodeMessage(retrieveKey(scope.moduleScope->subjectHints.start(), scope.moduleScope->intReference, 0), GET_MESSAGE_ID, 0));
   scope.writer->closeNode();

   scope.writer->insert(lxNewOp);

   if (isEmbeddable(flags)) {
      retVal.param = -3;
   }
   else {
      retVal.param = -5;
      // HOTFIX : allow lexical subjects as well
      if (subject == 0)
         subject = scope.moduleScope->module->mapSubject(node.Terminal(), false);
   }
   retVal.type = subject;

   appendObjectInfo(scope, retVal);
   appendTerminalInfo(scope.writer, node.FirstTerminal());

   scope.writer->closeNode();

   scope.writer->removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileExpression(DNode node, CodeScope& scope, ref_t targetType, int mode)
{
   scope.writer->newBookmark();

   ObjectInfo objectInfo;
   if (node != nsObject) {
      DNode member = node.firstChild();

      DNode operation = member.nextNode();
      if (operation == nsNewOperator) {
         objectInfo = compileNewOperator(member, scope, mode);
      }
      else if (operation != nsNone) {
         if (member == nsObject) {
            objectInfo = compileObject(member, scope, mode);
         }
         if (findSymbol(member, nsAssigning)) {
            objectInfo = compileAssigning(member, scope, objectInfo, mode);
         }
         else if (findSymbol(member, nsAltMessageOperation)) {
            scope.writer->insert(lxVariable);
            scope.writer->closeNode();

            scope.writer->newNode(lxAlt);
            scope.writer->newBookmark();
            scope.writer->appendNode(lxResult);
            objectInfo = compileOperations(member, scope, objectInfo, mode);
            scope.writer->removeBookmark();
            scope.writer->closeNode();

            scope.writer->appendNode(lxReleasing, 1);
         }
         else objectInfo = compileOperations(member, scope, objectInfo, mode);
      }
      else objectInfo = compileObject(member, scope, mode);
   }
   else objectInfo = compileObject(node, scope, mode);

   // if it is try-catch statement
   if (findSymbol(node.firstChild(), nsCatchMessageOperation)) {
      scope.writer->insert(lxTrying);
      scope.writer->closeNode();
   }

   if (targetType != 0) {
      scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));

      appendTerminalInfo(scope.writer, node.FirstTerminal());
      scope.writer->appendNode(lxType, targetType);

      scope.writer->closeNode();
   }

   scope.writer->removeBookmark();

   return objectInfo;
}

ObjectInfo Compiler :: compileAssigningExpression(DNode node, DNode assigning, CodeScope& scope, ObjectInfo target, int mode)
{      
   ref_t targetType = target.type;
   switch (target.kind)
   {
      case okLocal:
      case okField:
      case okOuterField:
      case okLocalAddress:
      case okFieldAddress:
      case okParamField:
      case okOuter:
         break;
      case okTemplateLocal:
         mode |= HINT_VIRTUAL_FIELD; // HOTFIX : typecast it like a virtual field
         break;
      case okUnknown:
         scope.raiseError(errUnknownObject, node.Terminal());
      default:
         scope.raiseError(errInvalidOperation, node.Terminal());
         break;
   }

   scope.writer->newBookmark();

   ObjectInfo objectInfo = compileExpression(assigning.firstChild(), scope, 0, 0);

   if (test(mode, HINT_VIRTUAL_FIELD)) {
      // HOTFIX : if it is a virtual field, provide an idle typecast
      scope.writer->insert(lxTypecasting);
      appendTerminalInfo(scope.writer, node.FirstTerminal());
      scope.writer->closeNode();
   }
   else if (targetType != 0) {
   //   ref_t ref = resolveObjectReference(scope, objectInfo);

   //   if (isPrimitiveRef(ref)) {
   //      scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));
   //      appendTerminalInfo(scope.writer, node.FirstTerminal());
   //      scope.writer->closeNode();
   //      //scope.writer->insert(lxBoxing);
   //      //scope.writer->appendNode(lxTarget, target.extraparam);
   //      //appendTerminalInfo(scope.writer, node.FirstTerminal());
   //      //scope.writer->closeNode();
   //   }
      /*else */if (objectInfo.type != targetType) {
         scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));
         appendTerminalInfo(scope.writer, node.FirstTerminal());
         scope.writer->closeNode();
      }      
   }
   else if (isPrimitiveRef(target.extraparam)) {
      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, resolveObjectReference(scope, objectInfo), true);

      if (target.extraparam == -1 && ((info.header.flags & elDebugMask)  == elDebugDWORD)) {

         // allow assigning an int wrapper to the primitive int
      }
      else scope.raiseError(errInvalidOperation, assigning.FirstTerminal());
   }
   else if (target.kind != okOuterField && target.extraparam > 0) {
      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, target.extraparam, false);

      // wrapper class can be used in this case
      if (test(info.header.flags, elWrapper)) {
         target.type = info.fieldTypes.get(0);

         scope.writer->insert(lxTypecasting, encodeMessage(target.type, GET_MESSAGE_ID, 0));
         scope.writer->closeNode();
         
         scope.writer->insert(lxBoxing, info.size);
         scope.writer->appendNode(lxTarget, target.extraparam);
         appendTerminalInfo(scope.writer, node.FirstTerminal());         
         scope.writer->closeNode();
      }
      //// HOTFIX : to allow boxing primitive array
      //else if (test(info.header.flags, elDynamicRole | elStructureRole) && objectInfo.kind == okLocalAddress && objectInfo.extraparam == -3 &&
      //   (objectInfo.type == info.fieldTypes.get(-1)))
      //{
      //   scope.writer->insert(lxBoxing, info.size);
      //   scope.writer->appendNode(lxTarget, target.extraparam);
      //   appendTerminalInfo(scope.writer, node.FirstTerminal());
      //   scope.writer->closeNode();
      //}
      else scope.raiseError(errInvalidOperation, assigning.FirstTerminal());
   }

   scope.writer->removeBookmark();

   return objectInfo;
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

      // HOTFIX : lxElse is used to be similar with branching code
      // because of optimization rules
      scope.writer->newNode(lxElse, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->falseReference : scope.moduleScope->trueReference);
      compileBranching(loopNode, scope/*, cond, _operators.get(loopNode.Terminal()), HINT_LOOP*/);
      scope.writer->closeNode();

      scope.writer->closeNode();
   }
   // if it is repeat loop
   else {
      scope.writer->newNode(lxLooping, scope.moduleScope->trueReference);

      ObjectInfo retVal = compileExpression(node.firstChild(), scope, scope.moduleScope->boolType, 0);

      scope.writer->closeNode();
   }
}

//void Compiler :: compileTry(DNode node, CodeScope& scope)
//{
////   scope.writer->newNode(lxTrying);
////
////   // implement try expression
////   compileExpression(node.firstChild(), scope, 0, 0);
////
//////   // implement finally block
//////   _writer.pushObject(*scope.tape, ObjectInfo(okAccumulator));
//////   compileCode(goToSymbol(node.firstChild(), nsSubCode), scope);
//////   _writer.popObject(*scope.tape, ObjectInfo(okAccumulator));
////
////   DNode catchNode = goToSymbol(node.firstChild(), nsCatchMessageOperation);
////   if (catchNode != nsNone) {
////      scope.writer->newBookmark();
////
////      scope.writer->appendNode(lxResult);
////
////      // implement catch message
////      compileMessage(catchNode, scope, ObjectInfo(okObject));
////
////      scope.writer->removeBookmark();
////   }
//////   // or throw the exception further
//////   else _writer.throwCurrent(*scope.tape);
////
////   scope.writer->closeNode();
////
////   // implement finally block
////   compileCode(goToSymbol(node.firstChild(), nsSubCode), scope);
//}

void Compiler :: compileLock(DNode node, CodeScope& scope)
{
   scope.writer->newNode(lxLocking);

   // implement the expression to be locked
   ObjectInfo object = compileExpression(node.firstChild(), scope, 0, 0);

   scope.writer->newNode(lxBody);

   // implement critical section
   CodeScope subScope(&scope);
   subScope.level += 4; // HOT FIX : reserve place for the lock variable and exception info

   compileCode(goToSymbol(node.firstChild(), nsSubCode), subScope);

   // HOT FIX : clear the sub block local variables
   if (subScope.level - 4 > scope.level) {
      scope.writer->appendNode(lxReleasing, subScope.level - scope.level - 4);
   }

   scope.writer->closeNode();
   scope.writer->closeNode();
}

ObjectInfo Compiler :: compileCode(DNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   DNode statement = node.firstChild();

   //// make a root bookmark for temporal variable allocating
   //scope.rootBookmark = scope.writer->newBookmark();

   // skip subject argument
   while (statement == nsSubjectArg || statement == nsMethodParameter)
      statement= statement.nextNode();

   while (statement != nsNone) {
      DNode hints = skipHints(statement);

      //_writer.declareStatement(*scope.tape);

      switch(statement) {
         case nsExpression:
         case nsRootExpression:
            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            scope.writer->newNode(lxExpression);
            appendTerminalInfo(scope.writer, node.FirstTerminal());
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
//         case nsTry:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            compileTry(statement, scope);
//            break;
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
         case nsExtern:
            scope.writer->newNode(lxExternFrame);
            compileCode(statement, scope);
            scope.writer->closeNode();
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

  //scope.rootBookmark = -1;
  // scope.writer->removeBookmark();

   return retVal;
}

void Compiler :: compileExternalArguments(DNode arg, CodeScope& scope/*, ExternalScope& externalScope*/)
{
   ModuleScope* moduleScope = scope.moduleScope;

   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();

      ref_t subject = moduleScope->mapSubject(terminal);
      ref_t classReference = moduleScope->subjectHints.get(subject);
      int flags = 0;
      ClassInfo classInfo;
      if (moduleScope->loadClassInfo(classInfo, moduleScope->module->resolveReference(classReference), true) == 0)
         scope.raiseError(errInvalidOperation, terminal);

      flags = classInfo.header.flags;

      if (test(flags, elStructureRole)) {
         if (testany(flags, elDynamicRole | elEmbeddable | elWrapper)) {
            //HOTFIX : allow to pass structure
            if ((flags & elDebugMask) == 0) {
               flags = elDebugBytes;
            }
         }
         else flags = 0;
      }
      else if (test(flags, elWrapper)) {
         //HOTFIX : allow to pass a normal object
         flags = elDebugBytes;
      }
      else flags = 0;

      LexicalType argType = lxNone;
      switch (flags & elDebugMask) {
         // if it is an integer number pass it directly
         case elDebugDWORD:
         case elDebugPTR:
         case elDebugSubject:
         case elDebugMessage:
            argType = test(flags, elReadOnlyRole) ? lxIntExtArgument : lxExtArgument;
            break;
         case elDebugReference:
            argType = lxExtInteranlRef;
            break;
         case elDebugWideLiteral:
         case elDebugLiteral:
         case elDebugIntegers:
         case elDebugShorts:
         case elDebugBytes:
         case elDebugQWORD:
         case elDebugDPTR:
            argType = lxExtArgument;
            break;
         default:
            scope.raiseError(errInvalidOperation, terminal);
            break;
      }

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
         if (argType == lxExtInteranlRef) {
            if (isSingleObject(arg.firstChild())) {
               ObjectInfo target = compileTerminal(arg.firstChild(), scope);
               if (target.kind == okInternal) {
                  scope.writer->appendNode(lxExtInteranlRef, target.param);
               }
               else scope.raiseError(errInvalidOperation, terminal);
            }
            else scope.raiseError(errInvalidOperation, terminal);
         }
         else {
            scope.writer->newNode(argType);

            ObjectInfo info = compileExpression(arg.firstChild(), scope, subject, 0);
            if (info.kind == okIntConstant) {
               int value = StringHelper::strToULong(moduleScope->module->resolveConstant(info.param), 16);

               scope.writer->appendNode(lxValue, value);
            }

            scope.writer->closeNode();
         }

         arg = arg.nextNode();
      }
      else scope.raiseError(errInvalidOperation, terminal);
   }
}

ObjectInfo Compiler :: compileExternalCall(DNode node, CodeScope& scope, ident_t dllAlias, int mode)
{
   ObjectInfo retVal(okExternal);

   ModuleScope* moduleScope = scope.moduleScope;

   bool rootMode = test(mode, HINT_ROOT);
   bool stdCall = false;
   bool apiCall = false;

   ident_t dllName = dllAlias + strlen(EXTERNAL_MODULE) + 1;
   if (emptystr(dllName)) {
      // if run time dll is used
      dllName = RTDLL_FORWARD;
      if (StringHelper::compare(node.Terminal(), COREAPI_MASK, COREAPI_MASK_LEN))
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
      name.append(node.Terminal());
   }
   else {
      name.copy(NATIVE_MODULE);
      name.combine(CORE_MODULE);
      name.combine(node.Terminal());
   }

   ref_t reference = moduleScope->module->mapReference(name);

   // To tell apart coreapi calls, the name convention is used
   if (apiCall) {
      scope.writer->newNode(lxCoreAPICall, reference);
   }
   else scope.writer->newNode(stdCall ? lxStdExternalCall : lxExternalCall, reference);

   if (!rootMode)
      scope.writer->appendNode(lxTarget, -1);

   compileExternalArguments(node.firstChild(), scope);

   scope.writer->closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(DNode node, CodeScope& scope, ObjectInfo routine)
{
   ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   int paramCount;
   ref_t sign_ref, verb_id, dummy;
   ref_t message = mapMessage(node, scope, dummy);
   decodeMessage(message, sign_ref, verb_id, paramCount);

   virtualReference.append('0' + paramCount);
   virtualReference.append('#');
   virtualReference.append(0x20 + verb_id);

   if (sign_ref != 0) {
      virtualReference.append('&');
      virtualReference.append(moduleScope->module->resolveSubject(sign_ref));
   }

   scope.writer->newNode(lxInternalCall, moduleScope->module->mapReference(virtualReference));

   DNode arg = node.firstChild();

   while (arg == nsSubjectArg) {
      TerminalInfo terminal = arg.Terminal();
      ref_t type = moduleScope->mapSubject(terminal);

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

bool Compiler :: allocateStructure(CodeScope& scope, int size, int flags, bool bytearray, ObjectInfo& exprOperand)
{
   if (size <= 0)
      return false;

   int offset = allocateStructure(bytearray, size, scope.reserved);

   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // if it is not enough place to allocate
   if (methodScope->reserved < scope.reserved) {
      methodScope->reserved += size;
   }

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
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
   if (node != nsDefaultGeneric && node != nsImplicitConstructor) {
	   verb_id = _verbs.get(verb.value);

	   // if it is a generic verb, make sure no parameters are provided
	   if (verb_id == DISPATCH_MESSAGE_ID) {
		   scope.raiseError(errInvalidOperation, verb);
	   }
	   else if (verb_id == 0) {
         sign_id = scope.mapSubject(verb, signature);
         if (verb == tsPrivate)
            scope.privat = true;
	   }
   }

   DNode arg = node.firstChild();
   if (verb_id == 0) {
      // if followed by argument list - it is a EVAL verb
      if (node == nsImplicitConstructor) {
         verb_id = EVAL_MESSAGE_ID;
      }
      else if (arg == nsSubjectArg || arg == nsMethodParameter) {
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

      // if it is shorthand of eval &subj - recognize the subject
      if (verb_id == EVAL_MESSAGE_ID && sign_id != 0 && paramCount == 0 && arg.nextNode() != nsMessageParameter) {
         scope.parameters.add(arg.Terminal(), Parameter(index, sign_id));
      }
      else scope.parameters.add(arg.Terminal(), Parameter(index));
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

      ref_t subj_ref = scope.mapSubject(subject, signature);

      arg = arg.nextNode();

      if (arg == nsMethodParameter) {
         if (scope.parameters.exist(arg.Terminal()))
            scope.raiseError(errDuplicatedLocal, arg.Terminal());

         int index = 1 + scope.parameters.Count();

         // if it is an open argument type
         if (scope.moduleScope->subjectHints.exist(subj_ref, scope.moduleScope->paramsReference)) {
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
      ref_t hintRef = mapHint(hints, *scope.moduleScope);
      if (hintRef == scope.moduleScope->genericHint) {
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

void Compiler :: compileDispatcher(DNode node, SyntaxWriter& writer, MethodScope& scope, bool withGenericMethods)
{
   CodeScope codeScope(&scope, &writer);

   CommandTape* tape = scope.tape;

   // HOTFIX : insert the node to make sure method hints are inside the method node
   writer.insert(lxClassMethod, scope.message); 
   writer.appendNode(lxSourcePath);  // the source path is first string

   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, writer, node.Terminal(), scope.message);
   }
   else {
      writer.newNode(lxDispatching);

      // if it is generic handler with redirect statement / redirect statement
      if (node != nsNone) {
         if (withGenericMethods) {
            writer.appendNode(lxDispatching, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));
         }
         compileDispatchExpression(node, codeScope, tape);
      }
      // if it is generic handler only
      else if (withGenericMethods) {
         writer.newNode(lxResending);
         writer.appendNode(lxMessage, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));
         writer.newNode(lxTarget, scope.moduleScope->superReference);
         writer.appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
         writer.closeNode();
         writer.closeNode();
      }

      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileActionMethod(DNode node, SyntaxWriter& writer, MethodScope& scope)
{
   CodeScope codeScope(&scope, &writer);

   // new stack frame
   // stack already contains previous $self value
   writer.newNode(lxNewFrame);
   codeScope.level++;

   declareParameterDebugInfo(scope, writer, false, true);

   if (isReturnExpression(node.firstChild())) {
      compileRetExpression(node.firstChild(), codeScope, HINT_ROOT);
   }
   else if (node == nsInlineExpression) {
      // !! this check should be removed, as it is no longer used
      compileCode(node.firstChild(), codeScope);
   }
   else compileCode(node, codeScope);

   writer.closeNode();
   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
   writer.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileLazyExpressionMethod(DNode node, SyntaxWriter& writer, MethodScope& scope)
{
   CodeScope codeScope(&scope, &writer);

   // new stack frame
   // stack already contains previous $self value
   writer.newNode(lxNewFrame);
   codeScope.level++;

   declareParameterDebugInfo(scope, writer, false, false);

   compileRetExpression(node, codeScope, 0);

   writer.closeNode();
   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
   writer.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // try to implement light-weight resend operation
   ObjectInfo target;
   if (node.firstChild() == nsNone && node.nextNode() == nsNone) {
      target = scope.mapObject(node.Terminal());
   }

   if (target.kind == okConstantSymbol || target.kind == okField) {
      scope.writer->newNode(lxResending, methodScope->message);
      scope.writer->newNode(lxExpression);

      if (target.kind == okField) {
         scope.writer->appendNode(lxResultField, target.param);
      }
      else scope.writer->appendNode(lxConstantSymbol, target.param);

      scope.writer->closeNode();
      scope.writer->closeNode();
   }
   else {
      scope.writer->newNode(lxResending, methodScope->message);
      scope.writer->newNode(lxNewFrame);

      ObjectInfo target = compileExpression(node, scope, 0, 0);

      scope.writer->closeNode();
      scope.writer->closeNode();
   }

   scope.writer->appendNode(lxParamCount, getParamCount(methodScope->message) + 1);
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
         scope.writer->newNode(lxNewFrame);
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

void Compiler :: compileConstructorDispatchExpression(DNode node, SyntaxWriter& writer, CodeScope& scope)
{
   if (node.firstChild() == nsNone) {
      ObjectInfo info = scope.mapObject(node.Terminal());
      // if it is an internal routine
      if (info.kind == okInternal) {
         importCode(node, *scope.moduleScope, writer, node.Terminal(), scope.getMessageID());
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileResendExpression(DNode node, CodeScope& scope, CommandTape* tape)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   // new stack frame
   // stack already contains current $self reference
   scope.writer->newNode(lxNewFrame);
   scope.level++;

   scope.writer->newBookmark();
   writeTerminal(TerminalInfo(), scope, ObjectInfo(okThisParam, 1));

   compileMessage(node, scope, ObjectInfo(okThisParam, 1));
   scope.freeSpace();

   scope.writer->removeBookmark();

   scope.writer->closeNode();

   scope.writer->appendNode(lxParamCount, getParamCount(methodScope->message) + 1);
}

//void Compiler :: compileImportCode(DNode node, CodeScope& codeScope, ref_t message, ident_t function, CommandTape* tape)
//{
//   _writer.declareIdleMethod(*tape, message);
//   importCode(node, *codeScope.moduleScope, tape, function);
//   _writer.endIdleMethod(*tape);
//}

void Compiler :: compileMethod(DNode node, SyntaxWriter& writer, MethodScope& scope)
{
   int paramCount = getParamCount(scope.message);

   CodeScope codeScope(&scope, &writer);

   CommandTape* tape = scope.tape;

   // HOTFIX : insert the node to make sure method hints are inside the method node
   writer.insert(lxClassMethod, scope.message);
   writer.appendNode(lxSourcePath);  // the source path is first string

   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   // check if it is a resend
   if (resendBody != nsNone) {
      compileResendExpression(resendBody.firstChild(), codeScope, tape);
   }
   // check if it is a dispatch
   else if (dispatchBody != nsNone) {
      if (isImportRedirect(dispatchBody.firstChild())) {
         importCode(node, *scope.moduleScope, writer, dispatchBody.firstChild().Terminal(), scope.message);
      }
      else compileDispatchExpression(dispatchBody.firstChild(), codeScope, tape);
   }
   else {
      // new stack frame
      // stack already contains current $self reference
      // the original message should be restored if it is a generic method
      writer.newNode(lxNewFrame, scope.generic ? -1 : 0u);

      codeScope.level++;
      // declare the current subject for a generic method
      if (scope.generic) {
         codeScope.level++;
         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0);
      }

      declareParameterDebugInfo(scope, writer, true, test(codeScope.getClassFlags(), elRole));

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
               appendTerminalInfo(&writer, goToSymbol(body.firstChild(), nsCodeEnd).Terminal());
               writer.closeNode();
            }
            else writer.appendNode(lxLocal, 1);
         }
      }

      writer.closeNode();
      writer.appendNode(lxParamCount, paramCount + scope.rootToFree);
   }

   writer.appendNode(lxReserved, scope.reserved);

   writer.closeNode();
}
//
//void Compiler :: compileEmbeddableConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope)
//{
//   ref_t originalMethodRef = scope.message;
//   ref_t embedddedMethodRef = overwriteSubject(scope.message, classClassScope.info.methodHints.get(Attribute(scope.message, maEmbeddedInit)));
//
//   // compile an embedded constructor
//   // HOTFIX: embedded constructor is declared in class class but should be executed if the class scope
//   scope.tape = &classClassScope.tape;
//   scope.message = embedddedMethodRef;
//   writer.newBookmark();
//   compileMethod(node, writer, scope);
//   writer.removeBookmark();
//
//   // compile a constructor calling the embedded method
//   scope.message = originalMethodRef;
//   compileConstructor(DNode(), writer, scope, classClassScope, embedddedMethodRef);
//}

void Compiler :: compileConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef)
{
   CodeScope codeScope(&scope, &writer);

   // HOTFIX: constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;

   writer.insert(lxClassMethod, scope.message);
   writer.appendNode(lxSourcePath);  // the source path is first string

   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();

   DNode bodyNode = goToSymbol(node.firstChild(), nsRetStatement, nsSubCode);
   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);
   if (dispatchBody != nsNone) {
      compileConstructorDispatchExpression(dispatchBody.firstChild(), writer, codeScope);
      writer.closeNode();
      return;
   }
   else if (resendBody != nsNone) {
      compileConstructorResendExpression(resendBody.firstChild(), codeScope, classClassScope, withFrame);
   }
   else if (bodyNode == nsRetStatement) {
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (!test(classFlags, elDynamicRole)) {
      writer.appendNode(lxCalling, -1);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node.Terminal());

   if (bodyNode != nsNone) {
      if (!withFrame) {
         withFrame = true;

         // new stack frame
         // stack already contains $self value
         writer.newNode(lxNewFrame);
         codeScope.level++;
      }
      else {
         writer.newNode(lxAssigning);
         writer.appendNode(lxLocal, 1);
         writer.appendNode(lxResult);
         writer.closeNode();
      }

      declareParameterDebugInfo(scope, writer, true, false);

      if (bodyNode == nsRetStatement) {
         recordDebugStep(codeScope, bodyNode.firstChild().FirstTerminal(), dsStep);

         writer.newNode(lxReturning);
         writer.newBookmark();
         ObjectInfo retVal = compileRetExpression(bodyNode.firstChild(), codeScope, HINT_CONSTRUCTOR_EPXR);
         if (resolveObjectReference(codeScope, retVal) != codeScope.getClassRefId()) {
            if (test(classFlags, elWrapper)) {
               writer.insert(lxTypecasting, codeScope.getFieldType(0));
               writer.closeNode();

               writer.insert(lxBoxing);
               writer.appendNode(lxTarget, codeScope.getClassRefId());
               writer.closeNode();
            }
            else if (test(classFlags, elDynamicRole) && (retVal.param == -3 ||retVal.param == -5)) {
               writer.insert(lxBoxing);
               writer.appendNode(lxTarget, codeScope.getClassRefId());
               writer.closeNode();
            }
            // HOTFIX : support numeric value for numeric classes
            else if (test(classFlags, elStructureRole | elDebugDWORD) && retVal.kind == okIntConstant) {
               writer.insert(lxBoxing);
               writer.appendNode(lxTarget, codeScope.getClassRefId());
               writer.closeNode();
            }
            else scope.raiseError(errIllegalConstructor, node.FirstTerminal());
         }
         writer.removeBookmark();
         writer.closeNode();
      }
      else {
         compileCode(bodyNode, codeScope);

         codeScope.writer->appendNode(lxLocal, 1);
      }
   }
   //// if the constructor has a body
   ///*else */if (body != nsNone) {
   //// if the constructor should call embeddable method
   //else if (embeddedMethodRef != 0) {
   //   writer.newNode(lxResending, embeddedMethodRef);
   //   writer.appendNode(lxTarget, classClassScope.reference);
   //   writer.newNode(lxAssigning);
   //   writer.appendNode(lxCurrent, 1);
   //   writer.appendNode(lxResult);
   //   writer.closeNode();
   //   writer.closeNode();
   //}

   if (withFrame)
      writer.closeNode();

   writer.appendNode(lxParamCount, getParamCount(scope.message) + 1);
   writer.appendNode(lxReserved, scope.reserved);

   writer.closeNode();
}

void Compiler :: compileDefaultConstructor(MethodScope& scope, SyntaxWriter& writer, ClassScope& classClassScope)
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

   writer.newNode(lxClassMethod, scope.message);
   writer.appendNode(lxSourcePath);  // the source path is first string

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

void Compiler :: compileDynamicDefaultConstructor(MethodScope& scope, SyntaxWriter& writer, ClassScope& classClassScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // HOTFIX: constructor is declared in class class but should be executed if the class scope
   scope.tape = &classClassScope.tape;

   writer.newNode(lxClassMethod, scope.message);
   writer.appendNode(lxSourcePath);  // the source path is first string

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

void Compiler :: compileVMT(DNode member, SyntaxWriter& writer, ClassScope& scope, bool warningsOnly)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      writer.newBookmark();

      switch(member) {
         case nsMethod:
         {
            MethodScope methodScope(&scope);

            // if it is a dispatch handler
            if (member.firstChild() == nsDispatchHandler) {
               if (test(scope.info.header.flags, elRole))
                  scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
               methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);

               compileMethodHints(hints, writer, methodScope, warningsOnly);

               compileDispatcher(member.firstChild().firstChild(), writer, methodScope, test(scope.info.header.flags, elWithGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(member, methodScope, hints);

               int hint = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
               methodScope.stackSafe = test(hint, tpStackSafe);
               methodScope.generic = test(hint, tpGeneric);
               if ((hint & tpMask) == tpPrivate) {
                  //HOTFIX : overwrite the message verb for the private method
                  methodScope.message = overwriteVerb(methodScope.message, PRIVATE_MESSAGE_ID);
               }

               compileMethodHints(hints, writer, methodScope, warningsOnly);

               compileMethod(member, writer, methodScope);
            }
            break;
         }
         case nsDefaultGeneric:
         {
            MethodScope methodScope(&scope);
            declareArgumentList(member, methodScope, hints);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));

            // mark as having generic methods
            scope.info.header.flags |= elWithGenerics;
            methodScope.generic = true;

            compileMethod(member, writer, methodScope);
            break;
         }
         case nsImplicitConstructor:
         {
            MethodScope methodScope(&scope);
            declareArgumentList(member, methodScope, hints);

            // override message with private verb
            methodScope.message = overwriteVerb(methodScope.message, PRIVATE_MESSAGE_ID);

            int hint = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
            methodScope.stackSafe = test(hint, tpStackSafe);

            compileMethod(member, writer, methodScope);

            break;
         }
      }
      writer.removeBookmark();

      member = member.nextNode();
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (test(scope.info.header.flags, elWithGenerics) && scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), false)) {
      MethodScope methodScope(&scope);
      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);

      writer.newBookmark();
      compileDispatcher(DNode(), writer, methodScope, true);
      writer.removeBookmark();
   }
}

void Compiler :: compileFieldDeclarations(DNode& member, SyntaxWriter& writer, ClassScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member==nsField) {
         writer.newNode(lxClassField);

         appendTerminalInfo(&writer, member.Terminal());
         compileFieldHints(hints, writer, scope);

         writer.closeNode();
      }
      else {
         // due to current syntax we need to reset hints back, otherwise they will be skipped
         if (hints != nsNone)
            member = hints;

         break;
      }
      member = member.nextNode();
   }
}

void Compiler :: compileSymbolCode(ClassScope& scope)
{
   CommandTape tape;

   // creates implicit symbol
   SymbolScope symbolScope(scope.moduleScope, scope.reference);

   _writer.generateSymbol(tape, symbolScope.reference, lxConstantClass, scope.reference);

   // create byte code sections
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.syntaxTree.Strings(), scope.moduleScope->sourcePathRef);
}

void Compiler :: compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   SyntaxWriter writer(classClassScope.syntaxTree);
   writer.newNode(lxRoot, classClassScope.reference);

   // if no construtors are defined inherits the default one
   if (!findSymbol(node.firstChild(), nsConstructor)) {
      if (classScope.info.header.parentRef == 0)
         classScope.raiseError(errInvalidParent, node.FirstTerminal());

      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.moduleScope->superReference));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }
   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef);
   
   // class class is always stateless
   writer.appendNode(lxClassFlag, elStateless);

   DNode member = node.firstChild();
   declareVMT(member, writer, classClassScope, true);
   
   // add virtual constructor
   writer.appendNode(lxClassMethod, encodeVerb(NEWOBJECT_MESSAGE_ID));
   writer.appendNode(lxSourcePath);  // the source path is first string

   writer.closeNode();

   generateClassDeclaration(classClassScope, false);

   // save declaration
   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(DNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   ModuleScope* moduleScope = classClassScope.moduleScope;

   SyntaxWriter writer(classClassScope.syntaxTree);
   writer.newNode(lxRoot, classClassScope.reference);

   DNode member = node.firstChild();
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member == nsConstructor) {
         MethodScope methodScope(&classScope);

         declareArgumentList(member, methodScope, hints);

         int hint = classClassScope.info.methodHints.get(Attribute(methodScope.message, maHint));
         methodScope.stackSafe = test(hint, tpStackSafe);

         writer.newBookmark();

         compileMethodHints(hints, writer, methodScope, true);

         compileConstructor(member, writer, methodScope, classClassScope);

         writer.removeBookmark();
      }
      member = member.nextNode();
   }

   // create a virtual constructor
   MethodScope methodScope(&classScope);
   methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

   if (test(classScope.info.header.flags, elDynamicRole)) {
      compileDynamicDefaultConstructor(methodScope, writer, classClassScope);
   }
   else compileDefaultConstructor(methodScope, writer, classClassScope);

   writer.closeNode();

   generateClassImplementation(classClassScope);
}

inline bool isClassMethod(Symbol symbol)
{
   switch (symbol)
   {
      case nsMethod:
      case nsDefaultGeneric:
      case nsImplicitConstructor:
         return true;
      default:
         return false;
   }
}

void Compiler :: declareVMT(DNode member, SyntaxWriter& writer, ClassScope& scope, bool classClassMode)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if ((classClassMode && member == nsConstructor) || (!classClassMode && isClassMethod(member))) {
         MethodScope methodScope(&scope);

         DNode firstChild = member.firstChild();
         if (firstChild == nsDispatchHandler) {
            methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
         }
         else if (member == nsDefaultGeneric) {
            declareArgumentList(member, methodScope, hints);

            // override subject with generic postfix
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));

            // mark as having generic methods
            writer.appendNode(lxClassFlag, elWithGenerics);
         }
         else if (member == nsImplicitConstructor) {
            declareArgumentList(member, methodScope, hints);

            methodScope.message = overwriteVerb(methodScope.message, PRIVATE_MESSAGE_ID);
         }
         else declareArgumentList(member, methodScope, hints);

         writer.newNode(lxClassMethod, methodScope.message);
         appendTerminalInfo(&writer, member.Terminal());

         compileMethodHints(hints, writer, methodScope, false);
         
         writer.closeNode();

         if (methodScope.generic)
            writer.appendNode(lxClassFlag, elWithGenerics);

         // save extensions if required ; private method should be ignored
         if (scope.extensionMode != 0 && !methodScope.privat) {
            if (scope.extensionMode != -1)
               scope.info.fieldTypes.add(-1, scope.extensionMode);

            scope.moduleScope->saveExtension(methodScope.message, scope.extensionMode, scope.reference);
         }
      }
      member = member.nextNode();
   }
}

ref_t Compiler :: generateTemplate(ModuleScope& moduleScope, TemplateInfo& templateInfo, ref_t reference)
{
   int initialParamCount = templateInfo.parameters.Count();

   if (!reference) {
      reference = moduleScope.mapNestedExpression();
   }

   ClassInfo info;
   if (moduleScope.loadClassInfo(info, reference, true)) {
      return reference;
   }

   ClassScope scope(&moduleScope, reference);

   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   if (templateInfo.templateParent != 0) {
      compileParentDeclaration(DNode(), scope, templateInfo.templateParent);
   }
   else compileParentDeclaration(DNode(), scope);

   writer.appendNode(lxClassFlag, elSealed);

   declareTemplate(scope, writer, templateInfo);   
   declareImportedTemplates(scope, writer);           // HOTFIX : import templates declared in templates

   writer.closeNode();

   generateClassDeclaration(scope, false);
   scope.save();

   // HOTFIX : generate syntax once again to properly import the template code
   writer.clear();
   writer.newNode(lxRoot, scope.reference);

   // HOT FIX : declare external parameters once again, 
   // intitial parameters must be preserved
   while (templateInfo.parameters.Count() > initialParamCount)
      templateInfo.parameters.erase(templateInfo.parameters.end());

   importTemplate(scope, writer, templateInfo);
   importTemplates(scope, writer);                 // HOTFIX : import templates declared in templates
   compileVirtualMethods(writer, scope);

   writer.closeNode();

   generateClassImplementation(scope);

   return scope.reference;
}

void Compiler :: generateClassFlags(ClassScope& scope, SNode root)
{
   SNode current = root.firstChild();
   while (current != lxNone) {
      scope.compileClassHint(current);

      current = current.nextNode();
   }
}

ref_t Compiler :: declareInlineTemplate(ModuleScope& scope, SNode node, TemplateInfo& templateInfo, ref_t inlineTemplateRef)
{
   TemplateInfo fieldTemplate;
   fieldTemplate.templateRef = inlineTemplateRef;

   ReferenceNs fulName(scope.module->Name(), scope.module->resolveSubject(fieldTemplate.templateRef));
   SNode param = node.firstChild();
   while (param != lxNone) {
      if (param == lxTemplateParam) {
         ref_t optionRef = templateInfo.parameters.get(param.argument);

         fieldTemplate.parameters.add(fieldTemplate.parameters.Count() + 1, optionRef);

         fulName.append('@');
         fulName.append(scope.module->resolveSubject(optionRef));
      }
      param = param.nextNode();
   }
   ref_t classRef = scope.module->mapReference(fulName);
   ref_t fieldType = scope.typifiedClasses.get(classRef);
   if (!fieldType) {
      fieldType = scope.mapNestedTemplate();
      scope.saveSubject(fieldType, classRef, true);
   }         

   generateTemplate(scope, fieldTemplate, classRef);

   return fieldType;
}

bool Compiler :: declareTemplate(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo)
{
   ModuleScope* moduleScope = scope.moduleScope;

   _Module* extModule = NULL;
   _Memory* section = moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule);
   if (!section)
      return false;

   SyntaxTree tree(section);
   SNode current = tree.readRoot();
   current = current.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         writer.appendNode(lxClassFlag, current.argument);
      }
      else if (current == lxTemplateField) {
         writer.newNode(lxTemplateField, current.argument);

         SNode attr = current.firstChild();
         while (attr != lxNone) {
            if (attr == lxTemplateFieldType) {               
               writer.appendNode(lxType, templateInfo.parameters.get(attr.argument));
            }
            else if (attr == lxTemplate) {
               writer.appendNode(lxType, declareInlineTemplate(*scope.moduleScope, attr, templateInfo, 
                  importSubject(extModule, attr.argument, moduleScope->module)));
            }
            else importNode(scope, attr, writer, extModule, templateInfo);

            attr = attr.nextNode();
         }
         writer.closeNode();
      }
      else if (current == lxTemplateSubject) {
         templateInfo.parameters.add(templateInfo.parameters.Count() + 1, templateInfo.messageSubject);
      }
      else if (current == lxClassMethod) {
         bool withGenericAttr = false;

         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(extModule, moduleScope->module, getSignature(current.argument), templateInfo));
         
         writer.newNode(lxTemplateMethod, messageRef);
         SNode attr = current.firstChild();
         while (attr != lxNone) {
            if (attr == lxClassMethodAttr) {
               writer.appendNode(lxClassMethodAttr, attr.argument);
               if (attr.argument == tpGeneric)
                  withGenericAttr = true;
            }
            else if (attr == lxType) {
               writer.appendNode(lxType, importTemplateSubject(extModule, moduleScope->module, attr.argument, templateInfo));
            }

            attr = attr.nextNode();
         }
         writer.closeNode();

         //HOTFIX : recognize generic handler
         if (withGenericAttr)
            writer.appendNode(lxClassFlag, elWithGenerics);
      }
      else if (current == lxTemplate) {
         importTemplateInfo(current, *moduleScope, scope.reference, extModule, templateInfo);
      }
   
      current = current.nextNode();
   }

   return true;
}

void Compiler :: generateClassFields(ClassScope& scope, SNode root)
{
   bool singleField = SyntaxTree::countChild(root, lxClassField, lxTemplateField) == 1;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassField || current == lxTemplateField) {
         int offset = 0;
         ident_t terminal = SyntaxTree::findChild(current, lxTerminal).identifier();

         // a role cannot have fields
         if (test(scope.info.header.flags, elStateless))
            scope.raiseError(errIllegalField, current);

         ref_t typeHint = SyntaxTree::findChild(current, lxType).argument;
         int sizeHint = SyntaxTree::findChild(current, lxSize).argument;
         ref_t target = SyntaxTree::findChild(current, lxTarget).argument;

         int size = (typeHint != 0) ? scope.moduleScope->defineSubjectSize(typeHint) : 0;
         if (sizeHint != 0) {
            if (size < 0) {
               size = sizeHint * (-size);
            }
            else if (size == 0) {
               size = sizeHint;
            }
            else scope.raiseError(errIllegalField, current);
         }

         if (test(scope.info.header.flags, elWrapper) && scope.info.fields.Count() > 0) {
            // wrapper may have only one field
            scope.raiseError(errIllegalField, current);
         }
         else if (isPrimitiveRef(target)) {
            if (testany(scope.info.header.flags, elNonStructureRole | elDynamicRole))
               scope.raiseError(errIllegalField, current);

            if (test(scope.info.header.flags, elStructureRole)) {
               scope.info.fields.add(terminal, scope.info.size);
               scope.info.size += size;
            }
            else scope.raiseError(errIllegalField, current);

            // if it is a primitive field
            if (singleField && scope.info.fields.Count() == 1) {
               switch (target) {
                  case -1:
                     scope.info.header.flags |= (elDebugDWORD | elReadOnlyRole);
                     break;
                  case -2:
                     scope.info.header.flags |= (elDebugQWORD | elReadOnlyRole) ;
                     break;
                  case -4:
                     scope.info.header.flags |= (elDebugReal64 | elReadOnlyRole);
                     break;
                  case -7:
                     scope.info.header.flags |= (elDebugReference | elReadOnlyRole | elSymbol);
                     break;
                  case -8:
                     scope.info.header.flags |= (elDebugSubject | elReadOnlyRole | elSignature);
                     break;
                  case -9:
                     scope.info.header.flags |= (elDebugMessage | elReadOnlyRole | elMessage);
                     break;
                  case -10:
                     scope.info.header.flags |= (elDebugMessage | elReadOnlyRole | elExtMessage);
                     break;
                  default:
                     scope.raiseError(errIllegalField, current);
                     break;
               }
            }
         }
         // a class with a dynamic length structure must have no fields
         else if (test(scope.info.header.flags, elDynamicRole)) {
            if (scope.info.size == 0 && scope.info.fields.Count() == 0) {
               // compiler magic : turn a field declaration into an array or string one 
               if (size != 0) {
                  if ((scope.info.header.flags & elDebugMask) == elDebugLiteral) {
                     scope.info.header.flags &= ~elDebugMask;
                     if (size == 2) {                        
                        scope.info.header.flags |= elDebugWideLiteral;
                     }
                     else if (size == 1) {
                        scope.info.header.flags |= elDebugLiteral;
                     }
                  }
                  scope.info.header.flags |= elStructureRole;
                  scope.info.size = -size;
               }

               scope.info.fieldTypes.add(-1, typeHint);
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

               if (size < 0) {
                  scope.info.header.flags |= elDynamicRole;
               }

               scope.info.fields.add(terminal, 0);
               scope.info.fieldTypes.add(0, typeHint);
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
               scope.info.fieldTypes.add(offset, typeHint);
            }
            // if it is a normal field
            else {
               scope.info.header.flags |= elNonStructureRole;

               offset = scope.info.fields.Count();
               scope.info.fields.add(terminal, offset);

               if (typeHint != 0)
                  scope.info.fieldTypes.add(offset, typeHint);

               //// byref variable may have only one field
               //if (test(scope.info.header.flags, elWrapper)) {
               //   if (scope.info.fields.Count() > 1)
               //      scope.raiseError(errIllegalField, current);
               //}
            }
         }

         // handle field template
         SNode templateNode = SyntaxTree::findChild(current, lxTemplate);
         if (templateNode.argument != 0) {
            declareFieldTemplateInfo(templateNode, scope, templateNode.argument, offset);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: generateMethodHints(ClassScope& scope, SNode node, ref_t message)
{
   ref_t outputType = 0;
   bool hintChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethodAttr) {
         hint |= current.argument;

         hintChanged = true;
      }
      else if (current == lxType) {
         outputType = current.argument;
      }
      else if (current == lxActionAttr) {
         scope.moduleScope->saveAction(message, scope.reference);
      }
//      else if (current == lxClassMethodOpt) {
//         SNode mssgAttr = SyntaxTree::findChild(current, lxMessage);
//         if (mssgAttr != lxNone) {
//            scope.info.methodHints.add(Attribute(message, current.argument), getSignature(mssgAttr.argument));
//         }         
//      }
//      else if (current == lxMethodTemplate) {
//         declareImportedTemplate(scope, current, 0, getSignature(message), 0);
//      }
      current = current.nextNode();
   }

   if (outputType != 0) {
      scope.info.methodHints.exclude(Attribute(message, maType));
      scope.info.methodHints.add(Attribute(message, maType), outputType);
   }

   if (hintChanged) {
      scope.info.methodHints.exclude(Attribute(message, maHint));
      scope.info.methodHints.add(Attribute(message, maHint), hint);
   }
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SNode root, bool closed)
{
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateMethod) {
         if (!scope.info.methods.exist(current.argument, true)) {
            generateMethodHints(scope, current, current.argument);

            if (!scope.info.methods.exist(current.argument))
               scope.info.methods.add(current.argument, false);
         }
      }
      else if (current == lxClassMethod) {
         generateMethodHints(scope, current, current.argument);

         int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(current.argument, maHint));

         //HOTFIX : overwrite the private message verb
         int message = current.argument;
         if ((methodHints & tpMask) == tpPrivate) {
            message = overwriteVerb(message, PRIVATE_MESSAGE_ID);
         }

         // check if there is no duplicate method
         if (scope.info.methods.exist(message, true))
            scope.raiseError(errDuplicatedMethod, current);

         bool included = scope.include(message);
         bool sealedMethod = (methodHints & tpMask) == tpSealed;
         // if the class is closed, no new methods can be declared
         if (included && closed)
            scope.raiseError(errClosedParent, current);

         // if the method is sealed, it cannot be overridden
         if (!included && sealedMethod)
            scope.raiseError(errClosedMethod, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: generateClassDeclaration(ClassScope& scope, bool closed)
{
   SNode root = scope.syntaxTree.readRoot();

   // generate flags
   generateClassFlags(scope, root);

   // generate fields
   generateClassFields(scope, root);

   // define the data type for the array
   if (test(scope.info.header.flags, elDynamicRole) && (scope.info.header.flags & elDebugMask) == 0) {
      if (test(scope.info.header.flags, elStructureRole)) {
         ref_t fieldClassRef = scope.moduleScope->subjectHints.get(scope.info.fieldTypes.get(-1));

         int fieldFlags = scope.moduleScope->getClassFlags(fieldClassRef) & elDebugMask;
         if (fieldFlags == elDebugDWORD) {
            switch (scope.info.size)
            {
               case -4:
                  scope.info.header.flags |= elDebugIntegers;
                  break;
               case -2:
                  scope.info.header.flags |= elDebugShorts;
                  break;
               case -1:
                  scope.info.header.flags |= elDebugBytes;
                  break;
            }
         }
         else if ((scope.info.size == -4) && (fieldFlags == elDebugMessage || fieldFlags == elDebugSubject)) {
            scope.info.header.flags |= elDebugIntegers;
         }
      }
      else scope.info.header.flags |= elDebugArray;
   }

   //HOTFIX : recognize pointer structure
   if (test(scope.info.header.flags, elStructureRole | elPointer)
      && ((scope.info.header.flags & elDebugMask) == 0) && scope.info.fields.Count() == 1)
   {
      switch (scope.moduleScope->getTypeFlags(scope.info.fieldTypes.get(0)) & elDebugMask)
      {
         case elDebugDWORD:
            scope.info.header.flags |= elDebugPTR;
            break;
         case elDebugQWORD:
            scope.info.header.flags |= elDebugDPTR;
            break;
         default:
            scope.info.header.flags &= ~elPointer;
            break;
      }
   }
   // generate methods
   generateMethodDeclarations(scope, root, closed);

   // verify if the class may be a wrapper
   if (isWrappable(scope.info.header.flags) && scope.info.fields.Count() == 1 &&
      test(scope.info.methodHints.get(Attribute(encodeVerb(DISPATCH_MESSAGE_ID), maHint)), tpEmbeddable))
   {
      if (test(scope.info.header.flags, elStructureRole)) {
         ref_t fieldClassRef = scope.moduleScope->subjectHints.get(*scope.info.fieldTypes.start());
         int fieldFlags = scope.moduleScope->getClassFlags(fieldClassRef);

         if (isEmbeddable(fieldFlags)) {
            // wrapper around embeddable object should be marked as embeddable wrapper
            scope.info.header.flags |= elEmbeddableWrapper;

            if ((scope.info.header.flags & elDebugMask) == 0)
               scope.info.header.flags |= fieldFlags & elDebugMask;
         }
      }
      else scope.info.header.flags |= elWrapper;
   }

   // declare virtual methods
   if (!closed)
      declareVirtualMethods(scope);
}

void Compiler :: generateInlineClassDeclaration(ClassScope& scope, bool closed)
{
   generateClassDeclaration(scope, closed);

   // stateless inline class
   if (scope.info.fields.Count() == 0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;

      // stateless inline class is its own class class
      scope.info.classClassRef = scope.reference;
   }
   else scope.info.header.flags &= ~elStateless;
}

//bool Compiler :: validateMethodTemplate(SyntaxTree::Node node, ref_t& targetMethod)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxTemplateCalling) {
//         if (targetMethod == 0) {
//            targetMethod = current.argument;
//         }
//         else if (targetMethod != current.argument)
//            return false;
//      }
//
//      if (!validateMethodTemplate(current, targetMethod))
//         return false;
//
//      current = current.nextNode();
//   }
//   return true;
//}

void Compiler :: compileTemplateFieldDeclaration(DNode& member, SyntaxWriter& writer, TemplateScope& scope)
{
   while (member != nsNone) {
      DNode fieldHints = skipHints(member);

      if (member == nsField) {
         ref_t param = 0;
         TerminalInfo terminal = member.Terminal();

         scope.info.fields.add(terminal, scope.info.fields.Count());

         writer.newNode(lxTemplateField);
         writer.appendNode(lxTerminal, terminal);

         if (fieldHints != nsNone) {
            while (fieldHints == nsHint) {
               param = scope.parameters.get(fieldHints.Terminal());
               if (param > 0) {
                  if (fieldHints.firstChild() != nsNone)
                     scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, fieldHints.Terminal());

                  writer.appendNode(lxTemplateFieldType, param);
               }
               else {
                  ref_t hintRef = mapHint(fieldHints, *scope.moduleScope);
                  if (hintRef != 0) {
                     if (scope.moduleScope->subjectHints.exist(hintRef)) {
                        writer.appendNode(lxType, hintRef);
                     }
                     else {
                        writer.newNode(lxTemplate, hintRef);
                        DNode option = fieldHints.firstChild();
                        while (option == nsHintValue) {
                           int param = scope.parameters.get(option.Terminal());
                           if (param != 0) {
                              writer.appendNode(lxTemplateParam, param);
                           }
                           else scope.raiseError(wrnInvalidHint, fieldHints.Terminal());

                           option = option.nextNode();
                        }
                        writer.closeNode();
                     }                     
                  }
                  else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, fieldHints.Terminal());
               }

               fieldHints = fieldHints.nextNode();
            }
         }
         writer.closeNode();
      }
      else {
         // due to current syntax we need to reset hints back, otherwise they will be skipped
         if (fieldHints != nsNone)
            member = fieldHints;

         break;
      }
      member = member.nextNode();
   }
}

void Compiler :: compileTemplateDeclaration(DNode node, TemplateScope& scope, DNode hints)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   DNode member = node.firstChild();

   // load template parameters
   while (member == nsMethodParameter) {
      if (!scope.parameters.exist(member.Terminal())) {
         scope.parameters.add(member.Terminal(), scope.parameters.Count() + 1);
      }
      else scope.raiseError(errDuplicatedDefinition, member.Terminal());

      member = member.nextNode();
   }

   compileTemplateHints(hints, writer, scope);

   if (scope.type == TemplateScope::ttMethod) {
      scope.parameters.add(TARGET_PSEUDO_VAR, scope.parameters.Count() + 1);

      writer.appendNode(lxTemplateSubject, scope.parameters.Count());
   }
   else if (scope.type == TemplateScope::ttField) {
      scope.info.fields.add(TARGET_PSEUDO_VAR, 0);
   }

   // load template fields
   compileTemplateFieldDeclaration(member, writer, scope);

   compileVMT(member, writer, scope, false);

   // declare template in template
   SNode templateNode = scope.moduleScope->templates.readRoot().firstChild();
   while (templateNode != lxNone) {
      if (templateNode.type == lxClass && templateNode.argument == scope.reference) {
         copyTemplateInfo(templateNode, writer);
      }

      templateNode = templateNode.nextNode();
   }

   writer.closeNode();

   // save declaration
   scope.save();

   scope.moduleScope->saveTemplate(scope.templateRef);
}

bool Compiler :: declareImportedTemplates(ClassScope& scope, SyntaxWriter& writer)
{
   SNode templateNode = scope.moduleScope->templates.readRoot().firstChild();
   while (templateNode != lxNone) {
      if (templateNode.type == lxClass && templateNode.argument == scope.reference) {
         TemplateInfo info;

         readTemplateInfo(templateNode, info);
         if (!declareTemplate(scope, writer, info))
            return false;
      }

      templateNode = templateNode.nextNode();
   }

   return true;
}

void Compiler :: compileClassDeclaration(DNode node, ClassScope& scope, DNode hints)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   DNode member = node.firstChild();
   if (member==nsBaseClass) {
      compileParentDeclaration(member, scope);

      member = member.nextNode();
   }
   else compileParentDeclaration(DNode(), scope);

   int flagCopy = scope.info.header.flags;

   compileClassHints(hints, writer, scope);
   compileFieldDeclarations(member, writer, scope);
   declareVMT(member, writer, scope, false);

   // declare imported methods / flags
   if (!declareImportedTemplates(scope, writer))
      scope.raiseError(errInvalidHint, node.FirstTerminal());

   writer.closeNode();

   generateClassDeclaration(scope, test(flagCopy, elClosed));

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

void Compiler :: generateClassImplementation(ClassScope& scope)
{
   optimizeClassTree(scope);

   _writer.generateClass(scope.tape, scope.syntaxTree);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save();
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.syntaxTree.Strings(), scope.moduleScope->sourcePathRef);
}

void Compiler :: importNode(ClassScope& scope, SyntaxTree::Node current, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info)
{
   if (current.type == lxTemplateTarget) {
      int offset = 0;
      ref_t type = 0;
      if (info.targetOffset >= 0) {
         type = info.targetType;
         offset = info.targetOffset;
      }
      else {
         ident_t field = SyntaxTree::findChild(current, lxTerminal).identifier();

         offset = scope.info.fields.get(field);
         type = scope.info.fieldTypes.get(offset);         
      }

      // if it is an array
      if (test(scope.info.header.flags, elDynamicRole)) {
         writer.newNode(lxThisLocal, 1);

         writer.appendNode(lxTarget, test(scope.info.header.flags, elStructureRole) ? -3 : -5);
      }
      // if it is a structure field
      else if (test(scope.info.header.flags, elStructureRole)) {
         writer.newNode(lxBoxing);
         writer.appendNode(lxFieldAddress, offset);
      }
      else writer.newNode(lxField, offset);

      if (type != 0)
         writer.appendNode(lxType, type);
   }
   else if (current == lxNestedTemplate) {
      writer.newNode(lxNested, current.argument);

      TemplateInfo nestedInfo;
      nestedInfo.templateRef = importSubject(templateModule, 
         SyntaxTree::findChild(current, lxTemplate).argument, scope.moduleScope->module);
      nestedInfo.templateParent = importReference(templateModule,
         SyntaxTree::findChild(current, lxNestedTemplateParent).argument, scope.moduleScope->module);

      nestedInfo.ownerRef = scope.reference;
      ref_t classRef = generateTemplate(*scope.moduleScope, nestedInfo, 0);
      writer.appendNode(lxTarget, classRef);
   }
   else if (current == lxNewOp) {
      //HOTFIX : recognize string of structures
      writer.newNode(current.type, current.argument);

      int flags = 0;
      ref_t target = 0;
      SNode child = current.firstChild();
      while (child != lxNone) {
         if (child == lxType) {
            ref_t type = importTemplateSubject(templateModule, scope.moduleScope->module, child.argument, info);
            flags = scope.moduleScope->getClassFlags(scope.moduleScope->subjectHints.get(type));
            writer.appendNode(lxType, type);
         }
         else if (child == lxTarget) {
            target = child.argument;
         }
         else importNode(scope, child, writer, templateModule, info);

         child = child.nextNode();
      }

      if (target == -5 && flags == elDebugDWORD) {
         writer.appendNode(lxTarget, -3);
      }
      else writer.appendNode(lxTarget, target);

      writer.closeNode();
      return;
   }
   else if (current == lxNestedTemplateOwner) {
      writer.newNode(lxTarget, info.ownerRef);
   }
   else if (current == lxTemplateType) {
      writer.newNode(lxType, declareInlineTemplate(*scope.moduleScope, current, info, 
         importSubject(templateModule, current.argument, scope.moduleScope->module)));
   }
   else if (current == lxTemplate || current == lxNestedTemplateParent) {
      // ignore template node, it should be already handled
      return;
   }
   else if (current == lxTerminal) {
      writer.newNode(lxTerminal, current.identifier());
   }
   else if (current == lxSourcePath) {
      writer.newNode(lxSourcePath, current.identifier());
   }
   else if (current == lxMessageVariable) {
      // message variable should be already set
      return;
   }
   else if (current == lxThisLocal) {
      writer.newNode(current.type, current.argument);
      writer.appendNode(lxTarget, scope.reference);
   }
   else if (current == lxCol && current.parentNode() != lxBreakpoint) {
      writer.newNode(lxCol, info.sourceCol);
   }
   else if (current == lxRow && current.parentNode() != lxBreakpoint) {
      writer.newNode(lxRow, info.sourceRow);
   }
   else if (test(current.type, lxMessageMask)) {
      ref_t signature = importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info);

      writer.newNode(current.type, overwriteSubject(current.argument, signature));

      // HOTFIX : insert calling target if required
      if (current.type == lxCalling) {
         SNode callee = SyntaxTree::findMatchedChild(current, lxObjectMask);
         if (callee == lxThisLocal) {
            writer.appendNode(lxCallTarget, scope.reference);
         }
         else if (callee == lxField || callee == lxTemplateTarget) {
            SNode attr = SyntaxTree::findChild(callee, lxNestedTemplateOwner, lxType, lxTemplateFieldType);
            if (attr == lxNestedTemplateOwner) {
               writer.appendNode(lxCallTarget, info.ownerRef);
            }
            else if (attr == lxType) {
               ref_t classRef = scope.moduleScope->subjectHints.get(attr.argument);
               if (classRef)
                  writer.appendNode(lxCallTarget, classRef);
            }
         }

         // HOTFIX : if it is typecast message, provide the type
         if (getVerb(current.argument) == GET_MESSAGE_ID && getParamCount(current.argument) == 0 && scope.moduleScope->subjectHints.exist(signature)) {
            writer.appendNode(lxType, signature);
         }
      }
   }
   else if (test(current.type, lxReferenceMask)) {
      if (isPrimitiveRef(current.argument)) {
         writer.newNode(current.type, current.argument);
      }
      else writer.newNode(current.type, importReference(templateModule, current.argument, scope.moduleScope->module));
   }
   else if (test(current.type, lxSubjectMask)) {
      writer.newNode(current.type, importTemplateSubject(templateModule, scope.moduleScope->module, current.argument, info));
   }
   else if (test(current.type, lxConstantMask)) {
      writer.newNode(current.type, importConstant(templateModule, current.argument, scope.moduleScope->module));
   }
   else writer.newNode(current.type, current.argument);

   importTree(scope, current, writer, templateModule, info);

   writer.closeNode();
}

void Compiler :: importTree(ClassScope& scope, SyntaxTree::Node node, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      importNode(scope, current, writer, templateModule, info);

      current = current.nextNode();
   }
}

void Compiler :: importTemplateTree(ClassScope& scope, SyntaxWriter& writer, SNode node, TemplateInfo& info, _Module* templateModule)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateSubject) {
         info.parameters.add(info.parameters.Count() + 1, info.messageSubject);
      }
      else if (current == lxTemplate) {
         // templates should be already imported
      }
      else if (current == lxClassMethod) {
         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info));

         // method should not be imported if it was already declared in the class scope
         if (!scope.info.methods.exist(messageRef, true)) {
            writer.newNode(lxClassMethod, messageRef);

            // NOTE : source path reference should be imported
            // but the message name should be overwritten
            writeMessage(*scope.moduleScope, writer, messageRef);

            importTree(scope, current, writer, templateModule, info);

            writer.closeNode();

            scope.include(messageRef);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: importTemplate(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo)
{
   _Module* extModule = NULL;
   SyntaxTree tree(scope.moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule));

   SNode root = tree.readRoot();
   importTemplateTree(scope, writer, root, templateInfo, extModule);
}

void Compiler :: compileVirtualTypecastMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument)
{
   int paramCount = getParamCount(scope.message);

   writer.newNode(lxClassMethod, scope.message);

   // new stack frame
   // stack already contains current $self reference
   writer.newNode(lxNewFrame);
   writer.newNode(lxReturning);

   writer.appendNode(target, argument);

   writer.closeNode();
   writer.closeNode();

   writer.appendNode(lxParamCount, paramCount + scope.rootToFree);
   writer.appendNode(lxReserved, scope.reserved);

   writer.closeNode();
}

void Compiler :: compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument)
{
   int paramCount = getParamCount(scope.message);

   writer.newNode(lxClassMethod, scope.message);
   writer.newNode(lxDispatching);
   writer.newNode(lxResending, scope.message);

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
   if (test(classScope->info.header.flags, elStructureWrapper)) {
      // new stack frame
      // stack already contains current $self reference
      writer.newNode(lxNewFrame);
      writer.newNode(lxBoxing);
      writer.appendNode(lxTarget, scope.moduleScope->subjectHints.get(classScope->info.fieldTypes.get(0)));
      writer.appendNode(lxFieldAddress);
      writer.closeNode();
      writer.closeNode();
   }
   else {
      writer.newNode(lxExpression);
      writer.appendNode(lxResultField);
      writer.closeNode();
   }  

   writer.closeNode();
   writer.closeNode();
   writer.closeNode();
}

void Compiler :: declareVirtualMethods(ClassScope& scope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // auto generate get&type message if required
   ClassMap::Iterator c_it = moduleScope->typifiedClasses.getIt(scope.reference);
   while (!c_it.Eof()) {
      if (c_it.key() == scope.reference) {
         MethodScope methodScope(&scope);
         methodScope.message = encodeMessage(*c_it, GET_MESSAGE_ID, 0);

         // skip if there is an explicit method
         if (!scope.info.methods.exist(methodScope.message, true)) {
            scope.info.methods.add(methodScope.message, false);
         }
      }
      c_it++;
   }
}

void Compiler :: compileVirtualMethods(SyntaxWriter& writer, ClassScope& scope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // auto generate get&type message if required
   ClassMap::Iterator c_it = moduleScope->typifiedClasses.getIt(scope.reference);
   while (!c_it.Eof()) {
      if (c_it.key() == scope.reference) {
         MethodScope methodScope(&scope);
         methodScope.message = encodeMessage(*c_it, GET_MESSAGE_ID, 0);

         // skip if there is an explicit method
         if (scope.info.methods.exist(methodScope.message, false)) {
            compileVirtualTypecastMethod(writer, methodScope, lxThisLocal, 1);
         }
      }
      c_it++;
   }
}

void Compiler :: importTemplates(ClassScope& scope, SyntaxWriter& writer)
{
   SNode templateNode = scope.moduleScope->templates.readRoot().firstChild();
   while (templateNode != lxNone) {
      if (templateNode.type == lxClass && templateNode.argument == scope.reference) {
         TemplateInfo info;

         readTemplateInfo(templateNode, info);
         importTemplate(scope, writer, info);
      }

      templateNode = templateNode.nextNode();
   }
}

void Compiler :: compileClassImplementation(DNode node, ClassScope& scope)
{
   if (test(scope.info.header.flags, elExtension)) {
      scope.extensionMode = scope.info.fieldTypes.get(-1);
      if (scope.extensionMode == 0)
         scope.extensionMode = -1;
   }

   ModuleScope* moduleScope = scope.moduleScope;

   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   // import templates
   importTemplates(scope, writer);

   DNode member = node.firstChild();
   compileVMT(member, writer, scope);
   compileVirtualMethods(writer, scope);

   writer.closeNode();

   generateClassImplementation(scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (scope.extensionMode == 0)
      compileSymbolCode(scope);
}

void Compiler :: declareSingletonClass(DNode node, DNode parentNode, ClassScope& scope, DNode hints)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   // inherit parent
   if (parentNode != nsNone) {
      compileParentDeclaration(parentNode, scope);
   }
   else {
      compileParentDeclaration(DNode(), scope);

      // nested class is sealed if it has no parent
      writer.appendNode(lxClassFlag, elSealed);
   }

   compileSingletonHints(hints, writer, scope);

   declareVMT(node.firstChild(), writer, scope, false);

   // declare imported methods / flags
   if (!declareImportedTemplates(scope, writer))
      scope.raiseError(errInvalidHint, node.FirstTerminal());

   writer.closeNode();

   generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));

   scope.save();
}

void Compiler :: declareSingletonAction(ClassScope& classScope, DNode objNode, DNode expression, DNode hints)
{
   if (hints != nsNone)
      classScope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

   SyntaxWriter writer(classScope.syntaxTree);
   writer.newNode(lxRoot, classScope.reference);

   if (objNode != nsNone) {
      ActionScope methodScope(&classScope);
      declareActionScope(objNode, classScope, expression, writer, methodScope, 0, false);
      writer.newNode(lxClassMethod, methodScope.message);

      writer.closeNode();
   }

   writer.closeNode();

   generateInlineClassDeclaration(classScope, test(classScope.info.header.flags, elClosed));

   classScope.save();
}

void Compiler :: compileSingletonClass(DNode node, ClassScope& scope)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   DNode member = node.firstChild();

   // import templates
   importTemplates(scope, writer);

   compileVMT(member, writer, scope);

   writer.closeNode();

   generateClassImplementation(scope);
}

void Compiler :: compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints)
{
   bool singleton = false;
   
   DNode expression = node.firstChild();
   // if it is a singleton
   if (isSingleStatement(expression)) {
      DNode objNode = expression.firstChild().firstChild();
      if (objNode == nsNestedClass) {
         DNode classNode = expression.firstChild();

         ClassScope classScope(scope.moduleScope, scope.reference);

         if (classNode.Terminal() != nsNone) {
            declareSingletonClass(classNode.firstChild(), classNode, classScope, hints);
            singleton = true;
         }
         // if it is normal nested class
         else {
            declareSingletonClass(classNode.firstChild(), DNode(), classScope, hints);
            singleton = true;
         }
      }
      else if (objNode == nsSubCode) {
         ClassScope classScope(scope.moduleScope, scope.reference);

         declareSingletonAction(classScope, objNode, DNode(), hints);
         singleton = true;
      }
      else if (objNode == nsInlineExpression) {
         ClassScope classScope(scope.moduleScope, scope.reference);

         declareSingletonAction(classScope, objNode, expression.firstChild(), hints);
         singleton = true;
      }
      else if (objNode == nsSubjectArg || objNode == nsMethodParameter) {
         ClassScope classScope(scope.moduleScope, scope.reference);

         declareSingletonAction(classScope, objNode, objNode, hints);
         singleton = true;
      }
      else compileSymbolHints(hints, scope, false);
   }
   else compileSymbolHints(hints, scope, false);

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

         compileAction(classNode, classScope, DNode(), 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
      else if (classNode == nsInlineExpression) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(classNode, classScope, expression.firstChild(), 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
      else if (classNode == nsSubjectArg || classNode == nsMethodParameter) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(goToSymbol(classNode, nsInlineExpression), classScope, classNode, 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
   }

   // compile symbol into byte codes

   SyntaxWriter writer(scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot, scope.reference);

   CodeScope codeScope(&scope, &writer);
   if (retVal.kind == okUnknown) {
      compileSymbolHints(hints, scope, true);
      
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
      else scope.raiseError(errInvalidOperation, expression.FirstTerminal());
   }

   // NOTE : close root node
   writer.closeNode();

   optimizeSymbolTree(scope);

   _writer.generateSymbol(scope.tape, scope.syntaxTree, isStatic);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.syntaxTree.Strings(), scope.moduleScope->sourcePathRef);
}

bool Compiler :: boxPrimitive(ModuleScope& scope, SyntaxTree::Node& node, ref_t targetRef, int warningLevel, int mode, bool& variable)
{
   LexicalType opType = node.type;

   int size = 0;
   if (isPrimitiveRef(targetRef)) {
      variable = false;
      if (targetRef == -1) {
         size = 4;
      }
   }
   else size = scope.defineStructSizeEx(targetRef, variable);

   if (size != 0) {
      int offset = allocateStructure(scope, node, size);

      // allocate place for the operation result
      node.injectNode(opType, node.argument);

      node = lxAssigning;
      node.setArgument(size);

      node.insertNode(lxLocalAddress, offset);

      if (!test(mode, HINT_NOBOXING)) {
         node.injectNode(node.type, node.argument);

         node = lxBoxing;
         node.setArgument(size);

         node.appendNode(lxTarget, targetRef);

         optimizeBoxing(scope, node, warningLevel, 0);

         node = SyntaxTree::findChild(node, lxAssigning);
      }
      else node.appendNode(lxTarget, targetRef);

      node = SyntaxTree::findChild(node, opType);

      return true;
   }
   else return false;
}

void Compiler :: optimizeExtCall(ModuleScope& scope, SNode node, int warningMask, int mode)
{
   SNode parentNode = node.parentNode();
   while (parentNode == lxExpression)
      parentNode = parentNode.parentNode();

   if (parentNode == lxAssigning) {
      if (parentNode.argument != 4) {
         boxPrimitive(scope, node, -1, warningMask, mode);
      }
   }
   else if (parentNode == lxTypecasting) {
      boxPrimitive(scope, node, -1, warningMask, mode);
   }

   SNode arg = node.firstChild();
   while (arg != lxNone) {
      if (arg == lxIntExtArgument || arg == lxExtArgument) {
         optimizeSyntaxExpression(scope, arg, warningMask, HINT_NOBOXING | HINT_EXTERNALOP);
      }
      arg = arg.nextNode();
   }
}

void Compiler :: optimizeInternalCall(ModuleScope& scope, SNode node, int warningMask, int mode)
{
   //SNode parentNode = node.parentNode();
   //while (parentNode == lxExpression)
   //   parentNode = parentNode.parentNode();

   //if (parentNode == lxAssigning) {
   //   boxPrimitive(scope, node, -1, warningMask, mode);
   //}

   optimizeSyntaxExpression(scope, node, warningMask, HINT_NOBOXING);
}

void Compiler :: optimizeDirectCall(ModuleScope& scope, SNode node, int warningMask)
{
   int mode = 0;

   bool stackSafe = SyntaxTree::existChild(node, lxStacksafe);

   if (node == lxDirectCalling && SyntaxTree::existChild(node, lxEmbeddable)) {
      // check if it is a virtual call
      if (getVerb(node.argument) == GET_MESSAGE_ID && getParamCount(node.argument) == 0) {
         SNode callTarget = SyntaxTree::findChild(node, lxCallTarget);

         ClassInfo info;
         scope.loadClassInfo(info, callTarget.argument);
         if (info.methodHints.get(Attribute(node.argument, maEmbeddableIdle)) == -1) {
            // if it is an idle call, remove it
            node = lxExpression;

            optimizeSyntaxExpression(scope, node, warningMask, mode);
         }
      }
   }

   if (stackSafe)
      mode |= HINT_NOBOXING;

   optimizeSyntaxExpression(scope, node, warningMask, mode);
}

void Compiler :: optimizeCall(ModuleScope& scope, SNode node, int warningMask)
{
   int mode = 0;

   bool stackSafe = false;
   bool methodNotFound = false;
   SNode target = SyntaxTree::findChild(node, lxCallTarget);
   // HOT FIX : if call target not defined
   if (target == lxNone) {
      SNode callee = SyntaxTree::findMatchedChild(node, lxObjectMask);
      if (callee == lxField || callee == lxLocal) {
         SNode attr = SyntaxTree::findChild(callee, lxType);
         if (attr == lxType) {
            ref_t classRef = scope.subjectHints.get(attr.argument);
            if (classRef) {
               node.appendNode(lxCallTarget, classRef);

               target = SyntaxTree::findChild(node, lxCallTarget);
            }               
         }
      }
   }

   if (target.argument != 0) {
      ClassInfo info;
      if (scope.loadClassInfo(info, target.argument)) {
         ref_t resultType;
         int hint = scope.checkMethod(info, node.argument, resultType);
         
         if (hint == tpUnknown) {
            // Compiler magic : allow to call wrapper content directly
            if (test(info.header.flags, elWrapper)) {
               target.setArgument(scope.subjectHints.get(info.fieldTypes.get(0)));

               hint = scope.checkMethod(target.argument, node.argument);

               // for dynamic object, target object should be overridden
               if (!test(info.header.flags, elStructureRole)) {
                  node.appendNode(lxOverridden);
                  SNode n = SyntaxTree::findChild(node, lxOverridden);
                  n.appendNode(lxCurrentField);
               }
               else {
                  node.appendNode(lxOverridden);
                  SNode n = SyntaxTree::findChild(node, lxOverridden);
                  n.appendNode(lxBoxing);
                  n = SyntaxTree::findChild(n, lxBoxing);
                  n.appendNode(lxCurrent);
                  n.appendNode(lxTarget, target.argument);
               }
            }            
         }

         methodNotFound = hint == tpUnknown;
         switch (hint & tpMask) {
            case tpSealed:
               stackSafe = test(hint, tpStackSafe);
               node = lxDirectCalling;
               if (resultType != 0)
                  node.appendNode(lxType, resultType);
               break;
            case tpClosed:
               stackSafe = test(hint, tpStackSafe);
               node = lxSDirctCalling;
               if (resultType != 0)
                  node.appendNode(lxType, resultType);
               break;
         }
      }
   }

   if (stackSafe)
      mode |= HINT_NOBOXING;

   optimizeSyntaxExpression(scope, node, warningMask, mode);

   if (methodNotFound && test(warningMask, WARNING_LEVEL_1)) {
      SNode row = SyntaxTree::findChild(node, lxRow);
      SNode col = SyntaxTree::findChild(node, lxCol);
      SNode terminal = SyntaxTree::findChild(node, lxTerminal);
      if (col != lxNone && row != lxNone) {
         scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, row.argument, col.argument, terminal.identifier());
      }
   }
}

int Compiler :: mapOpArg(Compiler::ModuleScope& scope, SNode arg, ref_t& target)
{
   target = SyntaxTree::findChild(arg, lxTarget).argument;

   int flags = mapOpArg(scope, arg);

   // HOTFIX : if the target is not defined - consider it as primitive one
   if (target == 0) {
      switch (flags) {
         case elDebugDWORD:
            target = -1;
            break;
         case elDebugQWORD:
            target = -2;
            break;
         case elDebugReal64:
            target = -4;
            break;
         default:
            break;
      }
   }

   // HOTFIX : check the type as well
   if (target == 0) {
      ref_t type = SyntaxTree::findChild(arg, lxType).argument;
      if (type != 0)
         target = scope.subjectHints.get(type);
   }

   return flags;
}

int Compiler :: mapOpArg(ModuleScope& scope, SNode arg)
{
   int flags = 0;

   ref_t ref = SyntaxTree::findChild(arg, lxTarget).argument;
   if (ref == 0) {
      ref_t type = SyntaxTree::findChild(arg, lxType).argument;
      if (type != 0)
         ref = scope.subjectHints.get(type);
   }

   if (isPrimitiveRef(ref) || ref == 0) {
      switch (ref) {
         case -1:
            return elDebugDWORD;
         case -2:
            return elDebugQWORD;
         case -4:
            return elDebugReal64;
         case -8:
            return elDebugSubject;
         default:
            return 0;
      }
   }
   else {
      flags = scope.getClassFlags(ref);

      return flags & elDebugMask;
   }   
}

inline LexicalType mapArrPrimitiveOp(int size)
{
   switch (size)
   {
      case 4:
         return lxIntArrOp;
      case 1:
         return lxByteArrOp;
      case 2:
         return lxShortArrOp;
      default:
         return lxBinArrOp;
   }
}

void Compiler :: optimizeBoolOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
{
   SNode typecastNode = SyntaxTree::findChild(node, lxTypecasting);
   SNode opNode = SyntaxTree::findChild(typecastNode, lxOp);

   if (opNode == lxOp && optimizeOp(scope, opNode, warningLevel, mode)) {
      typecastNode = lxExpression;
      node = lxExpression;
      // HOTFIX : mark it as a boolean operation
      node.appendNode(lxType, scope.boolType);
   }
   else optimizeSyntaxExpression(scope, node, warningLevel);
}

bool Compiler :: optimizeOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
{   
   ref_t destType = 0;
   SNode parent = node.parentNode();
   while (parent != lxNewFrame) {
      if (parent == lxTypecasting) {
         destType = getSignature(parent.argument);
         break;
      }
      else parent = parent.parentNode();;
   }

   if (node.argument == SET_REFER_MESSAGE_ID) {
      SNode larg, narg, rarg;
      assignOpArguments(node, larg, narg, rarg);

      ref_t lref = SyntaxTree::findChild(larg, lxTarget).argument;
      int nflags = mapOpArg(scope, narg);

      if (isPrimitiveRef(lref)) {
         if (lref == -3 && nflags == elDebugDWORD) {
            destType = SyntaxTree::findChild(larg, lxType).argument;
            if (checkIfCompatible(scope, destType, rarg)) {
               int size = scope.defineSubjectSize(destType);
               node.appendNode(lxSize, size);
               node = mapArrPrimitiveOp(size);
            }
         }
         else if (lref == -5 && nflags == elDebugDWORD) {
            destType = SyntaxTree::findChild(larg, lxType).argument;
            if (checkIfCompatible(scope, destType, rarg)) {
               node = lxArrOp;
            }
         }
      }

      if (node == lxOp) {
         node.setArgument(encodeMessage(0, node.argument, 2));
         node = lxCalling;

         optimizeCall(scope, node, warningLevel);

         return false;
      }
      else {
         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
         optimizeSyntaxNode(scope, narg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);

         return true;
      }
   }
   else {
      bool boxing = false;
      SNode larg, rarg;
      assignOpArguments(node, larg, rarg);

      if (larg == lxOp) {
         optimizeOp(scope, larg, /*warningLevel*/0, 0);
         //HOTFIX : arguments should be reread because larg can be modified
         larg = SyntaxTree::findMatchedChild(node, lxObjectMask);
         rarg = SyntaxTree::findSecondMatchedChild(node, lxObjectMask);
      }
      if (rarg == lxOp) {
         optimizeOp(scope, rarg, /*warningLevel*/0, 0);

         //HOTFIX : argument should be reread because rarg can be modified
         rarg = SyntaxTree::findSecondMatchedChild(node, lxObjectMask);
      }

      ref_t target = 0;
      int lflags = mapOpArg(scope, larg, target);
      int rflags = mapOpArg(scope, rarg);

      if (IsNumericOperator(node.argument)) {
         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
            target = -1;
            node = lxIntOp;
            boxing = true;
         }
         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
            target = -2;
            node = lxLongOp;
            boxing = true;
         }
         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
            target = -4;
            node = lxRealOp;
            boxing = true;
         }
      }
      else if (node.argument == READ_MESSAGE_ID) {
         if (target == -3 && rflags == elDebugDWORD) {
            target = scope.subjectHints.get(SyntaxTree::findChild(larg, lxType).argument);
            int size = scope.defineStructSize(target);
            node.appendNode(lxSize, size);
            node = mapArrPrimitiveOp(size);
         }
         else if (target == -5 && rflags == elDebugDWORD) {
            node = lxArrOp;
         }
         else if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
            target = -1;
            node = lxIntOp;
            boxing = true;
         }
         else if (lflags == elDebugQWORD && rflags == elDebugDWORD) {
            target = -2;
            node = lxLongOp;
            boxing = true;
         }
      }
      else if (node.argument == WRITE_MESSAGE_ID) {
         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
            target = -1;
            node = lxIntOp;
            boxing = true;
         }
         else if (lflags == elDebugQWORD && rflags == elDebugDWORD) {
            target = -2;
            node = lxLongOp;
            boxing = true;
         }
      }
      else if (IsBitwiseOperator(node.argument)) {
         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
            target = -1;
            node = lxIntOp;
            boxing = true;
         }
         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
            target = -2;
            node = lxLongOp;
            boxing = true;
         }
      }
      else if (IsCompOperator(node.argument)) {
         if (lflags == elDebugDWORD && (rflags == elDebugDWORD || rflags == elDebugPTR)) {
            node = lxIntOp;
         }
         else if (lflags == elDebugPTR && rflags == elDebugPTR) {
            node = lxIntOp;
         }
         else if (lflags == elDebugSubject && rflags == elDebugSubject) {
            node = lxIntOp;
         }
         else if (lflags == elDebugQWORD && (rflags == elDebugQWORD || rflags == elDebugDPTR)) {
            node = lxLongOp;
         }
         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
            node = lxRealOp;
         }

         node.appendNode(lxType, scope.boolType);
      }
      else if (IsVarOperator(node.argument)) {
         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
            target = -1;
            node = lxIntOp;
         }
         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
            target = -2;
            node = lxLongOp;
         }
         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
            target = -4;
            node = lxRealOp;
         }
      }
      else if (IsReferOperator(node.argument) && rflags == elDebugDWORD) {         
         destType = SyntaxTree::findChild(larg, lxType).argument;

         if (target == -3 && destType != 0) {
            target = scope.subjectHints.get(destType);
            int size = scope.defineStructSize(target);
            node.appendNode(lxSize, size);
            node = mapArrPrimitiveOp(size);
            boxing = true;
         }
         else if (target == -5 || target == scope.paramsReference) {
            node = lxArrOp;
         }
      }

      if (node == lxOp) {
         node.setArgument(encodeMessage(0, node.argument, 1));
         node = lxCalling;

         if (target != 0)
            node.appendNode(lxCallTarget, target);

         optimizeCall(scope, node, warningLevel);

         return false;
      }
      else {
         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);

         // HOTFIX : if larg is boxing, the second operator should be reassigned
         if (larg == lxBoxing) {
            rarg = SyntaxTree::findSecondMatchedChild(node, lxObjectMask);
         }
         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);

         if (boxing) {
            if (isPrimitiveRef(target)) {
               if (destType != 0) {
                  //if destination type is known try to check the compatibility
                  int flags = scope.getTypeFlags(destType);
                  if (test(flags, elWrapper)) {
                     ClassInfo destInfo;
                     scope.loadClassInfo(destInfo, scope.subjectHints.get(destType));
                     destType = destInfo.fieldTypes.get(0);

                     flags = scope.getTypeFlags(destType);
                  }

                  flags &= elDebugMask;

                  if (flags == elDebugDWORD && target == -1) {
                     target = scope.subjectHints.get(destType);
                  }
                  else if (flags == elDebugQWORD && target == -2) {
                     target = scope.subjectHints.get(destType);
                  }
                  else if (flags == elDebugReal64 && target == -4) {
                     target = scope.subjectHints.get(destType);
                  }
               }

               switch (target) {
                  case -1:
                     target = scope.intReference;
                     break;
                  case -2:
                     target = scope.longReference;
                     break;
                  case -4:
                     target = scope.realReference;
                     break;
                  default:
                     break;
               }
            }

            boxPrimitive(scope, node, target, warningLevel, mode);
         }            

         return true;
      }
   }
}

void Compiler :: optimizeNewOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
{
   optimizeSyntaxExpression(scope, node, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);

   SNode expr = SyntaxTree::findMatchedChild(node, lxObjectMask);
   if (expr == lxExpression)
      expr = SyntaxTree::findMatchedChild(expr, lxObjectMask);

   ref_t type = SyntaxTree::findChild(expr, lxType).argument;
   ref_t classRef = SyntaxTree::findChild(expr, lxTarget).argument;
   if (classRef == 0 && type != 0)
      classRef = scope.subjectHints.get(type);

   if (classRef != -1 && (scope.getClassFlags(classRef) & elDebugMask) != elDebugDWORD) {
      SNode row = SyntaxTree::findChild(node, lxRow);
      SNode col = SyntaxTree::findChild(node, lxCol);
      SNode terminal = SyntaxTree::findChild(node, lxTerminal);

      scope.raiseError(errInvalidOperation, row.argument, col.argument, terminal.identifier());
   }
}

void Compiler :: optimizeEmbeddableCall(ModuleScope& scope, SNode& assignNode, SNode& callNode)
{
   SNode callTarget = SyntaxTree::findChild(callNode, lxCallTarget);

   ClassInfo info;
   scope.loadClassInfo(info, callTarget.argument);

   ref_t subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddableGet));
   // if it is possible to replace get&subject operation with eval&subject2:local
   if (subject != 0) {
      // removing assinging operation
      assignNode = lxExpression;

      // move assigning target into the call node
      SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
      if (assignTarget != lxNone) {
         callNode.appendNode(assignTarget.type, assignTarget.argument);
         assignTarget = lxExpression;
         callNode.setArgument(encodeMessage(subject, EVAL_MESSAGE_ID, 1));
      }
   }
}

void Compiler :: optimizeAssigning(ModuleScope& scope, SNode node, int warningLevel)
{
   int mode = HINT_NOUNBOXING | HINT_ASSIGNING;
   if (node.argument != 0)
      mode |= HINT_NOBOXING;

   bool targetNode = true;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (targetNode) {
            targetNode = false;

            // HOTFIX : remove boxing node for assignee
            if (current == lxBoxing || current == lxCondBoxing) {
               SNode subNode = SyntaxTree::findMatchedChild(current, lxObjectMask);

               if (node.argument == 0 && subNode == lxFieldAddress) {
                  // HOT FIX : for template target define the assignment size
                  defineTargetSize(scope, current);
                  node.setArgument(current.argument);
                  if (node.argument != 0)
                     mode |= HINT_NOBOXING;
               }
               
               current = subNode.type;
               current.setArgument(subNode.argument);
            }
         }
         else optimizeSyntaxNode(scope, current, warningLevel, mode);
      }
      current = current.nextNode();
   }

   if (node.argument != 0) {
      SNode intValue = findSubNode(node, lxConstantInt);
      if (intValue != lxNone) {
         node = lxIntOp;
         node.setArgument(SET_MESSAGE_ID);
      }
      else {
         SNode directCall = findSubNode(node, lxDirectCalling, lxSDirctCalling);
         if (directCall != lxNone && SyntaxTree::existChild(directCall, lxEmbeddable)) {
            optimizeEmbeddableCall(scope, node, directCall);
         }
      }
   }

   // assignment operation
   SNode assignNode = findSubNode(node, lxAssigning);
   if (assignNode != lxNone) {
      SNode operationNode = SyntaxTree::findChild(assignNode, lxIntOp, lxRealOp);
      if (operationNode != lxNone) {
         SNode larg = findSubNodeMask(operationNode, lxObjectMask);
         SNode target = SyntaxTree::findMatchedChild(node, lxObjectMask);
         if (larg.type == target.type && larg.argument == target.argument) {
            // remove an extra assignment
            larg = findSubNodeMask(assignNode, lxObjectMask);

            larg = target.type;
            larg.setArgument(target.argument);
            node = lxExpression;
            target = lxIdle;
         }
      }
   }
}

bool Compiler :: defineTargetSize(ModuleScope& scope, SNode& node)
{
   bool variable = false;

   SNode target = SyntaxTree::findChild(node, lxTarget);
   ref_t type = SyntaxTree::findChild(node, lxType).argument;

   // HOTFIX : try to resolve target if it is not defined 
   if (target == lxNone && type != 0) {
      node.appendNode(lxTarget, scope.subjectHints.get(type));

      target = SyntaxTree::findChild(node, lxTarget);
   }

   // HOT FIX : box / assign primitive structures
   if (isPrimitiveRef(target.argument)) {      
      if (type == 0) {
         if (target.argument == -1) {
            target.setArgument(scope.intReference);
            node.setArgument(4);
         }
         else raiseWarning(scope, node, errInvalidOperation, 0, 0);
      }
      else {
         int size = scope.defineSubjectSizeEx(type, variable, false);

         if (target.argument == -3) {
            node.setArgument(-size);
         }
         else node.setArgument(size);

         target.setArgument(scope.subjectHints.get(type));
      }
   }
   else if (node.argument == 0) {
      node.setArgument(scope.defineStructSizeEx(target.argument, variable));
   }

   return variable;
}

void Compiler :: optimizeArgUnboxing(ModuleScope& scope, SNode node, int warningLevel)
{
   SNode object = SyntaxTree::findMatchedChild(node, lxObjectMask);
   if (object == lxArgBoxing)
      object = lxExpression;

   optimizeSyntaxExpression(scope, node, warningLevel);
}

void Compiler :: optimizeBoxing(ModuleScope& scope, SNode node, int warningLevel, int mode)
{
   bool boxing = true;
   bool variable = false;

   SNode exprNode = SyntaxTree::findMatchedChild(node, lxObjectMask);
   if (exprNode == lxNewOp) {
      boxing = false;
   }
   else {
      // if no boxing hint provided
      // then boxing should be skipped
      if (test(mode, HINT_NOBOXING)) {
         if (exprNode == lxFieldAddress && exprNode.argument > 0 && !test(mode, HINT_ASSIGNING)) {
            ref_t target = SyntaxTree::findChild(node, lxTarget).argument;
            if (!target)
               throw InternalError("Boxing can not be performed");

            boxPrimitive(scope, exprNode, target, warningLevel, mode, variable);

            node = variable ? lxLocalUnboxing : lxExpression;

            return;
         }
         boxing = false;
      }
      else if (test(mode, HINT_NOCONDBOXING) && node == lxCondBoxing) {
         node = lxBoxing;
      }
   }

   if (boxing) {
      variable = defineTargetSize(scope, node);
      if (variable)
         node = lxUnboxing;
   }
   // ignore boxing operation if allowed
   else node = lxExpression;

   optimizeSyntaxExpression(scope, node, warningLevel, HINT_NOBOXING);

   raiseWarning(scope, node, wrnBoxingCheck, WARNING_LEVEL_3, warningLevel, boxing);
}

bool Compiler :: checkIfImplicitBoxable(ModuleScope& scope, ref_t sourceClassRef, ClassInfo& targetInfo)
{
   if (sourceClassRef == -1 && (targetInfo.header.flags & elDebugMask) == elDebugDWORD) {
      return true;
   }
   else if (sourceClassRef == -2 && (targetInfo.header.flags & elDebugMask) == elDebugQWORD) {
      return true;
   }
   else if (sourceClassRef == -4 && (targetInfo.header.flags & elDebugMask) == elDebugReal64) {
      return true;
   }
   else if (sourceClassRef != 0 && scope.subjectHints.exist(targetInfo.fieldTypes.get(0), sourceClassRef)) {
      return true;
   }
   else return false;
}

void Compiler :: raiseWarning(ModuleScope& scope, SNode node, ident_t message, int warningLevel, int warningMask, bool triggered)
{
   if (test(warningMask, warningLevel) && triggered) {
      while (node != lxNewFrame) {
         SNode row = SyntaxTree::findChild(node, lxRow);
         SNode col = SyntaxTree::findChild(node, lxCol);
         SNode terminal = SyntaxTree::findChild(node, lxTerminal);
         if (col != lxNone && row != lxNone) {
            scope.raiseWarning(warningLevel, message, row.argument, col.argument, terminal.identifier());
            break;
         }
         else node = node.parentNode();
      }
   }
}

int Compiler :: tryTypecasting(ModuleScope& scope, ref_t targetType, SNode& node, SNode& object, bool& typecasted, int mode)
{
   int typecastMode = 0;

   ref_t sourceType = SyntaxTree::findChild(object, lxType).argument;
   ref_t sourceClassRef = SyntaxTree::findChild(object, lxTarget).argument;

   if (sourceClassRef == 0 && sourceType != 0) {
      sourceClassRef = scope.subjectHints.get(sourceType);
   }

   // NOTE : compiler magic!
   // if the target is wrapper (container) around the source
   ref_t targetClassRef = scope.subjectHints.get(targetType);
   if (targetClassRef != 0) {
      ClassInfo targetInfo;
      scope.loadClassInfo(targetInfo, targetClassRef, false);

      // HOT FIX : trying to typecast primitive structure array
      if (sourceClassRef == -3) {
         if (test(targetInfo.header.flags, elStructureRole | elDynamicRole) && targetInfo.fieldTypes.get(-1) == sourceType) {
            // if boxing is not required (stack safe) and can be passed directly
            if (test(mode, HINT_NOBOXING)) {
               node = lxExpression;
               typecastMode |= HINT_NOBOXING;
            }
            else if (object == lxNewOp) {
               object.setArgument(targetClassRef);
               object.appendNode(lxSize, targetInfo.size);
            }
            else {
               // if unboxing is not required
               if (test(mode, HINT_NOUNBOXING)) {
                  node = lxBoxing;
               }
               else node = lxUnboxing;

               node.setArgument(targetInfo.size);

               node.appendNode(lxTarget, targetClassRef);
            }

            typecasted = false;
         }
      }
      // HOT FIX : trying to typecast primitive object array
      else if (sourceClassRef == -5) {
         if (test(targetInfo.header.flags, elDynamicRole) && targetInfo.fieldTypes.get(-1) == sourceType) {
            if (object == lxNewOp) {
               object.setArgument(targetClassRef);
               object.appendNode(lxSize, targetInfo.size);
            }

            typecasted = false;
         }
      }
      else if (test(targetInfo.header.flags, elStructureRole | elEmbeddableWrapper)) {
         // if target is source wrapper (i.e. target is a source container)
         if (checkIfImplicitBoxable(scope, sourceClassRef, targetInfo)) {
            // if boxing is not required (stack safe) and can be passed directly
            if (test(mode, HINT_NOBOXING)) {
               node = lxExpression;
            }
            else {
               // if unboxing is not required
               if (test(targetInfo.header.flags, elReadOnlyRole) || test(mode, HINT_NOUNBOXING)) {
                  node = lxBoxing;
               }
               else node = lxUnboxing;

               node.setArgument(targetInfo.size);

               node.appendNode(lxTarget, targetClassRef);
            }

            typecastMode |= (HINT_NOBOXING | HINT_NOUNBOXING);
            typecasted = false;
         }
      }
      else if (test(targetInfo.header.flags, elStructureRole) && sourceClassRef != 0) {
         ClassInfo sourceInfo;
         scope.loadClassInfo(sourceInfo, sourceClassRef, false);
         // if source is target wrapper (i.e. source is a target container)
         if (test(sourceInfo.header.flags, elStructureRole | elEmbeddableWrapper) && scope.subjectHints.exist(sourceInfo.fieldTypes.get(0), targetClassRef)) {
            // if boxing is not required (stack safe) and can be passed directly
            if (test(mode, HINT_NOBOXING)) {
               node = lxExpression;
            }
            else {
               // if unboxing is not required
               if (test(sourceInfo.header.flags, elReadOnlyRole) || test(mode, HINT_NOUNBOXING)) {
                  node = lxBoxing;
               }
               else node = lxUnboxing;

               node.setArgument(sourceInfo.size);

               node.appendNode(lxTarget, sourceClassRef);
            }

            typecastMode |= (HINT_NOBOXING | HINT_NOUNBOXING);
            typecasted = false;
         }
         else if (isDWORD(targetInfo.header.flags) && isPTR(sourceInfo.header.flags)) {
            //HOTFIX : allow passing dirty_ptr as int
            typecastMode |= (HINT_NOBOXING | HINT_NOUNBOXING);
            typecasted = false;
            boxPrimitive(scope, object, targetClassRef, 0, typecastMode);

            //HOTFIX :  set the correct size
            SNode parent = object.parentNode();
            parent.setArgument(sourceInfo.size);
         }
         else if (test(targetInfo.header.flags, elSealed)) {
            int implicitMessage = encodeMessage(sourceType, PRIVATE_MESSAGE_ID, 1);
            if (targetInfo.methods.exist(implicitMessage)) {
               if (test(mode, HINT_ASSIGNING | HINT_NOUNBOXING) && test(targetInfo.methodHints.get(Attribute(implicitMessage, maHint)), tpStackSafe)) {
                  // if embeddable call is possible - assigning should be replaced with direct method call
                  SNode parent = node.parentNode();
                  parent = lxDirectCalling;
                  parent.setArgument(implicitMessage);
                  parent.appendNode(lxCallTarget, targetClassRef);

                  if (sourceInfo.size < targetInfo.size) {
                     // if the source is smaller than the target it should be boxed
                     boxPrimitive(scope, object, sourceClassRef, 0, HINT_NOBOXING);
                     //HOTFIX :  set the correct size
                     SNode objectParent = object.parentNode();
                     objectParent.setArgument(sourceInfo.size);
                  }
               }
               else {
                  node = lxCalling;
                  node.setArgument(implicitMessage);
                  node.insertNode(lxCreatingStruct, targetInfo.size);
                  SyntaxTree::findChild(node, lxCreatingStruct).appendNode(lxTarget, targetClassRef);

                  node.appendNode(lxCallTarget, targetClassRef);
               }

               typecasted = false;
            }
         }
      }
      else if (test(targetInfo.header.flags, elWrapper)) {
         // if the target is generic wrapper (container)
         if (!test(mode, HINT_EXTERNALOP)) {
            node.setArgument(0);
            node = test(mode, HINT_NOUNBOXING) ? lxBoxing : lxUnboxing;
            node.appendNode(lxTarget, targetClassRef);
         }
         else {
            // HOTFIX : allow to pass the reference to the object directly 
            // for an external operation
            node = lxExpression;
            typecastMode = mode;
         }

         typecasted = false;
      }
      // check if there is implicit constructors
      else if (test(targetInfo.header.flags, elSealed) && sourceType != 0) {
         int implicitMessage = encodeMessage(sourceType, PRIVATE_MESSAGE_ID, 1);
         if (targetInfo.methods.exist(implicitMessage)) {
            node = lxCalling;
            node.setArgument(implicitMessage);
            node.insertNode(lxCreatingClass, targetInfo.fields.Count());
            SyntaxTree::findChild(node, lxCreatingClass).appendNode(lxTarget, targetClassRef);

            node.appendNode(lxCallTarget, targetClassRef);
            typecasted = false;            
         }
      }
   }

   return typecastMode;
}

void Compiler :: optimizeTypecast(ModuleScope& scope, SNode node, int warningMask, int mode)
{
   // HOTFIX : virtual typecast
   if (node.argument == 0) {
      SNode parent = node.parentNode();
      if (parent == lxAssigning) {
         SNode assignTarget = SyntaxTree::findMatchedChild(parent, lxObjectMask);
         if (assignTarget == lxExpression)
            assignTarget = SyntaxTree::findMatchedChild(assignTarget, lxObjectMask);

         SNode type = SyntaxTree::findChild(assignTarget, lxType);

         node.setArgument(encodeMessage(type.argument, GET_MESSAGE_ID, 0));
      }
   }

   ref_t targetType = getSignature(node.argument);
   bool optimized = false;

   int typecastMode = 0;
   bool typecasted = true;
   if (scope.subjectHints.get(targetType) != 0) {
      SNode object = SyntaxTree::findMatchedChild(node, lxObjectMask);

      // HOTFIX : primitive / external operation should be done before
      if (object == lxOp) {
         optimizeOp(scope, object, warningMask, mode);

         object = SyntaxTree::findMatchedChild(node, lxObjectMask);

         optimized = true;
      }
      if (object == lxCalling) {
         optimizeCall(scope, object, warningMask);

         object = SyntaxTree::findMatchedChild(node, lxObjectMask);

         optimized = true;
      }
      else if (object == lxBoolOp) {
         optimizeBoolOp(scope, object, warningMask, mode);

         object = SyntaxTree::findMatchedChild(node, lxObjectMask);

         optimized = true;
      }
      else if (object == lxExternalCall || object == lxStdExternalCall || object == lxCoreAPICall) {
         optimizeExtCall(scope, object, warningMask, mode);

         object = SyntaxTree::findMatchedChild(node, lxObjectMask);

         optimized = true;         
      }

      if (!checkIfCompatible(scope, targetType, object)) {
         typecastMode = tryTypecasting(scope, targetType, node, object, typecasted, mode);
      }
      else typecasted = false;
   }
   else typecasted = false;

   if (!typecasted && node == lxTypecasting) {
      typecastMode = mode;

      node = lxExpression;
   }

   if (node == lxBoxing || node == lxUnboxing || node == lxLocalUnboxing) {
      optimizeBoxing(scope, node, warningMask, 0);
   }
   else if (!optimized) {
      if (typecasted) {
         optimizeSyntaxExpression(scope, node, warningMask, typecastMode);
      }
      else optimizeSyntaxNode(scope, node, warningMask, typecastMode);
   }

   raiseWarning(scope, node, wrnTypeMismatch, WARNING_LEVEL_2, warningMask, typecasted);
}

int Compiler :: allocateStructure(ModuleScope& scope, SNode node, int& size)
{
   // finding method's reserved attribute
   SNode methodNode = node.parentNode();
   while (methodNode != lxClassMethod)
      methodNode = methodNode.parentNode();

   SNode reserveNode = SyntaxTree::findChild(methodNode, lxReserved);
   int reserved = reserveNode.argument;

   // allocating space
   int offset = allocateStructure(false, size, reserved);

   // HOT FIX : size should be in bytes
   size *= 4;

   reserveNode.setArgument(reserved);

   return offset;
}

void Compiler :: optimizeSyntaxNode(ModuleScope& scope, SyntaxTree::Node current, int warningMask, int mode)
{
   switch (current.type) {
      case lxAssigning:
         optimizeAssigning(scope, current, warningMask);
         break;
      case lxTypecasting:
         optimizeTypecast(scope, current, warningMask, mode);
         break;
      case lxStdExternalCall:
      case lxExternalCall:
      case lxCoreAPICall:
         optimizeExtCall(scope, current, warningMask, mode);
         break;
      case lxInternalCall:
         optimizeInternalCall(scope, current, warningMask, mode);
         break;
      case lxExpression:
      case lxOverridden:
      case lxVariable:
         // HOT FIX : to pass the optimization mode into sub expression
         optimizeSyntaxExpression(scope, current, warningMask, mode);
         break;
      case lxReturning:
         optimizeSyntaxExpression(scope, current, warningMask, HINT_NOUNBOXING | HINT_NOCONDBOXING);
         break;
      case lxBoxing:
      case lxCondBoxing:
      case lxArgBoxing:
         optimizeBoxing(scope, current, warningMask, mode);
         break;
      case lxOp:
         optimizeOp(scope, current, warningMask, mode);
         break;
      case lxBoolOp:
         optimizeBoolOp(scope, current, warningMask, mode);
         break;
      case lxDirectCalling:
      case lxSDirctCalling:
         optimizeDirectCall(scope, current, warningMask);
         break;
      case lxCalling:
         optimizeCall(scope, current, warningMask);
         break;
      case lxNewOp:
         optimizeNewOp(scope, current, warningMask, 0);
         break;
      case lxNested:
      case lxMember:
         optimizeSyntaxExpression(scope, current, warningMask);
         break;
      case lxArgUnboxing:
         optimizeArgUnboxing(scope, current, warningMask);
         break;
      default:
         if (test(current.type, lxExpressionMask)) {
            optimizeSyntaxExpression(scope, current, warningMask);
         }
         break;
   }
}

void Compiler :: optimizeSyntaxExpression(ModuleScope& scope, SNode node, int warningMask, int mode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxWarningMask) {
         warningMask = current.argument;
      }
      else optimizeSyntaxNode(scope, current, warningMask, mode);

      current = current.nextNode();
   }
}

void Compiler :: optimizeClassTree(ClassScope& scope)
{
   int warningMask = scope.moduleScope->warningMask;
   SNode current = scope.syntaxTree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         optimizeSyntaxExpression(*scope.moduleScope, current, warningMask);

         if (test(_optFlag, 1)) {
            if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
               defineEmbeddableAttributes(scope, current);
            }
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: optimizeSymbolTree(SourceScope& scope)
{
   int warningMask = 0;
   SNode current = scope.syntaxTree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxWarningMask) {
         warningMask = current.argument;
      }
      else if (test(current.type, lxExpressionMask)) {
         optimizeSyntaxExpression(*scope.moduleScope, current, warningMask);
      }

      current = current.nextNode();
   }
}

bool Compiler :: recognizeEmbeddableGet(ModuleScope& scope, SyntaxTree& tree, SNode root, ref_t returningType, ref_t& subject)
{
   if (returningType != 0 && scope.defineSubjectSize(returningType) > 0) {
      root = SyntaxTree::findChild(root, lxNewFrame);

      if (tree.matchPattern(root, lxObjectMask, 2,
            SNodePattern(lxExpression),
            SNodePattern(lxReturning))) 
      {
         SNode message = tree.findPattern(root, 2,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling));

         // if it is eval&subject2:var[1] message
         if (getParamCount(message.argument) != 1)
            return false;

         // check if it is operation with $self
         SNode target = tree.findPattern(root, 3,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxThisLocal));

         //// if the target was optimized
         //if (target == lxExpression) {
         //   target = SyntaxTree::findChild(target, lxLocal);
         //}

         if (target == lxNone || target.argument != 1)
            return false;

         // check if the argument is returned
         SNode arg = tree.findPattern(root, 4,
            SNodePattern(lxExpression),
            SNodePattern(lxDirectCalling, lxSDirctCalling),
            SNodePattern(lxExpression),
            SNodePattern(lxLocalAddress));

         if (arg == lxNone) {
            arg = tree.findPattern(root, 5,
               SNodePattern(lxExpression),
               SNodePattern(lxDirectCalling, lxSDirctCalling),
               SNodePattern(lxExpression),
               SNodePattern(lxExpression),
               SNodePattern(lxLocalAddress));
         }

         SNode ret = tree.findPattern(root, 3,
            SNodePattern(lxReturning),
            SNodePattern(lxBoxing),
            SNodePattern(lxLocalAddress));

         if (arg != lxNone && ret != lxNone && arg.argument == ret.argument) {
            subject = getSignature(message.argument);

            return true;
         }
      }
   }

   return false;
}

bool Compiler :: recognizeEmbeddableIdle(SyntaxTree& tree, SNode methodNode)
{
   SNode object = tree.findPattern(methodNode, 4,
      SNodePattern(lxNewFrame),
      SNodePattern(lxReturning),
      SNodePattern(lxExpression),
      SNodePattern(lxLocal));

   if (object == lxNone) {
      object = tree.findPattern(methodNode, 3,
         SNodePattern(lxNewFrame),
         SNodePattern(lxReturning),
         SNodePattern(lxLocal));
   }

   return (object == lxLocal && object.argument == -1);
}

void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
{
   // Optimization : var = get&subject => eval&subject2:var[1]
   ref_t type = 0;
   ref_t returnType = classScope.info.methodHints.get(ClassInfo::Attribute(methodNode.argument, maType));
   if (recognizeEmbeddableGet(*classScope.moduleScope, *methodNode.Tree(), methodNode, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGet), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }

   // Optimization : subject'get = self
   if (recognizeEmbeddableIdle(*methodNode.Tree(), methodNode)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableIdle), -1);
   }
}

void Compiler :: compileIncludeModule(DNode node, ModuleScope& scope, DNode hints)
{
   if (hints != nsNone)
      scope.raiseWarning(1, wrnUnknownHint, hints.Terminal());

   TerminalInfo ns = node.Terminal();

   // check if the module exists
   _Module* module = scope.project->loadModule(ns, true);
   if (!module)
      scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, ns);

   ident_t value = retrieve(scope.defaultNs.start(), ns, NULL);
   if (value == NULL) {
      scope.defaultNs.add(ns.value);

      scope.loadModuleInfo(module);
   }
}

void Compiler :: declareSubject(DNode& member, ModuleScope& scope, DNode hints)
{
   bool internalSubject = member.Terminal().symbol == tsPrivate;

   // map a full type name
   ref_t subjRef = scope.mapNewSubject(member.Terminal());
   ref_t classRef = 0;

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }

   DNode body = goToSymbol(member.firstChild(), nsForward);
   if (body != nsNone) {
      TerminalInfo terminal = body.Terminal();
      
      DNode option = body.firstChild();
      if (option != nsNone) {
         ref_t hintRef = mapHint(body, scope);
         if (!hintRef)
            scope.raiseError(errInvalidHint, terminal);

         ReferenceNs fulName(scope.module->Name(), scope.module->resolveSubject(hintRef));
         while (option != nsNone) {
            ref_t optionRef = scope.mapSubject(option.Terminal());
            fulName.append('@');
            fulName.append(scope.module->resolveSubject(optionRef));

            option = option.nextNode();
         }

         classRef = scope.module->mapReference(fulName);
      }
      else {
         classRef = scope.mapTerminal(terminal);
         if (classRef == 0)
            scope.raiseError(errUnknownSubject, terminal);
      }
   }

   scope.saveSubject(subjRef, classRef, internalSubject);
}

void Compiler :: compileSubject(DNode& member, ModuleScope& scope, DNode hints)
{
   DNode body = goToSymbol(member.firstChild(), nsForward);
   DNode option = body.firstChild();

   if (option != nsNone) {
      TemplateInfo templateInfo;
      templateInfo.templateRef = mapHint(body, scope);
      templateInfo.targetType = scope.mapSubject(member.Terminal());
      templateInfo.sourceCol = body.FirstTerminal().Col();
      templateInfo.sourceRow = body.FirstTerminal().Row();

      declareTemplateParameters(body, scope, templateInfo.parameters);

      ref_t classRef = scope.subjectHints.get(templateInfo.targetType);

      classRef = generateTemplate(scope, templateInfo, classRef);
      if (classRef == 0)
         scope.raiseError(errInvalidHint, body.Terminal());
   }
}

void Compiler::compileDeclarations(DNode member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      TerminalInfo name = member.Terminal();

      switch (member) {
         case nsSubject:
            declareSubject(member, scope, hints);
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
         case nsTemplate:
         case nsFieldTemplate:
         case nsMethodTemplate:
         {
            ref_t templateRef = 0;
            int count = countSymbol(member, nsMethodParameter);
            if (count != 0) {
               IdentifierString templateName(name);
               templateName.append('#');
               templateName.appendInt(count);

               templateRef = scope.mapNewSubject(templateName);
            }
            else templateRef = scope.mapNewSubject(name);

            // check for duplicate declaration
            if (scope.module->mapSection(templateRef | mskSyntaxTreeRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(templateRef | mskSyntaxTreeRef, false);

            TemplateScope classScope(&scope, templateRef);
            classScope.type = TemplateScope::ttClass;
            if (member == nsFieldTemplate) {
               classScope.type = TemplateScope::ttField;
            }
            else if (member == nsMethodTemplate) {
               classScope.type = TemplateScope::ttMethod;
            }

            // compile class
            compileTemplateDeclaration(member, classScope, hints);

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

void Compiler :: compileImplementations(DNode member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      TerminalInfo name = member.Terminal();

      switch (member) {
         case nsSubject:
            compileSubject(member, scope, hints);
            break;
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
            compileIncludeModule(member, scope, hints);
            break;
         case nsImport:
            compileIncludeModule(member, scope, hints);
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
         // HOTFIX : the module path should be presaved
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

   _writer.clearImportInfo();

   return !project.HasWarnings();
}
