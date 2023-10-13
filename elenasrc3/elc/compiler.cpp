//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "compiler.h"
#include "langcommon.h"
#include <errno.h>
#include <utility>

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
      case ObjectKind::OuterSelf:
      case ObjectKind::ClassSelf:
      case ObjectKind::ConstructorSelf:
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

inline bool validateGenericClosure(ref_t* signature, size_t length)
{
   for (size_t i = 1; i < length; i++) {
      if (signature[i - 1] != signature[i])
         return false;
   }

   return true;
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

inline ref_t mapIntConstant(Compiler::Scope& scope, int integer)
{
   String<char, 20> s;

   // convert back to string as a decimal integer
   s.appendInt(integer, 16);

   return scope.moduleScope->module->mapConstant(s.str());
}

inline bool isConstant(ObjectKind kind)
{
   switch (kind) {
      case ObjectKind::IntLiteral:
      case ObjectKind::Float64Literal:
      case ObjectKind::LongLiteral:
      case ObjectKind::StringLiteral:
      case ObjectKind::WideStringLiteral:
      case ObjectKind::Singleton:
         return true;
      default:
         return false;
   }
}

inline bool areConstants(ArgumentsInfo& args)
{
   for (size_t i = 0; i < args.count(); i++) {
      if (!isConstant(args[i].kind))
         return false;
   }

   return true;
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

void Interpreter :: addTypeListItem(ref_t dictionaryRef, ref_t symbolRef, ref_t mask)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskTypeListRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeArrayEntry(dictionary, symbolRef | mask);
}

void Interpreter :: addConstArrayItem(ref_t dictionaryRef, ref_t item, ref_t mask)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskConstArray, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeArrayReference(dictionary, item | mask);
}

void Interpreter :: addIntArrayItem(ref_t dictionaryRef, int value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskConstant, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   MemoryWriter writer(dictionary);
   writer.writeDWord(value);
}

void Interpreter :: addLongArrayItem(ref_t dictionaryRef, long long value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskConstant, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   MemoryWriter writer(dictionary);
   writer.writeQWord(value);
}

