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

//#define FULL_OUTOUT_INFO 1

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

MethodHint operator | (const ref_t& l, const MethodHint& r)
{
   return (MethodHint)(l | (unsigned int)r);
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

inline bool isSelfCall(ObjectInfo target)
{
   switch (target.kind) {
      case ObjectKind::SelfLocal:
      case ObjectKind::SelfBoxableLocal:
         //case okOuterSelf:
      //case okClassSelf:
      //case okInternalSelf:
         return true;
      default:
         return false;
   }
}

inline ref_t getSignature(ModuleBase* module, mssg_t message)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(message, actionRef, argCount, flags);

   if (argCount <= (test(flags, FUNCTION_MESSAGE) ? 0u : 1u))
      return 0;

   module->resolveAction(actionRef, signRef);

   return signRef;
}

inline void addByRefRetVal(ArgumentsInfo& arguments, ObjectInfo& tempRetVal)
{
   if (tempRetVal.kind == ObjectKind::TempLocal) {
      arguments.add({ ObjectKind::RefLocal, { V_WRAPPER, tempRetVal.typeInfo.typeRef }, tempRetVal.argument, tempRetVal.extra });
   }
   else arguments.add({ ObjectKind::TempLocalAddress, { V_WRAPPER, tempRetVal.typeInfo.typeRef }, tempRetVal.argument, tempRetVal.extra });
}

void declareTemplateParameters(ModuleBase* module, TemplateTypeList& typeList,
   SyntaxTree& dummyTree, List<SyntaxNode>& parameters)
{
   SyntaxTreeWriter dummyWriter(dummyTree);
   dummyWriter.newNode(SyntaxKey::Root);

   for (size_t i = 0; i < typeList.count(); i++) {
      ref_t elementRef = typeList[i];

      dummyWriter.newNode(SyntaxKey::TemplateArg, elementRef);

      parameters.add(dummyWriter.CurrentNode());

      dummyWriter.newNode(SyntaxKey::Type);

      ustr_t referenceName = module->resolveReference(elementRef);
      if (isWeakReference(referenceName)) {
         dummyWriter.appendNode(SyntaxKey::reference, referenceName);
      }
      else dummyWriter.appendNode(SyntaxKey::globalreference, referenceName);

      dummyWriter.closeNode();
      dummyWriter.closeNode();
   }

   dummyWriter.closeNode();
}

// --- Interpreter ---

Interpreter :: Interpreter(ModuleScopeBase* scope, CompilerLogic* logic)
{
   _scope = scope;
   _logic = logic;
}

ObjectInfo Interpreter :: mapStringConstant(ustr_t s)
{
   return ObjectInfo(ObjectKind::StringLiteral, { V_STRING }, _scope->module->mapConstant(s));
}

ObjectInfo Interpreter :: mapWideStringConstant(ustr_t s)
{
   return ObjectInfo(ObjectKind::WideStringLiteral, { V_WIDESTRING }, _scope->module->mapConstant(s));
}

void Interpreter :: addTypeListItem(ref_t dictionaryRef, ref_t symbolRef)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskTypeListRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeArrayEntry(dictionary, symbolRef | mskSymbolRef);
}

void Interpreter :: setTypeMapValue(ref_t dictionaryRef, ustr_t key, ref_t reference)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskTypeMapRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeTypeMapEntry(dictionary, key, reference);
}

void Interpreter :: setAttributeMapValue(ref_t dictionaryRef, ustr_t key, int value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskAttributeMapRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeAttributeMapEntry(dictionary, key, value);
}

void Interpreter :: setAttributeMapValue(ref_t dictionaryRef, ustr_t key, ustr_t value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskStringMapRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeAttributeMapEntry(dictionary, key, value);
}

bool Interpreter :: evalDictionaryOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];

   if (args.count() == 3) {
      if (loperand.kind == ObjectKind::AttributeDictionary && roperand.kind == ObjectKind::IntLiteral
         && args[2].kind == ObjectKind::StringLiteral)
      {
         ObjectInfo ioperand = args[2];

         ustr_t key = _scope->module->resolveConstant(ioperand.reference);
         int value = roperand.extra;

         if (operator_id == SET_INDEXER_OPERATOR_ID) {
            setAttributeMapValue(loperand.reference, key, value);

            return true;
         }
      }
      else if (loperand.kind == ObjectKind::TypeDictionary
         && (roperand.kind == ObjectKind::Symbol || roperand.kind == ObjectKind::Template)
         && args[2].kind == ObjectKind::StringLiteral)
      {
         ObjectInfo ioperand = args[2];

         ustr_t key = _scope->module->resolveConstant(ioperand.reference);
         ref_t reference = roperand.reference;

         if (operator_id == SET_INDEXER_OPERATOR_ID) {
            setTypeMapValue(loperand.reference, key, reference);

            return true;
         }
      }
      else if (loperand.kind == ObjectKind::StringDictionary
         && (roperand.kind == ObjectKind::StringLiteral)
         && args[2].kind == ObjectKind::StringLiteral)
      {
         ObjectInfo ioperand = args[2];

         ustr_t key = _scope->module->resolveConstant(ioperand.reference);
         ustr_t value = _scope->module->resolveConstant(roperand.reference);

         if (operator_id == SET_INDEXER_OPERATOR_ID) {
            setAttributeMapValue(loperand.reference, key, value);

            return true;
         }
      }
   }

   return false;
}

bool Interpreter :: evalObjArrayOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];
   if (loperand.kind == ObjectKind::TypeList && roperand.kind == ObjectKind::Symbol) {
      if (operator_id == ADD_ASSIGN_OPERATOR_ID) {
         addTypeListItem(loperand.reference, roperand.reference | mskSymbolRef);

         return true;
      }
   }

   return false;
}

bool Interpreter :: evalDeclOp(ref_t operator_id, ArgumentsInfo& args, ObjectInfo& retVal)
{
   ObjectInfo loperand = args[0];
   if (operator_id == NAME_OPERATOR_ID) {
      switch (loperand.kind) {
         case ObjectKind::Template:
         {
            ReferenceProperName name(_scope->resolveFullName(loperand.reference));

            retVal = mapStringConstant(*name);

            return true;
         }
         case ObjectKind::Class:
            retVal = { ObjectKind::SelfName };
            return true;
         case ObjectKind::Method:
            retVal = { ObjectKind::MethodName };
            return true;
         case ObjectKind::Field:
            retVal = { ObjectKind::FieldName, { V_STRING }, loperand.reference };
            return true;
         default:
            break;
      }
   }

   return false;
}

bool Interpreter :: eval(BuildKey key, ref_t operator_id, ArgumentsInfo& arguments, ObjectInfo& retVal)
{
   switch (key) {
      case BuildKey::DictionaryOp:
         return evalDictionaryOp(operator_id, arguments);
      case BuildKey::ObjArrayOp:
         return evalObjArrayOp(operator_id, arguments);
      //case BuildKey::DeclDictionaryOp:
      //   return evalDeclDictionaryOp(operator_id, arguments);
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
   ObjectInfo info = {};

   bool metaOne = ExpressionAttributes::test(mode, ExpressionAttribute::Meta);
   //bool weakOne = ExpressionAttributes::test(mode, ExpressionAttribute::Weak);
   bool internOne = ExpressionAttributes::test(mode, ExpressionAttribute::Intern);

   if (reference) {
      if (metaOne) {
         // check if it is a meta symbol
         if (module->mapSection(reference | mskAttributeMapRef, true)) {
            info.kind = ObjectKind::AttributeDictionary;
            info.typeInfo = { V_DICTIONARY, V_INT32 };
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskStringMapRef, true)) {
            info.kind = ObjectKind::StringDictionary;
            info.typeInfo = { V_DICTIONARY, V_STRING };
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskTypeMapRef, true)) {
            info.kind = ObjectKind::TypeDictionary;
            info.typeInfo = { V_DICTIONARY, V_SYMBOL };
            info.reference = reference;

            return info;
         }
         else if (module->mapSection(reference | mskTypeListRef, true)) {
            info.kind = ObjectKind::TypeList;
            info.typeInfo = { V_OBJARRAY, V_SYMBOL };
            info.reference = reference;

            return info;
         }
   //      else if (module->mapSection(reference | mskMetaAttributesRef, true)) {
   //         info.kind = ObjectKind::MetaDictionary;
   //         info.type = V_OBJATTRIBUTES;
   //         info.reference = reference;

   //         return info;
   //      }
   //      else if (module->mapSection(reference | mskDeclAttributesRef, true)) {
   //         info.kind = ObjectKind::MetaDictionary;
   //         info.type = V_DECLATTRIBUTES;
   //         info.reference = reference;

   //         return info;
   //      }
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
                  return { ObjectKind::Singleton, { reference }, reference };
               }
               // if it is a normal class
               // then the symbol is reference to the class class
               else if (test(classInfo.header.flags, elStandartVMT) && classInfo.header.classRef != 0) {
                  return { ObjectKind::Class, { classInfo.header.classRef }, reference };
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
         return { ObjectKind::Nil, { reference }, 0 };
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

   return defineObjectInfo(reference, EAttr::None, true);
}

ObjectInfo Compiler::NamespaceScope :: mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode)
{
   IdentifierString metaIdentifier(META_PREFIX, identifier);

   // check if it is a meta dictionary
   return mapIdentifier(*metaIdentifier, referenceOne, mode | EAttr::Meta);
}

// --- Compiler::FieldScope ---

Compiler::FieldScope :: FieldScope(Scope* parent, ustr_t fieldName)
   : Scope(parent), fieldName(fieldName)
{
   
}

// --- Compiler::MetaScope ---

Compiler::MetaScope :: MetaScope(Scope* parent, ScopeLevel scopeLevel)
   : Scope(parent), scopeLevel(scopeLevel)
{
   
}

