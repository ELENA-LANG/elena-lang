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
#include "derivation.h"
//#include <errno.h>

using namespace _ELENA_;

#define INVALID_REF (size_t)-1

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- ModuleInfo ---
struct ModuleInfo
{
   _Module* codeModule;
   _Module* debugModule;

   ModuleInfo()
   {
      codeModule = debugModule = NULL;
   }

   ModuleInfo(_Module* codeModule, _Module* debugModule)
   {
      this->codeModule = codeModule;
      this->debugModule = debugModule;
   }
};

// --- Hint constants ---
#define HINT_MASK             0xFFFFF000

#define HINT_ROOT             0x80000000
#define HINT_NOBOXING         0x40000000
#define HINT_NOUNBOXING       0x20000000
//#define HINT_EXTERNALOP       0x10000000
#define HINT_NOCONDBOXING     0x08000000
#define HINT_EXTENSION_MODE   0x04000000
//#define HINT_TRY_MODE         0x02000000
#define HINT_LOOP             0x01000000
#define HINT_SWITCH           0x00800000
#define HINT_NODEBUGINFO      0x00020000
//#define HINT_ACTION           0x00020000
//#define HINT_ALTBOXING        0x00010000
#define HINT_CLOSURE          0x00008000
#define HINT_ASSIGNING        0x00004000
//#define HINT_CONSTRUCTOR_EPXR 0x00002000
//#define HINT_VIRTUAL_FIELD    0x00001000

typedef Compiler::ObjectInfo ObjectInfo;       // to simplify code, ommiting compiler qualifier
//typedef Compiler::ObjectKind ObjectKind;
typedef ClassInfo::Attribute Attribute;

// --- Auxiliary routines ---

inline bool isConstant(ObjectInfo object)
{
   switch (object.kind) {
      case Compiler::okConstantSymbol:
      case Compiler::okConstantClass:
      case Compiler::okLiteralConstant:
      case Compiler::okWideLiteralConstant:
      case Compiler::okCharConstant:
      case Compiler::okIntConstant:
      case Compiler::okLongConstant:
      case Compiler::okRealConstant:
      case Compiler::okMessageConstant:
      case Compiler::okExtMessageConstant:
      case Compiler::okSignatureConstant:
      case Compiler::okVerbConstant:
      case Compiler::okArrayConst:
         return true;
      default:
         return false;
   }
}

inline bool isCollection(SNode node)
{
   return (node == lxExpression && node.nextNode() == lxExpression);
}

inline bool isReturnExpression(SNode expr)
{
   return (expr == lxExpression && expr.nextNode() == lxNone);
}

//inline bool isSingleObject(DNode expr)
//{
//   return (expr == nsObject) && (expr.firstChild().nextNode() == nsNone);
//}

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

//inline ref_t importConstant(_Module* exporter, ref_t exportRef, _Module* importer)
//{
//   if (exportRef) {
//      ident_t reference = exporter->resolveConstant(exportRef);
//
//      return importer->mapConstant(reference);
//   }
//   else return 0;
//}

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
//
//// skip the hints and return the first hint node or none
//inline DNode skipHints(DNode& node)
//{
//   DNode hints;
//   if (node == nsHint)
//      hints = node;
//
//   while (node == nsHint)
//      node = node.nextNode();
//
//   return hints;
//}
//
//inline bool findSymbol(DNode node, Symbol symbol)
//{
//   while (node != nsNone) {
//      if (node==symbol)
//         return true;
//
//      node = node.nextNode();
//   }
//   return false;
//}
//
//inline bool countSymbol(DNode node, Symbol symbol)
//{
//   int counter = 0;
//   DNode current = node.firstChild();
//   while (current != nsNone) {
//      if (current == symbol)
//         counter++;
//
//      current = current.nextNode();
//   }
//   return counter;
//}
//
//inline DNode goToSymbol(DNode node, Symbol symbol)
//{
//   while (node != nsNone) {
//      if (node==symbol)
//         return node;
//
//      node = node.nextNode();
//   }
//   return node;
//}
//
//inline DNode goToSymbol(DNode node, Symbol symbol1, Symbol symbol2)
//{
//   while (node != nsNone) {
//      if (node == symbol1 || node == symbol2)
//         return node;
//
//      node = node.nextNode();
//   }
//   return node;
//}

inline bool isImportRedirect(SNode node)
{
   SNode terminal = node.firstChild(lxObjectMask);
   if (terminal == lxReference) {
      if (terminal.identifier().compare(INTERNAL_MASK, INTERNAL_MASK_LEN))
         return true;
   }
   return false;
}

//inline bool IsVarOperator(int operator_id)
//{
//   switch (operator_id) {
//      case APPEND_MESSAGE_ID:
//      case REDUCE_MESSAGE_ID:
//      case INCREASE_MESSAGE_ID:
//      case SEPARATE_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}
//
//inline bool IsNumericOperator(int operator_id)
//{
//   switch (operator_id) {
//      case ADD_MESSAGE_ID:
//      case SUB_MESSAGE_ID:
//      case MUL_MESSAGE_ID:
//      case DIV_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}
//
//inline bool IsBitwiseOperator(int operator_id)
//{
//   switch (operator_id) {
//      case AND_MESSAGE_ID:
//      case OR_MESSAGE_ID:
//      case XOR_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}
//
//
//inline bool IsShiftOperator(int operator_id)
//{
//   switch (operator_id) {
//      case READ_MESSAGE_ID:
//      case WRITE_MESSAGE_ID:
//      case XOR_MESSAGE_ID:
//         return true;
//      default:
//         return false;
//   }
//}
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
//
//inline bool IsReferOperator(int operator_id)
//{
//   return operator_id == REFER_MESSAGE_ID || operator_id == SET_REFER_MESSAGE_ID;
//}
//
//inline bool IsDoubleOperator(int operator_id)
//{
//   return operator_id == SET_REFER_MESSAGE_ID;
//}

//inline bool isTemplateRef(ref_t reference)
//{
//   return reference == -6;
//}
//
//// returns true if the stack allocated object described by the flag may be passed directly - be stacksafe
//inline bool isStacksafe(int flags)
//{
//   return test(flags, elStructureRole | elEmbeddable) | test(flags, elStructureRole | elEmbeddableWrapper);
//}
//
//inline bool isEmbeddable(int flags)
//{
//   return test(flags, elStructureRole | elEmbeddable);
//}
//
//inline bool isEmbeddable(ClassInfo& localInfo)
//{
//   if (isEmbeddable(localInfo.header.flags)) {
//      return true;
//   }
//
//   return false;
//}
//
//inline bool isDWORD(int flags)
//{
//   return (isEmbeddable(flags) && (flags & elDebugMask) == elDebugDWORD);
//}
//
//inline bool isPTR(int flags)
//{
//   return (isEmbeddable(flags) && (flags & elDebugMask) == elDebugPTR);
//}
//
//void appendTerminalInfo(SyntaxWriter* writer, TerminalInfo terminal)
//{
//   writer->appendNode(lxCol, terminal.Col());
//   writer->appendNode(lxRow, terminal.Row());
//   writer->appendNode(lxLength, terminal.length);
//   writer->appendNode(lxTerminal, terminal.value);
//}
//
//inline int importTemplateSubject(_Module* sour, _Module* dest, ref_t sign_ref, Compiler::TemplateInfo& info)
//{
//   if (sign_ref == 0)
//      return 0;
//
//   ident_t signature = sour->resolveSubject(sign_ref);
//
//   // if the target subject should be overridden
//   int index = StringHelper::find(signature, TARGET_POSTFIX);
//   if (index >= 0) {
//      IdentifierString newSignature;
//      while (index >= 0) {
//         newSignature.append(signature, index);
//         int param_index = signature[index + strlen(TARGET_POSTFIX)] - '0';
//         newSignature.append(dest->resolveSubject(info.parameters.get(param_index)));
//
//         signature += index + getlength(TARGET_POSTFIX);
//         index = StringHelper::find(signature, TARGET_POSTFIX);
//      }
//      newSignature.append(signature + 1);
//
//      return dest->mapSubject(newSignature, false);
//   }
//   else return importSubject(sour, sign_ref, dest);
//}

SNode findTerminalInfo(SNode node)
{
   if (node.findChild(lxTerminal))
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

//struct CoordinateInfo
//{
//   int col;
//   int row;
//};
//
//void filterCoordinates(SNode node, void* param)
//{
//   if (node == lxCol || node == lxRow) {
//      CoordinateInfo* info = (CoordinateInfo*)param;
//
//      node.setArgument(node == lxCol ? info->col : info->row);
//   }
//}

// --- Compiler::ModuleScope ---

Compiler::ModuleScope :: ModuleScope(_ProjectManager* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved)
   : constantHints(INVALID_REF), extensions(NULL, freeobj)
{
   this->project = project;
   this->sourcePath = sourcePath;
   this->module = module;
   this->debugModule = debugModule;
   this->sourcePathRef = -1;

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
   paramsReference = mapReference(project->resolveForward(PARAMS_FORWARD));
//   trueReference = mapReference(project->resolveForward(TRUE_FORWARD));
//   falseReference = mapReference(project->resolveForward(FALSE_FORWARD));
   arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));
   boolReference = mapReference(project->resolveForward(BOOL_FORWARD));

   // HOTFIX : package section should be created if at least literal class is declated
   if (literalReference != 0) {
      packageReference = module->mapReference(ReferenceNs(module->Name(), PACKAGE_SECTION));
   }
   else packageReference = 0;

   defaultNs.add(module->Name());

   loadModuleInfo(module);
}

ref_t Compiler::ModuleScope :: getBaseLazyExpressionClass()
{
   return mapReference(project->resolveForward(LAZYEXPR_FORWARD));
}

ObjectInfo Compiler::ModuleScope :: mapObject(SNode identifier)
{
   ident_t terminal = identifier.findChild(lxTerminal).identifier();

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

ref_t Compiler::ModuleScope :: mapNewAttribute(ident_t terminal)
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
      if (subj_ref && (!explicitOnly || attributeHints.exist(subj_ref))) {
         attributes.add(identifier, subj_ref);

         return subj_ref;
      }
      it++;
   }

   return 0;
}

ref_t Compiler::ModuleScope :: mapAttribute(SNode terminal, bool explicitOnly)
{
   ident_t identifier = NULL;
   if (terminal.type == lxIdentifier || terminal.type == lxPrivate) {
      identifier = terminal.findChild(lxTerminal).identifier();
   }
   else raiseError(errInvalidSubject, terminal);

   return resolveAttributeRef(identifier, explicitOnly);
}

ref_t Compiler::ModuleScope :: mapAttribute(SNode terminal, IdentifierString& output)
{
   ident_t identifier = terminal.findChild(lxTerminal).identifier();

   // add a namespace for the private message
   if (terminal.type == lxPrivate) {
      output.append(project->Namespace());
      output.append(identifier);

      return 0;
   }

   ref_t subjRef = mapAttribute(terminal);
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
   ident_t identifier = terminal.findChild(lxTerminal).identifier();
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

//ref_t Compiler::ModuleScope :: mapNestedTemplate()
//{
//   // otherwise auto generate the name
//   ReferenceNs name(module->Name(), INLINE_POSTFIX);
//
//   findUninqueSubject(module, name);
//
//   return module->mapSubject(name, false);
//}

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
               if (symbolInfo.listRef != 0) {
                  return ObjectInfo(okArrayConst, symbolInfo.listRef, attributeHints.get(symbolInfo.expressionTypeRef), symbolInfo.expressionTypeRef);
               }
               else return ObjectInfo(okConstantSymbol, reference, attributeHints.get(symbolInfo.expressionTypeRef), symbolInfo.expressionTypeRef);
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

//ref_t Compiler::ModuleScope ::mapSubject(ident_t reference, bool existing)
//{
//   if (emptystr(reference))
//      return 0;
//
//   return module->mapSubject(reference, existing);
//}

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
         if (test(key.value2, maSubjectMask))
            value = importSubject(exporter, value, module);

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

////bool Compiler::ModuleScope :: recognizePrimitive(ident_t name, ident_t value, size_t& roleMask, int& size)
////{
////   if (StringHelper::compare(name, HINT_INTEGER_NUMBER)) {
////      if (StringHelper::compare(value, "1")) {         
////         roleMask = elDebugDWORD;
////         size = 1;
////      }
////      else if (StringHelper::compare(value, "2")) {
////         roleMask = elDebugDWORD;
////         size = 2;
////      }
////      else if (StringHelper::compare(value, "4")) {
////         roleMask = elDebugDWORD;
////         size = 4;
////      }
////      else if (StringHelper::compare(value, "8")) {
////         roleMask = elDebugQWORD;
////         size = 8;
////      }
////      else return false;
////
////      return true;
////   }
////
////   return false;
////}
//
//int Compiler::ModuleScope :: defineSubjectSizeEx(ref_t type_ref, bool& variable, bool embeddableOnly)
//{
//   if (type_ref == 0)
//      return 0;
//
//   ref_t classReference = subjectHints.get(type_ref);
//   if (classReference != 0) {
//      return defineStructSizeEx(classReference, variable, embeddableOnly);
//   }
//   else return 0;
//}
//
//int Compiler::ModuleScope :: getClassFlags(ref_t reference)
//{
//   if (reference == 0)
//      return 0;
//
//   ClassInfo classInfo;
//   if(loadClassInfo(classInfo, module->resolveReference(reference), true) == 0)
//      return 0;
//
//   return classInfo.header.flags;
//}

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
   ident_t identifier = terminal.findChild(lxTerminal).identifier();

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
      bool owner = module == extModule;

      ReferenceNs sectionName(extModule->Name(), ATTRIBUTE_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t subj_ref = importSubject(extModule, metaReader.getDWord(), module);
            ref_t class_ref = metaReader.getDWord();
            if (class_ref != INVALID_REF) {
               class_ref = importReference(extModule, class_ref, module);

               if (owner && class_ref != 0)
                  typifiedClasses.add(class_ref, subj_ref);
            }

            attributeHints.add(subj_ref, class_ref);
         }
      }
   }
}

void Compiler::ModuleScope :: loadExtensions(_Module* extModule)
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
            //else raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, terminal);
         }
      }
   }
}

void Compiler::ModuleScope :: saveAttribute(ref_t attrRef, ref_t classReference, bool internalType)
{
   if (!internalType) {
      ReferenceNs sectionName(module->Name(), ATTRIBUTE_SECTION);

      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));

      metaWriter.writeDWord(attrRef);
      metaWriter.writeDWord(classReference);
   }

   if (classReference != 0 && classReference != INVALID_REF)
      typifiedClasses.add(classReference, attrRef);

   attributeHints.add(attrRef, classReference, true);
}

//void Compiler::ModuleScope :: saveTemplate(ref_t template_ref)
//{
//   ReferenceNs sectionName(module->Name(), TYPE_SECTION);
//
//   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
//
//   metaWriter.writeDWord(template_ref);
//   metaWriter.writeDWord(-1); // -1 indicates that it is a template declaration
//}

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
//   preloaded = false;
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
      int offset = info.fields.get(terminal);
      if (offset >= 0) {
         ClassInfo::FieldInfo fieldInfo = info.fieldTypes.get(offset);
         if (test(info.header.flags, elStructureRole)) {
            return ObjectInfo(okFieldAddress, offset, fieldInfo.value1, fieldInfo.value2);
         }
         return ObjectInfo(okField, offset, fieldInfo.value1, fieldInfo.value2);
      }
      else if (offset == -2 && test(info.header.flags, elDynamicRole)) {
         return ObjectInfo(okThisParam, 1, -2, info.fieldTypes.get(-1).value2);
      }
      else {
         ClassInfo::FieldInfo staticInfo = info.statics.get(terminal);
         if (staticInfo.value1 != 0) {
            return ObjectInfo(okStaticField, staticInfo.value1, 0, staticInfo.value2);
         }
         else return Scope::mapTerminal(terminal);
      }
   }
}

void Compiler::ClassScope :: compileClassAttribute(SNode hint)
{
   switch (hint.type)
   {
      case lxClassFlag:
         info.header.flags |= hint.argument;
         if (test(info.header.flags, elExtension))
            extensionMode = -1;
         break;
      case lxType:
         if (test(info.header.flags, elExtension)) {
            extensionMode = hint.argument;

            info.fieldTypes.add(-1, ClassInfo::FieldInfo(0, extensionMode));
         }
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
   this->stackSafe = this->classEmbeddable = false;
   this->generic = false;
//   this->sealed = false;

//   //NOTE : tape has to be overridden in the constructor
//   this->tape = &parent->tape;
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal)
{
   if (terminal.compare(THIS_VAR)) {
      if (stackSafe && classEmbeddable) {
         return ObjectInfo(okThisParam, 1, -1);
      }
      else return ObjectInfo(okThisParam, 1);
   }
   /*else if (StringHelper::compare(identifier, METHOD_SELF_VAR)) {
      return ObjectInfo(okParam, (size_t)-1);
   }*/
   else {
      Parameter param = parameters.get(terminal);

      int local = param.offset;
      if (local >= 0) {
         if (withOpenArg && moduleScope->attributeHints.exist(param.subj_ref, moduleScope->paramsReference)) {
            return ObjectInfo(okParams, -1 - local, 0, param.subj_ref);
         }
         else if (stackSafe && param.subj_ref != 0 && param.size != 0) {
            return ObjectInfo(okParam, -1 - local, -1, param.subj_ref);
         }
         return ObjectInfo(okParam, -1 - local, 0, param.subj_ref);
      }
      else return Scope::mapTerminal(terminal);

//         if (stackSafe && retVal.kind == okParam && retVal.param == -1 && retVal.type != 0) {
//            if (isEmbeddable(moduleScope->getClassFlags(moduleScope->subjectHints.get(retVal.type)))) {
//               return ObjectInfo(okParam, retVal.param, -1, retVal.type);
//            }
//            else return retVal;
//         }

//         return retVal;
//      }
   }
}

// --- Compiler::ActionScope ---

Compiler::ActionScope :: ActionScope(ClassScope* parent)
   : MethodScope(parent)
{
}

ObjectInfo Compiler::ActionScope :: mapTerminal(ident_t identifier)
{
   // HOTFIX : self / $self : closure should refer to the owner ones
   if (identifier.compare(THIS_VAR)) {
      return parent->mapTerminal(identifier);
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
//      else if (isTemplateRef(local.class_ref)) {
//         return ObjectInfo(okTemplateLocal, local.offset, local.class_ref, local.subj_ref);
//      }
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
   this->parent = owner;
   info.header.flags |= elNestedClass;
   //templateMode = false;
   //templateRef = 0;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   //String<char, 10> thisVar(THIS_VAR);

   Outer owner = outers.get(THIS_VAR);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();

      owner.outerObject = parent->mapTerminal(THIS_VAR);
      if (owner.outerObject.extraparam == 0)
         owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId();

      //// Compiler magic : if the owner is a template - switch to template closure mode
      //if (isTemplateRef(owner.outerObject.extraparam)) {
      //   templateMode = true;
      //}

      outers.add(THIS_VAR, owner);
      mapKey(info.fields, THIS_VAR, owner.reference);
   }
   return owner;
}

ObjectInfo Compiler::InlineClassScope :: mapTerminal(ident_t identifier)
{
   if (identifier.compare(THIS_VAR) || identifier.compare(OWNER_VAR)) {
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
         outer.outerObject = parent->mapTerminal(identifier);
         // handle outer fields in a special way: save only self
         if (outer.outerObject.kind == okField) {
            Outer owner = mapSelf();

            // save the outer field type if provided
            if (outer.outerObject.extraparam != 0) {
               outerFieldTypes.add(outer.outerObject.param, ClassInfo::FieldInfo(outer.outerObject.extraparam, outer.outerObject.type), true);
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
            mapKey(info.fields, identifier, outer.reference);

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
            if (outer.reference >= 0) {
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

// --- Compiler::TemplateScope ---

//Compiler::TemplateScope :: TemplateScope(ModuleScope* parent, ref_t reference)
//   : ClassScope(parent, 0)
//{
//   // NOTE : reference is defined in subject namespace, so templateRef should be initialized and used
//   // proper reference is 0 in this case
//   this->templateRef = reference;
//   this->reference = -6; // indicating template
//
//   type = ttNone;
//
//   // HOT FIX : overwrite source path to provide explicit namespace
//   _Memory* strings = syntaxTree.Strings();
//   strings->trim(0);
//
//   MemoryWriter writer(strings);
//   writer.writeLiteral(parent->module->Name());
//   writer.seek(writer.Position() - 1);
//   writer.writeChar('\'');
//   writer.writeLiteral(parent->sourcePath);
//}

void Compiler::TemplateScope :: loadParameters(SNode node)
{
   SNode current = node.firstChild();
   // load template parameters
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();

         parameters.add(name, parameters.Count() + 1);
      }
      else if (current == lxTemplateParam) {
         subjects.add(subjects.Count() + 1, current.argument);
      }
      else if (current == lxAttributeValue) {
         SNode terminalNode = current.firstChild(lxObjectMask);
         ref_t subject = mapSubject(terminalNode);
         if (subject == 0)
            subject = moduleScope->module->mapSubject(terminalNode.findChild(lxTerminal).identifier(), false);

         subjects.add(subjects.Count() + 1, subject);
      }

      current = current.nextNode();
   }
}

void Compiler::TemplateScope :: generateClassName(bool newName)
{
   ReferenceNs name;
   if (newName) {
      name.copy(moduleScope->module->Name());
      name.combine(moduleScope->module->resolveSubject(templateRef));
   }
   else name.copy(moduleScope->module->resolveSubject(templateRef));

   SubjectMap::Iterator it = subjects.start();
   while (!it.Eof()) {
      name.append('@');
      name.append(moduleScope->module->resolveSubject(*it));

      it++;
   }

   if (newName) {
      reference = moduleScope->module->mapReference(name);
   }
   else {
      reference = moduleScope->resolveIdentifier(name);
      if (!reference)
         reference = moduleScope->mapNestedExpression();
   }

   classMode = true;
}

//ObjectInfo Compiler::TemplateScope :: mapObject(TerminalInfo identifier)
//{
//   int index = info.fields.get(identifier);
//
//   if (index >= 0) {
//      return ObjectInfo(okTemplateTarget, index, 0, info.fieldTypes.get(index));
//   }
//   else return ClassScope::mapObject(identifier);
//}
//
//bool Compiler::TemplateScope :: validateTemplate(ref_t reference)
//{
//   _Module* extModule = NULL;
//   _Memory* section = moduleScope->loadTemplateInfo(reference, extModule);
//   if (!section)
//      return false;
//
//   // HOTFIX : inherite template fields
//   SyntaxTree tree(section);
//   SNode current = tree.readRoot();
//   current = current.firstChild();
//   while (current != lxNone) {
//      if (current == lxTemplateField) {
//         SNode typeNode = SyntaxTree::findChild(current, lxTemplateFieldType);
//
//         info.fields.add(SyntaxTree::findChild(current, lxTerminal).identifier(), current.argument);
//         if (typeNode.argument != 0)
//            info.fieldTypes.add(current.argument, typeNode.argument);
//      }
//
//      current = current.nextNode();
//   }
//
//   return true;
//}

// --- Compiler ---

Compiler :: Compiler(StreamReader* syntax, _CompilerLogic* logic)
   : _parser(syntax), _verbs(0)
{
   _optFlag = 0;

   this->_logic = logic;

   ByteCodeCompiler::loadVerbs(_verbs);
   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: insertMessage(SNode node, ModuleScope& scope, ref_t messageRef)
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

   node.insertNode(lxMessageVariable, name);
}

//void Compiler :: appendObjectInfo(CodeScope& scope, ObjectInfo object)
//{
//   if (object.type != 0 && object.kind != okTemplateLocal) {
//      scope.writer->appendNode(lxType, object.type);
//   }
//
//   ref_t objectRef = resolveObjectReference(scope, object);
//   if (isTemplateRef(objectRef)) {
//      if (object.kind == okTemplateLocal) {
//         TemplateScope* templateScope = (TemplateScope*)scope.getScope(Scope::slTemplate);
//
//         ident_t templateFullName = scope.moduleScope->module->resolveSubject(object.type);
//         int index = StringHelper::find(templateFullName, '@');
//
//         IdentifierString name(templateFullName, index);
//         //HOTFIX : virtual variable
//         scope.writer->newNode(lxTemplateType, scope.moduleScope->module->mapSubject(name, true));
//         while (index < getlength(templateFullName)) {
//            int pos = index + 1;
//            index = StringHelper::find(templateFullName + pos, '@', getlength(templateFullName));
//            IdentifierString param(templateFullName + pos, index - pos);
//
//            scope.writer->appendNode(lxTemplateParam, templateScope->parameters.get(param));
//         }
//         scope.writer->closeNode();
//      }
//      else scope.writer->appendNode(lxNestedTemplateOwner);
//   }
//   else if (objectRef != 0) {
//      scope.writer->appendNode(lxTarget, objectRef);
//   }
//   else if (object.type == 0) {
//      if (object.kind == okFieldAddress && object.param == 0) {
//         int flags = scope.getClassFlags();
//         switch (flags & elDebugMask) {
//            case elDebugDWORD:
//               scope.writer->appendNode(lxTarget, -1);
//               break;
//            case elDebugQWORD:
//               scope.writer->appendNode(lxTarget, -2);
//               break;
//            case elDebugReal64:
//               scope.writer->appendNode(lxTarget, -4);
//               break;
//            case elDebugSubject:
//               scope.writer->appendNode(lxTarget, -8);
//               break;
//         }
//      }
//   }
//}

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
         return V_INT32;
      case okLongConstant:
         return V_INT64;
      case okRealConstant:
         return V_REAL64;
      case okCharConstant:
         return scope.moduleScope->charReference;
      case okLiteralConstant:
         return scope.moduleScope->literalReference;
      case okWideLiteralConstant:
         return scope.moduleScope->wideReference;
      case okThisParam:
         if (object.extraparam == -2) {
            return _logic->definePrimitiveArray(*scope.moduleScope, scope.moduleScope->attributeHints.get(object.type));
         }
         else return scope.getClassRefId(false);
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
         else return (object.type != 0) ? scope.moduleScope->attributeHints.get(object.type) : 0;
   }
}

void Compiler :: declareParameterDebugInfo(SNode node, MethodScope& scope, bool withThis, bool withSelf)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // declare built-in variables
   if (withThis) {
      if (scope.stackSafe && scope.classEmbeddable) {
         SNode debugNode = node.insertNode(lxBinarySelf, 1);
         debugNode.appendNode(lxClassName, scope.moduleScope->module->resolveReference(scope.getClassRef()));
      }
      else node.insertNode(lxSelfVariable, 1);
   }

   if (withSelf)
      node.insertNode(lxSelfVariable, -1);

   insertMessage(node, *moduleScope, scope.message);

   SNode current = node.firstChild();
   // f method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         current = lxVariable;

         ident_t name = current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();
         Parameter param = scope.parameters.get(name);

         if (scope.moduleScope->attributeHints.exist(param.subj_ref, moduleScope->paramsReference)) {
            current = lxParamsVariable;
//         writer.appendNode(lxTerminal, it.key());
//         writer.appendNode(lxLevel, -1 - (*it).offset);
//         writer.closeNode();
         }
         else if (scope.moduleScope->attributeHints.exist(param.subj_ref, moduleScope->intReference)) {
            current = lxIntVariable;
         }
         else if (scope.moduleScope->attributeHints.exist(param.subj_ref, moduleScope->longReference)) {
            current = lxLongVariable;
         }
         else if (scope.moduleScope->attributeHints.exist(param.subj_ref, moduleScope->realReference)) {
            current = lxReal64Variable;
         }
         else if (scope.stackSafe && param.subj_ref != 0) {
            ref_t classRef = scope.moduleScope->attributeHints.get(param.subj_ref);
            if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {
               current = lxBinaryVariable;
               current.appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
            }
         }
//         else {
      //   writer.newNode(lxVariable, -1);
      //   writer.appendNode(lxTerminal, it.key());
      //   writer.appendNode(lxFrameAttr);
      //   writer.closeNode();
//         }
         current.appendNode(lxLevel, -1 - param.offset);
      }

      current = current.nextNode();
   }
}