void Interpreter :: addFloatArrayItem(ref_t dictionaryRef, double value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskConstant, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   MemoryWriter writer(dictionary);
   writer.write(&value, sizeof(value));
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

void Interpreter :: copyConstCollection(ref_t sourRef, ref_t destRef, bool byValue)
{
   ref_t mask = byValue ? mskConstant : mskConstArray;

   auto sourceInfo = _scope->getSection(_scope->module->resolveReference(sourRef), mask, true);
   MemoryBase* target = _scope->module->mapSection(destRef | mask, false);

   MemoryReader reader(sourceInfo.section);
   MemoryWriter writer(target);
   
   writer.copyFrom(&reader, sourceInfo.section->length());

   for (auto it = RelocationMap::Iterator(sourceInfo.section->getReferences()); !it.eof(); ++it) {
      ref_t currentMask = it.key() & mskAnyRef;
      ref_t currentRef = it.key() & ~mskAnyRef;
      
      target->addReference(_scope->importReference(sourceInfo.module, currentRef) | currentMask, *it);
   }
}

ObjectInfo Interpreter :: createConstCollection(ref_t arrayRef, ref_t typeRef, ArgumentsInfo& args, bool byValue)
{
   ref_t mask = byValue ? mskConstant : mskConstArray;
   auto section = _scope->module->mapSection(arrayRef | mask, false);

   for (size_t i = 0; i < args.count(); i++) {
      auto arg = args[i];
      switch (arg.kind) {
         case ObjectKind::StringLiteral:
            addConstArrayItem(arrayRef, arg.reference, mskLiteralRef);
            break;
         case ObjectKind::IntLiteral:
            if (byValue) {
               addIntArrayItem(arrayRef, arg.extra);
            }
            else addConstArrayItem(arrayRef, arg.reference, mskIntLiteralRef);
            break;
         case ObjectKind::LongLiteral:
            if (byValue) {
               ustr_t valStr = _scope->module->resolveConstant(arg.reference);
               long long val = StrConvertor::toLong(valStr, 16);

               addLongArrayItem(arrayRef, val);
            }
            else addConstArrayItem(arrayRef, arg.reference, mskLongLiteralRef);
            break;
         case ObjectKind::Float64Literal:
            if (byValue) {
               ustr_t valStr = _scope->module->resolveConstant(arg.reference);
               double val = StrConvertor::toDouble(valStr);

               addFloatArrayItem(arrayRef, val);
            }
            else addConstArrayItem(arrayRef, arg.reference, mskRealLiteralRef);
            break;
         case ObjectKind::Singleton:
         case ObjectKind::Class:
            addConstArrayItem(arrayRef, arg.reference, mskVMTRef);
            break;
         default:
            assert(false);
            break;
      }
   }

   if (typeRef)
      section->addReference(typeRef | mskVMTRef, (pos_t)-4);

   return { byValue ? ObjectKind::Constant : ObjectKind::ConstArray,
      { typeRef }, arrayRef };
}

bool Interpreter :: evalObjArrayOp(ref_t operator_id, ArgumentsInfo& args)
{
   ObjectInfo loperand = args[0];
   ObjectInfo roperand = args[1];
   if (loperand.kind == ObjectKind::TypeList && roperand.kind == ObjectKind::Symbol) {
      if (operator_id == ADD_ASSIGN_OPERATOR_ID) {
         ref_t mask = mskSymbolRef;

         addTypeListItem(loperand.reference, roperand.reference, mask);

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
         case ObjectKind::ClassSelf:
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
   else if (operator_id == REFERENCE_OPERATOR_ID && loperand.kind == ObjectKind::Class) {
      retVal = { ObjectKind::SelfPackage };

      return true;
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
   extensionTemplates(nullptr),
   extensionTargets(INVALID_REF),
   extensionDispatchers(INVALID_REF),
   declaredExtensions({}),
   intConstants(0)
{
   nsName.copy(*parent->nsName);
   sourcePath.copy(*parent->sourcePath);
   defaultVisibility = parent->defaultVisibility;
   errorProcessor = parent->errorProcessor;
   outerExtensionList = parent->outerExtensionList;
}

void Compiler::NamespaceScope :: addExtension(mssg_t message, ref_t extRef, mssg_t strongMessage)
{
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

ObjectInfo Compiler::NamespaceScope :: defineConstant(SymbolInfo info)
{
   if (info.typeRef == moduleScope->buildins.intReference) {
      int value = 0;
      if(!intConstants.exist(info.valueRef)) {
         auto sectionInfo = moduleScope->getSection(module->resolveReference(info.valueRef), mskConstant, true);
         assert(sectionInfo.section != nullptr);

         MemoryReader reader(sectionInfo.section);
         
         value = reader.getDWord();

         defineIntConstant(info.valueRef, value);
      }
      else value = intConstants.get(info.valueRef);

      return { ObjectKind::IntLiteral, { V_INT32 }, ::mapIntConstant(*this, value), value };
   }
   else if (info.symbolType == SymbolType::ConstantArray) {
      return { ObjectKind::ConstArray, { info.typeRef }, info.valueRef, info.valueRef };
   }
   return { ObjectKind::Constant, { info.typeRef }, info.valueRef };
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
               // if it is an extension
               if (test(classInfo.header.flags, elExtension)) {
                  return { ObjectKind::Extension, { reference }, reference };
               }
               // if it is a stateless symbol
               else if (test(classInfo.header.flags, elStateless)) {
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
                     case SymbolType::Constant:
                     case SymbolType::ConstantArray:
                        if (symbolInfo.valueRef) {
                           // HOTFIX : ingore declared but not defined constant
                           return defineConstant(symbolInfo);
                        }
                        break;
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

   SymbolScope* symbolScope = Scope::getScope<SymbolScope>(*this, ScopeLevel::Symbol);
   if (symbolScope != nullptr) {
      return { ObjectKind::Symbol, { V_DECLARATION }, symbolScope->reference };
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
   auto fieldInfo = info.fields.get(identifier);
   if (!ignoreFields && fieldInfo.offset >= 0) {
      bool readOnly = (test(info.header.flags, elReadOnlyRole) || fieldInfo.readOnly)
         && !EAttrs::test(attr, EAttr::InitializerScope);

      if (test(info.header.flags, elStructureRole)) {
         return { readOnly ? ObjectKind::ReadOnlyFieldAddress : ObjectKind::FieldAddress,
            fieldInfo.typeInfo, fieldInfo.offset };
      }
      else return { readOnly ? ObjectKind::ReadOnlyField : ObjectKind::Field,
         fieldInfo.typeInfo, fieldInfo.offset };
   }
   else if (!ignoreFields && fieldInfo.offset == -2) {
      bool readOnly = (test(info.header.flags, elReadOnlyRole) || fieldInfo.readOnly)
         && !EAttrs::test(attr, EAttr::InitializerScope);

      return { readOnly ? ObjectKind::ReadOnlySelfLocal : ObjectKind::SelfLocal, fieldInfo.typeInfo, 1u, TargetMode::ArrayContent };
   }
   else {
      auto staticFieldInfo = info.statics.get(identifier);
      if (staticFieldInfo.valueRef) {
         if (staticFieldInfo.offset == 0) {
            return { ObjectKind::ClassConstant, staticFieldInfo.typeInfo, staticFieldInfo.valueRef };
         }
         else if (staticFieldInfo.offset < 0) {
            return {
               ObjectKind::StaticConstField,
               staticFieldInfo.typeInfo,
               staticFieldInfo.offset,
               (staticFieldInfo.typeInfo.typeRef == V_PTR32 || staticFieldInfo.typeInfo.typeRef == V_PTR64)
                  ? TargetMode::BoxingPtr : TargetMode::None
            };

         }
         else return { ObjectKind::StaticField, staticFieldInfo.typeInfo, staticFieldInfo.valueRef };
      }

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

ObjectInfo Compiler::ClassScope :: mapMember(ustr_t identifier)
{
   return mapField(identifier, EAttr::InitializerScope);
}

void Compiler::ClassScope :: save()
{
   MemoryBase* section = moduleScope->mapSection(reference | mskMetaClassInfoRef, false);
   section->trim(0);

   // save class meta data
   MemoryWriter metaWriter(section);
   info.save(&metaWriter);
}

// --- ClassClassScope ---

Compiler::ClassClassScope::ClassClassScope(Scope* parent, ref_t reference, Visibility visibility, ClassInfo* classInfo)
   : ClassScope(parent, reference, visibility),
      classInfo(classInfo)
{
}

ObjectInfo Compiler::ClassClassScope :: mapField(ustr_t identifier, ExpressionAttribute attr)
{
   auto retVal = ClassScope::mapField(identifier, attr);
   if (retVal.kind == ObjectKind::Unknown) {
      retVal = mapClassInfoField(*classInfo, identifier, attr, true);
      // NOTE : override the class constant field - to proper reference it
      if (retVal.kind == ObjectKind::StaticConstField)
         retVal.kind = ObjectKind::ClassStaticConstField;
   }

   return retVal;
};


// --- Compiler::MethodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent) :
   Scope(parent),
   message(0),
   parameters({}),
   selfLocal(0),
   messageLocalAddress(0),
   reserved1(0),
   reserved2(0),
   reservedArgs(parent->moduleScope->minimalArgList),
   functionMode(false),
   closureMode(false),
   nestedMode(false),
   targetSelfMode(false),
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
   if (!memberMode) {
      return { ObjectKind::Param, { }, -1 };
   }
   else if (selfLocal != 0) {
      if (isExtension) {
         ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);

         return { ObjectKind::Param, { classScope->extensionClassRef }, -1 };
      }
      else if (isEmbeddable) {
         return { ObjectKind::SelfBoxableLocal, { getClassRef(false) }, (ref_t)selfLocal, TargetMode::Conditional };
      }
      else return { ObjectKind::SelfLocal, { getClassRef(false) }, (ref_t)selfLocal };
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapSuper()
{
   ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);

   return { ObjectKind::SuperLocal, { classScope->info.header.parentRef }, selfLocal };
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
         else return { ObjectKind::ParamAddress, local.typeInfo, prefix - local.offset, TargetMode::Conditional };
      }
      else if (local.typeInfo.typeRef == V_ARGARRAY) {
         TargetMode mode = TargetMode::None;
         if (EAttrs::test(attr, EAttr::Variadic))
            mode = TargetMode::UnboxingVarArgument;

         return { ObjectKind::VArgParam, local.typeInfo, prefix - local.offset, mode };
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
   if (!referenceOne) {
      auto paramInfo = mapParameter(identifier, attr);
      if (paramInfo.kind != ObjectKind::Unknown) {
         return paramInfo;
      }
      else if (moduleScope->selfVar.compare(identifier)) {
         if (EAttrs::test(attr, EAttr::Weak) || targetSelfMode) {
            return mapSelf(false);
         }
         else if ((functionMode && !constructorMode) || closureMode || nestedMode) {
            return parent->mapIdentifier(OWNER_VAR, false, attr);
         }
         else return mapSelf();
      }
      else if (moduleScope->superVar.compare(identifier)) {
         if (functionMode || closureMode || nestedMode) {
            return parent->mapIdentifier(identifier, false, attr);
         }
         else return mapSuper();
      }
      else if (moduleScope->receivedVar.compare(identifier) && messageLocalAddress != 0) {
         return { ObjectKind::LocalAddress, { V_MESSAGE }, messageLocalAddress };
      }
   }

   if (constructorMode)
      attr = attr | EAttr::InitializerScope;

   return Scope::mapIdentifier(identifier, referenceOne, attr);
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals({}), localNodes({})
{
   allocated1 = reserved1 = 0;
   allocated2 = reserved2 = 0;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals({}), localNodes({})
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
   if (object.kind == ObjectKind::Local || object.kind == ObjectKind::LocalAddress) {
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

ObjectInfo Compiler::ExprScope :: mapGlobal(ustr_t globalReference)
{
   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(*this, ScopeLevel::Namespace);

   return nsScope->mapGlobal(globalReference, EAttr::None);
}

ObjectInfo Compiler::ExprScope :: mapMember(ustr_t identifier)
{
   MethodScope* methodScope = Scope::getScope<MethodScope>(*this, ScopeLevel::Method);
   if (methodScope != nullptr && moduleScope->selfVar.compare(identifier)) {
      return methodScope->mapSelf();
   }

   ClassScope* classScope = Scope::getScope<ClassScope>(*this, ScopeLevel::Class);
   if (classScope) {
      //if (methodScope)
      return classScope->mapField(identifier, (methodScope != nullptr && methodScope->constructorMode) ? EAttr::InitializerScope : EAttr::None);
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

ObjectInfo Compiler::InlineClassScope :: mapMember(ustr_t identifier)
{
   if (moduleScope->selfVar.compare(identifier)) {
      auto outer = mapSelf();

      return { ObjectKind::OuterSelf, outer.outerObject.typeInfo, outer.reference };
   }

   return mapField(identifier, EAttr::None);
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
   if (identifier.compare(OWNER_VAR)) {
      Outer owner = mapOwner();

      return { ObjectKind::OuterSelf, owner.outerObject.typeInfo, owner.reference };
   }
   else {
      Outer outer = outers.get(identifier);
      if (outer.reference != INVALID_REF) {
         return { ObjectKind::Outer, outer.outerObject.typeInfo, outer.reference, outer.outerObject.reference };
      }
      else {
         outer.outerObject = parent->mapIdentifier(identifier, referenceOne, attr);
         switch (outer.outerObject.kind) {
            case ObjectKind::Field:
            case ObjectKind::ReadOnlyField:
            {
               // handle outer fields in a special way: save only self
               Outer owner = mapParent();

               return { ObjectKind::OuterField, outer.outerObject.typeInfo, owner.reference, outer.outerObject.reference };
            }
            case ObjectKind::Param:
            case ObjectKind::ParamAddress:
            case ObjectKind::Local:
            case ObjectKind::Outer:
            case ObjectKind::OuterField:
            case ObjectKind::OuterSelf:
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

               if (outer.outerObject.kind == ObjectKind::OuterSelf) {
                  return { ObjectKind::OuterSelf, outer.outerObject.typeInfo, outer.reference };
               }
               else return { ObjectKind::Outer, outer.outerObject.typeInfo, outer.reference };
            }
            case ObjectKind::Unknown:
            {
               // check if there is inherited fields
               ObjectInfo fieldInfo = mapField(identifier, EAttr::None);
               if (fieldInfo.kind != ObjectKind::Unknown) {
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
   if (object.kind == ObjectKind::Outer) {
      auto it = outers.start();
      while (!it.eof()) {
         if ((*it).reference == object.reference) {
            if ((*it).outerObject.kind == ObjectKind::Local || (*it).outerObject.kind == ObjectKind::LocalAddress) {
               (*it).updated = true;

               return true;
            }
            else if ((*it).outerObject.kind == ObjectKind::Outer) {
               InlineClassScope* closure = (InlineClassScope*)parent->getScope(Scope::ScopeLevel::Class);
               if (closure->markAsPresaved((*it).outerObject)) {
                  (*it).updated = true;

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

// --- Compiler ---

Compiler :: Compiler(
   PresenterBase* presenter,
   ErrorProcessor* errorProcessor,
   TemplateProssesorBase* templateProcessor,
   CompilerLogic* compilerLogic)
{
   _presenter = presenter;
   _logic = compilerLogic;
   _errorProcessor = errorProcessor;
   _templateProcessor = templateProcessor;

   _optMode = false;
   _tapeOptMode = false;
   _withMethodParamInfo = false;
   _withConditionalBoxing = false;

   _trackingUnassigned = false;
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

mssg_t Compiler :: mapMethodName(MethodScope& scope, pos_t paramCount, ustr_t actionName, ref_t actionRef,
   ref_t flags, ref_t* signature, size_t signatureLen,
   bool withoutWeakMessages, bool noSignature)
{
   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      paramCount = 1;
      // HOTFIX : extension is a special case - target should be included as well for variadic function
      if (scope.isExtension && test(flags, FUNCTION_MESSAGE))
         paramCount++;
   }

   // NOTE : a message target should be included as well for a normal message
   pos_t argCount = test(flags, FUNCTION_MESSAGE) ? 0 : 1;
   argCount += paramCount;

   if (actionRef != 0) {
      // HOTFIX : if the action was already resolved - do nothing
   }
   else if (actionName.length() > 0) {
      ref_t signatureRef = 0;
      if (signatureLen > 0)
         signatureRef = scope.moduleScope->module->mapSignature(signature, signatureLen, false);

      actionRef = scope.moduleScope->module->mapAction(actionName, signatureRef, false);

      if (withoutWeakMessages && noSignature && test(scope.getClassFlags(false), elClosed)) {
         // HOTFIX : for the nested closed class - special handling is requiered
         ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);
         if (!classScope->info.methods.exist(encodeMessage(actionRef, argCount, flags))) {
            actionRef = scope.moduleScope->module->mapAction(actionName, 0, false);
         }
      }
   }
   else return 0;

   return encodeMessage(actionRef, argCount, flags);
}

ref_t Compiler :: retrieveTemplate(NamespaceScope& scope, SyntaxNode node, List<SyntaxNode>& parameters,
   ustr_t prefix, SyntaxKey argKey)
{
   SyntaxNode identNode = node.firstChild(SyntaxKey::TerminalMask);

   IdentifierString templateName;

   if (argKey != SyntaxKey::None) {
      SyntaxNode current = node.firstChild();
      while (current.key != SyntaxKey::None) {
         if (current.key == argKey) {
            // !! refactoring needed
            if (argKey == SyntaxKey::Expression) {
               parameters.add(current.firstChild());
            }
            else parameters.add(current);
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
   TypeAttributes attributes = {};

   TemplateTypeList typeList;
   declareTemplateAttributes(scope, node, typeList, attributes, true, false);
   if (attributes.isNonempty())
      scope.raiseError(errInvalidOperation, node);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, nullptr, SyntaxKey::None);
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
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, postfix, SyntaxKey::Expression);
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
   SyntaxNode typeNode = target.findChild(SyntaxKey::Type, SyntaxKey::TemplateType);
   if (typeNode != SyntaxKey::None) {
      writer.newNode(SyntaxKey::TemplateArg);
      SyntaxTree::copyNode(writer, typeNode, true);

      parameters.add(writer.CurrentNode());

      writer.closeNode();
   }
   writer.closeNode();

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, postfix, SyntaxKey::TemplateArg);
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

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed)
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

         ref_t hints = (*it).hints;

         (*it).inherited = true;
         ++it;

         // private methods are not inherited
         // except for the initializer
         if (test(message, STATIC_MESSAGE) && !test(hints, (ref_t)MethodHint::Initializer))
            scope.info.methods.exclude(message);
      }
   }

   if (!ignoreSealed && test(scope.info.header.flags, elFinal)) {
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

   // cache the class class reference
   if (!scope.moduleScope->cachedClassReferences.exist(parentRef))
      scope.moduleScope->cachedClassReferences.add(parentRef, scope.info.header.classRef);

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

ref_t Compiler :: generateConstant(Scope& scope, ObjectInfo& retVal, ref_t constRef, bool saveScope)
{
   // check if the constant can be resolved immediately
   switch (retVal.kind) {
      case ObjectKind::Singleton:
      case ObjectKind::Constant:
      case ObjectKind::ConstArray:
         return retVal.reference;
      case ObjectKind::StringLiteral:
      case ObjectKind::WideStringLiteral:
      case ObjectKind::IntLiteral:
      case ObjectKind::Float64Literal:
         break;
      default:
         return 0;
   }

   // otherwise we have to create the constant
   ModuleBase* module = scope.module;
   if (!constRef)
      constRef = scope.moduleScope->mapAnonymous("const");

   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   MemoryWriter dataWriter(module->mapSection(constRef | mskConstant, false));
   switch (retVal.kind) {
      case ObjectKind::StringLiteral:
      {
         ustr_t value = module->resolveConstant(retVal.reference);
         if (emptystr(value)) {
            dataWriter.writeChar(0);
         }
         else dataWriter.writeString(value, value.length_pos() + 1);

         retVal.typeInfo = { scope.moduleScope->buildins.literalReference };
         break;
      }
      case ObjectKind::WideStringLiteral:
      {
         ustr_t value = module->resolveConstant(retVal.reference);
         if (!emptystr(value)) {
            WideMessage wideValue(value);

            dataWriter.writeWideString(*wideValue, wideValue.length_pos() + 1);
         }
         else dataWriter.writeWord(0);

         retVal.typeInfo = { scope.moduleScope->buildins.wideReference };
         break;
      }
      case ObjectKind::IntLiteral:
      {
         nsScope->defineIntConstant(constRef, retVal.extra);

         dataWriter.writeDWord(retVal.extra);

         retVal.typeInfo = { scope.moduleScope->buildins.intReference };
         break;
      }
      case ObjectKind::Float64Literal:
      {
         ustr_t valueStr = module->resolveConstant(retVal.reference);
         double value = StrConvertor::toDouble(valueStr);

         dataWriter.write(&value, sizeof(double));

         retVal.typeInfo = { scope.moduleScope->buildins.realReference };
         break;
      }
      default:
         break;
   }

   ref_t typeRef = retrieveStrongType(scope, retVal);
   if (!typeRef)
      return 0;

   dataWriter.Memory()->addReference(typeRef | mskVMTRef, (pos_t)-4);

   // save constant meta info
   if (saveScope) {
      SymbolInfo constantInfo = { SymbolType::Constant, constRef, typeRef, false };
      MemoryWriter metaWriter(module->mapSection(constRef | mskMetaSymbolInfoRef, false), 0);
      constantInfo.save(&metaWriter);
   }

   return constRef;
}

void Compiler :: generateMethodAttributes(ClassScope& scope, SyntaxNode node,
   MethodInfo& methodInfo, bool abstractBased)
{
   mssg_t message = node.arg.reference;

   if (abstractBased || scope.abstractMode) {
      methodInfo.hints &= ~((ref_t)MethodHint::Abstract);
   }

   methodInfo.hints |= node.findChild(SyntaxKey::Hints).arg.reference;

   if (node.existChild(SyntaxKey::Autogenerated)) {
      methodInfo.hints |= (ref_t)MethodHint::Autogenerated;
   }
   else methodInfo.hints &= ~((ref_t)MethodHint::Autogenerated);

   // add a stacksafe attribute for the embeddable structure automatically, except multi-methods
   if (_logic->isEmbeddableStruct(scope.info) && !MethodScope::checkHint(methodInfo, MethodHint::Multimethod))
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
         && message != scope.moduleScope->buildins.constructor_message)
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

   if (MethodScope::checkHint(methodInfo, MethodHint::Generic))
      scope.info.header.flags |= elWithGenerics;

   // adjust objects with custom dispatch handler
   if (message == scope.moduleScope->buildins.dispatch_message && scope.reference != scope.moduleScope->buildins.superReference) {
      scope.info.header.flags |= elWithCustomDispatcher;
   }
}

pos_t Compiler :: saveMetaInfo(ModuleBase* module, ustr_t value, ustr_t postfix)
{
   IdentifierString sectionName(META_PREFIX, postfix);

   MemoryBase* section = module->mapSection(module->mapReference(*sectionName, false) | mskLiteralListRef, false);
   MemoryWriter writer(section);

   pos_t position = writer.position();
   writer.writeString(value);

   return position;
}

void Compiler :: generateParamNameInfo(ClassScope& scope, SyntaxNode node, mssg_t message)
{
   ClassAttributeKey key = { message, ClassAttribute::ParameterName };

   while (scope.info.attributes.exist(key))
      scope.info.attributes.exclude(key);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         ustr_t paramName = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();

         scope.info.attributes.add(key, saveMetaInfo(scope.module, paramName, PARAMETER_NAMES));
      }

      current = current.nextNode();
   }
}

void Compiler :: generateMethodDeclaration(ClassScope& scope, SyntaxNode node, bool closed, bool hideDuplicate)
{
   mssg_t message = node.arg.reference;
   MethodInfo methodInfo = {};

   auto methodIt = scope.info.methods.getIt(message);
   bool existing = !methodIt.eof();
   if (existing)
      methodInfo = *methodIt;

   // check if there is no duplicate method
   if (existing && !methodInfo.inherited) {
      if (hideDuplicate) {
         node.setKey(SyntaxKey::Idle);
      }
      else scope.raiseError(errDuplicatedMethod, node);
   }
   else {
      generateMethodAttributes(scope, node, methodInfo, scope.abstractBasedMode);
      if (_withMethodParamInfo)
         generateParamNameInfo(scope, node, message);

      bool privateOne = MethodScope::checkAnyHint(methodInfo, MethodHint::Private, MethodHint::Initializer);
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

      // HOTFIX : auto generated sealed static methods should be allowed
      if (existing && MethodScope::checkType(methodInfo, MethodHint::Sealed) && !autoMultimethod)
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

inline mssg_t retrieveMethod(VirtualMethodList& implicitMultimethods, mssg_t multiMethod)
{
   return implicitMultimethods.retrieve<mssg_t>(multiMethod, [](mssg_t arg, VirtualMethod current)
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
   else {
      if (!evalInitializers(scope, classNode)) {
         injectInitializer(classNode, SyntaxKey::Method, scope.moduleScope->buildins.init_message);
      }

      if (!test(scope.info.header.flags, elNestedClass) && !test(scope.info.header.flags, elRole)) {
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

         bool inherited = info.methods.exist(tryMessage);;
         injectVirtualTryDispatch(classNode, methodType, info, tryMessage, multiMethod, inherited);
      }
   }
}

void Compiler :: injectVirtualMethods(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ref_t targetRef, ClassInfo& info, VirtualMethodList& implicitMultimethods)
{
   // generate implicit mutli methods
   for (auto it = implicitMultimethods.start(); !it.eof(); ++it) {
      auto methodInfo = *it;
      switch (methodInfo.value2) {
         case VirtualType::Multimethod:
            injectVirtualMultimethod(classNode, methodType, scope, info, methodInfo.value1);
            break;
         case VirtualType::EmbeddableWrapper:
            injectVirtualEmbeddableWrapper(classNode, methodType, scope, targetRef, info, methodInfo.value1, false);
            break;
         case VirtualType::AbstractEmbeddableWrapper:
            injectVirtualEmbeddableWrapper(classNode, methodType, scope, targetRef, info, methodInfo.value1, true);
            break;
         default:
            break;
      }
   }
}

mssg_t Compiler :: defineByRefMethod(ClassScope& scope, SyntaxNode node/*, bool isExtension*/)
{
   ref_t outputRef = node.findChild(SyntaxKey::OutputType).arg.reference;
   // NOTE : the embedable type should be read-only, otherwise it is possible that the changes will be lost
   if (outputRef && _logic->isEmbeddableAndReadOnly(*scope.moduleScope, outputRef)) {
      ref_t actionRef, flags;
      pos_t argCount;
      decodeMessage(node.arg.reference, actionRef, argCount, flags);

      size_t argDiff = /*isExtension ? 0 : */1;

      ref_t signRef = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, signRef);
      ref_t signArgs[ARG_COUNT];
      size_t signLen = scope.module->resolveSignature(signRef, signArgs);
      if (signLen == (size_t)argCount - argDiff) {
         signArgs[signLen++] = resolvePrimitiveType(scope, { V_WRAPPER, outputRef }, true);

         mssg_t byRefMessage = encodeMessage(
            scope.module->mapAction(
               actionName, scope.module->mapSignature(signArgs, signLen, false), false), argCount + 1, flags);

         return byRefMessage;
      }
   }

   return 0;
}

void Compiler ::verifyMultimethods(Scope& scope, SyntaxNode node, SyntaxKey methodKey, ClassInfo& info, VirtualMethodList& implicitMultimethods)
{
   if (_logic->isNeedVerification(info, implicitMultimethods)) {
      SyntaxNode current = node.firstChild();
      while (current != SyntaxKey::None) {
         if (current == methodKey) {
            if(!_logic->verifyMultimethod(*scope.moduleScope, info, current.arg.reference)) {
               scope.raiseError(errNotCompatibleMulti, current.findChild(SyntaxKey::Name));
            }
         }
         current = current.nextNode();
      }
   }
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey, bool closed)
{
   VirtualMethodList implicitMultimethods({});
   bool thirdPassRequired = false;

   // first pass - mark all multi-methods
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodKey) {
         ref_t hints = current.findChild(SyntaxKey::Hints).arg.reference;
         bool withRetOverload = test(hints, (ref_t)MethodHint::RetOverload);

         // HOTFIX : methods with ret overload are special case and should not be  part of the overload list
         mssg_t multiMethod = withRetOverload ? 0 : defineMultimethod(scope, current.arg.reference, scope.extensionClassRef != 0);
         if (multiMethod) {
            //COMPILER MAGIC : if explicit signature is declared - the compiler should contain the virtual multi method
            ref_t hints = (ref_t)MethodHint::Multimethod;
            if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_INTERNAL))
               hints |= (ref_t)MethodHint::Internal;
            if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_PROTECTED))
               hints |= (ref_t)MethodHint::Protected;
            if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_STATIC))
               hints |= (ref_t)MethodHint::Static;

            // mark weak message as a multi-method
            auto m_it = scope.info.methods.getIt(multiMethod);
            if (!m_it.eof()) {
               (*m_it).hints |= hints;
            }
            else if (closed && !test(multiMethod, STATIC_MESSAGE)) {
               // do not declare a new method for the closed class except the private one
               current = current.nextNode();
               continue;
            }
            else {
               MethodInfo info = {};
               info.hints |= hints;
               info.hints |= (ref_t)MethodHint::Predefined;
               info.inherited = true;

               scope.info.methods.add(multiMethod, info);
            }

            current.appendChild(SyntaxKey::Multimethod, multiMethod);

            if (retrieveMethod(implicitMultimethods, multiMethod) == 0) {
               implicitMultimethods.add({ multiMethod, VirtualType::Multimethod });
               thirdPassRequired = true;
            }
         }

         if (methodKey != SyntaxKey::Constructor && !test(hints, (ref_t)MethodHint::Constant)) {
            // HOTFIX : do not generate byref handler for methods returning constant value & variadic method & yieldable
            if ((current.arg.reference & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE && !SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_YIELDABLE)) {
               mssg_t byRefMethod = withRetOverload ? 
                  0 : defineByRefMethod(scope, current/*, scope.extensionClassRef != 0*/);

               if (byRefMethod) {
                  current.appendChild(SyntaxKey::ByRefRetMethod, byRefMethod);

                  // HOTFIX : do not need to generate byref stub for the private method, it will be added later in the code
                  // HOTFIX : ignore the redirect method
                  if ((!test(current.arg.reference, STATIC_MESSAGE) && !current.existChild(SyntaxKey::Redirect))
                     && retrieveMethod(implicitMultimethods, byRefMethod) == 0) 
                  {
                     if (SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_ABSTRACT)) {
                        implicitMultimethods.add({ byRefMethod, VirtualType::AbstractEmbeddableWrapper });
                     }
                     else implicitMultimethods.add({ byRefMethod, VirtualType::EmbeddableWrapper });
                     thirdPassRequired = true;
                  }
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
            generateMethodDeclaration(scope, current, closed, false);
         }
         else thirdPassRequired = true;
      }

      current = current.nextNode();
   }

   //COMPILER MAGIC : if strong signature is declared - the compiler should contain the virtual multi method
   if (implicitMultimethods.count() > 0) {
      injectVirtualMethods(node, methodKey, *scope.moduleScope, scope.reference, scope.info, implicitMultimethods);
   }

   // third pass - do not include overwritten template-based methods
   if (thirdPassRequired) {
      current = node.firstChild();
      while (current != SyntaxKey::None) {
         if (current == methodKey && current.existChild(SyntaxKey::Autogenerated)) {
            generateMethodDeclaration(scope, current, closed, true);
         }
         current = current.nextNode();
      }
   }

   if (implicitMultimethods.count() > 0)
      verifyMultimethods(scope, node, methodKey, scope.info, implicitMultimethods);
}

void Compiler :: generateClassFlags(ClassScope& scope, ref_t declaredFlags)
{
   scope.info.header.flags |= declaredFlags;

   if (test(scope.info.header.flags, elExtension))
      scope.addAttribute(ClassAttribute::ExtensionRef, scope.extensionClassRef);
}

void Compiler :: generateClassStaticField(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs)
{
   ustr_t name = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (scope.info.statics.exist(name)) {
      scope.raiseError(errDuplicatedField, node);
   }

   TypeInfo typeInfo = attrs.typeInfo;
   bool  isConst = attrs.isConstant;
   ref_t flags = scope.info.header.flags;

   if (attrs.size < 0) {
      if (!attrs.inlineArray) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         typeInfo.typeRef = resolveArrayTemplate(*scope.moduleScope, *nsScope->nsName, attrs.typeInfo.typeRef, true);
      }
      else scope.raiseError(errIllegalField, node);
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

inline bool checkPreviousDeclaration(SyntaxNode node, ustr_t name)
{
   SyntaxNode current = node.prevNode();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Field) {
         ustr_t currentName = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();

         if (currentName.compare(name))
            return true;
      }

      current = current.prevNode();
   }

   return false;
}

bool Compiler :: generateClassField(ClassScope& scope, FieldAttributes& attrs, ustr_t name, int sizeHint, 
   TypeInfo typeInfo, bool singleField)
{
   int offset = 0;
   bool  embeddable = attrs.isEmbeddable;
   bool  readOnly = attrs.isReadonly;
   ref_t flags = scope.info.header.flags;

   // a role cannot have fields
   if (test(flags, elStateless))
      return false;

   SizeInfo sizeInfo = {};
   if (typeInfo.isPrimitive()) {
      if (!sizeHint) {
         return false;
      }
      // for primitive types size should be specified
      else sizeInfo.size = sizeHint;
   }
   else if (typeInfo.typeRef)
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, typeInfo.typeRef);

   if (attrs.fieldArray) {
      if (attrs.size > 0) {
         sizeInfo.size *= attrs.size;

         typeInfo.elementRef = typeInfo.typeRef;
         typeInfo.typeRef = _logic->definePrimitiveArray(*scope.moduleScope, typeInfo.elementRef, true);
      }
      else return false;
   }

   if (test(flags, elWrapper) && scope.info.fields.count() > 0) {
      // wrapper may have only one field
      return false;
   }
   else if (embeddable && !attrs.fieldArray) {
      if (!singleField || scope.info.fields.count() > 0)
         return false;

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

         scope.info.fields.add(name, { -2, typeInfo, readOnly });
      }
      else return false;
   }
   else {
      if (scope.info.fields.exist(name)) {
         return false;
      }

      // if it is a structure field
      if (test(scope.info.header.flags, elStructureRole)) {
         if (test(scope.info.header.flags, elWithYieldable))
            return false;

         if (sizeInfo.size <= 0)
            return false;

         if (scope.info.size != 0 && scope.info.fields.count() == 0)
            return false;

         offset = scope.info.size;
         scope.info.size += sizeInfo.size;
         scope.info.fields.add(name, { offset, typeInfo, readOnly });

         if (typeInfo.isPrimitive())
            _logic->tweakPrimitiveClassFlags(scope.info, typeInfo.typeRef);
      }
      else {
         // primitive / virtual classes cannot be declared
         if (sizeInfo.size != 0 && typeInfo.isPrimitive())
            return false;

         scope.info.header.flags |= elNonStructureRole;

         offset = scope.info.fields.count();
         scope.info.fields.add(name, { offset, typeInfo, readOnly });
      }
   }

   return true;
}

void Compiler :: generateClassField(ClassScope& scope, SyntaxNode node,
   FieldAttributes& attrs, bool singleField)
{
   TypeInfo typeInfo = attrs.typeInfo;
   int   sizeHint = attrs.size;

   if (sizeHint == -1) {
      if (singleField && attrs.inlineArray) {
         scope.info.header.flags |= elDynamicRole;
      }
      else if (!test(scope.info.header.flags, elStructureRole)) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         typeInfo.typeRef = resolveArrayTemplate(*scope.moduleScope, *nsScope->nsName, attrs.typeInfo.typeRef, true);
      }
      else scope.raiseError(errIllegalField, node);

      sizeHint = 0;
   }

   ustr_t name = node.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (!generateClassField(scope, attrs, name, sizeHint, typeInfo, singleField))
   {
      if (attrs.overrideMode && checkPreviousDeclaration(node, name)) {
         // override the field type if both declared in the same scope
         auto it = scope.info.fields.getIt(name);
         (*it).typeInfo = typeInfo;
      }
      else scope.raiseError(errIllegalField, node);
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

         if ((attrs.isConstant && !isClassClassMode) || attrs.isStatic) {
            generateClassStaticField(scope, current, attrs);
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
      // generate static fields
      generateClassFields(scope, node, false);
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
   mssg_t mixedUpVariadicMessage = 0;
   _logic->validateClassDeclaration(*scope.moduleScope, _errorProcessor, scope.info, 
      emptyStructure, customDispatcher, withAbstractMethods, mixedUpVariadicMessage);
   if (withAbstractMethods)
      scope.raiseError(errAbstractMethods, node);
   if (emptyStructure)
      scope.raiseError(errEmptyStructure, node.findChild(SyntaxKey::Name));
   if (customDispatcher)
      scope.raiseError(errDispatcherInInterface, node.findChild(SyntaxKey::Name));
   if (mixedUpVariadicMessage) {
      IdentifierString messageName;
      ByteCodeUtil::resolveMessageName(messageName, scope.module, mixedUpVariadicMessage);

      _errorProcessor->info(infoMixedUpVariadic, *messageName);

      scope.raiseError(errMixedUpVariadicMessage, node.findChild(SyntaxKey::Name));
   }

   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info, scope.isClassClass());

   // generate operation list if required
   _logic->injectOverloadList(this, *scope.moduleScope, scope.info, scope.reference);
}

void Compiler :: declareSymbol(SymbolScope& scope, SyntaxNode node)
{
   declareSymbolAttributes(scope, node, false);
   declareSymbolMetaInfo(scope, node);

   scope.save();
}

void Compiler :: declareClassParent(ref_t parentRef, ClassScope& scope, SyntaxNode baseNode)
{
   scope.info.header.parentRef = parentRef;
   InheritResult res = InheritResult::irSuccessfull;
   if (scope.info.header.parentRef != 0) {
      res = inheritClass(scope, scope.info.header.parentRef/*, ignoreFields*/, test(scope.info.header.flags, elVirtualVMT));
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

void Compiler :: resolveClassPostfixes(ClassScope& scope, SyntaxNode node, bool extensionMode)
{
   ref_t parentRef = 0;

   // analizing class postfixes : if it is a parrent, template or inline template
   SyntaxNode parentNode = {};
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            if (!importInlineTemplate(scope, current, INLINE_PREFIX, node))
               scope.raiseError(errInvalidOperation, current);
            break;
         case SyntaxKey::Parent:
         {
            SyntaxNode child = current.firstChild();
            if (child == SyntaxKey::TemplateType) {
               if (!parentRef) {
                  parentNode = current;

                  parentRef = resolveStrongTypeAttribute(scope, child, extensionMode, false);
               }
               else if (!importTemplate(scope, child, node))
                  scope.raiseError(errUnknownTemplate, current);
            }
            else if (!parentRef) {
               parentNode = current;

               parentRef = resolveStrongTypeAttribute(scope, child, extensionMode, false);
            }
            else scope.raiseError(errInvalidSyntax, current);

            break;
         }
         default:
            break;
      }
      //else if (!parentRef) {
      //   parentNode = baseNode;

      //   parentRef = resolveStrongTypeAttribute(scope, baseNode.firstChild(), extensionMode);
      //}

      current = current.nextNode();
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
   Interpreter interpreter(scope.moduleScope, _logic);

   ObjectInfo retVal = evalObject(interpreter, scope, node);

   //ObjectInfo retVal = mapObject(scope, node, EAttr::NoTypeAllowed);
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
         case SyntaxKey::InlinePropertyTemplate:
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
         case SyntaxKey::TemplateType:
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

void Compiler :: declareSymbolMetaInfo(SymbolScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::InlineTemplate:
            if (!importInlineTemplate(scope, current, INLINE_PREFIX, node))
               scope.raiseError(errUnknownTemplate, node);
            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope metaScope(&scope, Scope::ScopeLevel::Symbol);

            evalStatement(metaScope, current);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(scope, current, Visibility::Public, Scope::ScopeLevel::Field);
            break;
      //case SyntaxKey::Name:
      //case SyntaxKey::Type:
      //case SyntaxKey::ArrayType:
      //case SyntaxKey::TemplateType:
      //case SyntaxKey::Attribute:
      //case SyntaxKey::Dimension:
      //case SyntaxKey::EOP:
      //   break;
         default:
      //   scope.raiseError(errInvalidSyntax, node);
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
               importCode(scope, current.firstChild(), noBodyNode);
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
         case SyntaxKey::WithoutBody:
            withoutBody = true;
            noBodyNode = current;
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: declareParameter(MethodScope& scope, SyntaxNode current, bool withoutWeakMessages,
   bool declarationMode, bool& variadicMode, bool& weakSignature, bool& noSignature,
   pos_t& paramCount, ref_t* signature, size_t& signatureLen)
{
   int index = 1 + scope.parameters.count();

   SizeInfo sizeInfo = {};
   TypeInfo paramTypeInfo = {};
   declareArgumentAttributes(scope, current, paramTypeInfo, declarationMode);

   // NOTE : an extension method must be strong-resolved
   if (withoutWeakMessages && !paramTypeInfo.typeRef) {
      paramTypeInfo.typeRef = scope.moduleScope->buildins.superReference;
   }
   else noSignature = false;

   if (!paramTypeInfo.typeRef) {
      paramTypeInfo.typeRef = scope.moduleScope->buildins.superReference;
   }
   else weakSignature = false;

   ustr_t terminal = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();
   if (scope.parameters.exist(terminal))
      scope.raiseError(errDuplicatedLocal, current);

   paramCount++;
   if (paramCount >= ARG_COUNT || variadicMode)
      scope.raiseError(errTooManyParameters, current);

   if (paramTypeInfo.typeRef == V_ARGARRAY) {
      // to indicate open argument list
      variadicMode = true;

      if (isPrimitiveRef(paramTypeInfo.elementRef)) {
         signature[signatureLen++] = resolvePrimitiveType(scope, { paramTypeInfo.elementRef }, declarationMode);
      }
      else signature[signatureLen++] = paramTypeInfo.elementRef;
   }
   else if (paramTypeInfo.isPrimitive()) {
      // primitive arguments should be replaced with wrapper classes
      signature[signatureLen++] = resolvePrimitiveType(scope, paramTypeInfo, declarationMode);
   }
   else signature[signatureLen++] = paramTypeInfo.typeRef;

   if (signature[signatureLen - 1] && !variadicMode)
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

   bool mixinFunction = false;
   bool noSignature = true; // NOTE : is similar to weakSignature except if withoutWeakMessages=true
   // if method has an argument list
   SyntaxNode current = node.findChild(SyntaxKey::Parameter);
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         declareParameter(scope, current, withoutWeakMessages,
            declarationMode, variadicMode, weakSignature, noSignature,
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
         if (paramCount == 0 && unnamedMessage) {
            if (scope.checkHint(MethodHint::Generic) && scope.checkHint(MethodHint::Generic)) {
               if (signatureLen > 0 || !unnamedMessage || scope.checkHint(MethodHint::Function))
                  scope.raiseError(errInvalidHint, node);

               actionStr.copy(GENERIC_PREFIX);
               flags |= CONVERSION_MESSAGE;
            }
            else if (scope.info.outputRef) {
               ref_t signatureRef = scope.moduleScope->module->mapSignature(&scope.info.outputRef, 1, false);
               actionRef = scope.moduleScope->module->mapAction(CAST_MESSAGE, signatureRef, false);
               flags |= CONVERSION_MESSAGE;
            }
            else scope.raiseError(errIllegalMethod, node);

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
      else if (scope.checkHint(MethodHint::Generic) && scope.checkHint(MethodHint::Generic)) {
         if (signatureLen > 0 || !unnamedMessage || scope.checkHint(MethodHint::Function))
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(GENERIC_PREFIX);
         unnamedMessage = false;
      }
      else if (scope.checkHint(MethodHint::Function)) {
         if (!unnamedMessage)
            scope.raiseError(errInvalidHint, node);

         actionStr.copy(INVOKE_MESSAGE);

         flags |= FUNCTION_MESSAGE;
         unnamedMessage = false;

         // Compiler Magic : if it is a generic closure - ignore fixed argument
         if (scope.checkHint(MethodHint::Mixin)) {
            if (variadicMode && validateGenericClosure(signature, signatureLen)) {
               signatureLen = 1;
               mixinFunction = true;
            }
            // mixin function should have a homogeneous signature (i.e. same types) and be variadic
            else scope.raiseError(errIllegalMethod, node);
         }
      }

      if (scope.checkHint(MethodHint::GetAccessor) || scope.checkHint(MethodHint::SetAccessor)) {
         flags |= PROPERTY_MESSAGE;
      }

      if (scope.checkHint(MethodHint::Mixin) && !scope.checkHint(MethodHint::Function))
         // only mixin function is supported
         scope.raiseError(errIllegalMethod, node);

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
               signature, signatureLen, withoutWeakMessages, noSignature);

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
            signature, signatureLen, withoutWeakMessages, noSignature);
         if (unnamedMessage || !scope.message)
            scope.raiseError(errIllegalMethod, node);
      }

      // if it is an explicit constant conversion
      if (constantConversion) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         addExtensionMessage(scope, scope.message, scope.getClassRef(), scope.message,
            scope.getClassVisibility() != Visibility::Public);
      }

      if (mixinFunction) {
         // Compiler Magic : if it is a mixin function - argument size cannot be directly defined
         scope.message = overwriteArgCount(scope.message, 0);
      }
   }
}

ref_t Compiler :: declareClosureParameters(MethodScope& methodScope, SyntaxNode argNode)
{
   IdentifierString messageStr;
   pos_t paramCount = 0;
   ref_t signRef = 0;

   bool weakSingature = true;
   bool noSignature = true;
   bool variadicMode = false;
   ref_t flags = FUNCTION_MESSAGE;
   ref_t signatures[ARG_COUNT];
   size_t signatureLen = 0;
   while (argNode == SyntaxKey::Parameter) {
      declareParameter(methodScope, argNode, false, false,
         variadicMode, weakSingature, noSignature,
         paramCount, signatures, signatureLen);

      if (variadicMode)
         flags |= VARIADIC_MESSAGE;

      argNode = argNode.nextNode();
   }

   messageStr.copy(INVOKE_MESSAGE);
   if (!weakSingature && !noSignature) {
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

void Compiler :: declareMethod(MethodScope& methodScope, SyntaxNode node, bool abstractMode, bool staticNotAllowed)
{
   if (methodScope.checkHint(MethodHint::Static)) {
      if (staticNotAllowed)
         methodScope.raiseError(errIllegalStaticMethod, node);

      node.setKey(SyntaxKey::StaticMethod);

      methodScope.info.hints |= (ref_t)MethodHint::Normal;
   }
   else if (methodScope.checkHint(MethodHint::Constructor)) {
      if (abstractMode && !methodScope.isPrivate() && !methodScope.isProtected()) {
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

   if (methodScope.checkHint(MethodHint::Yieldable)) {
      // raise an error if the method has arguments
      if (getArgCount(methodScope.message) > 1 || (test(methodScope.message, FUNCTION_MESSAGE) && getArgCount(methodScope.message) > 0))
         methodScope.raiseError(errIllegalMethod, node);

      // raise an error if the class is a struct
      // only a single yield method is allowed

      // inject yield context assigning
      node.parentNode()
         .appendChild(SyntaxKey::AssignOperation)
         .appendChild(SyntaxKey::YieldContext, methodScope.message);

      // inject yield context field
      node.parentNode()
         .appendChild(SyntaxKey::Field)
         .appendChild(SyntaxKey::Name)
         .appendChild(SyntaxKey::identifier, YIELD_CONTEXT_FIELD);
   }
}

void Compiler :: declareVMT(ClassScope& scope, SyntaxNode node, bool& withConstructors, bool& withDefaultConstructor, 
   bool& yieldMethodNotAllowed, bool staticNotAllowed)
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
            methodScope.isExtension = scope.extensionClassRef != 0;
            declareMethodAttributes(methodScope, current, methodScope.isExtension);

            if (!current.arg.reference) {
               // NOTE : an extension method must be strong-resolved
               declareVMTMessage(methodScope, current,
                  methodScope.checkHint(MethodHint::Extension), true);

               current.setArgumentReference(methodScope.message);
            }
            else methodScope.message = current.arg.reference;

            if (methodScope.isYieldable()) {
               if (yieldMethodNotAllowed) {
                  scope.raiseError(errIllegalMethod, current);
               }
               else yieldMethodNotAllowed = true;
            }

            declareMethodMetaInfo(methodScope, current);
            declareMethod(methodScope, current, scope.abstractMode, staticNotAllowed);

            if (methodScope.checkHint(MethodHint::Constructor)) {
               withConstructors = true;
               if ((methodScope.message & ~STATIC_MESSAGE) == scope.moduleScope->buildins.constructor_message) {
                  withDefaultConstructor = true;
               }
               else if (getArgCount(methodScope.message) == 0 && (methodScope.checkHint(MethodHint::Protected) 
                  || methodScope.checkHint(MethodHint::Internal)))
               {
                  // check if it is protected / iternal default constructor
                  ref_t dummy = 0;
                  ustr_t actionName = scope.module->resolveAction(getAction(methodScope.message), dummy);
                  if (actionName.endsWith(CONSTRUCTOR_MESSAGE2) || actionName.endsWith(CONSTRUCTOR_MESSAGE))
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

void Compiler :: inheritStaticMethods(ClassScope& scope, SyntaxNode classNode)
{
   // inject the inherited sealed static methods
   for (auto it = scope.info.attributes.start(); !it.eof(); ++it) {
      auto key = it.key();

      if (key.value2 == ClassAttribute::SealedStatic) {
         auto methodInfo = scope.info.methods.get(key.value1);
         if (methodInfo.inherited)
            injectInheritedStaticMethod(classNode, SyntaxKey::StaticMethod, *it, key.value1, methodInfo.outputRef);
      }
   }

   // save the newly declared static methods
   for (auto it = scope.info.methods.start(); !it.eof(); ++it) {
      auto methodInfo = *it;
      mssg_t key = it.key();

      if (!methodInfo.inherited && MethodScope::checkType(methodInfo, MethodHint::Sealed) &&
         MethodScope::checkHint(methodInfo, MethodHint::Static))
      {
         scope.info.attributes.add({ it.key(), ClassAttribute::SealedStatic }, scope.reference);
      }
   }
}

void Compiler :: declareClassClass(ClassScope& classClassScope, SyntaxNode node, ref_t parentRef)
{
   classClassScope.info.header.flags |= elClassClass; // !! IMPORTANT : classclass flags should be set

   declareClassParent(classClassScope.moduleScope->buildins.superReference, classClassScope, node);

   // inherit sealed static methods
   auto parentClass = mapClassSymbol(classClassScope, parentRef);
   ClassInfo parentInfo;
   if (_logic->defineClassInfo(*classClassScope.moduleScope, parentInfo, parentClass.typeInfo.typeRef)) {
      for (auto it = parentInfo.attributes.start(); !it.eof(); ++it) {
         auto key = it.key();

         if (key.value2 == ClassAttribute::SealedStatic) {
            classClassScope.info.attributes.add(key, *it);

            auto methodInfo = parentInfo.methods.get(key.value1);
            methodInfo.inherited = true;
            classClassScope.info.methods.add(key.value1, methodInfo);
         }
      }
   }

   generateClassDeclaration(classClassScope, node, 0);
   inheritStaticMethods(classClassScope, node);

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
   resolveClassPostfixes(scope, node, extensionDeclaration/*, lxParent*/ );

   ref_t declaredFlags = 0;
   declareClassAttributes(scope, node, declaredFlags);

   // NOTE : due to implementation the field meta information should be analyzed before
   // declaring VMT
   declareFieldMetaInfos(scope, node);

   bool withConstructors = false;
   bool withDefConstructor = false;
   bool yieldMethodNotAllowed = test(scope.info.header.flags, elWithYieldable) || test(declaredFlags, elStructureRole);
   declareVMT(scope, node, withConstructors, withDefConstructor,
      yieldMethodNotAllowed, false);

   if (yieldMethodNotAllowed && !test(scope.info.header.flags, elWithYieldable) && !test(declaredFlags, elStructureRole)) {
      // HOTFIX : trying to figure out if the yield method was declared inside declareVMT
      declaredFlags |= elWithYieldable;
   }

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

   if (scope.visibility == Visibility::Public)
      scope.info.attributes.add({ 0, ClassAttribute::RuntimeLoadable }, INVALID_REF);

   // save declaration
   scope.save();

   // declare class class if it available
   if (scope.info.header.classRef != scope.reference && scope.info.header.classRef != 0) {
      ClassScope classClassScope((NamespaceScope*)scope.parent, scope.info.header.classRef, scope.visibility);

      if (!withDefConstructor &&!scope.abstractMode && !test(scope.info.header.flags, elDynamicRole)) {
         // if default constructor has to be created
         injectDefaultConstructor(scope, node, withConstructors);
      }

      declareClassClass(classClassScope, node, scope.info.header.parentRef);

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
         case SyntaxKey::ExtensionTemplate:
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

   if (ns.declaredExtensions.count() > 0) {
      declareModuleExtensionDispatcher(ns, node);

      ns.declaredExtensions.clear();
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
            declareSymbolAttributes(symbolScope, current, true);

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
      _logic->readExtMessageEntry(sectionInfo.module, sectionInfo.section, ns.extensions, ns.extensionTemplates, ns.moduleScope);
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

void Compiler :: copyParentNamespaceExtensions(NamespaceScope& source, NamespaceScope& target)
{
   for (auto it = source.extensions.start(); !it.eof(); it++) {
      auto ext = *it;

      target.extensions.add(it.key(), { ext.value1, ext.value2 });
   }
}

void Compiler :: declareNamespace(NamespaceScope& ns, SyntaxNode node, bool ignoreImport, bool ignoreExtensions)
{
   // load the namespace name if available
   SyntaxNode nameNode = node.findChild(SyntaxKey::Name);
   if (nameNode == SyntaxKey::Name) {
      if (ns.nsName.length() > 0)
         ns.nsName.append('\'');

      ns.nsName.append(nameNode.firstChild(SyntaxKey::TerminalMask).identifier());
   }

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
                  ns.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, current.findChild(SyntaxKey::Name));
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

ObjectInfo Compiler :: evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id, bool ignoreErrors)
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

   return mapObject(scope, node, mode);
}

ObjectInfo Compiler :: evalPropertyOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors)
{
   SyntaxNode lnode = node.firstChild();

   ObjectInfo loperand = evalObject(interpreter, scope, lnode);
   mssg_t message = mapMessage(scope, node.findChild(SyntaxKey::Message), true, false, false);

   switch (loperand.kind) {
      case ObjectKind::Class:
      {
         CheckMethodResult result = {};
         bool found = _logic->resolveCallType(*scope.moduleScope, retrieveStrongType(scope, loperand), message, result);
         if (result.constRef) {
            NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

            return nsScope->defineObjectInfo(result.constRef, EAttr::None, true);
         }
         break;
      }
      default:
         if (ignoreErrors) {
            return {};
         }
         else scope.raiseError(errCannotEval, node);
         break;
   }

   if (!ignoreErrors)
      scope.raiseError(errCannotEval, node);

   return {};
}

ObjectInfo Compiler :: evalCollection(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool anonymousOne)
{
   SyntaxNode current = node.firstChild();

   ref_t collectionTypeRef = 0;
   ref_t elementTypeRef = 0;

   if (!anonymousOne) {
      ObjectInfo typeInfo = evalObject(interpreter, scope, current);
      if (typeInfo.kind != ObjectKind::Class)
         scope.raiseError(errInvalidOperation, node);

      current = current.nextNode();

      collectionTypeRef = retrieveStrongType(scope, typeInfo);

      ClassInfo collectionInfo;
      if (!_logic->defineClassInfo(*scope.moduleScope, collectionInfo, collectionTypeRef, false, true))
         scope.raiseError(errInvalidOperation, node);

      if (!test(collectionInfo.header.flags, elDynamicRole))
         scope.raiseError(errInvalidOperation, node);

      // if the array was already created
      if (node.arg.reference)
         return { ObjectKind::ConstArray, { collectionTypeRef }, node.arg.reference };

      auto fieldInfo = *(collectionInfo.fields.start());
      elementTypeRef = retrieveStrongType(scope, { ObjectKind::Object, { fieldInfo.typeInfo.elementRef }, 0 });
   }

   ArgumentsInfo arguments;
   EAttr paramMode = EAttr::Parameter;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         auto argInfo = evalExpression(interpreter, scope, current);

         argInfo.typeInfo.typeRef = retrieveStrongType(scope, argInfo);

         if (!isConstant(argInfo.kind)
            || (elementTypeRef && !_logic->isCompatible(*scope.moduleScope, { elementTypeRef }, argInfo.typeInfo, true)))
         {
            return {};
         }

         arguments.add(argInfo);
      }

      current = current.nextNode();
   }

   ref_t nestedRef = node.arg.reference;
   if (!nestedRef) {
      nestedRef = mapConstantReference(scope);
      if (!nestedRef)
         nestedRef = scope.moduleScope->mapAnonymous();

      node.setArgumentReference(nestedRef);
   }

   bool byValue = _logic->isEmbeddableArray(*scope.moduleScope, collectionTypeRef);

   return interpreter.createConstCollection(nestedRef, collectionTypeRef, arguments, byValue);
}

ObjectInfo Compiler :: evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node, bool ignoreErrors, bool resolveMode)
{
   ObjectInfo retVal = {};

   switch (node.key) {
      case SyntaxKey::Expression:
         retVal = evalExpression(interpreter, scope, node.firstChild(SyntaxKey::DeclarationMask), ignoreErrors, resolveMode);
         break;
      case SyntaxKey::AssignOperation:
      case SyntaxKey::AddAssignOperation:
      case SyntaxKey::NameOperation:
      case SyntaxKey::ReferOperation:
         retVal = evalOperation(interpreter, scope, node, (int)node.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::Object:
         retVal = evalObject(interpreter, scope, node);
         break;
      case SyntaxKey::PropertyOperation:
         retVal = evalPropertyOperation(interpreter, scope, node, ignoreErrors);
         break;
      case SyntaxKey::CollectionExpression:
         retVal = evalCollection(interpreter, scope, node, false);
         break;
      case SyntaxKey::PrimitiveCollection:
         retVal = evalCollection(interpreter, scope, node, true);
         break;
      default:
         if (ignoreErrors) {
            return {};
         }
         else scope.raiseError(errCannotEval, node);
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
   return retVal.kind == ObjectKind::Object || retVal.kind == ObjectKind::Extern || retVal.kind == ObjectKind::FloatExtern
      || retVal.kind == ObjectKind::Symbol;
}

inline void createObject(BuildTreeWriter& writer, ClassInfo& info, ref_t reference)
{
   if (test(info.header.flags, elStructureRole)) {
      writer.newNode(BuildKey::CreatingStruct, info.size);
   }
   else {
      writer.newNode(BuildKey::CreatingClass, info.fields.count());
   }

   writer.appendNode(BuildKey::Type, reference);
   writer.closeNode();

   if (!test(info.header.flags, elStructureRole) && info.fields.count() != 0) {
      writer.appendNode(BuildKey::FillOp, info.fields.count());
   }
}

inline void copyObjectToAcc(BuildTreeWriter& writer, ClassInfo& info, int offset)
{
   if (test(info.header.flags, elStructureRole)) {
      // NOTE : the target must be a stack allocated variable for size < 4
      writer.newNode(BuildKey::CopyingToAcc, offset);
      writer.appendNode(BuildKey::Size, info.size);
   }
   else writer.newNode(BuildKey::FieldAssigning, 0);

   writer.closeNode();
}

// NOTE : index should contain the size
inline void copyArray(BuildTreeWriter& writer, int size)
{
   writer.newNode(BuildKey::CopyingArr);
   writer.appendNode(BuildKey::Size, size);
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
   bool condBoxing = info.mode == TargetMode::Conditional && _withConditionalBoxing;
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

   ObjectInfo lenLocal = {};
   bool isBinaryArray = _logic->isEmbeddableArray(argInfo);
   if (isBinaryArray) {
      int elementSize = -(int)argInfo.size;

      // get the length
      lenLocal = declareTempLocal(scope, V_INT32, false);

      writeObjectInfo(writer, scope, info);
      writer.appendNode(BuildKey::SavingInStack, 0);
      writer.newNode(BuildKey::BinaryArraySOp, LEN_OPERATOR_ID);
      writer.appendNode(BuildKey::Index, lenLocal.argument);
      writer.appendNode(BuildKey::Size, elementSize);
      writer.closeNode();

      // allocate the object
      writeObjectInfo(writer, scope, lenLocal);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.newNode(BuildKey::NewArrayOp, typeRef);
      writer.appendNode(BuildKey::Size, argInfo.size);
      writer.closeNode();

      writer.appendNode(BuildKey::Assigning, tempLocal.argument);

      writeObjectInfo(writer, scope, info);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.appendNode(BuildKey::LoadingIndex, lenLocal.reference);

      writeObjectInfo(writer, scope, tempLocal);

      copyArray(writer, elementSize);
   }
   else if (_logic->isDynamic(argInfo)) {
      // get the length
      lenLocal = declareTempLocal(scope, V_INT32, false);

      writeObjectInfo(writer, scope, info);
      writer.appendNode(BuildKey::SavingInStack, 0);
      writer.newNode(BuildKey::ObjArraySOp, LEN_OPERATOR_ID);
      writer.appendNode(BuildKey::Index, lenLocal.argument);
      writer.closeNode();

      // allocate the object
      writeObjectInfo(writer, scope, lenLocal);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.newNode(BuildKey::NewArrayOp, typeRef);
      writer.closeNode();

      writer.appendNode(BuildKey::Assigning, tempLocal.argument);

      writeObjectInfo(writer, scope, info);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.appendNode(BuildKey::LoadingIndex, lenLocal.reference);

      writeObjectInfo(writer, scope, tempLocal);

      copyArray(writer, 0);
   }
   else {
      writeObjectInfo(writer, scope, info);

      if (condBoxing)
         writer.newNode(BuildKey::StackCondOp, IF_OPERATOR_ID);

      writer.appendNode(BuildKey::SavingInStack, 0);

      createObject(writer, argInfo, typeRef);

      copyObjectToAcc(writer, argInfo, tempLocal.reference);

      if (condBoxing)
         writer.closeNode();

      writer.appendNode(BuildKey::Assigning, tempLocal.argument);
   }

   if (!_logic->isReadOnly(argInfo))
      tempLocal.mode = condBoxing ? TargetMode::ConditionalUnboxingRequired : TargetMode::UnboxingRequired;

   return tempLocal;
}

inline bool isBoxingRequired(ObjectInfo info, bool allowByRefParam)
{
   switch (info.kind) {
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
      case ObjectKind::ParamAddress:
      case ObjectKind::ByRefParamAddress:
      case ObjectKind::SelfBoxableLocal:
      case ObjectKind::FieldAddress:
         return true;
      case ObjectKind::ParamReference:
         if (!allowByRefParam)
            return true;
      default:
         return false;
   }
}

int Compiler :: defineFieldSize(Scope& scope, ObjectInfo info)
{
   int size = 0;

   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);
   auto f_it = classScope->info.fields.start();
   while (!f_it.eof()) {
      auto fieldInfo = *f_it;
      if (fieldInfo.offset == info.reference)
         break;

      ++f_it;
   }

   ++f_it;
   if (!f_it.eof()) {
      auto nextFieldInfo = *f_it;
      size = nextFieldInfo.offset - info.reference;
   }
   else size = classScope->info.size - info.reference;

   return size;
}

ObjectInfo Compiler :: boxPtrLocally(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info)
{
   ObjectInfo tempLocal = declareTempLocal(scope, info.typeInfo.typeRef, false);

   writeObjectInfo(writer, scope, info);
   writer.appendNode(BuildKey::SavingInStack, 0);

   writeObjectInfo(writer, scope, tempLocal);

   writer.appendNode(BuildKey::SetImmediateField);

   return tempLocal;
}

ObjectInfo Compiler :: boxLocally(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info,
   bool stackSafe)
{
   // allocating temporal variable
   ObjectInfo tempLocal = {};
   bool fixedArray = false;
   int fixedSize = 0;
   if ((info.typeInfo.isPrimitive() && _logic->isPrimitiveArrRef(info.typeInfo.typeRef))
      || _logic->isEmbeddableArray(*scope.moduleScope, info.typeInfo.typeRef))
   {
      fixedSize = defineFieldSize(scope, info);

      tempLocal = declareTempStructure(scope, { fixedSize });
      tempLocal.typeInfo = info.typeInfo;

      fixedArray = true;
   }
   else tempLocal = declareTempLocal(scope, info.typeInfo.typeRef, false);

   if (stackSafe) {
      tempLocal.mode = TargetMode::LocalUnboxingRequired;

      scope.tempLocals.add({ info.kind, info.reference }, tempLocal);
   }

   writeObjectInfo(writer, scope, tempLocal);
   writer.appendNode(BuildKey::SavingInStack, 0);

   writeObjectInfo(writer, scope, scope.mapSelf());

   switch (info.kind) {
      case ObjectKind::FieldAddress:
      case ObjectKind::ReadOnlyField:
         writer.newNode(BuildKey::CopyingAccField, info.reference);
         break;
      case ObjectKind::StaticConstField:
         writer.appendNode(BuildKey::ClassOp, CLASS_OPERATOR_ID);
         writer.appendNode(BuildKey::Field, info.reference);
         writer.newNode(BuildKey::CopyingAccField, 0);
         break;
      case ObjectKind::ClassStaticConstField:
         writer.appendNode(BuildKey::Field, info.reference);
         writer.newNode(BuildKey::CopyingAccField, 0);
         break;
      default:
         writer.appendNode(BuildKey::Field, info.reference);
         writer.newNode(BuildKey::CopyingAccField, 0);
         break;
   }
   
   writer.appendNode(BuildKey::Size, tempLocal.extra);
   writer.closeNode();

   if (!stackSafe) {
      ObjectInfo dynamicTempLocal = {};
      if (fixedArray) {
         ref_t typeRef = info.typeInfo.isPrimitive() ? resolvePrimitiveType(scope, info.typeInfo, false) : info.typeInfo.typeRef;

         dynamicTempLocal = declareTempLocal(scope, typeRef, true);

         writer.newNode(BuildKey::CreatingStruct, fixedSize);
         writer.appendNode(BuildKey::Type, typeRef);
         writer.closeNode();

         writer.appendNode(BuildKey::Assigning, dynamicTempLocal.argument);

         writeObjectInfo(writer, scope, tempLocal);
         writer.appendNode(BuildKey::SavingInStack, 0);
         writeObjectInfo(writer, scope, dynamicTempLocal);

         writer.newNode(BuildKey::CopyingToAcc, tempLocal.reference);
         writer.appendNode(BuildKey::Size, fixedSize);
         writer.closeNode();

         dynamicTempLocal.mode = TargetMode::UnboxingRequired;
      }
      else dynamicTempLocal = boxArgument(writer, scope, tempLocal, false, true, false);

      scope.tempLocals.add({ info.kind, info.reference }, dynamicTempLocal);

      return dynamicTempLocal;
   }
   else return tempLocal;
}

ObjectInfo Compiler :: boxArgumentLocally(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info, 
   bool stackSafe, bool forced)
{
   switch (info.kind) {
      case ObjectKind::Field:
      case ObjectKind::Outer:
      case ObjectKind::OuterField:
         if (forced) {
            return boxLocally(writer, scope, info, stackSafe);
         }
         return info;
      case ObjectKind::ReadOnlyFieldAddress:
      case ObjectKind::FieldAddress:
         if (info.argument == 0 && !forced) {
            ObjectInfo retVal = scope.mapSelf();
            // HOTFIX : no conditional boxing in this case
            if (retVal.mode == TargetMode::Conditional)
               retVal.mode = TargetMode::None;

            retVal.typeInfo = info.typeInfo;

            return retVal;
         }
         else return boxLocally(writer, scope, info, stackSafe);
      case ObjectKind::StaticConstField:
      case ObjectKind::ClassStaticConstField:
         if (info.mode == TargetMode::BoxingPtr) {
            return boxPtrLocally(writer, scope, info);
         }
         else return info;
      default:
         return info;
   }
}

ObjectInfo Compiler :: boxVariadicArgument(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info)
{
   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   ref_t elementRef = info.typeInfo.elementRef;
   if (!elementRef)
      elementRef = scope.moduleScope->buildins.superReference;

   ref_t typeRef = resolveArgArrayTemplate(*scope.moduleScope, *nsScope->nsName, elementRef, false);

   ObjectInfo destLocal = declareTempLocal(scope, typeRef);
   ObjectInfo lenLocal = declareTempLocal(scope, scope.moduleScope->buildins.intReference, false);

   // get length
   writeObjectInfo(writer, scope, info);
   writer.appendNode(BuildKey::SavingInStack);
   writer.newNode(BuildKey::VArgSOp, LEN_OPERATOR_ID);
   writer.appendNode(BuildKey::Index, lenLocal.argument);
   writer.closeNode();

   // create  a dynamic array
   writeObjectInfo(writer, scope, lenLocal);
   writer.appendNode(BuildKey::SavingInStack);
   writer.appendNode(BuildKey::NewArrayOp, typeRef);
   compileAssigningOp(writer, scope, destLocal, { ObjectKind::Object, { typeRef }, 0 });

   // copy the content
   // index len
   // src:sp[0]
   // dst:acc
   writer.appendNode(BuildKey::LoadingIndex, lenLocal.argument);
   writeObjectInfo(writer, scope, info);
   writer.appendNode(BuildKey::SavingInStack);
   writeObjectInfo(writer, scope, destLocal);
   writer.appendNode(BuildKey::CopyingArr);

   if (info.typeInfo.typeRef && info.typeInfo.typeRef != typeRef) {
      // if the conversion is required
      ObjectInfo convInfo = convertObject(writer, scope, {}, destLocal, 
         info.typeInfo.typeRef, false);

      compileAssigningOp(writer, scope, destLocal, convInfo);

      destLocal.typeInfo = convInfo.typeInfo;
   }

   return destLocal;
}

ObjectInfo Compiler :: boxArgument(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo info,
   bool stackSafe, bool boxInPlace, bool allowingRefArg, ref_t targetRef)
{
   ObjectInfo retVal = { ObjectKind::Unknown };

   info = boxArgumentLocally(writer, scope, info, stackSafe, false);

   if (!stackSafe && isBoxingRequired(info, allowingRefArg)) {
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
   else if (info.kind == ObjectKind::VArgParam && !stackSafe) {
      retVal = boxVariadicArgument(writer, scope, info);
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
      case ObjectKind::MssgNameLiteral:
         writer.appendNode(BuildKey::MssgNameLiteral, info.reference);
         break;
      case ObjectKind::ExtMssgLiteral:
         writer.appendNode(BuildKey::ExtMssgLiteral, info.reference);
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
      case ObjectKind::Terminator:
         writer.appendNode(BuildKey::TerminatorReference, 0);
         break;
      case ObjectKind::Symbol:
         writer.appendNode(BuildKey::SymbolCall, info.reference);
         break;
      case ObjectKind::Extension:
      case ObjectKind::Class:
      case ObjectKind::ClassSelf:
      case ObjectKind::Singleton:
      case ObjectKind::ConstantRole:
         writer.appendNode(BuildKey::ClassReference, info.reference);
         break;
      case ObjectKind::Constant:
         writer.appendNode(BuildKey::ConstantReference, info.reference);
         break;
      case ObjectKind::ConstArray:
         writer.appendNode(BuildKey::ConstArrayReference, info.reference);
         break;
      case ObjectKind::Param:
      case ObjectKind::SelfLocal:
      case ObjectKind::SuperLocal:
      case ObjectKind::ReadOnlySelfLocal:
      case ObjectKind::Local:
      case ObjectKind::TempLocal:
      case ObjectKind::ParamAddress:
      case ObjectKind::ParamReference:
      case ObjectKind::SelfBoxableLocal:
      case ObjectKind::ByRefParamAddress:
      case ObjectKind::ConstructorSelf:
         writer.appendNode(BuildKey::Local, info.reference);
         break;
      case ObjectKind::LocalField:
         writer.appendNode(BuildKey::Local, info.reference);
         writer.appendNode(BuildKey::Field, info.extra);
         break;
      case ObjectKind::VArgParam:
         writer.appendNode(BuildKey::LocalReference, info.reference);
         break;
      case ObjectKind::LocalReference:
         writer.appendNode(BuildKey::LocalReference, info.reference);
         break;
      case ObjectKind::LocalAddress:
      case ObjectKind::TempLocalAddress:
         writer.appendNode(BuildKey::LocalAddress, info.reference);
         break;
      case ObjectKind::ReadOnlyField:
      case ObjectKind::Field:
      case ObjectKind::Outer:
      case ObjectKind::OuterSelf:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::OuterField:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::Field, info.reference);
         writer.appendNode(BuildKey::Field, info.extra);
         break;
      case ObjectKind::StaticConstField:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::ClassOp, CLASS_OPERATOR_ID);
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::ClassStaticConstField:
         writeObjectInfo(writer, scope, scope.mapSelf());
         writer.appendNode(BuildKey::Field, info.reference);
         break;
      case ObjectKind::StaticField:
         writer.appendNode(BuildKey::StaticVar, info.reference);
         break;
      case ObjectKind::ByRefParam:
         writeObjectInfo(writer, scope, { ObjectKind::Param, info.typeInfo, info.reference });
         writer.appendNode(BuildKey::Field);
         break;
      case ObjectKind::ClassConstant:
         if (info.reference == INVALID_REF)
            throw InternalError(errFatalError);

         writer.appendNode(BuildKey::ConstantReference, info.reference);
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
      if (info.typeInfo.typeRef == V_AUTO) {
         return info.typeInfo.typeRef;
      }
      else return resolvePrimitiveType(scope, info.typeInfo, false);
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
   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   return resolvePrimitiveType(*scope.moduleScope, *nsScope->nsName, typeInfo, declarationMode);
}

ref_t Compiler :: resolvePrimitiveType(ModuleScopeBase& moduleScope, ustr_t ns, TypeInfo typeInfo, 
   bool declarationMode)
{
   switch (typeInfo.typeRef) {
      case V_INT8:
         return moduleScope.buildins.byteReference;
      case V_INT16:
         return moduleScope.buildins.shortReference;
      case V_INT32:
         return moduleScope.buildins.intReference;
      case V_INT64:
         return moduleScope.buildins.longReference;
      case V_FLOAT64:
         return moduleScope.buildins.realReference;
      case V_UINT32:
         return moduleScope.buildins.uintReference;
      case V_STRING:
         return moduleScope.buildins.literalReference;
      case V_WIDESTRING:
         return moduleScope.buildins.wideReference;
      case V_MESSAGE:
         return moduleScope.buildins.messageReference;
      case V_MESSAGENAME:
         return moduleScope.buildins.messageNameReference;
      case V_EXTMESSAGE64:
      case V_EXTMESSAGE128:
         return moduleScope.buildins.extMessageReference;
      case V_FLAG:
         return moduleScope.branchingInfo.typeRef;
      case V_WRAPPER:
         return resolveWrapperTemplate(moduleScope, ns, typeInfo.elementRef, declarationMode);
      case V_INT8ARRAY:
      case V_INT16ARRAY:
      case V_INT32ARRAY:
      case V_BINARYARRAY:
         return resolveArrayTemplate(moduleScope, ns, typeInfo.elementRef, declarationMode);
      case V_NIL:
         return moduleScope.buildins.superReference;
      case V_ARGARRAY:
         return resolveArgArrayTemplate(moduleScope, ns, typeInfo.elementRef, declarationMode);
      case V_OBJARRAY:
         return resolveArrayTemplate(moduleScope, ns, typeInfo.elementRef, declarationMode);
      case V_PTR32:
      case V_PTR64:
         return moduleScope.buildins.pointerReference;
      default:
         return 0;
   }
}

void Compiler :: declareSymbolAttributes(SymbolScope& scope, SyntaxNode node, bool identifierDeclarationMode)
{
   bool constant = false;
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateSymbolAttribute(current.arg.value, scope.visibility, constant, scope.isStatic)) {
               current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            }
            break;
         case SyntaxKey::Type:
         case SyntaxKey::ArrayType:
         case SyntaxKey::TemplateType:
            if (!identifierDeclarationMode)
               scope.info.typeRef = resolveStrongTypeAttribute(scope, current, true, false);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   if (scope.visibility == Visibility::Public) {
      scope.info.loadableInRuntime = true;
   }

   if (constant && !identifierDeclarationMode) {
      scope.info.symbolType = SymbolType::Constant;

      Interpreter interpreter(scope.moduleScope, _logic);
      ObjectInfo operand = evalExpression(interpreter, scope, node.findChild(SyntaxKey::GetExpression).firstChild(), true);
      if (operand.kind == ObjectKind::IntLiteral) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
         nsScope->defineIntConstant(scope.reference, operand.extra);
      }
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
   TypeAttributes attributes = { false, false, false };
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
            // if it is a type attribute
            typeInfo = resolveTypeAttribute(scope, current, attributes, declarationMode, false);
            break;
         case SyntaxKey::ArrayType:
            // if it is a type attribute
            typeInfo = resolveTypeScope(scope, current, attributes, declarationMode, false);
            break;
         case SyntaxKey::Attribute:
            if (!_logic->validateArgumentAttribute(current.arg.reference, attributes))
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
   if (attributes.byRefOne) {
      typeInfo.elementRef = typeInfo.typeRef;
      typeInfo.typeRef = V_WRAPPER;
   }
   else if (attributes.variadicOne) {
      if (typeInfo.typeRef != V_ARGARRAY)
         scope.raiseError(errInvalidOperation, node);
   }
}

ref_t Compiler :: declareMultiType(Scope& scope, SyntaxNode& current, ref_t elementRef)
{
   bool eol = false;
   ArgumentsInfo items;
   items.add({ ObjectKind::Class, { elementRef }, 0 });

   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Type) {
         items.add({ ObjectKind::Class, { resolveStrongTypeAttribute(scope, current, true, false) }, 0 });
      }
      else break;

      current = current.nextNode();
   }

   return resolveTupleClass(scope, current, items);
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
            if (scope.info.outputRef) {
               scope.info.outputRef = declareMultiType(scope, current, scope.info.outputRef);

               continue;
            }
            else scope.info.outputRef = resolveStrongTypeAttribute(scope, current, true, false);
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

void Compiler :: registerTemplateSignature(TemplateScope& scope, SyntaxNode node, IdentifierString& signature)
{
   signature.append(TEMPLATE_PREFIX_NS);

   size_t signIndex = signature.length();

   IdentifierString templateName(node.firstChild(SyntaxKey::TerminalMask).identifier());
   int paramCounter = SyntaxTree::countChild(node, SyntaxKey::TemplateArg);

   templateName.append('#');
   templateName.appendInt(paramCounter);

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   ref_t ref = ns->resolveImplicitIdentifier(*templateName, false, true);
   if (!ref)
      scope.raiseError(errUnknownClass, node);

   ustr_t refName = scope.module->resolveReference(ref);
   if (isWeakReference(refName))
      signature.append(scope.module->name());

   signature.append(refName);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::TemplateArg) {
         SyntaxNode argNode = current.firstChild();

         if (argNode == SyntaxKey::Type) {
            signature.append('&');
            ref_t classRef = resolveStrongTypeAttribute(scope, argNode, false, false);
            if (!classRef)
               scope.raiseError(errUnknownClass, current);

            ustr_t className = scope.module->resolveReference(classRef);
            if (isWeakReference(className))
               signature.append(scope.module->name());

            signature.append(className);
         }
         else if (argNode == SyntaxKey::TemplateArgParameter) {
            signature.append('&');
            signature.append('{');
            signature.appendInt(argNode.arg.value);
            signature.append('}');
         }
         else assert(false);
      }

      current = current.nextNode();
   }

   signature.replaceAll('\'', '@', signIndex);
}

void Compiler :: registerExtensionTemplateMethod(TemplateScope& scope, SyntaxNode& node)
{
   IdentifierString messageName;
   pos_t argCount = 1;
   ref_t flags = 0;
   IdentifierString signaturePattern;
   ustr_t extensionName = scope.module->resolveReference(scope.reference);
   if (isWeakReference(extensionName)) {
      signaturePattern.append(scope.module->name());
   }
   signaturePattern.append(extensionName);
   signaturePattern.append('.');

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Name) {
         messageName.copy(current.firstChild(SyntaxKey::TerminalMask).identifier());
      }
      else if (current == SyntaxKey::Parameter) {
         argCount++;
         signaturePattern.append('/');
         SyntaxNode typeAttr = current.findChild(SyntaxKey::Type, SyntaxKey::ArrayType, SyntaxKey::TemplateArgParameter, SyntaxKey::TemplateType);
         if (typeAttr == SyntaxKey::TemplateArgParameter) {
            signaturePattern.append('{');
            signaturePattern.appendInt(typeAttr.arg.value);
            signaturePattern.append('}');
         }
         else if (typeAttr == SyntaxKey::TemplateType) {
            registerTemplateSignature(scope, typeAttr, signaturePattern);
         }
         else if (typeAttr != SyntaxKey::None) {
            ref_t classRef = resolveStrongTypeAttribute(scope, typeAttr, true, false);

            ustr_t className = scope.module->resolveReference(classRef);
            if (isWeakReference(className))
               signaturePattern.append(scope.module->name());

            signaturePattern.append(className);
         }
         else scope.raiseError(/*errNotApplicable*/errInvalidOperation, current);
      }
      current = current.nextNode();
   }

   mssg_t messageRef = encodeMessage(scope.module->mapAction(*messageName, 0, false), argCount, flags);

   addExtensionTemplateMessage(scope, messageRef, *signaturePattern, false);
}

void Compiler :: registerExtensionTemplate(TemplateScope& scope, SyntaxNode& node)
{
   SyntaxNode current = node.firstChild();
   while (current !=  SyntaxKey::None) {
      if (current == SyntaxKey::Method) {
         registerExtensionTemplateMethod(scope, current);
      }
      current = current.nextNode();
   }
}

void Compiler :: saveTemplate(TemplateScope& scope, SyntaxNode& node)
{
   MemoryBase* target = scope.module->mapSection(scope.reference | mskSyntaxTreeRef, false);

   if (node == SyntaxKey::ExtensionTemplate) {
      registerExtensionTemplate(scope, node);
   }

   SyntaxTree::saveNode(node, target);
}

void Compiler :: saveNamespaceInfo(SyntaxNode node, NamespaceScope* nsScope, bool outerMost)
{
   if (outerMost)
      node.appendChild(SyntaxKey::SourcePath, *nsScope->sourcePath);

   IdentifierString nsFullName(nsScope->module->name());
   if (nsScope->nsName.length() > 0) {
      nsFullName.append("'");
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

   switch (scope.type) {
      case TemplateType::Inline:
         prefix.append(INLINE_PREFIX);
         if (argCount > 0)
            scope.raiseError(errInvalidSyntax, node);
         break;
      case TemplateType::Statement:
         postfix.append('#');
         postfix.appendInt(argCount);
         break;
      default:
         break;
   }

   postfix.append('#');
   postfix.appendInt(paramCount);

   SyntaxNode name = node.findChild(SyntaxKey::Name);
   if (name.nextNode() == SyntaxKey::ComplexName) {
      SyntaxNode secondName = name.nextNode();
      size_t index = 0;
      while (secondName == SyntaxKey::ComplexName) {
         postfix.insert(secondName.firstChild().identifier(), index);
         postfix.insert(":", index);

         index += secondName.firstChild().identifier().length() + 1;

         secondName = secondName.nextNode();
      }
   }

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
         TypeAttributes typeAttributes = {};
         TypeInfo dictTypeInfo = resolveTypeAttribute(scope, current, typeAttributes, true, false);
         if (!typeAttributes.isNonempty() && _logic->isCompatible(*scope.moduleScope, dictTypeInfo, { V_STRING }, true)) {
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

            if (current.arg.reference == V_AUTO)
               typeInfo = { V_AUTO  };

            break;
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
         case SyntaxKey::ArrayType:
            if (!EAttrs::test(mode.attrs, EAttr::NoTypeAllowed)) {
               TypeAttributes attributes = {};
               typeInfo = resolveTypeAttribute(scope, current, attributes, false, false);

               if (attributes.mssgNameLiteral) {
                  mode |= ExpressionAttribute::MssgNameLiteral;
               }
               else if (attributes.newOp) {
                  mode |= ExpressionAttribute::NewOp;
               }
               else if (attributes.typecastOne) {
                  mode |= ExpressionAttribute::CastOp;
               }
               else {
                  if (!attributes.variableOne) {
                     if (attributes.isNonempty())
                        scope.raiseError(errInvalidHint, current);
                  }
                  mode |= ExpressionAttribute::NewVariable;
               }
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

   if (ns->outerExtensionList != nullptr) {
      // COMPILER MAGIC : if it is template extension compilation
      ns->outerExtensionList->add(message, { extRef, strongMessage });
   }
   else {
      IdentifierString sectionName(internalOne ? PRIVATE_PREFIX_NS : "'");
      if (!ns->nsName.empty()) {
         sectionName.append(*ns->nsName);
         sectionName.append('\'');
      }
      sectionName.append(EXTENSION_SECTION);

      MemoryBase* section = scope.module->mapSection(
         scope.module->mapReference(*sectionName, false) | mskMetaExtensionRef, false);

      _logic->writeExtMessageEntry(section, extRef, message, strongMessage);

      ns->declaredExtensions.add(message, { extRef, strongMessage });
   }

   ns->addExtension(message, extRef, strongMessage);
}

void Compiler :: addExtensionTemplateMessage(Scope& scope, mssg_t message, ustr_t pattern, bool internalOne)
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

   _logic->writeExtMessageEntry(section, message, pattern);

   ns->extensionTemplates.add(message, pattern.clone());
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
      case ObjectKind::ClassSelf:
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

ref_t Compiler :: mapTemplateType(Scope& scope, SyntaxNode terminal, pos_t paramCounter)
{
   IdentifierString templateName;
   templateName.append(terminal.identifier());
   templateName.append('#');
   templateName.appendInt(paramCounter);

   // NOTE : check it in declararion mode - we need only reference
   return resolveTypeIdentifier(scope, *templateName, terminal.key, true, false);
}

void Compiler :: declareTemplateAttributes(Scope& scope, SyntaxNode node,
   TemplateTypeList& parameters, TypeAttributes& attributes, bool declarationMode, bool objectMode)
{
   SyntaxNode current = objectMode ? node.nextNode() : node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateTypeScopeAttribute(current.arg.reference, attributes))
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            break;
         case SyntaxKey::TemplateArg:
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
         {
            ref_t typeRef = resolveStrongTypeAttribute(scope, current, declarationMode, attributes.mssgNameLiteral);
            parameters.add(typeRef);

            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

ref_t Compiler :: defineArrayType(Scope& scope, ref_t elementRef, bool declarationMode)
{
   ref_t retVal = _logic->definePrimitiveArray(*scope.moduleScope, elementRef,
      _logic->isEmbeddable(*scope.moduleScope, elementRef));

   if (!retVal && declarationMode)
      retVal = V_OBJARRAY;

   return retVal;
}

ObjectInfo Compiler :: defineArrayType(Scope& scope, ObjectInfo info, bool declarationMode)
{
   ref_t elementRef = info.typeInfo.typeRef;
   ref_t arrayRef = defineArrayType(scope, elementRef, declarationMode);

   info.typeInfo.typeRef = arrayRef;
   info.typeInfo.elementRef = elementRef;

   if (info.mode == TargetMode::Creating)
      info.mode = TargetMode::CreatingArray;

   return info;
}

ref_t Compiler :: resolveTypeTemplate(Scope& scope, SyntaxNode node,
   TypeAttributes& attributes, bool declarationMode, bool objectMode)
{
   TemplateTypeList typeList;
   declareTemplateAttributes(scope, node, typeList, attributes, declarationMode, objectMode);

   SyntaxNode terminalNode = node != SyntaxKey::TemplateType ? node : node.firstChild(SyntaxKey::TerminalMask);
   if (attributes.mssgNameLiteral) {
      if (typeList.count() != 1)
         scope.raiseError(errInvalidOperation, node);

      return typeList.get(0);
   }
   else {
      // HOTFIX : generate a temporal template to pass the type
      SyntaxTree dummyTree;
      List<SyntaxNode> parameters({});
      declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

      ref_t templateRef = mapTemplateType(scope, terminalNode, parameters.count());
      if (!templateRef)
         scope.raiseError(errUnknownClass, terminalNode);

      NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

      return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
         templateRef, parameters, declarationMode, nullptr);
   }
}

ref_t Compiler :: resolveTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t templateRef, 
   ref_t elementRef, bool declarationMode)
{
   if (isPrimitiveRef(elementRef))
      elementRef = resolvePrimitiveType(moduleScope, ns, { elementRef });

   TemplateTypeList typeList;
   typeList.add(elementRef);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(moduleScope.module, typeList, dummyTree, parameters);

   return _templateProcessor->generateClassTemplate(moduleScope, ns,
      templateRef, parameters, declarationMode, nullptr);
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
         templateReference, parameters, false, nullptr);
   }
}

ref_t Compiler :: resolveWrapperTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode)
{
   if (!elementRef)
      elementRef = moduleScope.buildins.superReference;

   return resolveTemplate(moduleScope, ns, moduleScope.buildins.wrapperTemplateReference, elementRef, declarationMode);
}

ref_t Compiler :: resolveArrayTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode)
{
   return resolveTemplate(moduleScope, ns, moduleScope.buildins.arrayTemplateReference, elementRef, declarationMode);
}

ref_t Compiler :: resolveArgArrayTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t elementRef, bool declarationMode)
{
   return resolveTemplate(moduleScope, ns, moduleScope.buildins.argArrayTemplateReference, elementRef, declarationMode);
}

TypeInfo Compiler :: resolveTypeScope(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
   bool declarationMode, bool allowRole)
{
   ref_t elementRef = 0;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateTypeScopeAttribute(current.arg.reference, attributes))
               scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
            break;
         case SyntaxKey::Type:
            elementRef = resolveStrongTypeAttribute(scope, current, declarationMode, false);
            break;
         case SyntaxKey::identifier:
         case SyntaxKey::reference:
            elementRef = resolveTypeIdentifier(scope, current.identifier(), node.key, declarationMode, allowRole);
            break;
         case SyntaxKey::ArrayType:
            elementRef = resolvePrimitiveType(scope, resolveTypeAttribute(scope, current, attributes, declarationMode, allowRole), declarationMode);
            break;
         default:
            assert(false);
            break;
      }

      current = current.nextNode();
   }

   if (node == SyntaxKey::ArrayType) {
      if (attributes.variadicOne) {
         return { V_ARGARRAY, elementRef };
      }
      else return { defineArrayType(scope, elementRef, declarationMode), elementRef };
   }
   else return {};
}

TypeInfo Compiler :: resolveTypeAttribute(Scope& scope, SyntaxNode node, TypeAttributes& attributes,
   bool declarationMode, bool allowRole)
{
   TypeInfo typeInfo = {};
   switch (node.key) {
      case SyntaxKey::TemplateArg:
         typeInfo = resolveTypeAttribute(scope, node.firstChild(), attributes, declarationMode, allowRole);
         break;
      case SyntaxKey::Type:
      {
         if (node.arg.reference)
            return { node.arg.reference };

         SyntaxNode current = node.firstChild();
         if (current == SyntaxKey::Type || current == SyntaxKey::ArrayType) {
            // !! should be refactored
            typeInfo = resolveTypeAttribute(scope, current, attributes, declarationMode, allowRole);
         }
         else if (current == SyntaxKey::TemplateType) {
            typeInfo.typeRef = resolveTypeTemplate(scope, current, attributes, declarationMode);
         }
         //else if (current == SyntaxKey::Object) {
         //   assert(false);

         //   // NOTE : template type is declared inside object node duee to current syntax grammar
         //   SyntaxNode objNode = current.firstChild();
         //   if (objNode == SyntaxKey::TemplateType) {
         //      typeInfo.typeRef = resolveTypeTemplate(scope, objNode, declarationMode, true);
         //   }
         //   else typeInfo = resolveTypeAttribute(scope, current, declarationMode, allowRole);
         //}
         else if (SyntaxTree::test(current.key, SyntaxKey::TerminalMask)) {
            if (current.nextNode() == SyntaxKey::TemplateArg) {
               // !! should be refactored : TemplateType should be used instead
               typeInfo.typeRef = resolveTypeTemplate(scope, current, attributes, declarationMode);
            }
            else typeInfo.typeRef = resolveTypeIdentifier(scope, current.identifier(), current.key, declarationMode, allowRole);
         }
         else assert(false);
         break;
      }
      case SyntaxKey::TemplateType:
         typeInfo.typeRef = resolveTypeTemplate(scope, node, attributes, declarationMode);
         break;
      case SyntaxKey::ArrayType:
      {
         typeInfo = resolveTypeScope(scope, node, attributes, declarationMode, allowRole);

         if (attributes.variadicOne)
            scope.raiseError(errInvalidOperation, node);
         break;
      }
      default:
         if (SyntaxTree::test(node.key, SyntaxKey::TerminalMask)) {
            typeInfo.typeRef = resolveTypeIdentifier(scope, node.identifier(), node.key, declarationMode, allowRole);
         }
         else assert(false);
         break;
   }

   validateType(scope, typeInfo.typeRef, node, declarationMode, allowRole || attributes.mssgNameLiteral);

   return typeInfo;
}

ref_t Compiler :: resolveStrongTypeAttribute(Scope& scope, SyntaxNode node, bool declarationMode, bool allowRole)
{
   TypeAttributes typeAttributes = {};
   TypeInfo typeInfo = resolveTypeAttribute(scope, node, typeAttributes, declarationMode, allowRole);
   if (typeAttributes.isNonempty())
      scope.raiseError(errInvalidOperation, node);

   if (isPrimitiveRef(typeInfo.typeRef)) {
      return resolvePrimitiveType(scope, typeInfo, declarationMode);
   }
   else return typeInfo.typeRef;
}

int Compiler :: resolveSize(Scope& scope, SyntaxNode node)
{
   Interpreter interpreter(scope.moduleScope, _logic);

   ObjectInfo retVal = evalObject(interpreter, scope, node);

   if (retVal.kind == ObjectKind::IntLiteral) {
      return retVal.extra;
   }
   else {
      scope.raiseError(errInvalidSyntax, node);

      return 0;
   }
}

void Compiler :: readFieldAttributes(ClassScope& scope, SyntaxNode node, FieldAttributes& attrs, bool declarationMode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateFieldAttribute(current.arg.reference, attrs))
               scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Type:
         case SyntaxKey::TemplateType:
            if (!attrs.typeInfo.typeRef) {
               TypeAttributes typeAttributes = {};

               attrs.typeInfo = resolveTypeAttribute(scope, current, typeAttributes, declarationMode, false);
               if (typeAttributes.isNonempty())
                  scope.raiseError(errInvalidHint, current);
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Dimension:
            if (!attrs.size && attrs.typeInfo.typeRef && !attrs.inlineArray) {
               if (current.arg.value) {
                  attrs.size = current.arg.value;
               }
               else attrs.size = resolveSize(scope, current.firstChild(SyntaxKey::TerminalMask));
               attrs.fieldArray = true;
            }
            else scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::ArrayType:
            if (!attrs.size) {
               attrs.size = -1;

               readFieldAttributes(scope, current, attrs, declarationMode);

               if (attrs.typeInfo.isPrimitive())
                  attrs.typeInfo = { resolvePrimitiveType(scope, attrs.typeInfo, declarationMode) };
            }
            else if (attrs.size == -1) {
               // if it is a nested array
               NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

               readFieldAttributes(scope, current, attrs, declarationMode);
               attrs.typeInfo = { resolveArrayTemplate(*scope.moduleScope, *nsScope->nsName, 
                  attrs.typeInfo.typeRef, declarationMode) };
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
   readFieldAttributes(scope, node, attrs, true);

   //HOTFIX : recognize raw data
   if (attrs.typeInfo.isPrimitive()) {
      bool valid = true;
      switch (attrs.typeInfo.typeRef) {
         case V_INTBINARY:
            switch (attrs.size) {
               case 1:
                  attrs.typeInfo.typeRef = V_INT8;
                  attrs.fieldArray = false;
                  break;
               case 2:
                  attrs.typeInfo.typeRef = V_INT16;
                  attrs.fieldArray = false;
                  break;
               case 4:
                  attrs.typeInfo.typeRef = V_INT32;
                  attrs.fieldArray = false;
                  break;
               case 8:
                  attrs.typeInfo.typeRef = V_INT64;
                  attrs.fieldArray = false;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_UINTBINARY:
            switch (attrs.size) {
               case 4:
                  attrs.typeInfo.typeRef = V_UINT32;
                  attrs.fieldArray = false;
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
                  attrs.fieldArray = false;
                  break;
               case 8:
                  attrs.typeInfo.typeRef = V_WORD64;
                  attrs.fieldArray = false;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_EXTMESSAGE:
            switch (scope.moduleScope->ptrSize) {
               case 4:
                  attrs.typeInfo.typeRef = V_EXTMESSAGE64;
                  attrs.size = 8;
                  attrs.fieldArray = false;
                  break;
               case 8:
                  attrs.typeInfo.typeRef = V_EXTMESSAGE128;
                  attrs.size = 16;
                  attrs.fieldArray = false;
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
                  attrs.fieldArray = false;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_SUBJBINARY:
            switch (attrs.size) {
               case 4:
                  attrs.typeInfo.typeRef = V_MESSAGENAME;
                  attrs.fieldArray = false;
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
                  attrs.fieldArray = false;
                  break;
               default:
                  valid = false;
                  break;
            }
            break;
         case V_POINTER:
            switch (attrs.size) {
               case 4:
                  attrs.typeInfo.typeRef = V_PTR32;
                  attrs.fieldArray = false;
                  break;
               case 8:
                  attrs.typeInfo.typeRef = V_PTR64;
                  attrs.fieldArray = false;
                  break;
               case 0:
                  attrs.fieldArray = false;
                  attrs.size = scope.moduleScope->ptrSize;
                  if (attrs.size == 4) {
                     attrs.typeInfo.typeRef = V_PTR32;
                  }
                  else if (attrs.size == 8) {
                     attrs.typeInfo.typeRef = V_PTR64;
                  }
                  else assert(false);
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
         scope.raiseError(errInvalidHint, node.findChild(SyntaxKey::Name));
   }
}

inline int newLocalAddr(int disp, int allocated)
{
   return -disp - allocated;
}

int Compiler :: allocateLocalAddress(Scope& scope, int size, bool binaryArray)
{
   int retVal = 0;

   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);
   if (codeScope != nullptr) {
      if (binaryArray)
         codeScope->allocLocalAddress(4);

      retVal = codeScope->allocLocalAddress(size);
   }
   else {
      SymbolScope* symbolScope = Scope::getScope<SymbolScope>(scope, Scope::ScopeLevel::Symbol);
      assert(symbolScope != nullptr);

      if (binaryArray)
         symbolScope->allocLocalAddress(4);

      retVal = symbolScope->allocLocalAddress(size);
   }

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

bool Compiler :: declareYieldVariable(Scope& scope, ustr_t name, TypeInfo typeInfo)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   FieldAttributes attrs = { typeInfo };

   // NOTE : should return false to indicate that it is not a variable
   return !generateClassField(*classScope, attrs, name, 0, typeInfo, false);
}

bool Compiler :: declareVariable(Scope& scope, SyntaxNode terminal, TypeInfo typeInfo, bool ignoreDuplicate)
{
   int size = 0;
   if (terminal == SyntaxKey::IndexerOperation) {
      // COMPILER MAGIC : if it is a fixed-sized array
      size = resolveArraySize(scope, terminal.firstChild(SyntaxKey::ScopeMask));

      terminal = terminal.findChild(SyntaxKey::Object).findChild(SyntaxKey::identifier);
   }

   ExprScope* exprScope = Scope::getScope<ExprScope>(scope, Scope::ScopeLevel::Expr);
   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);
   MethodScope* methodScope = Scope::getScope<MethodScope>(scope, Scope::ScopeLevel::Method);
   if (codeScope == nullptr) {
      scope.raiseError(errInvalidOperation, terminal);
      return false; // the code will never be reached
   }

   IdentifierString identifier(terminal.identifier());
   if (ignoreDuplicate) {
      auto var = codeScope->mapIdentifier(*identifier, false, EAttr::None);
      switch (var.kind) {
         case ObjectKind::Local:
         case ObjectKind::LocalAddress:
            // exit if the variable with this names does exist
            return false;
         default:
            break;
      }
   }

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
   else if (_logic->isPrimitiveArrRef(variable.typeInfo.typeRef)) {
      // if it is an array reference
      variable.typeInfo.typeRef = resolvePrimitiveType(scope, variable.typeInfo, false);
      variable.typeInfo.elementRef = 0;
   }

   ClassInfo localInfo;
   //bool binaryArray = false;
   if (!_logic->defineClassInfo(*scope.moduleScope, localInfo, variable.typeInfo.typeRef))
      scope.raiseError(errUnknownVariableType, terminal);

   if (variable.typeInfo.typeRef == V_BINARYARRAY)
      // HOTFIX : recognize binary array actual size
      localInfo.size *= _logic->defineStructSize(*scope.moduleScope, variable.typeInfo.elementRef).size;

   if (_logic->isEmbeddableArray(localInfo) && size != 0) {
      //binaryArray = true;
      size = size * (-((int)localInfo.size));

      variable.reference = allocateLocalAddress(*codeScope, size, true);
   }
   else if (_logic->isEmbeddableStruct(localInfo) && size == 0) {
      size = align(_logic->defineStructSize(localInfo).size,
         scope.moduleScope->rawStackAlingment);

      variable.reference = allocateLocalAddress(*codeScope, size, false);
   }
   else if (size != 0) {
      scope.raiseError(errInvalidOperation, terminal);
   }
   else if (methodScope && methodScope->isYieldable()) {
      // NOTE : yieildable method is a special case when the referece variable is declared as a special class field
      return declareYieldVariable(scope, *identifier, typeInfo);
   }
   else variable.reference = codeScope->newLocal();

   if (exprScope)
      exprScope->syncStack();

   if (!codeScope->locals.exist(*identifier)) {
      codeScope->mapNewLocal(*identifier, variable.reference, variable.typeInfo,
         size, true);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);

   if (_trackingUnassigned) {
      if (size > 0) {
         codeScope->localNodes.add(-(int)variable.reference, terminal);
      }
      else codeScope->localNodes.add((int)variable.reference, terminal);
   }

   return true;
}

inline ref_t mapClassConst(ModuleScopeBase* moduleScope, ref_t reference)
{
   IdentifierString name(moduleScope->module->resolveReference(reference));
   name.append(STATICFIELD_POSTFIX);

   return moduleScope->mapAnonymous(*name + 1);
}

inline bool isInherited(ModuleBase* module, ref_t reference, ref_t staticRef)
{
   ustr_t name = module->resolveReference(reference);
   ustr_t statName = module->resolveReference(staticRef);
   size_t len = getlength(name);

   if (statName[len] == '#' && statName.compare(name, len)) {
      return true;
   }
   else return false;
}

bool Compiler :: evalAccumClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo)
{
   auto it = scope.info.statics.getIt(constName);
   assert(!it.eof());

   ref_t collectionTypeRef = retrieveStrongType(scope, constInfo);
   ClassInfo collectionInfo;
   if (!_logic->defineClassInfo(*scope.moduleScope, collectionInfo, collectionTypeRef, false, true))
      scope.raiseError(errInvalidOperation, node);

   if (!test(collectionInfo.header.flags, elDynamicRole))
      scope.raiseError(errInvalidOperation, node);

   bool byValue = _logic->isEmbeddableArray(*scope.moduleScope, collectionTypeRef);

   Interpreter interpreter(scope.moduleScope, _logic);

   bool newOne = true;
   if (constInfo.reference != INVALID_REF) {
      // HOTFIX : inherit accumulating attribute list
      ClassInfo parentInfo;
      scope.moduleScope->loadClassInfo(parentInfo, scope.info.header.parentRef);
      ref_t targtListRef = (*it).valueRef & ~mskAnyRef;
      ref_t parentListRef = parentInfo.statics.get(constName).valueRef & ~mskAnyRef;

      if (parentListRef != INVALID_REF && !isInherited(scope.module, scope.reference, targtListRef)) {
         constInfo.reference = mapClassConst(scope.moduleScope, scope.reference);

         // inherit the parent list
         interpreter.copyConstCollection(parentListRef, constInfo.reference, byValue);
      }

      newOne = false;
   }
   else constInfo.reference = mapClassConst(scope.moduleScope, scope.reference);

   auto fieldInfo = *(collectionInfo.fields.start());
   ref_t elementTypeRef = retrieveStrongType(scope, { ObjectKind::Object, { fieldInfo.typeInfo.elementRef }, 0 });

   ObjectInfo value = evalExpression(interpreter, scope, node);
   if (value.kind == ObjectKind::Symbol && value.reference == scope.reference) {
      // HOTFIX : recognize the class
      value = { ObjectKind::Class, { scope.info.header.classRef }, scope.reference };
   }

   ArgumentsInfo arguments;
   arguments.add(value);

   interpreter.createConstCollection(constInfo.reference, newOne ? collectionTypeRef : 0, arguments, byValue);

   (*it).valueRef = constInfo.reference | (byValue ? mskConstant : mskConstArray);
   if (!(*it).offset) {
      scope.info.header.staticSize++;

      (*it).offset = -((int)scope.info.header.staticSize);
   }

   return true;
}

bool Compiler :: evalClassConstant(ustr_t constName, ClassScope& scope, SyntaxNode node, ObjectInfo& constInfo)
{
   Interpreter interpreter(scope.moduleScope, _logic);
   MetaScope metaScope(&scope, Scope::ScopeLevel::Class);

   auto it = scope.info.statics.getIt(constName);
   assert(!it.eof());

   ObjectInfo retVal = evalExpression(interpreter, metaScope, node, false, false);
   bool setIndex = false;
   switch (retVal.kind) {
      case ObjectKind::SelfName:
         constInfo.typeInfo = { V_STRING };
         constInfo.reference = mskNameLiteralRef;
         setIndex = true;
         break;
      case ObjectKind::SelfPackage:
         constInfo.typeInfo = { };
         constInfo.reference = mskPackageRef;
         setIndex = true;
         break;
      case ObjectKind::StringLiteral:
      case ObjectKind::WideStringLiteral:
      case ObjectKind::IntLiteral:
      case ObjectKind::Float64Literal:
         constInfo.typeInfo = retVal.typeInfo;
         constInfo.reference = generateConstant(scope, retVal, 0);
         break;
      default:
         return false;
   }

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
         SyntaxNode lnode = current.findChild(SyntaxKey::Object, SyntaxKey::YieldContext);
         if (lnode == SyntaxKey::YieldContext) {
            return false;
         }
         else {
            ObjectInfo target = mapObject(scope, lnode, EAttr::None);
            switch (target.kind) {
               case ObjectKind::Field:
                  evalulated = false;
                  break;
               case ObjectKind::ClassConstant:
                  if (target.reference == INVALID_REF) {
                     ustr_t fieldName = lnode.firstChild(SyntaxKey::TerminalMask).identifier();

                     if (evalClassConstant(fieldName,
                        scope, current.firstChild(SyntaxKey::ScopeMask), target))
                     {
                        current.setKey(SyntaxKey::Idle);
                     }
                     else scope.raiseError(errInvalidOperation, current);
                  }
                  break;
               case ObjectKind::StaticField:
                  if (!current.arg.reference) {
                     current.setArgumentReference(compileStaticAssigning(scope, current));
                  }
                  break;
               default:
                  evalulated = false;
                  break;
            }
         }
      }
      else if (current == SyntaxKey::AddAssignOperation) {
         SyntaxNode lnode = current.findChild(SyntaxKey::Object);
         ObjectInfo target = mapObject(scope, lnode, EAttr::None);
         switch (target.kind) {
            case ObjectKind::ClassConstant:
            case ObjectKind::StaticConstField:
               if (evalAccumClassConstant(lnode.firstChild(SyntaxKey::TerminalMask).identifier(),
                  scope, current.firstChild(SyntaxKey::ScopeMask), target))
               {
                  current.setKey(SyntaxKey::Idle);
               }
               else scope.raiseError(errInvalidOperation, current);
               break;
            default:
               evalulated = false;
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

      ref_t classClassRef = scope.moduleScope->cachedClassReferences.get(classRef);
      if (!classClassRef) {
         ClassInfo info;
         scope.moduleScope->loadClassInfo(info, classRef, true);

         classClassRef = info.header.classRef;

         scope.moduleScope->cachedClassReferences.add(classRef, classClassRef);
      }

      retVal.typeInfo = { classClassRef };

      ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);
      if (classScope != nullptr && (classScope->reference == retVal.typeInfo.typeRef))
      {
         retVal.kind = ObjectKind::ClassSelf;
      }

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

ref_t Compiler :: compileStaticAssigning(ClassScope& scope, SyntaxNode node)
{
   ref_t actionRef = 0;

   SyntaxNode rootNode = node.parentNode();
   while (rootNode != SyntaxKey::Class) {
      rootNode = rootNode.parentNode();
   }

   SyntaxNode staticInitializer = rootNode.firstChild(SyntaxKey::StaticInitializerMethod);
   if (staticInitializer == SyntaxKey::None) {
      IdentifierString sectionName(scope.module->resolveReference(scope.reference));
      sectionName.append(INITIALIZER_SECTION);

      actionRef = scope.moduleScope->mapAnonymous(*sectionName);

      staticInitializer = rootNode.appendChild(SyntaxKey::StaticInitializerMethod, actionRef);

      scope.addAttribute(ClassAttribute::Initializer, actionRef);
   }
   else actionRef = staticInitializer.arg.reference;

   SyntaxTreeWriter writer(staticInitializer);
   SyntaxTree::copyNode(writer, node, true);

   return actionRef;
}

ObjectInfo Compiler :: compileExternalOp(BuildTreeWriter& writer, ExprScope& scope, ref_t externalRef,
   bool stdCall, ArgumentsInfo& arguments, ref_t expectedRef)
{
   pos_t count = arguments.count_pos();

   writer.appendNode(BuildKey::Allocating, align(count, scope.moduleScope->stackAlingment));

   TypeInfo retType = { V_INT32 };
   ref_t intArgType = 0;
   BuildKey intArgOp = BuildKey::None;
   switch (scope.moduleScope->ptrSize) {
      case 4:
         intArgType = V_INT32;
         intArgOp = BuildKey::SavingNInStack;
         break;
      case 8:
         retType = { V_INT64 };
         intArgType = V_INT64;
         intArgOp = BuildKey::SavingLInStack;
         break;
      default:
         assert(false);
         break;
   }

   for (pos_t i = count; i > 0; i--) {
      ObjectInfo arg = boxArgumentLocally(writer, scope, arguments[i - 1], true, false);

      writeObjectInfo(writer, scope, arg);
      switch (arg.kind) {
         case ObjectKind::IntLiteral:
            writer.appendNode(BuildKey::SavingNInStack, i - 1);
            break;
         default:
            if (_logic->isCompatible(*scope.moduleScope, { intArgType },
               arg.typeInfo, true))
            {
               writer.appendNode(intArgOp, i - 1);
            }
            // NOTE : it is a duplicate for 32 bit target, but is required for 64 bit one
            else if (_logic->isCompatible(*scope.moduleScope, { V_INT32 },
               arg.typeInfo, true))
            {
               writer.appendNode(BuildKey::SavingNInStack, i - 1);
            }
            else writer.appendNode(BuildKey::SavingInStack, i - 1); // !! temporally - passing dynamic references to the exteranl routines should not be allowed
            break;
      }
   }

   writer.newNode(BuildKey::ExtCallOp, externalRef);

   BuildNode opNode = writer.CurrentNode();

   writer.appendNode(BuildKey::Count, count);

   writer.closeNode();

   if (!stdCall)
      writer.appendNode(BuildKey::Freeing, align(count, scope.moduleScope->stackAlingment));

   if (_logic->isCompatible(*scope.moduleScope, retType, { expectedRef }, true)) {
      retType = { expectedRef  };
   }
   else if (retType.typeRef == V_INT64 &&  _logic->isCompatible(*scope.moduleScope, { V_INT32 }, { expectedRef }, true)) {
      // HOTFIX 64bit : allow to convert the external operation to int32
      retType = { expectedRef };
   }
   else if (retType.typeRef == V_INT32 && _logic->isCompatible(*scope.moduleScope, { V_INT64 }, { expectedRef }, true)) {
      retType = { expectedRef };

      // HOTFIX : special case - returning long in 32 bit mode
      opNode.appendChild(BuildKey::LongMode);
   }
   else if (_logic->isCompatible(*scope.moduleScope, { V_FLOAT64 }, { expectedRef }, true)) {
      retType = { expectedRef };

      return { ObjectKind::FloatExtern, retType, 0 };
   }

   return { ObjectKind::Extern, retType, 0 };
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
      case SET_INDEXER_OPERATOR_ID:
         return scope->buildins.set_refer_message;
      case AND_OPERATOR_ID:
         return scope->buildins.and_message;
      case OR_OPERATOR_ID:
         return scope->buildins.or_message;
      case XOR_OPERATOR_ID:
         return scope->buildins.xor_message;
      default:
         throw InternalError(errFatalError);
   }
}

ObjectInfo Compiler :: declareTempStructure(ExprScope& scope, SizeInfo sizeInfo)
{
   if (sizeInfo.size <= 0)
      return {};

   ObjectInfo retVal = { ObjectKind::TempLocalAddress };
   retVal.reference = allocateLocalAddress(scope, sizeInfo.size, false);
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
   ObjectInfo& loperand, ArgumentsInfo& messageArguments, mssg_t message, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs)
{
   ObjectInfo retVal =  {};

   // resolving a message signature (excluding a target)
   bool weakSignature = false;
   for (pos_t i = 1; i < argLen; i++) {
      if (!arguments[i]) {
         weakSignature = true;
         break;
      }
      else if (isPrimitiveRef(arguments[i])) {
         arguments[i - 1] = resolvePrimitiveType(scope, { arguments[i] }, false);
      }
      else arguments[i - 1] = arguments[i];
   }

   ref_t signRef = (!weakSignature && argLen > 1) ? scope.module->mapSignature(arguments, argLen - 1, false) : 0;

   mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, loperand), expectedRef, message, signRef);
   if (byRefHandler) {
      ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

      addByRefRetVal(messageArguments, tempRetVal);
      // adding mark for optimization routine
      if (tempRetVal.kind == ObjectKind::TempLocalAddress)
         writer.appendNode(BuildKey::ByRefOpMark, tempRetVal.argument);

      compileMessageOperation(writer, scope, node, loperand, byRefHandler,
         signRef, messageArguments, EAttr::AlreadyResolved, updatedOuterArgs);

      retVal = tempRetVal;
   }
   else retVal = compileMessageOperation(writer, scope, node, loperand, message,
      signRef, messageArguments, EAttr::NoExtension, updatedOuterArgs);

   return retVal;
}

inline bool isPrimitiveArray(ref_t typeRef)
{
   switch (typeRef) {
      case V_BINARYARRAY:
      case V_OBJARRAY:
      case V_INT32ARRAY:
         return true;
      default:
         return false;
   }
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ArgumentsInfo& messageArguments,
   int operatorId, ref_t expectedRef, ArgumentsInfo* updatedOuterArgs)
{
   ObjectInfo loperand = messageArguments[0];

   pos_t      argLen = 1;
   ref_t      arguments[3] = {};
   arguments[0] = loperand.typeInfo.typeRef;
   if (messageArguments.count() > 1) {
      arguments[argLen++] = retrieveType(scope, messageArguments[1]);
   }
   if (messageArguments.count() > 2) {
      arguments[argLen++] = retrieveType(scope, messageArguments[2]);
   }

   if (operatorId == SET_INDEXER_OPERATOR_ID && isPrimitiveArray(arguments[0])) {
      if (_logic->isCompatible(*scope.moduleScope, { arguments[1] }, { loperand.typeInfo.elementRef }, false))
         // HOTFIX : for the generic binary array, recognize the element type
         arguments[1] = V_ELEMENT;
   }

   ref_t outputRef = 0;
   bool  needToAlloc = false;
   BuildKey op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef);

   ObjectInfo retVal = {};
   if (op != BuildKey::None) {
      ObjectInfo roperand = {};
      ObjectInfo ioperand = {};

      if (messageArguments.count() > 1)
         roperand = messageArguments[1];
      if (messageArguments.count() > 2)
         ioperand = messageArguments[2];

      if (outputRef == V_ELEMENT) {
         outputRef = loperand.typeInfo.elementRef;
      }

      if (op == BuildKey::NilCondOp) {
         // NOTE : the nil operation need only one (not nil) operand
         if (loperand.typeInfo.typeRef == V_NIL) {
            loperand = roperand;
         }

         roperand = {};
      }
      else if (op != BuildKey::ObjArrayOp && outputRef && _logic->isEmbeddable(*scope.moduleScope, outputRef))
         needToAlloc = true;

      if (needToAlloc) {
         retVal = allocateResult(scope, outputRef);
      }
      else retVal = { ObjectKind::Object, { outputRef }, 0 };

      // box argument locally if required
      loperand = boxArgumentLocally(writer, scope, loperand, true, false);
      roperand = boxArgumentLocally(writer, scope, roperand, true, false);

      writeObjectInfo(writer, scope, loperand);
      writer.appendNode(BuildKey::SavingInStack, 0);

      if (roperand.kind != ObjectKind::Unknown) {
         writeObjectInfo(writer, scope, roperand);
         writer.appendNode(BuildKey::SavingInStack, 1);
      }

      if (ioperand.kind != ObjectKind::Unknown)
         writeObjectInfo(writer, scope, ioperand);

      writer.newNode(op, operatorId);

      // check if the operation requires an extra arguments
      if (needToAlloc) {
         writer.appendNode(BuildKey::Index, retVal.argument);
      }

      switch (op) {
         case BuildKey::BinaryArraySOp:
         case BuildKey::BinaryArrayOp:
            writer.appendNode(BuildKey::Size, _logic->defineStructSize(*scope.moduleScope, loperand.typeInfo.elementRef).size);
            break;
         case BuildKey::BoolSOp:
         case BuildKey::IntCondOp:
         case BuildKey::UIntCondOp:
         case BuildKey::ByteCondOp:
         case BuildKey::ShortCondOp:
         case BuildKey::LongCondOp:
         case BuildKey::LongIntCondOp:
         case BuildKey::RealCondOp:
         case BuildKey::NilCondOp:
            writer.appendNode(BuildKey::TrueConst, scope.moduleScope->branchingInfo.trueRef);
            writer.appendNode(BuildKey::FalseConst, scope.moduleScope->branchingInfo.falseRef);
            break;
         default:
            break;
      }

      writer.closeNode();

      retVal = unboxArguments(writer, scope, retVal, updatedOuterArgs);

      scope.reserveArgs(argLen);
   }
   else {
      mssg_t message = resolveOperatorMessage(scope.moduleScope, operatorId);
      if (messageArguments.count() > 2) {
         overwriteArgCount(message, 3);

         // HOTFIX : index argument should be the second one
         ObjectInfo tmp = messageArguments[1];
         messageArguments[1] = messageArguments[2];
         messageArguments[2] = tmp;

         ref_t tmpRef = arguments[1];
         if (tmpRef == V_ELEMENT)
            tmpRef = loperand.typeInfo.elementRef;

         arguments[1] = arguments[2];
         arguments[2] = tmpRef;
      }

      retVal = compileWeakOperation(writer, scope, node, arguments, argLen, loperand,
         messageArguments, message, expectedRef, updatedOuterArgs);
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, SyntaxNode rnode,
   int operatorId, ref_t expectedRef)
{
   ObjectInfo     retVal;
   ArgumentsInfo  updatedOuterArgs;

   SyntaxNode lnode = node;
   SyntaxNode inode;

   if (operatorId == SET_INDEXER_OPERATOR_ID) {
      lnode = node.firstChild();
      inode = lnode.nextNode();
   }

   BuildKey   op = BuildKey::None;
   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter | EAttr::RetValExpected, &updatedOuterArgs);
   ObjectInfo roperand = {};
   ObjectInfo ioperand = {};

   ArgumentsInfo arguments;
   arguments.add(loperand);

   // HOTFIX : typecast the right-hand expression if required
   if (rnode != SyntaxKey::None) {
      ref_t rTargetRef = 0;
      if (operatorId == SET_OPERATOR_ID)
         rTargetRef = retrieveType(scope, loperand);

      roperand = compileExpression(writer, scope, rnode, rTargetRef, EAttr::Parameter | EAttr::RetValExpected, &updatedOuterArgs);

      arguments.add(roperand);
   }

   if (inode != SyntaxKey::None) {
      arguments.add(compileExpression(writer, scope, inode, 0, EAttr::Parameter | EAttr::RetValExpected, &updatedOuterArgs));
   }

   return compileOperation(writer, scope, node, arguments, operatorId, expectedRef, &updatedOuterArgs);
}

mssg_t Compiler :: mapMessage(Scope& scope, SyntaxNode current, bool propertyMode,
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
      classScope.info.attributes, &targets, targetResolver, ClassAttribute::OverloadList);

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

   generateMethodDeclaration(classScope, classNode.findChild(SyntaxKey::Method), false, false);
   classScope.save();

   BuildNode node = writer.CurrentNode();
   while (node != BuildKey::Root)
      node = node.parentNode();

   BuildTreeWriter nestedWriter(node);
   nestedWriter.newNode(BuildKey::Class, extRef);
   compileVMT(nestedWriter, classScope, classNode);
   nestedWriter.closeNode();

   return extRef;
}

ref_t Compiler :: compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current,
   ArgumentsInfo& arguments, ref_t expectedSignRef, EAttr mode, ArgumentsInfo* updatedOuterArgs, bool& variadicArgList)
{
   EAttr paramMode = EAttr::Parameter | EAttr::RetValExpected;
   if (EAttrs::testAndExclude(mode, EAttr::NoPrimitives))
      paramMode = paramMode | EAttr::NoPrimitives;

   // compile the message argument list
   ref_t signatures[ARG_COUNT] = { 0 };
   ref_t signatureLen = 0;
   ref_t superReference = scope.moduleScope->buildins.superReference;

   if (expectedSignRef)
      scope.module->resolveSignature(expectedSignRef, signatures);

   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         // try to recognize the message signature
         // NOTE : signatures[signatureLen] contains expected parameter type if expectedSignRef is provided
         auto argInfo = compileExpression(writer, scope, current, signatures[signatureLen],
            paramMode, updatedOuterArgs);

         if (argInfo.mode == TargetMode::UnboxingVarArgument) {
            if (argInfo.typeInfo.elementRef) {
               signatures[signatureLen++] = argInfo.typeInfo.elementRef;
            }
            else signatures[signatureLen++] = scope.moduleScope->buildins.superReference;

            if (!variadicArgList) {
               variadicArgList = true;
            }
            else scope.raiseError(errInvalidOperation, current);
         }
         else {
            ref_t argRef = retrieveStrongType(scope, argInfo);
            if (signatureLen >= ARG_COUNT) {
               signatureLen++;
            }
            else if (argRef) {
               signatures[signatureLen++] = argRef;
            }
            else signatures[signatureLen++] = superReference;
         }
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

ref_t Compiler :: mapExtension(BuildTreeWriter& writer, Scope& scope, mssg_t& message, ref_t& implicitSignatureRef, 
   ObjectInfo object)
{
   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   ref_t objectRef = retrieveStrongType(scope, object);
   if (objectRef == 0) {
      objectRef = scope.moduleScope->buildins.superReference;
   }

   if (implicitSignatureRef) {
      // auto generate extension template for strong-typed signature
      for (auto it = nsScope->extensionTemplates.getIt(message); !it.eof(); it = nsScope->extensionTemplates.nextIt(message, it)) {
         _logic->resolveExtensionTemplate(*scope.moduleScope, this, *it,
            implicitSignatureRef, *nsScope->nsName, nsScope->outerExtensionList ? nsScope->outerExtensionList : &nsScope->extensions);
      }
   }

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
            else if (_logic->isSignatureCompatible(*scope.moduleScope, resolvedMessage, extInfo.value2)) {
               //NOTE : if the extension is more precise than the previous resolved one - use the new one  
               resolvedMessage = extInfo.value2;
               resolvedExtRef = extInfo.value1;
            }
            else if (!_logic->isSignatureCompatible(*scope.moduleScope, extInfo.value2, resolvedMessage)) {
               // if they are incompatible - use dynamic resolving
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
      implicitSignatureRef = 0;
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

mssg_t Compiler :: resolveVariadicMessage(Scope& scope, mssg_t message)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0, dummy = 0;
   decodeMessage(message, actionRef, argCount, flags);

   ustr_t actionName = scope.module->resolveAction(actionRef, dummy);

   int argMultuCount = test(message, FUNCTION_MESSAGE) ? 1 : 2;

   return encodeMessage(scope.module->mapAction(actionName, 0, false), argMultuCount, flags | VARIADIC_MESSAGE);
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

   // check if the object handles the variadic message
   if (targetRef) {
      resolvedStackSafeAttr = 0;
      resolvedMessage = _logic->resolveMultimethod(*scope.moduleScope,
         resolveVariadicMessage(scope, weakMessage),
         targetRef, implicitSignatureRef, resolvedStackSafeAttr, isSelfCall(target));

      if (resolvedMessage != 0) {
         stackSafeAttr = resolvedStackSafeAttr;

         // if the object handles the compile-time resolved variadic message - use it
         return resolvedMessage;
      }
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

         _logic->setSignatureStacksafe(*scope.moduleScope, implicitSignatureRef, stackSafeAttr);

         return resolvedMessage;
      }

      // HOTFIX : do not check variadic message for the properties
      if ((weakMessage & PREFIX_MESSAGE_MASK) == PROPERTY_MESSAGE)
         return weakMessage;

      // check if the extension handles the variadic message
      mssg_t variadicMessage = resolveVariadicMessage(scope, weakMessage);

      extensionRef = mapExtension(writer, scope, variadicMessage, implicitSignatureRef, target);
      if (extensionRef != 0) {
         // if there is an extension to handle the compile-time resolved message - use it
         resolvedExtensionRef = extensionRef;

         return variadicMessage;
      }
   }

   // otherwise - use the weak message
   return weakMessage;
}

void Compiler :: unboxArgumentLocaly(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo temp, ObjectKey key)
{
   if ((temp.typeInfo.isPrimitive() && _logic->isPrimitiveArrRef(temp.typeInfo.typeRef))
      || _logic->isEmbeddableArray(*scope.moduleScope, temp.typeInfo.typeRef))
   {
      int size = defineFieldSize(scope, { key.value1, temp.typeInfo, key.value2 });

      compileAssigningOp(writer, scope, { key.value1, temp.typeInfo, key.value2, size }, temp);
   }
   else compileAssigningOp(writer, scope, { key.value1, temp.typeInfo, key.value2 }, temp);
}

void Compiler :: unboxOuterArgs(BuildTreeWriter& writer, ExprScope& scope, ArgumentsInfo* updatedOuterArgs)
{
   // first argument is a closure
   ObjectInfo closure;

   for (pos_t i = 0; i != updatedOuterArgs->count_pos(); i++) {
      ObjectInfo info = (*updatedOuterArgs)[i];
      if (info.kind == ObjectKind::ClosureInfo) {
         closure = (*updatedOuterArgs)[++i];
         closure.kind = ObjectKind::LocalField;
      }
      else if (info.kind == ObjectKind::MemberInfo) {
         ObjectInfo source = (*updatedOuterArgs)[++i];

         if (source.kind == ObjectKind::Local) {
            closure.extra = info.reference;
            compileAssigningOp(writer, scope, source, closure);
         }
         else if (source.kind == ObjectKind::LocalAddress) {
            closure.extra = info.reference;
            compileAssigningOp(writer, scope, source, closure);
         }
         else assert(false);

      }
      else assert(false);
   }
}

ObjectInfo Compiler :: unboxArguments(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo retVal, 
   ArgumentsInfo* updatedOuterArgs)
{
   // unbox the arguments if required
   bool resultSaved = false;
   for (auto it = scope.tempLocals.start(); !it.eof(); ++it) {
      ObjectInfo temp = *it;

      if (temp.mode == TargetMode::UnboxingRequired || temp.mode == TargetMode::RefUnboxingRequired
         || temp.mode == TargetMode::LocalUnboxingRequired || temp.mode == TargetMode::ConditionalUnboxingRequired)
      {
         if (!resultSaved && retVal.kind != ObjectKind::Unknown) {
            // presave the result
            ObjectInfo tempResult = declareTempLocal(scope, retVal.typeInfo.typeRef, false);
            compileAssigningOp(writer, scope, tempResult, retVal);
            retVal = tempResult;

            resultSaved = true;
         }

         // unbox the temporal variable
         auto key = it.key();
         if (temp.mode == TargetMode::LocalUnboxingRequired) {
            unboxArgumentLocaly(writer, scope, temp, key);
         }
         else if (temp.mode == TargetMode::RefUnboxingRequired) {
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
         else if (key.value1 == ObjectKind::FieldAddress) {
            unboxArgumentLocaly(writer, scope, temp, key);
         }
         else if (temp.mode == TargetMode::ConditionalUnboxingRequired) {
            writeObjectInfo(writer, scope, { key.value1, temp.typeInfo, key.value2 });
            writer.newNode(BuildKey::StackCondOp);
            compileAssigningOp(writer, scope, { key.value1, temp.typeInfo, key.value2 }, temp);
            writer.closeNode();
         }
         else compileAssigningOp(writer, scope, { key.value1, temp.typeInfo, key.value2 }, temp);
      }
   }

   if (updatedOuterArgs)
      unboxOuterArgs(writer, scope, updatedOuterArgs);

   scope.tempLocals.clear();

   return retVal;
}

void Compiler :: writeMessageArguments(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo& target, 
   mssg_t message, ArgumentsInfo& arguments, ObjectInfo& lenLocal, int& stackSafeAttr,
   bool targetOverridden, bool found, bool argUnboxingRequired, bool stackSafe)
{
   if (targetOverridden) {
      target = boxArgument(writer, scope, target, stackSafe, false, false);
   }

   if (!found)
      stackSafeAttr = 0;

   pos_t counter = arguments.count_pos();
   if (argUnboxingRequired) {
      counter--;

      ObjectInfo lenLocal = declareTempLocal(scope, scope.moduleScope->buildins.intReference, false);

      // get length
      writeObjectInfo(writer, scope, arguments[counter]);
      writer.appendNode(BuildKey::SavingInStack);
      writer.newNode(BuildKey::VArgSOp, LEN_OPERATOR_ID);
      writer.appendNode(BuildKey::Index, lenLocal.argument);
      writer.closeNode();

      writer.appendNode(BuildKey::LoadingIndex, lenLocal.argument);
      writer.newNode(BuildKey::UnboxMessage, arguments[counter].argument);
      writer.appendNode(BuildKey::Index, counter);
      writer.closeNode();
   }

   // box the arguments if required
   int argMask = 1;
   for (unsigned int i = 0; i < counter; i++) {
      // NOTE : byref dynamic arg can be passed semi-directly (via temporal variable) if the method resolved directly
      ObjectInfo arg = boxArgument(writer, scope, arguments[i],
         test(stackSafeAttr, argMask), false, found);

      arguments[i] = arg;
      argMask <<= 1;
   }

   if (isOpenArg(message) && !argUnboxingRequired) {
      // NOTE : in case of unboxing variadic argument, the terminator is already copied
      arguments.add({ ObjectKind::Terminator });
      counter++;
   }

   for (unsigned int i = counter; i > 0; i--) {
      ObjectInfo arg = arguments[i - 1];

      writeObjectInfo(writer, scope, arg);
      writer.appendNode(BuildKey::SavingInStack, i - 1);
   }
}

bool Compiler :: validateShortCircle(ExprScope& scope, mssg_t message, ObjectInfo target)
{
   ref_t targetRef = 0;
   switch (target.kind) {
      case ObjectKind::Class:
         targetRef = target.reference;
         break;
      case ObjectKind::ConstructorSelf:
         targetRef = target.extra;
         break;
      default:
         targetRef = target.typeInfo.typeRef;
         break;
   }

   MethodScope* methodScope = Scope::getScope<MethodScope>(scope, Scope::ScopeLevel::Method);
   if (methodScope != nullptr) {
      return (methodScope->message == message && methodScope->getClassRef() == targetRef);
   }

   return false;
}

inline SyntaxNode findMessageNode(SyntaxNode node)
{
   if (node == SyntaxKey::ResendDispatch) {
      node = node.findChild(SyntaxKey::MessageOperation);
   }
   return node.findChild(SyntaxKey::Message);
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo target,
   mssg_t weakMessage, ref_t implicitSignatureRef, ArgumentsInfo& arguments, ExpressionAttributes mode, ArgumentsInfo* updatedOuterArgs)
{
   bool argUnboxingRequired = EAttrs::testAndExclude(mode.attrs, EAttr::WithVariadicArg);
   bool checkShortCircle = EAttrs::testAndExclude(mode.attrs, EAttr::CheckShortCircle);

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

   bool functionMode = test(message, FUNCTION_MESSAGE);
   if (functionMode && target.kind != ObjectKind::ConstantRole) {
      stackSafeAttr >>= 1;
   }
   else stackSafeAttr &= 0xFFFFFFFE; // exclude the stack safe target attribute, it should be set by compileMessage

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
            // HOTFIX : do not box the variadic argument target for the direct operation
            if (arguments[0].kind == ObjectKind::VArgParam)
               result.stackSafe = true;

            if (checkShortCircle && validateShortCircle(scope, message, target)) {
               if (target.kind == ObjectKind::ConstructorSelf) {
                  scope.raiseError(errRedirectToItself, node);
               }
               else scope.raiseWarning(WARNING_LEVEL_1, wrnCallingItself, findMessageNode(node));
            }

            break;
         case MethodHint::Virtual:
            operation = BuildKey::SemiDirectCallOp;
            break;
         default:
            break;
      }
      if (operation != BuildKey::CallOp) {
         // if the method directly resolved and the target is not required to be dynamic, mark it as stacksafe
         if (result.stackSafe && !functionMode)
            stackSafeAttr |= 1;
      }
   }
   else if (targetRef) {
      if (EAttrs::test(mode.attrs, EAttr::StrongResolved)) {
         if (getAction(message) == getAction(scope.moduleScope->buildins.constructor_message)) {
            scope.raiseError(errUnknownDefConstructor, node);
         }
         else scope.raiseError(errUnknownMessage, findMessageNode(node));
      }
      else {
         bool weakTarget = targetRef == scope.moduleScope->buildins.superReference || result.withCustomDispatcher;

         // treat it as a weak reference
         targetRef = 0;

         SyntaxNode messageNode = findMessageNode(node);
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
      ObjectInfo lenLocal = {};
      writeMessageArguments(writer, scope, target, message, arguments, lenLocal,
         stackSafeAttr, targetOverridden, found, argUnboxingRequired, result.stackSafe);

      if (!targetOverridden) {
         writer.appendNode(BuildKey::Argument, 0);
      }
      else writeObjectInfo(writer, scope, target);

      writer.newNode(operation, message);

      if (targetRef)
         writer.appendNode(BuildKey::Type, targetRef);

      writer.closeNode();

      retVal = unboxArguments(writer, scope, retVal, updatedOuterArgs);

      if (argUnboxingRequired) {
         writer.appendNode(BuildKey::LoadingIndex, lenLocal.argument);
         writer.appendNode(BuildKey::FreeVarStack);
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

ObjectInfo Compiler :: compileNewArrayOp(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ObjectInfo source, ref_t targetRef, ArgumentsInfo& arguments)
{
   ref_t sourceRef = retrieveStrongType(scope, source);

   //if (!targetRef)
   //   targetRef = resolvePrimitiveReference(scope, source.type, source.element, false);

   ref_t argumentRefs[ARG_COUNT] = {};
   pos_t argLen = 0;
   for (pos_t i = 0; i < arguments.count(); i++) {
      argumentRefs[argLen++] = retrieveStrongType(scope, arguments[i]);
   }

   BuildKey operationKey = _logic->resolveNewOp(*scope.moduleScope, sourceRef, argumentRefs, argLen);
   if (operationKey == BuildKey::NewArrayOp) {
      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, sourceRef);

      if (targetRef) {
         NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

         auto conversionRoutine = _logic->retrieveConversionRoutine(this, *scope.moduleScope, *nsScope->nsName, 
            targetRef, source.typeInfo);
         if (conversionRoutine.result == ConversionResult::BoxingRequired) {
            source.typeInfo = { targetRef };
         }
         else source.typeInfo = { sourceRef };
      }
      else source.typeInfo = { sourceRef };

      assert(arguments.count() == 1); // !! temporally - only one argument is supported

      writeObjectInfo(writer, scope, arguments[0]);
      writer.appendNode(BuildKey::SavingInStack, 0);

      assert(!source.typeInfo.isPrimitive());

      writer.newNode(operationKey, source.typeInfo.typeRef);

      if (sizeInfo.size < 0)
         writer.appendNode(BuildKey::Size, sizeInfo.size);

      writer.closeNode();

      // fill the array
      if (!sizeInfo.size) {
         writer.appendNode(BuildKey::FillOp);
      }

      return { ObjectKind::Object, source.typeInfo, 0 };
   }

   scope.raiseError(errInvalidOperation, node);

   return {}; // !! temporal
}

ObjectInfo Compiler :: compileNativeConversion(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t operationKey)
{
   ObjectInfo retVal = {};

   source = boxArgumentLocally(writer, scope, source, false, false);

   switch (operationKey) {
      case INT16_32_CONVERSION:
         retVal = allocateResult(scope, resolvePrimitiveType(scope, { V_INT32 }, false));

         writeObjectInfo(writer, scope, retVal);
         writer.appendNode(BuildKey::SavingInStack, 0);

         writeObjectInfo(writer, scope, source);

         writer.appendNode(BuildKey::ConversionOp, operationKey);
         break;
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
      writer, scope, node, source, messageRef, signRef, arguments, EAttr::StrongResolved | EAttr::NoExtension, nullptr);

   // HOTFIX : to use weak reference for the created class
   retVal.typeInfo = { source.reference };

   return retVal;
}

ObjectInfo Compiler :: compilePropertyOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ref_t expectedRef, ExpressionAttribute attrs)
{
   ObjectInfo retVal = { };
   ArgumentsInfo arguments;
   ArgumentsInfo outerArgsToUpdate;

   SyntaxNode current = node.firstChild();
   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter, &outerArgsToUpdate);
   if (source.mode != TargetMode::None && source.mode != TargetMode::Conditional)
      scope.raiseError(errInvalidOperation, node);

   arguments.add(source);

   // NOTE : the operation target shouldn't be a primtive type
   source = validateObject(writer, scope, node, source, 0, true, true, false);

   current = current.nextNode();
   mssg_t messageRef = mapMessage(scope, current, true,
      source.kind == ObjectKind::Extension, false);

   mssg_t resolvedMessage = _logic->resolveSingleDispatch(*scope.moduleScope,
      retrieveType(scope, source), messageRef);

   bool variadicArgList = false;
   ref_t expectedSignRef = 0;
   if (resolvedMessage)
      scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

   ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments, expectedSignRef, EAttr::NoPrimitives, 
      &outerArgsToUpdate, variadicArgList);
   EAttr opMode = EAttr::None;
   if (variadicArgList) {
      // HOTFIX : set variadic flag if required
      messageRef |= VARIADIC_MESSAGE;

      opMode = EAttr::WithVariadicArg;
   }

   mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, source), expectedRef, messageRef, implicitSignatureRef);
   if (byRefHandler) {
      ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

      addByRefRetVal(arguments, tempRetVal);
      if (tempRetVal.kind == ObjectKind::TempLocalAddress)
         writer.appendNode(BuildKey::ByRefOpMark, tempRetVal.argument);

      compileMessageOperation(writer, scope, node, source, byRefHandler,
         implicitSignatureRef, arguments, opMode | EAttr::AlreadyResolved, &outerArgsToUpdate);

      retVal = tempRetVal;
   }
   else retVal = compileMessageOperation(writer, scope, node, source, messageRef,
      implicitSignatureRef, arguments, opMode, &outerArgsToUpdate);

   return retVal;
}

