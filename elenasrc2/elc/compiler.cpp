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

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- Hint constants ---
constexpr auto HINT_CLOSURE_MASK    = 0xC0008800;

constexpr auto HINT_ROOT            = 0x80000000;
constexpr auto HINT_ROOTSYMBOL      = 0xC0000000;
constexpr auto HINT_NOBOXING        = 0x20000000;
constexpr auto HINT_NOUNBOXING      = 0x10000000;
constexpr auto HINT_EXTERNALOP      = 0x08000000;
constexpr auto HINT_NOCONDBOXING    = 0x04000000;
//#define HINT_EXTENSION_MODE   0x02000000
//#define HINT_COLLECTION_MODE  0x01000000
constexpr auto HINT_LOOP            = 0x00800000;
//#define HINT_SWITCH           0x00400000
////#define HINT_ALT_MODE         0x00200000
//#define HINT_SINGLETON        0x00100000
////#define HINT_EXT_RESENDEXPR   0x00080400
constexpr auto HINT_ASSIGNING_EXPR  = 0x00040000;
constexpr auto HINT_NODEBUGINFO     = 0x00020000;
////#define HINT_PARAMETERSONLY   0x00010000
constexpr auto HINT_SUBCODE_CLOSURE = 0x00008800;
constexpr auto HINT_VIRTUALEXPR     = 0x00004000;
//constexpr auto HINT_ASSIGNTARGET    = 0x00002000;
constexpr auto HINT_INTERNALOP      = 0x00002000;
////#define HINT_RESENDEXPR       0x00000400
//#define HINT_LAZY_EXPR        0x00000200
constexpr auto HINT_DYNAMIC_OBJECT  = 0x00000100;  // indicates that the structure MUST be boxed
constexpr auto HINT_UNBOXINGEXPECTED= 0x00000080;
constexpr auto HINT_PROP_MODE       = 0x00000040;
constexpr auto HINT_SILENT          = 0x00000020;
constexpr auto HINT_FORWARD         = 0x00000010;
constexpr auto HINT_REFOP           = 0x00000008;
//#define HINT_INT64EXPECTED    0x00000004
//#define HINT_REAL64EXPECTED   0x00000002
constexpr auto HINT_NOPRIMITIVES    = 0x00000001;

// scope modes
constexpr auto INITIALIZER_SCOPE    = 0x0000001;   // indicates the constructor or initializer method

typedef Compiler::ObjectInfo                 ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef ClassInfo::Attribute                 Attribute;
typedef _CompilerLogic::ExpressionAttributes ExpressionAttributes;

// --- Auxiliary routines ---

inline bool isPrimitiveArrRef(ref_t reference)
{
   switch (reference) {
      case V_OBJARRAY:
      case V_INT32ARRAY:
      case V_INT16ARRAY:
      case V_INT8ARRAY:
      case V_BINARYARRAY:
         return true;
      default:
         return false;
   }
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

////inline bool existChildWithArg(SNode node, LexicalType type, ref_t arg)
////{
////   SNode current = node.firstChild();
////   while (current != lxNone) {
////      if (current.type == type && current.argument == arg)
////         return true;
////
////      current = current.nextNode();
////   }
////
////   return false;
////}

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

//inline SNode goToNode(SNode current, LexicalType type, ref_t argument)
//{
//   while (current != lxNone && current != type && current.argument != argument)
//      current = current.nextNode();
//
//   return current;
//}
//
//inline bool validateGenericClosure(ref_t* signature, size_t length)
//{
//   for (size_t i = 1; i < length; i++) {
//      if (signature[i - 1] != signature[i])
//         return false;
//   }
//
//   return true;
//}

////inline bool checkNode(SNode node, LexicalType type, ref_t argument)
////{
////   return node == type && node.argument == argument;
////}

inline bool isConstantArguments(SNode node)
{
   if (node == lxNone)
      return true;

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type)
      {
         case lxExpression:
            if (!isConstantArguments(current))
               return false;
            break;
         case lxLiteral:
         case lxWide:
         //case lxCharacter:
         case lxInteger:
         //case lxLong:
         case lxHexInteger:
         //case lxReal:
         //case lxExplicitConst:
         case lxMessage:
            break;
         default:
            return false;
      }

      current = current.nextNode();
   }

   return true;
}

// --- Compiler::NamespaceScope ---

Compiler::NamespaceScope :: NamespaceScope(_ModuleScope* moduleScope/*, ident_t path*/, ident_t ns/*, IdentifierList* imported*//*, bool withFullInfo*/)
   : Scope(moduleScope), constantHints(INVALID_REF), extensions(Pair<ref_t, ref_t>(0, 0))
{
   this->ns.copy(ns);
//   this->sourcePath = path;

   // load private namespaces
   loadExtensions(moduleScope->module->Name(), ns, true);

//   for (auto it = imported->start(); !it.Eof(); it++) {
//      ident_t imported_ns = *it;
//
//      importedNs.add(imported_ns);
//
////      if (withFullInfo) {
////         loadModuleInfo(imported_ns);
////      }         
//   }
}

pos_t Compiler::NamespaceScope :: saveSourcePath(ByteCodeWriter& writer)
{
   return saveSourcePath(writer, sourcePath);
}

pos_t Compiler::NamespaceScope :: saveSourcePath(ByteCodeWriter& writer, ident_t path)
{
   if (!emptystr(path)) {
      int position = moduleScope->savedPaths.get(path);
      if (position == -1) {
         position = writer.writeSourcePath(moduleScope->debugModule, path);

         moduleScope->savedPaths.add(path, position);
      }

      return position;
   }
   else return 0;
}

ObjectInfo Compiler::NamespaceScope :: mapGlobal(ident_t identifier)
{
   if (NamespaceName::isIncluded(FORWARD_MODULE, identifier)) {
      IdentifierString forwardName(FORWARD_PREFIX_NS, identifier + getlength(FORWARD_MODULE) + 1);

      // if it is a forward reference
      return defineObjectInfo(moduleScope->mapFullReference(forwardName.c_str(), false), false);
   }
   
   // if it is an existing full reference
   ref_t reference = moduleScope->mapFullReference(identifier, true);
   if (reference) {
      return defineObjectInfo(reference, true);
   }
   // if it is a forward reference
   else return defineObjectInfo(moduleScope->mapFullReference(identifier, false), false);
}

ObjectInfo Compiler::NamespaceScope :: mapTerminal(ident_t identifier, bool referenceOne, int mode)
{
   ref_t reference = 0;
   if (!referenceOne) {
      // try resolve as type-alias
      reference = moduleScope->attributes.get(identifier);
      if (isPrimitiveRef(reference))
         reference = 0;
   }

   if (!reference)
      reference = resolveImplicitIdentifier(identifier, referenceOne);
   
   if (reference)
      return defineObjectInfo(reference, true);

//   if (parent == NULL) {
      if (referenceOne) {
         return mapGlobal(identifier);
      }
      else if (identifier.compare(NIL_VAR)) {
         return ObjectInfo(okNil);
      }
//   }
   return Scope::mapTerminal(identifier, referenceOne, mode);
}

ref_t Compiler::NamespaceScope :: resolveImplicitIdentifier(ident_t identifier, bool referenceOne)
{
   ref_t reference = forwards.get(identifier);
   if (reference)
      return reference;

   reference = moduleScope->resolveImplicitIdentifier(ns, identifier, referenceOne, &importedNs);
   if (reference) {
      forwards.add(identifier, reference);
   }

   return reference;
}

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
         raiseError(errDuplicatedSymbol, terminal.firstChild(lxTerminalMask));

      return terminal.argument;
   }
////   else if (terminal == lxNone) {
////      return mapAnonymous();
////   }
//   else return moduleScope->mapNewIdentifier(ns, terminal.identifier(), false);
   else throw InternalError("Cannot map new terminal"); // !! temporal
}

//////bool Compiler::ModuleScope :: doesReferenceExist(ident_t referenceName)
//////{
//////   ref_t moduleRef = 0;
//////   _Module* module = project->resolveModule(referenceName, moduleRef, true);
//////
//////   if (module == NULL || moduleRef == 0)
//////      return false;
//////
//////   return module->mapReference(referenceName, true) != 0;
//////}

ObjectInfo Compiler::NamespaceScope :: defineObjectInfo(ref_t reference, bool checkState)
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
      ref_t r = moduleScope->loadClassInfo(info, reference, true);
      if (r) {
         // if it is an extension
         if (test(info.header.flags, elExtension)) {
            return ObjectInfo(okExtension, reference, reference);
         }
         // if it is a stateless symbol
         else if (test(info.header.flags, elStateless)) {
            return ObjectInfo(okConstantSymbol, reference, reference);
         }
         // if it is a normal class
         // then the symbol is reference to the class class
         else if (test(info.header.flags, elStandartVMT) && info.header.classRef != 0) {
            return ObjectInfo(okClass, reference, info.header.classRef);
         }
      }
      else {
         // check if the object is typed expression
         SymbolExpressionInfo symbolInfo;
         // check if the object can be treated like a constant object
         r = moduleScope->loadSymbolExpressionInfo(symbolInfo, module->resolveReference(reference));
         if (r) {
            // if it is a constant
            if (symbolInfo.constant) {
               ref_t classRef = symbolInfo.expressionClassRef;

               if (symbolInfo.listRef != 0) {
                  return ObjectInfo(okArrayConst, symbolInfo.listRef, classRef);
               }
               else return ObjectInfo(okConstantSymbol, reference, classRef);
            }
            // if it is a typed symbol
            else if (symbolInfo.expressionClassRef != 0) {
               return ObjectInfo(okSymbol, reference, symbolInfo.expressionClassRef);
            }
         }
      }
   }

   // otherwise it is a normal one
   return ObjectInfo(okSymbol, reference);
}

//////void Compiler::ModuleScope :: validateReference(SNode terminal, ref_t reference)
//////{
//////   // check if the reference may be resolved
//////   bool found = false;
//////
//////   if (warnOnWeakUnresolved || !isWeakReference(terminal.identifier())) {
//////      int   mask = reference & mskAnyRef;
//////      reference &= ~mskAnyRef;
//////
//////      ref_t    ref = 0;
//////      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);
//////
//////      if (refModule != NULL) {
//////         found = (refModule->mapSection(ref | mask, true)!=NULL);
//////      }
//////      if (!found) {
//////         if (!refModule || refModule == module) {
//////            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module,
//////               terminal.findChild(lxRow).argument,
//////               terminal.findChild(lxCol).argument));
//////         }
//////         else raiseWarning(WARNING_LEVEL_1, wrnUnresovableLink, terminal);
//////      }
//////   }
//////}

void Compiler::NamespaceScope :: loadExtensions(ident_t ns)
{
   ReferenceNs sectionName(ns, EXTENSION_SECTION);

   ref_t extRef = 0;
   _Module* extModule = moduleScope->loadReferenceModule(module->mapReference(sectionName, false), extRef);
   _Memory* section = extModule ? extModule->mapSection(extRef | mskMetaRDataRef, true) : NULL;
   if (section) {
      MemoryReader metaReader(section);
      while (!metaReader.Eof()) {
         ref_t type_ref = metaReader.getDWord();
         ref_t message = metaReader.getDWord();
         ref_t role_ref = metaReader.getDWord();
         if (extModule != module) {
            type_ref = importReference(extModule, type_ref, module);
            message = importMessage(extModule, message, module);
            role_ref = importReference(extModule, role_ref, module);
         }

         extensions.add(message, Pair<ref_t, ref_t>(type_ref, role_ref));
      }
   }
}

void Compiler::NamespaceScope :: saveExtension(ref_t message, ref_t typeRef, ref_t role, bool internalOne)
{
   if (typeRef == INVALID_REF || typeRef == moduleScope->superReference)
      typeRef = 0;

   IdentifierString sectionName(internalOne ? PRIVATE_PREFIX_NS : "'");
   if (!emptystr(ns)) {
      sectionName.append(ns);
      sectionName.append("'");
   }
   sectionName.append(EXTENSION_SECTION);

   MemoryWriter metaWriter(module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false));

   if (typeRef == moduleScope->superReference) {
      metaWriter.writeDWord(0);
   }
   else metaWriter.writeDWord(typeRef);
   metaWriter.writeDWord(message);
   metaWriter.writeDWord(role);

   extensions.add(message, Pair<ref_t, ref_t>(typeRef, role));
}

// --- Compiler::SourceScope ---

Compiler::SourceScope :: SourceScope(Scope* moduleScope, ref_t reference)
   : Scope(moduleScope)
{
   this->reference = reference;

   ident_t name = module->resolveReference(reference);
   this->internalOne = name.startsWith(PRIVATE_PREFIX_NS);
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(NamespaceScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   outputRef = 0;
   constant = false;
   staticOne = false;
   preloaded = false;
}

//ObjectInfo Compiler::SymbolScope :: mapTerminal(ident_t identifier)
//{
//   return Scope::mapTerminal(identifier);
//}

void Compiler::SymbolScope :: save()
{
   SymbolExpressionInfo info;
   info.expressionClassRef = outputRef;
   info.constant = constant;

   // save class meta data
   MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false), 0);
   info.save(&metaWriter);
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(Scope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   info.header.parentRef =  moduleScope->superReference;
   info.header.flags = elStandartVMT;
   info.header.count = 0;
   info.header.classRef = 0;
   info.header.staticSize = 0;
   info.size = 0;

   extensionClassRef = 0;
   embeddable = false;
   classClassMode = false;
   abstractMode = false;
   abstractBaseMode = false;
//   withImplicitConstructor = false;
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

ObjectInfo Compiler::ClassScope :: mapField(ident_t terminal, int scopeMode)
{
   int offset = info.fields.get(terminal);
   if (offset >= 0) {
      bool readOnlyMode = test(info.header.flags, elReadOnlyRole) && !test(scopeMode, INITIALIZER_SCOPE);
      ClassInfo::FieldInfo fieldInfo = info.fieldTypes.get(offset);
      if (test(info.header.flags, elStructureRole)) {
         return ObjectInfo(readOnlyMode ? okReadOnlyFieldAddress : okFieldAddress, offset, fieldInfo.value1, fieldInfo.value2, 0);
      }
      else return ObjectInfo(readOnlyMode ? okReadOnlyField : okField, offset, fieldInfo.value1, fieldInfo.value2, 0);
   }
   else if (offset == -2 && test(info.header.flags, elDynamicRole)) {
      auto fieldInfo = info.fieldTypes.get(-1);

      return ObjectInfo(okSelfParam, 1, fieldInfo.value1, fieldInfo.value2, -2);
   }
   else {
      ClassInfo::FieldInfo staticInfo = info.statics.get(terminal);
      if (staticInfo.value1 != 0) {
         if (!isSealedStaticField(staticInfo.value1)) {
            ref_t val = info.staticValues.get(staticInfo.value1);
            if (val != mskStatRef) {
               if (classClassMode) {
                  return ObjectInfo(okClassStaticConstantField, 0, staticInfo.value1, staticInfo.value2, 0);
               }
               else return ObjectInfo(okStaticConstantField, staticInfo.value1, staticInfo.value2);
            }
         }
         else if(info.staticValues.exist(staticInfo.value1, mskConstantRef)) {
            // if it is a constant static sealed field
            if (classClassMode) {
               return ObjectInfo(okClassStaticConstantField, 0, staticInfo.value1, staticInfo.value2, 0);
            }
            else return ObjectInfo(okStaticConstantField, staticInfo.value1, staticInfo.value2);
         }

         if (classClassMode) {
            return ObjectInfo(okClassStaticField, 0, staticInfo.value1, staticInfo.value2, 0);
         }
         else return ObjectInfo(okStaticField, staticInfo.value1, staticInfo.value2, 0, 0);

      }
      return ObjectInfo();
   }
}

ObjectInfo Compiler::ClassScope :: mapTerminal(ident_t identifier, bool referenceOne, int mode)
{
   //if (!referenceOne && identifier.compare(SUPER_VAR)) {
   //   return ObjectInfo(okSuper, info.header.parentRef);
   //}
   //else {
      if (!referenceOne) {
         ObjectInfo fieldInfo = mapField(identifier, mode);
         if (fieldInfo.kind != okUnknown) {
            return fieldInfo;
         }
      }
      return Scope::mapTerminal(identifier, referenceOne, mode);
   //}
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved = 0;
   this->scopeMode = 0;
   this->rootToFree = 1;
   this->hints = 0;
   this->outputRef = INVALID_REF; // to indicate lazy load
//   this->withOpenArg = false;
   this->classEmbeddable = false;
//   this->generic = false;
   this->extensionMode = false;
   this->multiMethod = false;
   this->closureMode = false;
//   this->nestedMode = parent->getScope(Scope::slOwnerClass) != parent;
   this->subCodeMode = false;
   this->abstractMethod = false;
//   this->genericClosure = false;
//   this->dispatchMode = false;
}

ObjectInfo Compiler::MethodScope :: mapSelf(/*bool forced*/)
{
   if (extensionMode/* && !forced*/) {
      //COMPILER MAGIC : if it is an extension ; replace $self with self
      ClassScope* extensionScope = (ClassScope*)getScope(slClass);

      return ObjectInfo(okLocal, (ref_t)-1, extensionScope->extensionClassRef, 0, extensionScope->embeddable ? -1 : 0);
   }
   else if (classEmbeddable) {
      return ObjectInfo(okSelfParam, 1, ((ClassScope*)getScope(slClass))->reference, 0, (ref_t)-1);
   }
   else return ObjectInfo(okSelfParam, 1, ((ClassScope*)getScope(slClass))->reference);
}

////ObjectInfo Compiler::MethodScope :: mapGroup()
////{
////   return ObjectInfo(okParam, (size_t)-1);
////}

ObjectInfo Compiler::MethodScope :: mapParameter(Parameter param)
{
   int prefix = closureMode ? 0 : -1;

   //if (withOpenArg && param.class_ref == V_ARGARRAY) {
   //   return ObjectInfo(okParams, prefix - param.offset, param.class_ref, param.element_ref);
   //}
   /*else */if (param.class_ref != 0 && param.size != 0) {
      // if the parameter may be stack-allocated
      return ObjectInfo(okParam, prefix - param.offset, param.class_ref, param.element_ref, (ref_t)-1);
   }
   return ObjectInfo(okParam, prefix - param.offset, param.class_ref, param.element_ref, 0);
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal, bool referenceOne, int mode)
{
   if (!referenceOne) {
      Parameter param = parameters.get(terminal);
      if (param.offset >= 0) {
         return mapParameter(param);
      }
      else {
         if (terminal.compare(SELF_VAR)) {
            //if (closureMode || nestedMode) {
            //   return parent->mapTerminal(OWNER_VAR, false, mode | scopeMode);
            //}
            /*else */return mapSelf();
         }
      //   else if (!closureMode && (terminal.compare(GROUP_VAR) || terminal.compare(OLD_GROUP_VAR))) {
      //      if (extensionMode) {
      //         return mapSelf();
      //      }
      //      else return mapGroup();
      //   }
         else if (terminal.compare(RETVAL_VAR) && subCodeMode) {
            ObjectInfo retVar = parent->mapTerminal(terminal, referenceOne, mode | scopeMode);
            if (retVar.kind == okUnknown) {
               InlineClassScope* closure = (InlineClassScope*)getScope(Scope::slClass);

               retVar = closure->allocateRetVar();
            }

            return retVar;
         }
      }
   }

   return Scope::mapTerminal(terminal, referenceOne, mode | scopeMode);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->rootBookmark = 0;
   this->level = 0;
   this->saved = this->reserved = 0;
   //this->genericMethod = false;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->rootBookmark = 0;
   this->level = 0;
   this->saved = this->reserved = 0;
//   this->genericMethod = parent->generic;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->rootBookmark = 0;
   this->level = parent->level;
   this->saved = parent->saved;
   this->reserved = parent->reserved;
//   this->genericMethod = parent->genericMethod;
}

////ObjectInfo Compiler::CodeScope :: mapGlobal(ident_t identifier)
////{
////   NamespaceScope* nsScope = (NamespaceScope*)getScope(Scope::slNamespace);
////
////   return nsScope->mapGlobal(identifier);
////}

ObjectInfo Compiler::CodeScope :: mapLocal(ident_t identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset) {
      /*if (genericMethod && identifier.compare(SUBJECT_VAR)) {
         return ObjectInfo(okSubject, local.offset);
      }
      /*else */if (local.size != 0) {
         return ObjectInfo(okLocalAddress, local.offset, local.class_ref, local.element_ref, 0);
      }
      else return ObjectInfo(okLocal, local.offset, local.class_ref, local.element_ref, 0);
   }
   //else if (genericMethod && identifier.compare(OLD_SUBJECT_VAR)) {
   //   // NOTE : temporally to support old built-in variable
   //   return ObjectInfo(okSubject, local.offset);
   //}
   else return ObjectInfo();
}

ObjectInfo Compiler::CodeScope :: mapMember(ident_t identifier)
{
   MethodScope* methodScope = (MethodScope*)getScope(Scope::slMethod);
   if (identifier.compare(SELF_VAR)) {      
      if (methodScope != nullptr) {
         return methodScope->mapSelf();
      }
   }
//   else if (identifier.compare(GROUP_VAR) || identifier.compare(OLD_GROUP_VAR)) {
//      if (methodScope != NULL) {
//         return methodScope->mapGroup();
//      }
//   }
   else {
      ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);
      if (classScope != nullptr) {
         return classScope->mapField(identifier, methodScope->scopeMode);
      }      
   }
   return ObjectInfo();
}

bool Compiler::CodeScope :: resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element)
{
   if (info.kind == okLocal) {
      for (auto it = locals.start(); !it.Eof(); it++) {
         if ((*it).offset == (int)info.param) {
            if ((*it).class_ref == V_AUTO) {
               (*it).class_ref = reference;
               (*it).element_ref = element;

               info.extraparam = reference;
               info.element = element;

               return true;
            }
         }
      }
   }

   return Scope::resolveAutoType(info, reference, element);
}

ObjectInfo Compiler::CodeScope :: mapTerminal(ident_t identifier, bool referenceOne, int mode)
{
   if (!referenceOne) {
      ObjectInfo info = mapLocal(identifier);
      if (info.kind != okUnknown)
         return info;
   }
   return Scope::mapTerminal(identifier, referenceOne, mode);
}

// --- Compiler::ResendScope ---

inline bool isField(Compiler::ObjectKind kind)
{
   switch (kind) {
      case  Compiler::okField:
      case  Compiler::okReadOnlyField:
      case  Compiler::okFieldAddress:
      case  Compiler::okReadOnlyFieldAddress:
      default:
         return false;
   }
}

