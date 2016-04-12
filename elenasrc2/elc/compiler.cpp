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
#define HINT_EXTENSION_MODE   0x04000000
//#define HINT_ACTION           0x00020000
//#define HINT_ALTBOXING        0x00010000
//#define HINT_CLOSURE          0x00008000

typedef Compiler::ObjectInfo ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef Compiler::ObjectKind ObjectKind;
typedef ClassInfo::Attribute Attribute;

// --- Auxiliary routines ---

//inline bool isCollection(DNode node)
//{
//   return (node == nsExpression && node.nextNode()==nsExpression);
//}

inline bool isReturnExpression(DNode expr)
{
   return ((expr == nsExpression || expr == nsRootExpression) && expr.nextNode() == nsNone);
}

inline bool isSingleStatement(DNode expr)
{
   return (expr == nsExpression) && (expr.firstChild().nextNode() == nsNone);
}

//inline bool isSingleObject(DNode expr)
//{
//   return (expr == nsObject) && (expr.firstChild().nextNode() == nsNone);
//}

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
         if (StringHelper::compare(terminal.value, INTERNAL_MASK, INTERNAL_MASK_LEN))
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

//inline bool IsCompOperator(int operator_id)
//{
//   switch(operator_id) {
//      case EQUAL_MESSAGE_ID:
//      case NOTEQUAL_MESSAGE_ID:
//      case LESS_MESSAGE_ID:
//      case NOTLESS_MESSAGE_ID:
//      case GREATER_MESSAGE_ID:
//      case NOTGREATER_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}

inline bool IsReferOperator(int operator_id)
{
   return operator_id == REFER_MESSAGE_ID || operator_id == SET_REFER_MESSAGE_ID;
}

inline bool IsDoubleOperator(int operator_id)
{
   return operator_id == SET_REFER_MESSAGE_ID;
}

//inline bool IsInvertedOperator(int& operator_id)
//{
//   if (operator_id == NOTEQUAL_MESSAGE_ID) {
//      operator_id = EQUAL_MESSAGE_ID;
//
//      return true;
//   }
//   else if (operator_id == NOTLESS_MESSAGE_ID) {
//      operator_id = LESS_MESSAGE_ID;
//
//      return true;
//   }
//   else if (operator_id == NOTGREATER_MESSAGE_ID) {
//      operator_id = GREATER_MESSAGE_ID;
//
//      return true;
//   }
//   else return false;
//}

inline bool isPrimitiveRef(ref_t reference)
{
   return (int)reference < 0;
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

//inline bool isArrayPrimitive(int flags)
//{
//   switch (flags & elDebugMask)
//   {
//      case elDebugIntegers:
//      case elDebugArray:
//      case elDebugBytes:
//      case elDebugShorts:
//         return true;
//      default:
//         return false;
//   }
//}

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
      newSignature.copy(signature, index);
      if (info.targetSubject != 0) {
         newSignature.append(dest->resolveSubject(info.targetSubject));
      }
      newSignature.append(signature + index + getlength(TARGET_POSTFIX));

      return dest->mapSubject(newSignature, false);
   }
   else return importSubject(sour, sign_ref, dest);
}

//inline SNode getFirstObject(SNode node)
//{
//   SNode child = SyntaxTree::findMatchedChild(node, lxObjectMask);
//   if (child == lxExpression) {
//      child = SyntaxTree::findMatchedChild(child, lxObjectMask);
//   }
//
//   return child;
//}

// --- Compiler::ModuleScope ---

Compiler::ModuleScope::ModuleScope(Project* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints((ref_t)-1)/*, extensions(NULL, freeobj)*/, templates(TemplateInfo())
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
   //charReference = mapReference(project->resolveForward(CHAR_FORWARD));
   //signatureReference = mapReference(project->resolveForward(SIGNATURE_FORWARD));
   //messageReference = mapReference(project->resolveForward(MESSAGE_FORWARD));
   //verbReference = mapReference(project->resolveForward(VERB_FORWARD));
   //paramsReference = mapReference(project->resolveForward(PARAMS_FORWARD));
   //trueReference = mapReference(project->resolveForward(TRUE_FORWARD));
   //falseReference = mapReference(project->resolveForward(FALSE_FORWARD));
   //arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));

   // cache the frequently used subjects / hints
   sealedHint = module->mapSubject(HINT_SEALED, false);
   integerHint = module->mapSubject(HINT_INTEGER_NUMBER, false);
   realHint = module->mapSubject(HINT_FLOAT_NUMBER, false);
   literalHint = module->mapSubject(HINT_STRING, false);
   varHint = module->mapSubject(HINT_VARIABLE, false);
   limitedHint = module->mapSubject(HINT_LIMITED, false);
   signHint = module->mapSubject(HINT_SIGNATURE, false);
   stackHint = module->mapSubject(HINT_STACKSAFE, false);
   warnHint = module->mapSubject(HINT_SUPPRESS_WARNINGS, false);
   dynamicHint = module->mapSubject(HINT_DYNAMIC, false);
   constHint = module->mapSubject(HINT_CONSTANT, false);
   structHint = module->mapSubject(HINT_STRUCT, false);
   embedHint = module->mapSubject(HINT_EMBEDDABLE, false);
   //boolType = module->mapSubject(project->resolveForward(BOOLTYPE_FORWARD), false);

   defaultNs.add(module->Name());

   loadModuleInfo(module);
}

////ref_t Compiler::ModuleScope :: getBaseFunctionClass(int paramCount)
////{
////   if (paramCount == 0) {
////      return mapReference(project->resolveForward(FUNCX_FORWARD));
////   }
////   else {
////      IdentifierString className(project->resolveForward(FUNCX_FORWARD));
////      className.appendInt(paramCount);
////
////      return mapReference(className);
////   }
////}
////
////ref_t Compiler::ModuleScope :: getBaseIndexFunctionClass(int paramCount)
////{
////   if (paramCount > 0) {
////      IdentifierString className(project->resolveForward(NFUNCX_FORWARD));
////      className.appendInt(paramCount);
////
////      return mapReference(className);
////   }
////   else return 0;
////}
////
////ref_t Compiler::ModuleScope :: getBaseLazyExpressionClass()
////{
////   return mapReference(project->resolveForward(LAZYEXPR_FORWARD));
////}

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