mssg_t Compiler :: resolveByRefHandler(Scope& scope, ref_t targetRef, ref_t expectedRef, mssg_t weakMessage, ref_t& signatureRef)
{
   pos_t argCount = 0;
   ref_t actionRef = 0, flags = 0;
   decodeMessage(weakMessage, actionRef, argCount, flags);

   if (expectedRef != 0 && targetRef != 0) {
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
   ArgumentsInfo updatedOuterArgs;

   SyntaxNode current = node.firstChild();
   ObjectInfo source = compileObject(writer, scope, current, EAttr::Parameter, &updatedOuterArgs);
   bool probeMode = source.mode == TargetMode::Probe;
   switch (source.mode) {
      case TargetMode::External:
      case TargetMode::WinApi:
      {
         bool dummy = false;
         compileMessageArguments(writer, scope, current, arguments, 0, EAttr::None, nullptr, dummy);
         if (dummy)
            scope.raiseError(errInvalidOperation, current);

         retVal = compileExternalOp(writer, scope, source.reference, source.mode == TargetMode::WinApi, arguments, expectedRef);
         break;
      }
      case TargetMode::CreatingArray:
      {
         bool dummy = false;
         compileMessageArguments(writer, scope, current, arguments, 0, EAttr::NoPrimitives, nullptr, dummy);
         if (dummy)
            scope.raiseError(errInvalidOperation, current);

         retVal = compileNewArrayOp(writer, scope, node, source, expectedRef, arguments);
         break;
      }
      case TargetMode::Creating:
      {
         bool dummy = false;
         ref_t signRef = compileMessageArguments(writer, scope, current, arguments, 0, EAttr::NoPrimitives, nullptr, dummy);
         if (dummy)
            scope.raiseError(errInvalidOperation, current);

         retVal = compileNewOp(writer, scope, node, mapClassSymbol(scope,
            retrieveStrongType(scope, source)), signRef, arguments);
         break;
      }
      case TargetMode::Casting:
      {
         bool dummy = false;
         compileMessageArguments(writer, scope, current, arguments, 0, EAttr::NoPrimitives, nullptr, dummy);
         if (arguments.count() == 1 && !dummy) {
            retVal = convertObject(writer, scope, current, arguments[0], retrieveStrongType(scope, source), false);
         }
         else scope.raiseError(errInvalidOperation, node);
         break;
      }
      default:
      {
         // NOTE : the operation target shouldn't be a primtive type
         source = validateObject(writer, scope, node, source, 0, true, true, false);

         current = current.nextNode();
         mssg_t messageRef = mapMessage(scope, current, false,
            source.kind == ObjectKind::Extension, probeMode);

         if (!test(messageRef, FUNCTION_MESSAGE))
            arguments.add(source);

         mssg_t resolvedMessage = _logic->resolveSingleDispatch(*scope.moduleScope,
            retrieveType(scope, source), messageRef);

         ref_t expectedSignRef = 0;
         if (resolvedMessage)
            scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

         bool withVariadicArg = false;
         ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments, expectedSignRef, EAttr::NoPrimitives,
            &updatedOuterArgs, withVariadicArg);

         EAttr opMode = EAttr::None;
         if (withVariadicArg) {
            messageRef |= VARIADIC_MESSAGE;

            opMode = EAttr::WithVariadicArg;
         }

         mssg_t byRefHandler = resolveByRefHandler(scope, retrieveStrongType(scope, source), expectedRef, messageRef, implicitSignatureRef);
         if (byRefHandler) {
            ObjectInfo tempRetVal = declareTempLocal(scope, expectedRef, false);

            addByRefRetVal(arguments, tempRetVal);
            // adding mark for optimization routine
            if (tempRetVal.kind == ObjectKind::TempLocalAddress)
               writer.appendNode(BuildKey::ByRefOpMark, tempRetVal.argument);

            compileMessageOperation(writer, scope, node, source, byRefHandler,
               implicitSignatureRef, arguments, opMode | EAttr::AlreadyResolved, &updatedOuterArgs);

            retVal = tempRetVal;
         }
         else retVal = compileMessageOperation(writer, scope, node, source, messageRef,
            implicitSignatureRef, arguments, opMode, &updatedOuterArgs);

         break;
      }
   }

   return retVal;
}

