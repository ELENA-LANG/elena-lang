//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiler.h"
#include "errors.h"
#include <errno.h>

using namespace _ELENA_;

////void test2(SNode node)
////{
////   SNode current = node.firstChild();
////   while (current != lxNone) {
////      test2(current);
////      current = current.nextNode();
////   }
////}

// --- Hint constants ---
//#define HINT_CLOSURE_MASK     0xC0008800

#define HINT_ROOT             0x80000000
//#define HINT_ROOTSYMBOL       0xC0000000
//#define HINT_NOBOXING         0x20000000
//#define HINT_NOUNBOXING       0x10000000
//#define HINT_EXTERNALOP       0x08000000
//#define HINT_NOCONDBOXING     0x04000000
//#define HINT_EXTENSION_MODE   0x02000000
//#define HINT_TRY_MODE         0x01000000
//#define HINT_LOOP             0x00800000
//#define HINT_SWITCH           0x00400000
//#define HINT_ALT_MODE         0x00200000
//#define HINT_SINGLETON        0x00100000
//#define HINT_EXT_RESENDEXPR   0x00080400
//#define HINT_ASSIGNING_EXPR   0x00040000
#define HINT_NODEBUGINFO      0x00020000
//#define HINT_PARAMETERSONLY   0x00010000
//#define HINT_SUBCODE_CLOSURE  0x00008800
//#define HINT_RESENDEXPR       0x00000400
//#define HINT_LAZY_EXPR        0x00000200
//#define HINT_DYNAMIC_OBJECT   0x00000100  // indicates that the structure MUST be boxed
//#define HINT_UNBOXINGEXPECTED 0x00000080
//#define HINT_INT64EXPECTED    0x00000004
//#define HINT_REAL64EXPECTED   0x00000002
//#define HINT_INQUIRY_MODE     0x00000001

typedef Compiler::ObjectInfo ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef ClassInfo::Attribute Attribute;

//// --- Auxiliary routines ---
//
//inline bool isCollection(SNode node)
//{
//   return (node == lxExpression && node.nextNode() == lxExpression);
//}
//
//inline bool isCollection(SNode node, bool nextedExpr)
//{
//   if (nextedExpr) {
//      if (isCollection(node)) {
//         return true;
//      }
//      else if (node == lxExpression && !node.existChild(lxMessage, lxAssign, lxOperator)) {
//         return isCollection(node.firstChild(lxObjectMask), true);
//      }
//      else return false;
//   }
//   else return isCollection(node);
//}
//
//inline bool isPrimitiveRef(ref_t reference)
//{
//   return (int)reference < 0;
//}
//
////inline bool isPrimitiveType(ref_t reference)
////{
////   return reference == V_ARGARRAY;
////}

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

inline SNode findParent(SNode node, LexicalType type)
{
   while (node.type != type && node != lxNone) {
      node = node.parentNode();
   }

   return node;
}

inline SNode findParent(SNode node, LexicalType type1, LexicalType type2)
{
   while (node.type != type1 && node.type != type2 && node != lxNone) {
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

//inline bool existChildWithArg(SNode node, LexicalType type, ref_t arg)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current.type == type && current.argument == arg)
//         return true;
//
//      current = current.nextNode();
//   }
//
//   return false;
//}
//
//inline SNode goToNode(SNode current, LexicalType type)
//{
//   while (current != lxNone && current != type)
//      current = current.nextNode();
//
//   return current;
//}
//
//inline bool validateGenericClosure(ident_t signature)
//{
//   size_t len = getlength(signature);
//   size_t start = 0;
//
//   while (true) {
//      size_t middle = signature.findSubStr(start + 1, "$", len);
//      size_t end = signature.findSubStr(middle + 1, "$", len);
//
//      IdentifierString first(signature, start, middle - start);
//      IdentifierString second(signature, middle, end - middle);
//      if (!first.compare(second))
//         return false;
//
//      if (end == len)
//         return true;
//
//      start = middle;
//   }
//}

//inline bool isSealedStaticField(ref_t ref)
//{
//   return (int)ref >= 0;
//}
//
////inline bool checkNode(SNode node, LexicalType type, ref_t argument)
////{
////   return node == type && node.argument == argument;
////}
//
//inline bool isConstantArguments(SNode node)
//{
//   if (node == lxNone)
//      return true;
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current.type)
//      {
//         case lxExpression:
//            if (!isConstantArguments(current))
//               return false;
//            break;
//         case lxLiteral:
//         case lxWide:
//         case lxCharacter:
//         case lxInteger:
//         case lxLong:
//         case lxHexInteger:
//         case lxReal:
//         case lxExplicitConst:
//         case lxMessage:
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

// --- Compiler::NamespaceScope ---

Compiler::NamespaceScope :: NamespaceScope(CompilerScope* moduleScope/*_ProjectManager* project, _Module* module, _Module* debugModule*//*, Unresolveds* forwardsUnresolved*/)
   : Scope(moduleScope)
   //: /*constantHints(INVALID_REF), extensions(NULL, freeobj), autoExtensions(NULL, freeobj), */
{
   this->sourcePath = NULL;

  //   this->forwardsUnresolved = forwardsUnresolved;

//   // cache the frequently used references
//   intReference = mapReference(project->resolveForward(INT_FORWARD));
//   longReference = mapReference(project->resolveForward(LONG_FORWARD));
//   realReference = mapReference(project->resolveForward(REAL_FORWARD));
//   literalReference = mapReference(project->resolveForward(STR_FORWARD));
//   wideReference = mapReference(project->resolveForward(WIDESTR_FORWARD));
//   charReference = mapReference(project->resolveForward(CHAR_FORWARD));
//   signatureReference = mapReference(project->resolveForward(SIGNATURE_FORWARD));
//   messageReference = mapReference(project->resolveForward(MESSAGE_FORWARD));
//   extMessageReference = mapReference(project->resolveForward(EXT_MESSAGE_FORWARD));
//   arrayReference = mapReference(project->resolveForward(ARRAY_FORWARD));
//   boolReference = mapReference(project->resolveForward(BOOL_FORWARD));
//
//   // HOTFIX : package section should be created if at least literal class is declated
//   if (literalReference != 0) {
//      packageReference = module->mapReference(ReferenceNs(module->Name(), PACKAGE_SECTION));
//   }
//   else packageReference = 0;

   importedNs.add(module->Name());

//   loadModuleInfo(module, true);
}

void Compiler::NamespaceScope :: loadNamespaceInfo(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxSourcePath:
            sourcePath = current.identifier();
            break;
         case lxImporting:
            importedNs.add(current.identifier());
            break;
         default:
            return;
      }

      current = current.nextNode();
   }
}

pos_t Compiler::NamespaceScope :: saveSourcePath(ByteCodeWriter& writer)
{
   if (!emptystr(sourcePath)) {
      int position = moduleScope->savedPaths.get(sourcePath);
      if (position == -1) {
         position = writer.writeSourcePath(moduleScope->debugModule, sourcePath);

         moduleScope->savedPaths.add(sourcePath, position);
      }

      return position;
   }
   else return 0;
}

////ref_t Compiler::ModuleScope :: getBaseLazyExpressionClass()
////{
////   return mapReference(project->resolveForward(LAZYEXPR_FORWARD));
////}

ObjectInfo Compiler::NamespaceScope :: mapTerminal(ident_t identifier)
{
   ref_t reference = resolveImplicitIdentifier(identifier);
   if (reference)
      return defineObjectInfo(reference, true);

   if (parent == NULL) {
      if (identifier.compare(NIL_VAR)) {
         return ObjectInfo(okNil);
      }
   }
   return Scope::mapTerminal(identifier);
}

//ObjectInfo Compiler::ModuleScope :: mapObject(SNode identifier)
//{
//   ident_t terminal = identifier.identifier();
//
////   if (identifier==lxReference) {
////      return mapReferenceInfo(terminal, false);
////   }
//   /*else */if (identifier==lxPrivate) {
//      if (terminal.compare(NIL_VAR)) {
//         return ObjectInfo(okNil);
//      }
//      else return /*defineObjectInfo(mapTerminal(identifier, true), true)*/ObjectInfo(); // !! temporal
//   }
//   else if (identifier==lxIdentifier) {
//      return defineObjectInfo(mapTerminal(identifier, true), true);
//   }
//   else return ObjectInfo();
//}

ref_t Compiler::NamespaceScope :: resolveImplicitIdentifier(ident_t identifier)
{
   ref_t reference = forwards.get(identifier);
   if (reference)
      return reference;

   // check private ones
   IdentifierString privateOne(PRIVATE_PREFIX_NS, identifier);
   reference = module->mapReference(privateOne.c_str(), true);
   if (reference) {
      forwards.add(identifier, reference);

      return reference;
   }

   // check imported references
   IdentifierString name("'", identifier);
   List<ident_t>::Iterator it = importedNs.start();
   while (!it.Eof()) {
      _Module* ext_module = moduleScope->project->loadModule(*it, true);
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

//////ref_t Compiler::ModuleScope :: mapNewSubject(ident_t terminal)
//////{
//////   IdentifierString fullName(terminal);
//////   fullName.append('$');
//////
//////   ident_t ns = module->Name();
//////   if (ns.compare(STANDARD_MODULE)) {
//////   }
//////   else if (ns.compare(STANDARD_MODULE, STANDARD_MODULE_LEN)) {
//////      fullName.append(ns + STANDARD_MODULE_LEN + 1);
//////   }
//////   else fullName.append(ns);
//////
//////   return module->mapSubject(fullName, false);
//////}
////
////ref_t Compiler::ModuleScope :: mapSubject(SNode terminal)
////{
////   ident_t identifier = NULL;
////   if (terminal.type == lxIdentifier || terminal.type == lxPrivate || terminal.type == lxMessage/* || terminal.type == lxAttribute*/) {
////      identifier = terminal.identifier();
////      if (emptystr(identifier))
////         identifier = terminal.findChild(lxTerminal).identifier();
////   }
////   else if (terminal.type == lxReference) {
////      identifier = terminal.identifier();
////      if (emptystr(identifier))
////         identifier = terminal.findChild(lxTerminal).identifier();
////
////      return module->mapReference(identifier);
////   }
////   else raiseError(errInvalidSubject, terminal);
////
////   ref_t classRef = 0;
////   if (terminal.type == lxIdentifier) {
////      classRef = attributes.get(identifier);
////      if (isPrimitiveRef(classRef)/* && !isPrimitiveType(classRef)*/)
////         classRef = 0; // ignore attributes in the message name
////         //raiseError(errInvalidSubject, terminal);
////   }
////
////   return classRef;
////}
////
////ref_t Compiler::ModuleScope :: mapSubject(SNode terminal, IdentifierString& output)
////{
////   ident_t identifier = terminal.identifier();
////   ref_t classRef = 0;
////
////   if (terminal == lxReference) {
////      classRef = module->mapReference(identifier, false);
////   }
////   else {
////      classRef = mapSubject(terminal);
////
////      // add a namespace for the private message
////      if (terminal.type == lxPrivate) {
////         output.append(project->Namespace());
////         output.append('#');
////         output.append(identifier.c_str() + 1);
////
////         return 0;
////      }
////   }
////
////   if (classRef == 0)
////      output.append(identifier);
////
////   return classRef;
////}

ref_t Compiler::NamespaceScope :: mapNewTerminal(SNode terminal)
{
   if (terminal == lxNameAttr) {
      // verify if the name is unique
      ident_t name = module->resolveReference(terminal.argument);
      ref_t reference = terminal.argument;
      if (name.startsWith(PRIVATE_PREFIX_NS)) {
         IdentifierString altName("'", name.c_str() + getlength(PRIVATE_PREFIX_NS));
         // if the public symbol with the same name was already declared -
         // raise an error
         ref_t dup = module->mapReference(altName.c_str(), true);
         if (dup)
            reference = dup;
      }
      else {
         IdentifierString altName(PRIVATE_PREFIX_NS, name.c_str() + 1);
         // if the private symbol with the same name was already declared -
         // raise an error
         ref_t dup = module->mapReference(altName.c_str(), true);
         if (dup)
            reference = dup;
      }

      if (module->mapSection(reference | mskSymbolRef, true))
         raiseError(errDuplicatedSymbol, terminal.parentNode().firstChild(lxTerminalMask));

      return terminal.argument;
   }
   else if (terminal == lxNone) {
      return mapAnonymous();
   }
   else return moduleScope->mapNewTerminal(terminal, false);
}

ref_t Compiler::NamespaceScope :: mapAnonymous()
{
   // auto generate the name
   IdentifierString name("'", INLINE_POSTFIX);

   findUninqueName(moduleScope->module, name);

   return moduleScope->module->mapReference(name);
}

//bool Compiler::ModuleScope :: doesReferenceExist(ident_t referenceName)
//{
//   ref_t moduleRef = 0;
//   _Module* module = project->resolveModule(referenceName, moduleRef, true);
//
//   if (module == NULL || moduleRef == 0)
//      return false;
//
//   return module->mapReference(referenceName, true) != 0;
//}

ObjectInfo Compiler::NamespaceScope :: defineObjectInfo(ref_t reference, bool checkState)
{
   // if reference is zero the symbol is unknown
   if (reference == 0) {
      return ObjectInfo();
   }
//   // check if symbol should be treated like constant one
//   else if (constantHints.exist(reference)) {
//      return ObjectInfo(okConstantSymbol, reference, constantHints.get(reference));
//   }
   else if (checkState) {
      ClassInfo info;
      // check if the object can be treated like a constant object
      ref_t r = moduleScope->loadClassInfo(info, reference, true);
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
//      else {
//         // check if the object is typed expression
//         SymbolExpressionInfo symbolInfo;
//         // check if the object can be treated like a constant object
//         r = loadSymbolExpressionInfo(symbolInfo, module->resolveReference(reference));
//         if (r) {
//            // if it is a constant
//            if (symbolInfo.constant) {
//               ref_t classRef = symbolInfo.expressionClassRef;
//
//               if (symbolInfo.listRef != 0) {
//                  return ObjectInfo(okArrayConst, symbolInfo.listRef, classRef);
//               }
//               else return ObjectInfo(okConstantSymbol, reference, classRef);
//            }
//            // if it is a typed symbol
//            else if (symbolInfo.expressionClassRef != 0) {
//               return ObjectInfo(okSymbol, reference, symbolInfo.expressionClassRef);
//            }
//         }
//      }
   }

   // otherwise it is a normal one
   return ObjectInfo(okSymbol, reference);
}

//ref_t Compiler::ModuleScope :: mapReference(ident_t referenceName, bool existing)
//{
//   if (emptystr(referenceName))
//      return 0;
//
//   ref_t reference = 0;
//   if (!isWeakReference(referenceName)) {
//      if (existing) {
//         // check if the reference does exist
//         ref_t moduleRef = 0;
//         _Module* argModule = project->resolveModule(referenceName, moduleRef);
//
//         if (argModule != NULL && moduleRef != 0)
//            reference = module->mapReference(referenceName);
//      }
//      else reference = module->mapReference(referenceName, existing);
//   }
//   else reference = module->mapReference(referenceName, existing);
//
//   return reference;
//}
//
////ObjectInfo Compiler::ModuleScope :: mapReferenceInfo(ident_t reference, bool existing)
////{
////   if (reference.compare(EXTERNAL_MODULE, strlen(EXTERNAL_MODULE))) {
////      char ch = reference[strlen(EXTERNAL_MODULE)];
////      if (ch == '\'' || ch == 0)
////         return ObjectInfo(okExternal);
////   }
////   // To tell apart primitive modules, the name convention is used
////   else if (reference.compare(INTERNAL_MASK, INTERNAL_MASK_LEN)) {
////      return ObjectInfo(okInternal, module->mapReference(reference));
////   }
////
////   ref_t referenceID = mapReference(reference, existing);
////
////   return defineObjectInfo(referenceID);
////}
//
////_Module* Compiler::ModuleScope :: loadReferenceModule(ref_t& reference)
////{
////   return project->resolveModule(module->resolveReference(reference), reference);
////}
////
////ident_t Compiler::ModuleScope :: resolveWeakTemplateReference(ident_t referenceName)
////{
////   ident_t resolvedName = project->resolveForward(referenceName);
////   if (emptystr(resolvedName)) {
////      // COMPILER MAGIC : try to find a template implementation
////      ref_t resolvedRef = 0;
////      _Module* refModule = project->resolveWeakModule(referenceName, resolvedRef, true);
////      if (refModule != nullptr) {
////         resolvedName = refModule->resolveReference(resolvedRef);
////
////         project->addForward(referenceName, resolvedName);
////      }
////   }
////
////   return resolvedName;
////}
//
////ref_t Compiler::ModuleScope :: loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol)
////{
////   if (emptystr(symbol))
////      return 0;
////
////   // load class meta data
////   ref_t moduleRef = 0;
////   _Module* argModule = project->resolveModule(symbol, moduleRef, true);
////
////   if (argModule == NULL || moduleRef == 0)
////      return 0;
////
////   // load argument VMT meta data
////   _Memory* metaData = argModule->mapSection(moduleRef | mskMetaRDataRef, true);
////   if (metaData == NULL || metaData->Length() != sizeof(SymbolExpressionInfo))
////      return 0;
////
////   MemoryReader reader(metaData);
////
////   info.load(&reader);
////
////   if (argModule != module) {
////      // import type
////      info.expressionClassRef = importReference(argModule, info.expressionClassRef, module);
////   }
////   return moduleRef;
////}
////
////void Compiler::ModuleScope :: validateReference(SNode terminal, ref_t reference)
////{
////   // check if the reference may be resolved
////   bool found = false;
////
////   if (warnOnWeakUnresolved || !isWeakReference(terminal.identifier())) {
////      int   mask = reference & mskAnyRef;
////      reference &= ~mskAnyRef;
////
////      ref_t    ref = 0;
////      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);
////
////      if (refModule != NULL) {
////         found = (refModule->mapSection(ref | mask, true)!=NULL);
////      }
////      if (!found) {
////         if (!refModule || refModule == module) {
////            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module,
////               terminal.findChild(lxRow).argument,
////               terminal.findChild(lxCol).argument));
////         }
////         else raiseWarning(WARNING_LEVEL_1, wrnUnresovableLink, terminal);
////      }
////   }
////}
//
//void Compiler::ModuleScope :: raiseError(const char* message, ident_t sourcePath, SNode node)
//{
//   SNode terminal = findTerminalInfo(node);
//
//   int col = terminal.findChild(lxCol).argument;
//   int row = terminal.findChild(lxRow).argument;
//   ident_t identifier = terminal.identifier();
//   if (emptystr(identifier))
//      identifier = terminal.identifier();
//
//   raiseError(message, row, col, sourcePath, identifier);
//}
//
//void Compiler::ModuleScope :: raiseWarning(int level, const char* message, int row, int col, ident_t sourcePath, ident_t terminal)
//{
//   if (test(warningMask, level))
//      project->raiseWarning(message, sourcePath, row, col, terminal);
//}
//
////void Compiler::ModuleScope::raiseWarning(int level, const char* message)
////{
////   if (test(warningMask, level))
////      project->raiseWarning(message, message);
////}
//
//void Compiler::ModuleScope :: raiseError(const char* message, int row, int col, ident_t sourcePath, ident_t terminal)
//{
//   project->raiseError(message, sourcePath, row, col, terminal);
//}
//
//void Compiler::ModuleScope::raiseError(const char* message)
//{
//   project->raiseError(message);
//}
//
////void Compiler::ModuleScope :: loadActions(_Module* extModule)
////{
////   if (extModule) {
////      ReferenceNs sectionName(extModule->Name(), ACTION_SECTION);
////
////      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
////      if (section) {
////         MemoryReader metaReader(section);
////         while (!metaReader.Eof()) {
////            ref_t mssg_ref = importMessage(extModule, metaReader.getDWord(), module);
////            ref_t class_ref = importReference(extModule, metaReader.getDWord(), module);
////
////            actionHints.add(mssg_ref, class_ref);
////         }
////      }
////   }
////}
//
////void Compiler::ModuleScope :: loadExtensions(_Module* extModule, bool& duplicateExtensions)
////{
////   if (extModule) {
////      ReferenceNs sectionName(extModule->Name(), EXTENSION_SECTION);
////
////      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
////      if (section) {
////         MemoryReader metaReader(section);
////         while (!metaReader.Eof()) {
////            ref_t type_ref = importReference(extModule, metaReader.getDWord(), module);
////            ref_t message = importMessage(extModule, metaReader.getDWord(), module);
////            ref_t role_ref = importReference(extModule, metaReader.getDWord(), module);
////
////            if(!extensionHints.exist(message, type_ref)) {
////               extensionHints.add(message, type_ref);
////
////               SubjectMap* typeExtensions = extensions.get(type_ref);
////               if (!typeExtensions) {
////                  typeExtensions = new SubjectMap();
////
////                  extensions.add(type_ref, typeExtensions);
////               }
////
////               typeExtensions->add(message, role_ref);
////            }
////            else {
////               SubjectMap* typeExtensions = extensions.get(type_ref);
////               ref_t duplicateRef = typeExtensions->get(message);
////
////               if (role_ref != duplicateRef) {
////                  // HOTFIX : ignore warning if it is virtual extension (for generic extension with open argument list)
////                  IdentifierString warningStr(wrnDuplicateInfo);
////                  warningStr.append(module->resolveSubject(getAction(message)));
////                  warningStr.append('[');
////                  warningStr.appendInt(getAbsoluteParamCount(message));
////                  warningStr.append("] - ");
////
////                  warningStr.append(module->resolveReference(role_ref));
////                  warningStr.append(" and ");
////                  warningStr.append(module->resolveReference(duplicateRef));
////
////                  raiseWarning(WARNING_LEVEL_3, warningStr.c_str());
////
////                  duplicateExtensions = true;
////               }
////            }
////         }
////      }
////   }
////}
////
////bool Compiler::ModuleScope :: saveExtension(ref_t message, ref_t typeRef, ref_t role)
////{
////   if (typeRef == INVALID_REF || typeRef ==superReference)
////      typeRef = 0;
////
////   ReferenceNs sectionName(module->Name(), EXTENSION_SECTION);
////
////   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
////
////   if (typeRef == superReference) {
////      metaWriter.writeDWord(0);
////   }
////   else metaWriter.writeDWord(typeRef);
////   metaWriter.writeDWord(message);
////   metaWriter.writeDWord(role);
////
////   if (!extensionHints.exist(message, typeRef)) {
////      extensionHints.add(message, typeRef);
////
////      SubjectMap* typeExtensions = extensions.get(typeRef);
////      if (!typeExtensions) {
////         typeExtensions = new SubjectMap();
////
////         extensions.add(typeRef, typeExtensions);
////      }
////
////      typeExtensions->add(message, role);
////
////      return true;
////   }
////   else return false;
////}
////
////void Compiler::ModuleScope :: saveIncludedModule(_Module* extModule)
////{
////   // HOTFIX : do not include itself
////   if (module == extModule)
////      return;
////
////   ReferenceNs sectionName(module->Name(), IMPORTS_SECTION);
////
////   _Memory* section = module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false);
////
////   // check if the module alread included
////   MemoryReader metaReader(section);
////   while (!metaReader.Eof()) {
////      ident_t name = metaReader.getLiteral(DEFAULT_STR);
////      if (name.compare(extModule->Name()))
////         return;
////   }
////
////   // otherwise add it to the list
////   MemoryWriter metaWriter(section);
////
////   metaWriter.writeLiteral(extModule->Name().c_str());
////}
////
////void Compiler::ModuleScope :: saveAction(ref_t mssg_ref, ref_t reference)
////{
////   ReferenceNs sectionName(module->Name(), ACTION_SECTION);
////
////   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
////
////   metaWriter.writeDWord(mssg_ref);
////   metaWriter.writeDWord(reference);
////
////   actionHints.add(mssg_ref, reference);
////}
//
//bool Compiler::ModuleScope :: saveAttribute(ident_t name, ref_t attr/*, bool internalType*/)
//{
//   if (!attr) {
//      return false;
//   }
////   else if (!internalType) {
//      ReferenceNs sectionName(module->Name(), ATTRIBUTE_SECTION);
//      MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
//
//      metaWriter.writeDWord(attr);
//      metaWriter.writeLiteral(name);
////   }
//
//   return attributes.add(name, attr, true);
//}
//
//ref_t Compiler::ModuleScope :: mapAttribute(SNode attribute)
//{
//   SNode terminal = attribute.findChild(/*lxPrivate, */lxIdentifier, lxExplicitAttr);
////   if (terminal == lxNone)
////      terminal = attribute;
////
//   if (terminal == lxExplicitAttr) {
//      ident_t value = terminal.identifier();
//
//      int attrRef = value.toInt(1);
//      return -attrRef;
//   }
//   else {
//      ident_t token = terminal.identifier();
////      if (emptystr(token))
////         token = terminal.findChild(lxTerminal).identifier();
//
//      return attributes.get(token);
//   }
//}
//
////void Compiler::ModuleScope :: loadAutogeneratedExtension(_Module* extModule)
////{
////   if (extModule) {
////      //bool owner = module == extModule;
////
////      ReferenceNs sectionName(extModule->Name(), AUTOEXTENSION_SECTION);
////
////      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
////      if (section) {
////         MemoryReader metaReader(section);
////         while (!metaReader.Eof()) {
////            ref_t attrRef = metaReader.getDWord();
////            ref_t extension = metaReader.getDWord();
////
////            SubjectList* list = autoExtensions.get(attrRef);
////            if (!list) {
////               list = new SubjectList();
////               autoExtensions.add(attrRef, list);
////            }
////
////            list->add(extension);
////         }
////      }
////   }
////}
////
////void Compiler::ModuleScope :: saveAutogerenatedExtension(ref_t attrRef, ref_t extension)
////{
////   ReferenceNs sectionName(module->Name(), AUTOEXTENSION_SECTION);
////   MemoryWriter metaWriter(module->mapSection(mapReference(sectionName, false) | mskMetaRDataRef, false));
////
////   metaWriter.writeDWord(attrRef);
////   metaWriter.writeDWord(extension);
////
////   SubjectList* list = autoExtensions.get(attrRef);
////   if (!list) {
////      list = new SubjectList();
////      autoExtensions.add(attrRef, list);
////   }
////
////   list->add(extension);
////}
////
////ref_t Compiler::ModuleScope :: mapTemplateClass(ident_t templateName, bool& alreadyDeclared)
////{
////   ReferenceNs forwardName;
////   forwardName.append("'");
////   forwardName.append(templateName);
////
////   if (emptystr(project->resolveForward(forwardName))) {
////      ReferenceNs fullName(module->Name());
////      fullName.combine(templateName);
////
////      project->addForward(forwardName, fullName);
////
////      alreadyDeclared = false;
////   }
////   else alreadyDeclared = true;
////
////   return module->mapReference(forwardName);
////}
////
////bool Compiler::ModuleScope :: includeModule(ident_t name, bool& duplicateExtensions, bool& duplicateAttributes, bool& duplicateInclusion)
////{
////   // check if the module exists
////   _Module* module = project->loadModule(name, true);
////   if (module) {
////      ident_t value = retrieve(defaultNs.start(), name, NULL);
////      if (value == NULL) {
////         defaultNs.add(module->Name());
////
////         loadModuleInfo(module, duplicateExtensions, duplicateAttributes);
////
////         return true;
////      }
////      else duplicateInclusion = true;
////   }
////   return false;
////}

// --- Compiler::SourceScope ---

Compiler::SourceScope :: SourceScope(NamespaceScope* moduleScope, ref_t reference/*, ident_t sourcePath*/)
   : Scope(moduleScope/*, sourcePath*/)
{
   this->reference = reference;
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(NamespaceScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   outputRef = 0;
//   constant = false;
   staticOne = false;
//   preloaded = false;
}

//////ObjectInfo Compiler::SymbolScope :: mapTerminal(ident_t identifier)
//////{
//////   return Scope::mapTerminal(identifier);
//////}
////
////void Compiler::SymbolScope :: save()
////{
////   SymbolExpressionInfo info;
////   info.expressionClassRef = outputRef;
////   info.constant = constant;
////
////   // save class meta data
////   MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false), 0);
////   info.save(&metaWriter);
////}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(NamespaceScope* parent, ref_t reference/*, ident_t sourcePath*/)
   : SourceScope(parent, reference/*, sourcePath*/)
{
   info.header.parentRef =  moduleScope->superReference;
   info.header.flags = elStandartVMT;
   info.header.count = 0;
   info.header.classRef = 0;
   info.header.staticSize = 0;
//   info.size = 0;
//
//   extensionClassRef = 0;
//   embeddable = false;
//   classClassMode = false;
//   abstractBasedMode = false;
}

////void Compiler::ClassScope :: copyStaticFields(ClassInfo::StaticFieldMap& statics, ClassInfo::StaticInfoMap& staticValues)
////{
////   // import static fields
////   ClassInfo::StaticFieldMap::Iterator static_it = statics.start();
////   while (!static_it.Eof()) {
////      info.statics.add(static_it.key(), *static_it);
////
////      static_it++;
////   }
////
////   auto staticValue_it = staticValues.start();
////   while (!staticValue_it.Eof()) {
////      ref_t val = *staticValue_it;
////      info.staticValues.add(staticValue_it.key(), val);
////
////      staticValue_it++;
////   }
////}
////
////ObjectInfo Compiler::ClassScope :: mapField(ident_t terminal)
////{
////   int offset = info.fields.get(terminal);
////   if (offset >= 0) {
////      ClassInfo::FieldInfo fieldInfo = info.fieldTypes.get(offset);
////      if (test(info.header.flags, elStructureRole)) {
////         return ObjectInfo(okFieldAddress, offset, fieldInfo.value1, fieldInfo.value2);
////      }
////      else return ObjectInfo(okField, offset, fieldInfo.value1, fieldInfo.value2);
////   }
////   else if (offset == -2 && test(info.header.flags, elDynamicRole)) {
////      return ObjectInfo(okThisParam, 1, -2, info.fieldTypes.get(-1).value1);
////   }
////   else {
////      ClassInfo::FieldInfo staticInfo = info.statics.get(terminal);
////      if (staticInfo.value1 != 0) {
////         if (!isSealedStaticField(staticInfo.value1)) {
////            ref_t val = info.staticValues.get(staticInfo.value1);
////            if (val != mskStatRef) {
////               if (classClassMode) {
////                  return ObjectInfo(okClassStaticConstantField, 0, staticInfo.value1, staticInfo.value2);
////               }
////               else return ObjectInfo(okStaticConstantField, staticInfo.value1, staticInfo.value2);
////            }
////         }
////         if (classClassMode) {
////            return ObjectInfo(okClassStaticField, 0, staticInfo.value1, staticInfo.value2);
////         }         
////         else return ObjectInfo(okStaticField, staticInfo.value1, staticInfo.value2);
////      }
////      else return ObjectInfo();
////   }
////}
////
////ObjectInfo Compiler::ClassScope :: mapTerminal(ident_t terminal)
////{
////   if (terminal.compare(SUPER_VAR)) {
////      return ObjectInfo(okSuper, info.header.parentRef);
////   }
////   else {
////      ObjectInfo fieldInfo = mapField(terminal);
////      if (fieldInfo.kind != okUnknown) {
////         return fieldInfo;
////      }
////      else return Scope::mapTerminal(terminal);
////   }
////}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved = 0;
   this->rootToFree = 1;
   this->hints = 0;
//   this->withOpenArg = false;
//   this->stackSafe = this->classEmbeddable = false;
//   this->generic = false;
//   this->extensionMode = false;
   this->multiMethod = false;
//   this->closureMode = false;
//   this->nestedMode = parent->getScope(Scope::slOwnerClass) != parent;
//   this->subCodeMode = false;
//   this->genericClosure = false;
}

ObjectInfo Compiler::MethodScope :: mapSelf(bool forced)
{
//   if (extensionMode && !forced) {
//      //COMPILER MAGIC : if it is an extension ; replace $self with self
//      ClassScope* extensionScope = (ClassScope*)getScope(slClass);
//
//      return ObjectInfo(okLocal, (ref_t)-1, extensionScope->extensionClassRef, extensionScope->embeddable ? -1 : 0);
//   }
//   else if (stackSafe && classEmbeddable) {
//      return ObjectInfo(okThisParam, 1, ((ClassScope*)getScope(slClass))->reference, (ref_t)-1);
//   }
   /*else */return ObjectInfo(okSelfParam, 1, ((ClassScope*)getScope(slClass))->reference);
}

ObjectInfo Compiler::MethodScope :: mapParameter(Parameter param)
{
   int prefix = /*closureMode ? 0 : */-1;

//   if (withOpenArg && param.class_ref == V_ARGARRAY) {
//      return ObjectInfo(okParams, prefix - param.offset, param.class_ref, param.element_ref);
//   }
//   else if (stackSafe && param.class_ref != 0 && param.size != 0) {
//      return ObjectInfo(okParam, prefix - param.offset, param.class_ref, (ref_t)-1);
//   }
   return ObjectInfo(okParam, prefix - param.offset, param.class_ref);
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal)
{
//   if (terminal.compare(THIS_VAR)) {
//      if (closureMode || nestedMode) {
//         return parent->mapTerminal(OWNER_VAR);
//      }
//      else return mapThis();
//   }
//   else if (terminal.compare(SELF_VAR) && !closureMode) {
//      if (extensionMode) {
//         return mapThis();
//      }
//      else return ObjectInfo(okParam, (size_t)-1);
//   }
//   else if (terminal.compare(RETVAL_VAR) && subCodeMode) {
//      ObjectInfo retVar = parent->mapTerminal(terminal);
//      if (retVar.kind == okUnknown) {
//         InlineClassScope* closure = (InlineClassScope*)getScope(Scope::slClass);
//
//         retVar = closure->allocateRetVar();
//      }
//
//      return retVar;
//   }
//   else {
      Parameter param = parameters.get(terminal);
      if (param.offset >= 0) {
         return mapParameter(param);
      }
      else return Scope::mapTerminal(terminal);
//   }
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent)//, locals(Parameter(0))
{
   this->level = 0;
   //this->saved = this->reserved = 0;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent)//, locals(Parameter(0))
{
   this->level = 0;
//   this->saved = this->reserved = 0;
}

//Compiler::CodeScope :: CodeScope(CodeScope* parent)
//   : Scope(parent), locals(Parameter(0))
//{
//   this->level = parent->level;
//   this->saved = parent->saved;
//   this->reserved = parent->reserved;
//}
//
//ObjectInfo Compiler::CodeScope :: mapMember(ident_t identifier)
//{
//   if (identifier.compare(SELF_VAR)) {
//      MethodScope* methodScope = (MethodScope*)getScope(Scope::slMethod);
//      if (methodScope != NULL) {
//         return methodScope->mapThis();
//      }
//   }
//   else {
//      ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);
//      if (classScope != NULL) {
//         return classScope->mapField(identifier);
//      }      
//   }
//   return ObjectInfo();
//}
//
//bool Compiler::CodeScope :: resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element)
//{
//   if (info.kind == okLocal) {
//      for (auto it = locals.start(); !it.Eof(); it++) {
//         if ((*it).offset == info.param) {
//            if ((*it).class_ref == V_AUTO) {
//               (*it).class_ref = reference;
//               (*it).element_ref = element;
//
//               info.extraparam = reference;
//               info.element = element;
//
//               return true;
//            }
//         }
//      }
//   }
//
//   return Scope::resolveAutoType(info, reference, element);
//}
//
//ObjectInfo Compiler::CodeScope :: mapTerminal(ident_t identifier)
//{
//   Parameter local = locals.get(identifier);
//   if (local.offset) {
//      if (identifier.compare(SUBJECT_VAR)) {
//         return ObjectInfo(okSubject, local.offset);
//      }
//      else if (local.size != 0) {
//         return ObjectInfo(okLocalAddress, local.offset, local.class_ref);
//      }
//      else return ObjectInfo(okLocal, local.offset, local.class_ref, local.element_ref);
//   }
//   else return Scope::mapTerminal(identifier);
//}
//
////// --- Compiler::ResendScope ---
////
////ObjectInfo Compiler::ResendScope :: mapTerminal(ident_t identifier)
////{
////   if (!withFrame && (identifier.compare(THIS_VAR) || identifier.compare(SELF_VAR)))
////   {
////      return ObjectInfo();
////   }
////
////   ObjectInfo info = CodeScope::mapTerminal(identifier);
////   if (consructionMode && (info.kind == okField || info.kind == okFieldAddress)) {
////      return ObjectInfo();
////   }
////   else return info;
////}
////
////// --- Compiler::InlineClassScope ---
////
////Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
////   : ClassScope(owner->moduleScope, reference), outers(Outer())//, outerFieldTypes(ClassInfo::FieldInfo(0, 0))
////{
////   this->returningMode = false;
////   this->parent = owner;
////   info.header.flags |= elNestedClass;
////}
////
////Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapParent()
////{
////   Outer parentVar = outers.get(PARENT_VAR);
////   // if owner reference is not yet mapped, add it
////   if (parentVar.outerObject.kind == okUnknown) {
////      parentVar.reference = info.fields.Count();
////      CodeScope* codeScope = (CodeScope*)parent->getScope(Scope::slCode);
////      if (codeScope) {
////         parentVar.outerObject = codeScope->mapMember(SELF_VAR);
////      }
////      else parentVar = mapOwner();
////
////      outers.add(PARENT_VAR, parentVar);
////      mapKey(info.fields, PARENT_VAR, (int)parentVar.reference);
////
////   }
////   return parentVar;
////}
////
////Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
////{
////   Outer owner = outers.get(THIS_VAR);
////   // if owner reference is not yet mapped, add it
////   if (owner.outerObject.kind == okUnknown) {
////      owner.reference = info.fields.Count();
////
////      owner.outerObject = parent->mapTerminal(THIS_VAR);
////      if (owner.outerObject.kind == okUnknown) {
////         // HOTFIX : if it is a singleton nested class
////         owner.outerObject = ObjectInfo(okThisParam, 1, reference);
////      }
////      else if (owner.outerObject.kind == okThisParam) {
////         owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId(false);
////      }
////
////      outers.add(THIS_VAR, owner);
////      mapKey(info.fields, THIS_VAR, (int)owner.reference);
////   }
////   return owner;
////}
////
////Compiler::InlineClassScope::Outer Compiler::InlineClassScope::mapOwner()
////{
////   Outer owner = outers.get(OWNER_VAR);
////   // if owner reference is not yet mapped, add it
////   if (owner.outerObject.kind == okUnknown) {
////      owner.outerObject = parent->mapTerminal(OWNER_VAR);
////      if (owner.outerObject.kind != okUnknown) {
////         owner.reference = info.fields.Count();
////
////         if (owner.outerObject.extraparam == 0)
////            owner.outerObject.extraparam = ((CodeScope*)parent)->getClassRefId(false);
////
////         outers.add(OWNER_VAR, owner);
////         mapKey(info.fields, OWNER_VAR, (int)owner.reference);
////      }
////      else return mapSelf();
////   }
////   return owner;
////}
////
////ObjectInfo Compiler::InlineClassScope :: mapTerminal(ident_t identifier)
////{
////   if (identifier.compare(SUPER_VAR)) {
////      return ObjectInfo(okSuper, info.header.parentRef);
////   }
////   else if (identifier.compare(OWNER_VAR)) {
////      Outer owner = mapOwner();
////
////      // map as an outer field (reference to outer object and outer object field index)
////      return ObjectInfo(okOuter, owner.reference, owner.outerObject.extraparam/*, owner.outerObject.type*/);
////   }
////   //else if (identifier.compare(SELF_VAR) && !closureMode) {
////   //   return ObjectInfo(okParam, (size_t)-1);
////   //}
////   else {
////      Outer outer = outers.get(identifier);
////
////      // if object already mapped
////      if (outer.reference != -1) {
////         if (outer.outerObject.kind == okSuper) {
////            return ObjectInfo(okSuper, outer.reference);
////         }
////         else return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam);
////      }
////      else {
////         outer.outerObject = parent->mapTerminal(identifier);
////         switch (outer.outerObject.kind) {
////            case okField:
////            case okStaticField:
////            {
////               // handle outer fields in a special way: save only self
////               Outer owner = mapParent();
////
////               // map as an outer field (reference to outer object and outer object field index)
////               if (outer.outerObject.kind == okOuterField) {
////                  return ObjectInfo(okOuterField, owner.reference, outer.outerObject.extraparam, outer.outerObject.element);
////               }
////               else if (outer.outerObject.kind == okOuterStaticField) {
////                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.extraparam, outer.outerObject.element);
////               }
////               else if (outer.outerObject.kind == okStaticField) {
////                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.param, outer.outerObject.extraparam);
////               }
////               else return ObjectInfo(okOuterField, owner.reference, outer.outerObject.param, outer.outerObject.extraparam);
////            }
////            case okParam:
////            case okLocal:
////            case okOuter:
////            case okSuper:
////            case okThisParam:
////            case okLocalAddress:
////            case okFieldAddress:
////            case okOuterField:
////            case okOuterStaticField:
////            case okParams:
////            {
////               // map if the object is outer one
////               outer.reference = info.fields.Count();
////
////               outers.add(identifier, outer);
////               mapKey(info.fields, identifier, (int)outer.reference);
////
////               if (outer.outerObject.kind == okOuter && identifier.compare(RETVAL_VAR)) {
////                  // HOTFIX : quitting several clsoures
////                  (*outers.getIt(identifier)).preserved = true;
////               }
////
////               return ObjectInfo(okOuter, outer.reference, outer.outerObject.extraparam);
////            }
////            case okUnknown:
////            {
////               // check if there is inherited fields
////               ObjectInfo fieldInfo = mapField(identifier);
////               if (fieldInfo.kind != okUnknown) {
////                  return fieldInfo;
////               }
////               else return outer.outerObject;
////            }
////            default:               
////               return outer.outerObject;
////         }
////      }
////   }
////}
////
////bool Compiler::InlineClassScope :: markAsPresaved(ObjectInfo object)
////{
////   if (object.kind == okOuter) {
////      Map<ident_t, Outer>::Iterator it = outers.start();
////      while (!it.Eof()) {
////         if ((*it).reference == object.param) {
////            if ((*it).outerObject.kind == okLocal || (*it).outerObject.kind == okLocalAddress) {
////               (*it).preserved = true;
////
////               return true;
////            }
////            else if ((*it).outerObject.kind == okOuter) {
////               InlineClassScope* closure = (InlineClassScope*)parent->getScope(Scope::slClass);
////               if (closure->markAsPresaved((*it).outerObject)) {
////                  (*it).preserved = true;
////
////                  return true;
////               }
////               else return false;
////            }
////            break;
////         }
////
////         it++;
////      }
////   }
////
////   return false;
////}
////
////ObjectInfo Compiler::InlineClassScope :: allocateRetVar()
////{
////   returningMode = true;
////
////   Outer outer;
////   outer.reference = info.fields.Count();
////   outer.outerObject = ObjectInfo(okNil, -1);
////
////   outers.add(RETVAL_VAR, outer);
////   mapKey(info.fields, RETVAL_VAR, (int)outer.reference);
////
////   return ObjectInfo(okOuter, outer.reference);
////}

// --- Compiler ---

Compiler :: Compiler(_CompilerLogic* logic)
   : _verbs(0)
{
   _optFlag = 0;

   this->_logic = logic;

   ByteCodeCompiler::loadVerbs(_verbs);
//   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: writeMessageInfo(SyntaxWriter& writer, CompilerScope& scope, ref_t messageRef)
{
   ref_t actionRef;
   int paramCount;
   decodeMessage(messageRef, actionRef, paramCount);

   IdentifierString name;
   ref_t signature = 0;
   name.append(scope.module->resolveAction(actionRef, signature));

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

//bool Compiler :: calculateIntOp(int operation_id, int arg1, int arg2, int& retVal)
//{
//   switch (operation_id)
//   {
//      case ADD_MESSAGE_ID:
//         retVal = arg1 + arg2;
//         break;
//      case SUB_MESSAGE_ID:
//         retVal = arg1 - arg2;
//         break;
//      case MUL_MESSAGE_ID:
//         retVal = arg1 * arg2;
//         break;
//      case DIV_MESSAGE_ID:
//         retVal = arg1 / arg2;
//         break;
//      case AND_MESSAGE_ID:
//         retVal = arg1 & arg2;
//         break;
//      case OR_MESSAGE_ID:
//         retVal = arg1 | arg2;
//         break;
//      case XOR_MESSAGE_ID:
//         retVal = arg1 ^ arg2;
//         break;
//      case READ_MESSAGE_ID:
//         retVal = arg1 >> arg2;
//         break;
//      case WRITE_MESSAGE_ID:
//         retVal = arg1 << arg2;
//         break;
//      default:
//         return false;
//   }
//
//   return true;
//}
//
//bool Compiler :: calculateRealOp(int operation_id, double arg1, double arg2, double& retVal)
//{
//   switch (operation_id)
//   {
//      case ADD_MESSAGE_ID:
//         retVal = arg1 + arg2;
//         break;
//      case SUB_MESSAGE_ID:
//         retVal = arg1 - arg2;
//         break;
//      case MUL_MESSAGE_ID:
//         retVal = arg1 * arg2;
//         break;
//      case DIV_MESSAGE_ID:
//         retVal = arg1 / arg2;
//         break;
//      default:
//         return false;
//   }
//
//   return true;
//}

//ref_t Compiler :: resolveConstantObjectReference(CodeScope& scope, ObjectInfo object)
//{
//   switch (object.kind) {
//      case okIntConstant:
//         return scope.moduleScope->intReference;
//      case okSignatureConstant:
//         return scope.moduleScope->signatureReference;
//      default:
//         return resolveObjectReference(scope, object);
//   }
//}
//
//ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object, ref_t targetRef)
//{
//   if (object.kind == okExternal) {
//      // HOTFIX : recognize external functions returning long / real
//      if (targetRef == scope.moduleScope->longReference) {
//         return V_INT64;
//      }
//      else if (targetRef == scope.moduleScope->realReference) {
//         return V_REAL64;
//      }
//      else return resolveObjectReference(scope, object);
//   }
//   else return resolveObjectReference(scope, object);
//}

ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object)
{
   if (object.kind == okSelfParam) {
//      if (object.extraparam == -2) {
//         return _logic->definePrimitiveArray(*scope.moduleScope, object.element);
//      }
      /*else */return scope.getClassRefId(false);
   }
   else return resolveObjectReference(/**scope.moduleScope, */object);
}

ref_t Compiler :: resolveObjectReference(/*ModuleScope& scope, */ObjectInfo object)
{
   // if static message is sent to a class class
   switch (object.kind)
   {
      case okConstantClass:
         return object.extraparam;
      //case okConstantRole:
      //   // if external role is provided
      //   return object.param;
      case okConstantSymbol:
         if (object.extraparam != 0) {
            return object.extraparam;
         }
         else return object.param;
      //case okLocalAddress:
      //   return object.extraparam;
      //case okIntConstant:
      //case okUIntConstant:
      //   return V_INT32;
      //case okLongConstant:
      //   return V_INT64;
      //case okRealConstant:
      //   return V_REAL64;
      //case okCharConstant:
      //   return scope.charReference;
      //case okLiteralConstant:
      //   return scope.literalReference;
      //case okWideLiteralConstant:
      //   return scope.wideReference;
      //case okSubject:
      //case okSignatureConstant:
      //   return V_SIGNATURE;
      //case okExtMessageConstant:
      //   return scope.extMessageReference;
      //case okSuper:
      //   return object.param;
      //case okParams:
      //   return V_ARGARRAY;
      //case okExternal:
      //   return V_INT32;
      //case okMessageConstant:
      //   return V_MESSAGE;
      case okNil:
         return V_NIL;
      //case okField:
      //case okLocal:
      //case okFieldAddress:
      //case okOuter:
      case okParam:
      case okSymbol:
      //case okStaticField:
      //case okStaticConstantField:
         return object.extraparam;
      //case okClassStaticConstantField:
      //case okOuterField:
      //case okOuterStaticField:
      //case okClassStaticField:
      //   return object.element;
      default:
         if (object.kind == okObject) {
            return object.param;
         }
         else return 0;
   }
}

void Compiler :: declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope/*, bool withThis, bool withSelf*/)
{
   CompilerScope* moduleScope = scope.moduleScope;

//   // declare built-in variables
//   if (withThis) {
//      if (scope.stackSafe && scope.classEmbeddable) {
//         SNode debugNode = node.insertNode(lxBinarySelf, 1);
//         debugNode.appendNode(lxClassName, scope.moduleScope->module->resolveReference(scope.getClassRef()));
//      }
//      else writer.appendNode(lxSelfVariable, 1);
//   }
//
//   if (withSelf)
//      writer.appendNode(lxSelfVariable, -1);

   writeMessageInfo(writer, *moduleScope, scope.message);

   int prefix = /*scope.closureMode ? 0 : */-1;

   SNode current = node.firstChild();
   // method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter/* || current == lxIdentifier*/) {
         ident_t name = /*(current == lxIdentifier) ? current.identifier() : */current.findChild(lxIdentifier, lxPrivate).identifier();
         Parameter param = scope.parameters.get(name);
         if (param.offset != -1) {
//            if (param.class_ref == V_ARGARRAY) {
//               writer.newNode(lxParamsVariable);
//            }
//            else if (param.class_ref == moduleScope->intReference) {
//               writer.newNode(lxIntVariable);
//            }
//            else if (param.class_ref == moduleScope->longReference) {
//               writer.newNode(lxLongVariable);
//            }
//            else if (param.class_ref == moduleScope->realReference) {
//               writer.newNode(lxReal64Variable);
//            }
//            else if (scope.stackSafe && param.class_ref != 0) {
//               ref_t classRef = param.class_ref;
//               if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {
//                  writer.newNode(lxBinaryVariable);
//                  writer.appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
//               }
//               else writer.newNode(lxVariable);
//            }
            /*else */writer.newNode(lxVariable);

            writer.appendNode(lxLevel, prefix - param.offset);
            writer.newNode(lxIdentifier, name);
            writer.closeNode();

            writer.closeNode();
         }
      }
      else if (current == lxSourcePath) {
         writer.appendNode(lxSourcePath, scope.saveSourcePath(_writer));
      }

      current = current.nextNode();
   }

//   //COMPILER MAGIC : if it is an extension ; replace $self with self
//   ClassScope* ownerScope = (ClassScope*)scope.getScope(Scope::slClass);
//   scope.extensionMode = (ownerScope != NULL && ownerScope->extensionClassRef != 0);
}

void Compiler :: importCode(SyntaxWriter& writer, SNode node, Scope& scope, ident_t function, ref_t message)
{
   CompilerScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(function);
   virtualReference.append('.');

   int paramCount;
   ref_t actionRef;
   decodeMessage(message, actionRef, paramCount);

   // HOTFIX : include self as a parameter
   paramCount++;

   size_t signIndex = virtualReference.Length();
   virtualReference.append('0' + (char)paramCount);

   ref_t signature = 0;
   virtualReference.append(moduleScope->module->resolveAction(actionRef, signature));

   virtualReference.replaceAll('\'', '@', signIndex);

   ref_t reference = 0;
   _Module* api = moduleScope->project->resolveModule(virtualReference, reference);

   _Memory* section = api != NULL ? api->mapSection(reference | mskCodeRef, true) : NULL;
   if (section != NULL) {
      writer.appendNode(lxImporting, _writer.registerImportInfo(section, api, moduleScope->module));
   }
   else scope.raiseError(errInvalidLink, node);
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed)
{
   CompilerScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.header.classRef;

   // get module reference
   ref_t moduleRef = 0;
   _Module* module = moduleScope->resolveReference(parentRef, moduleRef);

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

      //if (test(scope.info.header.flags, elAbstract)) {
      //   // exclude abstract flag
      //   scope.info.header.flags &= ~elAbstract;
      //   scope.abstractBasedMode = true;
      //}

      scope.info.header.flags |= flagCopy;

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

ref_t Compiler :: resolveParentRef(SNode node, CompilerScope& moduleScope, bool silentMode)
{
   ref_t parentRef = 0;
   /*SNode identifier = node.findChild(lxIdentifier, lxPrivate, lxReference);
   if (identifier != lxNone) {
      if (identifier == lxIdentifier || identifier == lxPrivate) {
         parentRef = moduleScope.mapTerminal(identifier, true);
      }
      else {
         ident_t name = identifier.findChild(lxTerminal).identifier();
         if (emptystr(name))
            name = identifier.identifier();

         parentRef = moduleScope.mapReference(name);
      }

      if (parentRef == 0 && !silentMode)
         moduleScope.raiseError(errUnknownClass, identifier);
   }*/

   return parentRef;
}

void Compiler :: compileParentDeclaration(SNode node, ClassScope& scope)
{
//   if (node.existChild(lxTemplate)) {
//      // hotfix : mark the class as template inherited one
//      scope.abstractBasedMode = true;
//   }

   ref_t parentRef = resolveParentRef(node, *scope.moduleScope, false);
   if (scope.info.header.parentRef == scope.reference) {
      if (parentRef != 0) {
         scope.raiseError(errInvalidSyntax, node);
      }
   }
   else if (parentRef == 0) {
      parentRef = scope.info.header.parentRef;
   }

   compileParentDeclaration(node, scope, parentRef);
}

void Compiler :: declareClassAttributes(SNode node, ClassScope& scope)
{
   int flags = scope.info.header.flags;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
//         if (current.argument == V_ARGARRAY) {
//            current.set(lxTarget, V_ARGARRAY);
//         }
//         else if (current.argument == V_ACTION) {
//            // to indicate the closure base class
//            current = lxClosureAttr;
//         }
//         else {
            int value = current.argument;
            if (!_logic->validateClassAttribute(value)) {
               current = lxIdle;

               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            }
            else {
               current.set(lxClassFlag, value);
               if (value != 0 && test(flags, value)) {
                  scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateAttribute, current);
               }
               flags |= value;
            }
         }
//      }
//      else if (current == lxClassRefAttr) {
//         current.set(lxTarget, scope.moduleScope->module->mapReference(current.identifier(), false));
//      }
      current = current.nextNode();
   }
}