ObjectInfo Compiler::MetaScope :: mapDecl()
{
   TemplateScope* tempScope = Scope::getScope<TemplateScope>(*this, ScopeLevel::Template);
   if (tempScope != nullptr) {
      return { ObjectKind::Template, { V_DECLARATION }, tempScope->reference };
   }

   MethodScope* methodScope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
   if (methodScope != nullptr) {
      return { ObjectKind::Method, { V_DECLARATION }, methodScope->message };
   }

   FieldScope* fieldScope = Scope::getScope<FieldScope>(*this, ScopeLevel::Field);
   if (fieldScope != nullptr) {
      ref_t nameRef = module->mapConstant(fieldScope->fieldName);

      return { ObjectKind::Field, { V_DECLARATION }, nameRef };
   }

   ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);
   if (classScope != nullptr) {
      return { ObjectKind::Class, { V_DECLARATION }, classScope->reference };
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
         ObjectInfo retVal = {};
         if (EAttrs::testAndExclude(attr, EAttr::Superior) && parent->parent != nullptr) {
            retVal = parent->parent->mapDictionary(identifier, referenceOne, attr);
         }
         else retVal = mapDictionary(identifier, referenceOne, attr);
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

inline ObjectInfo mapClassInfoField(ClassInfo& info, ustr_t identifier, ExpressionAttribute attr, bool ignoreFields)
{
   bool readOnly = test(info.header.flags, elReadOnlyRole)
      && !EAttrs::test(attr, EAttr::InitializerScope);

   auto fieldInfo = info.fields.get(identifier);
   if (!ignoreFields && fieldInfo.offset >= 0) {
      if (test(info.header.flags, elStructureRole)) {
         return { readOnly ? ObjectKind::ReadOnlyFieldAddress : ObjectKind::FieldAddress,
            fieldInfo.typeInfo, fieldInfo.offset };
      }
      else return { readOnly ? ObjectKind::ReadOnlyField : ObjectKind::Field,
         fieldInfo.typeInfo, fieldInfo.offset };
   }
   else if (!ignoreFields && fieldInfo.offset == -2) {
      return { readOnly ? ObjectKind::ReadOnlySelfLocal : ObjectKind::SelfLocal, fieldInfo.typeInfo, 1 };
   }
   else {
      auto staticFieldInfo = info.statics.get(identifier);
      if (staticFieldInfo.valueRef && staticFieldInfo.offset == 0) {
         return { ObjectKind::ClassConstant, staticFieldInfo.typeInfo, staticFieldInfo.valueRef };
      }
      else if (staticFieldInfo.offset < 0 && staticFieldInfo.valueRef != 0) {
         return { ObjectKind::StaticConstField, staticFieldInfo.typeInfo, staticFieldInfo.offset };
      }
      else if (staticFieldInfo.valueRef)
         return { ObjectKind::StaticField, staticFieldInfo.typeInfo, staticFieldInfo.valueRef };

      return {};
   }
}

ObjectInfo Compiler::ClassScope :: mapField(ustr_t identifier, ExpressionAttribute attr)
{
   if (extensionClassRef) {
      ClassInfo targetInfo;
      moduleScope->loadClassInfo(targetInfo, extensionClassRef, false, false);

      return mapClassInfoField(targetInfo, identifier, attr, true);
   }
   else return mapClassInfoField(info, identifier, attr, false);
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

ObjectInfo Compiler::ClassScope :: mapDictionary(ustr_t identifier, bool referenceOne, ExpressionAttribute mode)
{
   IdentifierString metaIdentifier(META_PREFIX,identifier);
   metaIdentifier.append('$');
   metaIdentifier.append(module->resolveReference(reference));
   metaIdentifier.replaceAll('\'', '@', 0);

   // check if it is a meta dictionary
   auto retVal = mapIdentifier(*metaIdentifier, referenceOne, mode | EAttr::Meta);
   if (retVal.kind == ObjectKind::Unknown) {
      return Scope::mapDictionary(identifier, referenceOne, mode);
   }
   else return retVal;
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
   closureMode(false),
   constructorMode(false),
   isEmbeddable(false),
   byRefReturnMode(false),
   isExtension(false)
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

ObjectInfo Compiler::MethodScope :: mapSelf(bool memberMode)
{
   if (!memberMode && isExtension) {
      ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);

      return { ObjectKind::Param, { classScope->extensionClassRef }, -1 };
   }
   else if (selfLocal != 0) {
      if (isEmbeddable) {
         return { ObjectKind::SelfBoxableLocal, { getClassRef(false) }, (ref_t)selfLocal };
      }
      else return { ObjectKind::SelfLocal, { getClassRef(false) }, (ref_t)selfLocal };
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapParameter(ustr_t identifier, ExpressionAttribute attr)
{
   int prefix = functionMode ? 0 : -1;

   Parameter local = parameters.get(identifier);
   if (local.offset != -1) {
      bool byRef = local.typeInfo.typeRef == V_WRAPPER;

      if (local.size > 0) {
         if (byRef) {
            return { ObjectKind::ByRefParamAddress, { local.typeInfo.elementRef }, prefix - local.offset, local.size };
         }
         else return { ObjectKind::ParamAddress, local.typeInfo, prefix - local.offset };
      }
      else {
         if (byRef) {
            return { ObjectKind::ByRefParam, { local.typeInfo.elementRef }, prefix - local.offset };
         }
         else return { ObjectKind::Param, local.typeInfo, prefix - local.offset };
      }
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
{
   auto paramInfo = mapParameter(identifier, attr);
   if (paramInfo.kind != ObjectKind::Unknown) {
      return paramInfo;
   }
   else if (moduleScope->selfVar.compare(identifier)) {
      return mapSelf();
   }
   else if (moduleScope->superVar.compare(identifier)) {
      ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);

      return { ObjectKind::SuperLocal, { classScope->info.header.parentRef }, selfLocal };
   }

   if (constructorMode)
      attr = attr | EAttr::InitializerScope;

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
         return { ObjectKind::LocalAddress, local.typeInfo, local.offset };
      }
      else return { ObjectKind::Local, local.typeInfo, local.offset };
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

ObjectInfo Compiler::CodeScope :: mapByRefReturnArg()
{
   MethodScope* scope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);

   return scope->mapParameter(RETVAL_ARG, EAttr::None);
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

bool Compiler::CodeScope :: resolveAutoType(ObjectInfo& info, TypeInfo typeInfo)
{
   if (info.kind == ObjectKind::Local) {
      for (auto it = locals.start(); !it.eof(); ++it) {
         if ((*it).offset == (int)info.reference) {
            if ((*it).typeInfo.typeRef == V_AUTO) {
               (*it).typeInfo.typeRef = typeInfo.typeRef;
               (*it).typeInfo.elementRef = typeInfo.elementRef;

               info.typeInfo = typeInfo;

               return true;
            }
         }
      }
   }

   return Scope::resolveAutoType(info, typeInfo);
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

ObjectInfo Compiler::ExprScope :: mapMember(ustr_t identifier)
{
   MethodScope* methodScope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
   if (methodScope != nullptr && moduleScope->selfVar.compare(identifier)) {
      return methodScope->mapSelf(true);
   }

   ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);
   if (classScope) {
      //if (methodScope)
      return classScope->mapField(identifier, EAttr::None);
   }

   return Scope::mapMember(identifier);
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
   : ClassScope(owner, reference, Visibility::Internal),
      outers({})
{
}

inline void mapNewField(ClassInfo::FieldMap& fields, ustr_t name, FieldInfo info)
{
   if (!fields.exist(name)) {
      fields.add(name, info);
   }
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   Outer ownerVar = outers.get(*moduleScope->selfVar);
   // if owner reference is not yet mapped, add it
   if (ownerVar.outerObject.kind == ObjectKind::Unknown) {
      ownerVar.reference = info.fields.count();

      ownerVar.outerObject = parent->mapIdentifier(*moduleScope->selfVar, false, EAttr::None);
      if (ownerVar.outerObject.kind == ObjectKind::Unknown) {
         // HOTFIX : if it is a singleton nested class
         ownerVar.outerObject = { ObjectKind::SelfLocal, {}, 1 };
      }
      else if (ownerVar.outerObject.kind == ObjectKind::SelfLocal) {
         ownerVar.outerObject.typeInfo.typeRef = ((ExprScope*)parent)->getClassRef(false);
      }

      outers.add(*moduleScope->selfVar, ownerVar);
      mapNewField(info.fields, *moduleScope->selfVar, FieldInfo{ (int)ownerVar.reference, ownerVar.outerObject.typeInfo });
   }

   return ownerVar;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapOwner()
{
   Outer ownerVar = outers.get(OWNER_VAR);
   if (ownerVar.outerObject.kind == ObjectKind::Unknown) {
      ownerVar.outerObject = parent->mapIdentifier(OWNER_VAR, false, EAttr::None);
      if (ownerVar.outerObject.kind != ObjectKind::Unknown) {
         ownerVar.reference = info.fields.count();

         outers.add(OWNER_VAR, ownerVar);
         mapNewField(info.fields, OWNER_VAR, FieldInfo{ (int)ownerVar.reference, ownerVar.outerObject.typeInfo });
      }
      else return mapSelf();
   }

   return ownerVar;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapParent()
{
   Outer parentVar = outers.get(PARENT_VAR);
   if (parentVar.outerObject.kind == ObjectKind::Unknown) {
      parentVar.reference = info.fields.count();
      ExprScope* exprScope = Scope::getScope<ExprScope>(*parent, ScopeLevel::Expr);
      if (exprScope) {
         parentVar.outerObject = exprScope->mapSelf();
      }
      else parentVar = mapOwner();

      outers.add(PARENT_VAR, parentVar);
      mapNewField(info.fields, PARENT_VAR, FieldInfo{ (int)parentVar.reference, parentVar.outerObject.typeInfo });
   }

   return parentVar;
}

ObjectInfo Compiler::InlineClassScope :: mapIdentifier(ustr_t identifier, bool referenceOne, ExpressionAttribute attr)
{
   Outer outer = outers.get(identifier);
   if (outer.reference != INVALID_REF) {
      return { ObjectKind::Outer, outer.outerObject.typeInfo, outer.reference };
   }
   else {
      outer.outerObject = parent->mapIdentifier(identifier, referenceOne, attr);
      switch (outer.outerObject.kind) {
         case ObjectKind::Field:
         case ObjectKind::ReadOnlyField:
         {
            // handle outer fields in a special way: save only self
            Outer owner = mapParent();

            return { ObjectKind::OuterField, outer.outerObject.typeInfo, outer.outerObject.reference };
         }
         case ObjectKind::Param:
         case ObjectKind::Local:
         case ObjectKind::Outer:
         case ObjectKind::OuterField:
         case ObjectKind::SuperLocal:
         case ObjectKind::SelfLocal:
         case ObjectKind::LocalAddress:
         case ObjectKind::FieldAddress:
         case ObjectKind::ReadOnlyFieldAddress:
         {
            // map if the object is outer one
            outer.reference = info.fields.count();

            outers.add(identifier, outer);
            mapNewField(info.fields, identifier, FieldInfo{ (int)outer.reference, outer.outerObject.typeInfo });

            return { ObjectKind::Outer, outer.outerObject.typeInfo, outer.reference };
         }
         default:
            return outer.outerObject;
      }
   }
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

   _optMode = false;
   _tapeOptMode = false;
}

inline ref_t resolveDictionaryMask(TypeInfo typeInfo)
{
   if (typeInfo.typeRef == V_DICTIONARY) {
      switch (typeInfo.elementRef) {
         case V_INT32:
            return mskAttributeMapRef;
         case V_STRING:
            return mskStringMapRef;
         case V_SYMBOL:
            return mskTypeMapRef;
         default:
            break;
      }
   }
   else if (typeInfo.typeRef == V_STRINGOBJ) {
      switch (typeInfo.elementRef) {
         case V_SYMBOL:
            return mskTypeListRef;
         default:
            break;
      }
   }

   return 0;
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
      if (scope.module->mapSection(reference | mskTypeListRef, true))
         scope.raiseError(errDuplicatedSymbol, nameNode.firstChild(SyntaxKey::TerminalMask));

      if (scope.module->mapSection(reference | mskTypeMapRef, true))
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

ref_t Compiler :: retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters, 
   ustr_t prefix, bool skipLoading)
{
   SyntaxNode identNode = node.firstChild(SyntaxKey::TerminalMask);

   IdentifierString templateName;

   if (!skipLoading) {
      SyntaxNode current = node.firstChild();
      while (current.key != SyntaxKey::None) {
         if (current.key == SyntaxKey::TemplateArg) {
            parameters.add(current);
         }

         current = current.nextNode();
      }
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

bool Compiler :: importTemplate(Scope& scope, SyntaxNode node, SyntaxNode target)
{
   TemplateTypeList typeList;
   declareTemplateAttributes(scope, node, typeList, true, false);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, nullptr, true);
   if (!templateRef)
      return false;

   if(!_templateProcessor->importTemplate(*scope.moduleScope, templateRef, target, parameters))
      scope.raiseError(errInvalidOperation, node);

   return true;
}

bool Compiler :: importInlineTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target)
{
   List<SyntaxNode> parameters({});

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, postfix, false);
   if (!templateRef)
      return false;

   if(!_templateProcessor->importInlineTemplate(*scope.moduleScope, templateRef, target, parameters))
      scope.raiseError(errInvalidOperation, node);

   return true;
}

bool Compiler :: importPropertyTemplate(Scope& scope, SyntaxNode node, ustr_t postfix, SyntaxNode target)
{
   List<SyntaxNode> parameters({});

   SyntaxTree tree;
   SyntaxTreeWriter writer(tree);
   writer.newNode(SyntaxKey::Root);
   // add implicit name
   SyntaxNode nameNode = target.findChild(SyntaxKey::Name);
   if (nameNode != SyntaxKey::None) {
      writer.newNode(SyntaxKey::TemplateArg);
      SyntaxTree::copyNode(writer, nameNode, true);

      parameters.add(writer.CurrentNode());

      writer.closeNode();
   }
   // add implicit type
   SyntaxNode typeNode = target.findChild(SyntaxKey::Type);
   if (typeNode != SyntaxKey::None) {
      writer.newNode(SyntaxKey::TemplateArg);
      SyntaxTree::copyNode(writer, typeNode, true);

      parameters.add(writer.CurrentNode());

      writer.closeNode();
   }
   writer.closeNode();

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, postfix, false);
   if (!templateRef)
      return false;

   if (!_templateProcessor->importPropertyTemplate(*scope.moduleScope, templateRef, 
      target.parentNode(), parameters))
   {
      scope.raiseError(errInvalidOperation, node);
   }

   return true;
}

void Compiler :: declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility, Scope::ScopeLevel level)
{
   bool superMode = false;
   TypeInfo typeInfo = { V_DICTIONARY, V_INT32 };
   declareDictionaryAttributes(scope, node, typeInfo, superMode);

   if (superMode) {
      switch (level) {
         case Scope::ScopeLevel::Class:
            level = Scope::ScopeLevel::Namespace;
            break;
         case Scope::ScopeLevel::Method:
         case Scope::ScopeLevel::Field:
            level = Scope::ScopeLevel::Class;
            break;
         default:
            break;
      }
   }

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   IdentifierString postfix;
   switch (level) {
      case Scope::ScopeLevel::Class:
      {
         ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);
         postfix.append('$');
         postfix.append(scope.module->resolveReference(classScope->reference));

         break;
      }
      default:
         break;
   }      

   postfix.replaceAll('\'', '@', 0);

   ref_t reference = mapNewTerminal(scope, META_PREFIX, name, *postfix, visibility);
   ref_t mask = resolveDictionaryMask(typeInfo);

   assert(mask != 0);

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
         dataWriter.writeString(value, value.length_pos() + 1);

         retVal.typeInfo = { scope.moduleScope->buildins.literalReference };
         break;
      }
      default:
         break;
   }

   ref_t typeRef = retrieveStrongType(scope, retVal);
   dataWriter.Memory()->addReference(typeRef | mskVMTRef, (pos_t)-4);

   // save constant meta info
   SymbolInfo constantInfo = { SymbolType::Constant, constRef, typeRef };
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

   if (node.existChild(SyntaxKey::Autogenerated)) {
      methodInfo.hints |= (ref_t)MethodHint::Autogenerated;
   }
   else methodInfo.hints &= ~((ref_t)MethodHint::Autogenerated);

   if (_logic->isEmbeddableStruct(scope.info))
      methodInfo.hints |= (ref_t)MethodHint::Stacksafe;

   ref_t outputRef = node.findChild(SyntaxKey::OutputType).arg.reference;

   mssg_t multiMethod = node.findChild(SyntaxKey::Multimethod).arg.reference;
   if (multiMethod)
      methodInfo.multiMethod = multiMethod;

   mssg_t byRefMethod = node.findChild(SyntaxKey::ByRefRetMethod).arg.reference;
   if (byRefMethod)
      methodInfo.byRefHandler = byRefMethod;

   // check duplicates with different visibility scope
   if (MethodScope::checkHint(methodInfo, MethodHint::Private)) {
      checkMethodDuplicates(scope, node, message, message & ~STATIC_MESSAGE, false, false);
   }
   else if (MethodScope::checkAnyHint(methodInfo, MethodHint::Protected, MethodHint::Internal)) {
      // if it is internal / protected message save the public alias
      ref_t signRef = 0;
      ustr_t name = scope.module->resolveAction(getAction(message), signRef);
      mssg_t publicMessage = 0;

      if (name.compare(CONSTRUCTOR_MESSAGE2)) {
         publicMessage = overwriteArgCount(scope.moduleScope->buildins.constructor_message, getArgCount(message));
      }
      else {
         size_t index = name.findStr("$$");
         if (index == NOTFOUND_POS)
            scope.raiseError(errDupInternalMethod, node);

         publicMessage = overwriteAction(message, scope.module->mapAction(name + index + 2, signRef, false));
      }

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

   if (isOpenArg(message))
      scope.info.header.flags |= elWithVariadics;
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

inline mssg_t retrieveMethod(Compiler::VirtualMethodList& implicitMultimethods, mssg_t multiMethod)
{
   return implicitMultimethods.retrieve<mssg_t>(multiMethod, [](mssg_t arg, Compiler::VirtualMethod current)
      {
         return current.value1 == arg;
      }).value1;
}

mssg_t Compiler :: defineMultimethod(Scope& scope, mssg_t messageRef, bool extensionMode)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0, signRef = 0;
   decodeMessage(messageRef, actionRef, argCount, flags);

   if (argCount <= (test(flags, FUNCTION_MESSAGE) ? 0u : 1u))
      return 0;

   ustr_t actionStr = scope.module->resolveAction(actionRef, signRef);

   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      // COMPILER MAGIC : for variadic message - use the most general message
      ref_t genericActionRef = scope.moduleScope->module->mapAction(actionStr, 0, false);

      pos_t genericArgCount = 2;
      // HOTFIX : a variadic extension is a special case of variadic function
      // - so the target should be included as well
      if (test(messageRef, FUNCTION_MESSAGE) && !extensionMode)
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

void Compiler :: injectVirtualCode(SyntaxNode classNode, ClassScope& scope, bool interfaceBased)
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
      if (scope.reference != scope.moduleScope->buildins.superReference && !interfaceBased) {
         // auto generate cast$<type> message for explicitly declared classes
         ref_t signRef = scope.module->mapSignature(&scope.reference, 1, false);
         ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

         injectVirtualReturningMethod(scope.moduleScope, classNode,
            encodeMessage(actionRef, 1, CONVERSION_MESSAGE),
            *scope.moduleScope->selfVar, scope.reference);
      }
   }
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
   ClassInfo& info, mssg_t multiMethod)
{
   MethodInfo methodInfo = {};

   auto m_it = info.methods.getIt(multiMethod);
   bool found = !m_it.eof();
   if (found)
      methodInfo = *m_it;

   if (!found || methodInfo.inherited) {
      Visibility visibility = Visibility::Public;
      if (MethodScope::checkHint(methodInfo, MethodHint::Internal)) {
         visibility = Visibility::Internal;
      }
      else if (MethodScope::checkHint(methodInfo, MethodHint::Protected)) {
         visibility = Visibility::Protected;
      }
      else if (MethodScope::checkHint(methodInfo, MethodHint::Private)) {
         visibility = Visibility::Private;
      }

      bool inherited = methodInfo.inherited;
      // HOTFIX : predefined is not inherited one
      if (MethodScope::checkHint(methodInfo, MethodHint::Predefined))
         inherited = false;

      injectVirtualMultimethod(classNode, methodType, scope, info, multiMethod, inherited, methodInfo.outputRef, visibility);

      // COMPILER MAGIC : injecting try-multi-method dispather
      if (_logic->isTryDispatchAllowed(scope, multiMethod)) {
         ref_t tryMessage = _logic->defineTryDispatcher(scope, multiMethod);

         injectVirtualTryDispatch(classNode, methodType, info, tryMessage, multiMethod, info.methods.exist(tryMessage));
      }
   }
}

void Compiler :: injectVirtualMethods(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, ClassInfo& info,
   VirtualMethodList& implicitMultimethods)
{
   // generate implicit mutli methods
   for (auto it = implicitMultimethods.start(); !it.eof(); ++it) {
      auto methodInfo = *it;
      switch (methodInfo.value2) {
         case VirtualType::Multimethod:
            injectVirtualMultimethod(classNode, methodType, scope, info, methodInfo.value1);
            break;
         case VirtualType::EmbeddableWrapper:
            injectVirtualEmbeddableWrapper(classNode, methodType, scope, info, methodInfo.value1);
            break;
         default:
            break;
      }      
   }
}

mssg_t Compiler :: defineByRefMethod(ClassScope& scope, SyntaxNode node)
{
   ref_t outputRef = node.findChild(SyntaxKey::OutputType).arg.reference;
   if (outputRef && _logic->isEmbeddable(*scope.moduleScope, outputRef)) {
      ref_t actionRef, flags;
      pos_t argCount;
      decodeMessage(node.arg.reference, actionRef, argCount, flags);

      ref_t signRef = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, signRef);
      ref_t signArgs[ARG_COUNT];
      size_t signLen = scope.module->resolveSignature(signRef, signArgs);
      if (signLen == (size_t)argCount - 1) {
         signArgs[signLen++] = resolvePrimitiveType(scope, { V_WRAPPER, outputRef }, true);

         mssg_t byRefMessage = encodeMessage(
            scope.module->mapAction(
               actionName, scope.module->mapSignature(signArgs, signLen, false), false), argCount + 1, flags);

         return byRefMessage;
      }
   }

   return 0;
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey, bool closed)
{
   VirtualMethodList implicitMultimethods({});
   bool thirdPassRequired = false;

   // first pass - mark all multi-methods
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodKey) {
         bool withRetOverload = test(current.findChild(SyntaxKey::Hints).arg.reference, (ref_t)MethodHint::RetOverload);

         // HOTFIX : methods with ret overload are special case and should not be  part of the overload list
         mssg_t multiMethod = withRetOverload ? 0 : defineMultimethod(scope, current.arg.reference, scope.extensionClassRef != 0);
         if (multiMethod) {
            //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
            current.appendChild(SyntaxKey::Multimethod, multiMethod);

            ref_t hints = (ref_t)MethodHint::Multimethod;
            if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_INTERNAL))
               hints |= (ref_t)MethodHint::Internal;
            if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_PROTECTED))
               hints |= (ref_t)MethodHint::Protected;

            // mark weak message as a multi-method
            auto m_it = scope.info.methods.getIt(multiMethod);
            if (!m_it.eof()) {
               (*m_it).hints |= hints;
            }
            else {
               MethodInfo info = {};
               info.hints |= hints;
               info.hints |= (ref_t)MethodHint::Predefined;
               info.inherited = true;

               scope.info.methods.add(multiMethod, info);
            }

            if (retrieveMethod(implicitMultimethods, multiMethod) == 0) {
               implicitMultimethods.add({ multiMethod, VirtualType::Multimethod });
               thirdPassRequired = true;
            }
         }

         if (methodKey != SyntaxKey::Constructor) {
            mssg_t byRefMethod = withRetOverload ? 0 : defineByRefMethod(scope, current);
            if (byRefMethod) {
               current.appendChild(SyntaxKey::ByRefRetMethod, byRefMethod);

               // HOTFIX : do not need to generate byref stub for the private method, it will be added later in the code
               if (!test(current.arg.reference, STATIC_MESSAGE) && retrieveMethod(implicitMultimethods, byRefMethod) == 0) {
                  implicitMultimethods.add({ byRefMethod, VirtualType::EmbeddableWrapper });
                  thirdPassRequired = true;
               }
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
      injectVirtualMethods(node, methodKey, *scope.moduleScope, scope.info, implicitMultimethods);
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

void Compiler :: generateClassStaticField(ClassScope& scope, SyntaxNode node, bool isConst, TypeInfo typeInfo)
{
   ustr_t name = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (scope.info.statics.exist(name)) {
      scope.raiseError(errDuplicatedField, node);
   }

   if (isConst) {
      // NOTE : the index is zero for the constants
      // NOTE : INVALID_REF indicates that the value should be assigned later
      scope.info.statics.add(name, { 0, typeInfo, INVALID_REF });
   }
   else {
      // if it is a static field
      ref_t staticRef = node.arg.reference;
      if (!staticRef) {
         // generate static reference
         IdentifierString name(scope.module->resolveReference(scope.reference));
         name.append(STATICFIELD_POSTFIX);

         staticRef = scope.moduleScope->mapAnonymous(*name);

         node.setArgumentReference(staticRef);
      }

      // NOTE : MAX_OFFSET indicates the sealed static field
      scope.info.statics.add(name, { MAX_OFFSET, typeInfo, staticRef });
   }
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxNode node,
   FieldAttributes& attrs, bool singleField)
{
   TypeInfo typeInfo = attrs.typeInfo;
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
   if (typeInfo.isPrimitive()) {
      if (!sizeHint) {
         scope.raiseError(errIllegalField, node);
      }
      // for primitive types size should be specified
      else sizeInfo.size = sizeHint;
   }
   else if (typeInfo.typeRef)
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, typeInfo.typeRef);

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

         typeInfo.elementRef = typeInfo.typeRef;

         typeInfo.typeRef = _logic->definePrimitiveArray(*scope.moduleScope, typeInfo.elementRef,
            test(scope.info.header.flags, elStructureRole));

         scope.info.fields.add(name, { -2, typeInfo });
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
         scope.info.fields.add(name, { offset, typeInfo });

         if (typeInfo.isPrimitive())
            _logic->tweakPrimitiveClassFlags(scope.info, typeInfo.typeRef);
      }
      else {
         // primitive / virtual classes cannot be declared
         if (sizeInfo.size != 0 && typeInfo.isPrimitive())
            scope.raiseError(errIllegalField, node);

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.count();
         scope.info.fields.add(name, { offset, typeInfo });
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

         if (attrs.isConstant || attrs.isStatic) {
            generateClassStaticField(scope, current, attrs.isConstant, attrs.typeInfo);
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

   injectVirtualCode(node, scope, closed);

   if (scope.isClassClass()) {
      generateMethodDeclarations(scope, node, SyntaxKey::StaticMethod, false);
      generateMethodDeclarations(scope, node, SyntaxKey::Constructor, false);
   }
   else {
      generateMethodDeclarations(scope, node, SyntaxKey::Method, closed);
   }

   bool emptyStructure = false;
   bool customDispatcher = false;
   bool withAbstractMethods = false;
   _logic->validateClassDeclaration(*scope.moduleScope, _errorProcessor, scope.info, emptyStructure, customDispatcher, withAbstractMethods);
   if (withAbstractMethods)
      scope.raiseError(errAbstractMethods, node);
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

void Compiler :: resolveClassPostfixes(ClassScope& scope, SyntaxNode baseNode, bool extensionMode)
{
   ref_t parentRef = 0;

   // analizing class postfixes : if it is a parrent, template or inline template
   SyntaxNode parentNode = {};
   while (baseNode == SyntaxKey::Parent) {
      SyntaxNode current = baseNode.firstChild();
      if (current == SyntaxKey::TemplatePostfix) {
         if (!parentRef) {
            if (!importInlineTemplate(scope, current, INLINE_PREFIX, baseNode.parentNode())) {
               parentNode = baseNode;

               parentRef = resolveStrongTypeAttribute(scope, current.firstChild(), false);
            }
         }
         else {
            if (!importInlineTemplate(scope, current, INLINE_PREFIX, baseNode.parentNode())) {
               if (!importTemplate(scope, current, baseNode.parentNode()))
                  scope.raiseError(errUnknownTemplate, baseNode);
            }
         }
      }
      else if (!parentRef) {
         parentNode = baseNode;

         parentRef = resolveStrongTypeAttribute(scope, baseNode.firstChild(), false);
      }
      else scope.raiseError(errInvalidSyntax, baseNode);

      baseNode = baseNode.nextNode();
   }

   if (scope.info.header.parentRef == scope.reference) {
      // if it is a super class
      if (parentRef != 0) {
         scope.raiseError(errInvalidSyntax, parentNode);
      }
   }
   else if (parentRef == 0) {
      parentRef = scope.info.header.parentRef;
   }

   if (extensionMode) {
      //COMPLIER MAGIC : treat the parent declaration in the special way for the extension
      scope.extensionClassRef = parentRef;

      declareClassParent(scope.moduleScope->buildins.superReference, scope, parentNode);
   }
   else declareClassParent(parentRef, scope, parentNode);
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
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            if(!importInlineTemplate(scope, current, INLINE_PREFIX, node))
               scope.raiseError(errUnknownTemplate, current);

            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope, Scope::ScopeLevel::Namespace);

            evalStatement(metaScope, current);
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareFieldMetaInfo(FieldScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            if (!importPropertyTemplate(scope, current, INLINE_PROPERTY_PREFIX, node)) {
               if (!importInlineTemplate(scope, current, INLINE_PROPERTY_PREFIX, node))
                  scope.raiseError(errUnknownTemplate, node);
            }
            break;
         case SyntaxKey::InlineImplicitTemplate:
            if (!importPropertyTemplate(scope, current, INLINE_PROPERTY_PREFIX, 
               node))
            {
               scope.raiseError(errUnknownTemplate, node);
            }
            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope, Scope::ScopeLevel::Field);

            evalStatement(metaScope, current);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(scope, current, Visibility::Public, Scope::ScopeLevel::Field);
            break;
         case SyntaxKey::Name:
         case SyntaxKey::Type:
         case SyntaxKey::ArrayType:
         case SyntaxKey::Attribute:
         case SyntaxKey::Dimension:
         case SyntaxKey::EOP:
            break;
         default:
            scope.raiseError(errInvalidSyntax, node);
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
            if(!importInlineTemplate(scope, current, INLINE_PREFIX, node))
               scope.raiseError(errUnknownTemplate, node);

            break;
         case SyntaxKey::IncludeStatement:
            if (withoutBody) {
               noBodyNode.setKey(SyntaxKey::Importing);
               importCode(scope, current, noBodyNode);
            }
            else scope.raiseError(errInvalidSyntax, node);

            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope, Scope::ScopeLevel::Method);

            evalStatement(metaScope, current);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(scope, current, Visibility::Public, Scope::ScopeLevel::Method);
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
         case SyntaxKey::ResendDispatch:
         case SyntaxKey::Redirect:
         case SyntaxKey::identifier:
         case SyntaxKey::SourcePath:
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

void Compiler :: declareParameter(MethodScope& scope, SyntaxNode current, bool withoutWeakMessages, 
   bool declarationMode, bool& variadicMode, bool& weakSignature,
   pos_t& paramCount, ref_t* signature, size_t& signatureLen)
{
   int index = 1 + scope.parameters.count();

   SizeInfo sizeInfo = {};
   TypeInfo paramTypeInfo = {};
   declareArgumentAttributes(scope, current, paramTypeInfo, declarationMode);

   if (withoutWeakMessages && !paramTypeInfo.typeRef)
      paramTypeInfo.typeRef = scope.moduleScope->buildins.superReference;

   if (paramTypeInfo.typeRef)
      weakSignature = false;

   ustr_t terminal = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (scope.parameters.exist(terminal))
      scope.raiseError(errDuplicatedLocal, current);

   paramCount++;
   if (paramCount >= ARG_COUNT || variadicMode)
      scope.raiseError(errTooManyParameters, current);

   if (paramTypeInfo.typeRef == V_ARGARRAY)
      variadicMode = true;

   if (paramTypeInfo.isPrimitive()) {
      // primitive arguments should be replaced with wrapper classes
      signature[signatureLen++] = resolvePrimitiveType(scope, paramTypeInfo, declarationMode);
   }
   else signature[signatureLen++] = paramTypeInfo.typeRef;

   if (signature[signatureLen - 1])
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, signature[signatureLen - 1]);

   scope.parameters.add(terminal, Parameter(index, paramTypeInfo, sizeInfo.size));
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

   bool constantConversion = false;
   bool weakSignature = true;
   bool variadicMode = false;
   pos_t paramCount = 0;
   if (scope.checkHint(MethodHint::Extension)) {
      ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

      // COMPILER MAGIC : for an extension method, self is a parameter
      paramCount++;

      signature[0] = classScope->extensionClassRef;
      signatureLen++;

      weakSignature = false;

      scope.parameters.add(*scope.moduleScope->selfVar, { 1, { signature[0] } });

      flags |= FUNCTION_MESSAGE;
   }

   //bool noSignature = true; // NOTE : is similar to weakSignature except if withoutWeakMessages=true
   // if method has an argument list
   SyntaxNode current = node.findChild(SyntaxKey::Parameter);
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         declareParameter(scope, current, withoutWeakMessages, 
            declarationMode, variadicMode, weakSignature, 
            paramCount, signature, signatureLen);
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
         else if (paramCount == 1 && !unnamedMessage && signature[0] == scope.moduleScope->buildins.literalReference) {
            constantConversion = true;

            actionStr.append(CONSTRUCTOR_MESSAGE);
            flags |= FUNCTION_MESSAGE;
            scope.info.hints |= (ref_t)MethodHint::Constructor;
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

      if (variadicMode)
         flags |= VARIADIC_MESSAGE;

      if (scope.checkHint(MethodHint::RetOverload)) {
         if (!scope.info.outputRef || (weakSignature && paramCount > 0))
            scope.raiseError(errIllegalMethod, node);

         // COMPILER MAGIC : if the message is marked as multiret - it turns into byref return handler
         // NOTE : it should contain the specific return type
         TypeInfo refType = { V_WRAPPER, scope.info.outputRef };

         int offset = scope.parameters.count() + 1u;
         scope.parameters.add(RETVAL_ARG, { offset, refType, 0 });

         signature[signatureLen++] = resolvePrimitiveType(scope, refType, true);
         paramCount++;

         scope.info.hints |= (ref_t)MethodHint::VirtualReturn;
      }

      if (scope.checkHint(MethodHint::Internal)) {
         actionStr.insert("$$", 0);
         actionStr.insert(scope.module->name(), 0);
      }
      else if (scope.checkHint(MethodHint::Protected)) {
         if (actionStr.compare(CONSTRUCTOR_MESSAGE) && paramCount == 0) {
            //HOTFIX : protected default constructor has a special name
            actionStr.copy(CONSTRUCTOR_MESSAGE2);
         }
         else {
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

      // if it is an explicit constant conversion
      if (constantConversion) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         addExtensionMessage(scope, scope.message, scope.getClassRef(), scope.message, 
            scope.getClassVisibility() != Visibility::Public);
      }
   }
}

ref_t Compiler :: declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode)
{
   IdentifierString messageStr;
   pos_t paramCount = 0;
   ref_t signRef = 0;

   bool weakSingature = true;
   bool variadicMode = false;
   ref_t flags = FUNCTION_MESSAGE;
   ref_t signatures[ARG_COUNT];
   size_t signatureLen = 0;
   while (argNode == SyntaxKey::Parameter) {
      declareParameter(methodScope, argNode, false, false, 
         variadicMode, weakSingature,
         paramCount, signatures, signatureLen);

      if (variadicMode)
         flags |= VARIADIC_MESSAGE;

      argNode = argNode.nextNode();
   }

   messageStr.copy(INVOKE_MESSAGE);
   if (!weakSingature) {
      signRef = methodScope.module->mapSignature(signatures, signatureLen, false);
   }

   ref_t actionRef = methodScope.moduleScope->module->mapAction(*messageStr, signRef, false);

   return encodeMessage(actionRef, paramCount, flags);
}

void Compiler :: declareClosureMessage(MethodScope& methodScope, SyntaxNode node)
{
   ref_t invokeAction = methodScope.module->mapAction(INVOKE_MESSAGE, 0, false);
   methodScope.message = encodeMessage(invokeAction, 0, FUNCTION_MESSAGE);
   methodScope.closureMode = true;

   SyntaxNode argNode = node.findChild(SyntaxKey::Parameter);
   if (argNode != SyntaxKey::None)
      methodScope.message = declareClosureParameters(methodScope, argNode);
}

void Compiler :: declareMethod(MethodScope& methodScope, SyntaxNode node, bool abstractMode)
{
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
      methodScope.info.outputRef = methodScope.getClassRef();
   }
   else if (methodScope.checkHint(MethodHint::Predefined)) {
      node.setKey(SyntaxKey::PredefinedMethod);
   }
   else methodScope.info.hints |= (ref_t)MethodHint::Normal;

   if (methodScope.info.outputRef)
      node.appendChild(SyntaxKey::OutputType, methodScope.info.outputRef);

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
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope, Scope::ScopeLevel::Class);

            evalStatement(metaScope, current);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(scope, current, Visibility::Public, Scope::ScopeLevel::Class);
            break;
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
               else if (getArgCount(methodScope.message) == 0 && methodScope.checkHint(MethodHint::Protected)) {
                  // check if it is protected default constructor
                  ref_t dummy = 0;
                  ustr_t actionName = scope.module->resolveAction(getAction(methodScope.message), dummy);
                  if (actionName.endsWith(CONSTRUCTOR_MESSAGE2))
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

void Compiler :: declareFieldMetaInfos(ClassScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Field) {
         IdentifierString fieldName(current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier());

         FieldScope fieldScope(&scope, *fieldName);

         declareFieldMetaInfo(fieldScope, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareClass(ClassScope& scope, SyntaxNode node)
{
   bool extensionDeclaration = isExtensionDeclaration(node);
   resolveClassPostfixes(scope, node.findChild(SyntaxKey::Parent), extensionDeclaration/*, lxParent*/ );

   ref_t declaredFlags = 0;
   declareClassAttributes(scope, node, declaredFlags);

   // NOTE : due to implementation the field meta information should be analyzed before
   // declaring VMT
   declareFieldMetaInfos(scope, node);

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
         injectDefaultConstructor(scope, node, withConstructors);
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
            MetaScope scope(&ns, Scope::ScopeLevel::Namespace);

            evalStatement(scope, current);
            break;
         }
         case SyntaxKey::ReloadStatement:
         {
            IdentifierString dictionaryName(
               FORWARD_PREFIX_NS,
               META_PREFIX,
               current.firstChild(SyntaxKey::TerminalMask).identifier());

            reloadMetaData(ns.moduleScope, *dictionaryName);
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
            if (current.arg.reference == INVALID_REF) {
               // if it is a template based class - its name was already resolved
               classScope.reference = current.findChild(SyntaxKey::Name).arg.reference;
            }
            else classScope.reference = mapNewTerminal(classScope, nullptr, 
               name, nullptr, classScope.visibility);

            classScope.module->mapSection(classScope.reference | mskSymbolRef, false);

            current.setArgumentReference(classScope.reference);
            break;
         }
      case SyntaxKey::MetaDictionary:
            declareDictionary(ns, current, Visibility::Public, Scope::ScopeLevel::Namespace);
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
   argumentRefs[0] = loperand.typeInfo.typeRef;
   arguments.add(loperand);

   if (argCount >= 2) {
      argumentRefs[1] = roperand.typeInfo.typeRef;
      arguments.add(roperand);
   }

   if (argCount == 3) {
      argumentRefs[2] = ioperand.typeInfo.typeRef;
      arguments.add(ioperand);
   }

   ref_t outputRef = 0;
   BuildKey opKey = _logic->resolveOp(*scope.moduleScope, operator_id, argumentRefs, argCount, outputRef);

   ObjectInfo retVal = loperand;
   if (!interpreter.eval(opKey, operator_id, arguments, retVal)) {
      scope.raiseError(errCannotEval, node);
   }

   return retVal;
}

ObjectInfo Compiler :: evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node)
{
   EAttrs mode = ExpressionAttribute::Meta;

   SyntaxNode terminalNode = node.lastChild(SyntaxKey::TerminalMask);

   TypeInfo declaredTypeInfo = {};
   declareExpressionAttributes(scope, node, declaredTypeInfo, mode);

   return mapTerminal(scope, terminalNode, declaredTypeInfo, mode.attrs);
}

ObjectInfo Compiler :: evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool resolveMode)
{
   ObjectInfo retVal = {};

   switch (node.key) {
      case SyntaxKey::Expression:
         retVal = evalExpression(interpreter, scope, node.firstChild(SyntaxKey::DeclarationMask), resolveMode);
         break;
      case SyntaxKey::AssignOperation:
      case SyntaxKey::AddAssignOperation:
      case SyntaxKey::NameOperation:
         retVal = evalOperation(interpreter, scope, node, (int)node.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::Object:
         retVal = evalObject(interpreter, scope, node);
         break;
      default:
         scope.raiseError(errCannotEval, node);
         break;
   }

   if (resolveMode) {
      switch (retVal.kind) {
         case ObjectKind::SelfName:
         {
            ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);
            if (classScope != nullptr) {
               ustr_t name = scope.module->resolveReference(classScope->reference);

               retVal = interpreter.mapStringConstant(name);
            }
            break;
         }
         case ObjectKind::MethodName:
         {
            MethodScope* methodScope = Scope::getScope<MethodScope>(scope, Scope::ScopeLevel::Method);
            if (methodScope) {
               IdentifierString methodName;
               ByteCodeUtil::resolveMessageName(methodName, scope.module, methodScope->message);

               retVal = interpreter.mapStringConstant(*methodName);
            }
            else retVal = {};
            break;
         }
         case ObjectKind::FieldName:
            retVal.kind = ObjectKind::StringLiteral;
            break;
         default:
            break;
      }
   }

   return retVal;
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
   return retVal.kind == ObjectKind::Object || retVal.kind == ObjectKind::Extern || retVal.kind == ObjectKind::Symbol;
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
      writer.newNode(BuildKey::CopyingToAcc, offset);
      writer.appendNode(BuildKey::Size, info.size);
   }
   else writer.newNode(BuildKey::FieldAssigning, 0);

   writer.closeNode();
}