bool Compiler :: resolveAutoType(ExprScope& scope, ObjectInfo source, ObjectInfo& target)
{
   ref_t sourceRef = retrieveStrongType(scope, source);

   if (!_logic->validateAutoType(*scope.moduleScope, sourceRef))
      return false;

   return scope.resolveAutoType(target, source.typeInfo);
}

bool Compiler :: compileAssigningOp(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo target, ObjectInfo exprVal)
{
   BuildKey operationType = BuildKey::None;
   int operand = 0;

   int  size = 0;
   bool stackSafe = false;
   bool fieldMode = false;
   bool fieldFieldMode = false;
   bool accMode = false;
   bool lenRequired = false;

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
      case ObjectKind::ParamAddress:
         accMode = true;
         operationType = BuildKey::CopyingToAcc;
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         stackSafe = true;
         break;
      case ObjectKind::TempLocalAddress:
      case ObjectKind::LocalAddress:
         scope.markAsAssigned(target);
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         if (size > 0) {
            operationType = BuildKey::Copying;
            operand = target.reference;
         }
         else {
            lenRequired = true;
            accMode = true;
            operationType = BuildKey::CopyingArr;
            size = -size;
         }
         stackSafe = true;
         break;
      case ObjectKind::Field:
         scope.markAsAssigned(target);
         operationType = BuildKey::FieldAssigning;
         operand = target.reference;
         fieldMode = true;
         break;
      case ObjectKind::OuterField:
         scope.markAsAssigned(target);
         operationType = BuildKey::FieldAssigning;
         operand = target.extra;
         fieldFieldMode = fieldMode = true;
         break;
      case ObjectKind::StaticField:
         scope.markAsAssigned(target);
         operationType = BuildKey::StaticAssigning;
         operand = target.reference;
         break;
      case ObjectKind::FieldAddress:
         scope.markAsAssigned(target);
         fieldMode = true;
         if (target.reference) {
            operationType = BuildKey::CopyingToAccField;
            operand = target.reference;
         }
         else operationType = BuildKey::CopyingToAccExact;
         operand = target.reference;
         size = _logic->defineStructSize(*scope.moduleScope, target.typeInfo.typeRef).size;
         if (size < 0) {
            size = target.extra;
         }
         stackSafe = true;

         assert(size > 0);

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
      case ObjectKind::Outer:
      {
         InlineClassScope* closure = Scope::getScope<InlineClassScope>(scope, Scope::ScopeLevel::Class);
         if (/*!method->subCodeMode || */!closure->markAsPresaved(target))
            return false;

         operationType = BuildKey::FieldAssigning;
         operand = target.reference;
         fieldMode = true;

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
      if (fieldFieldMode)
         writer.appendNode(BuildKey::Field, target.reference);
   }
   else if (accMode) {
      if(lenRequired)
         writer.appendNode(BuildKey::LoadingBinaryLen, size);

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

ObjectInfo Compiler :: compileTupleAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ArgumentsInfo targets;
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   targets.add(mapObject(scope, current, EAttr::None));
   current = current.nextNode();
   while (current == SyntaxKey::SubVariable) {
      ObjectInfo subVar = mapObject(scope, current, EAttr::NewVariable | EAttr::IgnoreDuplicate);
      if (subVar.kind == ObjectKind::Unknown)
         scope.raiseError(errUnknownObject, current);

      targets.add(subVar);

      current = current.nextNode();
   }

   ObjectInfo exprVal = compileExpression(writer, scope, current, 0, EAttr::Parameter, nullptr);
   for (pos_t i = 0; i < targets.count_pos(); i++) {
      arguments.clear();
      arguments.add(exprVal);
      arguments.add({ ObjectKind::IntLiteral, { V_INT32 }, ::mapIntConstant(scope, i), i});

      ObjectInfo targetVar = targets[i];

      ref_t actionRef = scope.module->mapAction(REFER_MESSAGE, 0, false);
      mssg_t getter = encodeMessage(actionRef, 2, 0);

      ObjectInfo sourceVar = compileMessageOperation(writer, scope, node, exprVal, getter,
         0, arguments, EAttr::None, nullptr);

      compileAssigningOp(writer, scope, targetVar, sourceVar);
   }

   return exprVal;
}

ObjectInfo Compiler :: compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand,
   SyntaxNode roperand, ExpressionAttribute mode)
{
   ObjectInfo target = mapObject(scope, loperand, mode);
   if (target.kind == ObjectKind::Unknown)
      scope.raiseError(errUnknownObject, loperand.lastChild(SyntaxKey::TerminalMask));

   ObjectInfo exprVal = {};

   ref_t targetRef = retrieveStrongType(scope, target);
   if (targetRef == V_AUTO) {
      // support auto attribute
      exprVal = compileExpression(writer, scope, roperand,
         0, EAttr::RetValExpected, nullptr);

      if (resolveAutoType(scope, exprVal, target)) {
         targetRef = retrieveStrongType(scope, exprVal);
         target.typeInfo.typeRef = targetRef;
      }
      else scope.raiseError(errInvalidOperation, roperand.parentNode());
   }
   else exprVal = compileExpression(writer, scope, roperand,
      targetRef, EAttr::RetValExpected, nullptr);

   if (!compileAssigningOp(writer, scope, target, exprVal))
      scope.raiseError(errInvalidOperation, loperand.parentNode());;

   if (target == exprVal)
      scope.raiseError(errAssigningToSelf, loperand.lastChild(SyntaxKey::TerminalMask));

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
         declareVariable(scope, node, info.typeInfo, false); // !! temporal - typeref should be provided or super class

         if (_trackingUnassigned) {
            CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);

            scope.markAsAssigned(codeScope->mapLocal(loperand.firstChild(SyntaxKey::TerminalMask).identifier()));
         }

         return {}; // !! temporally
      }
      else if (info.kind == ObjectKind::MssgLiteral) {
         return mapMessageConstant(scope, node, info.reference);
      }
      else if (info.kind == ObjectKind::ExtMssgLiteral) {
         return mapExtMessageConstant(scope, node, info.reference, info.typeInfo.elementRef);
      }
   }

   return compileOperation(writer, scope, node, operatorId, expectedRef, EAttr::None);
}