//void Compiler :: declareSymbolAttributes(SNode node, SymbolScope& scope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxAttribute) {
//         int value = current.argument;
//         if (!_logic->validateSymbolAttribute(value, scope.constant, scope.staticOne, scope.preloaded)) {
//            current = lxIdle; // HOTFIX : to prevent duplicate warnings
//            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
//         }
//      }
//      else if (current == lxClassRefAttr) {
//         scope.outputRef = scope.moduleScope->module->mapReference(current.identifier(), false);
//      }
//
//      current = current.nextNode();
//   }
//}

void Compiler :: declareFieldAttributes(SNode node, ClassScope& scope/*, ref_t& fieldRef, ref_t& elementRef, int& size, bool& isStaticField, bool& isSealed, bool& isConstant*/)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateFieldAttribute(value/*, isSealed, isConstant*/)) {
            //if (value == lxStaticAttr) {
            //   isStaticField = true;
            //}
            //else if (value == V_OBJARRAY) {
            //   elementRef = fieldRef;
            //   fieldRef = V_OBJARRAY;
            //}
            /*else */if (value == -1) {
               // ignore if constant / sealed attribute was set
            }
            //else if (fieldRef == 0) {
            //   fieldRef = current.argument;
            //}
            else scope.raiseError(errInvalidHint, node);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      //else if (current == lxSize) {
      //   if (size == 0) {
      //      size = current.argument;
      //   }
      //   else scope.raiseError(errInvalidHint, node);
      //}
      //else if (current == lxClassRefAttr) {
      //   if (fieldRef == 0) {
      //      fieldRef = scope.moduleScope->module->mapReference(current.identifier(), false);
      //      if (current.existChild(lxSize)) {
      //         if (size == 0) {
      //            size = current.findChild(lxSize).argument;
      //         }
      //         else scope.raiseError(errInvalidHint, node);
      //      }
      //   }
      //   else scope.raiseError(errInvalidHint, node);
      //}

      current = current.nextNode();
   }

   ////HOTFIX : recognize primitive numeric constants
   //if (fieldRef == V_OBJARRAY && isPrimitiveRef(elementRef)) {
   //   switch (elementRef) {
   //      case V_INT32:
   //      case V_PTR32:
   //      case V_REAL64:
   //      case V_MESSAGE:
   //      case V_SYMBOL:
   //      case V_SIGNATURE:
   //      case V_EXTMESSAGE:
   //         fieldRef = elementRef;
   //         elementRef = 0;
   //         break;
   //   }
   //}
}

//void Compiler :: declareLocalAttributes(SNode node, CodeScope& scope, ObjectInfo& variable, int& size)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxAttribute) {
//         int value = current.argument;
//         if (value > 0 && size == 0) {
//            size = value;
//         }
//         else if (_logic->validateLocalAttribute(value)) {
//            // negative value defines the target virtual class
//            if (variable.extraparam == 0) {
//               variable.extraparam = value;
//            }
//            else if (value == V_OBJARRAY) {
//               variable.element = variable.extraparam;
//               variable.extraparam = value;
//            }
//            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
//         }
//         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
//      }
//      else if (current == lxClassRefAttr || current == lxReference) {
//         if (variable.extraparam == 0) {
//            variable.extraparam = scope.moduleScope->module->mapReference(current.identifier(), false);
//         }
//         else scope.raiseError(errInvalidHint, node);
//      }
//      current = current.nextNode();
//   }
//
//   if (size != 0 && variable.extraparam != 0) {
//      if (!isPrimitiveRef(variable.extraparam)) {
//         variable.element = variable.extraparam;
//         variable.extraparam = _logic->definePrimitiveArray(*scope.moduleScope, variable.element);
//      }
//      else scope.raiseError(errInvalidHint, node);      
//   }
//}
//
//void Compiler :: compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   SNode targetNode = node.firstChild(lxObjectMask);
//
//   writer.newBookmark();
//
//   bool immMode = true;
//   int localOffs = 0;
//   ObjectInfo loperand;
//   if (targetNode == lxExpression) {
//      immMode = false;
//
//      localOffs = scope.newLocal();
//
//      loperand = compileExpression(writer, targetNode, scope, 0);
//
//      writer.insertChild(0, lxLocal, localOffs);
//      writer.insert(lxAssigning, 0);
//      writer.closeNode();
//   }
//
//   SNode current = node.findChild(lxOption, lxElse);
//   while (current == lxOption) {
//      writer.newNode(lxOption);
//      writer.newNode(lxExpression);
//
//      writer.newBookmark();
//
//      writer.appendNode(lxBreakpoint, dsStep);
//
//      int operator_id = current.argument;
//
//      if (!immMode) {
//         writer.newNode(lxLocal, localOffs);
//         writer.appendNode(lxTarget, resolveObjectReference(scope, loperand));
//         writer.closeNode();
//      }
//      else loperand = compileExpression(writer, targetNode, scope, 0);
//
//      // find option value
//      SNode valueNode = current.firstChild(lxObjectMask);
//
//      ObjectInfo roperand = compileExpression(writer, valueNode.firstChild(lxObjectMask), scope, 0);
//
//      ObjectInfo operationInfo = compileOperator(writer, node, scope, operator_id, 1, loperand, roperand, ObjectInfo());
//
//      ObjectInfo retVal;
//      compileBranchingOperand(writer, valueNode, scope, HINT_SWITCH, IF_MESSAGE_ID, operationInfo, retVal);
//
//      writer.removeBookmark();
//      writer.closeNode();
//      writer.closeNode();
//
//      current = current.nextNode();
//   }
//
//   if (current == lxElse) {
//      CodeScope subScope(&scope);
//      SNode thenCode = current.findSubNode(lxCode);
//
//      writer.newNode(lxElse);
//
//      SNode statement = thenCode.firstChild(lxObjectMask);
//      if (statement.nextNode() != lxNone || statement == lxEOF) {
//         compileCode(writer, thenCode, subScope);
//      }
//      // if it is inline action
//      else compileRetExpression(writer, statement, scope, 0);
//
//      // preserve the allocated space
//      scope.level = subScope.level;
//
//      writer.closeNode();
//   }
//
//   writer.insert(lxSwitching);
//   writer.closeNode();
//
//   writer.removeBookmark();
//}
//
//void Compiler :: compileVariable(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   SNode terminal = node.findChild(lxIdentifier, lxPrivate, lxExpression);
//   if (terminal == lxExpression)
//      terminal = terminal.findChild(lxIdentifier, lxPrivate);
//
//   ident_t identifier = terminal.identifier();
//   if (scope.moduleScope->mapAttribute(terminal) != 0)
//      scope.raiseWarning(WARNING_LEVEL_3, wrnAmbiguousVariable, terminal);
//
//   if (!scope.locals.exist(identifier)) {
//      LexicalType variableType = lxVariable;
//      int variableArg = 0;
//      int size = 0;
//      ident_t className = NULL;
//
//      ObjectInfo variable(okLocal);
//      declareLocalAttributes(node, scope, variable, size);
//
//      ClassInfo localInfo;
//      bool bytearray = false;
//      if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.extraparam))
//         scope.raiseError(errUnknownVariableType, terminal);
//
//      if (variable.extraparam == V_BINARYARRAY && variable.element != 0) {
//         localInfo.size *= _logic->defineStructSize(*scope.moduleScope, variable.element, 0);
//      }
//
//      if (_logic->isEmbeddableArray(localInfo) && size != 0) {
//         bytearray = true;
//         size = size * (-((int)localInfo.size));
//      }
//      else if (_logic->isEmbeddable(localInfo) && size == 0) {
//         bool dummy = false;
//         size = _logic->defineStructSize(localInfo, dummy);
//      }
//      else if (size != 0)
//         scope.raiseError(errInvalidOperation, terminal);
//
//      if (size > 0) {
//         if (!allocateStructure(scope, size, bytearray, variable))
//            scope.raiseError(errInvalidOperation, terminal);
//
//         // make the reservation permanent
//         scope.saved = scope.reserved;
//
//         switch (localInfo.header.flags & elDebugMask)
//         {
//            case elDebugDWORD:
//               variableType = lxIntVariable;
//               break;
//            case elDebugQWORD:
//               variableType = lxLongVariable;
//               break;
//            case elDebugReal64:
//               variableType = lxReal64Variable;
//               break;
//            case elDebugIntegers:
//               variableType = lxIntsVariable;
//               variableArg = size;
//               break;
//            case elDebugShorts:
//               variableType = lxShortsVariable;
//               variableArg = size;
//               break;
//            case elDebugBytes:
//               variableType = lxBytesVariable;
//               variableArg = size;
//               break;
//            default:
//               if (isPrimitiveRef(variable.extraparam)) {
//                  variableType = lxBytesVariable;
//                  variableArg = size;
//               }
//               else {
//                  variableType = lxBinaryVariable;
//                  // HOTFIX : size should be provide only for dynamic variables
//                  if (bytearray)
//                     variableArg = size;
//
//                  if (variable.extraparam != 0) {
//                     className = scope.moduleScope->module->resolveReference(variable.extraparam);
//                  }
//               }
//               break;
//         }
//      }
//      else variable.param = scope.newLocal();
//
//      writer.newNode(variableType, variableArg);
//
//      writer.appendNode(lxLevel, variable.param);
//      writer.appendNode(lxIdentifier, identifier);
//      if (!emptystr(className))
//         writer.appendNode(lxClassName, className);
//
//      writer.closeNode();
//
//      scope.mapLocal(identifier, variable.param, variable.extraparam, variable.element, size);
//   }
//   else scope.raiseError(errDuplicatedLocal, terminal);
//}

