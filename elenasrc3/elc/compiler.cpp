//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "compiler.h"
#include "langcommon.h"
#include <errno.h>

#include "bytecode.h"

using namespace elena_lang;

typedef ExpressionAttribute   EAttr;
typedef ExpressionAttributes  EAttrs;

// --- helper routines ---

EAttr operator | (const EAttr& l, const EAttr& r)
{
   return (EAttr)((pos64_t)l | (pos64_t)r);
}

EAttr operator & (const EAttr& l, const EAttr& r)
{
   return (EAttr)((pos64_t)l & (pos64_t)r);
}

MethodHint operator & (const ref_t& l, const MethodHint& r)
{
   return (MethodHint)(l & (unsigned int)r);
}

//inline void testNodes(SyntaxNode node)
//{
//   SyntaxNode current = node.firstChild();
//   while (current != SyntaxKey::None) {
//      testNodes(current);
//
//      current = current.nextNode();
//   }
//}

// --- Interpreter ---

Interpreter :: Interpreter(ModuleScopeBase* scope, CompilerLogic* logic)
{
   _scope = scope;
   _logic = logic;
}

ObjectInfo Interpreter :: mapStringConstant(ustr_t s)
{
   return ObjectInfo(ObjectKind::StringLiteral, V_STRING, _scope->module->mapConstant(s));
}

void Interpreter :: addArrayItem(ref_t dictionaryRef, ref_t symbolRef)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskMetaArrayRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeArrayEntry(dictionary, symbolRef | mskSymbolRef);
}

void Interpreter :: setAttrDictionaryValue(ref_t dictionaryRef, ustr_t key, ref_t reference)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskMetaAttributesRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeAttrDictionaryEntry(dictionary, key, reference);
}

void Interpreter :: setDeclDictionaryValue(ref_t dictionaryRef, ustr_t key, ref_t reference)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskDeclAttributesRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeDeclDictionaryEntry(dictionary, key, reference);
}

void Interpreter :: setDictionaryValue(ref_t dictionaryRef, ustr_t key, int value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskMetaDictionaryRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeDictionaryEntry(dictionary, key, value);
}

bool Interpreter :: evalAttrDictionaryOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];

   if (args.count() == 3 && loperand.kind == ObjectKind::MetaDictionary
      && (roperand.kind == ObjectKind::Class || roperand.kind == ObjectKind::Symbol)
      && args[2].kind == ObjectKind::StringLiteral)
   {
      ObjectInfo ioperand = args[2];

      ustr_t key = _scope->module->resolveConstant(ioperand.reference);
      ref_t reference = roperand.reference;

      if (operator_id == SET_INDEXER_OPERATOR_ID) {
         setAttrDictionaryValue(loperand.reference, key, reference);

         return true;
      }
   }

   return false;
}

bool Interpreter :: evalDeclDictionaryOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];

   if (args.count() == 3 && loperand.kind == ObjectKind::MetaDictionary
      && (roperand.kind == ObjectKind::Template))
   {
      ObjectInfo ioperand = args[2];

      ustr_t key = _scope->module->resolveConstant(ioperand.reference);
      ref_t reference = roperand.reference;

      if (operator_id == SET_INDEXER_OPERATOR_ID) {
         setDeclDictionaryValue(loperand.reference, key, reference);

         return true;
      }
   }

   return false;
}

bool Interpreter :: evalStrDictionaryOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];

   if (args.count() == 3 && loperand.kind == ObjectKind::MetaDictionary && roperand.kind == ObjectKind::IntLiteral
      && args[2].kind == ObjectKind::StringLiteral)
   {
      ObjectInfo ioperand = args[2];

      ustr_t key = _scope->module->resolveConstant(ioperand.reference);
      int value = roperand.extra;

      if(operator_id == SET_INDEXER_OPERATOR_ID) {
         setDictionaryValue(loperand.reference, key, value);

         return true;
      }
   }

   return false;
}

bool Interpreter :: evalObjArrayOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];
   if (loperand.kind == ObjectKind::MetaArray && roperand.kind == ObjectKind::Symbol) {
      if (operator_id == ADD_ASSIGN_OPERATOR_ID) {
         addArrayItem(loperand.reference, roperand.reference | mskSymbolRef);

         return true;
      }
   }

   return false;
}

bool Interpreter :: evalDeclOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal)
{
   ObjectInfo loperand = args[0];
   if (operator_id == NAME_OPERATOR_ID && loperand.kind == ObjectKind::Template) {
      ReferenceProperName name(_scope->resolveFullName(loperand.reference));

      retVal = mapStringConstant(*name);

      return true;
   }
   else if (operator_id == NAME_OPERATOR_ID && loperand.kind == ObjectKind::Class) {
      retVal = { ObjectKind::SelfName };

      return true;
   }

   return false;
}

bool Interpreter :: eval(BuildKey key, ref_t operator_id, ArgumentsInfo& arguments, ObjectInfo& retVal)
{
   switch (key) {
      case BuildKey::StrDictionaryOp:
         return evalStrDictionaryOp(operator_id, arguments);
      case BuildKey::ObjArrayOp:
         return evalObjArrayOp(operator_id, arguments);
      case BuildKey::AttrDictionaryOp:
         return evalAttrDictionaryOp(operator_id, arguments);
      case BuildKey::DeclDictionaryOp:
         return evalDeclDictionaryOp(operator_id, arguments);
      case BuildKey::DeclOp:
         return evalDeclOp(operator_id, arguments, retVal);
      default:
         return false;
   }
}

// --- Compiler::NamespaceScope ---

Compiler::NamespaceScope :: NamespaceScope(NamespaceScope* parent) :
   Scope(parent),
   forwards(0),
   importedNs(nullptr),
   extensions({}),
   extensionTargets(INVALID_REF),
   extensionDispatchers(INVALID_REF)
   //declaredExtensions({})
{
   nsName.copy(*parent->nsName);
   sourcePath.copy(*parent->sourcePath);
   defaultVisibility = parent->defaultVisibility;
   errorProcessor = parent->errorProcessor;
}

void Compiler::NamespaceScope :: addExtension(mssg_t message, ref_t extRef, mssg_t strongMessage)
{
   //ns->declaredExtensions.add(message, { extRef, strongMessage });

   extensions.add(message, { extRef, strongMessage });
}

ref_t Compiler::NamespaceScope :: resolveExtensionTarget(ref_t reference)
{
   ref_t resolved = extensionTargets.get(reference);
   if (resolved == INVALID_REF) {
      ClassInfo classInfo;
      moduleScope->loadClassInfo(classInfo, reference);

      resolved = classInfo.attributes.get({ 0, ClassAttribute::ExtensionRef });
      extensionTargets.add(reference, resolved);
   }

   return resolved;
}

void Compiler::NamespaceScope :: raiseError(int message, SyntaxNode terminal)
{
   errorProcessor->raiseTerminalError(message, *sourcePath, terminal);
}

void Compiler::NamespaceScope::raiseWarning(int level, int message, SyntaxNode terminal)
{
   errorProcessor->raiseTerminalWarning(level, message, *sourcePath, terminal);
}

ObjectInfo Compiler::NamespaceScope :: defineObjectInfo(ref_t reference, ExpressionAttribute mode, bool checkMode)
{
   ObjectInfo info;

   bool metaOne = ExpressionAttributes::test(mode, ExpressionAttribute::Meta);
   bool weakOne = ExpressionAttributes::test(mode, ExpressionAttribute::Weak);
   bool internOne = ExpressionAttributes::test(mode, ExpressionAttribute::Intern);

   if (reference) {
      if (metaOne) {
         // check if it is a meta symbol
         if (module->mapSection(reference | mskMetaDictionaryRef, true)) {
            info.kind = ObjectKind::MetaDictionary;
            info.type = V_DICTIONARY;
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskMetaArrayRef, true)) {
            info.kind = ObjectKind::MetaArray;
            info.type = V_OBJARRAY;
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskMetaAttributesRef, true)) {
            info.kind = ObjectKind::MetaDictionary;
            info.type = V_OBJATTRIBUTES;
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskDeclAttributesRef, true)) {
            info.kind = ObjectKind::MetaDictionary;
            info.type = V_DECLATTRIBUTES;
            info.reference = reference;

            return info;
         }
      }
      if (internOne) {
         // check if it is an internal procedure
         info.kind = ObjectKind::InternalProcedure;
         info.reference = reference;
      }
      else {
         if (checkMode) {
            ClassInfo classInfo;
            if (moduleScope->loadClassInfo(classInfo, reference, true) != 0) {
               // if it is a stateless symbol
               if (test(classInfo.header.flags, elStateless)) {
                  return { ObjectKind::Singleton, reference, reference };
               }
               // if it is a normal class
               // then the symbol is reference to the class class
               else if (test(classInfo.header.flags, elStandartVMT) && classInfo.header.classRef != 0) {
                  return {ObjectKind::Class, classInfo.header.classRef, reference };
               }
            }
            else {
               SymbolInfo symbolInfo;
               if (moduleScope->loadSymbolInfo(symbolInfo, reference)) {
                  switch (symbolInfo.symbolType) {
                     case SymbolType::Singleton:
                        return defineObjectInfo(symbolInfo.valueRef, mode, true);
                     default:
                        break;
                  }
               }
            }
         }
         // otherwise it is a normal one
         info.kind = ObjectKind::Symbol;
         info.reference = reference;
      }
   }

   return info;
}

ObjectInfo Compiler::NamespaceScope :: definePredefined(ref_t reference, ExpressionAttribute mode)
{
   switch (reference) {
      case V_NIL:
         return { ObjectKind::Nil, reference, 0 };
      default:
         return {};
   }
}

ref_t Compiler::NamespaceScope :: resolveImplicitIdentifier(ustr_t identifier, bool referenceOne, bool innnerMost)
{
   ref_t reference = forwards.get(identifier);
   if (reference)
      return reference;

   reference = moduleScope->resolveImplicitIdentifier(*nsName, identifier, Visibility::Public);

   // check if it is an internal one
   if (!reference)
      reference = moduleScope->resolveImplicitIdentifier(*nsName, identifier, Visibility::Internal);

   // check if it is a private one for the inner most
   if (!reference && innnerMost)
      reference = moduleScope->resolveImplicitIdentifier(*nsName, identifier, Visibility::Private);

   if (!reference && !referenceOne)
      reference = moduleScope->resolveImportedIdentifier(identifier, &importedNs);

   if (reference)
      forwards.add(identifier, reference);

   return reference;
}

ref_t Compiler::NamespaceScope :: mapNewIdentifier(ustr_t name, Visibility visibility)
{
   return moduleScope->mapNewIdentifier(*nsName, name, visibility);
}

ObjectInfo Compiler::NamespaceScope :: mapIdentifier(ustr_t identifier, bool referenceOne, EAttr mode)
{
   ref_t reference = 0;
   if (!referenceOne) {
      // try resolve as type-alias
      reference = moduleScope->aliases.get(identifier);
      if (isPrimitiveRef(reference))
         reference = 0;
   }

   if (!reference)
      reference = resolveImplicitIdentifier(identifier, referenceOne, !EAttrs::test(mode, EAttr::NestedNs));

   if (reference)
      return defineObjectInfo(reference, mode, true);

   if (parent == nullptr) {
      // outer most ns
      if (referenceOne) {
         if (isWeakReference(identifier)) {
            return mapWeakReference(identifier, false);
         }
         else return mapGlobal(identifier, mode);
      }
      else {
         reference = moduleScope->predefined.get(identifier);
         if (reference)
            return definePredefined(reference, mode);
      }

   }

   return Scope::mapIdentifier(identifier, referenceOne, mode | EAttr::NestedNs);
}

ObjectInfo Compiler::NamespaceScope :: mapGlobal(ustr_t identifier, EAttr mode)
{
   if (isForwardReference(identifier)) {
      // if it is a forward reference
      return defineObjectInfo(moduleScope->mapFullReference(identifier, false), EAttr::None, false);
   }

   // if it is an existing full reference
   ref_t reference = moduleScope->mapFullReference(identifier, true);
   if (reference) {
      return defineObjectInfo(reference, mode, true);
   }
   // if it is a forward reference
   else return defineObjectInfo(moduleScope->mapFullReference(identifier, false), EAttr::Weak, false);
}

ObjectInfo Compiler::NamespaceScope :: mapWeakReference(ustr_t identifier, bool directResolved)
{
   ref_t reference = 0;
   if (directResolved) {
      reference = moduleScope->mapWeakReference(identifier);
   }
   else reference = moduleScope->mapFullReference(identifier);

   return defineObjectInfo(reference, EAttr::None, false);
}

// --- Compiler::MetaScope ---

Compiler::MetaScope :: MetaScope(Scope* parent)
   : Scope(parent)
{
   
}

ObjectInfo Compiler::MetaScope :: mapDecl()
{
   TemplateScope* tempScope = Scope::getScope<TemplateScope>(*this, ScopeLevel::Template);
   if (tempScope != nullptr) {
      return { ObjectKind::Template, V_DECLARATION, tempScope->reference };
   }
   ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);
   if (classScope != nullptr) {
      return { ObjectKind::Class, V_DECLARATION, classScope->reference };
   }

   return {};
}

ObjectInfo Compiler::MetaScope :: mapIdentifier(ustr_t identifier, bool referenceOne, EAttr attr)
{
   if (!referenceOne) {
      if (moduleScope->declVar.compare(identifier)) {
         return mapDecl();
      }
      else {
         IdentifierString metaIdentifier(META_PREFIX, identifier);

         NamespaceScope* ns = Scope::getScope<NamespaceScope>(*this, ScopeLevel::Namespace);

         // check if it is a meta dictionary
         ObjectInfo retVal = parent->mapIdentifier(*metaIdentifier, referenceOne, attr | EAttr::Meta);
         if (retVal.kind == ObjectKind::Unknown) {
            return Scope::mapIdentifier(identifier, referenceOne, attr);
         }
         else return retVal;
      }
   }
   else return Scope::mapIdentifier(identifier, referenceOne, attr);
}

// --- Compiler::SourceScope ---

Compiler::SourceScope :: SourceScope(Scope* parent, ref_t reference, Visibility visibility)
   : Scope(parent)
{
   this->reference = reference;
   this->visibility = visibility;
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(NamespaceScope* ns, ref_t reference, Visibility visibility)
   : SourceScope(ns, reference, visibility), info({})
{
   isStatic = false;
   reserved1 = reserved2 = 0;
   reservedArgs = ns->moduleScope->minimalArgList;
}

void Compiler::SymbolScope :: load()
{
   // save class meta data
   MemoryReader metaReader(moduleScope->module->mapSection(reference | mskMetaSymbolInfoRef, true), 0);
   info.load(&metaReader);
}

void Compiler::SymbolScope :: save()
{
   // save class meta data
   MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaSymbolInfoRef, false), 0);
   info.save(&metaWriter);
}

// --- Compiler::TemplateScope ---