inline bool isConditionalOp(SyntaxKey key)
{
   switch (key) {
      case SyntaxKey::EqualOperation:
      case SyntaxKey::NotEqualOperation:
      case SyntaxKey::LessOperation:
      case SyntaxKey::NotLessOperation:
      case SyntaxKey::GreaterOperation:
      case SyntaxKey::NotGreaterOperation:
         return true;
      default:
         return false;
   }
}

ObjectInfo Compiler :: compileBoolOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId)
{
   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = lnode.nextNode();

   ObjectInfo loperand = {};

   bool condOp = isConditionalOp(lnode.key);
   bool nativeOp = false;
   if (!condOp) {
      // If it is not a comparison operation
      // we have to define if native short-circuit evaluation can be used
      loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter, nullptr);

      nativeOp = _logic->isCompatible(*scope.moduleScope, 
         { scope.moduleScope->branchingInfo.typeRef }, loperand.typeInfo, true);
   }
   else nativeOp = true;

   if (nativeOp) {
      writer.newNode(BuildKey::ShortCircuitOp, operatorId);

      writer.appendNode(BuildKey::TrueConst, scope.moduleScope->branchingInfo.trueRef);
      writer.appendNode(BuildKey::FalseConst, scope.moduleScope->branchingInfo.falseRef);

      writer.newNode(BuildKey::Tape);
      if (loperand.kind == ObjectKind::Unknown)
         loperand = compileExpression(writer, scope, lnode, scope.moduleScope->branchingInfo.typeRef, EAttr::None, nullptr);

      writeObjectInfo(writer, scope, loperand);
      writer.closeNode();

      writer.newNode(BuildKey::Tape);
      writeObjectInfo(writer, scope, compileExpression(writer, scope, rnode, scope.moduleScope->branchingInfo.typeRef, EAttr::None, nullptr));
      writer.closeNode();

      writer.closeNode();

      return { ObjectKind::Object, { scope.moduleScope->branchingInfo.typeRef }, 0 };
   }
   else {
      // bad luck - we have to implement weak short-circuit evaluation
      // using lazy expression
      SyntaxTree tempTree;
      SyntaxTreeWriter treeWriter(tempTree);
      treeWriter.newNode(SyntaxKey::LazyOperation);
      SyntaxTree::copyNode(treeWriter, rnode, true);
      treeWriter.closeNode();

      ObjectInfo roperand = compileClosure(writer, scope, tempTree.readRoot(), EAttr::Parameter, nullptr);
      ref_t      arguments[2] = 
      {
         retrieveType(scope, loperand),
         retrieveType(scope, roperand)
      };

      ArgumentsInfo messageArguments;
      messageArguments.add(loperand);
      messageArguments.add(roperand);

      mssg_t message = resolveOperatorMessage(scope.moduleScope, operatorId);

      return compileWeakOperation(writer, scope, node, arguments, 2, loperand,
         messageArguments, message, 0, nullptr);
   }
}

ObjectInfo Compiler :: compileAssignOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   int operatorId, ref_t expectedRef)
{
   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = lnode.nextNode();

   ArgumentsInfo updatedOuterArgs;
   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter, &updatedOuterArgs);
   ObjectInfo roperand = compileExpression(writer, scope, rnode, 0, EAttr::Parameter, &updatedOuterArgs);

   ref_t      arguments[2] = {};
   arguments[0] = loperand.typeInfo.typeRef;
   arguments[1] = roperand.typeInfo.typeRef;

   ref_t dummy = 0;
   BuildKey op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, 2, dummy);
   if (op != BuildKey::None) {
      // box argument locally if required
      loperand = boxArgumentLocally(writer, scope, loperand, true, true);
      roperand = boxArgumentLocally(writer, scope, roperand, true, false);

      writeObjectInfo(writer, scope, roperand);
      writer.appendNode(BuildKey::SavingInStack, 0);

      writer.newNode(op, operatorId);
      writer.appendNode(BuildKey::Index, loperand.argument);
      writer.closeNode();

      unboxArguments(writer, scope, {}, &updatedOuterArgs);

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
         messageArguments, message, expectedRef, &updatedOuterArgs);

      if(!compileAssigningOp(writer, scope, loperand, opVal))
         scope.raiseError(errInvalidOperation, node);
   }

   return loperand;
}

ObjectInfo Compiler :: compileSpecialOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef)
{
   ObjectInfo retVal = {};
   switch (operatorId) {
      case BREAK_OPERATOR_ID:
         writer.appendNode(BuildKey::BreakOp);
         break;
      case CONTINUE_OPERATOR_ID:
         writer.appendNode(BuildKey::ContinueOp);
         break;
      default:
         assert(false);
         break;
   }

   return retVal;
}

ObjectInfo Compiler :: compileOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, int operatorId, ref_t expectedRef,
   ExpressionAttribute mode)
{
   SyntaxNode loperand = node.firstChild();
   SyntaxNode roperand = loperand.nextNode();

   if (operatorId == SET_OPERATOR_ID){
      // assign operation is a special case
      if (loperand == SyntaxKey::IndexerOperation) {
         return compileOperation(writer, scope, loperand, roperand, SET_INDEXER_OPERATOR_ID, expectedRef);
      }
      else return compileAssigning(writer, scope, loperand, roperand, mode);

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
   ExpressionAttribute mode, bool withoutNewScope)
{
   bool retValExpected = EAttrs::testAndExclude(mode, EAttr::RetValExpected);

   scope.syncStack();

   CodeScope* parentCodeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);
   ObjectInfo retVal = {};
   if (!withoutNewScope) {
      CodeScope codeScope(parentCodeScope);
      retVal = compileCode(writer, codeScope, node, retValExpected);

      codeScope.syncStack(parentCodeScope);
   }
   else retVal = compileCode(writer, *parentCodeScope, node, retValExpected);

   if (!retValExpected) {
      retVal = { ObjectKind::Object };
   }

   return retVal;
}

ObjectInfo Compiler :: compileBranchingOperands(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode rnode,
   SyntaxNode r2node, bool retValExpected)
{
   writer.newNode(BuildKey::Tape);

   ObjectInfo subRetCode = {};
   if (rnode == SyntaxKey::ClosureBlock || rnode == SyntaxKey::SwitchCode) {
      subRetCode = compileSubCode(writer, scope, rnode.firstChild(), retValExpected ? EAttr::RetValExpected : EAttr::None);
   }
   else subRetCode = compileExpression(writer, scope, rnode, 0, EAttr::None, nullptr);

   if (retValExpected) {
      writeObjectInfo(writer, scope, subRetCode);
   }
   writer.closeNode();

   TypeInfo retType = {};
   if (r2node != SyntaxKey::None) {
      // NOTE : it should immediately follow if-block
      writer.newNode(BuildKey::Tape);
      ObjectInfo elseSubRetCode = {};
      if (r2node == SyntaxKey::ClosureBlock) {
         elseSubRetCode = compileSubCode(writer, scope, r2node.firstChild(), retValExpected ? EAttr::RetValExpected : EAttr::None);
      }
      else elseSubRetCode = compileExpression(writer, scope, r2node, 0, EAttr::None, nullptr);

      if (retValExpected) {
         writeObjectInfo(writer, scope, elseSubRetCode);

         if (subRetCode.typeInfo == elseSubRetCode.typeInfo)
            retType = subRetCode.typeInfo;
      }
      writer.closeNode();
   }

   writer.closeNode();

   ObjectInfo retVal = {};
   if (retValExpected)
      retVal = { ObjectKind::Object, retType, 0 };

   return retVal;
}

