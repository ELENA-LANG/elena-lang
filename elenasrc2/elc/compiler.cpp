//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

//#define FULL_OUTOUT_INFO 1

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiler.h"
#include "errors.h"
#include <errno.h>

using namespace _ELENA_;

//inline void test2(SNode node)
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

////constexpr auto HINT_CLOSURE_MASK    = EAttr::eaClosureMask;
constexpr auto HINT_SCOPE_MASK      = EAttr::eaScopeMask;
//constexpr auto HINT_OBJECT_MASK     = EAttr::eaObjectMask;
constexpr auto HINT_MODULESCOPE     = EAttr::eaModuleScope;
constexpr auto HINT_PREVSCOPE       = EAttr::eaPreviousScope;
constexpr auto HINT_NEWOP           = EAttr::eaNewOp;
constexpr auto HINT_CASTOP          = EAttr::eaCast;
constexpr auto HINT_SILENT          = EAttr::eaSilent;
constexpr auto HINT_ROOTSYMBOL      = EAttr::eaRootSymbol;
constexpr auto HINT_ROOT            = EAttr::eaRoot;
constexpr auto HINT_ROOTEXPR        = EAttr::eaRootExpr;
constexpr auto HINT_INLINE_EXPR     = EAttr::eaInlineExpr;
constexpr auto HINT_NOPRIMITIVES    = EAttr::eaNoPrimitives;
constexpr auto HINT_DYNAMIC_OBJECT  = EAttr::eaDynamicObject;  // indicates that the structure MUST be boxed
//constexpr auto HINT_NOBOXING        = EAttr::eaNoBoxing;
//constexpr auto HINT_NOUNBOXING      = EAttr::eaNoUnboxing;
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
//constexpr auto HINT_YIELD_EXPR      = EAttr::eaYieldExpr;
constexpr auto HINT_MESSAGEREF      = EAttr::eaMssg;
//constexpr auto HINT_VIRTUALEXPR     = EAttr::eaVirtualExpr;
constexpr auto HINT_SUBJECTREF      = EAttr::eaSubj;
constexpr auto HINT_DIRECTCALL      = EAttr::eaDirectCall;
constexpr auto HINT_PARAMETER       = EAttr::eaParameter;
constexpr auto HINT_LAZY_EXPR       = EAttr::eaLazy;
//constexpr auto HINT_INLINEARGMODE   = EAttr::eaInlineArg;  // indicates that the argument list should be unboxed
constexpr auto HINT_CONSTEXPR       = EAttr::eaConstExpr;
//constexpr auto HINT_CALLOP          = EAttr::eaCallOp;
constexpr auto HINT_REFEXPR         = EAttr::eaRefExpr;
constexpr auto HINT_CONVERSIONOP    = EAttr::eaConversionOp;
constexpr auto HINT_TARGET          = EAttr::eaTarget;
constexpr auto HINT_ASSIGNTARGET    = EAttr::eaAssignTarget;
constexpr auto HINT_TYPEOF          = EAttr::eaTypeOfOp;

// scope modes
constexpr auto INITIALIZER_SCOPE    = EAttr::eaInitializerScope;   // indicates the constructor or initializer method

typedef Compiler::ObjectInfo                 ObjectInfo;       // to simplify code, ommiting compiler qualifier
typedef Compiler::ArgumentsInfo              ArgumentsInfo;
typedef ClassInfo::Attribute                 Attribute;
////typedef _CompilerLogic::ExpressionAttributes ExpressionAttributes;
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
   extensionDispatchers(INVALID_REF), extensionTargets(INVALID_REF), extensionTemplates(NULL, freestr),
   declaredExtensions(Pair<ref_t, ref_t>(0, 0))
{
   // by default - private visibility
   defaultVisibility = Visibility::Private;

   this->outerExtensionList = outerExtensionList;
}

Compiler::NamespaceScope :: NamespaceScope(NamespaceScope* parent)
   : Scope(parent), constantHints(INVALID_REF), extensions(Pair<ref_t, ref_t>(0, 0)), importedNs(NULL, freestr),
   extensionDispatchers(INVALID_REF), extensionTargets(INVALID_REF), extensionTemplates(NULL, freestr),
   declaredExtensions(Pair<ref_t, ref_t>(0, 0))
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
         if (isWeakReference(identifier)) {
            return mapWeakReference(identifier, false);
         }
         else return mapGlobal(identifier);
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

inline ref_t mapIntConstant(_CompileScope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendHex(integer);

   return scope.moduleScope->module->mapConstant((const char*)s);
}

ObjectInfo Compiler::NamespaceScope :: defineObjectInfo(ref_t reference, bool checkState)
{
   // if reference is zero the symbol is unknown
   if (reference == 0) {
      return ObjectInfo();
   }
   // check if symbol should be treated like constant one
   else if (constantHints.exist(reference)) {
      ref_t classRef = constantHints.get(reference);
      if (classRef == V_INT32 && intConstants.exist(reference)) {
         int value = intConstants.get(reference);

         return ObjectInfo(okIntConstant, ::mapIntConstant(*this, value), V_INT32, 0, value);
      }
      else return ObjectInfo(okConstantSymbol, reference, classRef);
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
               if (outputRef == moduleScope->intReference && intConstants.exist(reference)) {
                  int value = intConstants.get(reference);

                  return ObjectInfo(okIntConstant, ::mapIntConstant(*this, value), V_INT32, 0, value);
               }
               else return ObjectInfo(okConstantSymbol, reference, outputRef);
            }
            else if (symbolInfo.type == SymbolExpressionInfo::Type::ArrayConst) {
               return ObjectInfo(okArrayConst, outputRef, symbolInfo.typeRef);
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

//void Compiler::ModuleScope :: validateReference(SNode terminal, ref_t reference)
//{
//   // check if the reference may be resolved
//   bool found = false;
//
//   if (warnOnWeakUnresolved || !isWeakReference(terminal.identifier())) {
//      int   mask = reference & mskAnyRef;
//      reference &= ~mskAnyRef;
//
//      ref_t    ref = 0;
//      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);
//
//      if (refModule != NULL) {
//         found = (refModule->mapSection(ref | mask, true)!=NULL);
//      }
//      if (!found) {
//         if (!refModule || refModule == module) {
//            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module,
//               terminal.findChild(lxRow).argument,
//               terminal.findChild(lxCol).argument));
//         }
//         else raiseWarning(WARNING_LEVEL_1, wrnUnresovableLink, terminal);
//      }
//   }
//}

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
         mssg_t message = metaReader.getDWord();
         mssg_t strongMessage = metaReader.getDWord();

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

void Compiler::NamespaceScope :: saveExtension(mssg_t message, ref_t extRef, mssg_t strongMessage, bool internalOne)
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

      declaredExtensions.add(message, extInfo);
   }

   extensions.add(message, extInfo);
}

void Compiler::NamespaceScope :: saveExtensionTemplate(mssg_t message, ident_t pattern)
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
   this->hints = 0;
   this->outputRef = INVALID_REF; // to indicate lazy load
   this->withOpenArg = false;
   this->classStacksafe = false;
   this->generic = false;
   this->extensionMode = false;
   this->multiMethod = false;
   this->functionMode = false;
   this->nestedMode = parent->getScope(Scope::ScopeLevel::slOwnerClass) != parent;
////   this->subCodeMode = false;
   this->abstractMethod = false;
   this->mixinFunction = false;
   this->embeddableRetMode = false;
   this->targetSelfMode = false;
//   this->yieldMethod = false;
   this->constMode = false;
}

ObjectInfo Compiler::MethodScope :: mapSelf()
{
   if (extensionMode) {
      //COMPILER MAGIC : if it is an extension ; replace self with this self
      ClassScope* extensionScope = (ClassScope*)getScope(ScopeLevel::slClass);

      return ObjectInfo(okLocal, (ref_t)-1, extensionScope->extensionClassRef, 0, extensionScope->stackSafe ? -1 : 0);
   }
   else if (classStacksafe) {
      return ObjectInfo(okSelfParam, 1, getClassRef(false), 0, (ref_t)-1);
   }
   else return ObjectInfo(okSelfParam, 1, getClassRef(false));
}

ObjectInfo Compiler::MethodScope :: mapGroup()
{
   return ObjectInfo(okParam, (ref_t)-1);
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
   else if (param.class_ref == V_WRAPPER && !EAttrs::testany(mode, HINT_ASSIGNTARGET | HINT_REFEXPR)) {
      return ObjectInfo(okParamField, prefix - param.offset, param.element_ref, 0, 0);
   }
   else return ObjectInfo(okParam, prefix - param.offset, param.class_ref, param.element_ref, 0);
}

ObjectInfo Compiler::MethodScope :: mapTerminal(ident_t terminal, bool referenceOne, EAttr mode)
{
   if (!referenceOne/* && !EAttrs::test(mode, HINT_MODULESCOPE)*/) {
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

//// --- Compiler::YieldMethodScope ---
//
//Compiler::YieldScope :: YieldScope(MethodScope* parent)
//   : Scope(parent)
//{
//}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = this->reserved1 = 0;
   this->allocated2 = this->reserved2 = 0;
   this->withRetStatement = this->genericMethod = false;
}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = this->reserved1 = 0;
   this->allocated2 = this->reserved2 = 0;
   this->genericMethod = parent->generic;
   this->withRetStatement = false;
}

//Compiler::CodeScope :: CodeScope(YieldScope* parent)
//   : Scope(parent), locals(Parameter(0))
//{
//   MethodScope* methodScope = (MethodScope*)parent->getScope(ScopeLevel::slMethod);
//
//   this->allocated1 = this->reserved1 = 0;
//   this->allocated2 = this->reserved2 = 0;
//   this->genericMethod = methodScope->generic;
//   this->withRetStatement = false;
//}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0))
{
   this->allocated1 = parent->allocated1;
   this->reserved1 = parent->reserved1;
   this->allocated2 = parent->allocated2;
   this->reserved2 = parent->reserved2;
   this->genericMethod = parent->genericMethod;
   this->withRetStatement = false;
}

void Compiler::CodeScope :: markAsAssigned(ObjectInfo object)
{
   if (object.kind == okLocal || object.kind == okLocalAddress) {
      for (auto it = locals.start(); !it.Eof(); it++) {
         if ((*it).offset == (int)object.param) {
            (*it).unassigned = false;
            return;
         }
      }
   }

   parent->markAsAssigned(object);
}

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
         if (info.kind != okUnknown) {
            if (EAttrs::test(mode, HINT_TYPEOF)) {
               if (info.reference) {
                  return Compiler::mapClassSymbol(*this, info.reference);
               }
               else return ObjectInfo();
            }
            else return info;
         }            
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
//   callNode = parent->parentCallNode;

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
   return -1 - allocated;
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
   if (constructionMode && isField(info.kind)) {
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
   _trackingUnassigned = false;
//   _dynamicDispatching = true; // !! temporal
//   _stackEvenMode = false;
//   _reservedAling = 4;

   this->_logic = logic;

   ByteCodeCompiler::loadOperators(_operators, _unaryOperators);
}

void Compiler :: writeMessageInfo(SyntaxWriter& writer, _ModuleScope& scope, mssg_t messageRef)
{
   ref_t actionRef, flags;
   pos_t argCount;
   decodeMessage(messageRef, actionRef, argCount, flags);

   IdentifierString name;
   ref_t signature = 0;
   name.append(scope.module->resolveAction(actionRef, signature));

   name.append('[');
   name.appendInt(argCount);
   name.append(']');

   writer.appendNode(lxMessageVariable, name);
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
      case BAND_OPERATOR_ID:
         retVal = arg1 & arg2;
         break;
      case BOR_OPERATOR_ID:
         retVal = arg1 | arg2;
         break;
      case BXOR_OPERATOR_ID:
         retVal = arg1 ^ arg2;
         break;
      case SHIFTR_OPERATOR_ID:
         retVal = arg1 >> arg2;
         break;
      case SHIFTL_OPERATOR_ID:
         retVal = arg1 << arg2;
         break;
      case BINVERTED_OPERATOR_ID:
         retVal = ~arg1;
         break;
      case NEGATIVE_OPERATOR_ID:
         retVal = -arg1;
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
   if (unboxWapper && object.reference == V_WRAPPER) {
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

inline void writeClassNameInfo(SyntaxWriter& writer, _Module* module, ref_t reference)
{
   ident_t className = module->resolveReference(reference);
   if (isTemplateWeakReference(className)) {
      // HOTFIX : save weak template-based class name directly
      writer.appendNode(lxClassName, className);
   }
   else {
      IdentifierString fullName(module->Name(), className);

      writer.appendNode(lxClassName, fullName.c_str());
   }
}

void Compiler :: declareCodeDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.appendNode(lxSourcePath, scope.saveSourcePath(_writer, node.identifier()));
}

void Compiler :: declareProcedureDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withSelf, bool withTargetSelf)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   // declare built-in variables
   if (withSelf) {
      if (scope.classStacksafe) {
         writer.newNode(lxBinarySelf, 1);
         writeClassNameInfo(writer, scope.module, scope.getClassRef());
         writer.closeNode();
      }
      else writer.appendNode(lxSelfVariable, 1);
   }

   if (withTargetSelf)
      writer.appendNode(lxSelfVariable, -1);

   writeMessageInfo(writer, *moduleScope, scope.message);

   int prefix = scope.functionMode ? 0 : -1;

   SNode current = node.firstChild();
   // method parameter debug info
   while (current != lxNone) {
      if (current == lxMethodParameter/* || current == lxIdentifier*/) {
         SNode identNode = current.findChild(lxNameAttr);
         if (identNode != lxNone) {
            identNode = identNode.firstChild(lxTerminalMask);
         }

         if (identNode != lxNone) {
            Parameter param = scope.parameters.get(identNode.identifier());
            if (param.offset != -1) {
               if (param.class_ref == V_ARGARRAY) {
                  writer.newNode(lxParamsVariable);
               }
               else if (param.class_ref == moduleScope->intReference) {
                  writer.newNode(lxIntVariable);
               }
               else if (param.class_ref == moduleScope->longReference) {
                  writer.newNode(lxLongVariable);
               }
               else if (param.class_ref == moduleScope->realReference) {
                  writer.newNode(lxReal64Variable);
               }
               else if (param.size != 0 && param.class_ref != 0) {
                  ref_t classRef = param.class_ref;
                  if (classRef != 0 && _logic->isEmbeddable(*moduleScope, classRef)) {

                     writer.newNode(lxBinaryVariable);
                     writeClassNameInfo(writer, scope.module, classRef);
                  }
                  else writer.newNode(lxVariable);
               }
               else writer.newNode(lxVariable);

               writer.appendNode(lxLevel, prefix - param.offset);

               IdentifierString name(identNode.identifier());
               writer.appendNode(lxIdentifier, name.c_str());

               writer.closeNode();
            }
         }
      }
      else if (current == lxSourcePath) {
         writer.appendNode(lxSourcePath, scope.saveSourcePath(_writer, current.identifier()));
      }

      current = current.nextNode();
   }
}

inline SNode findIdentifier(SNode current)
{
   while (current == lxAttribute)
      current = current.nextNode();

   if (current.firstChild(lxTerminalMask))
      return current.firstChild(lxTerminalMask);

   return current;
}

void Compiler :: importCode(SyntaxWriter& writer, SNode node, Scope& scope, ref_t functionRef, mssg_t message)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(scope.module->resolveReference(functionRef));
   virtualReference.append('.');

   pos_t argCount;
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

   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE)
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
      writer.appendNode(lxImporting, _writer.registerImportInfo(section, api, moduleScope->module));
   }
   else scope.raiseError(errInvalidLink, findIdentifier(node.firstSubNodeMask()));
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
            mssg_t message = it.key();

            (*it) = false;
            it++;

            if (test(message, STATIC_MESSAGE) && message != scope.moduleScope->init_message) {
               scope.info.methods.exclude(message);
               scope.info.methodHints.exclude(Attribute(message, maHint));
               scope.info.methodHints.exclude(Attribute(message, maReference));
            }
         }
      }

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

bool __FASTCALL isExtensionDeclaration(SNode node)
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

void Compiler :: compileParentDeclaration(SNode node, ClassScope& scope, bool extensionMode,
   LexicalType parentType)
{
   ref_t parentRef = 0;
   if (node == parentType) {
      parentRef = resolveParentRef(node, scope, false);
   }

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
   if (constant) {
      scope.info.type = SymbolExpressionInfo::Type::Constant;

      current = node.findChild(lxExpression).firstChild();
      if (current == lxInteger) {
         NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
         nsScope->defineIntConstant(scope.reference, current.identifier().toInt());
      }
   }
}

int Compiler :: resolveSize(SNode node, Scope& scope)
{
   if (node == lxInteger) {
      return node.identifier().toInt();
   }
   else if (node == lxHexInteger) {
      return node.identifier().toInt(16);
   }
   else if (node == lxIdentifier) {
      ObjectInfo constInfo = scope.mapTerminal(node.identifier(), false, EAttr::eaNone);
      if (constInfo.kind == okIntConstant) {
         return constInfo.extraparam;
      }
      else {
         scope.raiseError(errInvalidSyntax, node);

         return 0;
      }
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
         case 8:
            // treat it like qword
            attrs.fieldRef = V_PTR64;
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

ObjectInfo Compiler :: compileSwitchExpression(SyntaxWriter& writer, SNode node, ExprScope& scope)
{
   SNode lnode = node.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);

   ObjectInfo loperand = compileObject(writer, lnode, scope, HINT_PARAMETER, nullptr);

   writer.newNode(lxSwitching);

   // HOTFIX : the argument should not be type-less
   //          to use the branching over sending IF message
   if (!loperand.reference)
      loperand.reference = scope.moduleScope->superReference;

   SNode current = node.findChild(lxOption, lxElse);
   ArgumentsInfo arguments;
   while (current == lxOption) {
      SNode blockNode = current.firstChild(lxObjectMask);

      // find option value
//      SNode exprNode = optionNode.firstChild();

      int operator_id = EQUAL_OPERATOR_ID;

      writer.newNode(lxOption);

      ObjectInfo roperand = mapTerminal(current.firstChild(), scope, EAttr::eaNone);
      arguments.add(roperand);

      writer.newNode(lxExpression);
      ObjectInfo operationInfo = compileOperation(writer, current, scope, operator_id, loperand, &arguments, false, false, 0);

      ObjectInfo retVal;
      compileBranchingOp(writer, blockNode, scope, HINT_SWITCH, IF_OPERATOR_ID, operationInfo, retVal, EAttr::eaNone, nullptr);

      writer.closeNode();
      writer.closeNode();

      arguments.clear();

      current = current.nextNode();
   }

   if (current == lxElse) {
      bool withRetStatement = false;
      compileSubCode(writer, current.firstChild(), scope, false, withRetStatement);
   }

   writer.closeNode();

   return ObjectInfo(okObject);
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

LexicalType Compiler :: declareVariableType(CodeScope& scope, ObjectInfo& variable, ClassInfo& localInfo, int size,
   bool binaryArray, int& variableArg, ident_t& className)
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

   return variableType;
}

inline void copyIdentInfo(SNode target, SNode terminal, ident_t identifier)
{
   SNode ident = target.appendNode(lxIdentifier, identifier);

   SNode col = terminal.findChild(lxCol);
   ident.appendNode(col.type, col.argument);

   SNode row = terminal.findChild(lxRow);
   ident.appendNode(row.type, row.argument);
}

void Compiler :: declareVariable(SyntaxWriter& writer, SNode terminal, ExprScope& scope, ref_t typeRef, bool canBeIdle)
{
   int variableArg = 0;
   int size = 0;

   // COMPILER MAGIC : if it is a fixed-sized array
   if (terminal == lxArrayExpression) {
      //      if (size && opNode.nextNode() != lxNone)
      //         scope.raiseError(errInvalidSyntax, terminal);

      SNode sizeExprNode = terminal.firstChild(lxObjectMask);

      size = resolveArraySize(sizeExprNode, scope);

      terminal = terminal.firstChild(lxTerminalMask);
   }

   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   IdentifierString identifier(terminal.identifier());
   ident_t className = NULL;
   LexicalType variableType = lxVariable;

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
      codeScope->mapLocal(identifier, variable.param, variable.reference, variable.element, size, true);

      // injecting variable label
      SNode rootNode = findRootNode(writer.CurrentNode(), lxNewFrame, lxCode, lxCodeExpression);

      SNode varNode = rootNode.prependSibling(variableType, variableArg);
      varNode.appendNode(lxLevel, variable.param);
      copyIdentInfo(varNode, terminal, identifier);

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

//   if (opNode == lxNone && canBeIdle) {
//      // HOTFIX : remove the variable if the statement contains only a declaration
//      terminal = lxIdle;
//   }
}

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

//ObjectInfo Compiler :: compileTypeSymbol(SNode node, ExprScope& scope, EAttr mode)
//{
//   ObjectInfo retVal = mapClassSymbol(scope, resolveTemplateDeclaration(node, scope, false));
//
//   recognizeTerminal(node, retVal, scope, mode);
//
//   return retVal;
//}
//
//ObjectInfo Compiler :: compileYieldExpression(SNode objectNode, ExprScope& scope, EAttr mode)
//{
//   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);
//   MethodScope* methodScope = (MethodScope*)codeScope->getScope(Scope::ScopeLevel::slMethod);
//   int index = methodScope->getAttribute(maYieldContext);
//   int index2 = methodScope->getAttribute(maYieldLocals);
//
//   EAttrs objectMode(mode);
//   objectMode.include(HINT_NOPRIMITIVES);
//
//   objectNode.injectAndReplaceNode(lxSeqExpression);
//   SNode retExprNode = objectNode.firstChild(lxObjectMask);
//
//   YieldScope* yieldScope = (YieldScope*)scope.getScope(Scope::ScopeLevel::slYieldScope);
//
//   // save context
//   if (codeScope->reserved2 > 0) {
//      SNode exprNode = objectNode.insertNode(lxExpression);
//      SNode copyNode = exprNode.appendNode(lxCopying, codeScope->reserved2 << 2);
//      SNode fieldNode = copyNode.appendNode(lxFieldExpression);
//      fieldNode.appendNode(lxSelfLocal, 1);
//      fieldNode.appendNode(lxField, index);
//      fieldNode.appendNode(lxFieldAddress, 4);
//      copyNode.appendNode(lxLocalAddress, -2);
//
//      yieldScope->yieldContext.add(copyNode);
//   }
//
//   // save locals
//   int localsSize = codeScope->allocated1 - methodScope->preallocated;
//   if (localsSize) {
//      SNode expr2Node = objectNode.insertNode(lxExpression);
//      SNode copy2Node = expr2Node.appendNode(lxCopying, localsSize << 2);
//      SNode field2Node = copy2Node.appendNode(lxFieldExpression);
//      field2Node.appendNode(lxSelfLocal, 1);
//      field2Node.appendNode(lxField, index2);
//      SNode localNode = copy2Node.appendNode(lxLocalAddress, methodScope->preallocated/* + localsSize*//* - 1*/);
//
//      yieldScope->yieldLocals.add(localNode);
//
//      // HOTFIX : reset yield locals field on yield return to mark mg->yg reference
//      SNode expr3Node = objectNode.insertNode(lxAssigning);
//      SNode src3 = expr3Node.appendNode(lxFieldExpression);
//      src3.appendNode(lxSelfLocal, 1);
//      src3.appendNode(lxField, index2);
//      SNode dst3 = expr3Node.appendNode(lxFieldExpression);
//      dst3.appendNode(lxSelfLocal, 1);
//      dst3.appendNode(lxField, index2);
//   }
//
//   ObjectInfo retVal;
//   if (codeScope->withEmbeddableRet()) {
//      retVal = scope.mapTerminal(SELF_VAR, false, EAttr::eaNone);
//
//      // HOTFIX : the node should be compiled as returning expression
//      LexicalType ori = objectNode.type;
//      objectNode = lxReturning;
//      compileEmbeddableRetExpression(retExprNode, scope);
//      objectNode = ori;
//
//      recognizeTerminal(objectNode, retVal, scope, HINT_NODEBUGINFO | HINT_NOBOXING);
//
//      retExprNode.set(lxYieldReturning, index);
//   }
//   else {
//      //writer.appendNode(lxBreakpoint, dsStep);
//      retVal = compileExpression(retExprNode, scope, 0, objectMode);
//
//      analizeOperands(retExprNode, scope, 0, true);
//
//      retExprNode.injectAndReplaceNode(lxYieldReturning, index);
//   }
//
//   return retVal;
//}

ObjectInfo Compiler :: compileMessageReference(SyntaxWriter& writer, SNode terminal, SNode argNode, ExprScope& scope)
{
   ObjectInfo retVal;

   IdentifierString message;
   bool invalid = true;
   int argCount = 0;
   ObjectInfo roperand = compileExpression(writer, argNode, scope, 0, HINT_PARAMETER, nullptr);
   if (roperand.kind == okIntConstant) {
      argCount = roperand.extraparam;
      invalid = false;
   }

   if (invalid)
      scope.raiseError(errNotApplicable, terminal);

   if (terminal == lxIdentifier) {
      message.append('0' + (char)argCount);
      message.append(terminal.identifier());

      retVal.kind = okMessageConstant;

      retVal.reference = V_MESSAGE;
   }
   else if (terminal == lxType) {
      //SNode typeNode = terminal.findChild(lxType);
      //if (typeNode.nextNode() != lxNone)
         scope.raiseError(errNotApplicable, terminal);

      //ref_t extensionRef = resolveTypeAttribute(typeNode, scope, false, true);

      //message.append(scope.moduleScope->module->resolveReference(extensionRef));
      //message.append('.');
      //message.append('0' + (char)argCount);
      //message.append(terminal.firstChild(lxTerminalMask).identifier());

      //retVal.kind = okExtMessageConstant;
      //retVal.param = scope.moduleScope->module->mapReference(message);
      //retVal.reference = V_EXTMESSAGE;
   }
   else scope.raiseError(errNotApplicable, terminal);

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

   return retVal;
}

mssg_t Compiler :: mapMessage(SNode node, ExprScope& scope, bool extensionCall, bool newOpCall, bool propMode)
{
   ref_t actionFlags = 0;
   if (propMode)
      // COMPILER MAGIC : recognize the property get call
      actionFlags = PROPERTY_MESSAGE;

   if (extensionCall)
      actionFlags |= FUNCTION_MESSAGE;

   IdentifierString messageStr;

   SNode current = node.findChild(lxMessage);
   if (current != lxNone) {
      SNode name = current.firstChild(lxTerminalMask);
      //HOTFIX : if generated by a script / closure call
      if (name == lxNone) {
         if (current.argument)
            return current.argument;
      }

      messageStr.copy(name.identifier());

      current = current.nextNode();
   }
   else if (newOpCall) {
      // HOTFIX : skip the target
      current = node.findChild(lxIdentifier, lxReference, lxType, lxExpression).nextNode();
   }
   else current = node.firstChild(lxObjectMask);

   int argCount = 1;
   // if message has generic argument list
   while (true) {
      if (test(current.type, lxObjectMask)) {
         argCount++;
      }
//      else if (current == lxSubMessage) {
//         name = current.firstChild(lxTerminalMask);
//
//         int attr = scope.moduleScope->attributes.get(messageStr);
//         if (_logic->validateImplicitMethodAttribute(attr, true)) {
//            if (attr == tpSetAccessor) {
//               // COMPILER MAGIC : recognize an inline property set call
//               actionFlags = PROPERTY_MESSAGE;
//
//               messageStr.copy(name.identifier());
//            }
//            else scope.raiseError(errInvalidComlexMessageName, node);
//         }
//         else scope.raiseError(errInvalidComlexMessageName, node);
//      }
      else break;

      current = current.nextNode();
   }

   if (argCount >= ARG_COUNT) {
      actionFlags |= VARIADIC_MESSAGE;
      argCount = 2;
   }

   if (messageStr.Length() == 0) {
      if (newOpCall) {
         messageStr.copy(CONSTRUCTOR_MESSAGE);
      }
      else {
         actionFlags |= FUNCTION_MESSAGE;

         // if it is an implicit message
         messageStr.copy(INVOKE_MESSAGE);
      }
   }

   if (test(actionFlags, FUNCTION_MESSAGE))
      // exclude the target from the arg counter for the function
      argCount--;

   // if signature is presented
   ref_t actionRef = scope.moduleScope->module->mapAction(messageStr, 0, false);

   // create a message id
   return encodeMessage(actionRef, argCount, actionFlags);
}

ref_t Compiler :: mapExtension(Scope& scope, mssg_t& messageRef, ref_t implicitSignatureRef, ObjectInfo object, int& stackSafeAttr)
{
   ref_t objectRef = resolveObjectReference(scope, object, true);
   if (objectRef == 0) {
      objectRef = scope.moduleScope->superReference;
   }

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

   if (implicitSignatureRef) {
      // auto generate extension template for strong-typed signature
      for (auto it = nsScope->extensionTemplates.getIt(messageRef); !it.Eof(); it = nsScope->extensionTemplates.nextIt(messageRef, it)) {
         ref_t resolvedTemplateExtension = _logic->resolveExtensionTemplate(*scope.moduleScope, *this, *it,
            implicitSignatureRef, nsScope->ns, nsScope->outerExtensionList ? nsScope->outerExtensionList : &nsScope->extensions);
         if (resolvedTemplateExtension) {
            //ref_t strongMessage = encodeMessage()

            //nsScope->extensions.add(messageRef, Pair<ref_t, ref_t>(resolvedTemplateExtension, strongMessage));

         }
      }
   }

   // check extensions
   auto it = nsScope->extensions.getIt(messageRef);
   bool found = !it.Eof();
   if (found) {
      // generate an extension signature
      ref_t signatures[ARG_COUNT];
      ref_t signatureLen = scope.module->resolveSignature(implicitSignatureRef, signatures);
      for (size_t i = signatureLen; i > 0; i--)
         signatures[i] = signatures[i - 1];
      signatures[0] = objectRef;
      signatureLen++;

      size_t argCount = getArgCount(messageRef);
      while (signatureLen < argCount) {
         signatures[signatureLen] = scope.moduleScope->superReference;
         signatureLen++;
      }

      /*ref_t full_sign = */scope.module->mapSignature(signatures, signatureLen, false);
      mssg_t resolvedMessage = 0;
      ref_t resolvedExtRef = 0;
      int resolvedStackSafeAttr = 0;
      int counter = 0;
      while (!it.Eof()) {
         auto extInfo = *it;
         ref_t targetRef = nsScope->resolveExtensionTarget(extInfo.value1);
         int extStackAttr = 0;
         if (_logic->isMessageCompatibleWithSignature(*scope.moduleScope, targetRef, extInfo.value2,
            signatures, signatureLen, extStackAttr))
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
         counter++;

         it = nsScope->extensions.nextIt(messageRef, it);
      }

      if (resolvedMessage) {
         if (counter > 1 && implicitSignatureRef == 0) {
            // HOTFIX : does not resolve an ambigous extension for a weak message
         }
         else {
            // if we are lucky - use the resolved one
            messageRef = resolvedMessage;
            stackSafeAttr = resolvedStackSafeAttr;

            return resolvedExtRef;
         }
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

void Compiler :: compileBranchingNodes(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t ifReference,
   bool loopMode, bool switchMode)
{
   if (loopMode) {
      SNode thenCode = node.findSubNode(lxCode);
      if (thenCode == lxNone) {
         //HOTFIX : inline branching operator
         writer.newNode(lxIf, ifReference);

         thenCode = node.firstChild();
      }
      else writer.newNode(lxElse, ifReference);

      bool dummy = false;
      compileSubCode(writer, thenCode, scope, true, dummy);

      writer.closeNode();
   }
   else {
      SNode thenCode = node.findSubNode(lxCode);
      if (thenCode == lxNone) {
         thenCode = node;
      }

      writer.newNode(lxIf, ifReference);

      bool ifRetStatement = false;
      compileSubCode(writer, thenCode, scope, true, ifRetStatement);

      writer.closeNode();

      // HOTFIX : switch mode - ignore else
      if (!switchMode) {
         node = node.nextNode(lxObjectMask);
         if (node != lxNone) {
            SNode elseCode = node.findSubNode(lxCode);
            if (elseCode == lxNone) {
               writer.newNode(lxElse, ifReference);

               elseCode = node;
            }
            else writer.newNode(lxElse, 0);

            if (elseCode == lxNone)
               //HOTFIX : inline branching operator
               elseCode = node;

            bool elseRetStatement = false;
            compileSubCode(writer, elseCode, scope, true, elseRetStatement);
//            scope.setCodeRetStatementFlag(ifRetStatement && elseRetStatement);

            writer.closeNode();
         }
      }
   }
}

ref_t Compiler :: resolveOperatorMessage(Scope& scope, ref_t operator_id, pos_t argCount)
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
      case BAND_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(BAND_MESSAGE, 0, false), argCount, 0);
      case BOR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(BOR_MESSAGE, 0, false), argCount, 0);
      case BXOR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(BXOR_MESSAGE, 0, false), argCount, 0);
      case SHIFTR_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTR_MESSAGE, 0, false), argCount, 0);
      case SHIFTL_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SHIFTL_MESSAGE, 0, false), argCount, 0);
      case REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(REFER_MESSAGE, 0, false), argCount, 0);
      case SET_REFER_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(SET_REFER_MESSAGE, 0, false), argCount, 0);
      case NEGATIVE_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(NEGATIVE_MESSAGE, 0, false), argCount, PROPERTY_MESSAGE);
      case INVERTED_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(INVERTED_MESSAGE, 0, false), argCount, PROPERTY_MESSAGE);
      case BINVERTED_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(BINVERTED_MESSAGE, 0, false), argCount, PROPERTY_MESSAGE);
      case VALUE_OPERATOR_ID:
         return encodeMessage(scope.module->mapAction(VALUE_MESSAGE, 0, false), argCount, PROPERTY_MESSAGE);
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