Compiler::TemplateScope :: TemplateScope(Scope* parent, ref_t reference, Visibility visibility)
   : SourceScope(parent, reference, visibility)
{
   type = TemplateType::None;
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(Scope* ns, ref_t reference, Visibility visibility)
   : SourceScope(ns, reference, visibility)
{
   info.header.flags = elStandartVMT;
   info.header.parentRef = moduleScope->buildins.superReference;
   extensionClassRef = 0;
   abstractMode = abstractBasedMode = false;
   extensionDispatcher = false;
}

ObjectInfo Compiler::ClassScope :: mapField(ustr_t identifier, ExpressionAttribute attr)
{
   bool readOnly = test(info.header.flags, elReadOnlyRole);

   auto fieldInfo = info.fields.get(identifier);
   if (fieldInfo.offset >= 0) {
      if (test(info.header.flags, elStructureRole)) {
         return { readOnly ? ObjectKind::ReadOnlyFieldAddress : ObjectKind::FieldAddress, fieldInfo.typeRef, fieldInfo.offset };
      }
      else return { readOnly ? ObjectKind::ReadOnlyField : ObjectKind::Field, fieldInfo.typeRef, fieldInfo.offset };
   }
   else if (fieldInfo.offset == -2) {
      return { readOnly ? ObjectKind::ReadOnlySelfLocal : ObjectKind::SelfLocal, fieldInfo.typeRef, 1};
   }
   else {
      auto staticFieldInfo = info.statics.get(identifier);
      if (staticFieldInfo.offset != 0 && staticFieldInfo.valueRef != 0) {
         return { ObjectKind::StaticConstField, staticFieldInfo.typeRef, staticFieldInfo.offset };
      }
      else if (staticFieldInfo.valueRef) {
         return { ObjectKind::ClassConstant, staticFieldInfo.typeRef, staticFieldInfo.valueRef };
      }

      return {};
   }
}

ObjectInfo Compiler::ClassScope :: mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
{
   if (!referenceOne) {
      ObjectInfo fieldInfo = mapField(identifier, attr);
      if (fieldInfo.kind != ObjectKind::Unknown)
         return fieldInfo;
   }
   return Scope::mapIdentifier(identifier, referenceOne, attr);
}

void Compiler::ClassScope :: save()
{
   MemoryBase* section = moduleScope->mapSection(reference | mskMetaClassInfoRef, false);
   section->trim(0);

   // save class meta data
   MemoryWriter metaWriter(section);
   info.save(&metaWriter);
}

// --- Compiler::MethodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent) :
   Scope(parent),
   message(0),
   parameters({}),
   selfLocal(0),
   reserved1(0),
   reserved2(0),
   reservedArgs(parent->moduleScope->minimalArgList),
   functionMode(false),
   closureMode(false)
{
}

bool Compiler::MethodScope :: checkType(MethodHint type)
{
   return (info.hints & MethodHint::Mask) == type;
}

bool Compiler::MethodScope :: checkType(MethodInfo& methodInfo, MethodHint type)
{
   return (methodInfo.hints & MethodHint::Mask) == type;
}

ObjectInfo Compiler::MethodScope :: mapSelf()
{
   //if (checkHint(MethodHint::Extension)) {
   //   //COMPILER MAGIC : if it is an extension ; replace self with this self

   //}
   /*else */if (selfLocal != 0) {
      return { ObjectKind::SelfLocal, getClassRef(false), (ref_t)selfLocal };
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapParameter(ustr_t identifier)
{
   int prefix = functionMode ? 0 : -1;

   Parameter local = parameters.get(identifier);
   if (local.offset != -1) {
      return { ObjectKind::Param, local.class_ref, prefix - local.offset };
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
{
   auto paramInfo = mapParameter(identifier);
   if (paramInfo.kind != ObjectKind::Unknown) {
      return paramInfo;
   }
   else if (moduleScope->selfVar.compare(identifier)) {
      return mapSelf();
   }

   return Scope::mapIdentifier(identifier, referenceOne, attr);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals({})
{
   allocated1 = reserved1 = 0;
   allocated2 = reserved2 = 0;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals({})
{
   reserved1 = allocated1 = parent->allocated1;
   reserved2 = allocated2 = parent->allocated2;
}

ObjectInfo Compiler::CodeScope :: mapLocal(ustr_t identifier)
{
   Parameter local = locals.get(identifier);
   if (local.offset != -1) {
      if (local.size > 0) {
         return { ObjectKind::LocalAddress, local.class_ref, local.offset };
      }
      else return { ObjectKind::Local, local.class_ref, local.offset };
   }
   else return {};
}

ObjectInfo Compiler::CodeScope :: mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
{
   ObjectInfo info = mapLocal(identifier);
   if (info.kind != ObjectKind::Unknown || EAttrs::test(attr, ExpressionAttribute::Local)) {
      return info;
   }

   return Scope::mapIdentifier(identifier, referenceOne, attr);
}

void Compiler::CodeScope :: syncStack(MethodScope* methodScope)
{
   if (methodScope->reserved1 < reserved1)
      methodScope->reserved1 = reserved1;

   if (methodScope->reserved2 < reserved2)
      methodScope->reserved2 = reserved2;
}

void Compiler::CodeScope :: syncStack(CodeScope* parentScope)
{
   if (allocated1 > reserved1)
      reserved1 = allocated1;

   if (allocated2 > reserved2)
      reserved2 = allocated2;

   if (parentScope->reserved1 < reserved1)
      parentScope->reserved1 = reserved1;

   if (parentScope->reserved2 < reserved2)
      parentScope->reserved2 = reserved2;
}

void Compiler::CodeScope :: markAsAssigned(ObjectInfo object)
{
   if (object.kind == ObjectKind::Local/* || object.kind == okLocalAddress*/) {
      for (auto it = locals.start(); !it.eof(); ++it) {
         if ((*it).offset == (int)object.reference) {
            (*it).unassigned = false;
            return;
         }
      }
   }

   parent->markAsAssigned(object);
}

// --- Compiler::ExprScope ---

Compiler::ExprScope :: ExprScope(SourceScope* parent)
   : Scope(parent), tempLocals({})
{
   allocatedArgs = 0;
   tempAllocated2 = tempAllocated1 = 0;
}

Compiler::ExprScope :: ExprScope(CodeScope* parent)
   : Scope(parent), tempLocals({})
{
   allocatedArgs = 0;
   tempAllocated1 = parent->allocated1;
   tempAllocated2 = parent->allocated2;
}

int Compiler::ExprScope :: newTempLocal()
{
   tempAllocated1++;

   return tempAllocated1;
}

void Compiler::ExprScope :: syncStack()
{
   CodeScope* codeScope = Scope::getScope<CodeScope>(*this, Scope::ScopeLevel::Code);
   if (codeScope != nullptr) {
      if (codeScope->reserved1 < tempAllocated1)
         codeScope->reserved1 = tempAllocated1;

      if (codeScope->reserved2 < tempAllocated2)
         codeScope->reserved2 = tempAllocated2;

      if (tempAllocated1 < codeScope->allocated1)
         tempAllocated1 = codeScope->allocated1;

      if (tempAllocated2 < codeScope->allocated2)
         tempAllocated2 = codeScope->allocated2;

      MethodScope* methodScope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
      if (methodScope != nullptr) {
         methodScope->reservedArgs = _max(methodScope->reservedArgs, allocatedArgs);
      }
   }
   else {
      SymbolScope* symbolScope = Scope::getScope<SymbolScope>(*this, ScopeLevel::Symbol);
      if (symbolScope != nullptr) {
         symbolScope->reservedArgs = _max(symbolScope->reservedArgs, allocatedArgs);
         if (symbolScope->reserved1 < tempAllocated1)
            symbolScope->reserved1 = tempAllocated1;
      }
   }
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(ExprScope* owner, ref_t reference)
   : ClassScope(owner, reference, Visibility::Internal)
{
   
}

// --- Compiler ---

Compiler :: Compiler(
   ErrorProcessor* errorProcessor,
   TemplateProssesorBase* templateProcessor,
   CompilerLogic* compilerLogic)
{
   _logic = compilerLogic;
   _errorProcessor = errorProcessor;
   _templateProcessor = templateProcessor;

   _optMode = true; // !! temporally - should be set if the optimization is enabled
}

inline ref_t resolveDictionaryMask(ref_t typeRef)
{
   switch (typeRef) {
      case V_STRINGOBJ:
         return mskMetaArrayRef;
      case V_DICTIONARY:
         return mskMetaDictionaryRef;
      case V_OBJATTRIBUTES:
         return mskMetaAttributesRef;
      case V_DECLATTRIBUTES:
         return mskDeclAttributesRef;
      default:
         return 0;
   }
}

ref_t Compiler :: mapNewTerminal(Scope& scope, ustr_t prefix, SyntaxNode nameNode, ustr_t postfix, Visibility visibility)
{
   if (nameNode == SyntaxKey::Name) {
      SyntaxNode terminal = nameNode.firstChild(SyntaxKey::TerminalMask);
      ustr_t name = terminal.identifier();

      ref_t reference = 0;
      if (!prefix.empty() || !postfix.empty()) {
         IdentifierString nameWithFixes(prefix, name, postfix);

         reference = scope.mapNewIdentifier(*nameWithFixes, visibility);
      }
      else reference = scope.mapNewIdentifier(name, visibility);

      nameNode.setArgumentReference(reference);

      NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

      // verify if the name is unique
      if (visibility == Visibility::Public) {
         ref_t dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Internal);
         if (!dup)
            dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Private);

         if (dup)
            reference = dup;
      }
      else if (visibility == Visibility::Internal) {
         ref_t dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Public);
         if (!dup)
            dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Private);

         if (dup)
            reference = dup;
      }
      else if (visibility == Visibility::Private) {
         ref_t dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Public);
         if (!dup)
            dup = scope.moduleScope->resolveImplicitIdentifier(*nsScope->nsName, name, Visibility::Internal);

         if (dup)
            reference = dup;
      }

      // check for duplicates
      if (scope.module->mapSection(reference | mskMetaArrayRef, true))
         scope.raiseError(errDuplicatedSymbol, nameNode.firstChild(SyntaxKey::TerminalMask));

      if (scope.module->mapSection(reference | mskMetaDictionaryRef, true))
         scope.raiseError(errDuplicatedSymbol, nameNode.firstChild(SyntaxKey::TerminalMask));

      if (scope.module->mapSection(reference | mskSymbolRef, true))
         scope.raiseError(errDuplicatedSymbol, nameNode.firstChild(SyntaxKey::TerminalMask));

      if (scope.module->mapSection(reference | mskSyntaxTreeRef, true))
         scope.raiseError(errDuplicatedSymbol, nameNode.firstChild(SyntaxKey::TerminalMask));

      return reference;
   }
   else throw InternalError(errFatalError);
}

mssg_t Compiler :: mapMethodName(Scope& scope, pos_t paramCount, ustr_t actionName, ref_t actionRef,
   ref_t flags, ref_t* signature, size_t signatureLen)
{
   if (actionRef != 0) {
      // HOTFIX : if the action was already resolved - do nothing
   }
   else if (actionName.length() > 0) {
      ref_t signatureRef = 0;
      if (signatureLen > 0)
         signatureRef = scope.moduleScope->module->mapSignature(signature, signatureLen, false);

      actionRef = scope.moduleScope->module->mapAction(actionName, signatureRef, false);
   }
   else return 0;

   // NOTE : a message target should be included as well for a normal message
   pos_t argCount = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
   argCount += paramCount;

   return encodeMessage(actionRef, argCount, flags);
}

ref_t Compiler :: retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters, ustr_t prefix)
{
   SyntaxNode identNode = node.firstChild(SyntaxKey::TerminalMask);

   IdentifierString templateName;

   SyntaxNode current = node.firstChild();
   while (current.key != SyntaxKey::None) {
      if (current.key == SyntaxKey::TemplateArg) {
         parameters.add(current);
      }

      current = current.nextNode();
   }

   templateName.append(prefix);
   templateName.append(identNode.identifier());
   templateName.append('#');
   templateName.appendInt(parameters.count());

   NamespaceScope* ns = &scope;
   ref_t reference = ns->resolveImplicitIdentifier(*templateName, false, true);
   while (!reference && ns->parent != nullptr) {
      ns = Scope::getScope<NamespaceScope>(*ns->parent, Scope::ScopeLevel::Namespace);
      if (ns) {
         reference = ns->resolveImplicitIdentifier(*templateName, false, false);
      }
      else break;
   }

   return reference;
}

void Compiler :: importTemplate(Scope& scope, SyntaxNode node, ustr_t prefix, SyntaxNode target)
{
   List<SyntaxNode> parameters({});

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, prefix);
   if (!templateRef)
      scope.raiseError(errUnknownTemplate, node);

   if(_templateProcessor->importInlineTemplate(*scope.moduleScope, templateRef, target, parameters)) {

   }
   else scope.raiseError(errInvalidSyntax, node);
}

void Compiler :: declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility)
{
   ref_t targetType = V_DICTIONARY;
   declareDictionaryAttributes(scope, node, targetType);

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   ref_t reference = mapNewTerminal(scope, META_PREFIX, name, nullptr, visibility);
   ref_t mask = resolveDictionaryMask(targetType);

   // create a meta section
   scope.moduleScope->module->mapSection(reference | mask, false);
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef)
{
   ref_t flagCopy = scope.info.header.flags;
   ref_t classClassCopy = scope.info.header.classRef;

   auto parentInfo = scope.moduleScope->getModule(scope.moduleScope->resolveFullName(parentRef), true);
   if (parentInfo.unassigned())
      return InheritResult::irUnsuccessfull;

   MemoryBase* metaData = parentInfo.module->mapSection(parentInfo.reference | mskMetaClassInfoRef, true);
   if (metaData == nullptr)
      return InheritResult::irUnsuccessfull;

   MemoryReader reader(metaData);
   if (scope.moduleScope->module != parentInfo.module) {
      ClassInfo copy;
      copy.load(&reader);

      scope.moduleScope->importClassInfo(copy, scope.info, parentInfo.module, false, true);
   }
   else {
      scope.info.load(&reader, false);

      // mark all methods as inherited
      auto it = scope.info.methods.start();
      while (!it.eof()) {
         mssg_t message = it.key();

         (*it).inherited = true;
         ++it;

         // private methods are not inherited
         if (test(message, STATIC_MESSAGE))
            scope.info.methods.exclude(message);
      }
   }

   if (test(scope.info.header.flags, elFinal)) {
      // COMPILER MAGIC : if it is a unsealed nested class inheriting its owner
      if (!test(scope.info.header.flags, elSealed) && test(flagCopy, elNestedClass)) {
         ClassScope* owner = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::OwnerClass);
         if (test(owner->info.header.flags, elClassClass) && scope.info.header.classRef == owner->reference) {
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

   scope.info.header.flags |= flagCopy;

   return InheritResult::irSuccessfull;
}

void Compiler :: checkMethodDuplicates(ClassScope& scope, SyntaxNode node, mssg_t message,
   mssg_t publicMessage, bool protectedOne, bool internalOne)
{
   if (!test(message, STATIC_MESSAGE) && scope.info.methods.exist(publicMessage | STATIC_MESSAGE)) {
      // there should be no public method with the same name
      scope.raiseError(errDupPrivateMethod, node);
   }
   if (publicMessage != message && scope.info.methods.exist(publicMessage)) {
      // there should be no public method with the same name
      scope.raiseError(errDupPublicMethod, node);
   }
   if (!protectedOne && scope.getMssgAttribute(publicMessage, ClassAttribute::ProtectedAlias)) {
      // there should be no protected method with the same name
      scope.raiseError(errDupProtectedMethod, node);
   }
   if (!internalOne && scope.getMssgAttribute(publicMessage, ClassAttribute::InternalAlias)) {
      // there should be no internal method with the same name
      scope.raiseError(errDupInternalMethod, node);
   }
}

ref_t Compiler :: generateConstant(Scope& scope, ObjectInfo& retVal, ref_t constRef)
{
   // check if the constant can be resolved immediately
   switch (retVal.kind) {
      case ObjectKind::Singleton:
         return retVal.reference;
      case ObjectKind::StringLiteral:
         break;
      default:
         return 0;
   }

   // otherwise we have to create the constant
   ModuleBase* module = scope.module;
   if (!constRef)
      constRef = scope.moduleScope->mapAnonymous("const");

   MemoryWriter dataWriter(module->mapSection(constRef | mskConstant, false));
   switch (retVal.kind) {
      case ObjectKind::StringLiteral:
      {
         ustr_t value = module->resolveConstant(retVal.reference);
         dataWriter.writeString(value, value.length() + 1);

         retVal.type = scope.moduleScope->buildins.literalReference;
         break;
      }
      default:
         break;
   }

   dataWriter.Memory()->addReference(retVal.type | mskVMTRef, (pos_t)-4);

   // save constant meta info
   SymbolInfo constantInfo = { SymbolType::Constant, constRef, retVal.type };
   MemoryWriter metaWriter(module->mapSection(constRef | mskMetaSymbolInfoRef, false));
   constantInfo.save(&metaWriter);

   return constRef;
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SyntaxNode node,
   MethodInfo& methodInfo, bool abstractBased)
{
   mssg_t message = node.arg.reference;

   if (abstractBased) {
      methodInfo.hints &= ~((ref_t)MethodHint::Abstract);
   }

   methodInfo.hints |= node.findChild(SyntaxKey::Hints).arg.reference;

   ref_t outputRef = node.findChild(SyntaxKey::OutputType).arg.reference;

   mssg_t multiMethod = node.findChild(SyntaxKey::Multimethod).arg.reference;
   if (multiMethod)
      methodInfo.multiMethod = multiMethod;

   // check duplicates with different visibility scope
   if (MethodScope::checkHint(methodInfo, MethodHint::Private)) {
      checkMethodDuplicates(scope, node, message, message & ~STATIC_MESSAGE, false, false);
   }
   else if (MethodScope::checkAnyHint(methodInfo, MethodHint::Protected, MethodHint::Internal)) {
      // if it is internal / protected message save the public alias
      ref_t signRef = 0;
      ustr_t name = scope.module->resolveAction(getAction(message), signRef);
      mssg_t publicMessage = 0;

      size_t index = name.findStr("$$");
      if (index == NOTFOUND_POS)
         scope.raiseError(errDupInternalMethod, node);

      publicMessage = overwriteAction(message, scope.module->mapAction(name + index + 2, 0, false));

      if (MethodScope::checkHint(methodInfo, MethodHint::Protected)) {
         checkMethodDuplicates(scope, node, message, publicMessage, true, false);

         scope.addMssgAttribute(publicMessage, ClassAttribute::ProtectedAlias, message);
      }
      else {
         checkMethodDuplicates(scope, node, message, publicMessage, false, true);

         scope.addMssgAttribute(publicMessage, ClassAttribute::InternalAlias, message);
      }
   }
   else {
      checkMethodDuplicates(scope, node, message, message, false, false);
   }

   if (!outputRef) {
      if (methodInfo.outputRef && !node.existChild(SyntaxKey::Autogenerated) 
         && !test(methodInfo.hints, (ref_t)MethodHint::Constructor)
         && outputRef != scope.moduleScope->buildins.constructor_message) 
      {
         //warn if the method output was not redeclared, ignore auto generated methods
         //!!hotfix : ignore the warning for the constructor
         scope.raiseWarning(WARNING_LEVEL_1, wrnTypeInherited, node);
      }
   }
   else {
      if (methodInfo.outputRef && methodInfo.outputRef != outputRef) {
         scope.raiseError(errTypeAlreadyDeclared, node);
      }
      methodInfo.outputRef = outputRef;
   }
}

void Compiler :: generateMethodDeclaration(ClassScope& scope, SyntaxNode node, bool closed)
{
   mssg_t message = node.arg.reference;
   MethodInfo methodInfo = {};

   auto methodIt = scope.info.methods.getIt(message);
   bool existing = !methodIt.eof();
   if (existing)
      methodInfo = *methodIt;

   generateMethodAttributes(scope, node, methodInfo, scope.abstractBasedMode);

   // check if there is no duplicate method
   if (existing && !methodInfo.inherited) {
      scope.raiseError(errDuplicatedMethod, node);
   }
   else {
      bool privateOne = MethodScope::checkHint(methodInfo, MethodHint::Private);
      bool internalOne = MethodScope::checkHint(methodInfo, MethodHint::Internal);
      bool autoMultimethod = SyntaxTree::ifChildExists(node, SyntaxKey::Autogenerated, -1);
      bool predefined = MethodScope::checkHint(methodInfo, MethodHint::Predefined);

      // if the class is closed, no new methods can be declared
      // except private sealed ones (which are declared outside the class VMT)
      if (!existing && closed && !privateOne && !predefined) {
         IdentifierString messageName;
         ByteCodeUtil::resolveMessageName(messageName, scope.module, message);

         _errorProcessor->info(infoNewMethod, *messageName);

         scope.raiseError(errClosedParent, node);
      }

      if (existing && MethodScope::checkType(methodInfo, MethodHint::Sealed))
         scope.raiseError(errClosedMethod, node);

      if (test(scope.info.header.flags, elExtension) && (privateOne || internalOne))
         // private / internal methods cannot be declared in the extension
         scope.raiseError(errIllegalPrivate, node);

      // HOTFIX : ignore auto generated multi-methods
      if (test(scope.info.header.flags, elExtension) && !autoMultimethod)
         declareExtension(scope, message, scope.visibility != Visibility::Public);

      methodInfo.inherited = false;

      if (predefined)
         methodInfo.hints &= ~((ref_t)MethodHint::Predefined);

      if (!existing) {
         scope.info.methods.add(message, methodInfo);
      }
      else *methodIt = methodInfo;
   }
}

inline mssg_t retrieveMethod(List<mssg_t>& implicitMultimethods, mssg_t multiMethod)
{
   return implicitMultimethods.retrieve<mssg_t>(multiMethod, [](mssg_t arg, mssg_t current)
      {
         return current == arg;
      });
}

mssg_t Compiler :: defineMultimethod(ClassScope& scope, mssg_t messageRef)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(messageRef, actionRef, argCount, flags);

   if (argCount == 1)
      return 0;

   ustr_t actionStr = scope.module->resolveAction(actionRef, signRef);

   if (signRef) {
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);
      mssg_t genericMessage = encodeMessage(genericActionRef, argCount, flags);

      return genericMessage;
   }

   return 0;
}

void Compiler :: injectVirtualCode(SyntaxNode classNode, ClassScope& scope)
{
   if (test(scope.info.header.flags, elClassClass)) {

   }
   else if (!test(scope.info.header.flags, elNestedClass) && 
      !test(scope.info.header.flags, elRole))
   {
      if (!evalInitializers(scope, classNode)) {
         injectInitializer(classNode, SyntaxKey::Method, scope.moduleScope->buildins.init_message);
      }

      // skip class classes, extensions and singletons
      if (scope.reference != scope.moduleScope->buildins.superReference && !test(scope.info.header.flags, elClosed)) {
         // auto generate cast$<type> message for explicitly declared classes
         ref_t signRef = scope.module->mapSignature(&scope.reference, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         injectVirtualReturningMethod(scope.moduleScope, classNode,
            encodeMessage(actionRef, 1, CONVERSION_MESSAGE),
            *scope.moduleScope->selfVar, scope.reference);
      }
   }
}

void Compiler :: injectVirtualMultimethods(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, ClassInfo& info,
   List<mssg_t>& implicitMultimethods)
{
   // generate implicit mutli methods
   for (auto it = implicitMultimethods.start(); !it.eof(); ++it) {
      MethodInfo methodInfo = {};

      auto m_it = info.methods.getIt(*it);
      bool found = !m_it.eof();
      if (found)
         methodInfo = *m_it;

      if (!found || methodInfo.inherited) {
         injectVirtualMultimethod(classNode, methodType, scope, info, *it, methodInfo.inherited, methodInfo.outputRef);
      }
   }
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey, bool closed)
{
   List<mssg_t> implicitMultimethods(0);
   bool thirdPassRequired = false;

   // first pass - mark all multi-methods
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodKey) {
         mssg_t multiMethod = defineMultimethod(scope, current.arg.reference);
         if (multiMethod) {
            //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
            current.appendChild(SyntaxKey::Multimethod, multiMethod);

            if (retrieveMethod(implicitMultimethods, multiMethod) == 0) {
               implicitMultimethods.add(multiMethod);
               thirdPassRequired = true;
            }
         }
      }
      current = current.nextNode();
   }

   // second pass - ignore template based / autogenerated methods
   current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodKey) {
         if (!current.existChild(SyntaxKey::Autogenerated)) {
            generateMethodDeclaration(scope, current, closed);
         }
         else thirdPassRequired = true;
      }

      current = current.nextNode();
   }

   //COMPILER MAGIC : if strong signature is declared - the compiler should contain the virtual multi method
   if (implicitMultimethods.count() > 0) {
      injectVirtualMultimethods(node, methodKey, *scope.moduleScope, scope.info, implicitMultimethods);
   }

   // third pass - do not include overwritten template-based methods
   if (thirdPassRequired) {
      current = node.firstChild();
      while (current != SyntaxKey::None) {
         if (current == methodKey && current.existChild(SyntaxKey::Autogenerated)) {
            generateMethodDeclaration(scope, current, closed);
         }
         current = current.nextNode();
      }
   }

   if (implicitMultimethods.count() > 0)
      _logic->verifyMultimethods(/**scope.moduleScope, root, scope.info, implicitMultimethods*/);
}