ref_t Compiler::ModuleScope :: mapSubject(TerminalInfo terminal, bool implicitOnly)
{
   ident_t identifier = NULL;
   if (terminal.symbol == tsIdentifier || terminal.symbol == tsPrivate) {
      identifier = terminal.value;
   }
   else raiseError(errInvalidSubject, terminal);

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
      if (subj_ref && (!implicitOnly || subjectHints.exist(subj_ref))) {
         subjects.add(terminal, subj_ref);

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

int Compiler::ModuleScope :: defineStructSize(ref_t classReference, bool embeddableOnly)
{
   ClassInfo classInfo;
   if(loadClassInfo(classInfo, module->resolveReference(classReference), true) == 0)
      return 0;

   if (!embeddableOnly && test(classInfo.header.flags, elStructureRole)) {
      return classInfo.size;
   }
   else if (isEmbeddable(classInfo)) {
      //   variable = !test(classInfo.header.flags, elReadOnlyRole);

      return classInfo.size;
   }

   return 0;
}

int Compiler::ModuleScope :: defineSubjectSize(ref_t type_ref, bool embeddableOnly /*, ref_t& classReference, bool& variable*/)
{
   if (type_ref == 0)
      return 0;

   ref_t classReference = subjectHints.get(type_ref);
   if (classReference != 0) {
      return defineStructSize(classReference, embeddableOnly/*, variable*/);
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
   else return tpUnknown;
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

void Compiler::ModuleScope :: loadRoles(_Module* extModule)
{
   if (extModule) {
      ReferenceNs sectionName(extModule->Name(), ROLE_SECTION);
   
      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            int role = metaReader.getDWord();

            ref_t class_ref = importReference(extModule, metaReader.getDWord(), module);
   
            roleHints.add(role, class_ref);
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

//void Compiler::ModuleScope :: loadExtensions(TerminalInfo terminal, _Module* extModule)
//{
//   if (extModule) {
//      ReferenceNs sectionName(extModule->Name(), EXTENSION_SECTION);
//
//      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
//      if (section) {
//         MemoryReader metaReader(section);
//         while (!metaReader.Eof()) {
//            ref_t type_ref = importSubject(extModule, metaReader.getDWord(), module);
//            ref_t message = importMessage(extModule, metaReader.getDWord(), module);
//            ref_t role_ref = importReference(extModule, metaReader.getDWord(), module);
//
//            if(!extensionHints.exist(message, type_ref)) {
//               extensionHints.add(message, type_ref);
//
//               SubjectMap* typeExtensions = extensions.get(type_ref);
//               if (!typeExtensions) {
//                  typeExtensions = new SubjectMap();
//
//                  extensions.add(type_ref, typeExtensions);
//               }
//
//               typeExtensions->add(message, role_ref);
//            }
//            else raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, terminal);
//         }
//      }
//   }
//}

void Compiler::ModuleScope :: saveSubject(ref_t type_ref, ref_t classReference, bool internalType)
{
   if (!internalType) {
      ReferenceNs sectionName(module->Name(), TYPE_SECTION);

      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(type_ref);
      metaWriter.writeDWord(classReference);

      if (classReference != 0)
         typifiedClasses.add(classReference, type_ref);
   }

   subjectHints.add(type_ref, classReference, true);
}

void Compiler::ModuleScope :: saveTemplate(ref_t template_ref)
{
   ReferenceNs sectionName(module->Name(), TYPE_SECTION);

   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

   metaWriter.writeDWord(template_ref);
   metaWriter.writeDWord(-1); // -1 indicates that it is a template declaration
}

//bool Compiler::ModuleScope :: saveExtension(ref_t message, ref_t type, ref_t role)
//{
//   ReferenceNs sectionName(module->Name(), EXTENSION_SECTION);
//
//   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
//
//   metaWriter.writeDWord(type);
//   metaWriter.writeDWord(message);
//   metaWriter.writeDWord(role);
//
//   if (!extensionHints.exist(message, type)) {
//      extensionHints.add(message, type);
//
//      SubjectMap* typeExtensions = extensions.get(type);
//      if (!typeExtensions) {
//         typeExtensions = new SubjectMap();
//
//         extensions.add(type, typeExtensions);
//      }
//
//      typeExtensions->add(message, role);
//
//      return true;
//   }
//   else return false;
//}

void Compiler::ModuleScope :: saveRole(int role, ref_t reference)
{
   ReferenceNs sectionName(module->Name(), ROLE_SECTION);
   
   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
   
   metaWriter.writeDWord(role);
   metaWriter.writeDWord(reference);

   roleHints.add(role, reference);
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
//   typeRef = 0;
   constant = false;

   syntaxTree.writeString(parent->sourcePath);
}

void Compiler::SymbolScope :: compileHints(DNode hints)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      ref_t hintRef = moduleScope->mapSubject(terminal, false);
      if (hintRef == moduleScope->constHint) {
         constant = true;
      }
      //else if (StringHelper::compare(terminal, HINT_TYPE)) {
      //   DNode value = hints.select(nsHintValue);
      //   TerminalInfo typeTerminal = value.Terminal();

      //   typeRef = moduleScope->mapType(typeTerminal);
      //   if (typeRef == 0)
      //      raiseError(wrnInvalidHint, terminal);
      //}
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

   syntaxTree.writeString(parent->sourcePath);
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

void Compiler::ClassScope :: compileClassHint(SNode hint)
{
   switch (hint.type)
   {
      case lxClassFlag:
         info.header.flags |= hint.argument;
         break;
      case lxClassStructure:
      //case lxClassArray:
      {
         if (testany(info.header.flags, elStructureRole | elNonStructureRole | elWrapper))
            raiseError(wrnInvalidHint, hint);

         if (hint.type == lxClassStructure) {
            info.size = hint.argument;

            SNode typeNode = SyntaxTree::findChild(hint, lxType);
            if (typeNode.argument != 0)
               info.fieldTypes.add(-1, typeNode.argument);
         }
         break;
      }
   }
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved = 0;
   this->rootToFree = 1;
//   this->withOpenArg = false;
   this->stackSafe = false;
//   this->embeddable = false;
//   this->generic = false;

   //NOTE : tape has to be overridden in the constructor
   this->tape = &parent->tape;
}

ObjectInfo Compiler::MethodScope :: mapObject(TerminalInfo identifier)
{
   if (StringHelper::compare(identifier, THIS_VAR)) {
      return ObjectInfo(okThisParam, 1, stackSafe ? -1 : 0);
   }
   //else if (StringHelper::compare(identifier, METHOD_SELF_VAR)) {
   //   return ObjectInfo(okParam, (size_t)-1);
   //}
   else {
      Parameter param = parameters.get(identifier);

      int local = param.offset;
      if (local >= 0) {
   //      //if (withOpenArg && moduleScope->typeHints.exist(param.sign_ref, moduleScope->paramsReference)) {
   //      //   return ObjectInfo(okParams, -1 - local, 0, param.sign_ref);
   //      //}
         /*else */if (stackSafe && param.subj_ref != 0) {
            // HOTFIX : only embeddable parameter should be boxed in stacksafe method
            if (isEmbeddable(moduleScope->getClassFlags(moduleScope->subjectHints.get(param.subj_ref)))) {
               return ObjectInfo(okParam, -1 - local, -1, param.subj_ref);
            }
         }
         return ObjectInfo(okParam, -1 - local, 0, param.subj_ref);
      }
      else {
         ObjectInfo retVal = Scope::mapObject(identifier);

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
   //// HOTFIX : self / $self : closure should refer to the owner ones
   //if (StringHelper::compare(identifier, THIS_VAR)) {
   //   return parent->mapObject(identifier);
   //}
   /*else */return MethodScope::mapObject(identifier);
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
      //if (StringHelper::compare(identifier, SUBJECT_VAR)) {
      //   return ObjectInfo(okSubject, local.offset);
      //}
      /*else */if (local.size != 0) {
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
}

////Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
////{
////   String<ident_c, 10> thisVar(THIS_VAR);
////
////   Outer owner = outers.get(thisVar);
////   // if owner reference is not yet mapped, add it
////   if (owner.outerObject.kind == okUnknown) {
////      owner.reference = info.fields.Count();
////      owner.outerObject.kind = okThisParam;
////      owner.outerObject.param = 1;
////
////      outers.add(thisVar, owner);
////      mapKey(info.fields, thisVar, owner.reference);
////   }
////   return owner;
////}
//
////ObjectInfo Compiler::InlineClassScope :: mapObject(TerminalInfo identifier)
////{
////   //if (StringHelper::compare(identifier, THIS_VAR) || StringHelper::compare(identifier, OWNER_VAR)) {
////   //   Outer owner = mapSelf();
////
////   //   // map as an outer field (reference to outer object and outer object field index)
////   //   return ObjectInfo(okOuter, owner.reference);
////   //}
////   //else {
////      Outer outer = outers.get(identifier);
////
////   //   // if object already mapped
////   //   if (outer.reference != -1) {
////   //      if (outer.outerObject.kind == okSuper) {
////   //         return ObjectInfo(okSuper, outer.reference);
////   //      }
////   //      else return ObjectInfo(okOuter, outer.reference, 0, outer.outerObject.type);
////   //   }
////   //   else {
////         outer.outerObject = parent->mapObject(identifier);
////   //      // handle outer fields in a special way: save only self
////   //      if (outer.outerObject.kind == okField) {
////   //         Outer owner = mapSelf();
////
////   //         // save the outer field type if provided
////   //         if (outer.outerObject.extraparam != 0) {
////   //            outerFieldTypes.add(outer.outerObject.param, outer.outerObject.extraparam, true);
////   //         }
////
////   //         // map as an outer field (reference to outer object and outer object field index)
////   //         return ObjectInfo(okOuterField, owner.reference, outer.outerObject.param, outer.outerObject.type);
////   //      }
////   //      // map if the object is outer one
////   //      else if (outer.outerObject.kind == okParam || outer.outerObject.kind == okLocal || outer.outerObject.kind == okField
////   //         || outer.outerObject.kind == okOuter || outer.outerObject.kind == okSuper || outer.outerObject.kind == okThisParam
////   //         || outer.outerObject.kind == okOuterField || outer.outerObject.kind == okLocalAddress)
////   //      {
////   //         outer.reference = info.fields.Count();
////
////   //         outers.add(identifier, outer);
////   //         mapKey(info.fields, identifier.value, outer.reference);
////
////   //         return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam, outer.outerObject.type);
////   //      }
////   //      else if (outer.outerObject.kind == okUnknown) {
////   //         // check if there is inherited fields
////   //         outer.reference = info.fields.get(identifier);
////   //         if (outer.reference != -1) {
////   //            return ObjectInfo(okField, outer.reference);
////   //         }
////   //         else return outer.outerObject;
////   //      }
////         /*else */return outer.outerObject;
////   //   }
////   //}
////}

// --- Compiler::TemplateScope ---

Compiler::TemplateScope :: TemplateScope(ModuleScope* parent, ref_t reference)
   : ClassScope(parent, 0)
{
   // NOTE : reference is defined in subject namespace, so templateRef should be initialized and used
   // proper reference is 0 in this case
   templateRef = reference;

   templateType = lxNone;

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
   if (StringHelper::compare(identifier, TARGET_VAR)) {
      return ObjectInfo(okTemplateTarget);
   }
   else return ClassScope::mapObject(identifier);
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

bool Compiler :: checkIfCompatible(ModuleScope& scope, ref_t typeRef, SyntaxTree::Node node)
{
   ref_t nodeType = SyntaxTree::findChild(node, lxType).argument;   
   ref_t nodeRef = SyntaxTree::findChild(node, lxTarget).argument;

   if (nodeType == typeRef) {
      return true;
   }
   else if (isPrimitiveRef(nodeRef)) {
      return false;
   }
   else if (node == lxNil) {
      return true;
   }
   else if (node == lxConstantInt) {
      int flags = scope.getClassFlags(nodeRef);

      return (flags & elDebugMask) == elDebugDWORD;
   }
   else if (node == lxConstantReal) {
      int flags = scope.getClassFlags(nodeRef);

      return (flags & elDebugMask) == elDebugReal64;
   }
   else if (node == lxConstantLong) {
      int flags = scope.getClassFlags(nodeRef);

      return (flags & elDebugMask) == elDebugQWORD;
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
      //case okCharConstant:
      //   return scope.moduleScope->charReference;
      case okThisParam:
         return scope.getClassRefId(false);
      //case okSubject:
      //case okSignatureConstant:
      //   return scope.moduleScope->signatureReference;
      case okSuper:
         return object.param;
      //case okParams:
      //   return scope.moduleScope->paramsReference;
      case okExternal:
         return -1; // NOTE : -1 means primitve int32
      //case okMessageConstant:
      //   return scope.moduleScope->messageReference;
      default:
         if (object.kind == okObject && object.param != 0) {
            return object.param;
         }
         else if (object.kind == okLocal && object.extraparam > 0) {
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
      //if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->paramsReference)) {
      //   writer.newNode(lxParamsVariable);
      //   writer.appendNode(lxTerminal, it.key());
      //   writer.appendNode(lxLevel, -1 - (*it).offset);
      //   writer.closeNode();
      //}
      /*else */if (scope.moduleScope->subjectHints.exist((*it).subj_ref, moduleScope->intReference)) {
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
      //else if (scope.moduleScope->typeHints.exist((*it).sign_ref, moduleScope->realReference)) {
      //   writer.newNode(lxReal64Variable);
      //   writer.appendNode(lxTerminal, it.key());
      //   writer.appendNode(lxLevel, -1 - (*it).offset);
      //   writer.appendNode(lxFrameAttr);
      //   writer.closeNode();
      //}
      else {
         writer.newNode(lxVariable, -1);
         writer.appendNode(lxTerminal, it.key());
         writer.appendNode(lxLevel, -1 - (*it).offset);
         writer.appendNode(lxFrameAttr);
         writer.closeNode();
      }

      it++;
   }
   if (withThis)
      writer.appendNode(lxSelfVariable, 1);

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

bool Compiler :: compileClassHint(DNode hint, SyntaxWriter& writer, ClassScope& scope, bool directiveOnly)
{
   ModuleScope* moduleScope = scope.moduleScope;

   TerminalInfo terminal = hint.Terminal();

   // if it is a class modifier
   ref_t hintRef = moduleScope->mapSubject(terminal, false);
   if (hintRef == moduleScope->sealedHint) {
      writer.appendNode(lxClassFlag, elSealed);

      return true;
   }
   else if (hintRef == moduleScope->integerHint) {
      TerminalInfo sizeValue = hint.select(nsHintValue).Terminal();
      if (sizeValue.symbol == tsInteger) {
         int size = StringHelper::strToInt(sizeValue.value);
         writer.newNode(lxClassStructure, size);
         appendTerminalInfo(&writer, terminal);

         // !! HOTFIX : allow only 1,2,4 or 8
         if (size == 1 || size == 2 || size == 4) {
            writer.appendNode(lxClassFlag, elDebugDWORD);
         }
         else if (size == 8) {
            writer.appendNode(lxClassFlag, elDebugQWORD);
         }
         else return false;

         writer.appendNode(lxClassFlag, elReadOnlyRole);
         writer.appendNode(lxClassFlag, elEmbeddable | elStructureRole);

         writer.closeNode();

         return true;
      }
   }
   else if (hintRef == moduleScope->realHint) {
      TerminalInfo sizeValue = hint.select(nsHintValue).Terminal();
      if (sizeValue.symbol == tsInteger) {
         int size = StringHelper::strToInt(sizeValue.value);

         writer.newNode(lxClassStructure, size);
         appendTerminalInfo(&writer, terminal);
         writer.closeNode();

         // !! HOTFIX : allow only 8
         if (size == 8) {
            writer.appendNode(lxClassFlag, elDebugReal64);
         }
         else return false;

         writer.appendNode(lxClassFlag, elEmbeddable | elStructureRole | elReadOnlyRole);

         return true;
      }
   }
   else if (hintRef == moduleScope->varHint) {
      writer.appendNode(lxClassFlag, elWrapper);

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
   else if (hintRef == moduleScope->literalHint) {
      writer.appendNode(lxClassFlag, elDebugLiteral); // NOTE : template importer should tweak it depending on character size
      writer.appendNode(lxClassFlag, elStructureRole | elDynamicRole);

      return true;
   }
   else if (hintRef == moduleScope->constHint) {
      writer.appendNode(lxClassFlag, elReadOnlyRole);

      return true;
   }
   //else if (hintRef == moduleScope->signHint) {
   //   writer.newNode(lxClassStructure, 4);

   //   appendTerminalInfo(&writer, terminal);
   //   writer.appendNode(lxClassFlag, elDebugSubject);
   //   writer.appendNode(lxClassFlag, elStructureRole | elSignature | elEmbeddable | elReadOnlyRole);

   //   writer.closeNode();

   //   return true;
   //}
   else if (hintRef == moduleScope->structHint) {
      writer.appendNode(lxClassFlag, elStructureRole);

      DNode option = hint.select(nsHintValue);
      if (option != nsNone) {
         ref_t optionRef = moduleScope->mapSubject(option.Terminal(), false);
         if (optionRef == moduleScope->embedHint) {
            writer.appendNode(lxClassFlag, elEmbeddable);

            return true;
         }
         else scope.raiseError(wrnInvalidHint, terminal);
      }
      else return true;
   }
   else if (!directiveOnly) {
      ref_t hintRef = scope.moduleScope->mapSubject(terminal, false);
      if (hintRef != 0) {
         TemplateInfo templateInfo;
         templateInfo.templateRef = hintRef;

         TerminalInfo target = hint.firstChild().Terminal();
         if (!emptystr(target)) {
            templateInfo.targetSubject = scope.moduleScope->mapSubject(target);
            if (templateInfo.targetSubject == 0)
               templateInfo.targetSubject = scope.moduleScope->module->mapSubject(target, false);

            // HOTFIX : if it is a typified class template - set targetType
            if (scope.moduleScope->subjectHints.get(templateInfo.targetSubject) != 0) {
               templateInfo.targetType = templateInfo.targetSubject;
            }
         }

         scope.moduleScope->templates.add(scope.reference, templateInfo);

         return true;
      }
   }

   return false;
}

void Compiler :: compileClassHints(DNode hints, SyntaxWriter& writer, ClassScope& scope/*, bool& isExtension, ref_t& extensionType*/)
{
   // define class flags
   while (hints == nsHint) {
      if (!compileClassHint(hints, writer, scope, false))
         scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());

      //if (StringHelper::compare(terminal, HINT_GROUP)) {
      //   writer.appendNode(lxClassFlag, elGroup);
      //}
      //else if (StringHelper::compare(terminal, HINT_MESSAGE)) {
      //   writer.newNode(lxClassStructure, 4);
      //   appendTerminalInfo(&writer, terminal);

      //   writer.appendNode(lxClassFlag, elDebugDWORD);
      //   writer.appendNode(lxClassFlag, elStructureRole | elMessage | elEmbeddable | elReadOnlyRole);

      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_EXT_MESSAGE)) {
      //   writer.newNode(lxClassStructure, 8);

      //   appendTerminalInfo(&writer, terminal);
      //   writer.appendNode(lxClassFlag, elStructureRole | elExtMessage | elReadOnlyRole);

      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_SYMBOL)) {
      //   writer.newNode(lxClassStructure, 4);

      //   appendTerminalInfo(&writer, terminal);
      //   writer.appendNode(lxClassFlag, elDebugReference);
      //   writer.appendNode(lxClassFlag, elStructureRole | elSymbol | elReadOnlyRole);

      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_POINTER)) {
      //   writer.newNode(lxClassStructure, 4);

      //   appendTerminalInfo(&writer, terminal);
      //   writer.appendNode(lxClassFlag, elDebugPTR);
      //   writer.appendNode(lxClassFlag, elEmbeddable | elStructureRole);

      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_BINARY)) {
      //   TerminalInfo sizeValue = hints.select(nsHintValue).Terminal();
      //   if (sizeValue.symbol == tsIdentifier) {
      //      DNode value = hints.select(nsHintValue);
      //      size_t type = scope.moduleScope->mapType(value.Terminal());
      //      if (type == 0)
      //         scope.raiseError(errUnknownSubject, value.Terminal());

      //      int size = -scope.moduleScope->defineTypeSize(type);
      //      writer.newNode(lxClassStructure, size);
      //      appendTerminalInfo(&writer, terminal);

      //      writer.appendNode(lxType, type);

      //      writer.closeNode();
      //   }
      //   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      //   writer.appendNode(lxClassFlag, elEmbeddable | elStructureRole | elDynamicRole);
      //}
      //else if (StringHelper::compare(terminal, HINT_NONSTRUCTURE)) {
      //   writer.appendNode(lxClassFlag, elNonStructureRole);
      //}
      //else if (StringHelper::compare(terminal, HINT_XDYNAMIC)) {
      //   writer.newNode(lxClassArray);

      //   appendTerminalInfo(&writer, terminal);
      //   writer.appendNode(lxClassFlag, elDynamicRole);
      //   writer.appendNode(lxClassFlag, elDebugArray);

      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_DYNAMIC)) {
      //   writer.newNode(lxClassStructure);
      //   appendTerminalInfo(&writer, terminal);

      //   writer.appendNode(lxClassFlag, elDynamicRole);
      //   writer.appendNode(lxClassFlag, elDebugArray);
      //   DNode value = hints.select(nsHintValue);
      //   if (value != nsNone) {
      //      size_t type = scope.moduleScope->mapType(value.Terminal());
      //      if (type == 0)
      //         scope.raiseError(errUnknownSubject, value.Terminal());

      //      writer.appendNode(lxType, type);
      //   }
      //   writer.closeNode();
      //}
      //else if (StringHelper::compare(terminal, HINT_EXTENSION)) {
      //   DNode value = hints.select(nsHintValue);
      //   if (value != nsNone) {
      //      extensionType = scope.moduleScope->mapType(value.Terminal());
      //      if (extensionType == 0)
      //         scope.raiseError(errUnknownSubject, value.Terminal());
      //   }
      //   isExtension = true;
      //   writer.appendNode(lxClassFlag, elExtension);
      //   writer.appendNode(lxClassFlag, elSealed);    // extension should be sealed
      //}

      hints = hints.nextNode();
   }
}

void Compiler :: compileTemplateHints(DNode hints, SyntaxWriter& writer, TemplateScope& scope)
{
   // define class flags
   while (hints == nsHint) {
      if (!compileClassHint(hints, writer, scope, true))
         scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, hints.Terminal());

//      //if (StringHelper::compare(terminal, HINT_TARGET)) {
//      //   TerminalInfo target = hints.select(nsHintValue).Terminal();
//      //   if (StringHelper::compare(target.value, HINT_TARGET_FIELD)) {
//      //      scope.templateType = lxFieldTemplate;
//      //   }
//      //   else if (StringHelper::compare(target.value, HINT_TARGET_METHOD)) {
//      //      scope.templateType = lxMethodTemplate;
//      //   }
//      //   else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
//      //}
//      /*else */scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

void Compiler :: compileFieldHints(DNode hints, SyntaxWriter& writer, ClassScope& scope)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      ref_t hintRef = scope.moduleScope->mapSubject(terminal, false);
      if (hintRef != 0) {
         if (scope.moduleScope->subjectHints.exist(hintRef)) {
      //      if (type == 0 && classRef == 0) {
      //         type = hintRef;

            TerminalInfo target = hints.firstChild().Terminal();
            if (target != nsNone) {
      //            if (target.symbol == tsInteger) {
      //               size = StringHelper::strToInt(target);

      //               classRef = -3; // NOTE : -3 means an array of type
      //            }
               /*else */scope.raiseError(errInvalidHint, terminal);
            }
            else {
               writer.appendNode(lxType, hintRef);

      //            classRef = scope.moduleScope->subjectHints.get(type);

      //            int flags = scope.moduleScope->getClassFlags(classRef);
      //            //HOTFIX : recognize int wrapper as primitive value
      //            switch (flags & elDebugMask) {
      //            case elDebugDWORD:
      //               classRef = -1; // NOTE : -1 means primitive int32
      //               break;
      //            case elDebugQWORD:
      //               classRef = -2; // NOTE : -2 means primitive int64
      //               break;
      //            case elDebugReal64:
      //               classRef = -4; // NOTE : -4 means primitive real64
      //               break;
      //            }
      //         }
            }
      //      else scope.raiseError(errInvalidHint, terminal);
         }
         else {
      //      if (type != 0)
      //         scope.raiseError(errInvalidHint, terminal);

      //      TemplateInfo templateInfo;
      //      templateInfo.templateRef = hintRef;

      //      TerminalInfo target = hints.firstChild().Terminal();
      //      templateInfo.targetType = scope.moduleScope->mapSubject(target);
      //      templateInfo.targetSubject = templateInfo.targetType;
      //      if (templateInfo.targetSubject == 0)
      //         templateInfo.targetSubject = scope.moduleScope->module->mapSubject(target, false);

      //      classRef = generateTemplate(*scope.moduleScope, templateInfo, 0);
      //      if (classRef == 0)
               scope.raiseError(errInvalidHint, terminal);
         }
      }
      //if (StringHelper::compare(terminal, HINT_TYPE)) {
      //   DNode value = hints.select(nsHintValue);
      //   if (value!=nsNone) {
      //      TerminalInfo typeTerminal = value.Terminal();

      //      ref_t type = scope.moduleScope->mapType(typeTerminal);
      //      if (type == 0)
      //         scope.raiseError(errInvalidHint, terminal);

      //      
      //   }
      //   else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
      //}
      //else if (StringHelper::compare(terminal, HINT_SIZE)) {
      //   TerminalInfo sizeValue = hints.firstChild().Terminal();
      //   if (sizeValue.symbol == tsInteger) {
      //      writer.appendNode(lxSize, StringHelper::strToInt(sizeValue.value));
      //   }
      //   else if (sizeValue.symbol == tsHexInteger) {
      //      writer.appendNode(lxSize, StringHelper::strToLong(sizeValue.value, 16));
      //   }
      //   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      //}
      //else {
      //   ref_t templateRef = scope.moduleScope->resolveIdentifier(terminal);
      //   _Module* dummy;
      //   _Memory* section = scope.moduleScope->loadTemplateInfo(templateRef, dummy);
      //   if (section != NULL) {
      //      SyntaxTree tree(section);
      //      SNode node = tree.readRoot();
      //      if (node == lxFieldTemplate) {
      //         writer.newNode(lxFieldTemplate, templateRef);

      //         TerminalInfo typeTerminal = hints.firstChild().Terminal();
      //         writer.appendNode(lxSubject, scope.moduleScope->mapSubject(typeTerminal));

      //         writer.closeNode();
      //      }
      //      else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      //   }
      else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      //}

      hints = hints.nextNode();
   }
}

void Compiler :: compileMethodHints(DNode hints, SyntaxWriter& writer, MethodScope& scope, bool warningsOnly)
{
   ModuleScope* moduleScope = scope.moduleScope;
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      ref_t hintRef = moduleScope->mapSubject(terminal, false);

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
      else if (hintRef == moduleScope->sealedHint) {
         writer.appendNode(lxClassMethodAttr, tpSealed);
      }
      else if (hintRef == moduleScope->stackHint) {
         writer.appendNode(lxClassMethodAttr, tpStackSafe);
      }
      //else {
      ////   ref_t templateRef = scope.moduleScope->resolveIdentifier(terminal);
      ////   _Module* extModule;
      ////   _Memory* section = scope.moduleScope->loadTemplateInfo(templateRef, extModule);
      ////   if (section != NULL) {
      ////      SyntaxTree tree(section);
      ////      SNode node = tree.readRoot();
      ////      if (node == lxMethodTemplate) {
      ////         // validate if the template can be applied
      ////         ref_t targetMessage = importMessage(extModule, SyntaxTree::findChild(node, lxMessage).argument, scope.moduleScope->module);

      ////         bool invalid = true;
      ////         if (getParamCount(scope.message) == getParamCount(targetMessage) && getVerb(scope.message) == getVerb(targetMessage)) {
      ////            ident_t sourSign = scope.moduleScope->module->resolveSubject(getSignature(scope.message));
      ////            ident_t targetSign = scope.moduleScope->module->resolveSubject(getSignature(targetMessage));

      ////            int sourLen = getlength(sourSign);
      ////            int targetLen = getlength(targetSign);

      ////            if (sourLen >= targetLen && StringHelper::compare(targetSign, sourSign + sourLen - targetLen))
      ////               invalid = false;
      ////         }

      ////         if (!invalid) {
      ////            writer.newNode(lxMethodTemplate, templateRef);

      ////            TerminalInfo typeTerminal = hints.firstChild().Terminal();
      ////            writer.appendNode(lxSubject, scope.moduleScope->mapSubject(typeTerminal));

      ////            writer.closeNode();
      ////         }
      ////         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
      ////      }
      ////      else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      ////   }
      //}

      ////else if (StringHelper::compare(terminal, HINT_GENERIC)) {
      ////   scope.generic = true;         

      ////   writer.appendNode(lxClassMethodAttr, tpGeneric);
      ////}
      ////else if (StringHelper::compare(terminal, HINT_EMBEDDABLE)) {
      ////   scope.embeddable = true;

      ////   writer.appendNode(lxClassMethodAttr, tpEmbeddable);
      ////   writer.appendNode(lxClassMethodAttr, tpSealed);
      ////}
      else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

void Compiler :: compileLocalHints(DNode hints, CodeScope& scope, ref_t& type, ref_t& classRef, int& size)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();
      ref_t hintRef = scope.moduleScope->mapSubject(terminal, false);
      if (hintRef != 0) {
         if (scope.moduleScope->subjectHints.exist(hintRef)) {
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
               else {
                  classRef = scope.moduleScope->subjectHints.get(type);

                  int flags = scope.moduleScope->getClassFlags(classRef);
                  //HOTFIX : recognize int wrapper as primitive value
                  switch (flags & elDebugMask) {
                     case elDebugDWORD:
                        classRef = -1; // NOTE : -1 means primitive int32
                        break;
                     case elDebugQWORD:
                        classRef = -2; // NOTE : -2 means primitive int64
                        break;
                     case elDebugReal64:
                        classRef = -4; // NOTE : -4 means primitive real64
                        break;
                  }
               }
            }
            else scope.raiseError(errInvalidHint, terminal);
         }
         else {
            if (type != 0)
               scope.raiseError(errInvalidHint, terminal);

            TemplateInfo templateInfo;
            templateInfo.templateRef = hintRef;

            TerminalInfo target = hints.firstChild().Terminal();
            templateInfo.targetType = scope.moduleScope->mapSubject(target);
            templateInfo.targetSubject = templateInfo.targetType;
            if (templateInfo.targetSubject == 0)
               templateInfo.targetSubject = scope.moduleScope->module->mapSubject(target, false);
               
            classRef = generateTemplate(*scope.moduleScope, templateInfo, 0);
            if (classRef == 0)
               scope.raiseError(errInvalidHint, terminal);
         }
      }
      else scope.raiseError(errUnknownSubject, terminal);

   //   //      //if (StringHelper::compare(terminal, HINT_TYPE)) {
   //   //      //   TerminalInfo typeValue = hints.firstChild().Terminal();
   //   //
   //   //
   //   //      //   size = moduleScope->defineTypeSize(type, classReference);
   //   //      //}
   //   //      //else if (StringHelper::compare(terminal, HINT_SIZE)) {
   //   //      //   int itemSize = moduleScope->defineTypeSize(type);
   //   //
   //   //      //   TerminalInfo sizeValue = hints.firstChild().Terminal();
   //   //      //   if (itemSize < 0 && sizeValue.symbol == tsInteger) {
   //   //      //      itemSize = -itemSize;
   //   //
   //   //      //      size = StringHelper::strToInt(sizeValue.value) * itemSize;
   //   //      //   }
   //   //      //   else if (itemSize < 0 && sizeValue.symbol == tsHexInteger) {
   //   //      //      itemSize = -itemSize;
   //   //
   //   //      //      size = StringHelper::strToLong(sizeValue.value, 16) * itemSize;
   //   //      //   }
   //   //      //   else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
   //   //      //}
   //   //else raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}


//void Compiler :: compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue)
//{
//   //if (switchValue.kind == okObject) {
//   //   scope.writer->insert(lxVariable);
//   //   scope.writer->insert(lxSwitching);
//   //   scope.writer->closeNode();
//
//   //   switchValue.kind = okBlockLocal;
//   //   switchValue.param = 1;
//   //}
//   //else scope.writer->insert(lxSwitching);
//
//   //DNode option = node.firstChild();
//   //while (option == nsSwitchOption || option == nsBiggerSwitchOption || option == nsLessSwitchOption)  {
//   //   scope.writer->newNode(lxOption);
//   //   recordDebugStep(scope, option.firstChild().FirstTerminal(), dsStep);
//
//   //   //      _writer.declareSwitchOption(*scope.tape);
//
//   //   int operator_id = EQUAL_MESSAGE_ID;
//   //   if (option == nsBiggerSwitchOption) {
//   //      operator_id = GREATER_MESSAGE_ID;
//   //   }
//   //   else if (option == nsLessSwitchOption) {
//   //      operator_id = LESS_MESSAGE_ID;
//   //   }
//
//   //   scope.writer->newBookmark();
//
//   //   writeTerminal(TerminalInfo(), scope, switchValue);
//
//   //   DNode operand = option.firstChild();
//   //   ObjectInfo result = compileOperator(operand, scope, switchValue, 0, operator_id);
//   //   scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
//   //   appendTerminalInfo(scope.writer, node.FirstTerminal());
//   //   scope.writer->closeNode();
//
//   //   scope.writer->removeBookmark();
//
//   //   scope.writer->newNode(lxElse, scope.moduleScope->falseReference);
//
//   //   CodeScope subScope(&scope);
//   //   DNode thenCode = option.firstChild().nextNode();
//
//   //   //_writer.declareBlock(*scope.tape);
//
//   //   DNode statement = thenCode.firstChild();
//   //   if (statement.nextNode() != nsNone || statement == nsCodeEnd) {
//   //      compileCode(thenCode, subScope);
//   //   }
//   //   // if it is inline action
//   //   else compileRetExpression(statement, scope, 0);
//
//   //   scope.writer->closeNode();
//
//   //   scope.writer->closeNode();
//
//   //   option = option.nextNode();
//   //}
//   //if (option == nsLastSwitchOption) {
//   //   scope.writer->newNode(lxElse);
//
//   //   CodeScope subScope(&scope);
//   //   DNode thenCode = option.firstChild();
//
//   //   //_writer.declareBlock(*scope.tape);
//
//   //   DNode statement = thenCode.firstChild();
//   //   if (statement.nextNode() != nsNone || statement == nsCodeEnd) {
//   //      compileCode(thenCode, subScope);
//   //   }
//   //   // if it is inline action
//   //   else compileRetExpression(statement, scope, 0);
//
//   //   scope.writer->closeNode();
//   //}
//
//   //scope.writer->closeNode();
//}

void Compiler :: compileVariable(DNode node, CodeScope& scope, DNode hints)
{
   TerminalInfo terminal = node.Terminal();

   if (!scope.locals.exist(terminal)) {
      ref_t type = 0;
      ref_t classRef = 0;
      int size = 0;
      compileLocalHints(hints, scope, type, classRef, size);

      ClassInfo localInfo;
      bool bytearray = false;
      if (isPrimitiveRef(classRef)) {
         scope.moduleScope->loadClassInfo(localInfo, scope.moduleScope->subjectHints.get(type), true);
         if (isEmbeddable(localInfo)) {
            switch (classRef)
            {
               case -1:
               case -2:
               case -4:
                  size = localInfo.size;
                  break;
               case -3:
                  size = size * localInfo.size;
                  bytearray = true;
                  break;
            }
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
      else if (classRef != 0) {
         scope.moduleScope->loadClassInfo(localInfo, classRef, true);

         if (isEmbeddable(localInfo))
            size = localInfo.size;
      }      

      ObjectInfo variable(okLocal, 0, classRef, type);
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
   //            default:
   //               scope.writer->newNode(lxBinaryVariable);
   //               scope.writer->appendNode(lxTerminal, terminal.value);
   //               scope.writer->appendNode(lxLevel, variable.param);

   //               //if (type != 0) {
   //               //   ref_t classRef = scope.moduleScope->typeHints.get(type);
   //               //
   //               //   scope.writer->appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
   //               //}

   //               scope.writer->closeNode();
   //               break;
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
   //   //case okCharConstant:
   //   //   scope.writer->newNode(lxConstantChar, object.param);
   //   //   break;
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
      //case okOuter:
         scope.writer->newNode(lxField, object.param);
         break;
   //   //case okOuterField:
   //   //   scope.writer->newNode(lxExpression);
   //   //   scope.writer->appendNode(lxField, object.param);
   //   //   scope.writer->appendNode(lxResultField, object.extraparam);
   //   //   break;
      case okLocalAddress:
         scope.writer->newNode(lxBoxing);
         scope.writer->appendNode(lxLocalAddress, object.param);
         break;
      case okFieldAddress:
         scope.writer->newNode(lxBoxing);
         scope.writer->appendNode(lxFieldAddress, object.param);
         break;
   //   case okNil:
   //      scope.writer->newNode(lxNil, object.param);
   //      break;
   //   case okVerbConstant:
   //      scope.writer->newNode(lxVerbConstant, object.param);
   //      break;
   //   case okMessageConstant:
   //      scope.writer->newNode(lxMessageConstant, object.param);
   //      break;
   //   case okExtMessageConstant:
   //      scope.writer->newNode(lxExtMessageConstant, object.param);
   //      break;
   //   case okSignatureConstant:
   //      scope.writer->newNode(lxSignatureConstant, object.param);
   //      break;
   //   //case okSubject:
   //   //   scope.writer->newNode(lxLocalAddress, object.param);
   //   //   break;
   //   //case okBlockLocal:
   //   //   scope.writer->newNode(lxBlockLocal, object.param);
   //   //   break;
   //   //case okParams:
   //   //   scope.writer->newNode(lxBlockLocalAddr, object.param);
   //   //   break;
   //   case okObject:
   //      scope.writer->newNode(lxResult);
   //      break;
   //   case okConstantRole:
   //      scope.writer->newNode(lxConstantSymbol, object.param);
   //      break;
      case okTemplateTarget:
         scope.writer->newNode(lxTemplateTarget, object.param);
         break;
      case okExternal:
      case okInternal:
         // HOTFIX : external / internal node will be declared later
         return;
   }

   appendObjectInfo(scope, object);
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
//   else if (terminal==tsCharacter) {
//      object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(terminal));
//   }
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
//      case nsRetStatement:
         if (objectNode.Terminal() != nsNone) {
            result = compileClosure(objectNode, scope, 0);
            break;
         }
      case nsSubCode:
//      case nsSubjectArg:
//      case nsMethodParameter:
         result = compileClosure(member, scope, 0);
         break;
//      case nsInlineClosure:
//         result = compileClosure(member.firstChild(), scope, HINT_CLOSURE);
//         break;
//      case nsInlineExpression:
//         result = compileClosure(objectNode, scope, HINT_ACTION);
//         break;
      case nsExpression:
//         if (isCollection(member)) {
//            TerminalInfo parentInfo = objectNode.Terminal();
//            // if the parent class is defined
//            if (parentInfo == tsIdentifier || parentInfo == tsReference || parentInfo == tsPrivate) {
//               ref_t vmtReference = scope.moduleScope->mapTerminal(parentInfo, true);
//               if (vmtReference == 0)
//                  scope.raiseError(errUnknownObject, parentInfo);
//
//               result = compileCollection(member, scope, 0, vmtReference);
//            }
//            //else result = compileCollection(member, scope, 0);
//         }
         /*else */result = compileExpression(member, scope, 0, HINT_NOBOXING);
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

      //if (paramCount == OPEN_ARG_COUNT) {
      //   // HOT FIX : support open argument list
      //   ref_t openArgType = scope.moduleScope->defineType(scope.moduleScope->paramsReference);
      //   if (!emptystr(signature))
      //      signature.append('&');

      //   signature.append(scope.moduleScope->module->resolveSubject(openArgType));
      //}
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

ref_t Compiler :: mapMessage(DNode node, CodeScope& scope, size_t& paramCount/*, bool& argsUnboxing*/)
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
         //if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
         //   paramCount += OPEN_ARG_COUNT;
         //   if (paramCount > 0x0F)
         //      scope.raiseError(errNotApplicable, subject);

         //   ObjectInfo argListParam = scope.mapObject(arg.firstChild().Terminal());
         //   // HOTFIX : set flag if the argument list has to be unboxed
         //   if (arg.firstChild().nextNode() == nsNone && argListParam.kind == okParams) {
         //      argsUnboxing = true;
         //   }
         //}
         //else {
            paramCount++;

            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            arg = arg.nextNode();
         //}
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

//ref_t Compiler :: mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo object)
//{
//   // check typed extension if the type available
//   ref_t type = 0;
//   ref_t extRef = 0;
//
//   if (object.type != 0 && scope.moduleScope->extensionHints.exist(messageRef, object.type)) {
//      type = object.type;
//   }
//   else {
//      if (scope.moduleScope->extensionHints.exist(messageRef)) {
//         ref_t classRef = resolveObjectReference(scope, object);
//         // if class reference available - select the possible type
//         if (classRef != 0) {
//            SubjectMap::Iterator it = scope.moduleScope->extensionHints.start();
//            while (!it.Eof()) {
//               if (it.key() == messageRef) {
//                  if (scope.moduleScope->typeHints.exist(*it, classRef)) {
//                     type = *it;
//
//                     break;
//                  }
//               }
//
//               it++;
//            }
//         }
//      }
//   }
//
//   if (type != 0) {
//      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(type);
//
//      if (typeExtensions)
//         extRef = typeExtensions->get(messageRef);
//   }
//
//   // check generic extension
//   if (extRef == 0) {
//      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(0);
//
//      if (typeExtensions)
//         extRef = typeExtensions->get(messageRef);
//   }
//
//   return extRef;
//}
//
//ObjectInfo Compiler :: compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
//{
//   //scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
//   //appendTerminalInfo(scope.writer, node.FirstTerminal());
//   //scope.writer->closeNode();
//
//   //DNode elsePart = node.select(nsElseOperation);
//   //if (elsePart != nsNone) {
//   //   scope.writer->newNode(lxIf, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);
//
//   //   compileBranching(node, scope/*, object, operator_id, 0*/);
//
//   //   scope.writer->closeNode();
//   //   scope.writer->newNode(lxElse);
//
//   //   compileBranching(elsePart, scope); // for optimization, the condition is checked only once
//
//   //   scope.writer->closeNode();
//   //}
//   //else {
//   //   scope.writer->newNode(lxIf, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);
//
//   //   compileBranching(node, scope);
//
//   //   scope.writer->closeNode();
//   //}
//
//   //scope.writer->insert(lxBranching);
//   //scope.writer->closeNode();
//
//   return ObjectInfo(okObject);
//}

//int Compiler :: mapOperandType(CodeScope& scope, ObjectInfo operand)
//{
//   if (operand.kind == okIntConstant) {
//      return elDebugDWORD;
//   }
//   else if (operand.kind == okLongConstant) {
//      return elDebugQWORD;
//   }
//   //else if (operand.kind == okRealConstant) {
//   //   return elDebugReal64;
//   //}
//   //else if (operand.kind == okSubject) {
//   //   return elDebugSubject;
//   //}
//   else if (operand.kind == okLocalAddress && operand.extraparam == -1) {
//      ClassInfo info;
//      scope.moduleScope->loadClassInfo(info, scope.moduleScope->subjectHints.get(operand.type), true);
//      switch (info.header.flags & elDebugMask) {
//         case elDebugDWORD:
//            return elDebugIntegers;
//         default:
//            return 0;
//      }
//   }
//   else return scope.moduleScope->getClassFlags(resolveObjectReference(scope, operand)) & elDebugMask;
//}

//int Compiler :: mapVarOperandType(CodeScope& scope, ObjectInfo operand)
//{
//   int flags = scope.moduleScope->getClassFlags(resolveObjectReference(scope, operand));
//
//   // read only classes cannot be used for variable operations
//   if (test(flags, elReadOnlyRole))
//      flags = 0;
//
//   return flags & elDebugMask;
//}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   //ModuleScope* moduleScope = scope.moduleScope;

   ObjectInfo retVal(okObject);

   // HOTFIX : recognize SET_REFER_MESSAGE_ID
   if (operator_id == REFER_MESSAGE_ID && node.nextNode() == nsAssigning)
      operator_id = SET_REFER_MESSAGE_ID;

   bool dblOperator = IsDoubleOperator(operator_id);
   //bool notOperator = IsInvertedOperator(operator_id);

   ObjectInfo operand = compileExpression(node, scope, 0, 0);
   if (dblOperator)
      compileExpression(node.nextNode().firstChild(), scope, 0, 0);

   //// try to implement the primitive operation directly
   //LexicalType primitiveOp = lxNone;
   //size_t size = 0;

   ////// if it is comparing with nil
   ////if (object.kind == okNil && operator_id == EQUAL_MESSAGE_ID) {
   ////   primitiveOp = lxNilOp;
   ////}
   //// HOTFIX : primitive operations can be implemented only in the method
   //// because the symbol implementations do not open a new stack frame
   ///*else */if (scope.getScope(Scope::slMethod) != NULL) {
   //   int lflag, rflag;

   ////   if (IsVarOperator(operator_id)) {
   ////      lflag = mapVarOperandType(scope, object);
   ////   }
   //   /*else */lflag = mapOperandType(scope, object);

   //   rflag = mapOperandType(scope, operand);

   ////   if (lflag == rflag) {
   ////      if (lflag == elDebugDWORD && (IsExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
   ////         if (IsExprOperator(operator_id))
   ////            retVal.param = resolveObjectReference(scope, object);

   ////         primitiveOp = lxIntOp;
   ////         size = 4;
   ////      }
   ////      else if (lflag == elDebugQWORD && (IsExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
   ////         if (IsExprOperator(operator_id))
   ////            retVal.param = moduleScope->longReference;

   ////         primitiveOp = lxLongOp;
   ////         size = 8;
   ////      }
   ////      else if (lflag == elDebugReal64 && (IsRealExprOperator(operator_id) || IsCompOperator(operator_id) || IsVarOperator(operator_id))) {
   ////         if (IsExprOperator(operator_id))
   ////            retVal.param = moduleScope->realReference;

   ////         primitiveOp = lxRealOp;
   ////         size = 8;
   ////      }
   ////      else if (lflag == elDebugSubject && IsCompOperator(operator_id)) {
   ////         primitiveOp = lxIntOp;
   ////         size = 4;
   ////      }
   ////   }
   //   /*else */if (IsReferOperator(operator_id)) {
   //      if (isArrayPrimitive(lflag) && rflag == elDebugDWORD) {
   //         ref_t itemType = 0;
   //         if (object.kind == okLocalAddress && object.extraparam == -1) {
   //            itemType = object.type;

   //            size = moduleScope->defineSubjectSize(itemType);
   //         }
   //         else {
   //            ClassInfo info;
   //            moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(resolveObjectReference(scope, object)), false);

   //            itemType = info.fieldTypes.get(-1);
   //            size = -info.size;
   //         }
   //            
   //         else {
   //            primitiveOp = lxArrOp;

   //            retVal.type = itemType;
   //         }
   //      }
   //   }
   //}

   //if (primitiveOp != lxNone) {
      scope.writer->insert(lxOp, operator_id);

   //   if (size != 0)
   //      scope.writer->appendNode(lxSize, size);

   ////   if (IsCompOperator(operator_id)) {
   ////      if (notOperator) {
   ////         scope.writer->appendNode(lxIfValue, scope.moduleScope->falseReference);
   ////         scope.writer->appendNode(lxElseValue, scope.moduleScope->trueReference);
   ////      }
   ////      else {
   ////         scope.writer->appendNode(lxIfValue, scope.moduleScope->trueReference);
   ////         scope.writer->appendNode(lxElseValue, scope.moduleScope->falseReference);
   ////      }
   ////   }

   ////   if (IsCompOperator(operator_id))
   ////      retVal.type = moduleScope->boolType;

      appendObjectInfo(scope, retVal);

      scope.writer->closeNode();
   //}
   //else {
   //   if (operator_id == SET_REFER_MESSAGE_ID)
   //      compileExpression(node.nextNode().firstChild(), scope, 0, 0);

   //   int message_id = encodeMessage(0, operator_id, dblOperator ? 2 : 1);

   //   // otherwise operation is replaced with a normal message call
   //   retVal = compileMessage(node, scope, object, message_id, mode/* | HINT_INLINE*/);

   //   //if (notOperator) {
   //   //   scope.writer->insert(lxTypecasting, encodeMessage(scope.moduleScope->boolType, GET_MESSAGE_ID, 0));
   //   //   scope.writer->closeNode();

   //   //   scope.writer->insert(lxBoolOp, NOT_MESSAGE_ID);
   //   //   scope.writer->appendNode(lxIfValue, scope.moduleScope->trueReference);
   //   //   scope.writer->appendNode(lxElseValue, scope.moduleScope->falseReference);
   //   //   scope.writer->closeNode();

   //   //   retVal.type = scope.moduleScope->boolType;
   //   //}
   //}

   //if (dblOperator)
   //   node = node.nextNode();

   return retVal;
}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   TerminalInfo operator_name = node.Terminal();
   int operator_id = _operators.get(operator_name);

//   // if it is branching operators
//   if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
//      return compileBranchingOperator(node, scope, object, mode, operator_id);
//   }

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
   //bool dispatchCall = false;
   //bool templateCall = false;
   int methodHint = classReference != 0 ? scope.moduleScope->checkMethod(classReference, messageRef, classFound, retVal.type) : 0;
   int callType = methodHint & tpMask;

   if (target.kind == okConstantClass) {
      retVal.param = target.param;

      // constructors are always sealed
      callType = tpSealed;
   }
   //else if (classReference == scope.moduleScope->signatureReference) {
   //   dispatchCall = test(mode, HINT_EXTENSION_MODE);
   //}
   //else if (classReference == scope.moduleScope->messageReference) {
   //   dispatchCall = test(mode, HINT_EXTENSION_MODE);
   //}
   else if (target.kind == okSuper) {
      // parent methods are always sealed
      callType = tpSealed;
   }
   //else if (target.kind == okTemplateTarget) {
   //   templateCall = true;
   //}

   //if (dispatchCall) {
   //   scope.writer->insert(lxDirectCalling, encodeVerb(DISPATCH_MESSAGE_ID));

   //   scope.writer->appendNode(lxMessage, messageRef);
   //   scope.writer->appendNode(lxCallTarget, classReference);
   //   //scope.writer->appendNode(lxStacksafe);
   //}
   /*else */if (callType == tpClosed) {
      scope.writer->insert(lxSDirctCalling, messageRef);

      scope.writer->appendNode(lxCallTarget, classReference);
      if (test(methodHint, tpStackSafe))
         scope.writer->appendNode(lxStacksafe);
   }
   else if (callType == tpSealed) {
      scope.writer->insert(lxDirectCalling, messageRef);

      scope.writer->appendNode(lxCallTarget, classReference);
      if (test(methodHint, tpStackSafe))
         scope.writer->appendNode(lxStacksafe);
      //if (test(methodHint, tpEmbeddable))
      //   scope.writer->appendNode(lxEmbeddable);
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
   //bool argsUnboxing = false;
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount/*, argsUnboxing*/);

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
         //if (arg.nextNode() != nsSubjectArg && scope.moduleScope->typeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
         //   // check if argument list should be unboxed
         //   DNode param = arg.firstChild();

         //   ObjectInfo argListParam = scope.mapObject(arg.firstChild().Terminal());
         //   if (arg.firstChild().nextNode() == nsNone && argListParam.kind == okParams) {
         //      scope.writer->newNode(lxArgUnboxing);
         //      writeTerminal(arg.firstChild().Terminal(), scope, argListParam);
         //      scope.writer->closeNode();
         //   }
         //   else {
         //      while (arg != nsNone) {
         //         compileExpression(arg.firstChild(), scope, 0, paramMode);

         //         arg = arg.nextNode();
         //      }

         //      // terminator
         //      writeTerminal(TerminalInfo(), scope, ObjectInfo(okNil));
         //   }
         //}
         //else {
            compileExpression(arg.firstChild(), scope, subjRef, paramMode);

            arg = arg.nextNode();
         //}
      }
   }

   return messageRef;
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object)
{
   ref_t messageRef = compileMessageParameters(node, scope);
   //ref_t extensionRef = mapExtension(scope, messageRef, object);

   //if (extensionRef != 0) {
   //   //HOTFIX: A proper method should have a precedence over an extension one
   //   if (scope.moduleScope->checkMethod(resolveObjectReference(scope, object), messageRef) == tpUnknown) {
   //      object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
   //   }
   //}

   return compileMessage(node, scope, object, messageRef, 0);
}

ObjectInfo Compiler :: compileAssigning(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   DNode member = node.nextNode();

   // if it setat operator
   if (member == nsL0Operation) {
      return compileOperations(node, scope, object, mode);
   }
   //else if (member == nsMessageOperation) {
   //   // if it is shorthand property settings
   //   DNode arg = member.firstChild();
   //   if (arg != nsNone || member.nextNode() != nsAssigning)
   //      scope.raiseError(errInvalidSyntax, member.FirstTerminal());

   //   ref_t subject = scope.moduleScope->mapSubject(member.Terminal());
   //   ref_t messageRef = encodeMessage(subject, SET_MESSAGE_ID, 1);

   //   ref_t extensionRef = mapExtension(scope, messageRef, object);

   //   if (extensionRef != 0) {
   //      //HOTFIX: A proper method should have a precedence over an extension one
   //      if (scope.moduleScope->checkMethod(resolveObjectReference(scope, object), messageRef) == tpUnknown) {
   //         object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
   //      }
   //   }

   //   if (scope.moduleScope->typeHints.exist(subject)) {
   //      compileExpression(member.nextNode().firstChild(), scope, subject, 0);
   //   }
   //   else compileExpression(member.nextNode().firstChild(), scope, 0, 0);

   //   return compileMessage(member, scope, object, messageRef, 0);
   //}
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
      else if (object.kind == okLocal || object.kind == okField/* || object.kind == okOuterField*/) {

      }
      else if (object.kind == okParam) {
         // Compiler magic : allowing to assign byref / variable parameter
         classReference = scope.moduleScope->subjectHints.get(object.type);
         ClassInfo info;
         scope.moduleScope->loadClassInfo(info, classReference);
         if (test(info.header.flags, elWrapper)) {
            size = info.size;
            currentObject.kind = okParamField;
         }
         else scope.raiseError(errInvalidOperation, node.Terminal());
      }
      else if (object.kind == okTemplateTarget) {
         // if it is a template field
         // treates it like a normal field
         currentObject.kind = okField;
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());

      currentObject = compileAssigningExpression(node, member, scope, currentObject);

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
//      else if (member == nsMessageParameter) {
//         currentObject = compileMessage(member, scope, currentObject);
//
//         // skip all except the last message parameter
//         while (member.nextNode() == nsMessageParameter)
//            member = member.nextNode();
//      }
//      else if (member == nsExtension) {
//         currentObject = compileExtension(member, scope, currentObject, mode);
//      }
      else if (member == nsL3Operation || member == nsL4Operation || member == nsL5Operation || member == nsL6Operation
         || member == nsL7Operation || member == nsL0Operation)
      {
         currentObject = compileOperator(member, scope, currentObject, mode);
      }
//      else if (member == nsAltMessageOperation) {
//         scope.writer->newBookmark();
//
//         scope.writer->appendNode(lxCurrent);
//
//         currentObject = compileMessage(member, scope, ObjectInfo(okObject));
//
//         scope.writer->removeBookmark();
//      }
//      else if (member == nsCatchMessageOperation) {
//         scope.writer->newBookmark();
//
//         scope.writer->appendNode(lxResult);
//
//         currentObject = compileMessage(member, scope, ObjectInfo(okObject));
//
//         scope.writer->removeBookmark();
//      }
//      else if (member == nsSwitching) {
//         compileSwitch(member, scope, currentObject);
//
//         currentObject = ObjectInfo(okObject);
//      }

      member = member.nextNode();
   }

   return currentObject;
}

ObjectInfo Compiler :: compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;
   ObjectInfo   role;

   DNode roleNode = node.firstChild();
   //// check if the extension can be used as a static role (it is constant)
   //if (roleNode.firstChild() == nsNone) {
   //   int flags = 0;

   //   role = scope.mapObject(roleNode.Terminal());
   //   if (role.kind == okSymbol || role.kind == okConstantSymbol) {
   //      ref_t classRef = role.kind == okConstantSymbol ? role.extraparam : role.param;

   //      // if the symbol is used inside itself
   //      if (classRef == scope.getClassRefId()) {
   //         flags = scope.getClassFlags();
   //      }
   //      // otherwise
   //      else {
   //         ClassInfo roleClass;
   //         moduleScope->loadClassInfo(roleClass, moduleScope->module->resolveReference(classRef));

   //         flags = roleClass.header.flags;
   //      }
   //   }
   //   // if the symbol VMT can be used as an external role
   //   if (test(flags, elStateless)) {
   //      role = ObjectInfo(okConstantRole, role.param);
   //   }
   //}

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
   //bool lazyExpression = !test(mode, HINT_CLOSURE) && isReturnExpression(node.firstChild());

   methodScope.message = encodeVerb(EVAL_MESSAGE_ID);

   if (argNode != nsNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope);

      node = node.select(nsSubCode);
   }

   if (!alreadyDeclared) {
      ref_t parentRef = scope.info.header.parentRef;
      //if (lazyExpression) {
      //   parentRef = scope.moduleScope->getBaseLazyExpressionClass();
      //}
      //else if (getSignature(methodScope.message) == 0) {
      //   parentRef = scope.moduleScope->getBaseFunctionClass(getParamCount(methodScope.message));
      //}
      //else {
      //   // check if it is nfunc
      //   ref_t nfuncRef = scope.moduleScope->getBaseIndexFunctionClass(getParamCount(methodScope.message));

      //   if (nfuncRef != 0 && scope.moduleScope->checkMethod(nfuncRef, methodScope.message) != tpUnknown) {
      //      parentRef = nfuncRef;
      //   }
      //}

      compileParentDeclaration(DNode(), scope, parentRef);
   }

   // HOT FIX : mark action as stack safe if the hint was declared in the parent class
   methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);   

   return /*lazyExpression*/false;
}

void Compiler :: compileAction(DNode node, ClassScope& scope, DNode argNode, int mode, bool alreadyDeclared)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   ActionScope methodScope(&scope);
   /*bool lazyExpression = */declareActionScope(node, scope, argNode, writer, methodScope, mode, alreadyDeclared);

   writer.newNode(lxClassMethod, methodScope.message);
   writer.appendNode(lxSourcePath); // the source path is first string

   // if it is single expression
   //if (!lazyExpression) {
      compileActionMethod(node, writer, methodScope);
   //}
   //else compileLazyExpressionMethod(node.firstChild(), writer, methodScope);

   writer.closeNode();  // closing method

   writer.closeNode();

   if (!alreadyDeclared)
      generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));

   generateClassImplementation(scope);
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

   writer.closeNode();

   generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));
   generateClassImplementation(scope);
}