void Compiler :: compileBranchingOp(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, int operator_id,
   ObjectInfo loperand, ObjectInfo& retVal, EAttr operationMode, ArgumentsInfo* preservedArgs)
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
      writer.CurrentNode().set(loopMode ? lxLooping : lxBranching, switchMode ? -1 : 0);

      compileBranchingNodes(writer, node, scope, ifReference, loopMode, switchMode);

//      if (loopMode) {
//         // check if the loop has root boxing operations
//         SNode exprNode = branchNode;
//         SNode rootExpr = exprNode.parentNode();
//         while (rootExpr != lxSeqExpression || rootExpr.argument != -1) {
//            exprNode = rootExpr;
//            rootExpr = rootExpr.parentNode();
//         }
//         if (exprNode.prevNode() != lxNone && rootExpr == lxSeqExpression) {
//            // bad luck : we have to relocate boxing expressions into the loop
//            SNode seqNode = branchNode.insertNode(lxSeqExpression);
//
//            exprNode = exprNode.prevNode();
//            while (exprNode != lxNone) {
//               // copy to the new location
//               SNode copyNode = seqNode.insertNode(exprNode.type, exprNode.argument);
//               SyntaxTree::copyNode(exprNode, copyNode);
//
//               // commenting out old one
//               exprNode = lxIdle;
//
//               exprNode = exprNode.prevNode();
//            }
//         }
//      }
   }
   else {
      // bad luck : we have to create a closure
      operator_id = original_id;

      mssg_t message = resolveOperatorMessage(scope, operator_id, 2);

      if (loperand.kind == okObject)
         loperand = saveToTempLocal(writer, scope, loperand);

      ArgumentsInfo arguments;
      ObjectInfo roperand = compileClosure(writer, node, scope, defineBranchingOperandMode(node) | HINT_PARAMETER,
         preservedArgs);
      arguments.add(roperand);

      SNode elseNode = node.nextNode();
      if (elseNode != lxNone) {
         message = overwriteArgCount(message, 3);

         compileClosure(writer, elseNode, scope, defineBranchingOperandMode(elseNode) | HINT_PARAMETER,
            preservedArgs);
      }

//      SNode parentNode = roperandNode.parentNode();

      retVal = compileMessage(writer, node, scope, loperand, message, &arguments, operationMode, 0, 0, preservedArgs);

//      if (loopMode) {
//         parentNode.injectAndReplaceNode(lxLooping);
//      }
   }
}

ObjectInfo Compiler :: compileBranchingOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode,
   int operator_id, ArgumentsInfo* preservedArgs)
{
   ObjectInfo retVal(okObject);

   SNode lnode = node.firstChild();

   writer.newNode(lxSeqExpression);

   EAttrs objMode(mode, HINT_LOOP | HINT_PARAMETER);
   writer.newNode(lxExpression);
   ObjectInfo loperand = compileObject(writer, lnode, scope, objMode, preservedArgs);
   writer.closeNode();

   compileBranchingOp(writer, lnode.nextNode(lxObjectMask), scope, mode, operator_id, loperand,
      retVal, mode, preservedArgs);

   writer.closeNode();

   if (EAttrs::test(mode, HINT_PARAMETER) && retVal.kind == okObject)
      retVal = saveToTempLocal(writer, scope, retVal);

   return retVal;
}

ObjectInfo Compiler :: compileIsNilOperator(SyntaxWriter& writer, SNode node, ExprScope& scope)
{
   SNode lnode = node.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);

   bool altMode = lnode.compare(lxMessageExpression, lxPropertyExpression);

   ObjectInfo loperand;
   if (altMode) {
      writer.newNode(lxAlt);

      loperand = compileExpression(writer, lnode, scope, 0, EAttr::eaNone, nullptr);
      writer.appendNode(lxNil);

      writer.closeNode();

      loperand = saveToTempLocal(writer, scope, ObjectInfo(okObject));
   }
   else {
      EAttrs objMode(HINT_TARGET/*, HINT_PROP_MODE*/);
      loperand = compileObject(writer, lnode, scope, objMode, nullptr);
   }

   ObjectInfo roperand = compileExpression(writer, rnode, scope, 0, HINT_PARAMETER, nullptr);

   writer.newNode(lxNilOp, ISNIL_OPERATOR_ID);
   writeTerminal(writer, loperand, scope);
   writeTerminal(writer, roperand, scope);
   writer.closeNode();

   ref_t loperandRef = resolveObjectReference(scope, loperand, false);
   ref_t roperandRef = resolveObjectReference(scope, roperand, false);

   ref_t resultRef = _logic->isCompatible(*scope.moduleScope, loperandRef, roperandRef, false) ? loperandRef : 0;

   return ObjectInfo(okObject, 0, resultRef);
}

//inline bool isLocalBoxingRequiredForOp(LexicalType type)
//{
//   switch (type) {
//      case lxIntOp:
//      case lxLongOp:
//      case lxRealOp:
//      case lxRealIntOp:
//         return true;
//      default:
//         return false;
//   }
//}

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

inline bool isNumericOp(LexicalType type)
{
   switch (type) {
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
      case lxRealIntOp:
         return true;
      default:
         return false;
   }
}

inline bool isUnaryOperation(int arg)
{
   switch (arg) {
   case INVERTED_OPERATOR_ID:
   case NEGATIVE_OPERATOR_ID:
   case BINVERTED_OPERATOR_ID:
      return true;
   default:
      return false;
   }
}

ObjectInfo Compiler :: compileOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, int operator_id, ObjectInfo loperand,
   ArgumentsInfo* arguments, bool assignMode, bool shortCircuitMode, ref_t expectedRef)
{
   bool withUnboxing = false;
   ObjectInfo lorigin = loperand;
   // the operands should be locally boxed if required
   if (assignMode && (loperand.kind == okFieldAddress || loperand.kind == okField)) {
      boxArgument(writer, node, loperand, scope, true);
      withUnboxing = true;
   }
   else if (assignMode && loperand.kind == okOuter) {
      int size = _logic->defineStructSize(*scope.moduleScope, loperand.reference, loperand.element);
      if (size != 0) {
         boxArgument(writer, node, loperand, scope, true);

         InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
         closure->markAsPresaved(lorigin);
      }
      withUnboxing = true;
   }

   writer.newNode(lxExpression);
   SNode opNode = writer.CurrentNode();

   ObjectInfo roperand;
   pos_t argCount = 1;
   if (shortCircuitMode) {
      argCount++;

      writeTerminal(writer, loperand, scope);

      writer.newNode(lxSeqExpression);
      roperand = compileExpression(writer, node.firstChild().nextNode(lxObjectMask),
         scope, 0, EAttr::eaNone, nullptr);
      writer.closeNode();
   }
   else {
      if (localBoxingRequired(*scope.moduleScope, loperand)) {
         withUnboxing = (loperand.kind == okFieldAddress);
         boxArgument(writer, node, loperand, scope, true);
      }

      if (arguments) {
         argCount += arguments->Length();
         roperand = (*arguments)[0];
         if (localBoxingRequired(*scope.moduleScope, roperand)) {
            boxArgument(writer, node, roperand, scope, true);
            (*arguments)[0] = roperand;
         }
      }
      // assuming fake second operand for unary operation to reuse existing code
      else roperand = ObjectInfo(okObject, 0, V_OBJECT);
   }

   ObjectInfo retVal;
   if (loperand.kind == okIntConstant && (roperand.kind == okIntConstant || isUnaryOperation(operator_id))) {
      int result = 0;
      if (calculateIntOp(operator_id, loperand.extraparam, roperand.extraparam, result)) {
         retVal = mapIntConstant(scope, result);

         // closing open expression
         writer.closeNode();

         return retVal;
      }
   }
   else if (loperand.kind == okRealConstant && roperand.kind == okRealConstant) {
      double l = scope.module->resolveConstant(loperand.param).toDouble();
      double r = scope.module->resolveConstant(roperand.param).toDouble();

      double result = 0;
      if (calculateRealOp(operator_id, l, r, result)) {
         retVal = mapRealConstant(scope, result);
         //node.set(lxConstantReal, retVal.param);

         // closing open expression
         writer.closeNode();

         return retVal;
      }
   }

   ref_t loperandRef = resolveObjectReference(scope, loperand, false);
   ref_t roperandRef = resolveObjectReference(scope, roperand, false);
   ref_t resultClassRef = 0;

   int operationType = _logic->resolveOperationType(*scope.moduleScope, operator_id, loperandRef, roperandRef,
      resultClassRef);

   if (operationType != 0) {
      // if it is a primitive operation
      if (assignMode) {
         retVal = loperand;
      }
      else if ((IsExprOperator(operator_id) && (LexicalType)operationType != lxBoolOp)
         || IsArrExprOperator(operator_id, (LexicalType)operationType))
      {
         retVal = allocateResult(scope, /*false, */resultClassRef, loperand.element);
      }
      else retVal = ObjectInfo(okObject, 0, resultClassRef, loperand.element, 0);

      ref_t opElementRef = loperand.element;
      if (operator_id == LEN_OPERATOR_ID)
         opElementRef = roperand.element;

      if (!shortCircuitMode) {
         writeTerminal(writer, loperand, scope);
         if (arguments != nullptr) {
            for (unsigned int i = 0; i < arguments->Length(); i++) {
               writeTerminal(writer, (*arguments)[i], scope);
            }
         }
         // add a second argument for an unary operation to reuse existing code
         else writer.appendNode(lxNil);
      }

      if (assignMode) {
         opNode.set((LexicalType)operationType, operator_id);
      }
      else _logic->injectOperation(opNode, scope, *this, operator_id, operationType, resultClassRef, opElementRef, retVal.param);

      // HOTFIX : update the result type
      retVal.reference = resultClassRef;

      if (IsArrExprOperator(operator_id, (LexicalType)operationType)) {
         // inject to target for array operation
         opNode.appendNode(lxLocalAddress, retVal.param);
      }
   }
   // if not, replace with appropriate method call
   else {
      // HOTFIX : to prevent double unboxing
      if (assignMode) {
         assignMode = false;
      }

      if (shortCircuitMode)
         arguments->add(saveToTempLocal(writer, scope, roperand));

      EAttr operationMode = HINT_NODEBUGINFO;
      ref_t implicitSignatureRef = arguments ? resolveStrongArgument(scope, arguments) : 0;

      int stackSafeAttr = 0;
      ref_t dummy = 0;
      int messageRef = resolveMessageAtCompileTime(loperand, scope,
         resolveOperatorMessage(scope, operator_id, argCount), implicitSignatureRef, false, stackSafeAttr, dummy);

      if (!test(stackSafeAttr, 1)) {
         operationMode = operationMode | HINT_DYNAMIC_OBJECT;
      }
      else stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

      retVal = compileMessage(writer, node, scope, loperand, messageRef, arguments, operationMode, stackSafeAttr, 
                  expectedRef, nullptr);
   }

   writer.closeNode();

   if (withUnboxing) {
      retVal = saveToTempLocal(writer, scope, retVal);
      unboxArgument(writer, loperand, scope);
   }      

   return retVal;
}

ObjectInfo Compiler :: compileOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode,
   int operator_id, bool assingMode, bool shortCircuitMode, ref_t expectedRef)
{
   SNode lnode = node.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);

   EAttrs objMode(mode | HINT_TARGET/*, HINT_PROP_MODE*/);
   ObjectInfo loperand = compileObject(writer, lnode, scope, objMode, nullptr);

   if (loperand.kind == okMessageRef) {
      if (operator_id != REFER_OPERATOR_ID)
         scope.raiseError(errInvalidOperation, lnode);

      // HOTFIX : recognize message constant
      return compileMessageReference(writer, lnode, rnode, scope);
   }

   ArgumentsInfo arguments;
   if (!shortCircuitMode) {
      ObjectInfo roperand = compileExpression(writer, rnode, scope, 0, HINT_PARAMETER, nullptr);

      arguments.add(roperand);
   }

   return compileOperation(writer, node, scope, operator_id, loperand, &arguments/*, mode*/, assingMode, 
      shortCircuitMode, expectedRef);
}

ObjectInfo Compiler :: compileUnaryOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, int operator_id)
{
   SNode lnode = node.firstChild(lxObjectMask);

   EAttrs objMode(mode | HINT_TARGET/*, HINT_PROP_MODE*/);
   ObjectInfo loperand = compileExpression(writer, lnode, scope, 0, objMode, nullptr);

   return compileOperation(writer, node, scope, operator_id, loperand, nullptr, false, false, 0);
}

inline ident_t resolveOperatorName(SNode node)
{
   SNode terminal = node.firstChild(lxTerminalMask);
   if (terminal != lxNone) {
      return terminal.identifier();
   }
   else return node.identifier();
}

ObjectInfo Compiler :: compileUnaryExpression(SyntaxWriter& writer, SNode node, ExprScope& scope,
   EAttr mode)
{
   int operator_id = (int)node.argument > 0 ? node.argument : _unaryOperators.get(resolveOperatorName(node.findChild(lxOperator)));

   return compileUnaryOperation(writer, node, scope, mode, operator_id);
}

ObjectInfo Compiler :: compileOperationExpression(SyntaxWriter& writer, SNode node, ExprScope& scope,
   EAttr mode, ref_t expectedRef)
{
   ArgumentsInfo preservedArgs;

   int operator_id = (int)node.argument > 0 ? node.argument : _operators.get(resolveOperatorName(node.findChild(lxOperator)));

   switch (operator_id) {
      case IF_OPERATOR_ID:
      case IFNOT_OPERATOR_ID:
         // if it is branching operators
         return compileBranchingOperation(writer, node, scope, mode, operator_id, &preservedArgs);
      case CATCH_OPERATOR_ID:
      case FINALLY_OPERATOR_ID:
         return compileCatchOperator(writer, node, scope, operator_id);
      case ALT_OPERATOR_ID:
         return compileAltOperator(writer, node, scope/*, mode, operator_id*/);
      case ISNIL_OPERATOR_ID:
         return compileIsNilOperator(writer, node, scope);
      case APPEND_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, ADD_OPERATOR_ID, true, false, expectedRef);
      case REDUCE_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, SUB_OPERATOR_ID, true, false, expectedRef);
      case BAPPEND_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, BOR_OPERATOR_ID, true, false, expectedRef);
      case BINCREASE_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, BAND_OPERATOR_ID, true, false, expectedRef);
      case INCREASE_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, MUL_OPERATOR_ID, true, false, expectedRef);
      case SEPARATE_OPERATOR_ID:
         node.setArgument(DIV_OPERATOR_ID);
         return compileOperation(writer, node, scope, mode, DIV_OPERATOR_ID, true, false, expectedRef);
      case AND_OPERATOR_ID:
      case OR_OPERATOR_ID:
         return compileOperation(writer, node, scope, mode, operator_id, false, true, expectedRef);
      default:
         return compileOperation(writer, node, scope, mode, operator_id, false, false, expectedRef);
   }
}

ObjectInfo Compiler :: compileMessage(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target, mssg_t messageRef,
   ArgumentsInfo* arguments, EAttr mode, int stackSafeAttr, ref_t expectedRef, ArgumentsInfo* presavedArgs)
{
   ObjectInfo retVal(okObject);

   LexicalType operation = lxCalling_0;
   int argument = messageRef;

   // try to recognize the operation
   ref_t classReference = resolveObjectReference(scope, target, true);
   ref_t constRef = 0;

//   bool inlineArgCall = EAttrs::test(mode, HINT_INLINEARGMODE);
////   bool dispatchCall = false;
   bool directCall = EAttrs::test(mode, HINT_DIRECTCALL);
   _CompilerLogic::ChechMethodInfo result;
   int callType = 0;
   if (/*!inlineArgCall && */!directCall) {
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

   if (target.kind == okSuper) {
      // parent methods are always sealed
      callType = tpSealed;
   }

   /*if (inlineArgCall) {
      operation = lxInlineArgCall;
      argument = messageRef;
   }
   else */if (callType == tpClosed || callType == tpSealed) {
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
      // if the message is not supported - clear stack safe
      if (callType == tpUnknown)
         stackSafeAttr = 0;

      // if the sealed / closed class found and the message is not supported - warn the programmer and raise an exception
      if (EAttrs::test(mode, HINT_SILENT)) {
         // do nothing in silent mode
      }
      else if (result.found && !result.withCustomDispatcher && callType == tpUnknown) {
         bool ignoreWarning = false;
         if (result.withVariadicDispatcher) {
            ref_t variadicMessage = resolveVariadicMessage(scope, messageRef);

            _CompilerLogic::ChechMethodInfo variadicResult;
            int variadicCallType = _logic->resolveCallType(*scope.moduleScope, classReference, variadicMessage, variadicResult);
            if (variadicCallType)
               ignoreWarning = true;
         }

         if (target.reference == scope.moduleScope->superReference || !target.reference || ignoreWarning) {
            // ignore warning for super class / type-less one
         }
         else if (EAttrs::test(mode, HINT_CONVERSIONOP)) {
            SNode terminal = node.firstChild(lxObjectMask).firstChild(lxTerminalMask);
            if (terminal != lxNone) {
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownConversion, node);
            }
            else if (node.parentNode() == lxNewFrame){
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownEOPConversion, node.parentNode().findChild(lxEOP));
            }
         }
         else if (node.findChild(lxMessage).firstChild(lxTerminalMask) == lxNone) {
            if (getAction(messageRef) == getAction(scope.moduleScope->constructor_message)) {
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownDefConstructor, node);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownFunction, node);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, node.findChild(lxMessage));
      }
   }

   analizeArguments(writer, node, scope, stackSafeAttr, target, arguments/*, false*/);
   //   scope.originals.clear();

   if (expectedRef && result.withEmbeddableRet) {
      mssg_t byRefMessageRef = _logic->resolveEmbeddableRetMessage(
         scope, *this, resolveObjectReference(scope, target, true),
         messageRef, expectedRef);

      if (byRefMessageRef) {
         ObjectInfo tempVar = allocateResult(scope, expectedRef);

         if (tempVar.kind == okTempLocalAddress) {
            argument = byRefMessageRef;

            arguments->add(tempVar);
         }
         else if (tempVar.kind == okLocal) {
            ObjectInfo wrapper(okOutputBoxableLocal, tempVar.param, V_WRAPPER, tempVar.reference, 0);

            boxArgument(writer, node, wrapper, scope, false);
            arguments->add(wrapper);

            argument = byRefMessageRef;
         }
         else throw InternalError("Not yet implemented"); // !! temporal         

         retVal = tempVar;
      }
   }

   // inserting calling expression
   writer.newNode(operation, argument);

   if (result.embeddable) {
      writer.appendNode(lxEmbeddableAttr);
   }

   if (classReference)
      writer.appendNode(lxCallTarget, classReference);

   if (constRef && callType == tpSealed) {
      writer.appendNode(lxConstAttr, constRef);

      NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

      retVal = ns->defineObjectInfo(constRef, true);
   }

   bool withUnboxing = unboxingRequired(target);
   writeTerminal(writer, target, scope);
   if (arguments != nullptr) {
      for (unsigned int i = 0; i < arguments->Length(); i++) {
         ObjectInfo arg = (*arguments)[i];

         writeTerminal(writer, arg, scope);

         withUnboxing |= unboxingRequired(arg);
      }
   }

   writer.closeNode();

   if (withUnboxing || (presavedArgs && presavedArgs->Length() > 0)) {
      if (retVal.kind == okObject)
         retVal = saveToTempLocal(writer, scope, retVal);

      if (arguments)
         unboxArguments(writer, scope, target, arguments);

      if (presavedArgs)
         unboxPreservedArgs(writer, scope, presavedArgs);
   }

   return retVal;
}

void Compiler :: appendCopying(SyntaxWriter& writer, int size, bool variadic, /*bool primArray,*/
   ObjectInfo target, ExprScope& scope, int tempLocal)
{
   // copying boxed object
   if (variadic) {
      // NOTE : structure command is used to copy variadic argument list
      writer.newNode(lxCloning);
   }
//   else if (primArray) {
//      // if it is a dynamic srtructure boxing
//      copyingNode.injectAndReplaceNode(lxCloning);
//   }
   else if (size != 0) {
      writer.newNode(lxCopying, size);
   }
   // otherwise consider it as a byref variable
   else writer.newNode(lxByRefAssigning);

   writer.appendNode(lxTempLocal, tempLocal);
   writeTerminal(writer, target, scope);

   writer.closeNode();
}

void Compiler :: appendCreating(SyntaxWriter& writer, int size, ref_t typeRef)
{
   if (size == 0) {
      // HOTFIX : recognize byref boxing
      writer.newNode(lxCreatingClass, 1);
   }
   else writer.newNode(lxCreatingStruct, size);

   writer.appendNode(lxType, typeRef);

   writer.closeNode();
}

ObjectInfo Compiler :: boxExternal(SyntaxWriter& writer, ObjectInfo target, ExprScope& scope)
{
   // HOTFIX : replace dword with an integer
   if (target.reference == V_DWORD)
      target.reference = scope.moduleScope->intReference;

   ref_t targetRef = resolveObjectReference(scope, target, true, false);
   bool variable = false;
   int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, target.element, variable);

   writer.newNode(target.param == -1 ? lxFloatSaving : lxSaving, size);

   ObjectInfo retVal;
   allocateTempStructure(scope, size, false, retVal);
   retVal.reference = targetRef;

   writer.appendNode(lxLocalAddress, retVal.param);
   writer.appendNode(lxResult);

   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: boxArgumentInPlace(SyntaxWriter& writer, SNode node, ObjectInfo arg, ExprScope& scope,
   bool localBoxingMode, bool condBoxing)
{
   return boxArgumentInPlace(writer, node, arg, resolveObjectReference(scope, arg, true, false), scope,
      localBoxingMode, condBoxing);
}

ObjectInfo Compiler :: boxArgumentInPlace(SyntaxWriter& writer, SNode node, ObjectInfo source, ref_t targetRef,
   ExprScope& scope, bool localBoxingMode, bool condBoxing)
{
   // if the source is the result of an operation - presave it
   if (source.kind == okObject)
      source = saveToTempLocal(writer, scope, source);

   ObjectInfo boxedArg;

   bool variable = false;
   int size = _logic->defineStructSizeVariable(*scope.moduleScope, targetRef, source.element, variable);

   if (source.kind == okFieldAddress && isPrimitiveArrRef(source.reference))
      // use fixed size (for fixed-sized array fields)
      // note that we still need to execute defineStructSizeVariable to set variable bool value
      size = defineFieldSize(scope, source.param) * (-size);

   bool variadic = source.kind == okParams;
//   bool primArray = node == lxPrimArrBoxableExpression;

   if (targetRef != 0) {
      if (localBoxingMode) {
         bool fixedSizeArray = isPrimitiveArrRef(source.reference) && size > 0;

         // inject local boxed object
         ObjectInfo tempBuffer;
         allocateTempStructure(scope, size, fixedSizeArray, tempBuffer);

         if (fixedSizeArray) {
            writer.newNode(lxTempBinaryArray, size);
            writer.appendNode(lxLevel, tempBuffer.param);
            writer.closeNode();
         }

         writer.newNode(lxCopying, size);
         writer.appendNode(lxLocalAddress, tempBuffer.param);
         writeTerminal(writer, source, scope);
         writer.closeNode();

         boxedArg = ObjectInfo(okTempLocalAddress, tempBuffer.param, targetRef, 0, variable ? size : 0);
      }
      else {
         int tempLocal = 0;
         bool skipCopy = source.kind == okOutputBoxableLocal;
         //if (source.kind == okTempLocal) {
         //   tempLocal = source.param;
         //}
         /*else*/ tempLocal = scope.newTempLocal();

         //         if (isPrimitiveRef(typeRef)) {
//            ref_t elementRef = node.findChild(lxElementType).argument;
//
//            typeRef = resolvePrimitiveReference(scope, typeRef, elementRef, false);
//         }

//         SNode seqNode= objNode;
//         seqNode.injectAndReplaceNode(lxSeqExpression);
//
         if (condBoxing)
            writer.newNode(lxCondBoxing);

         writer.newNode(lxAssigning);
         writer.appendNode(lxTempLocal, tempLocal);

         if (size < 0) {
            writer.newNode(lxCloning);
            writer.appendNode(lxType, targetRef);
            writeTerminal(writer, source, scope);
            writer.closeNode();
         }
         else if (variadic) {
            // box a variadic argument list
            ObjectInfo sizeLocal = allocateResult(scope, scope.moduleScope->intReference);

            writer.newNode(lxSeqExpression);

            writer.newNode(lxArgArrOp, LEN_OPERATOR_ID);
            writeTerminal(writer, sizeLocal, scope);
            writeTerminal(writer, source, scope);
            writer.closeNode();

            writer.newNode(lxNewArrOp, targetRef);
            writer.appendNode(lxSize, 0);
            writeTerminal(writer, sizeLocal, scope);
            writer.closeNode();

            writer.closeNode();
         }
         else appendCreating(writer, size, targetRef);

         writer.closeNode();

         if (size >= 0 && !skipCopy)
            appendCopying(writer, size, variadic/*, primArray*/, source, scope, tempLocal);

         if (condBoxing)
            writer.closeNode();

         boxedArg = ObjectInfo(okTempLocal, tempLocal, targetRef, 0, variable ? size : 0);
         if (variable && (source.kind == okBoxableLocal || source.kind == okOutputBoxableLocal)) {
            boxedArg.kind = okTempBoxableLocal;
         }

         if (size < 0 || isPrimitiveArrRef(source.reference)) {
            boxedArg.element = source.element;
         }
      }
   }
   else scope.raiseError(errInvalidBoxing, node);

   return boxedArg;
}

void Compiler :: boxArgument(SyntaxWriter& writer, SNode node, ObjectInfo& target, ExprScope& scope,
   bool withLocalBoxing)
{
   Attribute key(target.kind, target.param);

   int tempLocal = scope.tempLocals.get(key);
   if (tempLocal == NOTFOUND_POS) {
      target = boxArgumentInPlace(writer, node, target, scope, withLocalBoxing, condBoxingRequired(target));

      scope.tempLocals.add(key, target.param);
   }
   else target = ObjectInfo(okTempLocal, tempLocal);

//   else if (current.compare(lxBoxableExpression, lxCondBoxableExpression)) {
//      // resolving double boxing
//      current.set(lxExpression, 0);
//
//      boxArgument(boxExprNode, current.firstChild(lxObjectMask), scope, boxingMode, withoutLocalBoxing,
//         inPlace, condBoxing);
//   }
//   else if (current == lxArgBoxableExpression) {
//      throw InternalError("Not yet implemented");
//   }
//   else {
//      if (current == lxNewArrOp) {
//         ref_t typeRef = boxExprNode.findChild(lxType).argument;
//
//         current.setArgument(typeRef);
//      }
//      else if (current.compare(lxStdExternalCall, lxExternalCall, lxCoreAPICall)) {
//         if (boxingMode) {
//            injectIndexBoxing(boxExprNode, current, scope);
//         }
//      }
//      else if (boxingMode || (current == lxFieldExpression && !withoutLocalBoxing)) {
//         if (inPlace || (test(current.type, lxOpScopeMask) && current != lxFieldExpression)) {
//            boxExpressionInPlace(boxExprNode, current, scope, !boxingMode, condBoxing);
//         }
//         else {
//            SNode argNode = current;
//            if (current == lxFieldExpression) {
//               argNode = current.lastChild(lxObjectMask);
//            }
//
//            if (boxingMode || (!withoutLocalBoxing && argNode == lxFieldAddress)) {
//               Attribute key(argNode.type, argNode.argument);
//
//               int tempLocal = scope.tempLocals.get(key);
//               if (tempLocal == NOTFOUND_POS) {
//                  tempLocal = scope.newTempLocal();
//                  scope.tempLocals.add(key, tempLocal);
//
//                  boxExpressionInRoot(boxExprNode, current, scope, lxTempLocal, tempLocal,
//                     !boxingMode, condBoxing);
//               }
//               else current.set(lxTempLocal, tempLocal);
//            }
//         }
//      }
//   }
}