ObjectInfo Compiler :: compileBranchingOperation(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo loperand, SyntaxNode node, SyntaxNode rnode, 
   SyntaxNode r2node, int operatorId, ArgumentsInfo* updatedOuterArgs, bool retValExpected)
{
   ObjectInfo retVal = {};
   BuildKey   op = BuildKey::None;

   ObjectInfo roperand = {};
   ObjectInfo roperand2 = {};
   if (rnode.existChild(SyntaxKey::ClosureBlock)) {
      rnode = rnode.findChild(SyntaxKey::ClosureBlock);

      roperand = { ObjectKind::Closure, { V_CLOSURE }, 0 };
   }
   else if (rnode == SyntaxKey::SwitchCode) {
      roperand = { ObjectKind::Closure, { V_CLOSURE }, 0 };
   }
   else roperand = { ObjectKind::Object, { V_OBJECT }, 0 };

   if (r2node.existChild(SyntaxKey::ClosureBlock)) {
      r2node = r2node.findChild(SyntaxKey::ClosureBlock);

      roperand2 = { ObjectKind::Closure, { V_CLOSURE }, 0 };
   }
   else if (r2node != SyntaxKey::None) 
      roperand2 = { ObjectKind::Object, { V_OBJECT }, 0 };

   size_t     argLen = 2;
   ref_t      arguments[3] = {};
   arguments[0] = retrieveType(scope, loperand);
   arguments[1] = retrieveType(scope, roperand);

   if (r2node != SyntaxKey::None) {
      argLen++;
      arguments[2] = retrieveType(scope, roperand2);
   }

   ref_t outputRef = 0;
   op = _logic->resolveOp(*scope.moduleScope, operatorId, arguments, argLen, outputRef);

   if (op != BuildKey::None) {
      writeObjectInfo(writer, scope, loperand);

      writer.newNode(op, operatorId);
      writer.appendNode(BuildKey::Const, scope.moduleScope->branchingInfo.trueRef);

      retVal = compileBranchingOperands(writer, scope, rnode, r2node, retValExpected);
   }
   else {
      mssg_t message = 0;
      if (rnode != SyntaxKey::ClosureBlock && r2node != SyntaxKey::None) {
         message = scope.moduleScope->buildins.iif_message;

         roperand = compileExpression(writer, scope, rnode, 0, EAttr::Parameter, updatedOuterArgs);
         roperand2 = compileExpression(writer, scope, r2node, 0, EAttr::Parameter, updatedOuterArgs);
      }
      else {
         message = resolveOperatorMessage(scope.moduleScope, operatorId);

         roperand = compileClosure(writer, scope, rnode, EAttr::None, updatedOuterArgs);
         roperand2 = compileClosure(writer, scope, r2node, EAttr::None, updatedOuterArgs);
      }

      ArgumentsInfo messageArguments;
      messageArguments.add(loperand);
      messageArguments.add(roperand);
      if (r2node != SyntaxKey::None) {
         messageArguments.add(roperand2);
      }

      ref_t signRef = scope.module->mapSignature(arguments, argLen, false);

      retVal = compileMessageOperation(writer, scope, node, loperand, message, signRef, messageArguments,
         EAttr::NoExtension, updatedOuterArgs);
   }

   // HOTFIX : to compenstate the closed statement above
   writer.appendNode(BuildKey::OpenStatement);

   return retVal;
}

ObjectInfo Compiler :: compileBranchingOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   int operatorId, bool retValExpected)
{
   SyntaxNode lnode = node.firstChild();
   SyntaxNode rnode = /*skipNestedExpression(*/lnode.nextNode()/*)*/;
   SyntaxNode r2node = {};
   if (operatorId == IF_ELSE_OPERATOR_ID)
      r2node = rnode.nextNode();

   ArgumentsInfo updatedOuterArgs;

   ObjectInfo loperand = compileExpression(writer, scope, lnode, 0, EAttr::Parameter, &updatedOuterArgs);

   // HOTFIX : to allow correct step over the branching statement
   writer.appendNode(BuildKey::EndStatement);
   writer.appendNode(BuildKey::VirtualBreakoint);

   auto retVal = compileBranchingOperation(writer, scope, loperand, node, rnode, r2node, operatorId, &updatedOuterArgs, retValExpected);

   writer.appendNode(BuildKey::OpenStatement); // HOTFIX : to match the closing statement

   return retVal;
}

ObjectInfo Compiler :: compileMessageOperationR(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo target, 
   SyntaxNode messageNode, bool propertyMode)
{
   ArgumentsInfo arguments;
   ArgumentsInfo updatedOuterArgs;

   switch (target.mode) {
      case TargetMode::Casting:
      {
         bool dummy = false;
         compileMessageArguments(writer, scope, messageNode, arguments, 0, EAttr::NoPrimitives, &updatedOuterArgs, dummy);
         if (arguments.count() == 1 && !dummy) {
            return convertObject(writer, scope, messageNode, arguments[0], retrieveStrongType(scope, target), false);
         }
         else scope.raiseError(errInvalidOperation, messageNode);
         break;
      }
      default:
         {
            // NOTE : the operation target shouldn't be a primitive type
            ObjectInfo source = validateObject(writer, scope, messageNode, target, 0, true, true, false);

            mssg_t messageRef = mapMessage(scope, messageNode, propertyMode, false, false);

            mssg_t resolvedMessage = _logic->resolveSingleDispatch(*scope.moduleScope,
               retrieveType(scope, source), messageRef);

            ref_t expectedSignRef = 0;
            if (resolvedMessage)
               scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

            if (!test(messageRef, FUNCTION_MESSAGE)) {
               arguments.add(source);
            }

            bool withVariadicArg = false;
            ref_t implicitSignatureRef = compileMessageArguments(writer, scope, messageNode, arguments, expectedSignRef, 
               EAttr::NoPrimitives, &updatedOuterArgs, withVariadicArg);

            EAttr opMode = EAttr::None;
            if (withVariadicArg) {
               messageRef |= VARIADIC_MESSAGE;

               opMode = EAttr::WithVariadicArg;
            }

            return compileMessageOperation(writer, scope, messageNode, source, messageRef,
               implicitSignatureRef, arguments, opMode, &updatedOuterArgs);

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
   if (current == SyntaxKey::MessageOperation || current == SyntaxKey::PropertyOperation) {
      SyntaxNode objNode = current.firstChild();

      target = compileObject(writer, scope, objNode, EAttr::Parameter, nullptr);

      writer.newNode(BuildKey::AltOp, ehLocal.argument);

      writer.newNode(BuildKey::Tape);
      compileMessageOperationR(writer, scope, target, objNode.nextNode(), current == SyntaxKey::PropertyOperation);
      writer.closeNode();
   }
   else scope.raiseError(errInvalidOperation, node);

   writer.newNode(BuildKey::Tape);
   SyntaxNode altNode = current.nextNode().firstChild();

   if (target.mode == TargetMode::Casting) {
      // HOTFIX : for the cast, the argument is a target
      target = compileExpression(writer, scope, current.findChild(SyntaxKey::Expression),
         0, EAttr::Parameter, nullptr);
   }

   compileMessageOperationR(writer, scope, target, altNode.firstChild(), false);

   writer.closeNode();

   writer.closeNode();

   return { ObjectKind::Object };
}

ObjectInfo Compiler :: compileIsNilOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   ObjectInfo loperand = {};
   SyntaxNode current = node.firstChild();

   if (current == SyntaxKey::MessageOperation || current == SyntaxKey::PropertyOperation) {
      SyntaxNode objNode = current.firstChild();

      loperand = compileObject(writer, scope, objNode, EAttr::Parameter, nullptr);

      writer.newNode(BuildKey::AltOp, ehLocal.argument);

      writer.newNode(BuildKey::Tape);
      compileMessageOperationR(writer, scope, loperand, objNode.nextNode(),
         current == SyntaxKey::PropertyOperation);
      writer.closeNode();

      writer.newNode(BuildKey::Tape);
      writer.appendNode(BuildKey::NilReference, 0);
      writer.closeNode();

      writer.closeNode();

      loperand = saveToTempLocal(writer, scope, { ObjectKind::Object });
   }
   else if (current == SyntaxKey::Object) {
      loperand = compileObject(writer, scope, current, EAttr::Parameter, nullptr);
   }
   else scope.raiseError(errInvalidOperation, node);

   SyntaxNode altNode = current.nextNode();
   ObjectInfo roperand = compileExpression(writer, scope, altNode, 0, EAttr::Parameter, nullptr);

   writeObjectInfo(writer, scope, roperand);
   writer.appendNode(BuildKey::SavingInStack);
   writeObjectInfo(writer, scope, loperand);
   writer.appendNode(BuildKey::NilOp, ISNIL_OPERATOR_ID);

   return { ObjectKind::Object };

}

ObjectInfo Compiler :: compileFinalOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   int index1 = scope.newTempLocal();

   SyntaxNode finallyNode = node.findChild(SyntaxKey::FinallyBlock).firstChild();
   SyntaxNode opNode = node.firstChild();
   if (opNode.existChild(SyntaxKey::ClosureBlock))
      opNode = opNode.findChild(SyntaxKey::ClosureBlock);

   writer.newNode(BuildKey::FinalOp, ehLocal.argument);
   writer.appendNode(BuildKey::Index, index1);

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, opNode, 0, EAttr::None, nullptr);
   writer.closeNode();

   if (finallyNode.existChild(SyntaxKey::ClosureBlock))
      finallyNode = finallyNode.findChild(SyntaxKey::ClosureBlock);

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, finallyNode, 0, EAttr::None, nullptr);
   writer.closeNode();

   writer.closeNode();

   return {};
}

ObjectInfo Compiler :: compileCatchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ObjectInfo ehLocal = declareTempStructure(scope, { (int)scope.moduleScope->ehTableEntrySize, false });

   SyntaxNode catchNode = node.findChild(SyntaxKey::CatchDispatch);
   SyntaxNode finallyNode = node.findChild(SyntaxKey::FinallyBlock).firstChild();
   SyntaxNode opNode = node.firstChild();
   if (opNode.existChild(SyntaxKey::ClosureBlock))
      opNode = opNode.findChild(SyntaxKey::ClosureBlock);

   writer.newNode(BuildKey::CatchOp, ehLocal.argument);

   writer.newNode(BuildKey::Tape);
   compileExpression(writer, scope, opNode, 0, EAttr::None, nullptr);
   writer.closeNode();

   writer.newNode(BuildKey::Tape);
   compileMessageOperationR(writer, scope, { ObjectKind::Object }, 
      catchNode.firstChild().firstChild(), false);
   writer.closeNode();

   if (finallyNode != SyntaxKey::None) {
      if (finallyNode.existChild(SyntaxKey::ClosureBlock))
         finallyNode = finallyNode.findChild(SyntaxKey::ClosureBlock);

      writer.newNode(BuildKey::Tape);
      compileExpression(writer, scope, finallyNode, 0, EAttr::None, nullptr);
      writer.closeNode();
   }

   writer.closeNode();

   return { ObjectKind::Object };
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


ObjectInfo Compiler :: mapExtMessageConstant(Scope& scope, SyntaxNode node, ref_t actionRef, ref_t extension)
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

   size_t index = (*messageName).find('[');
   assert(index != NOTFOUND_POS); // !! temporal

   ustr_t extRefName = scope.moduleScope->resolveFullName(extension);
   messageName.insert(">", index);
   messageName.insert(extRefName, index);
   if (isWeakReference(extRefName)) {
      messageName.insert(scope.module->name(), index);
   }
   messageName.insert("<", index);

   ref_t constRef = scope.module->mapConstant(*messageName);

   ref_t constType = V_EXTMESSAGE64;
   switch (scope.moduleScope->ptrSize) {
      case 4:
         constType = V_EXTMESSAGE64;
         break;
      case 8:
         constType = V_EXTMESSAGE128;
         break;
      default:
         break;
   }

   return { ObjectKind::ExtMssgLiteral, { constType, extension }, constRef };
}

ObjectInfo Compiler :: defineTerminalInfo(Scope& scope, SyntaxNode node, TypeInfo declaredTypeInfo, bool variableMode, 
   bool forwardMode, bool refOp, bool mssgOp, bool memberMode, bool& invalid, ExpressionAttribute attrs)
{
   ObjectInfo retVal = {};
   bool ignoreDuplicates = EAttrs::testAndExclude(attrs, ExpressionAttribute::IgnoreDuplicate);
   bool invalidForNonIdentifier = forwardMode || variableMode || refOp || mssgOp || memberMode;

   switch (node.key) {
      case SyntaxKey::TemplateType:
      {
         TypeAttributes typeAttributes = {};
         TypeInfo typeInfo = resolveTypeAttribute(scope, node, typeAttributes, false, false);
         retVal = { ObjectKind::Class, typeInfo, 0u };

         retVal = mapClassSymbol(scope, retrieveStrongType(scope, retVal));
         break;
      }
      case SyntaxKey::globalreference:
         invalid = variableMode;
         retVal = scope.mapGlobal(node.identifier());
         break;
      case SyntaxKey::identifier:
      case SyntaxKey::reference:
         if (variableMode) {
            invalid = forwardMode;

            if (declareVariable(scope, node, declaredTypeInfo, ignoreDuplicates)) {
               retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, 
                  attrs | ExpressionAttribute::Local);

               if (_trackingUnassigned && refOp) {
                  scope.markAsAssigned(retVal);
               }
            }
            else retVal = scope.mapIdentifier(node.identifier(), node.key == SyntaxKey::reference, attrs);
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
               case ObjectKind::ParamAddress:
                  retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                  break;
               case ObjectKind::Local:
                  retVal.kind = ObjectKind::RefLocal;
                  retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                  break;
               case ObjectKind::ByRefParam:
                  // allowing to pass by ref parameter directly
                  retVal.kind = ObjectKind::ParamReference;
                  retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                  break;
               case ObjectKind::ByRefParamAddress:
                  // allowing to pass by ref parameter directly
                  retVal.kind = ObjectKind::ParamAddress;
                  retVal.typeInfo = { V_WRAPPER, retVal.typeInfo.typeRef };
                  //retVal.mode = TargetMode::UnboxingRequired;
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

   return retVal;
}

ObjectInfo Compiler :: mapTerminal(Scope& scope, SyntaxNode node, TypeInfo declaredTypeInfo, EAttr attrs)
{
   bool forwardMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Forward);
   bool variableMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewVariable);
   bool externalOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::Extern);
   bool newOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewOp);
   bool castOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::CastOp);
   bool refOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::RefOp);
   bool mssgOp = EAttrs::testAndExclude(attrs, ExpressionAttribute::MssgNameLiteral);
   bool probeMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::ProbeMode);
   bool memberMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Member);

   ObjectInfo retVal;
   bool invalid = false;
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
      if (node.key == SyntaxKey::identifier && EAttrs::testAndExclude(attrs, ExpressionAttribute::RetrievingType)) {
         auto varInfo = scope.mapIdentifier(node.identifier(), false, attrs);
         if (varInfo.kind != ObjectKind::Unknown) {
            retVal = { ObjectKind::Class, varInfo.typeInfo, 0u, newOp ? TargetMode::Creating : TargetMode::Casting };
         }
         else return varInfo;
      }
      else {
         switch (node.key) {
            case SyntaxKey::TemplateType:
            case SyntaxKey::ArrayType:
            case SyntaxKey::Type:
            case SyntaxKey::identifier:
            case SyntaxKey::reference:
            {
               TypeAttributes typeAttributes = {};
               TypeInfo typeInfo = resolveTypeAttribute(scope, node, typeAttributes, false, false);

               retVal = { ObjectKind::Class, typeInfo, 0u, newOp ? TargetMode::Creating : TargetMode::Casting };
               if (CompilerLogic::isPrimitiveArrRef(retVal.typeInfo.typeRef) && newOp)
                  retVal.mode = TargetMode::CreatingArray;
               break;
            }
            default:
               invalid = true;
               break;
         }
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
   else retVal = defineTerminalInfo(scope, node, declaredTypeInfo, variableMode, forwardMode, 
      refOp, mssgOp, memberMode, invalid, attrs);

   if (invalid)
      scope.raiseError(errInvalidOperation, node);

   if (probeMode)
      retVal.mode = TargetMode::Probe;

   return retVal;
}

inline SyntaxNode retrieveTerminalOrType(SyntaxNode node)
{
   if (test((unsigned int)node.key, (unsigned int)SyntaxKey::TerminalMask))
      return node;

   SyntaxNode current = node.firstChild();
   SyntaxNode last = {};
   while (current != SyntaxKey::None) {
      if (test((unsigned int)current.key, (unsigned int)SyntaxKey::TerminalMask)) {
         last = current;
      }
      else if (current == SyntaxKey::ArrayType || current == SyntaxKey::Type || current == SyntaxKey::TemplateType) {
         last = current;
      }

      current = current.nextNode();
   }

   return last;
}

ObjectInfo Compiler :: mapObject(Scope& scope, SyntaxNode node, EAttrs mode)
{
   SyntaxNode terminalNode = retrieveTerminalOrType(node);

   TypeInfo declaredTypeInfo = {};
   declareExpressionAttributes(scope, node, declaredTypeInfo, mode);
   if (mode.test(EAttr::Lookahead)) {
      if (mode.test(EAttr::NewVariable)) {
         return { ObjectKind::NewVariable, declaredTypeInfo, 0, 0 };
      }
      else if (mode.test(EAttr::MssgNameLiteral)) {
         if (declaredTypeInfo.typeRef) {
            SyntaxNode actionTerminal = terminalNode.findChild(SyntaxKey::identifier);
            if (actionTerminal != SyntaxKey::None) {
               return { ObjectKind::ExtMssgLiteral, { V_MESSAGE, declaredTypeInfo.typeRef },
                  scope.module->mapAction(actionTerminal.identifier(), 0, false) };
            }
         }
         else return { ObjectKind::MssgLiteral, { V_MESSAGE },
            scope.module->mapAction(terminalNode.identifier(), 0, false) };
      }
      return {};
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
      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, object.typeInfo.typeRef);
      int tempLocal = allocateLocalAddress(scope, sizeInfo.size, false);

      if (sizeInfo.size == 8) {
         writer.appendNode(BuildKey::SavingLongIndex, tempLocal);
      }
      else writer.appendNode(BuildKey::SavingIndex, tempLocal);

      return { ObjectKind::TempLocalAddress, object.typeInfo, (ref_t)tempLocal };
   }
   else if (object.kind == ObjectKind::FloatExtern) {
      auto sizeInfo = _logic->defineStructSize(*scope.moduleScope, object.typeInfo.typeRef);
      int tempLocal = allocateLocalAddress(scope, sizeInfo.size, false);

      writer.appendNode(BuildKey::SavingFloatIndex, tempLocal);

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
   ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs)
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
   else return compileExpression(writer, scope, node, 0, mode, updatedOuterArgs);
}

ObjectInfo Compiler :: typecastObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef)
{
   if (targetRef == scope.moduleScope->buildins.superReference)
      return source;

   ustr_t refName2 = scope.module->resolveReference(targetRef);

   ref_t signRef = scope.module->mapSignature(&targetRef, 1, false);
   ref_t actionRef = scope.module->mapAction(CAST_MESSAGE, signRef, false);

   mssg_t typecastMssg = encodeMessage(actionRef, 1, CONVERSION_MESSAGE);

   ArgumentsInfo arguments;
   arguments.add(source);

   ObjectInfo retVal = compileMessageOperation(writer, scope, node, source, typecastMssg,
      0, arguments, EAttr::None, nullptr);
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

ObjectInfo Compiler :: convertIntLiteral(ExprScope& scope, SyntaxNode node, ObjectInfo source, ref_t targetRef)
{
   switch (targetRef) {
      case V_INT8:
         if (source.extra < 0 || source.extra > 255)
            scope.raiseError(errInvalidOperation, node);
         break;
      case V_INT16:
         if (source.extra < INT16_MIN || source.extra > INT16_MAX)
            scope.raiseError(errInvalidOperation, node);
         break;
      case V_INT64:
         source.kind = ObjectKind::LongLiteral;
         break;
      case V_FLOAT64:
         source.kind = ObjectKind::Float64Literal;
         source.reference = mapFloat64Const(scope, source.extra);
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         break;
   }
   
   source.typeInfo = { targetRef };

   return source;
}

ObjectInfo Compiler :: convertObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ObjectInfo source,
   ref_t targetRef, bool dynamicRequired)
{
   if (!_logic->isCompatible(*scope.moduleScope, { targetRef }, source.typeInfo, false)) {
      if (source.typeInfo.typeRef == V_WRAPPER) {
         // unbox wrapper for the conversion
         source.typeInfo = { source.typeInfo.elementRef };
      }
      NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
      
      auto conversionRoutine = _logic->retrieveConversionRoutine(this, *scope.moduleScope, *nsScope->nsName, 
         targetRef, source.typeInfo);
      if (conversionRoutine.result == ConversionResult::BoxingRequired) {
         // if it is implcitily compatible
         switch (source.kind) {
            case ObjectKind::TempLocalAddress:
            case ObjectKind::LocalAddress:
            case ObjectKind::IntLiteral:
            case ObjectKind::MssgLiteral:
            case ObjectKind::CharacterLiteral:
            case ObjectKind::RefLocal:
            case ObjectKind::ParamReference:
               source.typeInfo.typeRef = targetRef;
               break;
            case ObjectKind::SelfBoxableLocal:
            case ObjectKind::ParamAddress:
               if (source.mode == TargetMode::Conditional && source.typeInfo.typeRef != targetRef) {
                  source.mode = TargetMode::None;
                  source.typeInfo.typeRef = targetRef;

                  return source;
               }
               else source.typeInfo.typeRef = targetRef;
               break;
            default:
               if (source.kind == ObjectKind::SelfLocal && source.mode == TargetMode::ArrayContent) {
                  source.typeInfo.typeRef = targetRef;
                  source.kind = ObjectKind::SelfBoxableLocal;
               }
               else return boxArgument(writer, scope, source, false, true, false, targetRef);
         }
      }
      else if (conversionRoutine.result == ConversionResult::VariadicBoxingRequired) {
         switch (source.kind) {
            case ObjectKind::VArgParam:
               source.typeInfo.typeRef = targetRef;
               break;
            default:
               assert(false);
               break;
         }

      }
      else if (conversionRoutine.result == ConversionResult::Conversion) {
         if (!dynamicRequired && source.kind == ObjectKind::IntLiteral && _logic->isNumericType(*scope.moduleScope, targetRef)) {
            // HOTFIX : convert int literal in place
            source = convertIntLiteral(scope, node, source, targetRef);
         }
         else {
            ArgumentsInfo arguments;
            arguments.add(source);
            ref_t signRef = scope.module->mapSignature(&source.typeInfo.typeRef, 1, false);

            return compileNewOp(writer, scope, node, mapClassSymbol(scope, targetRef),
               signRef, arguments);
         }
      }
      else if (conversionRoutine.result == ConversionResult::NativeConversion) {
         if (source.kind == ObjectKind::IntLiteral && _logic->isNumericType(*scope.moduleScope, targetRef)) {
            // HOTFIX : convert int literal in place
            source = convertIntLiteral(scope, node, source, targetRef);
         }
         else source = compileNativeConversion(writer, scope, node, source, conversionRoutine.operationKey);
      }
      else source = typecastObject(writer, scope, node, source, targetRef);
   }

   return source;
}

ObjectInfo Compiler :: compileNestedExpression(BuildTreeWriter& writer, InlineClassScope& scope, ExprScope& ownerScope,
   EAttr mode, ArgumentsInfo* updatedOuterArgs)
{
   bool paramMode = EAttrs::test(mode, EAttr::Parameter);
   ref_t nestedRef = scope.reference;
   if (test(scope.info.header.flags, elVirtualVMT))
      nestedRef = scope.info.header.parentRef;

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
            case ObjectKind::OuterSelf:
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

      if (scope.outers.count() != scope.info.fields.count()) {
         if (scope.info.fields.count() != 0) {
            writer.appendNode(BuildKey::FillOp, scope.info.fields.count());
         }
      }

      // second pass : fill members
      int argIndex = 0;
      int preservedClosure = 0;
      for (auto it = scope.outers.start(); !it.eof(); ++it) {
         ObjectInfo source = (*it).outerObject;
         ObjectInfo arg = list[argIndex];

         auto fieldInfo = scope.info.fields.get(it.key());

         switch (arg.kind) {
            case ObjectKind::SelfLocal:
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

         if (updatedOuterArgs && (*it).updated) {
            if (!preservedClosure) {
               updatedOuterArgs->add({ ObjectKind::ClosureInfo });
               // reserve place for the closure
               preservedClosure = updatedOuterArgs->count_pos();
               updatedOuterArgs->add({ });
            }

            updatedOuterArgs->add({ ObjectKind::MemberInfo, (*it).reference });
            updatedOuterArgs->add(source);
         }

         argIndex++;
      }

      // call init handler if is available
      if (scope.info.methods.exist(scope.moduleScope->buildins.init_message)) {
         compileInlineInitializing(writer, scope, {});
      }

      if (paramMode || preservedClosure) {
         retVal = saveToTempLocal(writer, ownerScope, retVal);

         if (preservedClosure)
            (*updatedOuterArgs)[preservedClosure] = retVal;
      }

      return retVal;
   }
}

ref_t Compiler :: mapConstantReference(Scope& ownerScope)
{
   ref_t nestedRef = 0;
   SymbolScope* owner = Scope::getScope<SymbolScope>(ownerScope, Scope::ScopeLevel::Symbol);
   if (owner) {
      nestedRef = owner->reference;
   }

   if (!nestedRef)
      nestedRef = ownerScope.moduleScope->mapAnonymous();

   return nestedRef;
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

ObjectInfo Compiler :: compileClosure(BuildTreeWriter& writer, ExprScope& ownerScope, SyntaxNode node, ExpressionAttribute mode, 
   ArgumentsInfo* updatedOuterArgs)
{
   ref_t nestedRef = mapNested(ownerScope, mode);
   InlineClassScope scope(&ownerScope, nestedRef);

   BuildNode buildNode = writer.CurrentNode();
   while (buildNode != BuildKey::Root)
      buildNode = buildNode.parentNode();

   BuildTreeWriter nestedWriter(buildNode);
   compileClosureClass(nestedWriter, scope, node);

   return compileNestedExpression(writer, scope, ownerScope, mode, updatedOuterArgs);
}

ObjectInfo Compiler :: compileNested(BuildTreeWriter& writer, ExprScope& ownerScope, SyntaxNode node, ExpressionAttribute mode,
   ArgumentsInfo* updatedOuterArgs)
{
   TypeInfo parentInfo = { ownerScope.moduleScope->buildins.superReference };
   EAttrs nestedMode = { EAttr::NestedDecl };
   declareExpressionAttributes(ownerScope, node, parentInfo, nestedMode);

   //// allow only new and type attrobutes
   //if (nestedMode.attrs != EAttr::None && !EAttrs::test(nestedMode.attrs, EAttr::NewOp) && !EAttrs::test(nestedMode.attrs, EAttr::NewVariable))
   //   ownerScope.raiseError(errInvalidOperation, node);

   ref_t nestedRef = mapNested(ownerScope, mode);
   InlineClassScope scope(&ownerScope, nestedRef);

   compileNestedClass(writer, scope, node, parentInfo.typeRef);

   return compileNestedExpression(writer, scope, ownerScope, mode, updatedOuterArgs);
}

ObjectInfo Compiler :: compileLoopExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   ObjectInfo retVal = { ObjectKind::Object };

   writer.newNode(BuildKey::LoopOp);

   compileExpression(writer, scope, node, 0, mode, nullptr);

   writer.appendNode(BuildKey::VirtualBreakoint);

   writer.closeNode();

   return retVal;
}

ObjectInfo Compiler :: compileExternExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   writer.newNode(BuildKey::ExternOp);

   compileExpression(writer, scope, node, 0, mode, nullptr);

   writer.closeNode();

   return { };
}

ObjectInfo Compiler :: validateObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ObjectInfo retVal, ref_t targetRef, bool noPrimitives, bool paramMode, bool dynamicRequired)
{
   if (!targetRef && retVal.typeInfo.isPrimitive() && noPrimitives) {
      targetRef = retrieveStrongType(scope, retVal);
   }

   if ((paramMode || targetRef) && hasToBePresaved(retVal)) {
      retVal = saveToTempLocal(writer, scope, retVal);
   }
   if (targetRef) {
      retVal = convertObject(writer, scope, node, retVal, targetRef, dynamicRequired);
      if (paramMode && hasToBePresaved(retVal))
         retVal = saveToTempLocal(writer, scope, retVal);
   }

   return retVal;
}

void Compiler :: compileSwitchOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   Interpreter interpreter(scope.moduleScope, _logic);
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();

   ObjectInfo loperand = compileObject(writer, scope, current, EAttr::Parameter, nullptr);

   writer.newNode(BuildKey::Switching);

   current = current.nextNode();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::SwitchOption:
         {
            SyntaxNode optionNode = current.firstChild();

            writer.newNode(BuildKey::SwitchOption);

            int operator_id = EQUAL_OPERATOR_ID;
            ObjectInfo value = evalExpression(interpreter, scope, optionNode);
            arguments.clear();
            arguments.add(loperand);
            arguments.add(value);
            ObjectInfo retVal = compileOperation(writer, scope, node, arguments, operator_id, 0, nullptr);
            compileBranchingOperation(writer, scope, retVal, {}, optionNode.nextNode(), {}, IF_OPERATOR_ID, nullptr, false);

            writer.closeNode();
            break;
         }
         case SyntaxKey::SwitchLastOption:
            writer.newNode(BuildKey::ElseOption);
            compileSubCode(writer, scope, current.firstChild(), EAttr::None);
            writer.closeNode();
            break;
         default:
            assert(false);
            break;
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

ref_t Compiler :: resolveTupleClass(Scope& scope, SyntaxNode node, ArgumentsInfo& items)
{
   IdentifierString tupleName(scope.module->resolveReference(scope.moduleScope->buildins.tupleTemplateReference));

   List<SyntaxNode> parameters({});

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   SyntaxTreeWriter dummyWriter(dummyTree);
   dummyWriter.newNode(SyntaxKey::Root);

   for (size_t i = 0; i < items.count(); i++) {
      ref_t typeRef = retrieveStrongType(scope, items[i]);

      dummyWriter.newNode(SyntaxKey::TemplateArg, typeRef);
      dummyWriter.newNode(SyntaxKey::Type);

      ustr_t referenceName = scope.moduleScope->module->resolveReference(typeRef);
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

   tupleName.append('#');
   tupleName.appendInt(items.count_int());

   ref_t templateReference = 0;
   if (isWeakReference(*tupleName)) {
      templateReference = scope.module->mapReference(*tupleName, true);
   }
   else templateReference = scope.moduleScope->mapFullReference(*tupleName, true);

   if (!templateReference)
      scope.raiseError(errInvalidOperation, node);

   NamespaceScope* nsScope = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   return _templateProcessor->generateClassTemplate(*scope.moduleScope, *nsScope->nsName,
      templateReference, parameters, false, nullptr);
}

ObjectInfo Compiler :: compileTupleCollectiom(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ArgumentsInfo arguments;
   EAttr paramMode = EAttr::Parameter;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         auto argInfo = compileExpression(writer, scope, current, 0,
            paramMode, nullptr);

         arguments.add(argInfo);
      }

      current = current.nextNode();
   }

   ref_t tupleRef = resolveTupleClass(scope, node, arguments);

   writer.newNode(BuildKey::CreatingClass, arguments.count_pos());
   writer.appendNode(BuildKey::Type, tupleRef);
   writer.closeNode();

   for (size_t i = 0; i < arguments.count(); i++) {
      writer.appendNode(BuildKey::SavingInStack, 0);
      writeObjectInfo(writer, scope, arguments[i]);
      writer.appendNode(BuildKey::AccSwapping);
      writer.appendNode(BuildKey::FieldAssigning, (pos_t)i);
   }

   return { ObjectKind::Object, { tupleRef }, 0 };
}

ObjectInfo Compiler :: compileCollection(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, ExpressionAttribute mode)
{
   bool constOne = EAttrs::testAndExclude(mode, EAttr::ConstantExpr);

   SyntaxNode current = node.firstChild();

   ObjectInfo typeInfo = compileObject(writer, scope, node.firstChild(), EAttr::NestedDecl, nullptr);
   if (typeInfo.kind != ObjectKind::Class)
      scope.raiseError(errInvalidOperation, node);

   ref_t collectionTypeRef = retrieveStrongType(scope, typeInfo);

   ClassInfo collectionInfo;
   _logic->defineClassInfo(*scope.moduleScope, collectionInfo, collectionTypeRef, false, true);

   if (!test(collectionInfo.header.flags, elDynamicRole))
      scope.raiseError(errInvalidOperation, node);

   if (constOne && node.arg.reference) {
      bool byValue = _logic->isEmbeddableArray(*scope.moduleScope, collectionTypeRef);

      return { byValue ? ObjectKind::Constant : ObjectKind::ConstArray, { collectionTypeRef }, node.arg.reference };
   }

   auto fieldInfo = *(collectionInfo.fields.start());
   ref_t elementTypeRef = retrieveStrongType(scope, { ObjectKind::Object, { fieldInfo.typeInfo.elementRef }, 0 });

   auto sizeInfo = _logic->defineStructSize(collectionInfo);
   ArgumentsInfo arguments;
   EAttr paramMode = EAttr::Parameter;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         auto argInfo = compileExpression(writer, scope, current, elementTypeRef,
            paramMode, nullptr);
         //ref_t argRef = retrieveStrongType(scope, argInfo);
         //if (signatureLen >= ARG_COUNT) {
         //   signatureLen++;
         //}
         //else if (argRef) {
         //   signatures[signatureLen++] = argRef;
         //}
         //else signatures[signatureLen++] = superReference;

         arguments.add(argInfo);
      }

      current = current.nextNode();
   }

   bool structMode = false;
   if (sizeInfo.size < 0) {
      structMode = true;
      writer.newNode(BuildKey::CreatingStruct, arguments.count_pos() * -sizeInfo.size);
   }
   else if (sizeInfo.size == 0) {
      writer.newNode(BuildKey::CreatingClass, arguments.count_pos());
   }
   else scope.raiseError(errInvalidOperation, node);
   writer.appendNode(BuildKey::Type, collectionTypeRef);
   writer.closeNode();

   if (structMode) {
      writer.appendNode(BuildKey::SavingInStack, 0);

      for (size_t i = 0; i < arguments.count(); i++) {
         writeObjectInfo(writer, scope, arguments[i]);
         writer.newNode(BuildKey::CopyingItem, -sizeInfo.size);
         writer.appendNode(BuildKey::Index, (pos_t)i);
         writer.closeNode();
      }

      writer.appendNode(BuildKey::Argument, 0);
   }
   else {
      for (size_t i = 0; i < arguments.count(); i++) {
         writer.appendNode(BuildKey::SavingInStack, 0);
         writeObjectInfo(writer, scope, arguments[i]);
         writer.appendNode(BuildKey::AccSwapping);
         writer.appendNode(BuildKey::FieldAssigning, (pos_t)i);
      }
   }

   return { ObjectKind::Object, { collectionTypeRef }, 0 };
}