ObjectInfo Compiler :: boxRefArgumentInPlace(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, ref_t targetRef)
{
   ref_t typeRef = targetRef;
   if (!typeRef)
      typeRef = retrieveStrongType(scope, info);

   ObjectInfo tempLocal = declareTempLocal(scope, typeRef);
   tempLocal.mode = TargetMode::RefUnboxingRequired;

   info.kind = ObjectKind::Local;
   compileAssigningOp(writer, scope, tempLocal, info);

   tempLocal.kind = ObjectKind::LocalReference;

   return tempLocal;
}

ObjectInfo Compiler :: boxArgumentInPlace(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, ref_t targetRef)
{
   ref_t typeRef = targetRef;
   if (!typeRef)
      typeRef = retrieveStrongType(scope, info);

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

   if (!_logic->isReadOnly(argInfo))
      tempLocal.mode = TargetMode::UnboxingRequired;

   return tempLocal;
}

inline bool isBoxingRequired(ObjectInfo info)
{
   switch (info.kind) {
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
      case ObjectKind::ParamAddress:
      case ObjectKind::ByRefParamAddress:
      case ObjectKind::SelfBoxableLocal:
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
            retVal.typeInfo = info.typeInfo;

            return retVal;
         }
         else {
            // allocating temporal variable
            ObjectInfo tempLocal = declareTempLocal(scope, info.typeInfo.typeRef, false);

            writeObjectInfo(writer, scope, tempLocal);
            writer.appendNode(BuildKey::SavingInStack, 0);

            writeObjectInfo(writer, scope, scope.mapSelf());
            writer.newNode(BuildKey::AccFieldCopyingTo, info.reference);
            writer.appendNode(BuildKey::Size, tempLocal.extra);
            writer.closeNode();

            return tempLocal;
         }
      default:
         return info;
   }
}

ObjectInfo Compiler :: boxArgument(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, 
   bool stackSafe, bool boxInPlace, bool allowingRefArg, ref_t targetRef)
{
   ObjectInfo retVal = { ObjectKind::Unknown };

   info = boxArgumentLocally(writer, scope, info, boxInPlace);

   if (!stackSafe && isBoxingRequired(info)) {
      ObjectKey key = { info.kind, info.reference };

      if (!boxInPlace)
         retVal = scope.tempLocals.get(key);

      if (retVal.kind == ObjectKind::Unknown) {
         retVal = boxArgumentInPlace(writer, scope, info, targetRef);

         if (!boxInPlace)
            scope.tempLocals.add(key, retVal);
      }
   }
   else if (info.kind == ObjectKind::RefLocal) {
      ObjectKey key = { info.kind, info.reference };

      if (!boxInPlace)
         retVal = scope.tempLocals.get(key);

      if (retVal.kind == ObjectKind::Unknown) {
         if (!allowingRefArg) {
            info.kind = ObjectKind::Local;

            retVal = boxArgumentInPlace(writer, scope, info, targetRef);
         }
         else retVal = boxRefArgumentInPlace(writer, scope, info, targetRef);

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
      case ObjectKind::Float64Literal:
         writer.appendNode(BuildKey::RealLiteral, info.reference);
         break;
      case ObjectKind::LongLiteral:
         writer.appendNode(BuildKey::LongLiteral, info.reference);
         break;
      case ObjectKind::StringLiteral:
         writer.appendNode(BuildKey::StringLiteral, info.reference);
         break;
      case ObjectKind::WideStringLiteral:
         writer.appendNode(BuildKey::WideStringLiteral, info.reference);
         break;
      case ObjectKind::CharacterLiteral:
         writer.appendNode(BuildKey::CharLiteral, info.reference);
         break;
      case ObjectKind::MssgLiteral:
         writer.appendNode(BuildKey::MssgLiteral, info.reference);
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
      case ObjectKind::SuperLocal:
      case ObjectKind::ReadOnlySelfLocal:
      case ObjectKind::Local:
      case ObjectKind::TempLocal:
      case ObjectKind::ParamAddress:
      case ObjectKind::SelfBoxableLocal:
      case ObjectKind::ByRefParamAddress:
         writer.appendNode(BuildKey::Local, info.reference);
         break;
      case ObjectKind::LocalReference:
         writer.appendNode(BuildKey::LocalReference, info.reference);
         break;
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
         writer.appendNode(BuildKey::LocalAddress, info.reference);
         break;
      case ObjectKind::Field:
      case ObjectKind::Outer:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      //case ObjectKind::OuterField:
      //   writeObjectInfo(writer, scope, scope.mapSelf());
      //   writer.appendNode(BuildKey::Field, info.reference);
      //   break;
      case ObjectKind::StaticConstField:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::ClassOp, CLASS_OPERATOR_ID);
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::StaticField:
         writer.appendNode(BuildKey::StaticVar, info.reference);
         break;
      case ObjectKind::ByRefParam:
         writeObjectInfo(writer, scope, { ObjectKind::Param, info.typeInfo, info.reference });
         writer.appendNode(BuildKey::Field);
         break;
      case ObjectKind::Object:
         break;
      default:
         throw InternalError(errFatalError);
   }
}

ref_t Compiler :: retrieveStrongType(Scope& scope, ObjectInfo info)
{
   if (info.typeInfo.isPrimitive()) {
      return resolvePrimitiveType(scope, info.typeInfo, false);
   }
   else return info.typeInfo.typeRef;
}

ref_t Compiler :: retrieveType(Scope& scope, ObjectInfo info)
{
   if (info.typeInfo.isPrimitive() && info.typeInfo.elementRef) {
      return retrieveStrongType(scope, info);
   }
   else return info.typeInfo.typeRef;
}

ref_t Compiler :: resolvePrimitiveType(Scope& scope, TypeInfo typeInfo, bool declarationMode)
{
   switch (typeInfo.typeRef) {
      case V_INT8:
         return scope.moduleScope->buildins.byteReference;
      case V_INT16:
         return scope.moduleScope->buildins.shortReference;
      case V_INT32:
         return scope.moduleScope->buildins.intReference;
      case V_INT64:
         return scope.moduleScope->buildins.longReference;
      case V_FLOAT64:
         return scope.moduleScope->buildins.realReference;
      case V_WORD32:
         return scope.moduleScope->buildins.dwordReference;
      case V_STRING:
         return scope.moduleScope->buildins.literalReference;
      case V_WIDESTRING:
         return scope.moduleScope->buildins.wideReference;
      case V_MESSAGE:
         return scope.moduleScope->buildins.messageReference;
      case V_FLAG:
         return scope.moduleScope->branchingInfo.typeRef;
      case V_WRAPPER:
         return resolveWrapperTemplate(scope, typeInfo.elementRef, declarationMode);
      case V_INT8ARRAY:
         return resolveArrayTemplate(scope, typeInfo.elementRef, declarationMode);
      case V_NIL:
         return scope.moduleScope->buildins.superReference;
      case V_ARGARRAY:
         return resolveArrayTemplate(scope, typeInfo.elementRef, declarationMode);
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

void Compiler :: declareArgumentAttributes(MethodScope& scope, SyntaxNode node, TypeInfo& typeInfo,
   bool declarationMode)
{
   SyntaxNode current = node.firstChild();
   bool byRefArg = false;
   bool variadicArg = false;
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Type:
            // if it is a type attribute
            typeInfo = resolveTypeAttribute(scope, current, declarationMode, false);
            break;
         case SyntaxKey::ArrayType:
            // if it is a type attribute
            typeInfo = resolveTypeScope(scope, current, variadicArg, declarationMode, false);
            break;
         case SyntaxKey::Attribute:
            if (!_logic->validateArgumentAttribute(current.arg.reference, byRefArg, variadicArg))
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
   if (byRefArg) {
      typeInfo.elementRef = typeInfo.typeRef;
      typeInfo.typeRef = V_WRAPPER;
   }
   else if (variadicArg) {
      if (typeInfo.typeRef != V_OBJARRAY)
         scope.raiseError(errInvalidOperation, node);

      typeInfo.elementRef = typeInfo.elementRef;
      typeInfo.typeRef = V_ARGARRAY;
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
            scope.info.outputRef = resolveStrongTypeAttribute(scope, current, true);
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

void Compiler::saveNamespaceInfo(SyntaxNode node, NamespaceScope* nsScope, bool outerMost)
{
   if (outerMost)
      node.appendChild(SyntaxKey::SourcePath, *nsScope->sourcePath);

   IdentifierString nsFullName(nsScope->module->name());
   if (nsScope->nsName.length() > 0) {
      nsFullName.copy("'");
      nsFullName.append(*nsScope->nsName);
   }
   node.appendChild(SyntaxKey::Import)
      .appendChild(SyntaxKey::Name)
         .appendChild(SyntaxKey::reference, *nsFullName);

   for (auto it = nsScope->importedNs.start(); !it.eof(); ++it) {
      node.appendChild(SyntaxKey::Import)
         .appendChild(SyntaxKey::Name)
            .appendChild(SyntaxKey::reference, *it);
   }

   if (nsScope->parent)
      saveNamespaceInfo(node, (NamespaceScope*)nsScope->parent, false);
}

void Compiler :: declareTemplate(TemplateScope& scope, SyntaxNode& node)
{
   switch (scope.type) {
      case TemplateType::Class:
      case TemplateType::InlineProperty:
      {
         // COMPILER MAGIC : inject imported namespaces & source path
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         saveNamespaceInfo(node, nsScope, true);
         break;
      }
      case TemplateType::Inline:
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

   IdentifierString postfix;
   declareTemplateAttributes(scope, node, postfix);

   int argCount = SyntaxTree::countChild(node, SyntaxKey::TemplateArg);
   postfix.append('#');
   postfix.appendInt(argCount);

   IdentifierString prefix;
   switch (scope.type) {
      case TemplateType::InlineProperty:
         prefix.append(INLINE_PROPERTY_PREFIX);
         break;
      default:
         break;
   }

   SyntaxNode name = node.findChild(SyntaxKey::Name);
   scope.reference = mapNewTerminal(scope, *prefix, name, *postfix, scope.visibility);
   if (scope.module->mapSection(scope.reference | mskSyntaxTreeRef, true))
      scope.raiseError(errDuplicatedDictionary, name.firstChild(SyntaxKey::TerminalMask));

   declareTemplate(scope, node);
}

void Compiler :: declareDictionaryAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, bool& superMode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         if (!_logic->validateDictionaryAttribute(current.arg.value, typeInfo, superMode)) {
            current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }
      else if (current == SyntaxKey::Type) {
         TypeInfo dictTypeInfo = resolveTypeAttribute(scope, current, true, false);
         if (_logic->isCompatible(*scope.moduleScope, dictTypeInfo, { V_STRING }, true)) {
            typeInfo.typeRef = V_DICTIONARY;
            typeInfo.elementRef = V_STRING;
         }
         else scope.raiseError(errInvalidHint, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareExpressionAttributes(Scope& scope, SyntaxNode node, TypeInfo& typeInfo, ExpressionAttributes& mode)
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
               typeInfo = resolveTypeAttribute(scope, current, false, false);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Dimension:
            typeInfo.elementRef = typeInfo.typeRef;
            typeInfo.typeRef = V_OBJARRAY;
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

void Compiler :: validateType(Scope& scope, ref_t typeRef, SyntaxNode node, bool ignoreUndeclared, bool allowRole)
{
   if (!typeRef) {
      switch (node.key) {
         case SyntaxKey::string:
            scope.raiseError(errInvalidSyntax, node);
            break;
         default:
            scope.raiseError(errUnknownClass, node);
            break;
      }
   }

   if (!_logic->isValidType(*scope.moduleScope, typeRef, ignoreUndeclared, allowRole))
      scope.raiseError(errInvalidType, node);
}

ref_t Compiler :: resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type, 
   bool declarationMode, bool allowRole)
{
   ObjectInfo identInfo;

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   if (type == SyntaxKey::reference && isWeakReference(identifier)) {
      identInfo = ns->mapWeakReference(identifier, false);
   }
   else if (type == SyntaxKey::globalreference) {
      identInfo = ns->mapGlobal(identifier, EAttr::None);
   }
   else identInfo = ns->mapIdentifier(identifier, type == SyntaxKey::reference, EAttr::None);

   switch (identInfo.kind) {
      case ObjectKind::Class:
         return identInfo.reference;
      case ObjectKind::Symbol:
         if (declarationMode)
            return identInfo.reference;
      case ObjectKind::Extension:
         if (allowRole)
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
   templateName.append(node.identifier());
   templateName.append('#');
   templateName.appendInt(paramCounter);

   // NOTE : check it in declararion mode - we need only reference
   return resolveTypeIdentifier(scope, *templateName, node.key, true, false);
}

void Compiler :: declareTemplateAttributes(Scope& scope, SyntaxNode node, 
   TemplateTypeList& parameters, bool declarationMode, bool objectMode)
{
   SyntaxNode current = objectMode ? node.nextNode() : node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplateArg) {
         ref_t typeRef = resolveStrongTypeAttribute(scope, current, declarationMode);

         parameters.add(typeRef);
      }

      current = current.nextNode();
   }
}

ref_t Compiler :: defineArrayType(Scope& scope, ref_t elementRef)
{
   return _logic->definePrimitiveArray(*scope.moduleScope, elementRef,
      _logic->isEmbeddable(*scope.moduleScope, elementRef));
}

ObjectInfo Compiler :: defineArrayType(Scope& scope, ObjectInfo info)
{
   ref_t elementRef = info.typeInfo.typeRef;
   ref_t arrayRef = defineArrayType(scope, elementRef);

   info.typeInfo.typeRef = arrayRef;
   info.typeInfo.elementRef = elementRef;

   if (info.mode == TargetMode::Creating)
      info.mode = TargetMode::CreatingArray;

   return info;
}

ref_t Compiler :: resolveTypeTemplate(Scope& scope, SyntaxNode node, bool declarationMode)
{
   TemplateTypeList typeList;
   declareTemplateAttributes(scope, node, typeList, declarationMode, true);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

   ref_t templateRef = mapTemplateType(scope, node);
   if (!templateRef)
      scope.raiseError(errInvalidHint, node);

   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
      templateRef, parameters, declarationMode);
}

ref_t Compiler :: resolveTemplate(Scope& scope, ref_t templateRef, ref_t elementRef, bool declarationMode)
{
   TemplateTypeList typeList;
   typeList.add(elementRef);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
      templateRef, parameters, declarationMode);
}

ref_t Compiler :: resolveClosure(Scope& scope, mssg_t closureMessage, ref_t outputRef)
{
   ref_t signRef = 0;
   scope.module->resolveAction(getAction(closureMessage), signRef);

   int paramCount = getArgCount(closureMessage);

   IdentifierString closureName(scope.module->resolveReference(scope.moduleScope->buildins.closureTemplateReference));
   if (signRef == 0) {
      if (paramCount > 0) {
         closureName.appendInt(paramCount);
      }

      if (isWeakReference(*closureName)) {
         return scope.module->mapReference(*closureName, true);
      }
      else return scope.moduleScope->mapFullReference(*closureName, true);
   }
   else {
      ref_t signatures[ARG_COUNT];
      size_t signLen = scope.module->resolveSignature(signRef, signatures);

      List<SyntaxNode> parameters({});

      // HOTFIX : generate a temporal template to pass the type
      SyntaxTree dummyTree;
      SyntaxTreeWriter dummyWriter(dummyTree);
      dummyWriter.newNode(SyntaxKey::Root);

      for (size_t i = 0; i < signLen; i++) {
         dummyWriter.newNode(SyntaxKey::TemplateArg, signatures[i]);
         dummyWriter.newNode(SyntaxKey::Type);

         ustr_t referenceName = scope.moduleScope->module->resolveReference(signatures[i]);
         if (isWeakReference(referenceName)) {
            dummyWriter.appendNode(SyntaxKey::reference, referenceName);
         }
         else dummyWriter.appendNode(SyntaxKey::globalreference, referenceName);

         dummyWriter.closeNode();
         dummyWriter.closeNode();
      }

      if (outputRef) {
         dummyWriter.newNode(SyntaxKey::TemplateArg, outputRef);
         dummyWriter.newNode(SyntaxKey::Type);

         ustr_t referenceName = scope.moduleScope->module->resolveReference(outputRef);
         if (isWeakReference(referenceName)) {
            dummyWriter.appendNode(SyntaxKey::reference, referenceName);
         }
         else dummyWriter.appendNode(SyntaxKey::globalreference, referenceName);

         dummyWriter.closeNode();
         dummyWriter.closeNode();
      }

      dummyWriter.closeNode();

      SyntaxNode current = dummyTree.readRoot().firstChild();
      while (current == SyntaxKey::TemplateArg) {
         parameters.add(current);

         current = current.nextNode();
      }

      closureName.append('#');
      closureName.appendInt(paramCount + 1);

      ref_t templateReference = 0;
      if (isWeakReference(*closureName)) {
         templateReference = scope.module->mapReference(*closureName, true);
      }
      else templateReference = scope.moduleScope->mapFullReference(*closureName, true);

      NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

      return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
         templateReference, parameters, false);
   }
}

ref_t Compiler :: resolveWrapperTemplate(Scope& scope, ref_t elementRef, bool declarationMode)
{
   return resolveTemplate(scope, scope.moduleScope->buildins.wrapperTemplateReference, elementRef, declarationMode);
}

ref_t Compiler :: resolveArrayTemplate(Scope& scope, ref_t elementRef, bool declarationMode)
{
   return resolveTemplate(scope, scope.moduleScope->buildins.arrayTemplateReference, elementRef, declarationMode);
}

TypeInfo Compiler :: resolveTypeScope(Scope& scope, SyntaxNode node, bool& variadicArg, 
   bool declarationMode, bool allowRole)
{
   ref_t elementRef = 0;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateTypeScopeAttribute(current.arg.reference, variadicArg))
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            break;
         case SyntaxKey::Type:
            elementRef = resolveStrongTypeAttribute(scope, current, declarationMode);
            break;
         default:
            elementRef = resolveTypeIdentifier(scope, node.identifier(), node.key, declarationMode, allowRole);
            break;
      }

      current = current.nextNode();
   }

   if (node == SyntaxKey::ArrayType) {
      return { defineArrayType(scope, elementRef), elementRef };
   }
   else return {};
}