void Compiler :: analizeArguments(SyntaxWriter& writer, SNode node, ExprScope& scope, int stackSafeAttr,
   ObjectInfo& target, ArgumentsInfo* arguments/*, bool inPlace*/)
{
   int argBit = 1;
   if ((!test(stackSafeAttr, argBit) && boxingRequired(target))) {
      boxArgument(writer, node, target, scope, false);
   }
   else if (localBoxingRequired(*scope.moduleScope, target))
   {
      boxArgument(writer, node, target, scope, true);
   }

   if (arguments) {
      for (pos_t i = 0; i != arguments->Length(); i++) {
         argBit <<= 1;
         ObjectInfo arg = (*arguments)[i];
         if (!test(stackSafeAttr, argBit) && boxingRequired(arg)) {
            boxArgument(writer, node, arg, scope, false);

            (*arguments)[i] = arg;
         }
         else if (localBoxingRequired(*scope.moduleScope, arg)) {
            boxArgument(writer, node, arg, scope, true);

            (*arguments)[i] = arg;
         }
      }
   }
}

void Compiler :: unboxArgument(SyntaxWriter& writer, ObjectInfo& target, ExprScope& scope)
{
   ObjectInfo source;
   for (auto it = scope.tempLocals.start(); !it.Eof(); it++) {
      if (*it == (int)target.param) {
         auto key = it.key();
         source = ObjectInfo((ObjectKind)key.value1, key.value2);

         break;
      }
   }

   bool condBoxing = source.kind == okParam || source.kind == okSelfParam || source.kind == okSuper;
   if (condBoxing)
      writer.newNode(lxCondUnboxing);

   bool primArray = target.kind == okTempLocal && target.element != 0;
   bool refMode = false;
   if (target.kind == okTempBoxableLocal) {
      // HOTFIX : if it is byref variable unboxing
      writer.newNode(lxAssigning, 0);
      refMode = true;
   }
   else if (primArray) {
      writer.newNode(lxCloning, 0);
   }
   else if (target.kind == okTempLocalAddress && !target.extraparam) {
      pos_t size = _logic->defineStructSize(*scope.moduleScope, target.reference, target.element);

      writer.newNode(lxCopying, size);
   }
   else if ((int)target.extraparam < 0) {
      writer.newNode(lxCloning, 0);
   }
   else writer.newNode(lxCopying, target.extraparam);
   //         else if (condBoxing)
   //            unboxing.set(lxCondCopying, size);

   //         SNode unboxing = current.appendNode(lxCopying, size);

   writeTerminal(writer, source, scope);
   if (refMode) {
      writer.newNode(lxFieldExpression);
      writeTerminal(writer, target, scope);
      writer.appendNode(lxField, 0);
      writer.closeNode();
   }
   else writeTerminal(writer, target, scope);

   writer.closeNode();

   if (condBoxing)
      writer.closeNode();
}

void Compiler :: unboxPreservedArgs(SyntaxWriter& writer, ExprScope& scope, ArgumentsInfo* arguments)
{
   // first argument is a closure
   ObjectInfo target;

   for (pos_t i = 0; i != arguments->Length(); i++) {
      ObjectInfo info = (*arguments)[i];
      if (info.kind == okClosureInfo) {
         target = (*arguments)[++i];
      }
      else if (info.kind == okMemberInfo) {
         ObjectInfo source = (*arguments)[++i];
         //   SNode parent = node;
         //   SNode current;
         //   if (scope.callNode != lxNone) {
         //      // HOTFIX : closure member unboxing should be done right after the operation if it is possible
         //      if (scope.callNode == lxExpression) {
         //         scope.callNode.injectAndReplaceNode(lxSeqExpression);
         //
         //         current = scope.callNode.appendNode(lxNestedSeqExpression);
         //      }
         //      else if (scope.callNode == lxSeqExpression) {
         //         current = scope.callNode.findChild(lxNestedSeqExpression);
         //      }
         //      else current = injectRootSeqExpression(parent);
         //   }
         //   else current = injectRootSeqExpression(parent);
         //
         //   ref_t targetRef = resolveObjectReference(scope, member, false);

         if (source.kind == okLocalAddress) {
            // if the parameter may be stack-allocated
            bool variable = false;
            ref_t sourceRef = resolveObjectReference(scope, source, true);
            int size = _logic->defineStructSizeVariable(*scope.moduleScope, sourceRef, source.element, variable);

            writer.newNode(lxCopying, size);
            writeTerminal(writer, source, scope);
            writer.newNode(lxFieldExpression);
            writeTerminal(writer, target, scope);
            writer.appendNode(lxField, info.param);
            writer.closeNode();
            writer.closeNode();
         //      Attribute key(lxLocalAddress, member.param);
         //      int boxedLocal = scope.tempLocals.get(key);
         //      if (boxedLocal != NOTFOUND_POS) {
         //         // HOTFIX : check if the variable is used several times - modify the boxed argument as well
         //         SNode assignBoxNode = current.appendNode();
         //         assignBoxNode.appendNode(lxTempLocal, boxedLocal);
         //
         //         SNode fieldExpr = assignBoxNode.appendNode(lxFieldExpression);
         //         fieldExpr.appendNode(tempType, tempLocal);
         //         fieldExpr.appendNode(lxField, memberIndex);
         //      }
         }
         else if (source.kind == okLocal) {
            //      if (oriTempLocal == -1) {
            //         // HOTFIX : presave the original value
            //         oriTempLocal = scope.newTempLocal();
            //         SNode oriAssignNode = current.insertNode(lxAssigning);
            //         oriAssignNode.appendNode(lxTempLocal, oriTempLocal);
            //         oriAssignNode.appendNode(lxLocal, member.param);
            //      }
            //
            //      SNode assignNode = current.appendNode(lxAssigning);
            //      if (oriTempLocal) {
            //         assignNode.set(lxCondAssigning, oriTempLocal);
            //      }
            //
            //      assignNode.appendNode(lxLocal, member.param);
            //
            writer.newNode(lxAssigning);

            writeTerminal(writer, source, scope);

            writer.newNode(lxFieldExpression);
            writeTerminal(writer, target, scope);
            writer.appendNode(lxField, info.param);
            writer.closeNode();

            writer.closeNode();

            //      SNode fieldExpr = assignNode.appendNode(lxFieldExpression);
            //      fieldExpr.appendNode(tempType, tempLocal);
            //      fieldExpr.appendNode(lxField, memberIndex);
         }
         //   else if (member.kind == okOuter) {
         //      SNode assignNode = current.appendNode(lxAssigning);
         //      SNode target = assignNode.appendNode(lxVirtualReference);
         //      recognizeTerminal(target, member, scope, HINT_NODEBUGINFO);
         //
         //      SNode fieldExpr = assignNode.appendNode(lxFieldExpression);
         //      fieldExpr.appendNode(tempType, tempLocal);
         //      fieldExpr.appendNode(lxField, memberIndex);
         //   }
         //   else throw InternalError("Not yet implemented"); // !! temporal

      }
   }
}

void Compiler :: unboxArguments(SyntaxWriter& writer, ExprScope& scope, ObjectInfo& target, ArgumentsInfo* arguments)
{
   if ((target.kind == okTempLocal && target.extraparam != 0) || target.kind == okTempBoxableLocal) {
      unboxArgument(writer, target, scope);
   }

   if (arguments) {
      for (pos_t i = 0; i != arguments->Length(); i++) {
         ObjectInfo arg = (*arguments)[i];
         if ((arg.kind == okTempLocal && arg.extraparam != 0) || arg.kind == okTempBoxableLocal) {
            unboxArgument(writer, arg, scope);
         }
      }
   }
}

ObjectInfo Compiler :: injectImplicitConversion(SyntaxWriter& writer, SNode node, ExprScope& scope,
   ObjectInfo source, ref_t targetRef)
{
   ref_t sourceRef = resolveObjectReference(scope, source, false);

   auto info = _logic->injectImplicitConversion(scope, *this, targetRef, sourceRef,
      source.element/*, noUnboxing, fixedArraySize*/);

   if (info.result == ConversionResult::crCompatible) {
      return source;
   }
   else if (info.result == ConversionResult::crBoxingRequired) {
      switch (source.kind) {
         case okLocalAddress:
         case okTempLocalAddress:
            source.reference = targetRef;
            return source;
         case okLocal:
            source.kind = okBoxableLocal;
            source.reference = targetRef;
            return source;
         default:
            return boxArgumentInPlace(writer, node, source, targetRef, scope, false, false);
      }
   }
   else if (info.result == ConversionResult::crConverted) {
      if (source.kind == okObject) {
         // save the result of operation if required
         source = saveToTempLocal(writer, scope, source);
      }

      ArgumentsInfo arguments;
      arguments.add(source);
      ObjectInfo targetInfo(okClass, targetRef);
      analizeArguments(writer, node, scope, info.stackSafeAttr, targetInfo, &arguments/*, false*/);

      writer.newNode(lxDirectCalling, info.message);
      writer.appendNode(lxCallTarget, info.classRef);
      writer.appendNode(lxClassSymbol, targetRef);
      if (info.embeddable) {
         writer.appendNode(lxEmbeddableAttr);
      }

      writeTerminal(writer, arguments[0], scope);
      writer.closeNode();

      //if (node.compare(lxDirectCalling, lxSDirectCalling)) {
      //   // HOTFIX : box arguments if required
      //   analizeOperands(node, scope, stackSafeAttrs, false);
      //}

      return ObjectInfo(okObject, 0, targetRef);
   }
   else return ObjectInfo();
}

ObjectInfo Compiler :: convertObject(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef, ObjectInfo source, EAttr mode)
{
//   bool noUnboxing = EAttrs::test(mode, HINT_NOUNBOXING);
   ref_t sourceRef = resolveObjectReference(scope, source, false);
   bool dynamicHint = EAttrs::test(mode, HINT_DYNAMIC_OBJECT);
   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef, false)) {
      if ((source.kind == okIntConstant || source.kind == okUIntConstant)
         && targetRef == scope.moduleScope->intReference && !dynamicHint)
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->intReference;

         return source;
      }
      else if ((source.kind == okLongConstant)
         && targetRef == scope.moduleScope->longReference && !dynamicHint)
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->longReference;

         return source;
      }
      else if ((source.kind == okRealConstant)
         && targetRef == scope.moduleScope->realReference && !dynamicHint)
      {
         // HOTFIX : allow to pass the constant directly
         source.reference = scope.moduleScope->realReference;

         return source;
      }
      //      else if (source.kind == okExternal &&
      //         _logic->isCompatible(*scope.moduleScope, sourceRef, targetRef, true))
      //      {
      //         // HOTFIX : allow to pass the result of external operation directly
      //         return source;
      //      }
      else {
         int fixedArraySize = 0;
         if (source.kind == okFieldAddress && isPrimitiveArrRef(sourceRef))
            fixedArraySize = defineFieldSize(scope, source.param);

         ObjectInfo retVal = injectImplicitConversion(writer, node, scope, source, targetRef);
         if (retVal.kind == okUnknown) {
            return sendTypecast(writer, node, scope, targetRef, source, EAttrs::test(mode, HINT_SILENT));
         }
         else return retVal;
      }
   }

   return source;
}

ObjectInfo Compiler :: sendTypecast(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef,
   ObjectInfo source, bool silentMode)
{
   if (targetRef != 0 /*&& !isPrimitiveRef(targetRef)*/) {
      if (targetRef != scope.moduleScope->superReference) {
         //HOTFIX : ignore super object
         ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         //node.refresh();
         //if (node != lxExpression)
         //   node.injectAndReplaceNode(lxExpression);

         EAttr mode = silentMode ? HINT_SILENT : EAttr::eaNone;
         compileMessage(writer, node, scope, source, encodeMessage(actionRef, 1, CONVERSION_MESSAGE), nullptr,
            mode | HINT_NODEBUGINFO | HINT_CONVERSIONOP, 0, 0, nullptr);

         return ObjectInfo(okObject, 0, targetRef);
      }
      else return source;
   }
   // NOTE : if the typecasting is not possible, it returns unknown result
   else return ObjectInfo();
}

ref_t Compiler :: resolveStrongArgument(ExprScope& scope, ArgumentsInfo* arguments)
{
   ref_t argRef[2];
   argRef[0] = resolveObjectReference(scope, (*arguments)[0], true);
   if (arguments->Length() == 2) {
      argRef[1] = resolveObjectReference(scope, (*arguments)[1], true);

      if (!argRef[0] || !argRef[1])
         return 0;

      return scope.module->mapSignature(argRef, 2, false);
   }
   else {
      if (!argRef[0])
         return 0;

      return scope.module->mapSignature(argRef, 1, false);
   }
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

ref_t Compiler :: compileMessageParameters(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode, ref_t expectedSignRef,
   bool& variadicOne, /*bool& inlineArg, */ArgumentsInfo& arguments, ArgumentsInfo* preservedArgs)
{
//   if (node != lxNone)
//      scope.callNode = node.parentNode();
//
   EAttr paramMode = HINT_PARAMETER;
   if (EAttrs::test(mode, HINT_SILENT))
      paramMode = paramMode | HINT_SILENT;

//   bool externalMode = false;
   if (EAttrs::test(mode, HINT_EXTERNALOP)) {
//      externalMode = true;
   }
   else paramMode = paramMode | HINT_NOPRIMITIVES;

   SNode current = node;

   // compile the message argument list
   ref_t signatures[ARG_COUNT] = {0};
   if (expectedSignRef)
      scope.module->resolveSignature(expectedSignRef, signatures);

   ref_t signatureLen = 0;
   while (/*current != lxMessage && */current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         // try to recognize the message signature
         // NOTE : signatures[signatureLen] contains expected parameter type if expectedSignRef is provided
		   ObjectInfo paramInfo = compileExpression(writer, current, scope, signatures[signatureLen],
            paramMode, preservedArgs);

         ref_t argRef = resolveObjectReference(scope, paramInfo, false);
         if (signatureLen >= ARG_COUNT) {
            signatureLen++;
         }
      //   else if (inlineArg) {
      //      scope.raiseError(errNotApplicable, current);
      //   }
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
      //   else if (argRef == V_INLINEARG) {
      //      if (signatureLen == 0) {
      //         inlineArg = true;
      //      }
      //      else scope.raiseError(errNotApplicable, current);
      //   }
         else if (argRef) {
            signatures[signatureLen++] = argRef;

      //      if (externalMode && !current.existChild(lxType))
      //         current.appendNode(lxType, argRef);
         }
         else signatures[signatureLen++] = scope.moduleScope->superReference;

         arguments.add(paramInfo);

////         if (externalMode) {
////            writer.appendNode(lxExtArgumentRef, argRef);
////            writer.closeNode();
////         }
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
      if (!anonymous/* || variadicOne*/)
         return scope.module->mapSignature(signatures, signatureLen, false);
   }

   return 0;
}

bool Compiler :: unboxingRequired(ObjectInfo& info)
{
   if (info.kind == okTempLocal) {
      return info.extraparam != 0;
   }
   else return info.kind == okTempBoxableLocal;
}

bool Compiler :: localBoxingRequired(_ModuleScope& scope, ObjectInfo& info)
{
   switch (info.kind){
      case okFieldAddress:
      case okReadOnlyFieldAddress:
         if (info.param > 0 || (_logic->defineStructSize(scope, info.reference, 0) & 3) != 0) {
            return true;
         }
      default:
         return false;
   }
}

bool Compiler :: boxingRequired(ObjectInfo& info)
{
   switch (info.kind) {
      case okLocalAddress:
      case okTempLocalAddress:
      case okFieldAddress:
      case okBoxableLocal:
         return true;
      case okParam:
      case okSelfParam:
      case okSuper:
         return info.extraparam == -1;
      case okParams:
         return info.reference != V_UNBOXEDARGS;
      default:
         return false;
   }
}

inline bool Compiler :: condBoxingRequired(ObjectInfo& info)
{
   switch (info.kind) {
      case okParam:
      case okSelfParam:
      case okSuper:
         return info.extraparam == -1;
      default:
         return false;
   }
}

ObjectInfo Compiler :: compileAssigning(SyntaxWriter& writer, SNode current, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   LexicalType operationType = lxAssigning;
   int operand = 0;

   //   EAttr assignMode = HINT_NOUNBOXING/* | HINT_ASSIGNING_EXPR*/;
   ref_t targetRef = resolveObjectReference(scope, target, false, false);
   EAttr assignMode = HINT_PARAMETER/*|| HINT_NOUNBOXING*//* | HINT_ASSIGNING_EXPR*/;
   bool noBoxing = false;
   ////   bool byRefAssigning = false;

   switch (target.kind) {
      case okLocal:
         scope.markAsAssigned(target);
      case okField:
      case okStaticField:
         //      case okClassStaticField:
      case okOuterField:
         //      case okOuterStaticField:
         break;
      case okLocalAddress:
         scope.markAsAssigned(target);
      case okFieldAddress:
      {
         int size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
         if (size != 0) {
            noBoxing = true;
            operationType = lxCopying;
            operand = size;
         }
         else scope.raiseError(errInvalidOperation, current);
         //assignMode = assignMode | HINT_NOBOXING;
         break;
      }
      case okOuter:
         //case okOuterSelf:
      {
         InlineClassScope* closure = (InlineClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
         //MethodScope* method = (MethodScope*)scope.getScope(Scope::slMethod);

         if (/*!method->subCodeMode || */!closure->markAsPresaved(target))
            scope.raiseError(errInvalidOperation, current.parentNode());

         int size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
         if (size != 0 && target.kind == okOuter) {
            operand = size;
            //byRefAssigning = true;
         }
         break;
      }
      case okReadOnlyField:
      case okReadOnlyFieldAddress:
      case okOuterReadOnlyField:
         scope.raiseError(errReadOnlyField, current.parentNode());
         break;
      case okParam:
         if (targetRef == V_WRAPPER) {
            //byRefAssigning = true;
            targetRef = target.element;
            int size = _logic->defineStructSize(*scope.moduleScope, targetRef, 0u);
            if (size != 0) {
               operand = size;
               operationType = lxCopying;
               noBoxing = true;
            }
            else operationType = lxByRefAssigning;

            break;
         }
      default:
         scope.raiseError(errInvalidOperation, current.parentNode());
         break;
   }

   ObjectInfo exprVal;

   if (targetRef == V_AUTO) {
      // support auto attribute
      exprVal = compileExpression(writer, current, scope, 0, assignMode, nullptr);

      if (resolveAutoType(exprVal, target, scope)) {
         targetRef = resolveObjectReference(scope, exprVal, false);
         target.reference = targetRef;
      }
      else scope.raiseError(errInvalidOperation, current.parentNode());
   }
   //   else if (sourceNode == lxYieldContext) {
   //      int size = scope.getAttribute(sourceNode.argument, maYieldContextLength);
   //
   //      sourceNode.set(lxCreatingStruct, size << 2);
   //
   //      node.set(lxAssigning, 0);
   //   }
   //   else if (sourceNode == lxYieldLocals) {
   //      int len = scope.getAttribute(sourceNode.argument, maYieldLocalLength);
   //      if (len != 0) {
   //         sourceNode.set(lxCreatingClass, len);
   //
   //         node.set(lxAssigning, 0);
   //      }
   //      else operationType = lxIdle;
   //   }
   else exprVal = compileExpression(writer, current, scope, targetRef, assignMode, nullptr);

   if (!noBoxing && boxingRequired(exprVal)) {
      exprVal = boxArgumentInPlace(writer, current.parentNode(), exprVal, scope, false, condBoxingRequired(exprVal));
   }

   writer.newNode(operationType, operand);
   writeTerminal(writer, target, scope);
   writeTerminal(writer, exprVal, scope);
   writer.closeNode();

   return target;
}

ObjectInfo Compiler :: compileAssigningExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode)
{
   SNode current = node.firstChild();
   if (current == lxPropertyExpression) {
      return compilePropAssigning(writer, node, scope, mode);
   }
   else if (current == lxArrayExpression) {
      return compileArrAssigning(writer, node, scope, mode);
   }
   else {
      //   if (accumulateMode)
      //      // !! temporally
      //      scope.raiseError(errInvalidOperation, sourceNode);
      //
      ObjectInfo target = compileObject(writer, current, scope,
         mode | HINT_PARAMETER | HINT_NODEBUGINFO | HINT_ASSIGNTARGET, nullptr);

      current = current.nextNode();
      return compileAssigning(writer, current, scope, target, mode);
   }
}

mssg_t Compiler :: resolveVariadicMessage(Scope& scope, mssg_t message)
{
   pos_t argCount = 0;
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

mssg_t Compiler :: resolveMessageAtCompileTime(ObjectInfo target, ExprScope& scope, mssg_t generalMessageRef, ref_t implicitSignatureRef,
   bool withExtension, int& stackSafeAttr, ref_t& resolvedExtensionRef)
{
   int resolvedStackSafeAttr = 0;
   mssg_t resolvedMessageRef = 0;
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
         resolvedExtensionRef = extensionRef;

         return resolvedMessageRef;
      }

      // check if the extension handles the variadic message
      mssg_t variadicMessage = resolveVariadicMessage(scope, generalMessageRef);

      resolvedStackSafeAttr = 0;
      extensionRef = mapExtension(scope, variadicMessage, implicitSignatureRef, target, resolvedStackSafeAttr);
      if (extensionRef != 0) {
         stackSafeAttr = resolvedStackSafeAttr;

         // if there is an extension to handle the compile-time resolved message - use it
         resolvedExtensionRef = extensionRef;

         return variadicMessage;
      }
   }

   // otherwise - use the general message
   return generalMessageRef;
}

ObjectInfo Compiler :: compileMessageOperation(SyntaxWriter& writer, SNode current, SNode opNode, ExprScope& scope, ObjectInfo target,
   mssg_t messageRef, ref_t expectedSignRef, EAttr paramsMode, ArgumentsInfo* presavedArgs, ref_t expectedRef)
{
   ObjectInfo retVal;
   bool variadicOne = false;
   //   bool inlineArg = false;
   ArgumentsInfo arguments;
   ref_t implicitSignatureRef = compileMessageParameters(writer, current, scope, paramsMode, expectedSignRef, variadicOne, /*inlineArg, */
      arguments, presavedArgs);

   //   bool externalMode = false
   if (target.kind == okExternal) {
   //      EAttr extMode = mode & HINT_ROOT;

      retVal = compileExternalCall(writer, opNode, scope, expectedRef, &arguments);
   }
   else {
      EAttr mode = paramsMode & HINT_SILENT;
      if (variadicOne)
         // HOTFIX : set variadic flag if required
         messageRef |= VARIADIC_MESSAGE;

      if (target.kind == okInternal) {
         retVal = compileInternalCall(writer, opNode, scope, messageRef, implicitSignatureRef, target,
            &arguments);
      }
//      else if (inlineArg) {
//         SNode parentNode = node.parentNode();
//         bool dummy = false;
//         retVal = compileMessage(parentNode, scope, target, messageRef, mode | HINT_INLINEARGMODE, 0, dummy);
//      }
      else {
         int stackSafeAttr = 0;
         if (!EAttrs::test(mode, HINT_DIRECTCALL)) {
            ref_t extensionRef = 0;
            messageRef = resolveMessageAtCompileTime(target, scope, messageRef, implicitSignatureRef, true,
                           stackSafeAttr, extensionRef);

            if (extensionRef) {
               // if extension was found - make it a operation target
               arguments.insert(target);
               target = ObjectInfo(okConstantRole, extensionRef, extensionRef);
            }
         }

         if (!test(stackSafeAttr, 1)) {
            mode = mode | HINT_DYNAMIC_OBJECT;
         }
         else if (target.kind != okConstantRole)
            stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

         retVal = compileMessage(writer, opNode, scope, target, messageRef, &arguments, mode, stackSafeAttr,
            expectedRef, presavedArgs);
      }
   }

   return retVal;
}

ObjectInfo Compiler :: compileResendMessageOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target,
   ref_t expectedRef, EAttr mode)
{
   ArgumentsInfo presavedArgs;

   ref_t expectedSignRef = 0; // contains the expected message signature if it there is only single method overloading
   mssg_t messageRef = mapMessage(node.parentNode(), scope, target.kind == okExtension, false, false);

   mssg_t resolvedMessage = _logic->resolveSingleMultiDisp(*scope.moduleScope,
      resolveObjectReference(scope, target, false), messageRef);

   if (resolvedMessage)
      scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

   ObjectInfo retVal = compileMessageOperation(writer, node, node, scope, target, messageRef, expectedSignRef,
      EAttr::eaNone, &presavedArgs, expectedRef);

   if (EAttrs::test(mode, HINT_PARAMETER)) {
      if (retVal.kind != okTempLocal)
         retVal = saveToTempLocal(writer, scope, retVal);
   }
   else if (retVal.kind == okTempLocal)
      writeTerminal(writer, retVal, scope);

   return retVal;
}

ObjectInfo Compiler :: saveToTempLocal(SyntaxWriter& writer, ExprScope& scope, ObjectInfo retVal)
{
   int tempLocal = scope.newTempLocal();
   writer.newNode(lxAssigning);
   writer.appendNode(lxTempLocal, tempLocal);
   writer.appendNode(lxResult);
   writer.closeNode();

   return ObjectInfo(okTempLocal, tempLocal, resolveObjectReference(scope, retVal, false));
}

ObjectInfo Compiler :: compileMessageExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t expectedRef, EAttr mode)
{
   ArgumentsInfo presavedArgs;

   SNode current = node.firstChild();

   bool propMode = EAttrs::test(mode, HINT_PROP_MODE);
   bool silentMode = EAttrs::test(mode, HINT_SILENT);
   bool directMode = EAttrs::test(mode, HINT_DIRECTCALL);

   EAttrs objMode(mode | HINT_TARGET, HINT_PROP_MODE);
   ObjectInfo target = compileObject(writer, current, scope, objMode, &presavedArgs);

   ref_t expectedSignRef = 0; // contains the expected message signature if it there is only single method overloading
   mssg_t messageRef = 0;
   EAttr paramsMode = silentMode ? HINT_SILENT : EAttr::eaNone;
   if (target.kind == okNewOp) {
      if (target.reference == V_OBJARRAY) {
         target.kind = okClass;

         return compileNewArrOperation(writer, current, scope, target, expectedRef, mode);
      }
      else {
         messageRef = mapMessage(node, scope, false, true, propMode);
         if (getAction(messageRef) == getAction(scope.moduleScope->constructor_message)) {
            target.kind = okClass; 
         }
         else scope.raiseError(errInvalidOperation, node);
      }
   }
   else if (target.kind == okCastOp) {
      target.kind = okClass;

      return compileCastingExpression(writer, current, scope, target, EAttrs::exclude(mode, EAttr::eaTargetExpr));
   }
   else {
      current = current.nextNode(lxObjectMask);

      if (target.kind != okExternal) {
         messageRef = mapMessage(node, scope, target.kind == okExtension, false, propMode);

         mssg_t resolvedMessage = _logic->resolveSingleMultiDisp(*scope.moduleScope,
            resolveObjectReference(scope, target, false), messageRef);

         if (resolvedMessage)
            scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);
      }
      else paramsMode = paramsMode | HINT_EXTERNALOP;
   }

   if (directMode)
      paramsMode = paramsMode | HINT_DIRECTCALL;

   ObjectInfo retVal = compileMessageOperation(writer, current, node, scope, target, messageRef, expectedSignRef,
      paramsMode, &presavedArgs, expectedRef);

   return retVal;
}

//void Compiler :: inheritClassConstantList(_ModuleScope& scope, ref_t sourceRef, ref_t targetRef)
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

inline SNode findBookmarkOwner(SNode node, ref_t bookmark)
{
   while (!node.compare(lxClass, /*lxNestedClass, */lxNone))
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

void Compiler :: compileMetaConstantAssigning(ObjectInfo, SNode node, int bm, ClassScope& scope)
{
   ExprScope exprScope(&scope);

   ObjectInfo source = mapTarget(node, exprScope);
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

      if (targetNode == lxStaticMethod) {
         // HOTFIX : recognize class class meta info
         ClassScope classClassScope(&scope, scope.info.header.classRef, scope.visibility);
         scope.moduleScope->loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);

         classClassScope.info.mattributes.add(key, saveMetaInfo(*scope.moduleScope, info));
         classClassScope.save();
      }
      else {
         scope.info.mattributes.add(key, saveMetaInfo(*scope.moduleScope, info));
         scope.save();
      }
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

//inline bool isInherited(_Module* module, ref_t reference, ref_t staticRef)
//{
//   ident_t name = module->resolveReference(reference);
//   ident_t statName = module->resolveReference(staticRef);
//   size_t len = getlength(name);
//
//   if (statName[len] == '#' && statName.compare(name, 0, len)) {
//      return true;
//   }
//   else return false;
//}

void Compiler :: compileClassConstantAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, bool accumulatorMode*/)
{
   ref_t valueRef = scope.info.staticValues.get(target.param);

   //if (accumulatorMode) {
   //   // HOTFIX : inherit accumulating attribute list
   //   ClassInfo parentInfo;
   //   scope.moduleScope->loadClassInfo(parentInfo, scope.info.header.parentRef);
   //   ref_t targtListRef = valueRef & ~mskAnyRef;
   //   ref_t parentListRef = parentInfo.staticValues.get(target.param) & ~mskAnyRef;

   //   if (parentListRef != 0 && !isInherited(scope.module, scope.reference, targtListRef)) {
   //      valueRef = mapStaticField(scope.moduleScope, scope.reference, true);
   //      scope.info.staticValues.exclude(target.param);
   //      scope.info.staticValues.add(target.param, valueRef);
   //      scope.save();

   //      targtListRef = valueRef & ~mskAnyRef;

   //      // inherit the parent list
   //      inheritClassConstantList(*scope.moduleScope, parentListRef, targtListRef);
   //   }
   //}

   SymbolScope constantScope((NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace), valueRef & ~mskAnyRef, Visibility::Public);
   ExprScope exprScope(&constantScope);

   ObjectInfo source = mapTerminal(node, exprScope, EAttr::eaNone);
   ref_t targetRef = /*accumulatorMode ? target.element : */target.reference;
   //if (accumulatorMode && !targetRef)
   //   targetRef = _logic->resolveArrayElement(*scope.moduleScope, target.reference);

   ref_t sourceRef = resolveConstantObjectReference(scope, source);
   if (isPrimitiveRef(sourceRef))
      sourceRef = resolvePrimitiveReference(scope, sourceRef, source.element, false);

   if (compileSymbolConstant(/*node, */constantScope, source/*, accumulatorMode, target.reference*/)
      && _logic->isCompatible(*scope.moduleScope, targetRef, sourceRef, false))
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
   //SNode assignNode = node.findChild(lxAssign);
   //if (assignNode != lxNone) {
   //   SNode exprNode = assignNode.nextNode().findSubNode(lxMetaConstant);
   //   if (exprNode != lxNone) {
   //      if (exprNode.identifier().compare(SUBJECT_VAR)) {
   //         ref_t subjRef = resolveSubjectVar(node);
   //         if (subjRef) {
   //            exprNode.set(lxSubjectRef, subjRef);
   //         }
   //      }

   //   }
   //}
}