ObjectInfo Compiler :: compileClosure(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      ownerScope.writer->appendNode(lxConstantSymbol, scope.reference);

      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, scope.reference, scope.reference/*, scope.moduleScope->defineType(scope.reference)*/);
   }
   else {
      //// dynamic binary symbol
      //if (test(scope.info.header.flags, elStructureRole)) {
      //   ownerScope.writer->newNode(lxStruct, scope.info.size);
      //   ownerScope.writer->appendNode(lxTarget, scope.reference);

      //   if (scope.outers.Count() > 0)
      //      scope.raiseError(errInvalidInlineClass, node.Terminal());
      //}
      //else {
      //   // dynamic normal symbol
         ownerScope.writer->newNode(lxNested, /*scope.info.fields.Count()*/(size_t)0);
         ownerScope.writer->appendNode(lxTarget, scope.reference);
      //}

      //Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      ////int toFree = 0;
      //while(!outer_it.Eof()) {
      //   ObjectInfo info = (*outer_it).outerObject;

      //   ownerScope.writer->newNode(lxMember, (*outer_it).reference);
      //   ownerScope.writer->newBookmark();

      //   writeTerminal(TerminalInfo(), ownerScope, info);

      //   ownerScope.writer->removeBookmark();
      //   ownerScope.writer->closeNode();

      //   outer_it++;
      //}

      ownerScope.writer->closeNode();

      return ObjectInfo(okObject, scope.reference/*, 0, scope.moduleScope->defineType(scope.reference)*/);
   }
}

