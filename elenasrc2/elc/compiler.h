//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "elena.h"
#include "compilercommon.h"
#include "bcwriter.h"

namespace _ELENA_
{

////struct Unresolved
////{
////   ident_t    fileName;
////   ref_t      reference;
////   _Module*   module;
////   size_t     row;
////   size_t     col;           // virtual column
////
////   Unresolved()
////   {
////      reference = 0;
////   }
////   Unresolved(ident_t fileName, ref_t reference, _Module* module, size_t row, size_t col)
////   {
////      this->fileName = fileName;
////      this->reference = reference;
////      this->module = module;
////      this->row = row;
////      this->col = col;
////   }
////};
//
////typedef List<Unresolved> Unresolveds;

// --- Compiler class ---
class Compiler : public _Compiler
{
public:
   struct Parameter
   {
      int    offset;
      ref_t  class_ref;
      ref_t  element_ref;
      int    size;

      Parameter()
      {
         offset = -1;
         class_ref = 0;
         element_ref = 0;
         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->class_ref = 0;
         this->element_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t class_ref)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = 0;
         this->size = 0;
      }
//      Parameter(int offset, ref_t class_ref, int size)
//      {
//         this->offset = offset;
//         this->class_ref = class_ref;
//         this->element_ref = 0;
//         this->size = size;
//      }
      Parameter(int offset, ref_t class_ref, ref_t element_ref, int size)
      {
         this->offset = offset;
         this->class_ref = class_ref;
         this->element_ref = element_ref;
         this->size = size;
      }
   };

   // InheritResult
   enum InheritResult
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
      okLiteralConstant,              // param - reference
      okWideLiteralConstant,          // param - reference
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference, extraparam - imm argument
      okUIntConstant,                 // param - reference, extraparam - imm argument
//      okLongConstant,                 // param - reference
//      okRealConstant,                 // param - reference
//      okMessageConstant,              // param - reference
//      okExtMessageConstant,           // param - reference
//      okSignatureConstant,            // param - reference
      okArrayConst,
      okField,                        // param - reference, param - field offset, extraparam - class reference
      okReadOnlyField,                // param - reference, param - field offset, extraparam - class reference
      okStaticField,                  // param - reference
      okStaticConstantField,          // param - reference
      okClassStaticConstantField,     // param - class reference / 0 (for static methods), extraparam - field offset
      okFieldAddress,                 // param - field offset
      okReadOnlyFieldAddress,         // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okOuterReadOnlyField,           // param - field offset, extraparam - outer field offset
      okOuterSelf,                    // param - field offset, extraparam - outer field offset
      okOuterStaticField,             // param - field offset, extraparam - outer field offset
      okClassStaticField,             // param - class reference / 0 (for static methods), extraparam - field offset
//////      okCurrent,                      // param - stack offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = class reference
//////      okParamField,
//      okSubject,                      // param - parameter offset
      okSelfParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
//      okSuper,
      okLocalAddress,                 // param - local offset
      okParams,                       // param - local offset
////      okBlockLocal,                   // param - local offset
      okConstantRole,                 // param - role reference
//      okExplicitConstant,             // param - reference, extraparam - subject
      okExtension,
      okClassSelf,                    // param - class reference; used in class resending expression

      okExternal,
      okInternal,
//      okPrimitiveConv
   };

   enum ClassType
   {
      ctUndefined            = 0x100, 
      ctClass                = 0x000,
      ctClassClass           = 0x001,
      ctEmbeddable           = 0x002,

      ctUndefinedClass       = 0x100,
      ctEmbeddableClass      = 0x002,
      ctEmbeddableClassClass = 0x003,
   };

   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      // target class reference
      ref_t      reference;
      ref_t      element;
      ref_t      extraparam;

      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->reference = 0;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->reference = 0;
         this->extraparam = 0;
         this->element = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->reference = 0;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference)
      {
         this->kind = kind;
         this->param = param;
         this->reference = reference;
         this->element = 0;
         this->extraparam = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t reference, ref_t element, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->reference = reference;
         this->element = element;
         this->extraparam = extraparam;
      }
   };

   typedef MemoryMap<ident_t, Parameter>  LocalMap;

