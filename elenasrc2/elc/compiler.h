//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "elena.h"
#include "compilercommon.h"
#include "bcwriter.h"

namespace _ELENA_
{

//struct Unresolved
//{
//   ident_t    fileName;
//   ref_t      reference;
//   _Module*   module;
//   size_t     row;
//   size_t     col;           // virtual column
//
//   Unresolved()
//   {
//      reference = 0;
//   }
//   Unresolved(ident_t fileName, ref_t reference, _Module* module, size_t row, size_t col)
//   {
//      this->fileName = fileName;
//      this->reference = reference;
//      this->module = module;
//      this->row = row;
//      this->col = col;
//   }
//};

//typedef List<Unresolved> Unresolveds;

// --- Compiler class ---
class Compiler : public _Compiler
{
public:
   struct Parameter
   {
      int    offset;
      ref_t  class_ref;
//      ref_t  element_ref;
//      int    size;

      Parameter()
      {
         offset = -1;
         class_ref = 0;
//         element_ref = 0;
//         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->class_ref = 0;
//         this->element_ref = 0;
//         this->size = 0;
      }
      Parameter(int offset, ref_t class_ref)
      {
         this->offset = offset;
         this->class_ref = class_ref;
//         this->element_ref = 0;
//         this->size = 0;
      }
////      Parameter(int offset, ref_t class_ref, int size)
////      {
////         this->offset = offset;
////         this->class_ref = class_ref;
////         this->element_ref = 0;
////         this->size = size;
////      }
//      Parameter(int offset, ref_t class_ref, ref_t element_ref, int size)
//      {
//         this->offset = offset;
//         this->class_ref = class_ref;
//         this->element_ref = element_ref;
//         this->size = size;
//      }
   };

   // InheritResult
   enum class InheritResult
   {
      irNone = 0,
      irSuccessfull,
      irUnsuccessfull,
      irSealed,
      //irInvalid,
//      irObsolete
   };

   enum ObjectKind
   {
      okUnknown = 0,

      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference
      okClass,                        // param - reference
      okSingleton,                    // param - reference
//      okLiteralConstant,              // param - reference
//      okWideLiteralConstant,          // param - reference
//      okCharConstant,                 // param - reference
//      okIntConstant,                  // param - reference, extraparam - imm argument
//      okUIntConstant,                 // param - reference, extraparam - imm argument
//      okLongConstant,                 // param - reference
//      okRealConstant,                 // param - reference
//      okMessageConstant,              // param - reference
//      okExtMessageConstant,           // param - reference
//      okMessageNameConstant,          // param - reference
//      okArrayConst,
//      okField,                        // param - reference, param - field offset
//      okReadOnlyField,                // param - reference, param - field offset
//      okStaticField,                  // param - reference
//      okStaticConstantField,          // param - reference
//      okClassStaticConstantField,     // param - class reference / 0 (for static methods), extraparam - field offset
//      okFieldAddress,                 // param - field offset
//      okReadOnlyFieldAddress,         // param - field offset, extraparam - class reference
//      okOuter,                        // param - field offset
//      okOuterField,                   // param - field offset, extraparam - outer field offset
//      okOuterReadOnlyField,           // param - field offset, extraparam - outer field offset
//      okOuterSelf,                    // param - field offset, extraparam - outer field offset
//      okOuterStaticField,             // param - field offset, extraparam - outer field offset
//      okClassStaticField,             // param - class reference / 0 (for static methods), extraparam - field offset
////////      okCurrent,                      // param - stack offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = class reference
//      okParamField,
//      okSubject,                      // param - parameter offset
      okSelfParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
//      okSuper,
//      okLocalAddress,                 // param - local offset
//      okParams,                       // param - local offset
//////      okBlockLocal,                   // param - local offset
//      okConstantRole,                 // param - role reference
//      okExplicitConstant,             // param - reference, extraparam - subject
      okExtension,
      okClassSelf,                    // param - class reference; used in class resending expression

//      okExternal,
      okInternal,
//      okPrimitive,                    // param * 4 = size 
//      okPrimCollection                // param - length
   };

//   enum ClassType
//   {
//      ctUndefined            = 0x100, 
//      ctClass                = 0x000,
//      ctClassClass           = 0x001,
//      ctEmbeddable           = 0x002,
//
//      ctUndefinedClass       = 0x100,
//      ctEmbeddableClass      = 0x002,
//      ctEmbeddableClassClass = 0x003,
//   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      // target class reference
      ref_t      reference;
//      ref_t      element;
//      ref_t      extraparam;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->reference = 0;
//         this->element = 0;
//         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->reference = 0;
//         this->extraparam = 0;
//         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->reference = 0;
//         this->element = 0;
//         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference)
      {
         this->kind = kind;
         this->param = param;
         this->reference = reference;
//         this->element = 0;
//         this->extraparam = 0;
      }
//      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference, ref_t element, ref_t extraparam)
//      {
//         this->kind = kind;
//         this->param = param;
//         this->reference = reference;
//         this->element = element;
//         this->extraparam = extraparam;
//      }
   };