ObjectInfo Compiler :: compileClosure(DNode node, CodeScope& ownerScope, int mode)
{
   InlineClassScope scope(&ownerScope, ownerScope.moduleScope->mapNestedExpression());

   // if it is a lazy expression / multi-statement closure without parameters
   if (node == nsSubCode || node == nsInlineClosure) {
      compileAction(node, scope, DNode(), mode);
   }
//   // if it is a closure / labda function with a parameter
//   else if (node == nsObject && testany(mode, HINT_ACTION | HINT_CLOSURE)) {
//      compileAction(node.firstChild(), scope, node, mode);
//   }
//   // if it is an action code block
//   else if (node == nsMethodParameter || node == nsSubjectArg) {
//      compileAction(goToSymbol(node, nsInlineExpression), scope, node, 0);
//   }
   // if it is inherited nested class
   else if (node.Terminal() != nsNone) {
	   // inherit parent
      compileNestedVMT(node.firstChild(), node, scope);
   }
   // if it is normal nested class
   else compileNestedVMT(node, DNode(), scope);

   return compileClosure(node, ownerScope, scope, mode);
}

////ObjectInfo Compiler :: compileCollection(DNode objectNode, CodeScope& scope, int mode)
////{
////   return compileCollection(objectNode, scope, mode, scope.moduleScope->arrayReference);
////}
//
//ObjectInfo Compiler :: compileCollection(DNode node, CodeScope& scope, int mode, ref_t vmtReference)
//{
//   int counter = 0;
//
//   scope.writer->newBookmark();
//
//   // all collection memebers should be created before the collection itself
//   while (node != nsNone) {
//
//      scope.writer->newNode(lxMember, counter);
//
//      ObjectInfo current = compileExpression(node, scope, 0, mode);
//
//      scope.writer->closeNode();
//
//      node = node.nextNode();
//      counter++;
//   }
//
//   scope.writer->insert(lxNested, counter);
//   scope.writer->appendNode(lxTarget, vmtReference);
//   scope.writer->closeNode();
//
//   scope.writer->removeBookmark();
//
//   return ObjectInfo(okObject);
//}

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
   }

   scope.freeSpace();

   return ObjectInfo(okObject, 0, 0, subj);
}