ObjectInfo Compiler::ResendScope :: mapTerminal(ident_t identifier, bool referenceOne, int mode)
{
   if (!withFrame && (identifier.compare(SELF_VAR)/* || identifier.compare(GROUP_VAR) || identifier.compare(OLD_GROUP_VAR)*/))
   {
      return ObjectInfo();
   }

   ObjectInfo info = CodeScope::mapTerminal(identifier, referenceOne, mode);
   if (consructionMode && isField(info.kind)) {
      return ObjectInfo();
   }
   else return info;
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
   : ClassScope(owner, reference), outers(Outer()), outerFieldTypes(ClassInfo::FieldInfo(0, 0))
{
   this->returningMode = false;
   //this->parent = owner;
   info.header.flags |= elNestedClass;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapParent()
{
   Outer parentVar = outers.get(PARENT_VAR);
   // if owner reference is not yet mapped, add it
   if (parentVar.outerObject.kind == okUnknown) {
      parentVar.reference = info.fields.Count();
      CodeScope* codeScope = (CodeScope*)parent->getScope(Scope::slCode);
      if (codeScope) {
         parentVar.outerObject = codeScope->mapMember(SELF_VAR);
      }
      else parentVar = mapOwner();

      outers.add(PARENT_VAR, parentVar);
      mapKey(info.fields, PARENT_VAR, (int)parentVar.reference);

   }
   return parentVar;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   Outer owner = outers.get(SELF_VAR);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();

      owner.outerObject = parent->mapTerminal(SELF_VAR, false, 0);
      if (owner.outerObject.kind == okUnknown) {
         // HOTFIX : if it is a singleton nested class
         owner.outerObject = ObjectInfo(okSelfParam, 1, reference);
      }
      else if (owner.outerObject.kind == okSelfParam) {
         owner.outerObject.reference = ((CodeScope*)parent)->getClassRefId(false);
      }

      outers.add(SELF_VAR, owner);
      mapKey(info.fields, SELF_VAR, (int)owner.reference);
   }
   return owner;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapOwner()
{
   Outer owner = outers.get(OWNER_VAR);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.outerObject = parent->mapTerminal(OWNER_VAR, false, 0);
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

ObjectInfo Compiler::InlineClassScope :: mapTerminal(ident_t identifier, bool referenceOne, int mode)
{
   //if (identifier.compare(SUPER_VAR)) {
   //   return ObjectInfo(okSuper, info.header.parentRef);
   //}
   /*else */if (identifier.compare(OWNER_VAR)) {
      Outer owner = mapOwner();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuterSelf, owner.reference, owner.outerObject.reference, owner.outerObject.element, owner.outerObject.extraparam);
   }
   //else if (identifier.compare(SELF_VAR) && !closureMode) {
   //   return ObjectInfo(okParam, (size_t)-1);
   //}
   else {
      Outer outer = outers.get(identifier);
//
      // if object already mapped
      if (outer.reference != -1) {
         /*if (outer.outerObject.kind == okSuper) {
            return ObjectInfo(okSuper, outer.reference);
         }
         else */return ObjectInfo(okOuter, outer.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
      }
      else {
         outer.outerObject = parent->mapTerminal(identifier, referenceOne, mode);
         switch (outer.outerObject.kind) {
            case okReadOnlyField:
            case okField:
            case okStaticField:
            {
               // handle outer fields in a special way: save only self
               Outer owner = mapParent();

               // map as an outer field (reference to outer object and outer object field index)
               if (outer.outerObject.kind == okOuterField) {
                  return ObjectInfo(okOuterField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else if (outer.outerObject.kind == okOuterReadOnlyField) {
                  return ObjectInfo(okOuterReadOnlyField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else if (outer.outerObject.kind == okOuterStaticField) {
                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else if (outer.outerObject.kind == okStaticField) {
                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else if (outer.outerObject.kind == okReadOnlyField) {
                  return ObjectInfo(okOuterReadOnlyField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else return ObjectInfo(okOuterField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
            }
            case okParam:
            case okLocal:
            case okOuter:
            //case okSuper:
            case okSelfParam:
            case okLocalAddress:
            case okFieldAddress:
            case okReadOnlyFieldAddress:
            case okOuterField:
            case okOuterStaticField:
            case okOuterSelf:
            //case okParams:
            {
               // map if the object is outer one
               outer.reference = info.fields.Count();

               outers.add(identifier, outer);
               mapKey(info.fields, identifier, (int)outer.reference);

               if (outer.outerObject.kind == okOuter && identifier.compare(RETVAL_VAR)) {
                  // HOTFIX : quitting several clsoures
                  (*outers.getIt(identifier)).preserved = true;
               }
               else if (outer.outerObject.kind == okOuterSelf) {
                  // HOTFIX : to support self in deep nested closures
                  return ObjectInfo(okOuterSelf, outer.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }

               return ObjectInfo(okOuter, outer.reference, outer.outerObject.reference);
            }
            case okUnknown:
            {
               // check if there is inherited fields
               ObjectInfo fieldInfo = mapField(identifier, 0);
               if (fieldInfo.kind != okUnknown) {
                  return fieldInfo;
               }
               else return outer.outerObject;
            }
            default:               
               return outer.outerObject;
         }
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
   outer.outerObject = ObjectInfo(okNil, (ref_t)-1);

   outers.add(RETVAL_VAR, outer);
   mapKey(info.fields, RETVAL_VAR, (int)outer.reference);

   return ObjectInfo(okOuter, outer.reference);
}

// --- Compiler ---

Compiler :: Compiler(_CompilerLogic* logic)
//   : _verbs(0)
{
   _optFlag = 0;

   this->_logic = logic;

   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: writeMessageInfo(SyntaxWriter& writer, _ModuleScope& scope, ref_t messageRef)
{
   ref_t actionRef, flags;
   int paramCount;
   decodeMessage(messageRef, actionRef, paramCount, flags);

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

bool Compiler :: calculateIntOp(int operation_id, int arg1, int arg2, int& retVal)
{
   switch (operation_id)
   {
      case ADD_OPERATOR_ID:
         retVal = arg1 + arg2;
         break;
      case SUB_OPERATOR_ID:
         retVal = arg1 - arg2;
         break;
      case MUL_OPERATOR_ID:
         retVal = arg1 * arg2;
         break;
      case DIV_OPERATOR_ID:
         retVal = arg1 / arg2;
         break;
      case AND_OPERATOR_ID:
         retVal = arg1 & arg2;
         break;
      case OR_OPERATOR_ID:
         retVal = arg1 | arg2;
         break;
      case XOR_OPERATOR_ID:
         retVal = arg1 ^ arg2;
         break;
      case SHIFTR_OPERATOR_ID:
         retVal = arg1 >> arg2;
         break;
      case SHIFTL_OPERATOR_ID:
         retVal = arg1 << arg2;
         break;
      default:
         return false;
   }

   return true;
}

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
//
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

//ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object, ref_t targetRef)
//{
////   if (object.kind == okExternal) {
////      // HOTFIX : recognize external functions returning long / real
////      if (targetRef == scope.moduleScope->longReference) {
////         return V_INT64;
////      }
////      else if (targetRef == scope.moduleScope->realReference) {
////         return V_REAL64;
////      }
////      else return resolveObjectReference(scope, object);
////   }
//   /*else */return resolveObjectReference(scope, object);
//}

ref_t Compiler :: resolveObjectReference(CodeScope& scope, ObjectInfo object)
{
   if (object.kind == okSelfParam) {
      if (object.extraparam == -2u) {
         // HOTFIX : to return the primitive array
         return object.reference;
      }
      else return scope.getClassRefId(false);
   }
   else return resolveObjectReference(*scope.moduleScope, object);
}

ref_t Compiler :: resolveObjectReference(_ModuleScope& scope, ObjectInfo object)
{
   // if static message is sent to a class class
   switch (object.kind)
   {
//      case okConstantClass:
//         return object.extraparam;
//      case okConstantRole:
//         // if external role is provided
//         return object.param;
//      case okConstantSymbol:
//      case okExtension:
//         if (object.extraparam != 0) {
//            return object.extraparam;
//         }
//         else return object.param;
//      case okLocalAddress:
//         return object.extraparam;
//      case okIntConstant:
//      case okUIntConstant:
//         return V_INT32;
//      case okLongConstant:
//         return V_INT64;
//      case okRealConstant:
//         return V_REAL64;
//      case okCharConstant:
//         return scope.charReference;
//      case okLiteralConstant:
//         return scope.literalReference;
//      case okWideLiteralConstant:
//         return scope.wideReference;
//      case okSubject:
//      case okSignatureConstant:
//         return V_SIGNATURE;
//      case okExtMessageConstant:
//         return scope.extMessageReference;
//      case okSuper:
//         return object.param;
//      case okParams:
//         return V_ARGARRAY;
//      case okExternal:
//         return V_INT32;
//      case okMessageConstant:
//         return V_MESSAGE;
      case okNil:
         return V_NIL;
//      case okField:
//      case okReadOnlyField:
//      case okLocal:
//      case okFieldAddress:
//      case okReadOnlyFieldAddress:
//      case okOuter:
//      case okOuterSelf:
//      case okParam:
//      case okSymbol:
//      case okStaticField:
//      case okStaticConstantField:
//         return object.extraparam;
//      case okClassStaticConstantField:
//      case okOuterField:
//      case okOuterReadOnlyField:
//      case okOuterStaticField:
//      case okClassStaticField:
//         return object.element;
//      case okClassSelf:
//         return object.param;
      default:
//         if (object.kind == okObject) {
//            return object.param;
//         }
//         else return 0;
         return object.reference;
   }
}

void Compiler :: declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withSelf/*, bool withTargetSelf*/)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   // declare built-in variables
   if (withSelf) {
      /*if (scope.classEmbeddable) {
         IdentifierString className(scope.moduleScope->module->resolveReference(scope.getClassRef()));

         SNode debugNode = node.insertNode(lxBinarySelf, 1);
         debugNode.appendNode(lxClassName, className.c_str());
      }*/
      /*else */writer.appendNode(lxSelfVariable, 1);
   }

//   if (withTargetSelf)
//      writer.appendNode(lxSelfVariable, -1);

   writeMessageInfo(writer, *moduleScope, scope.message);

   int prefix = scope.closureMode ? 0 : -1;

   SNode current = node.firstChild();
   // method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter/* || current == lxIdentifier*/) {
         ident_t name = /*(current == lxIdentifier) ? current.identifier() : */current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();
         Parameter param = scope.parameters.get(name);
         if (param.offset != -1) {
            //if (param.class_ref == V_ARGARRAY) {
            //   writer.newNode(lxParamsVariable);
            //}
            /*else */if (param.class_ref == moduleScope->intReference) {
               writer.newNode(lxIntVariable);
            }
            //else if (param.class_ref == moduleScope->longReference) {
            //   writer.newNode(lxLongVariable);
            //}
            //else if (param.class_ref == moduleScope->realReference) {
            //   writer.newNode(lxReal64Variable);
            //}
            //else if (param.size != 0 && param.class_ref != 0) {
            //   ref_t classRef = param.class_ref;
            //   if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {
            //      writer.newNode(lxBinaryVariable);
            //      writer.appendNode(lxClassName, scope.moduleScope->module->resolveReference(classRef));
            //   }
            //   else writer.newNode(lxVariable);
            //}
            else writer.newNode(lxVariable);

            writer.appendNode(lxLevel, prefix - param.offset);
            writer.newNode(lxIdentifier, name);
            writer.closeNode();

            writer.closeNode();
         }
      }
      else if (current == lxSourcePath) {
         writer.appendNode(lxSourcePath, scope.saveSourcePath(_writer, current.identifier()));
      }

      current = current.nextNode();
   }
}

void Compiler :: importCode(SyntaxWriter& writer, SNode node, Scope& scope, ident_t function, ref_t message)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(function);
   virtualReference.append('.');

   int paramCount;
   ref_t actionRef, flags;
   decodeMessage(message, actionRef, paramCount, flags);

   // HOTFIX : include self as a parameter
   paramCount++;

   size_t signIndex = virtualReference.Length();
   virtualReference.append('0' + (char)paramCount);
   //if (test(message, SEALED_MESSAGE)) {
   //   virtualReference.append(scope.module->Name());
   //   virtualReference.append("$");
   //}

   ref_t signature = 0;
   virtualReference.append(moduleScope->module->resolveAction(actionRef, signature));

   if (signature) {
      ref_t signatures[ARG_COUNT];
      size_t len = moduleScope->module->resolveSignature(signature, signatures);
      for (size_t i = 0; i < len; i++) {
         ident_t paramName = moduleScope->module->resolveReference(signatures[i]);

         NamespaceName ns(paramName);
         if (isTemplateWeakReference(paramName)) {
            virtualReference.append('$');
            virtualReference.append(paramName + getlength(ns));
         }
         else if (isWeakReference(paramName)) {
            virtualReference.append('$');
            virtualReference.append(scope.module->Name());
            virtualReference.append(paramName);
         }
         else {
            virtualReference.append('$');
            virtualReference.append(paramName);
         }
      }
   }

   virtualReference.replaceAll('\'', '@', signIndex);

   ref_t reference = 0;
   _Module* api = moduleScope->project->resolveModule(virtualReference, reference);

   _Memory* section = api != NULL ? api->mapSection(reference | mskCodeRef, true) : NULL;
   if (section != NULL) {
      writer.appendNode(lxImporting, _writer.registerImportInfo(section, api, moduleScope->module));
   }
   else scope.raiseError(errInvalidLink, node);
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef/*, bool ignoreSealed*/)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.header.classRef;

   // get module reference
   ref_t moduleRef = 0;
   _Module* module = moduleScope->loadReferenceModule(parentRef, moduleRef);

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

         moduleScope->importClassInfo(copy, scope.info, module, false, true);
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

      if (/*!ignoreSealed && */test(scope.info.header.flags, elSealed))
         return irSealed;

      // restore parent and flags
      scope.info.header.parentRef = parentRef;
      scope.info.header.classRef = classClassCopy;

      if (test(scope.info.header.flags, elAbstract)) {
         // exclude abstract flag
         scope.info.header.flags &= ~elAbstract;

         scope.abstractBaseMode = true;
      }

      scope.info.header.flags |= flagCopy;

      return irSuccessfull;
   }
   else return irUnsuccessfull;
}

void Compiler :: compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef/*, bool ignoreSealed*/)
{
   scope.info.header.parentRef = parentRef;
   InheritResult res = irSuccessfull;
   if (scope.info.header.parentRef != 0) {
      res = inheritClass(scope, scope.info.header.parentRef/*, ignoreSealed*/);
   }

   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   //if (res == irInvalid) {
   //   scope.raiseError(errInvalidParent/*, baseNode*/);
   //}
   /*else */if (res == irSealed) {
      scope.raiseError(errSealedParent, baseNode);
   }
   else if (res == irUnsuccessfull)
      scope.raiseError(errUnknownBaseClass, baseNode);
}

ref_t Compiler :: resolveImplicitIdentifier(Scope& scope, ident_t identifier, bool referenceOne, bool gloabalOne)
{
   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::slNamespace);
   if (gloabalOne) {
      return ns->moduleScope->mapFullReference(identifier, true);
   }
   else {
      ref_t reference = ns->resolveImplicitIdentifier(identifier, referenceOne);
      if (!reference && referenceOne)
         reference = scope.moduleScope->mapFullReference(identifier, true);

      return reference;
   }
}

ref_t Compiler :: resolveImplicitIdentifier(Scope& scope, SNode terminal)
{
   if (terminal == lxGlobalReference) {
      return resolveImplicitIdentifier(scope, terminal.identifier() + 1, false, true);
   }
   else return resolveImplicitIdentifier(scope, terminal.identifier(), terminal == lxReference);
}

bool isExtensionDeclaration(SNode node)
{
   SNode current = node.firstChild();
   while (current == lxAttribute) {
      if (current.argument == V_EXTENSION) {
         return true;
      }

      current = current.nextNode();
   }

   return false;
}

void Compiler :: compileParentDeclaration(SNode node, ClassScope& scope, bool extensionMode)
{
   ref_t parentRef = 0;
   if (node == lxParent) {
      parentRef = resolveParentRef(node, scope, false);
   }
//   else if (node != lxNone) {
//      SNode terminal = node.firstChild(lxTerminalMask);
//      if (terminal != lxNone) {
//         parentRef = resolveImplicitIdentifier(scope, terminal);
//
//         if (!parentRef)
//            scope.raiseError(errInvalidParent, terminal);
//      }
//      else {
//         SNode classNode = node.findChild(lxClassRefAttr);
//         if (classNode != lxNone)
//            parentRef = scope.moduleScope->mapFullReference(classNode.identifier(), true);
//      }
//   }

   if (scope.info.header.parentRef == scope.reference) {
      if (parentRef != 0) {
         scope.raiseError(errInvalidSyntax/*, node*/);
      }
   }
   else if (parentRef == 0) {
      parentRef = scope.info.header.parentRef;
   }

   if (extensionMode) {
      // COMPLIER MAGIC : treat the parent declaration in the special way for the extension
      scope.extensionClassRef = parentRef;

      compileParentDeclaration(node, scope, scope.moduleScope->superReference);
   }
   else compileParentDeclaration(node, scope, parentRef);
}

void Compiler :: declareClassAttributes(SNode node, ClassScope& scope)
{
   int flags = scope.info.header.flags;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
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
            else if (test(value, elAbstract))
               scope.abstractMode = true;

            flags |= value;
         }
      }
      else if (current == lxTarget) {
         scope.raiseError(errInvalidSyntax, current);
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
         if (!_logic->validateSymbolAttribute(value, scope.constant, scope.staticOne, scope.preloaded)) {
            current = lxIdle; // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current == lxTarget) {
         scope.outputRef = current.argument != 0 ? current.argument : resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
         if (!scope.outputRef)
            scope.raiseError(errUnknownClass, current);
      }

      current = current.nextNode();
   }
}

int Compiler :: resolveSize(SNode node, Scope& scope)
{
   if (node == lxInteger) {
      return node.identifier().toInt();
   }
   else {
      scope.raiseError(errInvalidSyntax, node);

      return 0;
   }
}

void Compiler :: declareFieldAttributes(SNode node, ClassScope& scope, ref_t& fieldRef/*, ref_t& elementRef*/, int& size, bool& isStaticField, bool& isSealed, bool& isConstant, bool& isEmbeddable)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateFieldAttribute(value, isSealed, isConstant, isEmbeddable)) {
            if (value == lxStaticAttr) {
               isStaticField = true;
            }
            else if (value == -1) {
               // ignore if constant / sealed attribute was set
            }
            else if (!value && isPrimitiveRef(current.argument)) {
               if (current.argument == V_STRING) {
                  // if it is a string attribute
                  scope.info.header.flags |= elDebugLiteral;
               }
               else if (current.argument == V_TEMPLATE) {
                  fieldRef = resolveTemplateDeclaration(current, scope);
               }
               // if it is a primitive type
               else fieldRef = current.argument;
            }
               //else if (value == V_OBJARRAY) {
               //   elementRef = fieldRef;
               //   fieldRef = V_OBJARRAY;
               //}
            else scope.raiseError(errInvalidHint, node);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxTarget) {
         if (fieldRef == 0) {
            fieldRef = current.argument != 0 ? current.argument : resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
            if (!fieldRef)
               scope.raiseError(errUnknownClass, current);
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxSize) {
         if (size == 0) {
            if (current.argument) {
               size = current.argument;
            }
            else size = resolveSize(current.firstChild(lxTerminalMask), scope);
         }
         else scope.raiseError(errInvalidHint, node);
      }

      current = current.nextNode();
   }

   //HOTFIX : recognize raw data
   if (fieldRef == V_INTBINARY) {
      switch (size) {
         case 1:
         case 2:
         case 4:
            // treat it like dword
            fieldRef = V_INT32;
            break;
   //      case 8:
   //         // treat it like qword
   //         fieldRef = V_INT64;
   //         break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
   else if (fieldRef == V_BINARY) {
      switch (size) {
         case 4:
            // treat it like dword
            fieldRef = V_DWORD;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }

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
//         if (_logic->validateLocalAttribute(value)) {
//            // negative value defines the target virtual class
//            if (variable.extraparam == 0) {
//               variable.extraparam = value;
//            }
//            //else if (value == V_OBJARRAY) {
//            //   variable.element = variable.extraparam;
//            //   variable.extraparam = value;
//            //}
//            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
//         }
//         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
//      }
//      else if (current == lxSize) {
//         size = current.argument;
//      }
//      else if (current == lxClassRefAttr) {
//         if (variable.extraparam == 0) {
//      //      NamespaceScope* namespaceScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
//
//      //      variable.extraparam = namespaceScope->resolveImplicitIdentifier(current.identifier());
//
//            variable.extraparam = scope.moduleScope->mapFullReference(current.identifier(), true);
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
//      loperand = compileExpression(writer, targetNode, scope, 0, 0);
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
//      else loperand = compileObject(writer, targetNode, scope, 0, 0);
//
//      // find option value
//      SNode valueNode = current.firstChild(lxObjectMask);
//
//      ObjectInfo roperand = compileObject(writer, valueNode.firstChild(lxObjectMask), scope, 0, 0);
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

size_t Compiler :: resolveArraySize(SNode node, Scope& scope)
{
   if (isSingleStatement(node)) {
      SNode terminal = node.findSubNodeMask(lxTerminalMask);
      if (terminal.type == lxInteger) {
         return terminal.identifier().toInt();
      }
      else scope.raiseError(errInvalidSyntax, node);
   }
   else scope.raiseError(errInvalidSyntax, node);

   return 0; // !! dummy returning statement, the code never reaches this point
}

void Compiler :: compileVariable(SyntaxWriter& writer, SNode& terminal, CodeScope& scope, ref_t typeRef, bool dynamicArray)
{
//   //if (terminal == lxExpression)
//   //   terminal = terminal.findChild(lxIdentifier, lxPrivate);

   ident_t identifier = terminal.identifier();
//   //if (scope.moduleScope->mapAttribute(terminal) != 0)
//   //   scope.raiseWarning(WARNING_LEVEL_3, wrnAmbiguousVariable, terminal);

   if (!scope.locals.exist(identifier)) {
      LexicalType variableType = lxVariable;
      int variableArg = 0;
      int size = dynamicArray ? -1 : 0;
//      ident_t className = NULL;

      ObjectInfo variable(okLocal);
      variable.reference = typeRef;

      // COMPILER MAGIC : if it is a fixed-sized array
      SNode opNode = terminal.nextNode();
      if (opNode == lxOperator && opNode.argument == REFER_OPERATOR_ID) {
         if (size)
            scope.raiseError(errInvalidSyntax, terminal);

         SNode sizeExprNode = opNode.nextNode();

         size = resolveArraySize(sizeExprNode, scope);

         // HOTFIX : remove the size attribute
         opNode = lxIdle;
         sizeExprNode = lxIdle;

         opNode = sizeExprNode.nextNode();
      }
      if (opNode == lxNone) {
         // HOTFIX : remove the variable if the statement contains only a declaration
         terminal = lxIdle;
      }

      if (size != 0 && variable.reference != 0) {
         if (!isPrimitiveRef(variable.reference)) {
            // if it is a primitive array
            variable.element = variable.reference;
            variable.reference = _logic->definePrimitiveArray(*scope.moduleScope, variable.element);
         }
         else scope.raiseError(errInvalidHint, terminal);
      }

      ClassInfo localInfo;
      bool binaryArray = false;
      if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.reference))
         scope.raiseError(errUnknownVariableType, terminal);

      //if (_logic->isWrapper(localInfo)) {
      //   variable.reference = _logic->resolvePrimitive(localInfo, variable.element);
      //}

      if (variable.reference == V_BINARYARRAY && variable.element != 0) {
         localInfo.size *= _logic->defineStructSize(*scope.moduleScope, variable.element, 0);
      }

      if (_logic->isEmbeddableArray(localInfo) && size != 0) {
         binaryArray = true;
         size = size * (-((int)localInfo.size));
      }
      else if (variable.reference == V_OBJARRAY && size == -1) {
         // if it is a primitive dynamic array
      }
      else if (_logic->isEmbeddable(localInfo) && size == 0) {
         bool dummy = false;
         size = _logic->defineStructSize(localInfo, dummy);
      }
      else if (size != 0)
         scope.raiseError(errInvalidOperation, terminal);

      if (size > 0) {
         if (!allocateStructure(scope, size, binaryArray, variable))
            scope.raiseError(errInvalidOperation, terminal);

         // make the reservation permanent
         scope.saved = scope.reserved;

         switch (localInfo.header.flags & elDebugMask) {
            case elDebugDWORD:
               variableType = lxIntVariable;
               break;
//            case elDebugQWORD:
//               variableType = lxLongVariable;
//               break;
//            case elDebugReal64:
//               variableType = lxReal64Variable;
//               break;
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
         }
      }
      else {
         if (size < 0) {
         //   // replace dynamic primitive array with a wrapper
            size = 0;
         //   variable.reference = resolvePrimitiveReference(scope, variable.reference, variable.element);
         //   variable.element = 0;
         }

         variable.param = scope.newLocal();
      }

      scope.mapLocal(identifier, variable.param, variable.reference, variable.element, size);

      // injecting variable bookmark
      writer.insert(scope.rootBookmark, lxEnding, 0);

      writer.insertChild(scope.rootBookmark, lxLevel, variable.param);
      writer.insertChild(scope.rootBookmark, lxIdentifier, identifier);
      //if (!emptystr(className)) {
      //   if (isWeakReference(className)) {
      //      if (isTemplateWeakReference(className)) {
      //         // HOTFIX : save weak template-based class name directly
      //         writer.appendNode(lxClassName, className);
      //      }
      //      else {
      //         IdentifierString fullName(scope.module->Name(), className);

      //         writer.appendNode(lxClassName, fullName);
      //      }
      //   }
      //   else writer.appendNode(lxClassName, className);
      //}

      writer.insert(scope.rootBookmark, variableType, variableArg);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

void Compiler :: writeTerminalInfo(SyntaxWriter& writer, SNode terminal)
{
   SyntaxTree::copyNode(writer, lxRow, terminal);
   SyntaxTree::copyNode(writer, lxCol, terminal);
   SyntaxTree::copyNode(writer, lxLength, terminal);

   //ident_t ident = terminal.identifier();
   //if (ident)
   //   writer.appendNode(lxTerminal, terminal.identifier());
}

inline void writeTarget(SyntaxWriter& writer, ref_t targetRef, ref_t elementRef)
{
   if (targetRef)
      writer.appendNode(lxTarget, targetRef);

   if (isPrimitiveRef(targetRef) && elementRef)
      writer.appendNode(lxElement, elementRef);
}

int Compiler :: defineFieldSize(CodeScope& scope, int offset)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   ClassInfo::FieldMap::Iterator it = retrieveIt(classScope->info.fields.start(), offset);
   it++;
   if (!it.Eof()) {
      return *it - offset;
   }
   else return classScope->info.size - offset;
}

void Compiler :: writeParamTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, int mode, LexicalType type)
{
   if (object.extraparam == -1 && !test(mode, HINT_NOBOXING)) {
      // if the parameter may be stack-allocated
      ref_t targetRef = resolveObjectReference(scope, object);
      bool variable = false;
      int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, object.element, variable);
      writer.newNode((variable && !test(mode, HINT_NOUNBOXING)) ? lxUnboxing : lxCondBoxing, size);
      writer.appendNode(type, object.param);
      if (test(mode, HINT_DYNAMIC_OBJECT))
         writer.appendNode(lxBoxingRequired);
   }
   else writer.newNode(type, object.param);
}

void Compiler :: writeTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, ObjectInfo object, int mode)
{
   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
//         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         writer.newNode(lxSymbolReference, object.param);
         break;
      case okClass:
         writer.newNode(lxClassSymbol, object.param);
         break;
      case okExtension:
         scope.raiseWarning(WARNING_LEVEL_3, wrnExplicitExtension, terminal);
      case okConstantSymbol:
         writer.newNode(lxConstantSymbol, object.param);
         break;
      case okLiteralConstant:
         writer.newNode(lxConstantString, object.param);
         break;
      case okWideLiteralConstant:
         writer.newNode(lxConstantWideStr, object.param);
         break;
//      case okCharConstant:
//         writer.newNode(lxConstantChar, object.param);
//         break;
      case okIntConstant:
      case okUIntConstant:
         writer.newNode(lxConstantInt, object.param);
         writer.appendNode(lxIntValue, object.extraparam);
         break;
//      case okLongConstant:
//         writer.newNode(lxConstantLong, object.param);
//         break;
//      case okRealConstant:
//         writer.newNode(lxConstantReal, object.param);
//         break;
//      case okArrayConst:
//         writer.newNode(lxConstantList, object.param);
//         break;
      case okParam:
      case okLocal:
         writeParamTerminal(writer, scope, object, mode, lxLocal);
         break;
      case okSelfParam:
         writeParamTerminal(writer, scope, object, mode, lxSelfLocal);
         break;
//      case okSuper:
//         writer.newNode(lxLocal, 1);
//         break;
      case okReadOnlyField:
      case okField:
      case okOuter:
      case okOuterSelf:
         writer.newNode(lxField, object.param);
         break;
      case okStaticField:
         if ((int)object.param < 0) {
            // if it is a normal static field - field expression should be used
            writer.newNode(lxFieldExpression, 0);
            writer.appendNode(lxClassRefField, 1);
            writer.appendNode(lxStaticField, object.param);
         }
         // if it is a sealed static field
         else writer.newNode(lxStaticField, object.param);
         break;
      case okClassStaticField:
         writer.newNode(lxFieldExpression, 0);
         if (!object.param) {
            writer.appendNode(lxSelfLocal, 1);
         }
         else writer.appendNode(lxClassSymbol, object.param);
         writer.appendNode(lxStaticField, object.extraparam);
         break;
      case okStaticConstantField:
         if ((int)object.param < 0) {
            writer.newNode(lxFieldExpression, 0);
            writer.appendNode(lxClassRefField, 1);
            writer.appendNode(lxStaticConstField, object.param);
         }
         else writer.newNode(lxStaticField, object.param);
         break;
      case okClassStaticConstantField:
         writer.newNode(lxFieldExpression, 0);
         if (!object.param) {
            writer.appendNode(lxSelfLocal, 1);
         }         
         else writer.appendNode(lxClassSymbol, object.param);
         writer.appendNode(lxStaticConstField, object.extraparam);
         break;
      case okOuterField:
      case okOuterReadOnlyField:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxField, object.param);
         writer.appendNode(lxResultField, object.extraparam);
         break;
      case okOuterStaticField:
         writer.newNode(lxFieldExpression, 0);
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxField, object.param);
         //writer.appendNode(lxClassRefAttr, object.param);
         writer.closeNode();         
         writer.appendNode(lxStaticField, object.extraparam);
         break;
//      case okSubject:
//         writer.newNode(lxBoxing, _logic->defineStructSize(*scope.moduleScope, scope.moduleScope->signatureReference, 0u));
//         writer.appendNode(lxLocalAddress, object.param);
//         writer.appendNode(lxTarget, scope.moduleScope->signatureReference);
//         break;
      case okLocalAddress:
      case okFieldAddress:
      case okReadOnlyFieldAddress:
      {
         LexicalType type = object.kind == okLocalAddress ? lxLocalAddress : lxFieldAddress;
         if (!test(mode, HINT_NOBOXING) || test(mode, HINT_DYNAMIC_OBJECT)) {
            bool variable = false;
            int size = _logic->defineStructSizeVariable(*scope.moduleScope, resolveObjectReference(scope, object), object.element, variable);
            if (size < 0 && type == lxFieldAddress) {
               // if it is fixed-size array
               size = defineFieldSize(scope, object.param) * (-size);
            }
            writer.newNode((variable && !test(mode, HINT_NOUNBOXING)) ? lxUnboxing : lxBoxing, size);

            writer.appendNode(type, object.param);
            if (test(mode, HINT_DYNAMIC_OBJECT))
               writer.appendNode(lxBoxingRequired);
         }
         else writer.newNode(type, object.param);
         break;
      }
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
      case okConstantRole:
         writer.newNode(lxConstantSymbol, object.param);
         break;
      case okExternal:
         return;
      case okInternal:
         writer.appendNode(lxInternalRef, object.param);
         return;
   }

   writeTarget(writer, resolveObjectReference(scope, object), object.element);

   if (!test(mode, HINT_NODEBUGINFO))
      writeTerminalInfo(writer, terminal);

   writer.closeNode();
}

ObjectInfo Compiler :: compileTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, int mode)
{
   ident_t token = terminal.identifier();

   ObjectInfo object;
   switch (terminal.type) {
//      case lxConstantList:
//            // HOTFIX : recognize predefined constant lists
//            object = ObjectInfo(okArrayConst, terminal.argument, scope.moduleScope->arrayReference);
//         break;
      case lxLiteral:
         object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(token), scope.moduleScope->literalReference);
         break;
      case lxWide:
         object = ObjectInfo(okWideLiteralConstant, scope.moduleScope->module->mapConstant(token), scope.moduleScope->wideReference);
         break;
//      case lxCharacter:
//         object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(token));
//         break;
      case lxInteger:
      {
         String<char, 20> s;

         int integer = token.toInt();
         if (errno == ERANGE)
            scope.raiseError(errInvalidIntNumber, terminal);

         // convert back to string as a decimal integer
         s.appendHex(integer);

         object = ObjectInfo(okIntConstant, scope.module->mapConstant((const char*)s), V_INT32, 0, integer);
         break;
      }
//      case lxLong:
//      {
//         String<char, 30> s("_"); // special mark to tell apart from integer constant
//         s.append(token, getlength(token) - 1);
//
//         token.toULongLong(10, 1);
//         if (errno == ERANGE)
//            scope.raiseError(errInvalidIntNumber, terminal);
//
//         object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s));
//         break;
//      }
      case lxHexInteger:
      {
         String<char, 20> s;

         int integer = token.toULong(16);
         if (errno == ERANGE)
            scope.raiseError(errInvalidIntNumber, terminal);

         // convert back to string as a decimal integer
         s.appendHex(integer);

         object = ObjectInfo(okUIntConstant, scope.moduleScope->module->mapConstant((const char*)s), V_INT32, 0, integer);
         break;
      }
//      case lxReal:
//      {
//         String<char, 30> s(token, getlength(token) - 1);
//         token.toDouble();
//         if (errno == ERANGE)
//            scope.raiseError(errInvalidIntNumber, terminal);
//
//         // HOT FIX : to support 0r constant
//         if (s.Length() == 1) {
//            s.append(".0");
//         }
//
//         object = ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant((const char*)s));
//         break;
//      }
//      case lxMemberIdentifier:
//         object = scope.mapMember(token.c_str() + 1);
//         break;
//      case lxGlobalReference:
//         object = scope.mapGlobal(token.c_str() + 1);
//         break;
//      case lxPrivate:
//         object = scope.mapLocal(token.c_str() + 1);
//         break;
//      case lxLocal:
//         // if it is a temporal variable
//         object = ObjectInfo(okLocal, terminal.argument);
//         break;
//      case lxExplicitConst:
//      {
//         // try to resolve explicit constant
//         size_t len = getlength(token);
//         
//         IdentifierString action(token + len - 1);
//         ref_t dummyRef = 0;
//         ref_t actionRef = scope.module->mapAction(action, scope.module->mapSignature(&scope.moduleScope->literalReference, 1, false), dummyRef);
//
//         action.copy(token, len - 1);
//         object = ObjectInfo(okExplicitConstant, scope.moduleScope->module->mapConstant(action), actionRef);
//         break;
//      }
      default:
         if (test(mode, HINT_FORWARD)) {
            IdentifierString forwardName(FORWARD_MODULE, "'", token);

            object = scope.mapTerminal(forwardName.ident(), true, 0);
         }
         else if (test(mode, HINT_EXTERNALOP)) {
            object = ObjectInfo(okExternal, 0, V_INT32);
         }
         else if (test(mode, HINT_INTERNALOP)) {
            object = ObjectInfo(okInternal, scope.module->mapReference(token), V_INT32);
         }
         else object = scope.mapTerminal(token, terminal == lxReference, 0);
         break;
   }