void Compiler :: importCode(SNode node, ModuleScope& scope, ident_t function, ref_t message)
{
   IdentifierString virtualReference(function);
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
   if (section != NULL) {
      node = lxImporting;
      node.setArgument(_writer.registerImportInfo(section, api, scope.module));
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

ref_t Compiler::mapAttribute(SNode attribute, ModuleScope& scope)
{
   Scope dummyScope(&scope);
   int dummy;

   return mapAttribute(attribute, dummyScope, dummy);
}

ref_t Compiler :: mapAttribute(SNode attribute, Scope& scope, int& attrValue)
{
   int paramCounter =SyntaxTree::countChild(attribute, lxAttributeValue);
   SNode value = attribute.findChild(lxAttributeValue).firstChild(lxTerminalMask);
   if (value == lxInteger) {
      //HOTFIX : only one dimensional arrays are supported currently
      if (paramCounter == 1) {
         attrValue = value.findChild(lxTerminal).identifier().toInt();

         paramCounter = 0;
      }
      else return 0;
   }

   ref_t attrRef = 0;

   SNode terminal = attribute.findChild(lxPrivate, lxIdentifier, lxInteger, lxHexInteger);

   if (terminal == lxInteger) {
      ident_t value = terminal.findChild(lxTerminal).identifier();

      attrValue = value.toInt();
   }
   else if (terminal == lxHexInteger) {
      ident_t value = terminal.findChild(lxTerminal).identifier();

      attrValue = value.toLong(16);
   }
   else if (paramCounter > 0) {
      IdentifierString attrName(terminal.findChild(lxTerminal).identifier());      
      attrName.append('#');
      attrName.appendInt(paramCounter);

      attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
   }
   else {
      attrRef = scope.mapSubject(terminal);
      if (attrRef == 0) {
         IdentifierString attrName(terminal.findChild(lxTerminal).identifier());
         attrName.append('#');
         attrName.appendInt(paramCounter);

         attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
      }
   }

   return attrRef;
}

//bool Compiler :: compileClassHint(DNode hint, SyntaxWriter& writer, ClassScope& scope)
//{
//   TerminalInfo terminal = hint.Terminal();
//
//   // if it is a class modifier
//   if (terminal == tsInteger) {
//      writer.appendNode(lxClassFlag, StringHelper::strToInt(terminal));
//
//      return true;
//   }
//   else if (terminal == tsHexInteger) {
//      writer.appendNode(lxClassFlag, StringHelper::strToULong(terminal, 16));
//
//      return true;
//   }
//   else if (scope.isVirtualSubject(terminal)) {
//      writer.appendNode(lxType, scope.mapSubject(terminal));
//
//      return true;
//   }
//   else {
//      ModuleScope* moduleScope = scope.moduleScope;
//      ref_t hintRef = mapHint(hint, *moduleScope, 0);
//
//      if (hintRef != 0) {
//         return declareAttribute(hint, scope, writer, hintRef);
//      }
//   }
//
//   return false;
//}

void Compiler :: compileClassAttributes(SNode node, ClassScope& scope, SNode rootNode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int attrValue = 0;
         ref_t attrRef = mapAttribute(current, scope, attrValue);
         if (attrValue != 0) {
            if (_logic->validateClassAttribute(attrValue)) {
               current.set(lxClassFlag, attrValue);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (attrRef != 0) {
            if (scope.moduleScope->attributeHints.get(attrRef) != -1) {
               current.set(lxType, attrRef);
            }
            else if(!copyTemplate(rootNode, scope, attrRef, current))
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, current);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, current);
      }
      else if (current == lxTemplate) {
         TemplateScope templateScope(&scope);
         templateScope.loadParameters(current);

         SNode sourceNode = current.findChild(lxSourcePath);
         if (sourceNode != lxNone)
            templateScope.sourceRef = _writer.writeString(sourceNode.identifier());

         compileClassAttributes(current, /*scope*/templateScope, rootNode);
      }

      current = current.nextNode();
   }
}

void Compiler :: compileSymbolAttributes(SNode node, SymbolScope& scope, SNode rootNode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int attrValue = 0;
         ref_t attribute = mapAttribute(current, scope, attrValue);
         if (attrValue != 0) {
            if (_logic->validateSymbolAttribute(attrValue)) {
               rootNode.appendNode((LexicalType)attrValue);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (attribute) {
            ref_t classRef = scope.moduleScope->attributeHints.get(attribute);
            if (classRef == INVALID_REF) {
               copyTemplate(rootNode, scope, attribute, current);
            }
            else node.appendNode(lxType, attribute);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, current);
      }
      else if (current == lxTemplate) {
         compileSymbolAttributes(current, scope, rootNode);
      }

      current = current.nextNode();
   }
}

//void Compiler :: compileSingletonHints(DNode hints, SyntaxWriter& writer, ClassScope& scope)
//{
//   while (hints == nsHint) {
//      ref_t hintRef = mapHint(hints, *scope.moduleScope, 0);
//
//      TerminalInfo terminal = hints.Terminal();
//
//      if (hintRef != 0) {
//         declareAttribute(hints, scope, writer, hintRef);
//      }
//      else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());
//
//      hints = hints.nextNode();
//   }
//}
//
//void Compiler :: compileTemplateHints(DNode hints, SyntaxWriter& writer, TemplateScope& scope)
//{
//   if (scope.type == TemplateScope::ttClass) {
//      while (hints == nsHint) {
//         if (!compileClassHint(hints, writer, scope))
//            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, hints.Terminal());
//
//         hints = hints.nextNode();
//      }
//   }
//   else if (scope.type == TemplateScope::ttMethod) {
//      MethodScope methodScope(&scope);
//      compileMethodHints(hints, writer, methodScope);
//   }
//   else if (scope.type == TemplateScope::ttField) {
//      compileFieldHints(hints, writer, scope);
//   }
//}

void Compiler :: compileFieldAttributes(SNode node, ClassScope& scope, SNode rootNode)
{
//   ModuleScope* moduleScope = scope.moduleScope;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int attrValue = 0;
         ref_t attribute = mapAttribute(current, scope, attrValue);
         if (attrValue != 0) {
            if (attrValue > 0) {
               // positive value defines the target size
               rootNode.appendNode(lxSize, attrValue);
            }
            else if (_logic->validateFieldAttribute(attrValue)) {
               rootNode.appendNode((LexicalType)attrValue);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (attribute) {
            ref_t classRef = scope.moduleScope->attributeHints.get(attribute);
            if (classRef == INVALID_REF) {
               copyTemplate(rootNode, scope, attribute, current);
            }
            else node.appendNode(lxType, attribute);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, current);
      }
      else if (current == lxTemplate) {
         compileFieldAttributes(current, scope, rootNode);
      }

      current = current.nextNode();
   }
}

void Compiler :: compileMethodAttributes(SNode node, MethodScope& scope, SNode rootNode)
{
//   ModuleScope* moduleScope = scope.moduleScope;
//
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int attrValue = 0;
         ref_t attribute = mapAttribute(current, scope, attrValue);
         if (attrValue != 0) {
            if (_logic->validateMethodAttribute(attrValue)) {
               rootNode.appendNode(lxClassMethodAttr, attrValue);
            }
            else if (_logic->validateWarningAttribute(attrValue)) {
               rootNode.appendNode(lxWarningMask, attrValue);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (attribute) {
            ref_t classRef = scope.moduleScope->attributeHints.get(attribute);
            if (classRef == INVALID_REF) {
               copyTemplate(rootNode, scope, attribute, current);
            }
            else node.appendNode(lxType, attribute);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, rootNode);

            //      TerminalInfo terminal = hints.Terminal();
         //      //HOTFIX : if it is a virtual subject
         //      if (hints.firstChild() != nsHintValue && scope.isVirtualSubject(terminal)) {
         //         writer.appendNode(lxType, scope.mapSubject(terminal));
         //      }
         //      else if (terminal == tsHexInteger) {
         //         writer.appendNode(lxClassMethodAttr, StringHelper::strToULong(terminal, 16));
         //      }
         //      else if (terminal == tsInteger) {
         //         int attr = StringHelper::strToInt(terminal);
         //
         //         if (attr == -2) {
         //            writer.appendNode(lxWarningMask, WARNING_MASK_1);
         //         }
         //         else if (attr == -3) {
         //            writer.appendNode(lxWarningMask, WARNING_MASK_2);
         //         }
         //         else writer.appendNode(lxClassMethodAttr, attr);
         //      }
         //      else {
         //         ref_t hintRef = mapHint(hints, *moduleScope, 1000);
         //         if (moduleScope->subjectHints.exist(hintRef)) {
         //            writer.appendNode(lxType, hintRef);
         //         }
         //         else if (hintRef != 0) {
         //            if (!declareMethodAttribute(hints, scope, writer, hintRef))
         //               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, terminal);
         //         }
         //         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
         //      }
      }
      else if (current == lxTemplate) {
         compileMethodAttributes(current, scope, rootNode);
      }

      current = current.nextNode();
   }
}

//void Compiler :: updateMethodTemplateInfo(MethodScope& scope, size_t rollbackPosition)
//{
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//
//   MemoryReader reader(&classScope->imported, rollbackPosition);
//   MemoryWriter writer(&classScope->imported, rollbackPosition);
//   while (!reader.Eof()) {
//      TemplateInfo info;
//
//      info.load(reader);
//
//      ident_t signature = scope.moduleScope->module->resolveSubject(getSignature(scope.message));
//      IdentifierString customVerb(signature, StringHelper::find(signature, '&', getlength(signature)));
//      info.messageSubject = scope.moduleScope->module->mapSubject(customVerb, false);
//
//      info.save(writer);
//   }
//}

void Compiler :: compileLocalAttributes(SNode node, CodeScope& scope, ObjectInfo& variable, int& size)
{
   SNode current = node.firstChild(lxAttribute);
   while (current != lxNone) {
      if (current == lxAttribute) {
         int attrValue = 0;
         ref_t attrRef = mapAttribute(current, scope, attrValue);
         if (attrRef != 0 && attrValue != 0) {
            // if it is a primitive array declaration
            size = attrValue;
            variable.type = attrRef;
            variable.extraparam = _logic->definePrimitiveArray(*scope.moduleScope, scope.moduleScope->attributeHints.get(attrRef));
         }
         else if (attrValue != 0) {
            // positive value defines the target size
            if (attrValue > 0) {
               size = attrValue;
            }
            else if (_logic->validateLocalAttribute(attrValue)) {
               // negative value defines the target virtual class
               variable.extraparam = attrValue;
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (attrRef != 0) {
            variable.extraparam = scope.moduleScope->attributeHints.get(attrRef);
            if (variable.extraparam == INVALID_REF) {
               TemplateScope templateScope(&scope, attrRef);
               templateScope.loadParameters(current);

               templateScope.generateClassName();

               //               if (type != 0)
               //                  scope.raiseError(errInvalidHint, terminal);
               //               TemplateInfo templateInfo;
               //               templateInfo.templateRef = hintRef;
               //               templateInfo.sourceCol = terminal.Col();
               //               templateInfo.sourceRow = terminal.Row();
               //
               //               //HOTFIX: validate if the subjects are virtual
               //               if (scope.isVirtualSubject(hints.firstChild().Terminal())) {
               //                  classRef = -6;
               //                  type = scope.moduleScope->module->mapSubject(name, false);
               //               }
               //               else {
               variable.extraparam = generateTemplate(current, templateScope);
               //                  if (classRef == 0)
               //                     scope.raiseError(errInvalidHint, terminal);
               //               }
            }
            else if (variable.type == 0) {
               variable.type = attrRef;
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, current);

      }
//      TerminalInfo terminal = hints.Terminal();
//      else {
//         ref_t hintRef = mapHint(hints, *scope.moduleScope, 0);
//
//         if (hintRef != 0) {
//            if (scope.moduleScope->subjectHints.exist(hintRef)) {
//               if (type == 0 && classRef == 0) {
//                  type = hintRef;
//
//                  TerminalInfo target = hints.firstChild().Terminal();
//                  if (target != nsNone) {
//                     if (target.symbol == tsInteger) {
//                        size = StringHelper::strToInt(target);
//
//                        classRef = -3; // NOTE : -3 means an array of type
//                     }
//                     else scope.raiseError(errInvalidHint, terminal);
//                  }
//                  else classRef = scope.moduleScope->subjectHints.get(type);
//               }
//               else scope.raiseError(errInvalidHint, terminal);
//            }
//            else {

//            }
//         }
//         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());
//      }

      current = current.nextNode();
   }
}

void Compiler :: compileSwitch(SNode node, CodeScope& scope)
{
   SNode targetNode = node.firstChild(lxObjectMask);

   bool immMode = true;
   if (targetNode == lxExpression) {
      immMode = false;

      //      scope.writer->insert(lxVariable);
      //      scope.writer->insert(lxSwitching);
      //      scope.writer->closeNode();
      //
      //      switchValue.kind = okBlockLocal;
      //      switchValue.param = 1;
   }

   SNode option = node.findChild(lxOption, lxElse);
   while (option == lxOption) {
      insertDebugStep(option, dsStep);

      int operator_id = EQUAL_MESSAGE_ID;
//      if (option == nsBiggerSwitchOption) {
//         operator_id = GREATER_MESSAGE_ID;
//      }
//      else if (option == nsLessSwitchOption) {
//         operator_id = LESS_MESSAGE_ID;
//      }
      
      SNode expr = option.injectNode(lxExpression);
      
      // find option value
      SNode valueNode = expr.firstChild(lxObjectMask);

      // inject operation
      valueNode.injectNode(valueNode.type);
      valueNode = lxExpression;

      // inject target
      if (immMode) {
         SyntaxTree::copyNode(targetNode, valueNode.appendNode(targetNode.type));
      }
      valueNode.appendNode(lxOperator, operator_id);

      // inject code expression
      SNode codeExpr = expr.findChild(lxCode);
      codeExpr.injectNode(lxCode);
      codeExpr = lxExpression;

      compileBranchingOperator(expr, scope, HINT_SWITCH, IF_MESSAGE_ID);

      option = option.nextNode();
   }

//   if (option == nsLastSwitchOption) {
//      scope.writer->newNode(lxElse);
//
//      CodeScope subScope(&scope);
//      DNode thenCode = option.firstChild();
//
//      //_writer.declareBlock(*scope.tape);
//
//      DNode statement = thenCode.firstChild();
//      if (statement.nextNode() != nsNone || statement == nsCodeEnd) {
//         compileCode(thenCode, subScope);
//      }
//      // if it is inline action
//      else compileRetExpression(statement, scope, 0);
//
//      scope.writer->closeNode();
//   }
//
//   scope.writer->closeNode();
}

void Compiler :: compileVariable(SNode node, CodeScope& scope)
{
   SNode terminal = node.findChild(lxIdentifier, lxPrivate);
   ident_t identifier = terminal.findChild(lxTerminal).identifier();

   if (!scope.locals.exist(identifier)) {
      int size = 0;
      ObjectInfo variable(okLocal);
      compileLocalAttributes(node, scope, variable, size);

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
               node = lxIntVariable;
               break;
            case elDebugQWORD:
               node = lxLongVariable;
               break;
            case elDebugReal64:
               node = lxReal64Variable;
               break;
            case elDebugIntegers:
               node.set(lxIntsVariable, size);
               break;
            case elDebugShorts:
               node.set(lxShortsVariable, size);
               break;
            case elDebugBytes:
               node.set(lxBytesVariable, size);
               break;
            default:
               if (isPrimitiveRef(variable.extraparam)) {
                  node.set(lxBytesVariable, size);
               }
               else {
                  node = lxBinaryVariable;
                  // HOTFIX : size should be provide only for dynamic variables
                  if (bytearray)
                     node.setArgument(size);

                  if (variable.extraparam != 0) {
                     node.appendNode(lxClassName, scope.moduleScope->module->resolveReference(variable.extraparam));
                  }
               }
               break;
         }
      }
      else variable.param = scope.newLocal();

      node.appendNode(lxLevel, variable.param);

      scope.mapLocal(identifier, variable.param, variable.type, variable.extraparam, size);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

void Compiler :: setTerminal(SNode& terminal, CodeScope& scope, ObjectInfo object, int mode)
{
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         terminal = lxSymbolReference;
         terminal.setArgument(object.param);
         break;
      case okConstantClass:
         terminal = lxConstantClass;
         terminal.setArgument(object.param);
         break;
      case okConstantSymbol:
         terminal.set(lxConstantSymbol, object.param);
         break;
      case okLiteralConstant:
         terminal.set(lxConstantString, object.param);
         break;
      case okWideLiteralConstant:
         terminal.set(lxConstantWideStr, object.param);
         break;
      case okCharConstant:
         terminal.set(lxConstantChar, object.param);
         break;
      case okIntConstant:
         terminal.set(lxConstantInt, object.param);
         terminal.appendNode(lxIntValue, object.extraparam);
         break;
      case okLongConstant:
         terminal.set(lxConstantLong, object.param);
         break;
      case okRealConstant:
         terminal.set(lxConstantReal, object.param);
         break;
      case okArrayConst:
         terminal.set(lxConstantList, object.param);
         break;
//      case okTemplateLocal:
//         scope.writer->newNode(lxLocal, object.param);
//         break;
      case okLocal:
      case okParam:
         if (object.extraparam == -1) {
            ref_t targetRef = resolveObjectReference(scope, object);
            terminal.set(lxCondBoxing, _logic->defineStructSize(*scope.moduleScope, targetRef));
            terminal.appendNode(lxLocal, object.param);
            terminal.appendNode(lxTarget, targetRef);
         }
         else terminal.set(lxLocal, object.param);
         break;
      case okThisParam:
         if (object.extraparam == -1) {
            ref_t targetRef = resolveObjectReference(scope, object);
            terminal.set(lxCondBoxing, _logic->defineStructSize(*scope.moduleScope, targetRef));
            terminal.appendNode(lxThisLocal, object.param);
            terminal.appendNode(lxTarget, targetRef);
         }
         else terminal.set(lxThisLocal, object.param);
         break;
      case okSuper:
         terminal.set(lxLocal, 1);
         break;
      case okField:
      case okOuter:
         terminal.set(lxField, object.param);
         break;
      case okStaticField:
         terminal.set(lxStaticField, object.param);
         break;
      case okOuterField:
         terminal.set(lxFieldExpression, 0);
         terminal.appendNode(lxField, object.param);
         terminal.appendNode(lxResultField, object.extraparam);
         break;
      case okSubject:
         terminal.injectNode(lxLocalAddress, object.param);
         terminal.set(lxBoxing, _logic->defineStructSize(*scope.moduleScope, scope.moduleScope->signatureReference));
         terminal.appendNode(lxTarget, scope.moduleScope->signatureReference);
         break;
      case okLocalAddress:
         if (!test(mode, HINT_NOBOXING)) {
            terminal.injectNode(lxLocalAddress, object.param);
            terminal.set(lxBoxing, _logic->defineStructSize(*scope.moduleScope, object.extraparam));
            terminal.appendNode(lxTarget, object.extraparam);
         }
         else terminal.set(lxLocalAddress, object.param);
         break;
      case okFieldAddress:
         if (!test(mode, HINT_NOBOXING)) {
            ref_t target = resolveObjectReference(scope, object);

            terminal.injectNode(lxFieldAddress, object.param);
            terminal.set(lxBoxing, _logic->defineStructSize(*scope.moduleScope, target));
            terminal.appendNode(lxTarget, target);
         }
         else terminal.set(lxFieldAddress, object.param);
         break;
      case okNil:
         terminal.set(lxNil, object.param);
         break;
      case okVerbConstant:
         terminal.set(lxVerbConstant, object.param);
         break;
      case okMessageConstant:
         terminal.set(lxMessageConstant, object.param);
         break;
      case okExtMessageConstant:
         terminal.set(lxExtMessageConstant, object.param);
         break;
      case okSignatureConstant:
         terminal.set(lxSignatureConstant, object.param);
         break;
//      case okBlockLocal:
//         scope.writer->newNode(lxBlockLocal, object.param);
//         break;
      case okParams:
         terminal.set(lxArgBoxing, 0);
         terminal.appendNode(lxBlockLocalAddr, object.param);
         terminal.appendNode(lxTarget, scope.moduleScope->paramsReference);
         break;
      case okObject:
         terminal.set(lxResult, 0);
         break;
      case okConstantRole:
         terminal.set(lxConstantSymbol, object.param);
         break;
//      case okTemplateTarget:
//         scope.writer->newNode(lxTemplateTarget, object.param);
//         // HOTFIX : tempalte type is not an actual type, so it should be saved in special way and cleared after
//         scope.writer->appendNode(lxTemplateFieldType, object.type);
//         object.type = 0;
//         break;
      case okExternal:
      case okInternal:
         // HOTFIX : external / internal node will be declared later
         return;
   }

//   appendObjectInfo(scope, object);
//   if (terminal != nsNone)
//      appendTerminalInfo(scope.writer, terminal);
//
//   scope.writer->closeNode();
}

ObjectInfo Compiler :: compileTerminal(SNode terminal, CodeScope& scope, int mode)
{
   //if (!test(mode, HINT_NODEBUGINFO))
   //   insertDebugStep(terminal, dsStep);

   ident_t token = terminal.findChild(lxTerminal).identifier();

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

      long integer = token.toInt();
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
   }
   else if (terminal == lxLong) {
      String<char, 30> s("_"); // special mark to tell apart from integer constant
      s.append(token, getlength(token) - 1);

      long long integer = token.toULongLong(10, 1);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s));
   }
   else if (terminal == lxHexInteger) {
      String<char, 20> s;

      long integer = token.toULong(16);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.appendHex(integer);

      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
   }
   else if (terminal == lxReal) {
      String<char, 30> s(token, getlength(token) - 1);
      double number = token.toDouble();
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // HOT FIX : to support 0r constant
      if (s.Length() == 1) {
         s.append(".0");
      }

      object = ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant((const char*)s));
   }
   else if (terminal == lxResult) {
      object = ObjectInfo(okObject);
   }
   else if (!emptystr(token))
      object = scope.mapObject(terminal);

   setTerminal(terminal, scope, object, mode);

   return object;
}