bool Compiler :: recognizeCompileTimeAssigning(SNode node, ClassScope& scope)
{
   bool idle = true;
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxFieldInit) {
         ref_t dummy = 0;
         bool dummy2 = false;

         SNode exprNode = current.firstChild(lxObjectMask);
         SNode identNode = exprNode.firstChild();
         //if (identNode == lxBookmarkReference) {
         //   resolveMetaConstant(current);
         //}

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
   SNode exprNode = node.firstChild(lxObjectMask);
   SNode sourceNode = exprNode.firstChild(lxObjectMask);
   //bool accumulateMode = assignNode.argument == INVALID_REF;

   ExprScope scope(&classScope);

   ObjectInfo target = mapTarget(exprNode, scope);

   // HOTFIX : recognize static field initializer
   if (target.kind == okStaticField || target.kind == okStaticConstantField || target.kind == okMetaField) {
      if (target.kind == okMetaField) {
         compileMetaConstantAssigning(target, sourceNode, node.findChild(lxBookmarkReference).argument,
            *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass)));
      }
      else if (!isSealedStaticField(target.param) && target.kind == okStaticConstantField) {
         // HOTFIX : static field initializer should be compiled as preloaded symbol
         compileClassConstantAssigning(target, sourceNode, *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass))/*, accumulateMode*/);
      }
      else compileStaticAssigning(node, *((ClassScope*)scope.getScope(Scope::ScopeLevel::slClass)));
   }
}

ObjectInfo Compiler :: compileArrAssigning(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode)
{
   SNode opNode = node.firstChild();

   SNode lnode = opNode.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);
   SNode rnode2 = opNode.nextNode(lxObjectMask);

   EAttrs objMode(mode | HINT_TARGET/*, HINT_PROP_MODE*/);
   ObjectInfo loperand = compileObject(writer, lnode, scope, objMode, nullptr);

   ObjectInfo roperand = compileExpression(writer, rnode, scope, 0, HINT_PARAMETER, nullptr);
   ObjectInfo roperand2 = compileExpression(writer, rnode2, scope, 0, HINT_PARAMETER, nullptr);

   ArgumentsInfo arguments;
   arguments.add(roperand);
   arguments.add(roperand2);

   return compileOperation(writer, node, scope, SET_REFER_OPERATOR_ID, loperand, &arguments, false, false, 0);
}

ObjectInfo Compiler :: compilePropAssigning(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode)
{
   SNode current = node.firstChild();

   // tranfer the message into the property set one
   mssg_t messageRef = mapMessage(current, scope, false, false, true);
   ref_t actionRef, flags;
   pos_t argCount;
   decodeMessage(messageRef, actionRef, argCount, flags);
   if (argCount == 1 && (flags & PREFIX_MESSAGE_MASK) == PROPERTY_MESSAGE) {
      messageRef = encodeMessage(actionRef, 2, flags);
   }
   else scope.raiseError(errInvalidOperation, node);

   ref_t expectedSignRef = 0;
   EAttrs objMode(HINT_TARGET | HINT_ASSIGNTARGET);
   SNode objNode = current.firstChild();
   ObjectInfo target = compileObject(writer, objNode, scope, objMode, nullptr);

   current = current.nextNode(lxObjectMask);

   mssg_t resolvedMessage = _logic->resolveSingleMultiDisp(*scope.moduleScope,
      resolveObjectReference(scope, target, false), messageRef);

   if (resolvedMessage)
      scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

   ObjectInfo retVal = compileMessageOperation(writer, current, node, scope, target, messageRef, expectedSignRef, EAttr::eaNone,
      nullptr, 0);

   if (EAttrs::test(mode, HINT_PARAMETER)) {
      if (retVal.kind != okTempLocal)
         retVal = saveToTempLocal(writer, scope, retVal);
   }
   else if (retVal.kind == okTempLocal)
      writeTerminal(writer, retVal, scope);

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

//inline void copyMethodParameters(SNode target, SNode arg)
//{
//   while (arg != lxNone) {
//      if (arg == lxMethodParameter) {
//         SNode paramNode = target.insertNode(lxMethodParameter).appendNode(lxNameAttr);
//
//         SyntaxTree::copyNode(arg, paramNode);
//      }
//
//      arg = arg.nextNode();
//   }
//}

void Compiler :: compileAction(SNode node, ClassScope& scope, SNode argNode, EAttr mode)
{
   SyntaxTree buffer;
   SyntaxWriter writer(buffer);

   writer.newNode(lxRoot);
   writer.newNode(lxIdle);
   SNode tempNode = writer.CurrentNode();
   writer.closeNode();

   writer.newNode(lxClass, scope.reference);

   MethodScope methodScope(&scope);
   bool lazyExpression = declareActionScope(scope, argNode, methodScope, mode);
   bool inlineExpression = EAttrs::test(mode, HINT_INLINE_EXPR);
   methodScope.functionMode = true;

   mssg_t multiMethod = resolveMultimethod(scope, methodScope.message);

//   // HOTFIX : if the closure emulates code brackets
//   if (EAttrs::test(mode, HINT_SUBCODE_CLOSURE))
//      methodScope.subCodeMode = true;

   // if it is single expression
   if (inlineExpression || lazyExpression) {
      compileExpressionMethod(writer, node, methodScope, lazyExpression);
   }
   else {
      // inject a method
//      //!!HOTFIX : copy method parameters to be used for debug info
//      copyMethodParameters(current, argNode);

      initialize(scope, methodScope);
      methodScope.functionMode = true;

      if (multiMethod)
         // if it is a strong-typed closure, output should be defined by the closure
         methodScope.outputRef = V_AUTO;

      compileActionMethod(writer, node, methodScope);

      // HOTFIX : inject an output type if required or used super class
      if (methodScope.outputRef == V_AUTO) {
         methodScope.outputRef = scope.moduleScope->superReference;
      }
   }

   // the parent is defined after the closure compilation to define correctly the output type
   ref_t parentRef = scope.info.header.parentRef;
//   if (lazyExpression) {
//      parentRef = scope.moduleScope->lazyExprReference;
//   }
//   else {
      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);

      ref_t closureRef = scope.moduleScope->resolveClosure(methodScope.message, methodScope.outputRef, nsScope->nsName);
      if (closureRef) {
         parentRef = closureRef;
      }
      else throw InternalError(errClosureError);
//   }

   // NOTE : the fields are presaved, so the closure parent should be stateless
   compileParentDeclaration(SNode(), scope, parentRef, true);

   // set the message output if available
   if (methodScope.outputRef)
      scope.addAttribute(methodScope.message, maReference, methodScope.outputRef);

   if (multiMethod) {
      Attribute attr(methodScope.message, maMultimethod);
      scope.info.methodHints.exclude(attr);
      scope.info.methodHints.add(attr, multiMethod);

      scope.addHint(multiMethod, tpMultimethod);

      // inject a virtual invoke multi-method if required
      List<mssg_t> implicitMultimethods;
      implicitMultimethods.add(multiMethod);

      _logic->injectVirtualMultimethods(*scope.moduleScope, tempNode, *this, implicitMultimethods, lxClassMethod, scope.info);

      // HOTFIX : inject a method to be declared
      SNode methodDummy = tempNode.appendNode(lxClassMethod, methodScope.message);
      if (methodScope.outputRef)
         methodDummy.appendNode(lxType, methodScope.outputRef);

      generateMethodDeclaration(methodDummy, scope, false, test(scope.info.header.flags, elClosed), true);
      // comment out the inject method to prevent double compilation
      methodDummy = lxIdle;

      generateClassDeclaration(tempNode, scope);

      compileVMT(writer, tempNode, scope);
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

   writer.closeNode();
   writer.closeNode();

   generateClassImplementation(buffer.readRoot().findChild(lxClass), scope);
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
      //else if (current == lxType) {
      //   current.injectAndReplaceNode(lxParent);
      //}
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

   compileParentDeclaration(node.findChild(lxType), scope, false, lxType);

   if (scope.abstractBaseMode && test(scope.info.header.flags, elClosed | elNoCustomDispatcher)
      && _logic->isWithEmbeddableDispatcher(*scope.moduleScope, node))
   {
      // COMPILER MAGIC : inject interface implementation
      _logic->injectInterfaceDispatch(*scope.moduleScope, *this, node, scope.info.header.parentRef);
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

   SyntaxTree buffer;
   SyntaxWriter writer(buffer);

   writer.newNode(lxClass, scope.reference);
   // NOTE : ignore auto generated methods - multi methods should be compiled after the class flags are set
   compileVMT(writer, node, scope, true, true);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(*scope.moduleScope, *this, scope.reference, scope.info, false);

   // NOTE : compile once again only auto generated methods
   compileVMT(writer, node, scope, true, false);
   writer.closeNode();

   scope.save();

   generateClassImplementation(buffer.readRoot(), scope);
}

ref_t Compiler :: resolveMessageOwnerReference(_ModuleScope& scope, ClassInfo& classInfo, ref_t reference, mssg_t message,
   bool ignoreSelf)
{
   if (!classInfo.methods.exist(message, true) || ignoreSelf) {
      ClassInfo parentInfo;
      _logic->defineClassInfo(scope, parentInfo, classInfo.header.parentRef, false);

      return resolveMessageOwnerReference(scope, parentInfo, classInfo.header.parentRef, message);
   }
   else return reference;
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, ExprScope& ownerScope, InlineClassScope& scope,
   EAttr mode, ArgumentsInfo* preservedArgs)
{
//   bool noUnboxing = EAttrs::test(mode, HINT_NOUNBOXING);
   bool paramMode = EAttrs::test(mode, HINT_PARAMETER);
   ref_t closureRef = scope.reference;
   if (test(scope.info.header.flags, elVirtualVMT))
      closureRef = scope.info.header.parentRef;

   if (test(scope.info.header.flags, elStateless)) {
      ObjectInfo retVal = ObjectInfo(okSingleton, closureRef, closureRef);

      if (!paramMode) {
         writeTerminal(writer, retVal, ownerScope);
      }

      // if it is a stateless class
      return retVal;
   }
   else if (test(scope.info.header.flags, elDynamicRole)) {
      scope.raiseError(errInvalidInlineClass, node);

      // idle return
      return ObjectInfo();
   }
   else {
      ObjectInfo retVal(okObject, 0, closureRef);

      ArgumentsInfo members;
      // first pass : box an argument if required
      for (auto outer_it = scope.outers.start(); !outer_it.Eof(); outer_it++) {
         ObjectInfo info = (*outer_it).outerObject;
         if (boxingRequired(info)) {
            boxArgument(writer, node, info, ownerScope, false);
            members.add(info);
         }
      }

      writer.newNode(lxInitializing);

      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         writer.newNode(lxCreatingStruct, scope.info.size);
         writer.appendNode(lxType, closureRef);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node);
      }
      else {
         // dynamic normal symbol
         writer.newNode(lxCreatingClass, scope.info.fields.Count());
         writer.appendNode(lxType, closureRef);
      }
      writer.closeNode();

      // second pass : fill members
      int memberIndex = 0;
      int preservedClosure = 0;
      for (auto outer_it = scope.outers.start(); !outer_it.Eof(); outer_it++) {
         ObjectInfo info = (*outer_it).outerObject;
         writer.newNode(lxMember, (*outer_it).reference);

         if (boxingRequired(info)) {
            writeTerminal(writer, members[memberIndex], ownerScope);
            memberIndex++;
         }
         else writeTerminal(writer, info, ownerScope);

         if ((*outer_it).preserved && preservedArgs) {
            if (!preservedClosure) {
               preservedArgs->add(ObjectInfo(okClosureInfo));
               // reserve place for the closure
               preservedClosure = preservedArgs->Length();
               preservedArgs->add(ObjectInfo(okUnknown));
            }

            preservedArgs->add(ObjectInfo(okMemberInfo, (*outer_it).reference));
            preservedArgs->add(info);

            //   if (!tempLocal) {
            //      // save the closure in the temp variable
            //      tempLocal = ownerScope.newTempLocal();

            //      node.injectAndReplaceNode(lxAssigning);
            //      node.insertNode(lxTempLocal, tempLocal);
            //      node.injectAndReplaceNode(lxSeqExpression);
            //      node.appendNode(lxTempLocal, tempLocal);

            //      // hotfix : find initializing node again
            //      node = member.parentNode();
            //}

            //   // HOTFIX : save original to prevent double unboxing
            //   int oriTempLocal = info.kind == okLocal ? ownerScope.originals.get(info.param) : 0;
            //   bool hasToBeSaved = oriTempLocal == -1;

            //   if (hasToBeSaved) {
            //      ownerScope.originals.erase(info.param);
            //      ownerScope.originals.add(info.param, oriTempLocal);
            //   }
            //   else if (!oriTempLocal && info.kind == okLocal) {
            //      ownerScope.originals.add(info.param, -1);
            //   }
         }

         writer.closeNode();
      }

      if (scope.info.methods.exist(scope.moduleScope->init_message)) {
         ref_t messageOwnerRef = resolveMessageOwnerReference(*scope.moduleScope, scope.info, scope.reference,
                                    scope.moduleScope->init_message);

         // if implicit constructor is declared - it should be automatically called
         writer.CurrentNode().injectAndReplaceNode(lxDirectCalling, scope.moduleScope->init_message);
         writer.appendNode(lxCallTarget, messageOwnerRef);
      }

      writer.closeNode();

      if (paramMode || preservedClosure) {
         retVal.kind = okTempLocal;
         retVal.reference = closureRef;
         retVal.param = ownerScope.newTempLocal();
         writer.newNode(lxAssigning);
         writer.appendNode(lxTempLocal, retVal.param);
         writer.appendNode(lxResult);
         writer.closeNode();

         if (preservedClosure)
            (*preservedArgs)[preservedClosure] = retVal;
      }

      return retVal;
   }
}

ObjectInfo Compiler :: compileClosure(SyntaxWriter& writer, SNode node, ExprScope& ownerScope,
   EAttr mode, ArgumentsInfo* preservedArgs)
{
   ref_t nestedRef = 0;
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
   if (argNode == lxExpression)
      argNode = argNode.firstChild();

   if (EAttrs::testany(mode, HINT_LAZY_EXPR | HINT_INLINE_EXPR)) {
      compileAction(node, scope, SNode(), mode);
   }
   else if (argNode == lxCode) {
      compileAction(node, scope, SNode(), mode);
   }
   else if (node.existSubChild(lxCode, lxReturning)) {
//      //SNode codeNode = node.findChild(lxCode, lxReturning);

      // if it is a closure / lambda function with a parameter
      EAttr actionMode = mode;
      compileAction(node, scope, node.findChild(lxIdentifier, lxType, lxArrayType/*, lxMethodParameter*/), actionMode);
//
////      // HOTFIX : hide code node because it is no longer required
////      codeNode = lxIdle;
   }
   // if it is a nested class
   else if (argNode == lxClass)
      compileNestedVMT(argNode, scope);

   return compileClosure(writer, node, ownerScope, scope, mode, preservedArgs);
}

bool Compiler :: isConstantList(ArgumentsInfo& members)
{
   for (pos_t i = 0; i != members.Length(); i++) {
      ObjectInfo info = members[i];

      switch (info.kind) {
         case okClass:
         case okLiteralConstant:
         case okWideLiteralConstant:
         case okCharConstant:
         case okIntConstant:
         case okUIntConstant:
         case okLongConstant:
         case okRealConstant:
         case okSingleton:
            break;
         default:
            return false;
      }
   }

   return true;
}

ObjectInfo Compiler :: compileCollection(SyntaxWriter& writer, SNode node, ExprScope& scope/*, ObjectInfo target*/, EAttr mode)
{
   ObjectInfo target;

   SNode current = node.firstChild();
   ObjectInfo typeInfo = compileObject(writer, current, scope, HINT_TARGET, nullptr);
   if (typeInfo.kind == okNewOp) {
      if (typeInfo.reference == V_OBJARRAY) {
         typeInfo.reference = resolvePrimitiveArray(scope, scope.moduleScope->arrayTemplateReference, typeInfo.element, false);
      }
      else scope.raiseError(errInvalidOperation, node);
   }
   else scope.raiseError(errInvalidOperation, node);

   int counter = 0;
   int size = _logic->defineStructSize(*scope.moduleScope, typeInfo.reference, 0);

   ArgumentsInfo members;
   current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      ObjectInfo member = compileExpression(writer, current, scope, 0, HINT_PARAMETER, nullptr);
      members.add(member);
      counter++;

      current = current.nextNode(lxObjectMask);
   }

   if (size >= 0 && EAttrs::test(mode, HINT_CONSTEXPR) && isConstantList(members)) {
      ref_t reference = 0;
      SymbolScope* owner = (SymbolScope*)scope.getScope(Scope::ScopeLevel::slSymbol);
      if (owner) {
         reference = mapConstant(scope.moduleScope, owner->reference);
      }
      else reference = scope.moduleScope->mapAnonymous();

      target.kind = okArrayConst;
      target.param = reference;
      target.reference = typeInfo.reference;

      writer.newNode(lxConstantList, reference | mskConstArray);
      writer.appendNode(lxType, target.reference);
      
      int index = 0;
      for (pos_t i = 0; i != members.Length(); i++) {
         writer.newNode(lxMember, index++);
         writeTerminal(writer, members[i], scope);
         writer.closeNode();
      }

      _writer.generateConstantList(writer.CurrentNode(), scope.module, reference);

      writer.closeNode();
   }
   else {
      writer.newNode(lxInitializing);

      target.kind = okObject;
      target.reference = typeInfo.reference;
      if (size < 0) {
         writer.newNode(lxCreatingStruct, counter * (-size));
         writer.appendNode(lxType, target.reference);
         writer.appendNode(lxSize, -size);
         writer.closeNode();
      }
      else {
         writer.newNode(lxCreatingClass, counter);
         writer.appendNode(lxType, target.reference);
         writer.closeNode();
      }

      int index = 0;
      for (pos_t i = 0; i != members.Length(); i++) {
         writer.newNode(lxMember, index++);
         writeTerminal(writer, members[i], scope);
         writer.closeNode();
      }
      writer.closeNode();
   }

   return target;
}

ObjectInfo Compiler :: compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode)
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

   writer.newNode(lxSeqExpression);

   ExprScope exprScope(&scope);
   ObjectInfo retVal = compileExpression(writer, node.firstChild(), exprScope, targetRef,
      mode | HINT_PARAMETER | HINT_ROOT, nullptr);

   if (autoMode) {
      targetRef = resolveObjectReference(exprScope, retVal, true);

     _logic->validateAutoType(*scope.moduleScope, targetRef);

      scope.resolveAutoOutput(targetRef);
   }

////   // HOTFIX : implementing closure exit
////   if (EAttrs::test(mode, HINT_ROOT)) {
////      ObjectInfo retVar = scope.mapTerminal(RETVAL_VAR, false, EAttr::eaNone);
////      if (retVar.kind != okUnknown) {
////         writer.inject(lxAssigning);
////         writer.insertNode(lxField, retVar.param);
////         writer.closeNode();
////      }
////   }

   if ((retVal.kind != okSelfParam || EAttrs::test(mode, HINT_DYNAMIC_OBJECT)) && boxingRequired(retVal)) {
      retVal = boxArgumentInPlace(writer, node, retVal, exprScope, false, condBoxingRequired(retVal));
   }

   writer.closeNode();

   writer.newNode(lxReturning);
   writeTerminal(writer, retVal, exprScope);
   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileCatchOperator(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t operator_id)
{
   writer.newNode(lxTrying, 0);

   SNode lnode = node.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);

   writer.newNode(lxSeqExpression);
   ObjectInfo loperand = compileObject(writer, lnode, scope, EAttr::eaNone, nullptr);
   writer.closeNode();

   if (operator_id == FINALLY_OPERATOR_ID) {
      writer.newNode(lxFinalblock);

      writer.newNode(lxSeqExpression);
      compileExpression(writer, rnode, scope, 0, EAttr::eaNone, nullptr);
      writer.closeNode();

      // catch operation follow the finally operation
      rnode = rnode.nextNode(lxObjectMask);

      writer.closeNode();
   }

   writer.newNode(lxSeqExpression);

   ObjectInfo info = saveToTempLocal(writer, scope, ObjectInfo(okObject));

   if (rnode == lxExpression)
      rnode = rnode.findSubNodeMask(lxObjectMask);

   ObjectInfo retVal = compileResendMessageOperation(writer, rnode.firstChild(), scope, info, 0, EAttr::eaNone);

   writer.closeNode();
   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileAltOperator(SyntaxWriter& writer, SNode node, ExprScope& scope)
{
   writer.newNode(lxAlt, 0);

   SNode lnode = node.firstChild();
   SNode rnode = lnode.nextNode(lxObjectMask);
   if (lnode == lxExpression)
      lnode = lnode.firstChild();
   if (rnode == lxExpression)
      rnode = rnode.firstChild();

   ObjectInfo loperand;
   if (lnode == lxMessageExpression) {
      SNode objectNode = lnode.firstChild();
      loperand = compileObject(writer, objectNode, scope, HINT_PARAMETER, nullptr);

      writer.newNode(lxExpression);
      compileResendMessageOperation(writer, objectNode.nextNode(), scope, 
         loperand, 0, EAttr::eaNone);
      writer.closeNode();
   }
   // !! temporal terminator
   else scope.raiseError(errInvalidOperation, node);

   writer.newNode(lxExpression);
   compileResendMessageOperation(writer, rnode.firstChild(), scope, loperand, 0, EAttr::eaNone);
   writer.closeNode();

   writer.closeNode();

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

ObjectInfo Compiler :: compileReferenceExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, EAttr mode)
{
   bool paramMode = EAttrs::test(mode, HINT_PARAMETER);

   ObjectInfo retVal;
   ObjectInfo objectInfo = compileObject(writer, node, scope, HINT_PARAMETER | HINT_REFEXPR, nullptr);
//   if (node == lxTempLocal) {
//      // HOTFIX : to support return value dispatching
//      objectInfo.kind = okLocal;
//      objectInfo.param = node.argument;
//      objectInfo.reference = node.findChild(lxType).argument;
//   }

   ref_t operandRef = resolveObjectReference(scope, objectInfo, true, false);
   if (!operandRef)
      operandRef = scope.moduleScope->superReference;

   ref_t targetRef = 0;
   if (objectInfo.reference == V_WRAPPER) {
      // if the reference is passed further - do nothing
      retVal = objectInfo;
      retVal.reference = operandRef;
   }
   else {
      // generate an reference class
      targetRef = resolveReferenceTemplate(scope, operandRef, false);

      retVal = convertObject(writer, node, scope, targetRef, objectInfo, mode);

      if (retVal.kind == okUnknown)
         scope.raiseError(errInvalidOperation, node);
   }

   if (!paramMode) {
      writeTerminal(writer, retVal, scope);
   }

   return retVal;
}

ObjectInfo Compiler :: compileVariadicUnboxing(SNode node, ExprScope& scope, EAttr mode)
{
   ObjectInfo objectInfo = mapTerminal(node, scope, mode);
   ref_t operandRef = resolveObjectReference(scope, objectInfo, false, false);
   if (operandRef == V_ARGARRAY && EAttrs::test(mode, HINT_PARAMETER)) {
      objectInfo.reference = V_UNBOXEDARGS;
   }
//   else if (_logic->isArray(*scope.moduleScope, operandRef)) {
//      objectInfo.reference = V_UNBOXEDARGS;
//
//      node.injectAndReplaceNode(lxArgArray, (ref_t)-1);
//   }
   else scope.raiseError(errNotApplicable, node);

   return objectInfo;
}

ObjectInfo Compiler :: compileCastingExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo target, EAttr mode)
{
   ref_t targetRef = 0;
   if (target.kind == okClass || target.kind == okClassSelf) {
      targetRef = target.param;
   }
   else targetRef = resolveObjectReference(scope, target, true);

   ObjectInfo retVal(okObject, 0, targetRef);

   SNode opNode = node.parentNode();
   int paramCount = SyntaxTree::countNodeMask(opNode.firstChild(), lxObjectMask);
   if (paramCount == 1) {
      // if it is a cast expression
      retVal = compileExpression(writer, opNode.firstChild(lxObjectMask), scope, targetRef, mode, nullptr);
   }
   else scope.raiseError(errInvalidOperation, node.parentNode());

   return retVal;
}

ObjectInfo Compiler :: compileNewArrOperation(SyntaxWriter& writer, SNode node, ExprScope& scope, ObjectInfo object,
   ref_t expecteRef, EAttr mode)
{
   int paramCount = SyntaxTree::countChildMask(node.parentNode(), lxObjectMask);

   EAttr paramsMode = HINT_PARAMETER;
   bool variadicOne = false;
   //bool inlineArg = false;
   ArgumentsInfo arguments;
   ref_t implicitSignatureRef = compileMessageParameters(writer, node, scope, paramsMode, 0, variadicOne, /*inlineArg, */arguments, nullptr);
   //if (inlineArg)
   //   scope.raiseError(errInvalidOperation, node);

//   SNode exprNode = node.parentNode();
   mssg_t messageRef = overwriteArgCount(scope.moduleScope->constructor_message, paramCount + 1);
   if (paramCount == 1) {
      // HOTFIX : if it is an array creation
      ref_t roperand = resolveObjectReference(scope, arguments[0], false);

      int operationType = _logic->resolveNewOperationType(*scope.moduleScope, object.reference, roperand);
      if (operationType != 0) {
         int size = _logic->defineStructSize(*scope.moduleScope, object.reference, object.element);
   
         if (!expecteRef || expecteRef == scope.moduleScope->superReference)
            expecteRef = resolveObjectReference(scope, object, true);

         if (expecteRef) {
            auto info = _logic->injectImplicitConversion(scope, *this, expecteRef, object.reference,
               object.element/*, noUnboxing, fixedArraySize*/);

            if (info.result == ConversionResult::crBoxingRequired)
               object.reference = expecteRef;
         }

         // if it is a primitive operation
         writer.newNode((LexicalType)operationType, object.reference);

         //if (size != 0)
         if (size < 0)
            writer.appendNode(lxSize, size);

         writeTerminal(writer, arguments[0], scope);

         writer.closeNode();

         return ObjectInfo(okObject, 0, object.reference, object.element, 0);
      }
   }

   int stackSafeAttr = 0;
   ref_t dummy = 0;
   //if (!EAttrs::test(mode, HINT_DIRECTCALL))
   messageRef = resolveMessageAtCompileTime(object, scope, messageRef, implicitSignatureRef, false,
      stackSafeAttr, dummy);

   if (!test(stackSafeAttr, 1)) {
      mode = mode | HINT_DYNAMIC_OBJECT;
   }
   else stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

   return compileMessage(writer, node, scope, object, messageRef, &arguments, mode, stackSafeAttr,
      expecteRef, nullptr);

}

//ObjectInfo Compiler :: compileOperation(SNode& node, ExprScope& scope, ObjectInfo objectInfo, ref_t expectedRef,
//   EAttr mode, bool propMode)
//{
//   SNode current = node.firstChild(lxOperatorMask);
//
//   switch (current.type) {
//      case lxMessage:
//         else if (propMode) {
//            objectInfo = compilePropAssigning(current, scope, objectInfo);
//         }
//         else objectInfo = compileMessage(current, scope, expectedRef, objectInfo, mode);
//         break;
//      case lxNewOperation:
//         objectInfo = compileBoxingExpression(current, scope, objectInfo, mode);
//         break;
////      case lxTypecast:
////         objectInfo = compileBoxingExpression(writer, current, scope, objectInfo, mode);
////         break;
//      case lxAssign:
//         objectInfo = compileAssigning(current, scope, objectInfo, current.argument == -1);
//         break;
//      case lxArrOperator:
//         if (propMode) {
//            current.setArgument(SET_REFER_OPERATOR_ID);
//         }
//      case lxOperator:
//         objectInfo = compileOperator(current, scope, objectInfo, mode);
//         break;
////      case lxWrapping:
////         objectInfo = compileWrapping(writer, current, scope, objectInfo, EAttrs::test(mode, HINT_CALL_MODE));
////         break;
//   }
//   node.refresh();
//
//   return objectInfo;
//}

ref_t Compiler :: mapTemplateAttribute(SNode node, Scope& scope)
{
   SNode terminalNode = node.firstChild(lxTerminalMask);
   IdentifierString templateName(terminalNode.identifier());
   int paramCounter = 0;
   SNode current = node.findChild(lxType, lxArrayType);
   while (current != lxNone) {
      if (current.compare(lxType, lxTemplateParam, lxArrayType)) {
         paramCounter++;
      }
      else if (current != /*lxClassRef*/lxDeclaredType)
         scope.raiseError(errInvalidOperation, node);

      current = current.nextNode();
   }

   templateName.append('#');
   templateName.appendInt(paramCounter);

   // NOTE : check it in declararion mode - we need only reference
   return resolveTypeIdentifier(scope, templateName.c_str(), terminalNode.type, true, false);
}

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
   while (current != lxNone) {
      if (current.compare(lxType, lxArrayType)) {
         ref_t typeRef = current.argument;
         if (!typeRef || typeRef == V_TEMPLATE) {
            SNode classRefNode = current.findChild(lxDeclaredType);
            if (!declarationMode || !classRefNode.argument) {
               typeRef = resolveTypeAttribute(current, scope, declarationMode, false);
               if (!declarationMode) {
                  current.set(lxType, typeRef);

                  injectAttributeIdentidier(current, scope);
               }
               else current.appendNode(lxDeclaredType, typeRef);
            }
            else typeRef = classRefNode.argument;
         }

         parameters.add(current);
      }

      current = current.nextNode();
   }
}