TypeInfo Compiler :: resolveTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode, bool allowRole)
{
   TypeInfo typeInfo = {};
   if (SyntaxTree::test(node.key, SyntaxKey::TerminalMask)) {
      if (node.nextNode() == SyntaxKey::TemplateArg) {
         typeInfo.typeRef = resolveTypeTemplate(scope, node, declarationMode);
      }
      else typeInfo.typeRef = resolveTypeIdentifier(scope, node.identifier(), node.key, declarationMode, allowRole);
   }
   else if (node == SyntaxKey::TemplateArg) {
      typeInfo = resolveTypeAttribute(scope, node.firstChild(), declarationMode, allowRole);
   }
   else if (node == SyntaxKey::ArrayType) {
      bool variadicOne = false;

      typeInfo = resolveTypeScope(scope, node, variadicOne, declarationMode, allowRole);

      if (variadicOne)
         scope.raiseError(errInvalidOperation, node);
   }
   else {
      SyntaxNode current = node.firstChild();
      if (current == SyntaxKey::Object) {
         typeInfo = resolveTypeAttribute(scope, current, declarationMode, allowRole);
      }
      else {
         SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);

         if (terminal.nextNode() == SyntaxKey::TemplateArg) {
            typeInfo.typeRef = resolveTypeTemplate(scope, terminal, declarationMode);
         }
         else typeInfo.typeRef = resolveTypeIdentifier(scope,
            terminal.identifier(), terminal.key, declarationMode, allowRole);
      }
   }

   validateType(scope, typeInfo.typeRef, node, declarationMode, allowRole);

   return typeInfo;
}

ref_t Compiler :: resolveStrongTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode)
{
   TypeInfo typeInfo = resolveTypeAttribute(scope, node, declarationMode, false);

   if (isPrimitiveRef(typeInfo.typeRef)) {
      return resolvePrimitiveType(scope, typeInfo, declarationMode);
   }
   else return typeInfo.typeRef;
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
            if (!attrs.typeInfo.typeRef) {
               attrs.typeInfo = resolveTypeAttribute(scope, current, true, false);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Dimension:
            if (!attrs.size && attrs.typeInfo.typeRef) {
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
   if (attrs.typeInfo.isPrimitive()) {
      bool valid = true;
      switch (attrs.typeInfo.typeRef) {
         case V_INTBINARY:
            switch (attrs.size) {
               case 1:
                  attrs.typeInfo.typeRef = V_INT8;
                  break;
               case 2:
                  attrs.typeInfo.typeRef = V_INT16;
                  break;
               case 4:
                  attrs.typeInfo.typeRef = V_INT32;
                  break;
               case 8:
                  attrs.typeInfo.typeRef = V_INT64;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_WORDBINARY:
            switch (attrs.size) {
               case 4:
                  attrs.typeInfo.typeRef = V_WORD32;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_MSSGBINARY:
            switch (attrs.size) {
               case 4:
                  attrs.typeInfo.typeRef = V_MESSAGE;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_FLOATBINARY:
            switch (attrs.size) {
               case 8:
                  attrs.typeInfo.typeRef = V_FLOAT64;
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

inline int newLocalAddr(int disp, int allocated)
{
   return -disp - allocated;
}

int Compiler :: allocateLocalAddress(CodeScope* codeScope, int size, bool binaryArray)
{
   if (binaryArray)
      codeScope->allocLocalAddress(4);

   int retVal = codeScope->allocLocalAddress(size);

   return newLocalAddr(sizeof(intptr_t), retVal);
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

void Compiler :: declareVariable(Scope& scope, SyntaxNode terminal, TypeInfo typeInfo)
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
   variable.typeInfo = typeInfo;
   variable.kind = ObjectKind::Local;

   if (size != 0 && variable.typeInfo.typeRef != 0) {
      if (!variable.typeInfo.isPrimitive()) {
         // if it is a primitive array
         variable.typeInfo.elementRef = variable.typeInfo.typeRef;
         variable.typeInfo.typeRef = _logic->definePrimitiveArray(*scope.moduleScope, variable.typeInfo.elementRef, true);
      }
      else scope.raiseError(errInvalidHint, terminal);
   }

   ClassInfo localInfo;
   //bool binaryArray = false;
   if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.typeInfo.typeRef))
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
      codeScope->mapNewLocal(*identifier, variable.reference, variable.typeInfo, 
         size, true);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
}

bool Compiler :: evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo)
{
   Interpreter interpreter(scope.moduleScope, _logic);
   MetaScope metaScope(&scope, Scope::ScopeLevel::Class);

   ObjectInfo retVal = evalExpression(interpreter, metaScope, node, false);
   bool setIndex = false;
   switch (retVal.kind) {
      case ObjectKind::SelfName:
         constInfo.typeInfo = { V_STRING };
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
      retVal.typeInfo = { info.header.classRef };

      return retVal;
   }
   else return {};
}

ExternalInfo Compiler :: mapExternal(Scope& scope, SyntaxNode node)
{
   SyntaxNode objNode = node.parentNode();

   ustr_t dllAlias = node.identifier();
   ustr_t functionName = SyntaxTree::gotoNode(objNode, SyntaxKey::Message).firstChild(SyntaxKey::TerminalMask).identifier();

   if (functionName.empty()) {
      functionName = dllAlias;
      dllAlias = RT_FORWARD;
   }

   return scope.moduleScope->mapExternal(dllAlias, functionName);
}

ObjectInfo Compiler :: compileExternalOp(BuildTreeWriter& writer, ExprScope& scope, ref_t externalRef,
   bool stdCall, ArgumentsInfo& arguments)
{
   pos_t count = arguments.count_pos();

   writer.appendNode(BuildKey::Allocating, align(count, scope.moduleScope->stackAlingment));

   for (pos_t i = count; i > 0; i--) {
      ObjectInfo arg = boxArgumentLocally(writer, scope, arguments[i - 1], true);

      writeObjectInfo(writer, scope, arg);
      switch (arg.kind) {
         case ObjectKind::IntLiteral:
            writer.appendNode(BuildKey::SavingNInStack, i - 1);
            break;
         default:
            if (_logic->isCompatible(*scope.moduleScope, { V_INT32 },
               arg.typeInfo, true)) 
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

   return { ObjectKind::Extern, { V_INT32 }, 0 };
}

mssg_t Compiler :: resolveOperatorMessage(ModuleScopeBase* scope, int operatorId)
{
   switch (operatorId) {
      case INDEX_OPERATOR_ID:
         return scope->buildins.refer_message;
      case ADD_OPERATOR_ID:
         return scope->buildins.add_message;
      case SUB_OPERATOR_ID:
         return scope->buildins.sub_message;
      case MUL_OPERATOR_ID:
         return scope->buildins.mul_message;
      case DIV_OPERATOR_ID:
         return scope->buildins.div_message;
      case BAND_OPERATOR_ID:
         return scope->buildins.band_message;
      case BOR_OPERATOR_ID:
         return scope->buildins.bor_message;
      case BXOR_OPERATOR_ID:
         return scope->buildins.bxor_message;
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
      case GREATER_OPERATOR_ID:
         return scope->buildins.greater_message;
      case NOT_OPERATOR_ID:
         return scope->buildins.not_message;
      case NOTLESS_OPERATOR_ID:
         return scope->buildins.notless_message;
      case NOTGREATER_OPERATOR_ID:
         return scope->buildins.notgreater_message;
      case NEGATE_OPERATOR_ID:
         return scope->buildins.negate_message;
      case VALUE_OPERATOR_ID:
         return scope->buildins.value_message;
      default:
         throw InternalError(errFatalError);
   }
}

ObjectInfo Compiler :: declareTempStructure(ExprScope& scope, SizeInfo sizeInfo)
{
   if (sizeInfo.size <= 0)
      return {};

   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

   ObjectInfo retVal = { ObjectKind::TempLocalAddress };
   retVal.reference = allocateLocalAddress(codeScope, sizeInfo.size, false);
   retVal.extra = sizeInfo.size;

   scope.syncStack();

   return retVal;
}

ObjectInfo Compiler :: allocateResult(ExprScope& scope, ref_t resultRef)
{
   SizeInfo info = _logic->defineStructSize(*scope.moduleScope, resultRef);
   if (info.size > 0) {
      ObjectInfo retVal = declareTempStructure(scope, info);
      retVal.typeInfo.typeRef = resultRef;

      return retVal;
   }
   else throw InternalError(errFatalError);

   return {}; // NOTE : should never be reached
}

ObjectInfo Compiler :: compileWeakOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ref_t* arguments, pos_t argLen,
   ObjectInfo& loperand, ArgumentsInfo& messageArguments, mssg_t message, ref_t expectedRef)
{
   ObjectInfo retVal =  {};

   // resolving a message signature (excluding a target)
   for (pos_t i = 1; i < argLen; i++) {
      if (isPrimitiveRef(arguments[i])) {
         arguments[i - 1] = resolvePrimitiveType(scope, { arguments[i] }, false);
      }
      else arguments[i - 1] = arguments[i];
   }

   ref_t signRef = argLen > 1 ? scope.module->mapSignature(arguments, argLen - 1, false) : 0;

   mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, loperand), expectedRef, message, signRef);
   if (byRefHandler) {
      ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

      addByRefRetVal(messageArguments, tempRetVal);

      compileMessageOperation(writer, scope, node, loperand, byRefHandler,
         signRef, messageArguments, EAttr::AlreadyResolved);

      retVal = tempRetVal;
   }
   else retVal = compileMessageOperation(writer, scope, node, loperand, message,
      signRef, messageArguments, EAttr::NoExtension);

   return retVal;
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, SyntaxNode rnode, int operatorId, ref_t expectedRef)
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
   arguments[0] = loperand.typeInfo.typeRef;

   // HOTFIX : typecast the right-hand expression if required
   if (rnode != SyntaxKey::None) {
      ref_t rTargetRef = 0;
      if (operatorId == SET_OPERATOR_ID)
         rTargetRef = retrieveType(scope, loperand);

      roperand = compileExpression(writer, scope, rnode, rTargetRef, EAttr::Parameter);

      arguments[argLen++] = retrieveType(scope, roperand);
   }

   if (inode != SyntaxKey::None) {
      ioperand = compileExpression(writer, scope, inode, 0, EAttr::Parameter);
      arguments[argLen++] = retrieveType(scope, ioperand);
   }

   ref_t outputRef = 0;
   bool  needToAlloc = false;
   op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef);

   if (op != BuildKey::None) {
      if (outputRef == V_ELEMENT) {
         outputRef = loperand.typeInfo.elementRef;
      }

      if (op == BuildKey::NilCondOp) {
         // NOTE : the nil operation need only one (not nil) operand
         if (loperand.typeInfo.typeRef == V_NIL) {
            loperand = roperand;
         }

         rnode = {};
      }
      else if (outputRef && _logic->isEmbeddable(*scope.moduleScope, outputRef))
         needToAlloc = true;

      if (needToAlloc) {
         retVal = allocateResult(scope, outputRef);
      }
      else retVal = { ObjectKind::Object, { outputRef }, 0 };

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
         writeObjectInfo(writer, scope, ioperand);

      writer.newNode(op, operatorId);

      // check if the operation requires an extra arguments
      if (needToAlloc) {
         writer.appendNode(BuildKey::Index, retVal.argument);
      }

      switch (op) {
         case BuildKey::BoolSOp:
         case BuildKey::IntCondOp:
         case BuildKey::ByteCondOp:
         case BuildKey::ShortCondOp:
         case BuildKey::LongCondOp:
         case BuildKey::RealCondOp:
         case BuildKey::NilCondOp:
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

      retVal = compileWeakOperation(writer, scope, node, arguments, argLen, loperand, 
         messageArguments, message, expectedRef);
   }

   return retVal;
}

mssg_t Compiler :: mapMessage(ExprScope& scope, SyntaxNode current, bool propertyMode, 
   bool extensionMode, bool probeMode)
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

   if (argCount >= ARG_COUNT) {
      flags |= VARIADIC_MESSAGE;
      argCount = 2;
   }

   if (messageStr.empty()) {
      flags |= FUNCTION_MESSAGE;

      // if it is an implicit message
      messageStr.copy(probeMode ? TRY_INVOKE_MESSAGE : INVOKE_MESSAGE);
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

      methods.add(extInfo.value2, { false, 0, 0, genericMessage | FUNCTION_MESSAGE, 0 });
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
      0, outputRef, Visibility::Public);

   classWriter.closeNode();
   classWriter.closeNode();

   generateMethodDeclaration(classScope, classNode.findChild(SyntaxKey::Method), false);
   classScope.save();

   writer.newNode(BuildKey::NestedClass, extRef);
   compileVMT(writer, classScope, classNode);
   writer.closeNode();

   return extRef;
}

ref_t Compiler :: compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current, 
   ArgumentsInfo& arguments, EAttr mode)
{
   EAttr paramMode = EAttr::Parameter;
   if (EAttrs::testAndExclude(mode, EAttr::NoPrimitives))
      paramMode = paramMode | EAttr::NoPrimitives;

   // compile the message argument list
   ref_t signatures[ARG_COUNT] = { 0 };
   ref_t signatureLen = 0;
   ref_t superReference = scope.moduleScope->buildins.superReference;

   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         auto argInfo = compileExpression(writer, scope, current, 0, 
            paramMode);
         ref_t argRef = retrieveStrongType(scope, argInfo);
         if (signatureLen >= ARG_COUNT) {
            signatureLen++;
         }
         else if (argRef) {
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

   ref_t objectRef = retrieveStrongType(scope, object);

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
         /*ref_t targetRef = */nsScope->resolveExtensionTarget(extInfo.value1);
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
   ref_t implicitSignatureRef, bool ignoreExtensions, ref_t& resolvedExtensionRef, int& stackSafeAttr)
{
   mssg_t resolvedMessage = 0;
   ref_t targetRef = retrieveStrongType(scope, target);

   // try to resolve the message as is
   int resolvedStackSafeAttr = 0;
   resolvedMessage = _logic->resolveMultimethod(*scope.moduleScope, weakMessage, targetRef,
      implicitSignatureRef, resolvedStackSafeAttr, isSelfCall(target));
   if (resolvedMessage != 0) {
      stackSafeAttr = resolvedStackSafeAttr;

      // if the object handles the compile-time resolved message - use it
      return resolvedMessage;
   }

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

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo target,
   mssg_t weakMessage, ref_t implicitSignatureRef, ArgumentsInfo& arguments, ExpressionAttributes mode)
{
   ObjectInfo retVal(ObjectKind::Object);

   BuildKey operation = BuildKey::CallOp;
   ref_t resolvedExtensionRef = 0;
   int stackSafeAttr = 0;
   mssg_t message = 0;
   if (EAttrs::testAndExclude(mode.attrs, EAttr::AlreadyResolved)) {
      message = weakMessage;

      _logic->setSignatureStacksafe(*scope.moduleScope, implicitSignatureRef, stackSafeAttr);
   }
   else message = resolveMessageAtCompileTime(writer, target, scope, weakMessage,
      implicitSignatureRef, 
      EAttrs::testAndExclude(mode.attrs, EAttr::NoExtension), 
      resolvedExtensionRef, stackSafeAttr);

   if (resolvedExtensionRef) {
      // if extension was found - make it a operation target
      target = { ObjectKind::ConstantRole, { resolvedExtensionRef }, resolvedExtensionRef };
   }

   stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

   ref_t targetRef = retrieveStrongType(scope, target);

   CheckMethodResult result = {};
   bool found = _logic->resolveCallType(*scope.moduleScope, targetRef, message, result);
   if (found) {
      switch (result.visibility) {
         case Visibility::Private:
            if (isSelfCall(target)) {
               message = result.message;
            }
            else found = false;
            break;
         case Visibility::Protected:
            if (isSelfCall(target) || target.kind == ObjectKind::SuperLocal) {
               message = result.message;
            }
            else found = false;
            break;
         case Visibility::Internal:
            if (scope.moduleScope->isInternalOp(targetRef)) {
               message = result.message;
            }
            else found = false;
            break;
         default:
            break;
      }
   }

   if (found) {
      retVal.typeInfo = { result.outputRef };
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
      if (operation != BuildKey::CallOp) {
         // if the method directly resolved and the target is not required to be dynamic, mark it as stacksafe
         if (result.stackSafe)
            stackSafeAttr |= 1;
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
         bool weakTarget = targetRef == scope.moduleScope->buildins.superReference;

         // treat it as a weak reference
         targetRef = 0;

         SyntaxNode messageNode = node.findChild(SyntaxKey::Message);
         if (weakTarget/* || ignoreWarning*/) {
            // ignore warning for super class / type-less one
         }
         else if (messageNode == SyntaxKey::None) {
            if (test(message, CONVERSION_MESSAGE)) {
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownTypecast, node);
            }
            else if (message == scope.moduleScope->buildins.refer_message) {
               scope.raiseWarning(WARNING_LEVEL_1, wrnUnsupportedOperator, node);
            }
            else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownFunction, node);
         }
         else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownMessage, messageNode);
      }
   }

   if (target.kind == ObjectKind::SuperLocal) {
      if (found) {
         // parent methods are always sealed
         operation = BuildKey::DirectCallOp;
      }
      else scope.raiseError(errInvalidOperation, node);
   }

   if (operation != BuildKey::None) {
      bool targetOverridden = (target != arguments[0]);
      if (targetOverridden) {
         target = boxArgument(writer, scope, target, result.stackSafe, false, false);
      }

      if (!found)
         stackSafeAttr = 0;

      pos_t counter = arguments.count_pos();
      // box the arguments if required
      int argMask = 1;
      for (unsigned int i = 0; i < counter; i++) {
         // NOTE : byref dynamic arg can be passed semi-directly (via temporal variable) if the method resolved directly
         ObjectInfo arg = boxArgument(writer, scope, arguments[i], 
            test(stackSafeAttr, argMask), false, found);

         arguments[i] = arg;
         argMask <<= 1;
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

      // unbox the arguments if required
      bool resultSaved = false;
      for (auto it = scope.tempLocals.start(); !it.eof(); ++it) {
         ObjectInfo temp = *it;

         if (temp.mode == TargetMode::UnboxingRequired || temp.mode == TargetMode::RefUnboxingRequired) {
            if (!resultSaved) {
               // presave the result
               ObjectInfo tempResult = declareTempLocal(scope, retVal.typeInfo.typeRef);
               compileAssigningOp(writer, scope, tempResult, retVal);
               retVal = tempResult;

               resultSaved = true;
            }

            // unbox the temporal variable
            auto key = it.key();
            if (temp.mode == TargetMode::RefUnboxingRequired) {
               temp.kind = ObjectKind::Local;

               compileAssigningOp(writer, scope, { ObjectKind::Local, temp.typeInfo, key.value2 }, temp);
            }
            else if (key.value1 == ObjectKind::RefLocal) {
               writeObjectInfo(writer, scope, temp);
               writer.appendNode(BuildKey::Field);
               compileAssigningOp(writer, scope, 
                  { ObjectKind::Local, temp.typeInfo, key.value2 }, 
                  { ObjectKind::Object, temp.typeInfo, 0 });
            }
            else compileAssigningOp(writer, scope, { key.value1, temp.typeInfo, key.value2 }, temp);
         }
      }
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

ObjectInfo Compiler :: compileNewArrayOp(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo source, ref_t targetRef, ArgumentsInfo& arguments)
{
   ref_t sourceRef = retrieveStrongType(scope, source);

   //if (!targetRef)
   //   targetRef = resolvePrimitiveReference(scope, source.type, source.element, false);

   ref_t argumentRefs[ARG_COUNT];
   pos_t argLen = 0;
   for (pos_t i = 0; i < arguments.count(); i++) {
      argumentRefs[argLen++] = retrieveStrongType(scope, arguments[i]);
   }

   BuildKey operationKey = _logic->resolveNewOp(*scope.moduleScope, sourceRef, argumentRefs, argLen);
   if (operationKey == BuildKey::NewArrayOp) {
      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, sourceRef);

      if (targetRef) {
         auto conversionRoutine = _logic->retrieveConversionRoutine(*scope.moduleScope, targetRef, source.typeInfo);
         if (conversionRoutine.result == ConversionResult::BoxingRequired) {
            source.typeInfo = { targetRef };
         }
      }

      assert(arguments.count() == 1); // !! temporally - only one argument is supported

      writeObjectInfo(writer, scope, arguments[0]);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.newNode(operationKey, source.typeInfo.typeRef);

      if (sizeInfo.size < 0)
         writer.appendNode(BuildKey::Size, sizeInfo.size);

      writer.closeNode();

      return { ObjectKind::Object, source.typeInfo, 0 };
   }

   assert(false);

   return {}; // !! temporal
}

ObjectInfo Compiler :: compileNativeConversion(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t operationKey)
{
   ObjectInfo retVal = {};

   switch (operationKey) {
      case INT32_64_CONVERSION:
         retVal = allocateResult(scope, resolvePrimitiveType(scope, { V_INT64 }, false) );

         writeObjectInfo(writer, scope, retVal);
         writer.appendNode(BuildKey::SavingInStack, 0);

         writeObjectInfo(writer, scope, source);

         writer.appendNode(BuildKey::ConversionOp, operationKey);
         break;
      case INT32_FLOAT64_CONVERSION:
         retVal = allocateResult(scope, resolvePrimitiveType(scope, { V_FLOAT64 }, false));

         writeObjectInfo(writer, scope, retVal);
         writer.appendNode(BuildKey::SavingInStack, 0);

         writeObjectInfo(writer, scope, source);

         writer.appendNode(BuildKey::ConversionOp, operationKey);
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         break;
   }

   return retVal;
}

ObjectInfo Compiler :: compileNewOp(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source,
   ref_t signRef, ArgumentsInfo& arguments)
{
   mssg_t messageRef = 0;
   if (source.kind == ObjectKind::ConstantLiteral) {
      IdentifierString valueStr(scope.module->resolveConstant(source.reference));
      IdentifierString postfix;

      postfix.append(valueStr[valueStr.length() - 1]);
      valueStr.truncate(valueStr.length() - 1);

      arguments.add({ ObjectKind::StringLiteral, 
         { scope.moduleScope->buildins.literalReference }, scope.module->mapConstant(*valueStr) });

      postfix.append(CONSTRUCTOR_MESSAGE);

      ref_t signRef = scope.module->mapSignature(&scope.moduleScope->buildins.literalReference, 1, false);
      mssg_t conversionMssg = encodeMessage(scope.module->mapAction(*postfix, signRef, false), 1, FUNCTION_MESSAGE);

      NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
      auto constInfo = nsScope->extensions.get(conversionMssg);
      if (constInfo.value1) {
         messageRef = constInfo.value2;
         source = mapClassSymbol(scope, constInfo.value1);
      }
      else scope.raiseError(errInvalidOperation, node);
   }
   else messageRef = overwriteArgCount(scope.moduleScope->buildins.constructor_message, arguments.count_pos());

   ObjectInfo retVal = compileMessageOperation(
      writer, scope, node, source, messageRef, signRef, arguments, EAttr::StrongResolved | EAttr::NoExtension);

   return retVal;
}

ObjectInfo Compiler :: compilePropertyOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ref_t expectedRef, ExpressionAttribute attrs)
{
   ObjectInfo retVal = { };
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter);
   if (source.mode != TargetMode::None)
      scope.raiseError(errInvalidOperation, node);

   arguments.add(source);

   // NOTE : the operation target shouldn't be a primtive type
   source = validateObject(writer, scope, node, source, 0, true, true);

   current = current.nextNode();
   mssg_t messageRef = mapMessage(scope, current, true, 
      source.kind == ObjectKind::Extension, false);

   ref_t implicitSignatureRef = 0;
   mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, source), expectedRef, messageRef, implicitSignatureRef);
   if (byRefHandler) {
      ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

      addByRefRetVal(arguments, tempRetVal);

      compileMessageOperation(writer, scope, node, source, byRefHandler,
         implicitSignatureRef, arguments, EAttr::AlreadyResolved);

      retVal = tempRetVal;
   }
   else retVal = compileMessageOperation(writer, scope, node, source, messageRef,
      0, arguments, EAttr::None);

   return retVal;
}