void Compiler :: compileYieldOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   CodeScope* codeScope = Scope::getScope<CodeScope>(scope, Scope::ScopeLevel::Code);
   MethodScope* methodScope = Scope::getScope<MethodScope>(scope, Scope::ScopeLevel::Method);
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   if (!methodScope->isYieldable())
      scope.raiseError(errInvalidOperation, node);

   ObjectInfo contextField = classScope->mapField(YIELD_CONTEXT_FIELD, EAttr::None);

   writer.newNode(BuildKey::YieldingOp, -scope.moduleScope->ptrSize);
   writer.newNode(BuildKey::Tape);

   writeObjectInfo(writer, scope, contextField);
   writer.appendNode(BuildKey::SavingStackDump);

   compileRetExpression(writer, *codeScope, node, EAttr::None);

   writer.closeNode();
   writer.closeNode();
}

ObjectInfo Compiler :: compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ref_t targetRef, ExpressionAttribute mode, ArgumentsInfo* updatedOuterArgs)
{
   bool paramMode = EAttrs::testAndExclude(mode, EAttr::Parameter);
   bool noPrimitives = EAttrs::testAndExclude(mode, EAttr::NoPrimitives);
   bool dynamicRequired = EAttrs::testAndExclude(mode, EAttr::DynamicObject);

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
         retVal = compileOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS, targetRef, mode);
         break;
      case SyntaxKey::BreakOperation:
      case SyntaxKey::ContinueOperation:
         retVal = compileSpecialOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS, targetRef);
         break;
      case SyntaxKey::YieldOperation:
         compileYieldOperation(writer, scope, current);
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
         retVal = compileBranchingOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS,
            EAttrs::test(mode, EAttr::RetValExpected));
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
      case SyntaxKey::FinalOperation:
         retVal = compileFinalOperation(writer, scope, current);
         break;
      case SyntaxKey::AltOperation:
         retVal = compileAltOperation(writer, scope, current);
         break;
      case SyntaxKey::IsNilOperation:
         retVal = compileIsNilOperation(writer, scope, current);
         break;
      case SyntaxKey::ReturnExpression:
         retVal = compileExpression(writer, scope, current.firstChild(), 0, mode, updatedOuterArgs);
         break;
      case SyntaxKey::Expression:
         retVal = compileExpression(writer, scope, current, 0, mode, updatedOuterArgs);
         break;
      case SyntaxKey::Object:
         retVal = compileObject(writer, scope, current, mode, updatedOuterArgs);
         break;
      case SyntaxKey::NestedBlock:
         retVal = compileNested(writer, scope, current, mode, updatedOuterArgs);
         break;
      case SyntaxKey::ClosureBlock:
         retVal = compileClosure(writer, scope, current, mode, updatedOuterArgs);
         break;
      case SyntaxKey::LazyOperation:
         retVal = compileClosure(writer, scope, current, mode, updatedOuterArgs);
         break;
      case SyntaxKey::CodeBlock:
         retVal = compileSubCode(writer, scope, current, mode, true);
         break;
      case SyntaxKey::SwitchOperation:
         compileSwitchOperation(writer, scope, current);
         break;
      case SyntaxKey::CollectionExpression:
         retVal = compileCollection(writer, scope, current, mode);
         break;
      case SyntaxKey::Type:
      case SyntaxKey::ReferOperation:
         scope.raiseError(errInvalidOperation, node);
         break;
      case SyntaxKey::Attribute:
      {
         EAttrs exprAttr = mode;
         if (!_logic->validateExpressionAttribute(current.arg.reference, exprAttr))
            scope.raiseError(errInvalidHint, current);;

         return compileExpression(writer, scope, current.nextNode(), targetRef, exprAttr.attrs, updatedOuterArgs);
         break;
      }
      case SyntaxKey::TupleCollection:
         retVal = compileTupleCollectiom(writer, scope, current);
         break;
      case SyntaxKey::TupleAssignOperation:
         retVal = compileTupleAssigning(writer, scope, current);
         break;
      case SyntaxKey::None:
         assert(false);
         break;
      default:
         retVal = compileObject(writer, scope, node, mode, updatedOuterArgs);
         break;
   }

   retVal = validateObject(writer, scope, node, retVal, targetRef, 
      noPrimitives, paramMode, dynamicRequired);

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

ObjectInfo Compiler :: compileRetExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, EAttr mode)
{
   ExprScope scope(&codeScope);

   bool autoMode = false;
   bool dynamicRequired = EAttrs::testAndExclude(mode, EAttr::DynamicObject);
   ref_t outputRef = codeScope.getOutputRef();
   if (outputRef == V_AUTO) {
      autoMode = true;
      outputRef = 0;
   }

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(node), BuildKey::Breakpoint);

   ObjectInfo retVal = {};
   SyntaxNode exprNode = node.findChild(SyntaxKey::Expression, SyntaxKey::CodeBlock);
   switch (exprNode.key) {
      case SyntaxKey::Expression:
         retVal = compileExpression(writer, scope, node.findChild(SyntaxKey::Expression), outputRef,
            mode | EAttr::Root | EAttr::RetValExpected, nullptr);
         break;
      case SyntaxKey::CodeBlock:
         retVal = compileCode(writer, codeScope, exprNode, true);
         break;
      default:
         assert(false);
         break;
   }

   if (codeScope.isByRefHandler()) {
      compileAssigningOp(writer, scope, codeScope.mapByRefReturnArg(), retVal);

      retVal = {};
   }
   else {
      retVal = boxArgument(writer, scope, retVal, 
         !dynamicRequired && retVal.kind == ObjectKind::SelfBoxableLocal, true, false);

      if (!hasToBePresaved(retVal)) {
         writeObjectInfo(writer, scope, retVal);
      }
      else if (retVal.kind == ObjectKind::Symbol) {
         writeObjectInfo(writer, scope, retVal);

         retVal = { ObjectKind::Object, retVal.typeInfo, 0 };
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

ObjectInfo Compiler :: compileRootExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node, EAttr mode)
{
   ExprScope scope(&codeScope);

   writer.appendNode(BuildKey::OpenStatement);
   addBreakpoint(writer, findObjectNode(node), BuildKey::Breakpoint);

   auto retVal = compileExpression(writer, scope, node, 0, mode, nullptr);

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
   ref_t constRef = generateConstant(scope, retVal, scope.reference, false);
   if (constRef) {
      switch (retVal.kind) {
         case ObjectKind::Singleton:
            scope.info.symbolType = SymbolType::Singleton;
            scope.info.valueRef = retVal.reference;
            scope.info.typeRef = retrieveStrongType(scope, retVal);
            break;
         case ObjectKind::StringLiteral:
         case ObjectKind::WideStringLiteral:
         case ObjectKind::Float64Literal:
            scope.info.symbolType = SymbolType::Constant;
            scope.info.valueRef = constRef;
            scope.info.typeRef = retrieveStrongType(scope, retVal);
            break;
         case ObjectKind::IntLiteral:
            scope.info.symbolType = SymbolType::Constant;
            scope.info.valueRef = constRef;
            scope.info.typeRef = retrieveStrongType(scope, retVal);
            break;
         case ObjectKind::Constant:
            scope.info.symbolType = SymbolType::Constant;
            scope.info.valueRef = retVal.reference;
            scope.info.typeRef = retrieveStrongType(scope, retVal);
            break;
         case ObjectKind::ConstArray:
            scope.info.symbolType = SymbolType::ConstantArray;
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
   EAttr mode = ExpressionAttribute::RootSymbol;
   if (scope.info.symbolType == SymbolType::Constant)
      mode = mode | ExpressionAttribute::ConstantExpr;

   ObjectInfo retVal = compileExpression(writer, exprScope,
      bodyNode.firstChild(), 0, mode, nullptr);

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

   if (_logic->isLessAccessible(*scope.moduleScope, scope.visibility, scope.info.typeRef))
      scope.raiseWarning(WARNING_LEVEL_1, wrnLessAccessible, node);

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

   if (scope.info.attributes.exist({ 0, ClassAttribute::RuntimeLoadable })) {
      SymbolInfo symbolInfo = {};
      symbolInfo.loadableInRuntime = true;

      // save class meta data
      MemoryWriter metaWriter(scope.module->mapSection(scope.reference | mskMetaSymbolInfoRef, false), 0);
      symbolInfo.save(&metaWriter);
   }
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
      writeMessageInfo(writer, scope);

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
         if (embeddableArray && localInfo.size > 0) {
            node.appendChild(BuildKey::BinaryArray, localInfo.offset)
               .appendChild(BuildKey::Size, localInfo.size);
         }
      }

      if (localInfo.size > 0) {
         if (embeddableArray) {
            if (_logic->isCompatible(*codeScope.moduleScope,
               { V_INT8 }, { localInfo.typeInfo.elementRef }, false))
            {
               BuildNode varNode = node.appendChild(BuildKey::ByteArrayAddress, it.key());
               varNode.appendChild(BuildKey::Index, localInfo.offset);
            }
            else if (_logic->isCompatible(*codeScope.moduleScope,
               { V_INT16 }, { localInfo.typeInfo.elementRef }, false))
            {
               BuildNode varNode = node.appendChild(BuildKey::ShortArrayAddress, it.key());
               varNode.appendChild(BuildKey::Index, localInfo.offset);
            }
            else if (_logic->isCompatible(*codeScope.moduleScope,
               { V_INT32 }, { localInfo.typeInfo.elementRef }, false))
            {
               BuildNode varNode = node.appendChild(BuildKey::IntArrayAddress, it.key());
               varNode.appendChild(BuildKey::Index, localInfo.offset);
            }
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.intReference) {
            BuildNode varNode = node.appendChild(BuildKey::IntVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.uintReference) {
            BuildNode varNode = node.appendChild(BuildKey::UIntVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.byteReference) {
            BuildNode varNode = node.appendChild(BuildKey::IntVariableAddress, it.key());
            varNode.appendChild(BuildKey::Index, localInfo.offset);
         }
         else if (localInfo.typeInfo.typeRef == codeScope.moduleScope->buildins.shortReference) {
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
         else if (_logic->isCompatible(*codeScope.moduleScope,
            { V_INT32 }, { localInfo.typeInfo.typeRef }, false))
         {
            BuildNode varNode = node.appendChild(BuildKey::IntVariableAddress, it.key());
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

   EAttr mode = closureMode ? EAttr::RetValExpected : EAttr::None;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Expression:
            exprRetVal = compileRootExpression(writer, codeScope, current, mode);
            break;
         case SyntaxKey::ReturnExpression:
            exprRetVal = retVal = compileRetExpression(writer, codeScope, current, EAttr::None);
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

   if (_trackingUnassigned) {
      // warn if the variable was not assigned
      for (auto it = codeScope.locals.start(); !it.eof(); ++it) {
         if ((*it).unassigned) {
            if((*it).size > 0) {
               warnOnUnassignedLocal(node, codeScope, -(*it).offset);
            }
            else warnOnUnassignedLocal(node, codeScope, (*it).offset);
         }
      }
   }

   // NOTE : in the closure mode the last statement is the closure result
   return closureMode ? exprRetVal : retVal;
}

void Compiler :: warnOnUnassignedLocal(SyntaxNode node, CodeScope& scope, int level)
{
   SyntaxNode current = scope.localNodes.get(level);

   if (current != SyntaxKey::None)
      scope.raiseWarning(WARNING_LEVEL_3, wrnUnassignedVariable, current);
}

inline void clearYieldContext()
{
   // clearing yield context
//   writer.appendNode(BuildKey::SavingStackDump);
}

ObjectInfo Compiler :: mapConstructorTarget(MethodScope& scope)
{
   ObjectInfo classSymbol = mapClassSymbol(scope, scope.getClassRef());

   if (!test(scope.message, FUNCTION_MESSAGE)) {
      return { ObjectKind::ConstructorSelf, classSymbol.typeInfo, scope.selfLocal, classSymbol.reference };
   }
   else return classSymbol;
}

void Compiler :: compileMethodCode(BuildTreeWriter& writer, ClassScope* classScope, MethodScope& scope, CodeScope& codeScope,
   SyntaxNode node, bool newFrame)
{
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

   if (scope.isYieldable()) {
      ExprScope exprScope(&codeScope);

      // reserve the place for the next step
      int offset = allocateLocalAddress(codeScope, sizeof(addr_t), false);

      ObjectInfo contextField = classScope->mapField(YIELD_CONTEXT_FIELD, EAttr::None);

      writeObjectInfo(writer, exprScope, contextField);
      writer.appendNode(BuildKey::LoadingStackDump);
      writer.appendNode(BuildKey::YieldDispatch, offset);
   }
   if (scope.isGeneric()) {
      scope.messageLocalAddress = allocateLocalAddress(codeScope, sizeof(mssg_t), false);
      writer.appendNode(BuildKey::SavingIndex, scope.messageLocalAddress);
   }

   ObjectInfo retVal = { };

   SyntaxNode bodyNode = node.firstChild(SyntaxKey::ScopeMask);
   switch (bodyNode.key) {
      case SyntaxKey::CodeBlock:
         retVal = compileCode(writer, codeScope, bodyNode, scope.closureMode);
         break;
      case SyntaxKey::ReturnExpression:
         if (scope.isYieldable()) {
            clearYieldContext();
         }
         retVal = compileRetExpression(writer, codeScope, bodyNode, EAttr::None);
         break;
      case SyntaxKey::ResendDispatch:
         retVal = compileResendCode(writer, codeScope,
            scope.constructorMode ? mapConstructorTarget(scope) : scope.mapSelf(),
            bodyNode);

         if (codeScope.isByRefHandler() && retVal.kind != ObjectKind::Unknown) {
            ExprScope exprScope(&codeScope);
            compileAssigningOp(writer, exprScope, codeScope.mapByRefReturnArg(), retVal);
         }
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
            convertObject(writer, exprScope, node, retVal, outputRef, false);

            exprScope.syncStack();
         }

         writeObjectInfo(writer, exprScope,
            boxArgument(writer, exprScope, retVal, scope.checkHint(MethodHint::Stacksafe), true, false));
      }
   }

   if (scope.isYieldable()) {
      clearYieldContext();
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

void Compiler :: compileYieldInitializing(BuildTreeWriter& writer, CodeScope& scope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   ObjectInfo contextField = classScope->mapField(YIELD_CONTEXT_FIELD, EAttr::None);

   ExprScope exprScope(&scope);

   pos_t contextSize = classScope->getMssgAttribute(node.arg.reference, ClassAttribute::YieldContextSize);

   writer.appendNode(BuildKey::NilReference);
   writer.appendNode(BuildKey::SavingInStack);

   writer.newNode(BuildKey::CreatingStruct, contextSize);
   writer.appendNode(BuildKey::Type, scope.moduleScope->buildins.superReference);
   writer.closeNode();

   writer.appendNode(BuildKey::SetImmediateField, 0);

   compileAssigningOp(writer, exprScope, contextField, { ObjectKind::Object });
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
         if (current.existChild(SyntaxKey::YieldContext)) {
            compileYieldInitializing(writer, codeScope, current.findChild(SyntaxKey::YieldContext));
         }
         else compileRootExpression(writer, codeScope, current, EAttr::None);
      }
      current = current.nextNode();
   }

   codeScope.syncStack(&scope);

   writer.appendNode(BuildKey::CloseFrame);

   endMethod(writer, scope);
}

void Compiler :: compileStaticInitializerMethod(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   BuildNode buildNode = writer.CurrentNode();
   while (buildNode != BuildKey::Root)
      buildNode = buildNode.parentNode();

   BuildTreeWriter nestedWriter(buildNode);

   nestedWriter.newNode(BuildKey::Symbol, node.arg.reference);

   nestedWriter.newNode(BuildKey::Tape);
   nestedWriter.appendNode(BuildKey::OpenFrame);

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::AssignOperation) {
         nestedWriter.appendNode(BuildKey::OpenStatement);
         addBreakpoint(nestedWriter, findObjectNode(current), BuildKey::Breakpoint);

         ExprScope exprScope(&scope);
         compileExpression(nestedWriter, exprScope,
            current, 0, EAttr::None, nullptr);

         nestedWriter.appendNode(BuildKey::EndStatement);

         exprScope.syncStack();
      }
      current = current.nextNode();
   }

   nestedWriter.appendNode(BuildKey::CloseFrame);
   nestedWriter.appendNode(BuildKey::Exit);

   nestedWriter.closeNode();
   nestedWriter.closeNode();
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

         writer.newNode(BuildKey::RedirectOp, node.arg.reference);
         writer.closeNode();
      }
      else if (node.arg.reference) {
         writer.appendNode(BuildKey::RedirectOp, node.arg.reference);
      }
      else {
         SyntaxNode targetNode = node.findChild(SyntaxKey::Target);
         assert(targetNode != SyntaxKey::None);

         writer.newNode(BuildKey::StrongRedirectOp, message);
         writer.appendNode(BuildKey::Type, targetNode.arg.reference);
         writer.closeNode();
      }
   }
}

ObjectInfo Compiler :: compileRedirect(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);
   ArgumentsInfo arguments;
   ArgumentsInfo updatedOuterArgs;

   ObjectInfo target = compileExpression(writer, scope, node.firstChild(), 0, EAttr::Parameter, &updatedOuterArgs);

   mssg_t messageRef = codeScope.getMessageID();

   if (!test(messageRef, FUNCTION_MESSAGE))
      arguments.add(target);

   MethodScope* methodScope = Scope::getScope<MethodScope>(codeScope, Scope::ScopeLevel::Method);

   for (auto it = methodScope->parameters.start(); !it.eof(); ++it) {
      arguments.add(methodScope->mapParameter(it.key(), EAttr::None));
   }

   ref_t signRef = getSignature(scope.module, messageRef);
   ObjectInfo retVal = compileMessageOperation(writer, scope, {}, target, messageRef,
      signRef, arguments, EAttr::AlreadyResolved, & updatedOuterArgs);

   scope.syncStack();

   return retVal;
}

ObjectInfo Compiler :: compileResendCode(BuildTreeWriter& writer, CodeScope& codeScope, ObjectInfo source, SyntaxNode node)
{
   ObjectInfo retVal = {};

   if (!node.arg.reference) {
      bool propertyMode = node.firstChild().key == SyntaxKey::PropertyOperation;

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
         switch (source.kind) {
            case ObjectKind::SelfLocal:
               source.kind = ObjectKind::SuperLocal;
               target = source;
               break;
            case ObjectKind::ConstructorSelf:
            case ObjectKind::Class:
            case ObjectKind::ClassSelf:
            {
               // NOTE : for the constructor redirect - use the class parent as a target (still keeping the original class
               // as a parameter)
               ClassInfo classInfo;
               if (_logic->defineClassInfo(*codeScope.moduleScope, classInfo, 
                  source.kind == ObjectKind::ConstructorSelf ? source.extra : source.reference, 
                  true)) 
               {
                  ObjectInfo temp = mapClassSymbol(codeScope, classInfo.header.parentRef);
                  if (source.kind == ObjectKind::ConstructorSelf) {
                     target.typeInfo = temp.typeInfo;
                     target.extra = temp.reference;
                  }
                  else target = temp;
               }
               else codeScope.raiseError(errInvalidOperation, node);
               break;
            }
            default:
               codeScope.raiseError(errInvalidOperation, node);
               break;
         }
      }

      ExprScope scope(&codeScope);
      ArgumentsInfo arguments;
      ArgumentsInfo updatedOuterArgs;

      mssg_t messageRef = mapMessage(scope, current, propertyMode, false, false);

      mssg_t resolvedMessage = _logic->resolveSingleDispatch(*scope.moduleScope,
         retrieveType(scope, source), messageRef);

      ref_t expectedSignRef = 0;
      if (resolvedMessage)
         scope.module->resolveAction(getAction(resolvedMessage), expectedSignRef);

      if (!test(messageRef, FUNCTION_MESSAGE))
         arguments.add(source);

      bool withVariadicArg = false;
      ref_t implicitSignatureRef = compileMessageArguments(writer, scope, current, arguments, expectedSignRef, 
         EAttr::NoPrimitives, &updatedOuterArgs, withVariadicArg);

      EAttr opMode = EAttr::CheckShortCircle;
      if (withVariadicArg) {
         messageRef |= VARIADIC_MESSAGE;

         opMode = opMode | EAttr::WithVariadicArg;
      }

      retVal = compileMessageOperation(writer, scope, node, target, messageRef,
         implicitSignatureRef, arguments, opMode, &updatedOuterArgs);

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
   if (!disptachTarget)
      assert(false);

   writer.newNode(BuildKey::StrongRedirectOp, dispatchMessage);
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
      writer.newNode(BuildKey::StrongRedirectOp, message);
      writer.appendNode(BuildKey::Type, targetNode.arg.reference);
      writer.closeNode();
   }
   else {
      writer.appendNode(BuildKey::AccSwapping, 1);

      // get feedback arg
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
   privateScope.nestedMode = invokerScope.nestedMode;
   privateScope.functionMode = invokerScope.functionMode;
   privateScope.isEmbeddable = invokerScope.isEmbeddable;

   classScope->info.methods.add(privateScope.message, privateScope.info);
   classScope->save();

   compileMethod(writer, privateScope, node);

   return privateScope.message;
}

void Compiler::compileByRefRedirectHandler(BuildTreeWriter& writer, MethodScope& invokerScope, SyntaxNode node, 
   mssg_t byRefHandler)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(invokerScope, Scope::ScopeLevel::Class);

   MethodScope redirectScope(classScope);
   // copy parameters
   for (auto it = invokerScope.parameters.start(); !it.eof(); ++it) {
      redirectScope.parameters.add(it.key(), *it);
   }

   // add byref return arg
   TypeInfo refType = { V_WRAPPER, invokerScope.info.outputRef };
   auto sizeInfo = _logic->defineStructSize(*invokerScope.moduleScope, resolvePrimitiveType(invokerScope, refType, false));

   int offset = invokerScope.parameters.count() + 1u;
   redirectScope.parameters.add(RETVAL_ARG, { offset, refType, sizeInfo.size });

   redirectScope.message = byRefHandler;

   // HOTFIX : mark it as stacksafe if required
   if (_logic->isEmbeddableStruct(classScope->info))
      redirectScope.info.hints |= (ref_t)MethodHint::Stacksafe;

   redirectScope.nestedMode = invokerScope.nestedMode;
   redirectScope.functionMode = invokerScope.functionMode;
   redirectScope.isEmbeddable = invokerScope.isEmbeddable;

   classScope->info.methods.add(redirectScope.message, redirectScope.info);
   classScope->save();

   compileMethod(writer, redirectScope, node);
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

   ObjectInfo target = methodScope.mapSelf();
   arguments.add(target);
   for (auto it = methodScope.parameters.start(); !it.eof(); ++it) {
      arguments.add(methodScope.mapParameter(it.key(), EAttr::None));
   }
   addByRefRetVal(arguments, tempRetVal);

   ref_t signRef = getSignature(scope.module, handler);
   /*ObjectInfo retVal = */compileMessageOperation(writer, scope, {}, target, handler,
      signRef, arguments, EAttr::AlreadyResolved, nullptr);

   // return temp variable
   writeObjectInfo(writer, scope,
      boxArgument(writer, scope, tempRetVal, false, true, false));

   scope.syncStack();

   writer.appendNode(BuildKey::CloseFrame);
}

void Compiler :: writeMessageInfo(BuildTreeWriter& writer, MethodScope& scope)
{
   IdentifierString methodName;
   ByteCodeUtil::resolveMessageName(methodName, scope.module, scope.message);

   writer.appendNode(BuildKey::MethodName, *methodName);
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
      else if (paramInfo.size < 0) {
         if (paramInfo.typeInfo.typeRef == V_INT16ARRAY) {
            writer.newNode(BuildKey::ShortArrayParameter, it.key());
         }
         else if (paramInfo.typeInfo.typeRef == V_INT8ARRAY) {
            writer.newNode(BuildKey::ByteArrayParameter, it.key());
         }
         else if (paramInfo.typeInfo.typeRef == V_INT32ARRAY) {
            writer.newNode(BuildKey::IntArrayParameter, it.key());
         }
         else writer.newNode(BuildKey::Parameter, it.key()); // !! temporal
      }
      else writer.newNode(BuildKey::Parameter, it.key());

      writer.appendNode(BuildKey::Index, prefix - paramInfo.offset);
      writer.closeNode();
   }

   writer.closeNode();
}

void Compiler :: compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);

   CodeScope codeScope(&scope);
   if (scope.info.byRefHandler && !scope.checkHint(MethodHint::InterfaceDispatcher)) {
      if (current.key == SyntaxKey::Redirect) {
         compileByRefRedirectHandler(writer, scope, node, scope.info.byRefHandler);
      }
      else {
         mssg_t privateImplementation = compileByRefHandler(writer, scope, node, scope.info.byRefHandler);

         beginMethod(writer, scope, node, BuildKey::Method, false);
         compileByRefHandlerInvoker(writer, scope, codeScope, privateImplementation, scope.info.outputRef);
         codeScope.syncStack(&scope);
         endMethod(writer, scope);

         // NOTE : normal byrefhandler has an alternative implementation
         // overriding the normal routine
         return;
      }
   }
   beginMethod(writer, scope, node, BuildKey::Method, true);

   switch (current.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::ReturnExpression:
      case SyntaxKey::ResendDispatch:
      case SyntaxKey::Redirect:
         compileMethodCode(writer, classScope, scope, codeScope, node, false);
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

   if (scope.isYieldable()) {
      classScope->addMssgAttribute(scope.message, ClassAttribute::YieldContextSize, scope.reserved2);
   }
}

bool Compiler :: isDefaultOrConversionConstructor(Scope& scope, mssg_t message, bool internalOne, bool& isProtectedDefConst)
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
   else if (internalOne) {
      ref_t dummy = 0;
      ustr_t actionName = scope.module->resolveAction(actionRef, dummy);

      return actionName.endsWith(CONSTRUCTOR_MESSAGE);
   }
   else return false;
}

// NOTE : check if init_method is declared in the current class then call it
//        returns the parent class reference
void Compiler :: callInitMethod(BuildTreeWriter& writer, SyntaxNode node, ExprScope& exprScope, ClassInfo& info, ref_t reference)
{
   if (!info.methods.exist(exprScope.moduleScope->buildins.init_message))
      return;

   if (info.header.parentRef != 0) {
      ClassInfo classInfo;
      _logic->defineClassInfo(*exprScope.moduleScope, classInfo, info.header.parentRef);

      callInitMethod(writer, node, exprScope, classInfo, info.header.parentRef);
   }

   MethodInfo initInfo = info.methods.get(exprScope.moduleScope->buildins.init_message);
   if (!initInfo.inherited) {
      ArgumentsInfo args;
      args.add({ ObjectKind::Object, { reference }, 0 });

      compileMessageOperation(writer, exprScope, node, args[0], exprScope.moduleScope->buildins.init_message,
         0, args, EAttr::AlreadyResolved, nullptr);
   }
}

void Compiler :: compileInlineInitializing(BuildTreeWriter& writer, ClassScope& classScope, SyntaxNode node)
{
   ExprScope exprScope(&classScope);

   callInitMethod(writer, node, exprScope, classScope.info, classScope.reference);
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
      compileInlineInitializing(writer, *classScope, node);
   }
}