ref_t Compiler :: resolveTemplateDeclaration(SNode node, Scope& scope, bool declarationMode)
{
   // generate an reference class - inner implementation
   List<SNode> parameters;

   compileTemplateAttributes(node.firstChild(), parameters, scope, declarationMode);

   ref_t templateRef = mapTemplateAttribute(node, scope);
   if (!templateRef)
      scope.raiseError(errInvalidHint, node);

   SNode terminalNode = node.firstChild(lxTerminalMask);
   if (terminalNode != lxNone) {
      SNode sourceNode = terminalNode.findChild(lxTemplateSource);
      if (sourceNode == lxNone) {
         sourceNode = terminalNode.appendNode(lxTemplateSource);

         SNode col = terminalNode.findChild(lxCol);
         sourceNode.appendNode(col.type, col.argument);

         SNode row = terminalNode.findChild(lxRow);
         sourceNode.appendNode(row.type, row.argument);

         NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
         sourceNode.appendNode(lxSourcePath, nsScope->sourcePath.c_str());
      }

      parameters.add(sourceNode);
   }

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
   return scope.moduleScope->generateTemplate(templateRef, parameters, ns->nsName.c_str(), declarationMode, nullptr);
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

EAttr Compiler :: declareExpressionAttributes(SyntaxWriter& writer, SNode& current, ExprScope& scope, EAttr)
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
      else declareVariable(writer, current, scope, typeRef, /*!exprAttr.testany(HINT_REFOP)*/true);
   }

   return exprAttr;
}

ObjectInfo Compiler :: compileRootExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, EAttr mode)
{
   ExprScope exprScope(&scope);
   ObjectInfo retVal = compileExpression(writer, node, exprScope, targetRef, mode | HINT_ROOTEXPR, nullptr);

   return retVal;
}

ObjectInfo Compiler :: mapIntConstant(ExprScope& scope, int integer)
{
   return ObjectInfo(okIntConstant, ::mapIntConstant(scope, integer), V_INT32, 0, integer);
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

ObjectInfo Compiler :: mapTerminal(SNode node, ExprScope& scope, EAttr mode)
{
   ident_t token = node.identifier();
   ObjectInfo object;

   if (EAttrs::testany(mode, HINT_INTERNALOP | HINT_MEMBER | HINT_METAFIELD | HINT_EXTERNALOP | HINT_FORWARD |
      HINT_MESSAGEREF | HINT_SUBJECTREF | HINT_NEWOP | HINT_CASTOP | HINT_CLASSREF | HINT_PARAMSOP))
   {
      bool invalid = false;
      if (EAttrs::test(mode, HINT_NEWOP) || EAttrs::test(mode, HINT_CASTOP)) {
         if (EAttrs::test(mode, HINT_TARGET)) {
            if (node == lxType) {
               ref_t typeRef = resolveTypeAttribute(node, scope, false, false);
               object = mapClassSymbol(scope, typeRef);
            }
            else if (node == lxArrayType) {
               // if it is an array creation
               object.kind = okClass;
               object.element = resolveTypeAttribute(node.firstChild(), scope, false, false);
               object.param = resolvePrimitiveArray(scope,
                                 scope.moduleScope->arrayTemplateReference, object.element, false);
               object.reference = V_OBJARRAY;
            }
            else if (node == lxGlobalReference) {
               object = scope.mapGlobal(token);
            }
            else object = scope.mapTerminal(token, node == lxReference, EAttrs::exclude(mode, HINT_NEWOP | HINT_CASTOP));

            if (object.kind == okClass) {
               object.kind = EAttrs::test(mode, HINT_NEWOP) ? okNewOp : okCastOp;
            }
            else invalid = false;
         }
         else invalid = true;
      }
      else if (EAttrs::test(mode, HINT_INTERNALOP)) {
         if (node == lxReference) {
            object = ObjectInfo(okInternal, scope.module->mapReference(token), V_INT32);
         }
         else if (node == lxGlobalReference) {
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
      else if (EAttrs::test(mode, HINT_MESSAGEREF)) {
         object = ObjectInfo(okMessageRef);
      }
      else if (EAttrs::test(mode, HINT_FORWARD)) {
         IdentifierString forwardName(FORWARD_MODULE, "'", token);

         object = scope.mapTerminal(forwardName.ident(), true, EAttr::eaNone);
      }
      else if (EAttrs::test(mode, HINT_SUBJECTREF)) {
         object = compileSubjectReference(node, scope, mode);
      }
      else if (EAttrs::test(mode, HINT_CLASSREF)) {
         ref_t typeRef = resolveTypeAttribute(node, scope, false, false);

         object = mapClassSymbol(scope, typeRef);
      }
      else if (EAttrs::test(mode, HINT_PARAMSOP)) {
         object = compileVariadicUnboxing(node, scope, EAttrs::exclude(mode, HINT_PARAMSOP));
      }

      if (invalid)
         scope.raiseError(errInvalidOperation, node);
   }
   else {
      switch (node.type) {
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
               scope.raiseError(errInvalidIntNumber, node);

            object = mapIntConstant(scope, integer);
            break;
         }
         case lxLong:
         {
            String<char, 30> s("_"); // special mark to tell apart from integer constant
            s.append(token, getlength(token) - 1);

            token.toLongLong(10, 1);
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, node);

            object = ObjectInfo(okLongConstant, scope.moduleScope->module->mapConstant((const char*)s), V_INT64);
            break;
         }
         case lxHexInteger:
         {
            int integer = token.toULong(16);
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, node);

            object = mapIntConstant(scope, integer);
            break;
         }
         case lxReal:
         {
            double val = token.toDouble();
            if (errno == ERANGE)
               scope.raiseError(errInvalidIntNumber, node);

            object = mapRealConstant(scope, val);
            break;
         }
         case lxGlobalReference:
            object = scope.mapGlobal(token.c_str());
            break;
//         case lxMetaConstant:
//            // NOTE : is not allowed to be used outside const initialization
//            scope.raiseError(errIllegalOperation, terminal);
//            break;
//         case lxExplicitConst:
//         {
//            // try to resolve explicit constant
//            size_t len = getlength(token);
//
//            IdentifierString action(token + len - 1);
//            action.append(CONSTRUCTOR_MESSAGE);
//
//            ref_t dummyRef = 0;
//            ref_t actionRef = scope.module->mapAction(action, scope.module->mapSignature(&scope.moduleScope->literalReference, 1, false), dummyRef);
//
//            action.copy(token, len - 1);
//            object = ObjectInfo(okExplicitConstant, scope.moduleScope->module->mapConstant(action), 0, 0, actionRef);
//            break;
//         }
         case lxSubjectRef:
            object = compileSubjectReference(node, scope, mode);
            break;
         default:
            object = scope.mapTerminal(token, node == lxReference, mode & HINT_SCOPE_MASK);
            break;
      }
   }

//   if (object.kind == okExplicitConstant) {
//      // replace an explicit constant with the appropriate object
//      recognizeTerminal(terminal, ObjectInfo(okLiteralConstant, object.param), scope, mode);
//
//      mssg_t messageRef = encodeMessage(object.extraparam, 2, 0);
//      NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
//      Pair<ref_t, ref_t>  constInfo = nsScope->extensions.get(messageRef);
//      if (constInfo.value1 != 0) {
//         ref_t signRef = 0;
//         scope.module->resolveAction(object.extraparam, signRef);
//         if (!_logic->injectConstantConstructor(terminal, *scope.moduleScope, *this, constInfo.value1, messageRef))
//            scope.raiseError(errInvalidConstant, terminal);
//      }
//      else scope.raiseError(errInvalidConstant, terminal);
//
//      object = ObjectInfo(okObject, constInfo.value1);
//   }
   /*else */if (object.kind == okUnknown) {
      scope.raiseError(errUnknownObject, node);
   }

   return object;
}

//inline bool isAssigmentOp(SNode node)
//{
//   return node == lxAssign/* || (node == lxOperator && node.argument == -1)*/;
//}
//
//inline bool isCallingOp(SNode node)
//{
//   return node == lxMessage;
//}

void Compiler :: writeTerminal(SyntaxWriter& writer, ObjectInfo object, ExprScope& scope)
{
   switch (object.kind) {
      case okNil:
         writer.newNode(lxNil, 0/*object.param*/);
         break;
      case okClass:
      case okClassSelf:
         writer.newNode(lxClassSymbol, object.param);
         break;
      case okExtension:
      //   if (!EAttrs::test(mode, HINT_CALLOP)) {
      //      scope.raiseWarning(WARNING_LEVEL_3, wrnExplicitExtension, terminal);
      //   }
      case okConstantSymbol:
      case okConstantRole:
      case okSingleton:
         writer.newNode(lxConstantSymbol, object.param);
         break;
      case okParam:
      case okLocal:
      case okTempLocal:
      case okBoxableLocal:
      case okTempBoxableLocal:
      case okOutputBoxableLocal:
         writer.newNode(lxLocal, object.param);
         break;
      case okParamField:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxLocal, object.param);
         writer.appendNode(lxField, 0);
         break;
      case okSelfParam:
         writer.newNode(lxSelfLocal, object.param);
         break;
      case okSuper:
         writer.newNode(lxSelfLocal, object.param);
         break;
      case okLocalAddress:
      case okTempLocalAddress:
         writer.newNode(lxLocalAddress, object.param);
         break;
      case okReadOnlyField:
      case okField:
      case okOuter:
      case okOuterSelf:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxSelfLocal, 1);
         writer.appendNode(lxField, object.param);
         break;
      case okOuterField:
      case okOuterReadOnlyField:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxSelfLocal, 1);
         writer.appendNode(lxField, object.param);
         writer.appendNode(lxField, object.extraparam);
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
      case okUIntConstant:
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
      case okFieldAddress:
      case okReadOnlyFieldAddress:
         writer.newNode(lxFieldExpression, 0);
         writer.appendNode(lxSelfLocal, 1);
         if (object.param || (/*!EAttrs::test(mode, HINT_PROP_MODE)
            &&*/ (_logic->defineStructSize(*scope.moduleScope, object.reference, 0) & 3) != 0))
         {
            // if it a field address (except the first one, which size is 4-byte aligned)
            writer.appendNode(lxFieldAddress, object.param);
            //// the field address expression should always be boxed (either local or dynamic)
            //mode = EAttrs(mode, HINT_NOBOXING);
         }

         //if (isPrimitiveArrRef(object.reference)) {
         //   int size = defineFieldSize(scope, object.param);
         //
         //   setVariableTerminal(terminal, scope, object, mode, lxFieldExpression, size);
         //}
         //else setVariableTerminal(terminal, scope, object, mode, lxFieldExpression);
         break;
      case okStaticField:
//         if ((int)object.param < 0) {
//            // if it is a normal static field - field expression should be used
//            writer.newNode(lxFieldExpression, 0);
//            writer.appendNode(lxClassRefField, 1);
//            writer.appendNode(lxStaticField, object.param);
//         }
         // if it is a sealed static field
         /*else */writer.newNode(lxStaticField, object.param);
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
            writer.newNode(lxFieldExpression, 0);
            writer.appendNode(lxSelfLocal, 1);
            writer.appendNode(lxClassRef, scope.getClassRefId());
            writer.appendNode(lxStaticConstField, object.param);
         }
         else writer.newNode(lxStaticField, object.param);
         break;
      case okClassStaticConstantField:
         if ((int)object.param < 0) {
            writer.newNode(lxFieldExpression, 0);
            writer.appendNode(lxSelfLocal, 1);
            writer.appendNode(lxStaticConstField, object.param);
         }
         else throw InternalError("Not yet implemented"); // !! temporal
         break;
      case okMessageNameConstant:
         writer.newNode(lxSubjectConstant, object.param);
         break;
      case okMessageConstant:
         writer.newNode(lxMessageConstant, object.param);
         break;
      case okObject:
         writer.newNode(lxResult);
         break;
      case okSymbol:
         writer.newNode(lxSymbolReference, object.param);
         break;
      case okParams:
      {
         if (object.reference == V_UNBOXEDARGS) {
            writer.newNode(lxArgArray);
            writer.appendNode(lxBlockLocalAddr, object.param);
         }
         else {
            ref_t r = resolvePrimitiveReference(scope, object.reference, object.element, false);
            if (!r)
               throw InternalError("Cannot resolve variadic argument template");

            writer.newNode(lxBlockLocalAddr, object.param);
         }

         break;
      }
      default:
         return;
   }

   writer.closeNode();
}

ObjectInfo Compiler :: compileObject(SyntaxWriter& writer, SNode& node, ExprScope& scope, EAttr mode,
   ArgumentsInfo* preservedArgs)
{
   bool paramMode = EAttrs::test(mode, HINT_PARAMETER);

   ObjectInfo retVal;
   if (node.compare(lxAttribute, lxType, lxArrayType, lxBookmarkReference)) {
      mode = mode | declareExpressionAttributes(writer, node, scope, mode);

      // HOTFIX : ignore loop attribute for the object
      mode = EAttrs::exclude(mode, HINT_LOOP | HINT_DIRECTCALL);

      if (EAttrs::test(mode, HINT_REFOP)) {
         return compileReferenceExpression(writer, node, scope, EAttrs::exclude(mode, HINT_REFOP));
      }

      //      //      if (targetMode.testAndExclude(HINT_INLINEARGMODE)) {
      //      //         noPrimMode = true;
      //      //         inlineArgMode = true;
      //      //      }
   }

   if (test(node.type, lxTerminalMask) || node.compare(lxType, lxArrayType)) {
      if (!EAttrs::test(mode, HINT_NODEBUGINFO) && test(node.type, lxTerminalMask)) {
         if (node.existChild(lxRow)) {
            writer.newNode(lxBreakpoint, dsStep);
            SyntaxTree::copyNode(writer, node);
            writer.closeNode();
         }
      }

      if (!paramMode) {
         retVal = mapTerminal(node, scope, mode);
         writeTerminal(writer, retVal, scope);
      }
      else retVal = mapTerminal(node, scope, mode);

      return retVal;
   }
   else return compileExpression(writer, node, scope, 0, mode, preservedArgs);
}

void Compiler :: recognizeExprAttrs(SNode node, EAttr& mode)
{
   if (node.compare(lxExpression, lxMessageExpression, lxOperationExpression)) {
      recognizeExprAttrs(node.firstChild(), mode);
   }
   else if (node == lxAttribute) {
      switch (node.argument) {
         case V_LOOP:
            mode = mode | HINT_LOOP;
            break;
         case V_WEAKOP:
            mode = mode | HINT_DIRECTCALL;
            break;
         default:
            break;
      }
   }
}

bool Compiler :: isVariableDeclaration(SNode node, ExprScope& scope, ref_t& typeRef)
{
   SNode current = node.firstChild();
   if (current == lxAttribute) {
      if (current.argument != V_VARIABLE)
         return false;

      current = current.nextNode();
   }

   if (current.compare(lxType, lxArrayType) && test(current.nextNode(), lxTerminalMask)) {
      typeRef = resolveTypeAttribute(current, scope, false, false);

      return true;
   }

   return false;
}

ObjectInfo Compiler :: compileExpression(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t targetRef,
   EAttr mode, ArgumentsInfo* preservedArgs)
{
   bool noPrimMode = EAttrs::test(mode, HINT_NOPRIMITIVES);
   bool isParam = EAttrs::test(mode, HINT_PARAMETER);
   bool conversionMode = targetRef != 0;
   if (conversionMode)
      mode = mode | HINT_PARAMETER;

   ObjectInfo retVal;

   bool rootMode = EAttrs::test(mode, HINT_ROOTEXPR);
   if (rootMode) {
      writer.newNode(lxSeqExpression);
      mode = EAttrs::exclude(mode, HINT_ROOTEXPR);

      recognizeExprAttrs(node.firstChild(), mode);
   }

   switch (node.type) {
      case lxExpression:
         retVal = compileExpression(writer, node.firstChild(), scope, targetRef, mode, preservedArgs);
         break;
      case lxMessageExpression:
         if (EAttrs::test(mode, HINT_LOOP)) {
            writer.newNode(lxLooping);
            compileMessageExpression(writer, node, scope, targetRef, EAttrs::exclude(mode, HINT_PARAMETER | HINT_LOOP));
            writer.closeNode();

            retVal = ObjectInfo(okObject);
         }
         else retVal = compileMessageExpression(writer, node, scope, targetRef, mode);
         break;
      case lxPropertyExpression:
         retVal = compileMessageExpression(writer, node, scope, targetRef, mode | HINT_PROP_MODE);
         break;
      case lxOperationExpression:
         retVal = compileOperationExpression(writer, node, scope, mode, targetRef);
         break;
      case lxUnaryExpression:
         retVal = compileUnaryExpression(writer, node, scope, mode);
         break;
      case lxAssigningExpression:
         retVal = compileAssigningExpression(writer, node, scope, mode);
         break;
      case lxArrayExpression:
      {
         ref_t typeRef = 0;
         // HOTFIX : recognize fixed-size array declaration
         if (isVariableDeclaration(node, scope, typeRef)) {
            declareVariable(writer, node, scope, typeRef, true);

            SNode terminalNode = node.firstChild(lxTerminalMask);
            return compileObject(writer, terminalNode, scope, mode, preservedArgs);
         }
         retVal = compileOperation(writer, node, scope, mode, REFER_OPERATOR_ID, false, false, targetRef);
         break;
      }
      case lxNestedExpression:
      case lxClosureExpression:
         retVal = compileClosure(writer, node, scope, mode/* & HINT_CLOSURE_MASK*/, preservedArgs);
         break;
      case lxSwitchExpression:
         retVal = compileSwitchExpression(writer, node, scope);
         break;
      case lxCodeExpression:
      case lxCode:
      {
         bool withRetStatement = false;
         if (EAttrs::test(mode, HINT_EXTERNALOP)) {
            writer.newNode(lxExternFrame);
            retVal = compileSubCode(writer, node, scope, false, withRetStatement);
            writer.closeNode();
         }
         else retVal = compileSubCode(writer, node, scope, false, withRetStatement);
      //            scope.setCodeRetStatementFlag(withRetStatement);
         break;
      }
      case lxCollectionExpression:
         retVal = compileCollection(writer, node, scope, mode);
         break;
      default:
         retVal = compileObject(writer, node, scope, mode, preservedArgs);
         break;
   }

   if (retVal.kind == okExternal) {
      retVal.reference = V_DWORD;
      if (targetRef != 0) {
         if (_logic->isCompatible(*scope.moduleScope, V_DWORD, targetRef, true)) {
            retVal.reference = targetRef;
         }
         else if (_logic->isCompatible(*scope.moduleScope, V_REAL64, targetRef, true)) {
            retVal.reference = targetRef;
            retVal.param = (ref_t)-1;         // to indicate Float mode
         }
         else if (_logic->isCompatible(*scope.moduleScope, V_INT64, targetRef, true)) {
            retVal.reference = targetRef;
         }
         else if (_logic->isCompatible(*scope.moduleScope, V_QWORD, targetRef, true)) {
            retVal.reference = targetRef;
         }
      }

      retVal = boxExternal(writer, retVal, scope);
   }
   else {
      ref_t sourceRef = resolveObjectReference(scope, retVal, false);
      if (!targetRef && isPrimitiveRef(sourceRef) && noPrimMode) {
         /*if (objectInfo.reference == V_INLINEARG && EAttrs::test(modeAttrs, HINT_PARAMETER)) {
            // HOTFIX : do not resolve inline argument
         }
         else */targetRef = resolvePrimitiveReference(scope, sourceRef, retVal.element, false);
      }

      if (targetRef) {
         //if (noUnboxing)
         //   mode = mode | HINT_NOUNBOXING;

         retVal = convertObject(writer, node, scope, targetRef, retVal, mode);
         if (retVal.kind == okUnknown) {
            scope.raiseError(errInvalidOperation, node);
         }
      }
   }

   if (isParam) {
      if (retVal.kind == okObject)
         retVal = saveToTempLocal(writer, scope, retVal);
   }
   else if (conversionMode && retVal.kind != okObject)
      writeTerminal(writer, retVal, scope);

   if (rootMode)
      writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileSubCode(SyntaxWriter& writer, SNode codeNode, ExprScope& scope, bool branchingMode, bool& withRetStatement)
{
   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   CodeScope subScope(codeScope);

   writer.newNode(lxCode);
   if (branchingMode && codeNode == lxExpression) {
      //HOTFIX : inline branching operator
      compileRootExpression(writer, codeNode.firstChild(), subScope, 0, HINT_ROOT/* | HINT_DYNAMIC_OBJECT*/);
   }
   else compileCode(writer, codeNode, subScope);
   writer.closeNode();

   // preserve the allocated space
   subScope.syncStack(codeScope);

   withRetStatement = subScope.withRetStatement;

   return ObjectInfo(okObject);
}

void Compiler :: compileEmbeddableRetExpression(SyntaxWriter& writer, SNode node, ExprScope& scope)
{
   ObjectInfo retVar = scope.mapTerminal(RETVAL_ARG, false, HINT_ASSIGNTARGET);

   compileAssigning(writer, node.firstChild(), scope, retVar, EAttr::eaNone);
}

ObjectInfo Compiler :: compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope)
{
   ObjectInfo retVal;

   bool needVirtualEnd = true;
   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxExpression:
            compileRootExpression(writer, current, scope, 0, HINT_ROOT);
            break;
         case lxReturning:
         {
            needVirtualEnd = false;

            //if (test(scope.getMessageID(), SPECIAL_MESSAGE))
            //   scope.raiseError(errIllegalOperation, current);

            if (scope.withEmbeddableRet()) {
               ExprScope exprScope(&scope);

               writer.newNode(lxSeqExpression);

               retVal = scope.mapTerminal(SELF_VAR, false, EAttr::eaNone);

               compileEmbeddableRetExpression(writer, current, exprScope);

               writer.newNode(lxReturning);
               writeTerminal(writer, retVal, exprScope);
               writer.closeNode();

               writer.closeNode();
            }
            else retVal = compileRetExpression(writer, current, scope, HINT_ROOT/* | HINT_RETEXPR*/);

            scope.withRetStatement = true;
            break;
         }
//         case lxCode:
//         {
//            // compile sub code
//            ExprScope exprScope(&scope);
//            bool withRetStatement = false;
//            compileSubCode(current, exprScope, false, withRetStatement);
//            exprScope.setCodeRetStatementFlag(withRetStatement);
//
//            break;
//         }
         case lxEOP:
            needVirtualEnd = false;
            //writer.newNode(lxEOP);
            //current.injectNode(lxTerminalMask); // injecting virtual terminal token
            writer.newNode(lxEOP);
            writer.newNode(lxBreakpoint, dsEOP);
            SyntaxTree::copyNode(writer, current);
            writer.closeNode();
            writer.closeNode();
            break;
      }

//      scope.freeSpace();

      current = current.nextNode();
   }

   if (needVirtualEnd) {
      writer.newNode(lxEOP);
      writer.appendNode(lxBreakpoint, dsVirtualEnd);
      writer.closeNode();
   }

   if (_trackingUnassigned) {
      // warn if the variable was not assigned
      for (auto it = scope.locals.start(); !it.Eof(); it++) {
         if ((*it).unassigned) {
            warnOnUnassignedLocal(node, scope, (*it).offset);
         }
      }
   }

   return retVal;
}

void Compiler :: compileExternalArguments(SyntaxWriter& writer, SNode node, ExprScope& scope, ArgumentsInfo* arguments)
{
   for (pos_t i = 0; i != arguments->Length(); i++) {
      ObjectInfo param = (*arguments)[i];
      ref_t typeRef = resolveObjectReference(scope, param, false, false);

      if (param.kind == okIntConstant) {
         writer.newNode(lxExtIntConst);
         writeTerminal(writer, param, scope);
         writer.closeNode();
      }
      else if (_logic->isCompatible(*scope.moduleScope, V_DWORD, typeRef, true)
         && !_logic->isVariable(*scope.moduleScope, typeRef))
      {
         writer.newNode(lxExtIntArg);
         writeTerminal(writer, param, scope);
         writer.closeNode();
      }
      else writeTerminal(writer, param, scope);
   }
//      else {
//         if (objNode.compare(lxBoxableExpression, lxCondBoxableExpression)) {
//            if (!typeRef)
//               typeRef = objNode.findChild(lxType).argument;
//
//            analizeOperand(objNode, scope, false, false, false);
//
//            objNode = objNode.findSubNodeMask(lxObjectMask);
//         }
//         if (objNode.type != lxSymbolReference && (!test(objNode.type, lxOpScopeMask) || objNode == lxFieldExpression)) {
//            if (_logic->isCompatible(*scope.moduleScope, V_DWORD, typeRef, true)
//               && !_logic->isVariable(*scope.moduleScope, typeRef))
//            {
//               // if it is a integer variable
//               SyntaxTree::copyNode(objNode, callNode
//                  .appendNode(lxExtIntArg)
//                  .appendNode(objNode.type, objNode.argument));
//            }
//            else SyntaxTree::copyNode(objNode, callNode
//               .appendNode(objNode.type, objNode.argument));
//
//            current = lxIdle;
//         }
//         else
//      }
//
//      current = current.nextNode(lxObjectMask);
//   }
}

ObjectInfo Compiler :: compileExternalCall(SyntaxWriter& writer, SNode node, ExprScope& scope, ref_t expectedRef,
   ArgumentsInfo* arguments)
{
   ObjectInfo retVal(okExternal);

   _ModuleScope* moduleScope = scope.moduleScope;

   bool stdCall = false;
   bool apiCall = false;

   ident_t dllAlias = node.firstChild(lxTerminalMask).identifier();
   ident_t functionName = node.findChild(lxMessage).firstChild(lxTerminalMask).identifier();

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

   // To tell apart coreapi calls, the name convention is used
   if (apiCall) {
      writer.newNode(lxCoreAPICall, reference);
   }
   else writer.newNode(stdCall ? lxStdExternalCall : lxExternalCall, reference);

   compileExternalArguments(writer, node, scope, arguments);

   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileInternalCall(SyntaxWriter& writer, SNode node, ExprScope& scope, mssg_t message,
   ref_t signature, ObjectInfo routine, ArgumentsInfo* arguments)
{
   _ModuleScope* moduleScope = scope.moduleScope;

   IdentifierString virtualReference(moduleScope->module->resolveReference(routine.param));
   virtualReference.append('.');

   pos_t argCount;
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

   writer.newNode(lxInternalCall, moduleScope->module->mapReference(virtualReference));

   if (arguments != nullptr) {
      for (unsigned int i = 0; i < arguments->Length(); i++) {
         ObjectInfo arg = (*arguments)[i];

         writeTerminal(writer, arg, scope);
      }
   }

   writer.closeNode();

   return ObjectInfo(okObject);
}

int Compiler :: allocateStructure(bool bytearray, int alignment, int& allocatedSize, int& reserved)
{
   allocatedSize = align(allocatedSize, alignment);

   int sizeOffset = 0;
   if (bytearray) {
      // reserve place for byte array header if required
      sizeOffset = align(4, alignment);

      allocatedSize += sizeOffset;

      sizeOffset >>= 2;

      reserved += sizeOffset;
   }

   allocatedSize = allocatedSize >> 2;

   int retVal = reserved;
   reserved += (allocatedSize - sizeOffset);

   retVal = newLocalAddr(retVal);

   return retVal;
}

bool Compiler :: allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand)
{
   if (size <= 0)
      return false;

   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::ScopeLevel::slMethod);
   if (methodScope == NULL)
      return false;

   int offset = allocateStructure(binaryArray, scope.moduleScope->stackAlignment, size, scope.allocated2);

   exprOperand.kind = okLocalAddress;
   exprOperand.param = offset;

   return true;
}

bool Compiler :: allocateTempStructure(ExprScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand)
{
   if (size <= 0 || scope.tempAllocated2 == -1)
      return false;

   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::slCode);

   int offset = allocateStructure(binaryArray, scope.moduleScope->stackAlignment, size, scope.tempAllocated2);

   if (scope.tempAllocated2 > codeScope->reserved2)
      codeScope->reserved2 = scope.tempAllocated2;

   exprOperand.kind = okTempLocalAddress;
   exprOperand.param = offset;

   return true;
}