void Compiler :: generateClassFlags(ClassScope& scope, ref_t declaredFlags)
{
   scope.info.header.flags |= declaredFlags;

   if (test(scope.info.header.flags, elExtension))
      scope.addAttribute(ClassAttribute::ExtensionRef, scope.extensionClassRef);
}

void Compiler :: generateClassStaticField(ClassScope& scope, SyntaxNode node, bool isConst, ref_t typeRef)
{
   ustr_t name = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (scope.info.statics.exist(name)) {
      scope.raiseError(errDuplicatedField, node);
   }

   if (isConst) {
      // NOTE : the index is 0 for the constants
      // NOTE : INVALID_REF indicates that the value should be assigned later
      scope.info.statics.add(name, { 0, typeRef, INVALID_REF });
   }
   else assert(false);
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxNode node,
   FieldAttributes& attrs, bool singleField)
{
   ref_t classRef = attrs.typeRef;
   int   sizeHint = attrs.size;
   bool  embeddable = attrs.isEmbeddable;
   ref_t flags = scope.info.header.flags;

   if (sizeHint == -1) {
      if (singleField) {
         scope.info.header.flags |= elDynamicRole;
      }
      else scope.raiseError(errIllegalField, node);

      sizeHint = 0;
   }

   int offset = 0;
   ustr_t name = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();

   // a role cannot have fields
   if (test(flags, elStateless))
      scope.raiseError(errIllegalField, node);

   SizeInfo sizeInfo = {};
   if (isPrimitiveRef(classRef)) {
      if (!sizeHint) {
         scope.raiseError(errIllegalField, node);
      }
      // for primitive types size should be specified
      else sizeInfo.size = sizeHint;
   }
   else if (classRef)
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, classRef);

   if (test(flags, elWrapper) && scope.info.fields.count() > 0) {
      // wrapper may have only one field
      scope.raiseError(errIllegalField, node);
   }
   else if (embeddable/* && !fieldArray*/) {
      if (!singleField || scope.info.fields.count() > 0)
         scope.raiseError(errIllegalField, node);

      // if the sealed class has only one strong typed field (structure) it should be considered as a field wrapper
      if (test(scope.info.header.flags, elSealed)) {
         scope.info.header.flags |= elWrapper;
         if (sizeInfo.size > 0 && !test(scope.info.header.flags, elNonStructureRole))
            scope.info.header.flags |= elStructureRole;
      }
   }

   // a class with a dynamic length structure must have no fields
   if (test(scope.info.header.flags, elDynamicRole)) {
      if (scope.info.size == 0 && scope.info.fields.count() == 0) {
         // compiler magic : turn a field declaration into an array or string one
         if (sizeInfo.size > 0 && !test(scope.info.header.flags, elNonStructureRole)) {
            scope.info.header.flags |= elStructureRole;
            scope.info.size = -sizeInfo.size;
         }

         ref_t arrayRef = _logic->definePrimitiveArray(*scope.moduleScope, classRef,
            test(scope.info.header.flags, elStructureRole));

         scope.info.fields.add(name, { -2, arrayRef, classRef });
      }
      else scope.raiseError(errIllegalField, node);
   }
   else {
      if (scope.info.fields.exist(name)) {
         scope.raiseError(errDuplicatedField, node);
      }

      // if it is a structure field
      if (test(scope.info.header.flags, elStructureRole)) {
         if (sizeInfo.size <= 0)
            scope.raiseError(errIllegalField, node);

         if (scope.info.size != 0 && scope.info.fields.count() == 0)
            scope.raiseError(errIllegalField, node);

         offset = scope.info.size;
         scope.info.size += sizeInfo.size;
         scope.info.fields.add(name, { offset, classRef });

         if (isPrimitiveRef(classRef))
            _logic->tweakPrimitiveClassFlags(scope.info, classRef);
      }
      else {
         // primitive / virtual classes cannot be declared
         if (sizeInfo.size != 0 && isPrimitiveRef(classRef))
            scope.raiseError(errIllegalField, node);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.count();
         scope.info.fields.add(name, { offset, classRef });
      }
   }
}

void Compiler :: generateClassFields(ClassScope& scope, SyntaxNode node, bool singleField)
{
   bool isClassClassMode = scope.isClassClass();

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current.key == SyntaxKey::Field) {
         FieldAttributes attrs = {};
         declareFieldAttributes(scope, current, attrs);

         if (attrs.isConstant) {
            generateClassStaticField(scope, current, attrs.isConstant, attrs.typeRef);
         }
         else if (!isClassClassMode) {
            generateClassField(scope, current, attrs, singleField);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: generateClassDeclaration(ClassScope& scope, SyntaxNode node, ref_t declaredFlags)
{
   bool closed = test(scope.info.header.flags, elClosed);

   if (scope.isClassClass()) {

   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, declaredFlags);

      // generate fields
      generateClassFields(scope, node, SyntaxTree:: countChild(node, SyntaxKey::Field) == 1);
   }

   injectVirtualCode(node, scope);

   if (scope.isClassClass()) {
      generateMethodDeclarations(scope, node, SyntaxKey::StaticMethod, false);
      generateMethodDeclarations(scope, node, SyntaxKey::Constructor, false);
   }
   else {
      generateMethodDeclarations(scope, node, SyntaxKey::Method, closed);
   }

   bool emptyStructure = false;
   bool customDispatcher = false;
   _logic->validateClassDeclaration(*scope.moduleScope, scope.info, emptyStructure, customDispatcher);
   if (emptyStructure)
      scope.raiseError(errEmptyStructure, node.findChild(SyntaxKey::Name));
   if (customDispatcher)
      scope.raiseError(errDispatcherInInterface, node.findChild(SyntaxKey::Name));

   _logic->tweakClassFlags(scope.reference, scope.info, scope.isClassClass());

   // generate operation list if required
   _logic->injectOverloadList(this, *scope.moduleScope, scope.info, scope.reference);
}

void Compiler :: declareSymbol(SymbolScope& scope, SyntaxNode node)
{
   declareSymbolAttributes(scope, node);

   scope.save();
}

void Compiler :: declareClassParent(ref_t parentRef, ClassScope& scope, SyntaxNode baseNode)
{
   scope.info.header.parentRef = parentRef;
   InheritResult res = InheritResult::irSuccessfull;
   if (scope.info.header.parentRef != 0) {
      res = inheritClass(scope, scope.info.header.parentRef/*, ignoreFields, test(scope.info.header.flags, elVirtualVMT)*/);
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

void Compiler :: resolveClassParent(ClassScope& scope, SyntaxNode baseNode, bool extensionMode)
{
   ref_t parentRef = 0;
   if (baseNode == SyntaxKey::Parent) {
      parentRef = resolveTypeAttribute(scope, baseNode, false);
   }

   if (scope.info.header.parentRef == scope.reference) {
      // if it is a super class
      if (parentRef != 0) {
         scope.raiseError(errInvalidSyntax, baseNode);
      }
   }
   else if (parentRef == 0) {
      parentRef = scope.info.header.parentRef;
   }

   if (extensionMode) {
      //COMPLIER MAGIC : treat the parent declaration in the special way for the extension
      scope.extensionClassRef = parentRef;

      declareClassParent(scope.moduleScope->buildins.superReference, scope, baseNode);
   }
   else declareClassParent(parentRef, scope, baseNode);
}

void Compiler :: importCode(Scope& scope, SyntaxNode node, SyntaxNode& importNode)
{
   ObjectInfo retVal = mapObject(scope, node, EAttr::NoTypeAllowed);
   switch (retVal.kind) {
      case ObjectKind::InternalProcedure:
         importNode.setArgumentReference(retVal.reference);
         break;
      default:
         scope.raiseError(errInvalidSyntax, node);
         break;
   }
}

void Compiler :: declareMetaInfo(Scope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   SyntaxNode noBodyNode = {};
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            importTemplate(scope, current, INLINE_PREFIX, node);
            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope);

            evalStatement(metaScope, current);
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareMethodMetaInfo(MethodScope& scope, SyntaxNode node)
{
   bool withoutBody = false;

   SyntaxNode current = node.firstChild();
   SyntaxNode noBodyNode = {};
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            importTemplate(scope, current, INLINE_PREFIX, node);
            break;
         case SyntaxKey::IncludeStatement:
            if (withoutBody) {
               noBodyNode.setKey(SyntaxKey::Importing);
               importCode(scope, current, noBodyNode);
            }
            else scope.raiseError(errInvalidSyntax, node);

            break;
         case SyntaxKey::Name:
         case SyntaxKey::Attribute:
         case SyntaxKey::CodeBlock:
         case SyntaxKey::ReturnExpression:
         case SyntaxKey::Parameter:
         case SyntaxKey::Type:
         case SyntaxKey::ArrayType:
         case SyntaxKey::TemplateType:
         case SyntaxKey::EOP:
            break;
         case SyntaxKey::WithoutBody:
            withoutBody = true;
            noBodyNode = current;
            break;
         default:
            scope.raiseError(errInvalidSyntax, node);
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareVMTMessage(MethodScope& scope, SyntaxNode node, bool withoutWeakMessages, bool declarationMode)
{
   IdentifierString actionStr;
   ref_t            actionRef = 0;
   ref_t            flags = 0;

   ref_t            signature[ARG_COUNT] = {};
   size_t           signatureLen = 0;

   bool             unnamedMessage = false;

   SyntaxNode nameNode = node.findChild(SyntaxKey::Name);
   SyntaxNode terminal = nameNode.firstChild(SyntaxKey::TerminalMask);

   if (terminal.key == SyntaxKey::identifier) {
      actionStr.copy(terminal.identifier());
   }
   else unnamedMessage = true;

   bool weakSignature = true;
   pos_t paramCount = 0;
   if (scope.checkHint(MethodHint::Extension)) {
      ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

      // COMPILER MAGIC : for an extension method, self is a parameter
      paramCount++;

      signature[0] = classScope->extensionClassRef;
      signatureLen++;

      weakSignature = false;

      scope.parameters.add(*scope.moduleScope->selfVar, { 1, signature[0] });

      flags |= FUNCTION_MESSAGE;
   }

   //bool noSignature = true; // NOTE : is similar to weakSignature except if withoutWeakMessages=true
   // if method has an argument list
   SyntaxNode current = node.findChild(SyntaxKey::Parameter);
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         int index = 1 + scope.parameters.count();
         ref_t classRef = 0;
         ref_t elementRef = 0;
         int size = 0;
         declareArgumentAttributes(scope, current, classRef/*, elementRef*/, declarationMode);

         if (withoutWeakMessages && !classRef)
            classRef = scope.moduleScope->buildins.superReference;

         if (classRef) 
            weakSignature = false;

         ustr_t terminal = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
         if (scope.parameters.exist(terminal))
            scope.raiseError(errDuplicatedLocal, current);

         paramCount++;
         if (paramCount >= ARG_COUNT/* || (flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE*/)
            scope.raiseError(errTooManyParameters, current);

         signature[signatureLen++] = classRef;

         scope.parameters.add(terminal, Parameter(index, classRef, elementRef, size));
      }
      else break;

      current = current.nextNode();
   }

   // if the signature consists only of generic parameters - ignore it
   if (weakSignature)
      signatureLen = 0;

   // HOTFIX : do not overwrite the message on the second pass
   if (scope.message == 0) {
      if (scope.checkType(MethodHint::Dispatcher)) {
         if (paramCount == 0 && unnamedMessage) {
            actionRef = getAction(scope.moduleScope->buildins.dispatch_message);
            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (scope.checkHint(MethodHint::Conversion)) {
         if (paramCount == 0 && unnamedMessage && scope.info.outputRef) {
            ref_t signatureRef = scope.moduleScope->module->mapSignature(&scope.info.outputRef, 1, false);
            actionRef = scope.moduleScope->module->mapAction(CAST_MESSAGE, signatureRef, false);
            flags |= CONVERSION_MESSAGE;

            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }
      else if (scope.checkHint(MethodHint::Constructor) && unnamedMessage) {
         actionStr.copy(CONSTRUCTOR_MESSAGE);
         unnamedMessage = false;
         flags |= FUNCTION_MESSAGE;
      }
      else if (scope.checkHint(MethodHint::Function)) {
         if (!unnamedMessage)
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(INVOKE_MESSAGE);

         flags |= FUNCTION_MESSAGE;
         unnamedMessage = false;
      }

      if (scope.checkHint(MethodHint::GetAccessor) || scope.checkHint(MethodHint::SetAccessor)) {
         flags |= PROPERTY_MESSAGE;
      }

      if (scope.checkHint(MethodHint::Internal)) {
         actionStr.insert("$$", 0);
         actionStr.insert(scope.module->name(), 0);
      }
      else if (scope.checkHint(MethodHint::Protected)) {
         // check if protected method already declared
         mssg_t publicMessage = mapMethodName(scope, paramCount, *actionStr, actionRef, flags,
            signature, signatureLen);

         mssg_t declaredMssg = scope.getAttribute(publicMessage, ClassAttribute::ProtectedAlias);
         if (!declaredMssg) {
            ustr_t className = scope.module->resolveReference(scope.getClassRef());

            actionStr.insert("$$", 0);
            actionStr.insert(className + 1, 0);
            actionStr.insert("@", 0);
            actionStr.insert(scope.module->name(), 0);
            actionStr.replaceAll('\'', '@', 0);
         }
         else scope.message = declaredMssg;
      }
      else if (scope.checkHint(MethodHint::Private)) {
         flags |= STATIC_MESSAGE;
      }

      if (!scope.message) {
         scope.message = mapMethodName(scope, paramCount, *actionStr, actionRef, flags,
            signature, signatureLen);
         if (unnamedMessage || !scope.message)
            scope.raiseError(errIllegalMethod, node);
      }
   }
}

void Compiler :: declareClosureMessage(MethodScope& methodScope, SyntaxNode node)
{
   ref_t invokeAction = methodScope.module->mapAction(INVOKE_MESSAGE, 0, false);
   methodScope.message = encodeMessage(invokeAction, 0, FUNCTION_MESSAGE);
   methodScope.closureMode = true;
}

void Compiler :: declareMethod(MethodScope& methodScope, SyntaxNode node, bool abstractMode)
{
   if (methodScope.info.outputRef)
      node.appendChild(SyntaxKey::OutputType, methodScope.info.outputRef);

   if (methodScope.checkHint(MethodHint::Static)) {
      node.setKey(SyntaxKey::StaticMethod);

      methodScope.info.hints |= (ref_t)MethodHint::Normal;
   }
   else if (methodScope.checkHint(MethodHint::Constructor)) {
      if (abstractMode && !methodScope.isPrivate()) {
         // abstract class cannot have public constructors
         methodScope.raiseError(errIllegalConstructorAbstract, node);
      }
      else node.setKey(SyntaxKey::Constructor);

      methodScope.info.hints |= (ref_t)MethodHint::Normal;
   }
   else if (methodScope.checkHint(MethodHint::Predefined)) {
      node.setKey(SyntaxKey::PredefinedMethod);
   }
   else methodScope.info.hints |= (ref_t)MethodHint::Normal;

   if (methodScope.info.hints)
      node.appendChild(SyntaxKey::Hints, methodScope.info.hints);

   //SyntaxNode current = node.firstChild();
   //while (current.key != SyntaxKey::None) {

   //   current = current.nextNode();
   //}
}

void Compiler :: declareVMT(ClassScope& scope, SyntaxNode node, bool& withConstructors, bool& withDefaultConstructor)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Method:
         {
            MethodScope methodScope(&scope);
            declareMethodAttributes(methodScope, current, scope.extensionClassRef != 0);

            if (!current.arg.reference) {
               // NOTE : an extension method must be strong-resolved
               declareVMTMessage(methodScope, current,
                  methodScope.checkHint(MethodHint::Extension), true);

               current.setArgumentReference(methodScope.message);
            }
            else methodScope.message = current.arg.reference;

            declareMethodMetaInfo(methodScope, current);
            declareMethod(methodScope, current, scope.abstractMode);

            if (methodScope.checkHint(MethodHint::Constructor)) {
               withConstructors = true;
               if ((methodScope.message & ~STATIC_MESSAGE) == scope.moduleScope->buildins.constructor_message) {
                  withDefaultConstructor = true;
               }
            }
            else if (methodScope.checkHint(MethodHint::Predefined)) {
               auto info = scope.info.methods.get(methodScope.message);
               if (!info.hints) {
                  // HOTFIX : the predefined method info should be saved separately
                  scope.info.methods.add(methodScope.message, methodScope.info);
               }
               else scope.raiseError(errIllegalMethod, current);
            }

            if (!_logic->validateMessage(*scope.moduleScope, methodScope.info.hints, methodScope.message)) {
               scope.raiseError(errIllegalMethod, current);
            }
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareClassClass(ClassScope& classClassScope, SyntaxNode node)
{
   classClassScope.info.header.flags |= elClassClass; // !! IMPORTANT : classclass flags should be set

   declareClassParent(classClassScope.moduleScope->buildins.superReference, classClassScope, node);

   generateClassDeclaration(classClassScope, node, 0);

   // save declaration
   classClassScope.save();
}

bool inline isExtensionDeclaration(SyntaxNode node)
{
   if (SyntaxTree::ifChildExists(node, SyntaxKey::Attribute, V_EXTENSION))
      return true;

   return false;
}

void Compiler :: declareClass(ClassScope& scope, SyntaxNode node)
{
   bool extensionDeclaration = isExtensionDeclaration(node);
   resolveClassParent(scope, node.findChild(SyntaxKey::Parent), extensionDeclaration/*, lxParent*/ );

   ref_t declaredFlags = 0;
   declareClassAttributes(scope, node, declaredFlags);

   bool withConstructors = false;
   bool withDefConstructor = false;
   declareVMT(scope, node, withConstructors, withDefConstructor);

   // NOTE : generateClassDeclaration should be called for the proper class before a class class one
   //        due to dynamic array implementation (auto-generated default constructor should be removed)
   generateClassDeclaration(scope, node, declaredFlags);

   if (_logic->isRole(scope.info)) {
      // class is its own class class
      scope.info.header.classRef = scope.reference;
   }
   else {
      // define class class name
      IdentifierString classClassName(scope.moduleScope->resolveFullName(scope.reference));
      classClassName.append(CLASSCLASS_POSTFIX);

      scope.info.header.classRef = scope.moduleScope->mapFullReference(*classClassName);
   }

   // if it is a super class validate it, generate built-in attributes
   if (scope.reference == scope.moduleScope->buildins.superReference) {
      validateSuperClass(scope, node);
   }

   // save declaration
   scope.save();

   // declare class class if it available
   if (scope.info.header.classRef != scope.reference && scope.info.header.classRef != 0) {
      ClassScope classClassScope((NamespaceScope*)scope.parent, scope.info.header.classRef, scope.visibility);

      if (!withDefConstructor &&!scope.abstractMode && !test(scope.info.header.flags, elDynamicRole)) {
         // if default constructor has to be created
         injectDefaultConstructor(scope.moduleScope, node);
      }

      declareClassClass(classClassScope, node);

      // HOTFIX : if the default constructor is private - a class cannot be inherited
      if (withDefConstructor 
         && classClassScope.info.methods.exist(scope.moduleScope->buildins.constructor_message | STATIC_MESSAGE))
      {
         scope.info.header.flags |= elFinal;
         scope.save();
      }
   }
}

void Compiler :: declareMembers(NamespaceScope& ns, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Namespace:
         {
            NamespaceScope namespaceScope(&ns);
            declareNamespace(namespaceScope, current, false, true);

            declareMembers(namespaceScope, current);
            break;
         }
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, current.arg.reference, ns.defaultVisibility);

            declareSymbol(symbolScope, current);
            break;
         }
         case SyntaxKey::Class:
         {
            ClassScope classScope(&ns, current.arg.reference, ns.defaultVisibility);

            declareClass(classScope, current);
            break;
         }
         case SyntaxKey::MetaExpression:
         {
            MetaScope scope(&ns);

            evalStatement(scope, current);
            break;
         }
         case SyntaxKey::ReloadStatement:
         {
            IdentifierString dictionaryName(
               FORWARD_PREFIX_NS,
               META_PREFIX,
               current.firstChild(SyntaxKey::TerminalMask).identifier());

            reloadMetaDictionary(ns.moduleScope, *dictionaryName);
            break;
         }
         case SyntaxKey::Template:
         {
            TemplateScope templateScope(&ns, 0, ns.defaultVisibility);
            declareTemplateClass(templateScope, current);
            break;
         }
         case SyntaxKey::TemplateCode:
         {
            TemplateScope templateScope(&ns, 0, ns.defaultVisibility);
            declareTemplateCode(templateScope, current);
            break;
         }
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}


void Compiler :: declareMemberIdentifiers(NamespaceScope& ns, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Namespace:
         {
            NamespaceScope namespaceScope(&ns);
            declareNamespace(namespaceScope, current, true, true);
            ns.moduleScope->newNamespace(*namespaceScope.nsName);

            declareMemberIdentifiers(namespaceScope, current);
            break;
         }
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, 0, ns.defaultVisibility);
            declareSymbolAttributes(symbolScope, current);

            SyntaxNode name = current.findChild(SyntaxKey::Name);

            ref_t reference = mapNewTerminal(symbolScope, nullptr, name, nullptr, symbolScope.visibility);
            symbolScope.module->mapSection(reference | mskSymbolRef, false);

            current.setArgumentReference(reference);
            break;
         }
         case SyntaxKey::Class:
         {
            ClassScope classScope(&ns, 0, ns.defaultVisibility);

            ref_t flags = classScope.info.header.flags;
            declareClassAttributes(classScope, current, flags);

            SyntaxNode name = current.findChild(SyntaxKey::Name);

            classScope.reference = mapNewTerminal(classScope, nullptr, name, nullptr, classScope.visibility);
            classScope.module->mapSection(classScope.reference | mskSymbolRef, false);

            current.setArgumentReference(classScope.reference);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(ns, current, Visibility::Public);
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: importExtensions(NamespaceScope& ns, ustr_t importedNs)
{
   ReferenceName sectionName(importedNs, EXTENSION_SECTION);

   auto sectionInfo = ns.moduleScope->getSection(*sectionName, mskMetaExtensionRef, true);
   if (sectionInfo.module) {
      _logic->readExtMessageEntry(sectionInfo.module, sectionInfo.section, ns.extensions, ns.moduleScope);
   }
}

void Compiler :: loadExtensions(NamespaceScope& ns, bool internalOne)
{
   IdentifierString fullName(ns.module->name());
   if (internalOne)
      // HOTFIX : to exclude the tailing quote symbol
      fullName.append(PRIVATE_PREFIX_NS, getlength(PRIVATE_PREFIX_NS) - 1);

   if (!ns.nsName.empty()) {
      fullName.append("'");
      fullName.append(*ns.nsName);
   }
   importExtensions(ns, *fullName);
}

void Compiler :: declareNamespace(NamespaceScope& ns, SyntaxNode node, bool ignoreImport, bool ignoreExtensions)
{
   if (!ignoreExtensions) {
      // HOTFIX : load the module internal and public extensions
      loadExtensions(ns, false);
      loadExtensions(ns, true);
   }

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::SourcePath:
            ns.sourcePath.copy(current.identifier());
            break;
         case SyntaxKey::Name:
            if (ns.nsName.length() > 0)
               ns.nsName.append('\'');

            ns.nsName.append(current.firstChild(SyntaxKey::TerminalMask).identifier());
            break;
         case SyntaxKey::Import:
            if (!ignoreImport) {
               bool duplicateInclusion = false;
               ustr_t name = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
               if (ns.moduleScope->includeNamespace(ns.importedNs, name, duplicateInclusion)) {
                  if (!ignoreExtensions)
                     importExtensions(ns, name);
               }
               else if (duplicateInclusion) {
                  ns.raiseWarning(WARNING_LEVEL_1, wrnDuplicateInclude, current);

                  // HOTFIX : comment out, to prevent duplicate warnings
                  current.setKey(SyntaxKey::Idle);
               }
               else {
                  ns.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, current);
                  current.setKey(SyntaxKey::Idle); // remove the node, to prevent duplicate warnings
               }
            }
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}

ObjectInfo Compiler :: evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id)
{
   ObjectInfo loperand = {};
   ObjectInfo roperand = {};
   ObjectInfo ioperand = {};
   ref_t argCount = 1;

   SyntaxNode lnode = node.firstChild(SyntaxKey::DeclarationMask);
   SyntaxNode rnode = lnode.nextNode(SyntaxKey::DeclarationMask);
   if (lnode == SyntaxKey::IndexerOperation) {
      SyntaxNode sublnode = lnode.firstChild(SyntaxKey::DeclarationMask);
      SyntaxNode subrnode = sublnode.nextNode(SyntaxKey::DeclarationMask);

      loperand = evalExpression(interpreter, scope, sublnode);
      ioperand = evalExpression(interpreter, scope, subrnode);
      roperand = evalExpression(interpreter, scope, rnode);

      if (operator_id == SET_OPERATOR_ID) {
         operator_id = SET_INDEXER_OPERATOR_ID;
      }
      else scope.raiseError(errCannotEval, node);

      argCount = 3;
   }
   else {
      loperand = evalExpression(interpreter, scope, lnode);
      if (rnode != SyntaxKey::None) {
         argCount = 2;
         roperand = evalExpression(interpreter, scope, rnode);
      }
   }

   ArgumentsInfo arguments;
   ref_t         argumentRefs[3] = {};
   argumentRefs[0] = resolveObjectReference(scope, loperand, false);
   arguments.add(loperand);

   if (argCount >= 2) {
      argumentRefs[1] = resolveObjectReference(scope, roperand, false);
      arguments.add(roperand);
   }

   if (argCount == 3) {
      argumentRefs[2] = resolveObjectReference(scope, ioperand, false);
      arguments.add(ioperand);
   }

   ref_t outputRef = 0;
   bool needToAlloc = false;
   BuildKey opKey = _logic->resolveOp(*scope.moduleScope, operator_id, argumentRefs, argCount, outputRef, needToAlloc);

   ObjectInfo retVal = loperand;
   if (needToAlloc || !interpreter.eval(opKey, operator_id, arguments, retVal)) {
      scope.raiseError(errCannotEval, node);
   }

   return retVal;
}

ObjectInfo Compiler :: evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node)
{
   EAttrs mode = ExpressionAttribute::Meta;

   SyntaxNode terminalNode = node.lastChild(SyntaxKey::TerminalMask);

   ref_t declaredRef = 0;
   declareExpressionAttributes(scope, node, declaredRef, mode);

   return mapTerminal(scope, terminalNode, declaredRef, mode.attrs);
}

ObjectInfo Compiler :: evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node)
{
   switch (node.key) {
      case SyntaxKey::Expression:
         return evalExpression(interpreter, scope, node.firstChild(SyntaxKey::DeclarationMask));
      case SyntaxKey::AssignOperation:
      case SyntaxKey::AddAssignOperation:
      case SyntaxKey::NameOperation:
         return evalOperation(interpreter, scope, node, (int)node.key - OPERATOR_MAKS);
      case SyntaxKey::Object:
         return evalObject(interpreter, scope, node);
      default:
         scope.raiseError(errCannotEval, node);
         break;
   }

   return {};
}