ObjectInfo Compiler :: compileExpression(DNode node, CodeScope& scope, ref_t targetType, int mode)
{
   scope.writer->newBookmark();

   ObjectInfo objectInfo;
   if (node != nsObject) {
      DNode member = node.firstChild();

      if (member.nextNode() != nsNone) {
         if (member == nsObject) {
            objectInfo = compileObject(member, scope, mode);
         }
         if (findSymbol(member, nsAssigning)) {
            objectInfo = compileAssigning(member, scope, objectInfo, mode);
         }
   //      else if (findSymbol(member, nsAltMessageOperation)) {
   //         scope.writer->insert(lxVariable);
   //         scope.writer->closeNode();

   //         scope.writer->newNode(lxAlt);
   //         scope.writer->newBookmark();
   //         scope.writer->appendNode(lxResult);
   //         objectInfo = compileOperations(member, scope, objectInfo, mode);
   //         scope.writer->removeBookmark();
   //         scope.writer->closeNode();

   //         scope.writer->appendNode(lxReleasing, 1);
   //      }
         else objectInfo = compileOperations(member, scope, objectInfo, mode);
      }
      else objectInfo = compileObject(member, scope, mode);
   }
   else objectInfo = compileObject(node, scope, mode);

   //// if it is try-catch statement
   //if (findSymbol(node.firstChild(), nsCatchMessageOperation)) {
   //   scope.writer->insert(lxTrying);
   //   scope.writer->closeNode();
   //}

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
      //case okOuterField:
      case okLocalAddress:
      case okFieldAddress:
      case okParamField:
         break;
      case okUnknown:
         scope.raiseError(errUnknownObject, node.Terminal());
      default:
         scope.raiseError(errInvalidOperation, node.Terminal());
         break;
   }

   scope.writer->newBookmark();

   ObjectInfo objectInfo = compileExpression(assigning.firstChild(), scope, targetType, 0);

   // if target type is not defined, try to find it out
   if (target.extraparam > 0 && target.type == 0) {
      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, target.extraparam, false);

      // only wrapper class can be used in this case
      if (test(info.header.flags, elWrapper)) {
         target.type = info.fieldTypes.get(0);

         scope.writer->insert(lxTypecasting, target.type);
         scope.writer->insert(lxBoxing, info.size);
         scope.writer->appendNode(lxTarget, target.extraparam);
         appendTerminalInfo(scope.writer, node.FirstTerminal());
         scope.writer->closeNode();
         scope.writer->closeNode();
      }
      // HOTFIX : to allow boxing primitive array
      else if (test(info.header.flags, elDynamicRole | elStructureRole) && objectInfo.kind == okLocalAddress && objectInfo.extraparam == -3 &&
         (objectInfo.type == info.fieldTypes.get(-1)))
      {
         scope.writer->insert(lxBoxing, info.size);
         scope.writer->appendNode(lxTarget, target.extraparam);
         appendTerminalInfo(scope.writer, node.FirstTerminal());
         scope.writer->closeNode();
      }
      else scope.raiseError(errInvalidOperation, assigning.FirstTerminal());
   }

   scope.writer->removeBookmark();

   return objectInfo;
}

//ObjectInfo Compiler :: compileBranching(DNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodeMode*/)
//{
//   CodeScope subScope(&scope);
//
//   DNode thenCode = thenNode.firstChild();
//
//   DNode expr = thenCode.firstChild();
//   if (expr == nsCodeEnd || expr.nextNode() != nsNone) {
//      compileCode(thenCode, subScope);
//
//      if (subScope.level > scope.level) {
//         scope.writer->appendNode(lxReleasing, subScope.level - scope.level);
//      }
//   }
//   // if it is inline action
//   else compileRetExpression(expr, scope, 0);
//
//   return ObjectInfo(okObject);
//}
//
//void Compiler :: compileThrow(DNode node, CodeScope& scope, int mode)
//{
//   scope.writer->newNode(lxThrowing);
//
//   ObjectInfo object = compileExpression(node.firstChild(), scope, 0, mode);
//
//   scope.writer->closeNode();
//}
//
//void Compiler :: compileLoop(DNode node, CodeScope& scope)
//{
//   //DNode expr = node.firstChild().firstChild();
//
//   //// if it is while-do loop
//   //if (expr.nextNode() == nsL7Operation) {
//   //   scope.writer->newNode(lxLooping);
//
//   //   DNode loopNode = expr.nextNode();
//
//   //   ObjectInfo cond = compileExpression(expr, scope, scope.moduleScope->boolType, 0);
//
//   //   int operator_id = _operators.get(loopNode.Terminal());
//
//   //   // HOTFIX : lxElse is used to be similar with branching code
//   //   // because of optimization rules
//   //   scope.writer->newNode(lxElse, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->falseReference : scope.moduleScope->trueReference);
//   //   compileBranching(loopNode, scope/*, cond, _operators.get(loopNode.Terminal()), HINT_LOOP*/);
//   //   scope.writer->closeNode();
//
//   //   scope.writer->closeNode();
//   //}
//   //// if it is repeat loop
//   //else {
//   //   scope.writer->newNode(lxLooping, scope.moduleScope->trueReference);
//
//   //   ObjectInfo retVal = compileExpression(node.firstChild(), scope, scope.moduleScope->boolType, 0);
//
//   //   scope.writer->closeNode();
//   //}
//}
//
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
//
//void Compiler :: compileLock(DNode node, CodeScope& scope)
//{
//   scope.writer->newNode(lxLocking);
//
//   // implement the expression to be locked
//   ObjectInfo object = compileExpression(node.firstChild(), scope, 0, 0);
//
//   scope.writer->newNode(lxBody);
//
//   // implement critical section
//   CodeScope subScope(&scope);
//   subScope.level += 4; // HOT FIX : reserve place for the lock variable and exception info
//
//   compileCode(goToSymbol(node.firstChild(), nsSubCode), subScope);
//
//   // HOT FIX : clear the sub block local variables
//   if (subScope.level - 4 > scope.level) {
//      scope.writer->appendNode(lxReleasing, subScope.level - scope.level - 4);
//   }
//
//   scope.writer->closeNode();
//   scope.writer->closeNode();
//}

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
            compileExpression(statement, scope, 0, HINT_ROOT);
            scope.writer->closeNode();
            break;
//         case nsThrow:
//            compileThrow(statement, scope, 0);
//            break;
//         case nsLoop:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            //scope.writer->newNode(lxExpression);
//            compileLoop(statement, scope);
//            //scope.writer->closeNode();
//            break;
//         case nsTry:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            compileTry(statement, scope);
//            break;
//         case nsLock:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            compileLock(statement, scope);
//            break;
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
//         case nsExtern:
//            scope.writer->newNode(lxExternFrame);
//            compileCode(statement, scope);
//            scope.writer->closeNode();
//            break;
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
   scope.writer->removeBookmark();

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

      LexicalType argType = lxNone;
      // if it is an integer number pass it directly
      switch (flags & elDebugMask) {
         case elDebugDWORD:
      //   case elDebugPTR:
      //   case elDebugSubject:
            argType = test(flags, elReadOnlyRole) ? lxIntExtArgument : lxExtArgument;
            break;
      //   case elDebugReference:
      //      argType = lxExtInteranlRef;
      //      break;
         default:
            scope.raiseError(errInvalidOperation, terminal);
            break;
      }

      arg = arg.nextNode();
      if (arg == nsMessageParameter) {
      //   if (argType == lxExtInteranlRef) {
      //      if (isSingleObject(arg.firstChild())) {
      //         ObjectInfo target = compileTerminal(arg.firstChild(), scope);
      //         if (target.kind == okInternal) {
      //            scope.writer->appendNode(lxExtInteranlRef, target.param);
      //         }
      //         else scope.raiseError(errInvalidOperation, terminal);
      //      }
      //      else scope.raiseError(errInvalidOperation, terminal);
      //   }
      //   else {
            scope.writer->newNode(argType);

            ObjectInfo info = compileExpression(arg.firstChild(), scope, subject, 0);
            if (info.kind == okIntConstant) {
               int value = StringHelper::strToULong(moduleScope->module->resolveConstant(info.param), 16);

               scope.writer->appendNode(lxValue, value);
            }

            scope.writer->closeNode();
      //   }

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

   // only eval message is allowed
   TerminalInfo     verb = node.Terminal();
   if (_verbs.get(verb) != EVAL_MESSAGE_ID)
      scope.raiseError(errInvalidOperation, verb);

   scope.writer->newNode(lxInternalCall, routine.param);

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
   if (node != nsDefaultGeneric) {
	   verb_id = _verbs.get(verb.value);

	   // if it is a generic verb, make sure no parameters are provided
	   if (verb_id == DISPATCH_MESSAGE_ID) {
		   scope.raiseError(errInvalidOperation, verb);
	   }
	   else if (verb_id == 0) {
         sign_id = scope.mapSubject(verb, signature);
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
         //if (scope.moduleScope->typeHints.exist(subj_ref, scope.moduleScope->paramsReference)) {
         //   scope.parameters.add(arg.Terminal(), Parameter(index, subj_ref));

         //   // the generic arguments should be free by the method exit
         //   scope.rootToFree += paramCount;
         //   scope.withOpenArg = true;

         //   // to indicate open argument list
         //   paramCount += OPEN_ARG_COUNT;
         //   if (paramCount > 0xF)
         //      scope.raiseError(errNotApplicable, arg.Terminal());
         //}
         //else {
            paramCount++;
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            scope.parameters.add(arg.Terminal(), Parameter(index, subj_ref));

            arg = arg.nextNode();
         //}
      }
   }

   //while (hints == nsHint) {
   //   TerminalInfo terminal = hints.Terminal();
   //   if (StringHelper::compare(terminal, HINT_GENERIC)) {
   //      if (!emptystr(signature))
   //         scope.raiseError(errInvalidHint, terminal);

   //      signature.copy(GENERIC_PREFIX);
   //   }

   //   hints = hints.nextNode();
   //}

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

//void Compiler :: compileLazyExpressionMethod(DNode node, SyntaxWriter& writer, MethodScope& scope)
//{
//   CodeScope codeScope(&scope, &writer);
//
//   // new stack frame
//   // stack already contains previous $self value
//   writer.newNode(lxNewFrame);
//   codeScope.level++;
//
//   declareParameterDebugInfo(scope, writer, false, false);
//
//   compileRetExpression(node, codeScope, 0);
//
//   writer.closeNode();
//   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
//   writer.appendNode(lxReserved, scope.reserved);
//}

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

//void Compiler :: compileConstructorResendExpression(DNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
//{
//   ModuleScope* moduleScope = scope.moduleScope;
//   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
//
//   // find where the target constructor is declared in the current class
//   size_t count = 0;
//   ref_t messageRef = mapMessage(node, scope, count);
//   ref_t classRef = classClassScope.reference;
//   bool found = false;
//
//   // find where the target constructor is declared in the current class
//   // but it is not equal to the current method
//   if (methodScope->message != messageRef && classClassScope.info.methods.exist(messageRef)) {
//      found = true;
//   }
//   // otherwise search in the parent class constructors
//   else {
//      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//      ref_t parent = classScope->info.header.parentRef;
//      ClassInfo info;
//      while (parent != 0) {
//         moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(parent));
//
//         if (moduleScope->checkMethod(info.classClassRef, messageRef) != tpUnknown) {
//            classRef = info.classClassRef;
//            found = true;
//
//            break;
//         }
//         else parent = info.header.parentRef;
//      }
//   }
//   if (found) {
//      if (count != 0 && methodScope->parameters.Count() != 0) {
//         withFrame = true;
//
//         // new stack frame
//         // stack already contains $self value
//         scope.writer->newNode(lxNewFrame);
//         scope.level++;
//      }
//
//      scope.writer->newBookmark();
//
//      if (withFrame) {
//         writeTerminal(TerminalInfo(), scope, ObjectInfo(okThisParam, 1));
//         compileExtensionMessage(node, scope, ObjectInfo(okThisParam, 1), ObjectInfo(okConstantClass, 0, classRef));
//      }
//      else {
//         writeTerminal(TerminalInfo(), scope, ObjectInfo(okObject));
//         compileExtensionMessage(node, scope, ObjectInfo(okObject), ObjectInfo(okConstantClass, 0, classRef));
//
//         // HOT FIX : save the created object
//         scope.writer->newNode(lxAssigning);
//         scope.writer->appendNode(lxCurrent, 1);
//         scope.writer->appendNode(lxResult);
//         scope.writer->closeNode();
//      }
//
//      scope.writer->removeBookmark();
//   }
//   else scope.raiseError(errUnknownMessage, node.Terminal());
//}
//
//void Compiler :: compileConstructorDispatchExpression(DNode node, SyntaxWriter& writer, CodeScope& scope)
//{
//   if (node.firstChild() == nsNone) {
//      ObjectInfo info = scope.mapObject(node.Terminal());
//      // if it is an internal routine
//      if (info.kind == okInternal) {
//         importCode(node, *scope.moduleScope, writer, node.Terminal(), scope.getMessageID());
//      }
//      else scope.raiseError(errInvalidOperation, node.Terminal());
//   }
//   else scope.raiseError(errInvalidOperation, node.Terminal());
//}

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
      writer.newNode(lxNewFrame, /*scope.generic ? -1 : */0u);

      codeScope.level++;
      //// declare the current subject for a generic method
      //if (scope.generic) {
      //   codeScope.level++;
      //   codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0);
      //}

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
            //ref_t typeHint = scope.getReturningType();

            //if (typeHint != 0) {
            //   writer.newNode(lxTypecasting, encodeMessage(typeHint, GET_MESSAGE_ID, 0));
            //   writer.appendNode(lxLocal, 1);
            //   appendTerminalInfo(&writer, goToSymbol(body.firstChild(), nsCodeEnd).Terminal());
            //   writer.closeNode();
            //}
            /*else */writer.appendNode(lxLocal, 1);
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

   writer.newNode(lxClassMethod, scope.message);
   writer.appendNode(lxSourcePath);  // the source path is first string

   DNode body = node.select(nsSubCode);
   DNode resendBody = node.select(nsResendExpression);
   DNode dispatchBody = node.select(nsDispatchExpression);

   bool withFrame = false;

   //if (resendBody != nsNone) {
   //   compileConstructorResendExpression(resendBody.firstChild(), codeScope, classClassScope, withFrame);
   //}
   // if no redirect statement - call virtual constructor implicitly
   /*else */if (!test(codeScope.getClassFlags(), elDynamicRole)) {
      writer.appendNode(lxCalling, -1);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else if (dispatchBody == nsNone)
      scope.raiseError(errIllegalConstructor, node.Terminal());

   //if (dispatchBody != nsNone) {
   //   compileConstructorDispatchExpression(dispatchBody.firstChild(), writer, codeScope);
   //   writer.closeNode();
   //   return;
   //}
   // if the constructor has a body
   /*else */if (body != nsNone) {
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

      compileCode(body, codeScope);

      codeScope.writer->appendNode(lxLocal, 1);
   }
   // if the constructor should call embeddable method
   else if (embeddedMethodRef != 0) {
      writer.newNode(lxResending, embeddedMethodRef);
      writer.appendNode(lxTarget, classClassScope.reference);
      writer.newNode(lxAssigning);
      writer.appendNode(lxCurrent, 1);
      writer.appendNode(lxResult);
      writer.closeNode();
      writer.closeNode();
   }

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
      writer.newNode(lxCreatingClass, /*classScope->info.fields.Count()*/(ref_t)0);
      writer.appendNode(lxTarget, classScope->reference);
      writer.closeNode();
   }

   writer.closeNode();
}