   typedef MemoryMap<ident_t, Parameter>  LocalMap;

private:
   // - Scope -
   struct Scope
   {
         enum class ScopeLevel
         {
            slNamespace,
            slClass,
            slSymbol,
            slMethod,
            slCode,
            slYieldCode,
            slOwnerClass,
         };
   
         _ModuleScope* moduleScope;
         _Module*      module;
         Scope*        parent;

         virtual void raiseError(const char* message)
         {
            moduleScope->project->raiseError(message);
         }
         virtual void raiseError(const char* message, SNode terminal)
         {
            parent->raiseError(message, terminal);
         }
         virtual void raiseWarning(int level, const char* message, SNode terminal)
         {
            parent->raiseWarning(level, message, terminal);
         }
//         virtual void raiseWarning(int level, const char* message, ident_t identifier)
//         {
//            parent->raiseWarning(level, message, identifier);
//         }
//         virtual void raiseWarning(int level, const char* message)
//         {
//            parent->raiseWarning(level, message);
//         }

         virtual pos_t saveSourcePath(ByteCodeWriter& writer)
         {
            return parent->saveSourcePath(writer);
         }
         virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path)
         {
            return parent->saveSourcePath(writer, path);
         }

//         virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element)
//         {
//            if (parent) {
//               return parent->resolveAutoType(info, reference, element);
//            }
//            else return false;
//         }
//         virtual bool resolveAutoOutput(ref_t reference)
//         {
//            if (parent) {
//               return parent->resolveAutoOutput(reference);
//            }
//            else return false;
//         }

         virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode)
         {
            if (parent) {
               return parent->mapTerminal(identifier, referenceOne, mode);
            }
            else return ObjectInfo();
         }
   
         virtual Scope* getScope(ScopeLevel level)
         {
            if (parent) {
               return parent->getScope(level);
            }
            else return NULL;
         }
   
         Scope(_ModuleScope* moduleScope)
         {
            this->parent = NULL;
            this->moduleScope = moduleScope;
            this->module = moduleScope->module;
         }
         Scope(Scope* parent)
         {
            this->parent = parent;
            this->moduleScope = parent->moduleScope;
            this->module = parent->module;
         }
   };

   // - NamespaceScope -
   struct NamespaceScope : Scope
   {
      // imported namespaces
      IdentifierList    importedNs;
      ForwardMap        forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;

//      // extensions
//      ExtensionMap      extensions;
//      ExtensionTmplMap  extensionTemplates;
//
//      ref_t             packageReference;
//
////      // list of references to the current module which should be checked after the project is compiled
////      Unresolveds* forwardsUnresolved;

      Visibility        defaultVisibility;

      IdentifierString  ns;
      IdentifierString  name;
      IdentifierString  sourcePath;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slNamespace) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      virtual void raiseError(const char* message)
      {
         Scope::raiseError(message);
      }
      virtual void raiseError(const char* message, SNode terminal)
      {
         moduleScope->raiseError(message, sourcePath, terminal);
      }
      virtual void raiseWarning(int level, const char* message, SNode terminal)
      {
         moduleScope->raiseWarning(level, message, sourcePath, terminal);
      }
////      virtual void raiseWarning(int level, const char* message)
////      {
////         moduleScope->raiseWarning(level, message, sourcePath);
////      }
////      virtual void raiseWarning(int level, const char* message, ident_t identifier)
////      {
////         moduleScope->raiseWarning(level, message, sourcePath, identifier);
////      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      ObjectInfo mapGlobal(ident_t identifier);

      virtual pos_t saveSourcePath(ByteCodeWriter& writer);
      virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path);

      ref_t resolveImplicitIdentifier(ident_t name, bool referenceOne, bool innermost);

      ref_t mapNewTerminal(SNode terminal, Visibility visibility);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

//      void loadExtensions(ident_t ns);
//      void loadExtensions(ident_t ns, ident_t subns, bool internalOne)
//      {
//         IdentifierString fullName(ns);
//         if (internalOne)
//            fullName.append(PRIVATE_PREFIX_NS, getlength(PRIVATE_PREFIX_NS) - 1); // HOTFIX : to exclude the tailing quote symbol
//
//         if (!emptystr(subns)) {
//            fullName.append("'");
//            fullName.append(subns);
//         }
//         loadExtensions(fullName.c_str());
//      }
//
//      void saveExtension(ref_t message, ref_t type, ref_t role, bool internalOne);
//      void saveExtensionTemplate(ref_t message, ident_t pattern);
//
//      void loadModuleInfo(ident_t name)
//      {
//         loadExtensions(name);
//      }
//
//      bool defineForward(ident_t forward, ident_t referenceName)
//      {
//         ObjectInfo info = mapTerminal(referenceName, true, EAttr::eaNone);
//      
//         return forwards.add(forward, info.param, true);
//      }

      NamespaceScope(_ModuleScope* moduleScope/*, ident_t path, IdentifierList* imported*//*, bool withFullInfo*/);
      NamespaceScope(NamespaceScope* parent/*, ident_t path, IdentifierList* imported*//*, bool withFullInfo*/);
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
      ref_t      reference;
      Visibility visibility;

      SourceScope(Scope* parent, ref_t reference, Visibility visibility);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