//   if (object.kind == okExplicitConstant) {
//      // replace an explicit constant with the appropriate object
//      writer.newBookmark();
//      writeTerminal(writer, terminal, scope, ObjectInfo(okLiteralConstant, object.param) , mode);
//
//      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
//      Pair<ref_t, ref_t>  constInfo = nsScope->extensions.get(encodeMessage(object.extraparam, 1) | SPECIAL_MESSAGE);
//      if (constInfo.value2 != 0) {
//         ref_t signRef = 0;
//         scope.module->resolveAction(object.extraparam, signRef);
//         if (!convertObject(writer, scope, constInfo.value2, V_STRCONSTANT, signRef))
//            scope.raiseError(errInvalidConstant, terminal);
//      }
//      else scope.raiseError(errInvalidConstant, terminal);
//
//      object = ObjectInfo(okObject, constInfo.value2);
//
//      writer.removeBookmark();
//   }            
   /*else */if (!test(mode, HINT_VIRTUALEXPR))
      writeTerminal(writer, terminal, scope, object, mode);

   return object;
}

ObjectInfo Compiler :: mapClassSymbol(Scope& scope, int classRef)
{
   if (classRef) {
      ObjectInfo retVal(okClass);
      retVal.param = classRef;

      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, classRef, true);
      retVal.reference = info.header.classRef;

      return retVal;
   }
   else return ObjectInfo(okUnknown);
}

ObjectInfo Compiler :: compileTemplateSymbol(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   ObjectInfo retVal = mapClassSymbol(scope, resolveTemplateDeclaration(node, scope));
      
   writeTerminal(writer, node, scope, retVal, mode);

   return retVal;
}

ObjectInfo Compiler :: compileObject(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, int mode)
{
   ObjectInfo result;

   if (test(mode, HINT_REFOP)) {
      result = compileReferenceExpression(writer, node, scope, mode & ~HINT_REFOP);
   }
//   if (test(mode, HINT_COLLECTION_MODE)) {
//      result = compileCollection(writer, node.parentNode(), scope);
//   }
   else {
      switch (node.type) {
         case lxCodeExpression:
         case lxCode:
            result = compileCodeExpression(writer, node, scope, mode);
            break; 
         case lxTemplate:
            result = compileTemplateSymbol(writer, node, scope, mode);
            break;
         case lxNestedClass:
            result = compileClosure(writer, node, scope, mode & HINT_CLOSURE_MASK);
            break;
//            ////      case lxNestedClass:
//         case lxLazyExpression:
//            result = compileClosure(writer, node, scope, mode & HINT_CLOSURE_MASK);
//            break;
         case lxClosureExpr:
            result = compileClosure(writer, node, scope, mode & HINT_CLOSURE_MASK);
            break;
         case lxExpression:
//            else if (member == lxNestedClass) {
//               result = compileClosure(writer, member, scope, mode & HINT_CLOSURE_MASK);
//            }
            result = compileExpression(writer, node, scope, targetRef, mode);
            break;
//         case lxBoxing:
//            result = compileBoxingExpression(writer, node, scope, mode);
//            break;
//         case lxMessageReference:
//            result = compileMessageReference(writer, node, scope, mode);
//            break;
//         case lxAlt:
//            compileAltOperation(writer, node, scope);
//
//            result = ObjectInfo(okObject);
//            break;
//         case lxTrying:
//            compileTrying(writer, node, scope);
//
//            result = ObjectInfo(okObject);
//            break;
//         case lxSwitching:
//            compileSwitch(writer, node, scope);
//            
//            result = ObjectInfo(okObject);
//            break;
         case lxIdle:
            result = ObjectInfo(okUnknown);
            break;
         default:
            result = compileTerminal(writer, node, scope, mode);
      }
   }

   return result;
}

//ObjectInfo Compiler :: compileMessageReference(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
//{
//   SNode terminal = node.findChild(lxIdentifier, lxLiteral);
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
//         bool referenceOne = signature.ident().find('\'') != NOTFOUND_POS;
//         extensionRef = resolveImplicitIdentifier(scope, signature.c_str(), referenceOne);
//         if (extensionRef == 0)
//            scope.raiseError(errUnknownClass, terminal);
//
//         subject++;
//      }
//
//      int param = 0;
//      for (size_t i = subject; i < getlength(message); i++) {
//         if (message[i] == ':') {
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
////   if (arg.argument != 0)
////      return arg.argument;

   SNode name = current.findChild(/*lxPrivate, */lxIdentifier/*, lxReference*/);
   //HOTFIX : if generated by a script / closure call
   if (name == lxNone)
      name = current;

   if (name == lxNone)
      scope.raiseError(errInvalidOperation, node);

   messageStr.copy(name.identifier());

   current = current.nextNode();

   int paramCount = 0;
   // if message has generic argument list
   while (true) {
      if (current == lxPropertyParam) {
         // COMPILER MAGIC : recognize the property get call
         actionFlags = PROPERTY_MESSAGE;
      }
      else if (test(current.type, lxObjectMask)) {
         //if (paramCount < OPEN_ARG_COUNT)
         paramCount++;
      }
      else if (current == lxMessage) {
         messageStr.append(':');
         messageStr.append(current.firstChild(lxTerminalMask).identifier());
      }
      else break;

      current = current.nextNode();
   }

//   if (paramCount >= OPEN_ARG_COUNT)
//      paramCount = OPEN_ARG_COUNT;

   if (messageStr.Length() == 0) {
      actionFlags |= SPECIAL_MESSAGE;

      // if it is an implicit message
      messageStr.copy(INVOKE_MESSAGE);
   }

   // if signature is presented
   ref_t actionRef = scope.moduleScope->module->mapAction(messageStr, 0, false);

   // create a message id
   return encodeMessage(actionRef, paramCount, actionFlags);
}

ref_t Compiler :: mapExtension(CodeScope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo object, int& stackSafeAttr)
{
   ref_t objectRef = resolveObjectReference(scope, object);
   if (isPrimitiveRef(objectRef)) {
      //if (objectRef != V_ARGARRAY)
         objectRef = resolvePrimitiveReference(scope, objectRef, object.element);
   }
   else if (objectRef == scope.moduleScope->superReference) {
      objectRef = 0;
   }

   // general extension
   ref_t generalRoleRef1 = 0;
   ref_t roleRef1 = 0;
   ref_t strongMessage1 = 0;

   // typified extension
   ref_t generalRoleRef2 = 0;
   ref_t roleRef2 = 0;
   ref_t strongMessage2 = 0;

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
   for (auto it = nsScope->extensions.getIt(messageRef); !it.Eof(); it = nsScope->extensions.nextIt(messageRef, it)) {
      if (_logic->isCompatible(*scope.moduleScope, (*it).value1, objectRef)) {
         ref_t resolvedMessageRef = _logic->resolveMultimethod(*scope.moduleScope, messageRef, (*it).value2, implicitSignatureRef, stackSafeAttr);
         if (resolvedMessageRef) {
            if ((*it).value1) {
               if (!roleRef2) {
                  strongMessage2 = resolvedMessageRef;
                  roleRef2 = (*it).value2;
               }
            }
            else {
               strongMessage1 = resolvedMessageRef;
               roleRef1 = (*it).value2;
            }
         }
         else {
            if ((*it).value1) {
               if (!generalRoleRef2)
                  generalRoleRef2 = (*it).value2;
            }
            else {
               if (!generalRoleRef1)
                  generalRoleRef1 = (*it).value2;
            }
         }
      }
   }

   if (roleRef2) {
      // if it is strong typed message and extension
      messageRef = strongMessage2;
      stackSafeAttr |= 1;

      return roleRef2;
   }
   else if (roleRef1) {
      // if it is strong typed message and general extension
      messageRef = strongMessage1;

      return roleRef1;
   }
   else if (generalRoleRef2) {
      stackSafeAttr |= 1;

      // if it is general message and strong typed extension
      return generalRoleRef2;
   }
   else if (generalRoleRef1) {
      // if it is general message and extension
      return generalRoleRef1;
   }

   //if (objectRef == V_ARGARRAY) {
   //   // HOTFIX : variadic target - try to resolve extension for the array

   //   return mapExtension(scope, messageRef, implicitSignatureRef, ObjectInfo(okObject, scope.moduleScope->arrayReference), stackSafeAttr);
   //}

   return 0;
}

void Compiler :: compileBranchingNodes(SyntaxWriter& writer, SNode thenBody, CodeScope& scope, ref_t ifReference, bool loopMode/*, bool switchMode*/)
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

      //// HOTFIX : switch mode - ignore else
      //if (!switchMode) {
         SNode elseNode = thenBody./*firstChild().*/nextNode();
         SNode elseCode = elseNode.findSubNode(lxCode);
         if (elseCode != lxNone) {
            writer.newNode(lxElse, 0);
            compileBranching(writer, elseCode, scope);
            writer.closeNode();
         }
      //}
   }
}

ref_t Compiler :: resolveOperatorMessage(Scope& scope, ref_t operator_id, int paramCount)
{
   switch (operator_id) {
      case IF_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(IF_MESSAGE, 0, false), paramCount, 0);
      case EQUAL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(EQUAL_MESSAGE, 0, false), paramCount, 0);
      case NOTEQUAL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTEQUAL_MESSAGE, 0, false), paramCount, 0);
      case LESS_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(LESS_MESSAGE, 0, false), paramCount, 0);
      case NOTLESS_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTLESS_MESSAGE, 0, false), paramCount, 0);
      case GREATER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(GREATER_MESSAGE, 0, false), paramCount, 0);
      case NOTGREATER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTGREATER_MESSAGE, 0, false), paramCount, 0);
      case ADD_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(ADD_MESSAGE, 0, false), paramCount, 0);
      case SUB_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SUB_MESSAGE, 0, false), paramCount, 0);
      case MUL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(MUL_MESSAGE, 0, false), paramCount, 0);
      case DIV_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(DIV_MESSAGE, 0, false), paramCount, 0);
      case AND_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(AND_MESSAGE, 0, false), paramCount, 0);
      case OR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(OR_MESSAGE, 0, false), paramCount, 0);
      case XOR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(XOR_MESSAGE, 0, false), paramCount, 0);
      case SHIFTR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTR_OPERATOR, 0, false), paramCount, 0);
      case SHIFTL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTL_OPERATOR, 0, false), paramCount, 0);
      case REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(REFER_MESSAGE, 0, false), paramCount, 0);
      case SET_REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SET_REFER_MESSAGE, 0, false), paramCount, 0);
      default:
         throw InternalError("Not supported operator");
         break;
   }
}

void Compiler :: compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal)
{
   bool loopMode = test(mode, HINT_LOOP);
   //bool switchMode = test(mode, HINT_SWITCH);

   // HOTFIX : in loop expression, else node is used to be similar with branching code
   // because of optimization rules
   ref_t original_id = operator_id;
   if (loopMode) {
      operator_id = operator_id == IF_OPERATOR_ID ? IFNOT_OPERATOR_ID : IF_OPERATOR_ID;
   }

   ref_t ifReference = 0;
   ref_t resolved_operator_id = operator_id;
   // try to resolve the branching operator directly
   if (_logic->resolveBranchOperation(*scope.moduleScope, resolved_operator_id, resolveObjectReference(scope, loperand), ifReference)) {
      // good luck : we can implement branching directly
      compileBranchingNodes(writer, roperandNode, scope, ifReference, loopMode/*, switchMode*/);

      writer.insert(loopMode ? lxLooping : lxBranching, /*switchMode ? -1 : */0);
      writer.closeNode();
   }
   else {
      operator_id = original_id;

      // bad luck : we have to create closure
      SNode elseNode = roperandNode.nextNode();
      SNode elseCode = elseNode.findSubNode(lxCode);
      int message = resolveOperatorMessage(scope, operator_id, 1);
      if (elseCode != lxNone) {
         message = overwriteParamCount(message, 2);

         compileClosure(writer, elseCode.parentNode(), scope, HINT_SUBCODE_CLOSURE);
      }
      compileClosure(writer, roperandNode, scope, HINT_SUBCODE_CLOSURE);

      retVal = compileMessage(writer, roperandNode, scope, loperand, message, 0, 0);

      if (loopMode) {
         writer.insert(lxLooping);
         writer.closeNode();
      }
   }
}

ObjectInfo Compiler :: compileBranchingOperator(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, ObjectInfo loperand, int mode, int operator_id)
{
   ObjectInfo retVal(okObject);

   compileBranchingOperand(writer, roperandNode, scope, mode, operator_id, loperand, retVal);

   return retVal;
}

//ObjectInfo Compiler :: compileIsNilOperator(SyntaxWriter& writer, SNode, CodeScope& scope, ObjectInfo loperand, ObjectInfo roperand)
//{
//   ref_t loperandRef = resolveObjectReference(scope, loperand);
//   ref_t roperandRef = resolveObjectReference(scope, roperand);
//   
//   ref_t resultRef = _logic->isCompatible(*scope.moduleScope, loperandRef, roperandRef) ? loperandRef : 0;
//
//   writer.insert(lxNilOp, ISNIL_MESSAGE_ID);
//   writer.closeNode();
//
//   return ObjectInfo(okObject, resultRef);
//}

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
      //HOTFIX : allow to work with int constants
      if (roperand2.kind == okIntConstant && loperandRef == V_OBJARRAY)
         roperand2Ref = 0;

      operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, roperand2Ref, resultClassRef);

      //if (roperand2Ref == V_NIL && loperandRef == V_INT32ARRAY && operator_id == SET_REFER_MESSAGE_ID) {
      //   //HOTFIX : allow set operation with nil
      //   operator_id = SETNIL_REFER_MESSAGE_ID;
      //}
   }
   else operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, resultClassRef);

   // HOTFIX : primitive operations can be implemented only in the method
   // because the symbol implementations do not open a new stack frame
   if (operationType != 0 && resultClassRef != V_FLAG && scope.getScope(Scope::slMethod) == NULL) {
      operationType = 0;
   }

   //bool assignMode = false;
   if (operationType != 0) {
      // if it is a primitive operation
      _logic->injectOperation(writer, *scope.moduleScope, operator_id, operationType, resultClassRef, loperand.element);

      retVal = assignResult(writer, scope, resultClassRef, loperand.element);
   }
   // if not , replace with appropriate method call
   else {
      int operationMode = HINT_NODEBUGINFO;
      ref_t implicitSignatureRef = 0;
      if (roperand2.kind != okUnknown) {
         implicitSignatureRef = resolveStrongArgument(scope, roperand, roperand2);
      }
      else implicitSignatureRef = resolveStrongArgument(scope, roperand);

      int stackSafeAttr = 0;
      int messageRef = resolveMessageAtCompileTime(loperand, scope, resolveOperatorMessage(scope, operator_id, paramCount), 
         implicitSignatureRef, true, stackSafeAttr);

      if (!test(stackSafeAttr, 1)) {
         operationMode |= HINT_DYNAMIC_OBJECT;
      }
      else stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

      retVal = compileMessage(writer, node, scope, loperand, messageRef, operationMode, stackSafeAttr);
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo loperand, int, int operator_id)
{
   ObjectInfo retVal(okObject);
   int paramCount = 1;

//   writer.newBookmark();
//   writer.appendNode(lxOperatorAttr);

   ObjectInfo roperand;
   ObjectInfo roperand2;
   if (operator_id == SET_REFER_OPERATOR_ID) {
      // HOTFIX : overwrite the assigning part
      SNode exprNode = node;
      SNode expr2Node = node.parentNode().nextNode();
      expr2Node = lxIdle;
      expr2Node = expr2Node.nextNode(lxObjectMask);

      //SNode thirdNode = exprNode.nextNode(lxObjectMask);
      //SyntaxTree::copyNodeSafe(exprNode.nextNode(lxObjectMask), exprNode.appendNode(lxExpression), true);

      roperand = compileObject(writer, exprNode, scope, 0, 0);
      roperand2 = compileExpression(writer, expr2Node, scope, 0, 0);

      node = exprNode;

      paramCount++;
   }
   else {
      SNode roperandNode = node;
      /*if (roperandNode == lxLocal) {
         // HOTFIX : to compile switch statement
         roperand = ObjectInfo(okLocal, roperandNode.argument);
      }*/
      /*if (test(roperandNode.type, lxTerminalMask)) {*/
         roperand = compileObject(writer, roperandNode, scope, 0, 0);
      /*}
      else roperand = compileExpression(writer, roperandNode, scope, 0, 0);*/
   }

   //if (operator_id == ISNIL_MESSAGE_ID) {
   //   retVal = compileIsNilOperator(writer, node, scope, loperand, roperand);
   //}
   /*else */retVal = compileOperator(writer, node, scope, operator_id, paramCount, loperand, roperand, roperand2);

//   writer.removeBookmark();

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode)
{
   SNode current = node;
   int operator_id = current.argument != 0 ? current.argument : _operators.get(current.firstChild(lxTerminalMask).identifier());

   SNode roperand = node.nextNode();
//   if (operatorNode.prevNode() == lxNone)
//      roperand = roperand.nextNode(lxObjectMask);

   if (test(mode, HINT_PROP_MODE) && operator_id == REFER_OPERATOR_ID) {
      operator_id = SET_REFER_OPERATOR_ID;
   }

   // if it is branching operators
   if (operator_id == IF_OPERATOR_ID/* || operator_id == IFNOT_MESSAGE_ID*/) {
      return compileBranchingOperator(writer, roperand, scope, target, mode, operator_id);
   }
   else return compileOperator(writer, roperand, scope, target, mode, operator_id);
}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode, int stackSafeAttr)
{
   ObjectInfo retVal(okObject);

   LexicalType operation = lxCalling;
   int argument = messageRef;

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target);
   bool dispatchCall = false;
   _CompilerLogic::ChechMethodInfo result;
   int callType = _logic->resolveCallType(*scope.moduleScope, classReference, messageRef, result);
   if (result.found) {
      retVal.reference = result.outputReference;
   }

//   if ((target.kind == okSelfParam || target.kind == okOuterSelf || target.kind == okClassSelf) && callType == tpPrivate) {
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
//      argument = encodeAction(DISPATCH_MESSAGE_ID);
//
//      writer.appendNode(lxOvreriddenMessage, messageRef);
//   }
   /*else */if (callType == tpClosed || callType == tpSealed) {
      operation = callType == tpClosed ? lxSDirctCalling : lxDirectCalling;
      argument = messageRef;
      //if (result.withOpenArgDispatcher) {
      //   argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT);
      //}
      //else if (result.withOpenArg1Dispatcher) {
      //   argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT + 1);
      //}
      //else if (result.withOpenArg2Dispatcher) {
      //   argument = overwriteParamCount(messageRef, OPEN_ARG_COUNT + 2);
      //}

      if (!test(mode, HINT_DYNAMIC_OBJECT) && _logic->isEmbeddable(*scope.moduleScope, classReference) && result.stackSafe)
         // if the method directly resolved and the target is not required to be dynamic, mark it as stacksafe
         stackSafeAttr |= 1;

      //if (result.embeddable)
      //   writer.appendNode(lxEmbeddableAttr);
   }
   else {
      // if the sealed / closed class found and the message is not supported - warn the programmer and raise an exception
      if (test(mode, HINT_SILENT)) {
         // do nothing in silent mode
      }
      else if (/*result.found && !result.withCustomDispatcher && */callType == tpUnknown && result.directResolved) {
         if (test(mode, HINT_ASSIGNING_EXPR)) {
            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxExpression).findChild(lxMessage));
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node);
      }         
   }

   if (stackSafeAttr && !dispatchCall && !result.dynamicRequired)
      writer.appendNode(lxStacksafeAttr, stackSafeAttr);

   if (classReference)
      writer.appendNode(lxCallTarget, classReference);

   if (result.outputReference)
      writer.appendNode(lxTarget, result.outputReference);

   if (!test(mode, HINT_NODEBUGINFO)) {
      // set a breakpoint
      writer.newNode(lxBreakpoint, dsStep);

      SNode messageNode = node.findChild(lxIdentifier/*, lxPrivate*/);
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

   // inserting calling expression
   writer.insert(operation, argument);
   writer.closeNode();

   return retVal;
}

bool Compiler :: convertObject(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ObjectInfo source)
{
   ref_t sourceRef = resolveObjectReference(scope, source);
   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
      // if it can be boxed / implicitly converted
      if (!_logic->injectImplicitConversion(writer, *scope.moduleScope, *this, targetRef, sourceRef, source.element))
         return typecastObject(writer, scope, targetRef, source);
   }
   return true;
}

bool Compiler :: typecast(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t signature)
{
   ref_t signatures[ARG_COUNT];
   size_t count = scope.module->resolveSignature(signature, signatures);
   if (count == 1) {
      return typecastObject(writer, scope, targetRef, ObjectInfo(okObject, 0, signatures[0]));
   }
   else return false;   
}

bool Compiler :: typecastObject(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ObjectInfo source)
{
   if (targetRef != 0 && !isPrimitiveRef(targetRef)) {
      if (targetRef != scope.moduleScope->superReference) {
         //HOTFIX : ignore super object
         ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         //writer.appendNode(lxTypecasting);

         compileMessage(writer, SNode(), scope, source, encodeAction(actionRef), HINT_NODEBUGINFO | HINT_SILENT, 0);
      }

      return true;
   }
   else return false;
}

ref_t Compiler :: resolveStrongArgument(CodeScope& scope, ObjectInfo info)
{
   ref_t argRef = resolveObjectReference(scope, info);
   if (isPrimitiveRef(argRef))
      argRef = _logic->resolvePrimitiveReference(*scope.moduleScope, argRef);

   return scope.module->mapSignature(&argRef, 1, false);
}

ref_t Compiler::resolveStrongArgument(CodeScope& scope, ObjectInfo info1, ObjectInfo info2)
{
   ref_t argRef[2];

   argRef[0] = resolveObjectReference(scope, info1);
   argRef[1] = resolveObjectReference(scope, info2);

   if (isPrimitiveRef(argRef[0]))
      argRef[0] = _logic->resolvePrimitiveReference(*scope.moduleScope, argRef[0]);
   if (isPrimitiveRef(argRef[1]))
      argRef[1] = _logic->resolvePrimitiveReference(*scope.moduleScope, argRef[1]);

   return scope.module->mapSignature(argRef, 2, false);
}

ref_t Compiler :: resolvePrimitiveReference(Scope& scope, ref_t argRef, ref_t elementRef)
{
   if (argRef == V_WRAPPER) {
      argRef = resolveReferenceTemplate(scope, elementRef);
   }
   else if (isPrimitiveArrRef(argRef)) {
      argRef = resolvePrimitiveArray(scope, elementRef);
   }
   else argRef = _logic->resolvePrimitiveReference(*scope.moduleScope, argRef);

   return argRef;
}

ref_t Compiler :: compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   int paramMode = 0;
   bool externalMode = false;
   //bool anonymousVariadic = false;
   if (test(mode, HINT_EXTERNALOP)) {
      externalMode = true;
   }
   else paramMode |= HINT_NOPRIMITIVES;

   SNode current = node;

   // compile the message argument list
   ref_t signatures[ARG_COUNT];
   ref_t signatureLen = 0;
   while (/*current != lxMessage && */current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (externalMode)
            writer.newNode(lxExtArgument);

         // try to recognize the message signature
         ref_t argRef = resolveObjectReference(scope, compileExpression(writer, current, scope, 0, paramMode));
         //if (signatureLen == OPEN_ARG_COUNT) {
         //   if (signatures[signatureLen - 1] != argRef)
         //      anonymousVariadic = true;
         //}
         /*else */if (argRef) {
            signatures[signatureLen++] = argRef;
         }
         else signatures[signatureLen++] = scope.moduleScope->superReference;

         if (externalMode) {
            writer.appendNode(lxExtArgumentRef, argRef);
            writer.closeNode();
         }            
      }

      current = current.nextNode();
   }

   if (signatureLen > 0/* && !anonymousVariadic*/) {
      bool anonymous = true;
      for (ref_t i = 0; i < signatureLen; i++) {
         if (signatures[i] != scope.moduleScope->superReference) {
            anonymous = false;
            break;
         }
      }
      if (!anonymous)
         return scope.module->mapSignature(signatures, signatureLen, false);
   }

   return 0;
}

ref_t Compiler :: resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef, 
   bool withExtension, int& stackSafeAttr)
{
   ref_t resolvedMessageRef = 0;
   ref_t targetRef = resolveObjectReference(scope, target);

   resolvedMessageRef = _logic->resolveMultimethod(*scope.moduleScope, generalMessageRef, targetRef, implicitSignatureRef, stackSafeAttr);

   if (resolvedMessageRef != 0) {
      // if the object handles the compile-time resolved message - use it
      return resolvedMessageRef;
   }

   // check the existing extensions if allowed
   if (withExtension) {
      if (checkMethod(*scope.moduleScope, targetRef, generalMessageRef) != tpUnknown) {
         // could be stacksafe
         stackSafeAttr |= 1;

         // if the object handles the message - do not use extensions
         return generalMessageRef;
      }

      ref_t extensionRef = mapExtension(scope, generalMessageRef, implicitSignatureRef, target, stackSafeAttr);
      if (extensionRef != 0) {
         // if there is an extension to handle the compile-time resolved message - use it
         target = ObjectInfo(okConstantRole, extensionRef, extensionRef);

         return generalMessageRef;
      }
   }

   // otherwise - use the general message
   return generalMessageRef;
}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope/*, ref_t exptectedRef*/, ObjectInfo target, int mode)
{
   int paramsMode = 0;
   if (target.kind == okExternal) {
      paramsMode |= HINT_EXTERNALOP;
   }

//   ref_t implicitSignatureRef = 0;
//   size_t paramCount = 0;
   ObjectInfo retVal;
   ref_t implicitSignatureRef = compileMessageParameters(writer, node, scope, paramsMode/* & (HINT_RESENDEXPR | HINT_PARAMETERSONLY)*/);

   //   bool externalMode = false;
   if (target.kind == okExternal) {
      int extMode = mode & HINT_ROOT;
      
      retVal = compileExternalCall(writer, node.prevNode(), scope/*, exptectedRef*/, extMode);
   }
   else {
      ref_t messageRef = mapMessage(node, scope/*, paramCount*/);

      if (target.kind == okInternal) {
         retVal = compileInternalCall(writer, node, scope, messageRef, implicitSignatureRef, target);
      }
      else {
         int stackSafeAttr = 0;
         messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, true, stackSafeAttr);

         if (!test(stackSafeAttr, 1)) {
            mode |= HINT_DYNAMIC_OBJECT;
         }
         else if (target.kind != okConstantRole)
            stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

         retVal = compileMessage(writer, node, scope, target, messageRef, mode, stackSafeAttr);
      }
   }

   return retVal;
}

//void Compiler :: inheritClassConstantList(_CompilerScope& scope, ref_t sourceRef, ref_t targetRef)
//{
//   ref_t moduleRef = 0;
//   _Module* parent = scope.loadReferenceModule(sourceRef, moduleRef);
//
//   _Memory* source = parent->mapSection(moduleRef | mskRDataRef, true);
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
//void Compiler :: compileClassConstantAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo retVal)
//{
//   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
//   bool accumulatorMode = false;
//
//   if (scope.getMessageID() == (encodeAction(INIT_MESSAGE_ID) | SPECIAL_MESSAGE) && classScope != NULL) {
//      ref_t valueRef = classScope->info.staticValues.get(retVal.param);
//      SymbolScope constantScope((NamespaceScope*)scope.getScope(Scope::slNamespace), valueRef & ~mskAnyRef);
//
//      SNode sourceNode = node;
//      SNode operatorNode = sourceNode.findSubNode(lxOperator);
//      if (operatorNode != lxNone && operatorNode.identifier().compare("+")) {
//         SNode firstNode = sourceNode.findSubNodeMask(lxObjectMask);
//         ObjectInfo info = scope.mapTerminal(firstNode.identifier(), firstNode == lxReference, 0);
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
//      //ObjectInfo source = compileExpression(writer, sourceNode, scope, 0, 0);
//      ObjectInfo source = compileObject(writer, sourceNode, scope, 0, 0);
//      ref_t targetRef = accumulatorMode ? retVal.element : retVal.extraparam;
//      ref_t sourceRef = resolveConstantObjectReference(scope, source);
//
//      if (compileSymbolConstant(sourceNode, constantScope, source, accumulatorMode) && _logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
//      }
//      else scope.raiseError(errInvalidOperation, node);
//   }
//   else scope.raiseError(errInvalidOperation, node);
//}

bool Compiler :: resolveAutoType(ObjectInfo source, ObjectInfo& target, CodeScope& scope)
{
   ref_t sourceRef = resolveObjectReference(scope, source);

   if (isPrimitiveRef(sourceRef))
      sourceRef = _logic->resolvePrimitiveReference(*scope.moduleScope, sourceRef);

   return scope.resolveAutoType(target, sourceRef, source.element);
}