void Compiler :: evalStatement(MetaScope& scope, SyntaxNode node)
{
   Interpreter interpreter(scope.moduleScope, _logic);

   ObjectInfo retVal = evalExpression(interpreter, scope, node.findChild(SyntaxKey::Expression));
   if (retVal.kind == ObjectKind::Unknown)
      scope.raiseError(errCannotEval, node);
}

inline bool hasToBePresaved(ObjectInfo retVal)
{
   return retVal.kind == ObjectKind::Object || retVal.kind == ObjectKind::External || retVal.kind == ObjectKind::Symbol;
}

inline void createObject(BuildTreeWriter& writer, ClassInfo& info, ref_t reference)
{
   if (test(info.header.flags, elStructureRole)) {
      writer.newNode(BuildKey::CreatingStruct, info.size);
   }
   else writer.newNode(BuildKey::CreatingClass, info.fields.count());

   writer.appendNode(BuildKey::Type, reference);

   writer.closeNode();
}

inline void copyObjectToAcc(BuildTreeWriter& writer, ClassInfo& info, int offset)
{
   if (test(info.header.flags, elStructureRole)) {
      writer.newNode(BuildKey::AccCopying, offset);
      writer.appendNode(BuildKey::Size, info.size);
   }
   else throw InternalError(errFatalError);

   writer.closeNode();
}

ObjectInfo Compiler :: boxArgumentInPlace(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info)
{
   ref_t typeRef = resolveObjectReference(scope, info);

   ObjectInfo tempLocal = {};
   if (hasToBePresaved(info)) {
      info = saveToTempLocal(writer, scope, info);
   }
   tempLocal = declareTempLocal(scope, typeRef);

   ClassInfo argInfo;
   _logic->defineClassInfo(*scope.moduleScope, argInfo, typeRef, false, true);

   createObject(writer, argInfo, typeRef);
   writer.appendNode(BuildKey::Assigning, tempLocal.argument);

   writeObjectInfo(writer, scope, info);
   writer.appendNode(BuildKey::SavingInStack, 0);
   writeObjectInfo(writer, scope, tempLocal);
   copyObjectToAcc(writer, argInfo, tempLocal.reference);

   return tempLocal;
}

inline bool isBoxingRequired(ObjectInfo info)
{
   switch (info.kind) {
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
         return true;
      default:
         return false;
   }
}

ObjectInfo Compiler :: boxArgumentLocally(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, bool boxInPlace)
{
   switch (info.kind) {
      case ObjectKind::ReadOnlyFieldAddress:
      case ObjectKind::FieldAddress:
         if (info.argument == 0) {
            ObjectInfo retVal = scope.mapSelf();
            retVal.type = info.type;

            return retVal;
         }
         else throw InternalError(errFatalError);
      default:
         return info;
   }
}

ObjectInfo Compiler :: boxArgument(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, bool stackSafe, bool boxInPlace)
{
   ObjectInfo retVal = { ObjectKind::Unknown };

   if (!stackSafe && isBoxingRequired(info)) {
      ObjectKey key = { info.kind, info.reference };

      if (!boxInPlace)
         retVal = scope.tempLocals.get(key);

      if (retVal.kind == ObjectKind::Unknown) {
         retVal = boxArgumentInPlace(writer, scope, info);

         if (!boxInPlace)
            scope.tempLocals.add(key, retVal);
      }
   }
   else retVal = info;

   return retVal;
}

void Compiler :: writeObjectInfo(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info)
{
   switch (info.kind) {
      case ObjectKind::IntLiteral:
         writer.newNode(BuildKey::IntLiteral, info.reference);
         writer.appendNode(BuildKey::Value, info.extra);
         writer.closeNode();
         break;
      case ObjectKind::StringLiteral:
         writer.appendNode(BuildKey::StringLiteral, info.reference);
         break;
      case ObjectKind::CharacterLiteral:
         writer.appendNode(BuildKey::CharLiteral, info.reference);
         break;
         //case ObjectKind::MetaDictionary:
      //   writer.appendNode(BuildKey::MetaDictionary, info.reference);
      //   break;
      //case ObjectKind::MetaArray:
      //   writer.appendNode(BuildKey::MetaArray, info.reference);
      //   break;
      case ObjectKind::Nil:
         writer.appendNode(BuildKey::NilReference, 0);
         break;
      case ObjectKind::Symbol:
         writer.appendNode(BuildKey::SymbolCall, info.reference);
         break;
      case ObjectKind::Class:
      case ObjectKind::Singleton:
      case ObjectKind::ConstantRole:
         writer.appendNode(BuildKey::ClassReference, info.reference);
         break;
      case ObjectKind::Param:
      case ObjectKind::SelfLocal:
      case ObjectKind::ReadOnlySelfLocal:
      case ObjectKind::Local:
      case ObjectKind::TempLocal:
         writer.appendNode(BuildKey::Local, info.reference);
         break;
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
         writer.appendNode(BuildKey::LocalAddress, info.reference);
         break;
      case ObjectKind::Field:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::StaticConstField:
      case ObjectKind::StaticField:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::ClassOp, CLASS_OPERATOR_ID);
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::Object:
         break;
      default:
         throw InternalError(errFatalError);
   }
}

ref_t Compiler :: resolveObjectReference(Scope& scope, ObjectInfo info, bool noPrimitiveAllowed)
{
   ref_t typeRef = info.type;
   if (noPrimitiveAllowed && isPrimitiveRef(typeRef)) {
      typeRef = resolvePrimitiveReference(scope, info);
   }

   return typeRef;
}

ref_t Compiler :: resolvePrimitiveReference(Scope& scope, ObjectInfo info)
{
   switch (info.type) {
      case V_INT32:
         return scope.moduleScope->buildins.intReference;
      case V_STRING:
         return scope.moduleScope->buildins.literalReference;
      case V_FLAG:
         return scope.moduleScope->branchingInfo.typeRef;
      default:
         throw InternalError(errFatalError);
   }
}

void Compiler :: declareSymbolAttributes(SymbolScope& scope, SyntaxNode node)
{
   bool constant = false;
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         if (!_logic->validateSymbolAttribute(current.arg.value, scope.visibility, constant, scope.isStatic)) {
            current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }

      current = current.nextNode();
   }

   if (constant) {
      scope.info.symbolType = SymbolType::Constant;
   }
}

void Compiler :: declareClassAttributes(ClassScope& scope, SyntaxNode node, ref_t& flags)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateClassAttribute(current.arg.value, flags, scope.visibility)) {
               current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            }
            break;
         case SyntaxKey::Type:
            scope.raiseError(errInvalidSyntax, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   // handle the abstract flag
   if (test(scope.info.header.flags, elAbstract)) {
      if (!test(flags, elAbstract)) {
         scope.abstractBasedMode = true;
         scope.info.header.flags &= ~elAbstract;
      }
      else scope.abstractMode = true;
   }
   else scope.abstractMode = test(flags, elAbstract);
}

inline bool isMethodKind(ref_t hint)
{
   return (hint & (ref_t)MethodHint::Mask) != 0;
}

void Compiler :: declareArgumentAttributes(MethodScope& scope, SyntaxNode node, ref_t& typeRef, bool declarationMode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Type:
            // if it is a type attribute
            typeRef = resolveTypeAttribute(scope, current, declarationMode);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareMethodAttributes(MethodScope& scope, SyntaxNode node, bool exensionMode)
{
   if (exensionMode)
      scope.info.hints |= (ref_t)MethodHint::Extension;

   SyntaxNode current = node.firstChild();
   bool explicitMode = false;
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
         {
            ref_t value = current.arg.reference;

            ref_t hint = 0;
            if (_logic->validateMethodAttribute(value, hint, explicitMode)) {
               if (isMethodKind(hint) && isMethodKind(scope.info.hints)) {
                  // a method kind can be set only once
                  scope.raiseError(errInvalidHint, node);
               }
               else scope.info.hints |= hint;
            }
            else {
               current.setArgumentReference(0);

               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, node);
            }
            break;
         }
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
         case SyntaxKey::ArrayType:
            // if it is a type attribute
            scope.info.outputRef = resolveTypeAttribute(scope, current, true);
            break;
         case SyntaxKey::Name:
         {
            // resolving implicit method attributes
            ref_t attr = scope.moduleScope->attributes.get(current.firstChild(SyntaxKey::TerminalMask).identifier());
            ref_t hint = (ref_t)MethodHint::None;
            if (_logic->validateImplicitMethodAttribute(attr, hint)) {
               scope.info.hints |= hint;
               current.setKey(SyntaxKey::Attribute);
               current.setArgumentReference(attr);
            }
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareTemplateAttributes(TemplateScope& scope, SyntaxNode node, IdentifierString& postfix)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateTemplateAttribute(current.arg.value, scope.visibility, scope.type))
            {
               current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            }
            break;
         case SyntaxKey::Type:
            scope.raiseError(errInvalidSyntax, current);
            break;
         case SyntaxKey::Postfix:
            postfix.append(':');
            postfix.append(current.firstChild(SyntaxKey::TerminalMask).identifier());
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: saveTemplate(TemplateScope& scope, SyntaxNode& node)
{
   MemoryBase* target = scope.module->mapSection(scope.reference | mskSyntaxTreeRef, false);

   SyntaxTree::saveNode(node, target);
}

void Compiler :: declareTemplate(TemplateScope& scope, SyntaxNode& node)
{
   switch (scope.type) {
      case TemplateType::Inline:
      case TemplateType::Class:
      case TemplateType::Statement:
         break;
      default:
         scope.raiseError(errInvalidSyntax, node);
         break;
   }

   saveTemplate(scope, node);

   node.setKey(SyntaxKey::Idle);
}

void Compiler :: declareTemplateCode(TemplateScope& scope, SyntaxNode& node)
{
   IdentifierString prefix;
   IdentifierString postfix;
   declareTemplateAttributes(scope, node, postfix);
   if (scope.type == TemplateType::None)
      scope.type = TemplateType::Statement;

   int argCount = SyntaxTree::countChild(node, SyntaxKey::TemplateArg);
   int paramCount = SyntaxTree::countChild(node, SyntaxKey::Parameter);
   postfix.append('#');
   postfix.appendInt(argCount);

   switch (scope.type) {
      case TemplateType::Inline:
         prefix.append(INLINE_PREFIX);
         if (paramCount > 0)
            scope.raiseError(errInvalidSyntax, node);
         break;
      case TemplateType::Statement:
         postfix.append('#');
         postfix.appendInt(paramCount);
         break;
      default:
         break;
   }

   SyntaxNode name = node.findChild(SyntaxKey::Name);
   scope.reference = mapNewTerminal(scope, *prefix, name, *postfix, scope.visibility);
   if (scope.module->mapSection(scope.reference | mskSyntaxTreeRef, true))
      scope.raiseError(errDuplicatedDictionary, name.firstChild(SyntaxKey::TerminalMask));

   declareMetaInfo(scope, node);
   declareTemplate(scope, node);
}

void Compiler :: declareTemplateClass(TemplateScope& scope, SyntaxNode& node)
{
   scope.type = TemplateType::Class;

   declareTemplate(scope, node);
}

void Compiler :: declareDictionaryAttributes(Scope& scope, SyntaxNode node, ref_t& dictionaryType)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         if (!_logic->validateDictionaryAttribute(current.arg.value, dictionaryType)) {
            current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current == SyntaxKey::Type)
         scope.raiseError(errInvalidHint, current);

      current = current.nextNode();
   }
}