void Compiler :: writeTerminalInfo(SyntaxWriter& writer, SNode terminal)
{
   SyntaxTree::copyNode(writer, lxRow, terminal);
   SyntaxTree::copyNode(writer, lxCol, terminal);
   SyntaxTree::copyNode(writer, lxLength, terminal);

   //ident_t ident = terminal.identifier();
   //if (ident)
   //   writer.appendNode(lxTerminal, terminal.identifier());
}

//inline void writeTarget(SyntaxWriter& writer, ref_t targetRef)
//{
//   if (targetRef)
//      writer.appendNode(lxTarget, targetRef);
//}
//
//int Compiler :: defineFieldSize(CodeScope& scope, int offset)
//{
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//
//   ClassInfo::FieldMap::Iterator it = retrieveIt(classScope->info.fields.start(), offset);
//   it++;
//   if (!it.Eof()) {
//      return *it - offset;
//   }
//   else return classScope->info.size - offset;
//}

void Compiler :: writeParamTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, int mode, LexicalType type)
{
   //if (object.element == -1) {
   //   ref_t targetRef = resolveObjectReference(scope, object);
   //   bool variable = false;
   //   int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, 0u, variable);
   //   writer.newNode((variable && !test(mode, HINT_NOUNBOXING)) ? lxUnboxing : lxCondBoxing, size);
   //   writer.appendNode(type, object.param);
   //   if (test(mode, HINT_DYNAMIC_OBJECT))
   //      writer.appendNode(lxBoxingRequired);
   //}
   /*else */writer.newNode(type, object.param);
}

void Compiler :: writeTerminal(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ObjectInfo object, int mode)
{
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
//         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         writer.newNode(lxSymbolReference, object.param);
         break;
      case okConstantClass:
         writer.newNode(lxConstantClass, object.param);
         break;
      case okConstantSymbol:
         writer.newNode(lxConstantSymbol, object.param);
         break;
//      case okLiteralConstant:
//         writer.newNode(lxConstantString, object.param);
//         break;
//      case okWideLiteralConstant:
//         writer.newNode(lxConstantWideStr, object.param);
//         break;
//      case okCharConstant:
//         writer.newNode(lxConstantChar, object.param);
//         break;
//      case okIntConstant:
//      case okUIntConstant:
//         writer.newNode(lxConstantInt, object.param);
//         writer.appendNode(lxIntValue, object.extraparam);
//         break;
//      case okLongConstant:
//         writer.newNode(lxConstantLong, object.param);
//         break;
//      case okRealConstant:
//         writer.newNode(lxConstantReal, object.param);
//         break;
//      case okArrayConst:
//         writer.newNode(lxConstantList, object.param);
//         break;
//      case okLocal:
      case okParam:
         writeParamTerminal(writer, scope, object, mode, lxLocal);
         break;
      case okSelfParam:
         writeParamTerminal(writer, scope, object, mode, lxSelfLocal);
         break;
//      case okSuper:
//         writer.newNode(lxLocal, 1);
//         break;
//      case okField:
//      case okOuter:
//         writer.newNode(lxField, object.param);
//         break;
//      case okStaticField:
//         if ((int)object.param < 0) {
//            // if it is a normal static field - field expression should be used
//            writer.newNode(lxFieldExpression, 0);
//            writer.appendNode(lxClassRefField, 1);
//            writer.appendNode(lxStaticField, object.param);
//         }
//         // if it is a sealed static field
//         else writer.newNode(lxStaticField, object.param);
//         break;
//      case okClassStaticField:
//         writer.newNode(lxFieldExpression, 0);
//         if (!object.param) {
//            writer.appendNode(lxThisLocal, 1);
//         }
//         else writer.appendNode(lxConstantClass, object.param);
//         writer.appendNode(lxStaticField, object.extraparam);
//         break;
//      case okStaticConstantField:
//         writer.newNode(lxFieldExpression, 0);
//         writer.appendNode(lxClassRefField, 1);
//         writer.appendNode(lxStaticConstField, object.param);
//         break;
//      case okClassStaticConstantField:
//         writer.newNode(lxFieldExpression, 0);
//         if (!object.param) {
//            writer.appendNode(lxThisLocal, 1);
//         }         
//         else writer.appendNode(lxConstantClass, object.param);
//         writer.appendNode(lxStaticConstField, object.extraparam);
//         break;
//      case okOuterField:
//         writer.newNode(lxFieldExpression, 0);
//         writer.appendNode(lxField, object.param);
//         writer.appendNode(lxResultField, object.extraparam);
//         break;
//      case okOuterStaticField:
//         writer.newNode(lxFieldExpression, 0);
//         writer.newNode(lxFieldExpression, 0);
//         writer.appendNode(lxField, object.param);
//         writer.appendNode(lxClassRefAttr, object.param);
//         writer.closeNode();         
//         writer.appendNode(lxStaticField, object.extraparam);
//         break;
//      case okSubject:
//         writer.newNode(lxBoxing, _logic->defineStructSize(*scope.moduleScope, scope.moduleScope->signatureReference, 0u));
//         writer.appendNode(lxLocalAddress, object.param);
//         writer.appendNode(lxTarget, scope.moduleScope->signatureReference);
//         break;
//      case okLocalAddress:
//      case okFieldAddress:
//      {
//         LexicalType type = object.kind == okLocalAddress ? lxLocalAddress : lxFieldAddress;
//         if (!test(mode, HINT_NOBOXING) || test(mode, HINT_DYNAMIC_OBJECT)) {
//
//            bool variable = false;
//            int size = _logic->defineStructSizeVariable(*scope.moduleScope, resolveObjectReference(scope, object), object.element, variable);
//            if (size < 0 && type == lxFieldAddress) {
//               // if it is fixed-size array
//               size = defineFieldSize(scope, object.param) * (-size);
//            }
//            writer.newNode((variable && !test(mode, HINT_NOUNBOXING)) ? lxUnboxing : lxBoxing, size);
//
//            writer.appendNode(type, object.param);
//            if (test(mode, HINT_DYNAMIC_OBJECT))
//               writer.appendNode(lxBoxingRequired);
//         }
//         else writer.newNode(type, object.param);
//         break;
//      }
      case okNil:
         writer.newNode(lxNil/*, object.param*/);
         break;
//      case okMessageConstant:
//         writer.newNode(lxMessageConstant, object.param);
//         break;
//      case okExtMessageConstant:
//         writer.newNode(lxExtMessageConstant, object.param);
//         break;
//      case okSignatureConstant:
//         writer.newNode(lxSignatureConstant, object.param);
//         break;
////      case okBlockLocal:
////         terminal.set(lxBlockLocal, object.param);
////         break;
//      case okParams:
//         writer.newNode(lxArgBoxing, 0);
//         writer.appendNode(lxBlockLocalAddr, object.param);
//         writer.appendNode(lxTarget, scope.moduleScope->arrayReference);
//         if (test(mode, HINT_DYNAMIC_OBJECT))
//            writer.appendNode(lxBoxingRequired);
//         break;
      case okObject:
         writer.newNode(lxResult, 0);
         break;
//      case okConstantRole:
//         writer.newNode(lxConstantSymbol, object.param);
//         break;
//      case okExternal:
//         return;
//      case okInternal:
//         writer.appendNode(lxInternalRef, object.param);
//         return;
   }

//   writeTarget(writer, resolveObjectReference(scope, object));

   if (!test(mode, HINT_NODEBUGINFO))
      writeTerminalInfo(writer, terminal);

   writer.closeNode();
}

ObjectInfo Compiler :: compileTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, int mode)
{
//   ident_t token = terminal.identifier();

   ObjectInfo object;
//   if(terminal == lxConstantList) {
//      // HOTFIX : recognize predefined constant lists
//      object = ObjectInfo(okArrayConst, terminal.argument, scope.moduleScope->arrayReference);
//   }
//   else if (terminal==lxLiteral) {
//      object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(token));
//   }
//   else if (terminal == lxWide) {
//      object = ObjectInfo(okWideLiteralConstant, scope.moduleScope->module->mapConstant(token));
//   }
//   else if (terminal==lxCharacter) {
//      object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(token));
//   }
//   else if (terminal == lxInteger) {
//      String<char, 20> s;
//
//      int integer = token.toInt();
//      if (errno == ERANGE)
//         scope.raiseError(errInvalidIntNumber, terminal);
//
//      // convert back to string as a decimal integer
//      s.appendHex(integer);
//
//      object = ObjectInfo(okIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
//   }
//   else if (terminal == lxLong) {
//      String<char, 30> s("_"); // special mark to tell apart from integer constant
//      s.append(token, getlength(token) - 1);
//
//      token.toULongLong(10, 1);
//      if (errno == ERANGE)
//         scope.raiseError(errInvalidIntNumber, terminal);
//
//      object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s));
//   }
//   else if (terminal == lxHexInteger) {
//      String<char, 20> s;
//
//      int integer = token.toULong(16);
//      if (errno == ERANGE)
//         scope.raiseError(errInvalidIntNumber, terminal);
//
//      // convert back to string as a decimal integer
//      s.appendHex(integer);
//
//      object = ObjectInfo(okUIntConstant, scope.moduleScope->module->mapConstant((const char*)s), integer);
//   }
//   else if (terminal == lxReal) {
//      String<char, 30> s(token, getlength(token) - 1);
//      token.toDouble();
//      if (errno == ERANGE)
//         scope.raiseError(errInvalidIntNumber, terminal);
//
//      // HOT FIX : to support 0r constant
//      if (s.Length() == 1) {
//         s.append(".0");
//      }
//
//      object = ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant((const char*)s));
//   }
//   else if (terminal == lxExplicitConst) {
//      // try to resolve explicit constant
//      size_t len = getlength(token);
//
//      IdentifierString singature(token + len - 1);
//      singature.append('$');
//      singature.append(scope.moduleScope->module->resolveReference(scope.moduleScope->literalReference));
//
//      ref_t postfixRef = scope.moduleScope->module->mapSubject(singature, false);
//
//      IdentifierString constant(token, len - 1);
//
//      object = ObjectInfo(okExplicitConstant, scope.moduleScope->module->mapConstant(constant), postfixRef);
//   }
////   else if (terminal == lxResult) {
////      object = ObjectInfo(okObject);
////   }
//   else if (terminal == lxMemberIdentifier) {
//      object = scope.mapMember(token.c_str() + 1);
//   }
//   else if (!emptystr(token))
      object = scope.mapObject(terminal);

//   if (test(mode, HINT_INQUIRY_MODE)) {
//      // assingment optimization - do nothing for an inqiry mode
//   }
//   else if (object.kind == okExplicitConstant) {
//      // replace an explicit constant with the appropriate object
//      writer.newBookmark();
//      writeTerminal(writer, terminal, scope, ObjectInfo(okLiteralConstant, object.param) , mode);
//
//      ref_t constRef = scope.moduleScope->actionHints.get(encodeMessage(object.extraparam, 1) | CONVERSION_MESSAGE);
//      if (constRef != 0) {
//         if (!convertObject(writer, *scope.moduleScope, constRef, V_STRCONSTANT, object.extraparam))
//            scope.raiseError(errInvalidConstant, terminal);
//      }
//      else scope.raiseError(errInvalidConstant, terminal);
//
//      object = ObjectInfo(okObject, constRef);
//
//      writer.removeBookmark();
//   }
   /*else */writeTerminal(writer, terminal, scope, object, mode);

   return object;
}

ObjectInfo Compiler :: compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

//   SNode member = objectNode.findChild(lxCode, lxNestedClass, lxMessageReference, lxExpression, lxLazyExpression, lxBoxing);
//   switch (member.type)
//   {
//      case lxNestedClass:
//      case lxLazyExpression:
//         result = compileClosure(writer, member, scope, mode & HINT_CLOSURE_MASK);
//         break;
//      case lxCode:
//         result = compileClosure(writer, objectNode, scope, mode & HINT_CLOSURE_MASK);
//         break;
//      case lxExpression:
//         if (isCollection(member)) {
//            result = compileCollection(writer, objectNode, scope);
//         }
//         else result = compileExpression(writer, member, scope, mode & HINT_CLOSURE_MASK);
//         break;
//      case lxBoxing:
//         result = compileExpression(writer, member, scope, mode);
//         break;
//      case lxMessageReference:
//         result = compileMessageReference(writer, member, scope, mode);
//         break;
//      default:
         result = compileTerminal(writer, objectNode, scope, mode);
//   }

   return result;
}

//ObjectInfo Compiler :: compileMessageReference(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
//{
//   SNode terminal = node.findChild(lxPrivate, lxIdentifier, lxLiteral);
//   IdentifierString signature;
//   int paramCount = 0;
//   ref_t extensionRef = 0;
//   if (terminal == lxIdentifier || terminal == lxPrivate) {
//      ident_t name = terminal.identifier();
//      signature.copy(name);
//
//      paramCount = -1;
//   }
//   else {
//      ident_t message = terminal.identifier();
//
//      int subject = message.find('.',0);
//      if (subject != 0) {
//         signature.copy(message, subject);
//         if (signature.ident().find('\'') != NOTFOUND_POS) {
//            // if it is a full reference
//            extensionRef = scope.moduleScope->mapReference(signature.ident(), true);
//         }
//         else extensionRef = scope.moduleScope->resolveImplicitIdentifier(signature);
//         if (extensionRef == 0)
//            scope.raiseError(errInvalidSubject, terminal);
//
//         subject++;
//      }
//
//      int param = 0;
//      for (size_t i = subject; i < getlength(message); i++) {
//         if (message[i] == '&') {
//         }
//         else if (message[i] == '[') {
//            int len = getlength(message);
//            if (message[i+1] == ']') {
//               paramCount = OPEN_ARG_COUNT;
//            }
//            else if (message[len - 1] == ']') {
//               signature.copy(message + i + 1, len - i - 2);
//               paramCount = signature.ident().toInt();
//               if (paramCount > MAX_ARG_COUNT)
//                  scope.raiseError(errInvalidSubject, terminal);
//            }
//            else scope.raiseError(errInvalidSubject, terminal);
//
//            param = i;
//            break;
//         }
//         else if (message[i] >= 65 || (message[i] >= 48 && message[i] <= 57)) {
//         }
//         else scope.raiseError(errInvalidConstant, terminal);
//      }
//
//      if (param != 0) {
//         signature.copy(message + subject, param - subject);
//      }
//      else signature.copy(message + subject);
//   }
//
//   ObjectInfo retVal;
//   IdentifierString message;
//   if (extensionRef != 0) {
//      message.append(scope.moduleScope->module->resolveReference(extensionRef));
//      message.append('.');
//   }
//
//   if (paramCount != -1) {
//      message.append('0' + (char)paramCount);
//      message.append(signature);
//   }
//   else message.copy(signature);
//
//   if (extensionRef != 0) {
//      retVal.kind = okExtMessageConstant;
//   }
//   else if (paramCount == -1) {
//      retVal.kind = okSignatureConstant;
//   }
//   else retVal.kind = okMessageConstant;
//
//   retVal.param = scope.moduleScope->module->mapReference(message);
//
//   writeTerminal(writer, node, scope, retVal, mode);
//
//   return retVal;
//}

ref_t Compiler :: mapMessage(SNode node, CodeScope& scope)
{
   ref_t actionFlags = 0;

//   IdentifierString signature;
   IdentifierString messageStr;

   SNode current = node;
//   if (arg.argument != 0)
//      return arg.argument;

   SNode name = current.findChild(/*lxPrivate, */lxIdentifier/*, lxReference*/);
//   //HOTFIX : if generated by a script / closure call
//   if (name == lxNone)
//      name = arg;

   if (name == lxNone)
      scope.raiseError(errInvalidOperation, node);

   messageStr.copy(name.identifier()); // !! temporal
//   ref_t verbRef = scope.mapSubject(name, messageStr);
//   if (verbRef) {
//      if (arg.nextNode() == lxNone) {
//         signature.append('$');
//         signature.append(scope.moduleScope->module->resolveReference(verbRef));
//      }
//      else scope.raiseError(errInvalidSubject, name);
//   }
//   else {
//      // COMPILER MAGIC : recognize set property
//      int verb_id = _verbs.get(messageStr.c_str());
//      if (verb_id == SET_MESSAGE_ID) {
//         actionFlags = PROPSET_MESSAGE;
//      }
//   }

   current = current.nextNode();

   int paramCount = 0;
   // if message has generic argument list
   while (test(current.type, lxObjectMask)) {
      //if (paramCount < OPEN_ARG_COUNT)
         paramCount++;

      current = current.nextNode();
   }

//   // if message has named argument list
//   bool first = messageStr.Length() == 0;
//
//   while (arg == lxMessage) {
//      ref_t elementRef = 0;
//      ref_t class_ref = declareArgumentSubject(arg, *scope.moduleScope, first, messageStr, signature, elementRef);
//
//      // skip an argument
//      arg = arg.nextNode();
//
//      if (test(arg.type, lxObjectMask)) {
//         // if it is an open argument list
//         if (class_ref == V_ARGARRAY) {
//            paramCount += OPEN_ARG_COUNT;
//
//            // if open argument list virtual subject is used - replace it with []
//            if (paramCount > MAX_ARG_COUNT)
//               scope.raiseError(errInvalidOperation, node);
//
//            if (elementRef != scope.moduleScope->superReference || paramCount > OPEN_ARG_COUNT) {
//               signature.append('$');
//               signature.append(scope.moduleScope->module->resolveReference(elementRef));
//            }
//            else signature.clear();
//
//            break;
//         }
//         else {
//            paramCount++;
//            if (paramCount >= OPEN_ARG_COUNT)
//               scope.raiseError(errInvalidOperation, node);
//
//            arg = arg.nextNode();
//         }
//      }
//   }

   if (paramCount > MAX_ARG_COUNT)
      scope.raiseError(errInvalidOperation, node);

//   if (actionFlags == PROPSET_MESSAGE && paramCount == 1) {
//      // COMPILER MAGIC : set&x => x
//      if (messageStr.Length() > 3) {
//         messageStr.cut(0, 4);
//      }
//      else if (messageStr.compare(SET_MESSAGE) && !emptystr(signature)) {
//         messageStr.clear();
//      }
//   }
//
//   messageStr.append(signature);

   // if signature is presented
   ref_t actionRef = scope.moduleScope->module->mapAction(messageStr, 0, false);

   // create a message id
   return encodeMessage(actionRef, paramCount) | actionFlags;
}

//ref_t Compiler :: mapExtension(ModuleScope& scope, SubjectMap* typeExtensions, ref_t& messageRef, ref_t implicitSignatureRef)
//{
//   auto it = typeExtensions->getIt(messageRef);
//   while (!it.Eof()) {
//      ref_t ref = *it;
//      ref_t resolvedMessage = _logic->resolveMultimethod(scope, messageRef, ref, implicitSignatureRef);
//      if (resolvedMessage) {
//         messageRef = resolvedMessage;
//
//         return ref;
//      }
//
//      it = typeExtensions->nextIt(messageRef, it);
//   }
//
//   return 0;
//}
//
//ref_t Compiler :: mapExtension(CodeScope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo object, bool& genericOne)
//{
//   // check typed extension if the type available
//   ref_t typeRef = 0;
//   ref_t extRef = 0;
//
//   ref_t objectRef = resolveObjectReference(scope, object);
//   if (_logic->isPrimitiveRef(objectRef)) {
//      if (objectRef != V_ARGARRAY)
//         objectRef = _logic->resolvePrimitiveReference(*scope.moduleScope, objectRef);
//   }
//   else if (objectRef == scope.moduleScope->superReference) {
//      objectRef = 0;
//   }
//
//   if (objectRef != 0 && scope.moduleScope->extensionHints.exist(messageRef, objectRef)) {
//      typeRef = objectRef;
//   }
//   else {
//      if (scope.moduleScope->extensionHints.exist(messageRef)) {
//         // if class reference available - select the possible type
//         if (objectRef != 0) {
//            SubjectMap::Iterator it = scope.moduleScope->extensionHints.start();
//            while (!it.Eof()) {
//               if (it.key() == messageRef) {
//                  if (_logic->isCompatible(*scope.moduleScope, *it, objectRef)) {
//                     typeRef = *it;
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
//   // try to resolve strong typed extension and strong typed message
//   if (implicitSignatureRef) {
//      if (typeRef != 0) {
//         extRef = mapExtension(*scope.moduleScope, scope.moduleScope->extensions.get(typeRef), messageRef, implicitSignatureRef);
//         if (extRef)
//            return extRef;
//      }
//
//      // if no match found - try to resolve general extension and strong typed message
//      extRef = mapExtension(*scope.moduleScope, scope.moduleScope->extensions.get(0), messageRef, implicitSignatureRef);
//      if (extRef)
//         return extRef;
//   }
//
//   // if no match found - try to resolve strong typed extension and genral message
//   if (typeRef != 0) {
//      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(typeRef);
//
//      if (typeExtensions)
//         extRef = typeExtensions->get(messageRef);
//   }
//
//   // if no match found - try to resolve general extension and general message
//   if (extRef == 0) {
//      SubjectMap* typeExtensions = scope.moduleScope->extensions.get(0);
//
//      if (typeExtensions) {
//         extRef = typeExtensions->get(messageRef);
//         if (extRef != 0)
//            genericOne = true;
//      }
//   }
//
//   return extRef;
//}
//
//void Compiler :: compileBranchingNodes(SyntaxWriter& writer, SNode thenBody, CodeScope& scope, ref_t ifReference, bool loopMode, bool switchMode)
//{
//   if (loopMode) {
//      writer.newNode(lxElse, ifReference);
//
//      compileBranching(writer, thenBody.findSubNode(lxCode), scope);
//      writer.closeNode();
//   }
//   else {
//      SNode thenCode = thenBody.findSubNode(lxCode);
//
//      writer.newNode(lxIf, ifReference);
//      compileBranching(writer, thenCode, scope);
//      writer.closeNode();
//
//      // HOTFIX : switch mode - ignore else
//      if (!switchMode) {
//         SNode elseCode = thenBody.firstChild().nextNode();
//         if (elseCode != lxNone) {
//            writer.newNode(lxElse, 0);
//            compileBranching(writer, elseCode, scope);
//            writer.closeNode();
//         }
//      }
//   }
//}
//
//void Compiler :: compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal)
//{
//   bool loopMode = test(mode, HINT_LOOP);
//   bool switchMode = test(mode, HINT_SWITCH);
//
//   // HOTFIX : in loop expression, else node is used to be similar with branching code
//   // because of optimization rules
//   ref_t original_id = operator_id;
//   if (loopMode) {
//      operator_id = operator_id == IF_MESSAGE_ID ? IFNOT_MESSAGE_ID : IF_MESSAGE_ID;
//   }
//
//   ref_t ifReference = 0;
//   ref_t resolved_operator_id = operator_id;
//   // try to resolve the branching operator directly
//   if (_logic->resolveBranchOperation(*scope.moduleScope, *this, resolved_operator_id, resolveObjectReference(scope, loperand), ifReference)) {
//      // good luck : we can implement branching directly
//      compileBranchingNodes(writer, roperandNode, scope, ifReference, loopMode, switchMode);
//
//      writer.insert(loopMode ? lxLooping : lxBranching, switchMode ? -1 : 0);
//      writer.closeNode();
//   }
//   else {
//      operator_id = original_id;
//
//      // bad luck : we have to create closure
//      compileObject(writer, roperandNode, scope, HINT_SUBCODE_CLOSURE);
//
//      SNode roperand2Node = roperandNode.firstChild() == lxExpression ? roperandNode.findChild(lxCode) : SNode();
//      if (roperand2Node != lxNone) {
//         // HOTFIX : else sub code is located in the first expression, while if one - in second layer expression
//         compileClosure(writer, roperandNode, scope, HINT_SUBCODE_CLOSURE);
//
//         //compileObject(writer, roperand2Node, scope, HINT_SUBCODE_CLOSURE);
//
//         retVal = compileMessage(writer, roperandNode, scope, loperand, encodeMessage(operator_id, 2), 0);
//      }
//      else retVal = compileMessage(writer, roperandNode, scope, loperand, encodeMessage(operator_id, 1), 0);
//
//      if (loopMode) {
//         writer.insert(lxLooping);
//         writer.closeNode();
//      }
//   }
//}
//
//ObjectInfo Compiler :: compileBranchingOperator(SyntaxWriter& writer, SNode& node, CodeScope& scope, int mode, int operator_id)
//{
//   ObjectInfo retVal(okObject);
//
//   writer.newBookmark();
//
//   SNode loperandNode = node.firstChild(lxObjectMask);
//   ObjectInfo loperand = compileExpression(writer, loperandNode, scope, 0);
//
//   SNode roperandNode = loperandNode.nextNode(lxObjectMask);
//
//   compileBranchingOperand(writer, roperandNode, scope, mode, operator_id, loperand, retVal);
//
//   writer.removeBookmark();
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2)
//{
//   ObjectInfo retVal;
//
//   ref_t loperandRef = resolveObjectReference(scope, loperand);
//   ref_t roperandRef = resolveObjectReference(scope, roperand);
//   ref_t roperand2Ref = 0;
//   ref_t resultClassRef = 0;
//   int operationType = 0;
//
//   if (roperand2.kind != okUnknown) {
//      roperand2Ref = resolveObjectReference(scope, roperand2);
//      //HOTFIX : allow to work with int constants
//      if (roperand2.kind == okIntConstant && loperandRef == V_OBJARRAY)
//         roperand2Ref = 0;
//
//      operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, roperand2Ref, resultClassRef);
//
//      if (roperand2Ref == V_NIL && loperandRef == V_INT32ARRAY && operator_id == SET_REFER_MESSAGE_ID) {
//         //HOTFIX : allow set operation with $nil
//         operator_id = SETNIL_REFER_MESSAGE_ID;
//      }
//   }
//   else operationType = _logic->resolveOperationType(*scope.moduleScope, *this, operator_id, loperandRef, roperandRef, resultClassRef);
//
//   // HOTFIX : primitive operations can be implemented only in the method
//   // because the symbol implementations do not open a new stack frame
//   if (operationType != 0 && resultClassRef != V_FLAG && scope.getScope(Scope::slMethod) == NULL) {
//      operationType = 0;
//   }
//
//   //bool assignMode = false;
//   if (operationType != 0) {
//      // if it is a primitive operation
//      _logic->injectOperation(writer, *scope.moduleScope, *this, operator_id, operationType, resultClassRef, loperand.element);
//
//      retVal = assignResult(writer, scope, resultClassRef/*, loperand.element*/);
//   }
//   // if not , replace with appropriate method call
//   else retVal = compileMessage(writer, node, scope, loperand, encodeMessage(operator_id, paramCount), HINT_NODEBUGINFO);
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int, int operator_id)
//{
//   ObjectInfo retVal(okObject);
//   int paramCount = 1;
//
//   writer.newBookmark();
//   writer.appendNode(lxOperatorAttr);
//
//   ObjectInfo loperand;
//   ObjectInfo roperand;
//   ObjectInfo roperand2;
//   if (operator_id == SET_REFER_MESSAGE_ID) {
//      SNode exprNode = node.firstChild(lxObjectMask);
//
//      // HOTFIX : inject the third parameter to the first two ones
//      SNode thirdNode = exprNode.nextNode(lxObjectMask);
//      SyntaxTree::copyNodeSafe(exprNode.nextNode(lxObjectMask), exprNode.appendNode(lxExpression), true);
//
//      exprNode.nextNode(lxObjectMask) = lxIdle;
//
//      SNode loperandNode = exprNode.firstChild(lxObjectMask);
//      loperand = compileExpression(writer, loperandNode, scope, 0);
//
//      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
//      roperand = compileExpression(writer, roperandNode, scope, 0);
//      roperand2 = compileExpression(writer, roperandNode.nextNode(lxObjectMask), scope, 0);
//
//      node = exprNode;
//
//      paramCount++;
//   }
//   else {
//      SNode loperandNode = node.firstChild(lxObjectMask);
//      loperand = compileExpression(writer, loperandNode, scope, 0);
//
//      SNode roperandNode = loperandNode.nextNode(lxObjectMask);
//      /*if (roperandNode == lxLocal) {
//         // HOTFIX : to compile switch statement
//         roperand = ObjectInfo(okLocal, roperandNode.argument);
//      }
//      else */roperand = compileExpression(writer, roperandNode, scope, 0);
//   }
//
//   retVal = compileOperator(writer, node, scope, operator_id, paramCount, loperand, roperand, roperand2);
//
//   writer.removeBookmark();
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
//{
//   SNode operatorNode = node.findChild(lxOperator);
//   int operator_id = operatorNode.argument != 0 ? operatorNode.argument : _operators.get(operatorNode.identifier());
//
//   // if it is branching operators
//   if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
//      return compileBranchingOperator(writer, node, scope, mode, operator_id);
//   }
//   else return compileOperator(writer, node, scope, mode, operator_id);
//}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode)
{
   ObjectInfo retVal(okObject);

   LexicalType operation = lxCalling;
   int argument = messageRef;

//   ref_t actionRef;
//   int paramCount;
//   decodeMessage(messageRef, actionRef, paramCount);

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target);
//   bool dispatchCall = false;
   _CompilerLogic::ChechMethodInfo result;
   int callType = _logic->resolveCallType(*scope.moduleScope, classReference, messageRef, result);
   if (result.found) {
      retVal.param = result.outputReference;
   }

//   if (target.kind == okThisParam && callType == tpPrivate) {
//      messageRef |= SEALED_MESSAGE;
//
//      callType = tpSealed;
//   }
//   else if (classReference == scope.moduleScope->signatureReference) {
//      dispatchCall = test(mode, HINT_EXTENSION_MODE);
//   }
//   else if (classReference == scope.moduleScope->messageReference) {
//      dispatchCall = test(mode, HINT_EXTENSION_MODE);
//   }
//   else if (target.kind == okSuper) {
//      // parent methods are always sealed
//      callType = tpSealed;
//   }
//
//   if (dispatchCall) {
//      operation = lxDirectCalling;
//      argument = encodeVerb(DISPATCH_MESSAGE_ID);
//
//      writer.appendNode(lxOvreriddenMessage, messageRef);
//   }
   /*else */if (callType == tpClosed || callType == tpSealed) {
      operation = callType == tpClosed ? lxSDirctCalling : lxDirectCalling;
      argument = messageRef;
//      if (result.withOpenArgDispatcher) {
//         argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT);
//      }
//      else if (result.withOpenArg1Dispatcher) {
//         argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT + 1);
//      }
//      else if (result.withOpenArg2Dispatcher) {
//         argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT + 2);
//      }
//
//      if (result.stackSafe && target.kind != okParams && !test(mode, HINT_DYNAMIC_OBJECT))
//         writer.appendNode(lxStacksafeAttr);
//
//      if (result.embeddable)
//         writer.appendNode(lxEmbeddableAttr);
   }