mssg_t Compiler :: resolveByRefHandler(Scope& scope, ref_t targetRef, ref_t expectedRef, mssg_t weakMessage, ref_t& signatureRef)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0;
   decodeMessage(weakMessage, actionRef, argCount, flags);

   if (expectedRef != 0 && targetRef != 0 && signatureRef != 0) {
      ref_t dummySignRef = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, dummySignRef);

      ref_t byRefType = retrieveStrongType(scope, { ObjectKind::Object, { V_WRAPPER, expectedRef }, 0 });
      ref_t byRefSignature = _logic->defineByRefSignature(*scope.moduleScope, signatureRef, byRefType);

      ref_t byRefMessage = encodeMessage(scope.module->mapAction(actionName, byRefSignature, false), argCount + 1, flags);

      CheckMethodResult dummy = {};
      if (_logic->resolveCallType(*scope.moduleScope, targetRef, byRefMessage, dummy)) {
         signatureRef = byRefSignature;

         return byRefMessage;
      }
   }      

   return 0;
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ref_t expectedRef, ExpressionAttribute attrs)
{
   ObjectInfo retVal = { };
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter);
   bool probeMode = source.mode == TargetMode::Probe;
   switch (source.mode) {
      case TargetMode::External:
      case TargetMode::WinApi:
         compileMessageArguments(writer, scope, current, arguments, EAttr::None);

         retVal = compileExternalOp(writer, scope, source.reference, source.mode == TargetMode::WinApi, arguments);
         break;
      case TargetMode::CreatingArray:
      {
         compileMessageArguments(writer, scope, current, arguments, EAttr::NoPrimitives);

         retVal = compileNewArrayOp(writer, scope, source, expectedRef, arguments);
         break;
      }
      case TargetMode::Creating:
      {
         ref_t signRef = compileMessageArguments(writer, scope, current, arguments, EAttr::NoPrimitives);

         retVal = compileNewOp(writer, scope, node, mapClassSymbol(scope, 
            retrieveStrongType(scope, source)), signRef, arguments);
         break;
      }
      case TargetMode::Casting:
         compileMessageArguments(writer, scope, current, arguments, EAttr::NoPrimitives);
         if (arguments.count() == 1) {
            retVal = convertObject(writer, scope, current, arguments[0], retrieveStrongType(scope, source));
         }
         else scope.raiseError(errInvalidOperation, node);
         break;
      default:
      {
         // NOTE : the operation target shouldn't be a primtive type
         source = validateObject(writer, scope, node, source, 0, true, true);

         current = current.nextNode();
         mssg_t messageRef = mapMessage(scope, current, false,
            source.kind == ObjectKind::Extension, probeMode);

         if (!test(messageRef, FUNCTION_MESSAGE))
            arguments.add(source);

         ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments, EAttr::NoPrimitives);

         mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, source), expectedRef, messageRef, implicitSignatureRef);
         if (byRefHandler) {
            ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

            addByRefRetVal(arguments, tempRetVal);

            compileMessageOperation(writer, scope, node, source, byRefHandler,
               implicitSignatureRef, arguments, EAttr::AlreadyResolved);

            retVal = tempRetVal;
         }
         else retVal = compileMessageOperation(writer, scope, node, source, messageRef,
            implicitSignatureRef, arguments, EAttr::None);

         break;
      }
   }

   return retVal;
}

bool Compiler :: resolveAutoType(ExprScope& scope, ObjectInfo source, ObjectInfo& target)
{
   ref_t sourceRef = retrieveStrongType(scope, target);

   if (!_logic->validateAutoType(*scope.moduleScope, sourceRef))
      return false;

   return scope.resolveAutoType(target, source.typeInfo);
}

bool Compiler :: compileAssigningOp(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo target, ObjectInfo exprVal)
{
   BuildKey operationType = BuildKey::None;
   int operand = 0;

   int size = 0;
   bool stackSafe = false;
   bool fieldMode = false;
   bool accMode = false;
   
   switch (target.kind) {
      case ObjectKind::Local:
      case ObjectKind::TempLocal:
         scope.markAsAssigned(target);
         operationType = BuildKey::Assigning;
         operand = target.reference;
         break;
      case ObjectKind::ByRefParam:
         operationType = BuildKey::RefParamAssigning;
         operand = target.reference;
         break;
      case ObjectKind::SelfBoxableLocal:
         accMode = true;
         operationType = BuildKey::CopyingToAcc;
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         stackSafe = true;
         break;
      case ObjectKind::TempLocalAddress:
      case ObjectKind::LocalAddress:
         scope.markAsAssigned(target);
         operationType = BuildKey::Copying;
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         stackSafe = true;
         break;
      case ObjectKind::Field:
         scope.markAsAssigned(target);
         operationType = BuildKey::FieldAssigning;
         operand = target.reference;
         fieldMode = true;
         break;
      case ObjectKind::FieldAddress:
         scope.markAsAssigned(target);
         fieldMode = true;
         if (target.reference) {
            operationType = BuildKey::AccFieldCopying;
            operand = target.reference;
         }
         else operationType = BuildKey::CopyingToAcc; 
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         stackSafe = true;

         break;
         // NOTE : it should be the last condition
      case ObjectKind::ByRefParamAddress:
      {
         ref_t targetRef = retrieveStrongType(scope, target);
         size = _logic->defineStructSize(*scope.moduleScope, targetRef).size;
         if (size > 0) {
            stackSafe = true;
            operationType = BuildKey::CopyingToAcc;
            operand = target.reference;
            accMode = true;
         }
         else assert(false); // !! temporally

         break;
      }         
      default:
         return false;
   }

   writeObjectInfo(writer, scope,
      boxArgument(writer, scope, exprVal, stackSafe, true, false));

   if (fieldMode) {
      writer.appendNode(BuildKey::SavingInStack, 0);
      writeObjectInfo(writer, scope, scope.mapSelf());
   }
   else if (accMode) {
      writer.appendNode(BuildKey::SavingInStack, 0);
      writeObjectInfo(writer, scope, target);
   }

   writer.newNode(operationType, operand);
   if (size != 0) {
      writer.appendNode(BuildKey::Size, size);
   }
   writer.closeNode();

   return true;
}

ObjectInfo Compiler :: compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand)
{
   ObjectInfo target = mapObject(scope, loperand, EAttr::None);
   ObjectInfo exprVal = {};

   ref_t targetRef = retrieveStrongType(scope, target);
   if (targetRef == V_AUTO) {
      // support auto attribute
      exprVal = compileExpression(writer, scope, roperand,
         0, EAttr::Parameter);

      if (resolveAutoType(scope, exprVal, target)) {
         targetRef = retrieveStrongType(scope, exprVal);
         target.reference = targetRef;
      }
      else scope.raiseError(errInvalidOperation, roperand.parentNode());
   }
   else exprVal = compileExpression(writer, scope, roperand,
      targetRef, EAttr::Parameter);

   if (!compileAssigningOp(writer, scope, target, exprVal))
      scope.raiseError(errInvalidOperation, loperand.parentNode());;

   return target;
}

ObjectInfo Compiler :: compileIndexerOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef)
{
   // HOTFIX : recognize fixed-array declaration
   SyntaxNode loperand = node.firstChild();
   if (loperand == SyntaxKey::Object) {
      ObjectInfo info = mapObject(scope, loperand, EAttr::Lookahead);
      if (info.kind == ObjectKind::NewVariable) {
         // if it is a new variable declaration - treat it like a new array
         declareVariable(scope, node, info.typeInfo); // !! temporal - typeref should be provided or super class

         return {}; // !! temporally
      }
      else if (info.kind == ObjectKind::MssgLiteral) {
         return mapMessageConstant(scope, node, info.reference);
      }
   }

   return compileOperation(writer, scope, node, operatorId, expectedRef);
}

ObjectInfo Compiler :: compileBoolOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId)
{
   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = lnode.nextNode();

   writer.newNode(BuildKey::ShortCircuitOp, operatorId);

   writer.appendNode(BuildKey::TrueConst, scope.moduleScope->branchingInfo.trueRef);
   writer.appendNode(BuildKey::FalseConst, scope.moduleScope->branchingInfo.falseRef);

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, lnode, scope.moduleScope->branchingInfo.typeRef, EAttr::None);
   writer.closeNode();

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, rnode, scope.moduleScope->branchingInfo.typeRef, EAttr::None);
   writer.closeNode();

   writer.closeNode();

   return { ObjectKind::Object, { scope.moduleScope->branchingInfo.typeRef }, 0 };
}

ObjectInfo Compiler :: compileAssignOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   int operatorId, ref_t expectedRef)
{
   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = lnode.nextNode();

   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter);
   ObjectInfo roperand = compileExpression(writer, scope, rnode, 0, EAttr::Parameter);

   ref_t      arguments[2] = {};
   arguments[0] = loperand.typeInfo.typeRef;
   arguments[1] = roperand.typeInfo.typeRef;

   ref_t dummy = 0;
   BuildKey op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, 2, dummy);
   if (op != BuildKey::None) {
      // box argument locally if required
      loperand = boxArgumentLocally(writer, scope, loperand, true);
      roperand = boxArgumentLocally(writer, scope, roperand, true);

      writeObjectInfo(writer, scope, roperand);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.newNode(op, operatorId);
      writer.appendNode(BuildKey::Index, loperand.argument);
      writer.closeNode();

      scope.reserveArgs(2);
   }
   else {
      switch (operatorId) {
         case ADD_ASSIGN_OPERATOR_ID:
            operatorId = ADD_OPERATOR_ID;
            break;
         case SUB_ASSIGN_OPERATOR_ID:
            operatorId = SUB_OPERATOR_ID;
            break;
         case MUL_ASSIGN_OPERATOR_ID:
            operatorId = MUL_OPERATOR_ID;
            break;
         case DIV_ASSIGN_OPERATOR_ID:
            operatorId = DIV_OPERATOR_ID;
            break;
         default:
            break;
      }

      mssg_t message = resolveOperatorMessage(scope.moduleScope, operatorId);
      ArgumentsInfo messageArguments;
      messageArguments.add(loperand);

      if (roperand.kind != ObjectKind::Unknown)
         messageArguments.add(roperand);

      ObjectInfo opVal = compileWeakOperation(writer, scope, node, arguments, 2, loperand,
         messageArguments, message, expectedRef);

      if(!compileAssigningOp(writer, scope, loperand, opVal))
         scope.raiseError(errInvalidOperation, node);
   }

   return loperand;
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef)
{
   SyntaxNode loperand = node.firstChild();
   SyntaxNode roperand = loperand.nextNode();

   if (operatorId == SET_OPERATOR_ID){
      // assign operation is a special case
      if (loperand == SyntaxKey::IndexerOperation) {
         return compileOperation(writer, scope, loperand, roperand, SET_INDEXER_OPERATOR_ID, expectedRef);
      }
      else return compileAssigning(writer, scope, loperand, roperand);

   }
   else return compileOperation(writer, scope, loperand, roperand, operatorId, expectedRef);
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
   ObjectInfo roperand = { ObjectKind::Closure, { V_CLOSURE }, 0 };
   ObjectInfo roperand2 = {};

   // HOTFIX : to allow correct step over the branching statement 
   writer.appendNode(BuildKey::EndStatement);
   writer.appendNode(BuildKey::VirtualBreakoint);

   BuildKey   op = BuildKey::None;

   size_t     argLen = 2;
   ref_t      arguments[3];
   arguments[0] = retrieveType(scope, loperand);
   arguments[1] = retrieveType(scope, roperand);
   if (r2node != SyntaxKey::None) {
      roperand2 = { ObjectKind::Closure, { V_CLOSURE }, 0 };

      argLen++;
      arguments[2] = retrieveType(scope, roperand2);
   }

   ref_t outputRef = 0;
   op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef);

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

   // HOTFIX : to compenstate the closed statement above
   writer.appendNode(BuildKey::OpenStatement);

   return retVal;
}

ObjectInfo Compiler :: compileMessageOperationR(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo target, SyntaxNode messageNode)
{
   ArgumentsInfo arguments;

   switch (target.mode) {
      case TargetMode::Casting:
         compileMessageArguments(writer, scope, messageNode, arguments, EAttr::NoPrimitives);
         if (arguments.count() == 1) {
            return convertObject(writer, scope, messageNode, arguments[0], retrieveStrongType(scope, target));
         }
         else scope.raiseError(errInvalidOperation, messageNode);
         break;
      default:
         {
            // NOTE : the operation target shouldn't be a primitive type
            ObjectInfo source = validateObject(writer, scope, messageNode, target, 0, true, true);

            mssg_t messageRef = mapMessage(scope, messageNode, false, false, false);

            if (!test(messageRef, FUNCTION_MESSAGE)) {
               arguments.add(source);
            }

            ref_t implicitSignatureRef = compileMessageArguments(writer, scope, messageNode, arguments, EAttr::NoPrimitives);

            return compileMessageOperation(writer, scope, messageNode, source, messageRef,
               implicitSignatureRef, arguments, EAttr::None);

            break;
         }
   }

   return {};
}