void Compiler :: declareExpressionAttributes(Scope& scope, SyntaxNode node, ref_t& typeRef, ExpressionAttributes& mode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None)  {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateExpressionAttribute(current.arg.reference, mode))
               scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Type:
            if (!EAttrs::test(mode.attrs, EAttr::NoTypeAllowed)) {
               mode |= ExpressionAttribute::NewVariable;
               typeRef = resolveTypeAttribute(scope, current, false);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: addExtensionMessage(Scope& scope, mssg_t message, ref_t extRef, mssg_t strongMessage, bool internalOne)
{
   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   IdentifierString sectionName(internalOne ? PRIVATE_PREFIX_NS : "'");
   if (!ns->nsName.empty()) {
      sectionName.append(*ns->nsName);
      sectionName.append('\'');
   }
   sectionName.append(EXTENSION_SECTION);

   MemoryBase* section = scope.module->mapSection(
      scope.module->mapReference(*sectionName, false) | mskMetaExtensionRef, false);

   _logic->writeExtMessageEntry(section, extRef, message, strongMessage);

   ns->addExtension(message, extRef, strongMessage);
}

void Compiler :: declareExtension(ClassScope& scope, mssg_t message, bool internalOne)
{
   mssg_t extensionMessage = 0;

   // get generic message
   ref_t signRef = 0;
   ustr_t actionName = scope.module->resolveAction(getAction(message), signRef);
   if (signRef) {
      extensionMessage = overwriteAction(message, scope.module->mapAction(actionName, 0, false));
   }
   else extensionMessage = message;

   // exclude function flag
   extensionMessage = extensionMessage & ~FUNCTION_MESSAGE;

   addExtensionMessage(scope, extensionMessage, scope.reference, message, internalOne);
}

void Compiler :: validateType(Scope& scope, ref_t typeRef, SyntaxNode node)
{
   if (!typeRef)
      scope.raiseError(errUnknownClass, node);
}

ref_t Compiler :: resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type, bool declarationMode)
{
   ObjectInfo identInfo;

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   identInfo = ns->mapIdentifier(identifier, type == SyntaxKey::reference, EAttr::None);

   switch (identInfo.kind) {
      case ObjectKind::Class:
         return identInfo.reference;
      case ObjectKind::Symbol:
         if (declarationMode)
            return identInfo.reference;
      default:
         return 0;
   }
}

ref_t Compiler :: mapTemplateType(Scope& scope, SyntaxNode node)
{
   int paramCounter = 0;
   SyntaxNode current = node.nextNode();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::TemplateArg:
            paramCounter++;
         default:
            break;
      }

      current = current.nextNode();
   }

   IdentifierString templateName;
   templateName.appendInt(paramCounter);
   templateName.append('#');
   templateName.append(node.identifier());

   // NOTE : check it in declararion mode - we need only reference
   return resolveTypeIdentifier(scope, *templateName, node.key, true/*, false*/);
}

void Compiler :: declareTemplateAttributes(Scope& scope, SyntaxNode node, 
   List<SyntaxNode>& parameters, bool declarationMode)
{
   SyntaxNode current = node.nextNode();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplateArg && !current.arg.reference) {
         ref_t typeRef = resolveTypeAttribute(scope, current, declarationMode);

         current.setArgumentReference(typeRef);
      }

      parameters.add(current);

      current = current.nextNode();
   }
}

ref_t Compiler :: resolveTypeTemplate(Scope& scope, SyntaxNode node, bool declarationMode)
{
   List<SyntaxNode> parameters({});
   declareTemplateAttributes(scope, node, parameters, declarationMode);

   ref_t templateRef = mapTemplateType(scope, node);
   if (!templateRef)
      scope.raiseError(errInvalidHint, node);

   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
      templateRef, parameters, declarationMode);
}

ref_t Compiler :: resolveTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode)
{
   ref_t typeRef = 0;
   if (SyntaxTree::test(node.key, SyntaxKey::TerminalMask)) {
      if (node.nextNode() == SyntaxKey::TemplateArg) {
         typeRef = resolveTypeTemplate(scope, node, declarationMode);
      }
      else typeRef = resolveTypeIdentifier(scope, node.identifier(), node.key, declarationMode/*, allowRole*/);
   }
   else {
      SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);

      if (terminal.nextNode() == SyntaxKey::TemplateArg) {
         typeRef = resolveTypeTemplate(scope, terminal, declarationMode);
      }
      else typeRef = resolveTypeIdentifier(scope,
         terminal.identifier(), terminal.key, declarationMode/*, allowRole*/);
   }

   validateType(scope, typeRef, node);

   return typeRef;
}

int Compiler :: resolveSize(Scope& scope, SyntaxNode node)
{
   if (node == SyntaxKey::integer) {
      return StrConvertor::toInt(node.identifier(), 10);
   }
   else if (node == SyntaxKey::hexinteger) {
      return StrConvertor::toInt(node.identifier(), 16);
   }
   else {
      scope.raiseError(errInvalidSyntax, node);

      return 0;
   }
}

void Compiler :: readFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateFieldAttribute(current.arg.reference, attrs))
               scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Type:
            if (!attrs.typeRef) {
               attrs.typeRef = resolveTypeAttribute(scope, current, true);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Dimension:
            if (!attrs.size && attrs.typeRef) {
               if (current.arg.value) {
                  attrs.size = current.arg.value;
               }
               else attrs.size = resolveSize(scope, current.firstChild(SyntaxKey::TerminalMask));
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::ArrayType:
            if (!attrs.size) {
               attrs.size = -1;

               readFieldAttributes(scope, current, attrs);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs)
{
   readFieldAttributes(scope, node, attrs);

   //HOTFIX : recognize raw data
   if (isPrimitiveRef(attrs.typeRef)) {
      bool valid = true;
      switch (attrs.typeRef) {
      case V_INTBINARY:
         switch (attrs.size) {
         case 1:
            attrs.typeRef = V_INT8;
            break;
         case 4:
            attrs.typeRef = V_INT32;
            break;
         default:
            valid = false;
            break;
         }
         break;
      case V_WORDBINARY:
         switch (attrs.size) {
         case 4:
            attrs.typeRef = V_WORD32;
            break;
         default:
            valid = false;
            break;
         }
         break;
      default:
         valid = false;
         break;
      }

      if (!valid)
         scope.raiseError(errInvalidHint, node);
   }
}

inline int newLocalAddr(int allocated)
{
   return -allocated;
}

int Compiler :: allocateLocalAddress(CodeScope* codeScope, int size, bool binaryArray)
{
   int disp = binaryArray ? align(4, codeScope->moduleScope->rawStackAlingment) : 0;
   int retVal = codeScope->allocLocalAddress(size + disp);

   return newLocalAddr(retVal - disp);
}

int Compiler :: resolveArraySize(Scope& scope, SyntaxNode node)
{
   Interpreter interpreter(scope.moduleScope, _logic);
   ObjectInfo retVal = evalExpression(interpreter, scope, node);
   switch (retVal.kind) {
      case ObjectKind::IntLiteral:
         return retVal.extra;
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         return 0;
   }
}

void Compiler :: declareVariable(Scope& scope, SyntaxNode terminal, ref_t typeRef)
{
   int size = 0;
   if (terminal == SyntaxKey::IndexerOperation) {
      // COMPILER MAGIC : if it is a fixed-sized array
      size = resolveArraySize(scope, terminal.firstChild(SyntaxKey::ScopeMask));

      terminal = terminal.findChild(SyntaxKey::Object).findChild(SyntaxKey::identifier);
   }

   ExprScope* exprScope = Scope::getScope<ExprScope>(scope, Scope::ScopeLevel::Expr);
   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);
   if (codeScope == nullptr) {
      scope.raiseError(errInvalidOperation, terminal);
      return; // the code will never be reached
   }

   IdentifierString identifier(terminal.identifier());

   ObjectInfo variable;
   variable.type = typeRef;
   variable.kind = ObjectKind::Local;

   if (size != 0 && variable.type != 0) {
      if (!isPrimitiveRef(variable.type)) {
         // if it is a primitive array
         variable.element = variable.type;
         variable.type = _logic->definePrimitiveArray(*scope.moduleScope, variable.element, true);
      }
      else scope.raiseError(errInvalidHint, terminal);
   }

   ClassInfo localInfo;
   //bool binaryArray = false;
   if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.type))
      scope.raiseError(errUnknownVariableType, terminal);

   if (_logic->isEmbeddableArray(localInfo) && size != 0) {
      //binaryArray = true;
      size = size * (-((int)localInfo.size));

      variable.reference = allocateLocalAddress(codeScope, size, true);
   }
   else if (_logic->isEmbeddableStruct(localInfo) && size == 0) {
      size = align(_logic->defineStructSize(localInfo).size,
         scope.moduleScope->rawStackAlingment);

      variable.reference = allocateLocalAddress(codeScope, size, false);
   }
   else if (size != 0) {
      scope.raiseError(errInvalidOperation, terminal);
   }
   else variable.reference = codeScope->newLocal();

   if (exprScope)
      exprScope->syncStack();

   if (!codeScope->locals.exist(*identifier)) {
      codeScope->mapNewLocal(*identifier, variable.reference, variable.type, variable.element, 
         size, true);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

bool Compiler :: evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo)
{
   Interpreter interpreter(scope.moduleScope, _logic);
   MetaScope metaScope(&scope);

   ObjectInfo retVal = evalExpression(interpreter, metaScope, node);
   bool setIndex = false;
   switch (retVal.kind) {
      case ObjectKind::SelfName:
         constInfo.type = V_STRING;
         constInfo.reference = mskNameLiteralRef;
         setIndex = true;
         break;
      default:
         return false;
   }

   auto it = scope.info.statics.getIt(constName);

   assert(!it.eof());

   (*it).valueRef = constInfo.reference;
   if (setIndex && !(*it).offset) {
      scope.info.header.staticSize++;

      (*it).offset = -((int)scope.info.header.staticSize);
   }

   return true;
}

bool Compiler :: evalInitializers(ClassScope& scope, SyntaxNode node)
{
   bool found = false;
   bool evalulated = true;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::AssignOperation) {
         found = true;
         ObjectInfo target = mapObject(scope, current, EAttr::None);
         switch (target.kind) {
            case ObjectKind::Field:
               evalulated = false;
               break;
            case ObjectKind::ClassConstant:
               if (target.reference == INVALID_REF) {
                  if(evalClassConstant(current.firstChild(SyntaxKey::TerminalMask).identifier(),
                     scope, current.firstChild(SyntaxKey::ScopeMask), target))
                  {
                     current.setKey(SyntaxKey::Idle);
                  }
                  else scope.raiseError(errInvalidOperation, current);
               }
               break;
            default:
               scope.raiseError(errInvalidOperation, current);
               break;
         }
      }
      current = current.nextNode();
   }

   return !found || evalulated;
}

ObjectInfo Compiler :: mapClassSymbol(Scope& scope, ref_t classRef)
{
   if (classRef) {
      ObjectInfo retVal = { ObjectKind::Class };
      retVal.reference = classRef;

      ClassInfo info;
      scope.moduleScope->loadClassInfo(info, classRef, true);
      retVal.type = info.header.classRef;

      return retVal;
   }
   else return { };
}

ExternalInfo Compiler :: mapExternal(Scope& scope, SyntaxNode node)
{
   SyntaxNode objNode = node.parentNode();

   ustr_t dllAlias = node.identifier();
   ustr_t functionName = SyntaxTree::gotoNode(objNode, SyntaxKey::Message).firstChild(SyntaxKey::TerminalMask).identifier();

   return scope.moduleScope->mapExternal(dllAlias, functionName);
}

ObjectInfo Compiler :: compileExternalOp(BuildTreeWriter& writer, ExprScope& scope, ref_t externalRef,
   bool stdCall, ArgumentsInfo& arguments)
{
   pos_t count = arguments.count_pos();

   writer.appendNode(BuildKey::Allocating, align(count, scope.moduleScope->stackAlingment));

   for (pos_t i = count; i > 0; i--) {
      ObjectInfo arg = arguments[i - 1];

      writeObjectInfo(writer, scope, arg);
      switch (arg.kind) {
         case ObjectKind::IntLiteral:
            writer.appendNode(BuildKey::SavingNInStack, i - 1);
            break;
         default:
            if (_logic->isCompatible(*scope.moduleScope, V_INT32, 
               resolveObjectReference(scope, arg), true)) 
            {
               writer.appendNode(BuildKey::SavingNInStack, i - 1);
            }
            else writer.appendNode(BuildKey::SavingInStack, i - 1); // !! temporally - passing dynamic references to the exteranl routines should not be allowed
            break;
      }
   }

   writer.newNode(BuildKey::ExtCallOp, externalRef);

   writer.appendNode(BuildKey::Count, count);

   writer.closeNode();

   if (!stdCall)
      writer.appendNode(BuildKey::Freeing, align(count, scope.moduleScope->stackAlingment));

   return { ObjectKind::External, V_INT32, 0 };
}

mssg_t Compiler :: resolveOperatorMessage(ModuleScopeBase* scope, int operatorId)
{
   switch (operatorId) {
      case ADD_OPERATOR_ID:
         return scope->buildins.add_message;
      case IF_OPERATOR_ID:
         return scope->buildins.if_message;
      case IF_ELSE_OPERATOR_ID:
         return overwriteArgCount(scope->buildins.if_message, 3);
      case EQUAL_OPERATOR_ID:
         return scope->buildins.equal_message;
      case NOTEQUAL_OPERATOR_ID:
         return scope->buildins.notequal_message;
      case LESS_OPERATOR_ID:
         return scope->buildins.less_message;
      case NOT_OPERATOR_ID:
         return scope->buildins.not_message;
      default:
         throw InternalError(errFatalError);
   }
}


ObjectInfo Compiler :: declareTempStructure(ExprScope& scope, int size)
{
   if (size <= 0)
      return {};

   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

   ObjectInfo retVal = { ObjectKind::TempLocalAddress };
   retVal.reference = allocateLocalAddress(codeScope, size, false);
   retVal.extra = size;

   scope.syncStack();

   return retVal;
}

ObjectInfo Compiler :: allocateResult(ExprScope& scope, ref_t resultRef)
{
   SizeInfo info = _logic->defineStructSize(*scope.moduleScope, resultRef);
   if (info.size > 0) {
      ObjectInfo retVal = declareTempStructure(scope, info.size);
      retVal.type = resultRef;

      return retVal;
   }
   else throw InternalError(errFatalError);

   return {}; // NOTE : should never be reached
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, SyntaxNode rnode, int operatorId)
{
   ObjectInfo retVal;

   SyntaxNode lnode = node;
   SyntaxNode inode;

   if (operatorId == SET_INDEXER_OPERATOR_ID) {
      lnode = node.firstChild();
      inode = lnode.nextNode();
   }

   BuildKey   op = BuildKey::None;
   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter);
   ObjectInfo roperand = {};
   ObjectInfo ioperand = {};

   pos_t      argLen = 1;
   ref_t      arguments[3] = {};
   arguments[0] = resolveObjectReference(scope, loperand, false);

   // HOTFIX : typecast the right-hand expression if required
   if (rnode != SyntaxKey::None) {
      ref_t rTargetRef = 0;
      if (operatorId == SET_OPERATOR_ID)
         rTargetRef = resolveObjectReference(scope, loperand);

      roperand = compileExpression(writer, scope, rnode, rTargetRef, EAttr::Parameter);

      arguments[argLen++] = resolveObjectReference(scope, roperand, false);
   }

   if (inode != SyntaxKey::None) {
      ioperand = compileExpression(writer, scope, inode, 0, EAttr::Parameter);
      arguments[argLen++] = resolveObjectReference(scope, ioperand, false);
   }

   ref_t outputRef = 0;
   bool  needToAlloc = false;
   op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef, needToAlloc);

   if (op != BuildKey::None) {
      if (needToAlloc) {
         retVal = allocateResult(scope, outputRef);
      }
      else retVal = { ObjectKind::Object, outputRef, 0 };

      // box argument locally if required
      loperand = boxArgumentLocally(writer, scope, loperand, true);
      roperand = boxArgumentLocally(writer, scope, roperand, true);

      writeObjectInfo(writer, scope, loperand);
      writer.appendNode(BuildKey::SavingInStack, 0);

      if (rnode != SyntaxKey::None) {
         writeObjectInfo(writer, scope, roperand);
         writer.appendNode(BuildKey::SavingInStack, 1);
      }

      if (inode != SyntaxKey::None)
         throw InternalError(errFatalError);
         //writeObjectInfo(writer, ioperand);

      writer.newNode(op, operatorId);

      // check if the operation requires an extra arguments
      if (needToAlloc) {
         writer.appendNode(BuildKey::Index, retVal.argument);
      }

      switch (op) {
         case BuildKey::BoolSOp:
         case BuildKey::IntCondOp:
            writer.appendNode(BuildKey::TrueConst, scope.moduleScope->branchingInfo.trueRef);
            writer.appendNode(BuildKey::FalseConst, scope.moduleScope->branchingInfo.falseRef);
            break;
         default:
            break;
      }

      writer.closeNode();

      scope.reserveArgs(argLen);
   }
   else {
      mssg_t message = resolveOperatorMessage(scope.moduleScope, operatorId);
      ArgumentsInfo messageArguments;
      messageArguments.add(loperand);

      if (roperand.kind != ObjectKind::Unknown)
         messageArguments.add(roperand);

      if (ioperand.kind != ObjectKind::Unknown) {
         overwriteArgCount(message, 3);
         messageArguments.add(ioperand);
      }

      ref_t signRef = scope.module->mapSignature(arguments, argLen, false);

      retVal = compileMessageOperation(writer, scope, node, loperand, message, 
         signRef, messageArguments, EAttr::NoExtension);
   }

   return retVal;
}

mssg_t Compiler :: mapMessage(ExprScope& scope, SyntaxNode current, bool propertyMode, bool extensionMode)
{
   ref_t flags = propertyMode ? PROPERTY_MESSAGE : 0;
   if (extensionMode)
      flags |= FUNCTION_MESSAGE;

   IdentifierString messageStr;

   if (current == SyntaxKey::Message) {
      SyntaxNode terminal = current.firstChild(SyntaxKey::TerminalMask);

      messageStr.copy(terminal.identifier());

      current = current.nextNode();
   }

   pos_t argCount = 1;
   while (current != SyntaxKey::None) {
      argCount++;

      current = current.nextNode(SyntaxKey::ScopeMask);
   }

   if (messageStr.empty()) {
      flags |= FUNCTION_MESSAGE;

      // if it is an implicit message
      messageStr.copy(INVOKE_MESSAGE);
   }

   if (test(flags, FUNCTION_MESSAGE))
      // exclude the target from the arg counter for the function
      argCount--;

   ref_t actionRef = scope.module->mapAction(*messageStr, 0, false);

   return encodeMessage(actionRef, argCount, flags);
}

ref_t targetResolver(void* param, mssg_t mssg)
{
   return ((ResolvedMap*)param)->get(mssg);
}

ref_t Compiler :: compileExtensionDispatcher(BuildTreeWriter& writer, NamespaceScope& scope, mssg_t genericMessage, 
   ref_t outputRef)
{
   ref_t extRef = scope.moduleScope->mapAnonymous();
   ClassScope classScope(&scope, extRef, Visibility::Private);
   declareClassParent(classScope.info.header.parentRef, classScope, {});
   classScope.extensionDispatcher = true;
   classScope.info.header.classRef = classScope.reference;
   classScope.extensionClassRef = scope.moduleScope->buildins.superReference;
   generateClassFlags(classScope, elExtension | elSealed);

   // create a new overload list
   ClassInfo::MethodMap methods({});
   ResolvedMap targets(0);
   for(auto it = scope.extensions.getIt(genericMessage); !it.eof(); it = scope.extensions.nextIt(genericMessage, it)) {
      auto extInfo = *it;

      methods.add(extInfo.value2, { false, 0, 0, genericMessage | FUNCTION_MESSAGE });
      targets.add(extInfo.value2, extInfo.value1);
   }

   _logic->injectMethodOverloadList(this, *scope.moduleScope, 
      classScope.info.header.flags, genericMessage | FUNCTION_MESSAGE, methods, 
      classScope.info.attributes, &targets, targetResolver);

   SyntaxTree classTree;
   SyntaxTreeWriter classWriter(classTree);

   // build the class tree
   classWriter.newNode(SyntaxKey::Root);
   classWriter.newNode(SyntaxKey::Class, extRef);

   SyntaxNode classNode = classWriter.CurrentNode();
   injectVirtualMultimethod(classNode, SyntaxKey::Method, genericMessage | FUNCTION_MESSAGE, genericMessage, 
      0, outputRef);

   classWriter.closeNode();
   classWriter.closeNode();

   generateMethodDeclaration(classScope, classNode.findChild(SyntaxKey::Method), false);
   classScope.save();

   writer.newNode(BuildKey::NestedClass, extRef);
   compileVMT(writer, classScope, classNode);
   writer.closeNode();

   return extRef;
}