//void Compiler :: compileDynamicDefaultConstructor(MethodScope& scope, SyntaxWriter& writer, ClassScope& classClassScope)
//{
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//
//   // HOTFIX: constructor is declared in class class but should be executed if the class scope
//   scope.tape = &classClassScope.tape;
//
//   writer.newNode(lxClassMethod, scope.message);
//   writer.appendNode(lxSourcePath);  // the source path is first string
//
//   if (test(classScope->info.header.flags, elStructureRole)) {
//      writer.newNode(lxCreatingStruct, classScope->info.size);
//      writer.appendNode(lxTarget, classScope->reference);
//      writer.closeNode();
//   }
//   else {
//      writer.newNode(lxCreatingClass, -1);
//      writer.appendNode(lxTarget, classScope->reference);
//      writer.closeNode();
//   }
//
//   writer.closeNode();
//}

void Compiler :: compileVMT(DNode member, SyntaxWriter& writer, ClassScope& scope, bool warningsOnly)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      writer.newBookmark();

      switch(member) {
         case nsMethod:
         {
            MethodScope methodScope(&scope);
            compileMethodHints(hints, writer, methodScope, warningsOnly);

            // if it is a dispatch handler
            if (member.firstChild() == nsDispatchHandler) {
               if (test(scope.info.header.flags, elRole))
                  scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
               methodScope.stackSafe = test(scope.info.methodHints.get(Attribute(methodScope.message, maHint)), tpStackSafe);

               compileDispatcher(member.firstChild().firstChild(), writer, methodScope, test(scope.info.header.flags, elWithGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(member, methodScope, hints);

               int hint = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
               methodScope.stackSafe = test(hint, tpStackSafe);
               //methodScope.generic = test(hint, tpGeneric);

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
            //methodScope.generic = true;

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

   // if no construtors are defined inherits the parent one
   if (!findSymbol(node.firstChild(), nsConstructor)) {
      if (classScope.info.header.parentRef == 0)
         classScope.raiseError(errInvalidParent, node.FirstTerminal());

      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.info.header.parentRef));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }
   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef);
   
   // class class is always stateless
   writer.appendNode(lxClassFlag, elStateless);

   DNode member = node.firstChild();
   declareVMT(member, writer, classClassScope, nsConstructor/*, false, 0*/);
   
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

         //// if the constructor is stack safe, embeddable and the class is an embeddable structure
         //// the special method should be compiled
         //if (test(hint, tpEmbeddable)) {
         //   // make sure the constructor has no redirect / dispatch statements
         //   if (node.select(nsResendExpression) != nsNone || node.select(nsDispatchExpression))
         //      methodScope.raiseError(errInvalidOperation, member.Terminal());

         //   // make sure the class is embeddable
         //   if (!test(classScope.info.header.flags, elStructureRole | elEmbeddable) || !methodScope.stackSafe)
         //      methodScope.raiseError(errInvalidOperation, member.Terminal());

         //   compileEmbeddableConstructor(member, writer, methodScope, classClassScope);
         //}
         /*else */compileConstructor(member, writer, methodScope, classClassScope);
      }
      member = member.nextNode();
   }

   // create a virtual constructor
   MethodScope methodScope(&classScope);
   methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

   //if (test(classScope.info.header.flags, elDynamicRole)) {
   //   compileDynamicDefaultConstructor(methodScope, writer, classClassScope);
   //}
   /*else */compileDefaultConstructor(methodScope, writer, classClassScope);

   writer.closeNode();

   generateClassImplementation(classClassScope);
}

void Compiler :: declareVMT(DNode member, SyntaxWriter& writer, ClassScope& scope, Symbol methodSymbol/*, bool isExtension, ref_t extensionType*/)
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
            methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));

            // mark as having generic methods
            writer.appendNode(lxClassFlag, elWithGenerics);
         }
         else declareArgumentList(member, methodScope, hints);

         writer.newNode(lxClassMethod, methodScope.message);
         appendTerminalInfo(&writer, member.Terminal());

         compileMethodHints(hints, writer, methodScope, false);

         //// if the constructor is embeddable
         //// method hint should be added
         //// the special method should be declared as well
         //if (member == nsConstructor && methodScope.embeddable) {
         //   MethodScope specialMethodScope(&scope);

         //   IdentifierString signature(scope.moduleScope->module->resolveSubject(getSignature(methodScope.message)));
         //   signature.append(EMBEDDED_PREFIX);

         //   specialMethodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(signature, false));

         //   writer.newNode(lxClassMethodOpt, maEmbeddedInit);
         //   writer.appendNode(lxMessage, specialMethodScope.message);
         //   writer.closeNode();
         //   writer.closeNode();

         //   writer.newNode(lxClassMethod, specialMethodScope.message);
         //   writer.closeNode();
         //}
         /*else */writer.closeNode();

         //if (methodScope.generic)
         //   writer.appendNode(lxClassFlag, elWithGenerics);

         //// save extensions if any
         //if (isExtension) {
         //   scope.moduleScope->saveExtension(methodScope.message, extensionType, scope.reference);
         //}

      }
      member = member.nextNode();
   }
}

ref_t Compiler :: generateTemplate(ModuleScope& moduleScope, TemplateInfo& templateInfo, ref_t typeRef, ident_t className)
{
   ref_t reference = 0;
   if (!emptystr(className)) {
      reference = moduleScope.module->mapReference(className);
   }
   else reference = moduleScope.mapNestedExpression();

   ClassScope scope(&moduleScope, reference);

   // link the generated class to the specified type
   if (typeRef != 0)
      moduleScope.saveSubject(typeRef, scope.reference, false);

   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   compileParentDeclaration(DNode(), scope);

   writer.appendNode(lxClassFlag, elSealed);

   templateInfo.targetOffset = scope.info.fields.Count();

   writer.newNode(lxTemplateField);
   writer.appendNode(lxTerminal, TARGET_VAR);
   if (templateInfo.targetType != 0)
      writer.appendNode(lxType, templateInfo.targetType);
   writer.closeNode();

   importTemplate(scope, writer, templateInfo, true);

   writer.closeNode();

   generateClassDeclaration(scope, false);

   // HOTFIX : generate syntax once again to properly import the template code
   writer.clear();
   writer.newNode(lxRoot, scope.reference);

   importTemplate(scope, writer, templateInfo, false);
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
      if (current == lxClassStructure/* || current == lxClassArray*/) {
         generateClassFlags(scope, current);
      }         

      current = current.nextNode();
   }
}

bool Compiler :: declareImportedTemplate(ClassScope& scope, SyntaxWriter& writer, TemplateInfo templateInfo)
{
   _Module* extModule = NULL;
   _Memory* section = scope.moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule);
   if (!section)
      return false;

   SyntaxTree tree(section);
   SNode current = tree.readRoot();
   current = current.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         writer.appendNode(lxClassFlag, current.argument);
         // HOTFIX : import dynamic array template
         if (test(current.argument, elDynamicRole)) {
            writer.newNode(lxTemplateField);
            writer.appendNode(lxTerminal, TARGET_VAR);

            if (templateInfo.targetType != 0)
               writer.appendNode(lxType, templateInfo.targetType);
            writer.closeNode();
         }
      }
      else if (current == lxClassMethod) {
         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(extModule, scope.moduleScope->module, getSignature(current.argument), templateInfo));
           
         generateMethodHints(scope, current, messageRef);
   
         scope.include(messageRef);
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

         int size = (typeHint != 0) ? scope.moduleScope->defineSubjectSize(typeHint) : 0;
         if (sizeHint != 0) {
            if (size < 0) {
               size = sizeHint * (-size);
            }
            else scope.raiseError(errIllegalField, current);
         }

         // a class with a dynamic length structure must have no fields
         if (test(scope.info.header.flags, elDynamicRole)) {
            if (current == lxTemplateField && scope.info.size == 0 && scope.info.fields.Count() == 0) {
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

               // byref variable may have only one field
               if (test(scope.info.header.flags, elWrapper)) {
                  if (scope.info.fields.Count() > 1)
                     scope.raiseError(errIllegalField, current);
               }
            }
         }

         //// check for field templates
         //SNode templ = SyntaxTree::findChild(current, lxFieldTemplate);
         //if (templ != lxNone) {
         //   if (!test(scope.info.header.flags, elStructureRole))
         //      size = 0; // for normal field size should be ignored in the template

         //   declareImportedTemplate(scope, templ, offset, typeHint, size);
         //}
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
      if (current == lxClassMethod) {
         generateMethodHints(scope, current, current.argument);

         int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(current.argument, maHint));

         // check if there is no duplicate method
         if (scope.info.methods.exist(current.argument, true))
            scope.raiseError(errDuplicatedMethod, current);

         bool included = scope.include(current.argument);
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

   // verify fif the class may be a wrapper
   if (test(scope.info.header.flags, elWrapper)) {
      if (scope.info.fields.Count() != 1 || !test(scope.info.header.flags, elSealed)) {
         // wrapper should have only single field and be sealed
         scope.info.header.flags &= ~elWrapper;
      }
      else {
         ref_t fieldClassRef = scope.moduleScope->subjectHints.get(*scope.info.fieldTypes.start());

         int fieldFlags = scope.moduleScope->getClassFlags(fieldClassRef);
         if (isEmbeddable(fieldFlags) && test(scope.info.header.flags, elStructureRole)) {
            // wrapper around embeddable object is imbeddable itself
            scope.info.header.flags |= elEmbeddable;

            if ((scope.info.header.flags & elDebugMask) == 0)
               scope.info.header.flags |= fieldFlags & elDebugMask;
         }
      }
   }

   // define the data type for the array
   if (test(scope.info.header.flags, elDynamicRole) && (scope.info.header.flags & elDebugMask) == 0) {
      if (test(scope.info.header.flags, elStructureRole)) {
         ref_t fieldClassRef = scope.moduleScope->subjectHints.get(scope.info.fieldTypes.get(-1));

         int fieldFlags = scope.moduleScope->getClassFlags(fieldClassRef);
         if ((fieldFlags & elDebugMask) == elDebugDWORD) {
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
      }
      //else scope.info.header.flags |= elDebugArray;
   }

   // generate methods
   generateMethodDeclarations(scope, root, closed);
}