ref_t Compiler :: declareInlineArgumentList(SNode arg, MethodScope& scope, bool declarationMode)
{
   // HOTFIX : find an parameter terminal if it is a first argument
   if (arg.compare(lxType, lxArrayType))
      arg = arg.parentNode().firstChild(lxTerminalMask);

//   IdentifierString signature;
   IdentifierString messageStr;

   ref_t actionRef = 0;
   ref_t signRef = 0;

//   SNode sign = goToNode(arg, lxTypeAttr);
   ref_t signatures[ARG_COUNT];
   size_t signatureLen = 0;
//   bool first = true;
   bool weakSingature = true;
   while (arg.compare(lxIdentifier, lxMethodParameter)/* || arg == lxPrivate*/) {
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
      declareArgumentAttributes(terminalNode.parentNode(), scope, classRef, elementRef, declarationMode);

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

   if (signatureLen > 0 && !weakSingature) {
      if (scope.parameters.Count() == signatureLen) {
         signRef = scope.module->mapSignature(signatures, signatureLen, false);
      }
      else scope.raiseError(errInvalidOperation, arg);
   }

   actionRef = scope.moduleScope->module->mapAction(messageStr, signRef, false);

   return encodeMessage(actionRef, scope.parameters.Count(), FUNCTION_MESSAGE);
}

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
   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      paramCount = 1;
      // HOTFIX : extension is a special case - target should be included as well for variadic function
      if (scope.extensionMode && test(flags, FUNCTION_MESSAGE))
         paramCount++;
   }

   // NOTE : a message target should be included as well for a normal message
   size_t argCount = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
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
   size_t paramCount = 0;
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
         if (paramCount >= ARG_COUNT || (flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE)
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
      else break;

      current = current.nextNode();
   }

   // if the signature consists only of generic parameters - ignore it
   if (weakSignature)
      signatureLen = 0;

   // HOTFIX : do not overrwrite the message on the second pass
   if (scope.message == 0) {
      if ((scope.hints & tpMask) == tpDispatcher) {
         if (paramCount == 0 && unnamedMessage) {
            actionRef = getAction(scope.moduleScope->dispatch_message);
            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (test(scope.hints, tpConversion)) {
         if (paramCount == 0 && unnamedMessage && scope.outputRef) {
            if (test(scope.hints, tpSealed | tpGeneric)) {
               // COMPILER MAGIC : if it is generic conversion routine
               if (signatureLen > 0 || !unnamedMessage || test(scope.hints, tpFunction))
                  scope.raiseError(errInvalidHint, node);

               actionRef = scope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false);
               flags |= CONVERSION_MESSAGE;
            }
            else {
               ref_t signatureRef = scope.moduleScope->module->mapSignature(&scope.outputRef, 1, false);
               actionRef = scope.moduleScope->module->mapAction(CAST_MESSAGE, signatureRef, false);
               flags |= CONVERSION_MESSAGE;
            }

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
            mssg_t publicMessage = mapMethodName(scope, paramCount, actionRef, flags, actionStr,
               signature, signatureLen, withoutWeakMessages, noSignature);

            mssg_t declaredMssg = scope.getAttribute(publicMessage, maProtected);
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

void Compiler :: compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, LexicalType methodType,
   bool withGenericMethods, bool withOpenArgGenerics)
{
   writer.newNode(methodType, scope.message);

   SNode dispatchNode = node.findChild(lxDispatchCode);

   if (dispatchNode != lxNone) {
      CodeScope codeScope(&scope);

      compileDispatchExpression(writer, dispatchNode, codeScope, withGenericMethods);
   }
   else {
      writer.newNode(lxDispatching);
      writer.newNode(lxResending, 0);

      // if it is generic handler without redirect statement
      if (withGenericMethods) {
         // !! temporally
         if (withOpenArgGenerics)
            scope.raiseError(errInvalidOperation, node);

         writer.appendNode(lxMessage,
            encodeMessage(scope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 1, 0));

         writer.newNode(lxCallTarget, scope.moduleScope->superReference);
         writer.appendNode(lxMessage, scope.moduleScope->dispatch_message);
         writer.closeNode();
      }
      // if it is open arg generic without redirect statement
      else if (withOpenArgGenerics) {
         // HOTFIX : an extension is a special case of a variadic function and a target should be included
         int argCount = !scope.extensionMode && test(scope.message, FUNCTION_MESSAGE) ? 1 : 2;

         writer.appendNode(lxMessage, encodeMessage(getAction(scope.moduleScope->dispatch_message),
            argCount, VARIADIC_MESSAGE));

         writer.newNode(lxCallTarget, scope.moduleScope->superReference);
         writer.appendNode(lxMessage, scope.moduleScope->dispatch_message);
         writer.closeNode();
      }
      else throw InternalError("Not yet implemented"); // !! temporal

      writer.closeNode();
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileActionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   beginMethod(writer, node, scope, lxClassMethod);

   CodeScope codeScope(&scope);

   SNode body = node.findSubNode(lxCode, lxReturning);

   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains previous $self value
   codeScope.allocated1++;

   ObjectInfo retVal = compileCode(writer, body == lxReturning ? body.parentNode() : body, codeScope);

   writer.closeNode();

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileExpressionMethod(SyntaxWriter& writer, SNode node, MethodScope& scope, bool lazyMode)
{
   beginMethod(writer, node, scope, lxClassMethod);

   CodeScope codeScope(&scope);

   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains previous $self value
   codeScope.allocated1++;
//   scope.preallocated = codeScope.allocated1;

//   if (lazyMode) {
//      int tempLocal = newLocalAddr(codeScope.allocated2++);
//
//      SNode frameNode = current.parentNode();
//      // HOTFIX : lazy expression should presave incoming message
//      frameNode.insertNode(lxIndexSaving).appendNode(lxLocalAddress, tempLocal);
//
//      SNode attrNode = current.firstChild();
//      while ((attrNode != lxAttribute || attrNode.argument != V_LAZY) && attrNode != lxNone)
//         attrNode = attrNode.nextNode();
//
//      // HOTFIX : comment lazy attribute out to prevent short circuit
//      if (attrNode == lxAttribute && attrNode.argument == V_LAZY)
//         attrNode.setArgument(0);
//
//      compileRetExpression(current, codeScope, EAttr::eaNone);
//
//      // HOTFIX : lazy expression should presave incoming message
//      frameNode.appendNode(lxIndexLoading).appendNode(lxLocalAddress, tempLocal);;
//   }
   /*else */compileRetExpression(writer, node, codeScope, EAttr::eaNone);

   writer.closeNode();

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: warnOnUnassignedLocal(SNode node, Scope& scope, int level)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxVariable:
         case lxIntVariable:
         case lxReal64Variable:
         case lxLongVariable:
         {
            SNode levelNode = current.findChild(lxLevel);
            if (levelNode.argument == (ref_t)level) {
               scope.raiseWarning(WARNING_LEVEL_3, wrnUnassignedVaiable, current.firstChild(lxTerminalMask));
               break;
            }
         }
         default:
            break;
      }
      current = current.nextNode();
   }
}

//void Compiler :: warnOnUnresolvedDispatch(SNode node, Scope& scope, mssg_t message, bool errorMode)
//{
//   // ingore dispatch message
//   if (message == scope.moduleScope->dispatch_message)
//      return;
//
//   scope.moduleScope->printMessageInfo(infoAbstractMetod, message);
//
//   SNode terminal = node.firstChild(lxTerminalMask);
//   if (terminal != lxNone) {
//      if (errorMode) {
//         scope.raiseError(errUnresolvedDispatch, node);
//      }
//      else scope.raiseWarning(WARNING_LEVEL_1, wrnUnresolvedDispatch, node);
//   }
//   else {
//      SNode current = node.parentNode();
//      if (current == lxClassMethod && current.existChild(lxAutogenerated)) {
//         while (current != lxNone) {
//            current = current.parentNode();
//
//            if (current == lxClass && current.existChild(lxNameAttr)) {
//               SNode nameNode = current.findChild(lxNameAttr).firstChild(lxTerminalMask);
//               if (nameNode != lxNone) {
//                  if (errorMode) {
//                     scope.raiseError(errUnresolvedInterface, nameNode);
//                  }
//                  else scope.raiseWarning(WARNING_LEVEL_1, wrnUnresolvedInterface, nameNode);
//
//                  break;
//               }
//            }
//            else if (current == lxRoot) {
//               SNode sourceNode = current.findChild(lxTemplateSource);
//
//               if (errorMode) {
//                  scope.raiseError(errUnresolvedInterface, sourceNode);
//               }
//               else scope.raiseWarning(WARNING_LEVEL_1, wrnUnresolvedInterface, sourceNode);
//
//               break;
//            }
//         }
//      }
//   }
//}

ObjectInfo Compiler :: mapTarget(SNode node, ExprScope& scope)
{
   SNode current = node.firstChild();

   EAttr mode = EAttr::eaNone;
   ObjectInfo retVal;
   ref_t typeRef = 0;
   bool newVar = false;
   if (current.compare(lxAttribute, lxType, lxArrayType, lxBookmarkReference)) {
      mode = recognizeExpressionAttributes(current, scope, typeRef, newVar);
   }

   if (test(current.type, lxTerminalMask) && !typeRef && !newVar) {
      retVal = mapTerminal(current, scope, mode);
   }
   else if (current == lxExpression) {
      retVal = mapTarget(current, scope);
   }

   return retVal;
}

void Compiler :: compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool withGenericMethods)
{
   ExprScope exprScope(&scope);
   ObjectInfo target = mapTarget(node, exprScope);

   if (target.kind == okInternal) {
      importCode(writer, node, exprScope, target.param, exprScope.getMessageID());
   }
   else {
      MethodScope* methodScope = (MethodScope*)exprScope.getScope(Scope::ScopeLevel::slMethod);

      // try to implement light-weight resend operation
      ref_t targetRef = methodScope->getReturningRef(false);
      int stackSafeAttrs = 0;
      if (methodScope->message == methodScope->moduleScope->dispatch_message) {
         // HOTFIX : if it is a generic message resending
         if (withGenericMethods) {
            writer.newNode(lxDispatching, encodeMessage(scope.moduleScope->module->mapAction(GENERIC_PREFIX, 0, false), 0, 0));
         }
         else writer.newNode(lxDispatching);

         writer.newNode(lxGenericResending, methodScope->message);

         writer.newNode(lxNewFrame);
         writer.newNode(lxSeqExpression);
         // NOTE : predefined in generic resending handler
         exprScope.tempAllocated1++;
         exprScope.tempAllocated2++;

         target = compileExpression(writer, node.firstChild(), exprScope, 0, HINT_PARAMETER, nullptr);

         if (boxingRequired(target)) {
            target = boxArgumentInPlace(writer, node, target, exprScope, false, condBoxingRequired(target));
         }
         if (target.kind != okObject)
            writeTerminal(writer, target, exprScope);

         writer.closeNode();

         writer.appendNode(lxReserved, exprScope.tempAllocated2);
         writer.appendNode(lxAllocated, exprScope.tempAllocated1);
         writer.closeNode();

         writer.closeNode();
         writer.closeNode();
      }
      else {
         // append a flag to be used for optimization routine
         writer.appendNode(lxDispatchMode);

         writer.newNode(lxNewFrame, 0);

         // new stack frame
         // stack already contains current self reference
         // the original message should be restored if it is a generic method
         scope.allocated1++;
         exprScope.tempAllocated1++;

         writer.newNode(lxSeqExpression);

         SNode targetNode = node.firstChild();
         if (target.kind == okUnknown)
            // HOTFIX : if it was not recognized
            target = compileObject(writer, targetNode, exprScope, HINT_TARGET, nullptr);

         SNode exprNode = targetNode.nextNode(lxObjectMask);

         ArgumentsInfo arguments;
         EAttr mode = EAttr::eaNone;
         for (auto it = methodScope->parameters.start(); !it.Eof(); it++) {
            auto param = *it;
            EAttr paramMode = EAttr::eaNone;
            if (param.class_ref == V_WRAPPER)
               paramMode = HINT_REFEXPR;
            
            ObjectInfo paramInfo = methodScope->mapParameter(param, paramMode);

            arguments.add(paramInfo);
         }

         stackSafeAttrs = _logic->defineStackSafeAttrs(*scope.moduleScope, methodScope->message);
         ObjectInfo retVal = compileMessage(writer, exprNode, exprScope, target, methodScope->message, &arguments,
            mode | HINT_NODEBUGINFO, stackSafeAttrs, 0, nullptr);

         retVal = convertObject(writer, exprNode, exprScope, targetRef, retVal, mode);
         if (retVal.kind == okUnknown) {
            exprScope.raiseError(errInvalidOperation, node);
         }

         writer.closeNode();
         writer.closeNode();
      }
   }
}

inline mssg_t resolveProtectedMessage(ClassInfo& info, mssg_t protectedMessage)
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

void Compiler :: compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& codeScope, ClassScope& classClassScope,
   bool& withFrame)
{
   ResendScope resendScope(&codeScope);
   resendScope.constructionMode = true;

   SNode expr = node.firstChild(lxOpScopeMask);

   _ModuleScope* moduleScope = codeScope.moduleScope;
   MethodScope* methodScope = (MethodScope*)codeScope.getScope(Scope::ScopeLevel::slMethod);

   mssg_t messageRef = mapMessage(expr, resendScope, false, false, false);

   ref_t classRef = classClassScope.reference;
   bool found = false;

   if (node.existChild(lxCode) || !isConstantArguments(expr)) {
      withFrame = true;

      // new stack frame
      // stack already contains $self value
      writer.newNode(lxNewFrame, 0);
      codeScope.allocated1++;
      // HOTFIX : take into account saved self variable
      resendScope.tempAllocated1 = codeScope.allocated1;

   }
   writer.newNode(lxSeqExpression);

   resendScope.withFrame = withFrame;

   bool variadicOne = false;
//   bool inlineArg = false;
   ArgumentsInfo arguments;
   SNode argNode = expr.findChild(lxMessage).nextNode();
   ref_t implicitSignatureRef = compileMessageParameters(writer, argNode, resendScope, EAttr::eaNone, 0, 
      variadicOne/*, inlineArg*/, arguments, nullptr);

   ObjectInfo target(okClassSelf, resendScope.getClassRefId(), classRef);
   int stackSafeAttr = 0;
   ref_t dummy = 0;
   messageRef = resolveMessageAtCompileTime(target, resendScope, messageRef, implicitSignatureRef, false, stackSafeAttr, dummy);

   // find where the target constructor is declared in the current class
   // but it is not equal to the current method
   ClassScope* classScope = (ClassScope*)resendScope.getScope(Scope::ScopeLevel::slClass);
   int hints = methodScope->message != messageRef ? checkMethod(*moduleScope, classScope->info.header.classRef, messageRef) : tpUnknown;
   if (hints != tpUnknown) {
      found = true;
   }
   // otherwise search in the parent class constructors
   else {
      mssg_t publicRef = resolveProtectedMessage(classClassScope.info, messageRef);
      // HOTFIX : replace protected message with public one
      if (publicRef)
         messageRef = publicRef;

      ref_t parent = classScope->info.header.parentRef;
      ClassInfo info;
      while (parent != 0) {
         moduleScope->loadClassInfo(info, moduleScope->module->resolveReference(parent));

         mssg_t protectedConstructor = 0;
         if (checkMethod(*moduleScope, info.header.classRef, messageRef, protectedConstructor) != tpUnknown) {
            classRef = info.header.classRef;
            found = true;

            target.reference = classRef;
            messageRef = resolveMessageAtCompileTime(target, resendScope, messageRef, implicitSignatureRef,
               false, stackSafeAttr, dummy);

            break;
         }
         else if (protectedConstructor) {
            classRef = info.header.classRef;
            found = true;

            target.reference = classRef;
            messageRef = resolveMessageAtCompileTime(target, resendScope, protectedConstructor, implicitSignatureRef,
               false, stackSafeAttr, dummy);

            break;
         }
         else parent = info.header.parentRef;
      }
   }

   if (found) {
      compileMessage(writer, expr, resendScope, target, messageRef, &arguments, EAttr::eaNone, stackSafeAttr, 0, nullptr);
   }
   else resendScope.raiseError(errUnknownMessage, node);

   if (withFrame) {
      writer.newNode(lxAssigning);
      writer.appendNode(lxLocal, 1);
      writer.appendNode(lxResult);
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool isDefault)
{
   SNode redirect = node.findChild(lxRedirect);
   if (redirect != lxNone) {
      if (!isDefault) {
         writer.newNode(lxCalling_1, scope.moduleScope->constructor_message);
         writer.appendNode(lxResult);
         writer.closeNode();
      }
      else {
         MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::ScopeLevel::slMethod);

         compileDefConvConstructor(writer, node.parentNode(), *methodScope);
      }

      writer.newNode(lxImplicitJump, redirect.argument);
      writer.appendNode(lxCallTarget, scope.getClassRefId());
      writer.closeNode();
   }
   else {
      ExprScope exprScope(&scope);
      ObjectInfo target = mapTerminal(node, exprScope, EAttr::eaNone);
      if (target.kind == okInternal) {
         importCode(writer, node, exprScope, target.param, exprScope.getMessageID());
      }
      else scope.raiseError(errInvalidOperation, node);
   }
}

void Compiler :: compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope)
{
   mssg_t message = scope.getMessageID();
   ref_t overloadRef = classScope.info.methodHints.get(Attribute(message, maOverloadlist));
   if (!overloadRef)
      scope.raiseError(errIllegalOperation, node);

   LexicalType op = lxMultiDispatching;
   if (test(classScope.info.header.flags, elSealed) || test(message, STATIC_MESSAGE)) {
      op = lxSealedMultiDispatching;
   }

   if (node == lxResendExpression) {
      writer.appendNode(op, overloadRef);
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
         writer.newNode(lxDirectResending, node.argument & ~FUNCTION_MESSAGE);
         writer.appendNode(lxCurrent, 1);
         writer.closeNode();
      }
      else writer.appendNode(lxDirectResending, node.argument);
   }
   else writer.appendNode(op, overloadRef);
}

void Compiler :: compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& codeScope, bool multiMethod)
{
   if (node.firstChild() == lxImplicitJump) {
      SNode op = node.firstChild();
      writer.newNode(op.type, op.argument);
      writer.appendNode(lxCallTarget, op.findChild(lxCallTarget).argument);
      writer.closeNode();
   }
   else if (node.argument != 0 && multiMethod) {
      ClassScope* classScope = (ClassScope*)codeScope.getScope(Scope::ScopeLevel::slClass);

      compileMultidispatch(writer, node, codeScope, *classScope);
   }
   else {
      if (multiMethod) {
         ClassScope* classScope = (ClassScope*)codeScope.getScope(Scope::ScopeLevel::slClass);

         compileMultidispatch(writer, node.parentNode(), codeScope, *classScope);
      }

      bool silentMode = node.existChild(lxAutogenerated);

      SNode expr = node.firstChild(lxOpScopeMask);

      // new stack frame
      // stack already contains $self value
      writer.newNode(lxNewFrame, 0);
      codeScope.allocated1++;

      expr.insertNode(lxIdentifier, SELF_VAR);
      expr.insertNode(lxAttribute, V_MEMBER);

      writer.newNode(lxSeqExpression);
      ExprScope scope(&codeScope);
      EAttr resendMode = silentMode ? EAttr::eaSilent : EAttr::eaNone;
      if (expr == lxPropertyExpression)
         resendMode = resendMode | HINT_PROP_MODE;

      compileMessageExpression(writer, expr, scope, 0, resendMode);
      writer.closeNode();

      if (node.existChild(lxCode))
         scope.raiseError(errInvalidOperation, node);

      writer.closeNode();
   }
}

bool Compiler :: isMethodEmbeddable(MethodScope& scope)
{
   if (test(scope.hints, tpEmbeddable)) {
      ClassScope* ownerScope = (ClassScope*)scope.parent;

      return ownerScope->info.methodHints.exist(Attribute(scope.message, maEmbeddableRet));
   }
   else return false;
}

void Compiler :: compileEmbeddableMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
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

   if (scope.abstractMethod) {
      // COMPILER MAGIC : if the method retunging value can be passed as an extra argument
      compileAbstractMethod(writer, node, privateScope);
      compileAbstractMethod(writer, node, scope);
   }
   else {
//      if (scope.yieldMethod) {
//         compileYieldableMethod(cloneNode, privateScope);
//         //compileMethod(writer, node, privateScope);
//
//         compileYieldableMethod(node, scope);
//      }
//      else {
         compileMethod(writer, node, privateScope);

         SyntaxTree tempTree;
         generateResendToEmbeddable(tempTree, scope, privateScope.message, node.type);

         compileMethod(writer, tempTree.readRoot(), scope);
//      }
   }
}

void Compiler :: beginMethod(SyntaxWriter& writer, SNode node, MethodScope& scope, LexicalType type)
{
   writer.newNode(type, scope.message);

   declareProcedureDebugInfo(writer, node, scope, true, test(scope.getClassFlags(), elExtension));
}

void Compiler :: endMethod(SyntaxWriter& writer, MethodScope& scope)
{
   writer.appendNode(lxAllocated, scope.reserved1);  // allocate the space for the local variables
   writer.appendNode(lxReserved, scope.reserved2);

   writer.closeNode();
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

void Compiler :: compileMethodCode(SyntaxWriter& writer, SNode node, MethodScope& scope, CodeScope& codeScope)
{
   if (scope.multiMethod) {
      ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

      compileMultidispatch(writer, node, codeScope, *classScope);
   }

   int frameArg = scope.generic ? -1 : 0;
   writer.newNode(lxNewFrame, frameArg);

   // new stack frame
   // stack already contains current self reference
   // the original message should be restored if it is a generic method
   codeScope.allocated1++;
   // declare the current subject for a generic method
   if (scope.generic) {
      codeScope.allocated2++;
      codeScope.mapLocal(MESSAGE_VAR, -1, V_MESSAGE, 0, 0);
   }

   if (node == lxReturning)
      node = node.parentNode();

   ObjectInfo retVal = compileCode(writer, node, codeScope);

   // if the method returns itself
   if (retVal.kind == okUnknown && !codeScope.withRetStatement) {
      retVal = scope.mapSelf();

      // adding the code loading self / parameter (for set accessor)
      writer.newNode(lxReturning);
      ExprScope exprScope(&codeScope);
      //SNode exprNode = retNode.appendNode(lxExpression);
      writeTerminal(writer, retVal, exprScope);

      ref_t resultRef = scope.getReturningRef(false);
      if (resultRef != 0) {
         if (convertObject(writer, node, exprScope, resultRef, retVal, EAttr::eaNone).kind == okUnknown)
            scope.raiseError(errInvalidOperation, node.findChild(lxEOP));
      }
      writer.closeNode();
   }

   writer.closeNode();

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

void Compiler :: compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   beginMethod(writer, node, scope, node.type);

   CodeScope codeScope(&scope);

   SNode current = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression, lxNoBody);
   // check if it is a resend
   if (current == lxResendExpression) {
      compileResendExpression(writer, current, codeScope, scope.multiMethod);
      //scope.preallocated = 1;
   }
   // check if it is a dispatch
   else if (current == lxDispatchCode) {
      compileDispatchExpression(writer, current, codeScope, false);
   }
   else if (current == lxNoBody) {
      scope.raiseError(errNoBodyMethod, current);
   }
   else compileMethodCode(writer, current, scope, codeScope);

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

//void Compiler :: compileYieldDispatch(SNode node, MethodScope& scope)
//{
//   int size1 = scope.reserved1 - scope.preallocated;
//   int size2 = scope.reserved2;
//
//   int index = scope.getAttribute(maYieldContext);
//   int index2 = scope.getAttribute(maYieldLocals);
//
//   // dispatch
//   SNode dispNode = node.insertNode(lxYieldDispatch);
//   SNode contextExpr = dispNode.appendNode(lxFieldExpression);
//   contextExpr.appendNode(lxSelfLocal, 1);
//   contextExpr.appendNode(lxField, index);
//
//   // load context
//   if (size2 != 0) {
//      SNode exprNode = node.insertNode(lxExpression);
//      SNode copyNode = exprNode.appendNode(lxCopying, size2 << 2);
//      copyNode.appendNode(lxLocalAddress, -2);
//      SNode fieldNode = copyNode.appendNode(lxFieldExpression);
//      fieldNode.appendNode(lxSelfLocal, 1);
//      fieldNode.appendNode(lxField, index);
//      fieldNode.appendNode(lxFieldAddress, 4);
//   }
//
//   // load locals
//   if (size1 != 0) {
//      SNode expr2Node = node.insertNode(lxExpression);
//      SNode copy2Node = expr2Node.appendNode(lxCopying, size1 << 2);
//      copy2Node.appendNode(lxLocalAddress, scope.preallocated + size1);
//      SNode field2Node = copy2Node.appendNode(lxFieldExpression);
//      field2Node.appendNode(lxSelfLocal, 1);
//      field2Node.appendNode(lxField, index2);
//   }
//}
//
//void Compiler :: compileYieldableMethod(SNode node, MethodScope& scope)
//{
//   beginMethod(node, scope);
//
//   scope.preallocated = 0;
//
//   YieldScope yieldScope(&scope);
//   CodeScope codeScope(&yieldScope);
//
//   SNode body = node.findChild(lxCode, lxReturning, lxDispatchCode, lxResendExpression, lxNoBody);
//   if (body == lxCode) {
//      compileMethodCode(node, body, scope, codeScope);
//   }
//   else scope.raiseError(errInvalidOperation, body);
//
//   codeScope.syncStack(&scope);
//
//   compileYieldDispatch(body, scope);
//
//   endMethod(node, scope);
//
//   scope.addAttribute(maYieldContextLength, scope.reserved2 + 1);
//   scope.addAttribute(maYieldLocalLength, scope.reserved1 - scope.preallocated);
//
//   // HOTFIX : set correct context & locals offsets
//   //codeScope->reserved2 << 2
//   for (auto it = yieldScope.yieldLocals.start(); !it.Eof(); it++) {
//      SNode localNode = *it;
//      localNode.setArgument((*it).argument + scope.reserved1 - scope.preallocated);
//
//      SNode copyNode = localNode.parentNode();
//      if (copyNode == lxCopying) {
//         copyNode.setArgument((scope.reserved1 - scope.preallocated) << 2);
//      }
//   }
//   for (auto it = yieldScope.yieldContext.start(); !it.Eof(); it++) {
//      SNode copyNode = *it;
//
//      if (copyNode == lxCopying) {
//         copyNode.setArgument(scope.reserved2 << 2);
//      }
//   }
//}

void Compiler :: compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   writer.newNode(lxClassMethod, scope.message);

   SNode body = node.findChild(lxCode, lxNoBody);
   // abstract method should have an empty body
   if (body == lxNoBody) {
      // NOTE : abstract method should not have a body
   }
   else if (body != lxNone) {
      if (body.firstChild() == lxEOP) {
         scope.raiseWarning(WARNING_LEVEL_2, wrnAbstractMethodBody, body);
      }
      else scope.raiseError(errAbstractMethodCode, node);
   }
   else scope.raiseError(errAbstractMethodCode, node);

   writer.appendNode(lxNoBody);

   writer.closeNode();
}

void Compiler :: compileInitializer(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   SNode dummy;
   beginMethod(writer, dummy, scope, lxClassMethod);

   CodeScope codeScope(&scope);

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);
   if (checkMethod(*scope.moduleScope, classScope->info.header.parentRef, scope.message) != tpUnknown) {
      ref_t messageOwnerRef = resolveMessageOwnerReference(*scope.moduleScope, classScope->info, classScope->reference,
         scope.message, true);

      // check if the parent has implicit constructor - call it
      writer.newNode(lxDirectCalling, scope.message);
      writer.appendNode(lxResult);
      writer.appendNode(lxCallTarget, messageOwnerRef);
      writer.closeNode();
   }

   writer.newNode(lxNewFrame);

   // new stack frame
   // stack already contains current $self reference
   // the original message should be restored if it is a generic method
   codeScope.allocated1++;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current/*.compare(*/ == lxFieldInit/*, lxFieldAccum)*/) {
         SNode sourceNode = current.findChild(lxSourcePath);
         if (sourceNode != lxNone)
            declareCodeDebugInfo(writer, sourceNode, scope);

         compileRootExpression(writer, current.firstChild(lxObjectMask), codeScope, 0, HINT_ROOT);
      }

      current = current.nextNode();
   }

   writer.newNode(lxExpression);
   writer.appendNode(lxSelfLocal, 1);
   writer.closeNode();

   codeScope.syncStack(&scope);

   writer.closeNode();

   endMethod(writer, scope);
}

void Compiler :: compileDefConvConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::slClass);

   if (test(classScope->info.header.flags, elDynamicRole))
      throw InternalError("Invalid constructor");

   writer.newNode(lxSeqExpression);

   if (test(classScope->info.header.flags, elStructureRole)) {
      writer.newNode(lxCreatingStruct, classScope->info.size);
   }
   else writer.newNode(lxCreatingClass, classScope->info.fields.Count());

   writer.appendNode(lxType, classScope->reference);
   writer.closeNode();

   // call field initilizers if available for default constructor
   compileSpecialMethodCall(writer, *classScope, scope.moduleScope->init_message);

   writer.closeNode();
}

bool Compiler :: isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool& isProtectedDefConst)
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

void Compiler :: compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope)
{
   // if it is a default / conversion (unnamed) constructor
   bool isProtectedDefConst = false;
   bool isDefConvConstructor = isDefaultOrConversionConstructor(scope, scope.message, isProtectedDefConst);

   mssg_t defConstrMssg = scope.moduleScope->constructor_message;
   if (classClassScope.checkAttribute(defConstrMssg, maProtected)) {
      // if protected default constructor is declared - use it
      defConstrMssg = classClassScope.getAttribute(defConstrMssg, maProtected);
      isProtectedDefConst = true;
   }
   else if (classClassScope.info.methods.exist(defConstrMssg | STATIC_MESSAGE)) {
      // if private default constructor is declared - use it
      defConstrMssg = defConstrMssg | STATIC_MESSAGE;
   }

   writer.newNode(node.type, node.argument);
   declareProcedureDebugInfo(writer, node, scope, true, false);

   SNode attrNode = node.findChild(lxEmbeddableMssg);
   if (attrNode != lxNone) {
      // COMPILER MAGIC : copy an attribute so it will be recognized as embeddable call
      writer.appendNode(attrNode.type, attrNode.argument);
   }

   CodeScope codeScope(&scope);

   bool retExpr = false;
   bool withFrame = false;
   int classFlags = codeScope.getClassFlags();

   SNode bodyNode = node.findChild(lxResendExpression, lxCode, lxReturning, lxDispatchCode, lxNoBody);
   if (bodyNode == lxDispatchCode) {
      compileConstructorDispatchExpression(writer, bodyNode, codeScope, scope.message == defConstrMssg);

      codeScope.syncStack(&scope);
      endMethod(writer, scope);
      return;
   }
   else if (bodyNode == lxResendExpression) {
      if (scope.multiMethod && bodyNode.argument != 0) {
         compileMultidispatch(writer, bodyNode, codeScope, classClassScope);

         bodyNode = SNode();
      }
      else {
         if (isDefConvConstructor && getArgCount(scope.message) <= 1)
            scope.raiseError(errInvalidOperation, node);

         if (scope.multiMethod) {
            compileMultidispatch(writer, bodyNode.parentNode(), codeScope, classClassScope);
         }

         compileConstructorResendExpression(writer, bodyNode, codeScope, classClassScope, withFrame);

         bodyNode = bodyNode.findChild(lxCode);
      }
   }
   else if (bodyNode == lxReturning) {
      retExpr = true;
   }
   else if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
      // if it is a default / conversion (unnamed) constructor
      // it should create the object
      compileDefConvConstructor(writer, node, scope);
   }
   // if no redirect statement - call the default constructor
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(defConstrMssg)) {
      // HOTFIX : use dispatching routine for the protected default constructor
      writer.newNode(lxCalling_1, defConstrMssg);
      writer.appendNode(lxResult);
      writer.closeNode();
   }
   else if (!test(classFlags, elDynamicRole) && test(classFlags, elAbstract)) {
      // HOTFIX : allow to call the non-declared default constructor for abstract
      // class constructors
      writer.newNode(lxCalling_1, defConstrMssg);
      writer.appendNode(lxResult);
      writer.closeNode();
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node);

   if (bodyNode != lxNone) {
      if (!withFrame) {
         writer.newNode(lxNewFrame);

         withFrame = true;

         // new stack frame
         // stack already contains $self value
         codeScope.allocated1++;
      }

      if (retExpr) {
         writer.newNode(lxSeqExpression);

         ExprScope exprScope(&codeScope);
         ObjectInfo retVal = compileExpression(writer, bodyNode.firstChild(), exprScope, codeScope.getClassRefId(),
            HINT_DYNAMIC_OBJECT | HINT_NOPRIMITIVES | HINT_PARAMETER | HINT_ROOTEXPR, nullptr);

         if (boxingRequired(retVal))
            retVal = boxArgumentInPlace(writer, node, retVal, exprScope, false, false);

         if (retVal.kind != okObject)
            writeTerminal(writer, retVal, exprScope);

         writer.closeNode();
      }
      else {
         compileCode(writer, bodyNode, codeScope);

         // HOT FIX : returning the created object
         writer.newNode(lxExpression);
         writer.appendNode(lxLocal, 1);
         writer.closeNode();
      }
   }

   if (withFrame)
      writer.closeNode();

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, mssg_t message)
{
   if (classScope.info.methods.exist(message)) {
      if (classScope.info.methods.exist(message, true)) {
         // call the field in-place initialization
         writer.newNode(lxDirectCalling, message);
         writer.appendNode(lxResult);
         writer.appendNode(lxCallTarget, classScope.reference);
         writer.closeNode();
      }
      else {
         ref_t parentRef = classScope.info.header.parentRef;
         while (parentRef != 0) {
            // call the parent field in-place initialization
            ClassInfo parentInfo;
            _logic->defineClassInfo(*classScope.moduleScope, parentInfo, parentRef);

            if (parentInfo.methods.exist(message, true)) {
               writer.newNode(lxDirectCalling, message);
               writer.appendNode(lxResult);
               writer.appendNode(lxCallTarget, parentRef);
               writer.closeNode();

               break;
            }

            parentRef = parentInfo.header.parentRef;
         }
      }
   }
}