ObjectInfo Compiler :: compileObject(SNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

   SNode member = objectNode.findChild(lxCode, lxNestedClass, lxMessageReference, lxInlineExpression, lxExpression);
   switch (member.type)
   {
      case lxNestedClass:
//      case nsRetStatement:
//         if (objectNode.Terminal() != nsNone) {
//            result = compileClosure(objectNode, scope, 0);
//            break;
//         }
      case lxCode:
//      case nsSubjectArg:
//      case nsMethodParameter:
         result = compileClosure(member, scope, test(mode, HINT_CLOSURE) ? HINT_CLOSURE : 0);
         break;
//      case nsInlineClosure:
//         result = compileClosure(member.firstChild(), scope, HINT_CLOSURE);
//         break;
      case lxInlineExpression:
         result = compileClosure(member, scope, HINT_CLOSURE);
         break;
      case lxExpression:
         if (isCollection(member)) {
            result = compileCollection(objectNode, scope, mode);
         }
         /*else if (test(mode, HINT_TRY_MODE)) {
            result = compileExpression(member, scope, 0);

            objectNode = lxTrying;
         }*/
         else result = compileExpression(member, scope, 0);
         break;
      case lxMessageReference:
         result = compileMessageReference(member, scope, mode);
         break;
      default:
         result = compileTerminal(objectNode, scope, mode);
   }

   return result;
}

ObjectInfo Compiler :: compileMessageReference(SNode node, CodeScope& scope, int mode)
{
   SNode terminal = node.findChild(lxPrivate, lxIdentifier, lxLiteral);
   IdentifierString signature;
   ref_t verb_id = 0;
   int paramCount = -1;
   ref_t extensionRef = 0;
   if (terminal == lxIdentifier || terminal == lxPrivate) {
      ident_t name = terminal.findChild(lxTerminal).identifier();
      verb_id = _verbs.get(name);
      if (verb_id == 0) {
         signature.copy(name);
      }
   }
   else {
      ident_t message = terminal.findChild(lxTerminal).identifier();

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
               paramCount = signature.ident().toInt();
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
      //   ref_t openArgType = retrieveKey(scope.moduleScope->subjectHints.start(), scope.moduleScope->paramsReference, 0);
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

   setTerminal(node, scope, retVal, mode);

   return retVal;
}

ref_t Compiler :: mapMessage(SNode node, CodeScope& scope, size_t& paramCount/*, bool& argsUnboxing*/)
{
   bool   first = true;
   ref_t  verb_id = 0;

   IdentifierString signature;
   SNode arg = node.findChild(lxMessage);

   SNode name = arg.findChild(lxPrivate, lxIdentifier);

   verb_id = _verbs.get(name.findChild(lxTerminal).identifier());
   if (verb_id == 0) {
      ref_t id = scope.mapSubject(name, signature);
   
      // if followed by argument list - it is EVAL verb
      if (arg.nextNode() != lxNone) {
         // HOT FIX : strong types cannot be used as a custom verb with a parameter
         if (scope.moduleScope->attributeHints.exist(id))
            scope.raiseError(errStrongTypeNotAllowed, arg);

         verb_id = EVAL_MESSAGE_ID;

         first = false;
      }
      // otherwise it is GET message
      else verb_id = GET_MESSAGE_ID;
   }

   arg = arg.nextNode();

   paramCount = 0;
   // if message has generic argument list
   while (test(arg.type, lxObjectMask)) {
      paramCount++;
   
      arg = arg.nextNode();
   }
   
   // if message has named argument list
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxPrivate, lxIdentifier);
      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subjRef = scope.mapSubject(subject, signature);
      arg.setArgument(subjRef);
   
      arg = arg.nextNode();

      // skip an argument
      if (test(arg.type, lxObjectMask)) {
         // if it is an open argument list
         if (arg.nextNode() == lxNone && scope.moduleScope->attributeHints.exist(subjRef, scope.moduleScope->paramsReference)) {
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > 0x0F)
               scope.raiseError(errNotApplicable, subject);

   //            ObjectInfo argListParam = scope.mapObject(arg.firstChild().Terminal());
   //            // HOTFIX : set flag if the argument list has to be unboxed
   //            if (arg.firstChild().nextNode() == nsNone && argListParam.kind == okParams) {
   //               argsUnboxing = true;
   //            }
         }
         else {
            paramCount++;
   
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, node);
   
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
                  if (scope.moduleScope->attributeHints.exist(*it, classRef)) {
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

ObjectInfo Compiler :: compileBranchingOperator(SNode& node, CodeScope& scope, int mode, int operator_id)
{
   bool loopMode = test(mode, HINT_LOOP);
   ObjectInfo retVal(okObject);

   SNode loperandNode = node.firstChild(lxObjectMask);
   ObjectInfo loperand = compileExpression(loperandNode, scope, 0);

   // HOTFIX : in loop expression, else node is used to be similar with branching code
   // because of optimization rules
   ref_t original_id = operator_id;
   if (loopMode) {
      operator_id = operator_id == IF_MESSAGE_ID ? IFNOT_MESSAGE_ID : IF_MESSAGE_ID;
   }

   ref_t ifReference = 0;
   if (_logic->resolveBranchOperation(*scope.moduleScope, *this, operator_id, resolveObjectReference(scope, loperand), ifReference)) {
      node = lxBranching;
      // HOTFIX : mark it as switch branching if required
      if (test(mode, HINT_SWITCH))
         node.setArgument(-1);

      SNode thenBody = loperandNode.nextNode(lxObjectMask);
      if (loopMode) {
         thenBody.set(lxElse, ifReference);
         compileBranching(thenBody, scope);
      }
      else {
         thenBody.set(lxIf, ifReference);
         compileBranching(thenBody, scope);

         SNode elseBody = thenBody.nextNode(lxObjectMask);
         if (elseBody != lxNone) {
            elseBody.set(lxElse, 0);
            compileBranching(elseBody, scope);
         }
      }
   }
   else {
      operator_id = original_id;

      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
      compileObject(roperandNode, scope, HINT_CLOSURE);

      SNode roperand2Node = roperandNode.nextNode(lxObjectMask);
      if (roperand2Node != lxNone) {
         compileObject(roperand2Node, scope, HINT_CLOSURE);

         if (loopMode)
            node = node.injectNode(node);

         retVal = compileMessage(node, scope, loperand, encodeMessage(0, operator_id, 2), 0);
      }
      else {
         if (loopMode)
            node = node.injectNode(node);

         retVal = compileMessage(node, scope, loperand, encodeMessage(0, operator_id, 1), 0);
      }
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SNode node, CodeScope& scope, int mode, int operator_id)
{
   ObjectInfo retVal(okObject);
   int paramCount = 1;

   ObjectInfo loperand;
   ObjectInfo roperand;
   ObjectInfo roperand2;
   if (operator_id == SET_REFER_MESSAGE_ID) {
      SNode exprNode = node.firstChild(lxObjectMask);

      // HOTFIX : inject the third parameter to the first two ones
      SyntaxTree::copyNodeSafe(exprNode.nextNode(lxObjectMask), exprNode.appendNode(lxExpression));
      exprNode.nextNode(lxObjectMask) = lxIdle;

      SNode loperandNode = exprNode.firstChild(lxObjectMask);
      loperand = compileExpression(loperandNode, scope, 0);

      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
      roperand = compileExpression(roperandNode, scope, 0);
      roperand2 = compileExpression(roperandNode.nextNode(lxObjectMask), scope, 0);

      node = exprNode;

      paramCount++;
   }
   else {
      SNode loperandNode = node.firstChild(lxObjectMask);
      loperand = compileExpression(loperandNode, scope, 0);

      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
      roperand = compileExpression(roperandNode, scope, 0);
   }
      
   ref_t loperandRef = resolveObjectReference(scope, loperand);
   ref_t resultClassRef = 0;
   int operationType = 0;
   if (roperand2.kind != okUnknown) {
      operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef,
         resolveObjectReference(scope, roperand),
         resolveObjectReference(scope, roperand2), resultClassRef);
   }
   else operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef,
      resolveObjectReference(scope, roperand), resultClassRef);

   if (operationType != 0) {
      // if it is a primitive operation
      _logic->injectOperation(node, *scope.moduleScope, *this, operator_id, operationType, resultClassRef, loperand.type);

      retVal = assignResult(scope, node, resultClassRef);
   }
   else retVal = compileMessage(node, scope, loperand, encodeMessage(0, operator_id, paramCount), HINT_NODEBUGINFO);

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SNode node, CodeScope& scope, int mode)
{
   SNode operatorNode = node.findChild(lxOperator);
   SNode operatorName = operatorNode.findChild(lxTerminal);
   int operator_id = operatorNode.argument != 0 ? operatorNode.argument : _operators.get(operatorName.identifier());

   // if it is a new operator
   if (operator_id == -1) {
      return compileNewOperator(node, scope/*, mode*/);
   }
   // if it is branching operators
   else if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
      return compileBranchingOperator(node, scope, mode, operator_id);
   }
   else return compileOperator(node, scope, mode, operator_id);
}

ObjectInfo Compiler :: compileMessage(SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode)
{
   ObjectInfo retVal(okObject);

   int signRef = getSignature(messageRef);
   int paramCount = getParamCount(messageRef);

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target);
   bool classFound = false;
   bool dispatchCall = false;
   int callType = _logic->resolveCallType(*scope.moduleScope, classReference, messageRef, classFound, retVal.type);

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
      node.set(lxDirectCalling, encodeVerb(DISPATCH_MESSAGE_ID));

      node.appendNode(lxOvreriddenMessage, messageRef);
   }
   else if (callType == tpClosed || callType == tpSealed) {
      node.set(callType == tpClosed ? lxSDirctCalling : lxDirectCalling, messageRef);

//      if (test(methodHint, tpStackSafe))
//         scope.writer->appendNode(lxStacksafe);
   }
   else {
      node.set(lxCalling, messageRef);

      // if the sealed/ closed class found and the message is not supported - warn the programmer and raise an exception
      if (classFound && callType == tpUnknown)
         node.appendNode(lxNotFoundAttr);         
   }

   if (classReference)
      node.appendNode(lxCallTarget, classReference);

   if (!test(mode, HINT_NODEBUGINFO)) {
      // set a breakpoint
      setDebugStep(node.findChild(lxMessage, lxOperator), dsStep);
   }

   // define the message target if required
   if (target.kind == okConstantRole || target.kind == okSubject) {
      setTerminal(node.appendNode(lxOverridden).appendNode(lxExpression), scope, target, 0);
   }

   // the result of construction call is its instance
   if (target.kind == okConstantClass) {
      retVal.param = target.param;
   }
   // the result of get&type message should be typed
   else if (paramCount == 0 && getVerb(messageRef) == GET_MESSAGE_ID) {
      retVal.param = scope.moduleScope->attributeHints.get(signRef);
   }

   return retVal;
}

bool Compiler :: convertObject(SNode node, CodeScope& scope, ref_t targetRef, ObjectInfo source)
{
   ref_t sourceRef = resolveObjectReference(scope, source);

   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
      // if it can be boxed / implicitly converted
      return _logic->injectImplicitConversion(node, *scope.moduleScope, *this, targetRef, sourceRef, source.type);
   }
   else return true;
}

ObjectInfo Compiler :: typecastObject(SNode node, CodeScope& scope, ref_t subjectRef, ObjectInfo object)
{
   ref_t targetRef = scope.moduleScope->attributeHints.get(subjectRef);
   if (targetRef != 0) {
      if (!convertObject(node, scope, targetRef, object)) {
         node.appendNode(lxTypecastAttr);         

         // if not compatible - send a typecast message
         object = compileMessage(node, scope, object, encodeMessage(subjectRef, GET_MESSAGE_ID, 0), HINT_NODEBUGINFO);
      }
      else object = ObjectInfo(okObject, targetRef, 0, subjectRef);
   }

   return object;
}

ObjectInfo Compiler :: compileMessageParameters(SNode node, CodeScope& scope)
{
   ObjectInfo target;

   int paramMode = 0;
   //// HOTFIX : if open argument list has to be unboxed
   //// alternative boxing routine should be used (using a temporal variable)
   //if (argsUnboxing)
   //   paramMode |= HINT_ALTBOXING;
   
   SNode arg = node.firstChild();
   // compile the message target and generic argument list
   while (arg != lxMessage && arg != lxNone) {
      if (test(arg.type, lxObjectMask)) {
         if (target.kind == okUnknown) {
            // HOTFIX : to handle resend expression
            if (arg.type == lxThisLocal) {
               target = ObjectInfo(okThisParam, arg.argument);
            }
            else if (arg.type == lxResult) {
               target = ObjectInfo(okObject);
            }
            else target = compileExpression(arg, scope, paramMode);

            // HOTFIX : skip the prime message
            arg = arg.nextNode();
         }
         else compileExpression(arg, scope, paramMode);
      }

      arg = arg.nextNode();
   }

   // if message has named argument list
   while (arg == lxMessage) {
      ref_t subjectRef = arg.argument;

      arg = arg.nextNode();

      // compile an argument
      if (test(arg.type, lxObjectMask)) {
         // if it is an open argument list
         if (arg.nextNode() != lxMessage && scope.moduleScope->attributeHints.exist(subjectRef, scope.moduleScope->paramsReference)) {
            if (arg == lxExpression) {
               SNode argListNode = arg.firstChild();
               while (argListNode != lxNone) {
                  compileExpression(argListNode, scope, paramMode);
               
                  argListNode = argListNode.nextNode();
               }
               
               // terminator
               arg.appendNode(lxNil);

               // HOTFIX : copy the argument list into the call expression
               SyntaxTree::copyNode(arg, node);
               arg = lxIdle;
            }
            else {
               ObjectInfo argListParam = compileExpression(arg, scope, paramMode);
               if (argListParam.kind == okParams) {
                  arg.refresh();

                  // unbox param list
                  arg.injectNode(arg.type, arg.argument);
                  arg.set(lxArgUnboxing, 0);
               }
               else {
                  // treat it like an argument list with one parameter
                  node.appendNode(lxNil);
               }
            }
         }
         else {
            ObjectInfo param = compileExpression(arg, scope, paramMode);
            if (subjectRef != 0)
               typecastObject(arg, scope, subjectRef, param);

            arg = arg.nextNode();
         }
      }
   }

   return target;
}

ObjectInfo Compiler :: compileMessage(SNode node, CodeScope& scope, int mode)
{
   //   bool argsUnboxing = false;
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount/*, argsUnboxing*/);
   ObjectInfo target = compileMessageParameters(node, scope);

   //   bool externalMode = false;
   if (target.kind == okExternal) {
      return compileExternalCall(node, scope, mode);
   //
   //      externalMode = true;
   //      member = member.nextNode();
   //
   //      if (member != nsNone)
   //         scope.raiseError(errInvalidOperation, node.Terminal());
   }
   else if (target.kind == okInternal) {
      return compileInternalCall(node, scope, messageRef, target);
   }
   else {
      ref_t extensionRef = mapExtension(scope, messageRef, target);
      
      if (extensionRef != 0) {
         //HOTFIX: A proper method should have a precedence over an extension one
         if (checkMethod(*scope.moduleScope, resolveObjectReference(scope, target), messageRef) == tpUnknown) {
            target = ObjectInfo(okConstantRole, extensionRef, 0, target.type);
         }
      }

      return compileMessage(node, scope, target, messageRef, mode);
   }
}

ObjectInfo Compiler :: compileAssigning(SNode node, CodeScope& scope, int mode)
{
   int assignMode = 0;

   SNode exprNode = node;
   SNode operation = node.findChild(lxMessage, lxExpression, lxAssign);
   if (operation == lxExpression) {
      exprNode = operation;
      operation = exprNode.findChild(lxMessage, lxOperator);
   }

   // if it is shorthand property settings
   if (operation == lxMessage) {
      //if (operation.nextNode() != lxAssign)
      //   scope.raiseError(errInvalidSyntax, operation);

      SNode name = operation.findChild(lxIdentifier, lxPrivate);
      ref_t subject = scope.mapSubject(name);
      //HOTFIX : support lexical subjects
      if (subject == 0)
         subject = scope.moduleScope->module->mapSubject(name.findChild(lxTerminal).identifier(), false);

      ref_t messageRef = encodeMessage(subject, SET_MESSAGE_ID, 1);

      // compile target
      // NOTE : compileMessageParameters does not compile the parameter, it'll be done in the next statement
      ObjectInfo target = compileMessageParameters(exprNode, scope);

      // compile the parameter
      SNode sourceNode = exprNode.nextNode(lxObjectMask);
      ObjectInfo source = compileExpression(sourceNode, scope, 0);
      typecastObject(sourceNode, scope, subject, source);

//      ref_t extensionRef = mapExtension(scope, messageRef, object);
//      if (extensionRef != 0) {
//         //HOTFIX: A proper method should have a precedence over an extension one
//         if (scope.moduleScope->checkMethod(resolveObjectReference(scope, object), messageRef) == tpUnknown) {
//            object = ObjectInfo(okConstantRole, extensionRef, 0, object.type);
//         }
//      }

//      if (scope.moduleScope->subjectHints.exist(subject)) {
//         compileExpression(member.nextNode().firstChild(), scope, subject, 0);
//      }
//      else compileExpression(member.nextNode().firstChild(), scope, 0, 0);

      return compileMessage(node, scope, target, messageRef, HINT_NODEBUGINFO);
   }
   // if it setat operator
   else if (operation == lxOperator) {
      return compileOperator(node, scope, mode, SET_REFER_MESSAGE_ID);
   }
   else {
      SNode targetNode = node.firstChild(lxObjectMask);

      ObjectInfo target = compileObject(targetNode, scope, mode | HINT_NOBOXING);

      node = lxAssigning;
      ref_t targetRef = resolveObjectReference(scope, target);
      if (target.kind == okLocalAddress) {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef);
         if (size != 0) {
            node.setArgument(size);
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else if (target.kind == okFieldAddress) {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef);
         if (size != 0) {
            node.setArgument(size);
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else if (target.kind == okLocal || target.kind == okField || target.kind == okOuterField || target.kind == okStaticField) {
      }
      else if (target.kind == okParam || target.kind == okOuter) {
         // Compiler magic : allowing to assign byref / variable parameter
         if (_logic->isVariable(*scope.moduleScope, targetRef)) {
            node.setArgument(_logic->defineStructSize(*scope.moduleScope, targetRef));
            target.kind = (target.kind == okParam) ? okParamField : okOuterField;
         }
         // Compiler magic : allowing to assign outer local variables
         else if (target.kind == okOuter) {
            InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);

            if (!closure->markAsPresaved(target))
               scope.raiseError(errInvalidOperation, node);
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else scope.raiseError(errInvalidOperation, node);

      SNode sourceNode = targetNode.nextNode(lxObjectMask);
      ObjectInfo source = compileAssigningExpression(sourceNode, scope, assignMode);

      if (target.type != 0) {
         typecastObject(sourceNode, scope, target.type, source);
      }
      else {
         if (!convertObject(sourceNode, scope, targetRef, source)) {
            scope.raiseError(errInvalidOperation, node);
         }
      }

      return target;
   }   
}

ObjectInfo Compiler :: compileExtension(SNode node, CodeScope& scope, int mode)
{
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
            ////HOTFIX : typecast the extension target if required
            //if (test(flags, elExtension) && roleClass.fieldTypes.exist(-1)) {
            //   scope.writer->insert(lxTypecasting, encodeMessage(roleClass.fieldTypes.get(-1), GET_MESSAGE_ID, 0));
            //   scope.writer->closeNode();
            //}
         }
      }
      // if the symbol VMT can be used as an external role
      if (test(flags, elStateless)) {
         role = ObjectInfo(okConstantRole, role.param);
      }
   }

   // if it is a generic role
   if (role.kind != okConstantRole && role.kind != okSubject) {
      roleNode.set(lxOverridden, 0);
      role = compileExpression(roleNode, scope, 0);
   }

   return compileExtensionMessage(node, scope, role);
}

ObjectInfo Compiler :: compileExtensionMessage(SNode node, CodeScope& scope, ObjectInfo role)
{
   size_t paramCount = 0;
   ref_t  messageRef = mapMessage(node, scope, paramCount/*, argsUnboxing*/);
   /*ObjectInfo target = */compileMessageParameters(node, scope);

   return compileMessage(node, scope, role, messageRef, HINT_EXTENSION_MODE);
}

bool Compiler :: declareActionScope(SNode& node, ClassScope& scope, SNode argNode, ActionScope& methodScope, int mode, bool alreadyDeclared)
{
   bool lazyExpression = !test(mode, HINT_CLOSURE) && isReturnExpression(node.firstChild());

   methodScope.message = encodeVerb(EVAL_MESSAGE_ID);

   if (argNode != lxNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope);

      //node = node.select(nsSubCode);
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

      compileParentDeclaration(SNode(), scope, parentRef);
   }

   // HOT FIX : mark action as stack safe if the hint was declared in the parent class
   initialize(scope, methodScope);

   return lazyExpression;
}