ref_t Compiler :: compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current, ArgumentsInfo& arguments)
{
   // compile the message argument list
   ref_t signatures[ARG_COUNT] = { 0 };
   ref_t signatureLen = 0;
   ref_t superReference = scope.moduleScope->buildins.superReference;

   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         auto argInfo = compileExpression(writer, scope, current, 0, EAttr::Parameter);
         ref_t argRef = resolveObjectReference(scope, argInfo, false);
         if (argRef) {
            signatures[signatureLen++] = argRef;
         }
         else signatures[signatureLen++] = superReference;

         arguments.add(argInfo);
      }

      current = current.nextNode();
   }

   if (signatureLen > 0 && signatureLen <= ARG_COUNT) {
      bool anonymous = true;
      for (ref_t i = 0; i < signatureLen; i++) {
         if (signatures[i] != superReference) {
            anonymous = false;
            break;
         }
      }
      if (!anonymous)
         return scope.module->mapSignature(signatures, signatureLen, false);
   }

   return 0;
}

ref_t Compiler :: mapExtension(BuildTreeWriter& writer, Scope& scope, mssg_t& message, ref_t implicitSignatureRef, ObjectInfo object)
{
   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   ref_t objectRef = resolveObjectReference(scope, object, true);

   // check extensions
   auto it = nsScope->extensions.getIt(message);
   if (!it.eof()) {
      // generate an extension signature
      ref_t            signatures[ARG_COUNT] = {};
      size_t           signatureLen = scope.module->resolveSignature(implicitSignatureRef, signatures);
      for (size_t i = signatureLen; i > 0; i--)
         signatures[i] = signatures[i - 1];

      signatures[0] = objectRef;
      signatureLen++;

      size_t argCount = getArgCount(message);
      while (signatureLen < argCount) {
         signatures[signatureLen] = scope.moduleScope->buildins.superReference;
         signatureLen++;
      }

      scope.module->mapSignature(signatures, signatureLen, false);
      mssg_t resolvedMessage = 0;
      ref_t resolvedExtRef = 0;
      int counter = 0;
      while (!it.eof()) {
         auto extInfo = *it;
         ref_t targetRef = nsScope->resolveExtensionTarget(extInfo.value1);
         if (_logic->isMessageCompatibleWithSignature(*scope.moduleScope, extInfo.value2,
            signatures, signatureLen))
         {
            if (!resolvedMessage) {
               resolvedMessage = extInfo.value2;
               resolvedExtRef = extInfo.value1;
            }
            else if (!_logic->isSignatureCompatible(*scope.moduleScope, extInfo.value2, resolvedMessage)) {
               resolvedMessage = 0;
               break;
            }
         }
         counter++;

         it = nsScope->extensions.nextIt(message, it);
      }

      if (resolvedMessage) {
         if (counter > 1 && implicitSignatureRef == 0) {
            // HOTFIX : does not resolve an ambigous extension for a weak message
         }
         else {
            // if we are lucky - use the resolved one
            message = resolvedMessage;

            return resolvedExtRef;
         }
      }

      // bad luck - we have to generate run-time extension dispatcher
      ref_t extRef = nsScope->extensionDispatchers.get(message);
      if (extRef == INVALID_REF) {
         extRef = compileExtensionDispatcher(writer, *nsScope, message, 0);

         nsScope->extensionDispatchers.add(message, extRef);
      }

      message |= FUNCTION_MESSAGE;

      return extRef;
   }

   return 0;
}

mssg_t Compiler :: resolveMessageAtCompileTime(BuildTreeWriter& writer, ObjectInfo target, ExprScope& scope, mssg_t weakMessage,
   ref_t implicitSignatureRef, bool ignoreExtensions, ref_t& resolvedExtensionRef)
{
   mssg_t resolvedMessage = 0;
   ref_t targetRef = resolveObjectReference(scope, target, true);

   if (!ignoreExtensions) {
      // check the existing extensions if allowed
      resolvedMessage = weakMessage;

      // if the object handles the weak message - do not use extensions
      CheckMethodResult dummy = {};
      if(_logic->resolveCallType(*scope.moduleScope, targetRef, weakMessage, dummy)) {
         return weakMessage;
      }

      ref_t extensionRef = mapExtension(writer, scope, resolvedMessage, implicitSignatureRef, target);
      if (extensionRef != 0) {
         // if there is an extension to handle the compile-time resolved message - use it
         resolvedExtensionRef = extensionRef;

         return resolvedMessage;
      }
   }

   // otherwise - use the weak message
   return weakMessage;
}

inline bool isSelfCall(ObjectInfo target)
{
   switch (target.kind) {
      case ObjectKind::SelfLocal:
      //case okOuterSelf:
      //case okClassSelf:
      //case okInternalSelf:
         return true;
      default:
         return false;
   }
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo target,
   mssg_t weakMessage, ref_t implicitSignatureRef, ArgumentsInfo& arguments, ExpressionAttributes mode)
{
   ObjectInfo retVal(ObjectKind::Object);

   BuildKey operation = BuildKey::CallOp;
   ref_t resolvedExtensionRef = 0;
   mssg_t message = resolveMessageAtCompileTime(writer, target, scope, weakMessage,
      implicitSignatureRef,
      EAttrs::testAndExclude(mode.attrs, EAttr::NoExtension), 
      resolvedExtensionRef);

   if (resolvedExtensionRef) {
      // if extension was found - make it a operation target
      target = { ObjectKind::ConstantRole, resolvedExtensionRef, resolvedExtensionRef };
   }

   ref_t targetRef = resolveObjectReference(scope, target);

   CheckMethodResult result = {};
   bool found = _logic->resolveCallType(*scope.moduleScope, targetRef, message, result);
   if (found) {
      switch (result.visibility) {
         case Visibility::Private:
         case Visibility::Protected:
            if (isSelfCall(target)) {
               message = result.message;
            }
            else found = false;
            break;
         default:
            break;
      }
   }

   if (found) {
      retVal.type = result.outputRef;
      switch ((MethodHint)result.kind) {
         case MethodHint::Sealed:
            if (result.constRef && _optMode) {
               NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

               retVal = nsScope->defineObjectInfo(result.constRef, EAttr::None, true);

               operation = BuildKey::None;
            }
            else operation = BuildKey::DirectCallOp;
            break;
         default:
            break;
      }
   }
   else if (targetRef) {
      if (EAttrs::test(mode.attrs, EAttr::StrongResolved)) {
         if (getAction(message) == getAction(scope.moduleScope->buildins.constructor_message)) {
            scope.raiseError(errUnknownDefConstructor, node);
         }
         else scope.raiseError(errUnknownMessage, node.findChild(SyntaxKey::Message));
      }
      else {
         // treat it as a weak reference
         targetRef = 0;

         SyntaxNode messageNode = node.findChild(SyntaxKey::Message);
         if (messageNode == SyntaxKey::None) {
            scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownFunction, node);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, messageNode);
      }
   }

   if (operation != BuildKey::None) {
      bool targetOverridden = (target != arguments[0]);
      if (targetOverridden) {
         target = boxArgument(writer, scope, target, false, false);
      }

      pos_t counter = arguments.count_pos();
      // box the arguments if required
      for (unsigned int i = counter; i > 0; i--) {
         ObjectInfo arg = boxArgument(writer, scope, arguments[i - 1], false, false);

         arguments[i - 1] = arg;
      }

      for (unsigned int i = counter; i > 0; i--) {
         ObjectInfo arg = arguments[i - 1];

         writeObjectInfo(writer, scope, arg);
         writer.appendNode(BuildKey::SavingInStack, i - 1);
      }

      if (!targetOverridden) {
         writer.appendNode(BuildKey::Argument, 0);
      }
      else writeObjectInfo(writer, scope, target);

      writer.newNode(operation, message);

      if (targetRef)
         writer.appendNode(BuildKey::Type, targetRef);

      writer.closeNode();
   }

   scope.reserveArgs(arguments.count_pos());

   return retVal;
}

void Compiler :: addBreakpoint(BuildTreeWriter& writer, SyntaxNode node, BuildKey bpKey)
{
   SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);
   SyntaxNode row = terminal.findChild(SyntaxKey::Row);
   SyntaxNode col = terminal.findChild(SyntaxKey::Column);
   if (row != SyntaxKey::None) {
      writer.newNode(bpKey);
      writer.appendNode(BuildKey::Row, row.arg.value);
      writer.appendNode(BuildKey::Column, col.arg.value);
      writer.closeNode();
   }
}

ObjectInfo Compiler :: compileNewOp(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source,
   ref_t signRef, ArgumentsInfo& arguments)
{
   mssg_t messageRef = overwriteArgCount(scope.moduleScope->buildins.constructor_message, arguments.count_pos());
   ObjectInfo retVal = compileMessageOperation(
      writer, scope, node, source, messageRef, signRef, arguments, EAttr::StrongResolved | EAttr::NoExtension);

   return retVal;
}

ObjectInfo Compiler :: compilePropertyOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute attrs)
{
   ObjectInfo retVal = { };
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::Object) {
      addBreakpoint(writer, current, BuildKey::Breakpoint);
   }

   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter);
   switch (source.kind) {
      case ObjectKind::External:
      case ObjectKind::Creating:
      case ObjectKind::Casting:
         scope.raiseError(errInvalidOperation, node);
         break;
      default:
         arguments.add(source);
         break;
   }

   current = current.nextNode();
   mssg_t messageRef = mapMessage(scope, current, true, 
      source.kind == ObjectKind::Extension);

   retVal = compileMessageOperation(writer, scope, node, source, messageRef,
      0, arguments, EAttr::None);

   return retVal;
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute attrs)
{
   ObjectInfo retVal = { };
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter);
   switch (source.kind) {
      case ObjectKind::External:
         compileMessageArguments(writer, scope, current, arguments);

         retVal = compileExternalOp(writer, scope, source.reference, source.extra == -1, arguments);
         break;
      case ObjectKind::Creating:
      {
         ref_t signRef = compileMessageArguments(writer, scope, current, arguments);

         retVal = compileNewOp(writer, scope, node, mapClassSymbol(scope, source.type), 
            signRef, arguments);
         break;
      }
      case ObjectKind::Casting:
         compileMessageArguments(writer, scope, current, arguments);
         if (arguments.count() == 1) {
            retVal = convertObject(writer, scope, current, arguments[0], source.type);
         }
         else scope.raiseError(errInvalidOperation, node);
         break;
      default:
      {
         current = current.nextNode();
         mssg_t messageRef = mapMessage(scope, current, false,
            source.kind == ObjectKind::Extension);

         if (!test(messageRef, FUNCTION_MESSAGE))
            arguments.add(source);

         ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments);

         retVal = compileMessageOperation(writer, scope, node, source, messageRef,
            implicitSignatureRef, arguments, EAttr::None);
         break;
      }
   }

   return retVal;
}

ObjectInfo Compiler :: compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand)
{
   BuildKey operationType = BuildKey::None;
   int operand = 0;

   ObjectInfo target = mapObject(scope, loperand, EAttr::None);
   int size = 0;
   bool stackSafe = false;
   bool fieldMode = false;
   switch (target.kind) {
      case ObjectKind::Local:
         scope.markAsAssigned(target);
         operationType = BuildKey::Assigning;
         operand = target.reference;
         break;
      case ObjectKind::LocalAddress:
         scope.markAsAssigned(target);
         operationType = BuildKey::Copying;
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.type).size;
         stackSafe = true;
         break;
      case ObjectKind::Field:
         scope.markAsAssigned(target);
         operationType = BuildKey::FieldAssigning;
         operand = target.reference;
         fieldMode = true;
         break;
      default:
         scope.raiseError(errInvalidOperation, loperand.parentNode());
         break;
   }

   ObjectInfo exprVal;
   exprVal = compileExpression(writer, scope, roperand,
      resolveObjectReference(scope, target), EAttr::Parameter);

   writeObjectInfo(writer, scope,
      boxArgument(writer, scope, exprVal, stackSafe, true));

   if (fieldMode) {
      writer.appendNode(BuildKey::Argument, 0);
      writeObjectInfo(writer, scope, scope.mapSelf());
   }

   writer.newNode(operationType, operand);
   if (size != 0) {
      writer.appendNode(BuildKey::Size, size);
   }
   writer.closeNode();

   writeObjectInfo(writer, scope, target);

   return target;
}

ObjectInfo Compiler :: compileIndexerOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId)
{
   // HOTFIX : recognize fixed-array declaration
   SyntaxNode loperand = node.firstChild();
   if (loperand == SyntaxKey::Object) {
      ObjectInfo info = mapObject(scope, loperand, EAttr::Lookahead);
      if (info.kind == ObjectKind::Declaring) {
         // if it is a new variable declaration - treat it like a new array
         declareVariable(scope, node, info.type); // !! temporal - typeref should be provided or super class

         return {}; // !! temporally
      }
   }

   return compileOperation(writer, scope, node, operatorId);
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId)
{
   SyntaxNode loperand = node.firstChild();
   SyntaxNode roperand = loperand.nextNode();

   if (operatorId == SET_OPERATOR_ID){
      // assign operation is a special case
      if (loperand == SyntaxKey::IndexerOperation) {
         return compileOperation(writer, scope, loperand, roperand, SET_INDEXER_OPERATOR_ID);
      }
      else return compileAssigning(writer, scope, loperand, roperand);

   }
   else return compileOperation(writer, scope, loperand, roperand, operatorId);
}

inline SyntaxNode skipNestedExpression(SyntaxNode node)
{
   if (node == SyntaxKey::Expression) {
      SyntaxNode current = node.firstChild();
      while (current == SyntaxKey::Expression) {
         node = current;
         current = current.firstChild();
      }

      return node;
   }
   return node;
}

ObjectInfo Compiler :: compileSubCode(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   scope.syncStack();

   CodeScope* parentCodeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

   CodeScope codeScope(parentCodeScope);
   compileCode(writer, codeScope, node, false);
   codeScope.syncStack(parentCodeScope);

   return { ObjectKind::Object };
}

ObjectInfo Compiler :: compileBranchingOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId)
{
   ObjectInfo retVal = {};

   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = skipNestedExpression(lnode.nextNode());
   SyntaxNode r2node = {};
   if (operatorId == IF_ELSE_OPERATOR_ID)
      r2node = rnode.nextNode();

   if (rnode.existChild(SyntaxKey::ClosureBlock))
      rnode = rnode.findChild(SyntaxKey::ClosureBlock);
   if (r2node.existChild(SyntaxKey::ClosureBlock))
      r2node = r2node.findChild(SyntaxKey::ClosureBlock);

   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter);
   ObjectInfo roperand = { ObjectKind::Closure, V_CLOSURE, 0 };
   ObjectInfo roperand2 = {};

   BuildKey   op = BuildKey::None;

   size_t     argLen = 2;
   ref_t      arguments[3];
   arguments[0] = resolveObjectReference(scope, loperand, false);
   arguments[1] = resolveObjectReference(scope, roperand, false);
   if (r2node != SyntaxKey::None) {
      roperand2 = { ObjectKind::Closure, V_CLOSURE, 0 };

      argLen++;
      arguments[2] = resolveObjectReference(scope, roperand2, false);
   }

   ref_t outputRef = 0;
   bool  needToAlloc = false;
   op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef, needToAlloc);

   if (op != BuildKey::None) {
      writeObjectInfo(writer, scope, loperand);

      writer.newNode(op, operatorId);
      writer.appendNode(BuildKey::Const, scope.moduleScope->branchingInfo.trueRef);
      writer.newNode(BuildKey::Tape);
      compileSubCode(writer, scope, rnode.firstChild(), EAttr::None);
      writer.closeNode();
      if (r2node != SyntaxKey::None) {
         writer.newNode(BuildKey::Tape);
         compileSubCode(writer, scope, r2node.firstChild(), EAttr::None);
         writer.closeNode();
      }

      writer.closeNode();
   }
   else {
      mssg_t message = resolveOperatorMessage(scope.moduleScope, operatorId);

      roperand = compileClosure(writer, scope, rnode, EAttr::None);

      ArgumentsInfo messageArguments;
      messageArguments.add(loperand);
      messageArguments.add(roperand);
      if (r2node != SyntaxKey::None) {
         messageArguments.add(roperand2);
      }

      ref_t signRef = scope.module->mapSignature(arguments, argLen, false);

      retVal = compileMessageOperation(writer, scope, node, loperand, message, signRef, messageArguments, 
         EAttr::NoExtension);
   }

   return retVal;
}

ObjectInfo Compiler :: mapStringConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::StringLiteral, V_STRING, scope.module->mapConstant(node.identifier()) };
}

ObjectInfo Compiler :: mapCharacterConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::CharacterLiteral, V_WORD32, scope.module->mapConstant(node.identifier()) };
}

inline ref_t mapIntConstant(Compiler::Scope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendInt(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

inline ref_t mapUIntConstant(Compiler::Scope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendUInt(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

ObjectInfo Compiler :: mapIntConstant(Scope& scope, SyntaxNode node, int radix)
{
   int integer = StrConvertor::toInt(node.identifier(), radix);
   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return ObjectInfo(ObjectKind::IntLiteral, V_INT32, ::mapIntConstant(scope, integer), integer);
}

ObjectInfo Compiler :: mapUIntConstant(Scope& scope, SyntaxNode node, int radix)
{
   int integer = StrConvertor::toUInt(node.identifier(), radix);
   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return ObjectInfo(ObjectKind::IntLiteral, V_INT32, ::mapUIntConstant(scope, integer), integer);
}

ObjectInfo Compiler :: mapTerminal(Scope& scope, SyntaxNode node, ref_t declaredRef, EAttr attrs)
{
   bool forwardMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Forward);
   bool variableMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewVariable);
   bool externalOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::Extern);
   bool newOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewOp);
   bool castOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::CastOp);

   ObjectInfo retVal;
   bool invalid = false;
   if (externalOp) {
      auto externalInfo = mapExternal(scope, node);
      switch (externalInfo.type) {
         case ExternalType::WinApi:
            return { ObjectKind::External, 0, externalInfo.reference, -1 };
         default:
            return { ObjectKind::External, 0, externalInfo.reference, 0 };
      }
   }
   else if (newOp || castOp) {
      switch (node.key) {
         case SyntaxKey::identifier:
         case SyntaxKey::reference:
            retVal = { newOp ? ObjectKind::Creating : ObjectKind::Casting,
               resolveTypeAttribute(scope, node, false), 0, 0 };
            break;
         default:
            invalid = true;
            break;
      }
   }
   else {
      switch (node.key) {
         case SyntaxKey::identifier:
         case SyntaxKey::reference:
            if (variableMode) {
               invalid = forwardMode;

               declareVariable(scope, node, declaredRef);
               retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, attrs | ExpressionAttribute::Local);
            }
            else if (forwardMode) {
               IdentifierString forwardName(FORWARD_PREFIX_NS, node.identifier());

               retVal = scope.mapIdentifier(*forwardName, true, attrs);
            }
            else retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, attrs);
            break;
         case SyntaxKey::string:
            invalid = forwardMode || variableMode;

            retVal = mapStringConstant(scope, node);
            break;
         case SyntaxKey::character:
            invalid = forwardMode || variableMode;

            retVal = mapCharacterConstant(scope, node);
            break;
         case SyntaxKey::integer:
            invalid = forwardMode || variableMode;

            retVal = mapIntConstant(scope, node, 10);
            break;
         case SyntaxKey::hexinteger:
            invalid = forwardMode || variableMode;

            retVal = mapUIntConstant(scope, node, 16);
            break;
         default:
            // to make compiler happy
            invalid = true;
            break;
      }
   }

   if (invalid)
      scope.raiseError(errInvalidOperation, node);

   return retVal;
}