private:
   // - Scope -
   struct Scope
   {
         enum ScopeLevel
         {
            slNamespace,
            slClass,
            slSymbol,
            slMethod,
            slCode,
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
//
         virtual pos_t saveSourcePath(ByteCodeWriter& writer)
         {
            return parent->saveSourcePath(writer);
         }
         virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path)
         {
            return parent->saveSourcePath(writer, path);
         }

         virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element)
         {
            if (parent) {
               return parent->resolveAutoType(info, reference, element);
            }
            else return false;
         }
   
         virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode)
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
      IdentifierList importedNs;
      ForwardMap     forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;

      // extensions
      ExtensionMap      extensions;

////      ref_t packageReference;
//
////      // list of references to the current module which should be checked after the project is compiled
////      Unresolveds* forwardsUnresolved;

      IdentifierString ns;
      IdentifierString sourcePath;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slNamespace) {
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
////      //virtual void raiseWarning(int level, const char* message, int row, int col, ident_t sourcePath, ident_t terminal)
////      //{
////      //   moduleScope->raiseWarning(level, message, sourcePath, );
////      //}

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);

      ObjectInfo mapGlobal(ident_t identifier);

      virtual pos_t saveSourcePath(ByteCodeWriter& writer);
      virtual pos_t saveSourcePath(ByteCodeWriter& writer, ident_t path);

////      ref_t resolveFullReference(ident_t name);
      ref_t resolveImplicitIdentifier(ident_t name, bool referenceOne);

      ref_t mapNewTerminal(SNode terminal);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      void loadExtensions(ident_t ns);
      void loadExtensions(ident_t ns, ident_t subns, bool internalOne)
      {
         IdentifierString fullName(ns);
         if (internalOne)
            fullName.append(PRIVATE_PREFIX_NS, getlength(PRIVATE_PREFIX_NS) - 1); // HOTFIX : to exclude the tailing quote symbol

         if (!emptystr(subns)) {
            fullName.append("'");
            fullName.append(subns);
         }
         loadExtensions(fullName.c_str());
      }

      void saveExtension(ref_t message, ref_t type, ref_t role, bool internalOne);

      void loadModuleInfo(ident_t name)
      {
         loadExtensions(name);
      }

////      bool defineForward(ident_t forward, ident_t referenceName)
////      {
////         ObjectInfo info = mapTerminal(referenceName, true, 0);
////      
////         return forwards.add(forward, info.param, true);
////      }
////
//////      pos_t saveSourcePath(ByteCodeWriter& writer, ident_t sourcePath);

      NamespaceScope(_ModuleScope* moduleScope/*, ident_t path*/, ident_t ns/*, IdentifierList* imported*//*, bool withFullInfo*/);
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
      ref_t          reference;
      bool           internalOne;

      SourceScope(Scope* parent, ref_t reference/*, ident_t sourcePath*/);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
      ref_t       extensionClassRef;
      bool        embeddable;
      bool        classClassMode;
      bool        abstractMode;
      bool        abstractBaseMode;