void Compiler :: compileAction(SNode node, ClassScope& scope, SNode argNode, int mode, bool alreadyDeclared)
{
   ActionScope methodScope(&scope);
   bool lazyExpression = declareActionScope(node, scope, argNode, methodScope, mode, alreadyDeclared);

   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxClass, scope.reference);   
   writer.newNode(lxClassMethod, methodScope.message);
   writer.appendNode(lxSourcePath, scope.getSourcePathRef()); // the source path

   // injecting message parameter info
   // if method has generic (unnamed) argument list
   while (argNode == lxMethodParameter || argNode == lxIdentifier || argNode == lxPrivate) {
      writer.newNode(lxMethodParameter);
      SyntaxTree::copyNode(writer, argNode);
      writer.closeNode();

      argNode = argNode.nextNode();
   }
   while (argNode == lxMessage) {
      writer.newNode(lxMessage);
      SyntaxTree::copyNode(writer, argNode);
      writer.closeNode();

      argNode = argNode.nextNode();

      if (argNode == lxMethodParameter) {
         writer.newNode(lxMethodParameter);
         SyntaxTree::copyNode(writer, argNode);
         writer.closeNode();

         argNode = argNode.nextNode();
      }
   }

   // injecting action body
   writer.newNode(lxCode);

   if (node == lxCode) {
      SyntaxTree::copyNode(writer, node);
   }
   else SyntaxTree::copyNode(writer, node.findChild(lxCode));
   
   writer.closeNode();

   writer.closeNode();
   writer.closeNode();  // closing method

   // if it is single expression
   if (!lazyExpression) {
      compileActionMethod(syntaxTree.readRoot().findChild(lxClassMethod), methodScope);
   }
   else compileLazyExpressionMethod(syntaxTree.readRoot().findChild(lxClassMethod), methodScope);

//   //HOTFIX : recognize if it is nested template action
//   //!!should be refactored
//   if (scope.getScope(Scope::slOwnerClass) != &scope && ((InlineClassScope*)&scope)->templateMode) {
//      InlineClassScope* inlineScope = (InlineClassScope*)&scope;
//
//      // import fields
//      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = inlineScope->outers.start();
//      while (!outer_it.Eof()) {
//         writer.newNode(lxTemplateField, (*outer_it).reference);
//         writer.appendNode(lxTerminal, outer_it.key());
//         writer.closeNode();
//
//         outer_it++;
//      }
//
//      inlineScope->templateRef = scope.moduleScope->mapNestedTemplate();
//
//      _Memory* section = scope.moduleScope->module->mapSection(inlineScope->templateRef | mskSyntaxTreeRef, false);
//
//      scope.syntaxTree.save(section);
//   }
//   else {
      if (!alreadyDeclared)
         generateClassDeclaration(syntaxTree.readRoot(), scope, test(scope.info.header.flags, elClosed));

      generateClassImplementation(syntaxTree.readRoot(), scope);
//   }
}

void Compiler :: compileNestedVMT(SNode node, InlineClassScope& scope)
{
   compileParentDeclaration(node, scope);

   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot, scope.reference);
   SyntaxTree::copyNode(writer, node);
   writer.closeNode();

   declareVMT(syntaxTree.readRoot().firstChild(), scope);
   generateMethodDeclarations(scope, syntaxTree.readRoot(), false, test(scope.info.header.flags, elClosed));

   compileVMT(syntaxTree.readRoot(), scope);

   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info);

   generateClassImplementation(syntaxTree.readRoot(), scope);
}

ObjectInfo Compiler :: compileClosure(SNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      node.set(lxConstantSymbol, scope.reference);
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
         node.set(lxStruct, scope.info.size);
         node.appendNode(lxTarget, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node);
      }
//      else if (scope.templateMode) {
//         ownerScope.writer->newNode(lxNestedTemplate, scope.info.fields.Count());
//         ownerScope.writer->appendNode(lxTemplate, scope.templateRef);
//         ownerScope.writer->appendNode(lxNestedTemplateParent, scope.info.header.parentRef);
//      }
      else {
         // dynamic normal symbol
         node.set(lxNested, scope.info.fields.Count());
         node.appendNode(lxTarget, scope.reference);
      }

      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      //int toFree = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         SNode member = node.appendNode((*outer_it).preserved ? lxOuterMember : lxMember, (*outer_it).reference);
         setTerminal(member.appendNode(lxIdle), ownerScope, info, 0);

         outer_it++;
      }

//      ownerScope.writer->closeNode();

      return ObjectInfo(okObject, scope.reference);
   }
}

ObjectInfo Compiler :: compileClosure(SNode node, CodeScope& ownerScope, int mode)
{
   InlineClassScope scope(&ownerScope, ownerScope.moduleScope->mapNestedExpression());

   // if it is a lazy expression / multi-statement closure without parameters
   if (node == lxCode/* || node == nsInlineClosure*/) {
      compileAction(node, scope, SNode(), mode);
   }
   // if it is a closure / lambda function with a parameter
   else if (node == lxInlineExpression) {
      compileAction(node.findChild(lxCode), scope, node.findChild(lxIdentifier, lxPrivate, lxMethodParameter, lxMessage), mode);
   }
//   else if (node == nsObject && testany(mode, HINT_ACTION | HINT_CLOSURE)) {
//      compileAction(node.firstChild(), scope, node, mode);
//   }
//   // if it is an action code block
//   else if (node == nsMethodParameter || node == nsSubjectArg) {
//      compileAction(goToSymbol(node, nsInlineExpression), scope, node, 0);
//   }
   // if it is a nested class
   else compileNestedVMT(node, scope);

   return compileClosure(node, ownerScope, scope, mode);
}

ObjectInfo Compiler :: compileCollection(SNode node, CodeScope& scope, int mode)
{
   ref_t parentRef = scope.moduleScope->arrayReference;
   SNode parentNode = node.findChild(lxIdentifier, lxPrivate, lxReference);
   if (parentNode != lxNone) {
      parentRef = scope.moduleScope->mapTerminal(parentNode, true);
      if (parentRef == 0)
         scope.raiseError(errUnknownObject, node);
   }

   return compileCollection(node, scope, mode, parentRef);
}

ObjectInfo Compiler :: compileCollection(SNode node, CodeScope& scope, int mode, ref_t vmtReference)
{
   if (vmtReference == 0)
      vmtReference = scope.moduleScope->superReference;

   int counter = 0;

   // all collection memebers should be created before the collection itself
   SNode current = node.findChild(lxExpression);
   while (current != lxNone) {
      compileExpression(current, scope, 0);
      current.refresh();

      if (current != lxExpression) {
         current.injectNode(current.type, current.argument);
      }
      current.set(lxMember, counter);

      current = current.nextNode();
      counter++;
   }

   node.set(lxNested, counter);
   node.appendNode(lxTarget, vmtReference);

   return ObjectInfo(okObject, vmtReference);
}

ObjectInfo Compiler :: compileRetExpression(SNode node, CodeScope& scope, int mode)
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
         typecasting = true;
      }
      else if (classScope->info.methodHints.exist(Attribute(scope.getMessageID(), maType))) {
         subj = classScope->info.methodHints.get(Attribute(scope.getMessageID(), maType));
         typecasting = true;
      }
      else subj = 0;
   }

   ObjectInfo info = compileExpression(node, scope, mode);

   if (typecasting) {
      SNode exprNode = node.firstChild(lxExprMask);
      if (exprNode != lxExpression) {
         // HOTFIX : inject an expression node if required
         exprNode = node.injectNode(lxExpression);
      }                  

      info = typecastObject(exprNode, scope, subj, info);
   }

   return info;
}

ObjectInfo Compiler :: compileNewOperator(SNode node, CodeScope& scope/*, int mode*/)
{
   ObjectInfo retVal(okObject);

   SNode objectNode = node.firstChild(lxTerminalMask);

   retVal.type = scope.mapSubject(objectNode);

   ref_t loperand = scope.moduleScope->attributeHints.get(retVal.type);
   ref_t roperand = resolveObjectReference(scope, compileExpression(objectNode.nextNode(lxObjectMask), scope, 0));
   ref_t targetRef = 0;
   int operationType = _logic->resolveNewOperationType(*scope.moduleScope, loperand, roperand, targetRef);

   if (operationType != 0) {
   //   // if it is a primitive operation
      _logic->injectNewOperation(node, *scope.moduleScope, operationType, retVal.type, targetRef);

      retVal = assignResult(scope, node, targetRef, retVal.type);
   }
   else scope.raiseError(errInvalidOperation, node);

   return retVal;
}

void Compiler :: compileTrying(SNode node, CodeScope& scope)
{
   bool catchNode = false;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxExprMask)) {
         if (catchNode) {
            current.insertNode(lxResult);
         }
         else catchNode = true;

         compileExpression(current, scope, 0);
      }

      current = current.nextNode();
   }
}

void Compiler :: compileAltOperation(SNode node, CodeScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxExprMask)) {
         compileExpression(current, scope, 0);
      }

      current = current.nextNode();
   }

   // inject a nested expression
   node = lxAltExpression;
   SNode altNode = node.injectNode(lxAlt);

   // extract the expression target
   SNode firstExpr = altNode.firstChild(lxObjectMask);
   SNode targetNode = firstExpr.firstChild(lxObjectMask);

   LexicalType targetType = targetNode.type;
   int targetArg = targetNode.argument;

   targetNode.set(lxResult, 0);

   SNode secondExpr = firstExpr.nextNode(lxObjectMask);
   secondExpr.insertNode(lxCurrent, 0);

   SyntaxTree::copyNode(targetNode, node.insertNode(lxVariable).insertNode(targetType, targetArg));

   node.appendNode(lxReleasing, 1);
}

ObjectInfo Compiler :: compileExpression(SNode node, CodeScope& scope, int mode)
{
   ObjectInfo objectInfo;
   SNode child = node.findChild(lxAssign, lxExtension, lxMessage, lxOperator, lxTrying, lxAlt, lxSwitching);
   switch (child.type) {
      case lxAssign:
         objectInfo = compileAssigning(node, scope, mode);
         break;
      case lxMessage:
         objectInfo = compileMessage(node, scope, mode);
         break;
      case lxTrying:
         compileTrying(child, scope);

         objectInfo = ObjectInfo(okObject);
         break;
      case lxAlt:
         compileAltOperation(child, scope);

         objectInfo = ObjectInfo(okObject);
         break;
      case lxSwitching:
         compileSwitch(child, scope);

         objectInfo = ObjectInfo(okObject);
         break;
      case lxExtension:
         objectInfo = compileExtension(node, scope);
         break;
      case lxOperator:
         objectInfo = compileOperator(node, scope, mode);
         break;
      default:
         child = node.firstChild(lxObjectMask);
         SNode nextChild = child.nextNode(lxObjectMask);

         if (child == lxExpression && nextChild == lxNone) {
            // if it is a nested expression
            objectInfo = compileExpression(child, scope, mode);
         }
         else if (test(child.type, lxTerminalMask) && nextChild == lxNone) {
            objectInfo = compileObject(child, scope, mode);
         }
         else objectInfo = compileObject(node, scope, mode);
   }

   return objectInfo;
}

ObjectInfo Compiler :: compileAssigningExpression(SNode assigning, CodeScope& scope, int mode)
{
   insertDebugStep(assigning, dsStep);

   ObjectInfo objectInfo = compileExpression(assigning, scope/*, 0*/, 0);

//   if (test(mode, HINT_VIRTUAL_FIELD)) {
//      // HOTFIX : if it is a virtual field, provide an idle typecast
//      scope.writer->insert(lxTypecasting);
//      appendTerminalInfo(scope.writer, node.FirstTerminal());
//      scope.writer->closeNode();
//   }
//   else if (targetType != 0) {
//   //   ref_t ref = resolveObjectReference(scope, objectInfo);
//
//   //   if (isPrimitiveRef(ref)) {
//   //      scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));
//   //      appendTerminalInfo(scope.writer, node.FirstTerminal());
//   //      scope.writer->closeNode();
//   //      //scope.writer->insert(lxBoxing);
//   //      //scope.writer->appendNode(lxTarget, target.extraparam);
//   //      //appendTerminalInfo(scope.writer, node.FirstTerminal());
//   //      //scope.writer->closeNode();
//   //   }
//      /*else */if (objectInfo.type != targetType) {
//         scope.writer->insert(lxTypecasting, encodeMessage(targetType, GET_MESSAGE_ID, 0));
//         appendTerminalInfo(scope.writer, node.FirstTerminal());
//         scope.writer->closeNode();
//      }      
//   }
//   else if (isPrimitiveRef(target.extraparam)) {
//      ClassInfo info;
//      scope.moduleScope->loadClassInfo(info, resolveObjectReference(scope, objectInfo), true);
//
//      if (target.extraparam == -1 && ((info.header.flags & elDebugMask)  == elDebugDWORD)) {
//
//         // allow assigning an int wrapper to the primitive int
//      }
//      else scope.raiseError(errInvalidOperation, assigning.FirstTerminal());
//   }
//   else if (target.kind != okOuterField && target.extraparam > 0) {
//      ClassInfo info;
//      scope.moduleScope->loadClassInfo(info, target.extraparam, false);
//
//      // wrapper class can be used in this case
//      if (test(info.header.flags, elWrapper)) {
//         target.type = info.fieldTypes.get(0);
//
//         scope.writer->insert(lxTypecasting, encodeMessage(target.type, GET_MESSAGE_ID, 0));
//         scope.writer->closeNode();
//         
//         scope.writer->insert(lxBoxing, info.size);
//         scope.writer->appendNode(lxTarget, target.extraparam);
//         appendTerminalInfo(scope.writer, node.FirstTerminal());         
//         scope.writer->closeNode();
//      }
//      //// HOTFIX : to allow boxing primitive array
//      //else if (test(info.header.flags, elDynamicRole | elStructureRole) && objectInfo.kind == okLocalAddress && objectInfo.extraparam == -3 &&
//      //   (objectInfo.type == info.fieldTypes.get(-1)))
//      //{
//      //   scope.writer->insert(lxBoxing, info.size);
//      //   scope.writer->appendNode(lxTarget, target.extraparam);
//      //   appendTerminalInfo(scope.writer, node.FirstTerminal());
//      //   scope.writer->closeNode();
//      //}
//      else scope.raiseError(errInvalidOperation, assigning.FirstTerminal());
//   }
//
//   scope.writer->removeBookmark();

   return objectInfo;
}

ObjectInfo Compiler :: compileBranching(SNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodeMode*/)
{
   CodeScope subScope(&scope);

   SNode thenCode = thenNode.findSubNode(lxCode);

   SNode expr = thenCode.firstChild(lxObjectMask);
   if (expr == lxEOF || expr.nextNode() != lxNone) {
      compileCode(thenCode, subScope);

      if (subScope.level > scope.level) {
         thenCode.appendNode(lxReleasing, subScope.level - scope.level);
      }
   }
   // if it is inline action
   else compileRetExpression(expr, scope, 0);

   return ObjectInfo(okObject);
}

void Compiler :: compileThrow(SNode node, CodeScope& scope, int mode)
{
   ObjectInfo object = compileExpression(node.firstChild(), scope, mode);
}

void Compiler :: compileLoop(SNode node, CodeScope& scope)
{
   node = lxExpression;

   // find inner expression
   SNode expr = node;
   while (expr.findChild(lxMessage, lxAssign, lxOperator) == lxNone) {
      expr = expr.findChild(lxExpression);
   }

   compileExpression(expr, scope, HINT_LOOP);

   // mark the inner expression as a loop
   expr.set(lxLooping, 0);

//   // if it is while-do loop
//   if (expr.nextNode() == nsL7Operation) {
//      scope.writer->newNode(lxLooping);
//
//      DNode loopNode = expr.nextNode();
//
//      ObjectInfo cond = compileExpression(expr, scope, scope.moduleScope->boolType, 0);
//
//      int operator_id = _operators.get(loopNode.Terminal());
//
//      scope.writer->newNode(lxElse, (operator_id == IF_MESSAGE_ID) ? scope.moduleScope->falseReference : scope.moduleScope->trueReference);
//      compileBranching(loopNode, scope/*, cond, _operators.get(loopNode.Terminal()), HINT_LOOP*/);
//      scope.writer->closeNode();
//
//      scope.writer->closeNode();
//   }
//   // if it is repeat loop
//   else {
//      scope.writer->newNode(lxLooping, scope.moduleScope->trueReference);
//
//      ObjectInfo retVal = compileExpression(node.firstChild(), scope, scope.moduleScope->boolType, 0);
//
//      scope.writer->closeNode();
//   }
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

ObjectInfo Compiler :: compileCode(SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxExpression:
            compileExpression(current, scope, HINT_ROOT);
            insertDebugStep(current, dsStep);
            break;
         case lxThrowing:
            compileThrow(current, scope, 0);
            break;
         case lxLoop:
            insertDebugStep(current, dsStep);
            compileLoop(current, scope);
            break;
//         case nsTry:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            compileTry(statement, scope);
//            break;
//         case nsLock:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
//            compileLock(statement, scope);
//            break;
         case lxReturning:
         {
            needVirtualEnd = false;
            insertDebugStep(current, dsStep);
            retVal = compileRetExpression(current, scope, HINT_ROOT);

            break;
         }
         case lxVariable:
//            recordDebugStep(scope, statement.FirstTerminal(), dsStep);
            compileVariable(current, scope);
            break;
         case lxExtern:
            current = lxExternFrame;
            compileCode(current, scope);
            break;
         case lxEOF:
            needVirtualEnd = false;
            setDebugStep(current, dsEOP);
            break;
      }

      scope.freeSpace();

      current = current.nextNode();
   }

   if (needVirtualEnd) {
      appendDebugStep(node, dsVirtualEnd);
   }

   return retVal;
}

void Compiler :: compileExternalArguments(SNode node, CodeScope& scope/*, ExternalScope& externalScope*/)
{
   ModuleScope* moduleScope = scope.moduleScope;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMessage) {
         ref_t subject = current.argument;
         if (subject != 0) {
            ref_t classReference = moduleScope->attributeHints.get(subject);
            if (classReference) {
               ClassInfo classInfo;
               _logic->defineClassInfo(*moduleScope, classInfo, classReference);

               ref_t primitiveRef = _logic->retrievePrimitiveReference(*moduleScope, classInfo);
               switch (primitiveRef)
               {
                  case V_INT32:
                  case V_SIGNATURE:
                  case V_MESSAGE:
                  case V_VERB:
                     current.set(_logic->isVariable(classInfo) ? lxExtArgument : lxIntExtArgument, 0);
                     break;
                  default:
                     if (test(classInfo.header.flags, elStructureRole)) {
                        current.set(lxExtArgument, 0);

                        subject = 0;
                     }
                     else if (test(classInfo.header.flags, elWrapper)) {
                        //HOTFIX : allow to pass a normal object
                        current.set(lxExtArgument, 0);
                     }
                     else scope.raiseError(errInvalidOperation, current);
                     break;
               }
            }
            else scope.raiseError(errInvalidOperation, current);
         }         
      }

//      int flags = 0;
//
//      flags = classInfo.header.flags;
//
//      if (test(flags, elStructureRole)) {
//         if (testany(flags, elDynamicRole | elEmbeddable | elWrapper)) {
//            //HOTFIX : allow to pass structure
//            if ((flags & elDebugMask) == 0) {
//               flags = elDebugBytes;
//            }
//         }
//         else flags = 0;
//      }
//      else if (test(flags, elWrapper)) {
//         //HOTFIX : allow to pass a normal object
//         flags = elDebugBytes;
//      }
//      else flags = 0;
//
//      LexicalType argType = lxNone;
//      switch (flags & elDebugMask) {
//         // if it is an integer number pass it directly
//         case elDebugDWORD:
//         case elDebugPTR:
//         case elDebugSubject:
//         case elDebugMessage:
//            argType = test(flags, elReadOnlyRole) ? lxIntExtArgument : ;
//            break;
//         case elDebugReference:
//            argType = lxExtInteranlRef;
//            break;
//         case elDebugWideLiteral:
//         case elDebugLiteral:
//         case elDebugIntegers:
//         case elDebugShorts:
//         case elDebugBytes:
//         case elDebugQWORD:
//         case elDebugDPTR:
//            argType = lxExtArgument;
//            break;
//         default:
//            scope.raiseError(errInvalidOperation, terminal);
//            break;
//      }
//
//      arg = arg.nextNode();
//      if (arg == nsMessageParameter) {
//         if (argType == lxExtInteranlRef) {
//            if (isSingleObject(arg.firstChild())) {
//               ObjectInfo target = compileTerminal(arg.firstChild(), scope);
//               if (target.kind == okInternal) {
//                  scope.writer->appendNode(lxExtInteranlRef, target.param);
//               }
//               else scope.raiseError(errInvalidOperation, terminal);
//            }
//            else scope.raiseError(errInvalidOperation, terminal);
//         }
//         else {
//            scope.writer->newNode(argType);
//
//            ObjectInfo info = compileExpression(arg.firstChild(), scope, subject, 0);
//            if (info.kind == okIntConstant) {
//               int value = StringHelper::strToULong(moduleScope->module->resolveConstant(info.param), 16);
//
//               scope.writer->appendNode(lxValue, value);
//            }
//
//            scope.writer->closeNode();
//         }
//
//         arg = arg.nextNode();
//      }

      current = current.nextNode();
   }
}

ObjectInfo Compiler :: compileExternalCall(SNode node, CodeScope& scope, int mode)
{
   ObjectInfo retVal(okExternal);

   ModuleScope* moduleScope = scope.moduleScope;

//   bool rootMode = test(mode, HINT_ROOT);
   bool stdCall = false;
   bool apiCall = false;

   SNode targetNode = node.firstChild(lxTerminalMask);
   // HOTFIX : comment out dll reference
   targetNode = lxIdle;

   insertDebugStep(node.findChild(lxMessage), dsAtomicStep);

   ident_t dllAlias = targetNode.findChild(lxTerminal).identifier();
   ident_t functionName = node.findChild(lxMessage).firstChild(lxTerminalMask).findChild(lxTerminal).identifier();

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

   // To tell apart coreapi calls, the name convention is used
   if (apiCall) {
      node.set(lxCoreAPICall, reference);
   }
   else node.set(stdCall ? lxStdExternalCall : lxExternalCall, reference);

//   if (!rootMode)
//      scope.writer->appendNode(lxTarget, -1);

   compileExternalArguments(node, scope);

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(SNode node, CodeScope& scope, ref_t message, ObjectInfo routine)
{
   ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   int paramCount;
   ref_t sign_ref, verb_id, dummy;
   decodeMessage(message, sign_ref, verb_id, paramCount);

   virtualReference.append('0' + paramCount);
   virtualReference.append('#');
   virtualReference.append(0x20 + verb_id);

   if (sign_ref != 0) {
      virtualReference.append('&');
      virtualReference.append(moduleScope->module->resolveSubject(sign_ref));
   }

   node.set(lxInternalCall, moduleScope->module->mapReference(virtualReference));

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

      ident_t terminal = terminalNode.findChild(lxTerminal).identifier();
      int index = 1 + scope.parameters.Count();
      scope.parameters.add(terminal, Parameter(index));

      arg = arg.nextNode();
   }
   bool first = true;
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxIdentifier, lxPrivate);

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.mapSubject(subject, signature);

      // declare method parameter
      arg = arg.nextNode();

      if (arg == lxMethodParameter) {
         ident_t name = arg.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();

         // !! check duplicates
         if (scope.parameters.exist(name))
            scope.raiseError(errDuplicatedLocal, arg);

         int index = 1 + scope.parameters.Count();
         scope.parameters.add(name, Parameter(index, subj_ref));

         arg = arg.nextNode();
      }
   }

   if (!emptystr(signature))
      sign_id = scope.moduleScope->module->mapSubject(signature, false);

   return encodeMessage(sign_id, EVAL_MESSAGE_ID, scope.parameters.Count());
}