//      ref_t       extensionClassRef;
//      bool        embeddable;
      bool        classClassMode;
      bool        abstractMode;
      bool        abstractBaseMode;
//      bool        withInitializers;
////      bool        withImplicitConstructor;
//
//      void copyStaticFields(ClassInfo::StaticFieldMap& statics, ClassInfo::StaticInfoMap& staticValues);
//
//      ObjectInfo mapField(ident_t identifier, EAttr scopeMode);
//
//      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slClass || level == ScopeLevel::slOwnerClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save()
      {
         // save class meta data
         MemoryWriter metaWriter(moduleScope->mapSection(reference | mskMetaRDataRef, false), 0);
         metaWriter.Memory()->trim(0);
         info.save(&metaWriter);
      }

      void addAttribute(ref_t message, int attribute, ref_t value)
      {
         ClassInfo::Attribute attr(message, attribute);

         info.methodHints.exclude(attr);
         info.methodHints.add(attr, value);
      }
      int getHint(ref_t message)
      {
         ClassInfo::Attribute attr(message, maHint);

         return info.methodHints.get(attr);
      }
      void addHint(ref_t message, int hint)
      {
         ClassInfo::Attribute attr(message, maHint);

         hint |= info.methodHints.get(attr);
         info.methodHints.exclude(attr);
         info.methodHints.add(attr, hint);
      }
      void removeHint(ref_t message, int hintToRemove)
      {
         ClassInfo::Attribute attr(message, maHint);

         int hints = info.methodHints.get(attr);
         hints &= ~hintToRemove;
         info.methodHints.exclude(attr);
         if (hints != 0)
            info.methodHints.add(attr, hints);
      }

      bool include(ref_t message)
      {
         // check if the method is inhreited and update vmt size accordingly
         auto it = info.methods.getIt(message);
         if (it.Eof()) {
            info.methods.add(message, true);

            return true;
         }
         else {
            (*it) = true;

            return false;
         }
      }

      ClassScope(Scope* parent, ref_t reference, Visibility visibility);
      ClassScope(NamespaceScope* parent, Visibility visibility)
         : ClassScope(parent, 0, visibility)
      {
      }
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      bool  constant;
      bool  singleton;
//      bool  staticOne;
//      bool  preloaded;
      ref_t outputRef;

////      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save();

      SymbolScope(NamespaceScope* parent, ref_t reference, Visibility visibility);
      SymbolScope(NamespaceScope* parent, Visibility visibility)
         : SymbolScope(parent, 0, visibility)
      {
      }
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      ref_t        message;
      LocalMap     parameters;
//      EAttr        scopeMode;
//      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
//      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      int          hints;
      ref_t        outputRef;
//      bool         withOpenArg;
//      bool         classEmbeddable;
//      bool         generic;
//      bool         genericClosure;
//      bool         extensionMode;
//      bool         multiMethod;
      bool         functionMode;
//      bool         nestedMode;
//      bool         subCodeMode;       
//      bool         abstractMethod;
//      bool         yieldMethod;
//      bool         embeddableRetMode;
//      bool         targetSelfMode;     // used for script generated methods - self refers to __target
////      bool         dispatchMode;
      bool         constMode;

//      ref_t getAttribute(MethodAttribute attr, bool ownerClass = true)
//      {
//         ClassInfo::Attribute key(message, attr);
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope->info.methodHints.get(key);
//      }
//
//      void setAttribute(MethodAttribute attr, ref_t value, bool ownerClass = true)
//      {
//         ClassInfo::Attribute key(message, attr);
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         scope->info.methodHints.exclude(key);
//         scope->info.methodHints.add(key, value);
//      }

      bool isPrivate()
      {
         return (hints & tpMask) == tpPrivate;
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getReturningRef(bool ownerClass = true)
      {
         if (outputRef == INVALID_REF) {
            ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

            outputRef = scope ? scope->info.methodHints.get(ClassInfo::Attribute(message, maReference)) : 0;
         }
         return outputRef;
      }

//      ref_t getClassFlags(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->info.header.flags : 0;
//      }
      ref_t getClassRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->reference : 0;
      }

//      virtual bool resolveAutoOutput(ref_t reference)
//      {
//         if (outputRef == V_AUTO) {
//            outputRef = reference;
//
//            return true;
//         }
//         else return Scope::resolveAutoOutput(reference);
//      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      ObjectInfo mapSelf(/*bool forced = false*/);
//      ObjectInfo mapGroup();
      ObjectInfo mapParameter(Parameter param, EAttr mode);

      MethodScope(ClassScope* parent);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;
//      bool         genericMethod;
//      bool         yieldMethod;
//
//      bool         ignoreDuplicates; // used for code templates, should be applied only to the statement
//
//      // scope stack allocation
//      int          reserved;  // allocated for the current statement
//      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

      void mapLocal(ident_t local, int level)
      {
         locals.add(local, Parameter(level));
      }
      void mapLocal(ident_t local, int level, ref_t class_ref/*, int size*/)
      {
         locals.add(local, Parameter(level, class_ref/*, size*/));
      }