ObjectInfo Compiler :: compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target)
{
   ObjectInfo retVal = target;
   LexicalType operationType = lxAssigning;
   int operand = 0;

   SNode current = node;
   SNode sourceNode = current.nextNode(lxObjectMask);

//   if (scope.isInitializer()) {
//      // HOTFIX : recognize static field initializer
//      if (target.kind == okStaticField || target.kind == okStaticConstantField) {
//         // HOTFIX : static field initializer should be compiled as preloaded symbol
//         if (!isSealedStaticField(target.param) && target.kind == okStaticConstantField) {
//            compileClassConstantAssigning(writer, sourceNode, scope, target);
//         }
//         else compileStaticAssigning(target, sourceNode, *((ClassScope*)scope.getScope(Scope::slClass)));
//
//         writer.trim();
//
//         //writer.removeBookmark();
//
//         return ObjectInfo();
//      }
//   }

   ref_t targetRef = resolveObjectReference(scope, target);
   bool byRefAssigning = false;
   switch (target.kind) {
      case okLocal:
      case okField:
//      case okStaticField:
      case okOuterField:
      case okOuterStaticField:
         break;
      case okLocalAddress:
      case okFieldAddress:
      {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
         if (size != 0) {
            operand = size;
         }
         else scope.raiseError(errInvalidOperation, sourceNode);
         break;
      }
      case okOuter:
      case okOuterSelf:
      {
         InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::slClass);
            
         if (!closure->markAsPresaved(target))
            scope.raiseError(errInvalidOperation, sourceNode);

         break;
      }
      case okReadOnlyField:
      case okReadOnlyFieldAddress:
      case okOuterReadOnlyField:
         scope.raiseError(errReadOnlyField, node.parentNode());
         break;
      case okParam:
         if (targetRef == V_WRAPPER) {
            byRefAssigning = true;
            targetRef = target.element;
            size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
            if (size != 0) {
               operand = size;
            }

            break;
         }
      default:
         scope.raiseError(errInvalidOperation, sourceNode);
         break;
   }
      
   int assignMode = HINT_NOUNBOXING | HINT_ASSIGNING_EXPR;
   if (targetRef == 0 || targetRef == V_AUTO)
      assignMode |= HINT_DYNAMIC_OBJECT;

   //if (isPrimitiveRef(targetRef))
   //   targetRef = resolvePrimitiveReference(scope, targetRef, target.element);

   if (targetRef == V_AUTO) {
      // support auto attribute
      retVal = compileExpression(writer, sourceNode, scope, 0, assignMode);
      if (resolveAutoType(retVal, target, scope)) {
         targetRef = resolveObjectReference(scope, retVal);
      }
      else scope.raiseError(errInvalidOperation, node);
   }
   else retVal = compileExpression(writer, sourceNode, scope, targetRef, assignMode);

//   else if (retVal.kind == okPrimitiveConv) {
//      if (retVal.param == V_REAL64) {
//         if (retVal.extraparam == V_INT32) {
//            writer.appendNode(lxIntConversion);
//         }
//         writer.insert(lxRealOp, SET_MESSAGE_ID);
//         writer.closeNode();
//      }
//   }
//   else {
      writer.insert(operationType, operand);
      writer.closeNode();
   //}

   return retVal;
}

ObjectInfo Compiler :: compilePropAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target)
{
   ObjectInfo retVal;

   // tranfer the message into the property set one
   ref_t messageRef = mapMessage(node, scope);
   ref_t actionRef, flags;
   int paramCount;
   decodeMessage(messageRef, actionRef, paramCount, flags);
   if (paramCount == 0 && test(flags, PROPERTY_MESSAGE)) {
      messageRef = encodeMessage(actionRef, 1, flags);
   }
   else scope.raiseError(errInvalidOperation, node);

   // find and compile the parameter
   SNode current = node.parentNode().parentNode().findChild(lxAssign);
   SNode sourceNode = current.nextNode(lxObjectMask);

   ObjectInfo source = compileExpression(writer, sourceNode, scope, 0, 0);
   
   //messageRef = resolveMessageAtCompileTime(target, scope, messageRef, resolveStrongArgument(scope, source));
   //
   int mode = HINT_NODEBUGINFO;
   int stackSafeAttr = 0;
   messageRef = resolveMessageAtCompileTime(target, scope, messageRef, resolveStrongArgument(scope, source), true, stackSafeAttr);
   if (!test(stackSafeAttr, 1))
      mode |= HINT_DYNAMIC_OBJECT;

   retVal = compileMessage(writer, node, scope, target, messageRef, mode, stackSafeAttr);

   // remove the assign node to prevent the duplication
   current = lxIdle;

   return retVal;
}

//ObjectInfo Compiler :: compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target)
//{
//   ref_t extensionRef = 0;
//
//   ObjectInfo   role;
//
//   SNode roleNode = node;
//   // check if the extension can be used as a static role (it is constant)
//   SNode roleTerminal = roleNode.firstChild(lxTerminalMask);
//   if (roleTerminal != lxNone) {
//      int flags = 0;
//
//      role = scope.mapTerminal(roleTerminal.identifier(), roleTerminal == lxReference, 0);
//      if (role.kind == okSymbol || role.kind == okConstantSymbol || role.kind == okExtension) {
//         ref_t classRef = role.kind != okSymbol ? role.extraparam : role.param;
//
//         // if the symbol is used inside itself
//         if (classRef == scope.getClassRefId()) {
//            flags = scope.getClassFlags();
//         }
//         // otherwise
//         else {
//            ClassInfo roleClass;
//            scope.moduleScope->loadClassInfo(roleClass, classRef);
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
//      role = compileExpression(writer, roleNode, scope, 0, 0);
//      writer.closeNode();
//   }
//
//   ObjectInfo retVal = compileExtensionMessage(writer, node.nextNode(), scope, target, role, extensionRef);
//
//   return retVal;
//}
//
//// NOTE : targetRef refers to the type for the typified extension method
//ObjectInfo Compiler :: compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo object, ObjectInfo role, ref_t targetRef)
//{
//   ref_t  messageRef = mapMessage(node, scope);
//   ref_t implicitSignatureRef = 0;
//
//   if (targetRef != 0) {
//      if (!convertObject(writer, scope, targetRef, resolveObjectReference(scope, object), object.element))
//         scope.raiseError(errInvalidOperation, node);
//
//      //SNode targetNode = node.firstChild(lxObjectMask);
//
//      //object = compileExpression(writer, targetNode, scope, targetRef, 0);
//
//      //// the target node already compiler so it should be skipped
//      //targetNode = lxResult;
//      implicitSignatureRef = compileMessageParameters(writer, node, scope);
//   }
//   else implicitSignatureRef = compileMessageParameters(writer, node, scope);
//
//   messageRef = resolveMessageAtCompileTime(role, scope, messageRef, implicitSignatureRef);
//
//   return compileMessage(writer, node, scope, role, messageRef, HINT_EXTENSION_MODE, 0);
//}

/*bool*/void Compiler :: declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, int mode)
{
   //bool lazyExpression = test(mode, HINT_LAZY_EXPR);

   ref_t invokeAction = scope.module->mapAction(INVOKE_MESSAGE, 0, false);
   methodScope.message = encodeMessage(/*lazyExpression ? EVAL_MESSAGE_ID : */invokeAction, 0, SPECIAL_MESSAGE);

   ref_t outputRef = 0;
   if (argNode != lxNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope, outputRef);
   }

   ref_t parentRef = scope.info.header.parentRef;
   //if (lazyExpression) {
   //   parentRef = scope.moduleScope->lazyExprReference;
   //}
   //else {
      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

      ref_t closureRef = scope.moduleScope->resolveClosure(*this, methodScope.message/*, outputRef, &nsScope->extensions*/, nsScope->ns);
//      ref_t actionRef = scope.moduleScope->actionHints.get(methodScope.message);
      if (closureRef) {
         parentRef = closureRef;
      }
      else throw InternalError(errClosureError);
   //}

   compileParentDeclaration(SNode(), scope, parentRef);

   //return lazyExpression;
}

void Compiler :: compileAction(SNode node, ClassScope& scope, SNode argNode, int mode)
{
   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   writer.newNode(lxClass, scope.reference);

   MethodScope methodScope(&scope);
   /*bool lazyExpression = */declareActionScope(scope, argNode, methodScope, mode);

   methodScope.closureMode = true;

   scope.include(methodScope.message);
   scope.addHint(methodScope.message, tpAction);

   // exclude abstract flag if presented
   scope.removeHint(methodScope.message, tpAbstract);

   // HOTFIX : if the closure emulates code brackets
   if (test(mode, HINT_SUBCODE_CLOSURE))
      methodScope.subCodeMode = true;

   // if it is single expression
   //if (!lazyExpression) {
      initialize(scope, methodScope);

      compileActionMethod(writer, node, methodScope);
   //}
   //else compileLazyExpressionMethod(writer, node, methodScope);

   //if (node.existChild(lxTypeAttr)) {
   //   // inject a virtual invoke multi-method if required
   //   SyntaxTree virtualTree;
   //   virtualTree.insertNode(0, lxClass, 0);

   //   List<ref_t> implicitMultimethods;
   //   implicitMultimethods.add(encodeMessage(INVOKE_MESSAGE_ID, getAbsoluteParamCount(methodScope.message)));

   //   _logic->injectVirtualMultimethods(*scope.moduleScope, virtualTree.readRoot(), scope.info, *this, implicitMultimethods, lxClassMethod);

   //   generateClassDeclaration(virtualTree.readRoot(), scope, ClassType::ctNone);

   //   compileVMT(writer, virtualTree.readRoot(), scope);
   //}
   //else {
      generateClassDeclaration(SNode(), scope, ClassType::ctClass);

      //if (test(scope.info.header.flags, elWithMuti)) {
      //   // HOTFIX: temporally the closure does not generate virtual multi-method
      //   // so the class should be turned into limited one (to fix bug in multi-method dispatcher)
      //   scope.info.header.flags &= ~elSealed;
      //   scope.info.header.flags |= elClosed;
      //}
//   }

   writer.closeNode();

   scope.save();

   generateClassImplementation(expressionTree.readRoot(), scope);
}

void Compiler :: compileNestedVMT(SNode node, InlineClassScope& scope)
{
   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   // check if the class was already compiled
   if (!node.argument) {
      compileParentDeclaration(node, scope, false);

      //if (scope.abstractBaseMode && test(scope.info.header.flags, elClosed | elNoCustomDispatcher) && _logic->isWithEmbeddableDispatcher(*scope.moduleScope, node)) {
      //   // COMPILER MAGIC : inject interface implementation
      //   _logic->injectInterfaceDisaptch(*scope.moduleScope, *this, node, scope.info.header.parentRef); 
      //}

      declareVMT(node, scope);

      //// check if it is a virtual vmt (only for the class initialization)
      //SNode current = node.firstChild();
      //bool virtualClass = true;
      //while (current != lxNone) {
      //   if (current == lxClassField) {
      //      virtualClass = false;
      //   }
      //   else if (current == lxClassMethod) {
      //      if (!test(current.argument, SEALED_MESSAGE)) {
      //         virtualClass = false;
      //         break;
      //      }
      //   }
      //   current = current.nextNode();
      //}

      //if (virtualClass)
      //   scope.info.header.flags |= elVirtualVMT;

      generateClassDeclaration(node, scope, ClassType::ctClass, true);

      scope.save();
   }
   else scope.moduleScope->loadClassInfo(scope.info, scope.moduleScope->module->resolveReference(node.argument), false);

   writer.newNode(lxClass, scope.reference);

   compileVMT(writer, node, scope);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, false);

   writer.closeNode();
   scope.save();

   generateClassImplementation(expressionTree.readRoot(), scope);
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope)
{
   ref_t closureRef = scope.reference;
   //if (test(scope.info.header.flags, elVirtualVMT))
   //   closureRef = scope.info.header.parentRef;

   if (test(scope.info.header.flags, elStateless)) {
      //ref_t implicitConstructor = encodeMessage(DEFAULT_MESSAGE_ID, 0) | SPECIAL_MESSAGE;
      //if (scope.info.methods.exist(implicitConstructor, true)) {
      //   // if implicit constructor is declared - it should be automatically called
      //   writer.newNode(lxCalling, implicitConstructor);
      //   writer.appendNode(lxConstantSymbol, closureRef);
      //   writer.closeNode();
      //}
      /*else */writer.appendNode(lxConstantSymbol, closureRef);

      // if it is a stateless class
      return ObjectInfo(okConstantSymbol, closureRef, closureRef/*, scope.moduleScope->defineType(scope.reference)*/);
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
         writer.appendNode(lxTarget, closureRef);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node);
      }
      else {
         // dynamic normal symbol
         writer.newNode(lxNested, scope.info.fields.Count());
         writer.appendNode(lxTarget, closureRef);
      }

      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      //int toFree = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

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

      //ref_t initConstructor = encodeMessage(INIT_MESSAGE_ID, 0) | SPECIAL_MESSAGE;

      //if (scope.info.methods.exist(initConstructor)) {
      //   // if implicit constructor is declared - it should be automatically called
      //   writer.newNode(lxOvreriddenMessage, initConstructor);
      //   if (scope.reference != closureRef)
      //      writer.appendNode(lxTarget, scope.reference);
      //   writer.closeNode();
      //}

      writer.closeNode();

      return ObjectInfo(okObject, closureRef);
   }
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode)
{
   ref_t nestedRef = 0;
   //bool singleton = false;
   if (test(mode, HINT_ROOTSYMBOL)) {
      SymbolScope* owner = (SymbolScope*)ownerScope.getScope(Scope::slSymbol);
      if (owner) {
         nestedRef = owner->reference;
         // HOTFIX : symbol should refer to self and $self for singleton closure
         //singleton = node.existChild(lxCode);
      }
   }
   if (!nestedRef) {
      NamespaceScope* namespaceScope = (NamespaceScope*)ownerScope.getScope(Scope::slNamespace);
      nestedRef = ownerScope.moduleScope->mapAnonymous();
   }      

   InlineClassScope scope(&ownerScope, nestedRef);

   // if it is a lazy expression / multi-statement closure without parameters
   SNode argNode = node.firstChild();
//   if (node == lxLazyExpression) {
//      compileAction(node, scope, SNode(), HINT_LAZY_EXPR);
//   }
   /*else */if (argNode == lxCode) {
      compileAction(node, scope, SNode(), /*singleton ? mode | HINT_SINGLETON : */mode);
   }
   else if (node.existChild(lxCode, lxReturning)) {
      SNode codeNode = node.findChild(lxCode, lxReturning);

      // if it is a closure / lambda function with a parameter
      int actionMode = mode;
      //if (singleton)
      //   actionMode |= HINT_SINGLETON;

      compileAction(node, scope, node.findChild(lxIdentifier, /*lxPrivate, */lxMethodParameter/*, lxClosureMessage*/), actionMode);

      // HOTFIX : hide code node because it is no longer required
      codeNode = lxIdle;
   }
   // if it is a nested class
   else compileNestedVMT(node, scope);

   return compileClosure(writer, node, ownerScope, scope);
}

//ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   ref_t parentRef = scope.moduleScope->arrayReference;
//   SNode parentNode = node.firstChild(lxTerminalMask);
//   if (parentNode.compare(lxIdentifier, lxReference, lxGlobalReference)) {
//      parentRef = resolveImplicitIdentifier(scope, parentNode);
//      if (parentRef == 0)
//         scope.raiseError(errUnknownBaseClass, node);
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
//      compileExpression(writer, current, scope, 0, 0);
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
   ref_t targetRef = 0;
   if (test(mode, HINT_ROOT)) {
      targetRef = scope.getReturningRef();
   }

   writer.newBookmark();

   ObjectInfo info = compileExpression(writer, node, scope, targetRef, mode);

   // HOTFIX : implementing closure exit
   if (test(mode, HINT_ROOT)) {
      ObjectInfo retVar = scope.mapTerminal(RETVAL_VAR, false, 0);
      if (retVar.kind != okUnknown) {
         writer.insertChild(0, lxField, retVar.param);

         writer.insert(lxAssigning);
         writer.closeNode();
      }
   }

   writer.removeBookmark();

   return info;
}

//void Compiler :: compileTrying(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   writer.newBookmark();
//
//   //ObjectInfo objectInfo/*- = compileObject(writer, targetNode, scope, 0)*/;
//
//   bool catchNode = false;
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (test(current.type, lxObjectMask)) {
//         if (catchNode) {
//            SNode operationNode = current.firstChild();
//            writer.newBookmark();
//            writer.appendNode(lxResult);
//            while (operationNode != lxNone) {
//               compileOperation(writer, operationNode, scope, /*objectInfo*/ObjectInfo(okObject), 0, 0);
//
//               operationNode = operationNode.nextNode();
//            }
//            writer.removeBookmark();
//         }
//         else compileExpression(writer, current, scope, 0, 0);
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
//   writer.newBookmark(); // !! an extra breakpoint?
//
//   // inject a temporal variable
//   int tempLocal = scope.newLocal();
//   writer.newNode(lxAssigning);
//   writer.appendNode(lxLocal, tempLocal);
//   ObjectInfo objectInfo;
//   if (firstExpr == lxBoxing) {
//      // HOTFIX : to correctly compile alternative operation with typecasting expression
//      objectInfo = compileObject(writer, firstExpr, scope, 0, 0);
//      firstExpr = lxExpression;
//   }
//   else objectInfo = compileObject(writer, targetNode, scope, 0, 0);
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
//         if (catchNode) {
//            writer.newBookmark();
//            writer.appendNode(lxLocal, tempLocal);
//            SNode operationNode = current.firstChild();
//            while (operationNode != lxNone) {
//               compileOperation(writer, operationNode, scope, objectInfo, 0, 0);
//
//               operationNode = operationNode.nextNode();
//            }            
//
//            writer.removeBookmark();
//            //      SNode targetNode = node.parentNode().firstChild(lxObjectMask).firstChild(lxObjectMask);
//            //      if (targetNode == lxLocal) {
//            //         writer.insertChild(0, targetNode.type, targetNode.argument);
//            //      }
//            //      else writer.insertChild(0, lxCurrent, 0);
//            //
//            //      target = ObjectInfo(okObject);
//
//         }
//         else if (current == lxExpression) {
//            // HOTFIX : to correctly compile alternative operation with typecasting expression
//            writer.newNode(lxExpression);
//            compileExpression(writer, current, scope, 0, 0);
//            writer.closeNode();
//         }
//         else compileExpression(writer, current, scope, 0, 0);
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

ref_t Compiler :: resolveReferenceTemplate(Scope& scope, ref_t operandRef)
{
   if (!operandRef)
      operandRef = scope.moduleScope->superReference;

   List<SNode> parameters;

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxWriter dummyWriter(dummyTree);
   dummyWriter.newNode(lxRoot);
   dummyWriter.appendNode(lxTarget, operandRef);
   dummyWriter.closeNode();

   parameters.add(dummyTree.readRoot().firstChild());

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
   return scope.moduleScope->generateTemplate(*this, scope.moduleScope->refTemplateReference, parameters, nsScope->ns.c_str()/*, &nsScope->extensions*/);
}

ref_t Compiler :: resolvePrimitiveArray(Scope& scope, ref_t operandRef)
{
   if (!operandRef)
      operandRef = scope.moduleScope->superReference;

   // generate a reference class
   List<SNode> parameters;

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxWriter dummyWriter(dummyTree);
   dummyWriter.newNode(lxRoot);
   dummyWriter.appendNode(lxTarget, operandRef);
   dummyWriter.closeNode();

   parameters.add(dummyTree.readRoot().firstChild());

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
   return scope.moduleScope->generateTemplate(*this, scope.moduleScope->arrayTemplateReference, parameters, nsScope->ns.c_str()/*, &nsScope->extensions*/);
}

ObjectInfo Compiler :: compileReferenceExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   writer.newBookmark();

   ObjectInfo objectInfo = compileObject(writer, node, scope, 0, mode);
   ref_t operandRef = resolveObjectReference(scope, objectInfo);
   if (isPrimitiveRef(operandRef)) {
      operandRef = resolvePrimitiveReference(scope, operandRef, objectInfo.element);
   }
   else if (!operandRef)
      operandRef = scope.moduleScope->superReference;

   ref_t targetRef = 0;
   if (objectInfo.reference == V_WRAPPER) {
      // if the reference is passed further - do nothing
      targetRef = operandRef;
   }
   else {
      // generate an reference class
      targetRef = resolveReferenceTemplate(scope, operandRef);
      if (!convertObject(writer, scope, targetRef, objectInfo))
         scope.raiseError(errInvalidOperation, node);
   }
   
   writer.removeBookmark();
   return ObjectInfo(okObject, 0, targetRef);
}

ObjectInfo Compiler :: compileBoxingExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode)
{
   ref_t targetRef = 0;
   if (target.kind == okClass) {
      targetRef = target.param;
   }
   else scope.raiseError(errInvalidSyntax, node.parentNode());

   ObjectInfo retVal = ObjectInfo(okObject, 0, targetRef);

   int paramCount = SyntaxTree::countNodeMask(node, lxObjectMask);
   if (paramCount == 1 && node.argument == V_CONVERSION) {
      // if it is a cast expression
      ObjectInfo object = compileExpression(writer, node.nextNode(), scope, /*targetRef*/0, mode);
      if (!convertObject(writer, scope, targetRef, object))
         scope.raiseError(errIllegalOperation, node);
   }
   else if (node.argument == V_NEWOP) {
      // if it is a implicit constructor
      if (target.reference == V_OBJARRAY && paramCount == 1) {
         ObjectInfo roperand = compileExpression(writer, node.findNext(lxObjectMask), scope, 0, 0);

         int operationType = _logic->resolveNewOperationType(*scope.moduleScope, targetRef, resolveObjectReference(scope, roperand));
         if (operationType != 0) {
            // if it is a primitive operation
            _logic->injectNewOperation(writer, *scope.moduleScope, operationType, targetRef, target.element);

            retVal.reference = target.reference;
            retVal.element = target.element;
         }
         else scope.raiseError(errInvalidOperation, node);
      }
      else {
         int paramsMode = 0;
         int stackSafeAttr = 0;
         ref_t implicitSignatureRef = compileMessageParameters(writer, node, scope, paramsMode/* & (HINT_RESENDEXPR | HINT_PARAMETERSONLY)*/);

         ref_t messageRef = _logic->resolveImplicitConstructor(*scope.moduleScope, targetRef, implicitSignatureRef, paramCount, stackSafeAttr);
         if (messageRef) {
            // call the constructor if it can be resolved directly
            compileMessage(writer, node, scope, target, messageRef, HINT_SILENT | HINT_NODEBUGINFO, stackSafeAttr);
         }
         else scope.raiseError(errInvalidHint, node.parentNode());
      }
   }
   else scope.raiseError(errInvalidHint, node.parentNode());

//   SNode objectNode = node.findChild(lxExpression);
//   if (objectNode != lxNone) {
//      if (node.existChild(lxOperator)) {
//      }
//      else {
//         if (paramCount > 1) {
//            ref_t signRef = compileMessageParameters(writer, objectNode.firstChild(), scope);
//            if (!_logic->injectImplicitConstructor(writer, *scope.moduleScope, *this, targetRef, signRef))
//               scope.raiseError(errIllegalOperation, node);
//
//         }
//         else if (paramCount == 0) {
//            if (!_logic->injectImplicitCreation(writer, *scope.moduleScope, *this, targetRef))
//               scope.raiseError(errIllegalOperation, node);
//         }
//         else {
//            ObjectInfo object = compileExpression(writer, objectNode, scope, /*targetRef*/0, mode);
//            if (!convertObject(writer, scope, targetRef, resolveObjectReference(scope, object), object.element))
//               scope.raiseError(errIllegalOperation, node);
//
//            //if (!_logic->injectImplicitConversion(writer, *scope.moduleScope, *this, targetRef, resolveObjectReference(scope, object), 0))
//            //   scope.raiseError(errIllegalOperation, node);
//         }
//      }
//   }
//   else if (!_logic->injectImplicitCreation(writer, *scope.moduleScope, *this, targetRef))
//      scope.raiseError(errIllegalOperation, node);

   return retVal;
}

ObjectInfo Compiler :: compileOperation(SyntaxWriter& writer, SNode current, CodeScope& scope, ObjectInfo objectInfo/*, ref_t expectedRef*/, int mode)
{
   switch (current.type) {
      case lxSize:
         if (current.argument == -1 && current.nextNode() == lxTypecast && objectInfo.kind == okClass) {
            // COMPILER MAGIC : if it is a primitive array creation
            objectInfo.element = objectInfo.param;
            objectInfo.param = resolvePrimitiveArray(scope, objectInfo.element);
            objectInfo.reference = V_OBJARRAY;

            objectInfo = compileOperation(writer, current.nextNode(), scope, objectInfo, mode);
         }
         else scope.raiseError(errIllegalOperation, current);
         break;
      case lxMessage:
         if (test(mode, HINT_PROP_MODE)) {
            objectInfo = compilePropAssigning(writer, current, scope, objectInfo);
         }
         else objectInfo = compileMessage(writer, current, scope/*, expectedRef*/, objectInfo, mode);
         break;
      case lxTypecast:
         objectInfo = compileBoxingExpression(writer, current, scope, objectInfo, mode);
         break;
      case lxAssign:
         objectInfo = compileAssigning(writer, current, scope, objectInfo);
         break;
      case lxOperator:
         objectInfo = compileOperator(writer, current, scope, objectInfo, mode);
         break;
//      case lxExtension:
//         objectInfo = compileExtension(writer, current, scope, objectInfo);
//         break;
   }

   return objectInfo;
}

ref_t Compiler :: mapTemplateAttribute(SNode node, Scope& scope)
{
   IdentifierString templateName(node.firstChild(lxTerminalMask).identifier());
   int paramCounter = 0;
   SNode current = node.findChild(lxTarget);
   while (current != lxNone) {
      if (current == lxTarget) {
         paramCounter++;
      }
      else scope.raiseError(errInvalidOperation, node);

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(paramCounter);

   return resolveImplicitIdentifier(scope, templateName.c_str(), false, false);
}

ref_t Compiler :: mapTypeAttribute(SNode member, Scope& scope)
{
   ref_t ref = member.argument ? member.argument : resolveImplicitIdentifier(scope, member.firstChild(lxTerminalMask));
   if (!ref)
      scope.raiseError(errUnknownClass, member);

   return ref;
}

void Compiler :: compileTemplateAttributes(SNode current, List<SNode>& parameters, Scope& scope)
{
   if (current == lxIdentifier)
      current = current.nextNode();

   ExpressionAttributes attributes;
   while (current != lxNone) {
      if (current == lxTarget) {
         current.setArgument(mapTypeAttribute(current, scope));

         parameters.add(current);
      }
      else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);

      current = current.nextNode();
   }
}

ref_t Compiler :: resolveTemplateDeclaration(SNode node, Scope& scope)
{
   // generate an reference class
   List<SNode> parameters;
   compileTemplateAttributes(node.firstChild(), parameters, scope);
   
   ref_t templateRef = mapTemplateAttribute(node, scope);
   if (!templateRef)
      scope.raiseError(errInvalidHint, node);

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::slNamespace);
   return scope.moduleScope->generateTemplate(*this, templateRef, parameters, ns->ns.c_str()/*, &nsScope->extensions*/);
}

ref_t Compiler :: compileExpressionAttributes(SyntaxWriter& writer, SNode& current, CodeScope& scope, int mode)
{
   ref_t exprAttr = 0;

   bool  invalidExpr = false;
   bool  newVariable = false;
   bool  dynamicSize = false;
   ref_t typeRef = 0;
   
   // NOTE : root attributes (i.e. loop) are handled in compileRootExpression
   while (current == lxAttribute) {
      ExpressionAttributes attributes;

      int value = current.argument;
      if (!_logic->validateExpressionAttribute(value, attributes))
         scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);

      if (attributes.templateAttr && test(mode, HINT_ROOT)) {
         if (typeRef == 0) {
            typeRef = resolveTemplateDeclaration(current, scope);
         }
         else scope.raiseError(errIllegalOperation, current);

         newVariable = true;
      }      

      if (attributes.isExprAttr() && !newVariable && !typeRef) {
         if (attributes.forwardAttr) {
            exprAttr |= HINT_FORWARD;
         }
         if (attributes.refAttr) {
            exprAttr |= HINT_REFOP;
         }
         if (attributes.externAttr) {
            exprAttr |= HINT_EXTERNALOP;
         }
         if (attributes.internAttr) {
            exprAttr |= HINT_INTERNALOP;
         }
      }

      if (attributes.typeAttr && test(mode, HINT_ROOT) && !attributes.castAttr) {
         // if it is a variable declaration
         newVariable = true;

         if (value == V_AUTO)
            typeRef = value;
      }
      if (attributes.castAttr || attributes.newOpAttr) {
         SNode msgNode = goToNode(current, lxMessage);
         if (msgNode != lxNone && msgNode.firstChild() == lxNone) {
            if (attributes.castAttr) {
               exprAttr |= HINT_VIRTUALEXPR;
               msgNode.set(lxTypecast, V_CONVERSION);
            }
            else msgNode.set(lxTypecast, V_NEWOP);
         }
         else invalidExpr = true;
      }

      if (attributes.loopAttr)
         scope.raiseError(errIllegalOperation, current);

      current = current.nextNode();
   }

   if (current == lxTarget) {
      if (typeRef == 0) {
         typeRef = mapTypeAttribute(current, scope);
         newVariable = true;
         if (current.existChild(lxSize)) {
            dynamicSize = true;
         }
      }
      else scope.raiseError(errIllegalOperation, current);

      current = current.nextNode();
   }

   //            // negative value defines the target virtual class
   //            if (variable.extraparam == 0) {
   //               variable.extraparam = value;
   //            }
   //            //else if (value == V_OBJARRAY) {
   //            //   variable.element = variable.extraparam;
   //            //   variable.extraparam = value;
   //            //}
   //            else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);

   if (invalidExpr) {
      scope.raiseError(errInvalidSyntax, current.parentNode());
   }

   if (newVariable) {
      if (!typeRef)
         typeRef = scope.moduleScope->superReference;

      compileVariable(writer, current, scope, typeRef, dynamicSize);
   }

   return exprAttr;
}