void Compiler :: declareArgumentList(SNode node, MethodScope& scope)
{
   IdentifierString signature;
   ref_t verb_id = 0;
   ref_t sign_id = 0;
   bool first = true;

   SNode verb = node.findChild(lxIdentifier, lxPrivate);
   if (verb != lxNone) {
      //   if (node != nsDefaultGeneric && node != nsImplicitConstructor) {
      verb_id = _verbs.get(verb.findChild(lxTerminal).identifier());
      //
      //	   // if it is a generic verb, make sure no parameters are provided
      //	   if (verb_id == DISPATCH_MESSAGE_ID) {
      //		   scope.raiseError(errInvalidOperation, verb);
      //	   }
      /*else */if (verb_id == 0) {
         sign_id = scope.mapSubject(verb, signature);
      }
      //   }
   }

   SNode arg = node.findChild(lxMethodParameter, lxMessage);
   if (verb_id == 0) {
      // if followed by argument list - it is a EVAL verb
      if (node == lxImplicitConstructor) {
         verb_id = EVAL_MESSAGE_ID;
      }
      else if (arg != lxNone) {
         verb_id = EVAL_MESSAGE_ID;
         first = false;
      }
      // otherwise it is GET message
      else verb_id = GET_MESSAGE_ID;
   }

   int paramCount = 0;
   // if method has generic (unnamed) argument list
   while (arg == lxMethodParameter) {
      int index = 1 + scope.parameters.Count();

      ident_t terminal = arg.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();
      if (scope.parameters.exist(terminal))
         scope.raiseError(errDuplicatedLocal, arg);

      //// if it is shorthand of eval &subj - recognize the subject
      //if (verb_id == EVAL_MESSAGE_ID && sign_id != 0 && paramCount == 0 && arg.nextNode() != nsMessageParameter) {
      //   scope.parameters.add(arg.Terminal(), Parameter(index, sign_id));
      //}
      /*else */scope.parameters.add(terminal, Parameter(index));
      paramCount++;

      arg = arg.nextNode();
   }

   // if method has named argument list
   while (arg == lxMessage) {
      SNode subject = arg.findChild(lxIdentifier, lxPrivate);

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ref_t subj_ref = scope.mapSubject(subject, signature);

      arg = arg.nextNode();

      if (arg == lxMethodParameter) {
         ident_t name = arg.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();

         if (scope.parameters.exist(name))
            scope.raiseError(errDuplicatedLocal, arg);

         int index = 1 + scope.parameters.Count();

         // if it is an open argument type
         if (scope.moduleScope->attributeHints.exist(subj_ref, scope.moduleScope->paramsReference)) {
            scope.parameters.add(name, Parameter(index, subj_ref));

            // the generic arguments should be free by the method exit
            scope.rootToFree += paramCount;
            scope.withOpenArg = true;

            // to indicate open argument list
            paramCount += OPEN_ARG_COUNT;
            if (paramCount > 0xF)
               scope.raiseError(errNotApplicable, arg);
         }
         else {
            paramCount++;
            if (paramCount >= OPEN_ARG_COUNT)
               scope.raiseError(errTooManyParameters, verb);

            int size = subj_ref != 0 ? _logic->defineStructSize(*scope.moduleScope, 
               scope.moduleScope->attributeHints.get(subj_ref), 0, true) : 0;

            scope.parameters.add(name, Parameter(index, subj_ref, 0, size));

            arg = arg.nextNode();
         }
      }
   }

   // do not set message on the second pass
   if (scope.message == 0) {
      if (scope.generic) {
         if (!emptystr(signature))
            scope.raiseError(errInvalidHint, verb);

         signature.copy(GENERIC_PREFIX);
      }

      // if signature is presented
      if (!emptystr(signature)) {
         sign_id = scope.moduleScope->module->mapSubject(signature, false);
      }

      scope.message = encodeMessage(sign_id, verb_id, paramCount);
   }
}

void Compiler :: compileDispatcher(SNode node, MethodScope& scope, bool withGenericMethods)
{
   CodeScope codeScope(&scope);

//   CommandTape* tape = scope.tape;

   node.appendNode(lxSourcePath, scope.getSourcePathRef());  // the source path

   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, node.findChild(lxReference).findChild(lxTerminal).identifier(), scope.message);
   }
   else {
      // if it is generic handler with redirect statement / redirect statement
      if (node.firstChild(lxObjectMask) != lxNone) {
         node.injectNode(lxExpression);

         if (withGenericMethods) {
            node.insertNode(lxDispatching, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));
         }
         compileDispatchExpression(node.findChild(lxExpression), codeScope);
      }
      // if it is generic handler only
      else if (withGenericMethods) {
         SNode exprNode = node.appendNode(lxDispatching).appendNode(lxResending);

         exprNode.appendNode(lxMessage, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0, 0));
         exprNode.appendNode(lxTarget, scope.moduleScope->superReference).appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
      }
   }
}

void Compiler :: compileActionMethod(SNode node, MethodScope& scope)
{
   CodeScope codeScope(&scope/*, &writer*/);

   declareParameterDebugInfo(node, scope, false, true);

   SNode body = node.findChild(lxCode, lxReturning);
   if (body == lxReturning) {
      // HOTFIX : if it is an returning expression, inject returning node
      SNode expr = body.findChild(lxExpression);
      expr = lxReturning;
   }

   // new stack frame
   // stack already contains previous $self value
   codeScope.level++;

   body = lxNewFrame;

//   if (isReturnExpression(node.firstChild())) {
//      compileRetExpression(node.firstChild(), codeScope, HINT_ROOT);
//   }
//   else if (node == nsInlineExpression) {
//      // !! this check should be removed, as it is no longer used
//      compileCode(node.firstChild(), codeScope);
//   }
   /*else */compileCode(body, codeScope);

   node.appendNode(lxParamCount, scope.parameters.Count() + 1);
   node.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileLazyExpressionMethod(SNode node, MethodScope& scope)
{
   CodeScope codeScope(&scope);

   declareParameterDebugInfo(node, scope, false, false);

   SNode body = node.findChild(lxCode);

   // new stack frame
   // stack already contains previous $self value
   codeScope.level++;

   body = lxNewFrame;

   compileRetExpression(body.findChild(lxExpression), codeScope, 0);

   node.appendNode(lxParamCount, scope.parameters.Count() + 1);
   node.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileDispatchExpression(SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, node.findChild(lxReference).findChild(lxTerminal).identifier(), scope.getMessageID());
   }
   else {
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      // try to implement light-weight resend operation
      ObjectInfo target;
      if (isSingleStatement(node)) {
         target = scope.mapObject(node.firstChild(lxTerminalMask));
      }

      if (target.kind == okConstantSymbol || target.kind == okField) {
         node.set(lxResending, methodScope->message);

         SNode exprNode = node.firstChild(lxTerminalMask);
         exprNode.set(lxExpression, 0);
      
         if (target.kind == okField) {
            exprNode.appendNode(lxResultField, target.param);
         }
         else exprNode.appendNode(lxConstantSymbol, target.param);
      }
      else {
         node.set(lxResending, methodScope->message);
         SNode body = node.injectNode(lxNewFrame);
      
         ObjectInfo target = compileExpression(body, scope, 0);
      }
   }
}

void Compiler :: compileConstructorResendExpression(SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
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
         node = lxNewFrame;
         scope.level++;
      }
      else node = lxExpression;

      expr.insertNode(lxResult);

      if (withFrame) {
         compileExtensionMessage(expr, scope, ObjectInfo(okConstantClass, 0, classRef));

         // HOT FIX : inject saving of the created object
         SNode codeNode = node.findChild(lxCode);
         if (codeNode != lxNone) {
            SNode assignExpr = codeNode.insertNode(lxAssigning);
            assignExpr.appendNode(lxLocal, 1);
            assignExpr.appendNode(lxResult);         
         }
      }
      else compileExtensionMessage(expr, scope, ObjectInfo(okConstantClass, 0, classRef));
   }
   else scope.raiseError(errUnknownMessage, node);
}

void Compiler :: compileConstructorDispatchExpression(SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(node, *scope.moduleScope, node.findChild(lxReference).findChild(lxTerminal).identifier(), scope.getMessageID());
   }
   else scope.raiseError(errInvalidOperation, node);
}

void Compiler :: compileResendExpression(SNode node, CodeScope& scope)
{
   node = lxNewFrame;
   // new stack frame
   // stack already contains current $self reference
   scope.level++;

   SNode expr = node.findChild(lxExpression);

   expr.insertNode(lxThisLocal, 1);

   compileMessage(expr, scope, 0);

   scope.freeSpace();
}

void Compiler :: compileMethod(SNode node, MethodScope& scope)
{
   int paramCount = getParamCount(scope.message);

   CodeScope codeScope(&scope);

   node.appendNode(lxSourcePath, scope.getSourcePathRef());  // the source path

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   // check if it is a resend
   if (body == lxResendExpression) {
      compileResendExpression(body, codeScope);
   }
   // check if it is a dispatch
   else if (body == lxDispatchCode) {
      compileDispatchExpression(body, codeScope);
   }
   else {
      if (body == lxReturning) {
         // HOTFIX : if it is an returning expression, inject returning node
         SNode expr = body.findChild(lxExpression);
         expr = lxReturning;
      }

      body = lxNewFrame;
      body.setArgument(scope.generic ? -1 : 0u);

      // new stack frame
      // stack already contains current $self reference
      // the original message should be restored if it is a generic method
      codeScope.level++;
      // declare the current subject for a generic method
      if (scope.generic) {
         codeScope.level++;
         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, 0, V_MESSAGE, 0);
      }

      ObjectInfo retVal = compileCode(body, codeScope);

      // if the method returns itself
      if(retVal.kind == okUnknown) {
         // adding the code loading $self
         SNode exprNode = body.appendNode(lxExpression);
         SNode localNode = exprNode.appendNode(lxLocal, 1);

         ref_t typeAttr = scope.getReturningType(false);
         if (typeAttr != 0) {
            // HOTFIX : copy EOP coordinates
            SNode eop = body.lastChild().prevNode();
            if (eop != lxNone)
               SyntaxTree::copyNode(eop, localNode);

            typecastObject(exprNode, codeScope, typeAttr, ObjectInfo(okThisParam));
         }
      }
   }

   node.appendNode(lxParamCount, paramCount + scope.rootToFree);
   node.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef)
{
   CodeScope codeScope(&scope);

   node.appendNode(lxSourcePath, scope.getSourcePathRef());  // the source path 

   bool retExpr = false;
   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();

   SNode bodyNode = node.findChild(lxResendExpression, lxCode, lxReturning, lxDispatchCode);
   if (bodyNode == lxDispatchCode) {
      compileConstructorDispatchExpression(bodyNode, codeScope);
      return;
   }
   else if (bodyNode == lxResendExpression) {
      compileConstructorResendExpression(bodyNode, codeScope, classClassScope, withFrame);

      bodyNode = bodyNode.findChild(lxCode);
   }
   else if (bodyNode == lxReturning) {
      retExpr = true;

      // HOTFIX : if it is an returning expression, inject returning node
      SNode expr = bodyNode.findChild(lxExpression);
      expr = lxReturning;
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(encodeVerb(NEWOBJECT_MESSAGE_ID))) {
      node.insertNode(lxCalling, -1);

      // HOTFIX : body node should be found once again
      bodyNode = node.findChild(lxCode);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node);

   if (bodyNode != nsNone) {
      if (!withFrame) {
         withFrame = true;

         bodyNode = lxNewFrame;

         // new stack frame
         // stack already contains $self value
         codeScope.level++;
      }

      if (retExpr) {
         SNode expr = bodyNode.findChild(lxReturning);
//         recordDebugStep(codeScope, bodyNode.firstChild().FirstTerminal(), dsStep);

         ObjectInfo retVal = compileRetExpression(expr, codeScope, /*HINT_CONSTRUCTOR_EPXR*/0);

         if(!convertObject(expr, codeScope, codeScope.getClassRefId(), retVal))
            scope.raiseError(errIllegalConstructor, node);
      }
      else {
         compileCode(bodyNode, codeScope);

         // HOT FIX : returning the created object
         bodyNode.appendNode(lxLocal, 1);
      }
   }
   node.appendNode(lxParamCount, getParamCount(scope.message) + 1);
   node.appendNode(lxReserved, scope.reserved);
}

void Compiler :: compileDefaultConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   if (test(classScope->info.header.flags, elStructureRole)) {
      if (!test(classScope->info.header.flags, elDynamicRole)) {
         node.appendNode(lxCreatingStruct, classScope->info.size).appendNode(lxTarget, classScope->reference);
      }
   }
   else if (!test(classScope->info.header.flags, elDynamicRole)) {
      node.appendNode(lxCreatingClass, classScope->info.fields.Count()).appendNode(lxTarget, classScope->reference);
   }
}

void Compiler :: compileDynamicDefaultConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   if (test(classScope->info.header.flags, elStructureRole)) {
      node.appendNode(lxCreatingStruct, classScope->info.size).appendNode(lxTarget, classScope->reference);
   }
   else node.appendNode(lxCreatingClass, -1).appendNode(lxTarget, classScope->reference);
}

void Compiler :: initialize(Scope& scope, MethodScope& methodScope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   methodScope.stackSafe = _logic->isMethodStacksafe(classScope->info, methodScope.message);
   methodScope.classEmbeddable = _logic->isEmbeddable(classScope->info);
   methodScope.generic = _logic->isMethodGeneric(classScope->info, methodScope.message);

}

void Compiler :: compileTemplateMethods(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplate && current.existChild(lxClassMethod)) {
         TemplateScope templateScope(&scope);
         templateScope.loadParameters(current);
         templateScope.sourceRef = _writer.writeString(current.findChild(lxSourcePath).identifier());

         if (node == lxClassMethod || node == lxIdle) {
            ident_t signature = scope.moduleScope->module->resolveSubject(getSignature(node.argument));
            IdentifierString customVerb(signature, signature.find('&', getlength(signature)));

            templateScope.parameters.add(TARGET_PSEUDO_VAR, templateScope.parameters.Count() + 1);
            templateScope.subjects.add(templateScope.parameters.Count(), scope.moduleScope->module->mapSubject(customVerb, false));
         }

         compileVMT(current, templateScope);
      }
      current = current.nextNode();
   }
}

void Compiler :: compileVMT(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();

   while (current != nsNone) {
      switch(current) {
         case lxClassMethod:
         {
            MethodScope methodScope(&scope);
            methodScope.message = current.argument;

            // if it is a dispatch handler
//            if (member.firstChild() == nsDispatchHandler) {
            if (methodScope.message == encodeVerb(DISPATCH_MESSAGE_ID)) {
//               if (test(scope.info.header.flags, elRole))
//                  scope.raiseError(errInvalidRoleDeclr, member.Terminal());
               
               initialize(scope, methodScope);

               compileDispatcher(current.firstChild(lxCodeScopeMask), methodScope, test(scope.info.header.flags, elWithGenerics));
            }
            // if it is a normal method
            else {
               declareArgumentList(current, methodScope);

               initialize(scope, methodScope);

               declareParameterDebugInfo(current, methodScope, true, /*test(codeScope.getClassFlags(), elRole)*/false);
               compileMethod(current, methodScope);
            }

            compileTemplateMethods(current, scope);
            break;
         }
         case lxIdle:
            compileTemplateMethods(current, scope);
            break;
         //case lxDefaultGeneric:
         //{
         //   MethodScope methodScope(&scope);
         //   declareArgumentList(current, methodScope);

         //   // override subject with generic postfix
         //   methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));

         //   // mark as having generic methods
         //   scope.info.header.flags |= elWithGenerics;
         //   methodScope.generic = true;

         //   compileMethod(current, methodScope);
         //   break;
         //}
//         case nsImplicitConstructor:
//         {
//            MethodScope methodScope(&scope);
//            declareArgumentList(member, methodScope);
//
//            // override message with private verb
//            methodScope.message = overwriteVerb(methodScope.message, PRIVATE_MESSAGE_ID);
//
//            int hint = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
//            methodScope.stackSafe = test(hint, tpStackSafe);
               // 
         //initialize(scope, methodScope);
//            compileMethod(member, writer, methodScope);
//
//            break;
//         }
         case lxTemplate:
         {
            TemplateScope templateScope(&scope);
            templateScope.loadParameters(current);

            SNode sourceNode = current.findChild(lxSourcePath);
            if (sourceNode != lxNone)
               templateScope.sourceRef = _writer.writeString(sourceNode.identifier());

            compileVMT(current, templateScope);
         }
      }

      current = current.nextNode();
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (test(scope.info.header.flags, elWithGenerics) && scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), false)) {
      MethodScope methodScope(&scope);
      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);

      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);

      compileDispatcher(methodNode, methodScope, true);
   }
}

void Compiler :: compileFieldDeclarations(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();

   while (current != nsNone) {
      if (current == lxClassField) {
         compileFieldAttributes(current, scope, current);
      }
      else if (current == lxTemplate) {
         TemplateScope templateScope(&scope);
         templateScope.loadParameters(current);

         compileFieldDeclarations(current, templateScope);
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
   _writer.save(tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.moduleScope->sourcePathRef);
}

//void Compiler :: compilePreloadedCode(SymbolScope& scope)
//{
//   _Module* module = scope.moduleScope->module;
//
//   ReferenceNs sectionName(module->Name(), INITIALIZER_SECTION);
//
//   CommandTape tape;
//   _writer.generateInitializer(tape, module->mapReference(sectionName), lxSymbol, scope.reference);
//
//   // create byte code sections
//   _writer.save(tape, module, NULL, NULL, 0);
//}

inline bool copyConstructors(SyntaxWriter& writer, SNode node)
{
   // copy constructors
   SNode current = node.firstChild();
   bool inheritedConstructors = true;
   while (current != lxNone) {
      if (current == lxConstructor) {
         writer.newNode(lxClassMethod);
         SyntaxTree::copyNode(writer, current);
         writer.closeNode();

         inheritedConstructors = false;
      }
      current = current.nextNode();
   }

   return inheritedConstructors;
}

void Compiler :: compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxClass, classClassScope.reference);

   bool withDefaultConstructor = _logic->isDefaultConstructorEnabled(classScope.info);
   bool inheritedConstructors = copyConstructors(writer, node) && withDefaultConstructor;

   // if no construtors are defined inherits the default one
   if (inheritedConstructors) {
      if (classScope.info.header.parentRef == 0)
         classScope.raiseError(errNoConstructorDefined, node.findChild(lxIdentifier, lxPrivate));

      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.moduleScope->superReference));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }
   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef, true);

   // class class is always stateless and sealed
   writer.appendNode(lxClassFlag, elStateless);
   writer.appendNode(lxClassFlag, elSealed);

   writer.closeNode();

   SNode member = tree.readRoot();
   declareVMT(member.firstChild(), classClassScope);

   // add virtual constructor
   if (withDefaultConstructor) {
      member.appendNode(lxClassMethod, encodeVerb(NEWOBJECT_MESSAGE_ID));
   }

   generateClassDeclaration(member, classClassScope, false);

   // save declaration
   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxClass, classClassScope.reference);

   copyConstructors(writer, node);

   if (_logic->isDefaultConstructorEnabled(classScope.info)) {
      writer.appendNode(lxClassMethod, encodeVerb(NEWOBJECT_MESSAGE_ID));
   }
   
   writer.closeNode();

   SNode current = tree.readRoot().firstChild();
   while (current != nsNone) {
      //DNode hints = skipHints(member);
   
      if (current == lxClassMethod) {
         MethodScope methodScope(&classScope);
   
         compileMethodAttributes(current, methodScope, current);

         if (current.argument == encodeVerb(NEWOBJECT_MESSAGE_ID)) {
            if (test(classScope.info.header.flags, elDynamicRole)) {
               compileDynamicDefaultConstructor(current, methodScope, classClassScope);
            }
            else compileDefaultConstructor(current, methodScope, classClassScope);
         }
         else {
            //compileMethodHints(hints, writer, methodScope);
            declareArgumentList(current, methodScope);
            current.setArgument(methodScope.message);

            //int hint = classClassScope.info.methodHints.get(Attribute(methodScope.message, maHint));
            methodScope.stackSafe = _logic->isMethodStacksafe(classClassScope.info, methodScope.message);

            declareParameterDebugInfo(current, methodScope, true, false);

            compileConstructor(current, methodScope, classClassScope);
         }
      }
      current = current.nextNode();
   }
   
   generateClassImplementation(tree.readRoot(), classClassScope);
}

//inline bool isClassMethod(Symbol symbol)
//{
//   switch (symbol)
//   {
//      case nsMethod:
//      case nsDefaultGeneric:
//      case nsImplicitConstructor:
//         return true;
//      default:
//         return false;
//   }
//}

void Compiler :: declareTemplateMethods(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplate && current.existChild(lxClassMethod)) {
         TemplateScope templateScope(&scope);
         templateScope.loadParameters(current);

         declareVMT(current.firstChild(), templateScope);
      }
      current = current.nextNode();
   }
}

void Compiler :: declareVMT(SNode current, ClassScope& scope)
{
   while (current != lxNone) {
//      DNode hints = skipHints(member);

      if (current == lxClassMethod || current == lxImplicitConstructor || current == lxDefaultGeneric) {
         bool dispatchMethod = current == lxClassMethod && current.findChild(lxIdentifier, lxPrivate) == lxNone;

         MethodScope methodScope(&scope);

         compileMethodAttributes(current, methodScope, current);

//         DNode firstChild = member.firstChild();
//         if (firstChild == nsDispatchHandler) {
         if (dispatchMethod) {
            methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
         }
         else {
            declareArgumentList(current, methodScope);
            if (current == lxDefaultGeneric) {
               // override subject with generic postfix
               methodScope.message = overwriteSubject(methodScope.message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));

               current.appendNode(lxClassMethodAttr, tpGeneric);

               current = lxClassMethod;
            }
            else if (current == lxImplicitConstructor) {
               methodScope.message = overwriteVerb(methodScope.message, PRIVATE_MESSAGE_ID);

               current = lxClassMethod;
            }
         }

         current.setArgument(methodScope.message);

         // mark as having generic methods
         if (current == lxDefaultGeneric)
            current.parentNode().appendNode(lxClassFlag, elWithGenerics);

         declareTemplateMethods(current, scope);
      }
      else if (current == lxTemplate) {
         TemplateScope templateScope(&scope);
         templateScope.loadParameters(current);

         declareVMT(current.firstChild(), templateScope);
      }

      current = current.nextNode();
   }
}

ref_t Compiler :: generateTemplate(SNode attribute, TemplateScope& scope)
{
   if (scope.moduleScope->loadClassInfo(scope.info, scope.reference, true)) {
      return scope.reference;
   }   

   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot, scope.reference);

//   if (templateInfo.templateParent != 0) {
//      compileParentDeclaration(DNode(), scope, templateInfo.templateParent);
//   }
   /*else */compileParentDeclaration(SNode(), scope);

   writer.appendNode(lxClassFlag, elSealed);
   writer.closeNode();

   _Memory* body = scope.moduleScope->loadAttributeInfo(scope.templateRef);

   // import the template
   SyntaxTree::loadNode(syntaxTree.readRoot(), body);
   scope.loadParameters(syntaxTree.readRoot());

   // declare the template class
   compileClassAttributes(syntaxTree.readRoot(), scope, syntaxTree.readRoot());
   compileFieldDeclarations(syntaxTree.readRoot(), scope);

   declareVMT(syntaxTree.readRoot().firstChild(), scope);

   generateClassDeclaration(syntaxTree.readRoot(), scope, false);
   scope.save();

   // implement the template class
   compileVMT(syntaxTree.readRoot(), scope);
   generateClassImplementation(syntaxTree.readRoot(), scope);

   return scope.reference;
}

void Compiler :: generateClassFlags(ClassScope& scope, SNode root)
{
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxTemplate) {
         generateClassFlags(scope, current);
      }
      else if (current == lxClassFlag) {
         if (_logic->validateClassFlag(scope.info, current.argument)) {
            scope.compileClassAttribute(current);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }
      else if (current == lxType) {
         scope.compileClassAttribute(current);
      }

      current = current.nextNode();
   }
}