//      bool        withImplicitConstructor;
//
//      void copyStaticFields(ClassInfo::StaticFieldMap& statics, ClassInfo::StaticInfoMap& staticValues);

      ObjectInfo mapField(ident_t identifier, int scopeMode);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass || level == slOwnerClass) {
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

      ClassScope(Scope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      bool  constant;
      bool  staticOne;
      bool  preloaded;
      ref_t outputRef;

////      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      void save();

      SymbolScope(NamespaceScope* parent, ref_t reference);
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      ref_t        message;
      LocalMap     parameters;
      int          scopeMode;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      int          hints;
      ref_t        outputRef;
      bool         withOpenArg;
      bool         classEmbeddable;
//      bool         generic;
//      bool         genericClosure;
      bool         extensionMode;
      bool         multiMethod;
      bool         closureMode;
      bool         nestedMode;
      bool         subCodeMode;       
      bool         abstractMethod;
//      bool         dispatchMode;
      
      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getReturningRef(bool ownerClass = true)
      {
         if (outputRef == INVALID_REF) {
            ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

            outputRef = scope ? scope->info.methodHints.get(ClassInfo::Attribute(message, maReference)) : 0;
         }
         return outputRef;
      }

      //ref_t getClassFlags(bool ownerClass = true)
      //{
      //   ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

      //   return scope ? scope->info.header.flags : 0;
      //}
//      ref_t getClassRef(bool ownerClass = true)
//      {
//         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);
//
//         return scope ? scope->reference : 0;
//      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);

      ObjectInfo mapSelf(/*bool forced = false*/);
//      ObjectInfo mapGroup();
      ObjectInfo mapParameter(Parameter param);

      MethodScope(ClassScope* parent);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;
//      bool         genericMethod;

      // scope stack allocation
      int          reserved;  // allocated for the current statement
      int          saved;     // permanently allocated

      // scope bookmarks
      int rootBookmark;

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
      void mapLocal(ident_t local, int level, ref_t class_ref, ref_t element_ref, int size)
      {
         locals.add(local, Parameter(level, class_ref, element_ref, size));
      }

//      void freeSpace()
//      {
//         reserved = saved;
//      }

      ObjectInfo mapMember(ident_t identifier);

//      ObjectInfo mapGlobal(ident_t identifier);
//
      ObjectInfo mapLocal(ident_t identifier);

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);
      virtual bool resolveAutoType(ObjectInfo& info, ref_t reference, ref_t element);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

      //bool isInitializer()
      //{
      //   return getMessageID() == (encodeAction(INIT_MESSAGE_ID) | SPECIAL_MESSAGE);
      //}

      ref_t getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->message : 0;
      }

      ref_t getReturningRef()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->getReturningRef() : 0;
      }

      ref_t getClassRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.flags : 0;
      }

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
   };

   // --- ResendScope ---
   struct ResendScope : public CodeScope
   {
      bool withFrame;
      bool consructionMode;

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);

      ResendScope(CodeScope* parent)
         : CodeScope(parent)
      {
         consructionMode = withFrame = false;
      }
   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         ref_t      reference;
         bool       preserved;
         ObjectInfo outerObject;

         Outer()
         {
            reference = INVALID_REF;
            preserved = false;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
            this->preserved = false;
         }
      };

      bool                    returningMode;
      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();
      Outer mapOwner();
      Outer mapParent();

      ObjectInfo allocateRetVar();

      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier, bool referenceOne, int mode);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   _CompilerLogic*  _logic;

   ByteCodeWriter   _writer;

   MessageMap     _operators;                        // list of operators

   int            _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   bool calculateIntOp(int operation_id, int arg1, int arg2, int& retVal);
//   bool calculateRealOp(int operation_id, double arg1, double arg2, double& retVal);

   void writeMessageInfo(SyntaxWriter& writer, _ModuleScope& scope, ref_t messageRef);
   void initialize(ClassScope& scope, MethodScope& methodScope);

   int checkMethod(_ModuleScope& scope, ref_t reference, ref_t message)
   {
      _CompilerLogic::ChechMethodInfo dummy;

      return _logic->checkMethod(scope, reference, message, dummy);
   }