//      void mapLocal(ident_t local, int level, ref_t class_ref, ref_t element_ref, int size)
//      {
//         locals.add(local, Parameter(level, class_ref, element_ref, size));
//      }
//
//      // check if a local was declared in one of nested code scopes
//      bool checkLocal(ident_t local)
//      {
//         ObjectInfo info = mapTerminal(local, false, EAttr::eaNone);
//         return info.kind == okLocal || info.kind == okLocalAddress;
//      }
//
////      void freeSpace()
////      {
////         reserved = saved;
////      }
//
//      ObjectInfo mapMember(ident_t identifier);
//
//      ObjectInfo mapGlobal(ident_t identifier);

      ObjectInfo mapLocal(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);
//      virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

//      bool isInitializer()
//      {
//         return getMessageID() == moduleScope->init_message;
//      }
//
//      ref_t getMessageID()
//      {
//         MethodScope* scope = (MethodScope*)getScope(slMethod);
//
//         return scope ? scope->message : 0;
//      }

      ref_t getReturningRef()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);

         return scope ? scope->getReturningRef() : 0;
      }

//      bool withEmbeddableRet()
//      {
//         MethodScope* scope = (MethodScope*)getScope(slMethod);
//
//         return scope ? scope->embeddableRetMode : false;
//      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->reference : 0;
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);

         return scope ? scope->info.header.flags : 0;
      }

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
   };

   struct ExprScope : public Scope
   {
      ExprScope(SourceScope* parent);
      ExprScope(CodeScope* parent);

      ref_t getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(ScopeLevel::slMethod);
      
         return scope ? scope->message : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? ScopeLevel::slOwnerClass : ScopeLevel::slClass);
      
         return scope ? scope->reference : 0;
      }

      ObjectInfo mapGlobal(ident_t identifier);
   };

//   // --- ResendScope ---
//   struct ResendScope : public CodeScope
//   {
//      bool withFrame;
//      bool consructionMode;
//
//      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);
//
//      ResendScope(CodeScope* parent)
//         : CodeScope(parent)
//      {
//         consructionMode = withFrame = false;
//      }
//   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
//      struct Outer
//      {
//         ref_t      reference;
//         bool       preserved;
//         ObjectInfo outerObject;
//
//         Outer()
//         {
//            reference = INVALID_REF;
//            preserved = false;
//         }
//         Outer(int reference, ObjectInfo outerObject)
//         {
//            this->reference = reference;
//            this->outerObject = outerObject;
//            this->preserved = false;
//         }
//      };
//
//      bool                    returningMode;
//      Map<ident_t, Outer>     outers;
//      ClassInfo::FieldTypeMap outerFieldTypes;
//
//      Outer mapSelf();
//      Outer mapOwner();
//      Outer mapParent();
//
//      ObjectInfo allocateRetVar();
//
//      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == ScopeLevel::slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

//      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, EAttr mode);

      InlineClassScope(ExprScope* owner, ref_t reference);
   };

   _CompilerLogic*   _logic;
   ByteCodeWriter    _writer;
   MessageMap        _operators;                        // list of operators

   // optimization rules
//   int            _optFlag;
   TransformTape     _rules;
   SyntaxTrie        _sourceRules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   void validateType(Scope& scope, SNode current, ref_t typeRef, bool ignoreUndeclared);

//   bool calculateIntOp(int operation_id, int arg1, int arg2, int& retVal);
//   bool calculateRealOp(int operation_id, double arg1, double arg2, double& retVal);
//
//   bool isMethodEmbeddable(MethodScope& scope, SNode node);

   void writeMessageInfo(SNode node, _ModuleScope& scope, ref_t messageRef);
   void initialize(ClassScope& scope, MethodScope& methodScope);

//   ref_t resolveMessageOwnerReference(_ModuleScope& scope, ClassInfo& classInfo, ref_t reference, ref_t message);
//
//   int checkMethod(_ModuleScope& scope, ref_t reference, ref_t message)
//   {
//      _CompilerLogic::ChechMethodInfo dummy;
//
//      return _logic->checkMethod(scope, reference, message, dummy);
//   }
//
////   bool verifyGenericArgParamCount(ClassScope& scope, int expectedParamCount);

   bool loadAttributes(_ModuleScope& scope, ident_t name, MessageMap* attributes, bool silentMode);

   ObjectInfo mapClassSymbol(Scope& scope, int classRef);