void Compiler :: compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope, bool exclusiveMode, bool ignoreAutoMultimethods)
{
   scope.withInitializers = scope.info.methods.exist(scope.moduleScope->init_message, true);

   SNode current = node.firstChild();

   while (current != lxNone) {
      switch(current) {
         case lxStaticFieldInit:
            compileCompileTimeAssigning(current, scope);
            if (exclusiveMode) {
               // HOTFIX : comment out to prevent duplicate compilation for nested classes
               current = lxIdle;
            }
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
            scope.moduleScope->printMessageInfo("method %s", methodScope.message);
#endif // FULL_OUTOUT_INFO

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
               compileDispatcher(writer, current, methodScope,
                  lxClassMethod,
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithVariadics));
            }
            // if it is a normal method
            else {
               declareArgumentList(current, methodScope, false, false);

               if (methodScope.abstractMethod) {
                  if (isMethodEmbeddable(methodScope)) {
                     compileEmbeddableMethod(writer, current, methodScope);
                  }
                  else compileAbstractMethod(writer, current, methodScope);
               }
               else if (isMethodEmbeddable(methodScope)) {
                  // COMPILER MAGIC : if the method retunging value can be passed as an extra argument
                  compileEmbeddableMethod(writer, current, methodScope);
               }
               //else if (methodScope.yieldMethod) {
               //   compileYieldableMethod(current, methodScope);
               //}
               else compileMethod(writer, current, methodScope);
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
      compileInitializer(writer, node, methodScope);
   }

   // if the VMT conatains newly defined generic handlers, overrides default one
   if (testany(scope.info.header.flags, elWithGenerics | elWithVariadics)
      && scope.info.methods.exist(scope.moduleScope->dispatch_message, false))
   {
      MethodScope methodScope(&scope);
      methodScope.message = scope.moduleScope->dispatch_message;

      scope.include(methodScope.message);

      scope.info.header.flags |= elWithCustomDispatcher;

      compileDispatcher(writer, SNode(), methodScope,
         lxClassMethod,
         test(scope.info.header.flags, elWithGenerics),
         test(scope.info.header.flags, elWithVariadics));

      // overwrite the class info
      scope.save();
   }
}

void Compiler :: compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current) {
         case lxConstructor:
         {
            MethodScope methodScope(&classScope);
            methodScope.message = current.argument;

#ifdef FULL_OUTOUT_INFO
            // info
            classClassScope.moduleScope->printMessageInfo("method %s", methodScope.message);
#endif // FULL_OUTOUT_INFO

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope, false, false);

            compileConstructor(writer, current, methodScope, classClassScope);
            break;
         }
         case lxStaticMethod:
         {
            MethodScope methodScope(&classClassScope);
            methodScope.message = current.argument;

            initialize(classClassScope, methodScope);
            declareArgumentList(current, methodScope, false, false);

            if (isMethodEmbeddable(methodScope)) {
               compileEmbeddableMethod(writer, current, methodScope);
            }
            else compileMethod(writer, current, methodScope);
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

      SNode methodNode = node.appendNode(lxStaticMethod, methodScope.message);
   
      classClassScope.info.header.flags |= elWithCustomDispatcher;

      compileDispatcher(writer, methodNode, methodScope,
         lxStaticMethod,
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

void Compiler :: generateClassFields(SNode node, ClassScope& scope, bool singleField)
{
   SNode current = node.firstChild();

   bool isClassClassMode = scope.classClassMode;
   while (current != lxNone) {
      if (current == lxClassField) {
         FieldAttributes attrs;
         declareFieldAttributes(current, scope, attrs);

         if (attrs.isStaticField || attrs.isConstAttr) {
            generateClassStaticField(scope, current, attrs.fieldRef, attrs.elementRef, attrs.isStaticField, attrs.isConstAttr, attrs.isArray);
         }
         else if (!isClassClassMode)
            generateClassField(scope, current, attrs, singleField);
      }
      current = current.nextNode();
   }
}

void Compiler :: compileSymbolCode(SyntaxTree& tree, ClassScope& scope)
{
   CommandTape tape;

   bool publicAttr = scope.info.mattributes.exist(Attribute(caSerializable, 0));

   SyntaxWriter writer(tree);
   generateClassSymbol(writer, scope);

   _writer.generateSymbol(tape, tree.readRoot(), false, INVALID_REF);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);

   compileSymbolAttribtes(*scope.moduleScope, scope.reference, publicAttr);
}

void Compiler :: compilePreloadedExtensionCode(ClassScope& scope)
{
   _Module* module = scope.moduleScope->module;

   IdentifierString sectionName("'", EXT_INITIALIZER_SECTION);

   CommandTape tape;
   _writer.generateInitializer(tape, module->mapReference(sectionName), lxClassSymbol, scope.reference);

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

void Compiler :: compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);
   writer.newNode(lxRoot);
   compileClassVMT(writer, node, classClassScope, classScope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), classClassScope);
}

void Compiler :: initialize(ClassScope& scope, MethodScope& methodScope)
{
   methodScope.hints = scope.info.methodHints.get(Attribute(methodScope.message, maHint));
   methodScope.outputRef = scope.info.methodHints.get(ClassInfo::Attribute(methodScope.message, maReference));
   if (test(methodScope.hints, tpInitializer))
      methodScope.scopeMode = methodScope.scopeMode | INITIALIZER_SCOPE;

   methodScope.classStacksafe= _logic->isStacksafeArg(scope.info);
   methodScope.withOpenArg = isOpenArg(methodScope.message);

   methodScope.extensionMode = scope.extensionClassRef != 0;
   methodScope.functionMode = test(methodScope.message, FUNCTION_MESSAGE);

   methodScope.multiMethod = _logic->isMultiMethod(scope.info, methodScope.message);
   methodScope.abstractMethod = _logic->isMethodAbstract(scope.info, methodScope.message);
//   methodScope.yieldMethod = _logic->isMethodYieldable(scope.info, methodScope.message);
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
      if (current/*.compare(*/ == lxFieldInit/*, lxFieldAccum)*/) {
         scope.withInitializers = true;
      }
      else if (current == lxClassMethod) {
         MethodScope methodScope(&scope);

         declareMethodAttributes(current, methodScope);

         if (current.argument == 0) {
            if (scope.extensionClassRef != 0)
               methodScope.extensionMode = true;

            // NOTE : an extension message must be strong-resolved
            declareArgumentList(current, methodScope, methodScope.extensionMode || test(scope.info.header.flags, elNestedClass), true);
            current.setArgument(methodScope.message);
         }
         else methodScope.message = current.argument;

         if (test(methodScope.hints, tpConstructor)) {
            if ((_logic->isAbstract(scope.info) || scope.abstractMode) && !methodScope.isPrivate()
               && !test(methodScope.hints, tpProtected))
            {
               // abstract class cannot have nonpublic constructors
               scope.raiseError(errIllegalConstructorAbstract, current);
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
         //else if (test(methodScope.hints, tpYieldable)) {
         //   scope.info.header.flags |= elWithYieldable;

         //   // HOTFIX : the class should have intializer method
         //   scope.withInitializers = true;
         //}

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
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassFlag) {
         scope.info.header.flags |= current.argument;
      }

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
         SNode terminalNode = current.firstChild(lxObjectMask).firstChild(lxTerminalMask);
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
               SNode assignNode = initNode.firstChild(lxObjectMask).firstChild(lxObjectMask);
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
      }
   }
}

inline SNode findName(SNode node)
{
   return node.findChild(lxNameAttr).firstChild(lxTerminalMask);
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SNode node, mssg_t message, bool allowTypeAttribute)
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
      mssg_t publicMessage = 0;
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

void Compiler :: saveExtension(ClassScope& scope, mssg_t message, bool internalOne)
{
   mssg_t extensionMessage = 0;

   // get generic message
   ref_t signRef = 0;
   ident_t actionName = scope.module->resolveAction(getAction(message), signRef);
   if (signRef) {
      extensionMessage = overwriteAction(message, scope.module->mapAction(actionName, 0, false));
      if ((extensionMessage & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE)
         extensionMessage = overwriteArgCount(extensionMessage, 2);
   }
   else extensionMessage = message;

   // exclude function flag
   extensionMessage = extensionMessage & ~FUNCTION_MESSAGE;

   NamespaceScope* nsScope = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::slNamespace);
   nsScope->saveExtension(extensionMessage, scope.reference, message, internalOne);
}

void Compiler :: predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope)
{
   SNode body = node.findChild(lxCode, lxNoBody);
   if (body == lxNoBody) {
   }
   else if (body != lxCode || body.firstChild() != lxEOP)
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

inline bool checkNonpublicDuplicates(ClassInfo& info, mssg_t publicMessage)
{
   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      Attribute key = it.key();
      if (key.value1 == publicMessage && (key.value2 == maPrivate || key.value2 == maProtected || key.value2 == maInternal))
         return true;
   }

   return false;
}

//inline bool isDeclaredProtected(ClassInfo& info, mssg_t publicMessage, mssg_t& protectedMessage)
//{
//   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
//      Attribute key = it.key();
//      if (key.value1 == publicMessage && (key.value2 == maProtected)) {
//         protectedMessage = *it;
//
//         return true;
//      }
//   }
//
//   return false;
//}

void Compiler :: generateParamNameInfo(ClassScope& scope, SNode node, mssg_t message)
{
   SNode current = node.findChild(lxMethodParameter);
   while (current != lxNone) {
      if (current == lxMethodParameter) {
         ident_t terminal = current.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();

         Attribute key(caParamName, message);
         scope.info.mattributes.add(key, saveMetaInfo(*scope.moduleScope, terminal));

      }
      current = current.nextNode();
   }
}

void Compiler :: generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed,
   bool allowTypeAttribute)
{
   mssg_t message = current.argument;

   if (scope.info.methods.exist(message, true) && hideDuplicates) {
      // ignoring autogenerated duplicates
      current = lxIdle;

      return;
   }

   generateMethodAttributes(scope, current, message, allowTypeAttribute);
   generateParamNameInfo(scope, current, message);

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
      if (!included && sealedMethod) {
         scope.raiseError(errClosedMethod, findParent(current, lxClass/*, lxNestedClass*/));
      }

      // HOTFIX : make sure there are no duplicity between public and private / internal / statically linked ones
      if (!test(message, STATIC_MESSAGE)) {
         if (scope.info.methods.exist(message | STATIC_MESSAGE))
            scope.raiseError(errDuplicatedMethod, current);
      }
      if (!privateOne && !testany(methodHints, tpInternal | tpProtected)) {
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
         // HOTFIX : ignore auto generated multi-methods
         if (!current.existChild(lxAutoMultimethod))
            saveExtension(scope, message, scope.visibility != Visibility::Public);
      }

      if (!closed && test(methodHints, tpEmbeddable)
         && !testany(methodHints, tpDispatcher | tpFunction | tpConstructor | tpConversion | tpGeneric | tpCast)
         && ((message & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE)
         && !current.existChild(lxDispatchCode, lxResendExpression))
      {
         // COMPILER MAGIC : if embeddable returning argument is allowed
         ref_t outputRef = scope.info.methodHints.get(Attribute(message, maReference));

         bool embeddable = false;
         bool multiret = false;
         if (outputRef == scope.reference && _logic->isEmbeddable(scope.info)) {
            embeddable = true;
         }
         else if (_logic->isEmbeddable(*scope.moduleScope, outputRef)) {
            embeddable = true;
         }
         else if (test(methodHints, tpMultiRetVal)) {
            // supporting return value dispatching for dynamic types as well
            // when it is required
            multiret = true;
         }

         if (embeddable) {
            ref_t dummy, flags;
            pos_t argCount;
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
               mssg_t embeddableMessage = encodeMessage(
                  scope.module->mapAction(privateName.c_str(), scope.module->mapSignature(signArgs, signLen, false), false),
                  argCount + 1,
                  flags);

               //if (!test(scope.info.header.flags, elSealed) || scope.info.methods.exist(embeddableMessage)) {
               //   scope.include(embeddableMessage);
               //}
               //else embeddableMessage |= STATIC_MESSAGE;

               if (test(scope.info.header.flags, elSealed) && !scope.info.methods.exist(embeddableMessage))
                  embeddableMessage |= STATIC_MESSAGE;

               scope.include(embeddableMessage);

               scope.addAttribute(message, maEmbeddableRet, embeddableMessage);
               if (_logic->isStacksafeArg(scope.info))
                  scope.addHint(embeddableMessage, tpStackSafe);
            }
         }
         else if (multiret) {
            ref_t actionRef, flags;
            ref_t signRef = 0;
            pos_t argCount;
            decodeMessage(message, actionRef, argCount, flags);

            ident_t actionName = scope.module->resolveAction(actionRef, signRef);

            if (argCount > 1 && signRef != 0) {
               ref_t signArgs[ARG_COUNT];
               size_t signLen = scope.module->resolveSignature(signRef, signArgs);
               signLen--;
               argCount--;

               if (signLen > 0) {
                  signRef = scope.module->mapSignature(signArgs, signLen, false);
               }
               else signRef = 0;

               mssg_t targetMessage = encodeMessage(
                  scope.module->mapAction(actionName, signRef, false),
                  argCount,
                  flags);

               scope.addAttribute(targetMessage, maEmbeddableRet, message);
            }
            else scope.raiseError(errInvalidHint, current);
         }
      }
   }
}

mssg_t Compiler :: resolveMultimethod(ClassScope& scope, mssg_t messageRef)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(messageRef, actionRef, argCount, flags);

   if (test(messageRef, FUNCTION_MESSAGE)) {
      if (argCount == 0)
         return 0;
   }
   else if (argCount == 1)
      return 0;

   ident_t actionStr = scope.module->resolveAction(actionRef, signRef);

   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      // COMPILER MAGIC : for variadic message - use the most general message
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);

      int genericArgCount = 2;
      // HOTFIX : a variadic extension is a special case of variadic function
      // - so the target should be included as well
      if (test(messageRef, FUNCTION_MESSAGE) && scope.extensionClassRef == 0)
         genericArgCount = 1;

      mssg_t genericMessage = encodeMessage(genericActionRef, genericArgCount, flags);

      return genericMessage;
   }
   else if (signRef) {
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);
      mssg_t genericMessage = encodeMessage(genericActionRef, argCount, flags);

      return genericMessage;
   }

   return 0;
}

void Compiler :: generateMethodDeclarations(SNode root, ClassScope& scope, bool closed, LexicalType methodType,
   bool allowTypeAttribute)
{
   bool templateMethods = false;
   List<mssg_t> implicitMultimethods;

   // first pass - mark all multi-methods
   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == methodType) {
         mssg_t multiMethod = resolveMultimethod(scope, current.argument);
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
      _logic->injectVirtualMultimethods(*scope.moduleScope, root, *this, implicitMultimethods, methodType, scope.info);
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

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, node);
         }
      }
      else if (current.compare(lxType, lxArrayType)) {
         // if it is a type attribute
         scope.outputRef = resolveTypeAttribute(current, scope, true, false);
      }
      else if (current == lxNameAttr && !explicitMode) {
         // resolving implicit method attributes
         int attr = scope.moduleScope->attributes.get(current.firstChild(lxTerminalMask).identifier());
         if (_logic->validateImplicitMethodAttribute(attr, false)) {
            scope.hints |= attr;
            current.set(lxAttribute, attr);
         }
      }

      current = current.nextNode();
   }
}

////inline SNode findBaseParent(SNode node)
////{
////   SNode baseNode = node.findChild(lxBaseParent);
////   //if (baseNode != lxNone) {
////   //   if (baseNode.argument == -1 && existChildWithArg(node, lxBaseParent, 0u)) {
////   //      // HOTFIX : allow to override the template parent
////   //      baseNode = lxIdle;
////   //      baseNode = node.findChild(lxBaseParent);
////   //   }
////   //}
////
////   return baseNode;
////}

ref_t Compiler :: resolveParentRef(SNode node, Scope& scope, bool silentMode)
{
   ref_t parentRef = 0;
   if (node == lxNone) {
   }
   else {
      SNode typeNode = node == lxType ? node : node.findChild(lxType);
      parentRef = resolveTypeAttribute(typeNode, scope, silentMode, false);
   }

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
   compileParentDeclaration(node.findChild(lxParent), scope, extensionDeclaration, lxParent);

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
      scope.classClassMode ? isClassClassMethod : isClassMethod/*, _stackEvenMode*/);

   // optimize
   optimizeTape(tape);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope)
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

   expressionTree.clear();

   SyntaxWriter writer(expressionTree);
   writer.newNode(lxRoot);
   compileVMT(writer, node, scope);
   writer.closeNode();

   generateClassImplementation(expressionTree.readRoot(), scope);

   // compile explicit symbol
   // extension cannot be used stand-alone, so the symbol should not be generated
   if (scope.extensionClassRef == 0 && scope.info.header.classRef != 0) {
      expressionTree.clear();

      compileSymbolCode(expressionTree, scope);
   }
}

void Compiler :: compileSymbolDeclaration(SNode node, SymbolScope& scope)
{
   declareSymbolAttributes(node, scope, true, false);

   scope.save();
}

bool Compiler :: compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal/*, bool accumulatorMode, ref_t accumulatorRef*/)
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

         nsScope->defineConstantSymbol(classRef, parentRef);
      }
      else if (retVal.kind == okArrayConst) {
         scope.info.type = SymbolExpressionInfo::Type::ArrayConst;
         scope.info.exprRef = classRef;
         scope.info.typeRef = parentRef;
      }

      return true;
   }

   _Module* module = scope.moduleScope->module;
   MemoryWriter dataWriter(module->mapSection(scope.reference | mskRDataRef, false));

   //if (accumulatorMode) {
   //   if (dataWriter.Position() == 0 && ((Section*)dataWriter.Memory())->References().Eof()) {
   //      dataWriter.Memory()->addReference(accumulatorRef | mskVMTRef, (ref_t)-4);
   //   }

   //   if (retVal.kind == okLiteralConstant) {
   //      dataWriter.Memory()->addReference(retVal.param | mskLiteralRef, dataWriter.Position());

   //      dataWriter.writeDWord(0);
   //   }
   //   else if (retVal.kind == okWideLiteralConstant) {
   //      dataWriter.Memory()->addReference(retVal.param | mskWideLiteralRef, dataWriter.Position());

   //      dataWriter.writeDWord(0);
   //   }
   //   else if (retVal.kind == okMessageConstant) {
   //      dataWriter.Memory()->addReference(retVal.param | mskMessage, dataWriter.Position());

   //      dataWriter.writeDWord(0);
   //   }
   //   else if (retVal.kind == okMessageNameConstant) {
   //      dataWriter.Memory()->addReference(retVal.param | mskMessageName, dataWriter.Position());

   //      dataWriter.writeDWord(0);
   //   }
   //   else if (retVal.kind == okClass || retVal.kind == okClassSelf) {
   //      dataWriter.Memory()->addReference(retVal.param | mskVMTRef, dataWriter.Position());
   //      dataWriter.writeDWord(0);
   //   }
   //   else {
   //      SymbolScope memberScope(nsScope, nsScope->moduleScope->mapAnonymous(), Visibility::Public);
   //      if (!compileSymbolConstant(memberScope, retVal, false, 0))
   //         return false;

   //      dataWriter.Memory()->addReference(memberScope.reference | mskConstantRef, dataWriter.Position());
   //      dataWriter.writeDWord(0);
   //   }

   //   return true;
   //}
   //else {
      if (dataWriter.Position() > 0)
         return false;

      if (retVal.kind == okIntConstant || retVal.kind == okUIntConstant) {
         nsScope->defineIntConstant(scope.reference, retVal.extraparam);

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
//   }

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

void Compiler :: compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope)
{
   expressionTree.clear();

   SyntaxWriter writer(expressionTree);
   writer.newNode(lxSymbol, node.argument);
   writer.newNode(lxNewFrame);

   bool isStatic = scope.staticOne;

   SNode expression = node.findChild(lxExpression);

   EAttr exprMode = scope.info.type == SymbolExpressionInfo::Type::Constant ? HINT_CONSTEXPR : EAttr::eaNone;

   CodeScope codeScope(&scope);
   ExprScope exprScope(&codeScope);
   // HOTFIX : due to implementation (compileSymbolConstant requires constant types) typecast should be done explicitly
   ObjectInfo retVal = compileExpression(writer, expression, exprScope, 0,
      exprMode | HINT_ROOTSYMBOL, nullptr);

   if (scope.info.exprRef == 0) {
      ref_t ref = resolveObjectReference(scope, retVal, true);
      if (ref != 0 && scope.info.type != SymbolExpressionInfo::Type::Normal) {
         // HOTFIX : if the result of the operation is qualified - it should be saved as symbol type
         scope.info.exprRef = ref;
      }
   }
   else retVal = convertObject(writer, expression, exprScope, scope.info.exprRef, retVal, EAttr::eaNone);

   writer.closeNode();
   writer.closeNode();

   if (codeScope.reserved1 > 0 || codeScope.reserved2 > 0) {
      SNode root = expressionTree.readRoot();

      root.appendNode(lxAllocated, codeScope.reserved1);
      root.appendNode(lxReserved, codeScope.reserved2);
   }

   analizeSymbolTree(expressionTree.readRoot().firstChild(), scope);

   // create constant if required
   if (scope.info.type == SymbolExpressionInfo::Type::Constant) {
      // static symbol cannot be constant
      if (isStatic)
         scope.raiseError(errInvalidOperation, expression);

      if (!compileSymbolConstant(scope, retVal/*, false, 0*/))
         scope.raiseError(errInvalidOperation, expression);
   }

   scope.save();

   if (scope.preloaded) {
      compilePreloadedCode(scope);
   }

   pos_t sourcePathRef = scope.saveSourcePath(_writer);
   CommandTape tape;
   _writer.generateSymbol(tape, expressionTree.readRoot(), isStatic, sourcePathRef);

   // optimize
   optimizeTape(tape);

//   compileSymbolAttribtes(*scope.moduleScope, scope.reference, scope.visibility == Visibility::Public);

   // create byte code sections
   _writer.saveTape(tape, *scope.moduleScope);
}

void Compiler :: compileStaticAssigning(SNode node, ClassScope& scope/*, int mode*/)
{
   SyntaxTree expressionTree;
   SyntaxWriter writer(expressionTree);

   CodeScope codeScope(&scope);
   ExprScope exprScope(&codeScope);

   writer.newNode(lxRoot);
   writer.newNode(lxExpression);

   ObjectInfo source = compileExpression(writer, node, exprScope, /*target.extraparam*/0, HINT_NODEBUGINFO, nullptr);

   writer.closeNode();
   writer.closeNode();

   analizeSymbolTree(expressionTree.readRoot(), scope);

   ref_t actionRef = compileClassPreloadedCode(*scope.moduleScope, scope.reference, expressionTree.readRoot());
   scope.info.mattributes.add(Attribute(caInitializer, 0), actionRef);
   scope.save();
}

ref_t targetResolver(void* param, mssg_t mssg)
{
   return ((Map<ref_t, ref_t>*)param)->get(mssg);
}

void Compiler :: compileModuleExtensionDispatcher(NamespaceScope& scope)
{
   List<mssg_t> genericMethods;
   ClassInfo::CategoryInfoMap methods(0);
   Map<ref_t, ref_t> taregts;

   auto it = scope.declaredExtensions.start();
   while (!it.Eof()) {
      auto extInfo = *it;
      mssg_t genericMessageRef = it.key();

      ident_t refName = scope.module->resolveReference(extInfo.value1);
      if (isWeakReference(refName)) {
         if (NamespaceName::compare(refName, scope.ns)) {
            // if the extension is declared in the module namespace
            // add it to the list to be generated

            if (retrieveIndex(genericMethods.start(), genericMessageRef) == -1)
               genericMethods.add(genericMessageRef);

            methods.add(Attribute(extInfo.value2, maMultimethod), genericMessageRef | FUNCTION_MESSAGE);
            taregts.add(extInfo.value2, extInfo.value1);
         }
      }

      it++;
   }

   if (genericMethods.Count() > 0) {
      // if there are extension methods in the namespace
      ref_t extRef = scope.moduleScope->mapAnonymous();
      ClassScope classScope(&scope, extRef, Visibility::Private);
      classScope.extensionDispatcher = true;

      // declare the extension
      SyntaxTree classTree;
      SyntaxWriter writer(classTree);

      // build the class tree
      writer.newNode(lxRoot);
      writer.newNode(lxClass, extRef);
      writer.closeNode();
      writer.closeNode();

      SNode classNode = classTree.readRoot().firstChild();
      compileParentDeclaration(classNode, classScope, scope.moduleScope->superReference);
      classScope.info.header.flags |= (elExtension | elSealed);
      classScope.info.header.classRef = classScope.reference;
      classScope.extensionClassRef = scope.moduleScope->superReference;
      classScope.info.fieldTypes.add(-1, ClassInfo::FieldInfo(classScope.extensionClassRef, 0));

      for (auto g_it = genericMethods.start(); !g_it.Eof(); g_it++) {
         mssg_t genericMessageRef = *g_it;

         ref_t dispatchListRef = _logic->generateOverloadList(*scope.moduleScope, *this, genericMessageRef | FUNCTION_MESSAGE,
            methods, (void*)&taregts, targetResolver, elSealed);

         classScope.info.mattributes.add(Attribute(caExtOverloadlist, genericMessageRef), dispatchListRef);
      }

      classScope.save();

      // compile the extension
      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);

      compileVMT(bufferWriter, classNode, classScope);
      generateClassImplementation(buffer.readRoot(), classScope);

      compilePreloadedExtensionCode(classScope);
   }
}

ref_t Compiler :: compileExtensionDispatcher(NamespaceScope& scope, mssg_t genericMessageRef)
{
   ref_t extRef = scope.moduleScope->mapAnonymous();
   ClassScope classScope(&scope, extRef, Visibility::Private);
   classScope.extensionDispatcher = true;

   // create a new overload list
   ClassInfo::CategoryInfoMap methods(0);
   Map<ref_t, ref_t> targets;
   auto it = scope.extensions.getIt(genericMessageRef);
   while (!it.Eof()) {
      auto extInfo = *it;

      methods.add(Attribute(extInfo.value2, maMultimethod), genericMessageRef | FUNCTION_MESSAGE);
      targets.add(extInfo.value2, extInfo.value1);

      it = scope.extensions.nextIt(genericMessageRef, it);
   }

   ref_t dispatchListRef = _logic->generateOverloadList(*scope.moduleScope, *this, genericMessageRef | FUNCTION_MESSAGE, methods,
      (void*)&targets, targetResolver, elSealed);

   SyntaxTree classTree;
   SyntaxWriter writer(classTree);

   // build the class tree
   writer.newNode(lxRoot);
   writer.newNode(lxClass, extRef);

   injectVirtualMultimethod(*scope.moduleScope, writer.CurrentNode(),
      genericMessageRef | FUNCTION_MESSAGE, lxClassMethod, genericMessageRef, false, 0);

   SNode classNode = writer.CurrentNode();

   writer.closeNode();

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
   // !!NOTE : we create paralel class node for the code tree generation
   writer.newNode(lxClass, extRef);
   compileVMT(writer, classNode, classScope);

   // keep the command tree node
   classNode = writer.CurrentNode();
   writer.closeNode();

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

int Compiler :: allocateStructure(SNode node, int alignment, int& size)
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
   int offset = allocateStructure(false, alignment, size, reserved);

   // HOT FIX : size should be in bytes
   size *= 4;

   reserveNode.setArgument(reserved);

   return offset;
}