//   bool verifyGenericArgParamCount(ClassScope& scope, int expectedParamCount);

   void loadAttributes(_ModuleScope& scope, ident_t name, MessageMap* attributes);

   ObjectInfo mapClassSymbol(Scope& scope, int classRef);

   ref_t resolveMultimethod(ClassScope& scope, ref_t messageRef);

   ref_t resolvePrimitiveReference(Scope& scope, ref_t reference, ref_t elementRef);
   virtual ref_t resolvePrimitiveReference(_ModuleScope& scope, ref_t argRef, ref_t elementRef, ident_t ns);

   ref_t resolvePrimitiveArray(_ModuleScope& scope, ref_t templateRef, ref_t elementRef, ident_t ns);
   ref_t resolvePrimitiveArray(Scope& scope, ref_t elementRef);

   ref_t resolveReferenceTemplate(_ModuleScope& moduleScope, ref_t operandRef, ident_t ns);
   ref_t resolveReferenceTemplate(Scope& scope, ref_t elementRef);

//   ref_t resolveConstantObjectReference(CodeScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(_ModuleScope& scope, ObjectInfo object);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);
//   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object, ref_t targetRef);
   ref_t resolveImplicitIdentifier(Scope& scope, SNode terminal);
   ref_t resolveImplicitIdentifier(Scope& scope, ident_t identifier, bool referenceOne, bool gloabalOne = false);

   void saveExtension(ClassScope& scope, ref_t message, bool internalOne);
   void saveExtension(NamespaceScope& nsScope, ref_t reference, ref_t extensionClassRef, ref_t message, bool internalOne);
   ref_t mapExtension(CodeScope& scope, ref_t& messageRef, ref_t implicitSignatureRef, ObjectInfo target, int& stackSafeAttr);

   void importCode(SyntaxWriter& writer, SNode node, Scope& scope, ident_t reference, ref_t message);

   int defineFieldSize(CodeScope& scope, int offset);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef/*, bool ignoreSealed*/);
//   void inheritClassConstantList(_CompilerScope& scope, ref_t sourceRef, ref_t targetRef);

   // NOTE : the method is used to set template pseudo variable
   void declareParameterDebugInfo(SyntaxWriter& writer, SNode node, MethodScope& scope, bool withSelf/*, bool withTargetSelf*/);

   int resolveSize(SNode node, Scope& scope);
   ref_t resolveParentRef(SNode node, Scope& moduleScope, bool silentMode);
//   bool isDependentOnNotDeclaredClass(SNode baseNode, Scope& scope);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef/*, bool ignoreSealed = false*/);
   void compileParentDeclaration(SNode node, ClassScope& scope, bool extensionMode);
   void generateClassFields(SNode member, ClassScope& scope, bool singleField);

   void declareSymbolAttributes(SNode node, SymbolScope& scope);
   void declareClassAttributes(SNode node, ClassScope& scope);
//   void declareLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void declareFieldAttributes(SNode member, ClassScope& scope, ref_t& fieldRef/*, ref_t& elementRef*/, int& size, bool& isStaticField, 
      bool& isSealed, bool& isConstant, bool& isEmbeddable);
   void declareVMT(SNode member, ClassScope& scope);

   ref_t mapTypeAttribute(SNode member, Scope& scope);
   ref_t mapTemplateAttribute(SNode node, Scope& scope);
   void declareMethodAttributes(SNode member, MethodScope& scope);

   bool resolveAutoType(ObjectInfo source, ObjectInfo& target, CodeScope& scope);

   ref_t resolveVariadicMessage(Scope& scope, ref_t message);
   ref_t resolveOperatorMessage(Scope& scope, ref_t operator_id, int paramCount);
   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef,
                                     bool withExtension, int& stackSafeAttr);
//   ref_t resolveMessageAtCompileTime(ObjectInfo& target, CodeScope& scope, ref_t generalMessageRef, ref_t implicitSignatureRef)
//   {
//      int dummy;
//      return resolveMessageAtCompileTime(target, scope, generalMessageRef, implicitSignatureRef, false, dummy);
//   }
   ref_t mapMessage(SNode node, CodeScope& scope);

   size_t resolveArraySize(SNode node, Scope& scope);

   ref_t resolveTemplateDeclaration(SNode node, Scope& scope);