void Compiler :: compileConstructor(BuildTreeWriter& writer, MethodScope& scope,
   ClassScope& classClassScope, SyntaxNode node, bool abstractMode)
{
   bool isProtectedDefConst = false;
   bool isDefConvConstructor = isDefaultOrConversionConstructor(scope, scope.message, scope.checkHint(MethodHint::Internal), isProtectedDefConst);

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

   // NOTE : special case - abstract class with a protected constructor
   bool protectedAbstractMode = scope.isProtected() && abstractMode;

   beginMethod(writer, scope, node, BuildKey::Method, true);

   CodeScope codeScope(&scope);
   ref_t classFlags = codeScope.getClassFlags();

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   bool retExpr = current == SyntaxKey::ReturnExpression;

   bool newFrame = false;
   if (current == SyntaxKey::ResendDispatch || current == SyntaxKey::RedirectDispatch) {
      // do not create a frame for resend operation
      // the object should not be created, because of redirecting
      isDefConvConstructor = false;
   }
   else if (isDefConvConstructor && !test(classFlags, elDynamicRole)) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;

      if (retExpr) {
         // the object should not be created for returning expression
         isDefConvConstructor = false;
      }

   }
   else if (retExpr) {
      // new stack frame
      writer.appendNode(BuildKey::OpenFrame);
      newFrame = true;
   }
   else if (!test(classFlags, elDynamicRole) 
      && (classClassScope.info.methods.exist(defConstrMssg) || protectedAbstractMode))
   {
      if (scope.checkHint(MethodHint::Multimethod)) {
         // NOTE : the dispatch statement must be before the default constructor call
         // to avoid the doublicate allocating
         compileMultidispatch(writer, codeScope, classClassScope, node, false);
      }

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
      compileConstructorDispatchCode(writer, codeScope, classClassScope, current);
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
            compileMethodCode(writer, &classClassScope, scope, codeScope, node, newFrame);
            break;
         case SyntaxKey::ReturnExpression:
            compileRetExpression(writer, codeScope, current, EAttr::DynamicObject);
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
   methodScope.targetSelfMode = methodScope.checkHint(MethodHint::TargetSelf);
   methodScope.nestedMode = scope.getScope(Scope::ScopeLevel::OwnerClass) != &scope;

   declareVMTMessage(methodScope, current, false, false);

   if (methodScope.info.outputRef) {
      SyntaxNode typeNode = current.findChild(SyntaxKey::Type, SyntaxKey::ArrayType, SyntaxKey::TemplateType);
      if (typeNode != SyntaxKey::None) {
         resolveStrongTypeAttribute(scope, typeNode, false, false);

         //TypeAttributes typeAttributes = {};
         //resolveTypeAttribute(scope, typeNode, typeAttributes, false, false);
         //if (typeAttributes.isNonempty())
         //   scope.raiseError(errInvalidOperation, typeNode);
      }
      else validateType(scope, methodScope.info.outputRef, current, false, false);

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

void Compiler :: compileRedirectDispatcher(BuildTreeWriter& writer, MethodScope& scope, CodeScope& codeScope, SyntaxNode node,
   bool withGenerics)
{
   writer.appendNode(BuildKey::DispatchingOp);
   if (withGenerics) {
      writer.newNode(BuildKey::GenericDispatchingOp);
      writer.appendNode(BuildKey::Message,
         encodeMessage(scope.module->mapAction(GENERIC_PREFIX, 0, false), 0, 0));
      writer.closeNode();
   }

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
         retVal = compileExpression(writer, exprScope, bodyNode, 0, EAttr::None, nullptr);
         break;
      default:
         scope.raiseError(errInvalidOperation, node);
         break;
   }

   retVal = boxArgument(writer, exprScope, retVal, false, true, false);

   writeObjectInfo(writer, exprScope, retVal);

   writer.appendNode(BuildKey::LoadingIndex, mssgVar.reference);

   exprScope.syncStack();

   writer.appendNode(BuildKey::CloseFrame, -1);
   writer.appendNode(BuildKey::RedirectOp);
}

inline bool hasVariadicFunctionDispatcher(Compiler::ClassScope* classScope, bool& mixedDispatcher)
{
   bool normalVariadic = false;
   bool functionVariadic = false;
   for (auto it = classScope->info.methods.start(); !it.eof(); ++it) {
      mssg_t m = it.key();

      if ((m & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
         if (test(m, FUNCTION_MESSAGE)) {
            functionVariadic = true;
         }
         else normalVariadic = true;
      }
   }

   if (functionVariadic) {
      if (normalVariadic)
         mixedDispatcher = true;

      return true;
   }
   else return false;
}

void Compiler :: compileDispatcherMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node,
   bool withGenerics, bool withOpenArgGenerics)
{
   CodeScope codeScope(&scope);

   beginMethod(writer, scope, node, BuildKey::Method, false);

   if (node != SyntaxKey::None) {
      // if it is an explicit dispatcher
      SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
      switch (current.key) {
         case SyntaxKey::Importing:
            writer.appendNode(BuildKey::Import, current.arg.reference);
            break;
         case SyntaxKey::Redirect:
            compileRedirectDispatcher(writer, scope, codeScope, current, withGenerics);
            break;
         default:
            scope.raiseError(errInvalidOperation, node);
            break;
      }
   }
   else {
      // if it is an implicit dispatcher
      if (withGenerics) {
         ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

         // !! temporally
         if (withOpenArgGenerics)
            scope.raiseError(errInvalidOperation, node);

         writer.appendNode(BuildKey::DispatchingOp);
         writer.newNode(BuildKey::GenericDispatchingOp);
         writer.appendNode(BuildKey::Message,
            encodeMessage(scope.module->mapAction(GENERIC_PREFIX, 0, false), 0, 0));
         writer.closeNode();

         writer.newNode(BuildKey::StrongRedirectOp, scope.moduleScope->buildins.dispatch_message);
         writer.appendNode(BuildKey::Type, classScope->info.header.parentRef);
         writer.closeNode();
      }
      // if it is open arg generic without redirect statement
      else if (withOpenArgGenerics) {
         ExprScope exprScope(&codeScope);

         ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

         ref_t mask = VARIADIC_MESSAGE;
         bool mixedDispatcher = false;
         bool variadicFunction = hasVariadicFunctionDispatcher(classScope, mixedDispatcher);
         if (variadicFunction) {

            mask |= FUNCTION_MESSAGE;
         }

         // HOTFIX : an extension is a special case of a variadic function and a target should be included
         pos_t argCount = (!scope.isExtension && variadicFunction) ? 1 : 2;

         writer.appendNode(BuildKey::DispatchingOp);
         // open frame
         writer.appendNode(BuildKey::OpenFrame);
         // save the target
         ObjectInfo tempTarget = saveToTempLocal(writer, exprScope, { ObjectKind::Object });
         // save incoming message
         scope.messageLocalAddress =  allocateLocalAddress(codeScope, sizeof(mssg_t), false);
         writer.appendNode(BuildKey::SavingIndex, scope.messageLocalAddress);
         // unbox argument list
         writer.appendNode(BuildKey::LoadArgCount, 1);
         writer.appendNode(BuildKey::UnboxMessage, -1);

         // change incoming message to variadic multi-method
         writer.newNode(BuildKey::LoadingSubject,
            encodeMessage(getAction(scope.moduleScope->buildins.dispatch_message), argCount, mask));
         writer.appendNode(BuildKey::Index, scope.messageLocalAddress);
         if (mixedDispatcher)
            writer.appendNode(BuildKey::Special, -1);
         writer.closeNode();

         // select the target
         writeObjectInfo(writer, exprScope, tempTarget);

         // call the message
         writer.newNode(BuildKey::StrongResendOp, scope.moduleScope->buildins.dispatch_message);
         writer.appendNode(BuildKey::Type, classScope->info.header.parentRef);
         writer.closeNode();

         // close frame
         writer.appendNode(BuildKey::CloseFrame);

         exprScope.syncStack();
      }
   }

   codeScope.syncStack(&scope);
   endMethod(writer, scope);
}

void Compiler :: compileCustomDispatcher(BuildTreeWriter& writer, ClassScope& scope)
{
   MethodScope methodScope(&scope);
   methodScope.message = scope.moduleScope->buildins.dispatch_message;

   auto methodIt = scope.info.methods.getIt(methodScope.message);
   if (!methodIt.eof()) {
      methodScope.info = *methodIt;

      methodScope.info.inherited = false;

      *methodIt = methodScope.info;
   }
   else {
      methodScope.info.hints |= (ref_t)MethodHint::Dispatcher;
      scope.info.methods.add(methodScope.message, methodScope.info);
   }

   scope.info.header.flags |= elWithCustomDispatcher;

   compileDispatcherMethod(writer, methodScope, {},
      test(scope.info.header.flags, elWithGenerics),
      test(scope.info.header.flags, elWithVariadics));

   // overwrite the class info if required
   scope.save();
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
               compileDispatcherMethod(writer, methodScope, current,
                  test(scope.info.header.flags, elWithGenerics),
                  test(scope.info.header.flags, elWithVariadics));
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
         case SyntaxKey::StaticInitializerMethod:
            compileStaticInitializerMethod(writer, scope, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   // if the VMT conatains newly defined generic / variadic handlers, overrides default one
   if (testany(scope.info.header.flags, elWithGenerics | elWithVariadics)
      && scope.info.methods.get(scope.moduleScope->buildins.dispatch_message).inherited)
   {
      compileCustomDispatcher(writer, scope);
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

            compileConstructor(writer, methodScope, classClassScope, current, scope.isAbstract());
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }

   // if the VMT conatains newly defined generic / variadic handlers, overrides default one
   if (testany(classClassScope.info.header.flags, elWithGenerics | elWithVariadics)
      && classClassScope.info.methods.get(scope.moduleScope->buildins.dispatch_message).inherited)
   {
      compileCustomDispatcher(writer, classClassScope);
   }
}

void Compiler :: compileExpressionMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   beginMethod(writer, scope, node, BuildKey::Method, false);

   CodeScope codeScope(&scope);

   // new stack frame
   writer.appendNode(BuildKey::OpenFrame);

   // stack should contains current self reference
   // the original message should be restored if it is a generic method
   scope.selfLocal = codeScope.newLocal();
   writer.appendNode(BuildKey::Assigning, scope.selfLocal);

   compileRetExpression(writer, codeScope, node, EAttr::None);

   writer.appendNode(BuildKey::CloseFrame);

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileClosureMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   ClassScope* classScope = Scope::getScope<ClassScope>(scope, Scope::ScopeLevel::Class);

   beginMethod(writer, scope, node, BuildKey::Method, false);

   CodeScope codeScope(&scope);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   switch (current.key) {
      case SyntaxKey::CodeBlock:
      case SyntaxKey::ReturnExpression:
         compileMethodCode(writer, classScope, scope, codeScope, node, false);
         break;
      default:
         break;
   }

   codeScope.syncStack(&scope);

   endMethod(writer, scope);
}

void Compiler :: compileClosureClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   bool lazyExpression = node == SyntaxKey::LazyOperation;
   ref_t parentRef = scope.info.header.parentRef;

   writer.newNode(BuildKey::Class, scope.reference);

   MethodScope methodScope(&scope);
   declareClosureMessage(methodScope, node);

   methodScope.functionMode = true;

   mssg_t multiMethod = /*!lazyExpression && */defineMultimethod(scope, methodScope.message, false);
   if (multiMethod) {
      methodScope.info.multiMethod = multiMethod;
      methodScope.info.outputRef = V_AUTO;
   }

   if (lazyExpression) {
      compileExpressionMethod(writer, methodScope, node);
   }
   else {
      compileClosureMethod(writer, methodScope, node);

      // HOTFIX : inject an output type if required or used super class
      if (methodScope.info.outputRef == V_AUTO) {
         methodScope.info.outputRef = scope.moduleScope->buildins.superReference;
      }
   }

   if (!lazyExpression) {
      ref_t closureRef = resolveClosure(scope, methodScope.message, methodScope.info.outputRef);
      if (closureRef) {
         parentRef = closureRef;
      }
      else throw InternalError(errClosureError);
   }
   else parentRef = scope.moduleScope->buildins.lazyExpressionReference;

   declareClassParent(parentRef, scope, node);
   generateClassFlags(scope, elNestedClass | elSealed);

   // handle the abstract flag
   if (test(scope.info.header.flags, elAbstract)) {
      scope.abstractBasedMode = true;
      scope.info.header.flags &= ~elAbstract;
   }

   auto m_it = scope.info.methods.getIt(methodScope.message);
   if (!m_it.eof()) {
      (*m_it).inherited = true;
      (*m_it).hints &= ~(ref_t)MethodHint::Abstract;
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
         generateMethodDeclaration(scope, current, false, false);

         current = current.nextNode();
      }

      _logic->injectOverloadList(this, *scope.moduleScope, scope.info, scope.reference);

      compileVMT(writer, scope, classNode);
   }

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info, scope.isClassClass());

   writer.closeNode();

   scope.save();
}

bool isEmbeddableDispatcher(ModuleScopeBase* moduleScope, SyntaxNode current)
{
   SyntaxNode attr = current.firstChild();
   bool embeddable = false;
   bool implicit = true;
   while (attr != SyntaxKey::None) {
      if (attr == SyntaxKey::Attribute) {
         switch (attr.arg.reference) {
            case V_EMBEDDABLE:
               embeddable = true;
               break;
            case V_METHOD:
            case V_CONSTRUCTOR:
            case V_DISPATCHER:
               implicit = false;
               break;
         }
      }
      else if (attr == SyntaxKey::Name && embeddable && implicit) {
         if (moduleScope->attributes.get(attr.firstChild(SyntaxKey::TerminalMask).identifier()) == V_DISPATCHER) {
            return true;
         }
         else break;
      }

      attr = attr.nextNode();
   }

   return false;
}

void Compiler :: injectInterfaceDispatch(Scope& scope, SyntaxNode node, ref_t parentRef)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Method && current.existChild(SyntaxKey::Redirect)) {
         if (isEmbeddableDispatcher(scope.moduleScope, current)) {
            SyntaxNode exprNode = current.findChild(SyntaxKey::Redirect).findChild(SyntaxKey::Expression);
            SyntaxNode objNode = exprNode.firstChild();
            if (objNode.nextNode() != SyntaxKey::None)
               scope.raiseError(errInvalidSyntax, node);
            SyntaxNode terminalNode = objNode.firstChild();

            IdentifierString arg(terminalNode.identifier());

            VirtualMethods virtualMethods;
            _logic->generateVirtualDispatchMethod(*scope.moduleScope, parentRef, virtualMethods);

            for (pos_t i = 0; i < virtualMethods.count_pos(); i++) {
               auto methInfo = virtualMethods.get(i);

               injectVirtualDispatchMethod(scope, node, methInfo.value1, methInfo.value2, terminalNode.key, *arg);
            }

            // interface class should no have a custom dispatcher
            current.setKey(SyntaxKey::Idle);

            return;
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: compileNestedClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node, ref_t parentRef)
{
   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);
   scope.info.header.flags |= elNestedClass;

   bool virtualClass = true;
   // NOTE : check if it is in-place initialization
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Field:
         case SyntaxKey::Method:
            virtualClass = false;
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   if (virtualClass)
      scope.info.header.flags |= elVirtualVMT;

   declareClassParent(parentRef, scope, node);

   ref_t dummy = 0;
   declareClassAttributes(scope, {}, dummy);

   if (scope.abstractBasedMode && test(scope.info.header.flags, elClosed | elNoCustomDispatcher))
   {
      // COMPILER MAGIC : inject interface implementation if dispatch method available
      injectInterfaceDispatch(scope, node, scope.info.header.parentRef);
   }

   bool withConstructors = false;
   bool withDefaultConstructor = false;
   bool yieldMethodNotAllowed = true;
   declareVMT(scope, node, withConstructors, withDefaultConstructor, yieldMethodNotAllowed, true);
   if (withConstructors)
      scope.raiseError(errIllegalConstructor, node);

   generateClassDeclaration(scope, node, elNestedClass | elSealed);

   scope.save();

   BuildNode buildNode = writer.CurrentNode();
   while (buildNode != BuildKey::Root)
      buildNode = buildNode.parentNode();

   BuildTreeWriter nestedWriter(buildNode);

   nestedWriter.newNode(BuildKey::Class, scope.reference);

   nestedWriter.appendNode(BuildKey::Path, *ns->sourcePath);

   compileVMT(nestedWriter, scope, node, true, true);

   // set flags once again
   // NOTE : it should be called after the code compilation to take into consideration outer fields
   _logic->tweakClassFlags(*scope.moduleScope, scope.reference, scope.info, scope.isClassClass());

   // NOTE : compile once again only auto generated methods
   compileVMT(nestedWriter, scope, node, true, false);

   nestedWriter.closeNode();

   scope.save();
}

void Compiler :: validateClassFields(ClassScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Field) {
         FieldAttributes attrs = {};
         readFieldAttributes(scope, current, attrs, false);

         if (attrs.isConstant) {
            ustr_t name = current.findChild(SyntaxKey::Name).firstChild(SyntaxKey::TerminalMask).identifier();

            auto fieldInfo = scope.info.statics.get(name);
            if (fieldInfo.valueRef == INVALID_REF)
               scope.raiseError(errNoInitializer, current.findChild(SyntaxKey::Name));
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
#ifdef FULL_OUTOUT_INFO
   // info
   ustr_t name = scope.module->resolveReference(scope.reference);
   _errorProcessor->info(infoCurrentClass, name);
#endif // FULL_OUTOUT_INFO

   NamespaceScope* ns = Scope::getScope<NamespaceScope>(scope, Scope::ScopeLevel::Namespace);

   // validate field types
   if (scope.info.fields.count() > 0 || scope.info.statics.count() > 0) {
      validateClassFields(scope, node);
   }

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
#ifdef FULL_OUTOUT_INFO
   // info
   ustr_t name = scope.module->resolveReference(scope.reference);
   _errorProcessor->info(infoCurrentClass, name);
#endif // FULL_OUTOUT_INFO

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
            copyParentNamespaceExtensions(ns, namespaceScope);

            compileNamespace(writer, namespaceScope, current);
            break;
         }
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, current.arg.reference, ns.defaultVisibility);
            symbolScope.isStatic = SyntaxTree::ifChildExists(current, SyntaxKey::Attribute, V_STATIC);
            symbolScope.visibility = ns.moduleScope->retrieveVisibility(symbolScope.reference);

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
               ClassClassScope classClassScope(&ns, classScope.info.header.classRef, classScope.visibility, &classScope.info);
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

inline void addPackageItem(SyntaxTreeWriter& writer, ModuleBase* module, ustr_t str)
{
   writer.newNode(SyntaxKey::Expression);
   writer.newNode(SyntaxKey::Object);
   if (emptystr(str)) {
      writer.appendNode(SyntaxKey::string, "");
   }
   else writer.appendNode(SyntaxKey::string, str);
   writer.closeNode();
   writer.closeNode();
}

void Compiler :: createPackageInfo(ModuleScopeBase* moduleScope, ManifestInfo& manifestInfo)
{
   ReferenceName sectionName("", PACKAGE_SECTION);
   ref_t packageRef = moduleScope->module->mapReference(*sectionName);

   SyntaxTree tempTree;
   SyntaxTreeWriter tempWriter(tempTree);

   tempWriter.newNode(SyntaxKey::PrimitiveCollection, packageRef);

   // namespace
   addPackageItem(tempWriter, moduleScope->module, moduleScope->module->name());

   // package name
   addPackageItem(tempWriter, moduleScope->module, manifestInfo.maninfestName);

   // package version
   addPackageItem(tempWriter, moduleScope->module, manifestInfo.maninfestVersion);

   // package author
   addPackageItem(tempWriter, moduleScope->module, manifestInfo.maninfestAuthor);

   tempWriter.closeNode();

   Interpreter interpreter(moduleScope, _logic);
   MetaScope scope(nullptr, Scope::ScopeLevel::Namespace);
   scope.module = moduleScope->module;
   scope.moduleScope = moduleScope;
   evalCollection(interpreter, scope, tempTree.readRoot(), true);
}

void Compiler :: prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver,
   ManifestInfo& manifestInfo)
{
   _trackingUnassigned = (_errorProcessor->getWarningLevel() == WarningLevel::Level3);

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
   moduleScope->buildins.messageNameReference = safeMapReference(moduleScope, forwardResolver, MESSAGE_NAME_FORWARD);
   moduleScope->buildins.extMessageReference = safeMapReference(moduleScope, forwardResolver, EXT_MESSAGE_FORWARD);
   moduleScope->buildins.wrapperTemplateReference = safeMapReference(moduleScope, forwardResolver, WRAPPER_FORWARD);
   moduleScope->buildins.arrayTemplateReference = safeMapReference(moduleScope, forwardResolver, ARRAY_FORWARD);
   moduleScope->buildins.argArrayTemplateReference = safeMapReference(moduleScope, forwardResolver, VARIADIC_ARRAY_FORWARD);

   moduleScope->buildins.closureTemplateReference = safeMapWeakReference(moduleScope, forwardResolver, CLOSURE_FORWARD);
   moduleScope->buildins.tupleTemplateReference = safeMapWeakReference(moduleScope, forwardResolver, TUPLE_FORWARD);
   moduleScope->buildins.lazyExpressionReference = safeMapWeakReference(moduleScope, forwardResolver, LAZY_FORWARD);
   moduleScope->buildins.uintReference = safeMapReference(moduleScope, forwardResolver, UINT_FORWARD);
   moduleScope->buildins.pointerReference = safeMapReference(moduleScope, forwardResolver, PTR_FORWARD);

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
         1, STATIC_MESSAGE);
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
   moduleScope->buildins.and_message =
      encodeMessage(moduleScope->module->mapAction(AND_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.or_message =
      encodeMessage(moduleScope->module->mapAction(OR_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.xor_message =
      encodeMessage(moduleScope->module->mapAction(XOR_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.refer_message =
      encodeMessage(moduleScope->module->mapAction(REFER_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.set_refer_message =
      encodeMessage(moduleScope->module->mapAction(SET_REFER_MESSAGE, 0, false),
         3, 0);
   moduleScope->buildins.if_message =
      encodeMessage(moduleScope->module->mapAction(IF_MESSAGE, 0, false),
         2, 0);
   moduleScope->buildins.iif_message =
      encodeMessage(moduleScope->module->mapAction(IIF_MESSAGE, 0, false),
         3, 0);
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
   moduleScope->receivedVar.copy(moduleScope->predefined.retrieve<ref_t>("@received", V_RECEIVED_VAR,
      [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      }));

   createPackageInfo(moduleScope, manifestInfo);

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

void Compiler :: declareModuleIdentifiers(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic, outerExtensionList);

         // declare namespace
         declareNamespace(ns, current, true, true);
         ns.moduleScope->newNamespace(*ns.nsName);

         // declare all module members - map symbol identifiers
         declareMemberIdentifiers(ns, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declareModule(ModuleScopeBase* moduleScope, SyntaxNode node, ExtensionMap* outerExtensionList)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic, outerExtensionList);

         // declare namespace
         declareNamespace(ns, current, false, true);

         // declare all module members - map symbol identifiers
         declareMembers(ns, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: declare(ModuleScopeBase* moduleScope, SyntaxTree& input, ExtensionMap* outerExtensionList)
{
   validateScope(moduleScope);

   SyntaxNode root = input.readRoot();
   // declare all member identifiers
   declareModuleIdentifiers(moduleScope, root, outerExtensionList);

   // declare all members
   declareModule(moduleScope, root, outerExtensionList);
}

void Compiler :: compile(ModuleScopeBase* moduleScope, SyntaxTree& input, BuildTree& output, ExtensionMap* outerExtensionList)
{
   BuildTreeWriter writer(output);
   writer.newNode(BuildKey::Root);

   SyntaxNode node = input.readRoot();
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic, outerExtensionList);
         declareNamespace(ns, current);

         compileNamespace(writer, ns, current);

         _presenter->showProgress();
      }

      current = current.nextNode();
   }

   writer.closeNode();
}

inline SyntaxNode newVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, Visibility visibility)
{
   ref_t hints = (ref_t)MethodHint::Multimethod | (methodType == SyntaxKey::StaticMethod ? (ref_t)MethodHint::Sealed : (ref_t)MethodHint::Normal);

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

inline SyntaxNode newVirtualMethod(SyntaxNode classNode, SyntaxKey methodType, mssg_t message, Visibility visibility, bool abstractOne)
{
   ref_t hints = methodType == SyntaxKey::StaticMethod ? (ref_t)MethodHint::Sealed : (ref_t)MethodHint::Normal;
   if (abstractOne)
      hints |= (ref_t)MethodHint::Abstract;

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

void Compiler :: injectVirtualEmbeddableWrapper(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ref_t targetRef, ClassInfo& info, mssg_t message, bool abstractOne)
{
   MethodInfo methodInfo = {};

   auto m_it = info.methods.getIt(message);
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

      SyntaxNode methodNode = newVirtualMethod(classNode, methodType, message, visibility, abstractOne);
      methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated method

      if (!abstractOne) {
         mssg_t resendMessage = message | STATIC_MESSAGE;

         SyntaxNode resendOp = methodNode.appendChild(SyntaxKey::DirectResend, resendMessage);
         resendOp.appendChild(SyntaxKey::Target, targetRef);
      }
      else methodNode.appendChild(SyntaxKey::WithoutBody);
   }
}

inline bool isSingleDispatch(SyntaxNode node, SyntaxKey methodType, mssg_t message, mssg_t& targetMessage)
{
   // !! currently constructor is not supporting single dispatch operation
   if (methodType == SyntaxKey::Constructor)
      return false;

   mssg_t foundMessage = 0;

   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodType) {
         mssg_t multiMethod = current.findChild(SyntaxKey::Multimethod).arg.reference;
         if (multiMethod == message) {
            if (foundMessage) {
               return false;
            }
            else foundMessage = current.arg.reference;
         }
      }

      current = current.nextNode();
   }

   if (foundMessage) {
      targetMessage = foundMessage;

      return true;
   }
   else return false;
}

bool Compiler :: injectVirtualStrongTypedMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope, 
   mssg_t message, mssg_t resendMessage, ref_t outputRef, Visibility visibility )
{
   ref_t actionRef = getAction(resendMessage);
   ref_t signRef = 0;
   ustr_t actionName = scope.module->resolveAction(actionRef, signRef);

   ref_t signArgs[ARG_COUNT];
   size_t signLen = scope.module->resolveSignature(signRef, signArgs);
   // HOTFIX : make sure it has at least one strong-typed argument
   bool strongOne = false;
   for (size_t i = 0; i < signLen; i++) {
      if (signArgs[i] != scope.buildins.superReference) {
         strongOne = true;
         break;
      }
   }

   // HOTFIX : the weak argument list should not be type-casted
   // to avoid dispatching to the same method
   if (!strongOne)
      return false;

   SyntaxNode methodNode = newVirtualMultimethod(classNode, methodType, message, visibility);
   methodNode.appendChild(SyntaxKey::Autogenerated, -1); // -1 indicates autogenerated multi-method

   if (outputRef)
      methodNode.appendChild(SyntaxKey::OutputType, outputRef);

   SyntaxNode resendExpr = methodNode.appendChild(SyntaxKey::ResendDispatch);
   SyntaxNode operationNode = resendExpr.appendChild(SyntaxKey::MessageOperation);
   operationNode.appendChild(SyntaxKey::Message).appendChild(SyntaxKey::identifier, actionName);

   String<char, 10> arg;
   for (size_t i = 0; i < signLen; i++) {
      arg.copy("$");
      arg.appendInt((int)i);

      SyntaxNode param = methodNode.appendChild(SyntaxKey::Parameter);
      SyntaxNode nameParam = param.appendChild(SyntaxKey::Name);
      nameParam.appendChild(SyntaxKey::identifier, arg.str());

      if (signArgs[i] != scope.buildins.superReference) {
         SyntaxNode castNode = operationNode.appendChild(SyntaxKey::Expression).appendChild(SyntaxKey::MessageOperation);
         SyntaxNode castObject = castNode.appendChild(SyntaxKey::Object);
         castObject.appendChild(SyntaxKey::Attribute, V_CONVERSION);
         castObject.appendChild(SyntaxKey::Type, signArgs[i]);
         castNode.appendChild(SyntaxKey::Message);
         castNode.appendChild(SyntaxKey::Expression).appendChild(SyntaxKey::Object).appendChild(SyntaxKey::identifier, arg.str());
      }
      else operationNode.appendChild(SyntaxKey::Expression).appendChild(SyntaxKey::Object).appendChild(SyntaxKey::identifier, arg.str());
   }

   return true;
}

void Compiler :: injectVirtualMultimethod(SyntaxNode classNode, SyntaxKey methodType, ModuleScopeBase& scope,
   ClassInfo& info, mssg_t message, bool inherited, ref_t outputRef, Visibility visibility)
{
   mssg_t resendMessage = message;
   ref_t  resendTarget = 0;

   ref_t actionRef, flags;
   pos_t argCount;
   decodeMessage(message, actionRef, argCount, flags);

   info.attributes.exclude({ message, ClassAttribute::SingleDispatch });

   // try to resolve an argument list in run-time if it is only a single dispatch and argument list is not weak
   // !! temporally do not support variadic arguments
   if (isSingleDispatch(classNode, methodType, message, resendMessage) && ((message & PREFIX_MESSAGE_MASK) != VARIADIC_MESSAGE) &&
      injectVirtualStrongTypedMultimethod(classNode, methodType, scope, message, resendMessage, outputRef, visibility))
   {
      // mark the message as a signle dispatcher if the class is sealed / closed / class class
      // and default multi-method was not explicitly declared
      if (testany(info.header.flags, elClosed | elClassClass) && !inherited)
         info.attributes.add({ message, ClassAttribute::SingleDispatch }, resendMessage);
   }
   else {
      if (inherited) {
         // if virtual multi-method handler is overridden
         // redirect to the parent one
         resendTarget = info.header.parentRef;
      }
      else {
         ref_t dummy = 0;
         ustr_t actionName = scope.module->resolveAction(actionRef, dummy);

         ref_t signatureLen = 0;
         ref_t signatures[ARG_COUNT] = {};

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
   SyntaxNode methodNode = newVirtualMethod(classNode, methodType, message, Visibility::Public, false);
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

void Compiler :: injectInheritedStaticMethod(SyntaxNode classNode, SyntaxKey methodType, ref_t reference, mssg_t message, ref_t outputRef)
{
   SyntaxNode methodNode = classNode.appendChild(methodType, message);
   methodNode.appendChild(SyntaxKey::OutputType, outputRef);

   SyntaxNode resendNode = methodNode.appendChild(SyntaxKey::DirectResend, message);
   resendNode.appendChild(SyntaxKey::Target, reference);
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
      if (type == MethodHint::Normal) {
         metaWriter.insertDWord(0, 0);
      }
      else metaWriter.insertDWord(0, messageRef);

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

void Compiler :: injectVirtualDispatchMethod(Scope& scope, SyntaxNode classNode, mssg_t message, ref_t outputRef, SyntaxKey key, ustr_t arg)
{
   SyntaxNode methodNode = classNode.appendChild(SyntaxKey::Method, message);
   // HOTFIX : indicating virtual interface dispatcher, to ignore byref handler optimization
   methodNode.appendChild(SyntaxKey::Attribute, V_INTERFACE_DISPATCHER);

   if (outputRef)
      methodNode.appendChild(SyntaxKey::Type, outputRef);

   ref_t actionRef = getAction(message);
   ref_t signRef = 0;
   ustr_t actionName = scope.module->resolveAction(actionRef, signRef);

   if (signRef) {
      ref_t signatures[ARG_COUNT];
      size_t len = scope.module->resolveSignature(signRef, signatures);

      String<char, 10> arg;
      for (size_t i = 0; i < len; i++) {
         arg.copy("$");
         arg.appendInt((int)i);

         SyntaxNode param = methodNode.appendChild(SyntaxKey::Parameter);
         param.appendChild(SyntaxKey::Type, signatures[i]);
         SyntaxNode nameParam = param.appendChild(SyntaxKey::Name);
         nameParam.appendChild(SyntaxKey::identifier, arg.str());
      }
   }
   else {
      pos_t len = getArgCount(message);
      String<char, 10> arg;
      for (pos_t i = 1; i < len; i++) {
         arg.copy("$");
         arg.appendInt(i);

         SyntaxNode param = methodNode.appendChild(SyntaxKey::Parameter);
         SyntaxNode nameParam = param.appendChild(SyntaxKey::Name);
         nameParam.appendChild(SyntaxKey::identifier, arg.str());
      }
   }

   SyntaxNode body = methodNode.appendChild(SyntaxKey::Redirect);
   body
      .appendChild(SyntaxKey::Expression)
      .appendChild(SyntaxKey::Object)
      .appendChild(key, arg);
}

ref_t Compiler :: generateExtensionTemplate(ModuleScopeBase& scope, ref_t templateRef, size_t argumentLen, ref_t* arguments, 
   ustr_t ns, ExtensionMap* outerExtensionList)
{
   TemplateTypeList typeList;
   for (size_t i = 0; i < argumentLen; i++)
      typeList.add(arguments[i]);

   // HOTFIX : generate a temporal template to pass the type
   SyntaxTree dummyTree;
   List<SyntaxNode> parameters({});
   declareTemplateParameters(scope.module, typeList, dummyTree, parameters);

   return _templateProcessor->generateClassTemplate(scope, ns,
      templateRef, parameters, false, outerExtensionList);
}

inline int retrieveIndex(List<mssg_t>& list, mssg_t multiMethod)
{
   return list.retrieveIndex<mssg_t>(multiMethod, [](mssg_t arg, ref_t current)
      {
         return current == arg;
      });
}

inline void saveAsPreloaded(CompilerLogic* logic, ustr_t ns, ModuleBase* module, ref_t ref, ref_t mask)
{
   IdentifierString preloadedListName;
   if (!ns.empty()) {
      preloadedListName.append(ns);
   }
   else preloadedListName.append("'");

   preloadedListName.append(META_PREFIX);
   preloadedListName.append(PRELOADED_FORWARD);

   ref_t dictionaryRef = module->mapReference(*preloadedListName);

   MemoryBase* dictionary = module->mapSection(dictionaryRef | mskTypeListRef, false);

   logic->writeArrayEntry(dictionary, ref | mask);
}

void Compiler :: declareModuleExtensionDispatcher(NamespaceScope& scope, SyntaxNode node)
{
   List<mssg_t>         genericMethods(0);
   ClassInfo::MethodMap methods({});
   ResolvedMap          targets(0);

   auto it = scope.declaredExtensions.start();
   while (!it.eof()) {
      auto extInfo = *it;
      mssg_t genericMessage = it.key();

      ustr_t refName = scope.module->resolveReference(extInfo.value1);
      if (isWeakReference(refName)) {
         if (NamespaceString::compareNs(refName, *scope.nsName)) {
            // if the extension is declared in the module namespace
            // add it to the list to be generated

            if (retrieveIndex(genericMethods, genericMessage) == -1)
               genericMethods.add(genericMessage);

            methods.add(extInfo.value2, { false, 0, 0, genericMessage | FUNCTION_MESSAGE, 0 });
            targets.add(extInfo.value2, extInfo.value1);
         }
      }

      it++;
   }

   if (genericMethods.count() > 0) {
      // if there are extension methods in the namespace
      ref_t extRef = scope.moduleScope->mapAnonymous();
      ClassScope classScope(&scope, extRef, Visibility::Private);
      declareClassParent(classScope.info.header.parentRef, classScope, {});
      classScope.extensionDispatcher = true;
      classScope.info.header.classRef = classScope.reference;
      classScope.extensionClassRef = scope.moduleScope->buildins.superReference;
      generateClassFlags(classScope, elExtension | elSealed | elAutoLoaded);

      for (auto g_it = genericMethods.start(); !g_it.eof(); ++g_it) {
         mssg_t genericMessage = *g_it;

         _logic->injectMethodOverloadList(this, *scope.moduleScope,
            classScope.info.header.flags, genericMessage | FUNCTION_MESSAGE, methods,
            classScope.info.attributes, &targets, targetResolver, ClassAttribute::ExtOverloadList);
      }

      classScope.save();

      // build the class tree
      node.appendChild(SyntaxKey::Class, extRef);

      // save as preloaded class
      saveAsPreloaded(_logic, *scope.nsName, scope.module, extRef, mskVMTRef);
   }
}