ObjectInfo Compiler :: compileAltOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   ObjectInfo target = {};
   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::MessageOperation) {
      SyntaxNode objNode = current.firstChild();

      target = compileObject(writer, scope, objNode, EAttr::Parameter);

      writer.newNode(BuildKey::AltOp, ehLocal.argument);

      writer.newNode(BuildKey::Tape);
      compileMessageOperationR(writer, scope, target, objNode.nextNode());
      writer.closeNode();
   }
   else scope.raiseError(errInvalidOperation, node);

   writer.newNode(BuildKey::Tape);
   SyntaxNode altNode = current.nextNode().firstChild();

   compileMessageOperationR(writer, scope, target, altNode.firstChild());
   writer.closeNode();

   writer.closeNode();

   return {};
}

ObjectInfo Compiler :: compileIsNilOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   ObjectInfo loperand = {};
   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::MessageOperation) {
      SyntaxNode objNode = current.firstChild();

      loperand = compileObject(writer, scope, objNode, EAttr::Parameter);

      writer.newNode(BuildKey::AltOp, ehLocal.argument);

      writer.newNode(BuildKey::Tape);
      compileMessageOperationR(writer, scope, loperand, objNode.nextNode());
      writer.closeNode();

      writer.newNode(BuildKey::Tape);
      writer.appendNode(BuildKey::NilReference, 0);
      writer.closeNode();

      writer.closeNode();

      loperand = saveToTempLocal(writer, scope, { ObjectKind::Object });
   }

   SyntaxNode altNode = current.nextNode();
   ObjectInfo roperand = compileExpression(writer, scope, altNode, 0, EAttr::Parameter);

   writeObjectInfo(writer, scope, roperand);
   writer.appendNode(BuildKey::SavingInStack);
   writeObjectInfo(writer, scope, loperand);
   writer.appendNode(BuildKey::NilOp, ISNIL_OPERATOR_ID);

   return { ObjectKind::Object };

}

ObjectInfo Compiler :: compileCatchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   SyntaxNode catchNode = node.findChild(SyntaxKey::CatchDispatch);
   SyntaxNode opNode = node.firstChild();
   if (opNode.existChild(SyntaxKey::ClosureBlock))
      opNode = opNode.findChild(SyntaxKey::ClosureBlock);

   writer.newNode(BuildKey::CatchOp, ehLocal.argument);

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, opNode, 0, EAttr::None);
   writer.closeNode();

   writer.newNode(BuildKey::Tape);
   compileMessageOperationR(writer, scope, { ObjectKind::Object }, catchNode.firstChild().firstChild());
   writer.closeNode();

   writer.closeNode();

   return {};
}

ObjectInfo Compiler :: mapStringConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::StringLiteral, { V_STRING }, scope.module->mapConstant(node.identifier()) };
}

ObjectInfo Compiler :: mapWideStringConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::WideStringLiteral, { V_WIDESTRING }, scope.module->mapConstant(node.identifier()) };
}

ObjectInfo Compiler :: mapCharacterConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::CharacterLiteral, { V_WORD32 }, scope.module->mapConstant(node.identifier()) };
}

ObjectInfo Compiler :: mapConstant(Scope& scope, SyntaxNode node)
{
   return { ObjectKind::ConstantLiteral, { V_WORD32 }, scope.module->mapConstant(node.identifier()) };
}

inline ref_t mapIntConstant(Compiler::Scope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendInt(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

inline ref_t mapLongConstant(Compiler::Scope& scope, long long integer)
{
   String<char, 40> s;

   // convert back to string as a decimal integer
   s.appendLong(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

inline ref_t mapUIntConstant(Compiler::Scope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendUInt(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

inline ref_t mapFloat64Const(Compiler::Scope& scope, double val)
{
   String<char, 30> s;

   // convert back to string as a decimal integer
   s.appendDouble(val);

   return scope.moduleScope->module->mapConstant(s.str());
}

ObjectInfo Compiler :: mapIntConstant(Scope& scope, SyntaxNode node, int radix)
{
   int integer = StrConvertor::toInt(node.identifier(), radix);
   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return { ObjectKind::IntLiteral, { V_INT32 }, ::mapIntConstant(scope, integer), integer };
}

ObjectInfo Compiler :: mapUIntConstant(Scope& scope, SyntaxNode node, int radix)
{
   int integer = StrConvertor::toUInt(node.identifier(), radix);
   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return { ObjectKind::IntLiteral, { V_INT32 }, ::mapUIntConstant(scope, integer), integer };
}

ObjectInfo Compiler :: mapLongConstant(Scope& scope, SyntaxNode node, int radix)
{
   long long integer = 0;

   ustr_t val = node.identifier();
   if (val.endsWith("l")) {
      String<char, 50> tmp(val);
      tmp.truncate(tmp.length() - 1);

      integer = StrConvertor::toLong(tmp.str(), radix);
   }
   else integer = StrConvertor::toLong(node.identifier(), radix);

   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return { ObjectKind::LongLiteral, { V_INT64 }, ::mapLongConstant(scope, integer)};
}

ObjectInfo Compiler :: mapFloat64Constant(Scope& scope, SyntaxNode node)
{
   double real = 0;

   ustr_t val = node.identifier();
   if (val.endsWith("r")) {
      String<char, 50> tmp(val);
      tmp.truncate(tmp.length() - 1);

      real = StrConvertor::toDouble(tmp.str());
   }
   else real = StrConvertor::toDouble(node.identifier());
   if (errno == ERANGE)
      scope.raiseError(errInvalidIntNumber, node);

   return { ObjectKind::Float64Literal, { V_FLOAT64 }, ::mapFloat64Const(scope, real) };
}

ObjectInfo Compiler :: mapMessageConstant(Scope& scope, SyntaxNode node, ref_t actionRef)
{
   pos_t argCount = 0;

   Interpreter interpreter(scope.moduleScope, _logic);
   ObjectInfo retVal = evalExpression(interpreter, scope, node.findChild(SyntaxKey::Expression));
   switch (retVal.kind) {
      case ObjectKind::IntLiteral:
         argCount = retVal.extra;
         break;
      default:
         scope.raiseError(errCannotEval, node);
         break;
   }

   mssg_t message = encodeMessage(actionRef, argCount, 0);
   IdentifierString messageName;
   ByteCodeUtil::resolveMessageName(messageName, scope.module, message);

   ref_t constRef = scope.module->mapConstant(*messageName);

   return { ObjectKind::MssgLiteral, { V_MESSAGE }, constRef };
}

ObjectInfo Compiler :: mapTerminal(Scope& scope, SyntaxNode node, TypeInfo declaredTypeInfo, EAttr attrs)
{
   bool forwardMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Forward);
   bool variableMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewVariable);
   bool externalOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::Extern);
   bool newOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewOp);
   bool castOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::CastOp);
   bool refOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::RefOp);
   bool mssgOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::MssgLiteral);
   bool probeMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::ProbeMode);
   bool memberMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Member);

   ObjectInfo retVal;
   bool invalid = false;
   bool invalidForNonIdentifier = forwardMode || variableMode || refOp || mssgOp || memberMode;
   if (externalOp) {
      auto externalInfo = mapExternal(scope, node);
      switch (externalInfo.type) {
         case ExternalType::WinApi:
            return { ObjectKind::Extern, {}, externalInfo.reference, 0, TargetMode::WinApi };
         default:
            return { ObjectKind::Extern, {}, externalInfo.reference, 0, TargetMode::External };
      }
   }
   else if (newOp || castOp) {
      switch (node.key) {
         case SyntaxKey::identifier:
         case SyntaxKey::reference:
         {
            TypeInfo typeInfo = resolveTypeAttribute(scope, node, false, false);

            retVal = { ObjectKind::Class, typeInfo, 0u, newOp ? TargetMode::Creating : TargetMode::Casting };
            break;
         }
         default:
            invalid = true;
            break;
      }
   }
   else if (mssgOp) {
      switch (node.key) {
         case SyntaxKey::identifier:
         {
            retVal = { ObjectKind::MssgNameLiteral, { V_MESSAGENAME },
               scope.module->mapAction(node.identifier(), 0, false) };
            break;
         }
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

               declareVariable(scope, node, declaredTypeInfo);
               retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, attrs | ExpressionAttribute::Local);
            }
            else if (forwardMode) {
               IdentifierString forwardName(FORWARD_PREFIX_NS, node.identifier());

               retVal = scope.mapIdentifier(*forwardName, true, attrs);
            }
            else if (memberMode) {
               retVal = scope.mapMember(node.identifier());
            }
            else retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, attrs);

            if (refOp) {
               switch (retVal.kind) {
                  case ObjectKind::LocalAddress:
                     retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                     break;
                  case ObjectKind::Local:
                     retVal.kind = ObjectKind::RefLocal;
                     retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                     break;
                  default:
                     invalid = true;
                     break;
               }
            }
            break;
         case SyntaxKey::string:
            invalid = invalidForNonIdentifier;

            retVal = mapStringConstant(scope, node);
            break;
         case SyntaxKey::wide:
            invalid = invalidForNonIdentifier;

            retVal = mapWideStringConstant(scope, node);
            break;
         case SyntaxKey::character:
            invalid = invalidForNonIdentifier;

            retVal = mapCharacterConstant(scope, node);
            break;
         case SyntaxKey::integer:
            invalid = invalidForNonIdentifier;

            retVal = mapIntConstant(scope, node, 10);
            break;
         case SyntaxKey::hexinteger:
            invalid = invalidForNonIdentifier;

            retVal = mapUIntConstant(scope, node, 16);
            break;
         case SyntaxKey::longinteger:
            invalid = invalidForNonIdentifier;

            retVal = mapLongConstant(scope, node, 10);
            break;
         case SyntaxKey::real:
            invalid = invalidForNonIdentifier;

            retVal = mapFloat64Constant(scope, node);
            break;
         case SyntaxKey::constant:
            invalid = invalidForNonIdentifier;

            retVal = mapConstant(scope, node);
            break;
         default:
            // to make compiler happy
            invalid = true;
            break;
      }
   }

   if (invalid)
      scope.raiseError(errInvalidOperation, node);

   if (declaredTypeInfo.typeRef == V_OBJARRAY) {
      retVal = defineArrayType(scope, retVal);
   }

   if (probeMode)
      retVal.mode = TargetMode::Probe;

   return retVal;
}

ObjectInfo Compiler :: mapObject(Scope& scope, SyntaxNode node, EAttrs mode)
{
   SyntaxNode terminalNode = node == SyntaxKey::identifier ? node : node.lastChild(SyntaxKey::TerminalMask);

   TypeInfo declaredTypeInfo = {};
   declareExpressionAttributes(scope, node, declaredTypeInfo, mode);
   if (mode.test(EAttr::Lookahead)) {
      if (mode.test(EAttr::NewVariable)) {
         return { ObjectKind::NewVariable, declaredTypeInfo, 0, 0 };
      }
      else if (mode.test(EAttr::MssgNameLiteral)) {
         return { ObjectKind::MssgLiteral, { V_MESSAGE },
            scope.module->mapAction(terminalNode.identifier(), 0, false) };
      }
      else return {};
   }

   if (terminalNode.nextNode() == SyntaxKey::TemplateArg && !EAttrs::test(mode.attrs, ExpressionAttribute::NewOp)) {
      scope.raiseError(errInvalidSyntax, node);
   }

   ObjectInfo retVal = mapTerminal(scope, terminalNode, declaredTypeInfo, mode.attrs);

   return retVal;
}

ObjectInfo Compiler :: declareTempLocal(ExprScope& scope, ref_t typeRef, bool dynamicOnly)
{
   SizeInfo sizeInfo = {};
   if (!dynamicOnly) {
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, typeRef);
   }
   if (sizeInfo.size > 0) {
      ObjectInfo tempStruct = declareTempStructure(scope, sizeInfo);
      tempStruct.typeInfo = { typeRef };

      return tempStruct;
   }
   else {
      int tempLocal = scope.newTempLocal();
      return { ObjectKind::TempLocal, { typeRef }, (ref_t)tempLocal };
   }

}

ObjectInfo Compiler :: saveToTempLocal(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo object)
{
   if (object.kind == ObjectKind::Extern) {
      CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, object.typeInfo.typeRef);
      int tempLocal = allocateLocalAddress(codeScope, sizeInfo.size, false);

      writer.appendNode(BuildKey::SavingIndex, tempLocal);

      return { ObjectKind::TempLocalAddress, object.typeInfo, (ref_t)tempLocal };
   }
   else {
      int tempLocal = scope.newTempLocal();
      writeObjectInfo(writer, scope, object);
      writer.appendNode(BuildKey::Assigning, tempLocal);

      return { ObjectKind::TempLocal, object.typeInfo, (ref_t)tempLocal };
   }
}

ObjectInfo Compiler :: compileObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   if (node == SyntaxKey::Object) {
      ObjectInfo retVal = mapObject(scope, node, mode);
      switch (retVal.kind) {
         case ObjectKind::ConstantLiteral:
         {
            ArgumentsInfo arguments;
            ref_t typeRef = scope.moduleScope->buildins.literalReference;
            ref_t signRef = scope.module->mapSignature(&typeRef, 1, false);

            return compileNewOp(writer, scope, node, retVal, signRef, arguments);
         }
         case ObjectKind::Unknown:
            scope.raiseError(errUnknownObject, node.lastChild(SyntaxKey::TerminalMask));
            break;
         default:
            break;
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
   retVal.typeInfo = { targetRef };

   return retVal;
}

inline bool isNormalConstant(ObjectInfo info)
{
   switch (info.kind) {
      case ObjectKind::StringLiteral:
         return true;
   default:
      return false;
   }
}

ObjectInfo Compiler :: convertObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source,
   ref_t targetRef)
{
   if (!_logic->isCompatible(*scope.moduleScope, { targetRef }, source.typeInfo, false)) {
      if (source.typeInfo.typeRef == V_WRAPPER) {
         // unbox wrapper for the conversion
         source.typeInfo = { source.typeInfo.elementRef };
      }

      auto conversionRoutine = _logic->retrieveConversionRoutine(*scope.moduleScope, targetRef, source.typeInfo);
      if (conversionRoutine.result == ConversionResult::BoxingRequired) {
         // if it is implcitily compatible
         switch (source.kind) {
            case ObjectKind::TempLocalAddress:
            case ObjectKind::LocalAddress:
            case ObjectKind::IntLiteral:
            case ObjectKind::MssgLiteral:
            case ObjectKind::CharacterLiteral:
            case ObjectKind::RefLocal:
               source.typeInfo.typeRef = targetRef;
               break;
            default:
               return boxArgument(writer, scope, source, false, true, false, targetRef);
         }
      }
      else if (conversionRoutine.result == ConversionResult::Conversion) {
         ArgumentsInfo arguments;
         arguments.add(source);
         ref_t signRef = scope.module->mapSignature(&source.typeInfo.typeRef, 1, false);

         return compileNewOp(writer, scope, node, mapClassSymbol(scope, targetRef),
            signRef, arguments);
      }
      else if (conversionRoutine.result == ConversionResult::NativeConversion) {
         source = compileNativeConversion(writer, scope, node, source, conversionRoutine.operationKey);
      }
      else source = typecastObject(writer, scope, node, source, targetRef);
   }

   return source;
}

ObjectInfo Compiler :: compileNestedExpression(BuildTreeWriter& writer, InlineClassScope& scope, ExprScope& ownerScope, EAttr mode)
{
   ref_t nestedRef = scope.reference;

   if (test(scope.info.header.flags, elStateless)) {
      ObjectInfo retVal = { ObjectKind::Singleton, { nestedRef }, nestedRef };

      return retVal;
   }
   else {
      ObjectInfo retVal = { ObjectKind::Object, { nestedRef }, 0 };

      ArgumentsInfo list;
      // first pass : box an argument if required
      for (auto it = scope.outers.start(); !it.eof(); ++it) {
         ObjectInfo arg = (*it).outerObject;

         arg = boxArgument(writer, ownerScope, arg, false, false, false);
         switch (arg.kind) {
            case ObjectKind::Field:
            case ObjectKind::ReadOnlyField:
            case ObjectKind::Outer:
            case ObjectKind::OuterField:
               arg = saveToTempLocal(writer, ownerScope, arg);
               break;
            default:
               break;
         }

         list.add(arg);
      }

      writer.newNode(BuildKey::CreatingClass, scope.info.fields.count());
      writer.appendNode(BuildKey::Type, nestedRef);
      writer.closeNode();

      // second pass : fill members
      int argIndex = 0;
      for (auto it = scope.outers.start(); !it.eof(); ++it) {
         ObjectInfo arg = list[argIndex];

         auto fieldInfo = scope.info.fields.get(it.key());

         switch (arg.kind) {
            case ObjectKind::Local:
            case ObjectKind::TempLocal:
            case ObjectKind::Param:
               writer.appendNode(BuildKey::AssignLocalToStack, arg.reference);
               writer.appendNode(BuildKey::SetImmediateField, fieldInfo.offset);
               break;
            default:
               // NOTE : should neve be hit
               assert(false);
               break;
         }

         argIndex++;
      }

      return retVal;
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

   return compileNestedExpression(writer, scope, ownerScope, mode);
}

ObjectInfo Compiler :: compileNested(BuildTreeWriter& writer, ExprScope& ownerScope, SyntaxNode node, ExpressionAttribute mode)
{
   TypeInfo parentInfo = { ownerScope.moduleScope->buildins.superReference };
   EAttrs nestedMode = {};
   declareExpressionAttributes(ownerScope, node, parentInfo, nestedMode);

   // allow only new and type attrobutes
   if (nestedMode.attrs != EAttr::None && nestedMode.attrs != EAttr::NewOp && nestedMode.attrs != EAttr::NewVariable)
      ownerScope.raiseError(errInvalidOperation, node);

   ref_t nestedRef = mapNested(ownerScope, mode);
   InlineClassScope scope(&ownerScope, nestedRef);

   compileNestedClass(writer, scope, node, parentInfo.typeRef);

   return compileNestedExpression(writer, scope, ownerScope, mode);
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

ObjectInfo Compiler :: compileExternExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   writer.newNode(BuildKey::ExternOp);

   compileExpression(writer, scope, node, 0, mode);

   writer.closeNode();

   return { };
}

ObjectInfo Compiler :: validateObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ObjectInfo retVal, ref_t targetRef, bool noPrimitives, bool paramMode)
{
   if (!targetRef && retVal.typeInfo.isPrimitive() && noPrimitives) {
      targetRef = retrieveStrongType(scope, retVal);
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

ObjectInfo Compiler :: compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ref_t targetRef, ExpressionAttribute mode)
{
   bool paramMode = EAttrs::testAndExclude(mode, EAttr::Parameter);
   bool noPrimitives = EAttrs::testAndExclude(mode, EAttr::NoPrimitives);

   ObjectInfo retVal;

   SyntaxNode current = node == SyntaxKey::Expression ? node.firstChild() : node;
   switch (current.key) {
      case SyntaxKey::MessageOperation:
         retVal = compileMessageOperation(writer, scope, current, targetRef, mode);
         break;
      case SyntaxKey::PropertyOperation:
         retVal = compilePropertyOperation(writer, scope, current, targetRef, mode);
         break;
      case SyntaxKey::AssignOperation:
      case SyntaxKey::AddOperation:
      case SyntaxKey::SubOperation:
      case SyntaxKey::MulOperation:
      case SyntaxKey::DivOperation:
      case SyntaxKey::LenOperation:
      case SyntaxKey::LessOperation:
      case SyntaxKey::GreaterOperation:
      case SyntaxKey::NameOperation:
      case SyntaxKey::EqualOperation:
      case SyntaxKey::NotOperation:
      case SyntaxKey::NotEqualOperation:
      case SyntaxKey::NotLessOperation:
      case SyntaxKey::NotGreaterOperation:
      case SyntaxKey::NestedExpression:
      case SyntaxKey::ValueOperation:
      case SyntaxKey::BAndOperation:
      case SyntaxKey::BOrOperation:
      case SyntaxKey::BXorOperation:
      case SyntaxKey::BNotOperation:
      case SyntaxKey::ShlOperation:
      case SyntaxKey::ShrOperation:
      case SyntaxKey::NegateOperation:
         retVal = compileOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS, targetRef);
         break;
      case SyntaxKey::AddAssignOperation:
      case SyntaxKey::SubAssignOperation:
      case SyntaxKey::MulAssignOperation:
      case SyntaxKey::DivAssignOperation:
         retVal = compileAssignOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS, targetRef);
         break;
      case SyntaxKey::AndOperation:
      case SyntaxKey::OrOperation:
         retVal = compileBoolOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         if (targetRef)
            typecastObject(writer, scope, current, retVal, targetRef);

         break;
      case SyntaxKey::IndexerOperation:
         retVal = compileIndexerOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS, targetRef);
         break;
      case SyntaxKey::IfOperation:
      case SyntaxKey::IfNotOperation:
      case SyntaxKey::IfElseOperation:
         retVal = compileBranchingOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::LoopOperation:
         retVal = compileLoopExpression(writer, scope, current.firstChild(), mode);
         break;
      case SyntaxKey::ExternOperation:
         retVal = compileExternExpression(writer, scope, current.firstChild(), mode);
         break;
      case SyntaxKey::CatchOperation:
         retVal = compileCatchOperation(writer, scope, current);
         break;
      case SyntaxKey::AltOperation:
         retVal = compileAltOperation(writer, scope, current);
         break;
      case SyntaxKey::IsNilOperation:
         retVal = compileIsNilOperation(writer, scope, current);
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

   retVal = validateObject(writer, scope, node, retVal, targetRef, noPrimitives, paramMode);

   return retVal;
}

