//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

//#define FULL_OUTOUT_INFO 1

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

// --- Expr hint constants ---
constexpr auto HINT_NODEBUGINFO     = EAttr::eaNoDebugInfo;

// --- Scope hint constants ---
constexpr auto HINT_NESTEDNS        = EAttr::eaNestedNs;
constexpr auto HINT_INTERNALOP      = EAttr::eaIntern;

//constexpr auto HINT_CLOSURE_MASK    = EAttr::eaClosureMask;
constexpr auto HINT_SCOPE_MASK      = EAttr::eaScopeMask;
constexpr auto HINT_OBJECT_MASK     = EAttr::eaObjectMask;
constexpr auto HINT_MODULESCOPE     = EAttr::eaModuleScope;
constexpr auto HINT_PREVSCOPE       = EAttr::eaPreviousScope;
constexpr auto HINT_NEWOP           = EAttr::eaNewOp;
constexpr auto HINT_CASTOP          = EAttr::eaCast;
constexpr auto HINT_SILENT          = EAttr::eaSilent;
constexpr auto HINT_ROOTSYMBOL      = EAttr::eaRootSymbol;
constexpr auto HINT_ROOT            = EAttr::eaRoot;
constexpr auto HINT_INLINE_EXPR     = EAttr::eaInlineExpr;
constexpr auto HINT_NOPRIMITIVES    = EAttr::eaNoPrimitives;
constexpr auto HINT_DYNAMIC_OBJECT  = EAttr::eaDynamicObject;  // indicates that the structure MUST be boxed
constexpr auto HINT_NOBOXING        = EAttr::eaNoBoxing;
constexpr auto HINT_NOUNBOXING      = EAttr::eaNoUnboxing;
constexpr auto HINT_MEMBER          = EAttr::eaMember;
constexpr auto HINT_REFOP           = EAttr::eaRef;
constexpr auto HINT_PROP_MODE       = EAttr::eaPropExpr;
constexpr auto HINT_METAFIELD       = EAttr::eaMetaField;
constexpr auto HINT_LOOP            = EAttr::eaLoop;
constexpr auto HINT_EXTERNALOP      = EAttr::eaExtern;
constexpr auto HINT_FORWARD         = EAttr::eaForward;
constexpr auto HINT_PARAMSOP		   = EAttr::eaParams;
constexpr auto HINT_SWITCH          = EAttr::eaSwitch;
constexpr auto HINT_CLASSREF        = EAttr::eaClass;
constexpr auto HINT_YIELD_EXPR      = EAttr::eaYieldExpr;
constexpr auto HINT_MESSAGEREF      = EAttr::eaMssg;
constexpr auto HINT_VIRTUALEXPR     = EAttr::eaVirtualExpr;
constexpr auto HINT_SUBJECTREF      = EAttr::eaSubj;
constexpr auto HINT_DIRECTCALL      = EAttr::eaDirectCall;
constexpr auto HINT_PARAMETER       = EAttr::eaParameter;
constexpr auto HINT_LAZY_EXPR       = EAttr::eaLazy;
constexpr auto HINT_INLINEARGMODE   = EAttr::eaInlineArg;  // indicates that the argument list should be unboxed
constexpr auto HINT_CONSTEXPR       = EAttr::eaConstExpr;
constexpr auto HINT_CALLOP          = EAttr::eaCallOp;
constexpr auto HINT_REFEXPR         = EAttr::eaRefExpr;

// scope modes
constexpr auto INITIALIZER_SCOPE    = EAttr::eaInitializerScope;   // indicates the constructor or initializer method

typedef Compiler::ObjectInfo                 ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef ClassInfo::Attribute                 Attribute;
//typedef _CompilerLogic::ExpressionAttributes ExpressionAttributes;
typedef _CompilerLogic::FieldAttributes      FieldAttributes;

EAttr operator | (const EAttr& l, const EAttr& r)
{
   return (EAttr)((uint64_t)l | (uint64_t)r);
}

EAttr operator & (const EAttr& l, const EAttr& r)
{
   return (EAttr)((uint64_t)l & (uint64_t)r);
}

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

inline SNode findRootNode(SNode node, LexicalType type1, LexicalType type2, LexicalType type3)
{
   SNode lastNode = node;
   while (!node.compare(type1, type2, type3, lxNone)) {
      lastNode = node;
      node = node.parentNode();
   }

   return lastNode;
}

inline bool validateGenericClosure(ref_t* signature, size_t length)
{
   for (size_t i = 1; i < length; i++) {
      if (signature[i - 1] != signature[i])
         return false;
   }

   return true;
}

inline bool isConstantArg(SNode current)
{
   switch (current.type) {
      case lxLiteral:
      case lxWide:
      case lxCharacter:
      case lxInteger:
      case lxLong:
      case lxHexInteger:
      case lxReal:
      case lxExplicitConst:
      case lxMessage:
         return true;
      default:
         return false;
   }
}

inline bool isConstantArguments(SNode node)
{
   if (node == lxNone)
      return true;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression) {
         if (!isConstantArguments(current))
            return false;
      }
      else if (isConstantArg(current))
         return false;

      current = current.nextNode();
   }

   return true;
}

// --- Compiler::NamespaceScope ---

Compiler::NamespaceScope :: NamespaceScope(_ModuleScope* moduleScope, ExtensionMap* outerExtensionList)
   : Scope(moduleScope), constantHints(INVALID_REF), extensions(Pair<ref_t, ref_t>(0, 0)), importedNs(NULL, freestr),
   extensionDispatchers(INVALID_REF), extensionTargets(INVALID_REF), extensionTemplates(NULL, freestr)
{
   // by default - private visibility
   defaultVisibility = Visibility::Private;

   this->outerExtensionList = outerExtensionList;

//   // HOTFIX : package section should be created if at least literal class is declated
//   if (moduleScope->literalReference != 0) {
//      packageReference = module->mapReference(ReferenceNs("'", PACKAGE_SECTION));
//   }
//   else packageReference = 0;
}

Compiler::NamespaceScope :: NamespaceScope(NamespaceScope* parent)
   : Scope(parent), constantHints(INVALID_REF), extensions(Pair<ref_t, ref_t>(0, 0)), importedNs(NULL, freestr),
   extensionDispatchers(INVALID_REF), extensionTargets(INVALID_REF), extensionTemplates(NULL, freestr)
{
   outerExtensionList = parent->outerExtensionList;

   defaultVisibility = parent->defaultVisibility;
   sourcePath.copy(parent->sourcePath);
   nsName.copy(parent->ns);
}

ref_t Compiler::NamespaceScope :: resolveExtensionTarget(ref_t reference)
{
   ref_t resolved = extensionTargets.get(reference);
   if (resolved == INVALID_REF) {
      ClassInfo info;
      moduleScope->loadClassInfo(info, reference);

      auto key = info.fieldTypes.get(-1);
      resolved = key.value1;
      if (resolved)
         extensionTargets.add(reference, resolved);
   }

   return resolved;
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

ObjectInfo Compiler::NamespaceScope :: mapWeakReference(ident_t identifier, bool directResolved)
{
   ref_t reference = 0;
   if (directResolved) {
      reference = moduleScope->mapWeakReference(identifier);
   }
   else reference = moduleScope->mapFullReference(identifier);

   return defineObjectInfo(reference, true);
}

ObjectInfo Compiler::NamespaceScope :: mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
{
   ref_t reference = 0;
   if (!referenceOne) {
      // try resolve as type-alias
      reference = moduleScope->attributes.get(identifier);
      if (isPrimitiveRef(reference))
         reference = 0;
   }

   if (!reference)
      reference = resolveImplicitIdentifier(identifier, referenceOne, !EAttrs::test(mode, HINT_NESTEDNS));

   if (reference)
      return defineObjectInfo(reference, true);

   if (parent == NULL) {
      // outer most ns
      if (referenceOne) {
         return mapGlobal(identifier);
      }
      else if (identifier.compare(NIL_VAR)) {
         return ObjectInfo(okNil);
      }
   }

   return Scope::mapTerminal(identifier, referenceOne, mode | HINT_NESTEDNS);
}

ref_t Compiler::NamespaceScope :: resolveImplicitIdentifier(ident_t identifier, bool referenceOne, bool innermost)
{
   ref_t reference = forwards.get(identifier);
   if (reference)
      return reference;

   reference = moduleScope->resolveImplicitIdentifier(ns, identifier, Visibility::Public);

   // check if it is an internal one
   if (!reference)
      reference = moduleScope->resolveImplicitIdentifier(ns, identifier, Visibility::Internal);

   // check if it is a private one for the inner most
   if (innermost && !reference)
      reference = moduleScope->resolveImplicitIdentifier(ns, identifier, Visibility::Private);

   if (!reference && !referenceOne)
      reference = moduleScope->resolveImportedIdentifier(identifier, &importedNs);

   if (reference) {
      forwards.add(identifier, reference);
   }

   return reference;
}

ref_t Compiler::NamespaceScope :: mapNewTerminal(SNode terminal, Visibility visibility)
{
   if (terminal == lxNameAttr) {
      // verify if the name is unique
      ident_t name = terminal.firstChild(lxTerminalMask).identifier();

      terminal.setArgument(moduleScope->mapNewIdentifier(ns.c_str(), name, visibility));

      ref_t reference = terminal.argument;
      if (visibility == Visibility::Public) {
         ref_t dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Internal);
         if (!dup)
            dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Private);

         if (dup)
            reference = dup;
      }
      else if (visibility == Visibility::Internal) {
         ref_t dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Public);
         if (!dup)
            dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Private);

         if (dup)
            reference = dup;
      }
      else if (visibility == Visibility::Private) {
         ref_t dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Public);
         if (!dup)
            dup = moduleScope->resolveImplicitIdentifier(ns.c_str(), name, Visibility::Internal);

         if (dup)
            reference = dup;
      }

      if (module->mapSection(reference | mskSymbolRef, true))
         raiseError(errDuplicatedSymbol, terminal.firstChild(lxTerminalMask));

      return terminal.argument;
   }
   else if (terminal == lxNone) {
      return moduleScope->mapAnonymous();
   }
   else throw InternalError("Cannot map new terminal"); // !! temporal
}

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
            return ObjectInfo(okSingleton, reference, reference);
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
            ref_t outputRef = symbolInfo.exprRef;

            // if it is a constant
            if (symbolInfo.type == SymbolExpressionInfo::Type::Constant) {
               //ref_t classRef = symbolInfo.exprRef;

               /*if (symbolInfo.listRef != 0) {
                  return ObjectInfo(okArrayConst, symbolInfo.listRef, classRef);
               }
               else */return ObjectInfo(okConstantSymbol, reference, outputRef);
            }
            else if (symbolInfo.type == SymbolExpressionInfo::Type::Singleton) {
               return ObjectInfo(okSingleton, outputRef, outputRef);
            }
            else if (symbolInfo.type == SymbolExpressionInfo::Type::ConstantSymbol) {
               return defineObjectInfo(outputRef, true);
            }
            // if it is a typed symbol
            else if (outputRef != 0) {
               return ObjectInfo(okSymbol, reference, outputRef);
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
         extRef = metaReader.getDWord();
         ref_t message = metaReader.getDWord();
         ref_t strongMessage = metaReader.getDWord();

         if (extModule != module) {
            extRef = importReference(extModule, extRef, module);
            message = importMessage(extModule, message, module);
            strongMessage = importMessage(extModule, strongMessage, module);
         }

         if (!extRef) {
            // if it is an extension template
            ident_t pattern = metaReader.getLiteral(DEFAULT_STR);

            extensionTemplates.add(message, pattern.clone());
         }
         else extensions.add(message, Pair<ref_t, ref_t>(extRef, strongMessage));
      }
   }
}

void Compiler::NamespaceScope :: saveExtension(ref_t message, ref_t extRef, ref_t strongMessage, bool internalOne)
{
//   if (typeRef == INVALID_REF || typeRef == moduleScope->superReference)
//      typeRef = 0;

   Pair<ref_t, ref_t> extInfo(extRef, strongMessage);
   if (outerExtensionList != nullptr) {
      // COMPILER MAGIC : if it is template extension compilation
      outerExtensionList->add(message, extInfo);
   }
   else {
      IdentifierString sectionName(internalOne ? PRIVATE_PREFIX_NS : "'");
      if (!emptystr(ns)) {
         sectionName.append(ns);
         sectionName.append("'");
      }
      sectionName.append(EXTENSION_SECTION);

      MemoryWriter metaWriter(module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false));

      //   if (typeRef == moduleScope->superReference) {
      //      metaWriter.writeDWord(0);
      //   }
      /*else */metaWriter.writeDWord(extRef);
      metaWriter.writeDWord(message);
      metaWriter.writeDWord(strongMessage);
   }

   extensions.add(message, extInfo);
}

void Compiler::NamespaceScope :: saveExtensionTemplate(ref_t message, ident_t pattern)
{
   IdentifierString sectionName(/*internalOne ? PRIVATE_PREFIX_NS : */"'");
   if (!emptystr(ns)) {
      sectionName.append(ns);
      sectionName.append("'");
   }
   sectionName.append(EXTENSION_SECTION);

   MemoryWriter metaWriter(module->mapSection(module->mapReference(sectionName, false) | mskMetaRDataRef, false));

   metaWriter.writeDWord(0);
   metaWriter.writeDWord(message);
   metaWriter.writeDWord(0);
   metaWriter.writeLiteral(pattern.c_str());

   extensionTemplates.add(message, pattern.clone());
}

// --- Compiler::SourceScope ---

Compiler::SourceScope :: SourceScope(Scope* moduleScope, ref_t reference, Visibility visibility)
   : Scope(moduleScope)
{
   this->reference = reference;

   this->visibility = visibility;
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(NamespaceScope* parent, ref_t reference, Visibility visibility)
   : SourceScope(parent, reference, visibility)
{
   staticOne = false;
   preloaded = false;
}

//ObjectInfo Compiler::SymbolScope :: mapTerminal(ident_t identifier)
//{
//   return Scope::mapTerminal(identifier);
//}

void Compiler::SymbolScope :: save()
{
   // save class meta data
   MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false), 0);
   info.save(&metaWriter);
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(Scope* parent, ref_t reference, Visibility visibility)
   : SourceScope(parent, reference, visibility)
{
   info.header.parentRef =  moduleScope->superReference;
   info.header.flags = elStandartVMT;
   info.header.count = 0;
   info.header.classRef = 0;
   info.header.staticSize = 0;
   info.size = 0;

   extensionClassRef = 0;
   stackSafe = false;
   classClassMode = false;
   abstractMode = false;
   abstractBaseMode = false;
   withInitializers = false;
   extensionDispatcher = false;
}

ObjectInfo Compiler::ClassScope :: mapField(ident_t terminal, EAttr scopeMode)
{
   int offset = info.fields.get(terminal);
   if (offset >= 0) {
      bool readOnlyMode = test(info.header.flags, elReadOnlyRole) && !EAttrs::test(scopeMode, INITIALIZER_SCOPE);
      ClassInfo::FieldInfo fieldInfo = info.fieldTypes.get(offset);
      if (test(info.header.flags, elStructureRole)) {
         return ObjectInfo(readOnlyMode ? okReadOnlyFieldAddress : okFieldAddress, offset, fieldInfo.value1, fieldInfo.value2, 0);
      }
      else return ObjectInfo(readOnlyMode ? okReadOnlyField : okField, offset, fieldInfo.value1, fieldInfo.value2, 0);
   }
   else if (offset == -2 && test(info.header.flags, elDynamicRole)) {
      auto fieldInfo = info.fieldTypes.get(-1);

      return ObjectInfo(okSelfParam, 1, fieldInfo.value1, fieldInfo.value2, (ref_t)-2);
   }
   else {
      ClassInfo::FieldInfo staticInfo = info.statics.get(terminal);
      if (staticInfo.value1 != 0) {
         if (!isSealedStaticField(staticInfo.value1)) {
            //ref_t val = info.staticValues.get(staticInfo.value1);
            //            if (val != mskStatRef) {
            if (classClassMode) {
               return ObjectInfo(okClassStaticConstantField, staticInfo.value1, staticInfo.value2);
            }
            else return ObjectInfo(okStaticConstantField, staticInfo.value1, staticInfo.value2);
            //            }
         }
         else if(info.staticValues.exist(staticInfo.value1, mskConstantRef)) {
            // if it is a constant static sealed field
            if (classClassMode) {
               return ObjectInfo(okClassStaticConstantField, staticInfo.value1, staticInfo.value2);
            }
            else return ObjectInfo(okStaticConstantField, staticInfo.value1, staticInfo.value2);
         }
//
//         if (classClassMode) {
//            return ObjectInfo(okClassStaticField, 0, staticInfo.value2, 0, staticInfo.value1);
//         }
         /*else */return ObjectInfo(okStaticField, staticInfo.value1, staticInfo.value2, 0, 0);
//
      }
      return ObjectInfo();
   }
}

ObjectInfo Compiler::ClassScope :: mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
{
   if (!referenceOne && identifier.compare(SUPER_VAR)) {
      return ObjectInfo(okSuper, 1, info.header.parentRef, 0, stackSafe ? -1 : 0);
   }
   else {
      if (!referenceOne) {
         ObjectInfo fieldInfo = mapField(identifier, mode);
         if (fieldInfo.kind != okUnknown) {
            return fieldInfo;
         }
      }
      ObjectInfo retVal = Scope::mapTerminal(identifier, referenceOne, mode);
      if (retVal.kind == okClass) {
      }

      return retVal;
   }
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->reserved1 = this->reserved2 = 0;
   this->scopeMode = EAttr::eaNone;
//   this->rootToFree = 1;
   this->hints = 0;
   this->outputRef = INVALID_REF; // to indicate lazy load
   this->withOpenArg = false;
   this->classStacksafe = false;
   this->generic = false;
   this->extensionMode = false;
   this->multiMethod = false;
   this->functionMode = false;
   this->nestedMode = parent->getScope(Scope::ScopeLevel::slOwnerClass) != parent;
//   this->subCodeMode = false;
   this->abstractMethod = false;
   this->mixinFunction = false;
   this->embeddableRetMode = false;
   this->targetSelfMode = false;
   this->yieldMethod = false;
////   this->dispatchMode = false;
   this->constMode = false;

   this->preallocated = 0;
}

ObjectInfo Compiler::MethodScope :: mapSelf()
{
   if (extensionMode) {
      //COMPILER MAGIC : if it is an extension ; replace $self with self
      ClassScope* extensionScope = (ClassScope*)getScope(ScopeLevel::slClass);

      return ObjectInfo(okLocal, (ref_t)-1, extensionScope->extensionClassRef, 0, extensionScope->stackSafe ? -1 : 0);
   }
   else if (classStacksafe) {
      return ObjectInfo(okSelfParam, 1, getClassRef(), 0, (ref_t)-1);
   }
   else return ObjectInfo(okSelfParam, 1, getClassRef());
}

ObjectInfo Compiler::MethodScope :: mapGroup()
{
   return ObjectInfo(okParam, (size_t)-1);
}

ObjectInfo Compiler::MethodScope :: mapParameter(Parameter param, EAttr mode)
{
   int prefix = functionMode ? 0 : -1;

   if (withOpenArg && param.class_ref == V_ARGARRAY) {
      return ObjectInfo(okParams, prefix - param.offset, param.class_ref, param.element_ref, 0);
   }
   else if (param.class_ref != 0 && param.size != 0) {
      // if the parameter may be stack-allocated
      return ObjectInfo(okParam, prefix - param.offset, param.class_ref, param.element_ref, (ref_t)-1);
   }
   else if (param.class_ref == V_WRAPPER && !EAttrs::testany(mode, HINT_PROP_MODE | HINT_REFEXPR)) {
      return ObjectInfo(okParamField, prefix - param.offset, param.element_ref, 0, 0);
   }
   else return ObjectInfo(okParam, prefix - param.offset, param.class_ref, param.element_ref, 0);
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal, bool referenceOne, EAttr mode)
{
   if (!referenceOne && !EAttrs::test(mode, HINT_MODULESCOPE)) {
      Parameter param = parameters.get(terminal);
      if (param.offset >= 0) {
         return mapParameter(param, mode);
      }
      else {
         if (terminal.compare(SELF_VAR)) {
            if (targetSelfMode) {
               return mapGroup();
            }
            else if (functionMode || nestedMode) {
               return parent->mapTerminal(OWNER_VAR, false, mode | scopeMode);
            }
            else return mapSelf();
         }
         else if (!functionMode && (terminal.compare(GROUP_VAR))) {
            if (extensionMode) {
               return mapSelf();
            }
            else return mapGroup();
         }
//         else if (terminal.compare(RETVAL_VAR) && subCodeMode) {
//            ObjectInfo retVar = parent->mapTerminal(terminal, referenceOne, mode | scopeMode);
//            if (retVar.kind == okUnknown) {
//               InlineClassScope* closure = (InlineClassScope*)getScope(Scope::slClass);
//
//               retVar = closure->allocateRetVar();
//            }
//
//            return retVar;
//         }
      }
   }

   return Scope::mapTerminal(terminal, referenceOne, mode | scopeMode);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = this->reserved1 = 0;
   this->allocated2 = this->reserved2 = 0;
   this->genericMethod = false;
//   this->yieldMethod = false;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = this->reserved1 = 0;
   this->allocated2 = this->reserved2 = 0;
   this->genericMethod = parent->generic;
//   this->yieldMethod = parent->yieldMethod;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = parent->allocated1;
   this->reserved1 = parent->reserved1;
   this->allocated2 = parent->allocated2;
   this->reserved2 = parent->reserved2;
   this->genericMethod = parent->genericMethod;
//   this->yieldMethod = parent->yieldMethod;
}

//ObjectInfo Compiler::CodeScope :: mapGlobal(ident_t identifier)
//{
//   NamespaceScope* nsScope = (NamespaceScope*)getScope(Scope::slNamespace);
//
//   return nsScope->mapGlobal(identifier);
//}

ObjectInfo Compiler::CodeScope :: mapLocal(ident_t identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset) {
      if (genericMethod && identifier.compare(MESSAGE_VAR)) {
         return ObjectInfo(okMessage, local.offset, V_MESSAGE);
      }
      else if (local.size != 0) {
         return ObjectInfo(okLocalAddress, local.offset, local.class_ref, local.element_ref, 0);
      }
      else return ObjectInfo(okLocal, local.offset, local.class_ref, local.element_ref, 0);
   }
   else return ObjectInfo();
}

ObjectInfo Compiler::CodeScope :: mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
{
   if (!referenceOne) {
      if (!EAttrs::testany(mode, HINT_MODULESCOPE | HINT_PREVSCOPE)) {
         ObjectInfo info = mapLocal(identifier);
         if (info.kind != okUnknown)
            return info;
      }
      else mode = EAttrs::exclude(mode, HINT_PREVSCOPE);
   }

   return Scope::mapTerminal(identifier, referenceOne, mode);
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

// --- Compiler::ExprScope ---

Compiler::ExprScope :: ExprScope(CodeScope* parent)
   : Scope(parent), tempLocals(NOTFOUND_POS)
{
   tempAllocated1 = parent->allocated1;
   tempAllocated2 = parent->allocated2;

   ignoreDuplicates = false;
}

Compiler::ExprScope :: ExprScope(SourceScope* parent)
   : Scope(parent), tempLocals(NOTFOUND_POS)
{
   tempAllocated1 = -1;
   tempAllocated2 = -1;

   ignoreDuplicates = false;
}

int Compiler::ExprScope :: newTempLocal()
{
   CodeScope* codeScope = (CodeScope*)getScope(Scope::ScopeLevel::slCode);

   tempAllocated1++;
   if (tempAllocated1 > codeScope->reserved1)
      codeScope->reserved1 = tempAllocated1;

   return tempAllocated1;
}

inline int newLocalAddr(int allocated)
{
   return -2 - allocated;
}

int Compiler::ExprScope :: newTempLocalAddress()
{
   CodeScope* codeScope = (CodeScope*)getScope(Scope::ScopeLevel::slCode);

   int allocated = tempAllocated2;
   tempAllocated2++;
   if (tempAllocated2 > codeScope->reserved2)
      codeScope->reserved2 = tempAllocated2;

   return newLocalAddr(allocated);
}

ObjectInfo Compiler::ExprScope :: mapGlobal(ident_t identifier)
{
   NamespaceScope* nsScope = (NamespaceScope*)getScope(ScopeLevel::slNamespace);

   return nsScope->mapGlobal(identifier);
}

ObjectInfo Compiler::ExprScope :: mapMember(ident_t identifier)
{
   MethodScope* methodScope = (MethodScope*)getScope(Scope::ScopeLevel::slMethod);
   if (identifier.compare(SELF_VAR)) {
      if (methodScope != nullptr) {
         return methodScope->mapSelf();
      }
   }
   //else if (identifier.compare(GROUP_VAR)) {
   //   if (methodScope != NULL) {
   //      return methodScope->mapGroup();
   //   }
   //}
   else {
      ClassScope* classScope = (ClassScope*)getScope(Scope::ScopeLevel::slClass);
      if (classScope != nullptr) {
         if (methodScope != nullptr) {
            return classScope->mapField(identifier, methodScope->scopeMode);
         }
         else return classScope->mapField(identifier, INITIALIZER_SCOPE);
      }
   }
   return ObjectInfo();
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

ObjectInfo Compiler::ResendScope :: mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
{
   if (!withFrame && (identifier.compare(SELF_VAR) || identifier.compare(GROUP_VAR)))
   {
      return ObjectInfo();
   }

   ObjectInfo info = ExprScope::mapTerminal(identifier, referenceOne, mode);
   if (consructionMode && isField(info.kind)) {
      return ObjectInfo();
   }
   else return info;
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(ExprScope* owner, ref_t reference)
   : ClassScope(owner, reference, Visibility::Internal), outers(Outer())//, outerFieldTypes(ClassInfo::FieldInfo(0, 0))
{
//   this->returningMode = false;
//   //this->parent = owner;
   info.header.flags |= elNestedClass;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapParent()
{
   Outer parentVar = outers.get(PARENT_VAR);
   // if owner reference is not yet mapped, add it
   if (parentVar.outerObject.kind == okUnknown) {
      parentVar.reference = info.fields.Count();
      ExprScope* exprScope = (ExprScope*)parent->getScope(Scope::ScopeLevel::slExpression);
      if (exprScope) {
         parentVar.outerObject = exprScope->mapMember(SELF_VAR);
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

      owner.outerObject = parent->mapTerminal(SELF_VAR, false, EAttr::eaNone);
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
      owner.outerObject = parent->mapTerminal(OWNER_VAR, false, EAttr::eaNone);
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

ObjectInfo Compiler::InlineClassScope :: mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
{
   if (identifier.compare(SUPER_VAR)) {
      return ObjectInfo(okSuper, 0, info.header.parentRef);
   }
   else if (identifier.compare(OWNER_VAR)) {
      Outer owner = mapOwner();

      // map as an outer field (reference to outer object and outer object field index)
      return ObjectInfo(okOuterSelf, owner.reference, owner.outerObject.reference, owner.outerObject.element, owner.outerObject.extraparam);
   }
   else {
      Outer outer = outers.get(identifier);

      // if object already mapped
      if (outer.reference != -1) {
         if (outer.outerObject.kind == okSuper) {
            return ObjectInfo(okSuper, 0, outer.reference);
         }
         else return ObjectInfo(okOuter, outer.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
      }
      else {
         outer.outerObject = parent->mapTerminal(identifier, referenceOne, mode);
         switch (outer.outerObject.kind) {
            case okReadOnlyField:
            case okField:
//            case okStaticField:
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
               /*else if (outer.outerObject.kind == okOuterStaticField) {
                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }
               else if (outer.outerObject.kind == okStaticField) {
                  return ObjectInfo(okOuterStaticField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.extraparam);
               }*/
               else if (outer.outerObject.kind == okReadOnlyField) {
                  return ObjectInfo(okOuterReadOnlyField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.param);
               }
               else return ObjectInfo(okOuterField, owner.reference, outer.outerObject.reference, outer.outerObject.element, outer.outerObject.param);
            }
            case okParam:
            case okLocal:
            case okOuter:
            case okSuper:
            case okSelfParam:
            case okLocalAddress:
            case okFieldAddress:
            case okReadOnlyFieldAddress:
            case okOuterField:
            //case okOuterStaticField:
            case okOuterSelf:
            case okParams:
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
               ObjectInfo fieldInfo = mapField(identifier, EAttr::eaNone);
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
               InlineClassScope* closure = (InlineClassScope*)parent->getScope(Scope::ScopeLevel::slClass);
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

//ObjectInfo Compiler::InlineClassScope :: allocateRetVar()
//{
//   returningMode = true;
//
//   Outer outer;
//   outer.reference = info.fields.Count();
//   outer.outerObject = ObjectInfo(okNil, (ref_t)-1);
//
//   outers.add(RETVAL_VAR, outer);
//   mapKey(info.fields, RETVAL_VAR, (int)outer.reference);
//
//   return ObjectInfo(okOuter, outer.reference);
//}

// --- Compiler ---

Compiler :: Compiler(_CompilerLogic* logic)
   : _sourceRules(SNodePattern(lxNone))
{
   _optFlag = 0;
   _autoSystemImport = false;

   this->_logic = logic;

   ByteCodeCompiler::loadOperators(_operators);
}

void Compiler :: writeMessageInfo(SNode node, _ModuleScope& scope, ref_t messageRef)
{
   ref_t actionRef, flags;
   int argCount;
   decodeMessage(messageRef, actionRef, argCount, flags);

   IdentifierString name;
   ref_t signature = 0;
   name.append(scope.module->resolveAction(actionRef, signature));

   name.append('[');
   name.appendInt(argCount);
   name.append(']');

   node.appendNode(lxMessageVariable, name);
}

void Compiler :: loadRules(StreamReader* optimization)
{
   _rules.load(optimization);
}

void Compiler :: loadSourceRules(StreamReader* optimization)
{
   _sourceRules.load(optimization);
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

bool Compiler :: calculateRealOp(int operation_id, double arg1, double arg2, double& retVal)
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
      default:
         return false;
   }

   return true;
}

ref_t Compiler :: resolveConstantObjectReference(_CompileScope& scope, ObjectInfo object)
{
   switch (object.kind) {
      case okIntConstant:
         return scope.moduleScope->intReference;
      //case okSignatureConstant:
      //   return scope.moduleScope->signatureReference;
      default:
         return resolveObjectReference(scope, object, false);
   }
}

ref_t Compiler :: resolveObjectReference(_CompileScope& scope, ObjectInfo object, bool noPrimitivesMode, bool unboxWapper)
{
   ref_t retVal = object.reference;
   ref_t elementRef = object.element;
   /*if (object.kind == okSelfParam) {
      if (object.extraparam == (ref_t)-2) {
         // HOTFIX : to return the primitive array
         retVal = object.reference;
      }
      else retVal = scope.getClassRefId(false);
   }
   else */if (unboxWapper && object.reference == V_WRAPPER) {
      elementRef = 0;
      retVal = object.element;
   }
   else if (object.kind == okNil) {
      return V_NIL;
   }

   if (noPrimitivesMode && isPrimitiveRef(retVal)) {
      return resolvePrimitiveReference(scope, retVal, elementRef, false);
   }
   else return retVal;

}

inline void writeClassNameInfo(SNode& node, _Module* module, ref_t reference)
{
   ident_t className = module->resolveReference(reference);
   if (isTemplateWeakReference(className)) {
      // HOTFIX : save weak template-based class name directly
      node.appendNode(lxClassName, className);
   }
   else {
      IdentifierString fullName(module->Name(), className);

      node.appendNode(lxClassName, fullName.c_str());
   }
}

void Compiler :: declareCodeDebugInfo(SNode node, MethodScope& scope)
{
   node.appendNode(lxSourcePath, scope.saveSourcePath(_writer, node.identifier()));
}

void Compiler :: declareProcedureDebugInfo(SNode node, MethodScope& scope, bool withSelf/*, bool withTargetSelf*/)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   // declare built-in variables
   if (withSelf) {
      if (scope.classStacksafe) {
         SNode selfNode = node.appendNode(lxBinarySelf, 1);

         writeClassNameInfo(selfNode, scope.module, scope.getClassRef());
      }
      else node.appendNode(lxSelfVariable, 1);
   }

//   if (withTargetSelf)
//      writer.appendNode(lxSelfVariable, -1);

   writeMessageInfo(node, *moduleScope, scope.message);

   int prefix = scope.functionMode ? 0 : -1;

   SNode current = node.firstChild();
   // method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter/* || current == lxIdentifier*/) {
         SNode identNode = current.findChild(lxNameAttr);
         if (identNode != lxNone) {
            identNode = identNode.firstChild(lxTerminalMask);
         }
//         else identNode = current.firstChild(lxTerminalMask);

         if (identNode != lxNone) {
            Parameter param = scope.parameters.get(identNode.identifier());
            if (param.offset != -1) {
               SNode varNode;
               if (param.class_ref == V_ARGARRAY) {
                  varNode = node.appendNode(lxParamsVariable);
               }
               else if (param.class_ref == moduleScope->intReference) {
                  varNode = node.appendNode(lxIntVariable);
               }
               else if (param.class_ref == moduleScope->longReference) {
                  varNode = node.appendNode(lxLongVariable);
               }
               else if (param.class_ref == moduleScope->realReference) {
                  varNode = node.appendNode(lxReal64Variable);
               }
               else if (param.size != 0 && param.class_ref != 0) {
                  ref_t classRef = param.class_ref;
                  if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {

                     varNode = node.appendNode(lxBinaryVariable);
                     writeClassNameInfo(node, scope.module, classRef);
                  }
                  else varNode = node.appendNode(lxVariable);
               }
               else varNode = node.appendNode(lxVariable);

               varNode.appendNode(lxLevel, prefix - param.offset);

               IdentifierString name(identNode.identifier());
               varNode.appendNode(lxIdentifier, name.c_str());
            }
         }
      }
      else if (current == lxSourcePath) {
         current.setArgument(scope.saveSourcePath(_writer, current.identifier()));
      }

      current = current.nextNode();
   }
}

inline SNode findIdentifier(SNode current)
{
   if (current.firstChild(lxTerminalMask))
      return current.firstChild(lxTerminalMask);

   return current;
}

void Compiler :: importCode(SNode node, Scope& scope, ref_t functionRef, ref_t message)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(scope.module->resolveReference(functionRef));
   virtualReference.append('.');

   int argCount;
   ref_t actionRef, flags;
   decodeMessage(message, actionRef, argCount, flags);

//   // HOTFIX : include self as a parameter
//   paramCount++;

   size_t signIndex = virtualReference.Length();
   virtualReference.append('0' + (char)argCount);
   if (test(message, STATIC_MESSAGE)) {
      virtualReference.append(scope.module->Name());
      virtualReference.append("$");
   }

   if (test(flags, VARIADIC_MESSAGE))
      virtualReference.append("params#");

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
      node.set(lxImporting, _writer.registerImportInfo(section, api, moduleScope->module));
   }
   else scope.raiseError(errInvalidLink, findIdentifier(node.findChild(lxInternalRef)));
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreFields, bool ignoreSealed)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.header.classRef;

   // get module reference
   ref_t moduleRef = 0;
   _Module* module = moduleScope->loadReferenceModule(parentRef, moduleRef);

   if (module == NULL || moduleRef == 0)
      return InheritResult::irUnsuccessfull;

   // load parent meta data
   _Memory* metaData = module->mapSection(moduleRef | mskMetaRDataRef, true);
   if (metaData != NULL) {
      MemoryReader reader(metaData);
      // import references if we inheriting class from another module
      if (moduleScope->module != module) {
         ClassInfo copy;
         copy.load(&reader);

         moduleScope->importClassInfo(copy, scope.info, module, false, true, ignoreFields);
      }
      else {
         scope.info.load(&reader, false, ignoreFields);

         // mark all methods as inherited
         // private methods are not inherited
         ClassInfo::MethodMap::Iterator it = scope.info.methods.start();
         while (!it.Eof()) {
            ref_t message = it.key();

            (*it) = false;
            it++;

            if (test(message, STATIC_MESSAGE)) {
               scope.info.methods.exclude(message);
               scope.info.methodHints.exclude(Attribute(message, maHint));
               scope.info.methodHints.exclude(Attribute(message, maReference));
            }
         }
      }

      //// inherit static field values
      //auto staticValue_it = scope.info.staticValues.start();
      //while (!staticValue_it.Eof()) {
      //   if (staticValue_it.key() < MAX_ATTR_INDEX) {
      //      // NOTE : the built-in attributes will be set later
      //      ref_t ref = *staticValue_it;
      //      if (ref != mskStatRef) {
      //         int mask = ref & mskAnyRef;
      //         IdentifierString name(module->resolveReference(scope.reference));
      //         name.append(STATICFIELD_POSTFIX);

      //         ref_t newRef = scope.moduleScope->mapAnonymous(name.c_str());

      //         *staticValue_it = newRef | mask;
      //      }
      //   }

      //   staticValue_it++;
      //}

      // meta attributes are not directly inherited
      scope.info.mattributes.clear();

      if (!ignoreSealed && test(scope.info.header.flags, elFinal)) {
         // COMPILER MAGIC : if it is a unsealed nested class inheriting its owner
         if (!test(scope.info.header.flags, elSealed) && test(flagCopy, elNestedClass)) {
            ClassScope* owner = (ClassScope*)scope.getScope(Scope::ScopeLevel::slOwnerClass);
            if (owner->classClassMode && scope.info.header.classRef == owner->reference) {
               // HOTFIX : if it is owner class class - allow it as well
            }
            else if (owner->reference != parentRef) {
               return InheritResult::irSealed;
            }
         }
         else return InheritResult::irSealed;
      }

      // restore parent and flags
      scope.info.header.parentRef = parentRef;
      scope.info.header.classRef = classClassCopy;

      if (test(scope.info.header.flags, elAbstract)) {
         // exclude abstract flag
         scope.info.header.flags &= ~elAbstract;

         scope.abstractBaseMode = true;
      }

      scope.info.header.flags |= flagCopy;

      return InheritResult::irSuccessfull;
   }
   else return InheritResult::irUnsuccessfull;
}

void Compiler :: compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreFields)
{
   scope.info.header.parentRef = parentRef;
   InheritResult res = InheritResult::irSuccessfull;
   if (scope.info.header.parentRef != 0) {
      res = inheritClass(scope, scope.info.header.parentRef, ignoreFields, test(scope.info.header.flags, elVirtualVMT));
   }

   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   //if (res == irInvalid) {
   //   scope.raiseError(errInvalidParent/*, baseNode*/);
   //}
   /*else */if (res == InheritResult::irSealed) {
      scope.raiseError(errSealedParent, baseNode);
   }
   else if (res == InheritResult::irUnsuccessfull)
      scope.raiseError(errUnknownBaseClass, baseNode);
}

ref_t Compiler :: resolveTypeIdentifier(Scope& scope, SNode terminal, bool declarationMode, bool extensionAllowed)
{
   return resolveTypeIdentifier(scope, terminal.identifier(), terminal.type, declarationMode, extensionAllowed);
}

ref_t Compiler :: resolveTypeIdentifier(Scope& scope, ident_t terminal, LexicalType type,
   bool declarationMode, bool extensionAllowed)
{
   ObjectInfo identInfo;

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
   if (type == lxReference && isWeakReference(terminal)) {
      identInfo = ns->mapWeakReference(terminal, false);
   }
   else if (type == lxGlobalReference) {
      identInfo = ns->mapGlobal(terminal);
   }
   else identInfo = ns->mapTerminal(terminal, type == lxReference, EAttr::eaNone);

   switch (identInfo.kind) {
      case okClass:
      case okSingleton:
         return identInfo.param;
      case okSymbol:
         if (declarationMode)
            return identInfo.param;
      case okExtension:
         if (extensionAllowed)
            return identInfo.param;;
      default:
         return 0;
   }
}

bool isExtensionDeclaration(SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (current.argument == V_EXTENSION) {
            return true;
         }
      }
      else if (current == lxClassFlag) {
         if (current.argument == elExtension) {
            return true;
         }
      }
      else if (current != lxNameAttr)
         break;

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
//      while (node == lxAttribute)
//         // HOTFIX : skip attributes
//         node = node.nextNode();
//
//      if (node == lxTemplate || test(node.type, lxTerminalMask))
//         parentRef = resolveParentRef(node, scope, false);
//   }

   if (scope.info.header.parentRef == scope.reference) {
      if (parentRef != 0) {
         scope.raiseError(errInvalidSyntax, node);
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

void Compiler :: declareClassAttributes(SNode node, ClassScope& scope, bool visibilityOnly)
{
   int flags = scope.info.header.flags;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (!_logic->validateClassAttribute(value, scope.visibility)) {
            current.setArgument(0); // HOTFIX : to prevent duplicate warnings

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         else if (!visibilityOnly) {
            current.set(lxClassFlag, value);
            if (value != 0 && test(flags, value)) {
               scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateAttribute, current);
            }
            else if (test(value, elAbstract))
               scope.abstractMode = true;

            flags |= value;
         }
      }
      else if (current == lxType) {
         scope.raiseError(errInvalidSyntax, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: validateType(Scope& scope, SNode current, ref_t typeRef, bool ignoreUndeclared, bool allowType)
{
   if (!typeRef)
      scope.raiseError(errUnknownClass, current);

   if (!_logic->isValidType(*scope.moduleScope, typeRef, ignoreUndeclared, allowType))
      scope.raiseError(errInvalidType, current);
}

//ref_t Compiler :: resolveTypeAttribute(Scope& scope, SNode node, bool declarationMode)
//{
//   ref_t typeRef = 0;
//
//   SNode current = node.firstChild();
//   if (current == lxArrayType) {
//      typeRef = resolvePrimitiveArray(scope, resolveTypeAttribute(scope, current, declarationMode), declarationMode);
//   }
//   else if (current == lxTarget) {
//      if (current.argument == V_TEMPLATE) {
//         typeRef = resolveTemplateDeclaration(current, scope, declarationMode);
//      }
//      else typeRef = current.argument != 0 ? current.argument : resolveImplicitIdentifier(scope, current.firstChild(lxTerminalMask));
//   }
//
//   validateType(scope, node, typeRef, declarationMode);
//
//   return typeRef;
//}

void Compiler :: declareSymbolAttributes(SNode node, SymbolScope& scope, bool declarationMode, bool ignoreType)
{
   bool constant = false;
   ref_t outputRef = 0;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (!_logic->validateSymbolAttribute(value, constant, scope.staticOne, scope.preloaded,
            scope.visibility))
         {
            current.setArgument(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current.compare(lxType, lxArrayType) && !ignoreType) {
         // HOTFIX : do not resolve the output type for identifier declaration mode
         outputRef = resolveTypeAttribute(current, scope, declarationMode, false);
      }

      current = current.nextNode();
   }

   scope.info.exprRef = outputRef;
   if (constant)
      scope.info.type = SymbolExpressionInfo::Type::Constant;
}

int Compiler :: resolveSize(SNode node, Scope& scope)
{
   if (node == lxInteger) {
      return node.identifier().toInt();
   }
   else if (node == lxHexInteger) {
      return node.identifier().toInt(16);
   }
   else {
      scope.raiseError(errInvalidSyntax, node);

      return 0;
   }
}

void Compiler :: declareFieldAttributes(SNode node, ClassScope& scope, FieldAttributes& attrs)
{
   bool inlineArray = false;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateFieldAttribute(value, attrs)) {
            if (value == lxStaticAttr) {
               attrs.isStaticField = true;
            }
            else if (value == -1) {
               // ignore if constant / sealed attribute was set
            }
            else if (!value && isPrimitiveRef(current.argument)) {
               if (current.argument == V_STRING) {
                  // if it is an inline array attribute
                  inlineArray = true;
               }
               // if it is a primitive type
               else attrs.fieldRef = current.argument;
            }
            else scope.raiseError(errInvalidHint, node);
         }
         else scope.raiseError(errInvalidHint, current);
      }
      else if (current.compare(lxType, lxArrayType)) {
         if (attrs.fieldRef == 0) {
            if (inlineArray) {
               // if it is an inline array - it should be compiled differently
               if (current == lxArrayType) {
                  attrs.fieldRef = resolveTypeAttribute(current.firstChild(), scope, false, false);
                  attrs.size = -1;
               }
               else scope.raiseError(errInvalidHint, current);
            }
            // NOTE : the field type should be already declared only for the structure
            else {
               attrs.fieldRef = resolveTypeAttribute(current, scope,
                  !test(scope.info.header.flags, elStructureRole), false);

               attrs.isArray = current == lxArrayType;
            }
         }
         else scope.raiseError(errInvalidHint, node);
      }
      else if (current == lxSize) {
         if (attrs.size == 0) {
            if (current.argument) {
               attrs.size = current.argument;
            }
            else attrs.size = resolveSize(current.firstChild(lxTerminalMask), scope);
         }
         else scope.raiseError(errInvalidHint, node);
      }
//      else if (current == lxMessage) {
//         // COMPILER MAGIC : if the field should be mapped to the message
//         attrs.messageRef = current.argument;
//         attrs.messageAttr = current.findChild(lxAttribute).argument;
//      }

      current = current.nextNode();
   }

   //HOTFIX : recognize raw data
   if (attrs.fieldRef == V_INTBINARY) {
      switch (attrs.size) {
         case 1:
         case 2:
         case 4:
            // treat it like dword
            attrs.fieldRef = V_INT32;
            break;
         case 8:
            // treat it like qword
            attrs.fieldRef = V_INT64;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
   else if (attrs.fieldRef == V_BINARY) {
      switch (attrs.size) {
         case 4:
            // treat it like dword
            attrs.fieldRef = V_DWORD;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
   else if (attrs.fieldRef == V_PTRBINARY) {
      switch (attrs.size) {
         case 4:
            // treat it like dword
            attrs.fieldRef = V_PTR32;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
   else if (attrs.fieldRef == V_MESSAGE || attrs.fieldRef == V_SUBJECT) {
      if (attrs.size == 8 && attrs.fieldRef == V_MESSAGE) {
         attrs.fieldRef = V_EXTMESSAGE;
      }
      else if (attrs.size != 4) {
         scope.raiseError(errInvalidHint, node);
      }
   }
   else if (attrs.fieldRef == V_FLOAT) {
      switch (attrs.size) {
         case 8:
            // treat it like dword
            attrs.fieldRef = V_REAL64;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
}

void Compiler :: compileSwitch(SNode node, ExprScope& scope)
{
   SNode targetNode = node.firstChild();

   bool immMode = true;
   int localOffs = 0;
   ObjectInfo loperand;
   if (targetNode == lxExpression) {
      immMode = false;

      localOffs = scope.newTempLocal();

      loperand = compileExpression(targetNode, scope, 0, EAttr::eaNone);

      targetNode.injectAndReplaceNode(lxAssigning);
      targetNode.insertNode(lxLocal, localOffs);
   }

   SNode current = node.findChild(lxOption, lxElse);
   while (current == lxOption) {
      SNode optionNode = current.injectNode(lxExpression);
      SNode blockNode = optionNode.firstChild(lxObjectMask);

      // find option value
      SNode exprNode = optionNode.firstChild();
      exprNode.injectAndReplaceNode(lxExpression);

      int operator_id = current.argument;

      if (!immMode) {
         exprNode.insertNode(lxLocal, localOffs);
      }
      else {
         SNode localNode = SyntaxTree::insertNodeCopy(targetNode, exprNode);

         if (localNode != lxExpression) {
            localNode.injectAndReplaceNode(lxExpression);
         }
         loperand = compileExpression(localNode, scope, 0, EAttr::eaNone);
      }

      ObjectInfo roperand = mapTerminal(exprNode.lastChild(), scope, EAttr::eaNone);
      ObjectInfo operationInfo = compileOperator(exprNode, scope, operator_id, 2, loperand, roperand, ObjectInfo(), EAttr::eaNone);

      ObjectInfo retVal;
      compileBranchingOp(blockNode, scope, HINT_SWITCH, IF_OPERATOR_ID, operationInfo, retVal);

      current = current.nextNode();
   }

   if (current == lxElse) {
      compileSubCode(current, scope, false);

      //CodeScope subScope(&scope);
      //SNode thenCode = current.findSubNode(lxCode);

      //SNode statement = thenCode.firstChild(lxObjectMask);
      //if (statement.nextNode() != lxNone || statement == lxEOF) {
      //   compileCode(writer, thenCode, subScope);
      //}
      //// if it is inline action
      //else compileRetExpression(writer, statement, scope, EAttr::eaNone);

      //// preserve the allocated space
      //scope.level = subScope.level;
   }
}

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

LexicalType Compiler :: declareVariableType(CodeScope& scope, ObjectInfo& variable, ClassInfo& localInfo, int size, bool binaryArray,
   int& variableArg, ident_t& className)
{
   LexicalType variableType = lxVariable;

   if (size > 0) {
      switch (localInfo.header.flags & elDebugMask) {
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
               if (binaryArray)
                  variableArg = size;

               if (variable.reference != 0 && !isPrimitiveRef(variable.reference)) {
                  className = scope.moduleScope->module->resolveReference(variable.reference);
               }
            }
            break;
      }
   }
   else {
   }

   return variableType;
}

void Compiler :: declareVariable(SNode& terminal, ExprScope& scope, ref_t typeRef/*, bool dynamicArray*/, bool canBeIdle)
{
   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   IdentifierString identifier(terminal.identifier());
   ident_t className = NULL;
   LexicalType variableType = lxVariable;
   int variableArg = 0;
   int size = /*dynamicArray ? -1 : */0;

   // COMPILER MAGIC : if it is a fixed-sized array
   SNode opNode = terminal.nextNode();
   if (opNode == lxArrOperator/* && opNode.argument == REFER_OPERATOR_ID*/) {
      if (size && opNode.nextNode() != lxNone)
         scope.raiseError(errInvalidSyntax, terminal);

      SNode sizeExprNode = opNode.nextNode();

      size = resolveArraySize(sizeExprNode, scope);

      // HOTFIX : remove the size attribute
      opNode = lxIdle;
      sizeExprNode = lxIdle;

      opNode = sizeExprNode.nextNode();
   }
   ObjectInfo variable;
   variable.reference = typeRef;

   if (size != 0 && variable.reference != 0) {
      if (!isPrimitiveRef(variable.reference)) {
         // if it is a primitive array
         variable.element = variable.reference;
         variable.reference = _logic->definePrimitiveArray(*scope.moduleScope, variable.element, true);
      }
      else scope.raiseError(errInvalidHint, terminal);
   }

   ClassInfo localInfo;
   bool binaryArray = false;
   if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.reference))
      scope.raiseError(errUnknownVariableType, terminal);

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

   variable.kind = okLocal;

   if (size > 0) {
      if (scope.tempAllocated2 > codeScope->allocated2) {
         codeScope->allocated2 = scope.tempAllocated2;
         if (codeScope->allocated2 > codeScope->reserved2)
            codeScope->reserved2 = codeScope->allocated2;
      }

      if (!allocateStructure(*codeScope, size, binaryArray, variable))
         scope.raiseError(errInvalidOperation, terminal);

      // make the reservation permanent
      if (codeScope->reserved2 < codeScope->allocated2)
         codeScope->reserved2 = codeScope->allocated2;

      // take into account allocated space if requiered
      if (scope.tempAllocated2 < codeScope->allocated2)
         scope.tempAllocated2 = codeScope->allocated2;
   }
   else {
      if (size < 0) {
         size = 0;
      }

      if (scope.tempAllocated1 > codeScope->allocated1) {
         codeScope->allocated1 = scope.tempAllocated1;
         if (codeScope->allocated1 > codeScope->reserved1)
            codeScope->reserved1 = codeScope->allocated1;
      }

      variable.param = codeScope->newLocal();

      // take into account allocated space if requiered
      if (scope.tempAllocated1 < codeScope->allocated1)
         scope.tempAllocated1 = codeScope->allocated1;
   }

   variableType = declareVariableType(*codeScope, variable, localInfo, size, binaryArray, variableArg, className);

   if (!codeScope->locals.exist(identifier)) {
      codeScope->mapLocal(identifier, variable.param, variable.reference, variable.element, size);

      // injecting variable label
      SNode rootNode = findRootNode(terminal, lxNewFrame, lxCode, lxCodeExpression);

      SNode varNode = rootNode.prependSibling(variableType, variableArg);
      varNode.appendNode(lxLevel, variable.param);
      varNode.appendNode(lxIdentifier, identifier);

      if (!emptystr(className)) {
         if (isWeakReference(className)) {
            if (isTemplateWeakReference(className)) {
               // HOTFIX : save weak template-based class name directly
               varNode.appendNode(lxClassName, className);
            }
            else {
               IdentifierString fullName(scope.module->Name(), className);

               varNode.appendNode(lxClassName, fullName);
            }
         }
         else varNode.appendNode(lxClassName, className);
      }
   }
   else scope.raiseError(errDuplicatedLocal, terminal);

   if (opNode == lxNone && canBeIdle) {
      // HOTFIX : remove the variable if the statement contains only a declaration
      terminal = lxIdle;
   }
}

//void Compiler :: writeTerminalInfo(SyntaxWriter& writer, SNode terminal)
//{
//   SyntaxTree::copyNode(writer, lxRow, terminal);
//   SyntaxTree::copyNode(writer, lxCol, terminal);
//   SyntaxTree::copyNode(writer, lxLength, terminal);
//
//   //ident_t ident = terminal.identifier();
//   //if (ident)
//   //   writer.appendNode(lxTerminal, terminal.identifier());
//}

//inline void insertNodeChild(SNode target, SNode terminal, LexicalType type)
//{
//   SNode current = terminal.findChild(type);
//   if (current != lxNone)
//      target.insertNode(current.type, current.argument);
//}
//
//inline void insertTerminalInfo(SNode target, SNode terminal)
//{
//   if (terminal != lxNone) {
//      SNode current = target.insertNode(terminal.type);
//      insertNodeChild(current, terminal, lxRow);
//      insertNodeChild(current, terminal, lxCol);
//      insertNodeChild(current, terminal, lxLength);
//   }
//}

//inline void writeTarget(SyntaxWriter& writer, ref_t targetRef, ref_t elementRef)
//{
//   if (targetRef)
//      writer.appendNode(lxTarget, targetRef);
//
//   if (isPrimitiveRef(targetRef) && elementRef)
//      writer.appendNode(lxElement, elementRef);
//}

int Compiler :: defineFieldSize(Scope& scope, int offset)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

   ClassInfo::FieldMap::Iterator it = retrieveIt(classScope->info.fields.start(), offset);
   it++;
   if (!it.Eof()) {
      return *it - offset;
   }
   else return classScope->info.size - offset;
}

void Compiler :: setParamFieldTerminal(SNode& node, ExprScope&, ObjectInfo object, EAttr, LexicalType type)
{
   node.set(lxFieldExpression, 0);
   node.appendNode(type, object.param);
   node.appendNode(lxField, 0);
}

void Compiler :: appendBoxingInfo(SNode node, _CompileScope& scope, ObjectInfo object, bool noUnboxing,
   int fixedSize, ref_t targetRef)
{
   // if the parameter may be stack-allocated
   bool variable = false;
   int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, object.element, variable);

   if (fixedSize)
      // use fixed size (for fixed-sized array fields)
      // note that we still need to execute defineStructSizeVariable to set variable bool value
      size = fixedSize;

   node.appendNode(lxType, targetRef);
   if (isPrimitiveRef(targetRef))
      node.appendNode(lxElementType, object.element);

   node.appendNode(lxSize, size);
   if (variable && !noUnboxing)
      node.setArgument(INVALID_REF);
}

void Compiler :: setSuperTerminal(SNode& node, ExprScope& scope, ObjectInfo object, EAttr mode, LexicalType type)
{
   node.set(type, object.param);
   if (object.extraparam == -1 && !EAttrs::test(mode, HINT_NOBOXING)) {
      node.injectAndReplaceNode(lxCondBoxableExpression);

      appendBoxingInfo(node, scope, object, EAttrs::test(mode, HINT_NOUNBOXING), 0,
         scope.getClassRefId());
   }
}

void Compiler :: setParamTerminal(SNode& node, ExprScope& scope, ObjectInfo object, EAttr mode, LexicalType type)
{
   node.set(type, object.param);

   if (object.extraparam == -1 && !EAttrs::test(mode, HINT_NOBOXING)) {
      node.injectAndReplaceNode(lxCondBoxableExpression);

      appendBoxingInfo(node, scope, object, EAttrs::test(mode, HINT_NOUNBOXING), 0,
         resolveObjectReference(scope, object, false));
   }
}

void Compiler :: setParamsTerminal(SNode& node, _CompileScope&, ObjectInfo object, EAttr, ref_t wrapRef)
{
   node.set(lxBlockLocalAddr, object.param);

   node.injectAndReplaceNode(lxArgBoxableExpression);
   node.appendNode(lxType, wrapRef);

   //         writer.newNode(lxArgBoxing, 0);
   //         writer.appendNode(lxBlockLocalAddr, object.param);
   //         writer.appendNode(lxTarget, r);
   //         if (EAttrs::test(mode, HINT_DYNAMIC_OBJECT))
   //            writer.appendNode(lxBoxingRequired);
}

void Compiler :: setVariableTerminal(SNode& node, _CompileScope& scope, ObjectInfo object, EAttr mode, LexicalType type, int fixedSize)
{
   node.set(type, object.param);

   if (!EAttrs::test(mode, HINT_NOBOXING)) {
      node.injectAndReplaceNode(lxBoxableExpression);

      appendBoxingInfo(node, scope, object, EAttrs::test(mode, HINT_NOUNBOXING), fixedSize,
         resolveObjectReference(scope, object, false));
   }
}

ObjectInfo Compiler :: mapClassSymbol(Scope& scope, int classRef)
{
   if (classRef) {
      ObjectInfo retVal(okClass);
      retVal.param = classRef;

      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, classRef, true);
      retVal.reference = info.header.classRef;

      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
      if (classScope != nullptr && classScope->reference == retVal.reference)
         retVal.kind = okClassSelf;

      return retVal;
   }
   else return ObjectInfo(okUnknown);
}

ObjectInfo Compiler :: compileTypeSymbol(SNode node, ExprScope& scope, EAttr mode)
{
   ObjectInfo retVal = mapClassSymbol(scope, resolveTemplateDeclaration(node, scope, false));

   recognizeTerminal(node, retVal, scope, mode);

   return retVal;
}

ObjectInfo Compiler :: compileYieldExpression(SNode objectNode, ExprScope& scope, EAttr mode)
{
   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);
   MethodScope* methodScope = (MethodScope*)codeScope->getScope(Scope::ScopeLevel::slMethod);
   int index = methodScope->getAttribute(maYieldContext);
   int index2 = methodScope->getAttribute(maYieldLocals);

   EAttrs objectMode(mode);
   objectMode.include(HINT_NOPRIMITIVES);

   objectNode.injectAndReplaceNode(lxSeqExpression);
   SNode retExprNode = objectNode.firstChild(lxObjectMask);

   // save context
   SNode exprNode = objectNode.insertNode(lxExpression);
   SNode copyNode = exprNode.appendNode(lxCopying, codeScope->reserved2);
   SNode fieldNode = copyNode.appendNode(lxFieldExpression);
   fieldNode.appendNode(lxSelfLocal, 1);
   fieldNode.appendNode(lxField, index);
   fieldNode.appendNode(lxField, 1);
   copyNode.appendNode(lxLocalAddress, -2);

   // save locals
   SNode expr2Node = objectNode.insertNode(lxExpression);
   SNode copy2Node = expr2Node.appendNode(lxCopying, codeScope->reserved1 - methodScope->preallocated);
   SNode field2Node = copy2Node.appendNode(lxFieldExpression);
   field2Node.appendNode(lxSelfLocal, 1);
   field2Node.appendNode(lxField, index2);
   copy2Node.appendNode(lxLocalAddress, methodScope->preallocated);

   // HOTFIX : reset yield locals field on yield return to mark mg->yg reference
   SNode expr3Node = objectNode.insertNode(lxAssigning);
   SNode src3 = expr3Node.appendNode(lxFieldExpression);
   src3.appendNode(lxSelfLocal, 1);
   src3.appendNode(lxField, index2);
   SNode dst3 = expr3Node.appendNode(lxFieldExpression);
   dst3.appendNode(lxSelfLocal, 1);
   dst3.appendNode(lxField, index2);

   ObjectInfo retVal;
   if (codeScope->withEmbeddableRet()) {
      retVal = scope.mapTerminal(SELF_VAR, false, EAttr::eaNone);

      // HOTFIX : the node should be compiled as returning expression
      LexicalType ori = objectNode.type;
      objectNode = lxReturning;
      compileEmbeddableRetExpression(retExprNode, scope);
      objectNode = ori;

      recognizeTerminal(objectNode, retVal, scope, HINT_NODEBUGINFO | HINT_NOBOXING);

      retExprNode.set(lxYieldReturning, index);
   }
   else {
      //writer.appendNode(lxBreakpoint, dsStep);
      retVal = compileExpression(retExprNode, scope, 0, objectMode);

      retExprNode.injectAndReplaceNode(lxYieldReturning, index);
   }

   return retVal;
}

ObjectInfo Compiler :: compileMessageReference(SNode terminal, ExprScope& scope)
{
   ObjectInfo retVal;
   IdentifierString message;
   int argCount = 0;

   SNode paramNode = terminal.nextNode();
   bool invalid = true;
   if (paramNode == lxArrOperator && paramNode.argument == REFER_OPERATOR_ID) {
      if (isSingleStatement(paramNode.nextNode())) {
         ObjectInfo sizeInfo = mapTerminal(paramNode.nextNode().firstChild(lxTerminalMask), scope, HINT_VIRTUALEXPR);
         if (sizeInfo.kind == okIntConstant) {
            argCount = sizeInfo.extraparam;
            invalid = false;
         }
      }
   }

   if (invalid)
      scope.raiseError(errNotApplicable, terminal);

   // HOTFIX : prevent further compilation of the expression
   paramNode = lxIdle;

   if (terminal == lxIdentifier) {
      message.append('0' + (char)argCount);
      message.append(terminal.identifier());

      retVal.kind = okMessageConstant;

      retVal.reference = V_MESSAGE;
   }
   else if (terminal == lxType) {
      SNode typeNode = terminal.findChild(lxType);
      if (typeNode.nextNode() != lxNone)
         scope.raiseError(errNotApplicable, terminal);

      ref_t extensionRef = resolveTypeAttribute(typeNode, scope, false, true);

      message.append(scope.moduleScope->module->resolveReference(extensionRef));
      message.append('.');
      message.append('0' + (char)argCount);
      message.append(terminal.firstChild(lxTerminalMask).identifier());

      retVal.kind = okExtMessageConstant;
      retVal.param = scope.moduleScope->module->mapReference(message);
      retVal.reference = V_EXTMESSAGE;
   }

   retVal.param = scope.moduleScope->module->mapReference(message);

   return retVal;
}

ObjectInfo Compiler :: compileSubjectReference(SNode terminal, ExprScope& scope, EAttr)
{
   ObjectInfo retVal;
   IdentifierString messageName;
   if (terminal == lxIdentifier) {
      ident_t name = terminal.identifier();
      messageName.copy(name);
   }
   else if (terminal == lxSubjectRef) {
      ref_t dummy = 0;
      ident_t name = scope.module->resolveAction(terminal.argument, dummy);
      messageName.copy(name);
   }

   retVal.kind = okMessageNameConstant;
   retVal.param = scope.moduleScope->module->mapReference(messageName);
   retVal.reference = V_SUBJECT;

   //writeTerminal(writer, terminal, scope, retVal, mode);

   return retVal;
}

ref_t Compiler :: mapMessage(SNode node, ExprScope& scope, bool variadicOne, bool extensionCall)
{
   ref_t actionFlags = variadicOne ? VARIADIC_MESSAGE : 0;
   if (extensionCall)
      actionFlags |= FUNCTION_MESSAGE;

//   IdentifierString signature;
   IdentifierString messageStr;

   SNode current = node;
   //if (node.argument != 0)
   //   // if the message is already declared
   //   return node.argument;

   SNode name = current.firstChild(lxTerminalMask);
   //HOTFIX : if generated by a script / closure call
   if (name == lxNone)
      name = current;

   if (name == lxNone)
      scope.raiseError(errInvalidOperation, node);

   messageStr.copy(name.identifier());

   current = current.nextNode();

   int argCount = 1;
   // if message has generic argument list
   while (true) {
      if (current == lxPropertyParam) {
         // COMPILER MAGIC : recognize the property get call
         actionFlags = PROPERTY_MESSAGE;
      }
      else if (test(current.type, lxObjectMask)) {
         argCount++;
      }
//      else if (current == lxMessage) {
//         messageStr.append(':');
//         messageStr.append(current.firstChild(lxTerminalMask).identifier());
//      }
      else break;

      current = current.nextNode();
   }

   if (argCount >= ARG_COUNT) {
      actionFlags |= VARIADIC_MESSAGE;
      argCount = 2;
   }

   if (messageStr.Length() == 0) {
      actionFlags |= FUNCTION_MESSAGE;

      // if it is an implicit message
      messageStr.copy(INVOKE_MESSAGE);
   }

   if (test(actionFlags, FUNCTION_MESSAGE))
      // exclude the target from the arg counter for the function
      argCount--;

   // if signature is presented
   ref_t actionRef = scope.moduleScope->module->mapAction(messageStr, 0, false);

   // create a message id
   return encodeMessage(actionRef, argCount, actionFlags);
}

ref_t Compiler :: mapExtension(Scope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo object, int& stackSafeAttr)
{
   ref_t objectRef = resolveObjectReference(scope, object, true);
   if (objectRef == 0) {
      objectRef = scope.moduleScope->superReference;
   }

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   // auto generate extension template
   for (auto it = nsScope->extensionTemplates.getIt(messageRef); !it.Eof(); it = nsScope->extensionTemplates.nextIt(messageRef, it)) {
      ref_t resolvedTemplateExtension = _logic->resolveExtensionTemplate(*scope.moduleScope, *this, *it,
         implicitSignatureRef, nsScope->ns, nsScope->outerExtensionList ? nsScope->outerExtensionList : &nsScope->extensions);
      if (resolvedTemplateExtension) {
         //ref_t strongMessage = encodeMessage()

         //nsScope->extensions.add(messageRef, Pair<ref_t, ref_t>(resolvedTemplateExtension, strongMessage));

      }
   }

   // check extensions
   auto it = nsScope->extensions.getIt(messageRef);
   bool found = !it.Eof();
   if (found) {
      // generate an extension signature
      ref_t signaturues[ARG_COUNT];
      ref_t signatureLen = scope.module->resolveSignature(implicitSignatureRef, signaturues);
      for (size_t i = signatureLen; i > 0; i--)
         signaturues[i] = signaturues[i - 1];
      signaturues[0] = objectRef;
      signatureLen++;

      int argCount = getArgCount(messageRef);
      while (signatureLen < (ref_t)argCount) {
         signaturues[signatureLen] = scope.moduleScope->superReference;
         signatureLen++;
      }

      /*ref_t full_sign = */scope.module->mapSignature(signaturues, signatureLen, false);
      ref_t resolvedMessage = 0;
      ref_t resolvedExtRef = 0;
      int resolvedStackSafeAttr = 0;
      while (!it.Eof()) {
         auto extInfo = *it;
         ref_t targetRef = nsScope->resolveExtensionTarget(extInfo.value1);
         int extStackAttr = 0;
         if (_logic->isMessageCompatibleWithSignature(*scope.moduleScope, targetRef, extInfo.value2,
            signaturues, signatureLen, extStackAttr))
         {
            if (!resolvedMessage) {
               resolvedMessage = extInfo.value2;
               resolvedExtRef = extInfo.value1;
               resolvedStackSafeAttr = extStackAttr;
            }
            else if (!_logic->isSignatureCompatible(*scope.moduleScope, extInfo.value2, resolvedMessage)) {
               resolvedMessage = 0;
               break;
            }
         }

         it = nsScope->extensions.nextIt(messageRef, it);
      }

      if (resolvedMessage) {
         // if we are lucky - use the resolved one
         messageRef = resolvedMessage;
         stackSafeAttr = resolvedStackSafeAttr;

         return resolvedExtRef;
      }

      // bad luck - we have to generate run-time extension dispatcher
      ref_t extRef = nsScope->extensionDispatchers.get(messageRef);
      if (extRef == INVALID_REF) {
         extRef = compileExtensionDispatcher(*nsScope, messageRef);

         nsScope->extensionDispatchers.add(messageRef, extRef);
      }

      messageRef |= FUNCTION_MESSAGE;

      return extRef;
   }

   return 0;
}

void Compiler :: compileBranchingNodes(SNode node, ExprScope& scope, ref_t ifReference, bool loopMode, bool switchMode)
{
   if (loopMode) {
      SNode thenCode = node.findSubNode(lxCode);
      if (thenCode == lxNone) {
         //HOTFIX : inline branching operator
         node.injectAndReplaceNode(lxElse, ifReference);

         thenCode = node.firstChild();
      }
      else node.set(lxElse, ifReference);

      compileSubCode(thenCode, scope, true);
   }
   else {
      SNode thenCode = node.findSubNode(lxCode);
      if (thenCode == lxNone) {
         //HOTFIX : inline branching operator
         node.injectAndReplaceNode(lxIf, ifReference);

         thenCode = node.firstChild();
      }
      else node.set(lxIf, ifReference);

      compileSubCode(thenCode, scope, true);

      // HOTFIX : switch mode - ignore else
      if (!switchMode) {
         node = node.nextNode(lxObjectMask);
         if (node != lxNone) {
            SNode elseCode = node.findSubNode(lxCode);
            if (elseCode == lxNone) {
               node.injectAndReplaceNode(lxElse, ifReference);

               elseCode = node.firstChild();
            }
            else node.set(lxElse, 0);

            if (elseCode == lxNone)
               //HOTFIX : inline branching operator
               elseCode = node;

            compileSubCode(elseCode, scope, true);
         }
      }
   }
}

ref_t Compiler :: resolveOperatorMessage(Scope& scope, ref_t operator_id, int argCount)
{
   switch (operator_id) {
      case IF_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(IF_MESSAGE, 0, false), argCount, 0);
      case IFNOT_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(IFNOT_MESSAGE, 0, false), argCount, 0);
      case EQUAL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(EQUAL_MESSAGE, 0, false), argCount, 0);
      case NOTEQUAL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTEQUAL_MESSAGE, 0, false), argCount, 0);
      case LESS_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(LESS_MESSAGE, 0, false), argCount, 0);
      case NOTLESS_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTLESS_MESSAGE, 0, false), argCount, 0);
      case GREATER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(GREATER_MESSAGE, 0, false), argCount, 0);
      case NOTGREATER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NOTGREATER_MESSAGE, 0, false), argCount, 0);
      case ADD_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(ADD_MESSAGE, 0, false), argCount, 0);
      case SUB_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SUB_MESSAGE, 0, false), argCount, 0);
      case MUL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(MUL_MESSAGE, 0, false), argCount, 0);
      case DIV_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(DIV_MESSAGE, 0, false), argCount, 0);
      case AND_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(AND_MESSAGE, 0, false), argCount, 0);
      case OR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(OR_MESSAGE, 0, false), argCount, 0);
      case XOR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(XOR_MESSAGE, 0, false), argCount, 0);
      case SHIFTR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTR_MESSAGE, 0, false), argCount, 0);
      case SHIFTL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTL_MESSAGE, 0, false), argCount, 0);
      case REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(REFER_MESSAGE, 0, false), argCount, 0);
      case SET_REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SET_REFER_MESSAGE, 0, false), argCount, 0);
      default:
         throw InternalError("Not supported operator");
         break;
   }
}

inline EAttr defineBranchingOperandMode(SNode node)
{
   EAttr mode = /*HINT_SUBCODE_CLOSURE | */HINT_NODEBUGINFO;
   if (node.firstChild() != lxCode) {
      mode = mode | HINT_INLINE_EXPR;
   }

   return mode;
}

void Compiler :: compileBranchingOp(SNode roperandNode, ExprScope& scope, EAttr mode, int operator_id,
   ObjectInfo loperand, ObjectInfo& retVal)
{
   bool loopMode = EAttrs::test(mode, HINT_LOOP);
   bool switchMode = EAttrs::test(mode, HINT_SWITCH);

   // HOTFIX : in loop expression, else node is used to be similar with branching code
   // because of optimization rules
   ref_t original_id = operator_id;
   if (loopMode) {
      operator_id = operator_id == IF_OPERATOR_ID ? IFNOT_OPERATOR_ID : IF_OPERATOR_ID;
   }

   ref_t ifReference = 0;
   ref_t resolved_operator_id = operator_id;
   // try to resolve the branching operator directly
   if (_logic->resolveBranchOperation(*scope.moduleScope, resolved_operator_id,
      resolveObjectReference(scope, loperand, false), ifReference))
   {
      // we are lucky : we can implement branching directly
      compileBranchingNodes(roperandNode, scope, ifReference, loopMode, switchMode);

      SNode branchNode = roperandNode.parentNode();
      branchNode.set(loopMode ? lxLooping : lxBranching, switchMode ? -1 : 0);

      if (loopMode) {
         // check if the loop has root boxing operations
         SNode exprNode = branchNode;
         SNode rootExpr = exprNode.parentNode();
         while (rootExpr != lxSeqExpression || rootExpr.argument != -1) {
            exprNode = rootExpr;
            rootExpr = rootExpr.parentNode();
         }
         if (exprNode.prevNode() != lxNone && rootExpr == lxSeqExpression) {
            // bad luck : we have to relocate boxing expressions into the loop
            SNode seqNode = branchNode.insertNode(lxSeqExpression);

            exprNode = exprNode.prevNode();
            while (exprNode != lxNone) {
               // copy to the new location
               SNode copyNode = seqNode.insertNode(exprNode.type, exprNode.argument);
               SyntaxTree::copyNode(exprNode, copyNode);

               // commenting out old one
               exprNode = lxIdle;

               exprNode = exprNode.prevNode();
            }
         }
      }
   }
   else {
      operator_id = original_id;

      // bad luck : we have to create a closure
      int message = resolveOperatorMessage(scope, operator_id, 2);

      compileClosure(roperandNode, scope, defineBranchingOperandMode(roperandNode));
      // HOTFIX : comment out the method code 
      roperandNode = lxIdle;

      SNode elseNode = roperandNode.nextNode();
      if (elseNode != lxNone) {
         message = overwriteArgCount(message, 3);

         compileClosure(elseNode, scope, defineBranchingOperandMode(elseNode));
         // HOTFIX : comment out the method code 
         elseNode = lxIdle;
      }

      SNode parentNode = roperandNode.parentNode();
      bool dummy = false;
      retVal = compileMessage(parentNode, scope, loperand, message, EAttr::eaNone, 0, dummy);

      if (loopMode) {
         parentNode.injectAndReplaceNode(lxLooping);
      }
   }
}

ObjectInfo Compiler :: compileBranchingOperator(SNode roperandNode, ExprScope& scope, ObjectInfo loperand, EAttr mode, int operator_id)
{
   ObjectInfo retVal(okObject);

   compileBranchingOp(roperandNode, scope, mode, operator_id, loperand, retVal);

   return retVal;
}

ObjectInfo Compiler :: compileIsNilOperator(SNode current, ExprScope& scope, ObjectInfo loperand)
{
   SNode exprNode = current.parentNode();
   if (loperand.kind == okObject) {
      SNode firstNode = exprNode.firstChild(lxObjectMask);
      firstNode.injectAndReplaceNode(lxAlt);
      firstNode.appendNode(lxExpression).appendNode(lxNil);
   }

   ObjectInfo roperand = compileExpression(current, scope, 0, EAttr::eaNone);

   exprNode.set(lxNilOp, ISNIL_OPERATOR_ID);

   ref_t loperandRef = resolveObjectReference(scope, loperand, false);
   ref_t roperandRef = resolveObjectReference(scope, roperand, false);

   ref_t resultRef = _logic->isCompatible(*scope.moduleScope, loperandRef, roperandRef) ? loperandRef : 0;

   return ObjectInfo(okObject, resultRef);
}

inline bool IsArrExprOperator(int operator_id, LexicalType type)
{
   switch (type) {
      case lxIntArrOp:
      case lxShortArrOp:
      case lxByteArrOp:
         return operator_id == REFER_OPERATOR_ID;
      case lxBinArrOp:
         return operator_id == REFER_OPERATOR_ID;
      default:
         return false;
   }
}

ObjectInfo Compiler :: compileOperator(SNode& node, ExprScope& scope, int operator_id, int argCount, ObjectInfo loperand,
   ObjectInfo roperand, ObjectInfo roperand2, EAttr mode)
{
   ObjectInfo retVal;
   if (loperand.kind == okIntConstant && roperand.kind == okIntConstant) {
      int result = 0;
      if (calculateIntOp(operator_id, loperand.extraparam, roperand.extraparam, result)) {
         retVal = mapIntConstant(scope, result);
         node.set(lxConstantInt, retVal.param);
         node.appendNode(lxIntValue, result);

         return retVal;
      }
   }
   else if (loperand.kind == okRealConstant && roperand.kind == okRealConstant) {
      double l = scope.module->resolveConstant(loperand.param).toDouble();
      double r = scope.module->resolveConstant(roperand.param).toDouble();

      double result = 0;
      if (calculateRealOp(operator_id, l, r, result)) {
         retVal = mapRealConstant(scope, result);
         node.set(lxConstantReal, retVal.param);

         return retVal;
      }
   }

   ref_t loperandRef = resolveObjectReference(scope, loperand, false);
   ref_t roperandRef = resolveObjectReference(scope, roperand, false);
//   ref_t roperand2Ref = 0;
   ref_t resultClassRef = 0;
   int operationType = 0;

   //   if (roperand2.kind != okUnknown) {
//      roperand2Ref = resolveObjectReference(scope, roperand2, false);
//      //HOTFIX : allow to work with int constants
//      if (roperand2.kind == okIntConstant && loperandRef == V_OBJARRAY)
//         roperand2Ref = 0;
//
//      operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef, roperand2Ref, resultClassRef);
//
//      //if (roperand2Ref == V_NIL && loperandRef == V_INT32ARRAY && operator_id == SET_REFER_MESSAGE_ID) {
//      //   //HOTFIX : allow set operation with nil
//      //   operator_id = SETNIL_REFER_MESSAGE_ID;
//      //}
//   }
   /*else */operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef,
      resultClassRef);

   // HOTFIX : primitive operations can be implemented only in the method
   // because the symbol implementations do not open a new stack frame
   if (operationType != 0 && resultClassRef != V_FLAG && scope.getScope(Scope::ScopeLevel::slMethod) == NULL) {
      operationType = 0;
   }

//   //bool assignMode = false;
   if (operationType != 0) {
      // if it is a primitive operation
      if (IsExprOperator(operator_id) || IsArrExprOperator(operator_id, (LexicalType)operationType)) {
         retVal = allocateResult(scope, /*false, */resultClassRef, loperand.element);
      }
      else retVal = ObjectInfo(okObject, 0, resultClassRef, loperand.element, 0);

      // HOTFIX : remove boxing expressions
      analizeOperands(node, scope, -1, false);

      ref_t opElementRef = loperand.element;
      if (operator_id == LEN_OPERATOR_ID)
         opElementRef = roperand.element;

      _logic->injectOperation(node, scope, *this, operator_id, operationType, resultClassRef, opElementRef, retVal.param);
      // HOTFIX : update the result type
      retVal.reference = resultClassRef;

      if (IsArrExprOperator(operator_id, (LexicalType)operationType)) {
         // inject to target for array operation
         node.appendNode(lxLocalAddress, retVal.param);

         node.injectAndReplaceNode(lxSeqExpression);

         SNode valExpr = node.appendNode(lxBoxableExpression);
         valExpr.appendNode(lxLocalAddress, retVal.param);
         appendBoxingInfo(valExpr, scope, retVal, EAttrs::test(mode, HINT_NOUNBOXING),
            0, resolveObjectReference(scope, retVal, false));
      }
   }
   // if not , replace with appropriate method call
   else {
      EAttr operationMode = HINT_NODEBUGINFO;
      ref_t implicitSignatureRef = 0;
      if (roperand2.kind != okUnknown) {
         implicitSignatureRef = resolveStrongArgument(scope, roperand, roperand2);
      }
      else implicitSignatureRef = resolveStrongArgument(scope, roperand);

      int stackSafeAttr = 0;
      int messageRef = resolveMessageAtCompileTime(loperand, scope, resolveOperatorMessage(scope, operator_id, argCount),
         implicitSignatureRef, true, stackSafeAttr);

      if (!test(stackSafeAttr, 1)) {
         operationMode = operationMode | HINT_DYNAMIC_OBJECT;
      }
      else stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

      bool dummy = false;
      retVal = compileMessage(node, scope, loperand, messageRef, operationMode, stackSafeAttr, dummy);
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperator(SNode& node, ExprScope& scope, ObjectInfo loperand, EAttr mode, int operator_id)
{
   SNode opNode = node.parentNode();

   ObjectInfo retVal(okObject);
   int argCount = 2;

   ObjectInfo roperand;
   ObjectInfo roperand2;
   SNode roperandNode = node;
   if (operator_id == SET_REFER_OPERATOR_ID) {
      // HOTFIX : overwrite the assigning part
      SNode roperand2Node = node.parentNode().nextNode();
      if (roperand2Node == lxAssign) {
         roperand2Node = lxIdle;
         roperand2Node = roperand2Node.nextNode();
      }

      SyntaxTree::copyNode(roperand2Node, opNode.appendNode(roperand2Node.type, roperand2Node.argument));

      roperand2Node = lxIdle;
      roperand2Node = node.nextNode();

      roperand = compileExpression(roperandNode, scope, 0, EAttr::eaNone);
      roperand2 = compileExpression(roperand2Node, scope, 0, EAttr::eaNone);

      argCount++;
   }
   else {
//      /*if (roperandNode == lxLocal) {
//         // HOTFIX : to compile switch statement
//         roperand = ObjectInfo(okLocal, roperandNode.argument);
//      }*/
      if (test(roperandNode.type, lxTerminalMask)) {
         roperand = mapTerminal(roperandNode, scope, EAttr::eaNone);
      }
      else roperand = compileExpression(roperandNode, scope, 0, EAttr::eaNone);
   }

   return compileOperator(opNode, scope, operator_id, argCount, loperand, roperand, roperand2, mode);
}

inline ident_t resolveOperatorName(SNode node)
{
   SNode terminal = node.firstChild(lxTerminalMask);
   if (terminal != lxNone) {
      return terminal.identifier();
   }
   else return node.identifier();
}

ObjectInfo Compiler :: compileOperator(SNode& node, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   SNode current = node;
   int operator_id = (int)current.argument > 0 ? current.argument : _operators.get(resolveOperatorName(current));

   SNode roperand = node.nextNode();
//   if (operatorNode.prevNode() == lxNone)
//      roperand = roperand.nextNode(lxObjectMask);

   switch (operator_id) {
      case IF_OPERATOR_ID:
      case IFNOT_OPERATOR_ID:
         // if it is branching operators
         return compileBranchingOperator(roperand, scope, target, mode, operator_id);
      case CATCH_OPERATOR_ID:
      case FINALLY_OPERATOR_ID:
         return compileCatchOperator(roperand, scope/*, target, mode*/, operator_id);
      case ALT_OPERATOR_ID:
         return compileAltOperator(roperand, scope, target/*, mode, operator_id*/);
      case ISNIL_OPERATOR_ID:
         return compileIsNilOperator(roperand, scope, target);
      case APPEND_OPERATOR_ID:
         node.setArgument(ADD_OPERATOR_ID);
         return compileAssigning(node, scope, target, false);
      case REDUCE_OPERATOR_ID:
         node.setArgument(SUB_OPERATOR_ID);
         return compileAssigning(node, scope, target, false);
      case INCREASE_OPERATOR_ID:
         node.setArgument(MUL_OPERATOR_ID);
         return compileAssigning(node, scope, target, false);
      case SEPARATE_OPERATOR_ID:
         node.setArgument(DIV_OPERATOR_ID);
         return compileAssigning(node, scope, target, false);
      default:
         return compileOperator(roperand, scope, target, mode, operator_id);
   }
}

ObjectInfo Compiler :: compileMessage(SNode& node, ExprScope& scope, ObjectInfo target, int messageRef,
   EAttr mode, int stackSafeAttr, bool& embeddable)
{
   ObjectInfo retVal(okObject);

   LexicalType operation = lxCalling_0;
   int argument = messageRef;

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target, true);
   ref_t constRef = 0;

   bool inlineArgCall = EAttrs::test(mode, HINT_INLINEARGMODE);
//   bool dispatchCall = false;
   bool directCall = EAttrs::test(mode, HINT_DIRECTCALL);
   _CompilerLogic::ChechMethodInfo result;
   int callType = 0;
   if (!inlineArgCall && !directCall) {
      callType = _logic->resolveCallType(*scope.moduleScope, classReference, messageRef, result);
   }

   if (callType == tpPrivate) {
      if (isSelfCall(target)) {
         messageRef |= STATIC_MESSAGE;

         callType = tpSealed;
      }
      else result.found = false;
   }

   if (result.found) {
      retVal.reference = result.outputReference;
      constRef = result.constRef;
   }

////   else if (classReference == scope.moduleScope->signatureReference) {
////      dispatchCall = test(mode, HINT_EXTENSION_MODE);
////   }
////   else if (classReference == scope.moduleScope->messageReference) {
////      dispatchCall = test(mode, HINT_EXTENSION_MODE);
////   }
   /*else */if (target.kind == okSuper) {
      // parent methods are always sealed
      callType = tpSealed;
   }

   if (inlineArgCall) {
      operation = lxInlineArgCall;
      argument = messageRef;
   }
//   else if (dispatchCall) {
//      operation = lxDirectCalling;
//      argument = scope.moduleScope->dispatch_message;
//
//      writer.appendNode(lxOvreriddenMessage, messageRef);
//   }
   else if (callType == tpClosed || callType == tpSealed) {
      operation = callType == tpClosed ? lxSDirectCalling : lxDirectCalling;
      argument = messageRef;

      if (!EAttrs::test(mode, HINT_DYNAMIC_OBJECT)) {
         // if the method directly resolved and the target is not required to be dynamic, mark it as stacksafe
         if (target.kind == okParams) {
            // HOTFIX : if variadic argument should not be dynamic, mark it as stacksafe
            stackSafeAttr |= 1;
         }
         else if (_logic->isStacksafeArg(*scope.moduleScope, classReference) && result.stackSafe)
            stackSafeAttr |= 1;
      }
   }
   else {
      // if the sealed / closed class found and the message is not supported - warn the programmer and raise an exception
      if (EAttrs::test(mode, HINT_SILENT)) {
         // do nothing in silent mode
      }
      else if (result.found && !result.withCustomDispatcher && callType == tpUnknown && result.directResolved) {
//         if (EAttrs::test(mode, HINT_ASSIGNING_EXPR)) {
//            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxExpression).findChild(lxMessage));
//         }
         /*else */if (node.findChild(lxMessage).firstChild(lxTerminalMask) == lxNone) {
            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxMessage));
      }
   }

   if (result.embeddable) {
      embeddable = result.embeddable;
      node.appendNode(lxEmbeddableAttr);
   }

//   if (stackSafeAttr && !dispatchCall && !result.dynamicRequired)
//      writer.appendNode(lxStacksafeAttr, stackSafeAttr);

   if (classReference)
      node.insertNode(lxCallTarget, classReference);

   if (constRef && callType == tpSealed) {
      node.appendNode(lxConstAttr, constRef);

      NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

      retVal = ns->defineObjectInfo(constRef, true);
   }

//   if (!EAttrs::test(mode, HINT_NODEBUGINFO)) {
//      // set a breakpoint
//      writer.newNode(lxBreakpoint, dsStep);
//
//      SNode messageNode = node.findChild(lxIdentifier/*, lxPrivate*/);
//      if (messageNode != lxNone) {
//         writeTerminalInfo(writer, messageNode);
//      }
//
//      writer.closeNode();
//   }

   // define the message target if required
   if (target.kind == okConstantRole/* || target.kind == okSubject*/) {
      node.insertNode(lxConstantSymbol, target.reference);

//      writer.newNode(lxOverridden);
//      writer.newNode(lxExpression);
//      writeTerminal(writer, node, scope, target, EAttr::eaNone);
//      writer.closeNode();
//      writer.closeNode();
   }

   // inserting calling expression
   node.set(operation, argument);

   analizeOperands(node, scope, stackSafeAttr, false);

   return retVal;
}

void Compiler :: boxArgument(SNode boxExprNode, SNode current, ExprScope& scope,
   bool boxingMode, bool withoutLocalBoxing, bool inPlace, bool condBoxing)
{
   if (current == lxExpression) {
      boxArgument(boxExprNode, current.firstChild(lxObjectMask), scope, boxingMode, withoutLocalBoxing,
         inPlace, condBoxing);
   }
   else if (current == lxSeqExpression) {
      boxArgument(boxExprNode, current.lastChild(lxObjectMask), scope, boxingMode, withoutLocalBoxing,
         true, condBoxing);
   }
   else if (current.compare(lxBoxableExpression, lxCondBoxableExpression)) {
      // resolving double boxing
      current.set(lxExpression, 0);

      boxArgument(boxExprNode, current.firstChild(lxObjectMask), scope, boxingMode, withoutLocalBoxing,
         inPlace, condBoxing);
   }
   else if (current == lxArgBoxableExpression) {
      throw InternalError("Not yet implemented");
   }
   else {
      if (current == lxNewArrOp) {
         ref_t typeRef = boxExprNode.findChild(lxType).argument;

         current.setArgument(typeRef);
      }
      else if (current.compare(lxStdExternalCall, lxExternalCall, lxCoreAPICall)) {
         if (boxingMode) {
            injectIndexBoxing(boxExprNode, current, scope);
         }
      }
      else if (boxingMode || (current == lxFieldExpression && !withoutLocalBoxing)) {
         if (inPlace || (test(current.type, lxOpScopeMask) && current != lxFieldExpression)) {
            boxExpressionInPlace(boxExprNode, current, scope, !boxingMode, condBoxing);
         }
         else {
            SNode argNode = current;
            if (current == lxFieldExpression) {
               argNode = current.lastChild(lxObjectMask);
            }

            if (boxingMode || (!withoutLocalBoxing && argNode == lxFieldAddress)) {
               Attribute key(argNode.type, argNode.argument);

               int tempLocal = scope.tempLocals.get(key);
               if (tempLocal == NOTFOUND_POS) {
                  tempLocal = scope.newTempLocal();
                  scope.tempLocals.add(key, tempLocal);

                  boxExpressionInRoot(boxExprNode, current, scope, lxTempLocal, tempLocal,
                     !boxingMode, condBoxing);
               }
               else current.set(lxTempLocal, tempLocal);
            }
         }
      }
   }
}

void Compiler :: analizeOperand(SNode& current, ExprScope& scope, bool boxingMode, bool withoutLocalBoxing, bool inPlace)
{
   switch (current.type) {
      case lxArgBoxableExpression:
      case lxBoxableExpression:
      case lxCondBoxableExpression:
      case lxPrimArrBoxableExpression:
         boxArgument(current, current.firstChild(lxObjectMask), scope, boxingMode, withoutLocalBoxing,
            inPlace, current == lxCondBoxableExpression);
         current.set(lxExpression, 0);
         break;
      case lxExpression:
      {
         SNode opNode = current.firstChild(lxObjectMask);
         analizeOperand(opNode, scope, boxingMode, withoutLocalBoxing, inPlace);
         break;
      }
      case lxSeqExpression:
      {
         SNode opNode = current.lastChild(lxObjectMask);
         // HOTFIX : box in-place the result of sub operation
         analizeOperand(opNode, scope, boxingMode, withoutLocalBoxing, true);
         break;
      }
      case lxFieldExpression:
      {
         SNode opNode = current.lastChild(lxObjectMask);
         analizeOperand(opNode, scope, boxingMode, withoutLocalBoxing, inPlace);
         break;
      }
      default:
         break;
   }
}

void Compiler :: analizeOperands(SNode& node, ExprScope& scope, int stackSafeAttr, bool inPlace)
{
   // if boxing / unboxing required - insert SeqExpression, prepand boxing, replace operand with boxed arg, append unboxing
   SNode current = node.firstChild(lxObjectMask);
   int argBit = 1;
   while (current != lxNone) {
      analizeOperand(current, scope, !test(stackSafeAttr, argBit), false, inPlace);

      argBit <<= 1;
      current = current.nextNode(lxObjectMask);
   }
}

ObjectInfo Compiler :: convertObject(SNode& node, ExprScope& scope, ref_t targetRef, ObjectInfo source, EAttr mode)
{
   //NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   bool noUnboxing = EAttrs::test(mode, HINT_NOUNBOXING);
   ref_t sourceRef = resolveObjectReference(scope, source, false);
   int stackSafeAttrs = 0;
   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef)) {
      if ((source.kind == okIntConstant || source.kind == okUIntConstant)
         && targetRef == scope.moduleScope->intReference && !EAttrs::test(mode, HINT_DYNAMIC_OBJECT))
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->intReference;

         return source;
      }
      else if ((source.kind == okLongConstant)
         && targetRef == scope.moduleScope->longReference && !EAttrs::test(mode, HINT_DYNAMIC_OBJECT))
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->longReference;

         return source;
      }
      else if ((source.kind == okRealConstant)
         && targetRef == scope.moduleScope->realReference && !EAttrs::test(mode, HINT_DYNAMIC_OBJECT))
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->realReference;

         return source;
      }
      else if (source.kind == okExternal &&
         _logic->isCompatible(*scope.moduleScope, sourceRef, targetRef))
      {
         // HOTFIX : allow to pass the result of external operation directly
         return source;
      }
      else {
         int fixedArraySize = 0;
         if (source.kind == okFieldAddress && isPrimitiveArrRef(sourceRef))
            fixedArraySize = defineFieldSize(scope, source.param);

         if (_logic->injectImplicitConversion(scope, node, *this, targetRef, sourceRef,
            source.element, noUnboxing, stackSafeAttrs, fixedArraySize))
         {
            if (node.compare(lxDirectCalling, lxSDirectCalling)) {
               // HOTFIX : box arguments if required
               analizeOperands(node, scope, stackSafeAttrs, false);
            }

            return ObjectInfo(okObject, 0, targetRef);
         }
         else return sendTypecast(node, scope, targetRef, source);
      }

   }
   return source;
}

ObjectInfo Compiler :: sendTypecast(SNode& node, ExprScope& scope, ref_t targetRef, ObjectInfo source)
{
   if (targetRef != 0 /*&& !isPrimitiveRef(targetRef)*/) {
      if (targetRef != scope.moduleScope->superReference) {
         //HOTFIX : ignore super object
         ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         node.refresh();
         if (node != lxExpression)
            node.injectAndReplaceNode(lxExpression);

         bool dummy;
         compileMessage(node, scope, source, encodeMessage(actionRef, 1, 0), HINT_NODEBUGINFO | HINT_SILENT, 0, dummy);

         return ObjectInfo(okObject, 0, targetRef);
      }
      else return source;
   }
   // NOTE : if the typecasting is not possible, it returns unknown result
   else return ObjectInfo();
}

ref_t Compiler :: resolveStrongArgument(ExprScope& scope, ObjectInfo info)
{
   ref_t argRef = resolveObjectReference(scope, info, true);
   if (!argRef) {
      return 0;
   }

   return scope.module->mapSignature(&argRef, 1, false);
}

ref_t Compiler :: resolveStrongArgument(ExprScope& scope, ObjectInfo info1, ObjectInfo info2)
{
   ref_t argRef[2];

   argRef[0] = resolveObjectReference(scope, info1, true);
   argRef[1] = resolveObjectReference(scope, info2, true);

   if (!argRef[0] || !argRef[1])
      return 0;

   return scope.module->mapSignature(argRef, 2, false);
}

ref_t Compiler :: resolvePrimitiveReference(_CompileScope& scope, ref_t argRef, ref_t elementRef, bool declarationMode)
{
   switch (argRef) {
      case V_WRAPPER:
         return resolveReferenceTemplate(scope, elementRef, declarationMode);
      case V_ARGARRAY:
         return resolvePrimitiveArray(scope, scope.moduleScope->argArrayTemplateReference, elementRef, declarationMode);
      case V_INT32:
         return scope.moduleScope->intReference;
      case V_INT64:
         return scope.moduleScope->longReference;
      case V_REAL64:
         return scope.moduleScope->realReference;
      case V_SUBJECT:
         return scope.moduleScope->messageNameReference;
      case V_MESSAGE:
         return scope.moduleScope->messageReference;
      case V_EXTMESSAGE:
         return scope.moduleScope->extMessageReference;
      case V_UNBOXEDARGS:
         // HOTFIX : should be returned as is
         return argRef;
      case V_NIL:
         return scope.moduleScope->superReference;
      default:
         if (isPrimitiveArrRef(argRef)) {
            return resolvePrimitiveArray(scope, scope.moduleScope->arrayTemplateReference, elementRef, declarationMode);
         }
         throw InternalError("Not yet implemented"); // !! temporal
//         return scope.superReference;
   }
}

ref_t Compiler :: compileMessageParameters(SNode& node, ExprScope& scope, EAttr mode, bool& variadicOne, bool& inlineArg)
{
   EAttr paramMode = HINT_PARAMETER;
   bool externalMode = false;
   if (EAttrs::test(mode, HINT_EXTERNALOP)) {
      externalMode = true;
   }
   else paramMode = paramMode | HINT_NOPRIMITIVES;

   SNode current = node;

   // compile the message argument list
   ref_t signatures[ARG_COUNT];
   ref_t signatureLen = 0;
   while (/*current != lxMessage && */current != lxNone) {
      if (test(current.type, lxObjectMask)) {
//         if (externalMode)
//            writer.newNode(lxExtArgument);

         // try to recognize the message signature
		   ObjectInfo paramInfo = compileExpression(current, scope, 0, paramMode);

         ref_t argRef = resolveObjectReference(scope, paramInfo, false);
         if (signatureLen >= ARG_COUNT) {
            signatureLen++;
         }
         else if (inlineArg) {
            scope.raiseError(errNotApplicable, current);
         }
         else if (argRef == V_UNBOXEDARGS) {
            if (paramInfo.element) {
               signatures[signatureLen++] = paramInfo.element;
            }
            else signatures[signatureLen++] = scope.moduleScope->superReference;

			   if (!variadicOne) {
				   variadicOne = true;
			   }
			   else scope.raiseError(errNotApplicable, current);
         }
         else if (argRef == V_INLINEARG) {
            if (signatureLen == 0) {
               inlineArg = true;
            }
            else scope.raiseError(errNotApplicable, current);
         }
         else if (argRef) {
            signatures[signatureLen++] = argRef;

            if (externalMode && !current.existChild(lxType))
               current.appendNode(lxType, argRef);
         }
         else signatures[signatureLen++] = scope.moduleScope->superReference;

//         if (externalMode) {
//            writer.appendNode(lxExtArgumentRef, argRef);
//            writer.closeNode();
//         }
      }

      current = current.nextNode();
   }

   if (signatureLen > 0 && signatureLen <= ARG_COUNT) {
      bool anonymous = true;
      for (ref_t i = 0; i < signatureLen; i++) {
         if (signatures[i] != scope.moduleScope->superReference) {
            anonymous = false;
            break;
         }
      }
      if (!anonymous || variadicOne)
         return scope.module->mapSignature(signatures, signatureLen, false);
   }

   return 0;
}

ref_t Compiler :: resolveVariadicMessage(Scope& scope, ref_t message)
{
   int argCount = 0;
   ref_t actionRef = 0, flags = 0, dummy = 0;
   decodeMessage(message, actionRef, argCount, flags);

   ident_t actionName = scope.module->resolveAction(actionRef, dummy);

   int argMultuCount = test(message, FUNCTION_MESSAGE) ? 1 : 2;

   return encodeMessage(scope.module->mapAction(actionName, 0, false), argMultuCount, flags | VARIADIC_MESSAGE);
}

bool Compiler :: isSelfCall(ObjectInfo target)
{
   switch (target.kind) {
      case okSelfParam:
      case okOuterSelf:
      case okClassSelf:
      case okInternalSelf:
         return true;
      default:
         return false;
   }
}

ref_t Compiler :: resolveMessageAtCompileTime(ObjectInfo& target, ExprScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef,
   bool withExtension, int& stackSafeAttr)
{
   int resolvedStackSafeAttr = 0;
   ref_t resolvedMessageRef = 0;
   ref_t targetRef = resolveObjectReference(scope, target, true);

   // try to resolve the message as is
   resolvedMessageRef = _logic->resolveMultimethod(*scope.moduleScope, generalMessageRef, targetRef,
      implicitSignatureRef, resolvedStackSafeAttr, isSelfCall(target));
   if (resolvedMessageRef != 0) {
      stackSafeAttr = resolvedStackSafeAttr;

      // if the object handles the compile-time resolved message - use it
      return resolvedMessageRef;
   }

   // check if the object handles the variadic message
   if (targetRef) {
      resolvedStackSafeAttr = 0;
      resolvedMessageRef = _logic->resolveMultimethod(*scope.moduleScope,
         resolveVariadicMessage(scope, generalMessageRef),
         targetRef, implicitSignatureRef, resolvedStackSafeAttr, isSelfCall(target));

      if (resolvedMessageRef != 0) {
         stackSafeAttr = resolvedStackSafeAttr;

         // if the object handles the compile-time resolved variadic message - use it
         return resolvedMessageRef;
      }
   }

   if (withExtension) {
      resolvedMessageRef = generalMessageRef;

      // check the existing extensions if allowed
      if (checkMethod(*scope.moduleScope, targetRef, generalMessageRef) != tpUnknown) {
         // could be stacksafe
         stackSafeAttr |= 1;

         // if the object handles the general message - do not use extensions
         return generalMessageRef;
      }

      resolvedStackSafeAttr = 0;
      ref_t extensionRef = mapExtension(scope, resolvedMessageRef, implicitSignatureRef, target, resolvedStackSafeAttr);
      if (extensionRef != 0) {
         stackSafeAttr = resolvedStackSafeAttr;

         // if there is an extension to handle the compile-time resolved message - use it
         target = ObjectInfo(okConstantRole, extensionRef, extensionRef);

         return resolvedMessageRef;
      }

      // check if the extension handles the variadic message
      ref_t variadicMessage = resolveVariadicMessage(scope, generalMessageRef);

      resolvedStackSafeAttr = 0;
      extensionRef = mapExtension(scope, variadicMessage, implicitSignatureRef, target, resolvedStackSafeAttr);
      if (extensionRef != 0) {
         stackSafeAttr = resolvedStackSafeAttr;

         // if there is an extension to handle the compile-time resolved message - use it
         target = ObjectInfo(okConstantRole, extensionRef, extensionRef);

         return variadicMessage;
      }
   }

   // otherwise - use the general message
   return generalMessageRef;
}

ObjectInfo Compiler :: compileMessage(SNode node, ExprScope& scope, ref_t expectedRef, ObjectInfo target, EAttr mode)
{
   EAttr paramsMode = EAttr::eaNone;
   if (target.kind == okExternal) {
      paramsMode = paramsMode | HINT_EXTERNALOP;
   }

   ObjectInfo retVal;
   bool variadicOne = false;
   bool inlineArg = false;
   ref_t implicitSignatureRef = compileMessageParameters(node, scope, paramsMode, variadicOne, inlineArg);

   //   bool externalMode = false;
   if (target.kind == okExternal) {
      EAttr extMode = mode & HINT_ROOT;

      retVal = compileExternalCall(node, scope, expectedRef, extMode);
   }
   else {
      ref_t messageRef = mapMessage(node, scope, variadicOne, target.kind == okExtension);

      if (target.kind == okInternal) {
         retVal = compileInternalCall(node.parentNode(), scope, messageRef, implicitSignatureRef, target);
      }
      else if (inlineArg) {
         SNode parentNode = node.parentNode();
         bool dummy = false;
         retVal = compileMessage(parentNode, scope, target, messageRef, mode | HINT_INLINEARGMODE, 0, dummy);
      }
      else {
         int stackSafeAttr = 0;
         if (!EAttrs::test(mode, HINT_DIRECTCALL))
            messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, true, stackSafeAttr);

         if (!test(stackSafeAttr, 1)) {
            mode = mode | HINT_DYNAMIC_OBJECT;
         }
         else if (target.kind != okConstantRole)
            stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

         SNode opNode = node.parentNode();
         bool embeddable = false;
         retVal = compileMessage(opNode, scope, target, messageRef, mode, stackSafeAttr, embeddable);

         if (expectedRef && embeddable) {
            ref_t byRefMessageRef = _logic->resolveEmbeddableRetMessage(
               scope, *this, resolveObjectReference(scope, target, true),
               messageRef, expectedRef);

            if (byRefMessageRef) {
               ObjectInfo tempVar = allocateResult(scope, expectedRef);
               if (tempVar.kind == okLocalAddress) {
                  opNode.appendNode(lxLocalAddress, tempVar.param);
                  opNode.appendNode(lxRetEmbeddableAttr);

                  opNode.setArgument(byRefMessageRef);
                  opNode.injectAndReplaceNode(lxSeqExpression);
                  opNode.appendNode(lxLocalAddress, tempVar.param);

                  opNode.injectAndReplaceNode(lxBoxableExpression);
                  opNode.appendNode(lxType, expectedRef);
                  opNode.appendNode(lxSize, _logic->defineStructSize(*scope.moduleScope, expectedRef, 0));
               }
               else throw InternalError("Not yet implemented"); // !! temporal

               retVal = tempVar;
            }
         }
      }
   }

   return retVal;
}

void Compiler :: inheritClassConstantList(_ModuleScope& scope, ref_t sourceRef, ref_t targetRef)
{
   ref_t moduleRef = 0;
   _Module* parent = scope.loadReferenceModule(sourceRef, moduleRef);

   _Memory* source = parent->mapSection(moduleRef | mskRDataRef, true);
   _Memory* target = scope.module->mapSection(targetRef | mskRDataRef, false);

   MemoryReader reader(source);
   MemoryWriter writer(target);

   writer.read(&reader, source->Length());

   _ELENA_::RelocationMap::Iterator it(source->getReferences());
   ref_t currentMask = 0;
   ref_t currentRef = 0;
   while (!it.Eof()) {
      currentMask = it.key() & mskAnyRef;
      currentRef = it.key() & ~mskAnyRef;

      target->addReference(importReference(parent, currentRef, scope.module) | currentMask, *it);

      it++;
   }
}

inline SNode findBookmarkOwner(SNode node, ref_t bookmark)
{
   while (!node.compare(lxClass, lxNone))
      node = node.parentNode();

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.compare(lxClassMethod, lxConstructor, lxClassField, lxStaticMethod)) {
         SNode bm = current.findChild(lxBookmark);
         if (bm.argument == bookmark)
            return current;
      }

      current = current.nextNode();
   }

   return node;
}

void Compiler :: compileMetaConstantAssigning(ObjectInfo, SNode node, ClassScope& scope)
{
   int bm = node.parentNode().findChild(lxBookmarkReference).argument;

   ExprScope exprScope(&scope);

   ObjectInfo source = mapObject(node, exprScope, EAttr::eaNone);
   ref_t sourceRef = resolveConstantObjectReference(scope, source);
   if (isPrimitiveRef(sourceRef))
      sourceRef = resolvePrimitiveReference(scope, sourceRef, source.element, false);

   bool valid = false;
   ident_t info;
   if (sourceRef == scope.moduleScope->literalReference) {
      if (source.kind == okLiteralConstant) {
         info = scope.module->resolveConstant(source.param);
         valid = true;
      }
   }

   if (valid) {
      // resolve the meta field target
      SNode targetNode = findBookmarkOwner(node.parentNode(), bm);
      Attribute key;
      if (targetNode == lxClass) {
         key = Attribute(caInfo, 0);
      }
      else if (targetNode.compare(lxClassMethod, lxConstructor, lxStaticMethod)) {
         key = Attribute(caInfo, targetNode.argument);
      }

      scope.info.mattributes.add(key, saveMetaInfo(*scope.moduleScope, info));
      scope.save();
   }
   else scope.raiseError(errIllegalOperation, node);
}

inline ref_t mapStaticField(_ModuleScope* moduleScope, ref_t reference, bool isArray)
{
   int mask = isArray ? mskConstArray : mskConstantRef;
   IdentifierString name(moduleScope->module->resolveReference(reference));
   name.append(STATICFIELD_POSTFIX);

   return moduleScope->mapAnonymous(name.c_str() + 1) | mask;
}

inline ref_t mapConstant(_ModuleScope* moduleScope, ref_t reference)
{
   IdentifierString name(moduleScope->module->resolveReference(reference));
   name.append(CONSTANT_POSTFIX);

   return moduleScope->mapAnonymous(name.c_str() + 1);
}

inline bool isInherited(_Module* module, ref_t reference, ref_t staticRef)
{
   ident_t name = module->resolveReference(reference);
   ident_t statName = module->resolveReference(staticRef);
   size_t len = getlength(name);

   if (statName[len] == '#' && statName.compare(name, 0, len)) {
      return true;
   }
   else return false;
}

void Compiler :: compileClassConstantAssigning(ObjectInfo target, SNode node, ClassScope& scope, bool accumulatorMode)
{
   ref_t valueRef = scope.info.staticValues.get(target.param);

   if (accumulatorMode) {
      // HOTFIX : inherit accumulating attribute list
      ClassInfo parentInfo;
      scope.moduleScope->loadClassInfo(parentInfo, scope.info.header.parentRef);
      ref_t targtListRef = valueRef & ~mskAnyRef;
      ref_t parentListRef = parentInfo.staticValues.get(target.param) & ~mskAnyRef;

      if (parentListRef != 0 && !isInherited(scope.module, scope.reference, targtListRef)) {
         valueRef = mapStaticField(scope.moduleScope, scope.reference, true);
         scope.info.staticValues.exclude(target.param);
         scope.info.staticValues.add(target.param, valueRef);
         scope.save();

         targtListRef = valueRef & ~mskAnyRef;

         // inherit the parent list
         inheritClassConstantList(*scope.moduleScope, parentListRef, targtListRef);
      }
   }

   SymbolScope constantScope((NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace), valueRef & ~mskAnyRef, Visibility::Public);
   ExprScope exprScope(&constantScope);

   ObjectInfo source = mapObject(node, exprScope, EAttr::eaNone);
   ref_t targetRef = accumulatorMode ? target.element : target.reference;
   if (accumulatorMode && !targetRef)
      targetRef = _logic->resolveArrayElement(*scope.moduleScope, target.reference);

   ref_t sourceRef = resolveConstantObjectReference(scope, source);
   if (isPrimitiveRef(sourceRef))
      sourceRef = resolvePrimitiveReference(scope, sourceRef, source.element, false);

   if (compileSymbolConstant(/*node, */constantScope, source, accumulatorMode, target.reference)
      && _logic->isCompatible(*scope.moduleScope, targetRef, sourceRef))
   {
   }
   else scope.raiseError(errInvalidOperation, node);
}

bool Compiler :: resolveAutoType(ObjectInfo source, ObjectInfo& target, ExprScope& scope)
{
   ref_t sourceRef = resolveObjectReference(scope, source, true);

   if (!_logic->validateAutoType(*scope.moduleScope, sourceRef))
      return false;

   return scope.resolveAutoType(target, sourceRef, source.element);
}

inline ref_t resolveSubjectVar(SNode current)
{
   int bm = current.findChild(lxBookmarkReference).argument;
   if (bm) {
      SNode targetNode = findBookmarkOwner(current.parentNode(), bm);
      if (targetNode.compare(lxClassMethod, lxConstructor, lxStaticMethod)) {
         return getAction(targetNode.argument);
      }
   }

   return 0;
}

void Compiler :: resolveMetaConstant(SNode node)
{
   SNode assignNode = node.findChild(lxAssign);
   if (assignNode != lxNone) {
      SNode exprNode = assignNode.nextNode().findSubNode(lxMetaConstant);
      if (exprNode != lxNone) {
         if (exprNode.identifier().compare(SUBJECT_VAR)) {
            ref_t subjRef = resolveSubjectVar(node);
            if (subjRef) {
               exprNode.set(lxSubjectRef, subjRef);
            }
         }

      }
   }
}

bool Compiler :: recognizeCompileTimeAssigning(SNode node, ClassScope& scope)
{
   bool idle = true;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxFieldInit) {
         ref_t dummy = 0;
         bool dummy2 = false;

         SNode identNode = current.firstChild();
         if (identNode == lxBookmarkReference) {
            resolveMetaConstant(current);
         }

         EAttr mode = recognizeExpressionAttributes(identNode, scope, dummy, dummy2);
         if (identNode != lxNone) {
            ObjectInfo field;
            if (EAttrs::test(mode, HINT_METAFIELD)) {
               field = mapMetaField(identNode.identifier());
            }
            else field = scope.mapField(identNode.identifier(), mode);
            switch (field.kind)
            {
               case okStaticConstantField:
               case okStaticField:
               case okMetaField:
                  // HOTFIX : compile-time assigning should be implemented directly
                  current = lxStaticFieldInit;
                  break;
               default:
                  idle = false;
                  break;
            }
         }
         else idle = false;
      }
      current = current.nextNode();
   }

   return idle;
}

void Compiler :: compileCompileTimeAssigning(SNode node, ClassScope& classScope)
{
   SNode assignNode = node.findChild(lxAssign);
   SNode sourceNode = assignNode.nextNode();
   bool accumulateMode = assignNode.argument == INVALID_REF;

   ExprScope scope(&classScope);

   ObjectInfo target = mapObject(node, scope, EAttr::eaNone);

   // HOTFIX : recognize static field initializer
   if (target.kind == okStaticField || target.kind == okStaticConstantField || target.kind == okMetaField) {
      if (target.kind == okMetaField) {
         compileMetaConstantAssigning(target, sourceNode, *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass))/*, accumulateMode*/);
      }
      else if (!isSealedStaticField(target.param) && target.kind == okStaticConstantField) {
         // HOTFIX : static field initializer should be compiled as preloaded symbol
         compileClassConstantAssigning(target, sourceNode, *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass)), accumulateMode);
      }
      else compileStaticAssigning(target, sourceNode, *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass))/*, accumulateMode*/);
   }
}

ObjectInfo Compiler :: compileAssigning(SNode node, ExprScope& scope, ObjectInfo target, bool accumulateMode)
{
   ObjectInfo retVal = target;
   LexicalType operationType = lxAssigning;
   int operand = 0;

   SNode current = node;
   node = current.parentNode();

   SNode sourceNode;
//   if (current == lxReturning) {
//      sourceNode = current.firstChild(lxObjectMask);
//      if (test(sourceNode.type, lxTerminalMask)) {
//         // HOTFIX
//         sourceNode = current;
//      }
//   }
   /*else */sourceNode = current.nextNode(lxObjectMask);

   if (accumulateMode)
      // !! temporally
      scope.raiseError(errInvalidOperation, sourceNode);

   EAttr assignMode = HINT_NOUNBOXING/* | HINT_ASSIGNING_EXPR*/;
   ref_t targetRef = resolveObjectReference(scope, target, false, false);
   bool noBoxing = false;
//   bool byRefAssigning = false;
   switch (target.kind) {
      case okLocal:
      case okField:
      case okStaticField:
//      case okClassStaticField:
      case okOuterField:
//      case okOuterStaticField:
         break;
      case okLocalAddress:
      case okFieldAddress:
      {
         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
         if (size != 0) {
            noBoxing = true;
            operationType = lxCopying;
            operand = size;
            assignMode = assignMode | HINT_NOBOXING;
         }
         else scope.raiseError(errInvalidOperation, sourceNode);
         break;
      }
      case okOuter:
      case okOuterSelf:
      {
         InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
         //MethodScope* method = (MethodScope*)scope.getScope(Scope::slMethod);

         if (/*!method->subCodeMode || */!closure->markAsPresaved(target))
            scope.raiseError(errInvalidOperation, sourceNode);

         size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
         if (size != 0 && target.kind == okOuter) {
            operand = size;
            //byRefAssigning = true;
         }
         break;
      }
      case okReadOnlyField:
      case okReadOnlyFieldAddress:
      case okOuterReadOnlyField:
         scope.raiseError(errReadOnlyField, node.parentNode());
         break;
      case okParam:
         if (targetRef == V_WRAPPER) {
            //byRefAssigning = true;
            targetRef = target.element;
            size_t size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
            if (size != 0) {
               operand = size;
               operationType = lxCopying;
               noBoxing = true;
            }
            else operationType = lxByRefAssigning;

            break;
         }
      default:
         scope.raiseError(errInvalidOperation, node.firstChild(lxObjectMask));
         break;
   }

   ObjectInfo exprVal;
//   if (operand == 0)
//      assignMode = assignMode | HINT_DYNAMIC_OBJECT | HINT_NOPRIMITIVES;
//
//   if (isPrimitiveArrRef(targetRef))
//      targetRef = resolvePrimitiveReference(scope, targetRef, target.element, false);
//
   if (current == lxOperator) {
      // COMPILER MAGIC : implementing assignment operators
      sourceNode.injectAndReplaceNode(lxExpression);
      SNode roperand = sourceNode.firstChild(lxObjectMask);
      SNode loperand = sourceNode.insertNode(lxVirtualReference);
      recognizeTerminal(loperand, target, scope, EAttr::eaNoDebugInfo);

      compileOperator(roperand, scope, target, assignMode, current.argument);
   }
   else if (targetRef == V_AUTO) {
      // support auto attribute
      exprVal = compileExpression(sourceNode, scope, 0, assignMode);

      if (resolveAutoType(exprVal, target, scope)) {
         targetRef = resolveObjectReference(scope, exprVal, false);
         retVal.reference = targetRef;
      }
      else scope.raiseError(errInvalidOperation, node);
   }
   else if (sourceNode == lxYieldContext) {
      int size = scope.getAttribute(sourceNode.argument, maYieldContextLength);

      sourceNode.set(lxCreatingStruct, size);

      node.set(lxAssigning, 0);
   }
   else if (sourceNode == lxYieldLocals) {
      int size = scope.getAttribute(sourceNode.argument, maYieldLocalLength);
      if (size != 0) {
         sourceNode.set(lxCreatingClass, size);

         node.set(lxAssigning, 0);
      }
      else node = lxIdle;
   }
   else exprVal = compileExpression(sourceNode, scope, targetRef, assignMode);

   if (exprVal.kind == okExternal && operationType == lxCopying) {
      noBoxing = true;
      operationType = lxSaving;
   }

   node.set(operationType, operand);

   SNode fieldNode = node.firstChild(lxObjectMask);
   analizeOperand(fieldNode, scope, false, true, true);
   SNode rargNode = fieldNode.nextNode(lxObjectMask);
   analizeOperand(rargNode, scope, !noBoxing, true, true);

   return retVal;
}

ObjectInfo Compiler :: compilePropAssigning(SNode node, ExprScope& scope, ObjectInfo target)
{
   ObjectInfo retVal;

   // tranfer the message into the property set one
   ref_t messageRef = mapMessage(node, scope, false, false);
   ref_t actionRef, flags;
   int argCount;
   decodeMessage(messageRef, actionRef, argCount, flags);
   if (argCount == 1 && test(flags, PROPERTY_MESSAGE)) {
      messageRef = encodeMessage(actionRef, 2, flags);
   }
   else scope.raiseError(errInvalidOperation, node);

   // find and compile the parameter
   SNode roperand2Node = node.parentNode().nextNode();
   if (roperand2Node == lxAssign) {
      roperand2Node = lxIdle;
      roperand2Node = roperand2Node.nextNode();
   }

   SNode opNode = node.parentNode();
   SyntaxTree::copyNode(roperand2Node, opNode.appendNode(roperand2Node.type, roperand2Node.argument));

   // remove the assign node to prevent the duplication
   roperand2Node = lxIdle;
   roperand2Node = node.nextNode().nextNode();

   ObjectInfo source = compileExpression(roperand2Node, scope, 0, EAttr::eaNone);

   EAttr mode = HINT_NODEBUGINFO;
   int stackSafeAttr = 0;
   messageRef = resolveMessageAtCompileTime(target, scope, messageRef, resolveStrongArgument(scope, source), true, stackSafeAttr);
   if (!test(stackSafeAttr, 1))
      mode = mode | HINT_DYNAMIC_OBJECT;

   bool dummy = false;
   retVal = compileMessage(opNode, scope, target, messageRef, mode, stackSafeAttr, dummy);

   return retVal;
}

bool Compiler :: declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, EAttr mode)
{
   bool lazyExpression = EAttrs::test(mode, HINT_LAZY_EXPR);

   ref_t invokeAction = scope.module->mapAction(INVOKE_MESSAGE, 0, false);
   methodScope.message = encodeMessage(invokeAction, 0, FUNCTION_MESSAGE);

   if (argNode != lxNone) {
      // define message parameter
      methodScope.message = declareInlineArgumentList(argNode, methodScope, false);
   }

   return lazyExpression;
}

void Compiler :: compileAction(SNode& node, ClassScope& scope, SNode argNode, EAttr mode)
{
   MethodScope methodScope(&scope);
   bool lazyExpression = declareActionScope(scope, argNode, methodScope, mode);
   bool inlineExpression = EAttrs::test(mode, HINT_INLINE_EXPR);
   methodScope.functionMode = true;

   ref_t multiMethod = resolveMultimethod(scope, methodScope.message);

//   // HOTFIX : if the closure emulates code brackets
//   if (EAttrs::test(mode, HINT_SUBCODE_CLOSURE))
//      methodScope.subCodeMode = true;

   // if it is single expression
   if (inlineExpression || lazyExpression) {
      //inject a method
      node.injectAndReplaceNode(lxClass);
      SNode current = node.firstChild();
      current.injectAndReplaceNode(lxClassMethod, methodScope.message);

      compileExpressionMethod(current, methodScope, lazyExpression);
   }
   else {
      // inject a method
      SNode current = node.findChild(lxCode, lxReturning);
      current.injectAndReplaceNode(lxClassMethod, methodScope.message);

      initialize(scope, methodScope);
      methodScope.functionMode = true;

      if (multiMethod)
         // if it is a strong-typed closure, output should be defined by the closure
         methodScope.outputRef = V_AUTO;

      compileActionMethod(current, methodScope);

      // HOTFIX : inject an output type if required or used super class
      if (methodScope.outputRef == V_AUTO) {
         methodScope.outputRef = scope.moduleScope->superReference;
      }

      if (methodScope.outputRef)
         current.insertNode(lxType, methodScope.outputRef);
   }

   // the parent is defined after the closure compilation to define correctly the output type
   ref_t parentRef = scope.info.header.parentRef;
   if (lazyExpression) {
      parentRef = scope.moduleScope->lazyExprReference;
   }
   else {
      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

      ref_t closureRef = scope.moduleScope->resolveClosure(methodScope.message, methodScope.outputRef, nsScope->nsName);
      //      ref_t actionRef = scope.moduleScope->actionHints.get(methodScope.message);
      if (closureRef) {
         parentRef = closureRef;
      }
      else throw InternalError(errClosureError);
   }

   // NOTE : the fields are presaved, so the closure parent should be stateless
   compileParentDeclaration(SNode(), scope, parentRef, true);

   // set the message output if available
   if (methodScope.outputRef)
      scope.addAttribute(methodScope.message, maReference, methodScope.outputRef);

   if (multiMethod) {
      // inject a virtual invoke multi-method if required
      List<ref_t> implicitMultimethods;
      implicitMultimethods.add(multiMethod);

      _logic->injectVirtualMultimethods(*scope.moduleScope, node, *this, implicitMultimethods, lxClassMethod);

      generateClassDeclaration(node, scope);

      // HOTFIX : temporally commenting out the function code, to prevent if from double compiling
      SNode current = node.findChild(lxClassMethod);
      current = lxIdle;
      compileVMT(node, scope);
      current = lxClassMethod;
   }
   else {
      // include the message, it is done after the compilation due to the implemetation
      scope.include(methodScope.message);
      scope.addHint(methodScope.message, tpFunction);

      // exclude abstract flag if presented
      scope.removeHint(methodScope.message, tpAbstract);

      generateClassDeclaration(SNode(), scope);
   }

   scope.save();

//   if (scope.info.staticValues.Count() > 0)
//      copyStaticFieldValues(node, scope);

   generateClassImplementation(node, scope);

   // COMPILER MAGIC : prepand a virtual identifier, terminal info should be copied from the leading attribute
   SNode attrTerminal = argNode.firstChild(lxTerminalMask);
   node = node.prependSibling(lxVirtualReference);
   SyntaxTree::copyNode(attrTerminal, node);
}

void Compiler :: compileNestedVMT(SNode& node, InlineClassScope& scope)
{
   SNode current = node.firstChild();
   bool virtualClass = true;
   while (current != lxNone) {
      if (current == lxAttribute) {
         EAttrs attributes;
         bool dummy = false;
         if (_logic->validateExpressionAttribute(current.argument, attributes, dummy) && attributes.test(EAttr::eaNewOp)) {
            // only V_NEWOP attribute is allowed
            current.setArgument(0);
         }
         else scope.raiseError(errInvalidHint, current);
      }
      else if (current == lxType) {
         current.injectAndReplaceNode(lxParent);
      }
      if (current == lxClassField) {
         virtualClass = false;
      }
      else if (current == lxClassMethod) {
         if (!current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier().compare(INIT_MESSAGE)) {
            // HOTFIX : ignore initializer auto-generated method
            virtualClass = false;
            break;
         }
      }
      current = current.nextNode();
   }

   if (virtualClass)
      scope.info.header.flags |= elVirtualVMT;

   compileParentDeclaration(node.findChild(lxParent), scope, false);

   if (scope.abstractBaseMode && test(scope.info.header.flags, elClosed | elNoCustomDispatcher) && _logic->isWithEmbeddableDispatcher(*scope.moduleScope, node)) {
      // COMPILER MAGIC : inject interface implementation
      _logic->injectInterfaceDisaptch(*scope.moduleScope, *this, node, scope.info.header.parentRef);
   }

   bool withConstructors = false;
   bool withDefaultConstructor = false;
   declareVMT(node, scope, withConstructors, withDefaultConstructor);
   if (withConstructors)
      scope.raiseError(errIllegalConstructor, node);

   if (node.existChild(lxStaticMethod))
      scope.raiseError(errIllegalStaticMethod, node);

   generateClassDeclaration(node, scope, true);

   scope.save();

//   if (!test(scope.info.header.flags, elVirtualVMT) && scope.info.staticValues.Count() > 0)
//      // do not inherit the static fields for the virtual class declaration
//      copyStaticFieldValues(node, scope);

   // NOTE : ignore auto generated methods - multi methods should be compiled after the class flags are set
   compileVMT(node, scope, true, true);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, false);

   // NOTE : compile once again only auto generated methods
   compileVMT(node, scope, true, false);

   scope.save();

   generateClassImplementation(node, scope);

   // COMPILER MAGIC : prepand a virtual identifier, terminal info should be copied from the leading attribute
   SNode attrTerminal = node.findChild(lxAttribute).firstChild(lxTerminalMask);
   node = node.prependSibling(lxVirtualReference);
   SyntaxTree::copyNode(attrTerminal, node);
}

ref_t Compiler :: resolveMessageOwnerReference(_ModuleScope& scope, ClassInfo& classInfo, ref_t reference, ref_t message,
   bool ignoreSelf)
{
   if (!classInfo.methods.exist(message, true) || ignoreSelf) {
      ClassInfo parentInfo;
      _logic->defineClassInfo(scope, parentInfo, classInfo.header.parentRef, false);

      return resolveMessageOwnerReference(scope, parentInfo, classInfo.header.parentRef, message);
   }
   else return reference;
}

ObjectInfo Compiler :: compileClosure(SNode node, ExprScope& ownerScope, InlineClassScope& scope, EAttr mode)
{
   bool noUnboxing = EAttrs::test(mode, HINT_NOUNBOXING);
   ref_t closureRef = scope.reference;
   if (test(scope.info.header.flags, elVirtualVMT))
      closureRef = scope.info.header.parentRef;

   if (test(scope.info.header.flags, elStateless)) {
      ObjectInfo retVal = ObjectInfo(okSingleton, closureRef, closureRef);
      recognizeTerminal(node, retVal, ownerScope, mode);

      // if it is a stateless class
      return retVal;
   }
   else if (test(scope.info.header.flags, elDynamicRole)) {
      scope.raiseError(errInvalidInlineClass, node);

      // idle return
      return ObjectInfo();
   }
   else {
      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         node.set(lxCreatingStruct, scope.info.size);
         node.appendNode(lxType, closureRef);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node);
      }
      else {
         // dynamic normal symbol
         node.set(lxCreatingClass, scope.info.fields.Count());
         node.appendNode(lxType, closureRef);
      }

      node.injectAndReplaceNode(lxInitializing);

      Map<ident_t, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      //int toFree = 0;
      int tempLocal = 0;
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         SNode member = node.appendNode(lxMember, (*outer_it).reference);
         SNode objNode = member.appendNode(lxVirtualReference);

         recognizeTerminal(objNode, info, ownerScope, EAttr::eaNone);
         analizeOperand(objNode, ownerScope, true, false, false);

         if ((*outer_it).preserved && !noUnboxing) {
            if (!tempLocal) {
               // save the closure in the temp variable
               tempLocal = ownerScope.newTempLocal();

               node.injectAndReplaceNode(lxAssigning);
               node.insertNode(lxTempLocal, tempLocal);
               node.injectAndReplaceNode(lxSeqExpression);
               node.appendNode(lxTempLocal, tempLocal);

               // hotfix : find initializing node again
               node = member.parentNode();
            }

            injectMemberPreserving(member, ownerScope, lxTempLocal, tempLocal, info, (*outer_it).reference);
         }

//         writer.newNode((*outer_it).preserved ? lxOuterMember : lxMember, (*outer_it).reference);
//         writeTerminal(writer, node, ownerScope, info, EAttr::eaNone);
//         writer.closeNode();

         outer_it++;
      }

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

      if (scope.info.methods.exist(scope.moduleScope->init_message)) {
         ref_t messageOwnerRef = resolveMessageOwnerReference(*scope.moduleScope, scope.info, scope.reference,
                                    scope.moduleScope->init_message);

         // if implicit constructor is declared - it should be automatically called
         node.injectAndReplaceNode(lxDirectCalling, scope.moduleScope->init_message);
         node.appendNode(lxCallTarget, messageOwnerRef);
      }

      return ObjectInfo(okObject, 0, closureRef);
   }
}

ObjectInfo Compiler :: compileClosure(SNode node, ExprScope& ownerScope, EAttr mode)
{
   ref_t nestedRef = 0;
//   //bool singleton = false;
   if (EAttrs::test(mode, HINT_ROOTSYMBOL)) {
      SymbolScope* owner = (SymbolScope*)ownerScope.getScope(Scope::ScopeLevel::slSymbol);
      if (owner) {
         nestedRef = owner->reference;
         // HOTFIX : symbol should refer to self and $self for singleton closure
         //singleton = node.existChild(lxCode);
      }
   }
   else if (EAttrs::test(mode, HINT_ROOT)) {
      MethodScope* ownerMeth = (MethodScope*)ownerScope.getScope(Scope::ScopeLevel::slMethod);
      if (ownerMeth && ownerMeth->constMode) {
         ref_t dummyRef = 0;
         // HOTFIX : recognize property constant
         IdentifierString name(ownerScope.module->resolveReference(ownerScope.getClassRefId()));
         if (name.ident().endsWith(CLASSCLASS_POSTFIX)) {
            name.truncate(name.Length() - getlength(CLASSCLASS_POSTFIX));
         }

         name.append('#');
         name.append(ownerScope.module->resolveAction(getAction(ownerMeth->message), dummyRef));

         nestedRef = ownerMeth->module->mapReference(name.c_str());
      }
   }

   if (!nestedRef) {
      nestedRef = ownerScope.moduleScope->mapAnonymous();
   }

   InlineClassScope scope(&ownerScope, nestedRef);

   // if it is a lazy expression / multi-statement closure without parameters
   SNode argNode = node.firstChild();
   if (EAttrs::testany(mode, HINT_LAZY_EXPR | HINT_INLINE_EXPR)) {
      compileAction(node, scope, SNode(), mode);
   }
   else if (argNode == lxCode) {
      compileAction(node, scope, SNode(), /*singleton ? mode | HINT_SINGLETON : */mode);
   }
   else if (node.existChild(lxCode, lxReturning)) {
      //SNode codeNode = node.findChild(lxCode, lxReturning);

      // if it is a closure / lambda function with a parameter
      EAttr actionMode = mode;
//      //if (singleton)
//      //   actionMode |= HINT_SINGLETON;

      compileAction(node, scope, node.findChild(lxIdentifier, /*lxPrivate, */lxMethodParameter/*, lxClosureMessage*/), actionMode);

//      // HOTFIX : hide code node because it is no longer required
//      codeNode = lxIdle;
   }
   // if it is a nested class
   else compileNestedVMT(node, scope);

   return compileClosure(node, ownerScope, scope, mode);
}

inline bool isConstant(SNode current)
{
   switch (current.type) {
      case lxConstantString:
      case lxConstantWideStr:
      case lxConstantSymbol:
      case lxConstantInt:
      case lxClassSymbol:
         return true;
      default:
         return false;
   }
}

inline bool isConstantList(SNode node)
{
   bool constant = true;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMember) {
         SNode object = current.findSubNodeMask(lxObjectMask);
         if (!isConstant(object)) {
            constant = false;
            break;
         }
      }

      current = current.nextNode();
   }

   return constant;
}

ObjectInfo Compiler :: compileCollection(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   if (target.reference == V_OBJARRAY) {
      target.reference = resolvePrimitiveArray(scope, scope.moduleScope->arrayTemplateReference, target.element, false);
   }
   else if (target.kind == okClass) {
      // HOTFIX : class class reference should be turned into class one
      IdentifierString className(scope.module->resolveReference(target.reference));
      className.cut(getlength(className) - getlength(CLASSCLASS_POSTFIX), getlength(CLASSCLASS_POSTFIX));

      target.reference = scope.moduleScope->mapFullReference(className);
   }
   else scope.raiseError(errInvalidOperation, node);

   int counter = 0;
   int size = _logic->defineStructSize(*scope.moduleScope, target.reference, 0);

   // HOTFIX : comment out idle symbol
   SNode dummyNode = node.prevNode(lxObjectMask);
   if (dummyNode != lxNone)
      dummyNode = lxIdle;

   node.set(lxInitializing, 0);

   // all collection memebers should be created before the collection itself
   SNode current = node.findChild(lxExpression);
   int index = 0;
   while (current != lxNone) {
      current.injectAndReplaceNode(lxMember, index++);

      SNode memberNode = current.firstChild();
      compileExpression(memberNode, scope, target.element, EAttr::eaNone);
      analizeOperand(memberNode, scope, true, true, true);

      current = current.nextNode();
      counter++;
   }

   target.kind = okObject;
   if (size < 0) {
      SNode op = node.insertNode(lxCreatingStruct, counter * (-size));
      op.appendNode(lxType, target.reference);
      op.appendNode(lxSize, -size);
      //      writer.appendNode(lxSize);
      //      writer.inject(lxStruct, counter * (-size));
   }
   else {
      if (EAttrs::test(mode, HINT_CONSTEXPR) && isConstantList(node)) {
         ref_t reference = 0;
         SymbolScope* owner = (SymbolScope*)scope.getScope(Scope::ScopeLevel::slSymbol);
         if (owner) {
            reference = mapConstant(scope.moduleScope, owner->reference);
         }
         else reference = scope.moduleScope->mapAnonymous();

         node = lxConstantList;
         node.setArgument(reference | mskConstArray);
         node.appendNode(lxType, target.reference);

         _writer.generateConstantList(node, scope.module, reference);

         target.kind = okArrayConst;
         target.param = reference;
      }
      else {
         SNode op = node.insertNode(lxCreatingClass, counter);
         op.appendNode(lxType, target.reference);
      }
   }

   return target;
}

ObjectInfo Compiler :: compileRetExpression(SNode node, CodeScope& scope, EAttr mode)
{
   bool autoMode = false;
   ref_t targetRef = 0;
   if (EAttrs::test(mode, HINT_ROOT)) {
      targetRef = scope.getReturningRef();
      if (targetRef == V_AUTO) {
         autoMode = true;
         targetRef = 0;
      }
   }

   node.injectNode(lxExpression);
   node = node.findChild(lxExpression);

   ExprScope exprScope(&scope);
   ObjectInfo info = compileExpression(node, exprScope, targetRef, mode);

   if (autoMode) {
      targetRef = resolveObjectReference(exprScope, info, true);

     _logic->validateAutoType(*scope.moduleScope, targetRef);

      scope.resolveAutoOutput(targetRef);
   }

//   // HOTFIX : implementing closure exit
//   if (EAttrs::test(mode, HINT_ROOT)) {
//      ObjectInfo retVar = scope.mapTerminal(RETVAL_VAR, false, EAttr::eaNone);
//      if (retVar.kind != okUnknown) {
//         writer.inject(lxAssigning);
//         writer.insertNode(lxField, retVar.param);
//         writer.closeNode();
//      }
//   }

   int stackSafeAttr = 0;
   if (info.kind == okSelfParam && !EAttrs::test(mode, HINT_DYNAMIC_OBJECT)) {
      stackSafeAttr = 1;
   }

   node = node.parentNode();

   analizeOperands(node, exprScope, stackSafeAttr, true);

   return info;
}

ObjectInfo Compiler :: compileCatchOperator(SNode node, ExprScope& scope, ref_t operator_id)
{
   SNode opNode = node.parentNode();

//   writer.inject(lxExpression);
//   writer.closeNode();

//   SNode current = node.firstChild();
   if (operator_id == FINALLY_OPERATOR_ID) {
      SNode finalExpr = node;
      finalExpr.injectAndReplaceNode(lxFinalblock);

      SNode objNode = finalExpr.firstChild(lxObjectMask);
      compileExpression(objNode, scope, 0, EAttr::eaNone);

      // catch operation follow the finally operation
      node = node.nextNode();
   }

   node.insertNode(lxResult);
   compileOperation(node, scope, ObjectInfo(okObject), 0, EAttr::eaNone, false);

   opNode.set(lxTrying, 0);

   return ObjectInfo(okObject);
}

ObjectInfo Compiler :: compileAltOperator(SNode node, ExprScope& scope, ObjectInfo target)
{
   SNode opNode = node.parentNode();
   SNode targetNode = opNode.firstChild(lxObjectMask);
   while (test(targetNode.type, lxOpScopeMask)) {
      targetNode = targetNode.firstChild(lxObjectMask);
   }

   SNode altNode = node;

   opNode.injectNode(lxAlt);
   opNode.set(lxSeqExpression, 0);

   SNode assignNode = opNode.insertNode(lxAssigning);
   // allocate a temporal local
   int tempLocal = scope.newTempLocal();
   assignNode.appendNode(lxTempLocal, tempLocal);
   SyntaxTree::copyNode(assignNode.appendNode(targetNode.type, targetNode.argument), targetNode);

   targetNode.set(lxTempLocal, tempLocal);

   SNode op = node.firstChild(lxOperatorMask);
   altNode.insertNode(lxTempLocal, tempLocal);

   compileMessage(op, scope, 0, target, EAttr::eaNone);

   return ObjectInfo(okObject);
}

ref_t Compiler :: resolveReferenceTemplate(_CompileScope& scope, ref_t operandRef, bool declarationMode)
{
   if (!operandRef) {
      operandRef = scope.moduleScope->superReference;
   }
   else if (isPrimitiveRef(operandRef))
      operandRef = resolvePrimitiveReference(scope, operandRef, 0, declarationMode);

   List<SNode> parameters;

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxWriter dummyWriter(dummyTree);
   dummyWriter.newNode(lxRoot);
   dummyWriter.newNode(lxType, operandRef);
   ident_t refName = scope.moduleScope->module->resolveReference(operandRef);
   if (isWeakReference(refName)) {
      dummyWriter.appendNode(lxReference, refName);
   }
   else dummyWriter.appendNode(lxGlobalReference, refName);
   dummyWriter.closeNode();
   dummyWriter.closeNode();

   parameters.add(dummyTree.readRoot().firstChild());

   return scope.moduleScope->generateTemplate(scope.moduleScope->refTemplateReference, parameters, scope.ns,
      declarationMode, nullptr);
}

//ref_t Compiler :: resolveReferenceTemplate(Scope& scope, ref_t elementRef, bool declarationMode)
//{
//   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::slNamespace);
//
//   return resolveReferenceTemplate(*scope.moduleScope, elementRef, nsScope->ns.c_str(), declarationMode);
//}

ref_t Compiler :: resolvePrimitiveArray(_CompileScope& scope, ref_t templateRef, ref_t elementRef, bool declarationMode)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   if (!elementRef)
      elementRef = moduleScope->superReference;

   // generate a reference class
   List<SNode> parameters;

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxWriter dummyWriter(dummyTree);
   dummyWriter.newNode(lxRoot);
   dummyWriter.newNode(lxType, elementRef);
   ident_t refName = moduleScope->module->resolveReference(elementRef);
   if (isWeakReference(refName)) {
      dummyWriter.appendNode(lxReference, refName);
   }
   else dummyWriter.appendNode(lxGlobalReference, refName);
   dummyWriter.closeNode();
   dummyWriter.closeNode();

   parameters.add(dummyTree.readRoot().firstChild());

   return moduleScope->generateTemplate(templateRef, parameters, scope.ns, declarationMode, nullptr);
}

ObjectInfo Compiler :: compileReferenceExpression(SNode node, ExprScope& scope, EAttr mode)
{
   ObjectInfo objectInfo = mapTerminal(node, scope, mode | HINT_REFEXPR);
   ref_t operandRef = resolveObjectReference(scope, objectInfo, true, false);
   if (!operandRef)
      operandRef = scope.moduleScope->superReference;

   ref_t targetRef = 0;
   if (objectInfo.reference == V_WRAPPER) {
      // if the reference is passed further - do nothing
      targetRef = operandRef;
   }
   else {
      // generate an reference class
      targetRef = resolveReferenceTemplate(scope, operandRef, false);

      SNode opNode = node.parentNode();
      ObjectInfo retVal = convertObject(opNode, scope, targetRef, objectInfo, mode);

      if (retVal.kind == okUnknown)
         scope.raiseError(errInvalidOperation, node);
   }

   return ObjectInfo(okObject, 0, targetRef);
}

ObjectInfo Compiler :: compileVariadicUnboxing(SNode node, ExprScope& scope, EAttr mode)
{
   ObjectInfo objectInfo = mapTerminal(node, scope, mode);
   // HOTFIX : refresh node to see the changes
   node.refresh();
   analizeOperand(node, scope, false, true, true);

   ref_t operandRef = resolveObjectReference(scope, objectInfo, false, false);
   if (operandRef == V_ARGARRAY && EAttrs::test(mode, HINT_PARAMETER)) {
      objectInfo.reference = V_UNBOXEDARGS;

      node.injectAndReplaceNode(lxArgArray);
   }
   else if (_logic->isArray(*scope.moduleScope, operandRef)) {
      objectInfo.reference = V_UNBOXEDARGS;

      node.injectAndReplaceNode(lxArgArray, (ref_t)-1);
   }
   else scope.raiseError(errNotApplicable, node);

   return objectInfo;
}

ObjectInfo Compiler :: compileCastingExpression(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   ref_t targetRef = 0;
   if (target.kind == okClass || target.kind == okClassSelf) {
      targetRef = target.param;
   }
   else targetRef = resolveObjectReference(scope, target, true);

   ObjectInfo retVal(okObject, 0, targetRef);

   int paramCount = SyntaxTree::countNodeMask(node, lxObjectMask);

   if (paramCount == 1) {
      SNode current = node.nextNode();

      // if it is a cast expression
      ObjectInfo object = compileExpression(current, scope, /*targetRef*/0, mode);

      SNode opNode = node.parentNode();
      retVal = convertObject(opNode, scope, targetRef, object, mode);

      if(retVal.kind == okUnknown)
         scope.raiseError(errInvalidOperation, node);
   }
   else scope.raiseError(errInvalidOperation, node.parentNode());

   return retVal;
}

ObjectInfo Compiler :: compileBoxingExpression(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   ref_t targetRef = 0;
   if (target.kind == okClass || target.kind == okClassSelf) {
      targetRef = target.param;
   }
   else throw InternalError("Not implemented"); // !! temporal

   EAttr paramsMode = EAttr::eaNone;
   bool variadicOne = false;
   bool inlineArg = false;
   int paramCount = SyntaxTree::countNodeMask(node, lxObjectMask);
   ref_t implicitSignatureRef = compileMessageParameters(node, scope, paramsMode, variadicOne, inlineArg);
   if (inlineArg)
      scope.raiseError(errInvalidOperation, node);

   SNode exprNode = node.parentNode();
   ref_t messageRef = overwriteArgCount(scope.moduleScope->constructor_message, paramCount + 1);
   int stackSafeAttr = 0;
   if (target.reference == V_OBJARRAY && paramCount == 1) {
      // HOTFIX : if it is an array creation
      ref_t roperand = 0;
      scope.module->resolveSignature(implicitSignatureRef, &roperand);

      int operationType = _logic->resolveNewOperationType(*scope.moduleScope, targetRef, roperand);
      if (operationType != 0) {
         // if it is a primitive operation
         _logic->injectNewOperation(exprNode, *scope.moduleScope, operationType, targetRef, target.element);

         // HOTFIX : remove class symbol - the array will be created directly
         SNode classNode = exprNode.firstChild(lxObjectMask);
         classNode = lxIdle;

         // mark the argument as a stack safe
         analizeOperands(exprNode, scope, 1, true);

         return ObjectInfo(okObject, 0, target.reference, target.element, 0);
      }
      else scope.raiseError(errInvalidOperation, node.parentNode());
   }
   else messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, false, stackSafeAttr);

   bool dummy = false;
   ObjectInfo retVal = compileMessage(exprNode, scope, target, messageRef, mode | HINT_SILENT, stackSafeAttr, dummy);

   if (!resolveObjectReference(scope, retVal, false)) {
      scope.raiseError(errDefaultConstructorNotFound, exprNode);
   }

   // HOTFIX : set the correct template reference (weak template one) if required
   retVal.reference = targetRef;

   return retVal;
}

ObjectInfo Compiler :: compileOperation(SNode& node, ExprScope& scope, ObjectInfo objectInfo, ref_t expectedRef,
   EAttr mode, bool propMode)
{
   SNode current = node.firstChild(lxOperatorMask);

   switch (current.type) {
      case lxCollection:
         objectInfo = compileCollection(current, scope, objectInfo, mode);
         break;
      case lxMessage:
         if (EAttrs::test(mode, HINT_LOOP)) {
            EAttrs subMode(mode, HINT_LOOP);

            objectInfo = compileMessage(current, scope, expectedRef, objectInfo, subMode);
            current.parentNode().injectAndReplaceNode(lxLooping);
         }
         else if (propMode) {
            objectInfo = compilePropAssigning(current, scope, objectInfo);
         }
         else objectInfo = compileMessage(current, scope, expectedRef, objectInfo, mode);
         break;
      case lxNewOperation:
         objectInfo = compileBoxingExpression(current, scope, objectInfo, mode);
         break;
      case lxCastOperation:
         objectInfo = compileCastingExpression(current, scope, objectInfo, mode);
         break;
//      case lxTypecast:
//         objectInfo = compileBoxingExpression(writer, current, scope, objectInfo, mode);
//         break;
      case lxAssign:
         objectInfo = compileAssigning(current, scope, objectInfo, current.argument == -1);
         break;
      case lxArrOperator:
         if (propMode) {
            current.setArgument(SET_REFER_OPERATOR_ID);
         }
      case lxOperator:
         objectInfo = compileOperator(current, scope, objectInfo, mode);
         break;
//      case lxWrapping:
//         objectInfo = compileWrapping(writer, current, scope, objectInfo, EAttrs::test(mode, HINT_CALL_MODE));
//         break;
   }
   node.refresh();

   return objectInfo;
}

ref_t Compiler :: mapTemplateAttribute(SNode node, Scope& scope)
{
   SNode terminalNode = node.firstChild(lxTerminalMask);
   IdentifierString templateName(terminalNode.identifier());
   int paramCounter = 0;
   SNode current = node.findChild(lxType);
   while (current != lxNone) {
      if (current.compare(lxType, lxTemplateParam)) {
         paramCounter++;
      }
      else scope.raiseError(errInvalidOperation, node);

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(paramCounter);

   // NOTE : check it in declararion mode - we need only reference
   return resolveTypeIdentifier(scope, templateName.c_str(), terminalNode.type, true, false);
}

//ref_t Compiler :: mapTypeAttribute(SNode member, Scope& scope)
//{
//   ref_t ref = member.argument ? member.argument : resolveImplicitIdentifier(scope, member.firstChild(lxTerminalMask));
//   if (!ref)
//      scope.raiseError(errUnknownClass, member);
//
//   return ref;
//}

SNode Compiler :: injectAttributeIdentidier(SNode current, Scope& scope)
{
   ident_t refName = scope.module->resolveReference(current.argument);

   SNode terminalNode = current.firstChild(lxTerminalMask);
   if (terminalNode != lxNone) {
      if (isWeakReference(refName)) {
         terminalNode.set(lxReference, refName);
      }
      else terminalNode.set(lxGlobalReference, refName);
   }

   return terminalNode;
}

void Compiler :: compileTemplateAttributes(SNode current, List<SNode>& parameters, Scope& scope, bool declarationMode)
{
//   if (current.compare(lxIdentifier, lxReference))
//      current = current.nextNode();
//
//   ExpressionAttributes attributes;
   while (current != lxNone) {
      if (current.compare(lxType, lxArrayType)) {
         ref_t typeRef = current.argument;
         if (!typeRef || typeRef == V_TEMPLATE) {
            typeRef = resolveTypeAttribute(current, scope, declarationMode, false);
            current.set(lxType, typeRef);

            SNode terminalNode = injectAttributeIdentidier(current, scope);
         }

//         SNode targetNode = current.firstChild();
//         bool templateOne = targetNode.argument == V_TEMPLATE;
//         targetNode.set(lxTarget, typeRef);

//         // HOTFIX : inject the reference and comment the target nodes out
//         if (templateOne) {
//            do {
//               terminalNode = terminalNode.nextNode();
//               if (terminalNode == lxTypeAttribute) {
//                  // NOTE : Type attributes should be removed to correctly compile the array of templates
//                  terminalNode = lxIdle;
//               }
//            } while (terminalNode != lxNone);
//         }
//
         parameters.add(/*targetNode*/current);
      }

      current = current.nextNode();
   }
}

//bool Compiler :: isTemplateParameterDeclared(SNode node, Scope& scope)
//{
//   SNode current = node.firstChild();
//   if (current == lxIdentifier)
//      current = current.nextNode();
//
//   while (current != lxNone) {
//      if (current == lxTarget) {
//         ref_t arg = mapTypeAttribute(current, scope);
//         if (!scope.moduleScope->isClassDeclared(arg))
//            return 0;
//      }
//
//      current = current.nextNode();
//   }
//
//   return true;
//}

ref_t Compiler :: resolveTemplateDeclarationUnsafe(SNode node, Scope& scope, bool declarationMode)
{
   // generate an reference class - inner implementation
   List<SNode> parameters;

   compileTemplateAttributes(node.firstChild(), parameters, scope, declarationMode);

   ref_t templateRef = mapTemplateAttribute(node, scope);
   if (!templateRef)
      scope.raiseError(errInvalidHint, node);

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
   return scope.moduleScope->generateTemplate(templateRef, parameters, ns->nsName.c_str(), declarationMode, nullptr);
}

ref_t Compiler :: resolveTemplateDeclaration(SNode node, Scope& scope, bool declarationMode)
{
   // generate an reference class - safe implementation
   if (declarationMode) {
      // HOTFIX : clone the type attribute tree to prevent it from modifications
      SyntaxTree dummyTree;
      SyntaxWriter dummyWriter(dummyTree);
      dummyWriter.newNode(node.type);
      SyntaxTree::copyNode(dummyWriter, node);
      dummyWriter.closeNode();

      SNode dummyNode = dummyTree.readRoot();
      return resolveTemplateDeclarationUnsafe(dummyTree.readRoot(), scope, declarationMode);
   }
   else return resolveTemplateDeclarationUnsafe(node, scope, declarationMode);
}

ref_t Compiler :: resolveTypeAttribute(SNode node, Scope& scope, bool declarationMode, bool allowRole)
{
   ref_t typeRef = 0;
   if (test(node.type, lxTerminalMask)) {
      typeRef = resolveTypeIdentifier(scope, node, declarationMode, allowRole);
   }
   else if (node == lxArrayType) {
      typeRef = resolvePrimitiveArray(scope,
         scope.moduleScope->arrayTemplateReference,
         resolveTypeAttribute(node.firstChild(), scope, declarationMode, false), declarationMode);
   }
   else {
      typeRef = node.argument;

      if (typeRef == V_TEMPLATE) {
         typeRef = resolveTemplateDeclaration(node, scope, declarationMode);
      }
      else if (!typeRef)
         typeRef = resolveTypeIdentifier(scope, node.firstChild(lxTerminalMask), declarationMode, allowRole);
   }

   validateType(scope, node, typeRef, declarationMode, allowRole);

   return typeRef;
}

EAttr Compiler :: recognizeExpressionAttributes(SNode& current, Scope& scope, ref_t& typeRef, bool& newVariable)
{
   EAttrs exprAttr = EAttr::eaNone;

   // HOTFIX : skip bookmark reference
   if (current == lxBookmarkReference)
      current = current.nextNode();

   while (current == lxAttribute) {
      int value = current.argument;
      if (!_logic->validateExpressionAttribute(value, exprAttr, newVariable))
         scope.raiseError(errInvalidHint, current);

      if (value == V_AUTO)
         typeRef = value;

      current = current.nextNode();
   }

   return exprAttr;
}

EAttr Compiler :: declareExpressionAttributes(SNode& current, ExprScope& scope, EAttr)
{
   bool  newVariable = false;
   ref_t typeRef = 0;

   EAttrs exprAttr = recognizeExpressionAttributes(current, scope, typeRef, newVariable);

   if (exprAttr.testAndExclude(EAttr::eaIgnoreDuplicates)) {
      scope.ignoreDuplicates = true;
   }

   if (current.compare(lxType, lxArrayType) && test(current.nextNode(), lxTerminalMask)) {
      if (typeRef == 0) {
         typeRef = resolveTypeAttribute(current, scope, false, false);

         newVariable = true;
      }
      else scope.raiseError(errIllegalOperation, current);

      current = current.nextNode();
   }

   if (newVariable) {
      if (!typeRef)
         typeRef = scope.moduleScope->superReference;

      // COMPILER MAGIC : make possible to ignore duplicates - used for some code templates
      if (scope.ignoreDuplicates && scope.checkLocal(current.identifier())) {
         // ignore duplicates
      }
      else declareVariable(current, scope, typeRef/*, dynamicSize*/, !exprAttr.testany(HINT_REFOP));
   }

   return exprAttr;
}

//inline SNode findLeftMostNode(SNode current, LexicalType type)
//{
//   if (current == lxExpression) {
//      return findLeftMostNode(current.firstChild(), type);
//   }
//   else return current;
//}

ObjectInfo Compiler :: compileRootExpression(SNode node, CodeScope& scope, ref_t targetRef, EAttr mode)
{
   // renaming top expression into sequence one to be used in root boxing routines
   node.set(lxSeqExpression, (ref_t)-1);

   // inject a root expression
   node = node.injectNode(lxExpression);

   ExprScope exprScope(&scope);
   ObjectInfo retVal = compileExpression(node, exprScope, targetRef, mode);

   node = node.parentNode();

   int stackSafeAttr = EAttrs::test(mode, HINT_DYNAMIC_OBJECT) ? 0 : 1;
   analizeOperands(node, exprScope, stackSafeAttr, true);

   return retVal;
}

void Compiler :: recognizeTerminal(SNode& terminal, ObjectInfo object, ExprScope& scope, EAttr mode)
{
   if (EAttrs::test(mode, HINT_VIRTUALEXPR))
      return;

   // injecting an expression node
   terminal.injectAndReplaceNode(lxExpression);

   if (!EAttrs::test(mode, HINT_NODEBUGINFO))
      terminal.insertNode(lxBreakpoint, dsStep);

   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
         break;
      case okSymbol:
//         scope.moduleScope->validateReference(terminal, object.param | mskSymbolRef);
         terminal.set(lxSymbolReference, object.param);
         break;
      case okClass:
      case okClassSelf:
         terminal.set(lxClassSymbol, object.param);
         break;
      case okExtension:
         if (!EAttrs::test(mode, HINT_CALLOP)) {
            scope.raiseWarning(WARNING_LEVEL_3, wrnExplicitExtension, terminal);
         }
      case okConstantSymbol:
      case okSingleton:
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
      case okUIntConstant:
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
      case okParam:
      case okLocal:
         setParamTerminal(terminal, scope, object, mode, lxLocal);
         break;
      case okParamField:
         setParamFieldTerminal(terminal, scope, object, mode, lxLocal);
         break;
      case okSelfParam:
//         if (EAttrs::test(mode, HINT_RETEXPR))
//            mode = mode | HINT_NOBOXING;

         setParamTerminal(terminal, scope, object, mode, lxSelfLocal);
         break;
      case okSuper:
         setSuperTerminal(terminal, scope, object, mode, lxSelfLocal);
         break;
      case okReadOnlyField:
      case okField:
      case okOuter:
      case okOuterSelf:
         terminal.set(lxFieldExpression, 0);
         terminal.appendNode(lxSelfLocal, 1);
         terminal.appendNode(lxField, object.param);
         break;
      case okStaticField:
//         if ((int)object.param < 0) {
//            // if it is a normal static field - field expression should be used
//            writer.newNode(lxFieldExpression, 0);
//            writer.appendNode(lxClassRefField, 1);
//            writer.appendNode(lxStaticField, object.param);
//         }
         // if it is a sealed static field
         /*else */terminal.set(lxStaticField, object.param);
         break;
//      case okClassStaticField:
//         writer.newNode(lxFieldExpression, 0);
//         if (!object.param) {
//            writer.appendNode(lxSelfLocal, 1);
//         }
//         else writer.appendNode(lxClassSymbol, object.param);
//         writer.appendNode(lxStaticField, object.extraparam);
//         break;
      case okStaticConstantField:
         if ((int)object.param < 0) {
            terminal.set(lxFieldExpression, 0);
            terminal.appendNode(lxSelfLocal, 1);
            terminal.appendNode(lxClassRef, scope.getClassRefId());
            terminal.appendNode(lxStaticConstField, object.param);
         }
         else terminal.set(lxStaticField, object.param);
         break;
      case okClassStaticConstantField:
         if ((int)object.param < 0) {
            terminal.set(lxFieldExpression, 0);
            terminal.appendNode(lxSelfLocal, 1);
            terminal.appendNode(lxStaticConstField, object.param);
         }
         else throw InternalError("Not yet implemented"); // !! temporal
         break;
      case okOuterField:
      case okOuterReadOnlyField:
         terminal.set(lxFieldExpression, 0);
         terminal.appendNode(lxSelfLocal, 1);
         terminal.appendNode(lxField, object.param);
         terminal.appendNode(lxField, object.extraparam);
         break;
//      case okOuterStaticField:
//         writer.newNode(lxFieldExpression, 0);
//         writer.newNode(lxFieldExpression, 0);
//         writer.appendNode(lxField, object.param);
//         //writer.appendNode(lxClassRefAttr, object.param);
//         writer.closeNode();
//         writer.appendNode(lxStaticField, object.extraparam);
//         break;
      case okFieldAddress:
      case okReadOnlyFieldAddress:
         terminal.set(lxFieldExpression, 0);
         terminal.appendNode(lxSelfLocal, 1);
         if (object.param || (!EAttrs::test(mode, HINT_PROP_MODE)
            && (_logic->defineStructSize(*scope.moduleScope, object.reference, 0) & 3) != 0))
         {
            // if it a field address (except the first one, which size is 4-byte aligned)
            terminal.appendNode(lxFieldAddress, object.param);
            // the field address expression should always be boxed (either local or dynamic)
            mode = EAttrs(mode, HINT_NOBOXING);
         }

         if (isPrimitiveArrRef(object.reference)) {
            int size = defineFieldSize(scope, object.param);

            setVariableTerminal(terminal, scope, object, mode, lxFieldExpression, size);
         }
         else setVariableTerminal(terminal, scope, object, mode, lxFieldExpression);
         break;
      case okLocalAddress:
         setVariableTerminal(terminal, scope, object, mode, lxLocalAddress);
         break;
      case okMessage:
         setVariableTerminal(terminal, scope, object, mode, lxLocalAddress);
         break;
      case okNil:
         terminal.set(lxNil, 0/*object.param*/);
         break;
      case okMessageConstant:
         terminal.set(lxMessageConstant, object.param);
         break;
      case okExtMessageConstant:
         terminal.set(lxExtMessageConstant, object.param);
         break;
      case okMessageNameConstant:
         terminal.set(lxSubjectConstant, object.param);
         break;
//////      case okBlockLocal:
//////         terminal.set(lxBlockLocal, object.param);
//////         break;
      case okParams:
      {
         ref_t r = resolvePrimitiveReference(scope, object.reference, object.element, false);
         if (!r)
            throw InternalError("Cannot resolve variadic argument template");

         setParamsTerminal(terminal, scope, object, mode, r);
         break;
      }
//      case okObject:
//         writer.newNode(lxResult, 0);
//         break;
//      case okConstantRole:
//         writer.newNode(lxConstantSymbol, object.param);
//         break;
      case okExternal:
         return;
      case okInternal:
         terminal.set(lxInternalRef, object.param);
         return;
      //case okPrimitive:
      //case okPrimCollection:
      //{
      //   terminal.set(lxSeqExpression);
      //   SNode exprNode = terminal.appendNode(object.kind == okPrimitive ? lxCreatingStruct : lxCreatingClass, object.param);
      //   //         writer.newBookmark();
      //   if (EAttrs::test(mode, HINT_AUTOSIZE) && !writeSizeArgument(exprNode))
      //      scope.raiseError(errInvalidOperation, terminal);

      //   //         writer.appendNode();
      //   //         writer.inject(lxSeqExpression);
      //   //         writer.removeBookmark();
      //   break;
      //}
   }
   //
   //   writeTarget(writer, resolveObjectReference(scope, object, false), object.element);
   //
   //      writeTerminalInfo(writer, terminal);
}

ObjectInfo Compiler :: mapIntConstant(ExprScope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendHex(integer);

   return ObjectInfo(okIntConstant, scope.module->mapConstant((const char*)s), V_INT32, 0, integer);
}

ObjectInfo Compiler :: mapRealConstant(ExprScope& scope, double val)
{
   String<char, 30> s;

   s.appendDouble(val);
   // HOT FIX : to support 0r constant
   if (s.Length() == 1) {
      s.append(".0");
   }

   return ObjectInfo(okRealConstant, scope.moduleScope->module->mapConstant((const char*)s), V_REAL64);
}

ObjectInfo Compiler :: mapMetaField(ident_t token)
{
   if (token.compare(META_INFO_NAME)) {
      return ObjectInfo(okMetaField, ClassAttribute::caInfo);
   }
   else return ObjectInfo();
}

ObjectInfo Compiler :: mapTerminal(SNode terminal, ExprScope& scope, EAttr mode)
{
   //   EAttrs mode(modeAttr);
   ident_t token = terminal.identifier();
   ObjectInfo object;

   if (EAttrs::testany(mode, HINT_INTERNALOP | HINT_MEMBER | HINT_METAFIELD | HINT_EXTERNALOP | HINT_FORWARD
      | HINT_MESSAGEREF | HINT_SUBJECTREF))
   {
      bool invalid = false;
      if (EAttrs::test(mode, HINT_INTERNALOP)) {
         if (terminal == lxReference) {
            object = ObjectInfo(okInternal, scope.module->mapReference(token), V_INT32);
         }
         else if (terminal == lxGlobalReference) {
            object = ObjectInfo(okInternal, scope.moduleScope->mapFullReference(token), V_INT32);
         }
         else invalid = true;
      }
      else if (EAttrs::test(mode, HINT_EXTERNALOP)) {
         object = ObjectInfo(okExternal, 0, V_INT32);
      }
      else if (EAttrs::test(mode, HINT_MEMBER)) {
         object = scope.mapMember(token);
      }
      else if (EAttrs::test(mode, HINT_METAFIELD)) {
         object = mapMetaField(token);
      }
      else if (EAttrs::test(mode, HINT_FORWARD)) {
         IdentifierString forwardName(FORWARD_MODULE, "'", token);

         object = scope.mapTerminal(forwardName.ident(), true, EAttr::eaNone);
      }
      else if (EAttrs::test(mode, HINT_MESSAGEREF)) {
         // HOTFIX : if it is an extension message
         object = compileMessageReference(terminal, scope);
      }
      else if (EAttrs::test(mode, HINT_SUBJECTREF)) {
         object = compileSubjectReference(terminal, scope, mode);
      }

      if (invalid)
         scope.raiseError(errInvalidOperation, terminal);
   }
   else {
      switch (terminal.type) {
         case lxLiteral:
            object = ObjectInfo(okLiteralConstant, scope.moduleScope->module->mapConstant(token), scope.moduleScope->literalReference);
            break;
         case lxWide:
            object = ObjectInfo(okWideLiteralConstant, scope.moduleScope->module->mapConstant(token), scope.moduleScope->wideReference);
            break;
         case lxCharacter:
            object = ObjectInfo(okCharConstant, scope.moduleScope->module->mapConstant(token), scope.moduleScope->charReference);
            break;
         case lxInteger:
         {
            int integer = token.toInt();
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, terminal);

            object = mapIntConstant(scope, integer);
            break;
         }
         case lxLong:
         {
            String<char, 30> s("_"); // special mark to tell apart from integer constant
            s.append(token, getlength(token) - 1);

            token.toLongLong(10, 1);
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, terminal);

            object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s), V_INT64);
            break;
         }
         case lxHexInteger:
         {
            int integer = token.toULong(16);
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, terminal);

            object = mapIntConstant(scope, integer);
            break;
         }
         case lxReal:
         {
            double val = token.toDouble();
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, terminal);

            object = mapRealConstant(scope, val);
            break;
         }
         case lxGlobalReference:
            object = scope.mapGlobal(token.c_str());
            break;
         case lxMetaConstant:
            // NOTE : is not allowed to be used outside const initialization
            scope.raiseError(errIllegalOperation, terminal);
            break;
         case lxExplicitConst:
         {
            // try to resolve explicit constant
            size_t len = getlength(token);

            IdentifierString action(token + len - 1);
            action.append(CONSTRUCTOR_MESSAGE);

            ref_t dummyRef = 0;
            ref_t actionRef = scope.module->mapAction(action, scope.module->mapSignature(&scope.moduleScope->literalReference, 1, false), dummyRef);

            action.copy(token, len - 1);
            object = ObjectInfo(okExplicitConstant, scope.moduleScope->module->mapConstant(action), 0, 0, actionRef);
            break;
         }
         case lxSubjectRef:
            object = compileSubjectReference(terminal, scope, mode);
            break;
         default:
            object = scope.mapTerminal(token, terminal == lxReference, mode & HINT_SCOPE_MASK);
            break;
      }
   }

   if (object.kind == okExplicitConstant) {
      // replace an explicit constant with the appropriate object
      recognizeTerminal(terminal, ObjectInfo(okLiteralConstant, object.param), scope, mode);

      ref_t messageRef = encodeMessage(object.extraparam, 2, 0);
      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
      Pair<ref_t, ref_t>  constInfo = nsScope->extensions.get(messageRef);
      if (constInfo.value1 != 0) {
         ref_t signRef = 0;
         scope.module->resolveAction(object.extraparam, signRef);
         if (!_logic->injectConstantConstructor(terminal, *scope.moduleScope, *this, constInfo.value1, messageRef))
            scope.raiseError(errInvalidConstant, terminal);
      }
      else scope.raiseError(errInvalidConstant, terminal);

      object = ObjectInfo(okObject, constInfo.value1);
   }
   else if (object.kind == okUnknown) {
      scope.raiseError(errUnknownObject, terminal);
   }
   else recognizeTerminal(terminal, object, scope, mode);

   return object;
}

ObjectInfo Compiler :: mapObject(SNode node, ExprScope& scope, EAttr exprMode)
{
   EAttrs mode(exprMode & HINT_OBJECT_MASK);
   ObjectInfo result;

   SNode current = node.firstChild();
   if (current.compare(lxAttribute, lxType, lxArrayType, lxBookmarkReference)) {
      mode.include(declareExpressionAttributes(current, scope, exprMode));

      //      if (targetMode.testAndExclude(HINT_INLINEARGMODE)) {
      //         noPrimMode = true;
      //         inlineArgMode = true;
      //      }
   }

   if (mode.testAndExclude(HINT_NEWOP)) {
      if (current == lxArrayType) {
         // if it is an array creation
         result.kind = okClass;
         result.element = resolveTypeAttribute(current.firstChild(), scope, false, false);
         result.param = resolvePrimitiveArray(scope,
            scope.moduleScope->arrayTemplateReference, result.element, false);
         result.reference = V_OBJARRAY;
      }
      else {
         ref_t typeRef = resolveTypeAttribute(current, scope, false, false);

         result = mapClassSymbol(scope, typeRef);
      }

      recognizeTerminal(current, result, scope, mode);

      SNode mssgNode = node.findChild(lxMessage, lxCollection);
      if (mssgNode == lxMessage) {
         mssgNode.set(lxNewOperation, CONSTRUCTOR_MESSAGE);
      }
      else if (mssgNode == lxNone)
         scope.raiseError(errInvalidOperation, node);
   }
   else if (mode.testAndExclude(HINT_CASTOP)) {
      ref_t typeRef = resolveTypeAttribute(current, scope, false, false);

      result = mapClassSymbol(scope, typeRef);

      SNode mssgNode = node.findChild(lxMessage);
      if (mssgNode != lxNone) {
         mssgNode.set(lxCastOperation, 0);
      }
      else scope.raiseError(errInvalidOperation, node);
   }
   else if (mode.testAndExclude(HINT_CLASSREF)) {
      ref_t typeRef = resolveTypeAttribute(current, scope, false, false);
      result = mapClassSymbol(scope, typeRef);

      recognizeTerminal(current, result, scope, mode);
   }
   else if (mode.testAndExclude(HINT_REFOP)) {
      result = compileReferenceExpression(current, scope, mode);
   }
   else if (mode.testAndExclude(HINT_PARAMSOP)) {
	   result = compileVariadicUnboxing(current, scope, mode);
   }
   else if (mode.testAndExclude(HINT_YIELD_EXPR)) {
      compileYieldExpression(current, scope, mode);
   }
   else if (mode.test(HINT_LAZY_EXPR)) {
      result = compileClosure(node, scope, mode);
   }
   else {
      switch (current.type) {
//         case lxCollection:
//            result = compileCollection(writer, node, scope, ObjectInfo(okObject, 0, V_OBJARRAY, scope.moduleScope->superReference, 0));
//            break;
         case lxCode:
            current = lxCodeExpression;
         case lxCodeExpression:
            if (EAttrs::test(exprMode, HINT_EXTERNALOP)) {
               current.injectAndReplaceNode(lxExternFrame);

               current = current.firstChild(lxObjectMask);
            }
            result = compileSubCode(current, scope, false);
            break;
         case lxType:
            if (mode.testAndExclude(HINT_MESSAGEREF)) {
               // HOTFIX : if it is an extension message
               result = compileMessageReference(current, scope);

               recognizeTerminal(current, result, scope, mode);
            }
            else result = compileTypeSymbol(current, scope, mode);
            break;
         case lxNestedClass:
            result = compileClosure(current, scope, mode/* & HINT_CLOSURE_MASK*/);
            break;
         case lxClosureExpr:
            result = compileClosure(current, scope, mode/* & HINT_CLOSURE_MASK*/);
            break;
         case lxExpression:
            result = compileExpression(current, scope, 0, mode);
            break;
         case lxSwitching:
            compileSwitch(current, scope);

            result = ObjectInfo(okObject);
            break;
         case lxIdle:
            result = ObjectInfo(okUnknown);
            break;
         default:
            result = mapTerminal(current, scope, mode);
            break;
      }
   }

   if (mode.test(HINT_INLINEARGMODE)) {
      result.element = result.reference;
      result.reference = V_INLINEARG;
   }

   return result;
}

inline bool isAssigmentOp(SNode node)
{
   return node == lxAssign/* || (node == lxOperator && node.argument == -1)*/;
}

inline bool isCallingOp(SNode node)
{
   return node == lxMessage;
}

ObjectInfo Compiler :: compileExpression(SNode& node, ExprScope& scope, ref_t targetRef, EAttr mode)
{
   EAttrs objMode(mode, HINT_PROP_MODE);

   SNode operationNode = node.firstChild(lxOperatorMask);
   if (isAssigmentOp(operationNode)) {
      objMode.include(HINT_PROP_MODE);
      mode = mode | HINT_PROP_MODE;
   }
   else if (isCallingOp(operationNode)) {
      if (EAttrs::testany(mode, HINT_NOBOXING)) {
         objMode.exclude(HINT_NOBOXING);

         mode = EAttrs::exclude(mode, HINT_NOBOXING);
      }
      objMode.include(HINT_CALLOP);
   }

   return compileExpression(node, scope,
      mapObject(node, scope, objMode), targetRef, mode);
}

ObjectInfo Compiler :: compileExpression(SNode& node, ExprScope& scope, ObjectInfo objectInfo, ref_t exptectedRef, EAttr modeAttrs)
{
   bool noPrimMode = EAttrs::test(modeAttrs, HINT_NOPRIMITIVES);
   bool noUnboxing = EAttrs::test(modeAttrs, HINT_NOUNBOXING);
//   bool boxingMode = false;

   EAttrs mode(modeAttrs, HINT_OBJECT_MASK);
   ObjectInfo retVal = compileOperation(node, scope, objectInfo, exptectedRef, mode, EAttrs::test(modeAttrs, HINT_PROP_MODE));

   ref_t sourceRef = resolveObjectReference(scope, retVal, false/*, exptectedRef*/);
   if (!exptectedRef && isPrimitiveRef(sourceRef) && noPrimMode) {
      if (objectInfo.reference == V_INLINEARG && EAttrs::test(modeAttrs, HINT_PARAMETER)) {
         // HOTFIX : do not resolve inline argument
      }
      else exptectedRef = resolvePrimitiveReference(scope, sourceRef, objectInfo.element, false);
   }

   if (exptectedRef) {
      if (noUnboxing)
         mode = mode | HINT_NOUNBOXING;

      retVal = convertObject(node, scope, exptectedRef, retVal, mode);
      if (retVal.kind == okUnknown) {
         scope.raiseError(errInvalidOperation, node);
      }
   }

   return retVal;
}

ObjectInfo Compiler :: compileSubCode(SNode codeNode, ExprScope& scope, bool branchingMode)
{
   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   CodeScope subScope(codeScope);

   if (branchingMode && codeNode == lxExpression) {
      //HOTFIX : inline branching operator
      codeNode.injectAndReplaceNode(lxCode);

      compileRootExpression(codeNode.firstChild(), subScope, 0, HINT_ROOT | HINT_DYNAMIC_OBJECT);
   }
   else compileCode(codeNode, subScope);

   // preserve the allocated space
   subScope.syncStack(codeScope);

   return ObjectInfo(okObject);
}

void Compiler :: compileEmbeddableRetExpression(SNode node, ExprScope& scope)
{
   node.injectNode(lxExpression);
   node = node.findChild(lxExpression);
   node.injectNode(lxExpression);

   ObjectInfo retVar = scope.mapTerminal(RETVAL_ARG, false, HINT_PROP_MODE);

   SNode refNode = node.insertNode(lxVirtualReference);
   recognizeTerminal(refNode, retVar, scope, HINT_NOBOXING);

   compileAssigning(node.firstChild(), scope, retVar, false);
}

ObjectInfo Compiler :: compileCode(SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxExpression:
            compileRootExpression(current, scope, 0, HINT_ROOT);
            break;
         case lxReturning:
         {
            needVirtualEnd = false;

            //if (test(scope.getMessageID(), SPECIAL_MESSAGE))
            //   scope.raiseError(errIllegalOperation, current);

            if (scope.withEmbeddableRet()) {
               ExprScope exprScope(&scope);

               retVal = scope.mapTerminal(SELF_VAR, false, EAttr::eaNone);

               compileEmbeddableRetExpression(current, exprScope);

               current.injectAndReplaceNode(lxSeqExpression);
               SNode retNode = current.appendNode(lxReturning);
               retNode.appendNode(lxExpression);
               SNode objNode = retNode.firstChild();
               recognizeTerminal(objNode, retVal, exprScope, HINT_NODEBUGINFO | HINT_NOBOXING);
            }
            else retVal = compileRetExpression(current, scope, HINT_ROOT/* | HINT_RETEXPR*/);
            break;
         }
         case lxCode:
         {
            // compile sub code
            ExprScope exprScope(&scope);
            compileSubCode(current, exprScope, false);

            break;
         }
         case lxEOP:
            needVirtualEnd = false;
            current.injectNode(lxTerminalMask); // injecting virtual terminal token
            current.insertNode(lxBreakpoint, dsEOP);
            break;
      }

//      scope.freeSpace();

      current = current.nextNode();
   }

   if (needVirtualEnd) {
      SNode eop = node.appendNode(lxEOP);
      eop.insertNode(lxBreakpoint, dsVirtualEnd);
   }

   return retVal;
}

void Compiler :: compileExternalArguments(SNode node, ExprScope& scope, SNode callNode)
{
   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone && current != callNode) {
      SNode objNode = current;
      ref_t typeRef = current.findChild(lxType).argument;
      if (objNode == lxExpression) {
         objNode = current.findSubNodeMask(lxObjectMask);
      }

      if (objNode == lxConstantInt) {
         // move constant directly to ext call
         SyntaxTree::copyNode(objNode, callNode.appendNode(lxExtIntConst));

         current = lxIdle;
      }
      else {
         if (objNode.compare(lxBoxableExpression, lxCondBoxableExpression)) {
            if (!typeRef)
               typeRef = objNode.findChild(lxType).argument;

            analizeOperand(objNode, scope, false, false, false);

            objNode = objNode.findSubNodeMask(lxObjectMask);
         }
         if (objNode.type != lxSymbolReference && (!test(objNode.type, lxOpScopeMask) || objNode == lxFieldExpression)) {
            if (_logic->isCompatible(*scope.moduleScope, V_DWORD, typeRef) && !_logic->isVariable(*scope.moduleScope, typeRef)) {
               // if it is a integer variable
               SyntaxTree::copyNode(objNode, callNode
                  .appendNode(lxExtIntArg)
                  .appendNode(objNode.type, objNode.argument));
            }
            else SyntaxTree::copyNode(objNode, callNode
               .appendNode(objNode.type, objNode.argument));

            current = lxIdle;
         }
         else scope.raiseError(errInvalidOperation, current); // !! temporal
      }

      current = current.nextNode(lxObjectMask);
   }
}

ObjectInfo Compiler :: compileExternalCall(SNode node, ExprScope& scope, ref_t expectedRef, EAttr)
{
   ObjectInfo retVal(okExternal);

   _ModuleScope* moduleScope = scope.moduleScope;

   bool stdCall = false;
   bool apiCall = false;

   SNode targetNode = node.prevNode();

   ident_t dllAlias = targetNode.findSubNodeMask(lxTerminalMask).identifier();
   ident_t functionName = node.firstChild(lxTerminalMask).identifier();

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

   SNode exprNode = node.parentNode();
   exprNode.set(lxSeqExpression, 0);
   SNode callNode = exprNode.appendNode(lxVirtualReference);

////   if (!rootMode)
////      scope.writer->appendNode(lxTarget, -1);

   // To tell apart coreapi calls, the name convention is used
   if (apiCall) {
      callNode.set(lxCoreAPICall, reference);
   }
   else callNode.set(stdCall ? lxStdExternalCall : lxExternalCall, reference);

   // HOTFIX : remove the dll identifier
   targetNode = lxIdle;

   compileExternalArguments(exprNode, scope, callNode);

   callNode.injectAndReplaceNode(lxBoxableExpression);

   if (expectedRef != 0 && _logic->isCompatible(*scope.moduleScope, V_DWORD, expectedRef)) {
      retVal.reference = expectedRef;
      callNode.appendNode(lxSize, 4);
   }
   //   /*if (expectedRef == scope.moduleScope->realReference || expectedRef == V_REAL64) {
   //      retVal = assignResult(writer, scope, true, V_REAL64);
   //   }
   //   else if (expectedRef == V_INT64) {
   //      retVal = assignResult(writer, scope, false, expectedRef);
   //   }
   else {
      retVal.reference = V_DWORD;
      callNode.appendNode(lxSize, 4);
   }

   callNode.appendNode(lxType, retVal.reference);

//   writer.closeNode();

   //if (!EAttrs::test(mode, HINT_ROOT)) {
   //   /*if (expectedRef == scope.moduleScope->realReference || expectedRef == V_REAL64) {
   //      retVal = assignResult(writer, scope, true, V_REAL64);
   //   }
   //   else if (expectedRef == V_INT64) {
   //      retVal = assignResult(writer, scope, false, expectedRef);
   //   }
   //   else */retVal = assignResult(node, scope, false, V_INT32);
   //}

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(SNode node, ExprScope& scope, ref_t message, ref_t signature, ObjectInfo routine)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   int argCount;
   ref_t actionRef, flags;
   ref_t dummy = 0;
   decodeMessage(message, actionRef, argCount, flags);

   size_t signIndex = virtualReference.Length();
   virtualReference.append('0' + (char)(argCount - 1));
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

   node.set(lxInternalCall, moduleScope->module->mapReference(virtualReference));

   // NOTE : all arguments are passed directly
   analizeOperands(node, scope, -1, true);

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
   retVal = newLocalAddr(retVal);

   // reserve place for byte array header if required
   if (bytearray)
      retVal -= 2;

   return retVal;
}

bool Compiler :: allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand)
{
   if (size <= 0)
      return false;

   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::ScopeLevel::slMethod);
   if (methodScope == NULL)
      return false;

   int offset = allocateStructure(binaryArray, size, scope.allocated2);

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
}

bool Compiler :: allocateTempStructure(ExprScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand)
{
   if (size <= 0 || scope.tempAllocated2 == -1)
      return false;

   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   int offset = allocateStructure(binaryArray, size, scope.tempAllocated2);

   if (scope.tempAllocated2 > codeScope->reserved2)
      codeScope->reserved2 = scope.tempAllocated2;

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
}

ref_t Compiler :: declareInlineArgumentList(SNode arg, MethodScope& scope, bool declarationMode)
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
      declareArgumentAttributes(arg, scope, classRef, elementRef, declarationMode);

      int size = classRef != 0 ? _logic->defineStructSize(*scope.moduleScope, classRef, 0) : 0;
      scope.parameters.add(terminal, Parameter(index, classRef, elementRef, size));

      if (classRef != 0)
         weakSingature = false;

      if (isPrimitiveRef(classRef)) {
         // primitive arguments should be replaced with wrapper classes
         signatures[signatureLen++] = resolvePrimitiveReference(scope, classRef, elementRef, declarationMode);
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

   return encodeMessage(actionRef, scope.parameters.Count(), FUNCTION_MESSAGE);
}

//inline SNode findTerminal(SNode node)
//{
//   SNode ident = node.findChild(lxIdentifier/*, lxPrivate*/);
//   if (ident == lxNone)
//      //HOTFIX : if the tree is generated by the script engine
//      ident = node;
//
//   return ident;
//}

void Compiler :: declareArgumentAttributes(SNode node, Scope& scope, ref_t& classRef, ref_t& elementRef, bool declarationMode)
{
   bool byRefArg = false;
   bool paramsArg = false;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (_logic->validateArgumentAttribute(current.argument, byRefArg, paramsArg)) {
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
      }
      else if (current.compare(lxType, lxArrayType)) {
         if (paramsArg) {
            if (current != lxArrayType)
               scope.raiseError(errIllegalMethod, node);

            classRef = resolveTypeAttribute(current.firstChild(), scope, declarationMode, false);
         }
         else classRef = resolveTypeAttribute(current, scope, declarationMode, false);
      }

      current = current.nextNode();
   }

   if (byRefArg) {
      elementRef = classRef;
      classRef = V_WRAPPER;
   }
   if (paramsArg) {
      elementRef = classRef;
      classRef = V_ARGARRAY;
   }
}

ref_t Compiler :: mapMethodName(MethodScope& scope, int paramCount, ref_t actionRef, int flags,
   IdentifierString& actionStr, ref_t* signature, size_t signatureLen,
   bool withoutWeakMessages, bool noSignature)
{
   if (test(flags, VARIADIC_MESSAGE)) {
      paramCount = 1;
      // HOTFIX : extension is a special case - target should be included as well for variadic function
      if (scope.extensionMode && test(flags, FUNCTION_MESSAGE))
         paramCount++;
   }

   // NOTE : a message target should be included as well for a normal message
   int argCount = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
   argCount += paramCount;

   if (actionRef != 0) {
      // HOTFIX : if the action was already resolved - do nothing
   }
   else if (actionStr.Length() > 0) {
      ref_t signatureRef = 0;
      if (signatureLen > 0)
         signatureRef = scope.moduleScope->module->mapSignature(signature, signatureLen, false);

      actionRef = scope.moduleScope->module->mapAction(actionStr.c_str(), signatureRef, false);

      if (withoutWeakMessages && noSignature && test(scope.getClassFlags(false), elClosed)) {
         // HOTFIX : for the nested closed class - special handling is requiered
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
         if (!classScope->info.methods.exist(encodeMessage(actionRef, argCount, flags))) {
            actionRef = scope.moduleScope->module->mapAction(actionStr.c_str(), 0, false);
         }
      }
   }
   else return 0;

   return encodeMessage(actionRef, argCount, flags);
}

void Compiler :: declareArgumentList(SNode node, MethodScope& scope, bool withoutWeakMessages, bool declarationMode)
{
   if (withoutWeakMessages && test(scope.hints, tpGeneric))
      // HOTFIX : nested generic message should not have a multimethod
      withoutWeakMessages = false;

   IdentifierString actionStr;
   ref_t actionRef = 0;

   ref_t signature[ARG_COUNT];
   size_t signatureLen = 0;

   bool constantConversion = false;
   bool unnamedMessage = false;
   ref_t flags = 0;

   SNode nameNode = node.findChild(lxNameAttr/*, lxMessage*/);
   SNode identNode = nameNode.firstChild(lxTerminalMask);

   SNode current = /*action == lxNone ? */node.findChild(lxMethodParameter)/* : action.nextNode()*/;

   if (identNode == lxIdentifier) {
      actionStr.copy(identNode.identifier());
   }
   else unnamedMessage = true;

   bool weakSignature = true;
   int paramCount = 0;
   if (scope.extensionMode) {
      // COMPILER MAGIC : for an extension method, self is a parameter
      paramCount++;

      signature[0] = ((ClassScope*)scope.parent)->extensionClassRef;
      signatureLen++;

      weakSignature = false;

      int size = 0;
      if (!declarationMode)
         size = _logic->defineStructSize(*scope.moduleScope, signature[0], 0);

      scope.parameters.add(SELF_VAR, Parameter(1, signature[0], 0, size));

      flags |= FUNCTION_MESSAGE;
   }

   bool noSignature = true; // NOTE : is similar to weakSignature except if withoutWeakMessages=true
   // if method has an argument list
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         int index = 1 + scope.parameters.Count();
         int size = 0;
         ref_t classRef = 0;
         ref_t elementRef = 0;
         declareArgumentAttributes(current, scope, classRef, elementRef, declarationMode);

         //// NOTE : for the nested classes there should be no weak methods (see compileNestedVMT)
         // NOTE : an extension method must be strong-resolved
         if (withoutWeakMessages && !classRef) {
            classRef = scope.moduleScope->superReference;
         }
         else noSignature = false;

         if (!classRef) {
            classRef = scope.moduleScope->superReference;
         }
         else weakSignature = false;

         ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();
         if (scope.parameters.exist(terminal))
            scope.raiseError(errDuplicatedLocal, current);

         paramCount++;
         if (paramCount >= ARG_COUNT || test(flags, VARIADIC_MESSAGE))
            scope.raiseError(errTooManyParameters, current);

         if (classRef == V_ARGARRAY) {
            // to indicate open argument list
            flags |= VARIADIC_MESSAGE;

            // the generic arguments should be free by the method exit
            scope.withOpenArg = true;

            if (!elementRef)
               elementRef = scope.moduleScope->superReference;

            signature[signatureLen++] = elementRef;
         }
         else if (isPrimitiveRef(classRef)) {
            // primitive arguments should be replaced with wrapper classes
            signature[signatureLen++] = resolvePrimitiveReference(scope, classRef, elementRef, declarationMode);
         }
         else signature[signatureLen++] = classRef;

         size = _logic->defineStructSize(*scope.moduleScope, classRef, elementRef);

         scope.parameters.add(terminal, Parameter(index, classRef, elementRef, size));
      }
//      else if (current == lxMessage) {
//         actionStr.append(":");
//         actionStr.append(current.findChild(lxIdentifier).identifier());
//      }
      else break;

      current = current.nextNode();
   }

   // if the signature consists only of generic parameters - ignore it
   if (weakSignature)
      signatureLen = 0;

   // HOTFIX : do not overrwrite the message on the second pass
   if (scope.message == 0) {
//      if (test(scope.hints, tpSpecial))
//         flags |= SPECIAL_MESSAGE;

      if ((scope.hints & tpMask) == tpDispatcher) {
         if (paramCount == 0 && unnamedMessage) {
            actionRef = getAction(scope.moduleScope->dispatch_message);
            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (test(scope.hints, tpConversion)) {
         if (paramCount == 0 && unnamedMessage && scope.outputRef) {
            ref_t signatureRef = scope.moduleScope->module->mapSignature(&scope.outputRef, 1, false);
            actionRef = scope.moduleScope->module->mapAction(CAST_MESSAGE, signatureRef, false);

            unnamedMessage = false;
         }
         else if (paramCount == 1 && !unnamedMessage && signature[0] == scope.moduleScope->literalReference) {
            constantConversion = true;

            actionStr.append(CONSTRUCTOR_MESSAGE);
            scope.hints |= tpConstructor;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (test(scope.hints, tpConstructor) && unnamedMessage) {
         actionStr.copy(CONSTRUCTOR_MESSAGE);
         unnamedMessage = false;
      }
      else if (test(scope.hints, tpSealed | tpGeneric)) {
         if (signatureLen > 0 || !unnamedMessage || test(scope.hints, tpFunction))
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(GENERIC_PREFIX);
         unnamedMessage = false;
      }
      else if (test(scope.hints, tpFunction)) {
         if (!unnamedMessage)
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(INVOKE_MESSAGE);

         flags |= FUNCTION_MESSAGE;
         // Compiler Magic : if it is a generic closure - ignore fixed argument
         if (test(scope.hints, tpMixin)) {
            if (scope.withOpenArg && validateGenericClosure(signature, signatureLen)) {
               signatureLen = 1;
               scope.mixinFunction = true;
            }
            // mixin function should have a homogeneous signature (i.e. same types) and be variadic
            else scope.raiseError(errIllegalMethod, node);
         }
      }

      if (test(scope.hints, tpMixin) && !test(scope.hints, tpFunction))
         // only mixin function is supported
         scope.raiseError(errIllegalMethod, node);

      if (testany(scope.hints, tpGetAccessor | tpSetAccessor)) {
         flags |= PROPERTY_MESSAGE;
      }

      if (test(scope.hints, tpInternal)) {
         actionStr.insert("$$", 0);
         actionStr.insert(scope.module->Name(), 0);
      }
      else if (test(scope.hints, tpProtected)) {
         if (actionStr.compare(CONSTRUCTOR_MESSAGE) && paramCount == 0) {
            //HOTFIX : protected default constructor has a special name
            actionStr.copy(CONSTRUCTOR_MESSAGE2);
         }
         else {
            // check if protected method already declared
            ref_t publicMessage = mapMethodName(scope, paramCount, actionRef, flags, actionStr,
               signature, signatureLen, withoutWeakMessages, noSignature);

            ref_t declaredMssg = scope.getAttribute(publicMessage, maProtected);
            if (!declaredMssg) {
               ident_t className = scope.module->resolveReference(scope.getClassRef());

               actionStr.insert("$$", 0);
               actionStr.insert(className + 1, 0);
               actionStr.insert("@", 0);
               actionStr.insert(scope.module->Name(), 0);
               actionStr.replaceAll('\'', '@', 0);
            }
            else scope.message = declaredMssg;
         }
      }
      else if ((scope.hints & tpMask) == tpPrivate) {
         flags |= STATIC_MESSAGE;
      }

      if (!scope.message) {
         scope.message = mapMethodName(scope, paramCount, actionRef, flags, actionStr,
            signature, signatureLen, withoutWeakMessages, noSignature);
         if (!scope.message)
            scope.raiseError(errIllegalMethod, node);
      }

      // if it is an explicit constant conversion
      if (constantConversion) {
         NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

         nsScope->saveExtension(scope.message, scope.getClassRef(), scope.message,
            scope.getClassVisibility() != Visibility::Public);
      }
   }

   if (scope.mixinFunction) {
      // Compiler Magic : if it is a mixin function - argument size cannot be directly defined
      scope.message = overwriteArgCount(scope.message, 0);
   }
}

void Compiler :: compileDispatcher(SNode node, MethodScope& scope, bool withGenericMethods, bool withOpenArgGenerics)
{
   node.set(lxClassMethod, scope.message);

   SNode dispatchNode = node.findChild(lxDispatchCode);

   if (dispatchNode != lxNone) {
      CodeScope codeScope(&scope);
      ExprScope exprScope(&codeScope);
      ObjectInfo target = mapObject(dispatchNode, exprScope, EAttr::eaNone);
      if (target.kind != okInternal) {
         dispatchNode.injectAndReplaceNode(lxDispatching);
         if (withGenericMethods) {
            dispatchNode.setArgument(encodeMessage(codeScope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 0, 0));
         }

         dispatchNode = dispatchNode.firstChild();
      }

      compileDispatchExpression(dispatchNode, target, exprScope);
   }
   else {
      dispatchNode = node.appendNode(lxDispatching);

      SNode resendNode = dispatchNode.appendNode(lxResending, 0);

      // if it is generic handler without redirect statement
      if (withGenericMethods) {
         // !! temporally
         if (withOpenArgGenerics)
            scope.raiseError(errInvalidOperation, node);

         resendNode.appendNode(lxMessage,
            encodeMessage(scope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 1, 0));

         resendNode
            .appendNode(lxCallTarget, scope.moduleScope->superReference)
            .appendNode(lxMessage, scope.moduleScope->dispatch_message);
      }
      // if it is open arg generic without redirect statement
      else if (withOpenArgGenerics) {
         // HOTFIX : an extension is a special case of a variadic function and a target should be included
         int argCount = !scope.extensionMode && test(scope.message, FUNCTION_MESSAGE) ? 1 : 2;

         resendNode.appendNode(lxMessage, encodeMessage(getAction(scope.moduleScope->dispatch_message),
            argCount, VARIADIC_MESSAGE));

         resendNode
            .appendNode(lxCallTarget, scope.moduleScope->superReference)
            .appendNode(lxMessage, scope.moduleScope->dispatch_message);
      }
      else throw InternalError("Not yet implemented"); // !! temporal

   }
}

void Compiler :: compileActionMethod(SNode node, MethodScope& scope)
{
   declareProcedureDebugInfo(node, scope, false/*, false*/);

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning);

   if (body != lxCode) {
      body.injectAndReplaceNode(lxNewFrame);
      body = body.firstChild();
   }
   else body.set(lxNewFrame, 0);

   // new stack frame
   // stack already contains previous $self value
   codeScope.allocated1++;
   scope.preallocated = codeScope.allocated1;

   ObjectInfo retVal = compileCode(body == lxReturning ? body.parentNode() : body, codeScope);

   codeScope.syncStack(&scope);

   endMethod(node, scope);
}

void Compiler :: compileExpressionMethod(SNode node, MethodScope& scope, bool lazyMode)
{
   declareProcedureDebugInfo(node, scope, false/*, false*/);

   CodeScope codeScope(&scope);

   SNode current = node.findChild(lxExpression);
   current.injectAndReplaceNode(lxNewFrame);
   current = current.firstChild();

   // new stack frame
   // stack already contains previous $self value
   codeScope.allocated1++;
   scope.preallocated = codeScope.allocated1;

   if (lazyMode) {
      int tempLocal = newLocalAddr(codeScope.allocated2++);

      SNode frameNode = current.parentNode();
      // HOTFIX : lazy expression should presave incoming message
      frameNode.insertNode(lxIndexSaving).appendNode(lxLocalAddress, tempLocal);

      SNode attrNode = current.firstChild();
      while ((attrNode != lxAttribute || attrNode.argument != V_LAZY) && attrNode != lxNone)
         attrNode = attrNode.nextNode();

      // HOTFIX : comment lazy attribute out to prevent short circuit
      if (attrNode == lxAttribute && attrNode.argument == V_LAZY)
         attrNode.setArgument(0);

      compileRetExpression(current, codeScope, EAttr::eaNone);

      // HOTFIX : lazy expression should presave incoming message
      frameNode.appendNode(lxIndexLoading).appendNode(lxLocalAddress, tempLocal);;
   }
   else compileRetExpression(current, codeScope, EAttr::eaNone);

   codeScope.syncStack(&scope);

   endMethod(node, scope);
}

void Compiler :: compileDispatchExpression(SNode node, CodeScope& scope)
{
   ExprScope exprScope(&scope);
   ObjectInfo target = mapObject(node, exprScope, EAttr::eaNone);

   compileDispatchExpression(node, target, exprScope);
}

void Compiler :: compileDispatchExpression(SNode node, ObjectInfo target, ExprScope& exprScope)
{
   if (target.kind == okInternal) {
      importCode(node, exprScope, target.param, exprScope.getMessageID());
   }
   else {
      MethodScope* methodScope = (MethodScope*)exprScope.getScope(Scope::ScopeLevel::slMethod);

      // try to implement light-weight resend operation
      ref_t targetRef = methodScope->getReturningRef(false);

      int stackSafeAttrs = 0;
      bool directOp = _logic->isCompatible(*exprScope.moduleScope, targetRef, exprScope.moduleScope->superReference);
      if (isSingleStatement(node)) {
         if (!directOp) {
            // try to find out if direct dispatch is possible
            ref_t sourceRef = resolveObjectReference(exprScope, target, false, false);

            _CompilerLogic::ChechMethodInfo methodInfo;
            if (_logic->checkMethod(*exprScope.moduleScope, sourceRef, methodScope->message, methodInfo, false) != tpUnknown) {
               directOp = _logic->isCompatible(*exprScope.moduleScope, targetRef, methodInfo.outputReference);
            }
         }
      }

      LexicalType op = lxResending;
      if (methodScope->message == methodScope->moduleScope->dispatch_message)
         // HOTFIX : if it is a generic message resending
         op = lxGenericResending;

      // check if direct dispatch can be done
      if (directOp) {
         // we are lucky and can dispatch the message directly
         switch (target.kind) {
            case okConstantSymbol:
            case okField:
            case okReadOnlyField:
            case okOuterSelf:
            case okOuter:
            {
               node.set(op, methodScope->message);
               if (target.kind != okConstantSymbol) {
                  SNode fieldExpr = node.findChild(lxFieldExpression);
                  if (fieldExpr != lxNone) {
                     fieldExpr.set(lxField, target.param);
                  }
               }
               break;
            }
            default:
            {
               node.set(op, methodScope->message);
               SNode frameNode = node.injectNode(lxNewFrame);
               SNode exprNode = frameNode.injectNode(lxExpression);
               exprScope.tempAllocated1++;

               int preserved = exprScope.tempAllocated1;
               target = compileExpression(exprNode, exprScope, target, 0, EAttr::eaNone);
               analizeOperands(exprNode, exprScope, 0, false);
               // HOTFIX : allocate temporal variables
               if (exprScope.tempAllocated1 != preserved)
                  frameNode.appendNode(lxReserved, exprScope.tempAllocated1 - preserved);

               break;
            }
         }
      }
      else {
         EAttr mode = EAttr::eaNone;

         // bad luck - we have to dispatch and type cast the result
         node.set(lxNewFrame, 0);

         node.injectNode(lxExpression);
         SNode exprNode = node.firstChild(lxObjectMask);

         for (auto it = methodScope->parameters.start(); !it.Eof(); it++) {
            ObjectInfo param = methodScope->mapParameter(*it, EAttr::eaNone);

            SNode refNode = exprNode.appendNode(lxVirtualReference);
            setParamTerminal(refNode, exprScope, param, mode, lxLocal);
         }

         bool dummy = false;
         ObjectInfo retVal = compileMessage(exprNode, exprScope, target, methodScope->message, mode | HINT_NODEBUGINFO,
            stackSafeAttrs, dummy);

         retVal = convertObject(exprNode, exprScope, targetRef, retVal, mode);
         if (retVal.kind == okUnknown) {
            exprScope.raiseError(errInvalidOperation, node);
         }
      }
   }
}

inline ref_t resolveProtectedMessage(ClassInfo& info, ref_t protectedMessage)
{
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      if (*it == protectedMessage) {
         Attribute key = it.key();
         if (key.value2 == maProtected) {
            return key.value1;
         }
      }
   }

   return 0;
}

void Compiler :: compileConstructorResendExpression(SNode node, CodeScope& codeScope, ClassScope& classClassScope,
   bool& withFrame)
{
   ResendScope resendScope(&codeScope);
   resendScope.consructionMode = true;

   // inject a root expression
   SNode expr = node.findChild(lxExpression).injectNode(lxExpression);

   _ModuleScope* moduleScope = codeScope.moduleScope;
   MethodScope* methodScope = (MethodScope*)codeScope.getScope(Scope::ScopeLevel::slMethod);

   ref_t messageRef = 0;
//   bool implicitConstructor = false;
   SNode messageNode = expr.findChild(lxMessage);
//   if (messageNode.firstChild(lxTerminalMask) == lxNone) {
//      // HOTFIX : support implicit constructors
//      messageRef = encodeMessage(scope.module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
//         SyntaxTree::countNodeMask(messageNode, lxObjectMask), /*STATIC_MESSAGE*/0);
//
//      implicitConstructor = true;
//   }
   /*else */messageRef = mapMessage(messageNode, resendScope, false, false);

   ref_t classRef = classClassScope.reference;
   bool found = false;

   if (node.existChild(lxCode) || !isConstantArguments(expr)) {
      withFrame = true;

      // new stack frame
      // stack already contains $self value
      node.set(lxNewFrame, 0);
      codeScope.allocated1++;
      // HOTFIX : take into account saved self variable
      resendScope.tempAllocated1 = codeScope.allocated1;

      methodScope->preallocated = codeScope.allocated1;
   }
   else node.set(lxExpression, 0);

   if (withFrame) {
      expr.insertNode(lxSelfLocal, 1);
   }
   else expr.insertNode(lxResult);

   resendScope.withFrame = withFrame;

   bool variadicOne = false;
   bool inlineArg = false;
   SNode argNode = expr.findChild(lxMessage).nextNode();
   ref_t implicitSignatureRef = compileMessageParameters(argNode, resendScope, EAttr::eaNone,
      variadicOne, inlineArg);

   ObjectInfo target(okClassSelf, resendScope.getClassRefId(), classRef);
   int stackSafeAttr = 0;
//   if (implicitConstructor) {
//      ref_t resolvedMessageRef = _logic->resolveImplicitConstructor(*scope.moduleScope, target.param, implicitSignatureRef, getParamCount(messageRef),
//         stackSafeAttr, false);
//
//      if (resolvedMessageRef)
//         messageRef = resolvedMessageRef;
//   }
   /*else */messageRef = resolveMessageAtCompileTime(target, resendScope, messageRef, implicitSignatureRef, false, stackSafeAttr);

   // find where the target constructor is declared in the current class
   // but it is not equal to the current method
   ClassScope* classScope = (ClassScope*)resendScope.getScope(Scope::ScopeLevel::slClass);
   int hints = methodScope->message != messageRef ? checkMethod(*moduleScope, classScope->info.header.classRef, messageRef) : tpUnknown;
   if (hints != tpUnknown) {
      found = true;
   }
   // otherwise search in the parent class constructors
   else {
      ref_t publicRef = resolveProtectedMessage(classClassScope.info, messageRef);
      // HOTFIX : replace protected message with public one
      if (publicRef)
         messageRef = publicRef;

      ref_t parent = classScope->info.header.parentRef;
      ClassInfo info;
      while (parent != 0) {
         moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(parent));

         ref_t protectedConstructor = 0;
         if (checkMethod(*moduleScope, info.header.classRef, messageRef, protectedConstructor) != tpUnknown) {
            classRef = info.header.classRef;
            found = true;

            target.reference = classRef;
            messageRef = resolveMessageAtCompileTime(target, resendScope, messageRef, implicitSignatureRef, 
               false, stackSafeAttr);

            break;
         }
         else if (protectedConstructor) {
            classRef = info.header.classRef;
            found = true;

            target.reference = classRef;
            messageRef = resolveMessageAtCompileTime(target, resendScope, protectedConstructor, implicitSignatureRef, 
               false, stackSafeAttr);

            break;
         }
         else parent = info.header.parentRef;
      }
   }

   if (found) {
      bool dummy = false;
      compileMessage(expr, resendScope, target, messageRef, EAttr::eaNone, stackSafeAttr, dummy);

      if (withFrame) {
         // HOT FIX : inject saving of the created object
         SNode codeNode = node.findChild(lxCode);
         if (codeNode != lxNone) {
            SNode assignNode = codeNode.insertNode(lxAssigning);
            assignNode.appendNode(lxLocal, 1);
            assignNode.appendNode(lxResult);
         }
      }
   }
   else resendScope.raiseError(errUnknownMessage, node);
}

void Compiler :: compileConstructorDispatchExpression(SNode node, CodeScope& scope, bool isDefault)
{
   SNode redirect = node.findChild(lxRedirect);
   if (redirect != lxNone) {
      if (!isDefault) {
         SNode callNode = node.prependSibling(lxCalling_1, scope.moduleScope->constructor_message);
         callNode.appendNode(lxResult);
      }
      else {
         MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::ScopeLevel::slMethod);

         compileDefConvConstructor(node.parentNode(), *methodScope, isDefault);
      }

      node.set(lxImplicitJump, redirect.argument);
      node.appendNode(lxCallTarget, scope.getClassRefId());
   }
   else {
      ExprScope exprScope(&scope);
      ObjectInfo target = mapObject(node, exprScope, EAttr::eaNone);
      if (target.kind == okInternal) {
         importCode(node, exprScope, target.param, exprScope.getMessageID());
      }
      else scope.raiseError(errInvalidOperation, node);
   }
}

void Compiler :: compileMultidispatch(SNode node, CodeScope& scope, ClassScope& classScope)
{
   ref_t message = scope.getMessageID();
   ref_t overloadRef = classScope.info.methodHints.get(Attribute(message, maOverloadlist));
   if (!overloadRef)
      scope.raiseError(errIllegalOperation, node);

   LexicalType op = lxMultiDispatching;
   if (test(classScope.info.header.flags, elSealed) || test(message, STATIC_MESSAGE)) {
      op = lxSealedMultiDispatching;
   }

   if (node == lxResendExpression) {
      node.prependSibling(op, overloadRef);
      //      if (node.existChild(lxTypecasting)) {
      //         // if it is multi-method implicit conversion method
      //         ref_t targetRef = scope.getClassRefId();
      //         ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
      //
      //         writer.newNode(lxCalling, encodeAction(scope.module->mapAction(CAST_MESSAGE, signRef, false)));
      //         writer.appendNode(lxCurrent, 2);
      //         writer.closeNode();
      //      }
      //      else {
      if (classScope.extensionDispatcher) {
         node.set(lxResending, node.argument & ~FUNCTION_MESSAGE);
         node.appendNode(lxCurrent, 1);
      }
      else node.set(lxResending, node.argument);
      //         writer.newNode(lxDispatching, node.argument);
      //         SyntaxTree::copyNode(writer, lxTarget, node);
      //         writer.closeNode();
      //      }
   }
   else node.insertNode(op, overloadRef);
}

void Compiler :: compileResendExpression(SNode node, CodeScope& codeScope, bool multiMethod)
{
   if (node.firstChild() == lxImplicitJump) {
      // do nothing for redirect method
      node = lxExpression;
   }
   else if (node.argument != 0 && multiMethod) {
      ClassScope* classScope = (ClassScope*)codeScope.getScope(Scope::ScopeLevel::slClass);

      compileMultidispatch(node, codeScope, *classScope);
   }
   else {
      if (multiMethod) {
         ClassScope* classScope = (ClassScope*)codeScope.getScope(Scope::ScopeLevel::slClass);

         compileMultidispatch(node.parentNode(), codeScope, *classScope);
      }

      SNode expr = node.findChild(lxExpression);

      // new stack frame
      // stack already contains $self value
      node.set(lxNewFrame, 0);
      codeScope.allocated1++;

      expr.insertNode(lxSelfLocal, 1);

      SNode messageNode = expr.findChild(lxMessage);

      ExprScope scope(&codeScope);
      ObjectInfo target = scope.mapMember(SELF_VAR);
      compileMessage(messageNode, scope, 0, target, EAttr::eaNone);

      if (node.existChild(lxCode))
         scope.raiseError(errInvalidOperation, node);
   }
}

bool Compiler :: isMethodEmbeddable(MethodScope& scope, SNode)
{
   if (test(scope.hints, tpEmbeddable)) {
      ClassScope* ownerScope = (ClassScope*)scope.parent;

      return ownerScope->info.methodHints.exist(Attribute(scope.message, maEmbeddableRet));
   }
   else return false;
}

void Compiler :: compileEmbeddableMethod(SNode node, MethodScope& scope)
{
   ClassScope* ownerScope = (ClassScope*)scope.parent;

   // generate private static method with an extra argument - retVal
   MethodScope privateScope(ownerScope);
   privateScope.message = ownerScope->info.methodHints.get(Attribute(scope.message, maEmbeddableRet));

   ref_t ref = resolvePrimitiveReference(scope, V_WRAPPER, scope.outputRef, false);
   validateType(scope, node, ref, false, false);

   declareArgumentList(node, privateScope, true, false);
   privateScope.parameters.add(RETVAL_ARG, Parameter(1 + scope.parameters.Count(), V_WRAPPER, scope.outputRef, 0));
   privateScope.classStacksafe = scope.classStacksafe;
   privateScope.extensionMode = scope.extensionMode;
   privateScope.embeddableRetMode = true;

   // !! TEMPORAL : clone the method node, to compile it safely : until the proper implementation
   SNode cloneNode = node.prependSibling(node.type, privateScope.message);
   SyntaxTree::copyNode(node, cloneNode);

   if (scope.abstractMethod) {
      // COMPILER MAGIC : if the method retunging value can be passed as an extra argument
      compileAbstractMethod(node, scope);
      compileAbstractMethod(cloneNode, privateScope);
   }
   else {
      if (scope.yieldMethod) {
         compileYieldableMethod(cloneNode, privateScope);
         //compileMethod(writer, node, privateScope);

         compileYieldableMethod(node, scope);
      }
      else {
         compileMethod(cloneNode, privateScope);
         //compileMethod(node, privateScope);

         compileMethod(node, scope);
      }
   }
}

void Compiler :: beginMethod(SNode node, MethodScope& scope)
{
//   if (scope.functionMode) {
//      scope.rootToFree -= 1;
//   }

   declareProcedureDebugInfo(node, scope, true/*, test(scope.getClassFlags(), elExtension)*/);
}

void Compiler :: endMethod(SNode node, MethodScope& scope)
{
   node.insertNode(lxAllocated, scope.reserved1 - scope.preallocated);  // allocate the space for the local variables excluding preallocated ones ("$this", "$message")
   if (scope.mixinFunction) {
      // generic functions are special case of functions, consuming only fixed part of the argument list
      node.insertNode(lxArgCount, scope.parameters.Count() - 1);
   }
   else node.insertNode(lxArgCount, getArgCount(scope.message));
   node.insertNode(lxReserved, scope.reserved2);
}

ref_t Compiler :: resolveConstant(ObjectInfo retVal, ref_t& parentRef)
{
   switch (retVal.kind) {
      case okSingleton:
         parentRef = retVal.param;
         return retVal.param;
      case okConstantSymbol:
         parentRef = retVal.reference;
         return retVal.param;
      case okArrayConst:
         parentRef = retVal.reference;
         return retVal.param;
      default:
         return 0;
   }
}

ref_t Compiler :: generateConstant(_CompileScope& scope, ObjectInfo retVal)
{
   switch (retVal.kind) {
      case okSingleton:
         return retVal.param;
      case okLiteralConstant:
      case okWideLiteralConstant:
      case okIntConstant:
         break;
      default:
         return 0;
   }

   ref_t parentRef = 0;
   ref_t constRef = scope.moduleScope->mapAnonymous("const");

   _Module* module = scope.moduleScope->module;
   MemoryWriter dataWriter(module->mapSection(constRef | mskRDataRef, false));

   if (retVal.kind == okIntConstant || retVal.kind == okUIntConstant) {
      size_t value = module->resolveConstant(retVal.param).toULong(16);

      dataWriter.writeDWord(value);

      parentRef = scope.moduleScope->intReference;
   }
   else if (retVal.kind == okLongConstant) {
      long long value = module->resolveConstant(retVal.param).toLongLong(10, 1);

      dataWriter.write(&value, 8u);

      parentRef = scope.moduleScope->longReference;
   }
   else if (retVal.kind == okRealConstant) {
      double value = module->resolveConstant(retVal.param).toDouble();

      dataWriter.write(&value, 8u);

      parentRef = scope.moduleScope->realReference;
   }
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
   else if (retVal.kind == okCharConstant) {
      ident_t value = module->resolveConstant(retVal.param);

      dataWriter.writeLiteral(value, getlength(value));

      parentRef = scope.moduleScope->charReference;
   }
   else if (retVal.kind == okMessageNameConstant) {
      dataWriter.Memory()->addReference(retVal.param | mskMessageName, dataWriter.Position());
      dataWriter.writeDWord(0);

      parentRef = scope.moduleScope->messageNameReference;
   }
   //      else if (retVal.kind == okObject) {
   //         SNode root = node.findSubNodeMask(lxObjectMask);
   //
   //         if (root == lxConstantList/* && !accumulatorMode*/) {
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

   dataWriter.Memory()->addReference(parentRef | mskVMTRef, (ref_t)-4);

   SymbolExpressionInfo info;
   info.type = SymbolExpressionInfo::Type::Constant;
   info.exprRef = parentRef;

   // save constant meta data
   MemoryWriter metaWriter(scope.moduleScope->module->mapSection(constRef | mskMetaRDataRef, false), 0);
   info.save(&metaWriter);

   return constRef;
}

void Compiler :: compileMethodCode(SNode node, SNode body, MethodScope& scope, CodeScope& codeScope)
{
   if (scope.multiMethod) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

      compileMultidispatch(node, codeScope, *classScope);
   }

   int frameArg = scope.generic ? -1 : 0;
   if (body != lxCode) {
      body.injectAndReplaceNode(lxNewFrame, frameArg);
      body = body.firstChild();
   }
   else body.set(lxNewFrame, frameArg);

   // new stack frame
   // stack already contains current self reference
   // the original message should be restored if it is a generic method
   codeScope.allocated1++;
   // declare the current subject for a generic method
   if (scope.generic) {
      codeScope.allocated2++;
      codeScope.mapLocal(MESSAGE_VAR, -2, V_MESSAGE, 0, 0);
   }

   scope.preallocated = codeScope.allocated1;

   if (body == lxReturning)
      body = body.parentNode();

   ObjectInfo retVal = compileCode(body, codeScope);

   // if the method returns itself
   if (retVal.kind == okUnknown) {
      if (test(scope.hints, tpSetAccessor)) {
         retVal = scope.mapParameter(*scope.parameters.start(), EAttr::eaNone);
      }
      else retVal = scope.mapSelf();

      // adding the code loading self / parameter (for set accessor)
      SNode retNode = body.appendNode(lxReturning);
      ExprScope exprScope(&codeScope);
      SNode exprNode = retNode.appendNode(lxExpression);
      recognizeTerminal(exprNode, retVal, exprScope, HINT_NODEBUGINFO | HINT_NOBOXING);

      ref_t resultRef = scope.getReturningRef(false);
      if (resultRef != 0) {
         if (convertObject(retNode, exprScope, resultRef, retVal, EAttr::eaNone).kind == okUnknown)
            scope.raiseError(errInvalidOperation, node);
      }
   }

   if (scope.constMode) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

      ref_t constRef = generateConstant(scope, retVal);
      if (constRef) {
         classScope->addAttribute(scope.message, maConstant, constRef);

         classScope->save();
      }
      else scope.raiseError(errInvalidConstAttr, node);
   }
}

void Compiler :: compileMethod(SNode node, MethodScope& scope)
{
   beginMethod(node, scope);

   scope.preallocated = 0;

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   // check if it is a resend
   if (body == lxResendExpression) {
      compileResendExpression(body, codeScope, scope.multiMethod);
      scope.preallocated = 1;
   }
   // check if it is a dispatch
   else if (body == lxDispatchCode) {
      compileDispatchExpression(body, codeScope);
   }
   else compileMethodCode(node, body, scope, codeScope);

   codeScope.syncStack(&scope);

   endMethod(node, scope);
}

void Compiler :: compileYieldDispatch(SNode node, MethodScope& scope)
{
   int size1 = scope.reserved1 - scope.preallocated;
   int size2 = scope.reserved2;

   int index = scope.getAttribute(maYieldContext);
   int index2 = scope.getAttribute(maYieldLocals);

   // dispatch
   SNode dispNode = node.insertNode(lxYieldDispatch);
   SNode contextExpr = dispNode.appendNode(lxFieldExpression);
   contextExpr.appendNode(lxSelfLocal, 1);
   contextExpr.appendNode(lxField, index);

   // load context
   SNode exprNode = node.insertNode(lxExpression);
   SNode copyNode = exprNode.appendNode(lxCopying, size2);
   copyNode.appendNode(lxLocalAddress, -2);
   SNode fieldNode = copyNode.appendNode(lxFieldExpression);
   fieldNode.appendNode(lxSelfLocal, 1);
   fieldNode.appendNode(lxField, index);
   fieldNode.appendNode(lxField, 1);

   // load locals
   if (size1 != 0) {
      SNode expr2Node = node.insertNode(lxExpression);
      SNode copy2Node = expr2Node.appendNode(lxCopying, size1);
      copy2Node.appendNode(lxLocalAddress, scope.preallocated + size1);
      SNode field2Node = copy2Node.appendNode(lxFieldExpression);
      field2Node.appendNode(lxSelfLocal, 1);
      field2Node.appendNode(lxField, index2);
   }
}

void Compiler :: compileYieldableMethod(SNode node, MethodScope& scope)
{
   beginMethod(node, scope);

   scope.preallocated = 0;

   CodeScope codeScope(&scope);

   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression);
   if (body == lxCode) {
      compileMethodCode(node, body, scope, codeScope);
   }
   else scope.raiseError(errInvalidOperation, body);

   codeScope.syncStack(&scope);

//   // COMPILER MAGIC : struct variables should be synchronized with the context field
//   int index = scope.getAttribute(maYieldContext);
//   int index2 = scope.getAttribute(maYieldLocals);
//
//   // injecting the virtual field sizes
//   SyntaxWriter methodWriter(writer);
//   methodWriter.seekUp(lxClassMethod);
//   methodWriter.CurrentNode().insertNode(lxSetTapeArgument, scope.reserved).appendNode(lxTapeArgument, index);
//   methodWriter.CurrentNode().insertNode(lxSetTapeArgument, codeScope.level - preallocated).appendNode(lxTapeArgument, index2);

//   if (scope.yieldMethod) {
//      scope.setAttribute(maYieldPreallocated, preallocated);
   compileYieldDispatch(body, scope);
//   }

   endMethod(node, scope);

   scope.addAttribute(maYieldContextLength, scope.reserved2 + 1);
   scope.addAttribute(maYieldLocalLength, scope.reserved1);
}

void Compiler :: compileAbstractMethod(SNode node, MethodScope& scope)
{
   SNode body = node.findChild(lxCode);
   // abstract method should have an empty body
   if (body != lxNone) {
      if (body.firstChild() != lxEOP)
         scope.raiseError(errAbstractMethodCode, node);
   }
   else scope.raiseError(errAbstractMethodCode, node);

   body.set(lxNil, 0);
}

void Compiler :: compileInitializer(SNode node, MethodScope& scope)
{
   SNode methodNode = node.appendNode(lxClassMethod, scope.message);

   beginMethod(methodNode, scope);

   scope.preallocated = 0;

   CodeScope codeScope(&scope);

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
   if (checkMethod(*scope.moduleScope, classScope->info.header.parentRef, scope.message) != tpUnknown) {
      ref_t messageOwnerRef = resolveMessageOwnerReference(*scope.moduleScope, classScope->info, classScope->reference,
         scope.message, true);

      // check if the parent has implicit constructor - call it
      SNode callNode = methodNode.appendNode(lxDirectCalling, scope.message);
      callNode.appendNode(lxResult);
      callNode.appendNode(lxCallTarget, messageOwnerRef);
   }

   SNode frameNode = methodNode.appendNode(lxNewFrame);

   // new stack frame
   // stack already contains current $self reference
   // the original message should be restored if it is a generic method
   codeScope.allocated1++;

   scope.preallocated = codeScope.allocated1;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.compare(lxFieldInit, lxFieldAccum)) {
         SNode sourceNode = current.findChild(lxSourcePath);
         if (sourceNode != lxNone)
            declareCodeDebugInfo(frameNode, scope);

         SNode exprNode = frameNode.insertNode(lxExpression);
         SyntaxTree::copyNode(current, exprNode);

         compileRootExpression(exprNode, codeScope, 0, HINT_ROOT);
      }

      current = current.nextNode();
   }

   frameNode.appendNode(lxExpression).appendNode(lxSelfLocal, 1);

   codeScope.syncStack(&scope);

   endMethod(methodNode, scope);
}

void Compiler :: compileDefConvConstructor(SNode node, MethodScope& scope, bool isDefault)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

   if (test(classScope->info.header.flags, elDynamicRole))
      throw InternalError("Invalid constructor");

   SNode exprNode = node.insertNode(lxSeqExpression);

   if (test(classScope->info.header.flags, elStructureRole)) {
      exprNode
         .appendNode(lxCreatingStruct, classScope->info.size)
         .appendNode(lxType, classScope->reference);
   }
   else {
      exprNode
         .appendNode(lxCreatingClass, classScope->info.fields.Count())
         .appendNode(lxType, classScope->reference);
   }

   // call field initilizers if available for default constructor
   if (isDefault)
      compileSpecialMethodCall(exprNode, *classScope, scope.moduleScope->init_message);
}

bool Compiler :: isDefaultOrConversionConstructor(Scope& scope, ref_t message, bool& isProtectedDefConst)
{
   ref_t actionRef = getAction(message);
   if (actionRef == getAction(scope.moduleScope->constructor_message)) {
      return true;
   }
   else if (actionRef == getAction(scope.moduleScope->protected_constructor_message)) {
      isProtectedDefConst = true;

      return true;
   }
   else if (getArgCount(message) > 1) {
      ref_t dummy = 0;
      ident_t actionName = scope.module->resolveAction(actionRef, dummy);
      if (actionName.compare(CONSTRUCTOR_MESSAGE2)) {
         isProtectedDefConst = true;
         return true;
      }
      else return actionName.endsWith(CONSTRUCTOR_MESSAGE);
   }
   else return false;
}

void Compiler :: compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   // if it is a default / conversion (unnamed) constructor
   bool isProtectedDefConst = false;
   bool isDefConvConstructor = isDefaultOrConversionConstructor(scope, scope.message, isProtectedDefConst);

   ref_t defConstrMssg = scope.moduleScope->constructor_message;
   if (classClassScope.checkAttribute(defConstrMssg, maProtected)) {
      // if protected default constructor is declared - use it
      defConstrMssg = classClassScope.getAttribute(defConstrMssg, maProtected);
      isProtectedDefConst = true;
   }
   else if (classClassScope.info.methods.exist(defConstrMssg | STATIC_MESSAGE)) {
      // if private default constructor is declared - use it
      defConstrMssg = defConstrMssg | STATIC_MESSAGE;
   }

//   SNode attrNode = node.findChild(lxEmbeddableMssg);
//   if (attrNode != lxNone) {
//      // COMPILER MAGIC : copy an attribute so it will be recognized as embeddable call
//      writer.appendNode(attrNode.type, attrNode.argument);
//   }

   declareProcedureDebugInfo(node, scope, true/*, false*/);

   CodeScope codeScope(&scope);

   bool retExpr = false;
   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();
   scope.preallocated = 0;

   SNode bodyNode = node.findChild(lxResendExpression, lxCode, lxReturning, lxDispatchCode);
   if (bodyNode == lxDispatchCode) {
      compileConstructorDispatchExpression(bodyNode, codeScope, scope.message == defConstrMssg);

      return;
   }
   else if (bodyNode == lxResendExpression) {
      if (scope.multiMethod && bodyNode.argument != 0) {
         compileMultidispatch(bodyNode, codeScope, classClassScope);

         bodyNode = SNode();
      }
      else {
         if (isDefConvConstructor && getArgCount(scope.message) <= 1)
            scope.raiseError(errInvalidOperation, node);

         compileConstructorResendExpression(bodyNode, codeScope, classClassScope, withFrame);

         bodyNode = bodyNode.findChild(lxCode);
      }
   }
   else if (bodyNode == lxReturning) {
      retExpr = true;
   }
   else if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
      // if it is a default / conversion (unnamed) constructor
      // it should create the object
      compileDefConvConstructor(node, scope, scope.message == defConstrMssg);
   }
   // if no redirect statement - call the default constructor
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(defConstrMssg)) {
      // HOTFIX : use dispatching routine for the protected default constructor
      SNode callNode = node.insertNode(lxCalling_1, defConstrMssg);
      callNode.appendNode(lxResult);
   }
   else if (!test(classFlags, elDynamicRole) && test(classFlags, elAbstract)) {
      // HOTFIX : allow to call the non-declared default constructor for abstract
      // class constructors
      SNode callNode = node.insertNode(lxCalling_1, defConstrMssg);
      callNode.appendNode(lxResult);
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node);

   if (bodyNode != lxNone) {
      if (!withFrame) {
         if (bodyNode != lxCode) {
            bodyNode.injectAndReplaceNode(lxNewFrame);
            bodyNode = bodyNode.firstChild();
         }
         else bodyNode.set(lxNewFrame, 0);

         withFrame = true;

         // new stack frame
         // stack already contains $self value
         codeScope.allocated1++;
      }

      scope.preallocated = codeScope.allocated1;
      if (retExpr) {
         //ObjectInfo retVal = compileRootExpression(bodyNode, codeScope, codeScope.getClassRefId(), EAttr::eaNone);

         //retVal = convertObject(bodyNode, )

         compileRootExpression(bodyNode, codeScope, codeScope.getClassRefId(), HINT_DYNAMIC_OBJECT | HINT_NOPRIMITIVES);
      }
      else {
         compileCode(bodyNode, codeScope);

         // HOT FIX : returning the created object
         bodyNode.appendNode(lxExpression).appendNode(lxLocal, 1);
      }
   }
   else if (withFrame) {
      scope.preallocated = codeScope.allocated1;
   }

   codeScope.syncStack(&scope);

   endMethod(node, scope);
}

void Compiler :: compileSpecialMethodCall(SNode& node, ClassScope& classScope, ref_t message)
{
   if (classScope.info.methods.exist(message)) {
      if (classScope.info.methods.exist(message, true)) {
         // call the field in-place initialization
         SNode argNode = node.appendNode(lxDirectCalling, message);
         argNode.appendNode(lxResult);
         argNode.appendNode(lxCallTarget, classScope.reference);
      }
      else {
         ref_t parentRef = classScope.info.header.parentRef;
         while (parentRef != 0) {
            // call the parent field in-place initialization
            ClassInfo parentInfo;
            _logic->defineClassInfo(*classScope.moduleScope, parentInfo, parentRef);

            if (parentInfo.methods.exist(message, true)) {
               SNode argNode = node.appendNode(lxDirectCalling, message);
               argNode.appendNode(lxResult);
               argNode.appendNode(lxCallTarget, parentRef);

               break;
            }

            parentRef = parentInfo.header.parentRef;
         }
      }
   }
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

void Compiler :: compileVMT(SNode node, ClassScope& scope, bool exclusiveMode, bool ignoreAutoMultimethods)
{
   scope.withInitializers = scope.info.methods.exist(scope.moduleScope->init_message, true);

   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxStaticFieldInit:
            compileCompileTimeAssigning(current, scope);
            break;
         case lxClassMethod:
         {
            if (exclusiveMode && (ignoreAutoMultimethods == current.existChild(lxAutoMultimethod))) {
               current = current.nextNode();
               continue;
            }

            MethodScope methodScope(&scope);
            methodScope.message = current.argument;

#ifdef FULL_OUTOUT_INFO
            // info
            int x = 0;
            scope.moduleScope->printMessageInfo("method %s", methodScope.message);
#endif // FULL_OUTOUT_INFO

//            if (current.argument == (encodeAction(DEFAULT_MESSAGE_ID) | SPECIAL_MESSAGE)) {
//               scope.withImplicitConstructor = true;
//            }

            initialize(scope, methodScope);
            if (methodScope.outputRef) {
               // HOTFIX : validate the output type once again in case it was declared later in the code
               SNode typeNode = current.findChild(lxType, lxArrayType);
               if (typeNode) {
                  resolveTypeAttribute(typeNode, scope, false, false);
               }
               else validateType(scope, current, methodScope.outputRef, false, false);
            }

            // if it is a dispatch handler
            if (methodScope.message == scope.moduleScope->dispatch_message) {
               compileDispatcher(current, methodScope,
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithVariadics));
            }
            // if it is a normal method
            else {
               declareArgumentList(current, methodScope, false, false);

               if (methodScope.abstractMethod) {
                  if (isMethodEmbeddable(methodScope, current)) {
                     compileEmbeddableMethod(current, methodScope);
                  }
                  else compileAbstractMethod(current, methodScope);
               }
               else if (isMethodEmbeddable(methodScope, current)) {
                  // COMPILER MAGIC : if the method retunging value can be passed as an extra argument
                  compileEmbeddableMethod(current, methodScope);
               }
               else if (methodScope.yieldMethod) {
                  compileYieldableMethod(current, methodScope);
               }
               else compileMethod(current, methodScope);
            }

            break;
         }
      }

      current = current.nextNode();
   }

   if (scope.withInitializers && (ignoreAutoMultimethods || !exclusiveMode)) {
      // HOTFIX : compileVMT is called twice for nested classes - initializer should be called only once
      MethodScope methodScope(&scope);
      methodScope.message = scope.moduleScope->init_message;

      initialize(scope, methodScope);

      // if it is in-place class member initialization
      compileInitializer(node, methodScope);
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (testany(scope.info.header.flags, elWithGenerics | elWithVariadics)
      && scope.info.methods.exist(scope.moduleScope->dispatch_message, false))
   {
      MethodScope methodScope(&scope);
      methodScope.message = scope.moduleScope->dispatch_message;

      scope.include(methodScope.message);

      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);

      scope.info.header.flags |= elWithCustomDispatcher;

      compileDispatcher(methodNode, methodScope,
         test(scope.info.header.flags, elWithGenerics),
         test(scope.info.header.flags, elWithVariadics));

      // overwrite the class info
      scope.save();
   }
}

void Compiler :: compileClassVMT(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
//   bool staticFieldsCopied = false;

   //// add virtual constructor
   //if (classClassScope.info.methods.exist(classScope.moduleScope->newobject_message, true)) {
   //   MethodScope methodScope(&classScope);
   //   methodScope.message = classScope.moduleScope->newobject_message;

   //   //if (test(classScope.info.header.flags, elDynamicRole)) {
   //   //   compileDynamicDefaultConstructor(writer, methodScope);
   //   //}
   //   /*else */compileDefaultConstructor(node, methodScope);
   //}

   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxConstructor:
         {
            MethodScope methodScope(&classScope);
            methodScope.message = current.argument;

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope, false, false);

            compileConstructor(current, methodScope, classClassScope);
            break;
         }
         case lxStaticMethod:
         {
//            if (!staticFieldsCopied) {
//               // HOTFIX : inherit static fields
//               classClassScope.copyStaticFields(classScope.info.statics, classScope.info.staticValues);
//
//               staticFieldsCopied = true;
//            }

            MethodScope methodScope(&classClassScope);
            methodScope.message = current.argument;

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope, false, false);

            if (isMethodEmbeddable(methodScope, current)) {
               compileEmbeddableMethod(current, methodScope);
            }
            else compileMethod(current, methodScope);
            break;
         }
      }

      current = current.nextNode();
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (testany(classClassScope.info.header.flags, elWithGenerics | elWithVariadics)
      && classClassScope.info.methods.exist(classClassScope.moduleScope->dispatch_message, false))
   {
      MethodScope methodScope(&classClassScope);
      methodScope.message = classClassScope.moduleScope->dispatch_message;

      classClassScope.include(methodScope.message);

      SNode methodNode = node.appendNode(lxClassMethod, methodScope.message);

      compileDispatcher(methodNode, methodScope,
         test(classClassScope.info.header.flags, elWithGenerics),
         test(classClassScope.info.header.flags, elWithVariadics));

      // overwrite the class info
      classClassScope.save();
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

void Compiler :: validateClassFields(SNode node, ClassScope& scope)
{
   SNode current = node.firstChild();

   while (current != lxNone) {
      if (current == lxClassField) {
         SNode typeNode = current.findChild(lxType, lxArrayType);
         if (typeNode != lxNone) {
            resolveTypeAttribute(typeNode, scope, false, false);
         }
      }
      current = current.nextNode();
   }
}

//bool Compiler :: isValidAttributeType(Scope& scope, FieldAttributes& attrs)
//{
////   _ModuleScope* moduleScope = scope.moduleScope;
////
////   if (attrs.isSealedAttr && attrs.isConstAttr)
////      return false;
////
////   //if ()
////
////   //if (size != 0) {
////   //   return false;
////   //}
////   //else if (fieldRef == moduleScope->literalReference) {
////   //   return true;
////   //}
//   /*else */return true;
//}

void Compiler :: generateClassFields(SNode node, ClassScope& scope, bool singleField)
{
   SNode current = node.firstChild();

   bool isClassClassMode = scope.classClassMode;
   while (current != lxNone) {
      if (current == lxClassField) {
         FieldAttributes attrs;
         declareFieldAttributes(current, scope, attrs);

         if (attrs.isStaticField || attrs.isConstAttr) {
            //if (!isValidAttributeType(scope, attrs))
            //   scope.raiseError(errIllegalField, current);

            generateClassStaticField(scope, current, attrs.fieldRef, attrs.elementRef, attrs.isStaticField, attrs.isConstAttr, attrs.isArray);
         }
         else if (!isClassClassMode)
            generateClassField(scope, current, attrs, singleField);
      }
      current = current.nextNode();
   }
}

void Compiler :: compileSymbolCode(ClassScope& scope)
{
   CommandTape tape;

   bool publicAttr = scope.info.mattributes.exist(Attribute(caSerializable, 0));

   SyntaxTree tree;
   SyntaxWriter writer(tree);
   generateClassSymbol(writer, scope);

   _writer.generateSymbol(tape, tree.readRoot(), false, INVALID_REF);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);

   compileSymbolAttribtes(*scope.moduleScope, scope.reference, publicAttr);
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

ref_t Compiler :: compileClassPreloadedCode(_ModuleScope& scope, ref_t classRef, SNode node)
{
   _Module* module = scope.module;

   IdentifierString sectionName(scope.module->resolveReference(classRef));
   sectionName.append(INITIALIZER_SECTION);

   ref_t actionRef = module->mapReference(sectionName);

   CommandTape tape;
   _writer.generateInitializer(tape, actionRef, node);

   // create byte code sections
   _writer.saveTape(tape, scope);

   return actionRef;
}

//void Compiler :: compilePreloadedCode(_ModuleScope& scope, SNode node)
//{
//   _Module* module = scope.module;
//
//   IdentifierString sectionName("'", INITIALIZER_SECTION);
//
//   CommandTape tape;
//   _writer.generateInitializer(tape, module->mapReference(sectionName), node);
//
//   // create byte code sections
//   _writer.saveTape(tape, scope);
//}

void Compiler :: compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   if (classScope.abstractMode || test(classScope.info.header.flags, elDynamicRole)) {
      // dynamic class should not have default constructor
      classClassScope.abstractMode = true;
   }
   if (classScope.stackSafe) {
      classClassScope.stackSafe = true;
   }

   // NOTE : class class is not inheritable
   classClassScope.info.header.parentRef = classScope.moduleScope->superReference;

   compileParentDeclaration(node, classClassScope, classClassScope.info.header.parentRef/*, true*/);

   generateClassDeclaration(node, classClassScope);

   // generate constructor attributes
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.start();
   while (!it.Eof()) {
      int hints = classClassScope.info.methodHints.get(Attribute(it.key(), maHint));
      if (test(hints, tpConstructor)) {
         classClassScope.info.methodHints.exclude(Attribute(it.key(), maReference));
         classClassScope.info.methodHints.add(Attribute(it.key(), maReference), classScope.reference);
      }
      else if (test(hints, tpSealed | tpStatic) && *it) {
         classScope.addAttribute(it.key(), maStaticInherited, classClassScope.reference);
         classScope.save();
      }

      it++;
   }

   classClassScope.save();
}

void Compiler :: compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   //// HOTFIX : due to current implementation the default constructor can be declared as a special method and a constructor;
   ////          only one is allowed
   //if (classScope.withImplicitConstructor && classClassScope.info.methods.exist(encodeAction(DEFAULT_MESSAGE_ID)))
   //   classScope.raiseError(errOneDefaultConstructor, node.findChild(lxNameAttr));

   //if (classClassScope.info.staticValues.Count() > 0)
   //   copyStaticFieldValues(node, classClassScope);

   compileClassVMT(node, classClassScope, classScope);

   generateClassImplementation(node, classClassScope);
}

void Compiler :: initialize(ClassScope& scope, MethodScope& methodScope)
{
   methodScope.hints = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
   methodScope.outputRef = scope.info.methodHints.get(ClassInfo::Attribute(methodScope.message, maReference));
   if (test(methodScope.hints, tpInitializer))
      methodScope.scopeMode = methodScope.scopeMode | INITIALIZER_SCOPE;

////   methodScope.dispatchMode = _logic->isDispatcher(scope.info, methodScope.message);
   methodScope.classStacksafe= _logic->isStacksafeArg(scope.info);
   methodScope.withOpenArg = isOpenArg(methodScope.message);

   methodScope.extensionMode = scope.extensionClassRef != 0;
   methodScope.functionMode = test(methodScope.message, FUNCTION_MESSAGE);

   methodScope.multiMethod = _logic->isMultiMethod(scope.info, methodScope.message);
   methodScope.abstractMethod = _logic->isMethodAbstract(scope.info, methodScope.message);
   methodScope.yieldMethod = _logic->isMethodYieldable(scope.info, methodScope.message);
   methodScope.generic = _logic->isMethodGeneric(scope.info, methodScope.message);
   if (_logic->isMixinMethod(scope.info, methodScope.message)) {
      if (methodScope.withOpenArg && methodScope.functionMode)
         methodScope.mixinFunction = true;
   }   

   methodScope.targetSelfMode = test(methodScope.hints, tpTargetSelf);
   methodScope.constMode = test(methodScope.hints, tpConstant);
}

void Compiler :: declareVMT(SNode node, ClassScope& scope, bool& withConstructors, bool& withDefaultConstructor)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current.compare(lxFieldInit, lxFieldAccum)) {
         scope.withInitializers = true;
      }
      else if (current == lxClassMethod) {
         MethodScope methodScope(&scope);

         declareMethodAttributes(current, methodScope);

         if (current.argument == 0) {
            if (scope.extensionClassRef != 0)
               methodScope.extensionMode = true;

            // NOTE : an extension message must be strong-resolved
            declareArgumentList(current, methodScope, methodScope.extensionMode | test(scope.info.header.flags, elNestedClass), true);
            current.setArgument(methodScope.message);
         }
         else methodScope.message = current.argument;

         if (test(methodScope.hints, tpConstructor)) {
            if ((_logic->isAbstract(scope.info) || scope.abstractMode) && !methodScope.isPrivate() 
               && !test(methodScope.hints, tpProtected)) 
            {
               // abstract class cannot have nonpublic constructors
               scope.raiseError(errIllegalMethod, current);
            }
            else current = lxConstructor;

            withConstructors = true;
            if ((methodScope.message & ~STATIC_MESSAGE) == scope.moduleScope->constructor_message) {
               withDefaultConstructor = true;
            }
            else if (getArgCount(methodScope.message) == 1 && test(methodScope.hints, tpProtected)) {
               // check if it is protected default constructor
               ref_t dummy = 0;
               ident_t actionName = scope.module->resolveAction(getAction(methodScope.message), dummy);
               if (actionName.endsWith(CONSTRUCTOR_MESSAGE))
                  withDefaultConstructor = true;
            }
         }
         else if (test(methodScope.hints, tpPredefined)) {
            // recognize predefined message signatures
            predefineMethod(current, scope, methodScope);

            current = lxIdle;
         }
         else if (test(methodScope.hints, tpStatic)) {
            current = lxStaticMethod;
         }
         else if (test(methodScope.hints, tpYieldable)) {
            scope.info.header.flags |= elWithYieldable;

            // HOTFIX : the class should have intializer method
            scope.withInitializers = true;
         }

         if (!_logic->validateMessage(*methodScope.moduleScope, methodScope.message, methodScope.hints)) {
            if (test(methodScope.hints, tpConstant)) {
               scope.raiseError(errInvalidConstAttr, current);
            }
            else scope.raiseError(errIllegalMethod, current);
         }
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
////         if (test(current.argument, elExtension)) {
////            SNode argRef = current.findChild(lxClassRefAttr, lxAttribute);
////            if (argRef == lxClassRefAttr) {
////               extensionTypeRef = scope.moduleScope->mapFullReference(argRef.identifier(), true);
////            }
////            else if (argRef == lxAttribute) {
////               if (argRef.argument == V_ARGARRAY) {
////                  // HOTFIX : recognize open argument extension
////                  extensionTypeRef = V_ARGARRAY;
////               }
////               else scope.raiseError(errInvalidHint, root);
////            }
////         }
      }
////      else if (current == lxTarget) {
////         extensionTypeRef = current.argument;
////      }

      current = current.nextNode();
   }

   // check if extension is qualified
   bool extensionMode = test(scope.info.header.flags, elExtension);
   if (extensionMode) {

      scope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(scope.extensionClassRef, 0));
   }
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxTree::Node current, FieldAttributes& attrs, bool singleField)
{
   ref_t classRef = attrs.fieldRef;
   ref_t elementRef = attrs.elementRef;
   int   sizeHint = attrs.size;
   bool  embeddable = attrs.isEmbeddable;

   if (sizeHint == -1) {
      if (singleField) {
         scope.info.header.flags |= elDynamicRole;
      }
      else if (!test(scope.info.header.flags, elStructureRole)) {
         classRef = resolvePrimitiveArray(scope,
            scope.moduleScope->arrayTemplateReference,
            classRef, false);
      }
      else scope.raiseError(errIllegalField, current);

      sizeHint = 0;
   }

   int flags = scope.info.header.flags;
   int offset = 0;
   ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();

   // a role cannot have fields
   if (test(flags, elStateless))
      scope.raiseError(errIllegalField, current);

   int size = (classRef != 0) ? _logic->defineStructSize(*scope.moduleScope, classRef, elementRef) : 0;
   bool fieldArray = false;
   if (sizeHint != 0) {
      if (isPrimitiveRef(classRef) && (size == sizeHint || (classRef == V_INT32 && sizeHint <= size))) {
         // for primitive types size should be specified
         size = sizeHint;
      }
      else if (size > 0) {
         size *= sizeHint;

         // HOTFIX : to recognize the fixed length array
         if (elementRef == 0 && !isPrimitiveRef(classRef))
            elementRef = classRef;

         fieldArray = true;
         classRef = _logic->definePrimitiveArray(*scope.moduleScope, elementRef, true);
      }
      else scope.raiseError(errIllegalField, current);
   }

   if (test(flags, elWrapper) && scope.info.fields.Count() > 0) {
      // wrapper may have only one field
      scope.raiseError(errIllegalField, current);
   }
   // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
   else if (embeddable && !fieldArray) {
      if (!singleField || scope.info.fields.Count() > 0)
         scope.raiseError(errIllegalField, current);

      // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
      if (test(scope.info.header.flags, elSealed)) {
         scope.info.header.flags |= elWrapper;
         if (size > 0 && !test(scope.info.header.flags, elNonStructureRole))
            scope.info.header.flags |= elStructureRole;
      }
   }

   // a class with a dynamic length structure must have no fields
   if (test(scope.info.header.flags, elDynamicRole)) {
      if (scope.info.size == 0 && scope.info.fields.Count() == 0) {
         // compiler magic : turn a field declaration into an array or string one
         if (size > 0 && !test(scope.info.header.flags, elNonStructureRole)) {
            scope.info.header.flags |= elStructureRole;
            scope.info.size = -size;
         }

         ref_t arrayRef = _logic->definePrimitiveArray(*scope.moduleScope, classRef, test(scope.info.header.flags, elStructureRole));

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
         scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, elementRef));

         if (isPrimitiveRef(classRef))
            _logic->tweakPrimitiveClassFlags(classRef, scope.info);
      }
      // if it is a normal field
      else {
         // primitive / virtual classes cannot be declared
         if (size != 0 && isPrimitiveRef(classRef))
            scope.raiseError(errIllegalField, current);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.Count();
         scope.info.fields.add(terminal, offset);

         if (classRef != 0)
            scope.info.fieldTypes.add(offset, ClassInfo::FieldInfo(classRef, 0));
      }
  }

//   if (attrs.messageRef != 0) {
//      scope.addAttribute(attrs.messageRef, attrs.messageAttr, offset);
//   }
}

inline SNode findInitNode(SNode node, ident_t name)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxFieldInit) {
         SNode terminalNode = current.firstChild(lxTerminalMask);
         if (terminalNode.identifier().compare(name))
            break;
      }

      current = current.nextNode();
   }

   return current;
}

void Compiler :: generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, ref_t, bool isStatic, 
   bool isConst, bool isArray)
{
   _Module* module = scope.module;

   ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();

   if (scope.info.statics.exist(terminal)) {
      if (current.argument == INVALID_REF) {
         //HOTFIX : ignore duplicate autogenerated fields
         return;
      }
      else scope.raiseError(errDuplicatedField, current);
   }

   if (isStatic) {
      // if it is a static field
      ref_t ref = current.argument;
      if (!ref) {
         // generate static reference
         IdentifierString name(module->resolveReference(scope.reference));
         name.append(STATICFIELD_POSTFIX);

         ref = scope.moduleScope->mapAnonymous(name.c_str());

         current.setArgument(ref);
      }

      scope.info.statics.add(terminal, ClassInfo::FieldInfo(ref, fieldRef));
      if (isConst) {
         // HOTFIX : add read-only attribute (!= mskStatRef)
         scope.info.staticValues.add(ref, mskConstantRef);
      }
   }
   else {
      if (scope.classClassMode) {
         int index = current.findChild(lxStatIndex).argument;
         ref_t statRef = current.findChild(lxStatConstRef).argument;
         if (!statRef)
            throw InternalError("Cannot compile const field"); // !! temporal

         scope.info.statics.add(terminal, ClassInfo::FieldInfo(index, fieldRef));
         scope.info.staticValues.add(index, statRef);
      }
      else {
         int index = ++scope.info.header.staticSize;
         index = -index - 4;

         scope.info.statics.add(terminal, ClassInfo::FieldInfo(index, fieldRef));

         if (isConst) {
            ref_t statRef = 0;

            // HOTFIX : constant must have initialization part
            SNode initNode = findInitNode(current.parentNode(), terminal);
            if (initNode != lxNone) {
               SNode assignNode = initNode.findChild(lxAssign).nextNode(lxObjectMask);
               if (assignNode == lxExpression)
                  assignNode = assignNode.firstChild();

               if (assignNode == lxMetaConstant) {
                  // HOTFIX : recognize meta constants
                  if (assignNode.identifier().compare(CLASSNAME_VAR)) {
                     statRef = CLASSNAME_CONST;

                     // comment out the initializer
                     initNode = lxIdle;
                  }
                  else if (assignNode.identifier().compare(PACKAGE_VAR)) {
                     statRef = PACKAGE_CONST;

                     // comment out the initializer
                     initNode = lxIdle;
                  }
                  else if (assignNode.identifier().compare(SUBJECT_VAR)) {
                     ref_t subjRef = resolveSubjectVar(current);
                     if (subjRef) {
                        assignNode.set(lxSubjectRef, subjRef);

                        statRef = mapStaticField(scope.moduleScope, scope.reference, isArray);
                     }
                     else scope.raiseError(errInvalidOperation, current);
                  }
                  else scope.raiseError(errInvalidOperation, current);
               }
               else statRef = mapStaticField(scope.moduleScope, scope.reference, isArray);
            }
            else if (isArray) {
               //HOTFIX : allocate an empty array
               statRef = mapStaticField(scope.moduleScope, scope.reference, isArray);

               auto section = scope.module->mapSection((statRef & ~mskAnyRef) | mskRDataRef, false);
               section->addReference(fieldRef | mskVMTRef, (ref_t)-4);
            }
            else scope.raiseError(errInvalidOperation, current);

            scope.info.staticValues.add(index, statRef);

            current.appendNode(lxStatConstRef, statRef);
            current.appendNode(lxStatIndex, index);
         }
         else scope.raiseError(errDuplicatedField, current);
         //else scope.info.staticValues.add(index, (ref_t)mskStatRef);
      }
   }
}

inline SNode findName(SNode node)
{
   return node.findChild(lxNameAttr).firstChild(lxTerminalMask);
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, ref_t message, bool allowTypeAttribute)
{
   ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));
   bool hintChanged = false, outputChanged = false;
   int hint = scope.info.methodHints.get(Attribute(message, maHint));

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
      else if (current.compare(lxType, lxArrayType)) {
         if (!allowTypeAttribute) {
            scope.raiseError(errTypeNotAllowed, node);
         }
         else {
            ref_t ref = resolveTypeAttribute(current, scope, true, false);
            if (!outputRef) {
               outputRef = ref;
            }
            else if (outputRef != ref)
               scope.raiseError(errTypeAlreadyDeclared, node);

            outputChanged = true;
         }
      }
      current = current.nextNode();
   }

   if (test(hint, tpPrivate)) {
      if (scope.info.methods.exist(message & ~STATIC_MESSAGE)) {
         // there should be no public method with the same name
         scope.raiseError(errDupPublicMethod, findName(node));
      }
      // if it is private message save the visibility attribute
      else scope.addAttribute(message & ~STATIC_MESSAGE, maPrivate, message);
   }
   else if (testany(hint, tpInternal | tpProtected)) {
      // if it is internal / protected message save the visibility attribute
      ref_t signRef = 0;
      ident_t name = scope.module->resolveAction(getAction(message), signRef);
      ref_t publicMessage = 0;
      if (name.compare(CONSTRUCTOR_MESSAGE2)) {
         publicMessage = scope.moduleScope->constructor_message;
      }
      else {
         int index = name.find("$$");
         if (index == NOTFOUND_POS)
            scope.raiseError(errDupInternalMethod, findName(node));

         publicMessage = overwriteAction(message, scope.module->mapAction(name + index + 2, 0, false));
      }
      if (scope.info.methods.exist(publicMessage)) {
         // there should be no public method with the same name
         scope.raiseError(errDupPublicMethod, findName(node));
      }
      // if it is protected / internal message save the visibility attribute
      else if (test(hint, tpProtected)) {
         scope.addAttribute(publicMessage, maProtected, message);
      }
      else scope.addAttribute(publicMessage, maInternal, message);
   }

   if (outputRef) {
      if (outputRef == scope.reference && _logic->isEmbeddable(scope.info)) {
         hintChanged = true;
         hint |= tpEmbeddable;
      }
      else if (_logic->isEmbeddable(*scope.moduleScope, outputRef)) {
         hintChanged = true;
         hint |= tpEmbeddable;
      }
   }

   if (hintChanged) {
      scope.addHint(message, hint);
   }
   if (outputChanged) {
      scope.info.methodHints.add(Attribute(message, maReference), outputRef);
   }
   else if (outputRef != 0 && !node.existChild(lxAutogenerated) && !test(hint, tpConstructor)
      && outputRef != scope.moduleScope->superReference)
   {
      //warn if the method output was not redeclared, ignore auto generated methods
      //!!hotfix : ignore the warning for the constructor
      scope.raiseWarning(WARNING_LEVEL_1, wrnTypeInherited, node);
   }
}

//void Compiler :: saveExtension(NamespaceScope& nsScope, ref_t reference, ref_t extensionClassRef, ref_t message, bool internalOne)
//{
//   nsScope.saveExtension(message, extensionClassRef, reference, internalOne);
//   //if (isOpenArg(message)/* && _logic->isMethodGeneric(scope.info, message)*/) {
//   //   // if it is an extension with open argument list generic handler
//   //   // creates the references for all possible number of parameters
//   //   for (int i = 1; i < OPEN_ARG_COUNT; i++) {
//   //      nsScope.saveExtension(overwriteParamCount(message, i), extensionClassRef, reference, internalOne);
//   //   }
//   //}
//}

void Compiler :: saveExtension(ClassScope& scope, ref_t message, bool internalOne)
{
   ref_t extensionMessage = 0;

   // get generic message
   ref_t signRef = 0;
   ident_t actionName = scope.module->resolveAction(getAction(message), signRef);
   if (signRef) {
      extensionMessage = overwriteAction(message, scope.module->mapAction(actionName, 0, false));
      if (test(extensionMessage, VARIADIC_MESSAGE))
         extensionMessage = overwriteArgCount(extensionMessage, 2);
   }
   else extensionMessage = message;

   // exclude function flag
   extensionMessage = extensionMessage & ~FUNCTION_MESSAGE;

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
   nsScope->saveExtension(extensionMessage, scope.reference, message, internalOne);
}

//inline bool isGeneralMessage(_Module* module, ref_t message)
//{
//   if (getParamCount(message) == 0) {
//      return true;
//   }
//   else {
//      ref_t signRef = 0;
//      module->resolveAction(getAction(message), signRef);
//
//      return signRef == 0;
//   }
//}

void Compiler :: predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope)
{
   SNode body = node.findChild(lxCode);
   if (body != lxCode || body.firstChild() != lxEOP)
      scope.raiseError(errPedefineMethodCode, node);

   if (test(scope.hints, tpAbstract) || (scope.hints & tpMask) == tpPrivate)
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

inline bool checkNonpublicDuplicates(ClassInfo& info, ref_t publicMessage)
{
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      Attribute key = it.key();
      if (key.value1 == publicMessage && (key.value2 == maPrivate || key.value2 == maProtected || key.value2 == maInternal))
         return true;
   }

   return false;
}

inline bool isDeclaredProtected(ClassInfo& info, ref_t publicMessage, ref_t& protectedMessage)
{
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      Attribute key = it.key();
      if (key.value1 == publicMessage && (key.value2 == maProtected)) {
         protectedMessage = *it;

         return true;
      }         
   }

   return false;
}

void Compiler :: generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed,
   bool allowTypeAttribute)
{
   ref_t message = current.argument;

   if (scope.info.methods.exist(message, true) && hideDuplicates) {
      // ignoring autogenerated duplicates
      current = lxIdle;

      return;
   }

   generateMethodAttributes(scope, current, message, allowTypeAttribute);

   int methodHints = scope.info.methodHints.get(ClassInfo::Attribute(message, maHint));
   if (isOpenArg(message)) {
      scope.info.header.flags |= elWithVariadics;
   }
   else if (_logic->isMethodGeneric(scope.info, message)) {
      scope.info.header.flags |= elWithGenerics;
   }

   // check if there is no duplicate method
   if (scope.info.methods.exist(message, true)) {
      scope.raiseError(errDuplicatedMethod, current);
   }
   else {
      bool privateOne = (methodHints & tpMask) == tpPrivate;
      bool included = scope.include(message);
      bool sealedMethod = (methodHints & tpMask) == tpSealed;
      // if the class is closed, no new methods can be declared
      // except private sealed ones (which are declared outside the class VMT)
      if (included && closed && !privateOne) {
         //ref_t dummy = 0;
         //ident_t msg = scope.module->resolveAction(getAction(message), dummy);
         scope.moduleScope->printMessageInfo(infoNewMethod, message);

         scope.raiseError(errClosedParent, findParent(current, lxClass/*, lxNestedClass*/));
      }

      // if the method is sealed, it cannot be overridden
      if (!included && sealedMethod/* && !test(methodHints, tpAbstract)*/) {
         scope.raiseError(errClosedMethod, findParent(current, lxClass/*, lxNestedClass*/));
      }

      // HOTFIX : make sure there are no duplicity between public and private / internal / statically linked ones
      if (!test(message, STATIC_MESSAGE)) {
         if (scope.info.methods.exist(message | STATIC_MESSAGE))
            scope.raiseError(errDuplicatedMethod, current);
      }
      else if (!privateOne && !testany(methodHints, tpInternal | tpProtected)) {
         if (checkNonpublicDuplicates(scope.info, message))
            scope.raiseError(errDuplicatedMethod, current);
      }

      if (_logic->isStacksafeArg(scope.info) && !test(methodHints, tpMultimethod)) {
         // add a stacksafe attribute for the embeddable structure automatically, except multi-methods
         methodHints |= tpStackSafe;

         scope.addHint(message, tpStackSafe);
         if (privateOne) {
            // HOTFIX : if it is private message save its hints as public one
            scope.addHint(message & ~STATIC_MESSAGE, tpStackSafe);
         }
      }

      if (!included && test(methodHints, tpAbstract)) {
         scope.removeHint(message, tpAbstract);
      }

      if (test(methodHints, tpPredefined)) {
         // exclude the predefined attribute from declared method
         scope.removeHint(message, tpPredefined);
      }

      if (test(scope.info.header.flags, elExtension) && (privateOne || test(methodHints, tpInternal)))
         // private / internal methods cannot be declared in the extension
         scope.raiseError(errIllegalPrivate, current);

      if (test(scope.info.header.flags, elExtension) && !privateOne && !scope.extensionDispatcher) {
         saveExtension(scope, message, scope.visibility != Visibility::Public);
      }

      if (!closed && test(methodHints, tpEmbeddable)
         && !testany(methodHints, tpDispatcher | tpFunction | tpConstructor | tpConversion | tpGeneric | tpCast)
         && !test(message, VARIADIC_MESSAGE)
         && !current.existChild(lxDispatchCode, lxResendExpression))
      {
         // COMPILER MAGIC : if embeddable returning argument is allowed
         ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));

         bool embeddable = false;
         if (test(methodHints, tpSetAccessor)) {
            // HOTFIX : the result of set accessor should not be embeddable
         }
         else if (outputRef == scope.reference && _logic->isEmbeddable(scope.info)) {
            embeddable = true;
         }
         else if (_logic->isEmbeddable(*scope.moduleScope, outputRef)) {
            embeddable = true;
         }

         if (embeddable) {
            ref_t dummy, flags;
            int argCount;
            decodeMessage(message, dummy, argCount, flags);

            // declare a method with an extra argument - retVal
            IdentifierString privateName(EMBEDDAMLE_PREFIX);
            ref_t signRef = 0;
            privateName.append(scope.module->resolveAction(getAction(message), signRef));
            ref_t signArgs[ARG_COUNT];
            size_t signLen = scope.module->resolveSignature(signRef, signArgs);
            if (signLen == (size_t)argCount - 1) {
               // HOTFIX : inject emmeddable returning argument attribute only if the message is strong
               signArgs[signLen++] = resolvePrimitiveReference(scope, V_WRAPPER, outputRef, true);
               ref_t embeddableMessage = encodeMessage(
                  scope.module->mapAction(privateName.c_str(), scope.module->mapSignature(signArgs, signLen, false), false),
                  argCount + 1,
                  flags);

               if (!test(scope.info.header.flags, elSealed) || scope.info.methods.exist(embeddableMessage)) {
                  scope.include(embeddableMessage);
               }
               else embeddableMessage |= STATIC_MESSAGE;

               scope.addAttribute(message, maEmbeddableRet, embeddableMessage);
            }
         }
      }
   }
}

ref_t Compiler :: resolveMultimethod(ClassScope& scope, ref_t messageRef)
{
   int argCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(messageRef, actionRef, argCount, flags);

   if (test(messageRef, FUNCTION_MESSAGE)) {
      if (argCount == 0)
         return 0;
   }
   else if (argCount == 1)
      return 0;

   ident_t actionStr = scope.module->resolveAction(actionRef, signRef);

   if (test(flags, VARIADIC_MESSAGE)) {
      // COMPILER MAGIC : for variadic message - use the most general message
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);

      int genericArgCount = 2;
      // HOTFIX : a variadic extension is a special case of variadic function
      // - so the target should be included as well
      if (test(messageRef, FUNCTION_MESSAGE) && scope.extensionClassRef == 0)
         genericArgCount = 1;

      ref_t genericMessage = encodeMessage(genericActionRef, genericArgCount, flags);

      return genericMessage;
   }
   else if (signRef) {
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);
      ref_t genericMessage = encodeMessage(genericActionRef, argCount, flags);

      return genericMessage;
   }

   return 0;
}

void Compiler :: generateMethodDeclarations(SNode root, ClassScope& scope, bool closed, LexicalType methodType,
   bool allowTypeAttribute)
{
   //bool extensionMode = scope.extensionClassRef != 0;
   bool templateMethods = false;
   List<ref_t> implicitMultimethods;

   // first pass - mark all multi-methods
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == methodType) {
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
         if (!current.existChild(lxAutogenerated)) {
            generateMethodDeclaration(current, scope, false, closed, allowTypeAttribute);
         }
         else templateMethods = true;
      }
      current = current.nextNode();
   }

   //COMPILER MAGIC : if strong signature is declared - the compiler should contain the virtual multi method
   if (implicitMultimethods.Count() > 0) {
      _logic->injectVirtualMultimethods(*scope.moduleScope, root, *this, implicitMultimethods, methodType);
   }

   if (templateMethods) {
      // third pass - do not include overwritten template-based methods
      current = root.firstChild();
      while (current != lxNone) {
         if (current.existChild(lxAutogenerated) && (current == methodType)) {
            generateMethodDeclaration(current, scope, true, closed, allowTypeAttribute);
         }
         current = current.nextNode();
      }
   }

   if (implicitMultimethods.Count() > 0)
      _logic->verifyMultimethods(*scope.moduleScope, root, scope.info, implicitMultimethods);
}

void Compiler :: generateClassDeclaration(SNode node, ClassScope& scope, bool nestedDeclarationMode)
{
   bool closed = test(scope.info.header.flags, elClosed);

   if (scope.classClassMode) {
      // generate static fields
      generateClassFields(node, scope, countFields(node) == 1);
   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, node);

////      if (test(scope.info.header.flags, elExtension)) {
////         scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;
////      }

      // inject virtual fields
      _logic->injectVirtualFields(*scope.moduleScope, node, scope.reference, scope.info, *this);

      // generate fields
      generateClassFields(node, scope, countFields(node) == 1);

      if (_logic->isStacksafeArg(scope.info))
         scope.stackSafe = true;

      if (scope.extensionClassRef != 0 && _logic->isStacksafeArg(*scope.moduleScope, scope.extensionClassRef))
         scope.stackSafe = true;

      if (scope.withInitializers) {
         // HOTFIX : recognize compile-time assinging
         if (!recognizeCompileTimeAssigning(node, scope)) {
            // add special method initalizer
            scope.include(scope.moduleScope->init_message);

            int attrValue = V_INITIALIZER;
            bool dummy = false;
            _logic->validateMethodAttribute(attrValue, dummy);

            scope.addAttribute(scope.moduleScope->init_message, maHint, attrValue);
         }
      }
   }

   _logic->injectVirtualCode(*scope.moduleScope, node, scope.reference, scope.info, *this, closed);

   // generate methods
   if (scope.classClassMode) {
      generateMethodDeclarations(node, scope, closed, lxConstructor, false);
      generateMethodDeclarations(node, scope, closed, lxStaticMethod, true);
   }
   else generateMethodDeclarations(node, scope, closed, lxClassMethod, true);

   bool withAbstractMethods = false;
   bool disptacherNotAllowed = false;
   bool emptyStructure = false;
   _logic->validateClassDeclaration(*scope.moduleScope, scope.info, withAbstractMethods, disptacherNotAllowed, emptyStructure);
   if (withAbstractMethods) {
      scope.raiseError(errAbstractMethods, node);
   }
   if (disptacherNotAllowed)
      scope.raiseError(errDispatcherInInterface, node);
   if (emptyStructure)
      scope.raiseError(errEmptyStructure, node.findChild(lxNameAttr));

   // do not set flags for closure declaration - they will be set later
   if (!nestedDeclarationMode) {
      _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, scope.classClassMode);
   }
}

void Compiler :: declareMethodAttributes(SNode node, MethodScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      bool explicitMode = false;
      if (current == lxAttribute) {
         int value = current.argument;
         if (_logic->validateMethodAttribute(value, explicitMode)) {
            scope.hints |= value;

            current.setArgument(value);
         }
         else {
            current.setArgument(0);

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current.compare(lxType, lxArrayType)) {
         // if it is a type attribute
         scope.outputRef = resolveTypeAttribute(current, scope, true, false);
      }
      else if (current == lxNameAttr && !explicitMode) {
         // resolving implicit method attributes
         int attr = scope.moduleScope->attributes.get(current.firstChild(lxTerminalMask).identifier());
         if (_logic->validateImplicitMethodAttribute(attr/*, current.nextNode().type == lxMessage*/)) {
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
   if (node == lxNone) {
   }
   else {
      SNode typeNode = node.findChild(lxType);
      parentRef = resolveTypeAttribute(typeNode, scope, silentMode, false);
   }

//   else if (test(node.type, lxTerminalMask)) {
//      parentRef = resolveImplicitIdentifier(scope, node);
//   }
//   else if (node.existChild(lxTypeAttribute)) {
//      // if it is a template based class
//      parentRef = resolveTemplateDeclaration(node, scope, silentMode);
//   }
//   else parentRef = resolveImplicitIdentifier(scope, node.firstChild(lxTerminalMask));
//
//   if (parentRef == 0 && !silentMode)
//      scope.raiseError(errUnknownClass, node);

   return parentRef;
}

ref_t Compiler :: retrieveImplicitIdentifier(NamespaceScope& scope, ident_t identifier, bool referenceOne, bool innermost)
{
   ref_t reference = scope.resolveImplicitIdentifier(identifier, false, innermost);

   if (!reference && scope.parent != nullptr) {
      NamespaceScope* parentScope = (NamespaceScope*)scope.parent->getScope(Scope::ScopeLevel::slNamespace);
      if (parentScope)
         return retrieveImplicitIdentifier(*parentScope, identifier, referenceOne, false);
   }

   return reference;
}

void Compiler :: importClassMembers(SNode classNode, SNode importNode, NamespaceScope& scope)
{
   SNode nameNode = importNode.firstChild(lxTerminalMask);

   List<SNode> parameters;
   IdentifierString templateName;
   templateName.copy(nameNode.identifier());

   SNode current = importNode.findChild(lxType);
   while (current == lxType) {
      parameters.add(current);

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(parameters.Count());

   ref_t templateRef = retrieveImplicitIdentifier(scope, templateName.c_str(), false, true);
   if (!templateRef)
      scope.raiseError(errInvalidSyntax, importNode);

   SyntaxTree bufferTree;
   SyntaxWriter bufferWriter(bufferTree);
   bufferWriter.newNode(lxRoot);
   scope.moduleScope->importClassTemplate(bufferWriter, templateRef, parameters);
   bufferWriter.closeNode();

   SyntaxTree::copyNode(bufferTree.readRoot(), classNode);
}

void Compiler :: compileClassDeclaration(SNode node, ClassScope& scope)
{
   bool extensionDeclaration = isExtensionDeclaration(node);
   compileParentDeclaration(node.findChild(lxParent), scope, extensionDeclaration);

   if (scope.visibility == Visibility::Public) {
      // add seriazible meta attribute for the public class
      scope.info.mattributes.add(Attribute(caSerializable, 0), INVALID_REF);
   }

   // COMPILER MAGIC : "inherit" sealed static methods
   for (auto a_it = scope.info.methodHints.start(); !a_it.Eof(); a_it++) {
      auto key = a_it.key();

      if (key.value2 == maStaticInherited) {
         SNode methNode = node.insertNode(lxStaticMethod, key.value1);
         methNode
            .appendNode(lxResendExpression)
            .appendNode(lxImplicitJump, key.value1)
            .appendNode(lxCallTarget, *a_it);
      }
   }

   bool withConstructors = false;
   bool withDefaultConstructor = false;
   declareVMT(node, scope, withConstructors, withDefaultConstructor);

   // NOTE : generateClassDeclaration should be called for the proper class before a class class one
   //        due to dynamic array implementation (auto-generated default constructor should be removed)
   generateClassDeclaration(node, scope);

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

   // if it is a super class validate it, generate built-in attributes
   if (scope.info.header.parentRef == 0 && scope.reference == scope.moduleScope->superReference) {
      if (!scope.info.methods.exist(scope.moduleScope->dispatch_message))
         scope.raiseError(errNoDispatcher, node);

      // HOTFIX - constant fields cannot be compiled for super class class (because they are already
      //          declared in super class itself), so we mark them as auto-generated, so they will be
      //          skipped
      SNode current = node.firstChild();
      while (current != lxNone) {
         if (current == lxClassField)
            current.setArgument(INVALID_REF);

         current = current.nextNode();
      }
   }

   // save declaration
   scope.save();

   // compile class class if it available
   if (scope.info.header.classRef != scope.reference && scope.info.header.classRef != 0) {
      ClassScope classClassScope((NamespaceScope*)scope.parent, scope.info.header.classRef, scope.visibility);
      classClassScope.info.header.flags |= elClassClass; // !! IMPORTANT : classclass flags should be set
      classClassScope.classClassMode = true;

      if (!withDefaultConstructor && !scope.abstractMode && !test(scope.info.header.flags, elDynamicRole)) {
         // if default constructor has to be created
         injectDefaultConstructor(*scope.moduleScope, node, scope.reference, withConstructors);
      }

      compileClassClassDeclaration(node, classClassScope, scope);

      // HOTFIX : if the default constructor is private - a class cannot be inherited
      int hints = classClassScope.info.methodHints.get(Attribute(scope.moduleScope->constructor_message | STATIC_MESSAGE, maHint));
      if ((hints & tpMask) == tpPrivate) {
         scope.info.header.flags |= elFinal;
         scope.save();
      }
   }
}

bool isClassMethod(LexicalType type)
{
   return type == lxClassMethod;
}

bool isClassClassMethod(LexicalType type)
{
   return type == lxConstructor || type == lxStaticMethod;
}

void Compiler :: generateClassImplementation(SNode node, ClassScope& scope)
{
   analizeClassTree(node, scope,
      scope.classClassMode ? isClassClassMethod : isClassMethod);

   pos_t sourcePathRef = scope.saveSourcePath(_writer);

   CommandTape tape;
   _writer.generateClass(*scope.moduleScope, tape, node, scope.reference, sourcePathRef,
      scope.classClassMode ? isClassClassMethod : isClassMethod);

   // optimize
   optimizeTape(tape);

   //// create byte code sections
   //scope.save();
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compileClassImplementation(SNode node, ClassScope& scope)
{
   if (test(scope.info.header.flags, elExtension)) {
      scope.extensionClassRef = scope.info.fieldTypes.get(-1).value1;

      scope.stackSafe = _logic->isStacksafeArg(*scope.moduleScope, scope.extensionClassRef);
   }
   else if (_logic->isStacksafeArg(scope.info)) {
      scope.stackSafe = true;
   }

   // validate field types
   if (scope.info.fieldTypes.Count() > 0) {
      validateClassFields(node, scope);
   }
   else if (scope.info.statics.Count() > 0) {
      //HOTFIX : validate static fields as well
      validateClassFields(node, scope);
   }

   compileVMT(node, scope);

   generateClassImplementation(node, scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (scope.extensionClassRef == 0 && scope.info.header.classRef != 0) {
      compileSymbolCode(scope);
   }
}

void Compiler :: compileSymbolDeclaration(SNode node, SymbolScope& scope)
{
   declareSymbolAttributes(node, scope, true, false);

//   if (scope.moduleScope->module->mapSection(scope.reference | mskMetaRDataRef, true) == nullptr) {
      scope.save();
//   }
}

bool Compiler :: compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal, bool accumulatorMode, ref_t accumulatorRef)
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   ref_t parentRef = 0;
   ref_t classRef = resolveConstant(retVal,  parentRef);
   if (classRef) {
      if (retVal.kind == okSingleton) {
         // HOTFIX : singleton should be treated differently
         scope.info.type = SymbolExpressionInfo::Type::Singleton;
         scope.info.exprRef = classRef;
      }
      else if (retVal.kind == okConstantSymbol) {
         scope.info.type = SymbolExpressionInfo::Type::ConstantSymbol;
         scope.info.exprRef = classRef;
      }
      else if (retVal.kind == okArrayConst) {
         scope.info.type = SymbolExpressionInfo::Type::ConstantSymbol;
         scope.info.exprRef = classRef;
      }

      nsScope->defineConstantSymbol(classRef, parentRef);

      return true;
   }

   _Module* module = scope.moduleScope->module;
   MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

   if (accumulatorMode) {
      if (dataWriter.Position() == 0 && ((Section*)dataWriter.Memory())->References().Eof()) {
         dataWriter.Memory()->addReference(accumulatorRef | mskVMTRef, (ref_t)-4);
      }

      if (retVal.kind == okLiteralConstant) {
         dataWriter.Memory()->addReference(retVal.param | mskLiteralRef, dataWriter.Position());

         dataWriter.writeDWord(0);
      }
      else if (retVal.kind == okWideLiteralConstant) {
         dataWriter.Memory()->addReference(retVal.param | mskWideLiteralRef, dataWriter.Position());

         dataWriter.writeDWord(0);
      }
      else if (retVal.kind == okMessageConstant) {
         dataWriter.Memory()->addReference(retVal.param | mskMessage, dataWriter.Position());

         dataWriter.writeDWord(0);
      }
      else if (retVal.kind == okMessageNameConstant) {
         dataWriter.Memory()->addReference(retVal.param | mskMessageName, dataWriter.Position());

         dataWriter.writeDWord(0);
      }
      else if (retVal.kind == okClass || retVal.kind == okClassSelf) {
         dataWriter.Memory()->addReference(retVal.param | mskVMTRef, dataWriter.Position());
         dataWriter.writeDWord(0);
      }
      else {
         SymbolScope memberScope(nsScope, nsScope->moduleScope->mapAnonymous(), Visibility::Public);
         if (!compileSymbolConstant(memberScope, retVal, false, 0))
            return false;

         dataWriter.Memory()->addReference(memberScope.reference | mskConstantRef, dataWriter.Position());
         dataWriter.writeDWord(0);
      }

      return true;
   }
   else {
      if (dataWriter.Position() > 0)
         return false;

      if (retVal.kind == okIntConstant || retVal.kind == okUIntConstant) {
         size_t value = module->resolveConstant(retVal.param).toULong(16);

         dataWriter.writeDWord(value);

         parentRef = scope.moduleScope->intReference;
      }
      else if (retVal.kind == okLongConstant) {
         long long value = module->resolveConstant(retVal.param).toLongLong(10, 1);

         dataWriter.write(&value, 8u);

         parentRef = scope.moduleScope->longReference;
      }
      else if (retVal.kind == okRealConstant) {
         double value = module->resolveConstant(retVal.param).toDouble();

         dataWriter.write(&value, 8u);

         parentRef = scope.moduleScope->realReference;
      }
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
      else if (retVal.kind == okCharConstant) {
         ident_t value = module->resolveConstant(retVal.param);

         dataWriter.writeLiteral(value, getlength(value));

         parentRef = scope.moduleScope->charReference;
      }
      else if (retVal.kind == okMessageNameConstant) {
         dataWriter.Memory()->addReference(retVal.param | mskMessageName, dataWriter.Position());
         dataWriter.writeDWord(0);

         parentRef = scope.moduleScope->messageNameReference;
      }
      else if (retVal.kind == okClass || retVal.kind == okClassSelf) {
         dataWriter.Memory()->addReference(retVal.param | mskVMTRef, dataWriter.Position());
         dataWriter.writeDWord(0);

         parentRef = scope.moduleScope->superReference;
      }
      else return false;

      dataWriter.Memory()->addReference(parentRef | mskVMTRef, (ref_t)-4);

      if (parentRef == scope.moduleScope->intReference) {
         nsScope->defineConstantSymbol(scope.reference, V_INT32);
      }
      else nsScope->defineConstantSymbol(scope.reference, parentRef);
   }

   return true;
}

void Compiler :: compileSymbolAttribtes(_ModuleScope& moduleScope, ref_t reference, bool publicAttr)
{
   ClassInfo::CategoryInfoMap mattributes;
   if (publicAttr) {
      // add seriazible meta attribute for the public symbol
      mattributes.add(Attribute(caSymbolSerializable, 0), INVALID_REF);
   }

   if (mattributes.Count() > 0) {
      // initialize attribute section writers
      MemoryWriter attrWriter(moduleScope.mapSection(reference | mskSymbolAttributeRef, false));

      mattributes.write(&attrWriter);
   }
}

void Compiler :: compileSymbolImplementation(SNode node, SymbolScope& scope)
{
   bool isStatic = scope.staticOne;

   SNode expression = node.findChild(lxExpression);

   EAttr exprMode = scope.info.type == SymbolExpressionInfo::Type::Constant ? HINT_CONSTEXPR : EAttr::eaNone;

   ExprScope exprScope(&scope);
   // HOTFIX : due to implementation (compileSymbolConstant requires constant types) typecast should be done explicitly
   ObjectInfo retVal = compileExpression(expression, exprScope,
      mapObject(expression, exprScope, HINT_ROOTSYMBOL), 0, exprMode);

   if (scope.info.exprRef == 0) {
      ref_t ref = resolveObjectReference(scope, retVal, true);
      if (ref != 0) {
         // HOTFIX : if the result of the operation is qualified - it should be saved as symbol type
         scope.info.exprRef = ref;
      }
   }
   else retVal = convertObject(expression, exprScope, scope.info.exprRef, retVal, EAttr::eaNone);

   expression.refresh();
   analizeSymbolTree(expression, scope);

   // create constant if required
   if (scope.info.type == SymbolExpressionInfo::Type::Constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression);

      if (!compileSymbolConstant(scope, retVal, false, 0))
         scope.raiseError(errInvalidOperation, expression);
   }

   scope.save();

   if (scope.preloaded) {
      compilePreloadedCode(scope);
   }

   pos_t sourcePathRef = scope.saveSourcePath(_writer);
   CommandTape tape;
   _writer.generateSymbol(tape, node, isStatic, sourcePathRef);

   // optimize
   optimizeTape(tape);

   compileSymbolAttribtes(*scope.moduleScope, scope.reference, scope.visibility == Visibility::Public);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compileStaticAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, int mode*//*, bool accumulatorMode*/)
{
//   // !! temporal
//   if (accumulatorMode)
//      scope.raiseError(errIllegalOperation, node);

   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   CodeScope codeScope(&scope);
   ExprScope exprScope(&codeScope);

   writer.newNode(lxExpression);
   SNode exprNode = writer.CurrentNode();
   writer.closeNode();

   SNode assignNode = exprNode.appendNode(lxAssigning);
   SNode targetNode = assignNode.appendNode(lxVirtualReference);
   SNode sourceNode = assignNode.appendNode(lxExpression);

//   if (!isSealedStaticField(target.param)) {
//      if (target.kind == okStaticField) {
//         writeTerminal(writer, node, codeScope, ObjectInfo(okClassStaticField, scope.reference, target.reference, target.element, target.param), HINT_NODEBUGINFO);
//      }
//      else if (target.kind == okStaticConstantField) {
//         writeTerminal(writer, node, codeScope, ObjectInfo(okClassStaticConstantField, scope.reference, target.reference, target.element, target.param), HINT_NODEBUGINFO);
//      }
//   }
   /*else */recognizeTerminal(targetNode, target, exprScope, HINT_NODEBUGINFO);

   SyntaxTree::copyNode(node, sourceNode);

   ObjectInfo source = compileExpression(sourceNode, exprScope, target.extraparam, HINT_NODEBUGINFO);

   analizeSymbolTree(expressionTree.readRoot(), scope);

   ref_t actionRef = compileClassPreloadedCode(*scope.moduleScope, scope.reference, expressionTree.readRoot());
   scope.info.mattributes.add(Attribute(caInitializer, 0), actionRef);
   scope.save();
}

ref_t targetResolver(void* param, ref_t mssg)
{
   return ((Map<ref_t, ref_t>*)param)->get(mssg);
}

ref_t Compiler :: compileExtensionDispatcher(NamespaceScope& scope, ref_t genericMessageRef)
{
   ref_t extRef = scope.moduleScope->mapAnonymous();
   ClassScope classScope(&scope, extRef, Visibility::Private);
   classScope.extensionDispatcher = true;

   // create a new overload list
   ClassInfo::CategoryInfoMap methods(0);
   Map<ref_t, ref_t> taregts;
   auto it = scope.extensions.getIt(genericMessageRef);
   while (!it.Eof()) {
      auto extInfo = *it;

      methods.add(Attribute(extInfo.value2, maMultimethod), genericMessageRef | FUNCTION_MESSAGE);
      taregts.add(extInfo.value2, extInfo.value1);

      it = scope.extensions.nextIt(genericMessageRef, it);
   }

   ref_t dispatchListRef = _logic->generateOverloadList(*scope.moduleScope, *this, genericMessageRef | FUNCTION_MESSAGE, methods,
      (void*)&taregts, targetResolver, elSealed);

   SyntaxTree classTree;
   SyntaxWriter writer(classTree);

   // build the class tree
   writer.newNode(lxRoot);
   writer.newNode(lxClass, extRef);

   injectVirtualMultimethod(*scope.moduleScope, writer.CurrentNode(),
      genericMessageRef | FUNCTION_MESSAGE, lxClassMethod, genericMessageRef, false);

   writer.closeNode();
   writer.closeNode();

   SNode classNode = classTree.readRoot().firstChild();

   // declare the extension
   compileParentDeclaration(classNode, classScope, scope.moduleScope->superReference);
   classScope.info.header.flags |= (elExtension | elSealed);
   classScope.info.header.classRef = classScope.reference;
   classScope.extensionClassRef = scope.moduleScope->superReference;
   classScope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(classScope.extensionClassRef, 0));

   classScope.info.methodHints.exclude(Attribute(genericMessageRef | FUNCTION_MESSAGE, maOverloadlist));
   classScope.info.methodHints.add(Attribute(genericMessageRef | FUNCTION_MESSAGE, maOverloadlist), dispatchListRef);

   generateMethodDeclaration(classNode.findChild(lxClassMethod), classScope, false, false, false);
   //generateMethodDeclarations(classNode, classScope, false, lxClassMethod, true);
   classScope.save();

   // compile the extension
   compileVMT(classNode, classScope);
   generateClassImplementation(classNode, classScope);

   return extRef;
}

// NOTE : elementRef is used for binary arrays
ObjectInfo Compiler :: allocateResult(ExprScope& scope, /*bool fpuMode, */ref_t targetRef, ref_t elementRef)
{
   int size = _logic->defineStructSize(*scope.moduleScope, targetRef, elementRef);
   if (size > 0) {
      ObjectInfo retVal;

      allocateTempStructure(scope, size, false, retVal);
      retVal.reference = targetRef;

      return retVal;
   }
   else {
      int tempLocal = scope.newTempLocal();

      return ObjectInfo(okLocal, tempLocal, targetRef, /*elementRef*/0, 0);
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
   if (reserveNode == lxNone) {
      reserved = 0;
   }

   // allocating space
   int offset = allocateStructure(false, size, reserved);

   // HOT FIX : size should be in bytes
   size *= 4;

   reserveNode.setArgument(reserved);

   return offset;
}

inline SNode injectRootSeqExpression(SNode& parent)
{
   SNode current;
   while (!parent.compare(lxNewFrame, lxCodeExpression, lxCode/*, lxReturning*/)) {
      current = parent;
      parent = parent.parentNode();
   }

   if (current != lxSeqExpression) {
      current.injectAndReplaceNode(lxSeqExpression);
   }

   return current;
}

void Compiler :: injectMemberPreserving(SNode node, ExprScope& scope, LexicalType tempType, int tempLocal, ObjectInfo member, int memberIndex)
{
   SNode parent = node;
   SNode current = injectRootSeqExpression(parent);

   ref_t targetRef = resolveObjectReference(scope, member, false);

   if (member.kind == okLocalAddress) {
      // if the parameter may be stack-allocated
      bool variable = false;
      int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, member.element, variable);

      SNode assignNode = current.appendNode(lxCopying, size);
      assignNode.appendNode(lxLocalAddress, member.param);

      SNode fieldExpr = assignNode.appendNode(lxFieldExpression);
      fieldExpr.appendNode(tempType, tempLocal);
      fieldExpr.appendNode(lxField, memberIndex);
   }
   else if (member.kind == okLocal) {
      SNode assignNode = current.appendNode(lxAssigning);
      assignNode.appendNode(lxLocal, member.param);

      SNode fieldExpr = assignNode.appendNode(lxFieldExpression);
      fieldExpr.appendNode(tempType, tempLocal);
      fieldExpr.appendNode(lxField, memberIndex);
   }
   else if (member.kind == okOuter) {
      SNode assignNode = current.appendNode(lxAssigning);
      SNode target = assignNode.appendNode(lxVirtualReference);
      recognizeTerminal(target, member, scope, HINT_NODEBUGINFO);

      SNode fieldExpr = assignNode.appendNode(lxFieldExpression);
      fieldExpr.appendNode(tempType, tempLocal);
      fieldExpr.appendNode(lxField, memberIndex);
   }
   else throw InternalError("Not yet implemented"); // !! temporal
}

void Compiler :: injectIndexBoxing(SNode node, SNode objNode, ExprScope& scope)
{
   ref_t typeRef = node.findChild(lxType).argument;
   int size = node.findChild(lxSize).argument;

   if (typeRef == V_DWORD)
      typeRef = scope.moduleScope->intReference;

   objNode.injectAndReplaceNode(lxSaving, size);

   SNode newNode = objNode.insertNode(lxCreatingStruct, size);
   newNode.appendNode(lxType, typeRef);
}

void Compiler :: injectCopying(SNode& copyingNode, int size, bool variadic, bool primArray)
{
   // copying boxed object
   if (variadic) {
      // NOTE : structure command is used to copy variadic argument list
      copyingNode.injectAndReplaceNode(lxCloning);
   }
   else if (size < 0) {
      // if it is a dynamic srtructure boxing
      copyingNode.injectAndReplaceNode(lxCloning);
   }
   else if (primArray) {
      // if it is a dynamic srtructure boxing
      copyingNode.injectAndReplaceNode(lxCloning);
   }
   else if (size != 0) {
      copyingNode.injectAndReplaceNode(lxCopying, size);
   }
   // otherwise consider it as a byref variable
   else copyingNode.injectAndReplaceNode(lxByRefAssigning);
}

void Compiler :: injectCreating(SNode& assigningNode, SNode objNode, ExprScope& scope, bool insertMode, int size,
   ref_t typeRef, bool variadic)
{
   // NOTE that objNode may be no longer valid, so only cached values are used!

   SNode newNode = insertMode ?
      assigningNode.insertNode(lxCreatingStruct, size) : assigningNode.appendNode(lxCreatingStruct, size);

   if (variadic) {
      int tempSizeLocal = scope.newTempLocalAddress();
      SNode sizeSetNode = assigningNode.prependSibling(lxArgArrOp, LEN_OPERATOR_ID);
      sizeSetNode.appendNode(lxLocalAddress, tempSizeLocal);
      sizeSetNode.appendNode(objNode.type, objNode.argument);

      newNode.set(lxNewArrOp, typeRef);
      newNode.appendNode(lxSize, 0);
      newNode.appendNode(lxLocalAddress, tempSizeLocal);
   }
   else if (!size) {
      // HOTFIX : recognize byref boxing
      newNode.set(lxCreatingClass, 1);
   }
   newNode.appendNode(lxType, typeRef);
}

void Compiler :: boxExpressionInRoot(SNode node, SNode objNode, ExprScope& scope, LexicalType tempType,
   int tempLocal, bool localBoxingMode, bool condBoxing)
{
   SNode parent = node;
   SNode current = injectRootSeqExpression(parent);
   SNode boxingNode = current;
   if (condBoxing)
      boxingNode = current.insertNode(lxCondBoxing);

   ref_t typeRef = node.findChild(lxType).argument;
   int size = node.findChild(lxSize).argument;
   bool isVariable = node.argument == INVALID_REF;
   bool variadic = node == lxArgBoxableExpression;
   bool primArray = node == lxPrimArrBoxableExpression;
   if (typeRef != 0) {
      bool fixedSizeArray = isPrimitiveArrRef(typeRef) && size > 0;

      if (isPrimitiveRef(typeRef)) {
         ref_t elementRef = node.findChild(lxElementType).argument;

         typeRef = resolvePrimitiveReference(scope, typeRef, elementRef, false);
      }

      SNode copyNode = boxingNode.insertNode(objNode.type, objNode.argument);
      if (test(objNode.type, lxOpScopeMask))
         SyntaxTree::copyNode(objNode, copyNode);

      if (size < 0 || primArray) {
         injectCopying(copyNode, size, variadic, primArray);

         copyNode.appendNode(lxType, typeRef);

         copyNode.injectAndReplaceNode(lxAssigning);
         copyNode.insertNode(tempType, tempLocal);
      }
      else {
         SNode assignNode = boxingNode.insertNode(lxAssigning);
         assignNode.appendNode(tempType, tempLocal);

         if (localBoxingMode) {
            copyNode.injectAndReplaceNode(lxCopying, size);

            // inject local boxed object
            ObjectInfo tempBuffer;
            allocateTempStructure(scope, size, fixedSizeArray, tempBuffer);

            assignNode.appendNode(lxLocalAddress, tempBuffer.param);
            copyNode.insertNode(lxLocalAddress, tempBuffer.param);
         }
         else {
            injectCreating(assignNode, objNode, scope, false, size, typeRef, variadic);
            injectCopying(copyNode, size, variadic, primArray);

            copyNode.insertNode(tempType, tempLocal);
         }
      }

      if (isVariable) {
         SNode unboxing = current.appendNode(lxCopying, size);
         if (size < 0 || primArray) {
            unboxing.set(lxCloning, 0);
         }
         else if (condBoxing)
            unboxing.set(lxCondCopying, size);

         SyntaxTree::copyNode(objNode, unboxing.appendNode(objNode.type, objNode.argument));
         if (size == 0 && !primArray) {
            // HOTFIX : if it is byref variable unboxing
            unboxing.set(lxAssigning, 0);

            SNode unboxingByRef = unboxing.appendNode(lxFieldExpression);
            unboxingByRef.appendNode(tempType, tempLocal);
            unboxingByRef.appendNode(lxField);
         }
         else unboxing.appendNode(tempType, tempLocal);
      }

      // replace object with a temp local
      objNode.set(tempType, tempLocal);
   }
   else scope.raiseError(errInvalidBoxing, node);
}

void Compiler :: boxExpressionInPlace(SNode node, SNode objNode, ExprScope& scope,
   bool localBoxingMode, bool condBoxing)
{
   SNode rootNode = node.parentNode();
   while (rootNode == lxExpression)
      rootNode = rootNode.parentNode();

   SNode target;
   if (rootNode == lxAssigning) {
      target = rootNode.findSubNodeMask(lxObjectMask);
   }

   ref_t typeRef = node.findChild(lxType).argument;
   int size = node.findChild(lxSize).argument;
   bool variadic = node == lxArgBoxableExpression;
   bool primArray = node == lxPrimArrBoxableExpression;

   if (typeRef != 0) {
      if (localBoxingMode) {
         bool fixedSizeArray = isPrimitiveArrRef(typeRef) && size > 0;

         SNode assignNode = objNode;
         assignNode.injectAndReplaceNode(lxAssigning);

         // inject local boxed object
         ObjectInfo tempBuffer;
         allocateTempStructure(scope, size, fixedSizeArray, tempBuffer);

         assignNode.insertNode(lxLocalAddress, tempBuffer.param);
         assignNode.set(lxCopying, size);

         assignNode.injectAndReplaceNode(lxSeqExpression);
         assignNode.appendNode(lxLocalAddress, tempBuffer.param);
      }
      else {
         if (isPrimitiveRef(typeRef)) {
            ref_t elementRef = node.findChild(lxElementType).argument;

            typeRef = resolvePrimitiveReference(scope, typeRef, elementRef, false);
         }

         int tempLocal = 0;
         if (target == lxLocal) {
            tempLocal = target.argument;
         }
         else tempLocal = scope.newTempLocal();

         SNode seqNode= objNode;
         seqNode.injectAndReplaceNode(lxSeqExpression);

         SNode boxExpr = seqNode;
         if (condBoxing)
            boxExpr = seqNode.injectNode(lxCondBoxing);

         SNode copyingNode = boxExpr.firstChild();
         injectCopying(copyingNode, size, variadic, primArray);

         if (size < 0 || primArray) {
            copyingNode.appendNode(lxType, typeRef);

            copyingNode.injectAndReplaceNode(lxAssigning);
            copyingNode.insertNode(lxTempLocal, tempLocal);
         }
         else {
            copyingNode.insertNode(lxTempLocal, tempLocal);

            SNode assignNode = boxExpr.insertNode(lxAssigning);
            assignNode.insertNode(lxTempLocal, tempLocal);

            // !!NOTE: objNode is no longer valid, but injectCreating uses only
            //         cached values of a type and an argument
            injectCreating(assignNode, objNode, scope, false, size, typeRef, variadic);
         }

         if (target == lxLocal) {
            // comment out double assigning
            target = lxIdle;
            rootNode = lxExpression;
         }
         else seqNode.appendNode(lxTempLocal, tempLocal);
      }
   }
   else scope.raiseError(errInvalidBoxing, node);
}

bool Compiler :: optimizeEmbeddable(_ModuleScope& scope, SNode& node/*, bool argMode*/)
{
   bool applied = false;

   // verify the path
   SNode callNode = node.parentNode();
   SNode rootNode = callNode.parentNode();
   if (rootNode == lxCopying) {
      applied = _logic->optimizeEmbeddableOp(scope, *this, rootNode);
   }

   if (applied)
      node = lxIdle;

   return applied;
}

bool Compiler :: optimizeEmbeddableCall(_ModuleScope& scope, SNode& node)
{
   SNode rootNode = node.parentNode();

   if (_logic->optimizeEmbeddable(rootNode, scope)) {
      node = lxIdle;

      return true;
   }
   else return false;
}

bool Compiler :: optimizeConstantAssigning(_ModuleScope&, SNode& node)
{
   SNode parent = node.parentNode();
   while (parent == lxExpression)
      parent = parent.parentNode();

   SNode larg = parent.findSubNodeMask(lxObjectMask);

   if (larg == lxLocalAddress && parent.argument == 4) {
      // direct operation with numeric constants
      parent.set(lxIntOp, SET_OPERATOR_ID);

      return true;
   }
   else return false;
}

//inline bool existSubNode(SNode node, SNode target, bool skipFirstOpArg)
//{
//   SNode current = node.firstChild(lxObjectMask);
//
//   if (skipFirstOpArg && test(node.type, lxPrimitiveOpMask))
//      current = current.nextNode(lxObjectMask);
//
//   while (current != lxNone) {
//      if (current.type == target.type && current.argument == target.argument) {
//         return true;
//      }
//      else if (existSubNode(current, target, false))
//         return true;
//
//      current = current.nextNode(lxObjectMask);
//   }
//
//   return false;
//}

inline bool compareNodes(SNode node, SNode target)
{
   return (node.type == target.type && node.argument == target.argument);
}

inline bool existsNode(SNode node, SNode target)
{
   if (compareNodes(node, target))
      return true;

   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      if (existsNode(current, target))
         return true;

      current = current.nextNode(lxObjectMask);
   }

   return false;
}

inline void commentNode(SNode& node)
{
   node = lxIdle;
   SNode parent = node.parentNode();
   while (parent == lxExpression) {
      parent = lxIdle;
      parent = parent.parentNode();
   }

}

bool Compiler :: optimizeOpDoubleAssigning(_ModuleScope&, SNode& node)
{
   bool applied = false;

   // seqNode
   SNode seqNode = node.parentNode();
   SNode seqRet = seqNode.lastChild(lxObjectMask);
   if (seqRet == lxExpression)
      seqRet = seqRet.findSubNodeMask(lxObjectMask);

   // opNode
   SNode opNode = node.nextNode(lxObjectMask);
   SNode larg = opNode.firstChild(lxObjectMask);
   SNode rarg = larg.nextNode(lxObjectMask);

   // target
   SNode targetCopying = seqNode.parentNode();
   while (targetCopying == lxExpression)
      targetCopying = targetCopying.parentNode();
   SNode target = targetCopying.findSubNodeMask(lxObjectMask);
   if (target != lxLocalAddress)
      return false;

   SNode temp = node.firstChild(lxObjectMask);
   SNode tempSrc = temp.nextNode(lxObjectMask);
   if (tempSrc == lxExpression)
      tempSrc = tempSrc.findSubNodeMask(lxObjectMask);

   // validate if the target is not used in roperand
   if (existsNode(rarg, target))
      return false;

   // validate if temp is used in op and is returned in seq
   if (compareNodes(seqRet, temp) && compareNodes(larg, temp)) {
      temp.set(target.type, target.argument);
      seqRet.set(target.type, target.argument);
      larg.set(target.type, target.argument);
      if (compareNodes(tempSrc, target)) {
         node = lxIdle;
      }

      commentNode(target);

      targetCopying = lxExpression;

      applied = true;
   }

   return applied;
}

bool Compiler :: optimizeBranching(_ModuleScope& scope, SNode& node)
{
   return _logic->optimizeBranchingOp(scope, node);
}

bool Compiler :: optimizeConstProperty(_ModuleScope&, SNode& node)
{
   SNode callNode = node.parentNode();

   callNode.set(lxConstantSymbol, node.argument);

   return false;
}

inline void commetNode(SNode& node)
{
   node = lxIdle;
   SNode parent = node.parentNode();
   while (parent == lxExpression) {
      parent = lxIdle;

      parent = parent.parentNode();
   }
}

bool Compiler :: optimizeCallDoubleAssigning(_ModuleScope&, SNode& node)
{
   SNode callNode = node.parentNode();
   SNode seqNode = callNode.parentNode();
   while (seqNode == lxExpression)
      seqNode = seqNode.parentNode();

   if (seqNode != lxSeqExpression)
      return false;

   SNode copyNode = seqNode.parentNode();
   while (copyNode == lxExpression)
      copyNode = copyNode.parentNode();

   SNode src = copyNode.findSubNodeMask(lxObjectMask);
   if (src.type != lxLocalAddress) {
      node = lxIdle;

      return false;
   }

   SNode temp = callNode.lastChild(lxObjectMask);
   if (temp == lxExpression)
      temp = temp.findSubNodeMask(lxObjectMask);
   SNode seqRes = seqNode.lastChild(lxObjectMask);
   if (seqRes == lxExpression)
      seqRes = seqRes.findSubNodeMask(lxObjectMask);

   if (temp == lxLocalAddress && compareNodes(seqRes, temp)) {
      temp.setArgument(src.argument);
      seqRes.setArgument(src.argument);

      commentNode(src);
      copyNode = lxExpression;

      node = lxIdle;

      return true;
   }

   return false;
}

bool Compiler :: optimizeTriePattern(_ModuleScope& scope, SNode& node, int patternId)
{
   switch (patternId) {
      case 1:
         return optimizeConstProperty(scope, node);
      case 2:
         return optimizeEmbeddable(scope, node/*, false*/);
      case 3:
         return optimizeEmbeddableCall(scope, node);
      case 4:
         return optimizeBranching(scope, node);
      case 5:
         return optimizeConstantAssigning(scope, node);
      case 6:
         return optimizeOpDoubleAssigning(scope, node);
      case 7:
         return optimizeCallDoubleAssigning(scope, node);
      default:
         break;
   }

   return false;
}

bool Compiler :: matchTriePatterns(_ModuleScope& scope, SNode& node, SyntaxTrie& trie, List<SyntaxTrieNode>& matchedPatterns)
{
   List<SyntaxTrieNode> nextPatterns;
   if (test(node.type, lxOpScopeMask) || node.type == lxCode) {
      SyntaxTrieNode rootTrieNode(&trie._trie);
      nextPatterns.add(rootTrieNode);
   }

   // match existing patterns
   for (auto it = matchedPatterns.start(); !it.Eof(); it++) {
      auto pattern = *it;
      for (auto child_it = pattern.Children(); !child_it.Eof(); child_it++) {
         auto currentPattern = child_it.Node();
         auto currentPatternValue = currentPattern.Value();
         if (currentPatternValue.match(node)) {
            nextPatterns.add(currentPattern);
            if (currentPatternValue.patternId != 0 && optimizeTriePattern(scope, node, currentPatternValue.patternId))
               return true;
         }
      }
   }

   if (nextPatterns.Count() > 0) {
      SNode current = node.firstChild();
      while (current != lxNone) {
         if (current == lxExpression) {
            SNode subNode = current.findSubNodeMask(lxObjectMask);
            if (matchTriePatterns(scope, subNode, trie, nextPatterns))
               return true;
         }
         else if(matchTriePatterns(scope, current, trie, nextPatterns))
            return true;

         current = current.nextNode();
      }
   }

   return false;
}

void Compiler :: analizeCodePatterns(SNode node, NamespaceScope& scope)
{
   if (test(_optFlag, 1)) {
      //test2(node);

      bool applied = true;
      List<SyntaxTrieNode> matched;
      while (applied) {
         matched.clear();
         SyntaxTrieNode rootTrieNode(&_sourceRules._trie);
         matched.add(rootTrieNode);

         applied = matchTriePatterns(*scope.moduleScope, node, _sourceRules, matched);
      }

      //test2(node);
   }
}

void Compiler :: analizeMethod(SNode node, NamespaceScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxNewFrame) {
         analizeCodePatterns(current, scope);
      }
      current = current.nextNode();
   }
}

void Compiler :: analizeClassTree(SNode node, ClassScope& scope, bool(*cond)(LexicalType))
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   //LexicalType type = scope.classClassMode ? lxConstructor : lxClassMethod;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (cond(current)) {
         analizeMethod(current, *nsScope);

         if (test(_optFlag, 1)) {
            if (test(scope.info.methodHints.get(Attribute(current.argument, maHint)), tpEmbeddable)) {
               defineEmbeddableAttributes(scope, current);
            }
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: analizeSymbolTree(SNode node, Scope& scope)
{
   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   analizeCodePatterns(node, *nsScope);
}

void Compiler :: defineEmbeddableAttributes(ClassScope& classScope, SNode methodNode)
{
   // Optimization : subject'get = self / $self
   if (_logic->recognizeEmbeddableIdle(methodNode, classScope.extensionClassRef != 0)) {
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

//void Compiler :: compileForward(SNode ns, NamespaceScope& scope)
//{
//   ident_t shortcut = ns.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();
//   ident_t reference = ns.findChild(lxForward).firstChild(lxTerminalMask).identifier();
//
//   if (!scope.defineForward(shortcut, reference))
//      scope.raiseError(errDuplicatedDefinition, ns);
//}
//
////////bool Compiler :: validate(_ProjectManager& project, _Module* module, int reference)
//////{
//////   int   mask = reference & mskAnyRef;
//////   ref_t extReference = 0;
//////   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
//////   _Module* extModule = project.resolveModule(refName, extReference, true);
//////
//////   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
//////}
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
   ReferenceNs sectionName("'", PACKAGE_SECTION);
   ref_t reference = module->mapReference(sectionName);
   ref_t vmtReference = mapForwardRef(module, project, SUPER_FORWARD);
   if (vmtReference == 0)
      return;

   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxConstantList, reference);
   writer.appendNode(lxType, vmtReference);

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

int Compiler :: saveMetaInfo(_ModuleScope& scope, ident_t info)
{
   int position = 0;

   ReferenceNs sectionName("'", METAINFO_SECTION);
   _Memory* section = scope.module->mapSection(scope.module->mapReference(sectionName, false) | mskMetaRDataRef, false);
   if (section) {
      MemoryWriter metaWriter(section);

      position = metaWriter.Position();
      if (!position) {
         metaWriter.writeByte(0);
         position++;
      }

      metaWriter.writeLiteral(info);
   }

   return position;
}

void Compiler :: saveNamespaceInfo(SNode node, NamespaceScope& scope, bool innerMost)
{
   if (innerMost) {
      node.appendNode(lxSourcePath, scope.sourcePath);
   }

   IdentifierString nsFullName(scope.module->Name());
   if (scope.name.Length() > 0) {
      nsFullName.append("'");
      nsFullName.append(scope.name.c_str());
   }
   node.appendNode(lxImport, nsFullName.c_str());

   for (auto it = scope.importedNs.start(); !it.Eof(); it++) {
      node.appendNode(lxImport, *it);
   }

   if (scope.parent)
      saveNamespaceInfo(node, *(NamespaceScope*)scope.parent, false);
}

void Compiler :: declareTemplate(SNode node, NamespaceScope& scope)
{
   // COMPILER MAGIC : inject imported namespaces & source path
   saveNamespaceInfo(node, scope, true);

   SNode current = node.firstChild();

   ref_t templateRef = scope.mapNewTerminal(current.findChild(lxNameAttr), Visibility::Public);

   // check for duplicate declaration
   if (scope.module->mapSection(templateRef | mskSyntaxTreeRef, true))
      scope.raiseError(errDuplicatedSymbol, current.findChild(lxNameAttr));

   _Memory* target = scope.module->mapSection(templateRef | mskSyntaxTreeRef, false);

   if (node == lxExtensionTemplate) {
      registerExtensionTemplate(current, scope, templateRef);
   }

   SyntaxTree::saveNode(node, target);

   // HOTFIX : to prevent template double declaration in repeating mode
   node = lxIdle;
}

void Compiler :: compileImplementations(SNode current, NamespaceScope& scope)
{
   //SyntaxTree expressionTree; // expression tree is reused

   // second pass - implementation
   while (current != lxNone) {
      switch (current) {
//         case lxInclude:
//            compileForward(current, scope);
//            break;
         case lxClass:
         {
#ifdef FULL_OUTOUT_INFO
            // info
            ident_t name = scope.module->resolveReference(current.argument);
            scope.moduleScope->printInfo("class %s", name);
#endif // FULL_OUTOUT_INFO

            // compile class
            ClassScope classScope(&scope, current.argument, scope.defaultVisibility);
            declareClassAttributes(current, classScope, false);
            scope.moduleScope->loadClassInfo(classScope.info, current.argument, false);

            compileClassImplementation(current, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference && classScope.info.header.classRef != 0) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef, classScope.visibility);
               scope.moduleScope->loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);
               classClassScope.classClassMode = true;

               compileClassClassImplementation(current, classClassScope, classScope);
            }
            break;
         }
         case lxSymbol:
         {
            SymbolScope symbolScope(&scope, current.argument, scope.defaultVisibility);
            declareSymbolAttributes(current, symbolScope, false, false);

            compileSymbolImplementation(current, symbolScope);
            break;
         }
         case lxNamespace:
         {
            SNode node = current.firstChild();
            NamespaceScope namespaceScope(&scope/*, true*/);
            declareNamespace(node, namespaceScope, false, true);
            copyParentNamespaceExtensions(scope, namespaceScope);

            compileImplementations(node, namespaceScope);
            break;
         }
//         //case lxMeta:
//         //   compileMetaCategory(current, scope);
//         //   break;
      }
      current = current.nextNode();
   }
}

bool Compiler :: compileDeclarations(SNode current, NamespaceScope& scope, bool forced, bool& repeatMode)
{
   if (scope.moduleScope->superReference == 0)
      scope.raiseError(errNotDefinedBaseClass);

   // first pass - declaration
   bool declared = false;
   while (current != lxNone) {
      switch (current) {
         case lxNamespace:
         {
            SNode node = current.firstChild();

            NamespaceScope namespaceScope(&scope);
            declareNamespace(node, namespaceScope, false, false);

            // declare classes several times to ignore the declaration order
            declared |= compileDeclarations(node, namespaceScope, forced, repeatMode);

            break;
         }
         case lxClass:
            if (!scope.moduleScope->isDeclared(current.argument)) {
               if (forced || !current.findChild(lxParent)
                  || _logic->doesClassExist(*scope.moduleScope, resolveParentRef(current.findChild(lxParent), scope, true)))
               {
                  // HOTFIX : import template classes
                  SNode importNode = current.findChild(lxClassImport);
                  while (importNode == lxClassImport) {
                     importClassMembers(current, importNode, scope);

                     importNode = importNode.nextNode();
                  }

                  ClassScope classScope(&scope, current.argument, scope.defaultVisibility);
                  declareClassAttributes(current, classScope, false);

                  // declare class
                  compileClassDeclaration(current, classScope);

                  declared = true;
               }
               else repeatMode = true;
            }

            break;
         case lxSymbol:
            if (!scope.moduleScope->isDeclared(current.argument)) {
               SymbolScope symbolScope(&scope, current.argument, scope.defaultVisibility);
               declareSymbolAttributes(current, symbolScope, true, false);

               // declare symbol
               compileSymbolDeclaration(current, symbolScope);
               declared = true;
            }
            break;
         case lxTemplate:
         case lxExtensionTemplate:
            declareTemplate(current, scope);
            break;
      }
      current = current.nextNode();
   }

   return declared;
}

void Compiler :: copyParentNamespaceExtensions(NamespaceScope& source, NamespaceScope& target)
{
   for (auto it = source.extensions.start(); !it.Eof(); it++) {
      auto ext = *it;

      target.extensions.add(it.key(), Pair<ref_t, ref_t>(ext.value1, ext.value2));
   }
}

void Compiler :: declareNamespace(SNode& current, NamespaceScope& scope, bool ignoreImports, bool withFullInfo)
{
   if (!ignoreImports && !scope.module->Name().compare(STANDARD_MODULE) && _autoSystemImport) {
      bool dummy = false;
      scope.moduleScope->includeNamespace(scope.importedNs, STANDARD_MODULE, dummy);
   }

   while (current != lxNone) {
      if (current == lxSourcePath) {
         scope.sourcePath.copy(current.identifier());
      }
      else if (current == lxNameAttr) {
         // overwrite the parent namespace name
         scope.name.copy(current.firstChild(lxTerminalMask).identifier());

         if (scope.nsName.Length() > 0)
            scope.nsName.append('\'');

         scope.nsName.append(scope.name.c_str());
         scope.ns = scope.nsName.c_str();
      }
      else if (current == lxAttribute) {
         if (!_logic->validateNsAttribute(current.argument, scope.defaultVisibility))
            scope.raiseError(errInvalidHint, current);
      }
      else if (current == lxImport && !ignoreImports) {
         bool duplicateInclusion = false;
         if (scope.moduleScope->includeNamespace(scope.importedNs, current.identifier(), duplicateInclusion)) {
            //if (duplicateExtensions)
            //   scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, ns);
         }
         else if (duplicateInclusion) {
            if (current.identifier().compare(STANDARD_MODULE) && _autoSystemImport) {
               // HOTFIX : ignore the auto loaded module
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateInclude, current);

            // HOTFIX : comment out, to prevent duplicate warnings
            current = lxIdle;
         }
         else {
            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, current);
            current = lxIdle; // remove the node, to prevent duplicate warnings
         }
      }
      else if (current == lxIdle) {
         // skip idle node
      }
      else break;

      current = current.nextNode();
   }

   if (withFullInfo) {
      // HOTFIX : load the module internal and public extensions
      scope.loadExtensions(scope.module->Name(), scope.nsName.c_str(), false);
      scope.loadExtensions(scope.module->Name(), scope.nsName.c_str(), true);

      for (auto it = scope.importedNs.start(); !it.Eof(); it++) {
         ident_t imported_ns = *it;

         scope.loadModuleInfo(imported_ns);
      }
   }
}

void Compiler :: declareMembers(SNode current, NamespaceScope& scope)
{
   while (current != lxNone) {
      switch (current) {
         case lxNamespace:
         {
            SNode node = current.firstChild();

            NamespaceScope namespaceScope(&scope);
            declareNamespace(node, namespaceScope, true, false);
            scope.moduleScope->declareNamespace(namespaceScope.ns.c_str());

            declareMembers(node, namespaceScope);
            break;
         }
         case lxClass:
         {
            ClassScope classScope(&scope, scope.defaultVisibility);
            declareClassAttributes(current, classScope, true);

            if (current.argument == INVALID_REF) {
               // if it is a template based class - its name was already resolved
               classScope.reference = current.findChild(lxNameAttr).argument;
            }
            else classScope.reference = scope.mapNewTerminal(current.findChild(lxNameAttr), classScope.visibility);
            current.setArgument(classScope.reference);

            // NOTE : the symbol section is created even if the class symbol doesn't exist
            scope.moduleScope->mapSection(classScope.reference | mskSymbolRef, false);

            break;
         }
         case lxSymbol:
         {
            SymbolScope symbolScope(&scope, scope.defaultVisibility);
            declareSymbolAttributes(current, symbolScope, true, true);

            symbolScope.reference = scope.mapNewTerminal(current.findChild(lxNameAttr), symbolScope.visibility);
            current.setArgument(symbolScope.reference);

            scope.moduleScope->mapSection(symbolScope.reference | mskSymbolRef, false);

            break;
         }
      }
      current = current.nextNode();
   }
}

void Compiler :: declareModuleIdentifiers(SyntaxTree& syntaxTree, _ModuleScope& scope)
{
   SNode node = syntaxTree.readRoot().firstChild();
   while (node != lxNone) {
      if (node == lxNamespace) {
         SNode current = node.firstChild();

         NamespaceScope namespaceScope(&scope, nullptr);
         declareNamespace(current, namespaceScope, true, false);

         scope.declareNamespace(namespaceScope.ns.c_str());

         // declare all module members - map symbol identifiers
         declareMembers(current, namespaceScope);
      }

      node = node.nextNode();
   }
}

bool Compiler :: declareModule(SyntaxTree& syntaxTree, _ModuleScope& scope, bool forced, bool& repeatMode,
   ExtensionMap* outerExtensionList)
{
   SNode node = syntaxTree.readRoot().firstChild();
   bool retVal = false;
   while (node != lxNone) {
      if (node == lxNamespace) {
         SNode current = node.firstChild();

         NamespaceScope namespaceScope(&scope, outerExtensionList);
         declareNamespace(current, namespaceScope, false, false);

         // compile class declarations several times to ignore the declaration order
         retVal |= compileDeclarations(current, namespaceScope, forced, repeatMode);
      }

      node = node.nextNode();
   }

   return retVal;
}

void Compiler :: compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope, ident_t greeting,
   ExtensionMap* outerExtensionList)
{
   SNode node = syntaxTree.readRoot().firstChild();
   while (node != lxNone) {
      if (node == lxNamespace) {
         SNode current = node.firstChild();

         NamespaceScope namespaceScope(&scope, outerExtensionList);
         declareNamespace(current, namespaceScope, false, true);

         if (!emptystr(greeting))
            scope.project->printInfo("%s", greeting);

         compileImplementations(current, namespaceScope);
      }
      node = node.nextNode();
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

bool Compiler :: loadAttributes(_ModuleScope& scope, ident_t name, MessageMap* attributes, bool silenMode)
{
   _Module* extModule = scope.project->loadModule(name, silenMode);
   if (extModule) {
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
               scope.printInfo(wrnDuplicateAttribute, attrName);
         }
      }

      return true;
   }
   else return false;
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
   scope.longReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(LONG_FORWARD));
   scope.realReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(REAL_FORWARD));
   scope.literalReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(STR_FORWARD));
   scope.wideReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(WIDESTR_FORWARD));
   scope.charReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(CHAR_FORWARD));
   scope.messageReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(MESSAGE_FORWARD));
   scope.refTemplateReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(REFTEMPLATE_FORWARD));
   scope.arrayTemplateReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(ARRAYTEMPLATE_FORWARD));
   scope.argArrayTemplateReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(ARGARRAYTEMPLATE_FORWARD));
   scope.messageNameReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(MESSAGENAME_FORWARD));
   scope.extMessageReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(EXT_MESSAGE_FORWARD));
   scope.lazyExprReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(LAZYEXPR_FORWARD));
   scope.closureTemplateReference = safeMapWeakReference(scope.module, scope.project->resolveForward(CLOSURETEMPLATE_FORWARD));
//   scope.wrapReference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(WRAP_FORWARD));

   scope.branchingInfo.reference = safeMapReference(scope.module, scope.project, scope.project->resolveForward(BOOL_FORWARD));
   scope.branchingInfo.trueRef = safeMapReference(scope.module, scope.project, scope.project->resolveForward(TRUE_FORWARD));
   scope.branchingInfo.falseRef = safeMapReference(scope.module, scope.project, scope.project->resolveForward(FALSE_FORWARD));

   // cache the frequently used messages
   scope.dispatch_message = encodeMessage(scope.module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
   scope.init_message = encodeMessage(scope.module->mapAction(INIT_MESSAGE, 0, false), 0, FUNCTION_MESSAGE | STATIC_MESSAGE);
   scope.constructor_message = encodeMessage(scope.module->mapAction(CONSTRUCTOR_MESSAGE, 0, false), 1, 0);
   scope.protected_constructor_message = encodeMessage(scope.module->mapAction(CONSTRUCTOR_MESSAGE2, 0, false), 1, 0);

   if (!scope.module->Name().compare(STANDARD_MODULE)) {
      // system attributes should be loaded automatically
      if (!loadAttributes(scope, STANDARD_MODULE, &scope.attributes, true))
         scope.printInfo(wrnInvalidModule, STANDARD_MODULE);
   }

   createPackageInfo(scope.module, *scope.project);
}

void Compiler :: injectVirtualField(SNode classNode, LexicalType sourceType, ref_t sourceArg, int postfixIndex)
{
   // declare field
   IdentifierString fieldName(VIRTUAL_FIELD);
   fieldName.appendInt(postfixIndex);

   SNode fieldNode = classNode.appendNode(lxClassField, INVALID_REF);
   fieldNode.appendNode(lxNameAttr).appendNode(lxIdentifier, fieldName.c_str());

   // assing field
   SNode assignNode = classNode.appendNode(lxFieldInit, INVALID_REF); // INVALID_REF indicates the virtual code
   assignNode.appendNode(lxIdentifier, fieldName.c_str());
   assignNode.appendNode(lxAssign);
   assignNode.appendNode(sourceType, sourceArg);
}

////void Compiler :: injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef)
////{
////   // injecting auto-generated static sealed constant field, (argument=INVALID_REF)
////   SNode fieldNode = classNode.appendNode(lxClassField, INVALID_REF);
////   fieldNode.appendNode(lxAttribute, V_STATIC);
////   fieldNode.appendNode(lxAttribute, V_SEALED);
////   fieldNode.appendNode(lxAttribute, V_CONST);
////
////   fieldNode.appendNode(lxIdentifier, fieldName);
////   if (fieldRef) {
////      ident_t referenceName = scope.module->resolveReference(fieldRef);
////      if (isWeakReference(referenceName)) {
////         IdentifierString fullName(scope.module->Name(), referenceName);
////
////         fieldNode.appendNode(lxClassRefAttr, fullName.c_str());
////      }
////      else fieldNode.appendNode(lxClassRefAttr, referenceName);
////   }
////}
////
////void Compiler :: generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef)
////{
////   _Memory* section = scope.module->mapSection(enumRef | mskRDataRef, true);
////   if (!section) {
////      // if the member list is not available - create and assign to the static field
////      section = scope.module->mapSection(enumRef | mskRDataRef, false);
////
////      SyntaxTree expressionTree;
////      SyntaxWriter writer(expressionTree);
////
////      writer.newNode(lxExpression);
////      writer.newNode(lxAssigning);
////      writer.appendNode(lxStaticField, enumRef);
////      writer.appendNode(lxConstantList, enumRef | mskConstArray);
////      writer.closeNode();
////      writer.closeNode();
////
////      section->addReference(scope.arrayReference | mskVMTRef, (pos_t)-4);
////
////      compilePreloadedCode(scope, expressionTree.readRoot());
////   }
////
////   MemoryWriter metaWriter(section);
////
////   metaWriter.writeRef(memberRef | mskConstantRef, 0);
////}
////
//////void Compiler :: generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument)
//////{
//////   MemoryWriter writer(scope.module->mapSection(listRef | mskRDataRef, false));
//////
//////   _writer.generateConstantMember(writer, type, argument);
//////}

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

////ref_t Compiler :: readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader)
////{
////   ref_t memberRef = reader.getDWord() & ~mskAnyRef;
////
////   return importReference(extModule, memberRef, scope.module);
////}

void Compiler :: injectBoxingExpr(SNode& node, bool variable, int size, ref_t targetClassRef, bool arrayMode)
{
   node.injectAndReplaceNode(lxBoxableExpression, variable ? INVALID_REF : 0);

   if (arrayMode && size == 0) {
      // HOTFIX : to indicate a primitive array boxing
      node = lxPrimArrBoxableExpression;
   }

   node.appendNode(lxType, targetClassRef);
   node.appendNode(lxSize, size);

//   writer.inject(boxingType, argument);
//   writer.appendNode(lxTarget, targetClassRef);
//   writer.closeNode();
}

void Compiler :: injectConverting(SNode& node, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg, ref_t targetClassRef, int, bool embeddableAttr)
{
   if (node == lxExpression) {
   }
   else node.injectAndReplaceNode(lxExpression);

   node.appendNode(lxCallTarget, targetClassRef);

   if (embeddableAttr)
      node.appendNode(lxEmbeddableAttr);

   node.set(convertOp, convertArg);
   node.insertNode(targetOp, targetArg/*, lxTarget, targetClassRef*/);

   //analizeOperands(node, stackSafeAttr);
}

//void Compiler :: injectEmbeddableRet(SNode assignNode, SNode callNode, ref_t messageRef)
//{
//   // move assigning target into the call node
//   SNode assignTarget;
//   if (assignNode == lxByRefAssigning) {
//      assignTarget = assignNode.findSubNode(lxLocal);
//   }
//   else assignTarget = assignNode.findSubNode(lxLocalAddress);
//
//   if (assignTarget != lxNone) {
//      // removing assinging operation
//      assignNode = lxExpression;
//
//      callNode.appendNode(assignTarget.type, assignTarget.argument);
//      assignTarget = lxIdle;
//      callNode.setArgument(messageRef);
//   }
//}

void Compiler :: injectEmbeddableOp(_ModuleScope& scope, SNode assignNode, SNode callNode, ref_t subject, int paramCount/*, int verb*/)
{
   SNode assignTarget = assignNode.findSubNode(lxLocalAddress);
   if (assignTarget == lxNone)
      //HOTFIX : embeddable constructor should be applied only for the stack-allocated varaible
      return;

   if (paramCount == -1/* && verb == 0*/) {
      // if it is an embeddable constructor call
      SNode sourceNode = assignNode.findSubNodeMask(lxObjectMask);

      SNode callTargetNode = callNode.firstChild(lxObjectMask);
      callTargetNode.set(sourceNode.type, sourceNode.argument);

      callNode.setArgument(subject);

      // HOTFIX : class class reference should be turned into class one
      SNode callTarget = callNode.findChild(lxCallTarget);

      IdentifierString className(scope.module->resolveReference(callTarget.argument));
      className.cut(getlength(className) - getlength(CLASSCLASS_POSTFIX), getlength(CLASSCLASS_POSTFIX));

      callTarget.setArgument(scope.mapFullReference(className));

      assignNode = lxExpression;
      sourceNode = lxIdle;

      // check if inline initializer is declared
      ClassInfo targetInfo;
      scope.loadClassInfo(targetInfo, callTarget.argument);
      if (targetInfo.methods.exist(scope.init_message)) {
         // inject inline initializer call
         SNode initNode = callTargetNode.appendNode(lxDirectCalling, scope.init_message);
         initNode.appendNode(lxCallTarget, callTarget.argument);
      }
   }
   //else {
   //   // removing assinging operation
   //   assignNode = lxExpression;

   //   // move assigning target into the call node

   //   if (assignTarget != lxNone) {
   //      callNode.appendNode(assignTarget.type, assignTarget.argument);
   //      assignTarget = lxIdle;
   //      callNode.setArgument(encodeMessage(subject, paramCount));
   //   }
   //}
}

SNode Compiler :: injectTempLocal(SNode node, int size, bool boxingMode)
{
   SNode tempLocalNode;

   //HOTFIX : using size variable copy to prevent aligning
   int dummy = size;
   int offset = allocateStructure(node, dummy);

   if (boxingMode) {
      // allocate place for the local copy
      node.injectNode(node.type, node.argument);

      node.set(lxCopying, size);

      //node.insertNode(lxTempAttr, 0); // NOTE _ should be the last child

      tempLocalNode = node.insertNode(lxLocalAddress, offset);
   }
   else {
      tempLocalNode = node.appendNode(lxLocalAddress, offset);
   }

   return tempLocalNode;
}

void Compiler :: injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t embeddedMessageRef)
{
   SNode methNode = classNode.appendNode(lxConstructor, message);
   methNode.appendNode(lxEmbeddableMssg, embeddedMessageRef);
   methNode.appendNode(lxAttribute, tpEmbeddable);
   methNode.appendNode(lxAttribute, tpConstructor);

   SNode codeNode = methNode.appendNode(lxDispatchCode);
   codeNode.appendNode(lxRedirect, embeddedMessageRef);
}

void Compiler :: injectVirtualMultimethod(_ModuleScope&, SNode classNode, ref_t message, LexicalType methodType,
   ref_t resendMessage, bool privateOne)
{
   SNode methNode = classNode.appendNode(methodType, message);
   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   methNode.appendNode(lxAutoMultimethod); // !! HOTFIX : add a attribute for the nested class compilation (see compileNestedVMT)
   methNode.appendNode(lxAttribute, tpMultimethod);
   if (methodType == lxConstructor)
      methNode.appendNode(lxAttribute, tpConstructor);

   if (test(message, FUNCTION_MESSAGE))
      methNode.appendNode(lxAttribute, tpFunction);

   if (privateOne)
      methNode.appendNode(lxAttribute, tpPrivate);

   methNode.appendNode(lxResendExpression, resendMessage);
}

void Compiler :: injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType)
{
   ref_t resendMessage = message;
   ref_t actionRef, flags;
   int argCount;
   decodeMessage(message, actionRef, argCount, flags);

   ref_t dummy = 0;
   ident_t actionName = scope.module->resolveAction(actionRef, dummy);

   ref_t signatureLen = 0;
   ref_t signatures[ARG_COUNT];

   int firstArg = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
   if (test(message, VARIADIC_MESSAGE)) {
   }
   else {
      for (int i = firstArg; i < argCount; i++) {
         signatures[signatureLen++] = scope.superReference;
      }
   }
   ref_t signRef = scope.module->mapAction(actionName, scope.module->mapSignature(signatures, signatureLen, false), false);

   resendMessage = encodeMessage(signRef, argCount, flags);

   bool privateOne = false;
   if (test(flags, STATIC_MESSAGE)) {
      privateOne = true;
   }

   injectVirtualMultimethod(scope, classNode, message, methodType, resendMessage, privateOne);
}

void Compiler :: injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument)
{
   SyntaxTree subTree;
   SyntaxWriter subWriter(subTree);
   subWriter.newNode(lxRoot);
   subWriter.newNode(lxClassMethod, message);
   subWriter.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   subWriter.appendNode(lxAttribute, V_EMBEDDABLE);
   subWriter.newNode(lxDispatchCode);
   subWriter.appendNode(type, argument);
   subWriter.closeNode();
   subWriter.closeNode();
   subWriter.closeNode();

   SyntaxTree::copyNode(subTree.readRoot(), classNode);
}

void Compiler :: injectVirtualReturningMethod(_ModuleScope&, SNode classNode, ref_t message, ident_t variable, ref_t outputRef)
{
   SNode methNode = classNode.appendNode(lxClassMethod, message);
   methNode.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   methNode.appendNode(lxAttribute, tpEmbeddable);
   methNode.appendNode(lxAttribute, tpSealed);
   methNode.appendNode(lxAttribute, tpCast);

   if (outputRef) {
      methNode.appendNode(lxType, outputRef);
   }

   SNode exprNode = methNode.appendNode(lxReturning).appendNode(lxExpression);
   exprNode.appendNode(lxAttribute, V_NODEBUGINFO);
   exprNode.appendNode(lxIdentifier, variable);
}

//void Compiler :: injectDirectMethodCall(SyntaxWriter& writer, ref_t targetRef, ref_t message)
//{
//   writer.appendNode(lxCallTarget, targetRef);
//
//   writer.insert(lxDirectCalling, message);
//   writer.closeNode();
//
//}

void Compiler :: injectDefaultConstructor(_ModuleScope& scope, SNode classNode, ref_t, bool protectedOne)
{
   ref_t message = protectedOne ? scope.protected_constructor_message : scope.constructor_message;
   SNode methNode = classNode.appendNode(lxConstructor, message);
   methNode.appendNode(lxAutogenerated);
   methNode.appendNode(lxAttribute, tpConstructor);
   if (protectedOne)
      methNode.appendNode(lxAttribute, tpProtected);
}

void Compiler :: generateClassSymbol(SyntaxWriter& writer, ClassScope& scope)
{
   ExprScope exprScope(&scope);

   writer.newNode(lxSymbol, scope.reference);
   writer.newNode(lxAutogenerated);
   SNode current = writer.CurrentNode();
   recognizeTerminal(current, ObjectInfo(okClass, scope.reference/*, scope.info.header.classRef*/), exprScope, HINT_NODEBUGINFO);
   writer.closeNode();
   writer.closeNode();
}

//////void Compiler :: generateSymbolWithInitialization(SyntaxWriter& writer, ClassScope& scope, ref_t implicitConstructor)
//////{
//////   CodeScope codeScope(&scope);
//////
//////   writer.newNode(lxSymbol, scope.reference);
//////   writeTerminal(writer, SNode(), codeScope, ObjectInfo(okConstantClass, scope.reference, scope.info.header.classRef), HINT_NODEBUGINFO);
//////   writer.newNode(lxImplicitCall, implicitConstructor);
//////   writer.appendNode(lxTarget, scope.reference);
//////   writer.closeNode();
//////   writer.closeNode();
//////}

void Compiler :: registerTemplateSignature(SNode node, NamespaceScope& scope, IdentifierString& signature)
{
   signature.append(TEMPLATE_PREFIX_NS);

   int signIndex = signature.Length();

   IdentifierString templateName(node.firstChild(lxTerminalMask).identifier());
   int paramCounter = SyntaxTree::countChild(node, lxType, lxTemplateParam);

   templateName.append('#');
   templateName.appendInt(paramCounter);

   ref_t ref = /*mapTemplateAttribute(node, scope)*/scope.resolveImplicitIdentifier(templateName.c_str(), false, true);

   ident_t refName = scope.module->resolveReference(ref);
   if (isWeakReference(refName))
      signature.append(scope.module->Name());

   signature.append(refName);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxTemplateParam) {
         signature.append('&');
         signature.append('{');
         signature.appendInt(current.argument);
         signature.append('}');
      }
      else if (current.compare(lxType, lxArrayType)) {
         signature.append('&');

         ref_t classRef = resolveTypeAttribute(current, scope, true, false);
         if (!classRef)
            scope.raiseError(errUnknownClass, current);

         ident_t className = scope.module->resolveReference(classRef);
         if (isWeakReference(className))
            signature.append(scope.module->Name());

         signature.append(className);
      }
      //else scope.raiseError(errNotApplicable, current);
      //}
      else if (current == lxIdentifier) {
         // !! ignore identifier
      }
      else scope.raiseError(errNotApplicable, current);

      current = current.nextNode();
   }

   // '$auto'system@Func#2&CharValue&Object
   signature.replaceAll('\'', '@', signIndex);
}

void Compiler :: registerExtensionTemplateMethod(SNode node, NamespaceScope& scope, ref_t extensionRef)
{
   IdentifierString messageName;
   int argCount = 1;
   ref_t flags = 0;
   IdentifierString signaturePattern;
   ident_t extensionName = scope.module->resolveReference(extensionRef);
   if (isWeakReference(extensionName)) {
      signaturePattern.append(scope.module->Name());
   }
   signaturePattern.append(extensionName);
   signaturePattern.append('.');

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxNameAttr) {
         messageName.copy(current.firstChild(lxTerminalMask).identifier());
      }
      else if (current == lxMethodParameter) {
         argCount++;
         signaturePattern.append('/');
         SNode typeAttr = current.findChild(lxType, lxArrayType, lxTemplateParam);
         if (typeAttr == lxTemplateParam) {
            signaturePattern.append('{');
            signaturePattern.appendInt(typeAttr.argument);
            signaturePattern.append('}');
         }
         else if (typeAttr != lxNone) {
            if (typeAttr == lxType && typeAttr.argument == V_TEMPLATE) {
               registerTemplateSignature(typeAttr, scope, signaturePattern);
            }
            else {
               ref_t classRef = resolveTypeAttribute(typeAttr, scope, true, false);
               ident_t className = scope.module->resolveReference(classRef);
               if (isWeakReference(className))
                  signaturePattern.append(scope.module->Name());

               signaturePattern.append(className);
            }
         }
         else scope.raiseError(errNotApplicable, current);
      }
      current = current.nextNode();
   }

   ref_t messageRef = encodeMessage(scope.module->mapAction(messageName.c_str(), 0, false), argCount, flags);

   scope.saveExtensionTemplate(messageRef, signaturePattern.ident());
}

void Compiler :: registerExtensionTemplate(SNode node, NamespaceScope& scope, ref_t extensionRef)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         registerExtensionTemplateMethod(current, scope, extensionRef);
      }
      current = current.nextNode();
   }
}

//void Compiler :: registerExtensionTemplate(SyntaxTree& tree, _ModuleScope& scope, ident_t ns, ref_t extensionRef)
//{
//   SNode node = tree.readRoot();
//
//   // declare classes several times to ignore the declaration order
//   NamespaceScope namespaceScope(&scope, ns);
//   declareNamespace(node, namespaceScope, false);
//
//   registerExtensionTemplate(node.findChild(lxClass), namespaceScope, extensionRef);
//}

ref_t Compiler :: generateExtensionTemplate(_ModuleScope& moduleScope, ref_t templateRef, size_t argumentLen,
   ref_t* arguments, ident_t ns, ExtensionMap* outerExtensionList)
{
   List<SNode> parameters;

   // generate an extension if matched
   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxWriter dummyWriter(dummyTree);
   dummyWriter.newNode(lxRoot);
   for (size_t i = 0; i < argumentLen; i++) {
      dummyWriter.appendNode(lxType, arguments[i]);
   }
   dummyWriter.closeNode();

   SNode current = dummyTree.readRoot().firstChild();
   while (current != lxNone) {
      parameters.add(current);

      current = current.nextNode();
   }

   return moduleScope.generateTemplate(templateRef, parameters, ns, false, outerExtensionList);
}

void Compiler :: injectExprOperation(_CompileScope& scope, SNode& node, int size, int tempLocal, LexicalType op,
   int opArg, ref_t reference)
{
   SNode current = node;
   if (current != lxExpression) {
      current.injectNode(lxExpression);
      current = current.findChild(lxExpression);
   }

   SNode loperand = current.firstChild(lxObjectMask);
   loperand.injectAndReplaceNode(lxCopying, size);
   loperand.insertNode(lxLocalAddress, tempLocal);

   SNode roperand = loperand.nextNode(lxObjectMask);
   roperand.injectAndReplaceNode(op, opArg);
   roperand.insertNode(lxLocalAddress, tempLocal);

   current = lxSeqExpression;
   SNode retNode = current.appendNode(lxVirtualReference);

   setVariableTerminal(retNode, scope, ObjectInfo(okLocalAddress, tempLocal, reference), EAttr::eaNone, lxLocalAddress);
}