//   ref_t resolveTypeAttribute(Scope& scope, SNode node, bool declarationMode);
//
//   ref_t resolveMultimethod(ClassScope& scope, ref_t messageRef);
//
//   ref_t resolvePrimitiveReference(Scope& scope, ref_t reference, ref_t elementRef, bool declarationMode);
//   virtual ref_t resolvePrimitiveReference(_ModuleScope& scope, ref_t argRef, ref_t elementRef, ident_t ns, bool declarationMode);
//
//   ref_t resolvePrimitiveArray(_ModuleScope& scope, ref_t templateRef, ref_t elementRef, ident_t ns, bool declarationMode);
//   ref_t resolvePrimitiveArray(Scope& scope, ref_t elementRef, bool declarationMode);
//
//   ref_t resolveReferenceTemplate(_ModuleScope& moduleScope, ref_t operandRef, ident_t ns, bool declarationMode);
//   ref_t resolveReferenceTemplate(Scope& scope, ref_t elementRef, bool declarationMode);
//
//   ref_t resolveConstantObjectReference(CodeScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(_ModuleScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(ExprScope& scope, ObjectInfo object/*, bool noPrimitivesMode, bool unboxWrapper = true*/);
////   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object, ref_t targetRef);
   ref_t resolveTypeIdentifier(Scope& scope, ident_t terminal, LexicalType terminalType, bool declarationMode);
   ref_t resolveTypeIdentifier(Scope& scope, SNode terminal, bool declarationMode);

   ref_t resolveConstant(ObjectInfo retVal, ref_t& parentRef);

//   void saveExtension(ClassScope& scope, ref_t message, bool internalOne);
//   void saveExtension(NamespaceScope& nsScope, ref_t reference, ref_t extensionClassRef, ref_t message, bool internalOne);
//   ref_t mapExtension(CodeScope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo target, int& stackSafeAttr);

   void importCode(SNode node, Scope& scope, ref_t reference, ref_t message);

//   int defineFieldSize(CodeScope& scope, int offset);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreFields, bool ignoreSealed);
//   void inheritClassConstantList(_ModuleScope& scope, ref_t sourceRef, ref_t targetRef);

   // NOTE : the method is used to set template pseudo variable
   void declareProcedureDebugInfo(SNode node, MethodScope& scope, bool withSelf/*, bool withTargetSelf*/);
//   void declareCodeDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope);
//
//   int resolveSize(SNode node, Scope& scope);
   ref_t resolveParentRef(SNode node, Scope& moduleScope, bool silentMode);
//   bool isDependentOnNotDeclaredClass(SNode baseNode, Scope& scope);

//   bool isValidAttributeType(Scope& scope, _CompilerLogic::FieldAttributes& attrs);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreFields = false);
   void compileParentDeclaration(SNode node, ClassScope& scope/*, bool extensionMode*/);
   void generateClassFields(SNode member, ClassScope& scope/*, bool singleField*/);
//   void validateClassFields(SNode node, ClassScope& scope);
//
//   //void declareMetaAttributes(SNode node, NamespaceScope& nsScope);
   void declareSymbolAttributes(SNode node, SymbolScope& scope, bool declarationMode);
   void declareClassAttributes(SNode node, ClassScope& scope, bool visibilityOnly);
////   void declareLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void declareFieldAttributes(SNode member, ClassScope& scope, _CompilerLogic::FieldAttributes& attrs);
   void declareVMT(SNode member, ClassScope& scope);

//   ref_t mapTypeAttribute(SNode member, Scope& scope);
   ref_t mapTemplateAttribute(SNode node, Scope& scope);
   void declareMethodAttributes(SNode member, MethodScope& scope);

//   bool resolveAutoType(ObjectInfo source, ObjectInfo& target, CodeScope& scope);
//
//   bool isTemplateParameterDeclared(SNode node, Scope& scope);
//
//   ref_t resolveVariadicMessage(Scope& scope, ref_t message);
   ref_t resolveOperatorMessage(Scope& scope, ref_t operator_id, int paramCount);
//   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef,
//                                     bool withExtension, int& stackSafeAttr);
////   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef)
////   {
////      int dummy;
////      return resolveMessageAtCompileTime(target, scope, generalMessageRef, implicitSignatureRef, false, dummy);
////   }
   ref_t mapMessage(SNode node, ExprScope& scope/*, bool variadicOne*/);

//   size_t resolveArraySize(SNode node, Scope& scope);
//
   ref_t resolveTypeAttribute(SNode node, Scope& scope, bool declarationMode);
   //ref_t resolveTemplateDeclarationUnsafe(SNode node, Scope& scope, bool declarationMode);
   ref_t resolveTemplateDeclaration(SNode node, Scope& scope, bool declarationMode);

//   void compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope);

   LexicalType declareVariableType(CodeScope& scope, ObjectInfo& variable/*, ClassInfo& localInfo, int size, bool binaryArray, 
                                    int& variableArg, ident_t& className*/);
   void declareVariable(SNode& node, CodeScope& scope, ref_t typeRef/*, bool dynamicArray, bool canBeIdle*/);

   ObjectInfo compileClosure(SNode node, ExprScope& ownerScope, EAttr mode);
   ObjectInfo compileClosure(SNode node, ExprScope& ownerScope, InlineClassScope& scope, EAttr mode);
//   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ObjectInfo target);
//
//   ObjectInfo compileMessageReference(SyntaxWriter& writer, SNode objectNode, CodeScope& scope);
//   ObjectInfo compileSubjectReference(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, EAttr mode);
//   ObjectInfo compileYieldExpression(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, EAttr mode);
//
//   bool writeSizeArgument(SyntaxWriter& writer);
//
//   void writeTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, ObjectInfo object, EAttr mode);
//   void writeParamTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void writeVariableTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void writeParamFieldTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, EAttr mode, LexicalType type);
//   void writeTerminalInfo(SyntaxWriter& writer, SNode node);
//
//   ObjectInfo compileTemplateSymbol(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode);
//   ObjectInfo compileTerminal(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode);
//   ObjectInfo compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ref_t targetRef, EAttr mode);
//
//   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2);
//   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, EAttr mode, int operator_id);
   ObjectInfo compileOperator(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode);