//ref_t Compiler :: declareInlineTemplate(ModuleScope& scope, SNode node, TemplateInfo& templateInfo, ref_t inlineTemplateRef)
//{
//   TemplateInfo fieldTemplate;
//   fieldTemplate.templateRef = inlineTemplateRef;
//
//   ReferenceNs fulName(scope.module->Name(), scope.module->resolveSubject(fieldTemplate.templateRef));
//   SNode param = node.firstChild();
//   while (param != lxNone) {
//      if (param == lxTemplateParam) {
//         ref_t optionRef = templateInfo.parameters.get(param.argument);
//
//         fieldTemplate.parameters.add(fieldTemplate.parameters.Count() + 1, optionRef);
//
//         fulName.append('@');
//         fulName.append(scope.module->resolveSubject(optionRef));
//      }
//      param = param.nextNode();
//   }
//   ref_t classRef = scope.module->mapReference(fulName);
//   ref_t fieldType = scope.typifiedClasses.get(classRef);
//   if (!fieldType) {
//      fieldType = scope.mapNestedTemplate();
//      scope.saveSubject(fieldType, classRef, true);
//   }         
//
//   generateTemplate(scope, fieldTemplate, classRef);
//
//   return fieldType;
//}
//
//bool Compiler ::importTemplateDeclaration(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo)
//{
//   ModuleScope* moduleScope = scope.moduleScope;
//
//   _Module* extModule = NULL;
//   _Memory* section = moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule);
//   if (!section)
//      return false;
//
//   SyntaxTree tree(section);
//   SNode current = tree.readRoot();
//   current = current.firstChild();
//   while (current != lxNone) {
//      if (current == lxTemplateField) {
//         writer.newNode(lxTemplateField, current.argument);
//
//         SNode attr = current.firstChild();
//         while (attr != lxNone) {
//            if (attr == lxTemplateFieldType) {               
//               writer.appendNode(lxType, templateInfo.parameters.get(attr.argument));
//            }
//            else if (attr == lxTemplate) {
//               writer.appendNode(lxType, declareInlineTemplate(*scope.moduleScope, attr, templateInfo, 
//                  importSubject(extModule, attr.argument, moduleScope->module)));
//            }
//            else copyNode(scope, attr, writer, extModule, templateInfo);
//
//            attr = attr.nextNode();
//         }
//         writer.closeNode();
//      }
//      else if (current == lxTemplateSubject) {
//         templateInfo.parameters.add(templateInfo.parameters.Count() + 1, templateInfo.messageSubject);
//      }
//      else if (current == lxClassMethod/* || current == lxTargetMethod*/) {
//         bool withGenericAttr = false;
//         ref_t messageRef = templateInfo.targetMessage;
//   //      if (current == lxClassMethod) {
//            messageRef = overwriteSubject(current.argument, importTemplateSubject(extModule, moduleScope->module, getSignature(current.argument), templateInfo));
//
//            writer.newNode(lxTemplateMethod, messageRef);
//   //      }
//   //      else writer.newNode(lxTargetMethod, messageRef);
//                  
//         SNode attr = current.firstChild();
//         while (attr != lxNone) {
//            if (attr == lxClassMethodAttr) {
//               writer.appendNode(lxClassMethodAttr, attr.argument);
//               if (attr.argument == tpGeneric)
//                  withGenericAttr = true;
//            }
//            else if (attr == lxType) {
//               writer.appendNode(lxType, importTemplateSubject(extModule, moduleScope->module, attr.argument, templateInfo));
//            }
//
//            attr = attr.nextNode();
//         }
//         writer.closeNode();
//
//         //HOTFIX : recognize generic handler
//         if (withGenericAttr)
//            writer.appendNode(lxClassFlag, elWithGenerics);
//      }
//      else if (current == lxTemplate) {
//         TemplateInfo extTemplate;
//         importTemplateInfo(current, scope, scope.reference, extModule, templateInfo);
//      }
//      else if (current == lxClassFlag) {
//         writer.appendNode(current.type, current.argument);
//      }
//   
//      current = current.nextNode();
//   }
//
//   return true;
//}
//
//void Compiler :: readFieldTermplateHints(ModuleScope& scope, ref_t hintRef, ref_t& target, int& size)
//{
//   //HOTFIX : declare field template methods
//   _Module* extModule = NULL;
//   _Memory* section = scope.loadTemplateInfo(hintRef, extModule);
//   if (!section)
//      return;
//
//   SyntaxTree tree(section);
//   SNode current = tree.readRoot();
//   current = current.firstChild();
//   while (current != lxNone) {
//      if (current == lxTarget) {
//         target = current.argument;
//      }
//      else if (current == lxSize) {
//         size = current.argument;
//      }
//      current = current.nextNode();
//   }
//}
//
//bool Compiler :: readSymbolTermplateHints(SymbolScope& scope, ref_t hintRef)
//{
//   //HOTFIX : declare field template methods
//   _Module* extModule = NULL;
//   _Memory* section = scope.moduleScope->loadTemplateInfo(hintRef, extModule);
//   if (!section)
//      return false;
//
//   SyntaxTree tree(section);
//   SNode current = tree.readRoot();
//   current = current.firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxConstAttr:
//            scope.constant = true;
//            break;
//         case lxPreloadedAttr:
//            scope.preloaded = true;
//            break;
//         default:
//            return false;
//      }
//
//      current = current.nextNode();
//   }
//
//   return true;
//}

void Compiler :: generateClassField(ClassScope& scope, SyntaxTree::Node current, bool singleField)
{
   ModuleScope* moduleScope = scope.moduleScope;

   int offset = 0;
   ident_t terminal = current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();

   ref_t typeAttr = current.findChild(lxType).argument;
   ref_t classRef = typeAttr != 0 ? moduleScope->attributeHints.get(typeAttr) : 0;
   int sizeHint = current.findChild(lxSize).argument;

   // a role cannot have fields
   if (test(scope.info.header.flags, elStateless))
      scope.raiseError(errIllegalField, current);

   int size = (typeAttr != 0) ? _logic->defineStructSize(*moduleScope, classRef) : 0;
   if (sizeHint != 0) {
//      if (size < 0) {
//         size = sizeHint * (-size);
//      }
      /*else */if (size == 0) {
         size = sizeHint;
      }
      else scope.raiseError(errIllegalField, current);
   }

   SNode attr = current.firstChild(lxFieldAttrMask);
   if (test(scope.info.header.flags, elWrapper) && scope.info.fields.Count() > 0) {
      // wrapper may have only one field
      scope.raiseError(errIllegalField, current);
   }
   // if it is a primitive data wrapper
   else if (attr != lxNone) {
      if (classRef != 0 || testany(scope.info.header.flags, elNonStructureRole | elDynamicRole))
         scope.raiseError(errIllegalField, current);

      if (test(scope.info.header.flags, elStructureRole)) {
         scope.info.fields.add(terminal, scope.info.size);
         scope.info.size += size;
      }
      else scope.raiseError(errIllegalField, current);

      if (!_logic->tweakPrimitiveClassFlags(attr.type, scope.info))
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

         scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(classRef, typeAttr));
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
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeAttr));
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
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeAttr));
      }
      // if it is a normal field
      else {
         // primitive / virtual classes cannot be declared
         if (size != 0 && _logic->isPrimitiveRef(classRef))
            scope.raiseError(errIllegalField, current);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.Count();
         scope.info.fields.add(terminal, offset);

         if (typeAttr != 0)
            scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, typeAttr));
      }
   }
}

void Compiler :: generateClassStaticField(ClassScope& scope, SNode current)
{
   _Module* module = scope.moduleScope->module;

   ident_t terminal = current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier();
   ref_t typeHint = current.findChild(lxType).argument;

   if (scope.info.statics.exist(terminal))
      scope.raiseError(errDuplicatedField, current);

   // generate static reference
   ReferenceNs name(module->resolveReference(scope.reference));
   name.append(STATICFIELD_POSTFIX);

   findUninqueName(module, name);

   scope.info.statics.add(terminal, ClassInfo::FieldInfo(module->mapReference(name), typeHint));
}

inline int countFields(SNode node)
{
   int counter = 0;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassField) {
         counter++;
      }
      else if (current == lxTemplate)
         counter += countFields(current);

      current = current.nextNode();
   }

   return counter;
}

void Compiler :: generateClassFields(ClassScope& scope, SNode root, bool singleField)
{
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassField/* || current == lxTemplateField*/) {
         bool isStatic = current.existChild(lxStaticAttr);
         if (isStatic) {
            generateClassStaticField(scope, current);
         }
         else generateClassField(scope, current, singleField);
      }
      else if (current == lxTemplate) {
         generateClassFields(scope, current, singleField);
      }

      current = current.nextNode();
   }
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, ref_t& message)
{
   ref_t outputType = scope.info.methodHints.get(Attribute(message, maType));
   bool hintChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));
//   int methodType = hint & tpMask;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethodAttr) {
//         if ((current.argument & tpMask) == 0 || methodType == 0)
         if (current.argument == tpSealed && node.existChild(lxPrivate)) {
            //HOTFIX : private sealed method should be marked appropriately            
            hint |= tpPrivate;
         }
         else {
            hint |= current.argument;

            if (current.argument == tpAction)
               scope.moduleScope->saveAction(message, scope.reference);

            //HOTFIX : overwrite the message for the generic one
            if (hint == tpGeneric) {
               message = overwriteSubject(message, scope.moduleScope->module->mapSubject(GENERIC_PREFIX, false));
            }
         }

         hintChanged = true;
      }
      else if (current == lxType) {
         if (outputType == 0) {
            outputType = current.argument;
            scope.info.methodHints.add(Attribute(message, maType), outputType);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnTypeAlreadyDeclared, node);
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

   if (hintChanged) {
      scope.info.methodHints.exclude(Attribute(message, maHint));
      scope.info.methodHints.add(Attribute(message, maHint), hint);
   }
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SNode root, bool hideDuplicates, bool closed)
{
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         ref_t message = current.argument;

         generateMethodAttributes(scope, current, message);

         int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(message, maHint));
         bool privat = (methodHints & tpMask) == tpPrivate;

         if (_logic->isMethodGeneric(scope.info, message)) {
            scope.info.header.flags |= elWithGenerics;

            // HOTFIX : override the generic method attribute
            current.setArgument(message);
         }

         // check if there is no duplicate method
         if (scope.info.methods.exist(message, true)) {
            if (hideDuplicates) {
               current = lxIdle;
            }
            else scope.raiseError(errDuplicatedMethod, current);
         }
         else {
            if (privat) {
               //HOTFIX : private sealed method should be not be accessible outside the class
               message = overwriteVerb(message, PRIVATE_MESSAGE_ID);

               current.setArgument(message);
            }

            bool included = scope.include(message);
            bool sealedMethod = (methodHints & tpMask) == tpSealed;
            // if the class is closed, no new methods can be declared
            if (included && closed)
               scope.raiseError(errClosedParent, current);
            
            // if the method is sealed, it cannot be overridden
            if (!included && sealedMethod)
               scope.raiseError(errClosedMethod, current);
            
            // save extensions if required ; private method should be ignored
            if (test(scope.info.header.flags, elExtension) && !root.existChild(lxPrivate)) {
               scope.moduleScope->saveExtension(message, scope.extensionMode, scope.reference);
            }
         }

         // HOTFIX : generate nested template methods
         generateMethodDeclarations(scope, current, true, closed);
      }
      else if (current == lxTemplate) {
         generateMethodDeclarations(scope, current, true, closed);
      }
      current = current.nextNode();
   }
}

void Compiler :: generateClassDeclaration(SNode node, ClassScope& scope, bool closed)
{
   // generate flags
   generateClassFlags(scope, node);

   // generate fields
   generateClassFields(scope, node, countFields(node) == 1);

//   // define the data type for the array
//   if (test(scope.info.header.flags, elDynamicRole) && (scope.info.header.flags & elDebugMask) == 0) {
//      if (test(scope.info.header.flags, elStructureRole)) {
//         ref_t fieldClassRef = scope.moduleScope->subjectHints.get(scope.info.fieldTypes.get(-1));
//
//         int fieldFlags = scope.moduleScope->getClassFlags(fieldClassRef) & elDebugMask;
//         if (fieldFlags == elDebugDWORD) {
//            switch (scope.info.size)
//            {
//               case -4:
//                  scope.info.header.flags |= elDebugIntegers;
//                  break;
//               case -2:
//                  scope.info.header.flags |= elDebugShorts;
//                  break;
//               case -1:
//                  scope.info.header.flags |= elDebugBytes;
//                  break;
//            }
//         }
//         else if ((scope.info.size == -4) && (fieldFlags == elDebugMessage || fieldFlags == elDebugSubject)) {
//            scope.info.header.flags |= elDebugIntegers;
//         }
//      }
//   }
//
//   //HOTFIX : recognize pointer structure
//   if (test(scope.info.header.flags, elStructureRole | elPointer)
//      && ((scope.info.header.flags & elDebugMask) == 0) && scope.info.fields.Count() == 1)
//   {
//      switch (scope.moduleScope->getTypeFlags(scope.info.fieldTypes.get(0)) & elDebugMask)
//      {
//         case elDebugDWORD:
//            scope.info.header.flags |= elDebugPTR;
//            break;
//         case elDebugQWORD:
//            scope.info.header.flags |= elDebugDPTR;
//            break;
//         default:
//            scope.info.header.flags &= ~elPointer;
//            break;
//      }
//   }

   _logic->injectVirtualCode(node, *scope.moduleScope, scope.info, *this);

   // generate methods
   generateMethodDeclarations(scope, node, false, closed);

   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info);

//   // declare virtual methods
//   if (!closed)
//      declareVirtualMethods(scope);
}

////bool Compiler :: validateMethodTemplate(SyntaxTree::Node node, ref_t& targetMethod)
////{
////   SNode current = node.firstChild();
////   while (current != lxNone) {
////      if (current == lxTemplateCalling) {
////         if (targetMethod == 0) {
////            targetMethod = current.argument;
////         }
////         else if (targetMethod != current.argument)
////            return false;
////      }
////
////      if (!validateMethodTemplate(current, targetMethod))
////         return false;
////
////      current = current.nextNode();
////   }
////   return true;
////}

//void Compiler :: compileTemplateFieldDeclaration(DNode& member, SyntaxWriter& writer, TemplateScope& scope)
//{
//   while (member != nsNone) {
//      DNode fieldHints = skipHints(member);
//
//      if (member == nsField) {
//         ref_t param = 0;
//         TerminalInfo terminal = member.Terminal();
//
//         scope.info.fields.add(terminal, scope.info.fields.Count());
//
//         writer.newNode(lxTemplateField);
//         writer.appendNode(lxTerminal, terminal);
//
//         if (fieldHints != nsNone) {
//            while (fieldHints == nsHint) {
//               param = scope.parameters.get(fieldHints.Terminal());
//               if (param > 0) {
//                  if (fieldHints.firstChild() != nsNone)
//                     scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, fieldHints.Terminal());
//
//                  writer.appendNode(lxTemplateFieldType, param);
//               }
//               else {
//                  ref_t hintRef = mapHint(fieldHints, *scope.moduleScope, 2000);
//                  // HOTFIX : to support template classes
//                  if (hintRef == 0)
//                     hintRef = mapHint(fieldHints, *scope.moduleScope, 0);
//
//                  if (hintRef != 0) {
//                     if (scope.moduleScope->subjectHints.exist(hintRef)) {
//                        writer.appendNode(lxType, hintRef);
//                     }
//                     else {
//                        writer.newNode(lxTemplate, hintRef);
//                        DNode option = fieldHints.firstChild();
//                        while (option == nsHintValue) {
//                           int param = scope.parameters.get(option.Terminal());
//                           if (param != 0) {
//                              writer.appendNode(lxTemplateParam, param);
//                           }
//                           else scope.raiseError(wrnInvalidHint, fieldHints.Terminal());
//
//                           option = option.nextNode();
//                        }
//                        writer.closeNode();
//                     }                     
//                  }
//                  else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, fieldHints.Terminal());
//               }
//
//               fieldHints = fieldHints.nextNode();
//            }
//         }
//         writer.closeNode();
//      }
//      else {
//         // due to current syntax we need to reset hints back, otherwise they will be skipped
//         if (fieldHints != nsNone)
//            member = fieldHints;
//
//         break;
//      }
//      member = member.nextNode();
//   }
//}

bool Compiler :: copyTemplate(SNode node, Scope& scope, ref_t attrRef, SNode attributeNode)
{
   _Memory* body = scope.moduleScope->loadAttributeInfo(attrRef);
   if (body == NULL)
      return false;

   SNode templNode = node.appendNode(lxTemplate);

   // copy template attributes
   SNode attrValue = attributeNode.firstChild();
   while (attrValue != lxNone) {
      if (attrValue == lxAttributeValue) {
         SNode terminalNode = attrValue.firstChild(lxObjectMask);
         ref_t subject = scope.mapSubject(terminalNode);
         if (subject == 0)
            subject = scope.moduleScope->module->mapSubject(terminalNode.findChild(lxTerminal).identifier(), false);

         templNode.appendNode(lxTemplateParam, subject);
      }

      attrValue = attrValue.nextNode();
   }

   // copy attribute identifier
   SyntaxTree::copyNode(attributeNode.findChild(lxIdentifier), templNode);

   // load template body
   SyntaxTree::loadNode(templNode, body);

   return true;
}

//void Compiler :: importTemplateInfo(SyntaxTree::Node node, ClassScope& scope, ref_t ownerRef, _Module* templateModule, TemplateInfo& source)
//{
//   _Module* module = scope.moduleScope->module;
//
//   TemplateInfo info;
//   info.templateRef = importSubject(templateModule, node.argument, module);
//   info.sourceCol = source.sourceCol;
//   info.sourceRow = source.sourceRow;
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxTemplateSubject) {
//         info.parameters.add(info.parameters.Count() + 1, importTemplateSubject(templateModule, module, current.argument, source));
//      }
//
//      current = current.nextNode();
//   }
//
//   MemoryWriter writer(&scope.imported);
//   info.save(writer);
//}
//
//void Compiler :: compileTemplateDeclaration(DNode node, TemplateScope& scope, DNode hints)
//{
//   SyntaxWriter writer(scope.syntaxTree);
//   writer.newNode(lxRoot, scope.reference);
//
//   DNode member = node.firstChild();
//
//   // load template parameters
//   while (member == nsMethodParameter) {
//      if (!scope.parameters.exist(member.Terminal())) {
//         scope.parameters.add(member.Terminal(), scope.parameters.Count() + 1);
//      }
//      else scope.raiseError(errDuplicatedDefinition, member.Terminal());
//
//      member = member.nextNode();
//   }
//
//   compileTemplateHints(hints, writer, scope);
//
//   if (scope.type == TemplateScope::ttMethod) {
//      scope.parameters.add(TARGET_PSEUDO_VAR, scope.parameters.Count() + 1);
//
//      writer.appendNode(lxTemplateSubject, scope.parameters.Count());
//   }
//   else if (scope.type == TemplateScope::ttField) {
//      scope.info.fields.add(TARGET_PSEUDO_VAR, 0);
//   }
//
//   // load template fields
//   compileTemplateFieldDeclaration(member, writer, scope);
//
//   compileVMT(member, writer, scope);
//
//   // copy template in template
//   MemoryReader reader(&scope.imported);
//   while (!reader.Eof()) {
//      TemplateInfo info;
//      info.load(reader);
//
//      copyTemplateInfo(info, writer);
//   }
//
//   writer.closeNode();
//
//   // save declaration
//   scope.save();
//
//   scope.moduleScope->saveTemplate(scope.templateRef);
//}
//
//bool Compiler :: importTemplateDeclarations(ClassScope& scope, SyntaxWriter& writer)
//{
//   MemoryReader reader(&scope.imported);
//   while (!reader.Eof()) {
//      TemplateInfo info;
//
//      info.load(reader);
//      if (!importTemplateDeclaration(scope, writer, info))
//         return false;
//   }
//
//   return true;
//}

void Compiler :: compileClassDeclaration(SNode node, ClassScope& scope)
{
   SNode baseNode = node.findChild(lxBaseParent);
   if (baseNode!=lxNone) {
      compileParentDeclaration(baseNode, scope);
   }
   else compileParentDeclaration(SNode(), scope);

   int flagCopy = scope.info.header.flags;

   compileClassAttributes(node, scope, node);
   compileFieldDeclarations(node, scope);

   declareVMT(node.firstChild(), scope);

   generateClassDeclaration(node, scope, test(flagCopy, elClosed));

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
}

void Compiler :: generateClassImplementation(SNode node, ClassScope& scope)
{
   WarningScope warningScope(scope.moduleScope->warningMask);

   optimizeClassTree(node, scope, warningScope);

   _writer.generateClass(scope.tape, node);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save();
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.moduleScope->sourcePathRef);
}