//   else {
//      // if the sealed / closed class found and the message is not supported - warn the programmer and raise an exception
//      if (result.found && !result.withCustomDispatcher && callType == tpUnknown && result.directResolved) {
//         if (test(mode, HINT_ASSIGNING_EXPR)) {
//            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxExpression).findChild(lxMessage));
//         }
//         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxMessage, lxOperator));
//      }         
//   }
//
//   if (result.closure)
//      writer.appendNode(lxClosureAttr);

   if (classReference)
      writer.appendNode(lxCallTarget, classReference);

//   if (result.outputReference)
//      writer.appendNode(lxTarget, result.outputReference);

   if (!test(mode, HINT_NODEBUGINFO)) {
      // set a breakpoint
      writer.newNode(lxBreakpoint, dsStep);

      SNode messageNode = node.findChild(lxIdentifier, lxPrivate);
      if (messageNode != lxNone) {
         writeTerminalInfo(writer, messageNode);
      }

      writer.closeNode();
   }

//   // define the message target if required
//   if (target.kind == okConstantRole || target.kind == okSubject) {
//      writer.newNode(lxOverridden);
//      writer.newNode(lxExpression);
//      writeTerminal(writer, node, scope, target, 0);
//      writer.closeNode();
//      writer.closeNode();
//   }
//
//   // the result of get&type message should be typed
//
//   if (retVal.param == 0 && paramCount == 0 && actionRef != 0) {
//      ident_t sign = scope.moduleScope->module->resolveSubject(actionRef);
//      size_t index = sign.find('$');
//      if (index != NOTFOUND_POS && sign[index+1] != 0)
//         retVal.param = scope.moduleScope->module->mapReference(sign.c_str() + index + 1);
//
//      writer.appendNode(lxTarget, retVal.param);
//   }

   // inserting calling expression
   writer.insert(operation, argument);
   writer.closeNode();

   return retVal;
}

bool Compiler :: convertObject(SyntaxWriter& writer, Scope& scope, ref_t targetRef, ref_t sourceRef/*, ref_t elementRef*/)
{
   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
      // if it can be boxed / implicitly converted
      //if (!_logic->injectImplicitConversion(writer, scope, *this, targetRef, sourceRef, elementRef))
         return typecastObject(writer, scope, targetRef);
   }
   return true;
}

bool Compiler :: typecastObject(SyntaxWriter& writer, Scope& scope, ref_t targetRef)
{
//   if (targetRef != 0 && !isPrimitiveRef(targetRef)) {
//      IdentifierString sign;
//      sign.append('$');
//      sign.append(scope.module->resolveReference(targetRef));
//
//      writer.insert(lxCalling, encodeMessage(scope.module->mapSubject(sign, false), 0));
//      writer.closeNode();
//
//      return true;
//   }
   /*else */return false;
}

//void Compiler :: resolveStrongArgument(CodeScope& scope, ObjectInfo info, bool& anonymous, IdentifierString& signature)
//{
//   ref_t argRef = resolveObjectReference(scope, info);
//   if (isPrimitiveRef(argRef))
//      argRef = _logic->resolvePrimitiveReference(*scope.moduleScope, argRef);
//
//   if (!anonymous && argRef != 0) {
//      signature.append('$');
//      signature.append(scope.moduleScope->module->resolveReference(argRef));
//   }
//   else anonymous = true;
//}
//
//ref_t Compiler :: resolveStrongArgument(CodeScope& scope, ObjectInfo info)
//{
//   bool anonymous = false;
//   IdentifierString signature;
//   resolveStrongArgument(scope, info, anonymous, signature);
//   if (!anonymous) {
//      return scope.moduleScope->module->mapSubject(signature.ident(), false);
//   }
//   else return 0;
//}

/*ObjectInfo*/void Compiler :: compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope/*, ref_t& signatureRef, int mode*/)
{
//   ObjectInfo target;
//   bool anonymous = false;
//   IdentifierString signature;
//
//   if (test(mode, HINT_PARAMETERSONLY))
//      target = ObjectInfo(okObject);

   int paramMode = 0;

   SNode current = node;
//   if (test(mode, HINT_RESENDEXPR) && (arg == lxMessage && arg.nextNode() != lxMessage)) {
//      arg = arg.nextNode();
//   }

   // compile the message target and generic argument list
   while (/*current != lxMessage && */current != lxNone) {
      if (test(current.type, lxObjectMask)) {
//         if (target.kind == okUnknown) {
//            // HOTFIX : to handle alt expression
//            if (arg.type == lxLocal) {
//               target = ObjectInfo(okLocal, arg.argument);
//               writeTerminal(writer, arg, scope, target, 0);
//            }
//            // HOTFIX : to handle resend expression / explcit strong extension
//            else if (arg.type == lxResult) {
//               target = ObjectInfo(okObject);
//            }
//            else target = compileExpression(writer, arg, scope, paramMode);
//
//            // HOTFIX : recognize external operation
//            if (target.kind == okExternal) {
//               paramMode |= HINT_EXTERNALOP;
//            }
//
//            // HOTFIX : skip the prime message
//            arg = arg.nextNode();
//
//            if (test(mode, HINT_RESENDEXPR)) {
//               // HOTFIX : support several generic arguments for try mode
//               if (arg == lxExpression)
//                  continue;
//            }
//            else if (arg == lxExtension)
//               // HOTFIX : skip the extension node
//               arg = arg.nextNode();
//         }
//         // try to recognize the message signature
/*         else *//*resolveStrongArgument(scope, */compileExpression(writer, current, scope, 0, paramMode)/*, anonymous, signature)*/;
      }

      current = current.nextNode();
   }

//   // resend expression starts with the message, so it should be skipped
//   if (test(mode, HINT_RESENDEXPR))
//      arg = arg.nextNode();
//
//   // if message has named argument list
//   while (arg == lxMessage) {
//      bool openArg = arg.existChild(lxSize);
//      SNode subject = arg.findChild(lxPrivate, lxIdentifier, lxReference);
//      //HOTFIX : if generated by a script
//      if (subject == lxNone)
//         subject = arg;
//
//      ref_t classRef = scope.mapSubject(subject);
//      if (classRef)
//         anonymous = true;
//
//      arg = arg.nextNode();
//
//      // compile an argument
//      if (test(arg.type, lxObjectMask)) {
//         // if it is an open argument list
//         if (openArg) {
//            if (arg == lxExpression) {
//               SNode argListNode = arg.firstChild();
//
//               while (argListNode != lxNone) {
//                  writer.newBookmark();
//                  ObjectInfo param = compileExpression(writer, argListNode, scope, paramMode);
//                  if (classRef != 0 && classRef != scope.moduleScope->superReference) {
//                     if (!convertObject(writer, *scope.moduleScope, classRef, resolveObjectReference(scope, param, classRef), param.element))
//                        scope.raiseError(errInvalidOperation, arg);
//                  }
//                  writer.removeBookmark();
//
//                  argListNode = argListNode.nextNode();
//               }
//            }
//            else {
//               writer.newBookmark();
//               ObjectInfo argListParam = compileExpression(writer, arg, scope, paramMode);
//               if (argListParam.kind == okParams) {
//                  writer.insert(lxArgUnboxing);
//                  writer.closeNode();
//               }
//               writer.removeBookmark();
//            }
//         }
//         else {
//            writer.newBookmark();
//
//            int exprMode = paramMode;
//            // HOTFIX : generic parameter object MUST be boxed
//            if (classRef == 0)
//               exprMode |= HINT_DYNAMIC_OBJECT;
//
//            ObjectInfo param = compileExpression(writer, arg, scope, exprMode);
//            if (classRef != 0) {
//               if (!convertObject(writer, *scope.moduleScope, classRef, resolveObjectReference(scope, param, classRef), param.element))
//                  scope.raiseError(errInvalidOperation, arg);
//            }
//            else if (!anonymous)
//               resolveStrongArgument(scope, param, anonymous, signature);
//
//            // HOTFIX : externall operation arguments should be inside expression node
//            if (test(paramMode, HINT_EXTERNALOP)) {
//               writer.appendNode(lxExtArgumentRef, classRef);
//
//               writer.insert(lxExpression);
//               writer.closeNode();
//            }
//
//            writer.removeBookmark();
//
//            arg = arg.nextNode();
//         }
//      }
//   }
//
//   if (!anonymous && !emptystr(signature)) {
//      signatureRef = scope.moduleScope->module->mapSubject(signature.ident(), false);
//   }
//   return target;
}

//ref_t Compiler :: resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef, 
//   bool withExtension, bool& genericOne)
//{
//   ref_t resolvedMessageRef = 0;
//   ref_t targetRef = resolveObjectReference(scope, target);
//   ref_t extensionRef = 0;
//
//   resolvedMessageRef = _logic->resolveMultimethod(*scope.moduleScope, generalMessageRef, targetRef, implicitSignatureRef);
//
//   if (resolvedMessageRef != 0) {
//      // if the object handles the compile-time resolved message - use it
//      return resolvedMessageRef;
//   }
//
//   if (withExtension) {
//      // check the existing extensions if allowed
//      if (checkMethod(*scope.moduleScope, targetRef, generalMessageRef) != tpUnknown) {
//         // if the object handles the general message - use it
//         return generalMessageRef;
//      }
//
//      if (implicitSignatureRef) {
//         extensionRef = mapExtension(scope, generalMessageRef, implicitSignatureRef, target, genericOne);
//         if (extensionRef != 0) {
//            // if there is an extension to handle the compile-time resolved message - use it
//            target = ObjectInfo(okConstantRole, extensionRef/*, 0, target.type*/);
//
//            return generalMessageRef;
//         }
//      }
//
//      extensionRef = mapExtension(scope, generalMessageRef, 0, target, genericOne);
//      if (extensionRef != 0) {
//         // if there is an extension to handle the general message - use it
//         target = ObjectInfo(okConstantRole, extensionRef/*, 0, target.type*/);
//
//         return generalMessageRef;
//      }
//   }
//
//   // otherwise - use the general message
//   return generalMessageRef;
//}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode)
{
//   ref_t implicitSignatureRef = 0;
//   size_t paramCount = 0;
   ObjectInfo retVal;
   compileMessageParameters(writer, node, scope/*, implicitSignatureRef, mode & (HINT_RESENDEXPR | HINT_PARAMETERSONLY)*/);
//
//   if (test(mode, HINT_TRY_MODE)) {
//      writer.insertChild(0, lxResult, 0);
//
//      target = ObjectInfo(okObject);
//   }
//   else if (test(mode, HINT_ALT_MODE)) {
//      SNode targetNode = node.parentNode().firstChild(lxObjectMask).firstChild(lxObjectMask);
//      if (targetNode == lxLocal) {
//         writer.insertChild(0, targetNode.type, targetNode.argument);
//      }
//      else writer.insertChild(0, lxCurrent, 0);
//
//      target = ObjectInfo(okObject);
//   }
//   else if (test(mode, HINT_RESENDEXPR)) {
//      if (test(mode, HINT_EXT_RESENDEXPR)) {
//         //HOTFIX : fixing resending an extension message
//         writer.insertChild(0, lxLocal, (ref_t)-1);
//      }
//      else writer.insertChild(0, lxThisLocal, 1);
//
//      target = ObjectInfo(okThisParam, 1);
//   }
//
//   //   bool externalMode = false;
//   if (target.kind == okExternal) {
//      retVal = compileExternalCall(writer, node, scope);
//   }
//   else {
      ref_t messageRef = mapMessage(node, scope/*, paramCount*/);
//
//      if (target.kind == okInternal) {
//         retVal = compileInternalCall(writer, node, scope, messageRef, target);
//      }
//      else {
//         bool genericOne = false;
//         messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, true, genericOne);
//
//         if (genericOne)
//            mode |= HINT_DYNAMIC_OBJECT;

         retVal = compileMessage(writer, node, scope, target, messageRef, mode);
//      }
//   }

   return retVal;
}

//void Compiler :: inheritClassConstantList(ModuleScope& scope, ref_t sourceRef, ref_t targetRef)
//{
//   _Module* parent = scope.loadReferenceModule(sourceRef);
//
//   _Memory* source = parent->mapSection(sourceRef | mskRDataRef, true);
//   _Memory* target = scope.module->mapSection(targetRef | mskRDataRef, false);
//
//   MemoryReader reader(source);
//   MemoryWriter writer(target);
//
//   writer.read(&reader, source->Length());
//
//   _ELENA_::RelocationMap::Iterator it(source->getReferences());
//   ref_t currentMask = 0;
//   ref_t currentRef = 0;
//   while (!it.Eof()) {
//      currentMask = it.key() & mskAnyRef;
//      currentRef = it.key() & ~mskAnyRef;
//
//      target->addReference(importReference(parent, currentRef, scope.module) | currentMask, *it);
//
//      it++;
//   }
//}
//
//ObjectInfo Compiler :: compileAssigningClassConstant(SyntaxWriter& writer, SNode targetNode, CodeScope& scope, ObjectInfo retVal)
//{
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//   bool accumulatorMode = false;
//
//   if (scope.getMessageID() == (encodeVerb(NEWOBJECT_MESSAGE_ID) | CONVERSION_MESSAGE) && classScope != NULL) {
//      SymbolScope constantScope(scope.moduleScope, classScope->info.staticValues.get(retVal.param) & ~mskAnyRef);
//
//      SNode sourceNode = targetNode.nextNode(lxObjectMask);
//      SNode operatorNode = sourceNode.findSubNode(lxOperator);
//      if (operatorNode != lxNone && operatorNode.identifier().compare("+")) {
//         SNode firstNode = sourceNode.findSubNodeMask(lxObjectMask);
//         ObjectInfo info = scope.mapObject(firstNode);
//         if (info.kind == retVal.kind && info.param == retVal.param) {
//            // HOTFIX : support accumulating attribute list
//            ClassInfo parentInfo;
//            scope.moduleScope->loadClassInfo(parentInfo, classScope->info.header.parentRef);
//            ref_t parentListRef = parentInfo.staticValues.get(retVal.param) & ~mskAnyRef;
//
//            if (parentListRef != 0) {
//               // inherit the parent list
//               inheritClassConstantList(*scope.moduleScope, parentListRef, constantScope.reference);
//            }
//
//            accumulatorMode = true;
//            sourceNode = operatorNode.nextNode();
//         }
//      }
//      ObjectInfo source = compileAssigningExpression(writer, sourceNode, scope);
//      ref_t targetRef = accumulatorMode ? retVal.element : retVal.extraparam;
//      ref_t sourceRef = resolveConstantObjectReference(scope, source);
//
//      if (compileSymbolConstant(sourceNode, constantScope, source, accumulatorMode) && _logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
//      }
//      else scope.raiseError(errInvalidOperation, targetNode);
//   }
//   else scope.raiseError(errInvalidOperation, targetNode);
//
//   return ObjectInfo();
//}
//
//bool Compiler :: resolveAutoType(ObjectInfo source, ObjectInfo& target, CodeScope& scope)
//{
//   ref_t sourceRef = resolveObjectReference(scope, source);
//
//   if (isPrimitiveRef(sourceRef))
//      sourceRef = _logic->resolvePrimitiveReference(*scope.moduleScope, sourceRef);
//
//   return scope.resolveAutoType(target, sourceRef, source.element);
//}
//
//ObjectInfo Compiler :: compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
//{
//   writer.newBookmark();
//
//   ObjectInfo retVal;
//   LexicalType operationType = lxAssigning;
//   int operand = 0;
//
//   SNode exprNode = node;
//   SNode operation = node.findChild(lxMessage, lxExpression, lxAssign);
//   while (operation == lxExpression) {
//      exprNode = operation;
//      operation = exprNode.findChild(lxMessage, lxOperator, lxExpression);
//   }
//
//   if (operation == lxMessage) {
//      // try to figure out if it is property assignging
//      SNode firstToken = exprNode.firstChild(lxObjectMask);
//      ObjectInfo tokenInfo = scope.mapObject(firstToken);
//      //ref_t attrRef = scope.mapSubject(firstToken);
//      if (tokenInfo.kind != okUnknown) {
//         IdentifierString signature;
//
//         // if it is shorthand property settings
//         SNode name = operation.findChild(lxIdentifier, lxPrivate);
//
//         ref_t classRef = scope.mapSubject(name, signature);
//         if (classRef) {
//            signature.append('$');
//            signature.append(scope.moduleScope->module->resolveReference(classRef));
//         }
//
//         ref_t messageRef = encodeMessage(scope.moduleScope->module->mapSubject(signature, false), 1) | PROPSET_MESSAGE;
//
//         // compile target
//         // NOTE : compileMessageParameters does not compile the parameter, it'll be done in the next statement
//         ref_t dummy = 0;
//         ObjectInfo target = compileMessageParameters(writer, exprNode, scope, dummy);
//
//         // compile the parameter
//         SNode sourceNode = exprNode.nextNode(lxObjectMask);
//         ObjectInfo source = compileExpression(writer, sourceNode, scope, (classRef != 0) ? 0 : HINT_DYNAMIC_OBJECT);
//
//         messageRef = resolveMessageAtCompileTime(target, scope, messageRef, resolveStrongArgument(scope, source));
//
//         retVal = compileMessage(writer, node, scope, target, messageRef, HINT_NODEBUGINFO | HINT_ASSIGNING_EXPR);
//
//         operationType = lxNone;
//      }
//      else scope.raiseError(errUnknownObject, firstToken);
//   }
//   // if it setat operator
//   else if (operation == lxOperator) {
//      retVal = compileOperator(writer, node, scope, mode, SET_REFER_MESSAGE_ID);
//
//      operationType = lxNone;
//   }
//   else {
//      SNode targetNode = node.firstChild(lxObjectMask);
//      if (scope.isInitializer()) {
//         // HOTFIX : recognize static field initializer
//         retVal = compileExpression(writer, targetNode, scope, HINT_INQUIRY_MODE);
//         if (retVal.kind == okStaticField) {
//            // HOTFIX : static field initializer should be compiled as preloaded symbol
//            compileStaticAssigning(retVal, targetNode.nextNode(lxObjectMask), *((ClassScope*)scope.getScope(Scope::slClass)));
//
//            writer.removeBookmark();
//
//            return ObjectInfo();
//         }
//         else retVal = compileExpression(writer, targetNode, scope, mode | HINT_NOBOXING);
//      }
//      else retVal = compileExpression(writer, targetNode, scope, mode | HINT_NOBOXING);
//
//      ref_t targetRef = resolveObjectReference(scope, retVal);
//      if (retVal.kind == okLocalAddress) {
//         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
//         if (size != 0) {
//            operand = size;
//         }
//         else scope.raiseError(errInvalidOperation, node);
//      }
//      else if (retVal.kind == okFieldAddress) {
//         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
//         if (size != 0) {
//            operand = size;
//         }
//         else scope.raiseError(errInvalidOperation, node);
//      }
//      else if (retVal.kind == okLocal || retVal.kind == okField || retVal.kind == okOuterField || retVal.kind == okStaticField || retVal.kind == okOuterStaticField) {
//      }
//      else if (/*retVal.kind == okParam || */retVal.kind == okOuter) {
//         InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);
//
//         if (!closure->markAsPresaved(retVal))
//            scope.raiseError(errInvalidOperation, node);
//      }
//      else if (retVal.kind == okStaticConstantField) {
//         // HOTFIX : static constant in-place assigning
//         retVal = compileAssigningClassConstant(writer, targetNode, scope, retVal);
//
//         writer.trim();
//         writer.removeBookmark();
//
//         return retVal;
//      }
//      else scope.raiseError(errInvalidOperation, node);
//
//      writer.newBookmark();
//
//      SNode sourceNode = targetNode.nextNode(lxObjectMask);
//      ObjectInfo source = compileAssigningExpression(writer, sourceNode, scope);
//
//      // support auto attribute
//      if (targetRef == V_AUTO) {
//         if (resolveAutoType(source, retVal, scope)) {
//            targetRef = resolveObjectReference(scope, retVal);
//         }
//         else scope.raiseError(errInvalidOperation, node);
//      }
//
//      if (!convertObject(writer, *scope.moduleScope, targetRef, resolveObjectReference(scope, source, targetRef), source.element))
//         scope.raiseError(errInvalidOperation, node);
//
//      writer.removeBookmark();
//   }
//
//   if (operationType != lxNone) {
//      writer.insert(operationType, operand);
//      writer.closeNode();
//   }
//
//   writer.removeBookmark();
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   ref_t extensionRef = 0;
//
//   writer.newBookmark();
//
//   ModuleScope* moduleScope = scope.moduleScope;
//   ObjectInfo   role;
//
//   SNode roleNode = node.findChild(lxExtension);
//   // check if the extension can be used as a static role (it is constant)
//   SNode roleTerminal = roleNode.firstChild(lxTerminalMask);
//   if (roleTerminal != lxNone) {
//      int flags = 0;
//
//      role = scope.mapObject(roleTerminal);
//      if (role.kind == okSymbol || role.kind == okConstantSymbol) {
//         ref_t classRef = role.kind == okConstantSymbol ? role.extraparam : role.param;
//
//         // if the symbol is used inside itself
//         if (classRef == scope.getClassRefId()) {
//            flags = scope.getClassFlags();
//         }
//         // otherwise
//         else {
//            ClassInfo roleClass;
//            moduleScope->loadClassInfo(roleClass, moduleScope->module->resolveReference(classRef));
//
//            flags = roleClass.header.flags;
//            //HOTFIX : typecast the extension target if required
//            if (test(flags, elExtension) && roleClass.fieldTypes.exist(-1)) {
//               extensionRef = roleClass.fieldTypes.get(-1).value1;
//            }
//         }
//      }
//      // if the symbol VMT can be used as an external role
//      if (test(flags, elStateless)) {
//         role = ObjectInfo(okConstantRole, role.param);
//      }
//   }
//
//   // if it is a generic role
//   if (role.kind != okConstantRole && role.kind != okSubject) {
//      writer.newNode(lxOverridden);
//      role = compileExpression(writer, roleNode, scope, 0);
//      writer.closeNode();
//   }
//
//   ObjectInfo retVal = compileExtensionMessage(writer, node, scope, role, extensionRef);
//
//   writer.removeBookmark();
//
//   return retVal;
//}
//
//// NOTE : targetRef refers to the type for the typified extension method
//ObjectInfo Compiler :: compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo role, ref_t targetRef)
//{
//   size_t paramCount = 0;
//   ref_t  messageRef = mapMessage(node, scope, paramCount);
//   ref_t implicitSignatureRef = 0;
//
//   ObjectInfo object;
//   if (targetRef != 0) {
//      //HOTFIX : to compile strong typed explicit extension
//      writer.newBookmark();
//      SNode targetNode = node.firstChild(lxObjectMask);
//
//      object = compileExpression(writer, targetNode, scope, 0);
//
//      convertObject(writer, *scope.moduleScope, targetRef, resolveObjectReference(scope, object), object.element);
//      writer.removeBookmark();
//
//      // the target node already compiler so it should be skipped
//      targetNode = lxResult;
//      compileMessageParameters(writer, node, scope, implicitSignatureRef, HINT_EXTENSION_MODE);
//   }
//   else object = compileMessageParameters(writer, node, scope, implicitSignatureRef, HINT_EXTENSION_MODE);
//
//   messageRef = resolveMessageAtCompileTime(role, scope, messageRef, implicitSignatureRef);
//
//   return compileMessage(writer, node, scope, role, messageRef, HINT_EXTENSION_MODE);
//}
//
//bool Compiler :: declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, int mode)
//{
//   bool lazyExpression = test(mode, HINT_LAZY_EXPR);
//
//   methodScope.message = encodeVerb(lazyExpression ? EVAL_MESSAGE_ID : INVOKE_MESSAGE_ID);
//
//   if (argNode != lxNone) {
//      // define message parameter
//      methodScope.message = declareInlineArgumentList(argNode, methodScope);
//   }
//
//   ref_t parentRef = scope.info.header.parentRef;
//   if (lazyExpression) {
//      parentRef = scope.moduleScope->getBaseLazyExpressionClass();
//   }
//   else {
//      ref_t actionRef = scope.moduleScope->actionHints.get(methodScope.message);
//      if (actionRef)
//         parentRef = actionRef;
//   }
//
//   compileParentDeclaration(SNode(), scope, parentRef);
//
//   return lazyExpression;
//}
//
//void Compiler :: compileAction(SNode node, ClassScope& scope, SNode argNode, int mode)
//{
//   SyntaxTree expressionTree;
//   SyntaxWriter writer(expressionTree);
//
//   writer.newNode(lxClass, scope.reference);
//
//   MethodScope methodScope(&scope);
//   bool lazyExpression = declareActionScope(scope, argNode, methodScope, mode);
//
//   methodScope.closureMode = true; // !! do we need it??
//
//   scope.include(methodScope.message);
//   scope.addHint(methodScope.message, tpAction);
//
//   // HOTFIX : if the closure emulates code brackets
//   if (test(mode, HINT_SUBCODE_CLOSURE))
//      methodScope.subCodeMode = true;
//
//   // if it is single expression
//   if (!lazyExpression) {
//      initialize(scope, methodScope);
//
//      compileActionMethod(writer, node, methodScope);
//   }
//   else compileLazyExpressionMethod(writer, node, methodScope);
//
//   if (node.existChild(lxClosureMessage)) {
//      // inject a virtual invoke multi-method if required
//      SyntaxTree virtualTree;
//      virtualTree.insertNode(0, lxClass, 0);
//
//      List<ref_t> implicitMultimethods;
//      implicitMultimethods.add(encodeMessage(INVOKE_MESSAGE_ID, getAbsoluteParamCount(methodScope.message)));
//
//      _logic->injectVirtualMultimethods(*scope.moduleScope, virtualTree.readRoot(), scope.info, *this, implicitMultimethods, lxClassMethod);
//
//      generateClassDeclaration(virtualTree.readRoot(), scope, false);
//
//      compileVMT(writer, virtualTree.readRoot(), scope);
//   }
//   else {
//      generateClassDeclaration(SNode(), scope, false);
//
//      if (test(scope.info.header.flags, elWithMuti)) {
//         // HOTFIX: temporally the closure does not generate virtual multi-method
//         // so the class should be turned into limited one (to fix bug in multi-method dispatcher)
//         scope.info.header.flags &= ~elSealed;
//         scope.info.header.flags |= elClosed;
//      }
//   }
//
//   writer.closeNode();
//
//   scope.save();
//
//   generateClassImplementation(expressionTree.readRoot(), scope);
//}
//
//void Compiler :: compileNestedVMT(SNode node, InlineClassScope& scope)
//{
//   SyntaxTree expressionTree;
//   SyntaxWriter writer(expressionTree);
//
//   // check if the class was already compiled
//   if (!node.argument) {
//      compileParentDeclaration(node, scope);
//
//      declareVMT(node, scope);
//
//      // check if it is a virtual vmt (only for the class initialization)
//      SNode current = node.firstChild();
//      bool virtualClass = true;
//      while (current != lxNone) {
//         if (current == lxClassField) {
//            virtualClass = false;
//         }
//         else if (current == lxClassMethod) {
//            if (!test(current.argument, SEALED_MESSAGE)) {
//               virtualClass = false;
//               break;
//            }
//         }
//         current = current.nextNode();
//      }
//
//      if (virtualClass)
//         scope.info.header.flags |= elVirtualVMT;
//
//      generateClassDeclaration(node, scope, false, true);
//
//      scope.save();
//   }
//   else scope.moduleScope->loadClassInfo(scope.info, scope.moduleScope->module->resolveReference(node.argument), false);
//
//   writer.newNode(lxClass, scope.reference);
//
//   compileVMT(writer, node, scope);
//
//   // set flags once again
//   // NOTE : it should be called after the code compilation to take into consideration outer fields
//   _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, false);
//
//   writer.closeNode();
//   scope.save();
//
//   generateClassImplementation(expressionTree.readRoot(), scope);
//}
//
//ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope)
//{
//   ref_t closureRef = scope.reference;
//   if (test(scope.info.header.flags, elVirtualVMT))
//      closureRef = scope.info.header.parentRef;
//
//   if (test(scope.info.header.flags, elStateless)) {
//      ref_t implicitConstructor = encodeMessage(NEWOBJECT_MESSAGE_ID, 0) | CONVERSION_MESSAGE;
//      if (scope.info.methods.exist(implicitConstructor, true)) {
//         // if implicit constructor is declared - it should be automatically called
//         writer.newNode(lxCalling, implicitConstructor);
//         writer.appendNode(lxConstantSymbol, closureRef);
//         writer.closeNode();
//      }
//      else writer.appendNode(lxConstantSymbol, closureRef);
//
//      // if it is a stateless class
//      return ObjectInfo(okConstantSymbol, closureRef, closureRef/*, scope.moduleScope->defineType(scope.reference)*/);
//   }
//   else if (test(scope.info.header.flags, elDynamicRole)) {
//      scope.raiseError(errInvalidInlineClass, node);
//
//      // idle return
//      return ObjectInfo();
//   }
//   else {
//      // dynamic binary symbol
//      if (test(scope.info.header.flags, elStructureRole)) {
//         writer.newNode(lxStruct, scope.info.size);
//         writer.appendNode(lxTarget, closureRef);
//
//         if (scope.outers.Count() > 0)
//            scope.raiseError(errInvalidInlineClass, node);
//      }
//      else {
//         // dynamic normal symbol
//         writer.newNode(lxNested, scope.info.fields.Count());
//         writer.appendNode(lxTarget, closureRef);
//      }
//
//      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
//      //int toFree = 0;
//      while(!outer_it.Eof()) {
//         ObjectInfo info = (*outer_it).outerObject;
//
//         writer.newNode((*outer_it).preserved ? lxOuterMember : lxMember, (*outer_it).reference);
//         writeTerminal(writer, node, ownerScope, info, 0);
//         writer.closeNode();
//
//         outer_it++;
//      }
//
//      if (scope.returningMode) {
//         // injecting returning code if required
//         InlineClassScope::Outer retVal = scope.outers.get(RETVAL_VAR);
//
//         writer.newNode(lxCode);
//         writer.newNode(lxExpression);
//         writer.newNode(lxBranching);
//
//         writer.newNode(lxExpression);
//         writer.appendNode(lxCurrent);
//         writer.appendNode(lxResultField, retVal.reference); // !! current field
//         writer.closeNode();
//
//         writer.newNode(lxIfNot, -1);
//         writer.newNode(lxCode);
//         writer.newNode(lxReturning);
//         writer.appendNode(lxResult);
//         writer.closeNode();
//         writer.closeNode();
//         writer.closeNode();
//
//         writer.closeNode();
//         writer.closeNode();
//         writer.closeNode();
//      }
//
//      ref_t implicitConstructor = encodeMessage(NEWOBJECT_MESSAGE_ID, 0) | CONVERSION_MESSAGE;
//      if (scope.info.methods.exist(implicitConstructor, true)) {
//         // if implicit constructor is declared - it should be automatically called
//         writer.newNode(lxOvreriddenMessage, implicitConstructor);
//         if (scope.reference != closureRef)
//            writer.appendNode(lxTarget, scope.reference);
//         writer.closeNode();
//      }
//
//      writer.closeNode();
//
//      return ObjectInfo(okObject, closureRef);
//   }
//}
//
//ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode)
//{
//   ref_t nestedRef = 0;
//   bool singleton = false;
//   if (test(mode, HINT_ROOTSYMBOL)) {
//      SymbolScope* owner = (SymbolScope*)ownerScope.getScope(Scope::slSymbol);
//      if (owner) {
//         nestedRef = owner->reference;
//         // HOTFIX : symbol should refer to self and $self for singleton closure
//         singleton = node.existChild(lxCode);
//      }
//   }
//   if (!nestedRef)
//      nestedRef = ownerScope.moduleScope->mapAnonymous();
//
//   InlineClassScope scope(&ownerScope, nestedRef);
//
//   // if it is a lazy expression / multi-statement closure without parameters
//   SNode argNode = node.firstChild();
//   if (node == lxLazyExpression) {
//      compileAction(node, scope, SNode(), HINT_LAZY_EXPR);
//   }
//   else if (argNode == lxCode) {
//      compileAction(node, scope, SNode(), singleton ? mode | HINT_SINGLETON : mode);
//   }
//   else if (node.existChild(lxCode)) {
//      SNode codeNode = node.findChild(lxCode);
//
//      // if it is a closure / lambda function with a parameter
//      int actionMode = mode;
//      if (singleton)
//         actionMode |= HINT_SINGLETON;
//
//      compileAction(node, scope, node.findChild(lxIdentifier, lxPrivate, lxMethodParameter, lxClosureMessage), actionMode);
//
//      // HOTFIX : hide code node because it is no longer required
//      codeNode = lxIdle;
//   }
//   // if it is a nested class
//   else compileNestedVMT(node, scope);
//
//   return compileClosure(writer, node, ownerScope, scope);
//}
//
//ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   ref_t parentRef = scope.moduleScope->arrayReference;
//   SNode parentNode = node.findChild(lxIdentifier, lxPrivate, lxReference);
//   if (parentNode != lxNone) {
//      parentRef = scope.moduleScope->mapTerminal(parentNode, true);
//      if (parentRef == 0)
//         scope.raiseError(errUnknownObject, node);
//   }
//
//   return compileCollection(writer, node, scope, parentRef);
//}
//
//ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t vmtReference)
//{
//   if (vmtReference == 0)
//      vmtReference = scope.moduleScope->superReference;
//
//   int counter = 0;
//
//   writer.newBookmark();
//
//   // all collection memebers should be created before the collection itself
//   SNode current = node.findChild(lxExpression);
//   while (current != lxNone) {
//      writer.newNode(lxMember, counter);
//      compileExpression(writer, current, scope, 0);
//      writer.closeNode();
//
//      current = current.nextNode();
//      counter++;
//   }
//
//   writer.appendNode(lxTarget, vmtReference);
//   writer.insert(lxNested, counter);
//   writer.closeNode();
//
//   writer.removeBookmark();
//
//   return ObjectInfo(okObject, vmtReference);
//}