inline bool checkTerminalCoords(SyntaxNode node)
{
   SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);

   return terminal.existChild(SyntaxKey::Row);
}

inline SyntaxNode findObjectNode(SyntaxNode node)
{
   if (node == SyntaxKey::CodeBlock) {
      // HOTFIX : to prevent double breakpoint
      return {};
   }
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Object:
            if (checkTerminalCoords(current))
               return current;
            break;
         default:
         {
            SyntaxNode objectNode = findObjectNode(current);
            if (objectNode != SyntaxKey::None)
               return objectNode;

            break;
         }
      }

      current = current.nextNode();
   }

   return {};
}

ObjectInfo Compiler :: compileRetExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);

   //bool autoMode = false;
   ref_t outputRef = codeScope.getOutputRef();
   if (outputRef == V_AUTO) {
      //autoMode = true;
      outputRef = 0;
   }

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(node), BuildKey::Breakpoint);

   ObjectInfo retVal = compileExpression(writer, scope, node.findChild(SyntaxKey::Expression), outputRef, EAttr::Root);
   if (codeScope.isByRefHandler()) {
      compileAssigningOp(writer, scope, codeScope.mapByRefReturnArg(), retVal);

      retVal = {};
   }
   else {
      retVal = boxArgument(writer, scope, retVal, false, true, false);

      if (!hasToBePresaved(retVal)) {
         writeObjectInfo(writer, scope, retVal);
      }

      outputRef = retrieveStrongType(scope, retVal);

      _logic->validateAutoType(*scope.moduleScope, outputRef);

      scope.resolveAutoOutput(outputRef);
   }

   writer.appendNode(BuildKey::EndStatement);
   writer.appendNode(BuildKey::VirtualBreakoint);

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

   //if (isBoxingRequired(retVal)) {
   //   retVal = boxArgumentInPlace(writer, scope, retVal);
   //}

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
            scope.info.typeRef = retrieveStrongType(scope, retVal);
            break;
         case ObjectKind::StringLiteral:
            scope.info.symbolType = SymbolType::Constant;
            scope.info.valueRef = retVal.reference;
            scope.info.typeRef = retrieveStrongType(scope, retVal);
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
      boxArgument(writer, exprScope, retVal, false, true, false));

   writer.appendNode(BuildKey::EndStatement);

   writer.appendNode(BuildKey::VirtualBreakoint);

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
   ObjectInfo retVal(ObjectKind::Class, { scope.info.header.classRef }, scope.reference, 0);
   ExprScope exprScope(&scope);
   writeObjectInfo(writer, exprScope, retVal);
   writer.appendNode(BuildKey::CloseFrame);
   writer.appendNode(BuildKey::Exit);
   writer.closeNode();

   writer.closeNode();
}

void Compiler :: beginMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node, BuildKey scopeKey, bool withDebugInfo)
{
   writer.newNode(scopeKey, scope.message);

   if (withDebugInfo) {
      SyntaxNode pathNode = node.findChild(SyntaxKey::SourcePath);
      if (pathNode != SyntaxKey::None)
         writer.appendNode(BuildKey::Path, pathNode.identifier());

      writer.newNode(BuildKey::Tape);

      writeParameterDebugInfo(writer, scope);
   }
   else writer.newNode(BuildKey::Tape);
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
      bool embeddableArray = false;
      if (localInfo.typeInfo.typeRef) {
         ref_t typeRef = localInfo.typeInfo.typeRef;

         if (localInfo.typeInfo.isPrimitive() && localInfo.typeInfo.elementRef)
            typeRef = resolvePrimitiveType(codeScope, localInfo.typeInfo, false);

         embeddableArray = _logic->isEmbeddableArray(*codeScope.moduleScope, typeRef);
         if (embeddableArray) {
            node.appendChild(BuildKey::BinaryArray, localInfo.offset)
               .appendChild(BuildKey::Size, localInfo.size);
         }
      }

      if (localInfo.size > 0) {
         if (embeddableArray) {
            if (_logic->isCompatible(*codeScope.moduleScope, 
               { codeScope.moduleScope->buildins.byteReference }, { localInfo.typeInfo.elementRef }, false)) 
            {
               BuildNode varNode = node.appendChild(BuildKey::ByteArrayAddress, it.key());
               varNode.appendChild(BuildKey::Index, localInfo.offset);
            }
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.intReference) {
            BuildNode varNode = node.appendChild(BuildKey::IntVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.byteReference) {
            BuildNode varNode = node.appendChild(BuildKey::IntVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.longReference) {
            BuildNode varNode = node.appendChild(BuildKey::LongVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.realReference) {
            BuildNode varNode = node.appendChild(BuildKey::RealVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else {
            // !! temporal stub
            BuildNode varNode = node.appendChild(BuildKey::Variable, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
      }
      else {
         BuildNode varNode = node.appendChild(BuildKey::Variable, it.key());
         varNode.appendChild(BuildKey::Index, localInfo.offset);
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
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   if (!newFrame) {
      if (scope.checkHint(MethodHint::Multimethod)) {
         compileMultidispatch(writer, codeScope, *classScope, node, false);
      }

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
      case SyntaxKey::ResendDispatch:
         retVal = compileResendCode(writer, codeScope, 
            scope.constructorMode ? 
               mapClassSymbol(scope, scope.getClassRef()) : scope.mapSelf(), 
            bodyNode);
         break;
      case SyntaxKey::Redirect:
         retVal = compileRedirect(writer, codeScope, bodyNode);
         break;
      default:
         break;
   }

   // if the method returns itself
   if (retVal.kind == ObjectKind::Unknown) {
      ExprScope exprScope(&codeScope);

      retVal = scope.mapSelf();
      if (codeScope.isByRefHandler()) {
         compileAssigningOp(writer, exprScope, codeScope.mapByRefReturnArg(), retVal);
      }
      else {
         ref_t outputRef = scope.info.outputRef;
         if (outputRef && outputRef != V_AUTO) {
            convertObject(writer, exprScope, node, retVal, outputRef);

            exprScope.syncStack();
         }

         writeObjectInfo(writer, exprScope,
            boxArgument(writer, exprScope, retVal, false, true, false));
      }
   }

   writer.appendNode(BuildKey::CloseFrame);

   if (scope.checkHint(MethodHint::Constant)) {
      ref_t constRef = generateConstant(scope, retVal, 0);
      if (constRef) {
         classScope->addRefAttribute(scope.message, ClassAttribute::ConstantMethod, constRef);

         classScope->save();
      }
      else scope.raiseError(errInvalidConstAttr, node);
   }
}

void Compiler :: compileInitializerMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode classNode)
{
   beginMethod(writer, scope, classNode, BuildKey::Method, false);

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

void Compiler :: compileMultidispatch(BuildTreeWriter& writer, CodeScope& scope, ClassScope& classScope, 
   SyntaxNode node, bool implicitMode)
{
   mssg_t message = scope.getMessageID();

   BuildKey op = BuildKey::DispatchingOp;
   ref_t    opRef = classScope.info.attributes.get({ message, ClassAttribute::OverloadList });
   if (!opRef)
      scope.raiseError(errIllegalOperation, node);

   if (test(classScope.info.header.flags, elSealed) || test(message, STATIC_MESSAGE)) {
      op = BuildKey::SealedDispatchingOp;
   }

   writer.newNode(op, opRef);
   writer.appendNode(BuildKey::Message, message);
   writer.closeNode();
   if (implicitMode) {
      // if it is an implicit mode (auto generated multi-method)
      if (classScope.extensionDispatcher) {
         writer.appendNode(BuildKey::Argument, 0);

         writer.newNode(BuildKey::RedirectOp);
         writer.closeNode();
      }
      else if (node.arg.reference) {
         writer.appendNode(BuildKey::RedirectOp, node.arg.reference);
      }
      else {
         SyntaxNode targetNode = node.findChild(SyntaxKey::Target);
         assert(targetNode != SyntaxKey::None);

         writer.newNode(BuildKey::DirectResendOp, message);
         writer.appendNode(BuildKey::Type, targetNode.arg.reference);
         writer.closeNode();
      }
   }
}

ObjectInfo Compiler :: compileRedirect(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);
   ArgumentsInfo arguments;

   ObjectInfo target = compileExpression(writer, scope, node.firstChild(), 0, EAttr::Parameter);

   mssg_t messageRef = codeScope.getMessageID();

   if (!test(messageRef, FUNCTION_MESSAGE))
      arguments.add(target);

   MethodScope* methodScope = Scope::getScope<MethodScope>(codeScope, Scope::ScopeLevel::Method);

   for (auto it = methodScope->parameters.start(); !it.eof(); ++it) {
      arguments.add(methodScope->mapParameter(it.key(), EAttr::None));
   }

   ref_t signRef = getSignature(scope.module, messageRef);
   ObjectInfo retVal = compileMessageOperation(writer, scope, {}, target, messageRef,
      signRef, arguments, EAttr::AlreadyResolved);

   scope.syncStack();

   return retVal;
}

ObjectInfo Compiler :: compileResendCode(BuildTreeWriter& writer, CodeScope& codeScope, ObjectInfo source, SyntaxNode node)
{
   ObjectInfo retVal = {};

   if (!node.arg.reference) {
      SyntaxNode current = node.firstChild().firstChild();
      bool superMode = false;
      while (current == SyntaxKey::Attribute) {
         if (!_logic->validateResendAttribute(current.arg.reference, superMode)) {
            codeScope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
         current = current.nextNode();
      }

      ObjectInfo target = source;
      if (superMode) {
         if (source.kind == ObjectKind::SelfLocal) {
            source.kind = ObjectKind::SuperLocal;
            target = source;
         }
         else if (source.kind == ObjectKind::Class) {
            // NOTE : for the constructor redirect - use the class parent as a target (still keeping the original class
            // as a parameter)
            ClassInfo classInfo;
            if (_logic->defineClassInfo(*codeScope.moduleScope, classInfo, source.reference, true)) {
               target = mapClassSymbol(codeScope, classInfo.header.parentRef);
            }
            else codeScope.raiseError(errInvalidOperation, node);
         }
         else codeScope.raiseError(errInvalidOperation, node);
      }

      ExprScope scope(&codeScope);
      ArgumentsInfo arguments;

      mssg_t messageRef = mapMessage(scope, current, false, false, false);

      if (!test(messageRef, FUNCTION_MESSAGE))
         arguments.add(source);

      ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments, EAttr::NoPrimitives);

      retVal = compileMessageOperation(writer, scope, node, target, messageRef,
         implicitSignatureRef, arguments, EAttr::None);

      scope.syncStack();
   }
   else assert(false);

   SyntaxNode current = node.nextNode();
   if (current == SyntaxKey::CodeBlock) {
      MethodScope* methodScope = Scope::getScope<MethodScope>(codeScope, Scope::ScopeLevel::Method);
      if (methodScope->constructorMode) {
         // HOTFIX : overwrite self variable for the redirect constructor code
         
         // stack should contains current self reference
         // the original message should be restored if it is a generic method
         writer.appendNode(BuildKey::Assigning, methodScope->selfLocal);
      }

      retVal = compileCode(writer, codeScope, current, methodScope->closureMode);
   }

   return retVal;
}

void Compiler :: compileDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(codeScope, Scope::ScopeLevel::Class);

   compileMultidispatch(writer, codeScope, *classScope, node, true);
}

void Compiler :: compileConstructorDispatchCode(BuildTreeWriter& writer, CodeScope& codeScope, 
   ClassScope& classClassScope, SyntaxNode node)
{
   compileMultidispatch(writer, codeScope, classClassScope, node, true);
}

void Compiler :: compileDirectResendCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   mssg_t dispatchMessage = node.arg.reference;

   SyntaxNode targetNode = node.findChild(SyntaxKey::Target);
   ref_t disptachTarget = targetNode.arg.reference;

   writer.newNode(BuildKey::DirectResendOp, dispatchMessage);
   writer.appendNode(BuildKey::Type, disptachTarget);
   writer.closeNode();
}

void Compiler :: compileDispatchProberCode(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   mssg_t message = scope.getMessageID();
   mssg_t dispatchMessage = node.arg.reference;

   BuildKey op = BuildKey::DispatchingOp;
   ref_t    opRef = classScope->info.attributes.get({ dispatchMessage, ClassAttribute::OverloadList });
   if (!opRef)
      scope.raiseError(errIllegalOperation, node);

   if (test(classScope->info.header.flags, elSealed) || test(dispatchMessage, STATIC_MESSAGE)) {
      op = BuildKey::SealedDispatchingOp;
   }

   writer.newNode(op, opRef);
   writer.appendNode(BuildKey::Message, dispatchMessage);
   writer.closeNode();

   SyntaxNode targetNode = node.findChild(SyntaxKey::Target);
   if (targetNode != SyntaxKey::None) {
      writer.newNode(BuildKey::DirectCallOp, message);
      writer.appendNode(BuildKey::Type, targetNode.arg.reference);
      writer.closeNode();
   }
   else {
      writer.appendNode(BuildKey::AccSwapping, 1);

      writer.appendNode(BuildKey::RedirectOp, overwriteArgCount(scope.moduleScope->buildins.invoke_message, 2));
   }
}

mssg_t Compiler :: compileByRefHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node, mssg_t byRefHandler)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(invokerScope, Scope::ScopeLevel::Class);

   MethodScope privateScope(classScope);
   // copy parameters
   for (auto it = invokerScope.parameters.start(); !it.eof(); ++it) {
      privateScope.parameters.add(it.key(), *it);
   }

   // add byref return arg
   TypeInfo refType = { V_WRAPPER, invokerScope.info.outputRef };
   auto sizeInfo = _logic->defineStructSize(*invokerScope.moduleScope, resolvePrimitiveType(invokerScope, refType, false));

   int offset = invokerScope.parameters.count() + 1u;
   privateScope.parameters.add(RETVAL_ARG, { offset, refType, sizeInfo.size });

   privateScope.message = byRefHandler | STATIC_MESSAGE;
   privateScope.info.hints |= (ref_t)MethodHint::Private;
   privateScope.info.hints |= (ref_t)MethodHint::Sealed;

   // HOTFIX : mark it as stacksafe if required
   if (_logic->isEmbeddableStruct(classScope->info))
      privateScope.info.hints |= (ref_t)MethodHint::Stacksafe;

   privateScope.byRefReturnMode = true;

   classScope->info.methods.add(privateScope.message, privateScope.info);
   classScope->save();

   compileMethod(writer, privateScope, node);

   return privateScope.message;
}

void Compiler :: compileByRefHandlerInvoker(BuildTreeWriter& writer, MethodScope& methodScope, CodeScope& codeScope, mssg_t handler, ref_t targetRef)
{
   writer.appendNode(BuildKey::OpenFrame);

   // stack should contains current self reference
   // the original message should be restored if it is a generic method
   methodScope.selfLocal = codeScope.newLocal();
   writer.appendNode(BuildKey::Assigning, methodScope.selfLocal);

   // calling the byref handler
   ExprScope scope(&codeScope);
   ArgumentsInfo arguments;

   ObjectInfo tempRetVal = declareTempLocal(scope, targetRef, false);

   ObjectInfo target = methodScope.mapSelf(true);
   arguments.add(target);
   for (auto it = methodScope.parameters.start(); !it.eof(); ++it) {
      arguments.add(methodScope.mapParameter(it.key(), EAttr::None));
   }
   addByRefRetVal(arguments, tempRetVal);

   ref_t signRef = getSignature(scope.module, handler);
   /*ObjectInfo retVal = */compileMessageOperation(writer, scope, {}, target, handler,
      signRef, arguments, EAttr::AlreadyResolved);

   // return temp variable
   writeObjectInfo(writer, scope,
      boxArgument(writer, scope, tempRetVal, false, true, false));

   scope.syncStack();

   writer.appendNode(BuildKey::CloseFrame);
}

void Compiler :: writeParameterDebugInfo(BuildTreeWriter& writer, MethodScope& scope)
{
   writer.newNode(BuildKey::ParameterInfo);

   if (!scope.functionMode) {
      writer.newNode(BuildKey::Parameter, "self");
      writer.appendNode(BuildKey::Index, -1);
      writer.closeNode();
   }

   int prefix = scope.functionMode ? 0 : -1;
   for (auto it = scope.parameters.start(); !it.eof(); ++it) {
      auto paramInfo = *it;

      if (paramInfo.size > 0) {
         if (paramInfo.typeInfo.typeRef == scope.moduleScope->buildins.intReference) {
            writer.newNode(BuildKey::IntParameterAddress, it.key());
         }
         else if (paramInfo.typeInfo.typeRef == scope.moduleScope->buildins.longReference) {
            writer.newNode(BuildKey::LongParameterAddress, it.key());
         }
         else if (paramInfo.typeInfo.typeRef == scope.moduleScope->buildins.realReference) {
            writer.newNode(BuildKey::RealParameterAddress, it.key());
         }
         else {
            writer.newNode(BuildKey::ParameterAddress, it.key());

            ref_t classRef = paramInfo.typeInfo.typeRef;
            if (isPrimitiveRef(classRef))
               classRef = resolvePrimitiveType(scope, paramInfo.typeInfo, true);

            ustr_t className = scope.moduleScope->module->resolveReference(classRef);
            if (isWeakReference(className)) {
               IdentifierString fullName(scope.module->name());
               fullName.append(className);

               writer.appendNode(BuildKey::ClassName, *fullName);
            }
            else writer.appendNode(BuildKey::ClassName, className);
         }
      }
      else writer.newNode(BuildKey::Parameter, it.key());

      writer.appendNode(BuildKey::Index, prefix - paramInfo.offset);
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   CodeScope codeScope(&scope);
   if (scope.info.byRefHandler) {
      mssg_t privateImplementation = compileByRefHandler(writer, scope, node, scope.info.byRefHandler);

      beginMethod(writer, scope, node, BuildKey::Method, false);
      compileByRefHandlerInvoker(writer, scope, codeScope, privateImplementation, scope.info.outputRef);
      codeScope.syncStack(&scope);
      endMethod(writer, scope);
   }
   else {
      beginMethod(writer, scope, node, BuildKey::Method, true);

      CodeScope codeScope(&scope);

      SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
      switch (current.key) {
         case SyntaxKey::CodeBlock:
         case SyntaxKey::ReturnExpression:
         case SyntaxKey::ResendDispatch:
         case SyntaxKey::Redirect:
            compileMethodCode(writer, scope, codeScope, node, false);
            break;
         case SyntaxKey::DirectResend:
            compileDirectResendCode(writer, codeScope, current);
            break;
         case SyntaxKey::Importing:
            writer.appendNode(BuildKey::Import, current.arg.reference);
            break;
         case SyntaxKey::WithoutBody:
            scope.raiseError(errNoBodyMethod, node);
            break;
         case SyntaxKey::RedirectDispatch:
            compileDispatchCode(writer, codeScope, current);
            break;
         case SyntaxKey::RedirectTryDispatch:
            compileDispatchProberCode(writer, codeScope, current);
            break;
         default:
            break;
      }

      codeScope.syncStack(&scope);
      endMethod(writer, scope);
   }
}

bool Compiler :: isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool& isProtectedDefConst)
{
   ref_t actionRef = getAction(message);
   if (actionRef == getAction(scope.moduleScope->buildins.constructor_message)) {
      return true;
   }
   else if (actionRef == getAction(scope.moduleScope->buildins.protected_constructor_message)) {
      isProtectedDefConst = true;

      return true;
   }
   else if (getArgCount(message)) {
      ref_t dummy = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, dummy);
      if (actionName.compare(CONSTRUCTOR_MESSAGE2)) {
         isProtectedDefConst = true;
         return true;
      }
      else return actionName.endsWith(CONSTRUCTOR_MESSAGE);
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

   // call field initializers if available for default constructor
   if(classScope->info.methods.exist(scope.moduleScope->buildins.init_message)) {
      ExprScope exprScope(classScope);
      ArgumentsInfo args;

      compileMessageOperation(writer, exprScope, node, scope.mapSelf(), scope.moduleScope->buildins.init_message,
         0, args, EAttr::None);
   }
}

void Compiler :: compileConstructor(BuildTreeWriter& writer, MethodScope& scope,
   ClassScope& classClassScope, SyntaxNode node)
{
   bool isProtectedDefConst = false;
   bool isDefConvConstructor = isDefaultOrConversionConstructor(scope, scope.message, isProtectedDefConst);

   mssg_t defConstrMssg = scope.moduleScope->buildins.constructor_message;
   mssg_t protectedDefConstructor = classClassScope.getMssgAttribute(defConstrMssg, ClassAttribute::ProtectedAlias);
   if (protectedDefConstructor) {
      // if protected default constructor is declared - use it
      defConstrMssg = protectedDefConstructor;
      isProtectedDefConst = true;
   }
   else if (classClassScope.info.methods.exist(defConstrMssg | STATIC_MESSAGE)) {
      // if private default constructor is declared - use it
      defConstrMssg = defConstrMssg | STATIC_MESSAGE;
   }

   beginMethod(writer, scope, node, BuildKey::Method, true);

   CodeScope codeScope(&scope);
   ref_t classFlags = codeScope.getClassFlags();

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   bool retExpr = current == SyntaxKey::ReturnExpression;

   bool newFrame = false;
   if (current == SyntaxKey::ResendDispatch || current == SyntaxKey::RedirectDispatch) {
      // do not create a frame for resend operation
   }
   else if (isDefConvConstructor) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }
   else if (retExpr) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }
   else if (!test(classFlags, elDynamicRole) && classClassScope.info.methods.exist(defConstrMssg)) 
   {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;

      if (!retExpr) {
         // NOTE : the named constructor should be polymorphic, depending on the message target
         writer.appendNode(BuildKey::Local, -1);
         //writer.appendNode(BuildKey::ClassReference, scope.getClassRef());

         writer.newNode(BuildKey::CallOp, defConstrMssg);
         writer.appendNode(BuildKey::Index, 1); // built-in constructor entry should be the second entry in VMT
         writer.closeNode();
      }
   }
   // if it is a dynamic object implicit constructor call is not possible
   else scope.raiseError(errIllegalConstructor, node);

   if (current == SyntaxKey::RedirectDispatch) {
      compileConstructorDispatchCode(writer, codeScope, classClassScope, node);
   }
   else {
      if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
         // if it is a default / conversion (unnamed) constructor
         // it should create the object
         compileDefConvConstructorCode(writer, scope, node, newFrame);
      }
      switch (current.key) {
         case SyntaxKey::CodeBlock:
         case SyntaxKey::ResendDispatch:
            compileMethodCode(writer, scope, codeScope, node, newFrame);
            break;
         case SyntaxKey::ReturnExpression:
            compileRetExpression(writer, codeScope, current);
            writer.appendNode(BuildKey::CloseFrame);
            break;
         case SyntaxKey::None:
            if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
               writer.appendNode(BuildKey::CloseFrame);
               break;
            }
         default:
            throw InternalError(errFatalError);
      }
   }

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: initializeMethod(ClassScope& scope, MethodScope& methodScope, SyntaxNode current)
{
   methodScope.message = current.arg.reference;
   methodScope.info = scope.info.methods.get(methodScope.message);
   methodScope.functionMode = test(methodScope.message, FUNCTION_MESSAGE);
   methodScope.isEmbeddable = methodScope.checkHint(MethodHint::Stacksafe);
   methodScope.isExtension = methodScope.checkHint(MethodHint::Extension);

   declareVMTMessage(methodScope, current, false, false);

   if (methodScope.info.outputRef) {
      validateType(scope, methodScope.info.outputRef, current, false, false);

      if (methodScope.checkHint(MethodHint::VirtualReturn)) {
         TypeInfo refType = { V_WRAPPER, methodScope.info.outputRef };

         SizeInfo sizeInfo = {};
         // add byref return arg
         if (_logic->isEmbeddable(*scope.moduleScope, methodScope.info.outputRef)) {
            sizeInfo = _logic->defineStructSize(*scope.moduleScope, 
               resolvePrimitiveType(scope, refType, false));
         }

         int offset = methodScope.parameters.count() + 1u;
         methodScope.parameters.add(RETVAL_ARG, { offset, refType, sizeInfo.size });

         methodScope.byRefReturnMode = true;
      }
   }
}