//void Compiler :: copyNode(ClassScope& scope, SyntaxTree::Node current, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info)
//{
//   if (current.type == lxTemplateTarget) {
//      int offset = 0;
//      ref_t type = 0;
//      if (info.targetOffset >= 0) {
//         type = info.targetType;
//         offset = info.targetOffset;
//      }
//      else {
//         ident_t field = SyntaxTree::findChild(current, lxTerminal).identifier();
//   
//         offset = scope.info.fields.get(field);
//         type = scope.info.fieldTypes.get(offset);         
//      }
//   
//      // if it is an array
//      if (test(scope.info.header.flags, elDynamicRole)) {
//         writer.newNode(lxThisLocal, 1);
//   
//         writer.appendNode(lxTarget, test(scope.info.header.flags, elStructureRole) ? -3 : -5);
//      }
//      // if it is a structure field
//      else if (test(scope.info.header.flags, elStructureRole)) {
//         writer.newNode(lxBoxing);
//         writer.appendNode(lxFieldAddress, offset);
//      }
//      else writer.newNode(lxField, offset);
//   
//      if (type != 0)
//         writer.appendNode(lxType, type);
//   }
//   else if (current == lxNestedTemplate) {
//      writer.newNode(lxNested, current.argument);
//   
//      TemplateInfo nestedInfo;
//      nestedInfo.templateRef = importSubject(templateModule, 
//         SyntaxTree::findChild(current, lxTemplate).argument, scope.moduleScope->module);
//      nestedInfo.templateParent = importReference(templateModule,
//         SyntaxTree::findChild(current, lxNestedTemplateParent).argument, scope.moduleScope->module);
//   
//      nestedInfo.ownerRef = scope.reference;
//      ref_t classRef = generateTemplate(*scope.moduleScope, nestedInfo, 0);
//      writer.appendNode(lxTarget, classRef);
//   }
//   else if (current == lxNewOp) {
//   }
//   else if (current == lxNestedTemplateOwner) {
//      writer.newNode(lxTarget, info.ownerRef);
//   }
//   else if (current == lxTemplateType) {
//      writer.newNode(lxType, declareInlineTemplate(*scope.moduleScope, current, info, 
//         importSubject(templateModule, current.argument, scope.moduleScope->module)));
//   }
//   else if (/*current == lxTemplate || */current == lxNestedTemplateParent) {
//      // ignore template node, it should be already handled
//      return;
//   }
//   else if (current == lxTerminal) {
//      writer.newNode(lxTerminal, current.identifier());
//   }
//   else if (current == lxSourcePath) {
//      writer.newNode(lxSourcePath, current.identifier());
//   }
//   else if (current == lxMessageVariable) {
//      // message variable should be already set
//      return;
//   }
//   else if (current == lxThisLocal) {
//      writer.newNode(current.type, current.argument);
//      writer.appendNode(lxTarget, scope.reference);
//   }
//   else if (current == lxCol && current.parentNode() != lxBreakpoint) {
//      writer.newNode(lxCol, info.sourceCol);
//   }
//   else if (current == lxRow && current.parentNode() != lxBreakpoint) {
//      writer.newNode(lxRow, info.sourceRow);
//   }
//   else if (test(current.type, lxMessageMask)) {
//      ref_t signature = importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info);
//   
//      writer.newNode(current.type, overwriteSubject(current.argument, signature));
//   
//      // HOTFIX : insert calling target if required
//      if (current.type == lxCalling) {
//         SNode callee = SyntaxTree::findMatchedChild(current, lxObjectMask);
//         if (callee == lxThisLocal) {
//            writer.appendNode(lxCallTarget, scope.reference);
//         }
//         else if (callee == lxField || callee == lxTemplateTarget) {
//            SNode attr = SyntaxTree::findChild(callee, lxNestedTemplateOwner, lxType, lxTemplateFieldType);
//            if (attr == lxNestedTemplateOwner) {
//               writer.appendNode(lxCallTarget, info.ownerRef);
//            }
//            else if (attr == lxType) {
//               ref_t classRef = scope.moduleScope->subjectHints.get(attr.argument);
//               if (classRef)
//                  writer.appendNode(lxCallTarget, classRef);
//            }
//         }
//   
//         // HOTFIX : if it is typecast message, provide the type
//         if (getVerb(current.argument) == GET_MESSAGE_ID && getParamCount(current.argument) == 0 && scope.moduleScope->subjectHints.exist(signature)) {
//            writer.appendNode(lxType, signature);
//         }
//      }
//   }
//   else if (test(current.type, lxReferenceMask)) {
//      if (isPrimitiveRef(current.argument)) {
//         writer.newNode(current.type, current.argument);
//      }
//      else writer.newNode(current.type, importReference(templateModule, current.argument, scope.moduleScope->module));
//   }
//   else if (test(current.type, lxSubjectMask)) {
//      writer.newNode(current.type, importTemplateSubject(templateModule, scope.moduleScope->module, current.argument, info));
//   }
//   else if (test(current.type, lxConstantMask)) {
//      writer.newNode(current.type, importConstant(templateModule, current.argument, scope.moduleScope->module));
//   }
//   else writer.newNode(current.type, current.argument);
//   
//   copyTree(scope, current, writer, templateModule, info);
//   
//   writer.closeNode();
//}
//
//void Compiler :: copyTree(ClassScope& scope, SyntaxTree::Node node, SyntaxWriter& writer, _Module* templateModule, TemplateInfo& info)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      copyNode(scope, current, writer, templateModule, info);
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: importTemplateTree(ClassScope& scope, SyntaxWriter& writer, SNode node, TemplateInfo& info, _Module* templateModule)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxTemplateSubject) {
//         info.parameters.add(info.parameters.Count() + 1, info.messageSubject);
//      }
//      else if (current == lxTemplate) {
//         TemplateInfo extTemplate;
//         importTemplateInfo(current, scope, scope.reference, templateModule, info);
//      }
//      else if (current == lxClassMethod) {
//         ref_t messageRef = overwriteSubject(current.argument, importTemplateSubject(templateModule, scope.moduleScope->module, getSignature(current.argument), info));
//
//         // method should not be imported if it was already declared in the class scope
//         if (!scope.info.methods.exist(messageRef, true)) {
//            writer.newNode(lxClassMethod, messageRef);
//
//            // NOTE : source path reference should be imported
//            // but the message name should be overwritten
//            writeMessage(*scope.moduleScope, writer, messageRef);
//
//            copyTree(scope, current, writer, templateModule, info);
//
//            writer.closeNode();
//
//            scope.include(messageRef);
//         }
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: importTemplateImplementation(ClassScope& scope, SyntaxWriter& writer, TemplateInfo& templateInfo)
//{
//   _Module* extModule = NULL;
//   SyntaxTree tree(scope.moduleScope->loadTemplateInfo(templateInfo.templateRef, extModule));
//
//   SNode root = tree.readRoot();
//   importTemplateTree(scope, writer, root, templateInfo, extModule);
//}

//void Compiler :: compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument)
//{
//   int paramCount = getParamCount(scope.message);
//
//   writer.newNode(lxClassMethod, scope.message);
//   writer.newNode(lxDispatching);
//   writer.newNode(lxResending, scope.message);
//
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//   if (test(classScope->info.header.flags, elStructureWrapper)) {
//      // new stack frame
//      // stack already contains current $self reference
//      writer.newNode(lxNewFrame);
//      writer.newNode(lxBoxing);
//      writer.appendNode(lxTarget, scope.moduleScope->subjectHints.get(classScope->info.fieldTypes.get(0)));
//      writer.appendNode(lxFieldAddress);
//      writer.closeNode();
//      writer.closeNode();
//   }
//   else {
//      writer.newNode(lxExpression);
//      writer.appendNode(lxResultField);
//      writer.closeNode();
//   }  
//
//   writer.closeNode();
//   writer.closeNode();
//   writer.closeNode();
//}
//
//void Compiler :: declareVirtualMethods(ClassScope& scope)
//{
//   ModuleScope* moduleScope = scope.moduleScope;
//
//   // auto generate get&type message if required
//   ClassMap::Iterator c_it = moduleScope->typifiedClasses.getIt(scope.reference);
//   while (!c_it.Eof()) {
//      if (c_it.key() == scope.reference) {
//         MethodScope methodScope(&scope);
//         methodScope.message = encodeMessage(*c_it, GET_MESSAGE_ID, 0);
//
//         // skip if there is an explicit method
//         if (!scope.info.methods.exist(methodScope.message, true)) {
//            scope.info.methods.add(methodScope.message, false);
//         }
//      }
//      c_it++;
//   }
//}

//void Compiler :: importTemplateImplementations(ClassScope& scope, SyntaxWriter& writer)
//{
//   MemoryReader reader(&scope.imported);
//   while (!reader.Eof()) {
//      TemplateInfo info;
//
//      info.load(reader);
//      importTemplateImplementation(scope, writer, info);
//   }
//}

void Compiler :: compileClassImplementation(SNode node, ClassScope& scope)
{
   if (test(scope.info.header.flags, elExtension)) {
      scope.extensionMode = scope.info.fieldTypes.get(-1).value2;
      if (scope.extensionMode == 0)
         scope.extensionMode = -1;
   }

   compileVMT(node, scope);

//   // import templates
//   importTemplateImplementations(scope, writer);

   generateClassImplementation(node, scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (scope.extensionMode == 0)
      compileSymbolCode(scope);
}

void Compiler :: declareSingletonClass(SNode node, ClassScope& scope)
{
   // inherit parent
   compileParentDeclaration(node, scope);

   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot, scope.reference);
   SyntaxTree::copyNode(writer, node);
   writer.closeNode();

   //compileSingletonHints(hints, writer, scope);

   declareVMT(syntaxTree.readRoot().firstChild(), scope);

   //// declare imported methods / flags
   //if (!importTemplateDeclarations(scope, writer))
   //   scope.raiseError(errInvalidHint, node.FirstTerminal());   

   generateClassDeclaration(syntaxTree.readRoot(), scope, test(scope.info.header.flags, elClosed));

   scope.save();
}

void Compiler :: declareSingletonAction(ClassScope& classScope, SNode objNode)
{
//   if (hints != nsNone)
//      classScope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, hints.Terminal());
//
   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot, classScope.reference);

   if (objNode != nsNone) {
      ActionScope methodScope(&classScope);
      declareActionScope(objNode, classScope, objNode.findChild(lxIdentifier, lxPrivate, lxMethodParameter, lxMessage), methodScope, 0, false);
      writer.newNode(lxClassMethod, methodScope.message);

      writer.closeNode();
   }

   writer.closeNode();

   generateClassDeclaration(syntaxTree.readRoot(), classScope, test(classScope.info.header.flags, elClosed));

   classScope.save();
}

void Compiler :: compileSingletonClass(SNode node, ClassScope& scope)
{
   SyntaxTree syntaxTree;
   SyntaxWriter writer(syntaxTree);
   writer.newNode(lxRoot, scope.reference);
   SyntaxTree::copyNode(writer, node);
   writer.closeNode();

   //compileSingletonHints(hints, writer, scope);

   //// import templates
   //importTemplateImplementations(scope, writer);

   declareVMT(syntaxTree.readRoot().firstChild(), scope);
   compileVMT(syntaxTree.readRoot(), scope);

   generateClassImplementation(syntaxTree.readRoot(), scope);
}

void Compiler :: compileSymbolDeclaration(SNode node, SymbolScope& scope)
{
   bool singleton = false;

   SNode expression = node.findChild(lxExpression);

   // if it is a singleton
   if (isSingleStatement(expression)) {
      SNode objNode = expression.findChild(lxCode, lxNestedClass, lxInlineExpression, lxExpression);
      // HOTFIX : ignore sub expession
      if (objNode == lxExpression && isSingleStatement(objNode))
         objNode = objNode.findChild(lxCode, lxNestedClass, lxInlineExpression);

      if (objNode == lxNestedClass) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         classScope.info.header.flags |= elNestedClass;

         declareSingletonClass(objNode, classScope);
         singleton = true;
      }
      else if (objNode == lxCode) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         classScope.info.header.flags |= elNestedClass;

         declareSingletonAction(classScope, objNode);
         singleton = true;
      }
      else if (objNode == lxInlineExpression) {
         ClassScope classScope(scope.moduleScope, scope.reference);
         classScope.info.header.flags |= elNestedClass;

         declareSingletonAction(classScope, objNode);
         singleton = true;
      }
//      else if (objNode == nsSubjectArg || objNode == nsMethodParameter) {
//         ClassScope classScope(scope.moduleScope, scope.reference);
      // classScope.info.header.flags |= elNestedClass;
//
//         declareSingletonAction(classScope, objNode, objNode, hints);
//         singleton = true;
//      }
//      else compileSymbolHints(hints, scope, false);
   }
   compileSymbolAttributes(node, scope);

   SNode constNode = node.findChild(lxConstAttr);
   SNode typeNode = node.findChild(lxType);

   if (!singleton && (typeNode.argument != 0 || constNode != lxNone)) {
      SymbolExpressionInfo info;
      info.expressionTypeRef = typeNode.argument;
      info.constant = constNode != lxNone;

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

      long value = module->resolveConstant(retVal.param).toULongLong(10, 1);

      dataWriter.write(&value, 8);

      dataWriter.Memory()->addReference(scope.moduleScope->longReference | mskVMTRef, (ref_t)-4);

      scope.moduleScope->defineConstantSymbol(scope.reference, scope.moduleScope->longReference);
   }
   else if (retVal.kind == okRealConstant) {
      _Module* module = scope.moduleScope->module;
      MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

      double value = module->resolveConstant(retVal.param).toDouble();

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
   else if (retVal.kind == okObject) {
      SNode root = node.findSubNodeMask(lxObjectMask);

      if (root == lxConstantList) {
         SymbolExpressionInfo info;
         info.expressionTypeRef = scope.typeRef;
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

void Compiler :: compileSymbolImplementation(SNode node, SymbolScope& scope)
{
   bool isStatic = (node == lxStatic);

   ObjectInfo retVal;
   SNode expression = node.findChild(lxExpression);

   // if it is a singleton
   if (isSingleStatement(expression)) {
      SNode classNode = expression.findChild(lxCode, lxNestedClass, lxInlineExpression, lxExpression);
      // HOTFIX : ignore sub expession
      if (classNode == lxExpression && isSingleStatement(classNode))
         classNode = classNode.findChild(lxCode, lxNestedClass, lxInlineExpression);

      if (classNode == lxNestedClass) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileSingletonClass(classNode, classScope);

         if (test(classScope.info.header.flags, elStateless)) {
            // if it is a stateless singleton
            retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
         }
      }
      else if (classNode == lxCode) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(classNode, classScope, SNode(), 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
      else if (classNode == lxInlineExpression) {
         ModuleScope* moduleScope = scope.moduleScope;

         ClassScope classScope(moduleScope, scope.reference);
         moduleScope->loadClassInfo(classScope.info, moduleScope->module->resolveReference(scope.reference), false);

         compileAction(classNode, classScope, classNode.findChild(lxIdentifier, lxPrivate, lxMethodParameter, lxMessage), 0, true);

         retVal = ObjectInfo(okConstantSymbol, scope.reference, scope.reference);
      }
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

   CodeScope codeScope(&scope);
   if (retVal.kind == okUnknown) {
      scope.constant = node.existChild(lxConstAttr);
      scope.typeRef = node.findChild(lxType).argument;

      // compile symbol body, if it is not a singleton
      insertDebugStep(expression, dsStep);
      retVal = compileExpression(expression, codeScope, 0);

      if (scope.typeRef != 0) {
         typecastObject(expression, codeScope, scope.typeRef, retVal);
      }
   }
   else {
      SNode terminal = node.firstChild(lxTerminalMask);

      setTerminal(terminal, codeScope, retVal, 0);
   }

   optimizeSymbolTree(node, scope, scope.moduleScope->warningMask);

   // create constant if required
   if (scope.constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression);

      if (!compileSymbolConstant(node, scope, retVal))
         scope.raiseError(errInvalidOperation, expression);
   }

//   if (scope.preloaded) {
//      compilePreloadedCode(scope);
//   }

   _writer.generateSymbol(scope.tape, node, isStatic);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.save(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule, 
      scope.moduleScope->sourcePathRef);
}

// NOTE : targetType is used for binary arrays
ObjectInfo Compiler :: assignResult(CodeScope& scope, SNode& node, ref_t targetRef, ref_t targetType)
{
   ObjectInfo retVal(okObject, targetRef, 0, targetType);

   size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, targetType);
   if (size != 0) {
      if (allocateStructure(scope, size, false, retVal)) {
         retVal.extraparam = targetRef;

         SNode assignNode = node.injectNode(lxAssigning, size);
         assignNode.insertNode(lxLocalAddress, retVal.param);
      }      

      if (node != lxExpression) {
         SNode boxingNode = node.injectNode(lxBoxing, size);
         boxingNode.appendNode(lxTarget, targetRef);
      }
      else {
         node.set(lxBoxing, size);
         node.appendNode(lxTarget, targetRef);
      }

      return retVal;
   }
   else return retVal;
}

void Compiler :: optimizeExtCall(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
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

   optimizeSyntaxExpression(scope, node, warningScope, HINT_NOBOXING /* | HINT_EXTERNALOP*/);
}

void Compiler :: optimizeInternalCall(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
{
   //SNode parentNode = node.parentNode();
   //while (parentNode == lxExpression)
   //   parentNode = parentNode.parentNode();

   //if (parentNode == lxAssigning) {
   //   boxPrimitive(scope, node, -1, warningMask, mode);
   //}

   optimizeSyntaxExpression(scope, node, warningScope, HINT_NOBOXING);
}

//void Compiler :: optimizeDirectCall(ModuleScope& scope, SNode node, int warningMask)
//{
//   int mode = 0;
//
//   bool stackSafe = SyntaxTree::existChild(node, lxStacksafe);
//
//   if (node == lxDirectCalling && SyntaxTree::existChild(node, lxEmbeddable)) {
//      // check if it is a virtual call
//      if (getVerb(node.argument) == GET_MESSAGE_ID && getParamCount(node.argument) == 0) {
//         SNode callTarget = SyntaxTree::findChild(node, lxCallTarget);
//
//         ClassInfo info;
//         scope.loadClassInfo(info, callTarget.argument);
//         if (info.methodHints.get(Attribute(node.argument, maEmbeddableIdle)) == -1) {
//            // if it is an idle call, remove it
//            node = lxExpression;
//
//            optimizeSyntaxExpression(scope, node, warningMask, mode);
//         }
//      }
//   }
//
//   if (stackSafe)
//      mode |= HINT_NOBOXING;
//
//   optimizeSyntaxExpression(scope, node, warningMask, mode);
//}

void Compiler :: optimizeCall(ModuleScope& scope, SNode node, WarningScope& warningScope)
{
   int mode = 0;

//   bool stackSafe = false;
//   bool methodNotFound = false;
   SNode target = node.findChild(lxCallTarget);
   if (target.argument != 0) {
      bool dummy1 = false;
      ref_t dummy2 = 0;
      int hint = _logic->checkMethod(scope, target.argument, node.argument, dummy1, dummy2);

      if (test(hint, tpStackSafe))
         mode |= HINT_NOBOXING;

      if (test(hint, tpEmbeddable))
         node.appendNode(lxEmbeddable);

//      ClassInfo info;
//      if (scope.loadClassInfo(info, target.argument)) {
//         ref_t resultType;
//         int hint = scope.checkMethod(info, node.argument, resultType);
//         
//         if (hint == tpUnknown) {
//            // Compiler magic : allow to call wrapper content directly
//            if (test(info.header.flags, elWrapper)) {
//               target.setArgument(scope.subjectHints.get(info.fieldTypes.get(0)));
//
//               hint = scope.checkMethod(target.argument, node.argument);
//
//               // for dynamic object, target object should be overridden
//               if (!test(info.header.flags, elStructureRole)) {
//                  node.appendNode(lxOverridden);
//                  SNode n = SyntaxTree::findChild(node, lxOverridden);
//                  n.appendNode(lxCurrentField);
//               }
//               else {
//                  node.appendNode(lxOverridden);
//                  SNode n = SyntaxTree::findChild(node, lxOverridden);
//                  n.appendNode(lxBoxing);
//                  n = SyntaxTree::findChild(n, lxBoxing);
//                  n.appendNode(lxCurrent);
//                  n.appendNode(lxTarget, target.argument);
//               }
//            }            
//         }
//
//         methodNotFound = hint == tpUnknown;
//         switch (hint & tpMask) {
//            case tpSealed:
//               stackSafe = test(hint, tpStackSafe);
//               node = lxDirectCalling;
//               if (resultType != 0)
//                  node.appendNode(lxType, resultType);
//               break;
//            case tpClosed:
//               stackSafe = test(hint, tpStackSafe);
//               node = lxSDirctCalling;
//               if (resultType != 0)
//                  node.appendNode(lxType, resultType);
//               break;
//         }
//      }
   }

   optimizeSyntaxExpression(scope, node, warningScope, mode);

   if (node.existChild(lxTypecastAttr)) {
      warningScope.raise(scope, WARNING_LEVEL_2, wrnTypeMismatch, node.firstChild(lxObjectMask));
   }
   else if (node.existChild(lxNotFoundAttr)) {
      warningScope.raise(scope, WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxBreakpoint));
   }
}

//int Compiler :: mapOpArg(Compiler::ModuleScope& scope, SNode arg, ref_t& target)
//{
//   target = SyntaxTree::findChild(arg, lxTarget).argument;
//
//   int flags = mapOpArg(scope, arg);
//
//   // HOTFIX : if the target is not defined - consider it as primitive one
//   if (target == 0) {
//      switch (flags) {
//         case elDebugDWORD:
//            target = -1;
//            break;
//         case elDebugQWORD:
//            target = -2;
//            break;
//         case elDebugReal64:
//            target = -4;
//            break;
//         default:
//            break;
//      }
//   }
//
//   // HOTFIX : check the type as well
//   if (target == 0) {
//      ref_t type = SyntaxTree::findChild(arg, lxType).argument;
//      if (type != 0)
//         target = scope.subjectHints.get(type);
//   }
//
//   return flags;
//}
//
//int Compiler :: mapOpArg(ModuleScope& scope, SNode arg)
//{
//   int flags = 0;
//
//   ref_t ref = SyntaxTree::findChild(arg, lxTarget).argument;
//   if (ref == 0) {
//      ref_t type = SyntaxTree::findChild(arg, lxType).argument;
//      if (type != 0)
//         ref = scope.subjectHints.get(type);
//   }
//
//   if (isPrimitiveRef(ref) || ref == 0) {
//      switch (ref) {
//         case -1:
//            return elDebugDWORD;
//         case -2:
//            return elDebugQWORD;
//         case -4:
//            return elDebugReal64;
//         case -8:
//            return elDebugSubject;
//         default:
//            return 0;
//      }
//   }
//   else {
//      flags = scope.getClassFlags(ref);
//
//      return flags & elDebugMask;
//   }   
//}
//
//inline LexicalType mapArrPrimitiveOp(int size)
//{
//   switch (size)
//   {
//      case 4:
//         return lxIntArrOp;
//      case 1:
//         return lxByteArrOp;
//      case 2:
//         return lxShortArrOp;
//      default:
//         return lxBinArrOp;
//   }
//}
//
//void Compiler :: optimizeBoolOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
//{
//   SNode typecastNode = SyntaxTree::findChild(node, lxTypecasting);
//   SNode opNode = SyntaxTree::findChild(typecastNode, lxOp);
//
//   if (opNode == lxOp && optimizeOp(scope, opNode, warningLevel, mode)) {
//      typecastNode = lxExpression;
//      node = lxExpression;
//      // HOTFIX : mark it as a boolean operation
//      node.appendNode(lxType, scope.boolType);
//   }
//   else optimizeSyntaxExpression(scope, node, warningLevel);
//}

void Compiler :: optimizeOp(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
{
//   ref_t destType = 0;
//   SNode parent = node.parentNode();
//   while (parent != lxNewFrame) {
//      if (parent == lxTypecasting) {
//         destType = getSignature(parent.argument);
//         break;
//      }
//      else parent = parent.parentNode();;
//   }
//
//   if (node.argument == SET_REFER_MESSAGE_ID) {
//      SNode larg, narg, rarg;
//      assignOpArguments(node, larg, narg, rarg);
//
//      ref_t lref = SyntaxTree::findChild(larg, lxTarget).argument;
//      int nflags = mapOpArg(scope, narg);
//
//      if (isPrimitiveRef(lref)) {
//         if (lref == -3 && nflags == elDebugDWORD) {
//            destType = SyntaxTree::findChild(larg, lxType).argument;
//            if (checkIfCompatible(scope, destType, rarg)) {
//               int size = scope.defineSubjectSize(destType);
//               node.appendNode(lxSize, size);
//               node = mapArrPrimitiveOp(size);
//            }
//         }
//         else if (lref == -5 && nflags == elDebugDWORD) {
//            destType = SyntaxTree::findChild(larg, lxType).argument;
//            if (checkIfCompatible(scope, destType, rarg)) {
//               node = lxArrOp;
//            }
//         }
//      }
//
//      if (node == lxOp) {
//         node.setArgument(encodeMessage(0, node.argument, 2));
//         node = lxCalling;
//
//         optimizeCall(scope, node, warningLevel);
//
//         return false;
//      }
//      else {
//         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//         optimizeSyntaxNode(scope, narg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//
//         return true;
//      }
//   }
//   else {
//      bool boxing = false;
      SNode larg, rarg, rarg2;
      larg = node.firstChild(lxObjectMask);
      optimizeSyntaxNode(scope, larg, warningScope, HINT_NOBOXING);

      rarg = larg.nextNode(lxObjectMask);
      optimizeSyntaxNode(scope, rarg, warningScope, HINT_NOBOXING);

      rarg2 = rarg.nextNode(lxObjectMask);
      if (rarg2 != lxNone)
         optimizeSyntaxNode(scope, rarg2, warningScope, HINT_NOBOXING);

//      ref_t target = 0;
//      int lflags = mapOpArg(scope, larg, target);
//      int rflags = mapOpArg(scope, rarg);
//
//      if (IsNumericOperator(node.argument)) {
//         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
//            target = -1;
//            node = lxIntOp;
//            boxing = true;
//         }
//         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
//            target = -2;
//            node = lxLongOp;
//            boxing = true;
//         }
//         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
//            target = -4;
//            node = lxRealOp;
//            boxing = true;
//         }
//      }
//      else if (node.argument == READ_MESSAGE_ID) {
//         if (target == -3 && rflags == elDebugDWORD) {
//            target = scope.subjectHints.get(SyntaxTree::findChild(larg, lxType).argument);
//            int size = scope.defineStructSize(target);
//            node.appendNode(lxSize, size);
//            node = mapArrPrimitiveOp(size);
//         }
//         else if (target == -5 && rflags == elDebugDWORD) {
//            node = lxArrOp;
//         }
//         else if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
//            target = -1;
//            node = lxIntOp;
//            boxing = true;
//         }
//         else if (lflags == elDebugQWORD && rflags == elDebugDWORD) {
//            target = -2;
//            node = lxLongOp;
//            boxing = true;
//         }
//      }
//      else if (node.argument == WRITE_MESSAGE_ID) {
//         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
//            target = -1;
//            node = lxIntOp;
//            boxing = true;
//         }
//         else if (lflags == elDebugQWORD && rflags == elDebugDWORD) {
//            target = -2;
//            node = lxLongOp;
//            boxing = true;
//         }
//      }
//      else if (IsBitwiseOperator(node.argument)) {
//         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
//            target = -1;
//            node = lxIntOp;
//            boxing = true;
//         }
//         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
//            target = -2;
//            node = lxLongOp;
//            boxing = true;
//         }
//      }
//      else if (IsCompOperator(node.argument)) {
//         if (lflags == elDebugDWORD && (rflags == elDebugDWORD || rflags == elDebugPTR)) {
//            node = lxIntOp;
//         }
//         else if (lflags == elDebugPTR && rflags == elDebugPTR) {
//            node = lxIntOp;
//         }
//         else if (lflags == elDebugSubject && rflags == elDebugSubject) {
//            node = lxIntOp;
//         }
//         else if (lflags == elDebugQWORD && (rflags == elDebugQWORD || rflags == elDebugDPTR)) {
//            node = lxLongOp;
//         }
//         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
//            node = lxRealOp;
//         }
//
//         node.appendNode(lxType, scope.boolType);
//      }
//      else if (IsVarOperator(node.argument)) {
//         if (lflags == elDebugDWORD && rflags == elDebugDWORD) {
//            target = -1;
//            node = lxIntOp;
//         }
//         else if (lflags == elDebugQWORD && rflags == elDebugQWORD) {
//            target = -2;
//            node = lxLongOp;
//         }
//         else if (lflags == elDebugReal64 && rflags == elDebugReal64) {
//            target = -4;
//            node = lxRealOp;
//         }
//      }
//      else if (IsReferOperator(node.argument) && rflags == elDebugDWORD) {         
//         destType = SyntaxTree::findChild(larg, lxType).argument;
//
//         if (target == -3 && destType != 0) {
//            target = scope.subjectHints.get(destType);
//            int size = scope.defineStructSize(target);
//            node.appendNode(lxSize, size);
//            node = mapArrPrimitiveOp(size);
//            boxing = true;
//         }
//         else if (target == -5 || target == scope.paramsReference) {
//            node = lxArrOp;
//         }
//      }
//
//      if (node == lxOp) {
//         node.setArgument(encodeMessage(0, node.argument, 1));
//         node = lxCalling;
//
//         if (target != 0)
//            node.appendNode(lxCallTarget, target);
//
//         optimizeCall(scope, node, warningLevel);
//
//         return false;
//      }
//      else {
//         optimizeSyntaxNode(scope, larg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//
//         // HOTFIX : if larg is boxing, the second operator should be reassigned
//         if (larg == lxBoxing) {
//            rarg = SyntaxTree::findSecondMatchedChild(node, lxObjectMask);
//         }
//         optimizeSyntaxNode(scope, rarg, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//
//         if (boxing) {
//            if (isPrimitiveRef(target)) {
//               if (destType != 0) {
//                  //if destination type is known try to check the compatibility
//                  int flags = scope.getTypeFlags(destType);
//                  if (test(flags, elWrapper)) {
//                     ClassInfo destInfo;
//                     scope.loadClassInfo(destInfo, scope.subjectHints.get(destType));
//                     destType = destInfo.fieldTypes.get(0);
//
//                     flags = scope.getTypeFlags(destType);
//                  }
//
//                  flags &= elDebugMask;
//
//                  if (flags == elDebugDWORD && target == -1) {
//                     target = scope.subjectHints.get(destType);
//                  }
//                  else if (flags == elDebugQWORD && target == -2) {
//                     target = scope.subjectHints.get(destType);
//                  }
//                  else if (flags == elDebugReal64 && target == -4) {
//                     target = scope.subjectHints.get(destType);
//                  }
//               }
//
//               switch (target) {
//                  case -1:
//                     target = scope.intReference;
//                     break;
//                  case -2:
//                     target = scope.longReference;
//                     break;
//                  case -4:
//                     target = scope.realReference;
//                     break;
//                  default:
//                     break;
//               }
//            }
//
//            boxPrimitive(scope, node, target, warningLevel, mode);
//         }            
//
//         return true;
//      }
//   }
}

//void Compiler :: optimizeNewOp(ModuleScope& scope, SNode node, int warningLevel, int mode)
//{
//   optimizeSyntaxExpression(scope, node, warningLevel, HINT_NOBOXING | HINT_NOUNBOXING);
//
//   SNode expr = SyntaxTree::findMatchedChild(node, lxObjectMask);
//   if (expr == lxExpression)
//      expr = SyntaxTree::findMatchedChild(expr, lxObjectMask);
//
//   ref_t type = SyntaxTree::findChild(expr, lxType).argument;
//   ref_t classRef = SyntaxTree::findChild(expr, lxTarget).argument;
//   if (classRef == 0 && type != 0)
//      classRef = scope.subjectHints.get(type);
//
//   if (classRef != -1 && (scope.getClassFlags(classRef) & elDebugMask) != elDebugDWORD) {
//      SNode row = SyntaxTree::findChild(node, lxRow);
//      SNode col = SyntaxTree::findChild(node, lxCol);
//      SNode terminal = SyntaxTree::findChild(node, lxTerminal);
//
//      scope.raiseError(errInvalidOperation, row.argument, col.argument, terminal.identifier());
//   }
//}
//
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
//         assignTarget = lxIdle;
//         callNode.setArgument(encodeMessage(subject, EVAL_MESSAGE_ID, 1));
//      }
//   }
//}

void Compiler :: optimizeAssigning(ModuleScope& scope, SNode node, WarningScope& warningScope)
{
   int mode = HINT_NOUNBOXING | HINT_ASSIGNING;
   if (node.argument != 0)
      mode |= HINT_NOBOXING;

   optimizeSyntaxExpression(scope, node, warningScope, mode);

   if (node.argument != 0) {
      SNode intValue = node.findSubNode(lxConstantInt);
      if (intValue != lxNone) {
         // direct operation with numeric constants
         node.set(lxIntOp, SET_MESSAGE_ID);
      }
      else {
         SNode callNode = node.findSubNode(lxDirectCalling, lxSDirctCalling);
         if (callNode != lxNone && callNode.existChild(lxEmbeddable)) {
            _logic->optimizeEmbeddableGet(scope, *this, node);
         }
      }
   }

//   // assignment operation
//   SNode assignNode = node.findSubNode(lxAssigning);
//   if (assignNode != lxNone) {
//      SNode operationNode = assignNode.findChild(lxIntOp/*, lxRealOp*/);
//      if (operationNode != lxNone) {
//         SNode larg = operationNode.findSubNodeMask(lxObjectMask);
//         SNode target = node.firstChild(lxObjectMask);
//         if (larg.type == target.type && larg.argument == target.argument) {
//            // remove an extra assignment
//            larg = assignNode.findSubNodeMask(lxObjectMask);
//
////            larg = target.type;
////            larg.setArgument(target.argument);
////            node = lxExpression;
////            target = lxIdle;
//         }
//      }
//   }
}

void Compiler :: optimizeArgUnboxing(ModuleScope& scope, SNode node, WarningScope& warningScope)
{
   SNode object = node.firstChild(lxObjectMask);
   if (object == lxArgBoxing)
      object = lxExpression;

   optimizeSyntaxExpression(scope, node, warningScope);
}

void Compiler :: optimizeBoxing(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
{
   SNode target = node.findChild(lxTarget);

   optimizeSyntaxExpression(scope, node, warningScope, HINT_NOBOXING);

   SNode exprNode = node.findSubNodeMask(lxObjectMask);
   if (exprNode == lxNewOp) {
      exprNode.setArgument(target.argument);

      node = lxExpression;
   }
   else {
      bool boxing = !test(mode, HINT_NOBOXING);
      // HOTFIX : do not box constant classes
      if (exprNode == lxConstantInt && target.argument == scope.intReference) {
         boxing = false;
      }
      else if (exprNode == lxConstantSymbol && target.argument == scope.intReference) {
         boxing = false;
      }

      // if no boxing hint provided
      // then boxing should be skipped
      if (!boxing) {
         _logic->optimizeEmbeddableBoxing(scope, *this, node, target.argument, test(mode, HINT_ASSIGNING));
      }
      else {
         if (node == lxUnboxing && test(mode, HINT_NOUNBOXING)) {
            node = lxBoxing;
         }
         else if (test(mode, HINT_NOCONDBOXING) && node == lxCondBoxing) {
            node = lxBoxing;
         }

         // HOTFIX : replace virtual class with generic one
         if (_logic->isPrimitiveRef(target.argument))
            target.setArgument(_logic->resolvePrimitiveReference(scope, target.argument));

         warningScope.raise(scope, WARNING_LEVEL_3, wrnBoxingCheck, node);
      }
   }
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

void Compiler :: optimizeNestedExpression(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
{
//   if (current == lxNested) {
//      // check if the nested collection can be treated like constant one
//      bool constant = true;
//      SNode member = current.firstChild();
//      while (constant && member != lxNone) {
//         if (member == lxMember) {
//            SNode object = findSubNodeMask(member, lxObjectMask);
//            switch (object.type) {
//               case lxConstantChar:
//               case lxConstantClass:
//               case lxConstantInt:
//               case lxConstantLong:
//               case lxConstantList:
//               case lxConstantReal:
//               case lxConstantString:
//               case lxConstantWideStr:
//               case lxConstantSymbol:
//                  break;
//               default:
//                  constant = false;
//                  break;
//            }
//         }
//         member = member.nextNode();
//      }
//
//      // replace with constant array if possible
//      if (constant) {
//         ref_t reference = scope.mapNestedExpression();
//
//         current = lxConstantList;
//         current.setArgument(reference | mskConstArray);
//
//         _writer.generateConstantList(current, scope.module, reference);
//      }
//   }

   optimizeSyntaxExpression(scope, node, warningScope);
}

void Compiler :: optimizeSyntaxNode(ModuleScope& scope, SNode current, WarningScope& warningScope, int mode)
{
   switch (current.type) {
      case lxAssigning:
         optimizeAssigning(scope, current, warningScope);
         break;
//      case lxTypecasting:
//         optimizeTypecast(scope, current, warningMask, mode);
//         break;
      case lxStdExternalCall:
      case lxExternalCall:
      case lxCoreAPICall:
         optimizeExtCall(scope, current, warningScope, mode);
         break;
      case lxInternalCall:
         optimizeInternalCall(scope, current, warningScope, mode);
         break;
      case lxExpression:
      case lxOverridden:
//      case lxVariable:
         // HOT FIX : to pass the optimization mode into sub expression
         optimizeSyntaxExpression(scope, current, warningScope, mode);
         break;
      case lxReturning:
         optimizeSyntaxExpression(scope, current, warningScope, HINT_NOUNBOXING | HINT_NOCONDBOXING);
         break;
      case lxBoxing:
      case lxCondBoxing:
      case lxUnboxing:
      case lxArgBoxing:
         optimizeBoxing(scope, current, warningScope, mode);
         break;
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
      case lxIntArrOp:
      case lxShortArrOp:
      case lxByteArrOp:
      case lxArrOp:
      case lxBinArrOp:
      case lxNewOp:
      case lxNilOp:
         optimizeOp(scope, current, warningScope, mode);
         break;
//      case lxBoolOp:
//         optimizeBoolOp(scope, current, warningMask, mode);
//         break;
      case lxDirectCalling:
      case lxSDirctCalling:
      case lxCalling:
         optimizeCall(scope, current, warningScope);
         break;
      case lxNested:
      case lxMember:
         optimizeNestedExpression(scope, current, warningScope, mode);
         break; 
      case lxCode:
         optimizeSyntaxExpression(scope, current, warningScope/*, HINT_NOBOXING*/);
         break;
      case lxArgUnboxing:
         optimizeArgUnboxing(scope, current, warningScope);
         break;
      default:
         if (test(current.type, lxCodeScopeMask)) {
            optimizeSyntaxExpression(scope, current, warningScope);
         }
         break;
   }
}

void Compiler :: optimizeSyntaxExpression(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      optimizeSyntaxNode(scope, current, warningScope, mode);

      current = current.nextNode();
   }
}

void Compiler :: optimizeClassTree(SNode node, ClassScope& scope, WarningScope& warningScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         SNode mask = current.findChild(lxWarningMask);
         if (mask != lxNone)
            warningScope.warningMask = mask.argument;

         optimizeSyntaxExpression(*scope.moduleScope, current, warningScope);

         // HOTFIX : analize nested template methods
         optimizeClassTree(current, scope, warningScope);

         if (test(_optFlag, 1)) {
            if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
               defineEmbeddableAttributes(scope, current);
            }
         }
      }
      else if (current == lxIdle) {         
         // HOTFIX : analize nested template methods
         optimizeClassTree(current, scope, warningScope);
      }
      else if (current == lxTemplate) {
         WarningScope templateWarningScope;
         templateWarningScope.warningMask = warningScope.warningMask;
         templateWarningScope.col = current.findChild(lxCol).argument;
         templateWarningScope.row = current.findChild(lxRow).argument;

         // HOTFIX : analize nested template methods
         optimizeClassTree(current, scope, templateWarningScope);
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
         optimizeSyntaxExpression(*scope.moduleScope, current, warningScope);
      }

      current = current.nextNode();
   }
}

void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
{
   // Optimization : var = get&subject => eval&subject2:var[1]
   ref_t type = 0;
   ref_t returnType = classScope.info.methodHints.get(ClassInfo::Attribute(methodNode.argument, maType));
   if (_logic->recognizeEmbeddableGet(*classScope.moduleScope, methodNode, returnType, type)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGet), type);

      // HOTFIX : allowing to recognize embeddable get in the class itself
      classScope.save();
   }

   // Optimization : subject'get = self
   if (_logic->recognizeEmbeddableIdle(methodNode)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableIdle), -1);
   }
}