void Compiler :: generateInlineClassDeclaration(ClassScope& scope, bool closed)
{
   generateClassDeclaration(scope, closed);

   //// stateless inline class
   //if (scope.info.fields.Count() == 0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;

   //   // stateless inline class is its own class class
      scope.info.classClassRef = scope.reference;
   //}
   //else scope.info.header.flags &= ~elStateless;
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

void Compiler :: compileTemplateDeclaration(DNode node, TemplateScope& scope, DNode hints)
{
   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   compileTemplateHints(hints, writer, scope);

   DNode member = node.firstChild();

   // load template parameters
   while (member == nsMethodParameter) {
      if (!scope.parameters.exist(member.Terminal())) {
         scope.parameters.add(member.Terminal(), scope.parameters.Count() + 1);
      }
      else scope.raiseError(errDuplicatedDefinition, member.Terminal());

      member = member.nextNode();
   }

   compileVMT(member, writer, scope, false);

   writer.closeNode();

//   // validate template
//   if (scope.templateType == lxMethodTemplate) {
//      ref_t targetMessage = 0;
//      if (validateMethodTemplate(scope.syntaxTree.readRoot(), targetMessage)) {
//         scope.syntaxTree.readRoot().insertNode(lxMessage, targetMessage);
//      }
//      else scope.raiseError(errInvalidSyntax, node.FirstTerminal());
//   }

//   // update template tree
//   if (scope.templateType != lxNone) {
//      scope.syntaxTree.readRoot() = scope.templateType;
//   }
//   else scope.raiseError(errInvalidSyntax, node.FirstTerminal());

   // save declaration
   scope.save();

   scope.moduleScope->saveTemplate(scope.templateRef);
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

   //bool isExtension = false;
   //ref_t extensionType = 0;
   compileClassHints(hints, writer, scope/*, isExtension, extensionType*/);
   compileFieldDeclarations(member, writer, scope);

   // declare imported methods / flags
   TemplateMap::Iterator it = scope.moduleScope->templates.getIt(scope.reference);
   while (!it.Eof() && it.key() == scope.reference) {
      if (!declareImportedTemplate(scope, writer, *it))
         scope.raiseError(errInvalidHint, node.FirstTerminal());

      it++;
   }

   declareVMT(member, writer, scope, nsMethod/*, isExtension, extensionType*/);

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
      if (info.targetOffset >= 0) {
         // if it is an array
         if (test(scope.info.header.flags, elDynamicRole)) {
            writer.newNode(lxThisLocal, 1);

            if (test(scope.info.header.flags, elStructureRole))
               writer.appendNode(lxTarget, -3);
         }
         // if it is a structure field
         else if (test(scope.info.header.flags, elStructureRole)) {
            writer.newNode(lxBoxing);
            writer.appendNode(lxFieldAddress, info.targetOffset);

            writer.appendNode(lxTarget, scope.moduleScope->subjectHints.get(info.targetType));
         }
         else writer.newNode(lxField, info.targetOffset);
      }
      else {
         writer.newNode(lxThisLocal, 1);

         // HOTFIX : recognize primitive types
         switch (scope.info.header.flags & elDebugMask) {
            case elDebugDWORD:
               writer.appendNode(lxTarget, -1); // NOTE : -1 means primitive integer
               break;
            case elDebugQWORD:
               writer.appendNode(lxTarget, -2); // NOTE : -2 means primitive long integer
               break;
            case elDebugReal64:
               writer.appendNode(lxTarget, -4); // NOTE : -4 means primitive real number
               break;
            case elDebugIntegers:
            case elDebugBytes:
            case elDebugShorts:
               writer.appendNode(lxTarget, -3); // NOTE : -3 means primitive array
               break;
            default:
               writer.appendNode(lxTarget, scope.reference);
         }         
      }

//      writer.newNode(info.size != 0? lxFieldAddress : lxField, info.offset);
//
//      if (info.targetRef != 0)
//         writer.appendNode(lxTarget, info.targetRef);
      
      if (info.targetType != 0)
         writer.appendNode(lxType, info.targetType);
   }
//   else if (current.type == lxTemplateCalling) {
//      ref_t message = overwriteSubject(current.argument, info.type);
//      if (test(scope.info.header.flags, elSealed)) {
//         writer.newNode(lxDirectCalling, message);
//      }
//      else if (test(scope.info.header.flags, elClosed)) {
//         writer.newNode(lxSDirctCalling, message);
//      }
//      else writer.newNode(lxCalling, message);
//
//      writer.appendNode(lxCallTarget, scope.reference);
//
//      int methodHint = scope.info.methodHints.get(Attribute(message, maHint));
//
//      if (test(methodHint, tpStackSafe))
//         writer.appendNode(lxStacksafe);
//   }
//   else if (current.type == lxTemplAssigning) {
//      writer.newNode(lxAssigning, info.size);
//
//      if (info.targetRef != 0)
//         writer.appendNode(lxTarget, info.targetRef);
//
//      SNode subNode = current.firstChild();
//      while (subNode != lxNone) {
//         if (info.type != 0 && subNode != lxTemplateField && test(subNode.type, lxObjectMask)) {
//            writer.newNode(lxTypecasting, encodeMessage(info.type, GET_MESSAGE_ID, 0));
//            importNode(scope, subNode, writer, templateModule, info);
//            writer.closeNode();
//         }
//         else importNode(scope, subNode, writer, templateModule, info);
//
//         subNode = subNode.nextNode();
//      }
//
//      writer.closeNode();
//      return;
//   }
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
   else if (test(current.type, lxMessageMask)) {
      ref_t signature = importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info);

//      // HOTFIX : replace generic call with an array operation
//      if (IsReferOperator(getVerb(current.argument)) && signature == 0 && test(scope.info.header.flags, elDynamicRole) 
//         && getFirstObject(current) == lxTemplateTarget)
//      {
//         if (test(scope.info.header.flags, elStructureRole)) {
//            writer.newNode(lxBoxing);
//            writer.appendNode(lxTarget, scope.moduleScope->subjectHints.get(info.targetType));
//            writer.newNode(lxArrOp, current.argument);
//            writer.appendNode(lxSize, -scope.info.size);
//
//            importTree(scope, current, writer, templateModule, info);
//
//            writer.closeNode();
//            writer.closeNode();
//
//            return;
//         }
//         else {
//            writer.newNode(lxArrOp, current.argument);
//
//            writer.appendNode(lxSize, -scope.info.size);
//         }
//      }
      /*else */writer.newNode(current.type, overwriteSubject(current.argument, signature));
   }
   else if (test(current.type, lxReferenceMask)) {
      writer.newNode(current.type, importReference(templateModule, current.argument, scope.moduleScope->module));
   }
   else if (test(current.type, lxSubjectMask)) {
      writer.newNode(current.type, importTemplateSubject(templateModule, scope.moduleScope->module, current.argument, info));
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

void Compiler :: importTemplateTree(ClassScope& scope, SyntaxWriter& writer, SNode node, TemplateInfo& info, _Module* templateModule, bool declaringMode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag && declaringMode) {
         writer.appendNode(lxClassFlag, current.argument);
      }
      else if (current == lxClassMethod) {
         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info));

         if (declaringMode)
            generateMethodHints(scope, current, messageRef);

         writer.newNode(lxClassMethod, messageRef);

         // NOTE : source path reference should be imported
         // but the message name should be overwritten
         writeMessage(*scope.moduleScope, writer, messageRef);

      //   // HOT FIX : if the field is typified provide a method hint
      //   if (current.argument == encodeVerb(GET_MESSAGE_ID)) {
      //      scope.info.methodHints.add(Attribute(messageRef, maType), info.subject);
      //   }

         if (!declaringMode)
            importTree(scope, current, writer, templateModule, info);

         writer.closeNode();
      }

      current = current.nextNode();
   }
}

void Compiler :: importTemplate(ClassScope& scope, SyntaxWriter& writer, TemplateInfo templateInfo, bool declarationMode)
{
   // HOTFIX : if it is a typified class template - set targetType, 
   if (templateInfo.targetOffset == -1 && scope.moduleScope->subjectHints.exist(templateInfo.targetSubject, scope.reference)) {
      templateInfo.targetType = templateInfo.targetSubject;
   }

   _Module* extModule = NULL;
   SyntaxTree tree(scope.moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule));

   SNode root = tree.readRoot();
   importTemplateTree(scope, writer, root, templateInfo, extModule, declarationMode);
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
         if (!scope.info.methods.exist(methodScope.message)) {
            scope.include(methodScope.message);

            compileVirtualTypecastMethod(writer, methodScope, lxThisLocal, 1);
         }
      }
      c_it++;
   }

   // auto generate dispatch handler for wrapper class
   if (test(scope.info.header.flags, elWrapper) && !scope.info.methods.exist(DISPATCH_MESSAGE_ID, true)) {
      //scope.info.header.flags |= elWithGenerics;

      MethodScope methodScope(&scope);
      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);;

      compileVirtualDispatchMethod(writer, methodScope, lxResultField, 0);
   }
}

void Compiler :: compileClassImplementation(DNode node, ClassScope& scope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   SyntaxWriter writer(scope.syntaxTree);
   writer.newNode(lxRoot, scope.reference);

   // import templates
   TemplateMap::Iterator t_it = moduleScope->templates.getIt(scope.reference);
   while (!t_it.Eof() && t_it.key() == scope.reference) {
      importTemplate(scope, writer, *t_it, false);

      t_it++;
   }

   DNode member = node.firstChild();
   compileVMT(member, writer, scope);
   compileVirtualMethods(writer, scope);

   writer.closeNode();

   generateClassImplementation(scope);

   // compile explicit symbol
   compileSymbolCode(scope);
}

void Compiler :: declareSingletonClass(DNode node, DNode parentNode, ClassScope& scope)
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

   declareVMT(node.firstChild(), writer, scope, nsMethod/*, false, 0*/);

   writer.closeNode();

   generateInlineClassDeclaration(scope, test(scope.info.header.flags, elClosed));

   scope.save();
}

void Compiler :: declareSingletonAction(ClassScope& classScope, DNode objNode, DNode expression)
{
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

   compileVMT(member, writer, scope);

   writer.closeNode();

   generateClassImplementation(scope);
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
            declareSingletonClass(classNode.firstChild(), classNode, classScope);
            singleton = true;
         }
         // if it is normal nested class
         else {
            declareSingletonClass(classNode.firstChild(), DNode(), classScope);
            singleton = true;
         }
      }
      else if (objNode == nsSubCode) {
         ClassScope classScope(scope.moduleScope, scope.reference);

         declareSingletonAction(classScope, objNode, DNode());
         singleton = true;
      }
//      else if (objNode == nsInlineExpression) {
//         ClassScope classScope(scope.moduleScope, scope.reference);
//
//         declareSingletonAction(classScope, objNode, expression.firstChild());
//         singleton = true;
//      }
//      else if (objNode == nsSubjectArg || objNode == nsMethodParameter) {
//         ClassScope classScope(scope.moduleScope, scope.reference);
//
//         declareSingletonAction(classScope, objNode, objNode);
//         singleton = true;
//      }
   }

   if (!singleton && (/*scope.typeRef != 0 || */scope.constant)) {
      SymbolExpressionInfo info;
      info.expressionTypeRef = /*scope.typeRef*/0;
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

         compileAction(classNode, classScope, DNode(), 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
//      else if (classNode == nsInlineExpression) {
//         ModuleScope* moduleScope = scope.moduleScope;
//
//         ClassScope classScope(moduleScope, scope.reference);
//         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);
//
//         compileAction(classNode, classScope, expression.firstChild(), 0, true);
//
//         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
//      }
//      else if (classNode == nsSubjectArg || classNode == nsMethodParameter) {
//         ModuleScope* moduleScope = scope.moduleScope;
//
//         ClassScope classScope(moduleScope, scope.reference);
//         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);
//
//         compileAction(goToSymbol(classNode, nsInlineExpression), classScope, classNode, 0, true);
//
//         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
//      }
   }

   // compile symbol into byte codes

   SyntaxWriter writer(scope.syntaxTree);
   // NOTE : top expression is required for propery translation
   writer.newNode(lxRoot, scope.reference);

   CodeScope codeScope(&scope, &writer);
   if (retVal.kind == okUnknown) {
      // compile symbol body, if it is not a singleton
      recordDebugStep(codeScope, expression.FirstTerminal(), dsStep);
      writer.newNode(lxExpression);
      retVal = compileExpression(expression, codeScope, /*scope.typeRef*/0, 0);
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
      //else if (retVal.kind == okCharConstant) {
      //   _Module* module = scope.moduleScope->module;
      //   MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      //   ident_t value = module->resolveConstant(retVal.param);

      //   dataWriter.writeLiteral(value, getlength(value));

      //   dataWriter.Memory()->addReference(scope.moduleScope->charReference | mskVMTRef, (ref_t)-4);

      //   scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->charReference);
      //}
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

void Compiler :: boxPrimitive(ModuleScope& scope, SyntaxTree::Node& node, ref_t targetRef, ref_t targetType, int warningLevel, int mode)
{
   LexicalType opType = node.type;

   int size = 0;
   if (targetType != 0) {
      size = scope.defineSubjectSize(targetType, false);
   }
   else if (targetRef == -1) {
      size = 4;
   }

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
         node.appendNode(lxType, targetType);

         optimizeBoxing(scope, node, warningLevel, 0);

         node = SyntaxTree::findChild(node, lxAssigning);
      }
      else node.appendNode(lxType, targetType);

      node = SyntaxTree::findChild(node, opType);
   }
}

void Compiler :: optimizeExtCall(ModuleScope& scope, SNode node, int warningMask, int mode)
{
   SNode parentNode = node.parentNode();
   while (parentNode == lxExpression)
      parentNode = parentNode.parentNode();

   if (parentNode == lxAssigning && parentNode.argument == 4) {

   }
   else {
      ref_t type = SyntaxTree::findChild(parentNode, lxType).argument;

      boxPrimitive(scope, node, -1, type, warningMask, mode);
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
   boxPrimitive(scope, node, -1, 0, warningMask, mode);

   optimizeSyntaxExpression(scope, node, warningMask, HINT_NOBOXING);
}

void Compiler :: optimizeDirectCall(ModuleScope& scope, SNode node, int warningMask)
{
   int mode = 0;

   bool stackSafe = SyntaxTree::existChild(node, lxStacksafe);

//   if (node == lxDirectCalling && SyntaxTree::existChild(node, lxEmbeddable)) {
//      // check if it is a virtual call
//      if (getVerb(node.argument) == GET_MESSAGE_ID && getParamCount(node.argument) == 0) {
//         SNode callTarget = SyntaxTree::findChild(node, lxCallTarget);
//
//         ClassInfo info;
//         scope.loadClassInfo(info, callTarget.argument);
//         if (info.methodHints.get(Attribute(node.argument, maEmbeddableIdle)) == -1) {
//
//            // if it is an idle call, remove it
//            node = lxExpression;
//
//            return;
//         }
//      }
//   }

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
   if (target.argument != 0) {
      ClassInfo info;
      if (scope.loadClassInfo(info, target.argument)) {
         ref_t dummy;
         int hint = scope.checkMethod(info, node.argument, dummy);
         
         if (hint == tpUnknown) {
            // Compiler magic : allow to call wrapper content directly
            if (test(info.header.flags, elWrapper)) {
               target.setArgument(scope.subjectHints.get(info.fieldTypes.get(0)));

               hint = scope.checkMethod(target.argument, node.argument);

               // for dynamic object, target object should be overridden
               if (!test(info.header.flags, elStructureRole)) {
                  node.appendNode(lxOverridden);
                  SNode n = SyntaxTree::findChild(node, lxOverridden);
                  n.appendNode(lxField);
               }
            }            
         }

         methodNotFound = hint == tpUnknown;
         switch (hint & tpMask) {
            case tpSealed:
               stackSafe = test(hint, tpStackSafe);
               node = lxDirectCalling;
               break;
            case tpClosed:
               stackSafe = test(hint, tpStackSafe);
               node = lxSDirctCalling;
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
         scope.raiseWarning(WARNING_LEVEL_3, wrnUnknownMessage, row.argument, col.argument, terminal.identifier());
      }
   }
}

int Compiler :: mapOpArg(ModuleScope& scope, SNode arg)
{
   int flags = 0;

   ref_t ref = SyntaxTree::findChild(arg, lxTarget).argument;
   ref_t type = SyntaxTree::findChild(arg, lxType).argument;
   if (isPrimitiveRef(ref) || ref == 0) {
      flags = scope.getClassFlags(scope.subjectHints.get(type));
   }
   else flags = scope.getClassFlags(ref);

   return flags & elDebugMask;
}

void Compiler :: optimizeOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
{   
   if (node.argument == SET_REFER_MESSAGE_ID) {
      SNode larg, narg, rarg;
      assignOpArguments(node, larg, narg, rarg);

      int nflags = mapOpArg(scope, narg);

      ref_t lref = SyntaxTree::findChild(larg, lxTarget).argument;
      if (isPrimitiveRef(lref)) {
         if (lref == -3 && nflags == elDebugDWORD) {
            ref_t destType = SyntaxTree::findChild(larg, lxType).argument;
            if (checkIfCompatible(scope, destType, rarg)) {
               int size = scope.defineSubjectSize(destType);
               node.appendNode(lxSize, size);
               switch (size)
               {
                  case 4:
                     node = lxIntArrOp;
                     break;
                  case 1:
                     node = lxByteArrOp;
                     break;
                  case 2:
                     node = lxShortArrOp;
                     break;
                  default:
                     break;
               }
            }
         }
      }

      if (node == lxOp) {
         node.setArgument(encodeMessage(0, node.argument, 2));
         node = lxCalling;
      }
      else {
         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
         optimizeSyntaxNode(scope, narg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
      }
   }
   else {
      bool boxing = false;
      SNode larg, rarg;
      assignOpArguments(node, larg, rarg);

      ref_t destType = SyntaxTree::findChild(larg, lxType).argument;

      int lflags = mapOpArg(scope, larg);
      int rflags = mapOpArg(scope, rarg);

      ref_t lref = SyntaxTree::findChild(larg, lxTarget).argument;
      if (isPrimitiveRef(lref)) {
         if (IsNumericOperator(node.argument)) {
            if (lref == -1 && lflags == elDebugDWORD && rflags == elDebugDWORD) {
               node = lxIntOp;
               boxing = true;
            }
            else if (lref == -2 && lflags == elDebugQWORD && rflags == elDebugQWORD) {
               node = lxLongOp;
               boxing = true;
            }
            else if (lref == -4 && lflags == elDebugReal64 && rflags == elDebugReal64) {
               node = lxRealOp;
               boxing = true;
            }
         }
         else if (IsReferOperator(node.argument)) {
            if (lref == -3 && rflags == elDebugDWORD) {
               lref = scope.subjectHints.get(destType);
               int size = scope.defineStructSize(lref);
               node.appendNode(lxSize, size);
               switch (size)
               {
                  case 4:
                     node = lxIntArrOp;
                     break;
                  case 1:
                     node = lxByteArrOp;
                     break;
                  case 2:
                     node = lxShortArrOp;
                     break;
                  default:
                     break;
               }
               boxing = true;
            }
         }
      }

      if (node == lxOp) {
         node.setArgument(encodeMessage(0, node.argument, 1));
         node = lxCalling;

         optimizeSyntaxExpression(scope, node, warningLevel);
      }
      else {
         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);

         if (boxing)
            boxPrimitive(scope, node, lref, destType, warningLevel, mode);
      }
   }
}

void Compiler :: optimizeArrOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
{
//   SNode sizeNode = SyntaxTree::findChild(node, lxSize);
//   if (sizeNode.argument == 4) {
//      node = lxIntArrOp;
//   }
//
//   boxPrimitive(scope, node, mode);
//
//   bool first = true;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (test(current.type, lxObjectMask)) {
//         if (!first) {
//            optimizeSyntaxNode(scope, current, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//         }
//         else {
//            optimizeSyntaxNode(scope, current, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//
//            first = false;
//         }
//      }
//      current = current.nextNode();
//   }
}

//void Compiler :: optimizeEmbeddableCall(ModuleScope& scope, SNode& assignNode, SNode& callNode)
//{
//   SNode callTarget = SyntaxTree::findChild(callNode, lxCallTarget);
//
//   ClassInfo info;
//   scope.loadClassInfo(info, callTarget.argument);
//
//   ref_t subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddableGet));
//   // if it is possible to replace get&subject operation with eval&subject2:local
//   if (subject != 0) {
//      // removing assinging operation
//      assignNode = lxExpression;
//
//      // move assigning target into the call node
//      SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
//      if (assignTarget != lxNone) {
//         callNode.appendNode(assignTarget.type, assignTarget.argument);
//         assignTarget = lxExpression;
//         callNode.setArgument(encodeMessage(subject, EVAL_MESSAGE_ID, 1));
//      }
//   }
//
//   subject = info.methodHints.get(Attribute(callNode.argument, maEmbeddedInit));
//   // if it is possible to replace constructor call with embeddaded initialization without creating a temporal dynamic object
//   if (subject != 0) {
//      // move assigning target into the call node
//      SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
//      SNode callTarget = callNode.findPattern(SNodePattern(lxConstantClass));
//      if (callTarget != lxNone && assignTarget != lxNone) {
//         // removing assinging operation
//         assignNode = lxExpression;
//
//         // move assigning target into the call node
//         callTarget = assignTarget.type;
//         callTarget.setArgument(assignTarget.argument);
//         assignTarget = lxExpression;
//
//         callNode.setArgument(overwriteSubject(callNode.argument, subject));
//      }
//   }
//}

void Compiler :: optimizeAssigning(ModuleScope& scope, SNode node, int warningLevel)
{
   int mode = HINT_NOUNBOXING;
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
               }
               
               current = subNode.type;
               current.setArgument(subNode.argument);
            }
         }
         else optimizeSyntaxNode(scope, current, warningLevel, mode);
      }
      current = current.nextNode();
   }