//   void compileSwitch(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileVariable(SyntaxWriter& writer, SNode& node, CodeScope& scope, ref_t typeRef, bool dynamicArray);

   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileClosure(SyntaxWriter& writer, SNode node, CodeScope& ownerScope, InlineClassScope& scope);
//   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope);
//   ObjectInfo compileCollection(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ref_t vmtReference);
//
//   ObjectInfo compileMessageReference(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, int mode);
   void writeTerminal(SyntaxWriter& writer, SNode terminal, CodeScope& scope, ObjectInfo object, int mode);
   void writeParamTerminal(SyntaxWriter& writer, CodeScope& scope, ObjectInfo object, int mode, LexicalType type);
   void writeTerminalInfo(SyntaxWriter& writer, SNode node);

   ObjectInfo compileTemplateSymbol(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileTerminal(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(SyntaxWriter& writer, SNode objectNode, CodeScope& scope, ref_t targetRef, int mode);

   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, int operator_id, int paramCount, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode, int operator_id);
   ObjectInfo compileOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
//   ObjectInfo compileIsNilOperator(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo loperand, ObjectInfo roperand);
   void compileBranchingNodes(SyntaxWriter& writer, SNode loperandNode, CodeScope& scope, ref_t ifReference, bool loopMode/*, bool switchMode*/);
   void compileBranchingOperand(SyntaxWriter& writer, SNode roperandNode, CodeScope& scope, int mode, int operator_id, ObjectInfo loperand, ObjectInfo& retVal);
   ObjectInfo compileBranchingOperator(SyntaxWriter& writer, SNode roperand, CodeScope& scope, ObjectInfo target, int mode, int operator_id);

   ref_t resolveStrongArgument(CodeScope& scope, ObjectInfo info);
   ref_t resolveStrongArgument(CodeScope& scope, ObjectInfo param1, ObjectInfo param2);

   ref_t compileMessageParameters(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode = 0);

   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope/*, ref_t exptectedRef*/, ObjectInfo target, int mode);
   ObjectInfo compileMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode, int stackSafeAttr);
//   ObjectInfo compileExtensionMessage(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, ObjectInfo role, ref_t targetRef = 0);
//
   void compileTemplateAttributes(SNode current, List<SNode>& parameters, Scope& scope);
   ref_t compileExpressionAttributes(SyntaxWriter& writer, SNode& node, CodeScope& scope, int mode);

   ObjectInfo compileBoxingExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileReferenceExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target);
   ObjectInfo compilePropAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target);
//   ObjectInfo compileExtension(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo target);
   ObjectInfo compileRootExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   ObjectInfo compileExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t targetRef, int mode);
   ObjectInfo compileRetExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, int mode);

   ObjectInfo compileSubCode(SyntaxWriter& writer, SNode thenNode, CodeScope& scope, bool branchingMode);

//   void compileStaticAssigning(ObjectInfo target, SNode node, ClassScope& scope/*, int mode*/);
//   void compileClassConstantAssigning(SyntaxWriter& writer, SNode node, CodeScope& scope, ObjectInfo retVal);

   ObjectInfo compileOperation(SyntaxWriter& writer, SNode current, CodeScope& scope, ObjectInfo objectInfo/*, ref_t expectedRef*/, int mode);

   ObjectInfo compileCatchOperator(SyntaxWriter& writer, SNode roperand, CodeScope& scope);
//   void compileAltOperation(SyntaxWriter& writer, SNode node, CodeScope& scope);
//   void compileLoop(SyntaxWriter& writer, SNode node, CodeScope& scope);

   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
   int allocateStructure(SNode node, int& size);
   bool allocateStructure(CodeScope& scope, int size, bool binaryArray, ObjectInfo& exprOperand);

   ObjectInfo compileExternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, /*ref_t expectedRef, */int mode);
   ObjectInfo compileInternalCall(SyntaxWriter& writer, SNode node, CodeScope& scope, ref_t message, ref_t signature, ObjectInfo info);

   void compileConstructorResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