ObjectInfo Compiler :: compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
//   bool typecasting = false;
//   bool converting = false;
//   ref_t signature = 0;
   ref_t targetRef = 0;
   if (test(mode, HINT_ROOT)) {
      // type cast returning value if required
      //int paramCount;
      //decodeMessage(scope.getMessageID(), signature, paramCount);

      targetRef = scope.getReturningRef();
      //else if (paramCount == 0 && signature != 0) {
      //   ident_t signName = scope.moduleScope->module->resolveSubject(signature);
      //   int index = signName.find('$');
      //   // if it is a qualified message and is not a property
      //   if (index != NOTFOUND_POS && signName[index + 1] != 0) {
      //      IdentifierString className(signName.c_str() + index + 1);

      //      typecasting = true;
      //      targetRef = scope.moduleScope->module->mapReference(className);
      //   }
      //}
   }

//   writer.newBookmark();

   ObjectInfo info = compileExpression(writer, node, scope, targetRef, mode);

//   if (converting || typecasting)
//      convertObject(writer, *scope.moduleScope, targetRef, resolveObjectReference(scope, info), info.element);
//
//   // HOTFIX : implementing closure exit
//   if (test(mode, HINT_ROOT)) {
//      ObjectInfo retVar = scope.mapTerminal(RETVAL_VAR);
//      if (retVar.kind != okUnknown) {
//         writer.insertChild(0, lxField, retVar.param);
//
//         writer.insert(lxAssigning);
//         writer.closeNode();
//      }
//   }
//
//   writer.removeBookmark();

   return info;
}

//void Compiler :: compileTrying(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   writer.newBookmark();
//
//   bool catchNode = false;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (test(current.type, lxObjectMask)) {
//         compileExpression(writer, current, scope, catchNode ? HINT_TRY_MODE | HINT_RESENDEXPR : 0);
//
//         catchNode = true;
//      }
//
//      current = current.nextNode();
//   }
//
//   writer.insert(lxTrying);
//   writer.closeNode();
//
//   writer.removeBookmark();
//}
//
//void Compiler :: compileAltOperation(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   // extract the expression target
//   SNode firstExpr = node.firstChild(lxObjectMask);
//   SNode targetNode = firstExpr.firstChild(lxObjectMask);
//
//   writer.newBookmark();
//
//   // inject a temporal variable
//   int tempLocal = scope.newLocal();
//   writer.newNode(lxAssigning);
//   writer.appendNode(lxLocal, tempLocal);
//   compileExpression(writer, targetNode, scope, 0);
//   writer.closeNode();
//
//   targetNode.set(lxLocal, tempLocal);
//
//   writer.newBookmark();
//
//   bool catchNode = false;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (test(current.type, lxExprMask)) {
//         compileExpression(writer, current, scope, catchNode ? HINT_ALT_MODE | HINT_RESENDEXPR : 0);
//
//         catchNode = true;
//      }
//
//      current = current.nextNode();
//   }
//
//   writer.insert(lxAlt);
//   writer.closeNode();
//
//   writer.removeBookmark();
//
//   // inject a nested expression
//   writer.insert(lxAltExpression);
//   writer.closeNode();
//
//   writer.removeBookmark();
//}
//
//ObjectInfo Compiler :: compileBoxingExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
//{
//   writer.newBookmark();
//
//   ref_t targetRef = scope.moduleScope->module->mapReference(node.findChild(lxClassRefAttr).identifier(), false);
//
//   ObjectInfo retVal = ObjectInfo(okObject, targetRef);
//   SNode objectNode = node.findChild(lxExpression);
//   if (objectNode != lxNone) {
//      if (node.existChild(lxOperator)) {
//         ObjectInfo loperand = compileExpression(writer, objectNode, scope, 0);
//
//         ref_t arrayRef = 0;
//         int operationType = _logic->resolveNewOperationType(*scope.moduleScope, 
//            targetRef, resolveObjectReference(scope, loperand), arrayRef);
//
//         if (operationType != 0) {
//            // if it is a primitive operation
//            _logic->injectNewOperation(writer, *scope.moduleScope, operationType, arrayRef, targetRef);
//
//            retVal = assignResult(writer, scope, arrayRef, targetRef);
//         }
//         else scope.raiseError(errInvalidOperation, node);
//      }
//      else if (isCollection(objectNode, true)) {
//         SNode argNode = isCollection(objectNode, true) ? objectNode : objectNode.findChild(lxExpression);
//
//         int paramCount = SyntaxTree::countChild(argNode, lxExpression);
//
//         ref_t actionRef = 0;
//         compileMessageParameters(writer, argNode, scope, actionRef, HINT_PARAMETERSONLY);
//         if (!_logic->injectImplicitConstructor(writer, *scope.moduleScope, *this, targetRef, actionRef, paramCount))
//            scope.raiseError(errIllegalOperation, node);
//      }
//      else {
//         ObjectInfo object = compileExpression(writer, objectNode, scope, mode);
//         if (!convertObject(writer, *scope.moduleScope, targetRef, resolveObjectReference(scope, object), 0))
//            scope.raiseError(errIllegalOperation, node);
//
//         //if (!_logic->injectImplicitConversion(writer, *scope.moduleScope, *this, targetRef, resolveObjectReference(scope, object), 0))
//         //   scope.raiseError(errIllegalOperation, node);
//      }
//   }
//   else if (!_logic->injectImplicitCreation(writer, *scope.moduleScope, *this, targetRef))
//      scope.raiseError(errIllegalOperation, node);
//
//   writer.removeBookmark();
//
//   return retVal;
//}

ObjectInfo Compiler :: compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, int mode)
{
   writer.newBookmark();

   SNode object = node.firstChild(lxObjectMask);
   ObjectInfo objectInfo = compileObject(writer, object, scope, mode);

   SNode current = object.nextNode();
   switch (current.type) {
      case lxMessage:
         objectInfo = compileMessage(writer, current, scope, objectInfo, mode);
         break;
   }

   if (targetRef) {
      if (!convertObject(writer, scope, targetRef, resolveObjectReference(scope, objectInfo)/*, objectInfo.element*/))
         scope.raiseError(errInvalidOperation, node);

      objectInfo = ObjectInfo(okObject, targetRef);
   }


//   ObjectInfo objectInfo;
//   if (node == lxAlt) {
//      compileAltOperation(writer, node, scope);
//
//      objectInfo = ObjectInfo(okObject);
//   }
//   else if (node == lxTrying) {
//      compileTrying(writer, node, scope);
//
//      objectInfo = ObjectInfo(okObject);
//   }
//   else if (node == lxBoxing) {
//      objectInfo = compileBoxingExpression(writer, node, scope, mode);
//   }
//   else {
//      SNode current = node.findChild(lxAssign, lxExtension, lxMessage, lxOperator, lxSwitching);
//      switch (current.type) {
//         case lxAssign:
//            objectInfo = compileAssigning(writer, node, scope, mode);
//            break;
//         case lxMessage:
//            objectInfo = compileMessage(writer, node, scope, mode);
//            break;
//         case lxSwitching:
//            compileSwitch(writer, current, scope);
//
//            objectInfo = ObjectInfo(okObject);
//            break;
//         case lxExtension:
//            objectInfo = compileExtension(writer, node, scope);
//            break;
//         case lxOperator:
//            objectInfo = compileOperator(writer, node, scope, mode);
//            break;
//         default:
//         {
//            current = node.firstChild(lxObjectMask);
//            SNode nextChild = current.nextNode();
//
//            if ((/*current == lxExpression || */current == lxAlt || current == lxTrying) && nextChild == lxNone) {
//               // if it is a nested expression
//               objectInfo = compileExpression(writer, current, scope, mode);
//            }
//            else if (test(current.type, lxTerminalMask) && nextChild == lxExpression) {
//               //HOTFIX : to compile the collection
//               objectInfo = compileCollection(writer, node, scope);
//            }
//            else if (test(current.type, lxTerminalMask)/* && nextChild == lxNone*/) {
//               objectInfo = compileObject(writer, current, scope, mode);
//            }
//            else if (current == lxConstantList) {
//               objectInfo = compileObject(writer, current, scope, mode);
//            }
//            else objectInfo = compileObject(writer, node, scope, mode);
//            break;
//         }
//      }
//   }

   writer.removeBookmark();

   return objectInfo;
}

//ObjectInfo Compiler :: compileAssigningExpression(SyntaxWriter& writer, SNode assigning, CodeScope& scope)
//{
//   writer.newNode(lxExpression);
//   //writer.appendNode(lxBreakpoint, dsStep);
//
//   ObjectInfo objectInfo = compileExpression(writer, assigning, scope, HINT_NOUNBOXING);
//
//   writer.closeNode();
//
//   return objectInfo;
//}
//
//ObjectInfo Compiler :: compileBranching(SyntaxWriter& writer, SNode thenCode, CodeScope& scope)
//{
//   CodeScope subScope(&scope);
//
//   writer.newNode(lxCode);
//
//   compileCode(writer, thenCode, subScope);
//
//   // preserve the allocated space
//   scope.level = subScope.level;
//
//   writer.closeNode();
//
//   return ObjectInfo(okObject);
//}
//
//void Compiler :: compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   // find inner expression
//   SNode expr = node;
//   while (expr.findChild(lxMessage, lxAssign, lxOperator) == lxNone) {
//      expr = expr.findChild(lxExpression);
//   }
//
//   compileExpression(writer, expr, scope, HINT_LOOP);
//}

ObjectInfo Compiler :: compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
//         case lxExpression:
//            writer.newNode(lxExpression);
//            writer.appendNode(lxBreakpoint, dsStep);
//            compileExpression(writer, current, scope, HINT_ROOT);
//            writer.closeNode();
//            break;
//         case lxLoop:
//            writer.newNode(lxExpression);
//            writer.appendNode(lxBreakpoint, dsStep);
//            compileLoop(writer, current, scope);
//            writer.closeNode();
//            break;
         case lxReturning:
         {
            needVirtualEnd = false;

//            if (test(scope.getMessageID(), CONVERSION_MESSAGE))
//               scope.raiseError(errIllegalOperation, current);
//
            writer.newNode(lxReturning);
            writer.appendNode(lxBreakpoint, dsStep);
            retVal = compileRetExpression(writer, current, scope, HINT_ROOT);
            writer.closeNode();

            break;
         }
//         case lxVariable:
//            compileVariable(writer, current, scope);
//            break;
//         case lxExtern:
//            writer.newNode(lxExternFrame);
//            compileCode(writer, current.findSubNode(lxCode), scope);
//            writer.closeNode();
//            break;
         case lxEOF:
            needVirtualEnd = false;
            writer.newNode(lxBreakpoint, dsEOP);
            writeTerminalInfo(writer, current);
            writer.closeNode();
            break;
      }

//      scope.freeSpace();

      current = current.nextNode();
   }

   if (needVirtualEnd) {
      writer.appendNode(lxBreakpoint, dsVirtualEnd);
   }

   return retVal;
}

//void Compiler :: compileExternalArguments(SNode node, ModuleScope& moduleScope, WarningScope& warningScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (test(current.type, lxObjectMask)) {
//         analizeExpressionTree(current, moduleScope, warningScope, HINT_NOBOXING);
//
//         ref_t classReference = current.findChild(lxExtArgumentRef).argument;
//         if (classReference) {
//            ClassInfo classInfo;
//            _logic->defineClassInfo(moduleScope, classInfo, classReference);
//
//            ref_t primitiveRef = _logic->retrievePrimitiveReference(moduleScope, classInfo);
//            switch (primitiveRef) {
//               case V_INT32:
//               case V_SIGNATURE:
//               case V_MESSAGE:
//                  current.set(_logic->isVariable(classInfo) ? lxExtArgument : lxIntExtArgument, 0);
//                  break;
//               case V_SYMBOL:
//               {
//                  current.set(lxExtInteranlRef, 0);
//                  // HOTFIX : ignore call operation
//                  SNode callNode = current.findChild(lxCalling);
//                  callNode.set(lxExpression, 0);
//                  break;
//               }
//               default:
//                  if (test(classInfo.header.flags, elStructureRole)) {
//                     current.set(lxExtArgument, 0);
//                  }
//                  else if (test(classInfo.header.flags, elWrapper)) {
//                     //HOTFIX : allow to pass a normal object
//                     current.set(lxExtArgument, 0);
//                  }
//                  else moduleScope.raiseError(errInvalidOperation, current);
//                  break;
//            }
//         }
//         else moduleScope.raiseError(errInvalidOperation, current);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//ObjectInfo Compiler :: compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   ObjectInfo retVal(okExternal);
//
//   ModuleScope* moduleScope = scope.moduleScope;
//
////   bool rootMode = test(mode, HINT_ROOT);
//   bool stdCall = false;
//   bool apiCall = false;
//
//   SNode targetNode = node.firstChild(lxTerminalMask);
//   // HOTFIX : comment out dll reference
//   targetNode = lxIdle;
//
//   //SNode messageNode = node.findChild(lxMessage);
//   writer.appendNode(lxBreakpoint, dsAtomicStep);
//
//   ident_t dllAlias = targetNode.identifier();
//   ident_t functionName = node.findChild(lxMessage).firstChild(lxTerminalMask).identifier();
//
//   ident_t dllName = NULL;
//   if (dllAlias.compare(EXTERNAL_MODULE)) {
//      // if run time dll is used
//      dllName = RTDLL_FORWARD;
//
//      if (functionName.compare(COREAPI_MASK, COREAPI_MASK_LEN))
//         apiCall = true;
//   }
//   else dllName = moduleScope->project->resolveExternalAlias(dllAlias + strlen(EXTERNAL_MODULE) + 1, stdCall);
//
//   // legacy : if dll is not mapped, use the name directly
//   if (emptystr(dllName))
//      dllName = dllAlias + strlen(EXTERNAL_MODULE) + 1;
//
//   ReferenceNs name;
//   if (!apiCall) {
//      name.copy(DLL_NAMESPACE);
//      name.combine(dllName);
//      name.append(".");
//      name.append(functionName);
//   }
//   else {
//      name.copy(NATIVE_MODULE);
//      name.combine(CORE_MODULE);
//      name.combine(functionName);
//   }
//
//   ref_t reference = moduleScope->module->mapReference(name);
//
////   if (!rootMode)
////      scope.writer->appendNode(lxTarget, -1);
//
//   // To tell apart coreapi calls, the name convention is used
//   if (apiCall) {
//      writer.insert(lxCoreAPICall, reference);
//   }
//   else writer.insert(stdCall ? lxStdExternalCall : lxExternalCall, reference);
//   writer.closeNode();
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ObjectInfo routine)
//{
//   ModuleScope* moduleScope = scope.moduleScope;
//
//   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
//   virtualReference.append('.');
//
//   int paramCount;
//   ref_t actionRef;
//   decodeMessage(message, actionRef, paramCount);
//
//   size_t signIndex = virtualReference.Length();
//   virtualReference.append('0' + (char)paramCount);
//   virtualReference.append(moduleScope->module->resolveSubject(actionRef));
//
//   virtualReference.replaceAll('\'', '@', signIndex);
//
//   writer.insert(lxInternalCall, moduleScope->module->mapReference(virtualReference));
//   writer.closeNode();
//
//   SNode targetNode = node.firstChild(lxTerminalMask);
//   // HOTFIX : comment out dll reference
//   targetNode = lxIdle;
//
//   return ObjectInfo(okObject);
//}
//
//int Compiler :: allocateStructure(bool bytearray, int& allocatedSize, int& reserved)
//{
//   if (bytearray) {
//      // plus space for size
//      allocatedSize = ((allocatedSize + 3) >> 2) + 2;
//   }
//   else allocatedSize = (allocatedSize + 3) >> 2;
//
//   int retVal = reserved;
//   reserved += allocatedSize;
//
//   // the offset should include frame header offset
//   retVal = -2 - retVal;
//
//   // reserve place for byte array header if required
//   if (bytearray)
//      retVal -= 2;
//
//   return retVal;
//}
//
//bool Compiler :: allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand)
//{
//   if (size <= 0)
//      return false;
//
//   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
//   if (methodScope == NULL)
//      return false;
//
//   int offset = allocateStructure(bytearray, size, scope.reserved);
//
//   // if it is not enough place to allocate
//   if (methodScope->reserved < scope.reserved) {
//      methodScope->reserved += size;
//   }
//
//   exprOperand.kind = okLocalAddress;
//   exprOperand.param = offset;
//
//   return true;
//}
//
//ref_t Compiler :: declareInlineArgumentList(SNode arg, MethodScope& scope)
//{
//   IdentifierString signature;
//   IdentifierString messageStr;
//
//   ref_t actionRef = 0;
//
//   SNode sign = goToNode(arg, lxClosureMessage);
//   bool first = true;
//   while (arg == lxMethodParameter || arg == lxIdentifier || arg == lxPrivate) {
//      SNode terminalNode = arg;
//      if (terminalNode == lxMethodParameter) {
//         terminalNode = terminalNode.findChild(lxIdentifier, lxPrivate);
//      }
//
//      ident_t terminal = terminalNode.identifier();
//      int index = 1 + scope.parameters.Count();
//
//      // !! check duplicates
//      if (scope.parameters.exist(terminal))
//         scope.raiseError(errDuplicatedLocal, arg);
//
//      if (sign != lxNone) {
//         // if the closure has explicit signature
//         ref_t elementRef = 0;
//         ref_t class_ref = declareArgumentSubject(sign, *scope.moduleScope, first, messageStr, signature, elementRef);
//
//         int size = class_ref != 0 ? _logic->defineStructSize(*scope.moduleScope, class_ref, 0) : 0;
//         scope.parameters.add(terminal, Parameter(index, class_ref, elementRef, size));
//
//         sign = sign.nextNode();
//      }
//      else if (first) {
//         scope.parameters.add(terminal, Parameter(index));
//      }
//      else scope.raiseError(errInvalidSyntax, arg);      
//
//      arg = arg.nextNode();
//   }
//
//   if (emptystr(messageStr)) {
//      messageStr.copy(INVOKE_MESSAGE);
//   }
//   messageStr.append(signature);
//
//   actionRef = scope.moduleScope->module->mapSubject(messageStr, false);
//
//   return encodeMessage(actionRef, scope.parameters.Count());
//}

inline SNode findTerminal(SNode node)
{
   SNode ident = node.findChild(lxIdentifier, lxPrivate);
   //if (ident == lxNone)
   //   ident = node;

   return ident;
}

ref_t Compiler :: declareArgumentType(SNode attribute, Scope& scope/*, bool& first, IdentifierString& messageStr, IdentifierString& signature, ref_t& elementRef*/)
{
   NamespaceScope* namespaceScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

//   SNode type = arg.findChild(/*lxIdentifier, lxPrivate, */lxReference/*, lxClassRefAttr*/);
//   //HOTFIX : if generated by a script
//   if (subject == lxNone)
//      subject = arg;

   ref_t class_ref = 0;
   if (namespaceScope)
      class_ref = namespaceScope->resolveImplicitIdentifier(attribute.identifier());

////   if (arg.compare(lxMessage, lxClosureMessage) && subject != lxReference && subject != lxClassRefAttr) {
////      bool openArg = arg.existChild(lxSize);
////      if (arg == lxMessage/* && !openArg*/) {
////         if (!first) {
////            messageStr.append('&');
////         }
////      }
////
////      if (arg == lxClosureMessage) {
////         class_ref = scope.mapTerminal(subject, true);
////         if (!class_ref)
////            class_ref = scope.mapSubject(subject, messageStr);
////      }
////      else class_ref = scope.mapSubject(subject, messageStr);
////
////      if (class_ref) {
////         if (openArg) {
////            elementRef = class_ref;
////            class_ref = V_ARGARRAY;
////         }
////         else {
////            signature.append('$');
////            signature.append(scope.module->resolveReference(class_ref));
////         }
////
////         if (!first)
////            messageStr.truncate(messageStr.Length() - 1);
////      }
////      else first = false;
////   }
////   else {
////      if (arg == lxParamRefAttr) {
////         class_ref = scope.mapReference(arg.identifier());
////         if (arg.existChild(lxSize)) {
////            elementRef = class_ref;
////
////            return V_ARGARRAY;
////         }
////      }
////      else {
//         class_ref = scope.mapReference(attribute.identifier());
////         if (subject.existChild(lxSize)) {
////            elementRef = class_ref;
////
////            return V_ARGARRAY;
////         }
////      }
////      signature.append('$');
////      signature.append(scope.module->resolveReference(class_ref));
////   }

   if (!class_ref)
      scope.raiseError(errInvalidSubject, attribute);

   return class_ref;
}

void Compiler :: declareArgumentList(SNode node, MethodScope& scope)
{
   IdentifierString actionStr;
   ref_t actionRef = 0;

   ref_t signature[MAX_ARG_COUNT];
   size_t signatureLen = 0;

//   ref_t verbRef = 0;
//   bool propMode = false;
//   bool constantConversion = false;
//   bool unnamedMessage = false;
//   ref_t flags = 0;

   SNode action = node.firstChild(lxTerminalMask);
   SNode current = action/*.findChild(lxMethodParameter/*, lxMessage, lxParamRefAttr)*/.nextNode();

//   if (verb == lxNone) {
//      if (arg == lxMessage) {
//         verb = arg;
//         arg = verb.nextNode();
//      }
//   }

   if (action != lxNone)
      actionStr.copy(action.identifier()); // !! temporal
//      verbRef = scope.mapSubject(verb, messageStr);

//   if (node.argument == 0) {
//      if (verbRef) {
//         if (arg == lxMethodParameter || arg == lxNone) {
//            signature.append('$');
//            signature.append(scope.moduleScope->module->resolveReference(verbRef));
//         }
//         else scope.raiseError(errIllegalMethod, verb);
//      }
//      else if (verb != lxNone) {
//         // COMPILER MAGIC : recognize set property
//         int verb_id = _verbs.get(messageStr.c_str());
//         if (verb_id == SET_MESSAGE_ID) {
//            propMode = true;
//            flags |= PROPSET_MESSAGE;
//         }
//      }
//      else if (node.existChild(lxClassRefAttr) && arg == lxNone) {
//         signature.append('$');
//         signature.append(node.findChild(lxClassRefAttr).identifier());
//      }
//      else unnamedMessage = true;
//   }
//
//   bool first = messageStr.Length() == 0;
   int paramCount = 0;
//   int strongParamCounter = 0;
//   // if method has an argument list
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         int index = 1 + scope.parameters.Count();
         ref_t classRef = 0;

         ident_t terminal = findTerminal(current).identifier();
         if (scope.parameters.exist(terminal))
            scope.raiseError(errDuplicatedLocal, current);

         SNode attribute = current.findChild(lxTypeAttr);
         if (attribute != lxNone) {
            classRef = declareArgumentType(attribute.firstChild(lxTerminalMask), scope/*, first, messageStr, signature, elementRef*/);

            signature[signatureLen++] = classRef;
         }

         //      // if it is typified argument
         //      if (verbRef) {
         //         int size = _logic->defineStructSize(*scope.moduleScope, verbRef, 0);
         //
         //         scope.parameters.add(terminal, Parameter(index, verbRef, size));
         //         verbRef = 0;
         //         strongParamCounter++;
         //      }
         scope.parameters.add(terminal, Parameter(index, classRef));

         paramCount++;
         if (paramCount >= OPEN_ARG_COUNT)
            scope.raiseError(errTooManyParameters, current);

      }
      else break;

      current = current.nextNode();
   }

//   // if method has named argument list
//   while (arg == lxMessage || arg == lxParamRefAttr) {
//      ref_t elementRef = 0;
//      ref_t class_ref = declareArgumentSubject(arg, *scope.moduleScope, first, messageStr, signature, elementRef);
//      if (class_ref != 0)
//         strongParamCounter++;
//
//      arg = arg.nextNode();
//
//      if (arg == lxMethodParameter) {
//         ident_t name = arg.findChild(lxIdentifier, lxPrivate).identifier();
//
//         if (scope.parameters.exist(name))
//            scope.raiseError(errDuplicatedLocal, arg);
//
//         int index = 1 + scope.parameters.Count();
//
//         // if it is an open argument type
//         if (class_ref == V_ARGARRAY) {
//            scope.parameters.add(name, Parameter(index, class_ref, elementRef, 0));
//
//            // the generic arguments should be free by the method exit
//            scope.withOpenArg = true;
//
//            // to indicate open argument list
//            paramCount += OPEN_ARG_COUNT;
//            if (paramCount > MAX_ARG_COUNT) {
//               scope.raiseError(errNotApplicable, arg);
//            }
//            else if (elementRef != 0 && (elementRef != scope.moduleScope->superReference || paramCount > OPEN_ARG_COUNT)) {
//               signature.append('$');
//               signature.append(scope.moduleScope->module->resolveReference(elementRef));
//            }
//         }
//         else {
//            paramCount++;
//            int size = class_ref != 0 ? _logic->defineStructSize(*scope.moduleScope, class_ref, 0) : 0;
//
//            scope.parameters.add(name, Parameter(index, class_ref, elementRef, size));
//
//            arg = arg.nextNode();
//         }
//      }
//   }

   // HOTFIX : validate that strong parameters are not mixed with generic ones
   if (signatureLen > 0 && signatureLen != paramCount && paramCount < OPEN_ARG_COUNT)
      scope.raiseError(errIllegalMethod, node);

   // HOTFIX : do not overrwrite the message on the second pass
   if (scope.message == 0) {
//      if (test(scope.hints, tpSealed | tpGeneric) && paramCount < OPEN_ARG_COUNT) {
//         if (!emptystr(signature) || !emptystr(messageStr))
//            scope.raiseError(errInvalidHint, verb);
//
//         messageStr.copy(GENERIC_PREFIX);
//      }
//      else if (test(scope.hints, tpAction)) {
//         messageStr.copy(INVOKE_MESSAGE);
//         // Compiler Magic : if it is a generic closure - ignore fixed argument
//         if (test(scope.hints, tpGeneric) && paramCount > OPEN_ARG_COUNT) {
//            if (validateGenericClosure(signature)) {
//               signature.truncate(signature.ident().findSubStr(1, "$"));
//               paramCount = OPEN_ARG_COUNT;
//               scope.genericClosure = true;               
//               if (scope.moduleScope->mapReference(signature.ident() + 1) == scope.moduleScope->superReference) {
//                  // HOTFIX : ignore super object reference
//                  signature.clear();
//               }
//            }
//            // generic clsoure should have a homogeneous signature (i.e. same types)
//            else scope.raiseError(errIllegalMethod, node);
//         }
//      }
//
//      if (test(scope.hints, tpSealed | tpConversion)) {
//         if (strongParamCounter > 0) {
//            flags |= CONVERSION_MESSAGE;
//            if (!emptystr(messageStr) && strongParamCounter == 1)
//               constantConversion = true;
//         }
//         else if (emptystr(messageStr) && paramCount == 0) {
//            // if it is an implicit in-place constructor
//            flags |= CONVERSION_MESSAGE;
//            actionRef = NEWOBJECT_MESSAGE_ID;
//         }
//         else scope.raiseError(errIllegalMethod, node);
//      }
//      else if (test(scope.hints, tpSealed) && verb == lxPrivate) {
//         flags |= SEALED_MESSAGE;
//      }
//      else if (unnamedMessage && emptystr(messageStr))
//         messageStr.append(EVAL_MESSAGE);
//
//      if (propMode && paramCount == 1 && messageStr.Length() > 3) {
//         // COMPILER MAGIC : set&x => x
//         messageStr.cut(0, 4);
//
//         propMode = false;
//      }

      //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
      if (paramCount > 0 && (signatureLen > 0 || paramCount >= OPEN_ARG_COUNT)/* && flags != CONVERSION_MESSAGE*/) {
         actionRef = scope.moduleScope->module->mapAction(actionStr.c_str(), 0, false);

         ref_t genericMessage = encodeMessage(actionRef, paramCount)/* | flags*/;

         node.appendNode(lxMultiMethodAttr, genericMessage);
      }

//      if (scope.moduleScope->attributes.exist(messageStr) != 0)
//         scope.raiseWarning(WARNING_LEVEL_3, wrnAmbiguousMessageName, verb);
//
//      if (propMode && paramCount == 1 && !emptystr(signature)) {
//         // COMPILER MAGIC : set$int => $int
//         messageStr.copy(signature);
//      }
//      else messageStr.append(signature);
//
//      if (actionRef == NEWOBJECT_MESSAGE_ID) {
//         // HOTFIX : for implicit constructor
//      }
//      else if (messageStr.Length() > 0) {
         ref_t signatureRef = 0;
         if (signatureLen > 0)
            signatureRef = scope.moduleScope->module->mapSignature(signature, signatureLen, false);

         actionRef = scope.moduleScope->module->mapAction(actionStr.c_str(), signatureRef, false);
//         if (actionRef == DISPATCH_MESSAGE_ID) {
//            if (paramCount != 0)
//               scope.raiseError(errIllegalMethod, node);
//         }
//      }
//      else scope.raiseError(errIllegalMethod, node);

      scope.message = encodeMessage(actionRef, paramCount)/* | flags*/;

//      // if it is an explicit constant conversion
//      if (constantConversion) {
//         scope.moduleScope->saveAction(scope.message, scope.getClassRef());
//      }
   }