inline SNode findLeftMostNode(SNode current, LexicalType type)
{
   if (current == lxExpression) {
      return findLeftMostNode(current.firstChild(), type);
   }
   else return current;
}

ObjectInfo Compiler :: compileRootExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   int rootMode = HINT_ROOT;

   // COMPILER MAGIC : recognize root attributes
   SNode current = findLeftMostNode(node.firstChild(), lxAttribute);
   while (current == lxAttribute) {
      ExpressionAttributes attributes;
      if (_logic->validateExpressionAttribute(current.argument, attributes)) {
         if (attributes.loopAttr) {
            rootMode |= HINT_LOOP;

            current.setArgument(0);
         }
      }
      current = current.nextNode();
   }

   return compileExpression(writer, node, scope, 0, rootMode);
}

ObjectInfo Compiler :: compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t exptectedRef, int mode)
{
   ObjectInfo objectInfo;

   writer.newBookmark();

   bool noPrimMode = test(mode, HINT_NOPRIMITIVES);
//   bool assignMode = test(mode, HINT_ASSIGNING_EXPR);

   mode &= ~(HINT_NOPRIMITIVES | HINT_ASSIGNING_EXPR);

   int targetMode = mode & ~(HINT_PROP_MODE | HINT_LOOP);

   SNode current = node.firstChild();
   // COMPILER MAGIC : compile the expression attributes
   if (current.compare(lxAttribute, lxTarget)) {
      targetMode |= compileExpressionAttributes(writer, current, scope, mode);
   }
      
   SNode operationNode = current.nextNode();
   if (operationNode == lxAssign) {
      // recognize the property set operation
      targetMode |= HINT_PROP_MODE;
      if (isSingleStatement(current))
         targetMode |= (HINT_NOBOXING/* | HINT_ASSIGNTARGET*/);

      mode |= HINT_NOUNBOXING;
   }
   else if (operationNode == lxNone) {
      targetMode |= mode;
   }

//   if (object == lxMethodParameter || object == lxNone) {
//      objectInfo = compileObject(writer, node, scope, 0, targetMode);
//   }
//   else {
//      if (operationNode == lxNone && object.nextNode() == lxExpression) {
//         targetMode |= HINT_COLLECTION_MODE;
//      }
//
//      if (operationNode != lxNone) {
         objectInfo = compileObject(writer, current, scope, 0, targetMode);         
         if (operationNode != lxNone) {
            operationNode.refresh(); // HOTFIX : to reflect changes after the property compilation
            objectInfo = compileOperation(writer, operationNode, scope, objectInfo/*, exptectedRef*/, mode);
         }            
//
//         // HOTFIX : to compile property assigmment properly - reread them
//         operationNode = node.findChild(lxAssign, lxMessage, lxOperator, lxExtension);
//
//         objectInfo = compileOperation(writer, operationNode, scope, objectInfo, exptectedRef, mode);
//      }
//      // if the object is single, pass the target reference
//      else objectInfo = compileObject(writer, object, scope, exptectedRef, targetMode);
//   }   

   ref_t sourceRef = resolveObjectReference(scope, objectInfo/*, exptectedRef*/);
   if (!exptectedRef && isPrimitiveRef(sourceRef) && noPrimMode) {
      // resolve primitive object if required
      exptectedRef = resolvePrimitiveReference(scope, sourceRef, objectInfo.element);
   }

   if (exptectedRef) {
//      if (assignMode && exptectedRef == scope.moduleScope->realReference && (sourceRef == V_INT32 || sourceRef == scope.moduleScope->intReference)) {
//         objectInfo = ObjectInfo(okPrimitiveConv, V_REAL64, V_INT32);
//      }
      /*else */if (convertObject(writer, scope, exptectedRef, objectInfo)) {
         objectInfo = ObjectInfo(okObject, 0, exptectedRef);
      }
      else scope.raiseError(errInvalidOperation, node);
   }

   writer.removeBookmark();

   return objectInfo;
}

ObjectInfo Compiler :: compileBranching(SyntaxWriter& writer, SNode thenCode, CodeScope& scope)
{
   CodeScope subScope(&scope);

   writer.newNode(lxCode);

   compileCode(writer, thenCode, subScope);

   // preserve the allocated space
   scope.level = subScope.level;

   writer.closeNode();

   return ObjectInfo(okObject);
}

//void Compiler :: compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   // find inner expression
//   SNode expr = node;
//   while (expr.findChild(lxMessage, lxAssign, lxOperator) == lxNone) {
//      expr = expr.findChild(lxExpression);
//   }
//
//   compileExpression(writer, expr, scope, 0, HINT_LOOP);
//}

ObjectInfo Compiler :: compileCodeExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode)
{
   ObjectInfo retVal;

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxExpression:
            writer.newNode(lxExpression);
            writer.appendNode(lxBreakpoint, dsStep);
            retVal = compileRootExpression(writer, current, scope);
            writer.closeNode();
            break;
         case lxReturning:
         {
            writer.newNode(lxReturning);
            writer.appendNode(lxBreakpoint, dsStep);
            retVal = compileRetExpression(writer, current, scope, HINT_ROOT);
            writer.closeNode();

            break;
         }
      }

      current = current.nextNode();
   }

   return retVal;
}

ObjectInfo Compiler :: compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   scope.rootBookmark = writer.newBookmark();

   while (current != lxNone) {
      switch(current) {
         case lxExpression:
            writer.newNode(lxExpression);
            writer.appendNode(lxBreakpoint, dsStep);
            compileRootExpression(writer, current, scope);
            writer.closeNode();
            break;
//         case lxLoop:
//            writer.newNode(lxExpression);
//            writer.appendNode(lxBreakpoint, dsStep);
//            compileLoop(writer, current, scope);
//            writer.closeNode();
//            break;
         case lxReturning:
         {
            needVirtualEnd = false;

            //if (test(scope.getMessageID(), SPECIAL_MESSAGE))
            //   scope.raiseError(errIllegalOperation, current);

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

   writer.removeBookmark();

   return retVal;
}

void Compiler :: compileExternalArguments(SNode node, NamespaceScope& nsScope/*, WarningScope& warningScope*/)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExtArgument) {
         analizeExpressionTree(current, nsScope, /*warningScope, */HINT_NOBOXING);

         ref_t classReference = current.findChild(lxExtArgumentRef).argument;
         ClassInfo classInfo;
         if (classReference) {
            ref_t primitiveRef = classReference;
            bool variableOne = false;
            if (!isPrimitiveRef(classReference)) {               
               _logic->defineClassInfo(*nsScope.moduleScope, classInfo, classReference);

               variableOne = _logic->isVariable(classInfo);
               primitiveRef = _logic->retrievePrimitiveReference(*nsScope.moduleScope, classInfo);
            }

            switch (primitiveRef) {
               case V_INT32:
               //case V_PTR:
               case V_DWORD:
               //case V_SIGNATURE:
               //case V_MESSAGE:
                  current.set(variableOne ? lxExtArgument : lxIntExtArgument, 0);
                  break;
               //case V_INT8ARRAY:
               //   current.set(lxExtArgument, 0);
               //   break;
               //case V_SYMBOL:
               //{
               //   current.set(lxExtInteranlRef, 0);
               //   // HOTFIX : ignore call operation
               //   SNode callNode = current.findChild(lxCalling);
               //   callNode.set(lxExpression, 0);
               //   break;
               //}
               default:
                  if (test(classInfo.header.flags, elStructureRole)) {
                     // HOTFIX : to allow pass system'Handle value directly
                     if (!variableOne && classInfo.size == 4) {
                        current.set(lxIntExtArgument, 0);
                     }
                     else current.set(lxExtArgument, 0);
                  }
                  //else if (test(classInfo.header.flags, elWrapper)) {
                  //   //HOTFIX : allow to pass a normal object
                  //   current.set(lxExtArgument, 0);
                  //}
                  else nsScope.raiseError(errInvalidOperation, current);
                  break;
            }
         }
         else nsScope.raiseError(errInvalidOperation, current);
      }

      current = current.nextNode();
   }
}

ObjectInfo Compiler :: compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope/*, ref_t expectedRef*/, int mode)
{
   ObjectInfo retVal(okExternal);

   _ModuleScope* moduleScope = scope.moduleScope;

//   bool rootMode = test(mode, HINT_ROOT);
   bool stdCall = false;
   bool apiCall = false;

   SNode targetNode = node;
   // HOTFIX : comment out dll reference
   //targetNode = lxIdle;

   //SNode messageNode = node.findChild(lxMessage);
   writer.appendNode(lxBreakpoint, dsAtomicStep);

   ident_t dllAlias = targetNode.identifier();
   ident_t functionName = node.nextNode().firstChild(lxTerminalMask).identifier();

   ident_t dllName = NULL;
   if (dllAlias.compare(RT_MODULE)) {
      // if run time dll is used
      dllName = RTDLL_FORWARD;

      if (functionName.compare(COREAPI_MASK, COREAPI_MASK_LEN))
         apiCall = true;
   }
   else dllName = moduleScope->project->resolveExternalAlias(dllAlias, stdCall);

   // legacy : if dll is not mapped, use the name directly
   if (emptystr(dllName))
      dllName = dllAlias;

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

   if (!test(mode, HINT_ROOT)) {
      //if (expectedRef == scope.moduleScope->realReference) {
      //   retVal = assignResult(writer, scope, V_REAL64);
      //}
      //else if (expectedRef == V_REAL64 || expectedRef == V_INT64) {
      //   retVal = assignResult(writer, scope, expectedRef);
      //}
      /*else */retVal = assignResult(writer, scope, V_INT32);
   }   

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ref_t signature, ObjectInfo routine)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   int paramCount;
   ref_t actionRef, flags;
   ref_t dummy = 0;
   decodeMessage(message, actionRef, paramCount, flags);

   size_t signIndex = virtualReference.Length();
   virtualReference.append('0' + (char)paramCount);
   virtualReference.append(moduleScope->module->resolveAction(actionRef, dummy));

   ref_t signatures[ARG_COUNT];
   size_t len = scope.module->resolveSignature(signature, signatures);
   for (size_t i = 0; i < len; i++) {
      if (isPrimitiveRef(signatures[i])) {
         // !!
         scope.raiseError(errIllegalOperation, node);
      }
      else {
         virtualReference.append("$");
         ident_t name = scope.module->resolveReference(signatures[i]);
         if (isTemplateWeakReference(name)) {
            NamespaceName ns(name);

            virtualReference.append(name + getlength(ns));
         }
         else if (isWeakReference(name)) {
            virtualReference.append(scope.module->Name());
            virtualReference.append(name);
         }
         else virtualReference.append(name);
      }
   }

   virtualReference.replaceAll('\'', '@', signIndex);

   writer.insert(lxInternalCall, moduleScope->module->mapReference(virtualReference));
   writer.closeNode();

//   SNode targetNode = node.firstChild(lxTerminalMask);
//   // HOTFIX : comment out dll reference
//   targetNode = lxIdle;

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

bool Compiler :: allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand)
{
   if (size <= 0)
      return false;

   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
   if (methodScope == NULL)
      return false;

   int offset = allocateStructure(binaryArray, size, scope.reserved);

   // if it is not enough place to allocate
   if (methodScope->reserved < scope.reserved) {
      methodScope->reserved += size;
   }

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
}

ref_t Compiler :: declareInlineArgumentList(SNode arg, MethodScope& scope, ref_t& outputRef)
{
//   IdentifierString signature;
   IdentifierString messageStr;

   ref_t actionRef = 0;
   ref_t signRef = 0;

//   SNode sign = goToNode(arg, lxTypeAttr);
   ref_t signatures[ARG_COUNT];
   size_t signatureLen = 0;
//   bool first = true;
   bool weakSingature = true;
   while (arg == lxMethodParameter/* || arg == lxIdentifier || arg == lxPrivate*/) {
      SNode terminalNode = arg;
      if (terminalNode == lxMethodParameter) {
         terminalNode = terminalNode.firstChild(lxTerminalMask);
      }

      ident_t terminal = terminalNode.identifier();
      int index = 1 + scope.parameters.Count();

      // !! check duplicates
      if (scope.parameters.exist(terminal))
         scope.raiseError(errDuplicatedLocal, arg);

      ref_t elementRef = 0;
      ref_t classRef = 0;
      declareArgumentAttributes(arg, scope, classRef, elementRef);

      int size = classRef != 0 ? _logic->defineStructSize(*scope.moduleScope, classRef, 0) : 0;
      scope.parameters.add(terminal, Parameter(index, classRef, elementRef, size));

      if (classRef != 0)
         weakSingature = false;

      if (isPrimitiveRef(classRef)) {
         // primitive arguments should be replaced with wrapper classes
         signatures[signatureLen++] = resolvePrimitiveReference(scope, classRef, elementRef);
      }
      else signatures[signatureLen++] = classRef;

      arg = arg.nextNode();
   }
   //if (sign == lxTypeAttr) {
   //   // if the returning type is provided
   //   ref_t elementRef = 0;
   //   outputRef = declareArgumentType(sign, scope, /*first, messageStr, signature, */elementRef);

   //   if (sign.nextNode() == lxTypeAttr)
   //      scope.raiseError(errInvalidOperation, arg);
   //}

   if (emptystr(messageStr)) {
      messageStr.copy(INVOKE_MESSAGE);
   }
   //messageStr.append(signature);

   if (signatureLen > 0 && !weakSingature) {
      if (scope.parameters.Count() == signatureLen) {
         signRef = scope.module->mapSignature(signatures, signatureLen, false);
      }
      else scope.raiseError(errInvalidOperation, arg);
   }

   actionRef = scope.moduleScope->module->mapAction(messageStr, signRef, false);

   return encodeMessage(actionRef, scope.parameters.Count(), SPECIAL_MESSAGE);
}

inline SNode findTerminal(SNode node)
{
   SNode ident = node.findChild(lxIdentifier/*, lxPrivate*/);
   if (ident == lxNone)
      //HOTFIX : if the tree is generated by the script engine
      ident = node;

   return ident;
}

void Compiler :: declareArgumentAttributes(SNode node, Scope& scope, ref_t& classRef, ref_t& elementRef)
{
   bool byRefArg = false;
   bool arrayArg = false;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (_logic->validateArgumentAttribute(current.argument, byRefArg)) {

         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }      
      else if (current == lxTarget) {
         classRef = current.argument ? current.argument : resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
         if (!classRef)
            scope.raiseError(errUnknownClass, current);
      }
      else if (current == lxSize) {
         arrayArg = true;
      }

      current = current.nextNode();
   }

   if (byRefArg) {
      elementRef = classRef;
      classRef = V_WRAPPER;
   }
   if (arrayArg) {
      elementRef = classRef;
      classRef = _logic->definePrimitiveArray(*scope.moduleScope, elementRef);
   }
}

void Compiler :: declareArgumentList(SNode node, MethodScope& scope)
{
   IdentifierString actionStr;
   ref_t actionRef = 0;

   ref_t signature[ARG_COUNT];
   size_t signatureLen = 0;

//   bool constantConversion = false;
   bool unnamedMessage = false;
   ref_t flags = 0;

   SNode nameNode = node.findChild(lxNameAttr);
   SNode identNode = nameNode.firstChild(lxTerminalMask);
//   if (action == lxNone)
//      action = node.findChild(lxMessage);

   SNode current = /*action == lxNone ? */node.findChild(lxMethodParameter)/* : action.nextNode()*/;

   if (identNode/*.compare(*/ == lxIdentifier/*, lxMessage)*/) {
      actionStr.copy(identNode.identifier());
      // COMPILER MAGIC : adding complex message name
      SNode messageNode = nameNode.nextNode();
      while (messageNode == lxMessage) {
         actionStr.append(':');
         actionStr.append(messageNode.firstChild(lxTerminalMask).identifier());

         messageNode = messageNode.nextNode();
      }
   }
   else unnamedMessage = true;
      
   int paramCount = 0;
   // if method has an argument list
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         int index = 1 + scope.parameters.Count();
         int size = 0;
         ref_t classRef = scope.moduleScope->superReference;
         ref_t elementRef = 0;

         ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask)/*findTerminal(current.findChild(lxNameAttr))*/.identifier();
         if (scope.parameters.exist(terminal))
            scope.raiseError(errDuplicatedLocal, current);

         declareArgumentAttributes(current, scope, classRef, elementRef);

//            if (current.argument != 0) {
//               // HOTFIX : to recognize conversion arguments
//               classRef = current.argument;
//            }
//            else classRef = declareArgumentType(attribute, scope, /*first, messageStr, signature, */elementRef);
//            if (classRef == V_ARGARRAY) {
//               if (!test(scope.hints, tpGeneric) && !scope.withOpenArg)
//                  // !! temporal : only generic method may handle open argument list
//                  scope.raiseError(errNotApplicable, node);
//
//               // the generic arguments should be free by the method exit
//               scope.withOpenArg = true;
//
//               // to indicate open argument list
//               paramCount += OPEN_ARG_COUNT;
//               if (paramCount > MAX_ARG_COUNT) {
//                  scope.raiseError(errNotApplicable, node);
//               }
//
//               signature[signatureLen++] = elementRef;
//            }
//            else {
               paramCount++;
               if (paramCount >= ARG_COUNT)
                  scope.raiseError(errTooManyParameters, current);

               if (isPrimitiveRef(classRef)) {
                  // primitive arguments should be replaced with wrapper classes
                  signature[signatureLen++] = resolvePrimitiveReference(scope, classRef, elementRef);
               }
               else signature[signatureLen++] = classRef;

               size = _logic->defineStructSize(*scope.moduleScope, classRef, elementRef);

//            }

         scope.parameters.add(terminal, Parameter(index, classRef, elementRef, size));

      }
      else if (current == lxMessage) {
         actionStr.append(":");
         actionStr.append(current.findChild(lxIdentifier).identifier());
      }
      else break;

      current = current.nextNode();
   }

   if (signatureLen > 0/* && !scope.withOpenArg*/) {
      // validate generic signature (except an open argument one)
      bool weakSignature = true;
      for (size_t i = 0; i < signatureLen; i++) {
         if (signature[i] != scope.moduleScope->superReference) {
            weakSignature = false;
         }
      }
      // if the signature consists only of generic parameters - ignore it
      if (weakSignature)
         signatureLen = 0;
   }

   // HOTFIX : do not overrwrite the message on the second pass
   if (scope.message == 0) {
//      if (test(scope.hints, tpSealed | tpSpecial))
//         flags |= SPECIAL_MESSAGE;
//
      if ((scope.hints & tpMask) == tpDispatcher) {
         if (paramCount == 0 && unnamedMessage) {
            actionRef = getAction(scope.moduleScope->dispatch_message);
            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if ((scope.hints & tpMask) == tpSealed && test(scope.hints, tpConversion)) {
         if (paramCount == 0 && unnamedMessage && scope.outputRef) {
            ref_t signatureRef = scope.moduleScope->module->mapSignature(&scope.outputRef, 1, false);
            actionRef = scope.moduleScope->module->mapAction(CAST_MESSAGE, signatureRef, false);

            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (test(scope.hints, tpConstructor) && unnamedMessage) {
         actionStr.copy(CONSTRUCTOR_MESSAGE);
//         if (paramCount > 0) {
//            //HOTFIX : replace constructor attribute with conversion one
//            SNode attrNode = goToNode(node.firstChild(), lxAttribute, tpConstructor);
//            attrNode.setArgument(tpSealed | tpConversion);
//            scope.hints &= ~tpConstructor;
//            scope.hints |= tpSealed | tpConversion;
//
//            actionStr.copy(CAST_MESSAGE);
//            flags |= SPECIAL_MESSAGE;
//         }
//         else actionStr.copy(DEFAULT_MESSAGE);
         unnamedMessage = false;
      }
//      else if (test(scope.hints, tpSealed | tpGeneric) && paramCount < OPEN_ARG_COUNT) {
//         if (signatureLen > 0 || !unnamedMessage)
//            scope.raiseError(errInvalidHint, action);
//
//         unnamedMessage = false;
//         actionStr.copy(GENERIC_PREFIX);
//      }
      else if (test(scope.hints, tpAction)) {
         if (!unnamedMessage)
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(INVOKE_MESSAGE);

         flags |= SPECIAL_MESSAGE;
//         // Compiler Magic : if it is a generic closure - ignore fixed argument
//         if (test(scope.hints, tpGeneric) && paramCount > OPEN_ARG_COUNT) {
//            if (validateGenericClosure(signature, signatureLen)) {
//               signatureLen = 1;
//               paramCount = OPEN_ARG_COUNT;
//               scope.genericClosure = true;               
//            }
//            // generic clsoure should have a homogeneous signature (i.e. same types)
//            else scope.raiseError(errIllegalMethod, node);
//         }
      }

//      if (test(scope.hints, tpInternal)) {
//         actionStr.insert("$$", 0);
//         actionStr.insert(scope.module->Name(), 0);
//      }

      if (testany(scope.hints, tpGetAccessor | tpSetAccessor)) {
         if ((paramCount == 0 && test(scope.hints, tpGetAccessor)) || (paramCount == 1 && test(scope.hints, tpSetAccessor))) {
            flags |= PROPERTY_MESSAGE;
         }
         else scope.raiseError(errIllegalMethod, node);
      }

//      //if (test(scope.hints, tpSealed | tpConversion)) {
//      else if (test(scope.hints, tpSealed | tpConversion)) {
//         SNode typeNode = node.findChild(lxClassRefAttr);
//         if (typeNode != lxNone) {
//            if (signatureLen == 0) {
//               signature[0] = scope.moduleScope->mapFullReference(typeNode.identifier(), true);
//               signatureLen++;
//               actionStr.copy(CAST_MESSAGE);
//            }
//            else scope.raiseError(errIllegalMethod, node);
//         }
//         else if (signatureLen == 1 && signature[0] == scope.moduleScope->literalReference) {
//            flags |= SPECIAL_MESSAGE;
//            constantConversion = true;
//         }
//         else scope.raiseError(errIllegalMethod, node);
//      }
//      else if (test(scope.hints, tpPrivate)) {
//         flags |= SEALED_MESSAGE;
//      }
//      else if (unnamedMessage && emptystr(actionStr))
//         actionStr.append(EVAL_MESSAGE);

      if (actionRef != 0) {
         // HOTFIX : if the action was already resolved - do nothing
      }
      else if (actionStr.Length() > 0) {
         ref_t signatureRef = 0;
         if (signatureLen > 0)
            signatureRef = scope.moduleScope->module->mapSignature(signature, signatureLen, false);

         actionRef = scope.moduleScope->module->mapAction(actionStr.c_str(), signatureRef, false);
//         // COMPILER MAGIC : recognize set property
//         if (actionRef == SET_MESSAGE_ID && paramCount == 1) {
//            flags |= PROPSET_MESSAGE;
//         }
      }
      else scope.raiseError(errIllegalMethod, node);

      scope.message = encodeMessage(actionRef, paramCount, flags);

//      // if it is an explicit constant conversion
//      if (constantConversion) {
//         NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
//
//         saveExtension(*nsScope, scope.getClassRef(), 0, scope.message, false);
//      }
   }

//   if (scope.genericClosure && paramCount > OPEN_ARG_COUNT) {
//      // Compiler Magic : if it is a generic closure - ignore fixed argument but it should be removed from the stack
//      scope.rootToFree += (paramCount - OPEN_ARG_COUNT);
//   }
}

//bool Compiler :: verifyGenericArgParamCount(ClassScope& scope, int expectedParamCount)
//{
//   for (auto it = scope.info.methods.start(); !it.Eof(); it++) {
//      if (isOpenArg(it.key()) && _logic->isMethodGeneric(scope.info, it.key()) && getAbsoluteParamCount(it.key()) == expectedParamCount) {
//         return true;
//      }
//   }
//
//   return false;
//}

void Compiler :: compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope/*, bool withGenericMethods, bool withOpenArgGenerics*/)
{
   writer.newNode(lxClassMethod, scope.message);

//   CodeScope codeScope(&scope);

   if (isImportRedirect(node)) {
      importCode(writer, node, scope, node.findChild(lxReference).identifier(), scope.message);
   }
   else {
      throw InternalError("not yet supported");

//      writer.newNode(lxDispatching);
//
//      // if it is generic handler with redirect statement / redirect statement
//      if (node != lxNone && node.firstChild(lxObjectMask) != lxNone) {
//         // !! temporally
//         if (withOpenArgGenerics)
//            scope.raiseError(errInvalidOperation, node);
//
//         if (withGenericMethods) {
//            writer.appendNode(lxDispatching, encodeMessage(codeScope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 0));
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
//         writer.appendNode(lxMessage, encodeMessage(codeScope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 0));
//
//         writer.newNode(lxTarget, scope.moduleScope->superReference);
//         writer.appendNode(lxMessage, encodeAction(DISPATCH_MESSAGE_ID));
//         writer.closeNode();
//
//         writer.closeNode();
//      }
//      // if it is open arg generic without redirect statement
//      else if (withOpenArgGenerics) {
//         writer.newNode(lxResending);
//
//         for (int paramCount = MAX_ARG_COUNT; paramCount >= OPEN_ARG_COUNT; paramCount--) {
//            if (verifyGenericArgParamCount(*(ClassScope*)scope.parent, paramCount)) {
//               writer.appendNode(lxMessage, encodeMessage(DISPATCH_MESSAGE_ID, paramCount));
//            }
//         }
//
//         writer.newNode(lxTarget, scope.moduleScope->superReference);
//         writer.appendNode(lxMessage, encodeAction(DISPATCH_MESSAGE_ID));
//         writer.closeNode();
//
//         writer.closeNode();
//      }
//
//      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileActionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   declareParameterDebugInfo(writer, node, scope, false/*, false*/);

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

   writer.appendNode(lxParamCount, scope.parameters.Count()); // NOTE : the message target is not included into the stack for closure!!
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - 1);  // allocate the space for the local variables excluding "this" one

   writer.closeNode();
}

//void Compiler :: compileLazyExpressionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   declareParameterDebugInfo(writer, node, scope, false/*, false*/);
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
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      // try to implement light-weight resend operation
      ObjectInfo target;
      if (isSingleStatement(node)) {
         SNode terminal = node.firstChild(lxTerminalMask);

         target = scope.mapTerminal(terminal.identifier(), terminal == lxReference, 0);
      }

      if (target.kind == okConstantSymbol || target.kind == okField || target.kind == okReadOnlyField) {
         writer.newNode(lxResending, methodScope->message);
         writer.newNode(lxExpression);
         if (target.kind == okField || target.kind == okReadOnlyField) {
            writer.appendNode(lxResultField, target.param);
         }
         else writer.appendNode(lxConstantSymbol, target.param);

         writer.closeNode();
         writer.closeNode();
      }
      else {
         writer.newNode(lxResending, methodScope->message);
         writer.newNode(lxNewFrame);

         target = compileExpression(writer, node, scope, 0, 0);

         writer.closeNode();
         writer.closeNode();
      }
   }
}