//   ObjectInfo compileIsNilOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo loperand, ObjectInfo roperand);
   void compileBranchingNodes(SNode loperandNode, ExprScope& scope, ref_t ifReference/*, bool loopMode, bool switchMode*/);
   void compileBranchingOp(SNode roperandNode, ExprScope& scope, EAttr mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal);
   ObjectInfo compileBranchingOperator(SNode roperand, ExprScope& scope, ObjectInfo target, EAttr mode, int operator_id);

//   ref_t resolveStrongArgument(CodeScope& scope, ObjectInfo info);
//   ref_t resolveStrongArgument(CodeScope& scope, ObjectInfo param1, ObjectInfo param2);

   /*ref_t*/void compileMessageParameters(SNode node, ExprScope& scope, EAttr mode/*, 
      bool& variadicOne, bool& inlineArg*/);

   ObjectInfo compileMessage(SNode node, ExprScope& scope, /*ref_t exptectedRef,*/ ObjectInfo target, EAttr mode);
   ObjectInfo compileMessage(SNode node, ExprScope& scope, ObjectInfo target, int messageRef, EAttr mode/*, int stackSafeAttr*/);
////   ObjectInfo compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, ObjectInfo role, ref_t targetRef = 0);
//
//   SNode injectAttributeIdentidier(SNode current, Scope& scope);
   void compileTemplateAttributes(SNode current, List<SNode>& parameters, Scope& scope, bool declarationMode);
   EAttr declareExpressionAttributes(SNode& node, ExprScope& scope, EAttr mode);

   void recognizeTerminal(SNode node, ObjectInfo info, ExprScope& scope, EAttr mode);

   ObjectInfo mapTerminal(SNode node, ExprScope& scope, EAttr mode);
   ObjectInfo mapObject(SNode node, ExprScope& scope, EAttr mode);

   ObjectInfo compileExpression(SNode node, ExprScope& scope, ObjectInfo objectInfo, ref_t targetRef, EAttr mode);

   ObjectInfo compileCastingExpression(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode);
   ObjectInfo compileBoxingExpression(SNode node, ExprScope& scope, ObjectInfo target, ClassInfo& targetInfo, EAttr mode);
   ObjectInfo compileBoxingExpression(SNode node, ExprScope& scope, ObjectInfo target, EAttr mode);
//   ObjectInfo compileReferenceExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode);
//   ObjectInfo compileVariadicUnboxing(SyntaxWriter& writer, SNode node, CodeScope& scope, EAttr mode);
   ObjectInfo compileAssigning(SNode node, ExprScope& scope, ObjectInfo target/*, 
      bool accumulateMode*/);
//   ObjectInfo compilePropAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target);
//   ObjectInfo compileWrapping(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, bool callMode);
   ObjectInfo compileRootExpression(SNode node, CodeScope& scope);
   ObjectInfo compileRetExpression(SNode node, CodeScope& scope, EAttr mode);
//   void compileEmbeddableRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);

   ObjectInfo compileSubCode(SNode thenNode, ExprScope& scope, bool branchingMode);

//   void compileStaticAssigning(ObjectInfo target, SNode node, ClassScope& scope, bool accumulatorMode/*, int mode*/);
//   void compileClassConstantAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo retVal, bool accumulatorMode);

   ObjectInfo compileOperation(SNode node, ExprScope& scope, ObjectInfo objectInfo, /*ref_t expectedRef,*/ EAttr mode);

//   ObjectInfo compileCatchOperator(SyntaxWriter& writer, SNode roperand, CodeScope& scope, ref_t operator_id);
//   ObjectInfo compileAltOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo objectInfo);
////   void compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope);
//
//   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
//   int allocateStructure(SNode node, int& size);
//   bool allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand);
//
//   ObjectInfo compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t expectedRef, EAttr mode);
//   ObjectInfo compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ref_t signature, ObjectInfo info);
//
//   void compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
//   void compileConstructorDispatchExpression(SNode node, CodeScope& scope);
//   void compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod/*, bool extensionMode*/);
   void compileDispatchExpression(SNode node, CodeScope& scope);
//   void compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope);

   ObjectInfo compileCode(SNode node, CodeScope& scope);

   void declareArgumentAttributes(SNode node, Scope& scope, ref_t& classRef, /*ref_t& elementRef, */bool declarationMode);
   void declareArgumentList(SNode node, MethodScope& scope/*, bool withoutWeakMessages*/, bool declarationMode);
//   ref_t declareInlineArgumentList(SNode node, MethodScope& scope, bool declarationMode);
   /*bool*/void declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, EAttr mode);

//////   void declareSingletonClass(SNode node, ClassScope& scope);

   void compileActionMethod(SNode member, MethodScope& scope);
   void compileExpressionMethod(SNode member, MethodScope& scope/*, bool lazyMode*/);