//   if (scope.genericClosure && paramCount > OPEN_ARG_COUNT) {
//      // Compiler Magic : if it is a generic closure - ignore fixed argument but it should be removed from the stack
//      scope.rootToFree += (paramCount - OPEN_ARG_COUNT);
//   }
}

//int Compiler ::retrieveGenericArgParamCount(ClassScope& scope)
//{
//   int extraParamCount = -1;
//   for (auto it = scope.info.methods.start(); !it.Eof(); it++) {
//      if (isOpenArg(it.key()) && _logic->isMethodGeneric(scope.info, it.key())) {
//         extraParamCount = getParamCount(it.key());
//         break;
//      }
//   }
//
//   return extraParamCount;
//}

void Compiler :: compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope/*, bool withGenericMethods, bool withOpenArgGenerics*/)
{
   writer.newNode(lxClassMethod, scope.message);

   CodeScope codeScope(&scope);

   if (isImportRedirect(node)) {
      importCode(writer, node, scope, node.findChild(lxReference).identifier(), scope.message);
   }
   else {
      scope.raiseError(errIllegalOperation, node); // !! temporal

//      writer.newNode(lxDispatching);
//
//      // if it is generic handler with redirect statement / redirect statement
//      if (node != lxNone && node.firstChild(lxObjectMask) != lxNone) {
//         // !! temporally
//         if (withOpenArgGenerics)
//            scope.raiseError(errInvalidOperation, node);
//
//         if (withGenericMethods) {
//            writer.appendNode(lxDispatching, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0));
//         }
//
//         compileDispatchExpression(writer, node, codeScope);
//      }
//      // if it is generic handler without redirect statement
//      else if (withGenericMethods) {
//         // !! temporally
//         if (withOpenArgGenerics)
//            scope.raiseError(errInvalidOperation, node);
//
//         writer.newNode(lxResending);
//
//         writer.appendNode(lxMessage, encodeMessage(codeScope.moduleScope->module->mapSubject(GENERIC_PREFIX, false), 0));
//
//         writer.newNode(lxTarget, scope.moduleScope->superReference);
//         writer.appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
//         writer.closeNode();
//
//         writer.closeNode();
//      }
//      // if it is open arg generic without redirect statement
//      else if (withOpenArgGenerics) {
//         // retrieve the number of extra arguments
//         int extraParamCount = retrieveGenericArgParamCount(*(ClassScope*)scope.parent);
//
//         writer.newNode(lxResending);
//
//         writer.appendNode(lxMessage, encodeMessage(DISPATCH_MESSAGE_ID, OPEN_ARG_COUNT + extraParamCount));
//
//         writer.newNode(lxTarget, scope.moduleScope->superReference);
//         writer.appendNode(lxMessage, encodeVerb(DISPATCH_MESSAGE_ID));
//         writer.closeNode();
//
//         writer.closeNode();
//      }
//
//      writer.closeNode();
   }

   writer.closeNode();
}

//void Compiler :: compileActionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   declareParameterDebugInfo(writer, node, scope, false, false);
//
//   CodeScope codeScope(&scope);
//
//   SNode body = node.findChild(lxCode, lxReturning);
////   if (body == lxReturning) {
////      // HOTFIX : if it is an returning expression, inject returning node
////      SNode expr = body.findChild(lxExpression);
////      expr = lxReturning;
////   }
//
//   writer.newNode(lxNewFrame);
//
//   // new stack frame
//   // stack already contains previous $self value
//   codeScope.level++;
//
//   compileCode(writer, body == lxReturning ? node : body, codeScope);
//
//   writer.closeNode();
//
//   writer.appendNode(lxParamCount, scope.parameters.Count()); // NOTE : the message target is not included into the stack for closure!!
//   writer.appendNode(lxReserved, scope.reserved);
//   writer.appendNode(lxAllocated, codeScope.level - 1);  // allocate the space for the local variables excluding "this" one
//
//   writer.closeNode();
//}
//
//void Compiler :: compileLazyExpressionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   declareParameterDebugInfo(writer, node, scope, false, false);
//
//   CodeScope codeScope(&scope);
//
//   writer.newNode(lxNewFrame);
//
//   // new stack frame
//   // stack already contains previous $self value
//   codeScope.level++;
//
//   compileRetExpression(writer, node.findChild(lxExpression), codeScope, 0);
//
//   writer.closeNode();
//
//   writer.appendNode(lxParamCount, scope.parameters.Count() + 1);
//   writer.appendNode(lxReserved, scope.reserved);
//   writer.appendNode(lxAllocated, codeScope.level - 1);  // allocate the space for the local variables excluding "this" one
//
//   writer.closeNode();
//}

void Compiler :: compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(writer, node, scope, node.findChild(lxReference).identifier(), scope.getMessageID());
   }
   else {
      scope.raiseError(errIllegalOperation, node); // !! temporal

//      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
//
//      // try to implement light-weight resend operation
//      ObjectInfo target;
//      if (isSingleStatement(node)) {
//         target = scope.mapObject(node.firstChild(lxTerminalMask));
//      }
//
//      if (target.kind == okConstantSymbol || target.kind == okField) {
//         writer.newNode(lxResending, methodScope->message);
//         writer.newNode(lxExpression);
//         if (target.kind == okField) {
//            writer.appendNode(lxResultField, target.param);
//         }
//         else writer.appendNode(lxConstantSymbol, target.param);
//
//         writer.closeNode();
//         writer.closeNode();
//      }
//      else {
//         writer.newNode(lxResending, methodScope->message);
//         writer.newNode(lxNewFrame);
//
//         target = compileExpression(writer, node, scope, 0);
//
//         writer.closeNode();
//         writer.closeNode();
//      }
   }
}

void Compiler :: compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
{
   //SNode expr = node.findChild(lxExpression);

   //ModuleScope* moduleScope = scope.moduleScope;
   //MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   //// find where the target constructor is declared in the current class
   //size_t count = 0;
   //ref_t messageRef = mapMessage(expr, scope, count);

   //ref_t classRef = classClassScope.reference;
   //bool found = false;

   //// find where the target constructor is declared in the current class
   //// but it is not equal to the current method
   //if (methodScope->message != messageRef && classClassScope.info.methods.exist(messageRef)) {
   //   found = true;
   //}
   //// otherwise search in the parent class constructors
   //else {
   //   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
   //   ref_t parent = classScope->info.header.parentRef;
   //   ClassInfo info;
   //   while (parent != 0) {
   //      moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(parent));

   //      if (checkMethod(*moduleScope, info.header.classRef, messageRef) != tpUnknown) {
   //         classRef = info.header.classRef;
   //         found = true;

   //         break;
   //      }
   //      else parent = info.header.parentRef;
   //   }
   //}
   //if (found) {
   //   if ((count != 0 && methodScope->parameters.Count() != 0) || node.existChild(lxCode) || !isConstantArguments(expr)) {
   //      withFrame = true;

   //      // new stack frame
   //      // stack already contains $self value
   //      writer.newNode(lxNewFrame);
   //      scope.level++;
   //   }
   //   else writer.newNode(lxExpression);

   //   writer.newBookmark();

   //   if (withFrame) {
   //      writer.appendNode(lxThisLocal, 1);
   //   }
   //   else writer.appendNode(lxResult);

   //   writer.appendNode(lxCallTarget, classRef);

   //   ResendScope resendScope(&scope);
   //   resendScope.consructionMode = true;
   //   resendScope.withFrame = withFrame;

   //   ref_t implicitSignatureRef = 0;
   //   compileMessageParameters(writer, expr, resendScope, implicitSignatureRef, HINT_RESENDEXPR);

   //   ObjectInfo target(okObject, classRef);
   //   messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef);

   //   compileMessage(writer, expr, resendScope, target, messageRef, HINT_RESENDEXPR);

   //   writer.removeBookmark();

   //   if (withFrame) {
   //      // HOT FIX : inject saving of the created object
   //      SNode codeNode = node.findChild(lxCode);
   //      if (codeNode != lxNone) {
   //         writer.newNode(lxAssigning);
   //         writer.appendNode(lxLocal, 1);
   //         writer.appendNode(lxResult);
   //         writer.closeNode();
   //      }
   //   }
   //   else writer.closeNode();
   //}
   /*else */scope.raiseError(errUnknownMessage, node);
}

void Compiler :: compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   if (isImportRedirect(node)) {
      importCode(writer, node, scope, node.findChild(lxReference).identifier(), scope.getMessageID());
   }
   else scope.raiseError(errInvalidOperation, node);
}

void Compiler :: compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope)
{
   //if (node.existChild(lxArgDispatcherAttr)) {
   //   // if it is a argument list unboxing routine
   //   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   //   writer.newNode(lxNewFrame);

   //   // new stack frame
   //   // stack already contains current $self reference
   //   scope.level++;

   //   writer.newNode(lxCalling, node.argument);
   //   writeTerminal(writer, node, scope, methodScope->mapThis(), HINT_NODEBUGINFO);
   //   // copy the argument list
   //   int paramCount = getParamCount(node.argument) + 1;
   //   for (int i = 1; i <= paramCount; i++) {
   //      if (i == paramCount)
   //         // unbox the last argument ist
   //         writer.newNode(lxArgUnboxing, scope.moduleScope->arrayReference);

   //      writeTerminal(writer, node, scope, methodScope->mapParameter(Parameter(i)), HINT_NODEBUGINFO);

   //      if (i == paramCount)
   //         writer.closeNode();
   //   }

   //   if (methodScope->extensionMode) {
   //      ObjectInfo target = methodScope->mapThis(true);

   //      writer.newNode(lxOverridden);
   //      writeParamTerminal(writer, scope, target, HINT_DYNAMIC_OBJECT, lxThisLocal);
   //      writeTarget(writer, resolveObjectReference(scope, target));
   //      writer.closeNode();
   //      writer.closeNode();
   //   }

   //   writer.closeNode();

   //   scope.freeSpace();

   //   writer.closeNode();
   //}
   //else {
      ref_t message = scope.getMessageID();
      ref_t overloadRef = classScope.info.methodHints.get(Attribute(message, maOverloadlist));
      if (overloadRef) {
         // !! hotfix : temporal do not use direct multi method resolving for the class constructors
         if (test(classScope.info.header.flags, /*elFinal*/elSealed)/* || test(message, SEALED_MESSAGE)*/) {
            writer.newNode(lxSealedMultiDispatching, overloadRef);
         }
         else writer.newNode(lxMultiDispatching, overloadRef);
      }
      else scope.raiseError(errIllegalOperation, node);

      if (node == lxResendExpression) {
         ref_t openArgMessage = encodeMessage(getAction(message), getParamCount(message) + OPEN_ARG_COUNT - 1);
         if (classScope.info.methods.exist(openArgMessage)) {
            writer.newNode(lxResending);

            writer.appendNode(lxMessage, encodeMessage(DISPATCH_MESSAGE_ID, getAbsoluteParamCount(openArgMessage)));
            writer.appendNode(lxOvreriddenMessage, message);

            writer.newNode(lxTarget, scope.moduleScope->superReference);
            writer.appendNode(lxMessage, encodeAction(DISPATCH_MESSAGE_ID));
            writer.closeNode();

            writer.closeNode();
         }
         else {
            writer.newNode(lxDispatching, node.argument);
            SyntaxTree::copyNode(writer, lxTarget, node);
            writer.closeNode();
         }
      }
      writer.closeNode();
   //}
}

void Compiler :: compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod/*, bool extensionMode*/)
{
   if (node.argument != 0 && multiMethod) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

      compileMultidispatch(writer, node, scope, *classScope);
   }
   else {
      scope.raiseError(errIllegalOperation, node); // !! temporal

      //if (multiMethod) {
      //   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

      //   compileMultidispatch(writer, SNode(), scope, *classScope);
      //}

      //writer.newNode(lxNewFrame);

      //// new stack frame
      //// stack already contains current $self reference
      //scope.level++;

      //writer.newNode(lxExpression);
      //compileMessage(writer, node.firstChild(lxObjectMask), scope, (/*extensionMode ? HINT_EXT_RESENDEXPR : */HINT_RESENDEXPR) | HINT_PARAMETERSONLY);
      //writer.closeNode();

      ////scope.freeSpace();

      //writer.closeNode();
   }
}

void Compiler :: compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

//   if (scope.closureMode) {
//      scope.rootToFree -= 1;
//   }

   declareParameterDebugInfo(writer, node, scope/*, true, test(scope.getClassFlags(), elRole)*/);

   int paramCount = getParamCount(scope.message);
   int preallocated = 0;

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   // check if it is a resend
   if (body == lxResendExpression) {
      compileResendExpression(writer, body, codeScope, scope.multiMethod/*, scope.extensionMode*/);
      preallocated = 1;
   }
   // check if it is a dispatch
   else if (body == lxDispatchCode) {
      compileDispatchExpression(writer, body, codeScope);
   }
   else {
      if (scope.multiMethod) {
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

         compileMultidispatch(writer, SNode(), codeScope, *classScope);
      }

      writer.newNode(lxNewFrame/*, scope.generic ? -1 : 0*/);

      // new stack frame
      // stack already contains current self reference
      // the original message should be restored if it is a generic method
      codeScope.level++;
//      // declare the current subject for a generic method
//      if (scope.generic) {
//         codeScope.level++;
//         codeScope.mapLocal(SUBJECT_VAR, codeScope.level, V_MESSAGE, 0);
//      }

      preallocated = codeScope.level;

      ObjectInfo retVal = compileCode(writer, body == lxReturning ? node : body, codeScope);

      // if the method returns itself
      // HOTFIX : it should not be applied to the embeddable conversion routine
      if(retVal.kind == okUnknown/* && (!test(scope.message, CONVERSION_MESSAGE) || !scope.classEmbeddable)*/) {
         ObjectInfo thisParam = scope.mapSelf();

         // adding the code loading $self
         writer.newNode(lxReturning);
         writer.newBookmark();
         writeTerminal(writer, node, codeScope, thisParam, HINT_NODEBUGINFO);

         ref_t resultRef = scope.getReturningRef(false);
         if (resultRef != 0) {
            if (!convertObject(writer, codeScope, resultRef, resolveObjectReference(codeScope, thisParam)/*, 0*/))
               scope.raiseError(errInvalidOperation, node);
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

//void Compiler :: compileImplicitConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   declareParameterDebugInfo(writer, node, scope, true, test(scope.getClassFlags(), elRole));
//
//   int preallocated = 0;
//
//   CodeScope codeScope(&scope);
//
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//   if (checkMethod(*scope.moduleScope, classScope->info.header.parentRef, scope.message) != tpUnknown) {
//      // check if the parent has implicit constructor - call it
//      writer.newNode(lxCalling, scope.message);
//      writer.appendNode(lxTarget, classScope->info.header.parentRef);
//      writer.closeNode();
//   }
//
//   writer.newNode(lxNewFrame/*, scope.generic ? -1 : 0*/);
//
//   // new stack frame
//   // stack already contains current $self reference
//   // the original message should be restored if it is a generic method
//   codeScope.level++;
//
//   preallocated = codeScope.level;
//
//   SNode body = node.findChild(lxCode);
//   ObjectInfo retVal = compileCode(writer, body, codeScope);
//
//   // if the method returns itself
//   if (retVal.kind == okUnknown) {
//      // adding the code loading $self
//      writer.newNode(lxExpression);
//      writer.appendNode(lxLocal, 1);
//
//      ref_t resultRef = scope.getReturningRef(false);
//      if (resultRef != 0) {
//         scope.raiseError(errInvalidOperation, node);
//      }
//
//      writer.closeNode();
//   }
//   else scope.raiseError(errIllegalMethod, node);
//
//   writer.closeNode();
//
//   writer.appendNode(lxParamCount, getParamCount(scope.message));
//   writer.appendNode(lxReserved, scope.reserved);
//   writer.appendNode(lxAllocated, codeScope.level - preallocated);  // allocate the space for the local variables excluding preallocated ones ("$this", "$message")
//
//   writer.closeNode();
//}

void Compiler :: compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   writer.newNode(lxClassMethod, scope.message);

   //SNode attrNode = node.findChild(lxEmbeddableMssg);
   //if (attrNode != lxNone) {
   //   writer.appendNode(attrNode.type, attrNode.argument);
   //}

   declareParameterDebugInfo(writer, node, scope/*, true, false*/);

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
      if (scope.multiMethod && bodyNode.argument != 0) {
         compileMultidispatch(writer, bodyNode, codeScope, classClassScope);

         bodyNode = lxNone;
      }
      else {
         compileConstructorResendExpression(writer, bodyNode, codeScope, classClassScope, withFrame);

         bodyNode = bodyNode.findChild(lxCode);
      }
   }
   else if (bodyNode == lxReturning) {
      retExpr = true;
   }
   // if no redirect statement - call virtual constructor implicitly
   else if (/*!test(classFlags, elDynamicRole) && */classClassScope.info.methods.exist(encodeAction(NEWOBJECT_MESSAGE_ID))) {
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
         //writer.newNode(lxReturning);
         //writer.newBookmark();

         //ObjectInfo retVal = compileRetExpression(writer, bodyNode, codeScope, /*HINT_CONSTRUCTOR_EPXR*/0);
         //convertObject(writer, *codeScope.moduleScope, codeScope.getClassRefId(), resolveObjectReference(codeScope, retVal), retVal.element);

         //writer.removeBookmark();
         //writer.closeNode();
         scope.raiseError(errIllegalOperation, node);
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

   //if (test(classScope->info.header.flags, elStructureRole)) {
   //   if (!test(classScope->info.header.flags, elDynamicRole)) {
   //      writer.newNode(lxCreatingStruct, classScope->info.size);
   //      writer.appendNode(lxTarget, classScope->reference);
   //      writer.closeNode();
   //   }
   //}
   //else if (!test(classScope->info.header.flags, elDynamicRole)) {
      writer.newNode(lxCreatingClass, classScope->info.fields.Count());
      writer.appendNode(lxTarget, classScope->reference);
      writer.closeNode();
   //}

   //ref_t implicitMessage = encodeVerb(NEWOBJECT_MESSAGE_ID) | CONVERSION_MESSAGE;
   //if (classScope->info.methods.exist(implicitMessage)) {
   //   if (classScope->info.methods.exist(implicitMessage, true)) {
   //      // call the field in-place initialization
   //      writer.newNode(lxCalling, implicitMessage);
   //      writer.appendNode(lxTarget, classScope->reference);
   //      writer.closeNode();
   //   }
   //   else {
   //      ref_t parentRef = classScope->info.header.parentRef;
   //      while (parentRef != 0) {
   //         // call the parent field in-place initialization
   //         ClassInfo parentInfo;
   //         _logic->defineClassInfo(*scope.moduleScope, parentInfo, parentRef);

   //         if (parentInfo.methods.exist(implicitMessage, true)) {
   //            writer.newNode(lxCalling, implicitMessage);
   //            writer.appendNode(lxTarget, parentRef);
   //            writer.closeNode();

   //            break;
   //         }

   //         parentRef = parentInfo.header.parentRef;
   //      }
   //   }
   //}

   writer.closeNode();
}

//void Compiler :: compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
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
            if (methodScope.message == encodeAction(DISPATCH_MESSAGE_ID)) {
               //if (test(scope.info.header.flags, elRole))
               //   scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               initialize(scope, methodScope);

               compileDispatcher(writer, current.findChild(lxDispatchCode), methodScope/*,
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithArgGenerics)*/);
            }
            // if it is a normal method
            else {
               initialize(scope, methodScope);
               declareArgumentList(current, methodScope);
//
//               if (methodScope.message == (encodeVerb(NEWOBJECT_MESSAGE_ID) | CONVERSION_MESSAGE)) {
//                  // if it is in-place class member initialization
//                  compileImplicitConstructor(writer, current, methodScope);
//               }
               /*else */compileMethod(writer, current, methodScope);
            }
            break;
         }
      }

      current = current.nextNode();
   }

//   // if the VMT conatains newly defined generic handlers, overrides default one
//   if (testany(scope.info.header.flags, elWithGenerics | elWithArgGenerics) && scope.info.methods.exist(encodeVerb(DISPATCH_MESSAGE_ID), false)) {
//      MethodScope methodScope(&scope);
//      methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
//
//      scope.include(methodScope.message);
//
//      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);
//
//      compileDispatcher(writer, SNode(), methodScope,
//         test(scope.info.header.flags, elWithGenerics),
//         test(scope.info.header.flags, elWithArgGenerics));
//
//      // overwrite the class info
//      scope.save();
//   }
}

void Compiler :: compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
//   bool staticFieldsInherited = false;

   // add virtual constructor
   if (classClassScope.info.methods.exist(encodeAction(NEWOBJECT_MESSAGE_ID), true)) {
      MethodScope methodScope(&classScope);
      methodScope.message = encodeAction(NEWOBJECT_MESSAGE_ID);

      /*if (test(classScope.info.header.flags, elDynamicRole)) {
         compileDynamicDefaultConstructor(writer, methodScope);
      }
      else */compileDefaultConstructor(writer, methodScope);
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
//         case lxStaticMethod:
//         {
//            if (!staticFieldsInherited) {
//               // HOTFIX : inherit static fields
//               classClassScope.copyStaticFields(classScope.info.statics, classScope.info.staticValues);
//
//               staticFieldsInherited = true;
//            }
//
//            MethodScope methodScope(&classClassScope);
//            methodScope.message = current.argument;
//            declareArgumentList(current, methodScope);
//
//            initialize(classClassScope, methodScope);
//
//            compileMethod(writer, current, methodScope);
//            break;
//         }
      }

      current = current.nextNode();
   }
}

//inline int countFields(SNode node)
//{
//   int counter = 0;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxClassField) {
//         counter++;
//      }
//
//      current = current.nextNode();
//   }
//
//   return counter;
//}

void Compiler :: generateClassFields(SNode node, ClassScope& scope/*, bool singleField*/)
{
   SNode current = node.firstChild();

   while (current != lxNone) {
      if (current == lxClassField) {
         //ref_t fieldRef = 0;
         //ref_t elementRef = 0;
         //bool isStatic = false;
         //bool isSealed = false;
         //bool isConst = false;
         //int sizeHint = 0;
         declareFieldAttributes(current, scope/*, fieldRef, elementRef, sizeHint, isStatic, isSealed, isConst*/);

         //if (isStatic) {
         //   generateClassStaticField(scope, current, fieldRef, elementRef, isSealed, isConst);
         //}
         //else if (isSealed || isConst) {
         //   scope.raiseError(errIllegalField, current);
         //}
         /*else */generateClassField(scope, current/*, fieldRef, elementRef, sizeHint, singleField*/);
      }
      //else if (current == lxFieldInit) {
      //   // HOTFIX : reallocate static constant
      //   SNode nameNode = current.findChild(lxMemberIdentifier);
      //   ObjectInfo info = scope.mapField(nameNode.identifier().c_str() + 1);
      //   if (info.kind == okStaticConstantField) {
      //      ReferenceNs name(scope.moduleScope->module->resolveReference(scope.reference));
      //      name.append(STATICFIELD_POSTFIX);
      //      name.append("##");
      //      name.appendInt(-(int)info.param);

      //      *scope.info.staticValues.getIt(info.param) = (scope.moduleScope->module->mapReference(name) | mskConstArray);
      //   }
      //}
      current = current.nextNode();
   }
}

void Compiler :: compileSymbolCode(ClassScope& scope)
{
   CommandTape tape;

//   // creates implicit symbol
//   SymbolScope symbolScope(scope.moduleScope, scope.reference);
//
//   ref_t implicitConstructor = encodeMessage(NEWOBJECT_MESSAGE_ID, 0) | CONVERSION_MESSAGE;
//   if (scope.info.methods.exist(implicitConstructor, true)) {
//      _writer.generateSymbolWithInitialization(tape, symbolScope.reference, lxConstantClass, scope.reference, implicitConstructor);
//   }
//   else _writer.generateSymbol(tape, symbolScope.reference, lxConstantClass, scope.reference);

   SyntaxTree tree;
   SyntaxWriter writer(tree);
   //   if (scope.info.methods.exist(implicitConstructor, true)) {
   //      _writer.generateSymbolWithInitialization(tape, symbolScope.reference, lxConstantClass, scope.reference, implicitConstructor);
   //   }
   generateClassSymbol(writer, scope);
   _writer.generateSymbol(tape, tree.readRoot(), false, INVALID_REF);

   // create byte code sections
   _writer.save(tape, *scope.moduleScope, INVALID_REF);
}

//void Compiler :: compilePreloadedCode(SymbolScope& scope)
//{
//   _Module* module = scope.moduleScope->module;
//
//   ReferenceNs sectionName(module->Name(), INITIALIZER_SECTION);
//
//   CommandTape tape;
//   _writer.generateInitializer(tape, module->mapReference(sectionName), lxSymbolReference, scope.reference);
//
//   // create byte code sections
//   _writer.save(tape, *scope.moduleScope);
//}
//
//void Compiler :: compilePreloadedCode(ClassScope& scope, SNode node)
//{
//   _Module* module = scope.moduleScope->module;
//
//   ReferenceNs sectionName(module->Name(), INITIALIZER_SECTION);
//
//   CommandTape tape;
//   _writer.generateInitializer(tape, module->mapReference(sectionName), node);
//
//   // create byte code sections
//   _writer.save(tape, *scope.moduleScope);
//}

void Compiler :: compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   if (classScope.info.header.parentRef == 0) {
      //   classScope.raiseError(errNoConstructorDefined, node.findChild(lxIdentifier, lxPrivate));
      classScope.info.header.parentRef = classScope.moduleScope->superReference;
   }
   else {
      ref_t parentRef = /*classScope.abstractBasedMode ? classScope.moduleScope->superReference : */classScope.info.header.parentRef;
      IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(parentRef));
      classClassParentName.append(CLASSCLASS_POSTFIX);

      classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
   }

   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef, true);

   //// !! hotfix : remove closed
   //classClassScope.info.header.flags &= ~elClosed;

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
//   methodScope.stackSafe = _logic->isMethodStacksafe(scope.info, methodScope.message);
//   methodScope.classEmbeddable = _logic->isEmbeddable(scope.info);
//   methodScope.withOpenArg = isOpenArg(methodScope.message);
//   methodScope.closureMode = _logic->isClosure(scope.info, methodScope.message);
   methodScope.multiMethod = _logic->isMultiMethod(scope.info, methodScope.message);
//   if (!methodScope.withOpenArg) {
//      // HOTFIX : generic with open argument list is compiled differently
//      methodScope.generic = _logic->isMethodGeneric(scope.info, methodScope.message);
//   }
//   else if (_logic->isMethodGeneric(scope.info, methodScope.message))
//      methodScope.genericClosure = true;
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

         if (test(methodScope.hints, tpConstructor)) {
            current = lxConstructor;
         }
//         else if (test(methodScope.hints, tpStatic))
//            current = lxStaticMethod;

         if (!_logic->validateMessage(methodScope.message, false))
            scope.raiseError(errIllegalMethod, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: generateClassFlags(ClassScope& scope, SNode root/*, bool& closureBaseClass*/)
{
   ref_t extensionTypeRef = 0;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         scope.info.header.flags |= current.argument;
      }
//      else if (current == lxTarget) {
//         extensionTypeRef = current.argument;
//      }
//      else if (current == lxClosureAttr) {
//         closureBaseClass = true;
//      }

      current = current.nextNode();
   }

//   // check if extension is qualified
//   bool extensionMode = test(scope.info.header.flags, elExtension);
//   if (extensionMode) {
//      if (extensionTypeRef == 0)
//         extensionTypeRef = scope.moduleScope->superReference;
//   }
//
//   if (extensionTypeRef != 0) {
//      if (extensionMode) {
//         scope.extensionClassRef = extensionTypeRef;
//
//         scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(scope.extensionClassRef, 0));
//      }
//      else scope.raiseError(errInvalidHint, root);
//   }
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxTree::Node current/*, ref_t classRef, ref_t elementRef, int sizeHint, bool singleField*/)
{
   //ModuleScope* moduleScope = scope.moduleScope;

   //if (singleField && sizeHint == -1) {
   //   scope.info.header.flags |= elDynamicRole;
   //   sizeHint = 0;
   //}

   int flags = scope.info.header.flags;
   int offset = 0;
   ident_t terminal = current.findChild(lxIdentifier/*, lxPrivate*/).identifier();

//   if (!classRef && typeAttr) {
//      classRef = moduleScope->attributeHints.get(typeAttr);
//   }

   // a role cannot have fields
   if (test(flags, elStateless))
      scope.raiseError(errIllegalField, current);

   //int size = (classRef != 0) ? _logic->defineStructSize(*moduleScope, classRef, elementRef) : 0;
   //bool fieldArray = false;
   //if (sizeHint != 0) {
   //   if (isPrimitiveRef(classRef) && (size == sizeHint || (classRef == V_INT32 && sizeHint <= size))) {
   //      // for primitive types size should be specified
   //      size = sizeHint;
   //   }
   //   else if (sizeHint == 8 && classRef == V_INT32) {
   //      // HOTFIX : turn int32 flag into int64
   //      classRef = V_INT64;
   //      size = 8;
   //   }
   //   else if (size > 0) {
   //      size *= sizeHint;

   //      // HOTFIX : to recognize the fixed length array
   //      if (elementRef == 0 && !isPrimitiveRef(classRef))
   //         elementRef = classRef;

   //      fieldArray = true;
   //      classRef = _logic->definePrimitiveArray(*scope.moduleScope, elementRef);
   //   }
   //   else scope.raiseError(errIllegalField, current);
   //}

   //if (test(flags, elWrapper) && scope.info.fields.Count() > 0) {
   //   // wrapper may have only one field
   //   scope.raiseError(errIllegalField, current);
   //}
   //// if it is a primitive data wrapper
   //else if (isPrimitiveRef(classRef) && !fieldArray) {
   //   if (testany(flags, elNonStructureRole | elDynamicRole))
   //      scope.raiseError(errIllegalField, current);

   //   if (test(flags, elStructureRole)) {
   //      scope.info.fields.add(terminal, scope.info.size);
   //      scope.info.size += size;
   //   }
   //   else scope.raiseError(errIllegalField, current);

   //   if (!_logic->tweakPrimitiveClassFlags(classRef, scope.info))
   //      scope.raiseError(errIllegalField, current);
   //}
   //// a class with a dynamic length structure must have no fields
   //else if (test(scope.info.header.flags, elDynamicRole)) {
   //   if (scope.info.size == 0 && scope.info.fields.Count() == 0) {
   //      // compiler magic : turn a field declaration into an array or string one
   //      if (size != 0 && !test(scope.info.header.flags, elNonStructureRole)) {
   //         scope.info.header.flags |= elStructureRole;
   //         scope.info.size = -size;
   //      }

   //      scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(classRef, /*typeRef*/0));
   //      scope.info.fields.add(terminal, -2);
   //   }
   //   else scope.raiseError(errIllegalField, current);
   //}
   //else {
      if (scope.info.fields.exist(terminal)) {
         if (current.argument == INVALID_REF) {
            //HOTFIX : ignore duplicate autogenerated fields
            return;
         }
         else scope.raiseError(errDuplicatedField, current);
      }

      //// if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
      //if (!test(scope.info.header.flags, elNonStructureRole) && singleField
      //   && test(scope.info.header.flags, elSealed) && size > 0 && scope.info.fields.Count() == 0)
      //{
      //   scope.info.header.flags |= elStructureRole;
      //   scope.info.size = size;

      //   scope.info.fields.add(terminal, 0);
      //   scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, /*typeRef*/0));
      //}
      //// if it is a structure field
      //else if (test(scope.info.header.flags, elStructureRole)) {
      //   if (size <= 0)
      //      scope.raiseError(errIllegalField, current);

      //   if (scope.info.size != 0 && scope.info.fields.Count() == 0)
      //      scope.raiseError(errIllegalField, current);

      //   offset = scope.info.size;
      //   scope.info.size += size;

      //   scope.info.fields.add(terminal, offset);
      //   scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, elementRef));
      //}
      //// if it is a normal field
      //else {
         //// primitive / virtual classes cannot be declared
         //if (size != 0 && _logic->isPrimitiveRef(classRef))
         //   scope.raiseError(errIllegalField, current);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.Count();
         scope.info.fields.add(terminal, offset);

         //if (/*typeRef != 0 || */classRef != 0)
         //   scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, /*typeRef*/0));
      //}
   //}
}