ObjectInfo Compiler :: mapObject(Scope& scope, SyntaxNode node, EAttrs mode)
{
   SyntaxNode terminalNode = node == SyntaxKey::identifier ? node : node.lastChild(SyntaxKey::TerminalMask);

   ref_t declaredRef = 0;
   declareExpressionAttributes(scope, node, declaredRef, mode);
   if (mode.test(EAttr::Lookahead)) {
      if (mode.test(EAttr::NewVariable)) {
         return { ObjectKind::Declaring, declaredRef, 0 };
      }
      else return {};
   }

   if (terminalNode.nextNode() == SyntaxKey::TemplateArg && !EAttrs::test(mode.attrs, ExpressionAttribute::NewOp)) {
      scope.raiseError(errInvalidSyntax, node);
   }

   ObjectInfo retVal = mapTerminal(scope, terminalNode, declaredRef, mode.attrs);

   return retVal;
}

ObjectInfo Compiler :: declareTempLocal(ExprScope& scope, ref_t typeRef)
{
   int tempLocal = scope.newTempLocal();
   return { ObjectKind::TempLocal, typeRef, (ref_t)tempLocal };
}

ObjectInfo Compiler :: saveToTempLocal(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo object)
{
   if (object.kind == ObjectKind::External) {
      CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, object.type);
      int tempLocal = allocateLocalAddress(codeScope, sizeInfo.size, false);

      writer.appendNode(BuildKey::SavingIndex, tempLocal);

      return { ObjectKind::TempLocalAddress, object.type, (ref_t)tempLocal };
   }
   else {
      int tempLocal = scope.newTempLocal();
      writeObjectInfo(writer, scope, object);
      writer.appendNode(BuildKey::Assigning, tempLocal);

      return { ObjectKind::TempLocal, object.type, (ref_t)tempLocal };
   }
}

ObjectInfo Compiler :: compileObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   if (node == SyntaxKey::Object) {
      bool paramMode = EAttrs::testAndExclude(mode, EAttr::Parameter);

      ObjectInfo retVal = mapObject(scope, node, mode);
      switch (retVal.kind) {
         case ObjectKind::External:
            return retVal;
         case ObjectKind::Unknown:
            scope.raiseError(errUnknownObject, node.lastChild(SyntaxKey::TerminalMask));
            break;
         default:
            break;
      }

      if (paramMode && hasToBePresaved(retVal)) {
         retVal = saveToTempLocal(writer, scope, retVal);
      }

      return retVal;
   }
   else return compileExpression(writer, scope, node, 0, mode);
}

ObjectInfo Compiler :: typecastObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef)
{
   if (targetRef == scope.moduleScope->buildins.superReference)
      return source;

   ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
   ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

   mssg_t typecastMssg = encodeMessage(actionRef, 1, CONVERSION_MESSAGE);

   ArgumentsInfo arguments;
   arguments.add(source);

   ObjectInfo retVal = compileMessageOperation(writer, scope, node, source, typecastMssg, 
      0, arguments, EAttr::None);
   // NOTE : typecasting message is guaranteed to return the instance of the target type
   retVal.type = targetRef;

   return retVal;
}

ObjectInfo Compiler :: convertObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source,
   ref_t targetRef)
{
   ref_t sourceRef = resolveObjectReference(scope, source, false);
   if (!_logic->isCompatible(*scope.moduleScope, targetRef, sourceRef, false)) {
      auto conversionRoutine = _logic->retrieveConversionRoutine(*scope.moduleScope, targetRef, sourceRef);
      if (conversionRoutine.result == ConversionResult::BoxingRequired) {
         // if it is implcitily compatible
         switch (source.kind) {
            case ObjectKind::TempLocalAddress:
               source.type = targetRef;
               break;
            default:
               throw InternalError(errFatalError);
         }
      }
      else source = typecastObject(writer, scope, node, source, targetRef);
   }

   return source;
}

ObjectInfo Compiler :: compileNestedExpression(InlineClassScope& scope, EAttr mode)
{
   ref_t nestedRef = scope.reference;

   if (test(scope.info.header.flags, elStateless)) {
      ObjectInfo retVal = { ObjectKind::Singleton, nestedRef, nestedRef };

      return retVal;
   }
   else {
      throw InternalError(errFatalError); // !! temporal
   }
}

ref_t Compiler :: mapNested(ExprScope& ownerScope, ExpressionAttribute mode)
{
   ref_t nestedRef = 0;
   if (EAttrs::testAndExclude(mode, EAttr::RootSymbol)) {
      SymbolScope* owner = Scope::getScope<SymbolScope>(ownerScope, Scope::ScopeLevel::Symbol);
      if (owner)
         nestedRef = owner->reference;
   }
   else if (EAttrs::testAndExclude(mode, EAttr::Root)) {
      MethodScope* ownerMeth = Scope::getScope<MethodScope>(ownerScope, Scope::ScopeLevel::Method);
      if (ownerMeth && ownerMeth->checkHint(MethodHint::Constant)) {
         ref_t dummyRef = 0;
         // HOTFIX : recognize property constant

         IdentifierString name(ownerScope.module->resolveReference(ownerScope.getClassRef()));
         if ((*name).endsWith(CLASSCLASS_POSTFIX))
            name.truncate(name.length() - getlength(CLASSCLASS_POSTFIX));

         name.append('#');
         name.append(ownerScope.module->resolveAction(getAction(ownerMeth->message), dummyRef));

         nestedRef = ownerMeth->module->mapReference(*name);
      }
   }

   if (!nestedRef)
      nestedRef = ownerScope.moduleScope->mapAnonymous();

   return nestedRef;
}

ObjectInfo Compiler :: compileClosure(BuildTreeWriter& writer, ExprScope& ownerScope, SyntaxNode node, ExpressionAttribute mode)
{
   ref_t nestedRef = mapNested(ownerScope, mode);
   InlineClassScope scope(&ownerScope, nestedRef);

   compileClosureClass(writer, scope, node);

   return compileNestedExpression(scope, mode);
}

ObjectInfo Compiler :: compileNested(BuildTreeWriter& writer, ExprScope& ownerScope, SyntaxNode node, ExpressionAttribute mode)
{
   ref_t parentRef = ownerScope.moduleScope->buildins.superReference;
   EAttrs nestedMode = {};
   declareExpressionAttributes(ownerScope, node, parentRef, nestedMode);

   // allow only new and type attrobutes
   if (nestedMode.attrs != EAttr::None && nestedMode.attrs != EAttr::NewOp && nestedMode.attrs != EAttr::NewVariable)
      ownerScope.raiseError(errInvalidOperation, node);

   ref_t nestedRef = mapNested(ownerScope, mode);
   InlineClassScope scope(&ownerScope, nestedRef);

   compileNestedClass(writer, scope, node, parentRef);

   return compileNestedExpression(scope, mode);
}

ObjectInfo Compiler :: compileLoopExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ExpressionAttribute mode)
{
   ObjectInfo retVal = { ObjectKind::Object };

   writer.newNode(BuildKey::LoopOp);

   compileExpression(writer, scope, node, 0, mode);

   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ref_t targetRef, ExpressionAttribute mode)
{
   bool paramMode = EAttrs::testAndExclude(mode, EAttr::Parameter);

   ObjectInfo retVal;

   SyntaxNode current = node == SyntaxKey::Expression ? node.firstChild() : node;
   switch (current.key) {
      case SyntaxKey::MessageOperation:
         retVal = compileMessageOperation(writer, scope, current, mode);
         break;
      case SyntaxKey::PropertyOperation:
         retVal = compilePropertyOperation(writer, scope, current, mode);
         break;
      case SyntaxKey::AssignOperation:
      //case SyntaxKey::AddAssignOperation:
      case SyntaxKey::AddOperation:
      case SyntaxKey::SubOperation:
      case SyntaxKey::LenOperation:
      case SyntaxKey::LessOperation:
      case SyntaxKey::NameOperation:
      case SyntaxKey::EqualOperation:
      case SyntaxKey::NotOperation:
      case SyntaxKey::NotEqualOperation:
         retVal = compileOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::IndexerOperation:
         retVal = compileIndexerOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::IfOperation:
      case SyntaxKey::IfNotOperation:
      case SyntaxKey::IfElseOperation:
         retVal = compileBranchingOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::LoopOperation:
         retVal = compileLoopExpression(writer, scope, current.firstChild(), mode);
         break;
      case SyntaxKey::ReturnExpression:
         retVal = compileExpression(writer, scope, current.firstChild(), 0, mode);
         break;
      case SyntaxKey::Expression:
         retVal = compileExpression(writer, scope, current, 0, mode);
         break;
      case SyntaxKey::Object:
         retVal = compileObject(writer, scope, current, mode);
         break;
      case SyntaxKey::NestedBlock:
         retVal = compileNested(writer, scope, current, mode);
         break;
      case SyntaxKey::ClosureBlock:
         retVal = compileClosure(writer, scope, current, mode);
         break;
      case SyntaxKey::CodeBlock:
         compileSubCode(writer, scope, current, mode);
         break;
      case SyntaxKey::None:
         assert(false);
         break;
      default:
         retVal = compileObject(writer, scope, node, mode);
         break;
   }

   if ((paramMode || targetRef) && hasToBePresaved(retVal)) {
      retVal = saveToTempLocal(writer, scope, retVal);
   }
   if (targetRef) {
      retVal = convertObject(writer, scope, node, retVal, targetRef);
      if (paramMode && hasToBePresaved(retVal))
         retVal = saveToTempLocal(writer, scope, retVal);
   }

   return retVal;
}

inline SyntaxNode findObjectNode(SyntaxNode node)
{
   if (node == SyntaxKey::CodeBlock) {
      // HOTFIX : to prevent double breakpoint
      return {};
   }
   if (node != SyntaxKey::None && node != SyntaxKey::Object) {
      return findObjectNode(node.firstChild());
   }
   else return node;
}

ObjectInfo Compiler :: compileRetExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);

   ref_t outputRef = codeScope.getOutputRef();

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(node), BuildKey::Breakpoint);

   ObjectInfo retVal = boxArgument(writer, scope,
      compileExpression(writer, scope, node.findChild(SyntaxKey::Expression), outputRef, EAttr::Root),
      false, true);

   if (!hasToBePresaved(retVal)) {
      writeObjectInfo(writer, scope, retVal);
   }
   writer.appendNode(BuildKey::EndStatement);

   writer.appendNode(BuildKey::goingToEOP);

   scope.syncStack();

   return retVal;
}

ObjectInfo Compiler :: compileRootExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);

   ExpressionAttribute mode = EAttr::None;

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(node), BuildKey::Breakpoint);

   auto retVal = compileExpression(writer, scope, node, 0, mode);
   writer.appendNode(BuildKey::EndStatement);

   scope.syncStack();

   return retVal;
}

void Compiler :: saveFrameAttributes(BuildTreeWriter& writer, Scope& scope, pos_t reserved, pos_t reservedN)
{
   reserved = align(reserved, scope.moduleScope->stackAlingment);
   reservedN = align(reservedN, scope.moduleScope->rawStackAlingment);

   if (reserved)
      writer.appendNode(BuildKey::Reserved, reserved);

   if (reservedN > 0)
      writer.appendNode(BuildKey::ReservedN, reservedN);
}

bool Compiler :: compileSymbolConstant(SymbolScope& scope, ObjectInfo retVal)
{
   ref_t constRef = generateConstant(scope, retVal, scope.reference);
   if (constRef) {
      switch (retVal.kind) {
         case ObjectKind::Singleton:
            scope.info.symbolType = SymbolType::Singleton;
            scope.info.valueRef = retVal.reference;
            scope.info.typeRef = retVal.type;
            break;
         case ObjectKind::StringLiteral:
            scope.info.symbolType = SymbolType::Constant;
            scope.info.valueRef = retVal.reference;
            scope.info.typeRef = retVal.type;
            break;
         default:
            assert(false);
            break;
      }

      return true;
   }
   else return false;
}

void Compiler :: compileSymbol(BuildTreeWriter& writer, SymbolScope& scope, SyntaxNode node)
{
   scope.load();

   writer.newNode(BuildKey::Symbol, node.arg.reference);

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   writer.appendNode(BuildKey::Path, *ns->sourcePath);

   writer.newNode(BuildKey::Tape);
   if (scope.isStatic)
      writer.appendNode(BuildKey::OpenStatic, node.arg.reference);

   writer.appendNode(BuildKey::OpenFrame);

   SyntaxNode bodyNode = node.findChild(SyntaxKey::GetExpression);

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(bodyNode), BuildKey::Breakpoint);

   ExprScope exprScope(&scope);
   ObjectInfo retVal = compileExpression(writer, exprScope,
      bodyNode.firstChild(), 0, ExpressionAttribute::RootSymbol);

   writeObjectInfo(writer, exprScope,
      boxArgument(writer, exprScope, retVal, false, true));

   writer.appendNode(BuildKey::EndStatement);

   exprScope.syncStack();

   writer.appendNode(BuildKey::CloseFrame);

   if (scope.isStatic)
      writer.appendNode(BuildKey::CloseStatic, node.arg.reference);

   writer.appendNode(BuildKey::Exit);

   writer.closeNode();
   saveFrameAttributes(writer, scope, scope.reserved1 + scope.reservedArgs, scope.reserved2);
   writer.closeNode();

   // create constant if required
   if (scope.info.symbolType == SymbolType::Constant) {
      if (!compileSymbolConstant(scope, retVal))
         scope.raiseError(errInvalidOperation, node);
   }

   scope.save();
}

void Compiler :: compileClassSymbol(BuildTreeWriter& writer, ClassScope& scope)
{
   writer.newNode(BuildKey::Symbol, scope.reference);

   writer.newNode(BuildKey::Tape);
   writer.appendNode(BuildKey::OpenFrame);
   ObjectInfo retVal(ObjectKind::Class, scope.info.header.classRef, scope.reference, 0);
   ExprScope exprScope(&scope);
   writeObjectInfo(writer, exprScope, retVal);
   writer.appendNode(BuildKey::CloseFrame);
   writer.appendNode(BuildKey::Exit);
   writer.closeNode();

   writer.closeNode();
}

void Compiler :: beginMethod(BuildTreeWriter& writer, MethodScope& scope, BuildKey scopeKey)
{
   writer.newNode(scopeKey, scope.message);
   writer.newNode(BuildKey::Tape);
}

void Compiler :: endMethod(BuildTreeWriter& writer, MethodScope& scope)
{
   writer.appendNode(BuildKey::Exit);
   writer.closeNode();

   saveFrameAttributes(writer, scope, scope.reserved1 + scope.reservedArgs, scope.reserved2);

   writer.closeNode();
}

void Compiler :: injectVariableInfo(BuildNode node, CodeScope& codeScope)
{
   for (auto it = codeScope.locals.start(); !it.eof(); ++it) {
      auto localInfo = *it;
      if (localInfo.class_ref && _logic->isEmbeddableArray(*codeScope.moduleScope, localInfo.class_ref)) {
         node.appendChild(BuildKey::BinaryArray, localInfo.offset)
            .appendChild(BuildKey::Size, localInfo.size);
      }
   }
}

ObjectInfo Compiler :: compileCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, bool closureMode)
{
   ObjectInfo retVal = {};
   ObjectInfo exprRetVal = {};

   // variable declaration node
   writer.newNode(BuildKey::VariableInfo);
   BuildNode variableNode = writer.CurrentNode();
   writer.closeNode();

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Expression:
            exprRetVal = compileRootExpression(writer, codeScope, current);
            break;
         case SyntaxKey::ReturnExpression:
            exprRetVal = retVal = compileRetExpression(writer, codeScope, current);
            break;
         case SyntaxKey::CodeBlock:
         {
            CodeScope subScope(&codeScope);
            exprRetVal = compileCode(writer, subScope, current, false);
            subScope.syncStack(&codeScope);
            break;
         }
         case SyntaxKey::EOP:
            addBreakpoint(writer, current, BuildKey::EOPBreakpoint);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   injectVariableInfo(variableNode, codeScope);

   // NOTE : in the closure mode the last statement is the closure result
   return closureMode ? exprRetVal : retVal;
}

void Compiler :: compileMethodCode(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope,
   SyntaxNode node, bool newFrame)
{
   if (!newFrame) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }

   // stack should contains current self reference
   // the original message should be restored if it is a generic method
   scope.selfLocal = codeScope.newLocal();
   writer.appendNode(BuildKey::Assigning, scope.selfLocal);

   ObjectInfo retVal = { };

   SyntaxNode bodyNode = node.firstChild(SyntaxKey::ScopeMask);
   switch (bodyNode.key) {
      case SyntaxKey::CodeBlock:
         retVal = compileCode(writer, codeScope, bodyNode, scope.closureMode);
         break;
      case SyntaxKey::ReturnExpression:
         retVal = compileRetExpression(writer, codeScope, bodyNode);
         break;
      default:
         break;
   }

   // if the method returns itself
   if (retVal.kind == ObjectKind::Unknown) {
      ExprScope exprScope(&codeScope);

      retVal = scope.mapSelf();
      ref_t outputRef = scope.info.outputRef;
      if (outputRef) {
         typecastObject(writer, exprScope, node, retVal, outputRef);

         exprScope.syncStack();
      }

      writeObjectInfo(writer, exprScope,
         boxArgument(writer, exprScope, retVal, false, true));
   }

   writer.appendNode(BuildKey::CloseFrame);

   if (scope.checkHint(MethodHint::Constant)) {
      ref_t constRef = generateConstant(scope, retVal, 0);
      if (constRef) {
         ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

         classScope->addRefAttribute(scope.message, ClassAttribute::ConstantMethod, constRef);

         classScope->save();
      }
      else scope.raiseError(errInvalidConstAttr, node);
   }
}

void Compiler :: compileInitializerMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode classNode)
{
   beginMethod(writer, scope, BuildKey::Method);

   CodeScope codeScope(&scope);

   // new stack frame
   writer.appendNode(BuildKey::OpenFrame);

   // stack should contains current self reference
   // the original message should be restored if it is a generic method
   scope.selfLocal = codeScope.newLocal();
   writer.appendNode(BuildKey::Assigning, scope.selfLocal);

   SyntaxNode current = classNode.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::AssignOperation) {
         compileRootExpression(writer, codeScope, current);
      }
      current = current.nextNode();
   }

   codeScope.syncStack(&scope);

   writer.appendNode(BuildKey::CloseFrame);

   endMethod(writer, scope);
}

void Compiler :: compileAbstractMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, bool abstractMode)
{
   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   if (current.key == SyntaxKey::WithoutBody) {
      // NOTE : abstract method should not have a body
      if (!abstractMode)
         scope.raiseError(errNotAbstractClass, node);
   }
   else scope.raiseError(errAbstractMethodCode, node);

   writer.newNode(BuildKey::AbstractMethod, scope.message);
   writer.closeNode();
}

void Compiler :: compileMultidispatch(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   mssg_t message = scope.getMessageID();

   BuildKey op = BuildKey::DispatchingOp;
   ref_t    opRef = classScope->info.attributes.get({ message, ClassAttribute::OverloadList });
   if (!opRef)
      scope.raiseError(errIllegalOperation, node);

   if (test(classScope->info.header.flags, elSealed) || test(message, STATIC_MESSAGE)) {
      op = BuildKey::SealedDispatchingOp;
   }

   writer.newNode(op, opRef);
   writer.appendNode(BuildKey::Message, message);
   writer.closeNode();
   if (classScope->extensionDispatcher) {
      writer.appendNode(BuildKey::Argument, 0);

      writer.newNode(BuildKey::ResendOp);
      writer.closeNode();
   }
}