//   if (node.argument != 0) {
//      SNode directCall = findSubNode(node, lxDirectCalling);
//      if (directCall != lxNone && SyntaxTree::existChild(directCall, lxEmbeddable)) {
//         optimizeEmbeddableCall(scope, node, directCall);
//      }
//   }

   //// assignment operation
   //SNode assignNode = findSubNode(node, lxAssigning);
   //if (assignNode != lxNone) {
   //   SNode operationNode = SyntaxTree::findChild(assignNode, lxIntOp, lxRealOp);
   //   if (operationNode != lxNone) {
   //      SNode larg = findSubNodeMask(operationNode, lxObjectMask);
   //      SNode target = SyntaxTree::findMatchedChild(node, lxObjectMask);
   //      if (larg.type == target.type && larg.argument == target.argument) {
   //         // remove an extra assignment
   //         larg = findSubNodeMask(assignNode, lxObjectMask);

   //         larg = target.type;
   //         larg.setArgument(target.argument);
   //         node = lxExpression;
   //         target = lxExpression;
   //      }
   //   }
   //}
}

void Compiler :: defineTargetSize(ModuleScope& scope, SNode& node)
{
   SNode target = SyntaxTree::findChild(node, lxTarget);

   // HOT FIX : box / assign primitive structures
   if (isPrimitiveRef(target.argument)) {
      ref_t type = SyntaxTree::findChild(node, lxType).argument;
      int size = scope.defineSubjectSize(type, false);

      if (target.argument == -3) {
         node.setArgument(-size);
      }
      else node.setArgument(size);

      target.setArgument(scope.subjectHints.get(type));
   }
   else if (node.argument == 0)
      node.setArgument(scope.defineStructSize(target.argument));
}

void Compiler :: optimizeBoxing(ModuleScope& scope, SNode node, int warningLevel, int mode)
{
   bool boxing = true;

   SNode target = SyntaxTree::findChild(node, lxTarget);

   defineTargetSize(scope, node);

   // if no boxing hint provided
   // then boxing should be skipped
   if (test(mode, HINT_NOBOXING)) {
      boxing = false;
   }

   // ignore boxing operation if allowed
   if (!boxing)
      node = lxExpression;

   optimizeSyntaxExpression(scope, node, warningLevel, HINT_NOBOXING);

   if (boxing && test(warningLevel, WARNING_LEVEL_3)) {
      SNode row = SyntaxTree::findChild(node, lxRow);
      SNode col = SyntaxTree::findChild(node, lxCol);
      SNode terminal = SyntaxTree::findChild(node, lxTerminal);
      if (col != lxNone && row != lxNone) {
         scope.raiseWarning(WARNING_LEVEL_3, wrnBoxingCheck, row.argument, col.argument, terminal.identifier());
      }
   }
}

bool Compiler :: checkIfImplicitBoxable(ModuleScope& scope, ref_t sourceClassRef, ClassInfo& targetInfo)
{
   if (sourceClassRef == -1 && (targetInfo.header.flags & elDebugMask) == elDebugDWORD)
   {
      return true;
   }
   else if (sourceClassRef != 0 && scope.subjectHints.exist(targetInfo.fieldTypes.get(0), sourceClassRef)) {
      return true;
   }
   else return false;
}

void Compiler :: optimizeTypecast(ModuleScope& scope, SNode node, int warningMask, int mode)
{
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
      else if (object == lxExternalCall || object == lxStdExternalCall || object == lxCoreAPICall) {
         optimizeExtCall(scope, object, warningMask, mode);

         object = SyntaxTree::findMatchedChild(node, lxObjectMask);

         optimized = true;
      }

      if (!checkIfCompatible(scope, targetType, object)) {
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
         
            // HOT FIX : trying to typecast primitive array
            if (sourceClassRef == -3) {
               if (test(targetInfo.header.flags, elStructureRole | elDynamicRole) && targetInfo.fieldTypes.get(-1) == sourceType) {
                  // if boxing is not required (stack safe) and can be passed directly
                  if (test(mode, HINT_NOBOXING)) {
                     node = lxExpression;
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
            else if (test(targetInfo.header.flags, elStructureWrapper | elEmbeddable)) {
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
               if (isPrimitiveRef(sourceClassRef) && sourceType != 0)
                  sourceClassRef = scope.subjectHints.get(sourceType);

               ClassInfo sourceInfo;
               scope.loadClassInfo(sourceInfo, sourceClassRef, false);
               // if source is target wrapper (i.e. source is a target container)
               if (test(sourceInfo.header.flags, elStructureWrapper | elEmbeddable) && scope.subjectHints.exist(sourceInfo.fieldTypes.get(0), targetClassRef)) {
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
         }
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

   if (!optimized)
      optimizeSyntaxExpression(scope, node, warningMask, typecastMode);

   if (test(warningMask, WARNING_LEVEL_2) && typecasted) {
      SNode row = SyntaxTree::findChild(node, lxRow);
      SNode col = SyntaxTree::findChild(node, lxCol);
      SNode terminal = SyntaxTree::findChild(node, lxTerminal);
      if (col != lxNone && row != lxNone)
         scope.raiseWarning(WARNING_LEVEL_2, wrnTypeMismatch, row.argument, col.argument, terminal.identifier());
   }
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

//void Compiler :: optimizeBoxableObject(ModuleScope& scope, SNode node, int warningLevel, int mode)
//{
//   if (!test(mode, HINT_NOBOXING) || (node == lxFieldAddress && node.argument > 0)) {
//      SNode target = SyntaxTree::findChild(node, lxTarget);
//      ClassInfo info;
//      scope.loadClassInfo(info, target.argument, true);
//
//      int size = 0;
//      bool variable = false;
//      //   int size = (target.argument == scope.paramsReference) ? -1 : scope.defineStructSize(target.argument, variable);
//      if (isEmbeddable(info)) {
//         size = info.size;
//
//         variable = !test(info.header.flags, elReadOnlyRole);
//      }
//      if (size == 0)
//         return;
//
//   //   // allocating temporal stack allocated variable for structure field if required
//   //   if (node == lxFieldAddress && node.argument > 0) {
//   //      int offset = allocateStructure(scope, node, size);
//
//   //      // injecting assinging, boxing / unboxing operation
//   //      LexicalType nodeType = lxAssigning;
//   //      SNode assignNode = node;
//   //      if (test(mode, HINT_NOBOXING)) {
//   //         if (variable && !test(mode, HINT_NOUNBOXING)) {
//   //            node.appendNode(lxAssigning, size);
//   //            assignNode = SyntaxTree::findChild(node, lxAssigning);
//
//   //            nodeType = lxLocalUnboxing;
//   //         }
//   //      }
//   //      else {
//   //         node.appendNode(lxAssigning, size);
//   //         assignNode = SyntaxTree::findChild(node, lxAssigning);
//
//   //         nodeType = variable && !test(mode, HINT_NOUNBOXING) ? lxUnboxing : lxBoxing;
//   //      }
//   //      assignNode.appendNode(lxLocalAddress, offset);
//   //      assignNode.appendNode(lxFieldAddress, node.argument);
//
//   //      node = nodeType;
//   //      node.setArgument(size);
//   //   }
//   //   else {
//         // inject boxing node
//         node.appendNode(node.type, node.argument);
//
//         if (node == lxBlockLocalAddr && size == -1) {
//            node = lxArgBoxing;
//         }
//         else if (variable && !test(mode, HINT_NOUNBOXING)) {
//            node = lxUnboxing;
//         }
//         else node = (node == lxBoxableLocal) ? lxCondBoxing : lxBoxing;
//
//         node.setArgument(size);
//   //   }
//   }
//
//   if (node == lxBoxing || node == lxUnboxing || node == lxLocalUnboxing) {
//      optimizeBoxing(scope, node, warningLevel);
//   }
//}

void Compiler :: optimizeSyntaxNode(ModuleScope& scope, SyntaxTree::Node current, int warningMask, int mode)
{
   switch (current.type) {
      case lxAssigning:
         optimizeAssigning(scope, current, warningMask);
         break;
      case lxTypecasting:
         optimizeTypecast(scope, current, warningMask, mode);
         break;
      case lxArrOp:
         optimizeArrOp(scope, current, warningMask, mode);
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
         optimizeSyntaxExpression(scope, current, warningMask, HINT_NOUNBOXING);
         break;
      case lxBoxing:
      case lxCondBoxing:
         optimizeBoxing(scope, current, warningMask, mode);
         break;
      case lxOp:
         optimizeOp(scope, current, warningMask, mode);
         break;
      case lxDirectCalling:
      case lxSDirctCalling:
         optimizeDirectCall(scope, current, warningMask);
         break;
      case lxCalling:
         optimizeCall(scope, current, warningMask);
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

         //if (test(_optFlag, 1)) {
         //   if (test(scope.info.methodHints.get(ClassInfo::Attribute(current.argument, maHint)), tpEmbeddable)) {
         //      defineEmbeddableAttributes(scope, current);
         //   }
         //}
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

//bool Compiler :: recognizeEmbeddableGet(ModuleScope& scope, SyntaxTree& tree, SNode root, ref_t returningType, ref_t& subject)
//{
//   //if (returningType != 0 && scope.defineTypeSize(returningType) > 0) {
//   //   root = SyntaxTree::findChild(root, lxNewFrame);
//
//   //   if (tree.matchPattern(root, lxObjectMask, 2,
//   //         SNodePattern(lxExpression),
//   //         SNodePattern(lxReturning))) 
//   //   {
//   //      SNode message = tree.findPattern(root, 2,
//   //         SNodePattern(lxExpression),
//   //         SNodePattern(lxDirectCalling, lxSDirctCalling));
//
//   //      // if it is eval&subject2:var[1] message
//   //      if (getParamCount(message.argument) != 1)
//   //         return false;
//
//   //      // check if it is operation with $self
//   //      SNode target = tree.findPattern(root, 3,
//   //         SNodePattern(lxExpression),
//   //         SNodePattern(lxDirectCalling, lxSDirctCalling),
//   //         SNodePattern(lxLocal, lxExpression));
//
//   //      // if the target was optimized
//   //      if (target == lxExpression) {
//   //         target = SyntaxTree::findChild(target, lxLocal);
//   //      }
//
//   //      if (target == lxNone || target.argument != 1)
//   //         return false;
//
//   //      // check if the argument is returned
//   //      SNode arg = tree.findPattern(root, 4,
//   //         SNodePattern(lxExpression),
//   //         SNodePattern(lxDirectCalling, lxSDirctCalling),
//   //         SNodePattern(lxExpression),
//   //         SNodePattern(lxLocalAddress));
//
//   //      SNode ret = tree.findPattern(root, 3,
//   //         SNodePattern(lxReturning),
//   //         SNodePattern(lxBoxing),
//   //         SNodePattern(lxLocalAddress));
//
//   //      if (arg != lxNone && ret != lxNone && arg.argument == ret.argument) {
//   //         subject = getSignature(message.argument);
//
//   //         return true;
//   //      }
//   //   }
//   //}
//
//   return false;
//}
//
//bool Compiler :: recognizeEmbeddableIdle(SyntaxTree& tree, SNode methodNode)
//{
//   SNode object = tree.findPattern(methodNode, 4,
//      SNodePattern(lxNewFrame),
//      SNodePattern(lxReturning),
//      SNodePattern(lxExpression),
//      SNodePattern(lxLocal));
//
//   if (object == lxNone) {
//      object = tree.findPattern(methodNode, 3,
//         SNodePattern(lxNewFrame),
//         SNodePattern(lxReturning),
//         SNodePattern(lxLocal));
//   }
//
//   return (object == lxLocal && object.argument == -1);
//}
//
//void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
//{
//   // Optimization : var = get&subject => eval&subject2:var[1]
//   ref_t type = 0;
//   ref_t returnType = classScope.info.methodHints.get(ClassInfo::Attribute(methodNode.argument, maType));
//   if (recognizeEmbeddableGet(*classScope.moduleScope, *methodNode.Tree(), methodNode, returnType, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGet), type);
//   }
//
//   // Optimization : subject'get = self
//   if (recognizeEmbeddableIdle(*methodNode.Tree(), methodNode)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableIdle), -1);
//   }
//}

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

void Compiler :: compileSubject(DNode& member, ModuleScope& scope, DNode hints)
{
   bool internalSubject = member.Terminal().symbol == tsPrivate;

   // map a full type name
   ref_t subjRef = scope.mapNewSubject(member.Terminal());
   ref_t classRef = 0;

   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      // obsolete, body should be used
      if (StringHelper::compare(terminal, HINT_WRAPPER)) {
         TerminalInfo value = hints.select(nsHintValue).Terminal();
         ref_t classRef = scope.mapTerminal(value);
         if (classRef == 0)
            scope.raiseError(errUnknownSubject, value);

         scope.validateReference(value, classRef);
      }
      else {
         ref_t hintRef = scope.mapSubject(terminal, false);
         if (hintRef != 0) {
            TemplateInfo templateInfo;
            templateInfo.templateRef = hintRef;

            TerminalInfo target = hints.firstChild().Terminal();
            templateInfo.targetType = scope.mapSubject(target);
            templateInfo.targetSubject = templateInfo.targetType;
            if (templateInfo.targetSubject == 0)
               templateInfo.targetSubject = scope.module->mapSubject(target, false);

            ReferenceNs name(scope.module->Name(), scope.module->resolveSubject(hintRef));
            name.append('@');
            name.append(scope.module->resolveSubject(templateInfo.targetSubject));

            classRef = generateTemplate(scope, templateInfo, subjRef, name);
            if (classRef == 0)
               scope.raiseError(errInvalidHint, terminal);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
      }      

      hints = hints.nextNode();
   }

   DNode body = goToSymbol(member.firstChild(), nsForward);
   if (body != nsNone) {
      TerminalInfo terminal = body.Terminal();

      if (classRef != 0)
         scope.raiseError(errInvalidSyntax, terminal);

      classRef = scope.mapTerminal(terminal);
      if (classRef == 0)
         scope.raiseError(errUnknownSubject, terminal);
   }

   scope.saveSubject(subjRef, classRef, internalSubject);
}

void Compiler::compileDeclarations(DNode member, ModuleScope& scope)
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
         {
            ref_t templateRef = scope.mapNewSubject(name);

            // check for duplicate declaration
            if (scope.module->mapSection(templateRef | mskSyntaxTreeRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(templateRef | mskSyntaxTreeRef, false);

            // compile class
            TemplateScope classScope(&scope, templateRef);
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