//void Compiler :: generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, ref_t elementRef, bool isSealed, bool isConst)
//{
//   _Module* module = scope.moduleScope->module;
//
//   ident_t terminal = current.findChild(lxIdentifier, lxPrivate).identifier();
//
//   if (scope.info.statics.exist(terminal)) {
//      if (current.argument == INVALID_REF) {
//         //HOTFIX : ignore duplicate autogenerated fields
//         return;
//      }
//      else scope.raiseError(errDuplicatedField, current);
//   }
//
//   if (isSealed) {
//      // generate static reference
//      ReferenceNs name(module->resolveReference(scope.reference));
//      name.append(STATICFIELD_POSTFIX);
//
//      findUninqueName(module, name);
//
//      scope.info.statics.add(terminal, ClassInfo::FieldInfo(module->mapReference(name), fieldRef));
//   }
//   else {
//      int index = ++scope.info.header.staticSize;
//      index = -index - 4;
//
//      scope.info.statics.add(terminal, ClassInfo::FieldInfo(index, fieldRef));
//
//      if (isConst) {
//         ReferenceNs name(scope.moduleScope->module->resolveReference(scope.reference));
//         name.append(STATICFIELD_POSTFIX);
//         name.append("##");
//         name.appendInt(-index);
//
//         scope.info.staticValues.add(index, scope.moduleScope->module->mapReference(name) | mskConstArray);
//      }
//      else scope.info.staticValues.add(index, (ref_t)mskStatRef);
//   }
//}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, ref_t message/*, bool allowTypeAttribute*/)
{
   //ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));
   bool hintChanged = false, outputChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));

   // HOTFIX : multimethod attribute should not be inherited
   if (test(hint, tpMultimethod)) {
      hint &= ~tpMultimethod;
      hintChanged = true;
   }

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         hint |= current.argument;

         hintChanged = true;
      }
      //else if (current == lxClassRefAttr) {
      //   if (!allowTypeAttribute) {
      //      scope.raiseError(errTypeNotAllowed, node);
      //   }
      //   else if (outputRef == 0) {
      //      outputRef = scope.moduleScope->module->mapReference(current.identifier(), false);

      //      outputChanged = true;
      //   }
      //   else if (outputRef != scope.moduleScope->module->mapReference(current.identifier(), false)) {
      //      scope.raiseError(errTypeAlreadyDeclared, node);
      //   }
      //   else outputChanged = true;
      //}
//      else if (current == lxClassMethodOpt) {
//         SNode mssgAttr = SyntaxTree::findChild(current, lxMessage);
//         if (mssgAttr != lxNone) {
//            scope.info.methodHints.add(Attribute(message, current.argument), getSignature(mssgAttr.argument));
//         }
//      }
      current = current.nextNode();
   }

   //if ((message & MESSAGE_FLAG_MASK) == SEALED_MESSAGE) {
   //   // if it is private message set private hint and save it as EVAL one
   //   hintChanged = true;
   //   hint |= tpPrivate;

   //   scope.info.methodHints.add(Attribute(message & ~SEALED_MESSAGE, maHint), hint);
   //}

   if (hintChanged) {
      //if (test(hint, tpSealed | tpGeneric) && getAction(message) == INVOKE_MESSAGE_ID) {
      //   // HOTFIX : generic closure cannot be sealed
      //   hint &= ~tpSealed;
      //}

      scope.info.methodHints.exclude(Attribute(message, maHint));
      scope.info.methodHints.add(Attribute(message, maHint), hint);
   }
   //if (outputChanged) {
   //   scope.info.methodHints.add(Attribute(message, maReference), outputRef);
   //}
   //else if (outputRef != 0 && !node.existChild(lxAutogenerated) && !test(hint, tpConstructor))
   //   //warn if the method output was not redclared, ignore auto generated methods
   //   //!!hotfix : ignore the warning for the constructor
   //   scope.raiseWarning(WARNING_LEVEL_1, wrnTypeInherited, node);
}

//void Compiler :: saveExtension(ClassScope& scope, ref_t message)
//{
//   scope.moduleScope->saveExtension(message, scope.extensionClassRef, scope.reference);
//   if (isOpenArg(message) && _logic->isMethodGeneric(scope.info, message)) {
//      // if it is an extension with open argument list generic handler
//      // creates the references for all possible number of parameters
//      for (int i = 1; i < OPEN_ARG_COUNT; i++) {
//         scope.moduleScope->saveExtension(overwriteParamCount(message, i), scope.extensionClassRef, scope.reference);
//      }
//   }
//}

void Compiler :: generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed/*, bool allowTypeAttribute, bool closureBaseClass*/)
{
   ref_t message = current.argument;

   if (scope.info.methods.exist(message, true) && hideDuplicates) {
      // ignoring autogenerated duplicates
      current = lxIdle;

      return;
   }

   generateMethodAttributes(scope, current, message/*, allowTypeAttribute*/);

   int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(message, maHint));
//   if (isOpenArg(message)) {
//      if (_logic->isMethodGeneric(scope.info, message)) {
//         // HOTFIX : verify that only generics with similar argument signature available
//         int extraParamCount = retrieveGenericArgParamCount(scope);
//         if (extraParamCount != -1 && getParamCount(message) != extraParamCount)
//            scope.raiseError(errIllegalMethod, current);
//
//         scope.info.header.flags |= elWithArgGenerics;
//      }
//   }
//   else if (_logic->isMethodGeneric(scope.info, message)) {
//      scope.info.header.flags |= elWithGenerics;
//   }
//   if (test(methodHints, tpAction) && closureBaseClass) {
//      scope.moduleScope->saveAction(message, scope.reference);
//   }

   // check if there is no duplicate method
   if (scope.info.methods.exist(message, true)) {
      scope.raiseError(errDuplicatedMethod, current);
   }
   else {
//      bool privateOne = test(message, SEALED_MESSAGE);
//      bool castingOne = test(message, CONVERSION_MESSAGE);
      bool included = scope.include(message);
      bool sealedMethod = (methodHints & tpMask) == tpSealed;
      // if the class is closed, no new methods can be declared
      // except private sealed ones (which are declared outside the class VMT)
      if (included && closed/* && !privateOne*/)
         scope.raiseError(errClosedParent, findParent(current, lxClass/*, lxNestedClass*/));

      // if the method is sealed, it cannot be overridden
      if (!included && sealedMethod/* && !castingOne*/)
         scope.raiseError(errClosedMethod, findParent(current, lxClass/*, lxNestedClass*/));

//      // save extensions if required ; private method should be ignored
//      if (test(scope.info.header.flags, elExtension) && !test(methodHints, tpPrivate)) {
//         saveExtension(scope, message);
//      }

      // create overloadlist if required
      if (test(methodHints, tpMultimethod)) {
         NamespaceScope* namespaceScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

         scope.info.methodHints.exclude(Attribute(message, maOverloadlist));
         scope.info.methodHints.add(Attribute(message, maOverloadlist), namespaceScope->mapAnonymous());

         scope.info.header.flags |= elWithMuti;
      }
   }
}

void Compiler :: generateMethodDeclarations(SNode root, ClassScope& scope, bool closed, LexicalType methodType/*, bool closureBaseClass*/)
{
   bool classClassMode = methodType == lxConstructor || methodType == lxStaticMethod;
   bool templateMethods = false;
   List<ref_t> implicitMultimethods;

   // first pass - ignore template based / autogenerated methods
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == methodType) {
         SNode multiMethAttr = current.findChild(lxMultiMethodAttr);
         if (multiMethAttr != lxNone) {
            implicitMultimethods.add(multiMethAttr.argument);
            templateMethods = true;
         }

         if (methodType != lxConstructor) {
            if (!current.existChild(/*lxTemplate, */lxAutogenerated)) {
               generateMethodDeclaration(current, scope, false, closed/*, !classClassMode, closureBaseClass*/);
            }
            else templateMethods = true;
         }
         else generateMethodDeclaration(current, scope, false, closed/*, !classClassMode, false*/);
      }
      current = current.nextNode();
   }

   //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
   if (implicitMultimethods.Count() > 0) {
      _logic->injectVirtualMultimethods(*scope.moduleScope, root, scope.info, *this, implicitMultimethods, methodType);
   }

   if (templateMethods) {
      // third pass - do not include overwritten template-based methods
      current = root.firstChild();
      while (current != lxNone) {
         if (current.existChild(/*lxTemplate, */lxAutogenerated) && (current == methodType)) {
            generateMethodDeclaration(current, scope, true, closed/*, !classClassMode, false*/);
         }
         current = current.nextNode();
      }
   }

   if (!classClassMode && implicitMultimethods.Count() > 0)
      _logic->verifyMultimethods(*scope.moduleScope, root, scope.info, implicitMultimethods);
}

void Compiler :: generateClassDeclaration(SNode node, ClassScope& scope, bool classClassMode/*, bool nestedDeclarationMode*/)
{
   bool closed = test(scope.info.header.flags, elClosed);
//   bool closureBaseClass = false;

   if (classClassMode) {
      if (_logic->isDefaultConstructorEnabled(scope.info)) {
         scope.include(encodeAction(NEWOBJECT_MESSAGE_ID));
      }
   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, node/*, closureBaseClass*/);

//      if (test(scope.info.header.flags, elExtension)) {
//         scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;
//      }

      // generate fields
      generateClassFields(node, scope/*, countFields(node) == 1*/);
   }

//   _logic->injectVirtualCode(*scope.moduleScope, node, scope.reference, scope.info, *this, closed);

   // generate methods
   if (classClassMode) {
      generateMethodDeclarations(node, scope, closed, lxConstructor/*, closureBaseClass*/);
      generateMethodDeclarations(node, scope, closed, lxStaticMethod/*, closureBaseClass*/);
   }
   else generateMethodDeclarations(node, scope, closed, lxClassMethod/*, closureBaseClass*/);

//   // do not set flags for closure declaration - they will be set later
//   if (!nestedDeclarationMode) {
      _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, classClassMode);
//   }
//   else if (test(scope.info.header.flags, elNestedClass)) {
//      // HOTFIX : nested class should be marked as sealed to generate multi-method properly
//      scope.info.header.flags |= elSealed;
//   }
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
      //else if (current == lxReference) {
      //   if (current.nextNode().findNext(lxTerminalMask) != lxNone) {
      //      // HOTFIX : to correctly recognize the method returning type
      //      current = lxClassRefAttr;
      //   }
      //}

      current = current.nextNode();
   }
}

inline SNode findBaseParent(SNode node)
{
   SNode baseNode = /*node.findChild(lxBaseParent)*/SNode(); // !! temporal
   //if (baseNode != lxNone) {
   //   if (baseNode.argument == -1 && existChildWithArg(node, lxBaseParent, 0u)) {
   //      // HOTFIX : allow to override the template parent
   //      baseNode = lxIdle;
   //      baseNode = node.findChild(lxBaseParent);
   //   }
   //}

   return baseNode;
}

//bool Compiler :: isDependentOnNotDeclaredClass(SNode baseNode, ModuleScope& scope)
//{
//   if (baseNode == lxNone)
//      return false;
//
//   ref_t parentRef = resolveParentRef(baseNode, scope, true);
//   if (parentRef == 0)
//      return true;
//
//   // get module reference
//   ref_t moduleRef = 0;
//   _Module* module = scope.project->resolveModule(scope.module->resolveReference(parentRef), moduleRef);
//
//   if (module == NULL || moduleRef == 0)
//      return true;
//
//   // load parent meta data
//   return module->mapSection(moduleRef | mskMetaRDataRef, true) == NULL;
//}

void Compiler :: compileClassDeclaration(SNode node, ClassScope& scope)
{
   compileParentDeclaration(findBaseParent(node), scope);

   declareClassAttributes(node, scope);

   declareVMT(node, scope);

   generateClassDeclaration(node, scope, false);

   if (_logic->isRole(scope.info)) {
      // class is its own class class
      scope.info.header.classRef = scope.reference;
   }
//   else if (_logic->isAbstract(scope.info)) {
//      scope.info.header.classRef = 0;
//   }
   else {
      // define class class name
      IdentifierString classClassName(scope.moduleScope->resolveFullName(scope.reference));
      classClassName.append(CLASSCLASS_POSTFIX);

      scope.info.header.classRef = scope.moduleScope->module->mapReference(classClassName);
   }

   // if it is a super class validate it
   if (scope.info.header.parentRef == 0 && scope.reference == scope.moduleScope->superReference) {
      if (!scope.info.methods.exist(encodeAction(DISPATCH_MESSAGE_ID)))
         scope.raiseError(errNoDispatcher, node.findChild(lxIdentifier, lxPrivate));
   }

   // save declaration
   scope.save();

   // compile class class if it available
   if (scope.info.header.classRef != scope.reference && scope.info.header.classRef != 0) {
      ClassScope classClassScope((NamespaceScope*)scope.parent, scope.info.header.classRef);
      classClassScope.info.header.flags |= (elClassClass | elFinal); // !! IMPORTANT : classclass / final flags should be set

      compileClassClassDeclaration(node, classClassScope, scope);
   }
}

void Compiler :: generateClassImplementation(SNode node, ClassScope& scope)
{
//   WarningScope warningScope(scope.moduleScope->warningMask);
//
//   analizeClassTree(node, scope, warningScope);

   pos_t sourcePathRef = scope.saveSourcePath(_writer);

   CommandTape tape;
   _writer.generateClass(tape, node, sourcePathRef);

   // optimize
   optimizeTape(tape);

   //// create byte code sections
   //scope.save();
   _writer.save(tape, *scope.moduleScope, sourcePathRef);
}

void Compiler :: compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

//   if (test(scope.info.header.flags, elExtension)) {
//      scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;
//
//      scope.embeddable = _logic->isEmbeddable(*scope.moduleScope, scope.extensionClassRef);
//   }
//   else if (_logic->isEmbeddable(scope.info)) {
//      scope.embeddable = true;
//   }

   writer.newNode(lxClass, node.argument);
   compileVMT(writer, node, scope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (/*scope.extensionClassRef == 0 && */scope.info.header.classRef != 0) {
      compileSymbolCode(scope);
   }
}

void Compiler :: compileSymbolDeclaration(SNode node, SymbolScope& scope)
{
//   declareSymbolAttributes(node, scope);
//
//   if ((scope.constant || scope.outputRef != 0) && scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, true) == false) {
//      scope.save();
//   }
}

//bool Compiler :: compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal, bool accumulatorMode)
//{
//   ref_t parentRef = 0;
//
//   _Module* module = scope.moduleScope->module;
//   MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));
//
//   if (accumulatorMode) {
//      if (dataWriter.Position() == 0)
//         dataWriter.Memory()->addReference(scope.moduleScope->arrayReference | mskVMTRef, (ref_t)-4);
//
//      if (retVal.kind == okSignatureConstant) {
//         dataWriter.Memory()->addReference(retVal.param | mskSignature, dataWriter.Position());
//
//         dataWriter.writeDWord(0);
//      }
//      else {
//         SymbolScope memberScope(scope.moduleScope, scope.moduleScope->mapAnonymous());
//         if (!compileSymbolConstant(node, memberScope, retVal))
//            return false;
//
//         dataWriter.Memory()->addReference(memberScope.reference | mskConstantRef, dataWriter.Position());
//         dataWriter.writeDWord(0);
//      }
//   }
//   else {
//      if (retVal.kind == okIntConstant || retVal.kind == okUIntConstant) {
//         size_t value = module->resolveConstant(retVal.param).toULong(16);
//
//         dataWriter.writeDWord(value);
//
//         parentRef = scope.moduleScope->intReference;
//      }
//      else if (retVal.kind == okLongConstant) {
//         long long value = module->resolveConstant(retVal.param).toULongLong(10, 1);
//
//         dataWriter.write(&value, 8u);
//
//         parentRef = scope.moduleScope->longReference;
//      }
//      else if (retVal.kind == okRealConstant) {
//         double value = module->resolveConstant(retVal.param).toDouble();
//
//         dataWriter.write(&value, 8u);
//
//         parentRef = scope.moduleScope->realReference;
//      }
//      else if (retVal.kind == okLiteralConstant) {
//         ident_t value = module->resolveConstant(retVal.param);
//
//         dataWriter.writeLiteral(value, getlength(value) + 1);
//
//         parentRef = scope.moduleScope->literalReference;
//      }
//      else if (retVal.kind == okWideLiteralConstant) {
//         WideString wideValue(module->resolveConstant(retVal.param));
//
//         dataWriter.writeLiteral(wideValue, getlength(wideValue) + 1);
//
//         parentRef = scope.moduleScope->wideReference;
//      }
//      else if (retVal.kind == okCharConstant) {
//         ident_t value = module->resolveConstant(retVal.param);
//
//         dataWriter.writeLiteral(value, getlength(value));
//
//         parentRef = scope.moduleScope->charReference;
//      }
//      else if (retVal.kind == okObject) {
//         SNode root = node.findSubNodeMask(lxObjectMask);
//
//         if (root == lxConstantList && !accumulatorMode) {
//            SymbolExpressionInfo info;
//            info.expressionClassRef = scope.outputRef;
//            info.constant = scope.constant;
//            info.listRef = root.argument;
//
//            // save class meta data
//            MemoryWriter metaWriter(scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, false), 0);
//            info.save(&metaWriter);
//
//            return true;
//         }
//         else return false;
//      }
//      else return false;
//
//      dataWriter.Memory()->addReference(parentRef | mskVMTRef, (ref_t)-4);
//
//      if (parentRef == scope.moduleScope->intReference) {
//         scope.moduleScope->defineConstantSymbol(scope.reference, V_INT32);
//      }
//      else scope.moduleScope->defineConstantSymbol(scope.reference, parentRef);
//   }
//
//   return true;
//}

void Compiler :: compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

//   declareSymbolAttributes(node, scope);

   bool isStatic = scope.staticOne;

   SNode expression = node.findChild(lxExpression);

   CodeScope codeScope(&scope);

   writer.newNode(lxSymbol, node.argument);
   writer.newNode(lxExpression);
   writer.appendNode(lxBreakpoint, dsStep);
   /*ObjectInfo retVal = */compileExpression(writer, expression, codeScope, scope.outputRef, /*isSingleStatement(expression) ? HINT_ROOTSYMBOL : */0);
//   else if (resolveObjectReference(*scope.moduleScope, retVal) != 0) {
//      // HOTFIX : if the result of the operation is qualified - it should be saved as symbol type
//      scope.outputRef = resolveObjectReference(*scope.moduleScope, retVal);
//      scope.save();
//   }
//
   writer.closeNode();
   writer.closeNode();

//   analizeSymbolTree(expressionTree.readRoot(), scope, scope.moduleScope->warningMask);
//   node.refresh();
//
//   // create constant if required
//   if (scope.constant) {
//      // static symbol cannot be constant
//      if (isStatic)
//         scope.raiseError(errInvalidOperation, expression);
//
//      if (!compileSymbolConstant(expressionTree.readRoot(), scope, retVal))
//         scope.raiseError(errInvalidOperation, expression);
//   }
//
//   if (scope.preloaded) {
//      compilePreloadedCode(scope);
//   }

   pos_t sourcePathRef = scope.saveSourcePath(_writer);

   CommandTape tape;
   _writer.generateSymbol(tape, expressionTree.readRoot(), isStatic, sourcePathRef);

   // optimize
   optimizeTape(tape);

   // create byte code sections
   _writer.save(tape, *scope.moduleScope, sourcePathRef);
}