//   void compileConstructorDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileResendExpression(SyntaxWriter& writer, SNode node, CodeScope& scope, bool multiMethod/*, bool extensionMode*/);
   void compileDispatchExpression(SyntaxWriter& writer, SNode node, CodeScope& scope);
   void compileMultidispatch(SyntaxWriter& writer, SNode node, CodeScope& scope, ClassScope& classScope);

   ObjectInfo compileCode(SyntaxWriter& writer, SNode node, CodeScope& scope);

   void declareArgumentAttributes(SNode node, Scope& scope, ref_t& classRef, ref_t& elementRef);
   void declareArgumentList(SNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(SNode node, MethodScope& scope, ref_t& outputRef);
   /*bool*/void declareActionScope(ClassScope& scope, SNode argNode, MethodScope& methodScope, int mode/*, bool alreadyDeclared*/);

////   void declareSingletonClass(SNode node, ClassScope& scope);

   void compileActionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
//   void compileLazyExpressionMethod(SyntaxWriter& writer, SNode member, MethodScope& scope);
   void compileDispatcher(SyntaxWriter& writer, SNode node, MethodScope& scope/*, bool withGenericMethods = false, bool withOpenArgGenerics = false*/);

   void predefineMethod(SNode node, ClassScope& classScope, MethodScope& scope);
   void compileMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileAbstractMethod(SyntaxWriter& writer, SNode node, MethodScope& scope);
   void compileConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope, ClassScope& classClassScope);
//   void compileImplicitConstructor(SyntaxWriter& writer, SNode node, MethodScope& scope);
//
//   void compileSpecialMethodCall(SyntaxWriter& writer, ClassScope& classScope, ref_t message);

   void compileDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);
   //void compileDynamicDefaultConstructor(SyntaxWriter& writer, MethodScope& scope);

   void compilePreloadedCode(SymbolScope& scope);
//   void compilePreloadedCode(_CompilerScope& scope, SNode node);
   void compileSymbolCode(ClassScope& scope);

   void compileAction(SNode node, ClassScope& scope, SNode argNode, int mode/*, bool alreadyDeclared = false*/);
   void compileNestedVMT(SNode node, InlineClassScope& scope);

   void compileVMT(SyntaxWriter& writer, SNode node, ClassScope& scope);
   void compileClassVMT(SyntaxWriter& writer, SNode node, ClassScope& classClassScope, ClassScope& classScope);

   void generateClassField(ClassScope& scope, SNode node, ref_t fieldRef, ref_t elementRef, int sizeHint, bool singleField, bool embeddable);
   void generateClassStaticField(ClassScope& scope, SNode current, ref_t fieldRef, /*ref_t elementRef, */bool isSealed, bool isConst);

   void generateClassFlags(ClassScope& scope, SNode node/*, bool& closureBaseClass*/);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t message, bool allowTypeAttribute);

   void generateMethodDeclaration(SNode current, ClassScope& scope, bool hideDuplicates, bool closed, bool allowTypeAttribute, bool embeddableClass);
   void generateMethodDeclarations(SNode node, ClassScope& scope, bool closed, LexicalType methodType, bool allowTypeAttribute, bool embeddableClass);
//   // classClassType == None for generating a class, classClassType == Normal | Embeddable for a class class
   void generateClassDeclaration(SNode node, ClassScope& scope, ClassType classType, bool nestedDeclarationMode = false);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SyntaxTree& expressionTree, SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SyntaxTree& expressionTree, SNode node, SymbolScope& scope);
   bool compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal/*, bool accumulatorMode = false*/);
   void compileForward(SNode node, NamespaceScope& scope);