void Compiler :: compileDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   compileMultidispatch(writer, codeScope, node);
   // adding resend / redirect
}

void Compiler :: compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   beginMethod(writer, scope, BuildKey::Method);

   CodeScope codeScope(&scope);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   switch (current.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::ReturnExpression:
         compileMethodCode(writer, scope, codeScope, node, false);
         break;
      case SyntaxKey::Importing:
         writer.appendNode(BuildKey::Import, current.arg.reference);
         break;
      case SyntaxKey::WithoutBody:
         scope.raiseError(errNoBodyMethod, node);
         break;
      case SyntaxKey::ResendDispatch:
      case SyntaxKey::RedirectDispatch:
         compileDispatchCode(writer, codeScope, node);
         break;
      default:
         break;
   }

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

bool Compiler :: isDefaultOrConversionConstructor(Scope& scope, mssg_t message)
{
   ref_t actionRef = getAction(message);
   if (actionRef == getAction(scope.moduleScope->buildins.constructor_message)) {
      return true;
   }
   else return false;
}

void Compiler :: compileDefConvConstructorCode(BuildTreeWriter& writer, MethodScope& scope,
   SyntaxNode node, bool newFrame)
{
   if (!newFrame) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }

   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   if (test(classScope->info.header.flags, elDynamicRole))
      throw InternalError(errFatalError);

   createObject(writer, classScope->info, classScope->reference);

   // call field initilizers if available for default constructor
   if(classScope->info.methods.exist(scope.moduleScope->buildins.init_message)) {
      ExprScope exprScope(classScope);
      ArgumentsInfo args;

      compileMessageOperation(writer, exprScope, node, scope.mapSelf(), scope.moduleScope->buildins.init_message,
         0, args, EAttr::None);
   }

   writer.appendNode(BuildKey::CloseFrame);
}

void Compiler :: compileConstructor(BuildTreeWriter& writer, MethodScope& scope,
   ClassScope& classClassScope, SyntaxNode node)
{
   bool isDefConvConstructor = isDefaultOrConversionConstructor(scope, scope.message/*, isProtectedDefConst*/);

   mssg_t defConstrMssg = scope.moduleScope->buildins.constructor_message;

   beginMethod(writer, scope, BuildKey::Method);

   CodeScope codeScope(&scope);
   ref_t classFlags = codeScope.getClassFlags();

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);

   bool newFrame = false;
   if (isDefConvConstructor) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }
   else {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;

      writer.appendNode(BuildKey::ClassReference, scope.getClassRef());
      writer.newNode(BuildKey::CallOp, defConstrMssg);
      writer.appendNode(BuildKey::Index, 1); // built-in constructor entry should be the second entry in VMT
      writer.closeNode();
   }

   switch (current.key) {
      case SyntaxKey::CodeBlock:
         compileMethodCode(writer, scope, codeScope, node, newFrame);
         break;
      case SyntaxKey::None:
         if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
            // if it is a default / conversion (unnamed) constructor
            // it should create the object
            compileDefConvConstructorCode(writer, scope, node, newFrame);
            break;
         }
      default:
         throw InternalError(errFatalError);
   }

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileVMT(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, 
   bool exclusiveMode, bool ignoreAutoMultimethod)
{

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Method:
         {
            if (exclusiveMode 
               && (ignoreAutoMultimethod == SyntaxTree::ifChildExists(current, SyntaxKey::Autogenerated, -1)))
            {
               current = current.nextNode();
               continue;
            }

            MethodScope methodScope(&scope);
            methodScope.message = current.arg.reference;
            methodScope.info = scope.info.methods.get(methodScope.message);
            methodScope.functionMode = test(methodScope.message, FUNCTION_MESSAGE);

            declareVMTMessage(methodScope, current, false, false);

            if (methodScope.checkHint(MethodHint::Abstract)) {
               compileAbstractMethod(writer, methodScope, current, scope.abstractMode);
            }
            else if (methodScope.checkHint(MethodHint::Initializer)) {
               compileInitializerMethod(writer, methodScope, node);
            }
            else compileMethod(writer, methodScope, current);
            break;
         }
         case SyntaxKey::Constructor:
            if (_logic->isRole(scope.info)) {
               scope.raiseError(errIllegalConstructor, node);
            }
            break;
         case SyntaxKey::StaticMethod:
            if (_logic->isRole(scope.info)) {
               scope.raiseError(errIllegalStaticMethod, node);
            }
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: compileClassVMT(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::StaticMethod:
         {
            MethodScope methodScope(&classClassScope);
            methodScope.message = current.arg.reference;
            methodScope.info = classClassScope.info.methods.get(methodScope.message);

            compileMethod(writer, methodScope, current);
            break;
         }
         case SyntaxKey::Constructor:
         {
            MethodScope methodScope(&scope);
            methodScope.message = current.arg.reference;
            methodScope.info = classClassScope.info.methods.get(methodScope.message);

            compileConstructor(writer, methodScope, classClassScope, current);
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: compileClosureMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   beginMethod(writer, scope, BuildKey::Method);

   CodeScope codeScope(&scope);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   switch (current.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::ReturnExpression:
         compileMethodCode(writer, scope, codeScope, node, false);
         break;
      default:
         break;
   }

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileClosureClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   ref_t parentRef = scope.info.header.parentRef;

   writer.newNode(BuildKey::NestedClass, scope.reference);

   MethodScope methodScope(&scope);
   declareClosureMessage(methodScope, node);

   methodScope.functionMode = true;

   compileClosureMethod(writer, methodScope, node);

   declareClassParent(parentRef, scope, node);
   generateClassFlags(scope, elNestedClass);

   auto m_it = scope.info.methods.getIt(methodScope.message);
   if (!m_it.eof()) {
      (*m_it).inherited = true;
   }
   else scope.info.methods.add(methodScope.message, methodScope.info);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(scope.reference, scope.info, scope.isClassClass());

   writer.closeNode();

   scope.save();
}

void Compiler :: compileNestedClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, ref_t parentRef)
{
   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   scope.info.header.flags |= elNestedClass;
   declareClassParent(parentRef, scope, node);

   ref_t dummy = 0;
   declareClassAttributes(scope, {}, dummy);

   bool withConstructors = false;
   bool withDefaultConstructor = false;
   declareVMT(scope, node, withConstructors, withDefaultConstructor);
   if (withConstructors)
      scope.raiseError(errIllegalConstructor, node);

   generateClassDeclaration(scope, node, elNestedClass | elSealed);

   scope.save();

   writer.newNode(BuildKey::NestedClass, scope.reference);

   writer.appendNode(BuildKey::Path, *ns->sourcePath);

   compileVMT(writer, scope, node, true, true);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(scope.reference, scope.info, scope.isClassClass());

   // NOTE : compile once again only auto generated methods
   compileVMT(writer, scope, node, true, false);

   writer.closeNode();

   scope.save();
}

void Compiler :: compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   writer.newNode(BuildKey::Class, scope.reference);
   writer.appendNode(BuildKey::Path, *ns->sourcePath);

   compileVMT(writer, scope, node);
   writer.closeNode();

   // compile explicit symbol
   compileClassSymbol(writer, scope);
}

void Compiler :: compileClassClass(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope,
   SyntaxNode node)
{
   writer.newNode(BuildKey::Class, classClassScope.reference);
   compileClassVMT(writer, classClassScope, scope, node);
   writer.closeNode();
}

void Compiler :: compileNamespace(BuildTreeWriter& writer, NamespaceScope& ns, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::SourcePath:
            ns.sourcePath.copy(current.identifier());
            break;
         case SyntaxKey::Namespace:
         {
            NamespaceScope namespaceScope(&ns);
            declareNamespace(namespaceScope, current, false, false);

            compileNamespace(writer, namespaceScope, current);
            break;
         }
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, current.arg.reference, ns.defaultVisibility);
            symbolScope.isStatic = SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_STATIC);

            compileSymbol(writer, symbolScope, current);
            break;
         }
         case SyntaxKey::Class:
         {
            ClassScope classScope(&ns, current.arg.reference, ns.defaultVisibility);
            ns.moduleScope->loadClassInfo(classScope.info, current.arg.reference, false);
            classScope.abstractMode = test(classScope.info.header.flags, elAbstract);
            if (test(classScope.info.header.flags, elExtension))
               classScope.extensionClassRef = classScope.getAttribute(ClassAttribute::ExtensionRef);

            compileClass(writer, classScope, current);

            // compile class class if it available
            if (classScope.info.header.classRef != classScope.reference && classScope.info.header.classRef != 0) {
               ClassScope classClassScope(&ns, classScope.info.header.classRef, classScope.visibility);
               ns.moduleScope->loadClassInfo(classClassScope.info, classClassScope.reference, false);

               compileClassClass(writer, classClassScope, classScope, current);
            }
            break;
         }
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}

inline ref_t safeMapReference(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver, ustr_t forward)
{
   ustr_t resolved = forwardResolver->resolveForward(forward);
   if (!resolved.empty()) {
      if (moduleScope->isStandardOne()) {
         return moduleScope->module->mapReference(resolved + getlength(STANDARD_MODULE));
      }
      else return moduleScope->importReference(moduleScope->module, resolved);
   }
   else return 0;
}

bool Compiler :: reloadMetaDictionary(ModuleScopeBase* moduleScope, ustr_t name)
{
   if (name.compare(PREDEFINED_FORWARD)) {
      moduleScope->predefined.clear();

      auto predefinedInfo = moduleScope->getSection(PREDEFINED_FORWARD, mskMetaDictionaryRef, true);
      if (predefinedInfo.section) {
         _logic->readDictionary(predefinedInfo.section, moduleScope->predefined);
      }
   }
   else if (name.compare(ATTRIBUTES_FORWARD)) {
      moduleScope->attributes.clear();

      auto attributeInfo = moduleScope->getSection(ATTRIBUTES_FORWARD, mskMetaDictionaryRef, true);
      if (attributeInfo.section) {
         _logic->readDictionary(attributeInfo.section, moduleScope->attributes);
      }
   }
   else if (name.compare(OPERATION_FORWARD)) {
      moduleScope->operations.clear();

      auto operationInfo = moduleScope->getSection(OPERATION_FORWARD, mskDeclAttributesRef, true);
      if (operationInfo.section) {
         _logic->readDeclDictionary(operationInfo.module, operationInfo.section, moduleScope->operations, moduleScope);
      }
   }
   else if (name.compare(ALIASES_FORWARD)) {
      moduleScope->aliases.clear();

      auto aliasInfo = moduleScope->getSection(ALIASES_FORWARD, mskMetaAttributesRef, true);
      if (aliasInfo.section) {
         _logic->readAttrDictionary(aliasInfo.module, aliasInfo.section, moduleScope->aliases, moduleScope);
      }
   }
   else return false;

   return true;
}

void Compiler :: prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver)
{
   reloadMetaDictionary(moduleScope, PREDEFINED_FORWARD);
   reloadMetaDictionary(moduleScope, ATTRIBUTES_FORWARD);
   reloadMetaDictionary(moduleScope, OPERATION_FORWARD);
   reloadMetaDictionary(moduleScope, ALIASES_FORWARD);

   // cache the frequently used references
   moduleScope->buildins.superReference = safeMapReference(moduleScope, forwardResolver, SUPER_FORWARD);
   moduleScope->buildins.intReference = safeMapReference(moduleScope, forwardResolver, INTLITERAL_FORWARD);
   moduleScope->buildins.literalReference = safeMapReference(moduleScope, forwardResolver, LITERAL_FORWARD);

   moduleScope->branchingInfo.typeRef = safeMapReference(moduleScope, forwardResolver, BOOL_FORWARD);
   moduleScope->branchingInfo.trueRef = safeMapReference(moduleScope, forwardResolver, TRUE_FORWARD);
   moduleScope->branchingInfo.falseRef = safeMapReference(moduleScope, forwardResolver, FALSE_FORWARD);

   // cache the frequently used messages
   moduleScope->buildins.dispatch_message = encodeMessage(
      moduleScope->module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);
   moduleScope->buildins.init_message =
      encodeMessage(moduleScope->module->mapAction(INIT_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE | STATIC_MESSAGE);
   moduleScope->buildins.add_message =
      encodeMessage(moduleScope->module->mapAction(ADD_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.if_message =
      encodeMessage(moduleScope->module->mapAction(IF_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.equal_message =
      encodeMessage(moduleScope->module->mapAction(EQUAL_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.not_message =
      encodeMessage(moduleScope->module->mapAction(NOT_MESSAGE, 0, false),
         1, PROPERTY_MESSAGE);
   moduleScope->buildins.notequal_message =
      encodeMessage(moduleScope->module->mapAction(NOTEQUAL_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.less_message =
      encodeMessage(moduleScope->module->mapAction(LESS_MESSAGE, 0, false),
         2, 0);

   // cache self variable
   moduleScope->selfVar.copy(moduleScope->predefined.retrieve<ref_t>("@self", V_SELF_VAR, 
      [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      }));
   moduleScope->declVar.copy(moduleScope->predefined.retrieve<ref_t>("@decl", V_DECL_VAR, 
      [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      }));
}

void Compiler :: validateScope(ModuleScopeBase* moduleScope)
{
   if (moduleScope->buildins.superReference == 0)
      _errorProcessor->raiseInternalError(errNotDefinedBaseClass);
}

void Compiler :: validateSuperClass(ClassScope& scope, SyntaxNode node)
{
   if (!scope.info.methods.exist(scope.moduleScope->buildins.dispatch_message))
      scope.raiseError(errNoDispatcher, node);
}

void Compiler :: declareModuleIdentifiers(ModuleScopeBase* moduleScope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic);

         // declare namespace
         declareNamespace(ns, current, true, true);
         ns.moduleScope->newNamespace(*ns.nsName);

         // declare all module members - map symbol identifiers
         declareMemberIdentifiers(ns, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareModule(ModuleScopeBase* moduleScope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic);

         // declare namespace
         declareNamespace(ns, current, false, true);

         // declare all module members - map symbol identifiers
         declareMembers(ns, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declare(ModuleScopeBase* moduleScope, SyntaxTree& input)
{
   validateScope(moduleScope);

   SyntaxNode root = input.readRoot();
   // declare all memeber identifiers
   declareModuleIdentifiers(moduleScope, root);

   // declare all memebers
   declareModule(moduleScope, root);
}

void Compiler :: compile(ModuleScopeBase* moduleScope, SyntaxTree& input, BuildTree& output)
{
   BuildTreeWriter writer(output);
   writer.newNode(BuildKey::Root);

   SyntaxNode node = input.readRoot();
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic);
         declareNamespace(ns, current);

         compileNamespace(writer, ns, current);
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

inline SyntaxNode newVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message)
{
   SyntaxNode methodNode = classNode.appendChild(methodType, message);
   methodNode.appendChild(SyntaxKey::Hints, (ref_t)MethodHint::Multimethod);

   return methodNode;
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ClassInfo& info, mssg_t message, bool inherited, ref_t outputRef)
{
   mssg_t resendMessage = message;
   ref_t  resendTarget = 0;

   ref_t actionRef, flags;
   pos_t argCount;
   decodeMessage(message, actionRef, argCount, flags);

   if (inherited) {
      // if virtual multi-method handler is overridden
      // redirect to the parent one
      resendTarget = info.header.parentRef;
   }
   else {
      ref_t dummy = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, dummy);

      ref_t signatureLen = 0;
      ref_t signatures[ARG_COUNT];

      pos_t firstArg = /*test(flags, FUNCTION_MESSAGE) ? 0 : */1;
      for (pos_t i = firstArg; i < argCount; i++) {
         signatures[signatureLen++] = scope.buildins.superReference;
      }
      ref_t signRef = scope.module->mapAction(actionName,
         scope.module->mapSignature(signatures, signatureLen, false), false);

      resendMessage = encodeMessage(signRef, argCount, flags);
   }

   injectVirtualMultimethod(classNode, methodType, message, resendMessage, resendTarget, outputRef);
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message,
   mssg_t resendMessage, ref_t resendTarget, ref_t outputRef)
{
   SyntaxNode methodNode = newVirtualMultimethod(classNode, methodType, message);
   methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated multi-method

   if (outputRef)
      methodNode.appendChild(SyntaxKey::OutputType, outputRef);

   if (message == resendMessage) {
      methodNode.appendChild(SyntaxKey::RedirectDispatch, resendTarget);
   }
   else methodNode.appendChild(SyntaxKey::ResendDispatch, resendMessage);
}

void Compiler :: injectInitializer(SyntaxNode classNode, SyntaxKey methodType, mssg_t message)
{
   SyntaxNode methodNode = classNode.appendChild(methodType, message);
   methodNode.appendChild(SyntaxKey::Hints, (ref_t)MethodHint::Initializer);
   methodNode.appendChild(SyntaxKey::Autogenerated);

   methodNode.appendChild(SyntaxKey::FieldInitializer);
}

void Compiler :: injectDefaultConstructor(ModuleScopeBase* scope, SyntaxNode node)
{
   mssg_t message = /*protectedOne ? scope.protected_constructor_message : */scope->buildins.constructor_message;
   MethodHint hints = MethodHint::Constructor;
   //if (protectedOne) hints |= MethodHint::Protected;

   SyntaxNode methodNode = node.appendChild(SyntaxKey::Constructor, message);
   methodNode.appendChild(SyntaxKey::Autogenerated);
   methodNode.appendChild(SyntaxKey::Hints, (int)hints);
}

void Compiler :: injectVirtualReturningMethod(ModuleScopeBase* scope, SyntaxNode classNode,
   mssg_t message, ustr_t retVar, ref_t classRef)
{
   SyntaxNode methNode = classNode.appendChild(SyntaxKey::Method, message);
   //methNode.appendChild(lxAutogenerated); // !! HOTFIX : add a template attribute to enable explicit method declaration
   //methNode.appendChild(lxAttribute, tpEmbeddable);
   methNode.appendChild(SyntaxKey::Hints, (ref_t)MethodHint::Sealed | (ref_t)MethodHint::Conversion);

   if (classRef) {
      methNode.appendChild(SyntaxKey::OutputType, classRef);
   }

   SyntaxNode exprNode = methNode.appendChild(SyntaxKey::ReturnExpression).appendChild(SyntaxKey::Expression);
   //exprNode.appendNode(lxAttribute, V_NODEBUGINFO);
   exprNode.appendChild(SyntaxKey::Object).appendChild(SyntaxKey::identifier, retVar);
}

void Compiler :: generateOverloadListMember(ModuleScopeBase& scope, ref_t listRef, ref_t classRef, 
   mssg_t messageRef, MethodHint type)
{
   MemoryWriter metaWriter(scope.module->mapSection(listRef | mskConstArray, false));
   if (metaWriter.position() == 0) {
      metaWriter.writeDReference(0, messageRef);
      switch (type) {
         case MethodHint::Sealed:
            metaWriter.writeDReference(classRef | mskVMTMethodAddress, messageRef);
            break;
         case MethodHint::Virtual:
            metaWriter.writeDReference(classRef | mskVMTMethodOffset, messageRef);
            break;
         default:
            metaWriter.writeDWord(0);
            break;
      }
      metaWriter.writeDWord(0);
   }
   else {
      metaWriter.insertDWord(0, 0);
      metaWriter.insertDWord(0, messageRef);
      metaWriter.Memory()->addReference(0, 0);
      switch (type) {
         case MethodHint::Sealed:
            metaWriter.Memory()->addReference(classRef | mskVMTMethodAddress, 4);
            break;
         case MethodHint::Virtual:
            metaWriter.Memory()->addReference(classRef | mskVMTMethodOffset, 4);
            break;
         default:
            break;
      }
   }
}