//void Compiler :: compileStaticAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, int mode*/)
//{
//   SyntaxTree expressionTree;
//   SyntaxWriter writer(expressionTree);
//
//   CodeScope codeScope(&scope);
//
//   writer.newNode(lxExpression);
//   writer.newNode(lxAssigning);
//   if (isPrimitiveRef(target.param)) {
//      writeTerminal(writer, node, codeScope, ObjectInfo(okClassStaticField, scope.reference, target.param, target.extraparam), HINT_NODEBUGINFO);
//   }
//   else writeTerminal(writer, node, codeScope, target, HINT_NODEBUGINFO);
//
//   writer.newBookmark();
//   ObjectInfo source = compileExpression(writer, node, codeScope, 0);
//   convertObject(writer, *scope.moduleScope, target.extraparam, resolveObjectReference(codeScope, source), source.element);
//
//   writer.removeBookmark();
//   writer.closeNode();
//   writer.closeNode();
//
//   analizeSymbolTree(expressionTree.readRoot(), scope, scope.moduleScope->warningMask);
//
//   compilePreloadedCode(scope, expressionTree.readRoot());
//}
//
//// NOTE : elementRef is used for binary arrays
//ObjectInfo Compiler :: assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t elementRef)
//{
//   ObjectInfo retVal(okObject, targetRef, 0, elementRef);
//
//   int size = _logic->defineStructSize(*scope.moduleScope, targetRef, elementRef);
//   if (size != 0) {
//      if (allocateStructure(scope, size, false, retVal)) {
//         retVal.extraparam = targetRef;
//
//         writer.insertChild(0, lxLocalAddress, retVal.param);
//         writer.appendNode(lxTempAttr);
//         writer.insert(lxAssigning, size);
//         writer.closeNode();
//      }
//      else if (size > 0) {
//         writer.appendNode(lxTarget, targetRef);
//         writer.insert(lxCreatingStruct, size);
//         writer.closeNode();
//
//         writer.insert(lxAssigning, size);
//         writer.closeNode();
//      }
//      
//      switch (targetRef) {
//         case V_INT32:
//            targetRef = scope.moduleScope->intReference;
//            break;
//         case V_INT64:
//            targetRef = scope.moduleScope->longReference;
//            break;
//         case V_REAL64:
//            targetRef = scope.moduleScope->realReference;
//            break;
//         case V_SIGNATURE:
//            targetRef = scope.moduleScope->signatureReference;
//            break;
//         case V_MESSAGE:
//            targetRef = scope.moduleScope->messageReference;
//            break;
//      }
//
//      writer.appendNode(lxTarget, targetRef);
//      writer.appendNode(lxBoxableAttr);
//      writer.insert(lxBoxing,  size);
//      writer.closeNode();
//
//      retVal.kind = okObject;
//      retVal.param = targetRef;
//
//      return retVal;
//   }
//   else return retVal;
//}
//
//ref_t Compiler :: analizeExtCall(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   compileExternalArguments(node, scope, warningScope);
//
//   if (test(mode, HINT_INT64EXPECTED)) {
//      return V_INT64;
//   }
//   else if (test(mode, HINT_REAL64EXPECTED)) {
//      // HOTFIX : to recognize external function returning real number
//      node.appendNode(lxFPUTarget);
//
//      return V_REAL64;
//   }
//   else return V_INT32;
//}
//
//ref_t Compiler :: analizeInternalCall(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   analizeExpressionTree(node, scope, warningScope, HINT_NOBOXING);
//
//   return V_INT32;
//}
//
//ref_t Compiler :: analizeArgUnboxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int)
//{
//   analizeExpressionTree(node, scope, warningScope, HINT_NOBOXING);
//
//   return 0;
//}
//
//int Compiler :: allocateStructure(SNode node, int& size)
//{
//   // finding method's reserved attribute
//   SNode methodNode = node.parentNode();
//   while (methodNode != lxClassMethod)
//      methodNode = methodNode.parentNode();
//
//   SNode reserveNode = methodNode.findChild(lxReserved);
//   int reserved = reserveNode.argument;
//
//   // allocating space
//   int offset = allocateStructure(false, size, reserved);
//
//   // HOT FIX : size should be in bytes
//   size *= 4;
//
//   reserveNode.setArgument(reserved);
//
//   return offset;
//}
//
//ref_t Compiler :: analizeNestedExpression(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   // check if the nested collection can be treated like constant one
//   bool constant = true;
//   ref_t memberCounter = 0;
//   SNode current = node.firstChild();
//   while (constant && current != lxNone) {
//      if (current == lxMember) {
//         SNode object = current.findSubNodeMask(lxObjectMask);
//         switch (object.type) {
//            case lxConstantChar:
//            case lxConstantClass:
//            case lxConstantInt:
//            case lxConstantLong:
//            case lxConstantList:
//            case lxConstantReal:
//            case lxConstantString:
//            case lxConstantWideStr:
//            case lxConstantSymbol:
//               break;
//            case lxNested:
//               analizeNestedExpression(object, scope, warningScope);
//               object.refresh();
//               if (object != lxConstantList)
//                  constant = false;
//
//               break;
//            case lxUnboxing:
//               current = lxOuterMember;
//               analizeBoxing(object, scope, warningScope, HINT_NOUNBOXING);
//               constant = false;
//               break;
//            default:
//               constant = false;
//               analizeExpressionTree(current, scope, warningScope);
//               break;
//         }
//         memberCounter++;
//      }
//      else if (current == lxOuterMember) {
//         // nested class with outer member must not be constant
//         constant = false;
//
//         analizeExpression(current, scope, warningScope);
//      }
//      else if (current == lxOvreriddenMessage) {
//         constant = false;
//      }
//      current = current.nextNode();
//   }
//
//   if (node.argument != memberCounter)
//      constant = false;
//
//   // replace with constant array if possible
//   if (constant && memberCounter > 0) {
//      ref_t reference = scope.mapAnonymous();
//
//      node = lxConstantList;
//      node.setArgument(reference | mskConstArray);
//
//      _writer.generateConstantList(node, scope.module, reference);
//   }
//
//   return node.findChild(lxTarget).argument;
//}
//
//ref_t Compiler :: analizeMessageCall(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   int mode = 0;
//
//   if (node.existChild(lxStacksafeAttr)) {
//      mode |= HINT_NOBOXING;
//   }
//
//   if (node.existChild(lxEmbeddableAttr)) {
//      if (!_logic->optimizeEmbeddable(node, scope))
//         node.appendNode(lxEmbeddable);
//   }
//
//   analizeExpressionTree(node, scope, warningScope, mode);
//
//   return node.findChild(lxTarget).argument;
//}
//
//ref_t Compiler :: analizeAssigning(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   //ref_t targetRef = node.findChild(lxTarget).argument;
//   SNode targetNode = node.firstChild(lxObjectMask);
//   SNode sourceNode = targetNode.nextNode(lxObjectMask);
//
//   ref_t sourceRef = analizeExpression(sourceNode, scope, warningScope,node.argument != 0 ? HINT_NOBOXING | HINT_NOUNBOXING : HINT_NOUNBOXING);
//
//   if (node.argument != 0) {
//      SNode intValue = node.findSubNode(lxConstantInt);
//      if (intValue != lxNone && node.argument == 4) {
//         // direct operation with numeric constants
//         node.set(lxIntOp, SET_MESSAGE_ID);
//      }
//      else {
//         SNode subNode = node.findSubNode(lxDirectCalling, lxSDirctCalling, lxAssigning);
//         if (subNode == lxAssigning) {
//            bool tempAttr = subNode.existChild(lxTempAttr);
//
//            // assignment operation
//            SNode operationNode = subNode.findChild(lxIntOp, lxRealOp, lxLongOp, lxIntArrOp, lxByteArrOp, lxShortArrOp);
//            if (operationNode != lxNone) {
//               SNode larg = operationNode.findSubNodeMask(lxObjectMask);
//               SNode rarg = operationNode.firstChild(lxObjectMask).nextSubNodeMask(lxObjectMask);
//               SNode target = node.firstChild(lxObjectMask);
//               if (rarg.type == targetNode.type && rarg.argument == targetNode.argument) {
//                  // if the target is used in the subexpression rvalue
//                  // do nothing
//               }
//               // if it is an operation with the same target
//               else if (larg.type == target.type && larg.argument == target.argument) {
//                  // remove an extra assignment
//                  larg = subNode.findSubNodeMask(lxObjectMask);
//
//                  larg = target.type;
//                  larg.setArgument(target.argument);
//                  node = lxExpression;
//                  target = lxIdle;
//
//                  // replace add / subtract with append / reduce and remove an assignment
//                  switch (operationNode.argument) {
//                     case ADD_MESSAGE_ID:
//                        operationNode.setArgument(APPEND_MESSAGE_ID);
//                        subNode = lxExpression;
//                        larg = lxIdle;
//                        break;
//                     case SUB_MESSAGE_ID:
//                        operationNode.setArgument(REDUCE_MESSAGE_ID);
//                        subNode = lxExpression;
//                        larg = lxIdle;
//                        break;
//                  }
//               }
//               // if it is an operation with an extra temporal variable
//               else if ((node.argument == subNode.argument || operationNode == lxByteArrOp || operationNode == lxShortArrOp) && tempAttr) {
//                  larg = subNode.findSubNodeMask(lxObjectMask);
//
//                  if ((larg.type == targetNode.type && larg.argument == targetNode.argument) || (tempAttr && subNode.argument == node.argument && larg == lxLocalAddress)) {
//                     // remove an extra assignment
//                     subNode = lxExpression;
//                     larg = lxIdle;
//                  }
//               }
//            }
//            else if (tempAttr && subNode.argument == node.argument) {
//               SNode larg = subNode.firstChild(lxObjectMask);
//               if (larg == lxLocalAddress) {
//                  // remove an extra assignment
//                  subNode = lxExpression;
//                  larg = lxIdle;
//               }
//            }
//         }
//         else if (subNode != lxNone) {
//            if (subNode.existChild(lxEmbeddable)) {
//               if (!_logic->optimizeEmbeddableGet(scope, *this, node)) {
//                  _logic->optimizeEmbeddableOp(scope, *this, node);
//               }
//            }
//            else if (subNode.existChild(lxBoxableAttr) && subNode.existChild(lxStacksafeAttr)) {
//               SNode createNode = subNode.findChild(lxCreatingStruct);
//               if (createNode != lxNone && targetNode == lxLocalAddress) {
//                  // if it is implicit conversion
//                  createNode.set(targetNode.type, targetNode.argument);
//
//                  node = lxExpression;
//                  targetNode = lxIdle;
//               }
//            }
//         }
//      }
//   }
//
//   return sourceRef;
//}
//
//ref_t Compiler :: analizeBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   if (node == lxCondBoxing && test(mode, HINT_NOCONDBOXING))
//      node = lxBoxing;
//
//   if (node == lxUnboxing && test(mode, HINT_NOUNBOXING))
//      node = lxBoxing;
//
//   ref_t targetRef = node.findChild(lxTarget).argument;
//   ref_t sourceRef = 0;
//   bool boxing = !test(mode, HINT_NOBOXING);
//
//   // HOTFIX : override the stacksafe attribute if the object must be boxed
//   if (!boxing && node.existChild(lxBoxingRequired))
//      boxing = true;
//
//   SNode sourceNode = node.findSubNodeMask(lxObjectMask);
//   if (sourceNode == lxBoxing) {
//      SNode newNode = sourceNode.findSubNodeMask(lxObjectMask);
//      if (newNode == lxNewOp) {
//         // HOTFIX : boxing inside boxing
//         sourceNode = lxExpression;
//
//         sourceNode = newNode;
//      }
//   }
//
//   if (sourceNode == lxNewOp) {
//      // HOTFIX : set correct target for the new operator
//      sourceNode.setArgument(targetRef);
//
//      analizeExpression(sourceNode, scope, warningScope, HINT_NOBOXING);
//
//      boxing = false;
//   }
//   else {
//      // for boxing stack allocated / embeddable variables - source is the same as target
//      if ((sourceNode == lxLocalAddress || sourceNode == lxFieldAddress || sourceNode == lxLocal || sourceNode == lxThisLocal) && node.argument != 0) {
//         sourceRef = targetRef;
//      }
//      // HOTFIX : do not box constant classes
//      else if (sourceNode == lxConstantInt && targetRef == scope.intReference) {
//         boxing = false;
//      }
//      else if (sourceNode == lxConstantReal && targetRef == scope.realReference) {
//         boxing = false;
//      }
//      else if (sourceNode == lxConstantSymbol && targetRef == scope.intReference) {
//         boxing = false;
//      }
//      else if (sourceNode == lxMessageConstant && targetRef == scope.messageReference) {
//         boxing = false;
//      }
//      else if (sourceNode == lxSignatureConstant && targetRef == scope.signatureReference) {
//         boxing = false;
//      }
//      else if (node == lxUnboxing && !boxing) {
//         //HOTFIX : to unbox structure field correctly
//         sourceRef = analizeExpression(sourceNode, scope, warningScope, HINT_NOBOXING | HINT_UNBOXINGEXPECTED);
//      }
//      else {
//         if ((sourceNode == lxBoxing || sourceNode == lxUnboxing) && (int)node.argument < 0 && (int)sourceNode.argument > 0) {
//            //HOTFIX : boxing fixed-sized array
//            if (sourceNode.existChild(lxFieldAddress)) {
//               node.setArgument(-((int)sourceNode.argument / (int)node.argument));
//            }
//         }
//
//         int subMode = HINT_NOBOXING;
//         if (targetRef == scope.longReference) {
//            subMode |= HINT_INT64EXPECTED;
//         }
//         else if (targetRef == scope.realReference) {
//            subMode |= HINT_REAL64EXPECTED;
//         }
//
//         sourceRef = analizeExpression(sourceNode, scope, warningScope, subMode);
//      }
//
//      // adjust primitive target
//      if (_logic->isPrimitiveRef(targetRef) && boxing) {
//         targetRef = _logic->resolvePrimitiveReference(scope, targetRef);
//         node.findChild(lxTarget).setArgument(targetRef);
//      }
//
//      if (!_logic->validateBoxing(scope, *this, node, targetRef, sourceRef, test(mode, HINT_UNBOXINGEXPECTED))) {
//         scope.raiseError(errIllegalOperation, node);
//      }
//   }
//
//   if (!boxing && node != lxLocalUnboxing) {
//      node = lxExpression;
//   }
//
//   return targetRef;
//}
//
//ref_t Compiler :: analizeArgBoxing(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   bool boxing = !test(mode, HINT_NOBOXING);
//
//   // HOTFIX : override the stacksafe attribute if the object must be boxed
//   if (!boxing && node.existChild(lxBoxingRequired))
//      boxing = true;
//
//   if (!boxing)
//      node = lxExpression;
//
//   analizeExpressionTree(node, scope, warningScope, HINT_NOBOXING);
//
//   return scope.arrayReference;
//}
//
//ref_t Compiler :: analizeSymbol(SNode& node, ModuleScope& scope, WarningScope&)
//{
//   ObjectInfo result = scope.defineObjectInfo(node.argument, true);
//   switch (result.kind) {
//      case okConstantClass:
//         node = lxConstantClass;
//         break;
//      case okConstantSymbol:
//         node = lxConstantSymbol;
//         break;
//   }
//
//   return resolveObjectReference(scope, result);
//}
//
//ref_t Compiler :: analizeOp(SNode current, ModuleScope& scope, WarningScope& warningScope)
//{
//   int lmask = HINT_NOBOXING;
//   if (current.argument == REFER_MESSAGE_ID) {
//      switch (current.type) {
//         case lxIntArrOp:
//         case lxByteArrOp:
//         case lxShortArrOp:
//         case lxBinArrOp:
//            lmask |= HINT_NOUNBOXING;
//            break;
//         default:
//            break;
//      }
//   }
//
//   SNode loperand = current.firstChild(lxObjectMask);
//   analizeExpression(loperand, scope, warningScope, lmask);
//
//   SNode roperand = loperand.nextNode(lxObjectMask);
//   analizeExpression(roperand, scope, warningScope, HINT_NOBOXING);
//
//   SNode roperand2 = roperand.nextNode(lxObjectMask);
//   if (roperand2 != lxNone)
//      analizeExpression(roperand2, scope, warningScope, HINT_NOBOXING);
//
//   if (current == lxIntOp && loperand == lxConstantInt && roperand == lxConstantInt) {
//      int val = 0;
//      if (calculateIntOp(current.argument, loperand.findChild(lxIntValue).argument, roperand.findChild(lxIntValue).argument, val)) {
//         loperand = lxIdle;
//         roperand = lxIdle;
//
//         IdentifierString str;
//         str.appendHex(val);
//         current.set(lxConstantInt, scope.module->mapConstant(str.c_str()));
//         current.appendNode(lxIntValue, val);
//
//         return V_INT32;
//      }
//   }
//   else if (current == lxRealOp && loperand == lxConstantReal && roperand == lxConstantReal) {
//      double d1 = scope.module->resolveConstant(loperand.argument).toDouble();
//      double d2 = scope.module->resolveConstant(roperand.argument).toDouble();
//      double val = 0;
//      if (calculateRealOp(current.argument, d1, d2, val)) {
//         loperand = lxIdle;
//         roperand = lxIdle;
//
//         IdentifierString str;
//         str.appendDouble(val);
//         current.set(lxConstantReal, scope.module->mapConstant(str.c_str()));
//
//         return V_REAL64;
//      }
//   }
//
//   switch (current) {
//      case lxIntOp:
//      case lxByteArrOp:
//      case lxIntArrOp:
//      case lxShortArrOp:
//         return V_INT32;
//      case lxLongOp:
//         return V_INT64;
//      case lxRealOp:
//         return V_REAL64;
//      case lxBinArrOp:
//         return V_BINARY;
//      case lxNewOp:
//         return current.argument;
//      default:
//         return 0;
//   }
//}
//
//ref_t Compiler ::analizeExpression(SNode current, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   switch (current.type) {
//      case lxCalling:
//      case lxDirectCalling:
//      case lxSDirctCalling:
//         return analizeMessageCall(current, scope, warningScope);
//      case lxExpression:
//      case lxReturning:
//         return analizeExpression(current.firstChild(lxObjectMask), scope, warningScope, mode);
//      case lxAltExpression:
//      case lxBranching:
//      case lxTrying:
//         analizeExpressionTree(current, scope, warningScope);
//         return 0;
//      case lxBoxing:
//      case lxCondBoxing:
//      case lxUnboxing:
//         return analizeBoxing(current, scope, warningScope, mode);
//      case lxArgBoxing:
//         return analizeArgBoxing(current, scope, warningScope, mode);
//      case lxArgUnboxing:
//         return analizeArgUnboxing(current, scope, warningScope, mode);
//      case lxAssigning:
//         return analizeAssigning(current, scope, warningScope);
//      case lxSymbolReference:
//         return analizeSymbol(current, scope, warningScope);
//      case lxIntOp:
//      case lxLongOp:
//      case lxRealOp:
//      case lxIntArrOp:
//      case lxShortArrOp:
//      case lxByteArrOp:
//      case lxArrOp:
//      case lxBinArrOp:
//      case lxNewOp:
//      case lxArgArrOp:
//      case lxBoolOp:
//         return analizeOp(current, scope, warningScope);
//      case lxInternalCall:
//         return analizeInternalCall(current, scope, warningScope);
//      case lxStdExternalCall:
//      case lxExternalCall:
//      case lxCoreAPICall:
//         return analizeExtCall(current, scope, warningScope, mode);
//      case lxLooping:
//      case lxSwitching:
//      case lxOption:
//      case lxElse:
//         analizeExpressionTree(current, scope, warningScope);
//         return 0;
//      case lxNested:
//         return analizeNestedExpression(current, scope, warningScope);
//      default:
//         return current.findChild(lxTarget).argument;
//   }
//}
//
//void Compiler :: analizeBranching(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   analizeExpressionTree(node, scope, warningScope, mode);
//
//   _logic->optimizeBranchingOp(scope, node);
//}
//
//void Compiler :: analizeExpressionTree(SNode node, ModuleScope& scope, WarningScope& warningScope, int mode)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxElse:
//         case lxCode:
//         case lxIf:
//         case lxExternFrame:
//            analizeExpressionTree(current, scope, warningScope);
//            break;
//         case lxBranching:
//         case lxLooping:
//            analizeBranching(current, scope, warningScope);
//            break;
//         default:
//            if (test(current.type, lxObjectMask)) {
//               analizeExpression(current, scope, warningScope, mode);
//            }
//            break;
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: analizeCode(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      switch (current.type) {
//         case lxReturning:
//            analizeExpressionTree(current, scope, warningScope, HINT_NOUNBOXING | HINT_NOCONDBOXING);
//            break;
//         case lxExpression:
//         case lxExternFrame:
//         case lxDirectCalling:
//         case lxSDirctCalling:
//         case lxCalling:
//            analizeExpressionTree(current, scope, warningScope);
//            break;
//         default:
//            if (test(current.type, lxObjectMask)) {
//               analizeExpression(current, scope, warningScope, 0);
//            }
//            break;
//      }
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: analizeMethod(SNode node, ModuleScope& scope, WarningScope& warningScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxNewFrame) {
//         analizeCode(current, scope, warningScope);
//      }
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: analizeClassTree(SNode node, ClassScope& scope, WarningScope& warningScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxClassMethod) {
////         SNode mask = current.findChild(lxWarningMask);
////         if (mask != lxNone)
////            warningScope.warningMask = mask.argument;
//
//         analizeMethod(current, *scope.moduleScope, warningScope);
//
//         if (test(_optFlag, 1)) {
//            if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
//               defineEmbeddableAttributes(scope, current);
//            }
//         }
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: analizeSymbolTree(SNode node, Scope& scope, int warningMask)
//{
//   WarningScope warningScope(warningMask);
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      /*if (current == lxWarningMask) {
//         warningMask = current.argument;
//      }
//      else */if (test(current.type, lxExprMask)) {
//         analizeExpressionTree(current, *scope.moduleScope, warningScope, HINT_NOUNBOXING);
//      }
//
//      current = current.nextNode();
//   }
//}
//
//void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
//{
//   // Optimization : var = get&subject => eval&subject&var[1]
//   ref_t type = 0;
//   ref_t returnRef = classScope.info.methodHints.get(ClassInfo::Attribute(methodNode.argument, maReference));
//   if (_logic->recognizeEmbeddableGet(*classScope.moduleScope, methodNode, classScope.extensionClassRef != 0 ? classScope.reference : 0, returnRef, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGet), type);
//
//      // HOTFIX : allowing to recognize embeddable get in the class itself
//      classScope.save();
//   }
//   // Optimization : var = getAt&int => read&int&subject&var[2]
//   else if (_logic->recognizeEmbeddableGetAt(*classScope.moduleScope, methodNode, classScope.extensionClassRef != 0 ? classScope.reference : 0, returnRef, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGetAt), type);
//
//      // HOTFIX : allowing to recognize embeddable get in the class itself
//      classScope.save();
//   }
//   // Optimization : var = getAt&int => read&int&subject&var[2]
//   else if (_logic->recognizeEmbeddableGetAt2(*classScope.moduleScope, methodNode, classScope.extensionClassRef != 0 ? classScope.reference : 0, returnRef, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableGetAt2), type);
//
//      // HOTFIX : allowing to recognize embeddable get in the class itself
//      classScope.save();
//   }
//   // Optimization : var = eval&subj&int => eval&subj&var[2]
//   else if (_logic->recognizeEmbeddableEval(*classScope.moduleScope, methodNode, classScope.extensionClassRef != 0 ? classScope.reference : 0, returnRef, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableEval), type);
//
//      // HOTFIX : allowing to recognize embeddable get in the class itself
//      classScope.save();
//   }
//   // Optimization : var = eval&int&int => evald&int&subject&var[2]
//   else if (_logic->recognizeEmbeddableEval2(*classScope.moduleScope, methodNode, classScope.extensionClassRef != 0 ? classScope.reference : 0, returnRef, type)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableEval2), type);
//
//      // HOTFIX : allowing to recognize embeddable get in the class itself
//      classScope.save();
//   }
//
//   // Optimization : subject'get = self / $self
//   if (_logic->recognizeEmbeddableIdle(methodNode, classScope.extensionClassRef != 0)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableIdle), INVALID_REF);
//
//      classScope.save();
//   }
//
//   // Optimization : embeddable constructor call
//   ref_t message = 0;
//   if (_logic->recognizeEmbeddableMessageCall(methodNode, message)) {
//      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableNew), message);
//
//      classScope.save();
//   }
//}
//
//void Compiler :: compileForward(SNode ns, ModuleScope& scope)
//{
//   ident_t shortcut = ns.findChild(lxIdentifier, lxReference).identifier();
//   ident_t reference = ns.findChild(lxForward).findChild(lxIdentifier, lxReference).identifier();
//
//   if (!scope.defineForward(shortcut, reference))
//      scope.raiseError(errDuplicatedDefinition, ns);
//}
//
//bool Compiler :: validate(_ProjectManager& project, _Module* module, int reference)
//{
//   int   mask = reference & mskAnyRef;
//   ref_t extReference = 0;
//   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
//   _Module* extModule = project.resolveModule(refName, extReference, true);
//
//   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
//}

void Compiler :: validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project)
{
   //for (List<Unresolved>::Iterator it = unresolveds.start() ; !it.Eof() ; it++) {
   //   if (!validate(project, (*it).module, (*it).reference)) {
   //      ident_t refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);

   //      project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
   //   }
   //}
}

//inline void addPackageItem(SyntaxWriter& writer, _Module* module, ident_t str)
//{
//   writer.newNode(lxMember);
//   if (!emptystr(str)) {
//      writer.appendNode(lxConstantString, module->mapConstant(str));
//   }
//   else writer.appendNode(lxNil);
//   writer.closeNode();
//}
//
//inline ref_t mapForwardRef(_Module* module, _ProjectManager& project, ident_t forward)
//{
//   ident_t name = project.resolveForward(forward);
//
//   return emptystr(name) ? 0 : module->mapReference(name);
//}
//
//void Compiler :: createPackageInfo(_Module* module, _ProjectManager& project)
//{
//   ReferenceNs sectionName(module->Name(), PACKAGE_SECTION);
//   ref_t reference = module->mapReference(sectionName);
//   ref_t vmtReference = mapForwardRef(module, project, SUPER_FORWARD);
//   if (vmtReference == 0)
//      return;
//
//   SyntaxTree tree;
//   SyntaxWriter writer(tree);
//
//   writer.newNode(lxConstantList, reference);
//   writer.appendNode(lxTarget, vmtReference);
//
//   // namespace
//   addPackageItem(writer, module, module->Name());
//
//   // package name
//   addPackageItem(writer, module, project.getManinfestName());
//
//   // package version
//   addPackageItem(writer, module, project.getManinfestVersion());
//
//   // package author
//   addPackageItem(writer, module, project.getManinfestAuthor());
//
//   writer.closeNode();
//
//   _writer.generateConstantList(tree.readRoot(), module, reference);
//}

void Compiler :: compileImplementations(SNode node, NamespaceScope& scope)
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
            scope.moduleScope->loadClassInfo(classScope.info, current.argument, false);

            compileClassImplementation(expressionTree, current, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference && classScope.info.header.classRef != 0) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef/*, classScope.sourcePath*/);
               scope.moduleScope->loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);
               //classClassScope.classClassMode = true;

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

bool Compiler :: compileDeclarations(SNode node, NamespaceScope& scope/*, bool& repeatMode*/)
{
   SNode current = node.firstChild();

   if (scope.moduleScope->superReference == 0)
      scope.raiseError(errNotDefinedBaseClass);

   // first pass - declaration
   bool declared = false;
   //repeatMode = false;
   while (current != lxNone) {
      //      if (scope.mapAttribute(name) != 0)
      //         scope.raiseWarning(WARNING_LEVEL_3, wrnAmbiguousIdentifier, name);

      if (current.argument == 0) {
         // hotfix : ignore already declared classes and symbols
         switch (current) {
   //         //         case lxInclude:
   //         //            compileForward(current, scope);
   //         //            break;
            case lxClass:
               /*if (!isDependentOnNotDeclaredClass(findBaseParent(current), scope))*/ {
                  current.setArgument(/*name == lxNone ? scope.mapNestedExpression() : */scope.mapNewTerminal(current.findChild(lxNameAttr)));
            
                  ClassScope classScope(&scope, current.argument/*, current.findChild(lxSourcePath).identifier()*/);
            
                  // NOTE : the symbol section is created even if the class symbol doesn't exist
                  scope.moduleScope->mapSection(classScope.reference | mskSymbolRef, false);

                  // build class expression tree
                  compileClassDeclaration(current, classScope);
            
                  declared = true;
               }
               //else repeatMode = true;
               break;
            case lxSymbol:
            {
               current.setArgument(/*name == lxNone ? scope.mapNestedExpression() : */scope.mapNewTerminal(current.findChild(lxNameAttr)));

               SymbolScope symbolScope(&scope, current.argument);

               scope.moduleScope->mapSection(symbolScope.reference | mskSymbolRef, false);
               
               // declare symbol
               compileSymbolDeclaration(current, symbolScope);
               declared = true;
               break;
            }
         }
      }
      current = current.nextNode();
   }

   return declared;
}

//void Compiler :: compileSyntaxTree(_ProjectManager& project, ident_t file, SyntaxTree& syntaxTree, ModuleInfo& info, Unresolveds& unresolveds)
//{
//   ModuleScope scope(&project, file, info.codeModule, info.debugModule, &unresolveds);
//
//   project.printInfo("%s", file);
//
//   compileSyntaxTree(syntaxTree, scope);
//}
//
////inline bool isAttribute(_CompilerScope& scope, ref_t subjRef)
////{
////   return (subjRef != 0 && isPrimitiveRef(scope.subjectHints.get(subjRef)));
////}

//bool Compiler :: buildSyntaxTree(SyntaxTree& syntaxTree)
//{
//   SyntaxWriter writer(syntaxTree);
//
//}

//void Compiler :: generateSyntaxTree(ident_t sourcePath, _ProjectManager& project, SyntaxWriter& writer, _DerivationTransformer& builder)
//{
//   ModuleScope scope(&project, info.codeModule, info.debugModule/*, &unresolveds*/);
//
//   builder.generateSyntaxTree(writer, scope, sourcePath);
//}

void Compiler :: declareModule(_ProjectManager& project, SyntaxTree& syntaxTree, CompilerScope& scope/*, Unresolveds& unresolveds, */)
{
   // declare classes several times to ignore the declaration order
   //bool repeatMode = true;
   //bool idle = false;
   //while (repeatMode && !idle) {
   //   idle = true;

      SNode current = syntaxTree.readRoot().firstChild();
      while (current != lxNone) {
         if (current == lxNamespace) {
            NamespaceScope namespaceScope(&scope/*, &project, info.codeModule, info.debugModule*//*, &unresolveds*/);
            namespaceScope.loadNamespaceInfo(current);

            /*idle &= !*/compileDeclarations(current, namespaceScope/*, repeatMode*/);
         }
         current = current.nextNode();
      }
   //}
}

void Compiler :: compileModule(_ProjectManager& project, SyntaxTree& syntaxTree, CompilerScope& scope/*, Unresolveds& unresolveds*/)
{
   SNode current = syntaxTree.readRoot().firstChild();
   while (current != lxNone) {
      if (current == lxNamespace) {
         NamespaceScope namespaceScope(&scope/*, &project, info.codeModule, info.debugModule*//*, &unresolveds*/);
         namespaceScope.loadNamespaceInfo(current);

         compileImplementations(current, namespaceScope);
      }
      current = current.nextNode();
   }
}

void Compiler :: initializeScope(ident_t name, CompilerScope& scope, bool withDebugInfo)
{
   scope.module = scope.project->createModule(name);

   // load predefine messages
   for (MessageMap::Iterator it = _verbs.start(); !it.Eof(); it++) {
      scope.module->mapPredefinedAction(it.key(), *it);
   }

   if (withDebugInfo)
      scope.debugModule = scope.project->createDebugModule(name);

   // cache the frequently used references
   scope.superReference = scope.module->mapReference(/*project->resolveForward(*/SUPER_FORWARD/*)*/);

//   createPackageInfo(info.codeModule, project);
}

//void Compiler :: generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef)
//{
//   MemoryWriter metaWriter(scope.module->mapSection(enumRef | mskRDataRef, false));
//
//   metaWriter.writeDWord(memberRef | mskConstantRef);
//}

//void Compiler :: generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument)
//{
//   MemoryWriter writer(scope.module->mapSection(listRef | mskRDataRef, false));
//
//   _writer.generateConstantMember(writer, type, argument);
//}

void Compiler :: generateOverloadListMember(_CompilerScope& scope, ref_t listRef, ref_t messageRef)
{
   MemoryWriter metaWriter(scope.module->mapSection(listRef | mskRDataRef, false));
   if (metaWriter.Position() == 0) {
      metaWriter.writeRef(0, messageRef);
      metaWriter.writeDWord(0);
      metaWriter.writeDWord(0);
   }
   else {
      metaWriter.insertDWord(0, 0);
      metaWriter.insertDWord(0, messageRef);
      metaWriter.Memory()->addReference(0, 0);
   }
}

void Compiler ::generateClosedOverloadListMember(_CompilerScope& scope, ref_t listRef, ref_t messageRef, ref_t classRef)
{
   MemoryWriter metaWriter(scope.module->mapSection(listRef | mskRDataRef, false));
   if (metaWriter.Position() == 0) {
      metaWriter.writeRef(0, messageRef);
      metaWriter.writeRef(classRef | mskVMTEntryOffset, messageRef);
      metaWriter.writeDWord(0);
   }
   else {
      metaWriter.insertDWord(0, messageRef);
      metaWriter.insertDWord(0, messageRef);
      metaWriter.Memory()->addReference(0, 0);
      metaWriter.Memory()->addReference(classRef | mskVMTEntryOffset, 4);
   }
}

void Compiler :: generateSealedOverloadListMember(_CompilerScope& scope, ref_t listRef, ref_t messageRef, ref_t classRef)
{
   MemoryWriter metaWriter(scope.module->mapSection(listRef | mskRDataRef, false));
   if (metaWriter.Position() == 0) {
      metaWriter.writeRef(0, messageRef);
      metaWriter.writeRef(classRef | mskVMTMethodAddress, messageRef);
      metaWriter.writeDWord(0);
   }
   else {
      metaWriter.insertDWord(0, messageRef);
      metaWriter.insertDWord(0, messageRef);
      metaWriter.Memory()->addReference(0, 0);
      metaWriter.Memory()->addReference(classRef | mskVMTMethodAddress, 4);
   }
}

//ref_t Compiler :: readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader)
//{
//   ref_t memberRef = reader.getDWord() & ~mskAnyRef;
//
//   return importReference(extModule, memberRef, scope.module);
//}
//
//void Compiler :: injectBoxing(SyntaxWriter& writer, _CompilerScope&, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode)
//{
//   if (arrayMode && argument == 0) {
//      // HOTFIX : to iundicate a primitive array boxing
//      writer.appendNode(lxBoxableAttr, -1);
//   }
//   else writer.appendNode(lxBoxableAttr);
//
//   writer.appendNode(lxTarget, targetClassRef);
//   writer.insert(boxingType, argument);
//   writer.closeNode();
//}
//
//void Compiler :: injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, bool stacksafe)
//{
//   writer.insertChildren(0, createOp, createArg, lxTarget, targetClassRef);
//
//   writer.appendNode(lxCallTarget, targetClassRef);
//   writer.appendNode(lxBoxableAttr);
//   if (stacksafe)
//      writer.appendNode(lxStacksafeAttr);
//
//   writer.insert(convertOp, convertArg);
//   writer.closeNode();
//}
//
//void Compiler :: injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject)
//{
//   // removing assinging operation
//   assignNode = lxExpression;
//
//   // move assigning target into the call node
//   SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
//   if (assignTarget != lxNone) {
//      callNode.appendNode(assignTarget.type, assignTarget.argument);
//      assignTarget = lxIdle;
//      callNode.setArgument(encodeMessage(subject, 1));
//   }
//}
//
//void Compiler :: injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb)
//{
//   SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
//
//   if (paramCount == -1 && verb == 0) {
//      assignNode.set(callNode.type, subject);
//      callNode = lxIdle;
//
//      SNode targetNode = assignTarget.findChild(lxTarget);
//      assignNode.appendNode(lxCallTarget, targetNode.argument);
//   }
//   else {
//      // removing assinging operation
//      assignNode = lxExpression;
//
//      // move assigning target into the call node
//
//      if (assignTarget != lxNone) {
//         callNode.appendNode(assignTarget.type, assignTarget.argument);
//         assignTarget = lxIdle;
//         callNode.setArgument(encodeMessage(subject, paramCount));
//      }
//   }
//}
//
//void Compiler :: injectLocalBoxing(SNode node, int size)
//{
//   //HOTFIX : using size variable copy to prevent aligning
//   int dummy = size;
//   int offset = allocateStructure(node, dummy);
//
//   // allocate place for the local copy
//   node.injectNode(node.type, node.argument);
//
//   node.set(lxAssigning, size);
//
//   node.insertNode(lxLocalAddress, offset);
//   node.insertNode(lxTempAttr, 0);
//}
//
////void Compiler :: injectFieldExpression(SyntaxWriter& writer)
////{
////   writer.appendNode(lxResultField);
////
////   writer.insert(lxFieldExpression);
////   writer.closeNode();
////}
//
//void Compiler :: injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t embeddedMessageRef)
//{
//   SNode methNode = classNode.appendNode(lxConstructor, message);
//   methNode.appendNode(lxAttribute, tpEmbeddable);
//   methNode.appendNode(lxEmbeddableMssg, embeddedMessageRef);
//   methNode.appendNode(lxAttribute, tpConstructor);
//   SNode codeNode = methNode.appendNode(lxCode);
//   SNode exprNode = codeNode.appendNode(lxExpression);
//   exprNode.appendNode(lxIdentifier, THIS_VAR);
//   exprNode.appendNode(lxMessage, embeddedMessageRef);
//}

void Compiler :: injectVirtualMultimethod(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType, ref_t parentRef)
{
   ref_t resendMessage = message;
   ref_t actionRef = getAction(message);
   if (!parentRef) {
      int paramCount = getAbsoluteParamCount(message);
      ref_t dummy = 0;
      ident_t actionName = scope.module->resolveAction(actionRef, dummy);

      ref_t signatureLen = 0;
      ref_t signatures[OPEN_ARG_COUNT];

      /*if (paramCount >= OPEN_ARG_COUNT) {
         for (int i = OPEN_ARG_COUNT + 1; i <= paramCount; i++) {
            sign.append('$');
            sign.append(scope.module->resolveReference(scope.superReference));
         }
         sign.append('$');
         sign.append(scope.module->resolveReference(scope.superReference));
      }
      else {*/
         for (int i = 0; i < paramCount; i++) {
            signatureLen++;
            signatures[i] = scope.superReference;
         }
      //}
      ref_t signRef = scope.module->mapAction(actionName, scope.module->mapSignature(signatures, signatureLen, false), false);

      resendMessage = encodeMessage(signRef, paramCount);
   }

   SNode methNode = classNode.appendNode(methodType, message);
   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   methNode.appendNode(lxAttribute, tpMultimethod);
   if (methodType == lxConstructor)
      methNode.appendNode(lxAttribute, tpConstructor);

   //if (actionRef == INVOKE_MESSAGE_ID)
   //   methNode.appendNode(lxAttribute, tpAction);

   SNode codeNode = methNode.appendNode(lxResendExpression, resendMessage);
   if (parentRef)
      codeNode.appendNode(lxTarget, parentRef);
}

//void Compiler :: injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType)
//{
//   ref_t actionRef = getAction(message);
//   IdentifierString sign(scope.module->resolveSubject(actionRef));
//   int paramCount = getAbsoluteParamCount(message);
//   for (int i = OPEN_ARG_COUNT + 1; i <= paramCount; i++) {
//      sign.append('$');
//      sign.append(scope.module->resolveReference(scope.superReference));
//   }
//   sign.append('$');
//   sign.append(scope.module->resolveReference(scope.arrayReference));
//
//   ref_t signRef = scope.module->mapSubject(sign, false);
//   ref_t resendMessage = encodeMessage(signRef, getParamCount(message) + 1);
//
//   SNode methNode = classNode.appendNode(methodType, resendMessage);
//   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
//   methNode.appendNode(lxAttribute, tpArgDispatcher);
//   if (methodType == lxConstructor)
//      methNode.appendNode(lxAttribute, tpConstructor);
//
//   SNode codeNode = methNode.appendNode(lxResendExpression, message);
//   codeNode.appendNode(lxArgDispatcherAttr);
//}
//
//void Compiler :: injectVirtualReturningMethod(_CompilerScope&, SNode classNode, ref_t message, ident_t variable)
//{
//   SNode methNode = classNode.appendNode(lxClassMethod, message);
//   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
//   methNode.appendNode(lxAttribute, tpEmbeddable);
//   methNode.appendNode(lxAttribute, tpSealed);
//
//   SNode expr = methNode.appendNode(lxReturning).appendNode(lxExpression);
//   expr.appendNode(lxIdentifier, variable);
//}

void Compiler :: generateClassSymbol(SyntaxWriter& writer, ClassScope& scope)
{
   CodeScope codeScope(&scope);

   writer.newNode(lxSymbol, scope.reference);
   writeTerminal(writer, SNode(), codeScope, ObjectInfo(okConstantClass, scope.reference, scope.info.header.classRef), HINT_NODEBUGINFO);
   writer.closeNode();
}