//inline SNode injectRootSeqExpression(SNode& parent)
//{
//   SNode current;
//   while (!parent.compare(lxNewFrame, lxCodeExpression, lxCode/*, lxReturning*/)) {
//      current = parent;
//      parent = parent.parentNode();
//
//      if (current == lxSeqExpression && current.argument == -2) {
//         // HOTFIX : to correctly unbox the variable in some cases (e.g. ret value dispatching)
//         return current;
//      }
//   }
//
//   if (current != lxSeqExpression) {
//      current.injectAndReplaceNode(lxSeqExpression);
//   }
//
//   return current;
//}
//
//void Compiler :: injectIndexBoxing(SNode node, SNode objNode, ExprScope& scope)
//{
//   ref_t typeRef = node.findChild(lxType).argument;
//   int size = node.findChild(lxSize).argument;
//
//   if (typeRef == V_DWORD)
//      typeRef = scope.moduleScope->intReference;
//
//   objNode.injectAndReplaceNode(lxSaving, size);
//
//   SNode newNode = objNode.insertNode(lxCreatingStruct, size);
//   newNode.appendNode(lxType, typeRef);
//}
//
//void Compiler :: boxExpressionInRoot(SNode node, SNode objNode, ExprScope& scope, LexicalType tempType,
//   int tempLocal, bool localBoxingMode, bool condBoxing)
//{
//   SNode parent = node;
//   SNode current = injectRootSeqExpression(parent);
//   SNode boxingNode = current;
//   if (condBoxing)
//      boxingNode = current.insertNode(lxCondBoxing);
//
//   ref_t typeRef = node.findChild(lxType).argument;
//   int size = node.findChild(lxSize).argument;
//   bool isVariable = node.argument == INVALID_REF;
//   bool variadic = node == lxArgBoxableExpression;
//   bool primArray = node == lxPrimArrBoxableExpression;
//   if (typeRef != 0) {
//      bool fixedSizeArray = isPrimitiveArrRef(typeRef) && size > 0;
//
//      if (isPrimitiveRef(typeRef)) {
//         ref_t elementRef = node.findChild(lxElementType).argument;
//
//         typeRef = resolvePrimitiveReference(scope, typeRef, elementRef, false);
//      }
//
//      SNode copyNode = boxingNode.insertNode(objNode.type, objNode.argument);
//      if (test(objNode.type, lxOpScopeMask))
//         SyntaxTree::copyNode(objNode, copyNode);
//
//      if (size < 0 || primArray) {
//         injectCopying(copyNode, size, variadic, primArray);
//
//         copyNode.appendNode(lxType, typeRef);
//
//         copyNode.injectAndReplaceNode(lxAssigning);
//         copyNode.insertNode(tempType, tempLocal);
//      }
//      else {
//         SNode assignNode = boxingNode.insertNode(lxAssigning);
//         assignNode.appendNode(tempType, tempLocal);
//
//         if (localBoxingMode) {
//            copyNode.injectAndReplaceNode(lxCopying, size);
//
//            // inject local boxed object
//            ObjectInfo tempBuffer;
//            allocateTempStructure(scope, size, fixedSizeArray, tempBuffer);
//
//            assignNode.appendNode(lxLocalAddress, tempBuffer.param);
//            copyNode.insertNode(lxLocalAddress, tempBuffer.param);
//         }
//         else {
//            injectCreating(assignNode, objNode, scope, false, size, typeRef, variadic);
//            injectCopying(copyNode, size, variadic, primArray);
//
//            copyNode.insertNode(tempType, tempLocal);
//         }
//      }
//
//      if (isVariable) {
//         SNode unboxing = current.appendNode(lxCopying, size);
//         if (size < 0 || primArray) {
//            unboxing.set(lxCloning, 0);
//         }
//         else if (condBoxing)
//            unboxing.set(lxCondCopying, size);
//
//         SyntaxTree::copyNode(objNode, unboxing.appendNode(objNode.type, objNode.argument));
//         if (size == 0 && !primArray) {
//            // HOTFIX : if it is byref variable unboxing
//            unboxing.set(lxAssigning, 0);
//
//            SNode unboxingByRef = unboxing.appendNode(lxFieldExpression);
//            unboxingByRef.appendNode(tempType, tempLocal);
//            unboxingByRef.appendNode(lxField);
//         }
//         else unboxing.appendNode(tempType, tempLocal);
//      }
//
//      // replace object with a temp local
//      objNode.set(tempType, tempLocal);
//   }
//   else scope.raiseError(errInvalidBoxing, node);
//}

bool Compiler :: optimizeEmbeddable(_ModuleScope& scope, SNode& node)
{
   bool applied = false;

   // verify the path
   SNode callNode = node.parentNode();
   SNode rootNode = callNode.parentNode();
   SNode assignNode = callNode.nextNode();
   SNode copyNode = assignNode.nextNode();

   if (callNode.compare(lxDirectCalling, lxSDirectCalling) && assignNode == lxAssigning && copyNode == lxCopying) {
      applied = _logic->optimizeEmbeddableOp(scope, *this, rootNode);
   }
   else if (_logic->optimizeEmbeddable(callNode, scope)) {
      applied = true;
   }

   return applied;
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

////inline bool existSubNode(SNode node, SNode target, bool skipFirstOpArg)
////{
////   SNode current = node.firstChild(lxObjectMask);
////
////   if (skipFirstOpArg && test(node.type, lxPrimitiveOpMask))
////      current = current.nextNode(lxObjectMask);
////
////   while (current != lxNone) {
////      if (current.type == target.type && current.argument == target.argument) {
////         return true;
////      }
////      else if (existSubNode(current, target, false))
////         return true;
////
////      current = current.nextNode(lxObjectMask);
////   }
////
////   return false;
////}
//
//inline bool compareNodes(SNode node, SNode target)
//{
//   return (node.type == target.type && node.argument == target.argument);
//}
//
//inline bool existsNode(SNode node, SNode target)
//{
//   if (compareNodes(node, target))
//      return true;
//
//   SNode current = node.firstChild(lxObjectMask);
//   while (current != lxNone) {
//      if (existsNode(current, target))
//         return true;
//
//      current = current.nextNode(lxObjectMask);
//   }
//
//   return false;
//}
//
//inline void commentNode(SNode& node)
//{
//   node = lxIdle;
//   SNode parent = node.parentNode();
//   while (parent == lxExpression) {
//      parent = lxIdle;
//      parent = parent.parentNode();
//   }
//
//}
//
//bool Compiler :: optimizeOpDoubleAssigning(_ModuleScope&, SNode& node)
//{
//   bool applied = false;
//
//   // seqNode
//   SNode seqNode = node.parentNode();
//   SNode seqRet = seqNode.lastChild(lxObjectMask);
//   if (seqRet == lxExpression)
//      seqRet = seqRet.findSubNodeMask(lxObjectMask);
//
//   // opNode
//   SNode opNode = node.nextNode(lxObjectMask);
//   SNode larg = opNode.firstChild(lxObjectMask);
//   SNode rarg = larg.nextNode(lxObjectMask);
//
//   // target
//   SNode targetCopying = seqNode.parentNode();
//   while (targetCopying == lxExpression)
//      targetCopying = targetCopying.parentNode();
//   SNode target = targetCopying.findSubNodeMask(lxObjectMask);
//   if (target != lxLocalAddress)
//      return false;
//
//   SNode temp = node.firstChild(lxObjectMask);
//   SNode tempSrc = temp.nextNode(lxObjectMask);
//   if (tempSrc == lxExpression)
//      tempSrc = tempSrc.findSubNodeMask(lxObjectMask);
//
//   // validate if the target is not used in roperand
//   if (existsNode(rarg, target))
//      return false;
//
//   // validate if temp is used in op and is returned in seq
//   if (compareNodes(seqRet, temp) && compareNodes(larg, temp)) {
//      temp.set(target.type, target.argument);
//      seqRet.set(target.type, target.argument);
//      larg.set(target.type, target.argument);
//      if (compareNodes(tempSrc, target)) {
//         node = lxIdle;
//      }
//
//      commentNode(target);
//
//      targetCopying = lxExpression;
//
//      applied = true;
//   }
//
//   return applied;
//}

bool Compiler :: optimizeTempAllocating(_ModuleScope& scope, SNode& node)
{
   SNode copyNode = node.parentNode();
   SNode copy2Node = copyNode.nextNode(lxObjectMask);
   SNode assignNode = copyNode.prevNode();

   if (copyNode.type == copy2Node.type && copyNode.argument == copy2Node.argument) {
      SNode temp = node;
      SNode target = copy2Node.firstChild(lxObjectMask);
      SNode temp2 = target.nextNode(lxObjectMask);
      if (target == lxLocalAddress && temp2.compare(lxLocal, lxTempLocal) && temp2.argument == temp.argument) {
         temp.set(target.type, target.argument);
         copy2Node = lxIdle;

         if (assignNode == lxAssigning) {
            SNode assignTarget = assignNode.firstChild(lxObjectMask);
            if (assignTarget == lxTempLocal && assignTarget.argument == temp2.argument) {
               assignNode = lxIdle;
            }
         }

         return true;
      }
   }

   return false;
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

//inline void commetNode(SNode& node)
//{
//   node = lxIdle;
//   SNode parent = node.parentNode();
//   while (parent == lxExpression) {
//      parent = lxIdle;
//
//      parent = parent.parentNode();
//   }
//}
//
//bool Compiler :: optimizeCallDoubleAssigning(_ModuleScope&, SNode& node)
//{
//   SNode callNode = node.parentNode();
//   SNode seqNode = callNode.parentNode();
//   while (seqNode == lxExpression)
//      seqNode = seqNode.parentNode();
//
//   if (seqNode != lxSeqExpression)
//      return false;
//
//   SNode copyNode = seqNode.parentNode();
//   while (copyNode == lxExpression)
//      copyNode = copyNode.parentNode();
//
//   SNode src = copyNode.findSubNodeMask(lxObjectMask);
//   if (src.type != lxLocalAddress) {
//      node = lxIdle;
//
//      return false;
//   }
//
//   SNode temp = callNode.lastChild(lxObjectMask);
//   if (temp == lxExpression)
//      temp = temp.findSubNodeMask(lxObjectMask);
//   SNode seqRes = seqNode.lastChild(lxObjectMask);
//   if (seqRes == lxExpression)
//      seqRes = seqRes.findSubNodeMask(lxObjectMask);
//
//   if (temp == lxLocalAddress && compareNodes(seqRes, temp)) {
//      temp.setArgument(src.argument);
//      seqRes.setArgument(src.argument);
//
//      commentNode(src);
//      copyNode = lxExpression;
//
//      node = lxIdle;
//
//      return true;
//   }
//
//   return false;
//}
//
//inline void commentArgs(SNode opNode)
//{
//   SNode argNode = opNode.firstChild(lxObjectMask);
//   argNode = argNode.nextNode(lxObjectMask);
//   while (argNode != lxNone) {
//      argNode = lxIdle;
//      argNode = argNode.nextNode(lxObjectMask);
//   }
//}
//
//inline void replaceCallToResend(SNode& node)
//{
//   switch (node.type) {
//      case lxCalling_0:
//         node = lxResending;
//         break;
//      case lxDirectCalling:
//         node = lxDirectResending;
//         break;
//      case lxSDirectCalling:
//         node = lxSDirectResending;
//         break;
//   }
//}
//
//bool Compiler :: optimizeDispatchingExpr(_ModuleScope&, SNode& node)
//{
//   bool valid = false;
//   SNode opNode;
//   if (node == lxSelfLocal) {
//      node = lxIdle;
//      valid = true;
//   }
//
//   if (valid) {
//      // comment out a frame, replace operation with resend
//      SNode current = node.parentNode();
//      while (current != lxNone) {
//         switch (current.type) {
//            case lxCalling_0:
//            case lxDirectCalling:
//            case lxSDirectCalling:
//               commentArgs(current);
//               replaceCallToResend(current);
//               break;
//            case lxNewFrame:
//               current = lxExpression;
//               return true;
//         }
//         current = current.parentNode();
//      }
//   }
//
//   return valid;
//}

bool Compiler :: optimizeTriePattern(_ModuleScope& scope, SNode& node, int patternId)
{
   switch (patternId) {
      case 1:
         return optimizeConstProperty(scope, node);
      case 2:
         return optimizeEmbeddable(scope, node);
      case 3:
         return optimizeTempAllocating(scope, node);
      case 4:
         return optimizeBranching(scope, node);
      case 5:
         return optimizeConstantAssigning(scope, node);
//      case 6:
//         return optimizeOpDoubleAssigning(scope, node);
//      case 7:
//         return optimizeCallDoubleAssigning(scope, node);
//      case 8:
//         return optimizeDispatchingExpr(scope, node);
      default:
         break;
   }

   return false;
}

bool Compiler :: matchTriePatterns(_ModuleScope& scope, SNode& node, SyntaxTrie& trie, List<SyntaxTrieNode>& matchedPatterns)
{
   List<SyntaxTrieNode> nextPatterns;
   if (test(node.type, lxOpScopeMask)) {
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
      if (current.compare(lxNewFrame, lxDispatching)) {
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
   mssg_t message = 0;
   if (_logic->recognizeEmbeddableMessageCall(methodNode, message)) {
      classScope.info.methodHints.add(Attribute(methodNode.argument, maEmbeddableNew), message);

      classScope.save();
   }
}

////void Compiler :: compileForward(SNode ns, NamespaceScope& scope)
////{
////   ident_t shortcut = ns.findChild(lxNameAttr).firstChild(lxTerminalMask).identifier();
////   ident_t reference = ns.findChild(lxForward).firstChild(lxTerminalMask).identifier();
////
////   if (!scope.defineForward(shortcut, reference))
////      scope.raiseError(errDuplicatedDefinition, ns);
////}
////
//////////bool Compiler :: validate(_ProjectManager& project, _Module* module, int reference)
////////{
////////   int   mask = reference & mskAnyRef;
////////   ref_t extReference = 0;
////////   ident_t refName = module->resolveReference(reference & ~mskAnyRef);
////////   _Module* extModule = project.resolveModule(refName, extReference, true);
////////
////////   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
////////}
////
//////void Compiler :: validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project)
//////{
//////   //for (List<Unresolved>::Iterator it = unresolveds.start() ; !it.Eof() ; it++) {
//////   //   if (!validate(project, (*it).module, (*it).reference)) {
//////   //      ident_t refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);
//////
//////   //      project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
//////   //   }
//////   //}
//////}

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
   SyntaxTree expressionTree; // expression tree is reused

   // second pass - implementation
   while (current != lxNone) {
      switch (current) {
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

            compileClassImplementation(expressionTree, current, classScope);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference && classScope.info.header.classRef != 0) {
               ClassScope classClassScope(&scope, classScope.info.header.classRef, classScope.visibility);
               scope.moduleScope->loadClassInfo(classClassScope.info, scope.module->resolveReference(classClassScope.reference), false);
               classClassScope.classClassMode = true;

#ifdef FULL_OUTOUT_INFO
               // info
               ident_t classClassName = scope.module->resolveReference(current.argument);
               scope.moduleScope->printInfo("class %s#class", classClassName);
#endif // FULL_OUTOUT_INFO

               compileClassClassImplementation(expressionTree, current, classClassScope, classScope);
            }
            break;
         }
         case lxSymbol:
         {
            SymbolScope symbolScope(&scope, current.argument, scope.defaultVisibility);
            declareSymbolAttributes(current, symbolScope, false, false);

            compileSymbolImplementation(expressionTree, current, symbolScope);
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

   if (scope.declaredExtensions.Count() > 0) {
      compileModuleExtensionDispatcher(scope);

      scope.declaredExtensions.clear();
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

         ident_t name = current.identifier();
         if (emptystr(name))
            name = current.firstChild(lxTerminalMask).identifier();

         if (scope.moduleScope->includeNamespace(scope.importedNs, name, duplicateInclusion)) {
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

void Compiler :: initializeScope(ident_t name, _ModuleScope& scope, bool withDebugInfo, int stackAlignment)
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

   // compiler options
   if (stackAlignment)
      scope.stackAlignment = stackAlignment;
}

//void Compiler :: injectVirtualField(SNode classNode, LexicalType sourceType, ref_t sourceArg, int postfixIndex)
//{
//   // declare field
//   IdentifierString fieldName(VIRTUAL_FIELD);
//   fieldName.appendInt(postfixIndex);
//
//   SNode fieldNode = classNode.appendNode(lxClassField, INVALID_REF);
//   fieldNode.appendNode(lxNameAttr).appendNode(lxIdentifier, fieldName.c_str());
//
//   // assing field
//   SNode assignNode = classNode.appendNode(lxFieldInit, INVALID_REF); // INVALID_REF indicates the virtual code
//   assignNode.appendNode(lxIdentifier, fieldName.c_str());
//   assignNode.appendNode(lxAssign);
//   assignNode.appendNode(sourceType, sourceArg);
//}

void Compiler :: generateOverloadListMember(_ModuleScope& scope, ref_t listRef, mssg_t messageRef)
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

void Compiler :: generateClosedOverloadListMember(_ModuleScope& scope, ref_t listRef, mssg_t messageRef, ref_t classRef)
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

void Compiler :: generateSealedOverloadListMember(_ModuleScope& scope, ref_t listRef, mssg_t messageRef, ref_t classRef)
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
//
//void Compiler :: injectConverting(SNode& node, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg,
//   ref_t targetClassRef, int, bool embeddableAttr)
//{
//   SNode op = node.appendNode(convertOp, convertArg);
//
//   op.appendNode(lxCallTarget, targetClassRef);
//   op.appendNode(targetOp, targetArg);
//}

bool Compiler :: injectEmbeddableOp(_ModuleScope& scope, SNode assignNode, SNode callNode, SNode copyNode, ref_t subject, int paramCount)
{
   SNode tempLocal = assignNode.firstChild(lxObjectMask);
   if (tempLocal != lxTempLocal)
      return false;

   SNode copyTarget = copyNode.findSubNode(lxLocalAddress);
   if (copyTarget != lxLocalAddress)
      return false;

   SNode copySrc = copyTarget.nextNode(lxObjectMask);
   if (!copySrc.compare(lxLocal, lxTempLocal) || copySrc.argument != tempLocal.argument)
      return false;

   if (paramCount == -1) {
      //      // if it is an embeddable constructor call
      //      SNode sourceNode = assignNode.findSubNodeMask(lxObjectMask);

      SNode callTargetNode = callNode.firstChild(lxObjectMask);
      callTargetNode.set(copyTarget.type, copyTarget.argument);

      callNode.setArgument(subject);

      // HOTFIX : class class reference should be turned into class one
      SNode callTarget = callNode.findChild(lxCallTarget);

      IdentifierString className(scope.module->resolveReference(callTarget.argument));
      className.cut(getlength(className) - getlength(CLASSCLASS_POSTFIX), getlength(CLASSCLASS_POSTFIX));

      callTarget.setArgument(scope.mapFullReference(className));

      assignNode = lxIdle;
      copyNode = lxIdle;

      // check if inline initializer is declared
      ClassInfo targetInfo;
      scope.loadClassInfo(targetInfo, callTarget.argument);
      if (targetInfo.methods.exist(scope.init_message)) {
         // inject inline initializer call
         callTargetNode.injectAndReplaceNode(lxDirectCalling, scope.init_message);
         callTargetNode.appendNode(lxCallTarget, callTarget.argument);

         callTargetNode = callTargetNode.firstChild(lxObjectMask);
      }

      return true;
   }
   //   //else {
   //   //   // removing assinging operation
   //   //   assignNode = lxExpression;
   //
   //   //   // move assigning target into the call node
   //
   //   //   if (assignTarget != lxNone) {
   //   //      callNode.appendNode(assignTarget.type, assignTarget.argument);
   //   //      assignTarget = lxIdle;
   //   //      callNode.setArgument(encodeMessage(subject, paramCount));
   //   //   }
   //   //}
   else return false;
}

//SNode Compiler :: injectTempLocal(SNode node, int size, bool boxingMode)
//{
//   SNode tempLocalNode;
//
//   //HOTFIX : using size variable copy to prevent aligning
//   int dummy = size;
//   int offset = allocateStructure(node, dummy);
//
//   if (boxingMode) {
//      // allocate place for the local copy
//      node.injectNode(node.type, node.argument);
//
//      node.set(lxCopying, size);
//
//      //node.insertNode(lxTempAttr, 0); // NOTE _ should be the last child
//
//      tempLocalNode = node.insertNode(lxLocalAddress, offset);
//   }
//   else {
//      tempLocalNode = node.appendNode(lxLocalAddress, offset);
//   }
//
//   return tempLocalNode;
//}

void Compiler :: injectEmbeddableConstructor(SNode classNode, mssg_t message, mssg_t embeddedMessageRef)
{
   SNode methNode = classNode.appendNode(lxConstructor, message);
   methNode.appendNode(lxEmbeddableMssg, embeddedMessageRef);
   methNode.appendNode(lxAttribute, tpEmbeddable);
   methNode.appendNode(lxAttribute, tpConstructor);

   SNode codeNode = methNode.appendNode(lxDispatchCode);
   codeNode.appendNode(lxRedirect, embeddedMessageRef);
}

inline SNode newVirtualMultimethod(SNode classNode, mssg_t message, LexicalType methodType, bool privateOne)
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

   return methNode;
}

void Compiler :: injectVirtualMultimethod(_ModuleScope&, SNode classNode, mssg_t message, LexicalType methodType,
   mssg_t resendMessage, bool privateOne, ref_t callTargetRef)
{
   SNode methNode = newVirtualMultimethod(classNode, message, methodType, privateOne);

   SNode resendNode = methNode.appendNode(lxResendExpression, resendMessage);
   if (callTargetRef)
      resendNode.appendNode(lxCallTarget, callTargetRef);
}

bool Compiler :: injectVirtualStrongTypedMultimethod(_ModuleScope& moduleScope, SNode classNode, mssg_t message,
   LexicalType methodType, mssg_t resendMessage, bool privateOne)
{
   ref_t actionRef = getAction(resendMessage);
   ref_t signRef = 0;
   ident_t actionName = moduleScope.module->resolveAction(actionRef, signRef);

   ref_t signatures[ARG_COUNT];
   size_t len = moduleScope.module->resolveSignature(signRef, signatures);
   // HOTFIX : make sure it has at least one strong-typed argument
   bool strongOne = false;
   for (size_t i = 0; i < len; i++) {
      if (signatures[i] != moduleScope.superReference) {
         strongOne = true;
         break;
      }
   }

   // HOTFIX : the weak argument list should not be type-casted
   // to avoid dispatching to the same method
   if (!strongOne)
      return false;

   SNode methNode = newVirtualMultimethod(classNode, message, methodType, privateOne);

   SNode resendNode = methNode.appendNode(lxResendExpression);

   SNode resendExpr = resendNode.appendNode(lxMessageExpression);
   SNode messageNode = resendExpr.appendNode(lxMessage);
   if (!test(resendMessage, FUNCTION_MESSAGE)) {
      // NOTE : for a function message - ignore the message name
      messageNode.appendNode(lxIdentifier, actionName);
   }
   messageNode.appendNode(lxTemplate, actionName);

   //if ((resendMessage & PREFIX_MESSAGE_MASK) == PROPERTY_MESSAGE)
   //   resendExpr.appendNode(lxPropertyParam);

   IdentifierString arg;
   for (size_t i = 0; i < len; i++) {
      arg.copy("$");
      arg.appendInt(i);

      SNode paramNode = methNode.appendNode(lxMethodParameter);
      paramNode.appendNode(lxNameAttr).appendNode(lxIdentifier, arg.c_str());

      SNode exprNode = resendExpr.appendNode(lxExpression).appendNode(lxMessageExpression);
      if (signatures[i] != moduleScope.superReference) {
         // HOTFIX : typecasting to super object should be omitted (it is useless)
         exprNode.appendNode(lxAttribute, V_CONVERSION);
         exprNode.appendNode(lxType, signatures[i]);
         exprNode.appendNode(lxMessage);
         exprNode.appendNode(lxExpression).appendNode(lxIdentifier, arg.c_str());
      }
      else exprNode.appendNode(lxExpression).appendNode(lxIdentifier, arg.c_str());
   }

   // HOTFIX : to ignore typecasting warnings
   resendNode.appendNode(lxAutogenerated);

   return true;
}

inline bool isSingleDispatch(ClassInfo& info, mssg_t message, mssg_t& targetMessage)
{
   mssg_t foundMessage = 0;

   for (auto it = info.methodHints.start(); !it.Eof(); it++) {
      if (it.key().value2 == maMultimethod && *it == message) {
         if (foundMessage) {
            return false;
         }
         else foundMessage = it.key().value1;
      }
   }

   if (foundMessage) {
      targetMessage = foundMessage;

      return true;
   }
   else return false;
}

void Compiler :: injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, mssg_t message,
   LexicalType methodType, ClassInfo& info)
{
   bool privateOne = false;
   mssg_t resendMessage = message;
   ref_t callTargetRef = 0;

   ref_t actionRef, flags;
   pos_t argCount;
   decodeMessage(message, actionRef, argCount, flags);
   if (test(flags, STATIC_MESSAGE)) {
      privateOne = true;
   }

   info.methodHints.exclude(Attribute(message & ~STATIC_MESSAGE, maSingleMultiDisp));

   // try to resolve an argument list in run-time if it is only a single dispatch and argument list is not weak
   // !! temporally do not support variadic arguments
   if (isSingleDispatch(info, message, resendMessage) && ((message & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE) &&
      injectVirtualStrongTypedMultimethod(scope, classNode, message, methodType, resendMessage, privateOne))
   {
      // mark the message as a signle multi-method dispatcher if the class is sealed / closed
      // and default multi-method was not explicitly declared
      if (test(info.header.flags, elClosed) && !info.methods.exist(message, true))
         info.methodHints.add(Attribute(message & ~STATIC_MESSAGE, maSingleMultiDisp), resendMessage);
   }
   else {
      if (info.methods.exist(message, false)) {
         // if virtual multi-method handler is overridden
         // redirect to the parent one
         callTargetRef = info.header.parentRef;
      }
      else {
         ref_t dummy = 0;
         ident_t actionName = scope.module->resolveAction(actionRef, dummy);

         ref_t signatureLen = 0;
         ref_t signatures[ARG_COUNT];

         int firstArg = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
         if ((message & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
         }
         else {
            for (size_t i = firstArg; i < argCount; i++) {
               signatures[signatureLen++] = scope.superReference;
            }
         }
         ref_t signRef = scope.module->mapAction(actionName, scope.module->mapSignature(signatures, signatureLen, false), false);

         resendMessage = encodeMessage(signRef, argCount, flags);
      }

      injectVirtualMultimethod(scope, classNode, message, methodType, resendMessage, privateOne, callTargetRef);
   }
}

void Compiler :: injectVirtualDispatchMethod(_ModuleScope& scope, SNode classNode, mssg_t message, LexicalType type, ident_t argument)
{
   SyntaxTree subTree;
   SyntaxWriter subWriter(subTree);
   subWriter.newNode(lxRoot);
   subWriter.newNode(lxClassMethod, message);
   subWriter.appendNode(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   subWriter.appendNode(lxAttribute, V_EMBEDDABLE);
   
   // inject a method parameters
   ref_t actionRef = getAction(message);
   ref_t signRef = 0;
   ident_t actionName = scope.module->resolveAction(actionRef, signRef);

   if (signRef) {
      ref_t signatures[ARG_COUNT];
      size_t len = scope.module->resolveSignature(signRef, signatures);

      IdentifierString arg;
      for (size_t i = 0; i < len; i++) {
         arg.copy("$");
         arg.appendInt(i);

         subWriter.newNode(lxMethodParameter);
         subWriter.newNode(lxNameAttr);
         subWriter.appendNode(lxIdentifier, arg.c_str());
         subWriter.closeNode();
         subWriter.appendNode(lxType, signatures[i]);
         subWriter.closeNode();
      }
   }
   else {
      pos_t len = getArgCount(message);
      IdentifierString arg;
      for (pos_t i = 1; i < len; i++) {
         arg.copy("$");
         arg.appendInt(i);

         subWriter.newNode(lxMethodParameter);
         subWriter.newNode(lxNameAttr);
         subWriter.appendNode(lxIdentifier, arg.c_str());
         subWriter.closeNode();
         subWriter.closeNode();
      }
   }

   subWriter.newNode(lxDispatchCode);
   subWriter.appendNode(type, argument);
   subWriter.closeNode();
   subWriter.closeNode();
   subWriter.closeNode();

   SyntaxTree::copyNode(subTree.readRoot(), classNode);
}

void Compiler :: injectVirtualReturningMethod(_ModuleScope&, SNode classNode, mssg_t message, ident_t variable, ref_t outputRef)
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

void Compiler :: injectDefaultConstructor(_ModuleScope& scope, SNode classNode, ref_t, bool protectedOne)
{
   mssg_t message = protectedOne ? scope.protected_constructor_message : scope.constructor_message;
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
   writer.appendNode(lxAutogenerated);
   writer.newNode(lxExpression);
   writeTerminal(writer, ObjectInfo(okClass, scope.reference/*, scope.info.header.classRef*/), exprScope/*, HINT_NODEBUGINFO*/);
   writer.closeNode();
   writer.closeNode();
}

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
   size_t argCount = 1;
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

   mssg_t messageRef = encodeMessage(scope.module->mapAction(messageName.c_str(), 0, false), argCount, flags);

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

void Compiler :: injectExprOperation(SNode& node, int size, int tempLocal, LexicalType op, int opArg)
{
   if (isUnaryOperation(opArg)) {
      SNode loperand = node.firstChild(lxObjectMask);
      SNode roperand = loperand.nextNode(lxObjectMask);

      // left argument should be result variable
      // so we have to move our original larg to right
      roperand.set(loperand.type, loperand.argument);
      loperand.set(lxLocalAddress, tempLocal);

      node.set(op, opArg);
   }
   else {
      node.set(lxSeqExpression, 0);

      SNode loperand = node.firstChild(lxObjectMask);
      loperand.injectAndReplaceNode(lxCopying, size);
      loperand.insertNode(lxLocalAddress, tempLocal);

      SNode roperand = loperand.nextNode(lxObjectMask);
      roperand.injectAndReplaceNode(op, opArg);
      roperand.insertNode(lxLocalAddress, tempLocal);
   }
}

void Compiler :: generateResendToEmbeddable(SyntaxTree& tree, MethodScope& scope, ref_t resendMessage, LexicalType methodType)
{
   SyntaxWriter writer(tree);

   writer.newNode(methodType, scope.message);
   writer.newNode(lxCode);

   writer.newNode(lxExpression);

   writer.newNode(lxMessageExpression);
   
   writer.newNode(lxExpression);
   writer.appendNode(lxAttribute, V_MEMBER);
   writer.appendNode(lxIdentifier, SELF_VAR);
   writer.closeNode();

   writer.appendNode(lxMessage, resendMessage);

   for (auto it = scope.parameters.start(); !it.Eof(); it++) {
      writer.newNode(lxExpression);
      writer.appendNode(lxIdentifier, it.key());
      writer.closeNode();
   }

   writer.newNode(lxExpression);
   writer.appendNode(lxAttribute, V_WRAPPER);
   writer.appendNode(lxAttribute, V_VARIABLE);
   writer.appendNode(lxType, scope.outputRef);
   writer.appendNode(lxIdentifier, RETVAL_VAR);
   writer.closeNode();

   writer.closeNode();
   writer.closeNode();

   writer.newNode(lxReturning);
   writer.appendNode(lxIdentifier, RETVAL_VAR);
   writer.closeNode();

   writer.closeNode();
   writer.closeNode();
}