////   bool validate(_ProjectManager& project, _Module* module, int reference);

   ObjectInfo assignResult(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t elementRef = 0);

   bool convertObject(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ObjectInfo source);
   bool typecastObject(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ObjectInfo source);
   bool typecast(SyntaxWriter& writer, CodeScope& scope, ref_t targetRef, ref_t signature);

   void compileExternalArguments(SNode node, NamespaceScope& scope);

   ref_t analizeOp(SNode current, NamespaceScope& scope);
   ref_t analizeSymbol(SNode& node, NamespaceScope& scope);
   ref_t analizeAssigning(SNode node, NamespaceScope& scope, int mode);
   ref_t analizeBoxing(SNode node, NamespaceScope& scope, int mode);
   ref_t analizeArgBoxing(SNode node, NamespaceScope& scope, int mode);
//   ref_t analizeArgUnboxing(SNode node, NamespaceScope& scope, int mode);
   ref_t analizeMessageCall(SNode node, NamespaceScope& scope, int mode);
   ref_t analizeExpression(SNode node, NamespaceScope& scope, int mode = 0);
   ref_t analizeInternalCall(SyntaxTree::Node node, NamespaceScope& scope);
   ref_t analizeExtCall(SyntaxTree::Node node, NamespaceScope& scope);
   ref_t analizeNestedExpression(SNode node, NamespaceScope& scope);
   void analizeExpressionTree(SNode node, NamespaceScope& scope, int mode = 0);
   void analizeBranching(SNode node, NamespaceScope& scope, int mode = 0);
   void analizeCode(SNode node, NamespaceScope& scope);
   void analizeMethod(SNode node, NamespaceScope& scope);
   void analizeClassTree(SNode node, ClassScope& scope);
   void analizeSymbolTree(SNode node, Scope& scope);

//   void defineEmbeddableAttributes(ClassScope& scope, SNode node);
//
//   void createPackageInfo(_Module* module, _ProjectManager& project);

   bool compileDeclarations(SNode node, NamespaceScope& scope, bool& repeatMode);
   void compileImplementations(SNode node, NamespaceScope& scope);

////   void generateSyntaxTree(SyntaxWriter& writer, SNode node, ModuleScope& scope, SyntaxTree& autogenerated);

   //void generateListMember(_CompilerScope& scope, ref_t listRef, LexicalType type, ref_t argument);

   void generateClassSymbol(SyntaxWriter& writer, ClassScope& scope);
////   void generateSymbolWithInitialization(SyntaxWriter& writer, ClassScope& scope, ref_t implicitConstructor);

   void declareNamespace(SNode node, NamespaceScope& scope, bool withFullInfo);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   // return true if no forward class declarations are encountered
   bool declareModule(SyntaxTree& tree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*/, bool& repeatMode/*, ExtensionMap* extensionsToExport*/);
   void compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*//*, Unresolveds& unresolveds*/);

////   void compileSyntaxTree(_ProjectManager& project, ident_t file, SyntaxTree& tree, ModuleInfo& moduleInfo, Unresolveds& unresolveds);

   void initializeScope(ident_t name, _ModuleScope& scope, bool withDebugInfo);

////   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

   // _Compiler interface implementation
   //virtual void injectVirtualReturningMethod(SyntaxWriter& writer, ref_t messagRef, LexicalType type, int argument);
   virtual void injectBoxing(SyntaxWriter& writer, _ModuleScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode = false);
   virtual void injectLocalBoxing(SNode node, int size);
   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg, ref_t targetClassRef/*,
      ref_t targetRef*/, int stacksafeAttr);
//   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject);
//   virtual void injectEmbeddableOp(_CompilerScope& scope, SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb);
//////   virtual void injectFieldExpression(SyntaxWriter& writer);
//   virtual void injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t privateRef, ref_t genericMessage);
   virtual void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType);
//   virtual void injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType);
   virtual void injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, ref_t message, ident_t variable, ref_t outputRef);
   virtual void injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument);
//   virtual void injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef);
//   virtual void injectDirectMethodCall(SyntaxWriter& writer, ref_t targetRef, ref_t message);
//   virtual void generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual void generateOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef);
   virtual void generateClosedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);
   virtual void generateSealedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef);

//   //virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