void Compiler :: compileRedirectDispatcher(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope, SyntaxNode node)
{
   writer.appendNode(BuildKey::DispatchingOp);

   // new stack frame
   writer.appendNode(BuildKey::OpenFrame);

   // stack should contains current self reference
   // the original message should be restored if it is a generic method
   scope.selfLocal = codeScope.newLocal();
   writer.appendNode(BuildKey::Assigning, scope.selfLocal);

   ExprScope exprScope(&codeScope);

   ObjectInfo mssgVar = declareTempStructure(exprScope, { sizeof(mssg_t)});
   writer.appendNode(BuildKey::SavingIndex, mssgVar.reference);

   ObjectInfo retVal = { };

   SyntaxNode bodyNode = node.firstChild(SyntaxKey::ScopeMask);
   switch (bodyNode.key) {
   case SyntaxKey::Expression:
      retVal = compileExpression(writer, exprScope, bodyNode, 0, EAttr::None);
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         break;
   }

   retVal = boxArgument(writer, exprScope, retVal, false, true, false);

   writeObjectInfo(writer, exprScope, retVal);

   writer.appendNode(BuildKey::LoadingIndex, mssgVar.reference);
   writer.appendNode(BuildKey::RedirectOp);

   exprScope.syncStack();

   writer.appendNode(BuildKey::CloseFrame, -1);

   writer.appendNode(BuildKey::DispatchingOp);
}

void Compiler :: compileDispatcherMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   CodeScope codeScope(&scope);

   beginMethod(writer, scope, node, BuildKey::Method, false);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   switch (current.key) {
      case SyntaxKey::Importing:
         writer.appendNode(BuildKey::Import, current.arg.reference);
         break;
      case SyntaxKey::Redirect:
         compileRedirectDispatcher(writer, scope, codeScope, current);
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         break;
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
            initializeMethod(scope, methodScope, current);

#ifdef FULL_OUTOUT_INFO
            IdentifierString messageName;
            ByteCodeUtil::resolveMessageName(messageName, scope.module, methodScope.message);

            _errorProcessor->info(infoCurrentMethod, *messageName);
#endif // FULL_OUTOUT_INFO

            // if it is a dispatch handler
            if (methodScope.message == scope.moduleScope->buildins.dispatch_message) {
               compileDispatcherMethod(writer, methodScope, current);
            }
            // if it is an abstract one
            else if (methodScope.checkHint(MethodHint::Abstract)) {
               compileAbstractMethod(writer, methodScope, current, scope.abstractMode);
            }
            // if it is an initializer
            else if (methodScope.checkHint(MethodHint::Initializer)) {
               compileInitializerMethod(writer, methodScope, node);
            }
            // if it is a normal method
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

   // if the VMT conatains newly defined generic / variadic handlers, overrides default one
   if (testany(scope.info.header.flags, /*elWithGenerics | */elWithVariadics)
      && scope.info.methods.get(scope.moduleScope->buildins.dispatch_message).inherited)
   {
      //MethodScope methodScope(&scope);
      //methodScope.message = scope.moduleScope->dispatch_message;

      //scope.include(methodScope.message);

      //scope.info.header.flags |= elWithCustomDispatcher;

      //compileDispatcher();
      assert(false);

      //compileDispatcher(writer, SNode(), methodScope,
      //   lxClassMethod,
      //   test(scope.info.header.flags, elWithGenerics),
      //   test(scope.info.header.flags, elWithVariadics));

      // overwrite the class info
      scope.save();
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
            initializeMethod(classClassScope, methodScope, current);

#ifdef FULL_OUTOUT_INFO
            IdentifierString messageName;
            ByteCodeUtil::resolveMessageName(messageName, scope.module, methodScope.message);

            _errorProcessor->info(infoCurrentMethod, *messageName);
#endif // FULL_OUTOUT_INFO

            compileMethod(writer, methodScope, current);
            break;
         }
         case SyntaxKey::Constructor:
         {
            MethodScope methodScope(&scope);
            initializeMethod(classClassScope, methodScope, current);
            methodScope.constructorMode = true;

#ifdef FULL_OUTOUT_INFO
            IdentifierString messageName;
            ByteCodeUtil::resolveMessageName(messageName, scope.module, methodScope.message);

            _errorProcessor->info(infoCurrentMethod, *messageName);
#endif // FULL_OUTOUT_INFO

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
   beginMethod(writer, scope, node, BuildKey::Method, false);

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

   mssg_t multiMethod = defineMultimethod(scope, methodScope.message, false);
   if (multiMethod) {
      methodScope.info.multiMethod = multiMethod;
      methodScope.info.outputRef = V_AUTO;
   }

   compileClosureMethod(writer, methodScope, node);

   // HOTFIX : inject an output type if required or used super class
   if (methodScope.info.outputRef == V_AUTO) {
      methodScope.info.outputRef = scope.moduleScope->buildins.superReference;
   }

   ref_t closureRef = resolveClosure(scope, methodScope.message, methodScope.info.outputRef);
   if (closureRef) {
      parentRef = closureRef;
   }
   else throw InternalError(errClosureError);

   declareClassParent(parentRef, scope, node);
   generateClassFlags(scope, elNestedClass);

   // handle the abstract flag
   if (test(scope.info.header.flags, elAbstract)) {
      scope.abstractBasedMode = true;
      scope.info.header.flags &= ~elAbstract;
   }

   auto m_it = scope.info.methods.getIt(methodScope.message);
   if (!m_it.eof()) {
      (*m_it).inherited = true;
   }
   else scope.info.methods.add(methodScope.message, methodScope.info);

   if (multiMethod) {
      SyntaxTree classTree;
      SyntaxTreeWriter classWriter(classTree);

      // build the class tree
      classWriter.newNode(SyntaxKey::Root);
      classWriter.newNode(SyntaxKey::Class, scope.reference);

      SyntaxNode classNode = classWriter.CurrentNode();
      injectVirtualMultimethod(classNode, SyntaxKey::Method, *scope.moduleScope, scope.info, multiMethod);

      classWriter.closeNode();
      classWriter.closeNode();

      SyntaxNode current = classNode.firstChild();
      while (current != SyntaxKey::None) {
         generateMethodDeclaration(scope, current, false);

         current = current.nextNode();
      }

      _logic->injectOverloadList(this, *scope.moduleScope, scope.info, scope.reference);

      compileVMT(writer, scope, classNode);
   }      

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
#ifdef FULL_OUTOUT_INFO
   // info
   ustr_t name = scope.module->resolveReference(scope.reference);
   _errorProcessor->info(infoCurrentClass, name);
#endif // FULL_OUTOUT_INFO

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   writer.newNode(BuildKey::Class, scope.reference);
   writer.appendNode(BuildKey::Path, *ns->sourcePath);

   compileVMT(writer, scope, node);
   writer.closeNode();

   scope.save();

   // compile explicit symbol
   compileClassSymbol(writer, scope);
}

void Compiler :: compileClassClass(BuildTreeWriter& writer, ClassScope& classClassScope, ClassScope& scope,
   SyntaxNode node)
{
   NamespaceScope* ns = Scope::getScope<NamespaceScope>(classClassScope, Scope::ScopeLevel::Namespace);

   writer.newNode(BuildKey::Class, classClassScope.reference);
   writer.appendNode(BuildKey::Path, *ns->sourcePath);

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

inline ref_t safeMapWeakReference(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver, ustr_t forward)
{
   ustr_t resolved = forwardResolver->resolveForward(forward);
   if (!emptystr(resolved)) {
      // HOTFIX : for the standard module the references should be mapped forcefully
      if (moduleScope->isStandardOne()) {
         return moduleScope->module->mapReference(resolved + getlength(STANDARD_MODULE), false);
      }
      else return moduleScope->module->mapReference(resolved, false);
   }
   else return 0;
}

bool Compiler :: reloadMetaData(ModuleScopeBase* moduleScope, ustr_t name)
{
   if (name.compare(PREDEFINED_MAP)) {
      moduleScope->predefined.clear();

      auto predefinedInfo = moduleScope->getSection(PREDEFINED_MAP, mskAttributeMapRef, true);
      if (predefinedInfo.section) {
         _logic->readAttributeMap(predefinedInfo.section, moduleScope->predefined);
      }
   }
   else if (name.compare(ATTRIBUTES_MAP)) {
      moduleScope->attributes.clear();

      auto attributeInfo = moduleScope->getSection(ATTRIBUTES_MAP, mskAttributeMapRef, true);
      if (attributeInfo.section) {
         _logic->readAttributeMap(attributeInfo.section, moduleScope->attributes);
      }
   }
   else if (name.compare(OPERATION_MAP)) {
      moduleScope->operations.clear();

      auto operationInfo = moduleScope->getSection(OPERATION_MAP, mskTypeMapRef, true);
      if (operationInfo.section) {
         _logic->readTypeMap(operationInfo.module, operationInfo.section, moduleScope->operations, moduleScope);
      }
   }
   else if (name.compare(ALIASES_MAP)) {
      moduleScope->aliases.clear();

      auto aliasInfo = moduleScope->getSection(ALIASES_MAP, mskTypeMapRef, true);
      if (aliasInfo.section) {
         _logic->readTypeMap(aliasInfo.module, aliasInfo.section, moduleScope->aliases, moduleScope);
      }
   }
   else return false;

   return true;
}

void Compiler :: prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver)
{
   reloadMetaData(moduleScope, PREDEFINED_MAP);
   reloadMetaData(moduleScope, ATTRIBUTES_MAP);
   reloadMetaData(moduleScope, OPERATION_MAP);
   reloadMetaData(moduleScope, ALIASES_MAP);

   // cache the frequently used references
   moduleScope->buildins.superReference = safeMapReference(moduleScope, forwardResolver, SUPER_FORWARD);
   moduleScope->buildins.intReference = safeMapReference(moduleScope, forwardResolver, INTLITERAL_FORWARD);
   moduleScope->buildins.longReference = safeMapReference(moduleScope, forwardResolver, LONGLITERAL_FORWARD);
   moduleScope->buildins.realReference = safeMapReference(moduleScope, forwardResolver, REALLITERAL_FORWARD);
   moduleScope->buildins.shortReference = safeMapReference(moduleScope, forwardResolver, INT16LITERAL_FORWARD);
   moduleScope->buildins.byteReference = safeMapReference(moduleScope, forwardResolver, INT8LITERAL_FORWARD);
   moduleScope->buildins.literalReference = safeMapReference(moduleScope, forwardResolver, LITERAL_FORWARD);
   moduleScope->buildins.wideReference = safeMapReference(moduleScope, forwardResolver, WIDELITERAL_FORWARD);
   moduleScope->buildins.messageReference = safeMapReference(moduleScope, forwardResolver, MESSAGE_FORWARD);
   moduleScope->buildins.wrapperTemplateReference = safeMapReference(moduleScope, forwardResolver, WRAPPER_FORWARD);
   moduleScope->buildins.arrayTemplateReference = safeMapReference(moduleScope, forwardResolver, ARRAY_FORWARD);
   moduleScope->buildins.closureTemplateReference = safeMapWeakReference(moduleScope, forwardResolver, CLOSURE_FORWARD);
   moduleScope->buildins.dwordReference = safeMapReference(moduleScope, forwardResolver, DWORD_FORWARD);

   moduleScope->branchingInfo.typeRef = safeMapReference(moduleScope, forwardResolver, BOOL_FORWARD);
   moduleScope->branchingInfo.trueRef = safeMapReference(moduleScope, forwardResolver, TRUE_FORWARD);
   moduleScope->branchingInfo.falseRef = safeMapReference(moduleScope, forwardResolver, FALSE_FORWARD);

   // cache the frequently used messages
   moduleScope->buildins.dispatch_message = encodeMessage(
      moduleScope->module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);
   moduleScope->buildins.protected_constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE2, 0, false),
         0, FUNCTION_MESSAGE);
   moduleScope->buildins.init_message =
      encodeMessage(moduleScope->module->mapAction(INIT_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE | STATIC_MESSAGE);
   moduleScope->buildins.invoke_message = encodeMessage(
      moduleScope->module->mapAction(INVOKE_MESSAGE, 0, false), 1, FUNCTION_MESSAGE);
   moduleScope->buildins.add_message =
      encodeMessage(moduleScope->module->mapAction(ADD_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.sub_message =
      encodeMessage(moduleScope->module->mapAction(SUB_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.mul_message =
      encodeMessage(moduleScope->module->mapAction(MUL_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.div_message =
      encodeMessage(moduleScope->module->mapAction(DIV_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.band_message =
      encodeMessage(moduleScope->module->mapAction(BAND_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.bor_message =
      encodeMessage(moduleScope->module->mapAction(BOR_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.bxor_message =
      encodeMessage(moduleScope->module->mapAction(BXOR_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.refer_message =
      encodeMessage(moduleScope->module->mapAction(REFER_MESSAGE, 0, false),
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
   moduleScope->buildins.negate_message =
      encodeMessage(moduleScope->module->mapAction(NEGATE_MESSAGE, 0, false),
         1, PROPERTY_MESSAGE);
   moduleScope->buildins.value_message =
         encodeMessage(moduleScope->module->mapAction(VALUE_MESSAGE, 0, false),
            1, PROPERTY_MESSAGE);
   moduleScope->buildins.notequal_message =
      encodeMessage(moduleScope->module->mapAction(NOTEQUAL_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.less_message =
      encodeMessage(moduleScope->module->mapAction(LESS_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.notless_message =
      encodeMessage(moduleScope->module->mapAction(NOTLESS_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.greater_message =
      encodeMessage(moduleScope->module->mapAction(GREATER_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.notgreater_message =
      encodeMessage(moduleScope->module->mapAction(NOTGREATER_MESSAGE, 0, false),
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
   moduleScope->superVar.copy(moduleScope->predefined.retrieve<ref_t>("@super", V_SUPER_VAR,
      [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      }));

   if (!moduleScope->tapeOptMode)
      moduleScope->tapeOptMode = _tapeOptMode;
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
   // declare all member identifiers
   declareModuleIdentifiers(moduleScope, root);

   // declare all members
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

inline SyntaxNode newVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, Visibility visibility)
{
   ref_t hints = (ref_t)MethodHint::Multimethod;
   switch (visibility) {
      case Visibility::Protected:
         hints |= (ref_t)MethodHint::Protected;
         break;
      case Visibility::Internal:
         hints |= (ref_t)MethodHint::Internal;
         break;
      default:
         break;
   }

   SyntaxNode methodNode = classNode.appendChild(methodType, message);
   methodNode.appendChild(SyntaxKey::Hints, hints);

   return methodNode;
}

inline SyntaxNode newVirtualMethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message)
{
   SyntaxNode methodNode = classNode.appendChild(methodType, message);

   return methodNode;
}

void Compiler :: injectVirtualEmbeddableWrapper(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ClassInfo& info, mssg_t message)
{
   MethodInfo methodInfo = {};

   auto m_it = info.methods.getIt(message);
   bool found = !m_it.eof();
   if (found)
      methodInfo = *m_it;

   if (!found || methodInfo.inherited) {
      SyntaxNode methodNode = newVirtualMethod(classNode, methodType, message);
      methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated method

      mssg_t resendMessage = message | STATIC_MESSAGE;

      SyntaxNode resendOp = methodNode.appendChild(SyntaxKey::DirectResend, resendMessage);
      resendOp.appendChild(SyntaxKey::Target, classNode.arg.reference);
   }
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ClassInfo& info, mssg_t message, bool inherited, ref_t outputRef, Visibility visibility)
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

      pos_t firstArg = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
      for (pos_t i = firstArg; i < argCount; i++) {
         signatures[signatureLen++] = scope.buildins.superReference;
      }
      ref_t signRef = scope.module->mapAction(actionName,
         scope.module->mapSignature(signatures, signatureLen, false), false);

      resendMessage = encodeMessage(signRef, argCount, flags);
   }

   injectVirtualMultimethod(classNode, methodType, message, resendMessage, resendTarget, outputRef, visibility);
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message,
   mssg_t resendMessage, ref_t resendTarget, ref_t outputRef, Visibility visibility)
{
   SyntaxNode methodNode = newVirtualMultimethod(classNode, methodType, message, visibility);
   methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated multi-method

   if (outputRef)
      methodNode.appendChild(SyntaxKey::OutputType, outputRef);

   if (message == resendMessage) {
      SyntaxNode dispatchOp = methodNode.appendChild(SyntaxKey::RedirectDispatch);
      if (resendTarget)
         dispatchOp.appendChild(SyntaxKey::Target, resendTarget);
   }
   else methodNode.appendChild(SyntaxKey::RedirectDispatch, resendMessage);
}

void Compiler :: injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, 
   mssg_t message, mssg_t dispatchMessage, ref_t originalTarget)
{
   SyntaxNode methodNode = newVirtualMethod(classNode, methodType, message);
   methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated method

   SyntaxNode dispatchOp = methodNode.appendChild(SyntaxKey::RedirectTryDispatch, dispatchMessage);
   if (originalTarget)
      dispatchOp.appendChild(SyntaxKey::Target, originalTarget);
}

void Compiler :: injectVirtualTryDispatch(SyntaxNode classNode, SyntaxKey methodType, ClassInfo& info, 
   mssg_t message, mssg_t dispatchMessage, bool inherited)
{
   ref_t originalTarget = 0;
   if (inherited) {
      // if virtual multi-method handler is overridden
      // redirect to the parent one
      originalTarget = info.header.parentRef;
   }

   injectVirtualTryDispatch(classNode, methodType, message, dispatchMessage, originalTarget);
}

void Compiler :: injectInitializer(SyntaxNode classNode, SyntaxKey methodType, mssg_t message)
{
   SyntaxNode methodNode = classNode.appendChild(methodType, message);
   methodNode.appendChild(SyntaxKey::Hints, (ref_t)MethodHint::Initializer);
   methodNode.appendChild(SyntaxKey::Autogenerated);

   methodNode.appendChild(SyntaxKey::FieldInitializer);
}

void Compiler :: injectDefaultConstructor(ClassScope& scope, SyntaxNode node, bool protectedOne)
{
   mssg_t message = protectedOne ? scope.moduleScope->buildins.protected_constructor_message
                        : scope.moduleScope->buildins.constructor_message;
   MethodHint hints = (MethodHint)((ref_t)MethodHint::Constructor | (ref_t)MethodHint::Normal);
   if (protectedOne) 
      hints = (ref_t)hints | MethodHint::Protected;

   SyntaxNode methodNode = node.appendChild(SyntaxKey::Constructor, message);
   methodNode.appendChild(SyntaxKey::Autogenerated);
   methodNode.appendChild(SyntaxKey::Hints, (ref_t)hints);
   methodNode.appendChild(SyntaxKey::OutputType, scope.reference);
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