void Compiler :: compileIncludeModule(SNode ns, ModuleScope& scope)
{
   ident_t name = ns.findChild(lxIdentifier, lxReference).findChild(lxTerminal).identifier();

   // check if the module exists
   _Module* module = scope.project->loadModule(name, true);
   if (module) {
      ident_t value = retrieve(scope.defaultNs.start(), name, NULL);
      if (value == NULL) {
         scope.defaultNs.add(module->Name());

         scope.loadModuleInfo(module);
      }
   }
   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, ns);
}

void Compiler :: declareSubject(SNode member, ModuleScope& scope)
{
   SNode name = member.findChild(lxIdentifier, lxPrivate);

   bool internalSubject = name == lxPrivate;

   // map a full type name
   ref_t subjRef = scope.mapNewAttribute(name.findChild(lxTerminal).identifier());
   ref_t classRef = 0;

//   while (hints == nsHint) {
//      TerminalInfo terminal = hints.Terminal();
//
//      scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownHint, terminal);
//
//      hints = hints.nextNode();
//   }
//
   SNode classNode = member.findChild(lxForward);
   if (classNode != lxNone) {
      SNode terminal = classNode.findChild(lxPrivate, lxIdentifier, lxReference);

      SNode option = classNode.findChild(lxAttributeValue);
      if (option != nsNone) {
         ref_t attrRef = mapAttribute(classNode, scope);
         if (!attrRef)
            scope.raiseError(errInvalidHint, terminal);

         TemplateScope templateScope(&scope, attrRef);
         templateScope.loadParameters(classNode);

         templateScope.generateClassName(true);

         classRef = templateScope.reference;
      }
      else {
         classRef = scope.mapTerminal(terminal);
         if (classRef == 0)
            scope.raiseError(errUnknownClass, terminal);
      }
   }

   scope.saveAttribute(subjRef, classRef, internalSubject);
}

void Compiler :: compileSubject(SNode member, ModuleScope& scope)
{
   SNode classNode = member.findChild(lxForward);
   if (classNode != lxNone) {
      SNode terminal = classNode.findChild(lxPrivate, lxIdentifier, lxReference);

      SNode option = classNode.findChild(lxAttributeValue);
      if (option != nsNone) {
         ref_t attrRef = mapAttribute(classNode, scope);
         if (!attrRef)
            scope.raiseError(errInvalidHint, terminal);

         TemplateScope templateScope(&scope, attrRef);
         templateScope.loadParameters(classNode);

         templateScope.generateClassName();

         ref_t classRef = generateTemplate(member, templateScope);
         if (classRef == 0)
            scope.raiseError(errInvalidHint, member);
      }
   }
}

void Compiler :: compileDeclarations(SNode current, ModuleScope& scope)
{
   while (current != lxNone) {
      SNode name = current.findChild(lxIdentifier, lxPrivate);

      switch (current) {
         case lxSubject:
            declareSubject(current, scope);
            break;
         case lxClass:
         {
            current.setArgument(scope.mapTerminal(name));

            // check for duplicate declaration
            if (scope.module->mapSection(current.argument | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(current.argument | mskSymbolRef, false);

            // compile class
            ClassScope classScope(&scope, current.argument);
            compileClassDeclaration(current, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef);
               compileClassClassDeclaration(current, classClassScope, classScope);
            }

            break;
         }
         case lxTemplate:
         {
            int count = SyntaxTree::countChild(current, lxMethodParameter);

            IdentifierString templateName(name.findChild(lxTerminal).identifier());
            templateName.append('#');
            templateName.appendInt(count);

            ref_t templateRef = scope.mapNewAttribute(templateName);

            // check for duplicate declaration
            if (scope.module->mapSection(templateRef | mskSyntaxTreeRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            // HOTFIX : save the template source path

            IdentifierString fullPath(scope.module->Name());
            fullPath.append('\'');
            fullPath.append(scope.sourcePath);

            current.appendNode(lxSourcePath, scope.sourcePath);

            SyntaxTree::saveNode(current, scope.module->mapSection(templateRef | mskSyntaxTreeRef, false));

            scope.saveAttribute(templateRef, INVALID_REF, false);

            break;
         }
         case lxSymbol:
         case lxStatic:
         {
            current.setArgument(scope.mapTerminal(name));

            // check for duplicate declaration
            if (scope.module->mapSection(current.argument | mskSymbolRef, true))
               scope.raiseError(errDuplicatedSymbol, name);

            scope.module->mapSection(current.argument | mskSymbolRef, false);

            SymbolScope symbolScope(&scope, current.argument);
            compileSymbolDeclaration(current, symbolScope/*, hints*/);
            break;
         }
      }
      current = current.nextNode();
   }
}

void Compiler :: compileImplementations(SNode member, ModuleScope& scope)
{
   while (member != nsNone) {
      switch (member) {
         case lxSubject:
            compileSubject(member, scope);
            break;
         case lxClass:
         {
            // compile class
            ClassScope classScope(&scope, member.argument);
            scope.loadClassInfo(classScope.info, scope.module->resolveReference(member.argument), false);
            compileClassImplementation(member, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef);
               scope.loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);

               compileClassClassImplementation(member, classClassScope, classScope);
            }
            break;
         }
         case lxSymbol:
         case lxStatic:
         {
            SymbolScope symbolScope(&scope, member.argument);
            compileSymbolImplementation(member, symbolScope/*, hints*/);
            break;
         }
      }
      member = member.nextNode();
   }
}

void Compiler :: compileIncludeSection(SNode& member, ModuleScope& scope)
{
   while (member != lxNone) {
      switch (member) {
         //case nsInclude:
         //   // NOTE: obsolete, used for backward compatibility
         //   //       should be removed in 2.1.x
         //   compileIncludeModule(member, scope, hints);
         //   break;
         case lxImport:
            compileIncludeModule(member, scope/*, hints*/);
            break;
      }
      member = member.nextNode();
   }
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

void Compiler :: compileModule(SNode node, ModuleScope& scope)
{
   compileIncludeSection(node.firstChild(), scope);
   
   if (scope.superReference == 0)
      scope.raiseError(errNotDefinedBaseClass, node.firstChild().firstChild(lxTerminalMask));
   
   // first pass - declaration
   compileDeclarations(node.firstChild(), scope);
   
   // second pass - implementation
   compileImplementations(node.firstChild(), scope);
}

void Compiler :: compileModule(ident_t source, ModuleScope& scope)
{
   Path path(source);

   // parse
   TextFileReader sourceFile(path.c_str(), scope.project->getDefaultEncoding(), true);
   if (!sourceFile.isOpened())
      scope.project->raiseError(errInvalidFile, source);
   
   SyntaxTree tree;
   DerivationWriter writer(tree);
   _parser.parse(&sourceFile, writer, scope.project->getTabSize());
   
   // compile
   compileModule(tree.readRoot(), scope);
}

bool Compiler :: run(_ProjectManager& project, bool withDebugInfo)
{
   Map<ident_t, ModuleInfo> modules(ModuleInfo(NULL, NULL));

   Unresolveds unresolveds(Unresolved(), NULL);

   Path modulePath;
   ReferenceNs name(project.Namespace());
   int rootLength = name.Length();
   for (SourceIterator it = project.getSourceIt(); !it.Eof(); it++) {
      try {
         // build module namespace
         modulePath.copySubPath(it.key());
         name.truncate(rootLength);
         name.pathToName(modulePath.c_str());

         // create or update module
         ModuleInfo info = modules.get(name);
         if (info.codeModule == NULL) {
            info.codeModule = project.createModule(name);
            if (withDebugInfo)
               info.debugModule = project.createDebugModule(name);

            createPackageInfo(info.codeModule, project);

            modules.add(name, info);
         }

         ModuleScope scope(&project, it.key(), info.codeModule, info.debugModule, &unresolveds);

         // HOTFIX : the module path should be presaved in debug section
         scope.sourcePathRef = _writer.writeSourcePath(info.debugModule, scope.sourcePath);

         // HOTFIX : the module path should be the first saved string
         _writer.clear();
         _writer.writeString(it.key());

         project.printInfo("%s", it.key());

         // compile source
         compileModule(*it, scope);
      }
      catch (LineTooLong& e)
      {
         project.raiseError(errLineTooLong, it.key(), e.row, 1);
      }
      catch (InvalidChar& e)
      {
         size_t destLength = 6;

         String<char, 6> symbol;
         Convertor::copy(symbol, (_ELENA_::unic_c*)&e.ch, 1, destLength);

         project.raiseError(errInvalidChar, it.key(), e.row, e.column, (const char*)symbol);
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

void Compiler :: injectVirtualReturningMethod(SNode node, ident_t variable)
{
   SNode expr = node.appendNode(lxReturning).appendNode(lxExpression);
   expr.appendNode(lxIdentifier).appendNode(lxTerminal, variable);
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

bool checkConstantCompatibility(_CompilerScope& scope, LexicalType type, ref_t targetClassRef)
{
   switch (type) {
      case lxConstantInt:
         return targetClassRef == scope.intReference;
      case lxConstantLong:
         return targetClassRef == scope.longReference;
      case lxConstantReal:
         return targetClassRef == scope.realReference;
      default:
         return false;
   }
}

void Compiler :: injectBoxing(_CompilerScope& scope, SNode node, LexicalType boxingType, int argument, ref_t targetClassRef)
{
   //HOTFIX : reload node
   node.refresh();

   SNode objectNode = node.firstChild(lxObjectMask);
   bool single = isSingleStatement(node);
   if (single && checkConstantCompatibility(scope, objectNode, targetClassRef)) {
      //HOTFIX : do not box compatible numeric constants
   }
   else {
      // inject boxing node if required
      if (node == lxExpression) {
         node.set(boxingType, argument);

         node.appendNode(lxTarget, targetClassRef);
      }
      else if (node == lxBoxing && node.argument == argument) {
         node.set(boxingType, argument);
         node.findChild(lxTarget).setArgument(targetClassRef);
      }
      else if (node == lxReturning) {
         SNode boxingNode = node.injectNode(boxingType, argument);

         boxingNode.appendNode(lxTarget, targetClassRef);
      }
      else {
         node.injectNode(node.type, node.argument);
         node.set(boxingType, argument);
         node.appendNode(lxTarget, targetClassRef);
      }
   }
}

void Compiler :: injectConverting(SNode node, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef)
{
   node.set(convertOp, convertArg);

   node.insertNode(createOp, createArg).appendNode(lxTarget, targetClassRef);

   node.appendNode(lxCallTarget, targetClassRef);
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

void Compiler :: injectLocalBoxing(SNode node, int size)
{
   //HOTFIX : using size variable copy to prevent aligning
   int dummy = size;
   int offset = allocateStructure(node, dummy);

   // allocate place for the local copy
   node.injectNode(node.type, node.argument);

   node.set(lxAssigning, size);

   node.insertNode(lxLocalAddress, offset);
}