void Compiler :: compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame)
{
   SNode expr = node/*.findChild(lxExpression)*/;

   _ModuleScope* moduleScope = scope.moduleScope;
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   ref_t messageRef = 0;
   bool implicitConstructor = false;
   SNode messageNode = expr.findChild(lxMessage);
   if (messageNode.firstChild(lxTerminalMask) == lxNone) {
      // HOTFIX : support implicit constructors
      messageRef = encodeMessage(scope.module->mapAction(CONSTRUCTOR_MESSAGE, 0, false), 
         SyntaxTree::countNodeMask(messageNode, lxObjectMask), STATIC_MESSAGE);

      implicitConstructor = true;
   }
   else messageRef = mapMessage(messageNode, scope);

   ref_t classRef = classClassScope.reference;
   bool found = false;

   if ((getParamCount(messageRef) != 0 && methodScope->parameters.Count() != 0) || node.existChild(lxCode) || !isConstantArguments(expr)) {
      withFrame = true;

      // new stack frame
      // stack already contains $self value
      writer.newNode(lxNewFrame);
      scope.level++;
   }
   else writer.newNode(lxExpression);

   writer.newBookmark();

   if (withFrame) {
      writer.appendNode(lxSelfLocal, 1);
   }
   else writer.appendNode(lxResult);

   ResendScope resendScope(&scope);
   resendScope.consructionMode = true;
   resendScope.withFrame = withFrame;

   ref_t implicitSignatureRef = compileMessageParameters(writer, expr.findChild(lxMessage).nextNode(), resendScope);

   ObjectInfo target(okClassSelf, scope.getClassRefId(), classRef);
   int stackSafeAttr = 0;
   if (implicitConstructor) {
      messageRef = _logic->resolveImplicitConstructor(*scope.moduleScope, target.param, implicitSignatureRef, getParamCount(messageRef), stackSafeAttr);
   }
   else messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, false, stackSafeAttr);

   // find where the target constructor is declared in the current class
   // but it is not equal to the current method
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);
   int hints = methodScope->message != messageRef ? checkMethod(*moduleScope, classScope->info.header.classRef, messageRef) : tpUnknown;
   if (hints != tpUnknown) {
      found = true;
   }
   // otherwise search in the parent class constructors
   else {
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
      writer.appendNode(lxCallTarget, classRef);

      compileMessage(writer, expr, resendScope, target, messageRef, /*HINT_RESENDEXPR*/0, stackSafeAttr);

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

//void Compiler :: compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope)
//{
//   if (isImportRedirect(node)) {
//      importCode(writer, node, scope, node.findChild(lxReference).identifier(), scope.getMessageID());
//   }
//   else {
//      SNode dispatchMssg = node.parentNode().findChild(lxEmbeddableMssg);
//
//      if (node.argument == -1 && dispatchMssg != lxNone) {
//         writer.appendNode(lxCalling, -1);
//         writer.newNode(lxImplicitJump, dispatchMssg.argument);
//         writer.appendNode(lxTarget, scope.getClassRefId());
//         writer.closeNode();
//      }
//      else scope.raiseError(errInvalidOperation, node);
//   }   
//}

void Compiler :: compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope)
{
   //if (node != lxClass && node.existChild(lxArgDispatcherAttr)) {
   //   // if it is a argument list unboxing routine
   //   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   //   writer.newNode(lxNewFrame);

   //   // new stack frame
   //   // stack already contains current $self reference
   //   scope.level++;

   //   writer.newNode(lxCalling, node.argument);
   //   writeTerminal(writer, node, scope, methodScope->mapSelf(), HINT_NODEBUGINFO);
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
   //      ObjectInfo target = methodScope->mapSelf(true);

   //      writer.newNode(lxOverridden);
   //      writeParamTerminal(writer, scope, target, HINT_DYNAMIC_OBJECT, lxSelfLocal);
   //      writeTarget(writer, resolveObjectReference(scope, target), target.element);
   //      writer.closeNode();
   //      writer.closeNode();
   //   }

   //   writer.closeNode();

   //   //scope.freeSpace();

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
         //ref_t openArgMessage = encodeMessage(getAction(message), getParamCount(message) + OPEN_ARG_COUNT - 1) | (message & MESSAGE_FLAG_MASK);
         //if (classScope.info.methods.exist(openArgMessage)) {
         //   writer.newNode(lxResending);

         //   writer.appendNode(lxMessage, encodeMessage(DISPATCH_MESSAGE_ID, getAbsoluteParamCount(openArgMessage)));
         //   writer.appendNode(lxOvreriddenMessage, message);

         //   writer.newNode(lxTarget, scope.moduleScope->superReference);
         //   writer.appendNode(lxMessage, encodeAction(DISPATCH_MESSAGE_ID));
         //   writer.closeNode();

         //   writer.closeNode();
         //}
         //else {
            writer.newNode(lxDispatching, node.argument);
            SyntaxTree::copyNode(writer, lxTarget, node);
            writer.closeNode();
         //}
      }
      writer.closeNode();
   //}
}

void Compiler :: compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod)
{
   if (node.argument != 0 && multiMethod) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

      compileMultidispatch(writer, node, scope, *classScope);
   }
   else {
      if (multiMethod) {
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

         compileMultidispatch(writer, node.parentNode(), scope, *classScope);
      }

      writer.newNode(lxNewFrame);

      // new stack frame
      // stack already contains current $self reference
      scope.level++;

      writer.newNode(lxExpression);
      writer.newBookmark();

      ObjectInfo target = scope.mapMember(SELF_VAR);
      writeTerminal(writer, node, scope, target, HINT_NODEBUGINFO);
      compileMessage(writer, node.firstChild(lxObjectMask).findChild(lxMessage), scope, /*0, */target, 0);

      writer.removeBookmark();
      writer.closeNode();

      writer.closeNode();
   }
}

void Compiler :: compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   if (scope.closureMode) {
      scope.rootToFree -= 1;
   }

   declareParameterDebugInfo(writer, node, scope, true/*, test(scope.getClassFlags(), elRole)*/);

   int paramCount = getParamCount(scope.message);
   int preallocated = 0;

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   // check if it is a resend
   if (body == lxResendExpression) {
      compileResendExpression(writer, body, codeScope, scope.multiMethod);
      preallocated = 1;
   }
   // check if it is a dispatch
   else if (body == lxDispatchCode) {
      compileDispatchExpression(writer, body, codeScope);
   }
   else {
      if (scope.multiMethod) {
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

         compileMultidispatch(writer, node.parentNode(), codeScope, *classScope);
      }

      writer.newNode(lxNewFrame/*, scope.generic ? -1 : 0*/);

      // new stack frame
      // stack already contains current self reference
      // the original message should be restored if it is a generic method
      codeScope.level++;
      //// declare the current subject for a generic method
      //if (scope.generic) {
      //   codeScope.level++;
      //   codeScope.mapLocal(SUBJECT_VAR, codeScope.level, V_MESSAGE, 0);
      //}

      preallocated = codeScope.level;

      ObjectInfo retVal = compileCode(writer, body == lxReturning ? node : body, codeScope);

      // if the method returns itself
      if(retVal.kind == okUnknown) {
         ObjectInfo thisParam = scope.mapSelf();

         // adding the code loading self
         writer.newNode(lxReturning);
         writer.newBookmark();
         writeTerminal(writer, node, codeScope, thisParam, HINT_NODEBUGINFO | HINT_NOBOXING) ;

         ref_t resultRef = scope.getReturningRef(false);
         if (resultRef != 0) {
            if (!convertObject(writer, codeScope, resultRef, thisParam/*, 0*/))
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

void Compiler ::compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   SNode body = node.findChild(lxCode);
   // abstract method should have an empty body
   if (body != lxNone) {
      if (body.firstChild() != lxEOF)
         scope.raiseError(errAbstractMethodCode, node);
   }
   else scope.raiseError(errAbstractMethodCode, node);

   writer.appendNode(lxNil);

   writer.closeNode();
}

//void Compiler :: compileImplicitConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope)
//{
//   writer.newNode(lxClassMethod, scope.message);
//
//   declareParameterDebugInfo(writer, node, scope, true/*, test(scope.getClassFlags(), elRole)*/);
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
//   writer.newNode(lxNewFrame, scope.generic ? -1 : 0);
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

//   SNode attrNode = node.findChild(lxEmbeddableMssg);
//   if (attrNode != lxNone) {
//      writer.appendNode(attrNode.type, attrNode.argument);
//   }

   declareParameterDebugInfo(writer, node, scope, true/*, false*/);

   CodeScope codeScope(&scope);

   bool retExpr = false;
   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();
   int preallocated = 0;

   SNode bodyNode = node.findChild(lxResendExpression, lxCode, lxReturning/*, lxDispatchCode*/);
//   if (bodyNode == lxDispatchCode) {
//      compileConstructorDispatchExpression(writer, bodyNode, codeScope);
//
//      writer.closeNode();
//      return;
//   }
   /*else */if (bodyNode == lxResendExpression) {
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
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(scope.moduleScope->newobject_message)) {
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
         writer.newNode(lxReturning);
         //writer.appendNode(lxBreakpoint, dsStep);
         compileExpression(writer, bodyNode, codeScope, codeScope.getClassRefId(), HINT_DYNAMIC_OBJECT);
         writer.closeNode();
      }
      else {
         preallocated = codeScope.level;

         compileCode(writer, bodyNode, codeScope);

         // HOT FIX : returning the created object
         writer.newNode(lxExpression);
         writer.appendNode(lxLocal, 1);
         writer.closeNode();
      }
   }

   if (withFrame)
      writer.closeNode();

   writer.appendNode(lxParamCount, getParamCount(scope.message) + 1);
   writer.appendNode(lxReserved, scope.reserved);
   writer.appendNode(lxAllocated, codeScope.level - preallocated);  // allocate the space for the local variables excluding preallocated ones ("$this", "$message")

   writer.closeNode();
}

//void Compiler :: compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, ref_t message)
//{
//   if (classScope.info.methods.exist(message)) {
//      if (classScope.info.methods.exist(message, true)) {
//         // call the field in-place initialization
//         writer.newNode(lxCalling, message);
//         writer.appendNode(lxTarget, classScope.reference);
//         writer.closeNode();
//      }
//      else {
//         ref_t parentRef = classScope.info.header.parentRef;
//         while (parentRef != 0) {
//            // call the parent field in-place initialization
//            ClassInfo parentInfo;
//            _logic->defineClassInfo(*classScope.moduleScope, parentInfo, parentRef);
//
//            if (parentInfo.methods.exist(message, true)) {
//               writer.newNode(lxCalling, message);
//               writer.appendNode(lxTarget, parentRef);
//               writer.closeNode();
//
//               break;
//            }
//
//            parentRef = parentInfo.header.parentRef;
//         }
//      }
//   }
//}

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

   //// call field initilizers if available
   //compileSpecialMethodCall(writer, *classScope, encodeAction(INIT_MESSAGE_ID) | SPECIAL_MESSAGE);

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

//            if (current.argument == (encodeAction(DEFAULT_MESSAGE_ID) | SPECIAL_MESSAGE)) {
//               scope.withImplicitConstructor = true;
//            }

            initialize(scope, methodScope);

            // if it is a dispatch handler
            if (methodScope.message == scope.moduleScope->dispatch_message) {
               //if (test(scope.info.header.flags, elRole))
               //   scope.raiseError(errInvalidRoleDeclr, member.Terminal());

               compileDispatcher(writer, current.findChild(lxDispatchCode), methodScope/*,
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithArgGenerics)*/);
            }
            // if it is a normal method
            else {
               declareArgumentList(current, methodScope);
//
//               if (methodScope.message == (encodeAction(INIT_MESSAGE_ID) | SPECIAL_MESSAGE)) {
//                  // if it is in-place class member initialization
//                  compileImplicitConstructor(writer, current, methodScope);
//               }
               /*else */if (methodScope.abstractMethod) {
                  compileAbstractMethod(writer, current, methodScope);
               }
               else compileMethod(writer, current, methodScope);
            }
            break;
         }
      }

      current = current.nextNode();
   }

//   // if the VMT conatains newly defined generic handlers, overrides default one
//   if (testany(scope.info.header.flags, elWithGenerics | elWithArgGenerics) && scope.info.methods.exist(encodeAction(DISPATCH_MESSAGE_ID), false)) {
//      MethodScope methodScope(&scope);
//      methodScope.message = encodeAction(DISPATCH_MESSAGE_ID);
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
   if (classClassScope.info.methods.exist(classScope.moduleScope->newobject_message, true)) {
      MethodScope methodScope(&classScope);
      methodScope.message = classScope.moduleScope->newobject_message;

      if (test(classScope.info.header.flags, elDynamicRole)) {
         //compileDynamicDefaultConstructor(writer, methodScope);
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

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope);

            compileConstructor(writer, current, methodScope, classClassScope);
            break;
         }
         case lxStaticMethod:
         {
            //if (!staticFieldsInherited) {
            //   // HOTFIX : inherit static fields
            //   classClassScope.copyStaticFields(classScope.info.statics, classScope.info.staticValues);

            //   staticFieldsInherited = true;
            //}

            MethodScope methodScope(&classClassScope);
            methodScope.message = current.argument;

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope);

            compileMethod(writer, current, methodScope);
            break;
         }
      }

      current = current.nextNode();
   }

//   // if the VMT conatains newly defined generic handlers, overrides default one
//   if (testany(classClassScope.info.header.flags, elWithGenerics | elWithArgGenerics) && classClassScope.info.methods.exist(encodeAction(DISPATCH_MESSAGE_ID), false)) {
//      MethodScope methodScope(&classClassScope);
//      methodScope.message = encodeAction(DISPATCH_MESSAGE_ID);
//
//      classClassScope.include(methodScope.message);
//
//      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);
//
//      compileDispatcher(writer, SNode(), methodScope,
//         test(classClassScope.info.header.flags, elWithGenerics),
//         test(classClassScope.info.header.flags, elWithArgGenerics));
//
//      // overwrite the class info
//      classClassScope.save();
//   }
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
         ref_t elementRef = 0;
         bool isStatic = false;
         bool isSealed = false;
         bool isConst = false;
         bool isEmbeddable = false;
         int sizeHint = 0;
         declareFieldAttributes(current, scope, fieldRef/*, elementRef*/, sizeHint, isStatic, isSealed, isConst, isEmbeddable);

         if (isStatic) {
            generateClassStaticField(scope, current, fieldRef/*, elementRef*/, isSealed, isConst);
         }
         else if (isSealed || isConst) {
            scope.raiseError(errIllegalField, current);
         }
         else generateClassField(scope, current, fieldRef, elementRef, sizeHint, singleField, isEmbeddable);
      }
      //else if (current == lxFieldInit) {
      //   // HOTFIX : reallocate static constant
      //   SNode nameNode = current.findChild(lxMemberIdentifier);
      //   ObjectInfo info = scope.mapField(nameNode.identifier().c_str() + 1, 0);
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

//   ref_t implicitConstructor = encodeMessage(NEWOBJECT_MESSAGE_ID, 0) | CONVERSION_MESSAGE;

   SyntaxTree tree;
   SyntaxWriter writer(tree);
   /*if (scope.info.methods.exist(implicitConstructor, true)) {
      generateSymbolWithInitialization(writer, scope, implicitConstructor);
   }
   else*/ generateClassSymbol(writer, scope);

   _writer.generateSymbol(tape, tree.readRoot(), false, INVALID_REF);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compilePreloadedCode(SymbolScope& scope)
{
   _Module* module = scope.moduleScope->module;

   IdentifierString sectionName("'", INITIALIZER_SECTION);

   CommandTape tape;
   _writer.generateInitializer(tape, module->mapReference(sectionName), lxSymbolReference, scope.reference);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
}

//void Compiler :: compilePreloadedCode(_CompilerScope& scope, SNode node)
//{
//   _Module* module = scope.module;
//
//   IdentifierString sectionName("'", INITIALIZER_SECTION);
//
//   CommandTape tape;
//   _writer.generateInitializer(tape, module->mapReference(sectionName), node);
//
//   // create byte code sections
//   _writer.save(tape, scope);
//}

void Compiler :: compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   if (classScope.info.header.parentRef == 0) {
      //   classScope.raiseError(errNoConstructorDefined, node.findChild(lxIdentifier, lxPrivate));
      classScope.info.header.parentRef = classScope.moduleScope->superReference;
   }
   else {
      // the constructors aren't inherited for abstract or dynamic classes
      if (!classScope.abstractMode && !test(classScope.info.header.flags, elDynamicRole)) {
         IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.info.header.parentRef));
         classClassParentName.append(CLASSCLASS_POSTFIX);

         classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
      }
      else {
         classClassScope.info.header.parentRef = classScope.moduleScope->superReference;
         classClassScope.abstractMode = true;
      }
   }

   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef/*, true*/);

   //// !! hotfix : remove closed
   //classClassScope.info.header.flags &= ~elClosed;

   generateClassDeclaration(node, classClassScope, 
      _logic->isEmbeddable(classScope.info) ? ClassType::ctEmbeddableClassClass : ClassType::ctClassClass);

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

   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   //// HOTFIX : due to current implementation the default constructor can be declared as a special method and a constructor;
   ////          only one is allowed
   //if (classScope.withImplicitConstructor && classClassScope.info.methods.exist(encodeAction(DEFAULT_MESSAGE_ID)))
   //   classScope.raiseError(errOneDefaultConstructor, node.findChild(lxNameAttr));

   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

   writer.newNode(lxClass, classClassScope.reference);
   compileClassVMT(writer, node, classClassScope, classScope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), classClassScope);
}

void Compiler :: initialize(ClassScope& scope, MethodScope& methodScope)
{
   methodScope.hints = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
   methodScope.outputRef = scope.info.methodHints.get(ClassInfo::Attribute(methodScope.message, maReference));
   if (test(methodScope.hints, tpInitializer))
      methodScope.scopeMode |= INITIALIZER_SCOPE;

//   methodScope.dispatchMode = _logic->isDispatcher(scope.info, methodScope.message);
   methodScope.classEmbeddable = _logic->isEmbeddable(scope.info);
//   methodScope.withOpenArg = isOpenArg(methodScope.message);
   methodScope.closureMode = _logic->isClosure(scope.info, methodScope.message);
   methodScope.multiMethod = _logic->isMultiMethod(scope.info, methodScope.message);
   methodScope.abstractMethod = _logic->isMethodAbstract(scope.info, methodScope.message);
   methodScope.extensionMode = scope.extensionClassRef != 0;

//   if (!methodScope.withOpenArg) {
//      // HOTFIX : generic with open argument list is compiled differently
//      methodScope.generic = _logic->isMethodGeneric(scope.info, methodScope.message);
//   }
//   else if (_logic->isMethodGeneric(scope.info, methodScope.message) && methodScope.closureMode)
//      methodScope.genericClosure = true;
}

void Compiler :: declareVMT(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         MethodScope methodScope(&scope);

         declareMethodAttributes(current, methodScope);

         if (current.argument == 0) {
            declareArgumentList(current, methodScope);
            current.setArgument(methodScope.message);
         }
         else methodScope.message = current.argument;

         if (test(methodScope.hints, tpConstructor)) {
            if (_logic->isAbstract(scope.info)) {
               // abstract class cannot have constructors
               scope.raiseError(errIllegalMethod, current);
            }
//            else if (methodScope.message == encodeAction(DEFAULT_MESSAGE_ID) && !current.existChild(lxReturning)) {
//               // if it is a special default constructor
//               current.setArgument(methodScope.message | SPECIAL_MESSAGE);
//            }
            else current = lxConstructor;
         }
         else if (test(methodScope.hints, tpPredefined)) {
            // recognize predefined message signatures
            predefineMethod(current, scope, methodScope);

            current = lxIdle;
         }
         else if (test(methodScope.hints, tpStatic))
            current = lxStaticMethod;

         if (!_logic->validateMessage(*methodScope.moduleScope, methodScope.message, false))
            scope.raiseError(errIllegalMethod, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: generateClassFlags(ClassScope& scope, SNode root)
{
//   ref_t extensionTypeRef = 0;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         scope.info.header.flags |= current.argument;
//         if (test(current.argument, elExtension)) {
//            SNode argRef = current.findChild(lxClassRefAttr, lxAttribute);
//            if (argRef == lxClassRefAttr) {
//               extensionTypeRef = scope.moduleScope->mapFullReference(argRef.identifier(), true);
//            }
//            else if (argRef == lxAttribute) {
//               if (argRef.argument == V_ARGARRAY) {
//                  // HOTFIX : recognize open argument extension
//                  extensionTypeRef = V_ARGARRAY;
//               }
//               else scope.raiseError(errInvalidHint, root);
//            }
//         }         
      }
//      else if (current == lxTarget) {
//         extensionTypeRef = current.argument;
//      }

      current = current.nextNode();
   }

   // check if extension is qualified
   bool extensionMode = test(scope.info.header.flags, elExtension);
   if (extensionMode) {
//      if (extensionTypeRef == 0)
//         extensionTypeRef = scope.moduleScope->superReference;
//   }
//         scope.extensionClassRef = extensionTypeRef;

      scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(scope.extensionClassRef, 0));
   }
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxTree::Node current, ref_t classRef, ref_t elementRef, int sizeHint, bool singleField, bool embeddable)
{
   if (singleField && sizeHint == -1) {
      scope.info.header.flags |= elDynamicRole;
      sizeHint = 0;
   }

   int flags = scope.info.header.flags;
   int offset = 0;
   ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();

   // a role cannot have fields
   if (test(flags, elStateless))
      scope.raiseError(errIllegalField, current);

   int size = (classRef != 0) ? _logic->defineStructSize(*scope.moduleScope, classRef, elementRef) : 0;
   //bool fieldArray = false;
   if (sizeHint != 0) {
      if (isPrimitiveRef(classRef) && (size == sizeHint || (classRef == V_INT32 && sizeHint <= size))) {
         // for primitive types size should be specified
         size = sizeHint;
      }
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
      else scope.raiseError(errIllegalField, current);
   }

   if (test(flags, elWrapper) && scope.info.fields.Count() > 0) {
      // wrapper may have only one field
      scope.raiseError(errIllegalField, current);
   }
   // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
   else if (embeddable/* && !fieldArray*/) {
      if (!singleField || scope.info.fields.Count() > 0)
         scope.raiseError(errIllegalField, current);

      // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
      if (test(scope.info.header.flags, elSealed)) {
         scope.info.header.flags |= elWrapper;
         if (size > 0)
            scope.info.header.flags |= elStructureRole;
      }         
   }

   // a class with a dynamic length structure must have no fields
   if (test(scope.info.header.flags, elDynamicRole)) {
      if (scope.info.size == 0 && scope.info.fields.Count() == 0) {
         // compiler magic : turn a field declaration into an array or string one
         if (size != 0 && !test(scope.info.header.flags, elNonStructureRole)) {
            scope.info.header.flags |= elStructureRole;
            scope.info.size = -size;
         }

         ref_t arrayRef = _logic->definePrimitiveArray(*scope.moduleScope, classRef);

         scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(arrayRef, classRef));
         scope.info.fields.add(terminal, -2);
      }
      else scope.raiseError(errIllegalField, current);
   }
   else {
      if (scope.info.fields.exist(terminal)) {
         //if (current.argument == INVALID_REF) {
         //   //HOTFIX : ignore duplicate autogenerated fields
         //   return;
         //}
         /*else */scope.raiseError(errDuplicatedField, current);
      }

      // if it is a structure field
      if (test(scope.info.header.flags, elStructureRole)) {
         if (size <= 0)
            scope.raiseError(errIllegalField, current);

         if (scope.info.size != 0 && scope.info.fields.Count() == 0)
            scope.raiseError(errIllegalField, current);

         offset = scope.info.size;
         scope.info.size += size;

         scope.info.fields.add(terminal, offset);
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, /*elementRef*/0));

         if (isPrimitiveRef(classRef))
            _logic->tweakPrimitiveClassFlags(classRef, scope.info);
      }
      // if it is a normal field
      else {
         //// primitive / virtual classes cannot be declared
         //if (size != 0 && isPrimitiveRef(classRef))
         //   scope.raiseError(errIllegalField, current);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.Count();
         scope.info.fields.add(terminal, offset);

         if (classRef != 0)
            scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, /*typeRef*/0));
      }
   }
}

void Compiler :: generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef/*, ref_t elementRef*/, bool isSealed, bool isConst)
{
   _Module* module = scope.module;

   ident_t terminal = current.firstChild(lxTerminalMask).identifier();

   if (scope.info.statics.exist(terminal)) {
      if (current.argument == INVALID_REF) {
         //HOTFIX : ignore duplicate autogenerated fields
         return;
      }
      else scope.raiseError(errDuplicatedField, current);
   }

   if (isSealed) {
      // generate static reference
      IdentifierString name(module->resolveReference(scope.reference));
      name.append(STATICFIELD_POSTFIX);

      ref_t ref = scope.moduleScope->mapAnonymous(name.c_str());
         module->mapReference(name);

      scope.info.statics.add(terminal, ClassInfo::FieldInfo(ref, fieldRef));
      if (isConst) {
         // HOTFIX : add read-only attribute (!= mskStatRef)
         scope.info.staticValues.add(ref, mskConstantRef);
      }
   }
   else {
      int index = ++scope.info.header.staticSize;
      index = -index - 4;

      scope.info.statics.add(terminal, ClassInfo::FieldInfo(index, fieldRef));

      if (isConst) {
         ReferenceNs name(module->resolveReference(scope.reference));
         name.append(STATICFIELD_POSTFIX);
         name.append("##");
         name.appendInt(-index);

         scope.info.staticValues.add(index, module->mapReference(name) | mskConstArray);
      }
      else scope.info.staticValues.add(index, (ref_t)mskStatRef);
   }
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, ref_t message, bool allowTypeAttribute)
{
   ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));
   bool hintChanged = false, outputChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));

   //// HOTFIX : multimethod attribute should not be inherited
   //if (test(hint, tpMultimethod)) {
   //   hint &= ~tpMultimethod;
   //   hintChanged = true;
   //}

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (test(current.argument, tpAbstract)) {
            if (!_logic->isAbstract(scope.info))
               // only abstract class may have an abstract methods
               scope.raiseError(errNotAbstractClass, current);

            if (scope.info.methods.exist(message))
               // abstract method should be newly declared
               scope.raiseError(errNoMethodOverload, current);
         }

         hint |= current.argument;

         hintChanged = true;
      }
      else if (current == lxTarget) {
         if (!allowTypeAttribute) {
            scope.raiseError(errTypeNotAllowed, node);
         }
         else if (current.argument != 0) {
            outputRef = current.argument;

            outputChanged = true;
         }
         else if (outputRef == 0) {
            outputRef = resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
            if (!outputRef)
               scope.raiseError(errUnknownClass, current);

            outputChanged = true;
         }
         else if (outputRef != resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask))) {
            scope.raiseError(errTypeAlreadyDeclared, node);
         }
         else outputChanged = true;
      }
//      else if (current == lxClassMethodOpt) {
//         SNode mssgAttr = SyntaxTree::findChild(current, lxMessage);
//         if (mssgAttr != lxNone) {
//            scope.info.methodHints.add(Attribute(message, current.argument), getSignature(mssgAttr.argument));
//         }
//      }
      current = current.nextNode();
   }

   //if ((message & MESSAGE_FLAG_MASK) == SEALED_MESSAGE) {
   //   // if it is private message set private hint and save it as public one
   //   hintChanged = true;
   //   hint |= tpPrivate;

   //   scope.info.methodHints.add(Attribute(message & ~SEALED_MESSAGE, maHint), hint);
   //}
   //else if (test(message, SPECIAL_MESSAGE) && message == (encodeAction(DEFAULT_MESSAGE_ID) | SPECIAL_MESSAGE)) {
   //   hint |= tpSpecial;
   //   hint |= tpSealed;
   //}
   //else if (test(hint, tpInternal)) {
   //   // if it is an internal message save internal hint as a public general one
   //   // so it could be later recognized
   //   ref_t signRef = 0;
   //   ident_t name = scope.module->resolveAction(getAction(message), signRef);
   //   int index = name.find("$$");
   //   ref_t publicMessage = overwriteAction(message, scope.module->mapAction(name + index + 2, 0, false));
   //   if (scope.info.methods.exist(publicMessage)) {
   //      // there should be no public method with the same name
   //      scope.raiseError(errDupPublicMethod, node.findChild(lxIdentifier));
   //   }
   //   else {
   //      scope.info.methodHints.exclude(Attribute(publicMessage, maHint));
   //      scope.info.methodHints.add(Attribute(publicMessage, maHint), tpInternal);
   //   }
   //}

   if (hintChanged) {
      //if (test(hint, tpSealed | tpGeneric | tpAction)) {
      //   // HOTFIX : generic closure cannot be sealed
      //   hint &= ~tpSealed;
      //}
      scope.info.methodHints.exclude(Attribute(message, maHint));
      scope.info.methodHints.add(Attribute(message, maHint), hint);
   }
   if (outputChanged) {
      scope.info.methodHints.add(Attribute(message, maReference), outputRef);
   }
   else if (outputRef != 0 && !node.existChild(lxAutogenerated) && !test(hint, tpConstructor))
      //warn if the method output was not redclared, ignore auto generated methods
      //!!hotfix : ignore the warning for the constructor
      scope.raiseWarning(WARNING_LEVEL_1, wrnTypeInherited, node);
}

void Compiler :: saveExtension(NamespaceScope& nsScope, ref_t reference, ref_t extensionClassRef, ref_t message, bool internalOne)
{
   nsScope.saveExtension(message, extensionClassRef, reference, internalOne);
   //if (isOpenArg(message)/* && _logic->isMethodGeneric(scope.info, message)*/) {
   //   // if it is an extension with open argument list generic handler
   //   // creates the references for all possible number of parameters
   //   for (int i = 1; i < OPEN_ARG_COUNT; i++) {
   //      nsScope.saveExtension(overwriteParamCount(message, i), extensionClassRef, reference, internalOne);
   //   }
   //}
}

void Compiler :: saveExtension(ClassScope& scope, ref_t message, bool internalOne)
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

   saveExtension(*nsScope, scope.reference, scope.extensionClassRef, message, internalOne);
}

inline bool isGeneralMessage(_Module* module, ref_t message)
{
   if (getParamCount(message) == 0) {
      return true;
   }
   else {
      ref_t signRef = 0;
      module->resolveAction(getAction(message), signRef);

      return signRef == 0;
   }
}

void Compiler :: predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope)
{
   SNode body = node.findChild(lxCode);
   if (body != lxCode || body.firstChild() != lxEOF)
      scope.raiseError(errPedefineMethodCode, node);

   if (testany(scope.hints, tpAbstract/* | tpPrivate*/))
      // abstract or private methods cannot be predefined
      scope.raiseError(errIllegalMethod, node);

   if (scope.extensionMode)
      // an extension cannot have predefined methods
      scope.raiseError(errIllegalMethod, node);

   ref_t signRef;
   scope.module->resolveAction(scope.message, signRef);
   if (signRef)
      // a predefine method should not contain a strong typed signature
      scope.raiseError(errIllegalMethod, node);

   generateMethodAttributes(classScope, node, scope.message, true);

}

void Compiler :: generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed, bool allowTypeAttribute, bool embeddableClass)
{
   ref_t message = current.argument;

   if (scope.info.methods.exist(message, true) && hideDuplicates) {
      // ignoring autogenerated duplicates
      current = lxIdle;

      return;
   }

   generateMethodAttributes(scope, current, message, allowTypeAttribute);

   int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(message, maHint));
