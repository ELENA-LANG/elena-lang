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

void Interpreter :: addArrayItem(ref_t dictionaryRef, ref_t symbolRef)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskMetaArrayRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeArrayEntry(dictionary, symbolRef | mskSymbolRef);
}

void Interpreter :: setDictionaryValue(ref_t dictionaryRef, ustr_t key, int value)
{
   MemoryBase* dictionary = _scope->module->mapSection(dictionaryRef | mskMetaDictionaryRef, true);
   if (!dictionary)
      throw InternalError(errFatalError);

   _logic->writeDictionaryEntry(dictionary, key, value);
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

bool Interpreter :: eval(BuildKey key, ref_t operator_id, ArgumentsInfo& arguments)
{
   switch (key) {
      case BuildKey::StrDictionaryOp:
         return evalStrDictionaryOp(operator_id, arguments);
      case BuildKey::ObjArrayOp:
         return evalObjArrayOp(operator_id, arguments);
      default:
         return false;
   }
}

// --- Compiler::NamespaceScope ---

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
         }
         else if (module->mapSection(reference | mskMetaArrayRef, true)) {
            info.kind = ObjectKind::MetaArray;
            info.type = V_OBJARRAY;
            info.reference = reference;
         }
      }
      else if (internOne) {
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

Compiler::MetaScope :: MetaScope(NamespaceScope* parent)
   : Scope(parent)
{
}

ObjectInfo Compiler::MetaScope :: mapIdentifier(ustr_t identifier, bool referenceOne, EAttr attr)
{
   if (!referenceOne) {
      IdentifierString metaIdentifier(META_PREFIX, identifier);

      NamespaceScope* ns = (NamespaceScope*)getScope(ScopeLevel::Namespace);

      // check if it is a meta dictionary
      ObjectInfo retVal = parent->mapIdentifier(*metaIdentifier, referenceOne, attr | EAttr::Meta);
      if (retVal.kind == ObjectKind::Unknown) {
         return Scope::mapIdentifier(identifier, referenceOne, attr);
      }
      else return retVal;
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
   : SourceScope(ns, reference, visibility)
{
   reserved1 = reserved2 = 0;
   reservedArgs = ns->moduleScope->minimalArgList;
}

// --- Compiler::TemplateScope ---

Compiler::TemplateScope :: TemplateScope(Scope* parent, ref_t reference, Visibility visibility)
   : SourceScope(parent, reference, visibility)
{
   type = TemplateType::None;
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(NamespaceScope* ns, ref_t reference, Visibility visibility)
   : SourceScope(ns, reference, visibility)
{
   info.header.flags = elStandartVMT;
   info.header.parentRef = moduleScope->buildins.superReference;
}

void Compiler::ClassScope :: save()
{
   // save class meta data
   MemoryWriter metaWriter(moduleScope->mapSection(reference | mskMetaClassInfoRef, false));
   metaWriter.Memory()->trim(0);
   info.save(&metaWriter);
}

// --- Compiler::MethodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters({})
{
   message = 0;
   reserved1 = reserved2 = 0;
   selfLocal = 0;
   reservedArgs = parent->moduleScope->minimalArgList;
}

ObjectInfo Compiler::MethodScope :: mapSelf()
{
   if (selfLocal != 0) {
      return { ObjectKind::SelfParam, getClassRef(false), (ref_t)selfLocal };
   }
   else return {};
}

ObjectInfo Compiler::MethodScope :: mapParameter(ustr_t identifier)
{
   int prefix = /*functionMode ? 0 : */-1;

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
      return { ObjectKind::Local, local.class_ref, local.offset };
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
   : Scope(parent)
{
   allocatedArgs = 0;
   tempAllocated2 = tempAllocated1 = 0;
}

Compiler::ExprScope :: ExprScope(CodeScope* parent)
   : Scope(parent)
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
   CodeScope* codeScope = (CodeScope*)getScope(Scope::ScopeLevel::Code);
   if (codeScope != nullptr) {
      if (codeScope->reserved1 < tempAllocated1)
         codeScope->reserved1 = tempAllocated1;

      MethodScope* methodScope = (MethodScope*)getScope(ScopeLevel::Method);
      if (methodScope != nullptr) {
         methodScope->reservedArgs = _max(methodScope->reservedArgs, allocatedArgs);
      }
   }
   else {
      SymbolScope* symbolScope = (SymbolScope*)getScope(ScopeLevel::Symbol);
      if (symbolScope != nullptr) {
         symbolScope->reservedArgs = _max(symbolScope->reservedArgs, allocatedArgs);
         if (symbolScope->reserved1 < tempAllocated1)
            symbolScope->reserved1 = tempAllocated1;
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
}

inline ref_t resolveDictionaryMask(ref_t typeRef)
{
   switch (typeRef) {
      case V_STRINGOBJ:
         return mskMetaArrayRef;
      case V_DICTIONARY:
         return mskMetaDictionaryRef;
      default:
         return 0;
   }
}

ref_t Compiler :: mapNewTerminal(Scope& scope, SyntaxNode nameNode, ustr_t prefix, Visibility visibility)
{
   if (nameNode == SyntaxKey::Name) {
      SyntaxNode terminal = nameNode.firstChild(SyntaxKey::TerminalMask);

      ref_t reference = 0;
      if (!prefix.empty()) {
         IdentifierString nameWithPrefix(prefix, terminal.identifier());

         reference = scope.mapNewIdentifier(*nameWithPrefix, visibility);
      }
      else reference = scope.mapNewIdentifier(terminal.identifier(), visibility);

      nameNode.setArgumentReference(reference);

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

   pos_t argCount = paramCount + 1;

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

   templateName.appendInt(parameters.count());
   templateName.append('#');
   templateName.append(prefix);
   templateName.append(identNode.identifier());

   NamespaceScope* ns = &scope;
   ref_t reference = ns->resolveImplicitIdentifier(*templateName, false, true);
   while (!reference && ns->parent != nullptr) {
      ns = (NamespaceScope*)ns->parent->getScope(Scope::ScopeLevel::Namespace);
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

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::Namespace);
   ref_t templateRef = retrieveTemplate(*ns, node, parameters, prefix);
   if (!templateRef)
      scope.raiseError(errUnknownTemplate, node);

   if(_templateProcessor->importInlineTemplate(*ns->moduleScope, templateRef | mskSyntaxTreeRef, target, 
      parameters)) 
   {
      
   }
   else scope.raiseError(errInvalidSyntax, node);
}

void Compiler :: declareDictionary(Scope& scope, SyntaxNode node, Visibility visibility)
{
   ref_t targetType = V_DICTIONARY;
   declareDictionaryAttributes(scope, node, targetType);

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   ref_t reference = mapNewTerminal(scope, name, META_PREFIX, visibility);
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
   }

   if (test(scope.info.header.flags, elFinal)) {
      //// COMPILER MAGIC : if it is a unsealed nested class inheriting its owner
      //if (!test(scope.info.header.flags, elSealed) && test(flagCopy, elNestedClass)) {
      //   ClassScope* owner = (ClassScope*)scope.getScope(Scope::ScopeLevel::slOwnerClass);
      //   if (owner->classClassMode && scope.info.header.classRef == owner->reference) {
      //      // HOTFIX : if it is owner class class - allow it as well
      //   }
      //   else if (owner->reference != parentRef) {
      //      return InheritResult::irSealed;
      //   }
      //}
      /*else */return InheritResult::irSealed;
   }

   // restore parent and flags
   scope.info.header.parentRef = parentRef;
   scope.info.header.classRef = classClassCopy;

   scope.info.header.flags |= flagCopy;

   return InheritResult::irSuccessfull;
}

void Compiler :: generateMethodDeclaration(ClassScope& scope, SyntaxNode node)
{
   mssg_t message = node.arg.reference;
   MethodInfo methodInfo;

   auto methodIt = scope.info.methods.getIt(message);
   bool existing = !methodIt.eof();
   if (existing)
      methodInfo = *methodIt;

   methodInfo.hints |= node.findChild(SyntaxKey::Hints).arg.reference;

   // check if there is no duplicate method
   if (existing && !methodInfo.inherited) {
      scope.raiseError(errDuplicatedMethod, node);
   }
   else {
      methodInfo.inherited = false;

      if (!existing) {
         scope.info.methods.add(message, methodInfo);
      }
      else *methodIt = methodInfo;
   }
}

void Compiler :: generateMethodDeclarations(ClassScope& scope, SyntaxNode node, SyntaxKey methodKey)
{
   // first pass - mark all multi-methods
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      current = current.nextNode();
   }

   // second pass - ignore template based / autogenerated methods
   current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == methodKey) {
         generateMethodDeclaration(scope, current);
      }

      current = current.nextNode();
   }
}

void Compiler :: generateClassFlags(ClassScope& scope, ref_t declaredFlags)
{
   scope.info.header.flags |= declaredFlags;
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
   if (classRef)
      sizeInfo = _logic->defineStructSize(*scope.moduleScope, classRef);

   if (sizeHint != 0) {
      if (isPrimitiveRef(classRef) && (sizeInfo.size == sizeHint || (classRef == V_INT32 && sizeHint <= sizeInfo.size))) {
         // for primitive types size should be specified
         sizeInfo.size = sizeHint;
      }
      else scope.raiseError(errIllegalField, node);
   }

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

         scope.info.fields.add(name, { -1, arrayRef, classRef });
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

         if (!isClassClassMode) {
            generateClassField(scope, current, attrs, singleField);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: generateClassDeclaration(ClassScope& scope, SyntaxNode node, ref_t declaredFlags)
{
   if (scope.isClassClass()) {
      
   }
   else {
      // HOTFIX : flags / fields should be compiled only for the class itself
      generateClassFlags(scope, declaredFlags);

      // generate fields
      generateClassFields(scope, node, SyntaxTree:: countChild(node, SyntaxKey::Field) == 1);
   }

   //_logic->injectVirtualCode(scope.info);

   if (scope.isClassClass()) {
      generateMethodDeclarations(scope, node, SyntaxKey::StaticMethod);
      generateMethodDeclarations(scope, node, SyntaxKey::Constructor);
   }
   else {
      generateMethodDeclarations(scope, node, SyntaxKey::Method);
   }

   bool emptyStructure = false;
   _logic->validateClassDeclaration(scope.info, emptyStructure);
   if (emptyStructure)
      scope.raiseError(errEmptyStructure, node.findChild(SyntaxKey::Name));

   _logic->tweakClassFlags(scope.info, scope.isClassClass());
}

void Compiler :: declareSymbol(SymbolScope& scope, SyntaxNode node)
{
   declareSymbolAttributes(scope, node);

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   ref_t reference = mapNewTerminal(scope, name, nullptr, scope.visibility);
   if (scope.module->mapSection(reference | mskSymbolRef, true))
      scope.raiseError(errDuplicatedDictionary, name.firstChild(SyntaxKey::TerminalMask));

   node.setArgumentReference(reference);
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

void Compiler :: resolveClassParent(ClassScope& scope, SyntaxNode baseNode)
{
   ref_t parentRef = 0;
   if (baseNode == SyntaxKey::Parent) {
      parentRef = resolveTypeAttribute(scope, baseNode);
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

   declareClassParent(parentRef, scope, baseNode);
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
         case SyntaxKey::Parameter:
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

void Compiler :: declareVMTMessage(MethodScope& scope, SyntaxNode node)
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

   //bool noSignature = true; // NOTE : is similar to weakSignature except if withoutWeakMessages=true
   // if method has an argument list
   SyntaxNode current = node.findChild(SyntaxKey::Parameter);
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Parameter) {
         int index = 1 + scope.parameters.count();
         ref_t classRef = 0;
         ref_t elementRef = 0;
         int size = 0;

         if (!classRef)
            classRef = scope.moduleScope->buildins.superReference;

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
      if (scope.checkHint(MethodHint::Dispatcher)) {
         if (paramCount == 0 && unnamedMessage) {
            actionRef = getAction(scope.moduleScope->buildins.dispatch_message);
            unnamedMessage = false;
         }
         else scope.raiseError(errIllegalMethod, node);
      }

      scope.message = mapMethodName(scope, paramCount, *actionStr, actionRef, flags,
         signature, signatureLen);
      if (unnamedMessage || !scope.message)
         scope.raiseError(errIllegalMethod, node);
   }
}

void Compiler :: declareMethod(MethodScope& methodScope, SyntaxNode node)
{
   if (methodScope.info.hints)
      node.appendChild(SyntaxKey::Hints, methodScope.info.hints);

   if (methodScope.checkHint(MethodHint::Static)) {
      node.setKey(SyntaxKey::StaticMethod);
   }
   else if (methodScope.checkHint(MethodHint::Constructor)) {
      node.setKey(SyntaxKey::Constructor);
   }

   SyntaxNode current = node.firstChild();
   while (current.key != SyntaxKey::None) {

      current = current.nextNode();
   }
}

void Compiler :: declareVMT(ClassScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Method:
         {
            MethodScope methodScope(&scope);
            declareMethodAttributes(methodScope, current);

            if (!current.arg.reference) {
               declareVMTMessage(methodScope, current);
               current.setArgumentReference(methodScope.message);
            }
            else methodScope.message = current.arg.reference;

            declareMethodMetaInfo(methodScope, current);
            declareMethod(methodScope, current);

            if (!_logic->validateMessage(methodScope.message)) {
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

   ref_t dummy = 0; // NOTE : do not set class flags for a class class
   generateClassDeclaration(classClassScope, node, dummy);

   // save declaration
   classClassScope.save();
}

void Compiler :: declareClass(ClassScope& scope, SyntaxNode node)
{
   ref_t flags = scope.info.header.flags;
   declareClassAttributes(scope, node, flags);

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   scope.reference = mapNewTerminal(scope, name, nullptr, scope.visibility);
   if (scope.module->mapSection(scope.reference | mskSymbolRef, true))
      scope.raiseError(errDuplicatedSymbol, name.firstChild(SyntaxKey::TerminalMask));

   node.setArgumentReference(scope.reference);

   resolveClassParent(scope, node.findChild(SyntaxKey::Parent)/*, extensionDeclaration, lxParent*/);

   declareVMT(scope, node);

   // NOTE : generateClassDeclaration should be called for the proper class before a class class one
   //        due to dynamic array implementation (auto-generated default constructor should be removed)
   generateClassDeclaration(scope, node, flags);

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

      // if default constructor has to be created
      injectDefaultConstructor(scope.moduleScope, node);

      declareClassClass(classClassScope, node);
   }
}

void Compiler :: declareNamespace(NamespaceScope& ns, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::SourcePath:
            ns.sourcePath.copy(current.identifier());
            break;
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, 0, ns.defaultVisibility);

            declareSymbol(symbolScope, current);
            break;
         }            
         case SyntaxKey::Class:
         {
            ClassScope classScope(&ns, 0, ns.defaultVisibility);

            declareClass(classScope, current);
            break;
         }
         case SyntaxKey::MetaDictionary:
            declareDictionary(ns, current, Visibility::Public);
            break;
         case SyntaxKey::MetaExpression:
         {
            MetaScope scope(&ns);

            evalStatement(scope, current);
            break;
         }
         case SyntaxKey::Template:
            declareTemplate(ns, current);
            break;
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

ObjectInfo Compiler :: evalOperation(Interpreter& interpreter, Scope& scope, SyntaxNode node, ref_t operator_id)
{
   ObjectInfo loperand = {};
   ObjectInfo roperand = {};
   ObjectInfo ioperand = {};
   ref_t argCount = 2;

   SyntaxNode lnode = node.firstChild(SyntaxKey::Declaration);
   SyntaxNode rnode = lnode.nextNode(SyntaxKey::Declaration);
   if (lnode == SyntaxKey::IndexerOperation) {
      SyntaxNode sublnode = lnode.firstChild(SyntaxKey::Declaration);
      SyntaxNode subrnode = sublnode.nextNode(SyntaxKey::Declaration);

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
      roperand = evalExpression(interpreter, scope, rnode);
   }

   ArgumentsInfo arguments;
   ref_t         argumentRefs[3];
   argumentRefs[0] = resolvePrimitiveReference(loperand);
   arguments.add(loperand);

   argumentRefs[1] = resolvePrimitiveReference(roperand);
   arguments.add(roperand);

   if (argCount == 3) {
      argumentRefs[2] = resolvePrimitiveReference(ioperand);
      arguments.add(ioperand);
   }

   BuildKey opKey = _logic->resolveOp(operator_id, argumentRefs, argCount);

   if (!interpreter.eval(opKey, operator_id, arguments)) {
      scope.raiseError(errCannotEval, node);
   }

   return loperand;
}

ObjectInfo Compiler :: evalObject(Interpreter& interpreter, Scope& scope, SyntaxNode node)
{
   EAttrs mode = ExpressionAttribute::Meta;

   SyntaxNode terminalNode = node.lastChild(SyntaxKey::TerminalMask);

   declareExpressionAttributes(scope, node, mode);

   return mapTerminal(scope, terminalNode, mode.attrs);
}

ObjectInfo Compiler :: evalExpression(Interpreter& interpreter, Scope& scope, SyntaxNode node)
{
   switch (node.key) {
      case SyntaxKey::Expression:
         return evalExpression(interpreter, scope, node.firstChild(SyntaxKey::Declaration));
      case SyntaxKey::AssignOperation:
         return evalOperation(interpreter, scope, node, SET_OPERATOR_ID);
      case SyntaxKey::AddAssignOperation:
         return evalOperation(interpreter, scope, node, ADD_ASSIGN_OPERATOR_ID);
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

void Compiler :: writeObjectInfo(BuildTreeWriter& writer, ObjectInfo info)
{
   switch (info.kind) {
      case ObjectKind::IntLiteral:
         writer.newNode(BuildKey::IntLiteral, info.reference);
         writer.appendNode(BuildKey::Value, info.extra);
         writer.closeNode();
         break;
      //case ObjectKind::StringLiteral:
      //   writer.appendNode(BuildKey::StringLiteral, info.reference);
      //   break;
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
         writer.appendNode(BuildKey::ClassReference, info.reference);
         break;
      case ObjectKind::Param:
      case ObjectKind::SelfParam:
      case ObjectKind::Local:
      case ObjectKind::TempLocal:
         writer.appendNode(BuildKey::Local, info.reference);
         break;
      case ObjectKind::Object:
         break;
      default:
         throw InternalError(errFatalError);
   }
}

ref_t Compiler :: resolveObjectReference(ObjectInfo info)
{
   return info.type;
}

ref_t Compiler :: resolvePrimitiveReference(ObjectInfo info)
{
   if (!isPrimitiveRef(info.type)) {
      return V_OBJECT; // !!temporal
   }

   return info.type;
}

void Compiler :: declareSymbolAttributes(SymbolScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         if (!_logic->validateSymbolAttribute(current.arg.value/*, constant, scope.staticOne, scope.preloaded*/,
            scope.visibility))
         {
            current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: declareClassAttributes(ClassScope& scope, SyntaxNode node, ref_t& flags)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateClassAttribute(current.arg.value, flags, scope.visibility))
            {
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
}

void Compiler :: declareMethodAttributes(MethodScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   bool explicitMode = false;
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         ref_t value = current.arg.reference;

         MethodHint hint = MethodHint::None;
         if (_logic->validateMethodAttribute(value, hint, explicitMode)) {
            scope.info.hints |= (ref_t)hint;
         }
         else {
            current.setArgumentReference(0);

            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, node);
         }
      }
      else if (!explicitMode && current == SyntaxKey::Name) {
         // resolving implicit method attributes
         ref_t attr = scope.moduleScope->attributes.get(current.firstChild(SyntaxKey::TerminalMask).identifier());
         MethodHint hint = MethodHint::None;
         if (_logic->validateImplicitMethodAttribute(attr, hint)) {
            scope.info.hints |= (ref_t)hint;
            current.setKey(SyntaxKey::Attribute);
            current.setArgumentReference(attr);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: declareTemplateAttributes(TemplateScope& scope, SyntaxNode node)
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
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: saveTemplate(TemplateScope& scope, SyntaxNode& node)
{
   IdentifierString prefix;

   int argCount = SyntaxTree::countChild(node, SyntaxKey::TemplateArg);
   prefix.appendInt(argCount);
   prefix.append('#');
   switch (scope.type) {
      case TemplateType::Inline:
         prefix.append(INLINE_PREFIX);
         break;
      default:
         break;
   }

   SyntaxNode name = node.findChild(SyntaxKey::Name);

   ref_t reference = mapNewTerminal(scope, name, *prefix, scope.visibility);
   if (scope.module->mapSection(reference | mskSyntaxTreeRef, true))
      scope.raiseError(errDuplicatedDictionary, name.firstChild(SyntaxKey::TerminalMask));

   MemoryBase* target = scope.module->mapSection(reference | mskSyntaxTreeRef, false);

   SyntaxTree::saveNode(node, target);
}

void Compiler :: declareTemplateCode(TemplateScope& scope, SyntaxNode& node)
{
   declareTemplateAttributes(scope, node);
   switch (scope.type) {
      case TemplateType::Inline:
         break;
      default:
         scope.raiseError(errInvalidSyntax, node);
         break;
   }

   saveTemplate(scope, node);

   node.setKey(SyntaxKey::Idle);
}

void Compiler :: declareTemplate(NamespaceScope& scope, SyntaxNode& node)
{
   node.setKey(SyntaxKey::Idle);
}

void Compiler :: declareDictionaryAttributes(Scope& scope, SyntaxNode node, ref_t& dictionaryType)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Attribute) {
         if (!_logic->validateDictionaryAttribute(current.arg.value, dictionaryType))
         {
            current.setArgumentValue(0); // HOTFIX : to prevent duplicate warnings
            scope.raiseWarning(WARNING_LEVEL_1, wrnInvalidHint, current);
         }
      }

      current = current.nextNode();
   }
}

void Compiler :: declareExpressionAttributes(Scope& scope, SyntaxNode node, ExpressionAttributes& mode)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None)  {
      switch (current.key) {
         case SyntaxKey::Attribute:
            if (!_logic->validateExpressionAttribute(current.arg.reference, mode))
               scope.raiseError(errInvalidHint, current);
            break;
         case SyntaxKey::Type:
            /*if (!EAttrs::test(mode.attrs, EAttr::NoTypeAllowed)) {
               
            }
            else */scope.raiseError(errInvalidHint, current);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: validateType(Scope& scope, ref_t typeRef, SyntaxNode node)
{
   if (!typeRef)
      scope.raiseError(errUnknownClass, node);
}

ref_t Compiler :: resolveTypeIdentifier(Scope& scope, ustr_t identifier, SyntaxKey type)
{
   ObjectInfo identInfo;

   NamespaceScope* ns = (NamespaceScope*)scope.getScope(Scope::ScopeLevel::Namespace);

   identInfo = ns->mapIdentifier(identifier, type == SyntaxKey::reference, EAttr::None);

   switch (identInfo.kind) {
      case ObjectKind::Class:
         return identInfo.reference;
      default:
         return 0;
   }
}

ref_t Compiler :: resolveTypeAttribute(Scope& scope, SyntaxNode node)
{
   ref_t typeRef = 0;
   if (SyntaxTree::test(node.key, SyntaxKey::TerminalMask)) {
      typeRef = resolveTypeIdentifier(scope, node.identifier(), node.key/*, declarationMode, allowRole*/);
   }
   else {
      SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);

      typeRef = resolveTypeIdentifier(scope, 
         terminal.identifier(), terminal.key/*, declarationMode, allowRole*/);
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
               attrs.typeRef = resolveTypeAttribute(scope, current);
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
   if (attrs.typeRef == V_INTBINARY) {
      switch (attrs.size) {
         case 1:
         case 2:
         case 4:
            // treat it like dword
            attrs.typeRef = V_INT32;
            break;
         default:
            scope.raiseError(errInvalidHint, node);
            break;
      }
   }
}

void Compiler :: declareVariable(Scope& scope, SyntaxNode terminal, ref_t typeRef)
{
   ExprScope* exprScope = (ExprScope*)scope.getScope(Scope::ScopeLevel::Expr);
   CodeScope* codeScope = (CodeScope*)scope.getScope(Scope::ScopeLevel::Code);
   if (codeScope == nullptr)
      scope.raiseError(errInvalidOperation, terminal);

   IdentifierString identifier(terminal.identifier());

   ObjectInfo variable;
   variable.type = typeRef;
   variable.kind = ObjectKind::Local;

   if (exprScope)
      exprScope->syncStack();

   variable.reference = codeScope->newLocal();

   if (!codeScope->locals.exist(*identifier)) {
      codeScope->mapNewLocal(*identifier, variable.reference, variable.type, /*variable.element*/0, /*size*/0, true);
   }
   else scope.raiseError(errDuplicatedLocal, terminal);
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
   ref_t      arguments[3];
   ObjectInfo loperand = compileExpression(writer, scope, lnode, EAttr::Parameter);
   ObjectInfo roperand = compileExpression(writer, scope, rnode, EAttr::Parameter);
   ObjectInfo ioperand;
   if (inode != SyntaxKey::None)
      ioperand = compileExpression(writer, scope, inode, EAttr::Parameter);

   arguments[0] = resolvePrimitiveReference(loperand);
   arguments[1] = resolvePrimitiveReference(roperand);
   if (inode != SyntaxKey::None) {
      arguments[2] = resolvePrimitiveReference(ioperand);

      op = _logic->resolveOp(operatorId, arguments, 3);
   }
   else op = _logic->resolveOp(operatorId, arguments, 2);

   if (op != BuildKey::None) {
      writeObjectInfo(writer, loperand);
      writeObjectInfo(writer, roperand);
      if (inode != SyntaxKey::None)
         writeObjectInfo(writer, ioperand);

      writer.appendNode(op);
   }
   else throw InternalError(errFatalError);

   return retVal;
}

mssg_t Compiler :: mapMessage(ExprScope& scope, SyntaxNode current)
{
   ref_t flags = 0;

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

   ref_t actionRef = scope.module->mapAction(*messageStr, 0, false);

   return encodeMessage(actionRef, argCount, flags);
}

ref_t Compiler :: compileMessageArguments(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode current, ArgumentsInfo& arguments)
{
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Expression) {
         arguments.add(compileExpression(writer, scope, current, EAttr::Parameter));
      }

      current = current.nextNode();
   }

   return 0;
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, /*SyntaxNode node, ObjectInfo target, */mssg_t message, ArgumentsInfo & arguments)
{
   ObjectInfo retVal(ObjectKind::Object);

   BuildKey operation = BuildKey::CallOp;
   ref_t argument = message;

   pos_t counter = arguments.count();
   for (unsigned int i = counter; i > 0; i--) {
      ObjectInfo arg = arguments[i - 1];

      writeObjectInfo(writer, arg);
      writer.appendNode(BuildKey::SavingInStack, i - 1);

      //withUnboxing |= unboxingRequired(arg);
   }

   writer.appendNode(operation, argument);

   return retVal;
}

void Compiler :: addBreakpoint(BuildTreeWriter& writer, SyntaxNode node, BuildKey bpKey)
{
   SyntaxNode terminal = node.firstChild(SyntaxKey::TerminalMask);
   if (terminal != SyntaxKey::None) {
      SyntaxNode row = terminal.findChild(SyntaxKey::Row);
      SyntaxNode col = terminal.findChild(SyntaxKey::Column);

      writer.newNode(bpKey);
      writer.appendNode(BuildKey::Row, row.arg.value);
      writer.appendNode(BuildKey::Column, col.arg.value);
      writer.closeNode();
   }
}

ObjectInfo Compiler :: compileMessageOperation(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node)
{
   ArgumentsInfo arguments;

   SyntaxNode current = node.firstChild();
   if (current == SyntaxKey::Object) {
      addBreakpoint(writer, current, BuildKey::Breakpoint);
   }

   arguments.add(compileObject(writer, scope, current, EAttr::Parameter));

   current = current.nextNode();
   mssg_t messageReg = 0;
   messageReg = mapMessage(scope, current);

   compileMessageArguments(writer, scope, current, arguments);

   ObjectInfo retVal = compileMessageOperation(writer, scope, messageReg, arguments);

   scope.reserveArgs(arguments.count());

   return retVal;
}

ObjectInfo Compiler :: compileAssigning(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode loperand, SyntaxNode roperand)
{
   BuildKey operationType = BuildKey::None;
   int operand = 0;

   ObjectInfo target = mapObject(scope, loperand, EAttr::None);
   switch (target.kind) {
      case ObjectKind::Local:
         scope.markAsAssigned(target);
         operationType = BuildKey::Assigning;
         operand = target.reference;
         break;
      default:
         scope.raiseError(errInvalidOperation, loperand.parentNode());
         break;
   }

   ObjectInfo exprVal;
   exprVal = compileExpression(writer, scope, roperand, EAttr::Parameter);

   writeObjectInfo(writer, exprVal);
   writer.appendNode(operationType, operand);

   writeObjectInfo(writer, target);

   return target;
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

ObjectInfo Compiler :: mapStringConstant(Scope& scope, SyntaxNode node)
{
   return ObjectInfo(ObjectKind::StringLiteral, V_STRING, scope.module->mapConstant(node.identifier()));
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

ObjectInfo Compiler :: mapTerminal(Scope& scope, SyntaxNode node, EAttr attrs)
{
   bool forwardMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::Forward);
   bool variableMode = EAttrs::testAndExclude(attrs, ExpressionAttribute::NewVariable);

   ObjectInfo retVal;
   bool invalid = false;
   switch (node.key) {
      case SyntaxKey::identifier:
      case SyntaxKey::reference:
         if (variableMode) {
            invalid = forwardMode;

            declareVariable(scope, node, 0); // !! temporal - typeref should be provided or super class
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

   if (invalid)
      scope.raiseError(errInvalidOperation, node);

   return retVal;
}

ObjectInfo Compiler :: mapObject(Scope& scope, SyntaxNode node, EAttrs mode)
{
   SyntaxNode terminalNode = node.lastChild(SyntaxKey::TerminalMask);

   declareExpressionAttributes(scope, node, mode);

   ObjectInfo retVal = mapTerminal(scope, terminalNode, mode.attrs);

   return retVal;
}

ObjectInfo Compiler :: saveToTempLocal(BuildTreeWriter& writer, ExprScope& scope, ObjectInfo object)
{
   int tempLocal = scope.newTempLocal();
   writer.appendNode(BuildKey::Assigning, tempLocal);

   return { ObjectKind::TempLocal, object.type, (ref_t)tempLocal };
}

ObjectInfo Compiler :: compileObject(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node,
   ExpressionAttribute mode)
{
   if (node == SyntaxKey::Object) {
      ObjectInfo retVal = mapObject(scope, node, mode);

      if (retVal.kind == ObjectKind::Unknown)
         scope.raiseError(errUnknownObject, node.lastChild(SyntaxKey::TerminalMask));

      return retVal;
   }
   else return compileExpression(writer, scope, node, mode);
}

ObjectInfo Compiler :: compileExpression(BuildTreeWriter& writer, ExprScope& scope, SyntaxNode node, 
   ExpressionAttribute mode)
{
   bool paramMode = EAttrs::testAndExclude(mode, EAttr::Parameter);

   ObjectInfo retVal;

   SyntaxNode current = node == SyntaxKey::Expression ? node.firstChild() : node;
   switch (current.key) {
      case SyntaxKey::MessageOperation:
         retVal = compileMessageOperation(writer, scope, current);
         break;
      case SyntaxKey::AssignOperation:
      //case SyntaxKey::AddAssignOperation:
         retVal = compileOperation(writer, scope, current, (int)current.key - OPERATOR_MAKS);
         break;
      case SyntaxKey::Expression:
         retVal = compileExpression(writer, scope, current, mode);
         break;
      case SyntaxKey::Object:
         retVal = compileObject(writer, scope, current, mode);
         break;
      default:
         retVal = compileObject(writer, scope, node, mode);
         break;
   }

   if (paramMode && retVal.kind == ObjectKind::Object) {
      retVal = saveToTempLocal(writer, scope, retVal);
   }

   return retVal;
}

ObjectInfo Compiler::compileRootExpression(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   ExprScope scope(&codeScope);

   writer.appendNode(BuildKey::OpenStatement);
   auto retVal = compileExpression(writer, scope, node, EAttr::None);
   writer.appendNode(BuildKey::EndStatement);

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

void Compiler :: compileSymbol(BuildTreeWriter& writer, SymbolScope& scope, SyntaxNode node)
{
   writer.newNode(BuildKey::Symbol, node.arg.reference);
   writer.newNode(BuildKey::Tape);
   writer.appendNode(BuildKey::OpenFrame);

   ExprScope exprScope(&scope);
   ObjectInfo retVal = compileExpression(writer, exprScope, 
      node.findChild(SyntaxKey::Expression),
      ExpressionAttribute::None);

   writeObjectInfo(writer, retVal);

   exprScope.syncStack();

   writer.appendNode(BuildKey::CloseFrame);
   writer.appendNode(BuildKey::Exit);

   writer.closeNode();
   saveFrameAttributes(writer, scope, scope.reserved1 + scope.reservedArgs, scope.reserved2);
   writer.closeNode();
}

void Compiler :: compileClassSymbol(BuildTreeWriter& writer, ClassScope& scope)
{
   writer.newNode(BuildKey::Symbol, scope.reference);

   writer.newNode(BuildKey::Tape);
   writer.appendNode(BuildKey::OpenFrame);
   ObjectInfo retVal(ObjectKind::Class, scope.info.header.classRef, scope.reference, 0);
   writeObjectInfo(writer, retVal);
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

ObjectInfo Compiler :: compileCode(BuildTreeWriter& writer, CodeScope& codeScope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Expression:
            compileRootExpression(writer, codeScope, current);
            break;
         case SyntaxKey::EOP:
            addBreakpoint(writer, current, BuildKey::EOPBreakpoint);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   return {};
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
   codeScope.mapNewLocal(*scope.moduleScope->selfVar, scope.selfLocal, scope.getClassRef(false));
   writer.appendNode(BuildKey::Assigning, scope.selfLocal);

   ObjectInfo retVal = { };

   SyntaxNode bodyNode = node.firstChild(SyntaxKey::ScopeMask);
   switch (bodyNode.key) {
      case SyntaxKey::CodeBlock:
         retVal = compileCode(writer, codeScope, bodyNode);
         break;
      default:
         break;
   }

   // if the method returns itself
   if (retVal.kind == ObjectKind::Unknown) {
      retVal = scope.mapSelf();

      writeObjectInfo(writer, retVal);
   }

   writer.appendNode(BuildKey::CloseFrame);
}

void Compiler :: compileMethod(BuildTreeWriter& writer, MethodScope& scope, SyntaxNode node)
{
   beginMethod(writer, scope, BuildKey::Method);

   CodeScope codeScope(&scope);

   SyntaxNode current = node.firstChild(SyntaxKey::MemberMask);
   switch (current.key) {
      case SyntaxKey::CodeBlock:
         compileMethodCode(writer, scope, codeScope, node, false);
         break;
      case SyntaxKey::Importing:
         writer.appendNode(BuildKey::Import, current.arg.reference);
         break;
      case SyntaxKey::WithoutBody:
         scope.raiseError(errNoBodyMethod, node);
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

   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::ScopeLevel::Class);

   if (test(classScope->info.header.flags, elDynamicRole))
      throw InternalError(errFatalError);

   if (test(classScope->info.header.flags, elStructureRole)) {
      writer.newNode(BuildKey::CreatingStruct, classScope->info.size);
   }
   else writer.newNode(BuildKey::CreatingClass, classScope->info.fields.count());
   writer.appendNode(BuildKey::Type, classScope->reference);
   writer.closeNode();

   //// call field initilizers if available for default constructor
   //compileSpecialMethodCall(writer, *classScope, scope.moduleScope->init_message);

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

void Compiler :: compileVMT(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      switch (current.key) {
         case SyntaxKey::Method:
         {
            MethodScope methodScope(&scope);
            methodScope.message = current.arg.reference;

            declareVMTMessage(methodScope, current);
            compileMethod(writer, methodScope, current);
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

            compileMethod(writer, methodScope, current);
            break;
         }
         case SyntaxKey::Constructor:
         {
            MethodScope methodScope(&scope);
            methodScope.message = current.arg.reference;

            compileConstructor(writer, methodScope, classClassScope, current);
            break;
         }
         default:
            break;
      }

      current = current.nextNode();
   }
}

void Compiler :: compileClass(BuildTreeWriter& writer, ClassScope& scope, SyntaxNode node)
{
   writer.newNode(BuildKey::Class, scope.reference);
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
         case SyntaxKey::Symbol:
         {
            SymbolScope symbolScope(&ns, current.arg.reference, ns.defaultVisibility);

            compileSymbol(writer, symbolScope, current);
            break;
         }
         case SyntaxKey::Class:
         {
            ClassScope classScope(&ns, current.arg.reference, ns.defaultVisibility);
            ns.moduleScope->loadClassInfo(classScope.info, current.arg.reference, false);

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

void Compiler :: prepare(ModuleScopeBase* moduleScope, ForwardResolverBase* forwardResolver)
{
   auto predefinedInfo = moduleScope->getSection(PREDEFINED_FORWARD, mskMetaDictionaryRef, true);
   if (predefinedInfo.section) {
      _logic->readDictionary(predefinedInfo.section, moduleScope->predefined);
   }
   auto attributeInfo = moduleScope->getSection(ATTRIBUTES_FORWARD, mskMetaDictionaryRef, true);
   if (attributeInfo.section) {
      _logic->readDictionary(attributeInfo.section, moduleScope->attributes);
   }

   // cache the frequently used references
   moduleScope->buildins.superReference = safeMapReference(moduleScope, forwardResolver, SUPER_FORWARD);

   // cache the frequently used messages
   moduleScope->buildins.dispatch_message = encodeMessage(moduleScope->module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
   moduleScope->buildins.constructor_message = encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false), 1, 0);

   // cache self variable
   moduleScope->selfVar.copy(moduleScope->predefined.retrieve<ref_t>("$self", V_SELF_VAR, [](ref_t reference, ustr_t key, ref_t current)
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

void Compiler :: declare(ModuleScopeBase* moduleScope, SyntaxTree& input)
{
   validateScope(moduleScope);

   SyntaxNode node = input.readRoot();
   SyntaxNode current = node.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Namespace) {
         NamespaceScope ns(moduleScope, _errorProcessor, _logic);

         declareNamespace(ns, current);
      }

      current = current.nextNode();
   }
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

         compileNamespace(writer, ns, current);
      }

      current = current.nextNode();
   }

   writer.closeNode();
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