//   void compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withGenericMethods = false, bool withOpenArgGenerics = false);

   void beginMethod(SNode node, MethodScope& scope);
   void endMethod(SNode node, MethodScope& scope, CodeScope& codeScope, int argCount, int preallocated);
   void compileMethodCode(SNode node, SNode body, MethodScope& scope, CodeScope& codeScope,
      int& preallocated);

//   void predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope);
//   void compileEmbeddableMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileMethod(/*SyntaxWriter& writer, */SNode node, MethodScope& scope);
//   void compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
//   void compileInitializer(SyntaxWriter& writer, SNode node, MethodScope& scope);
//
//   void compileYieldDispatch(SyntaxWriter& writer, int index, int index2, int preallocated);
//   void compileYieldEnd(SyntaxWriter& writer, int index);
//   void compileYieldableMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
//
//   void compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, ref_t message);

   //void compileDefaultConstructor(SNode node, MethodScope& scope);
   //void compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);

//   ref_t compileClassPreloadedCode(_ModuleScope& scope, ref_t classRef, SNode node);
//   void compilePreloadedCode(SymbolScope& scope);
//   void compilePreloadedCode(_ModuleScope& scope, SNode node);
   void compileSymbolCode(ClassScope& scope);

   void compileAction(SNode& node, ClassScope& scope, SNode argNode, EAttr mode);
   void compileNestedVMT(SNode& node, InlineClassScope& scope);

   void compileVMT(SNode node, ClassScope& scope, bool exclusiveMode = false, bool ignoreAutoMultimethods = false);
   void compileClassVMT(SNode node, ClassScope& classClassScope, ClassScope& classScope);
//   void compileForward(SNode ns, NamespaceScope& scope);

   void generateClassField(ClassScope& scope, SNode node, _CompilerLogic::FieldAttributes& attrs/*, bool singleField*/);
//   void generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, ref_t elementRef, bool isSealed, bool isConst, bool isArray);
   
   void generateClassFlags(ClassScope& scope, SNode node);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t message, bool allowTypeAttribute);

   void generateMethodDeclaration(SNode current, ClassScope& scope/*, bool hideDuplicates*/, bool closed, 
      bool allowTypeAttribute/*, bool embeddableClass*/);
   void generateMethodDeclarations(SNode node, ClassScope& scope, bool closed, LexicalType methodType, 
      bool allowTypeAttribute/*, bool embeddableClass*/);
////   // classClassType == None for generating a class, classClassType == Normal | Embeddable for a class class
   void generateClassDeclaration(SNode node, ClassScope& scope/*, ClassType classType*/, bool nestedDeclarationMode = false);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SNode node, SymbolScope& scope);
   bool compileSymbolConstant(/*SNode node, */SymbolScope& scope, ObjectInfo retVal/*, bool accumulatorMode, ref_t accumulatorRef*/);
//   void compileSymbolAttribtes(_ModuleScope& scope, ref_t reference, bool publicAttr);
//   //void compileMetaCategory(SNode node, NamespaceScope& scope);
//
//////   bool validate(_ProjectManager& project, _Module* module, int reference);
//
//   ObjectInfo assignResult(SyntaxWriter& writer, CodeScope& scope, bool fpuMode, ref_t targetRef, ref_t elementRef = 0);

   bool convertObject(SNode node, ExprScope& scope, ref_t targetRef, ObjectInfo source, EAttr mode);
   bool sendTypecast(SNode& node, ExprScope& scope, ref_t targetRef, ObjectInfo source);

//   void compileExternalArguments(SNode node, Scope& scope);
//
//   void injectBoxingTempLocal(SNode node, int& counter, Map<ClassInfo::Attribute, int>& boxed, Map<int, int>& tempLocal);
//   bool analizeParameterBoxing(SNode node, int& counter, Map<ClassInfo::Attribute, int>& boxed, Map<int, int>& tempLocal);
   void analizeCodePatterns(SNode node, NamespaceScope& scope);
   void analizeMethod(SNode node, NamespaceScope& scope);
   void analizeClassTree(SNode node, ClassScope& scope);
   void analizeSymbolTree(SNode node, Scope& scope);
//   void analizeMessageParameters(SNode node);
//
//   void defineEmbeddableAttributes(ClassScope& scope, SNode node);
//
//   void createPackageInfo(_Module* module, _ProjectManager& project);

   void declareMembers(SNode node, NamespaceScope& scope);

   bool compileDeclarations(SNode node, NamespaceScope& scope, bool forced, bool& repeatMode);
   void compileImplementations(SNode node, NamespaceScope& scope);

////   void generateSyntaxTree(SyntaxWriter& writer, SNode node, ModuleScope& scope, SyntaxTree& autogenerated);

//   //void generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument);

   void generateClassSymbol(SyntaxWriter& writer, ClassScope& scope);
//   void generateSymbolWithInitialization(SyntaxWriter& writer, ClassScope& scope, ref_t implicitConstructor);

   void declareNamespace(SNode& node, NamespaceScope& scope, bool withImports);