//   if (isOpenArg(message)) {
//      if (_logic->isMethodGeneric(scope.info, message)) {
//         // HOTFIX : verify that only generics with similar argument signature available
//         //int extraParamCount = retrieveGenericArgParamCount(scope);
//         //if (extraParamCount != -1 && getParamCount(message) != extraParamCount)
//         //   scope.raiseError(errIllegalMethod, current);
//
//         scope.info.header.flags |= elWithArgGenerics;
//      }
//   }
//   else if (_logic->isMethodGeneric(scope.info, message)) {
//      scope.info.header.flags |= elWithGenerics;
//   }

   // check if there is no duplicate method
   if (scope.info.methods.exist(message, true)) {
      scope.raiseError(errDuplicatedMethod, current);
   }
   else {
//      bool privateOne = test(message, SEALED_MESSAGE);
//      bool specialOne = test(methodHints, tpConversion);
//      if (test(message, SPECIAL_MESSAGE)) {
//         // initialize method can be overridden
//         specialOne = true;
//      }

      bool included = scope.include(message);
      bool sealedMethod = (methodHints & tpMask) == tpSealed;
      // if the class is closed, no new methods can be declared
      // except private sealed ones (which are declared outside the class VMT)
      if (included && closed/* && !privateOne*/) {
         scope.raiseError(errClosedParent, findParent(current, lxClass/*, lxNestedClass*/));
      }

      // if the method is sealed, it cannot be overridden
      if (!included && sealedMethod/* && !specialOne*/) {
         scope.raiseError(errClosedMethod, findParent(current, lxClass/*, lxNestedClass*/));
      }

//      // HOTFIX : make sure there are no duplicity between public and private ones
//      if (privateOne) {
//         if (scope.info.methods.exist(message & ~SEALED_MESSAGE))
//            scope.raiseError(errDupPublicMethod, current.findChild(lxIdentifier));
//      }
//      else  if (scope.info.methods.exist(message | SEALED_MESSAGE))
//         scope.raiseError(errDuplicatedMethod, current);
//
      if (embeddableClass && !test(methodHints, tpMultimethod)) {
         // add a stacksafe attribute for the embeddable structure automatically, except multi-methods

         methodHints |= tpStackSafe;

         scope.info.methodHints.exclude(Attribute(message, maHint));
         scope.info.methodHints.add(Attribute(message, maHint), methodHints);

         ////HOTFIX : for the private message : update the virtual method as well
         //if ((message & MESSAGE_FLAG_MASK) == SEALED_MESSAGE) {
         //   scope.info.methodHints.exclude(Attribute(message & ~SEALED_MESSAGE, maHint));
         //   scope.info.methodHints.add(Attribute(message & ~SEALED_MESSAGE, maHint), methodHints);
         //}
      }

      if (!included && test(methodHints, tpAbstract)) {
         scope.removeHint(message, tpAbstract);
      }

      if (test(methodHints, tpPredefined)) {
         // exclude the predefined attribute from declared method
         scope.removeHint(message, tpPredefined);
      }

//      if (test(scope.info.header.flags, elExtension) && (test(methodHints, tpPrivate) || test(methodHints, tpInternal)))
//         // private / internal methods cannot be declared in the extension
//         scope.raiseError(errIllegalPrivate, current);

      //// create overloadlist if required
      //if (test(methodHints, tpMultimethod)) {
      //   NamespaceScope* namespaceScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

      //   scope.info.methodHints.exclude(Attribute(message, maOverloadlist));
      //   scope.info.methodHints.add(Attribute(message, maOverloadlist), namespaceScope->mapAnonymous(resolveActionName(scope.module, message)));

      //   scope.info.header.flags |= elWithMuti;

      //   //// save extensions if required ; private method should be ignored
      //   //if (test(scope.info.header.flags, elExtension) && !test(methodHints, tpPrivate)) {
      //   //   // NOTE : only general message should be saved
      //   //   saveExtension(scope, message, scope.internalOne);
      //   //}
      //}
      /*else */if (test(scope.info.header.flags, elExtension)/* && !test(methodHints, tpPrivate)*/ && isGeneralMessage(scope.module, message)) {
         // NOTE : only general message should be saved
         saveExtension(scope, message, scope.internalOne);
      }
   }
}

ref_t Compiler :: resolveMultimethod(ClassScope& scope, ref_t messageRef)
{
   int paramCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(messageRef, actionRef, paramCount, flags);

   // HOTFIX : do not resolve multi-methods for private methods / implicit constructors
   if (paramCount == 0 || test(flags, STATIC_MESSAGE))
      return 0;

   ident_t actionStr = scope.module->resolveAction(actionRef, signRef);
   if (actionStr[0] == '#' && actionStr.compare(CONSTRUCTOR_MESSAGE))
      return 0;
   
   if (signRef) {
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);
      ref_t genericMessage = encodeMessage(genericActionRef, paramCount, flags);

      return genericMessage;
   }

   return 0;
}

void Compiler :: generateMethodDeclarations(SNode root, ClassScope& scope, bool closed, LexicalType methodType, bool allowTypeAttribute, bool embeddableClass)
{
   bool templateMethods = false;
   List<ref_t> implicitMultimethods;

   // first pass - mark all multi-methods
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == methodType && !test(current.argument, STATIC_MESSAGE)) {
         //HOTFIX : ignore private methods
         ref_t multiMethod = resolveMultimethod(scope, current.argument);
         if (multiMethod) {
            //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
            Attribute attr(current.argument, maMultimethod);
            scope.info.methodHints.exclude(attr);
            scope.info.methodHints.add(attr, multiMethod);

            if (retrieveIndex(implicitMultimethods.start(), multiMethod) == -1) {
               implicitMultimethods.add(multiMethod);
               templateMethods = true;

               // HOTFIX : mark the generic message as a multi-method
               scope.addHint(multiMethod, tpMultimethod);
            }
         }
      }
      current = current.nextNode();
   }

   // second pass - ignore template based / autogenerated methods
   current = root.firstChild();
   while (current != lxNone) {
      if (current == methodType) {
         if (!current.existChild(/*lxTemplate, */lxAutogenerated)) {
            generateMethodDeclaration(current, scope, false, closed, allowTypeAttribute, embeddableClass);
         }
         else templateMethods = true;
      }
      current = current.nextNode();
   }

   //COMPILER MAGIC : if strong signature is declared - the compiler should contain the virtual multi method
   if (implicitMultimethods.Count() > 0) {
      _logic->injectVirtualMultimethods(*scope.moduleScope, root, scope.info, *this, implicitMultimethods, methodType);
   }

   if (templateMethods) {
      // third pass - do not include overwritten template-based methods
      current = root.firstChild();
      while (current != lxNone) {
         if (current.existChild(/*lxTemplate, */lxAutogenerated) && (current == methodType)) {
            generateMethodDeclaration(current, scope, true, closed, allowTypeAttribute, embeddableClass);
         }
         current = current.nextNode();
      }
   }

   if (implicitMultimethods.Count() > 0)
      _logic->verifyMultimethods(*scope.moduleScope, root, scope.info, implicitMultimethods);
}

inline bool isClassClass(Compiler::ClassType classType)
{
   return test(classType, Compiler::ClassType::ctClassClass);
}

inline bool isEmbeddable(Compiler::ClassType classType)
{
   return test(classType, Compiler::ClassType::ctEmbeddable);
}

void Compiler :: generateClassDeclaration(SNode node, ClassScope& scope, ClassType classType, bool nestedDeclarationMode)
{
   bool closed = test(scope.info.header.flags, elClosed);

   if (isClassClass(classType)) {
      if (!scope.abstractMode && _logic->isDefaultConstructorEnabled(scope.info)) {
         scope.include(scope.moduleScope->newobject_message);
      }
   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, node);

//      if (test(scope.info.header.flags, elExtension)) {
//         scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;
//      }
//
//      // inject virtual fields
//      _logic->injectVirtualFields(*scope.moduleScope, node, scope.reference, scope.info, *this);

      // generate fields
      generateClassFields(node, scope, countFields(node) == 1);

      if (scope.extensionClassRef != 0 ? _logic->isEmbeddable(*scope.moduleScope, scope.extensionClassRef) : _logic->isEmbeddable(scope.info))
         classType = ClassType::ctEmbeddableClass;
   }

   _logic->injectVirtualCode(*scope.moduleScope, node, scope.reference, scope.info, *this, closed);

   // generate methods
   if (isClassClass(classType)) {
      generateMethodDeclarations(node, scope, closed, lxConstructor, false, isEmbeddable(classType));
      generateMethodDeclarations(node, scope, closed, lxStaticMethod, true, isEmbeddable(classType));
   }
   else generateMethodDeclarations(node, scope, closed, lxClassMethod, true, isEmbeddable(classType));

   bool withAbstractMethods = false;
   bool disptacherNotAllowed = false;
   bool emptyStructure = false;
   _logic->validateClassDeclaration(scope.info, withAbstractMethods, disptacherNotAllowed, emptyStructure);
   if (withAbstractMethods) {
      scope.raiseError(errAbstractMethods, node);
   }      
   if (disptacherNotAllowed)
      scope.raiseError(errDispatcherInInterface, node);
   if (emptyStructure)
      scope.raiseError(errEmptyStructure, node.findChild(lxNameAttr));

   // do not set flags for closure declaration - they will be set later
   if (!nestedDeclarationMode) {
      _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, isClassClass(classType));
   }
   ///*else */if (test(scope.info.header.flags, elNestedClass)) {
   //   // HOTFIX : nested class should be marked as sealed to generate multi-method properly
   //   scope.info.header.flags |= elSealed;
   //}
}

void Compiler :: declareMethodAttributes(SNode node, MethodScope& scope)
{
   SNode current = node.firstChild();
   bool explicitMode = false;
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateMethodAttribute(value, explicitMode)) {
            scope.hints |= value;

            current.setArgument(value);
         }
         else {
            current = lxIdle;

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current == lxTarget) {
         // if it is a type attribute
         scope.outputRef = current.argument ? current.argument : resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
         if (!scope.outputRef)
            scope.raiseError(errUnknownClass, current);
      }
      else if (current == lxNameAttr && !explicitMode) {
         // resolving implicit method attributes
         int attr = scope.moduleScope->attributes.get(current.firstChild(lxTerminalMask).identifier());
         if (_logic->validateImplicitMethodAttribute(attr)) {
            scope.hints |= attr;
            current.set(lxAttribute, attr);
         }
      }

      current = current.nextNode();
   }
}

//inline SNode findBaseParent(SNode node)
//{
//   SNode baseNode = node.findChild(lxBaseParent);
//   //if (baseNode != lxNone) {
//   //   if (baseNode.argument == -1 && existChildWithArg(node, lxBaseParent, 0u)) {
//   //      // HOTFIX : allow to override the template parent
//   //      baseNode = lxIdle;
//   //      baseNode = node.findChild(lxBaseParent);
//   //   }
//   //}
//
//   return baseNode;
//}

ref_t Compiler :: resolveParentRef(SNode node, Scope& scope, bool silentMode)
{
   ref_t parentRef = 0;
   if (node.existChild(lxTarget)) {
      // if it is a template based class
      parentRef = resolveTemplateDeclaration(node, scope);
   }
   else parentRef = resolveImplicitIdentifier(scope, node.firstChild(lxTerminalMask));

   //if (isWeakReference(baseClassName) && !isTemplateWeakReference(baseClassName)) {
   //   parentRef = scope.module->mapReference(baseClassName, true);
   //}
   //else parentRef = scope.moduleScope->mapFullReference(baseClassName, true);

   if (parentRef == 0 && !silentMode)
      scope.raiseError(errUnknownClass, node);

   return parentRef;
}

//bool Compiler :: isDependentOnNotDeclaredClass(SNode baseNode, Scope& scope)
//{
//   if (baseNode == lxNone)
//      return false;
//
//   ref_t parentRef = resolveParentRef(baseNode, scope, false);
//   if (parentRef == 0)
//      return true;
//
//   // get module reference
//   ref_t moduleRef = 0;
//   _Module* module = scope.moduleScope->loadReferenceModule(scope.module->resolveReference(parentRef), moduleRef);
//
//   if (module == NULL || moduleRef == 0)
//      return true;
//
//   // load parent meta data
//   return module->mapSection(moduleRef | mskMetaRDataRef, true) == NULL;
//}

void Compiler :: compileClassDeclaration(SNode node, ClassScope& scope)
{
   bool extensionDeclaration = isExtensionDeclaration(node);   
   compileParentDeclaration(node.findChild(lxParent), scope, extensionDeclaration);

   declareClassAttributes(node, scope);

   declareVMT(node, scope);

   ClassType type = ClassType::ctUndefinedClass;
   generateClassDeclaration(node, scope, type);

   if (_logic->isRole(scope.info)) {
      // class is its own class class
      scope.info.header.classRef = scope.reference;
   }
   else {
      // define class class name
      IdentifierString classClassName(scope.moduleScope->resolveFullName(scope.reference));
      classClassName.append(CLASSCLASS_POSTFIX);

      scope.info.header.classRef = scope.moduleScope->module->mapReference(classClassName);
   }

   // if it is a super class validate it
   if (scope.info.header.parentRef == 0 && scope.reference == scope.moduleScope->superReference) {
      if (!scope.info.methods.exist(scope.moduleScope->dispatch_message))
         scope.raiseError(errNoDispatcher, node);
   }

   // save declaration
   scope.save();

   // compile class class if it available
   if (scope.info.header.classRef != scope.reference && scope.info.header.classRef != 0) {
      ClassScope classClassScope((NamespaceScope*)scope.parent, scope.info.header.classRef);
      classClassScope.info.header.flags |= /*(*/elClassClass/* | elFinal)*/; // !! IMPORTANT : classclass flags should be set

      compileClassClassDeclaration(node, classClassScope, scope);
   }
}

void Compiler :: generateClassImplementation(SNode node, ClassScope& scope)
{
   analizeClassTree(node, scope);

   pos_t sourcePathRef = scope.saveSourcePath(_writer);

   CommandTape tape;
   _writer.generateClass(tape, node, sourcePathRef);

   // optimize
   optimizeTape(tape);

   //// create byte code sections
   //scope.save();
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);

   if (test(scope.info.header.flags, elExtension)) {
      scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;

      scope.embeddable = _logic->isEmbeddable(*scope.moduleScope, scope.extensionClassRef);
   }
   else if (_logic->isEmbeddable(scope.info)) {
      scope.embeddable = true;
   }

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
   declareSymbolAttributes(node, scope);

   if ((scope.constant || scope.outputRef != 0) && scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, true) == false) {
      scope.save();
   }
}

bool Compiler :: compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal/*, bool accumulatorMode*/)
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

   ref_t parentRef = 0;

   _Module* module = scope.moduleScope->module;
   MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

   //if (accumulatorMode) {
   //   if (dataWriter.Position() == 0)
   //      dataWriter.Memory()->addReference(scope.moduleScope->arrayReference | mskVMTRef, (ref_t)-4);

   //   //if (retVal.kind == okSignatureConstant) {
   //   //   dataWriter.Memory()->addReference(retVal.param | mskSignature, dataWriter.Position());

   //   //   dataWriter.writeDWord(0);
   //   //}
   //   //else {
   //      SymbolScope memberScope(nsScope, nsScope->mapAnonymous());
   //      if (!compileSymbolConstant(node, memberScope, retVal))
   //         return false;

   //      dataWriter.Memory()->addReference(memberScope.reference | mskConstantRef, dataWriter.Position());
   //      dataWriter.writeDWord(0);
   //   //}
   //}
   //else {
      if (retVal.kind == okIntConstant || retVal.kind == okUIntConstant) {
         size_t value = module->resolveConstant(retVal.param).toULong(16);

         dataWriter.writeDWord(value);

         parentRef = scope.moduleScope->intReference;
      }
      //else if (retVal.kind == okLongConstant) {
      //   long long value = module->resolveConstant(retVal.param).toULongLong(10, 1);

      //   dataWriter.write(&value, 8u);

      //   parentRef = scope.moduleScope->longReference;
      //}
      //else if (retVal.kind == okRealConstant) {
      //   double value = module->resolveConstant(retVal.param).toDouble();

      //   dataWriter.write(&value, 8u);

      //   parentRef = scope.moduleScope->realReference;
      //}
      else if (retVal.kind == okLiteralConstant) {
         ident_t value = module->resolveConstant(retVal.param);

         dataWriter.writeLiteral(value, getlength(value) + 1);

         parentRef = scope.moduleScope->literalReference;
      }
      else if (retVal.kind == okWideLiteralConstant) {
         WideString wideValue(module->resolveConstant(retVal.param));

         dataWriter.writeLiteral(wideValue, getlength(wideValue) + 1);

         parentRef = scope.moduleScope->wideReference;
      }
      //else if (retVal.kind == okCharConstant) {
      //   ident_t value = module->resolveConstant(retVal.param);

      //   dataWriter.writeLiteral(value, getlength(value));

      //   parentRef = scope.moduleScope->charReference;
      //}
      //else if (retVal.kind == okSignatureConstant) {
      //   dataWriter.Memory()->addReference(retVal.param | mskSignature, dataWriter.Position());
      //   dataWriter.writeDWord(0);

      //   parentRef = scope.moduleScope->signatureReference;
      //}
      else if (retVal.kind == okObject) {
         SNode root = node.findSubNodeMask(lxObjectMask);

         if (root == lxConstantList/* && !accumulatorMode*/) {
            SymbolExpressionInfo info;
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

      dataWriter.Memory()->addReference(parentRef | mskVMTRef, (ref_t)-4);

      if (parentRef == scope.moduleScope->intReference) {
         nsScope->defineConstantSymbol(scope.reference, V_INT32);
      }
      else nsScope->defineConstantSymbol(scope.reference, parentRef);
   //}

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
   // HOTFIX : due to implementation (compileSymbolConstant requires constant types) typecast should be done explicitly
   ObjectInfo retVal = compileExpression(writer, expression, codeScope, 0, isSingleStatement(expression) ? HINT_ROOTSYMBOL : 0);
   if (scope.outputRef == 0) {
      if (resolveObjectReference(*scope.moduleScope, retVal) != 0) {
         // HOTFIX : if the result of the operation is qualified - it should be saved as symbol type
         scope.outputRef = resolveObjectReference(*scope.moduleScope, retVal);
         scope.save();
      }
   }
   else convertObject(writer, codeScope, scope.outputRef, retVal);

   writer.removeBookmark();
   writer.closeNode();
   writer.closeNode();

   analizeSymbolTree(expressionTree.readRoot(), scope);
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

   pos_t sourcePathRef = scope.saveSourcePath(_writer);

   CommandTape tape;
   _writer.generateSymbol(tape, expressionTree.readRoot(), isStatic, sourcePathRef);

   // optimize
   optimizeTape(tape);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
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
//   if (!isSealedStaticField(target.param)) {
//      if (target.kind == okStaticField) {
//         writeTerminal(writer, node, codeScope, ObjectInfo(okClassStaticField, scope.reference, target.param, target.extraparam), HINT_NODEBUGINFO);
//      }
//      else if (target.kind == okStaticConstantField) {
//         writeTerminal(writer, node, codeScope, ObjectInfo(okClassStaticConstantField, scope.reference, target.param, target.extraparam), HINT_NODEBUGINFO);
//      }
//   }
//   else writeTerminal(writer, node, codeScope, target, HINT_NODEBUGINFO);
//
//   writer.newBookmark();
//   ObjectInfo source = compileExpression(writer, node, codeScope, target.extraparam, 0);
//
//   writer.removeBookmark();
//   writer.closeNode();
//   writer.closeNode();
//
//   analizeSymbolTree(expressionTree.readRoot(), scope);
//
//   compilePreloadedCode(*scope.moduleScope, expressionTree.readRoot());
//}

// NOTE : elementRef is used for binary arrays
ObjectInfo Compiler :: assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t elementRef)
{
   ObjectInfo retVal(okObject, 0, targetRef, 0, elementRef);

   int size = _logic->defineStructSize(*scope.moduleScope, targetRef, elementRef);
   if (size != 0) {
      if (allocateStructure(scope, size, false, retVal)) {
         retVal.extraparam = targetRef;

         writer.insertChild(0, lxLocalAddress, retVal.param);
         writer.appendNode(lxTempAttr);
         writer.insert(lxAssigning, size);
         writer.closeNode();
      }
      else if (size > 0) {
         writer.appendNode(lxTarget, targetRef);
         writer.insert(lxCreatingStruct, size);
         writer.closeNode();

         writer.insert(lxAssigning, size);
         writer.closeNode();
      }
      
      switch (targetRef) {
         case V_INT32:
            targetRef = scope.moduleScope->intReference;
            break;
         //case V_INT64:
         //   targetRef = scope.moduleScope->longReference;
         //   break;
         //case V_REAL64:
         //   targetRef = scope.moduleScope->realReference;
         //   break;
         //case V_SIGNATURE:
         //   targetRef = scope.moduleScope->signatureReference;
         //   break;
         //case V_MESSAGE:
         //   targetRef = scope.moduleScope->messageReference;
         //   break;
      }

      writer.appendNode(lxTarget, targetRef);
      writer.appendNode(lxBoxableAttr);
      writer.insert(lxBoxing,  size);
      writer.closeNode();

      retVal.kind = okObject;
      retVal.param = targetRef;

      return retVal;
   }
   else return retVal;
}

ref_t Compiler :: analizeExtCall(SNode node, NamespaceScope& scope)
{
   compileExternalArguments(node, scope);

   return V_INT32;
}

ref_t Compiler :: analizeInternalCall(SNode node, NamespaceScope& scope)
{
   analizeExpressionTree(node, scope, HINT_NOBOXING);

   return V_INT32;
}

//ref_t Compiler :: analizeArgUnboxing(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int)
//{
//   analizeExpressionTree(node, scope, /*warningScope, */HINT_NOBOXING);
//
//   return 0;
//}

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

ref_t Compiler :: analizeNestedExpression(SNode node, NamespaceScope& scope)
{
   // check if the nested collection can be treated like constant one
   bool constant = true;
   ref_t memberCounter = 0;
   SNode current = node.firstChild();
   while (constant && current != lxNone) {
      if (current == lxMember) {
         SNode object = current.findSubNodeMask(lxObjectMask);
         switch (object.type) {
            //case lxConstantChar:
            //case lxConstantClass:
            case lxConstantInt:
            //case lxConstantLong:
            case lxConstantList:
            //case lxConstantReal:
            case lxConstantString:
            case lxConstantWideStr:
            case lxConstantSymbol:
               break;
            case lxNested:
               analizeNestedExpression(object, scope);
               object.refresh();
               if (object != lxConstantList)
                  constant = false;

               break;
            case lxUnboxing:
               current = lxOuterMember;
               analizeBoxing(object, scope, HINT_NOUNBOXING);
               constant = false;
               break;
            default:
               constant = false;
               analizeExpressionTree(current, scope);
               break;
         }
         memberCounter++;
      }
      else if (current == lxOuterMember) {
         // nested class with outer member must not be constant
         constant = false;

         analizeExpression(current, scope);
      }
      //else if (current == lxOvreriddenMessage) {
      //   constant = false;
      //}
      current = current.nextNode();
   }

   if (node.argument != memberCounter)
      constant = false;

   // replace with constant array if possible
   if (constant && memberCounter > 0) {
      ref_t reference = scope.moduleScope->mapAnonymous();

      node = lxConstantList;
      node.setArgument(reference | mskConstArray);

      _writer.generateConstantList(node, scope.module, reference);
   }

   return node.findChild(lxTarget).argument;
}

ref_t Compiler :: analizeMessageCall(SNode node, NamespaceScope& scope, int mode)
{   
   //if (node.existChild(lxEmbeddableAttr)) {
   //   if (!_logic->optimizeEmbeddable(node, *scope.moduleScope))
   //      node.appendNode(lxEmbeddable);
   //}

   int stackSafeAttr = node.findChild(lxStacksafeAttr).argument;
   int flag = 1;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         int paramMode = 0;
         if (test(stackSafeAttr, flag)) {
            paramMode |= HINT_NOBOXING;
         }
         else paramMode |= HINT_DYNAMIC_OBJECT;

         analizeExpression(current, scope, /*warningScope, */paramMode);

         flag <<= 1;
      }
      current = current.nextNode();
   }

   return node.findChild(lxTarget).argument;
}

ref_t Compiler :: analizeAssigning(SNode node, NamespaceScope& scope, int mode)
{
   //ref_t targetRef = node.findChild(lxTarget).argument;
   SNode targetNode = node.firstChild(lxObjectMask);
   SNode sourceNode = targetNode.nextNode(lxObjectMask);

   ref_t sourceRef = analizeExpression(sourceNode, scope, (node.argument != 0 ? HINT_NOBOXING | HINT_NOUNBOXING : HINT_NOUNBOXING));
   //switch (sourceNode) {
   //   case lxStdExternalCall:
   //   case lxExternalCall:
   //   case lxCoreAPICall:
   //      if (test(mode, HINT_INT64EXPECTED)) {
   //         // HOTFIX : to recognize external function returning long number
   //         sourceRef = V_INT64;
   //      }
   //      else if (test(mode, HINT_REAL64EXPECTED)) {
   //         // HOTFIX : to recognize external function returning real number
   //         node.appendNode(lxFPUTarget);

   //         sourceRef = V_REAL64;
   //      }
   //      break;
   //}

   if (node.argument != 0) {
      SNode intValue = node.findSubNode(lxConstantInt);
      if (intValue != lxNone && node.argument == 4) {
         // direct operation with numeric constants
         node.set(lxIntOp, SET_OPERATOR_ID);
      }
      else {
         SNode subNode = node.findSubNode(lxDirectCalling, lxSDirctCalling, lxAssigning);
         if (subNode == lxAssigning && targetNode != lxFieldAddress) {
            // HOTFIX : an extra assignment should be removed only for the operations with local variables
            bool tempAttr = subNode.existChild(lxTempAttr);

            // assignment operation
            SNode operationNode = subNode.findChild(lxIntOp/*, lxRealOp, lxLongOp*/, lxIntArrOp, lxByteArrOp, lxShortArrOp);
            if (operationNode != lxNone) {
               SNode larg = operationNode.findSubNodeMask(lxObjectMask);
               SNode rarg = operationNode.firstChild(lxObjectMask).nextSubNodeMask(lxObjectMask);
               SNode target = node.firstChild(lxObjectMask);
               if (rarg.type == targetNode.type && rarg.argument == targetNode.argument) {
                  // if the target is used in the subexpression rvalue
                  // do nothing
               }
               // if it is an operation with the same target
               else if (larg.type == target.type && larg.argument == target.argument) {
                  // remove an extra assignment
                  larg = subNode.findSubNodeMask(lxObjectMask);

                  larg = target.type;
                  larg.setArgument(target.argument);
                  node = lxExpression;
                  target = lxIdle;

                  // replace add / subtract with append / reduce and remove an assignment
                  switch (operationNode.argument) {
                     case ADD_OPERATOR_ID:
                        operationNode.setArgument(APPEND_OPERATOR_ID);
                        subNode = lxExpression;
                        larg = lxIdle;
                        break;
                     case SUB_OPERATOR_ID:
                        operationNode.setArgument(REDUCE_OPERATOR_ID);
                        subNode = lxExpression;
                        larg = lxIdle;
                        break;
                  }
               }
               // if it is an operation with an extra temporal variable
               else if ((node.argument == subNode.argument || operationNode == lxByteArrOp || operationNode == lxShortArrOp) && tempAttr) {
                  larg = subNode.findSubNodeMask(lxObjectMask);

                  if ((larg.type == targetNode.type && larg.argument == targetNode.argument) || (tempAttr && subNode.argument == node.argument && larg == lxLocalAddress)) {
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
            //if (subNode.existChild(lxEmbeddable)) {
            //   if (!_logic->optimizeEmbeddableGet(*scope.moduleScope, *this, node)) {
            //      _logic->optimizeEmbeddableOp(*scope.moduleScope, *this, node);
            //   }
            //}
            /*else */if (subNode.existChild(lxBoxableAttr) && subNode.existChild(lxStacksafeAttr)) {
               SNode createNode = subNode.findChild(lxCreatingStruct/*, lxImplicitCall*/);
               //if (createNode == lxImplicitCall && createNode.argument != encodeAction(NEWOBJECT_MESSAGE_ID))
               //   // HOTFIX : recognize only implicit constructor call
               //   createNode = lxNone;

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

ref_t Compiler :: analizeBoxing(SNode node, NamespaceScope& scope, int mode)
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
   if (sourceNode == lxNewArrOp) {
      // HOTFIX : set correct target for the new operator and comment out the outer boxing
      sourceNode.setArgument(targetRef);

      analizeExpression(sourceNode, scope, HINT_NOBOXING);

      boxing = false;
   }
   else {
      // for boxing stack allocated / embeddable variables - source is the same as target
      if ((sourceNode == lxLocalAddress || sourceNode == lxFieldAddress || sourceNode == lxLocal || sourceNode == lxSelfLocal) && node.argument != 0) {
         sourceRef = targetRef;
      }
      // HOTFIX : do not box constant classes
      else if (sourceNode == lxConstantInt && targetRef == scope.moduleScope->intReference) {
         boxing = false;
      }
      //else if (sourceNode == lxConstantReal && targetRef == scope.moduleScope->realReference) {
      //   boxing = false;
      //}
      else if (sourceNode == lxConstantSymbol && targetRef == scope.moduleScope->intReference) {
         boxing = false;
      }
      //else if (sourceNode == lxMessageConstant && targetRef == scope.moduleScope->messageReference) {
      //   boxing = false;
      //}
      //else if (sourceNode == lxSignatureConstant && targetRef == scope.moduleScope->signatureReference) {
      //   boxing = false;
      //}
      else if (node == lxUnboxing && !boxing) {
         //HOTFIX : to unbox structure field correctly
         sourceRef = analizeExpression(sourceNode, scope, /*warningScope, */HINT_NOBOXING | HINT_UNBOXINGEXPECTED);
      }
      else {
         if ((sourceNode == lxBoxing || sourceNode == lxUnboxing) && (int)node.argument < 0 && (int)sourceNode.argument > 0) {
            //HOTFIX : boxing fixed-sized array
            if (sourceNode.existChild(lxFieldAddress)) {
               node.setArgument(-((int)sourceNode.argument / (int)node.argument));
            }
         }

         int subMode = HINT_NOBOXING;
         //if (targetRef == scope.moduleScope->longReference) {
         //   subMode |= HINT_INT64EXPECTED;
         //}
         //else if (targetRef == scope.moduleScope->realReference) {
         //   subMode |= HINT_REAL64EXPECTED;
         //}

         sourceRef = analizeExpression(sourceNode, scope, subMode);
      }

      // adjust primitive target
      if (/*_logic->*/isPrimitiveRef(targetRef) && boxing) {
         targetRef = resolvePrimitiveReference(scope, targetRef, node.findChild(lxElement).argument);
         node.findChild(lxTarget).setArgument(targetRef);
      }

      if (!_logic->validateBoxing(*scope.moduleScope, *this, node, targetRef, sourceRef, test(mode, HINT_UNBOXINGEXPECTED), test(mode, HINT_DYNAMIC_OBJECT))) {
         scope.raiseError(errIllegalOperation, node);
      }
   }

   if (!boxing && node != lxLocalUnboxing) {
      node = lxExpression;
   }

   return targetRef;
}

//ref_t Compiler :: analizeArgBoxing(SNode node, NamespaceScope& scope, /*WarningScope& warningScope, */int mode)
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
//   analizeExpressionTree(node, scope, /*warningScope, */HINT_NOBOXING);
//
//   return scope.moduleScope->arrayReference;
//}

ref_t Compiler :: analizeSymbol(SNode& node, NamespaceScope& scope)
{
   ObjectInfo result = scope.defineObjectInfo(node.argument, true);
   switch (result.kind) {
      case okClass:
         node = lxClassSymbol;
         break;
      case okConstantSymbol:
      case okExtension:
         node = lxConstantSymbol;
         break;
   }

   return resolveObjectReference(*scope.moduleScope, result);
}

ref_t Compiler :: analizeOp(SNode current, NamespaceScope& scope/*, WarningScope& warningScope*/)
{
   int lmask = HINT_NOBOXING;
   if (current.argument == REFER_OPERATOR_ID) {
      switch (current.type) {
         case lxIntArrOp:
         case lxByteArrOp:
         case lxShortArrOp:
         //case lxBinArrOp:
            lmask |= HINT_NOUNBOXING;
            break;
         default:
            break;
      }
   }

   SNode loperand = current.firstChild(lxObjectMask);
   analizeExpression(loperand, scope, /*warningScope, */lmask);

   SNode roperand = loperand.nextNode(lxObjectMask);
   analizeExpression(roperand, scope, /*warningScope, */HINT_NOBOXING);

   SNode roperand2 = roperand.nextNode(lxObjectMask);
   if (roperand2 != lxNone)
      analizeExpression(roperand2, scope, /*warningScope, */HINT_NOBOXING);

   if (current == lxIntOp && loperand == lxConstantInt && roperand == lxConstantInt) {
      int val = 0;
      if (calculateIntOp(current.argument, loperand.findChild(lxIntValue).argument, roperand.findChild(lxIntValue).argument, val)) {
         loperand = lxIdle;
         roperand = lxIdle;

         IdentifierString str;
         str.appendHex(val);
         current.set(lxConstantInt, scope.module->mapConstant(str.c_str()));
         current.appendNode(lxIntValue, val);

         return V_INT32;
      }
   }
   //else if (current == lxRealOp && loperand == lxConstantReal && roperand == lxConstantReal) {
   //   double d1 = scope.module->resolveConstant(loperand.argument).toDouble();
   //   double d2 = scope.module->resolveConstant(roperand.argument).toDouble();
   //   double val = 0;
   //   if (calculateRealOp(current.argument, d1, d2, val)) {
   //      loperand = lxIdle;
   //      roperand = lxIdle;

   //      IdentifierString str;
   //      str.appendDouble(val);
   //      current.set(lxConstantReal, scope.module->mapConstant(str.c_str()));

   //      return V_REAL64;
   //   }
   //}

   switch (current) {
      case lxIntOp:
      case lxByteArrOp:
      case lxIntArrOp:
      case lxShortArrOp:
         return V_INT32;
      //case lxLongOp:
      //   return V_INT64;
      //case lxRealOp:
      //   return V_REAL64;
      //case lxBinArrOp:
      //   return V_BINARY;
      case lxNewArrOp:
         return current.argument;
      default:
         return 0;
   }
}

ref_t Compiler :: analizeExpression(SNode current, NamespaceScope& scope, int mode)
{
   switch (current.type) {
      case lxCalling:
      case lxDirectCalling:
      case lxSDirctCalling:
//      case lxImplicitCall:
         return analizeMessageCall(current, scope, mode);
      case lxExpression:
      case lxReturning:
         return analizeExpression(current.firstChild(lxObjectMask), scope, mode);
      //case lxAltExpression:
      case lxBranching:
      //case lxTrying:
         analizeExpressionTree(current, scope);
         return 0;
      case lxBoxing:
      case lxCondBoxing:
      case lxUnboxing:
         return analizeBoxing(current, scope, /*warningScope, */mode);
      //case lxArgBoxing:
      //   return analizeArgBoxing(current, scope, /*warningScope, */mode);
      //case lxArgUnboxing:
      //   return analizeArgUnboxing(current, scope, /*warningScope, */mode);
      case lxAssigning:
         return analizeAssigning(current, scope, mode);
      case lxSymbolReference:
         return analizeSymbol(current, scope/*, warningScope*/);
      case lxIntOp:
      //case lxLongOp:
      //case lxRealOp:
      case lxIntArrOp:
      case lxShortArrOp:
      case lxByteArrOp:
      case lxArrOp:
      //case lxBinArrOp:
      case lxNewArrOp:
      //case lxArgArrOp:
      //case lxBoolOp:
         return analizeOp(current, scope);
      case lxInternalCall:
         return analizeInternalCall(current, scope);
      case lxStdExternalCall:
      case lxExternalCall:
      case lxCoreAPICall:
         return analizeExtCall(current, scope);
      case lxLooping:
      //case lxSwitching:
      //case lxOption:
      case lxElse:
         analizeExpressionTree(current, scope);
         return 0;
      case lxNested:
         return analizeNestedExpression(current, scope);
      default:
         return current.findChild(lxTarget).argument;
   }
}

void Compiler :: analizeBranching(SNode node, NamespaceScope& scope, int mode)
{
   analizeExpressionTree(node, scope, mode);

   _logic->optimizeBranchingOp(*scope.moduleScope, node);
}

void Compiler :: analizeExpressionTree(SNode node, NamespaceScope& scope, int mode)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxElse:
         case lxCode:
         //case lxIf:
         //case lxExternFrame:
            analizeExpressionTree(current, scope);
            break;
         case lxBranching:
         case lxLooping:
            analizeBranching(current, scope);
            break;
         default:
            if (test(current.type, lxObjectMask)) {
               analizeExpression(current, scope, mode);
            }
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: analizeCode(SNode node, NamespaceScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxReturning:
            analizeExpressionTree(current, scope, HINT_NOUNBOXING | HINT_NOCONDBOXING);
            break;
         case lxExpression:
         //case lxExternFrame:
         case lxCalling:
            analizeExpressionTree(current, scope);
            break;
         default:
            if (test(current.type, lxObjectMask)) {
               analizeExpression(current, scope/*, 0*/);
            }
            break;
      }
      current = current.nextNode();
   }
}

void Compiler :: analizeMethod(SNode node, NamespaceScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxNewFrame) {
         analizeCode(current, scope);
      }
      current = current.nextNode();
   }
}

void Compiler :: analizeClassTree(SNode node, ClassScope& scope)
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         analizeMethod(current, *nsScope);

         //if (test(_optFlag, 1)) {
         //   if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
         //      defineEmbeddableAttributes(scope, current);
         //   }
         //}
      }

      current = current.nextNode();
   }
}

void Compiler :: analizeSymbolTree(SNode node, Scope& scope)
{
   //WarningScope warningScope(warningMask);

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);

   SNode current = node.firstChild();
   while (current != lxNone) {
      /*if (current == lxWarningMask) {
         warningMask = current.argument;
      }
      else */if (test(current.type, lxExprMask)) {
         analizeExpressionTree(current, *nsScope, HINT_NOUNBOXING);
      }

      current = current.nextNode();
   }
}

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
//void Compiler :: compileForward(SNode ns, NamespaceScope& scope)
//{
//   ident_t shortcut = ns.findChild(lxIdentifier, lxReference).identifier();
//   ident_t reference = ns.findChild(lxForward).findChild(lxIdentifier, lxReference).identifier();
//
//   if (!scope.defineForward(shortcut, reference))
//      scope.raiseError(errDuplicatedDefinition, ns);
//}

void Compiler :: compileForward(SNode ns, NamespaceScope& scope)
{
   ident_t shortcut = ns.firstChild(lxTerminalMask).identifier();
   SNode referenceNode = ns.findChild(lxAttribute);

   if (scope.moduleScope->attributes.exist(shortcut))
      scope.raiseError(errDuplicatedDefinition, ns);

   ref_t classRef = resolveImplicitIdentifier(scope, referenceNode.firstChild(lxTerminalMask));
   if (!classRef)
      scope.raiseError(errInvalidHint, referenceNode);

   scope.moduleScope->attributes.add(shortcut, classRef);
   scope.moduleScope->saveAttribute(shortcut, classRef);
}

//////bool Compiler :: validate(_ProjectManager& project, _Module* module, int reference)
////{
////   int   mask = reference & mskAnyRef;
////   ref_t extReference = 0;
////   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
////   _Module* extModule = project.resolveModule(refName, extReference, true);
////
////   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
////}
//
////void Compiler :: validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project)
////{
////   //for (List<Unresolved>::Iterator it = unresolveds.start() ; !it.Eof() ; it++) {
////   //   if (!validate(project, (*it).module, (*it).reference)) {
////   //      ident_t refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);
////
////   //      project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
////   //   }
////   //}
////}
//
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
//         case lxInclude:
//            compileForward(current, scope);
//            break;
         case lxClass:
         {
            ident_t name = scope.module->resolveReference(current.argument);

            // compile class
            ClassScope classScope(&scope, current.argument);
            scope.moduleScope->loadClassInfo(classScope.info, current.argument, false);

            compileClassImplementation(expressionTree, current, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference && classScope.info.header.classRef != 0) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef);
               scope.moduleScope->loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);
               classClassScope.classClassMode = true;

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

bool Compiler :: compileDeclarations(SNode node, NamespaceScope& scope, bool& repeatMode)
{
   SNode current = node.firstChild();

   if (scope.moduleScope->superReference == 0)
      scope.raiseError(errNotDefinedBaseClass);

   // first pass - declaration
   bool declared = false;
   while (current != lxNone) {
      //      if (scope.mapAttribute(name) != 0)
      //         scope.raiseWarning(WARNING_LEVEL_3, wrnAmbiguousIdentifier, name);

      if (current.argument == 0 || current.argument == INVALID_REF) {
         // hotfix : ignore already declared classes and symbols
         switch (current) {
            case lxClass:
               /*if (!isDependentOnNotDeclaredClass(findBaseParent(current), scope))*/ {
                  current.setArgument(/*name == lxNone ? scope.mapNestedExpression() : */scope.mapNewTerminal(current.findChild(lxNameAttr)));
            
                  ClassScope classScope(&scope, current.argument);
            
                  // NOTE : the symbol section is created even if the class symbol doesn't exist
                  scope.moduleScope->mapSection(classScope.reference | mskSymbolRef, false);

                  // build class expression tree
                  compileClassDeclaration(current, classScope);
            
                  declared = true;
               }
//               else repeatMode = true;
               break;
            case lxSymbol:
            {
               current.setArgument(scope.mapNewTerminal(current.findChild(lxNameAttr)));

               SymbolScope symbolScope(&scope, current.argument);

               scope.moduleScope->mapSection(symbolScope.reference | mskSymbolRef, false);

               // declare symbol
               compileSymbolDeclaration(current, symbolScope);
               declared = true;
               break;
            }
            case lxForward:
               compileForward(current, scope);
               break;
         }
      }
      current = current.nextNode();
   }

   return declared;
}

void Compiler :: declareNamespace(SNode node, NamespaceScope& scope, bool withFullInfo)
{
   // add the module itself
   scope.importedNs.add(scope.module->Name().clone());

   // system module should be included by default
   if (!scope.module->Name().compare(STANDARD_MODULE)) {
      scope.importedNs.add(_ELENA_::ident_t(STANDARD_MODULE).clone());
   }

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxSourcePath) {
         scope.sourcePath.copy(current.identifier());
      }
      else if (current == lxImport) {
         bool duplicateInclusion = false;
         if (scope.moduleScope->includeNamespace(scope.importedNs, current.identifier(), duplicateInclusion)) {
            //if (duplicateExtensions)
            //   scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, ns);
         }
         else if (duplicateInclusion) {
            scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateInclude, current);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, current);
      }

      current = current.nextNode();
   }

   if (withFullInfo) {
      for (auto it = scope.importedNs.start(); !it.Eof(); it++) {
         ident_t imported_ns = *it;

         scope.loadModuleInfo(imported_ns);
      }
   }
}

bool Compiler :: declareModule(SyntaxTree& syntaxTree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*/, bool& repeatMode/*, ExtensionMap* extensions*/)
{
   SNode current = syntaxTree.readRoot().firstChild();
   bool retVal = false;
   while (current != lxNone) {
      // declare classes several times to ignore the declaration order
      NamespaceScope namespaceScope(&scope, current.identifier());
      declareNamespace(current, namespaceScope, false);

      retVal |= compileDeclarations(current, namespaceScope, repeatMode);
      
      //   if (extensions != NULL) {
      //      for (auto it = namespaceScope.extensions.start(); !it.Eof(); it++) {
      //         extensions->add(it.key(), *it);
      //      }
      //   }

      current = current.nextNode();
   }

   return retVal;
}

void Compiler :: compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*//*, Unresolveds& unresolveds*/)
{
   SNode current = syntaxTree.readRoot().firstChild();
   while (current != lxNone) {
      // declare classes several times to ignore the declaration order
      NamespaceScope namespaceScope(&scope, current.identifier()/*, true*/);
      declareNamespace(current, namespaceScope, true);
      
      if (!emptystr(namespaceScope.sourcePath))
         scope.project->printInfo("%s", namespaceScope.sourcePath);
      
      compileImplementations(current, namespaceScope);

      current = current.nextNode();
   }
}

inline ref_t safeMapReference(_Module* module, _ProjectManager* project, ident_t referenceName)
{
   if (!emptystr(referenceName)) {
      // HOTFIX : for the standard module the references should be mapped forcefully
      if (module->Name().compare(STANDARD_MODULE)) {
         return module->mapReference(referenceName + getlength(module->Name()));
      }
      else {
         ref_t extRef = 0;
         _Module* extModule = project->resolveModule(referenceName, extRef, true);

         return importReference(extModule, extRef, module);
      }
   }
   else return 0;
}

inline ref_t safeMapWeakReference(_Module* module, ident_t referenceName)
{
   if (!emptystr(referenceName)) {
      // HOTFIX : for the standard module the references should be mapped forcefully
      if (module->Name().compare(STANDARD_MODULE)) {
         return module->mapReference(referenceName + getlength(module->Name()), false);
      }
      else return module->mapReference(referenceName, false);
   }
   else return 0;
}

void Compiler :: loadAttributes(_ModuleScope& scope, ident_t name, MessageMap* attributes)
{
   _Module* extModule = scope.project->loadModule(name, true);
   bool duplicates = false;
   if (extModule) {
      //      //bool owner = module == extModule;
      //
      ReferenceNs sectionName("'", ATTRIBUTE_SECTION);

      _Memory* section = extModule->mapSection(extModule->mapReference(sectionName, true) | mskMetaRDataRef, true);
      if (section) {
         MemoryReader metaReader(section);
         while (!metaReader.Eof()) {
            ref_t attrRef = metaReader.getDWord();
            if (!isPrimitiveRef(attrRef)) {
               attrRef = importReference(extModule, attrRef, scope.module);
            }

            ident_t attrName = metaReader.getLiteral(DEFAULT_STR);

            if (!attributes->add(attrName, attrRef, true))
               duplicates = true;
         }
      }
   }
   //   return duplicates;
}

void Compiler :: initializeScope(ident_t name, _ModuleScope& scope, bool withDebugInfo)
{
   scope.module = scope.project->createModule(name);

   if (withDebugInfo) {
      scope.debugModule = scope.project->createDebugModule(name);
      // HOTFIX : save the module name in strings table
      _writer.writeSourcePath(scope.debugModule, name);
   }      

   // cache the frequently used references
   scope.superReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(SUPER_FORWARD));
   scope.intReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(INT_FORWARD));
//   scope.longReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(LONG_FORWARD));
//   scope.realReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(REAL_FORWARD));
//   scope.arrayReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(ARRAY_FORWARD));
   scope.literalReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(STR_FORWARD));
   scope.wideReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(WIDESTR_FORWARD));
//   scope.charReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(CHAR_FORWARD));
//   scope.boolReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(BOOL_FORWARD));
//   scope.messageReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(MESSAGE_FORWARD));
   scope.refTemplateReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(REFTEMPLATE_FORWARD));
   scope.arrayTemplateReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(ARRAYTEMPLATE_FORWARD));
//   scope.signatureReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(SIGNATURE_FORWARD));
//   scope.extMessageReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(EXT_MESSAGE_FORWARD));
//   scope.lazyExprReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(LAZYEXPR_FORWARD));
   scope.closureTemplateReference = safeMapWeakReference(scope.module, scope.project->resolveForward(CLOSURETEMPLATE_FORWARD));

   scope.branchingInfo.reference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(BOOL_FORWARD));
   scope.branchingInfo.trueRef = safeMapReference(scope.module, scope.project, scope.project->resolveForward(TRUE_FORWARD));
   scope.branchingInfo.falseRef = safeMapReference(scope.module, scope.project, scope.project->resolveForward(FALSE_FORWARD));

   // cache the frequently used messages
   scope.dispatch_message = encodeAction(scope.module->mapAction(DISPATCH_MESSAGE, 0, false));
   scope.newobject_message = encodeAction(scope.module->mapAction(NEWOBJECT_MESSAGE, 0, false));

   if (!scope.module->Name().compare(STANDARD_MODULE)) {
      // system attributes should be loaded automatically
      loadAttributes(scope, STANDARD_MODULE, &scope.attributes);
   }

//   createPackageInfo(scope.module, *scope.project);
}

//void Compiler :: injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef)
//{
//   // injecting auto-generated static sealed constant field, (argument=INVALID_REF)
//   SNode fieldNode = classNode.appendNode(lxClassField, INVALID_REF); 
//   fieldNode.appendNode(lxAttribute, V_STATIC);
//   fieldNode.appendNode(lxAttribute, V_SEALED);
//   fieldNode.appendNode(lxAttribute, V_CONST);
//
//   fieldNode.appendNode(lxIdentifier, fieldName);
//   if (fieldRef) {
//      ident_t referenceName = scope.module->resolveReference(fieldRef);
//      if (isWeakReference(referenceName)) {
//         IdentifierString fullName(scope.module->Name(), referenceName);
//
//         fieldNode.appendNode(lxClassRefAttr, fullName.c_str());
//      }
//      else fieldNode.appendNode(lxClassRefAttr, referenceName);
//   }      
//}
//
//void Compiler :: generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef)
//{
//   _Memory* section = scope.module->mapSection(enumRef | mskRDataRef, true);
//   if (!section) {
//      // if the member list is not available - create and assign to the static field
//      section = scope.module->mapSection(enumRef | mskRDataRef, false);
//
//      SyntaxTree expressionTree;
//      SyntaxWriter writer(expressionTree);
//
//      writer.newNode(lxExpression);
//      writer.newNode(lxAssigning);
//      writer.appendNode(lxStaticField, enumRef);
//      writer.appendNode(lxConstantList, enumRef | mskConstArray);
//      writer.closeNode();
//      writer.closeNode();
//
//      section->addReference(scope.arrayReference | mskVMTRef, (pos_t)-4);
//
//      compilePreloadedCode(scope, expressionTree.readRoot());
//   }
//
//   MemoryWriter metaWriter(section);
//
//   metaWriter.writeRef(memberRef | mskConstantRef, 0);
//}
//
////void Compiler :: generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument)
////{
////   MemoryWriter writer(scope.module->mapSection(listRef | mskRDataRef, false));
////
////   _writer.generateConstantMember(writer, type, argument);
////}

void Compiler :: generateOverloadListMember(_ModuleScope& scope, ref_t listRef, ref_t messageRef)
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

void Compiler :: generateClosedOverloadListMember(_ModuleScope& scope, ref_t listRef, ref_t messageRef, ref_t classRef)
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

void Compiler :: generateSealedOverloadListMember(_ModuleScope& scope, ref_t listRef, ref_t messageRef, ref_t classRef)
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

void Compiler :: injectBoxing(SyntaxWriter& writer, _ModuleScope&, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode)
{
   if (arrayMode && argument == 0) {
      // HOTFIX : to iundicate a primitive array boxing
      writer.appendNode(lxBoxableAttr, -1);
   }
   else writer.appendNode(lxBoxableAttr);

   writer.appendNode(lxTarget, targetClassRef);
   writer.insert(boxingType, argument);
   writer.closeNode();
}

void Compiler :: injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg, ref_t targetClassRef/*, ref_t targetRef*/, int stackSafeAttr)
{
   writer.insertChild(0, targetOp, targetArg/*, lxTarget, targetClassRef*/);

   if (convertOp != lxNone) {
      writer.appendNode(lxCallTarget, targetClassRef);
      writer.appendNode(lxBoxableAttr);
      if (stackSafeAttr)
         writer.appendNode(lxStacksafeAttr, stackSafeAttr);
      
      writer.insert(convertOp, convertArg);
      writer.closeNode();
   }
}

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
//void Compiler :: injectEmbeddableOp(_CompilerScope& scope, SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb)
//{
//   SNode assignTarget = assignNode.findPattern(SNodePattern(lxLocalAddress));
//
//   if (paramCount == -1 && verb == 0) {
//      // if it is an embeddable constructor call
//      SNode sourceNode = assignNode.firstChild(lxObjectMask);
//      
//      SNode callTargetNode = callNode.firstChild(lxObjectMask);
//      callTargetNode.set(sourceNode.type, sourceNode.argument);
//
//      callNode.setArgument(subject);
//
//      SNode targetNode = assignTarget.findChild(lxTarget);
//
//      SNode callTarget = callNode.findChild(lxCallTarget);
//      callTarget.setArgument(targetNode.argument);
//
//      assignNode = lxExpression;
//      sourceNode = lxIdle;
//
//      // check if inline initializer is declared
//      ClassInfo targetInfo;
//      scope.loadClassInfo(targetInfo, targetNode.argument);
//      ref_t initConstructor = encodeMessage(INIT_MESSAGE_ID, 0) | SPECIAL_MESSAGE;
//      if (targetInfo.methods.exist(initConstructor)) {
//         // inject inline initializer call
//         SNode initNode = callTargetNode.appendNode(lxImplicitCall, initConstructor);
//         initNode.appendNode(lxTarget, targetNode.argument);
//      }
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

////void Compiler :: injectFieldExpression(SyntaxWriter& writer)
////{
////   writer.appendNode(lxResultField);
////
////   writer.insert(lxFieldExpression);
////   writer.closeNode();
////}
//
//void Compiler :: injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t embeddedMessageRef, ref_t genericMessage)
//{
//   SNode methNode = classNode.appendNode(lxConstructor, message);
//   methNode.appendNode(lxEmbeddableMssg, embeddedMessageRef);
//   methNode.appendNode(lxAttribute, tpEmbeddable);
//   methNode.appendNode(lxAttribute, tpConstructor);
//
//   if (genericMessage)
//      methNode.appendNode(lxMultiMethodAttr, genericMessage);
//
//   SNode codeNode = methNode.appendNode(lxDispatchCode, -1);
//}

void Compiler :: injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType)
{
   ref_t resendMessage = message;
   ref_t actionRef, flags;
   int paramCount;
   decodeMessage(message, actionRef, paramCount, flags);

   ref_t dummy = 0;
   ident_t actionName = scope.module->resolveAction(actionRef, dummy);

   ref_t signatureLen = 0;
   ref_t signatures[ARG_COUNT];

   //if (paramCount >= OPEN_ARG_COUNT) {
   //   for (int i = OPEN_ARG_COUNT + 1; i <= paramCount; i++) {
   //      signatures[signatureLen++] = scope.superReference;
   //   }
   //   signatures[signatureLen++] = scope.superReference;
   //}
   //else {
      for (int i = 0; i < paramCount; i++) {
         signatureLen++;
         signatures[i] = scope.superReference;
      }
   //}
   ref_t signRef = scope.module->mapAction(actionName, scope.module->mapSignature(signatures, signatureLen, false), false);

   resendMessage = encodeMessage(signRef, paramCount, flags);

   SNode methNode = classNode.appendNode(methodType, message);
   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   methNode.appendNode(lxAttribute, tpMultimethod);
   if (methodType == lxConstructor)
      methNode.appendNode(lxAttribute, tpConstructor);

   //if (actionRef == INVOKE_MESSAGE_ID)
   //   methNode.appendNode(lxAttribute, tpAction);

   methNode.appendNode(lxResendExpression, resendMessage);
}

//void Compiler :: injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType)
//{
//   ref_t actionRef = getAction(message);
//   ref_t signRef = 0;
//   IdentifierString sign(scope.module->resolveAction(actionRef, signRef));
//   int paramCount = getAbsoluteParamCount(message);
//
//   size_t signatureLen = 0;
//   ref_t  signatures[OPEN_ARG_COUNT];
//   for (int i = OPEN_ARG_COUNT + 1; i <= paramCount; i++) {
//      signatures[signatureLen++] = scope.superReference;
//   }
//   signatures[signatureLen++] = scope.arrayReference;
//
//   ref_t resendActionRef = scope.module->mapAction(sign, scope.module->mapSignature(signatures, signatureLen, false), false);
//   ref_t resendMessage = encodeMessage(resendActionRef, getParamCount(message) + 1);
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
//void Compiler :: injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument)
//{
//   SNode methNode = classNode.appendNode(lxClassMethod, message);
//   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
//   methNode.appendNode(lxAttribute, V_EMBEDDABLE);
////   methNode.appendNode(lxAttribute, V_SEALED);
//
//   SNode expr = methNode.appendNode(lxDispatchCode);
//   expr.appendNode(type, argument);
//}

void Compiler :: injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, ref_t message, ident_t variable, ref_t outputRef)
{
   SNode methNode = classNode.appendNode(lxClassMethod, message);
   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   //methNode.appendNode(lxAttribute, tpEmbeddable);
   methNode.appendNode(lxAttribute, tpSealed);

   if (outputRef)
      methNode.appendNode(lxTarget, outputRef);

   SNode expr = methNode.appendNode(lxReturning).appendNode(lxExpression);
   expr.appendNode(lxIdentifier, variable);
}

//void Compiler :: injectDirectMethodCall(SyntaxWriter& writer, ref_t targetRef, ref_t message)
//{
//   writer.appendNode(lxCallTarget, targetRef);
//
//   writer.insert(lxDirectCalling, message);
//   writer.closeNode();
//
//}

void Compiler :: generateClassSymbol(SyntaxWriter& writer, ClassScope& scope)
{
   CodeScope codeScope(&scope);

   writer.newNode(lxSymbol, scope.reference);
   writer.newNode(lxExpression);
   writeTerminal(writer, SNode(), codeScope, ObjectInfo(okClass, scope.reference/*, scope.info.header.classRef*/), HINT_NODEBUGINFO);
   writer.closeNode();
   writer.closeNode();
}

////void Compiler :: generateSymbolWithInitialization(SyntaxWriter& writer, ClassScope& scope, ref_t implicitConstructor)
////{
////   CodeScope codeScope(&scope);
////
////   writer.newNode(lxSymbol, scope.reference);
////   writeTerminal(writer, SNode(), codeScope, ObjectInfo(okConstantClass, scope.reference, scope.info.header.classRef), HINT_NODEBUGINFO);
////   writer.newNode(lxImplicitCall, implicitConstructor);
////   writer.appendNode(lxTarget, scope.reference);
////   writer.closeNode();
////   writer.closeNode();
////}