//   void registerExtensionTemplateMethod(SNode node, NamespaceScope& scope, ref_t extensionRef);
//   void registerExtensionTemplate(SNode node, NamespaceScope& scope, ref_t extensionRef);
//   void registerTemplateSignature(SNode node, NamespaceScope& scope, IdentifierString& signature);

   bool matchTriePatterns(_ModuleScope& scope, SNode& node, SyntaxTrie& trie, List<SyntaxTrieNode>& matchedPatterns);
   bool optimizeTriePattern(_ModuleScope& scope, SNode& node, int patternId);
   bool optimizeConstProperty(_ModuleScope& scope, SNode& node);
//   bool optimizeEmbeddableReturn(_ModuleScope& scope, SNode& node, bool argMode);
//   bool optimizeEmbeddableCall(_ModuleScope& scope, SNode& node);
//   bool optimizeAssigningBoxing(_ModuleScope& scope, SNode& node);
//   void optimizeBoxing(_ModuleScope& scope, SNode& node);
//   bool optimizeConstantAssigning(_ModuleScope& scope, SNode& node);
//   bool optimizeStacksafeCall(_ModuleScope& scope, SNode& node);
//   bool optimizeStacksafeOp(_ModuleScope& scope, SNode& node);
//   bool optimizeBoxingBoxing(_ModuleScope& scope, SNode& node);
//   bool optimizeAssigningOp(_ModuleScope& scope, SNode& node);
//   bool optimizeDoubleAssigning(_ModuleScope& scope, SNode& node);
//   bool optimizeDirectRealOp(_ModuleScope& scope, SNode& node);
//   bool optimizeDirectIntOp(_ModuleScope& scope, SNode& node);
//   bool optimizeBranching(_ModuleScope& scope, SNode& node);
//   bool optimizeConstants(_ModuleScope& scope, SNode& node);
//   bool optimizeArgBoxing(_ModuleScope& scope, SNode& node);
//   bool optimizeArgOp(_ModuleScope& scope, SNode& node);
//   bool optimizeByRefAssigning(_ModuleScope& scope, SNode& node);
//   bool optimizeDuplicateboxing(_ModuleScope& scope, SNode& node);
//   bool optimizeUnboxing(_ModuleScope& scope, SNode& node);
//   bool optimizeNestedExpression(_ModuleScope& scope, SNode& node);
//   bool optimizeNewArrBoxing(_ModuleScope& scope, SNode& node);
//   bool optimizeAssigningTargetBoxing(_ModuleScope& scope, SNode& node);
//
//   //int saveMetaInfo(_ModuleScope& scope, ident_t info);

   void saveNamespaceInfo(SNode node, NamespaceScope& scope, bool innerMost);
   void declareTemplate(SNode node, NamespaceScope& scope);

public:
   void loadRules(StreamReader* optimization);
   void loadSourceRules(StreamReader* optimization);
//   void turnOnOptimiation(int level)
//   {
//      _optFlag |= level;
//   }

   void declareModuleIdentifiers(SyntaxTree& tree, _ModuleScope& scope);

   // return true if no forward class declarations are encountered
   bool declareModule(SyntaxTree& tree, _ModuleScope& scope, bool forced, bool& repeatMode);
   void compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope, ident_t greeting);

   void initializeScope(ident_t name, _ModuleScope& scope, bool withDebugInfo);

//////   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);
//   void copyStaticFieldValues(SNode node, ClassScope& scope);

   // _Compiler interface implementation
//   virtual void injectBoxing(SyntaxWriter& writer, _ModuleScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode = false);
//   virtual SNode injectTempLocal(SNode node, int size, bool boxingMode);
//   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg, ref_t targetClassRef,
//      int stacksafeAttr, bool embeddableAttr);
//   virtual void injectEmbeddableRet(SNode assignNode, SNode callNode, ref_t actionRef);
//   virtual void injectEmbeddableOp(_ModuleScope& scope, SNode assignNode, SNode callNode, ref_t subject, int paramCount/*, int verb*/);
//   virtual void injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t privateRef);
//   virtual void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType);
//   virtual void injectVirtualMultimethodConversion(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType);
////   virtual void injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType);
   virtual void injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, ref_t message, ident_t variable, ref_t outputRef);
//   virtual void injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument);
//   virtual void injectVirtualField(SNode classNode, ref_t arg, LexicalType subType, ref_t subArg, int postfixIndex, 
//      LexicalType objType, int objArg);
////   virtual void injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef);
////   virtual void injectDirectMethodCall(SyntaxWriter& writer, ref_t targetRef, ref_t message);
   virtual void injectDefaultConstructor(_ModuleScope& scope, SNode classNode);
////   virtual void generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
//   virtual void generateOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef);
//   virtual void generateClosedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);
//   virtual void generateSealedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);
//
//   virtual void registerExtensionTemplate(SyntaxTree& tree, _ModuleScope& scope, ident_t ns, ref_t extensionRef);
//   virtual ref_t generateExtensionTemplate(_ModuleScope& scope, ref_t templateRef, size_t argumentLen, ref_t* arguments, ident_t ns);
//
////   //virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
