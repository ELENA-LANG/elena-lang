//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "elena.h"
#include "compilercommon.h" 
#include "bcwriter.h"

namespace _ELENA_
{

// --- ModuleInfo ---
struct ModuleInfo
{
   _Module* codeModule;
   _Module* debugModule;

   ModuleInfo()
   {
      codeModule = debugModule = NULL;
   }

   ModuleInfo(_Module* codeModule, _Module* debugModule)
   {
      this->codeModule = codeModule;
      this->debugModule = debugModule;
   }
};

struct Unresolved
{
   ident_t    fileName;
   ref_t      reference;
   _Module*   module;
   size_t     row;
   size_t     col;           // virtual column

   Unresolved()
   {
      reference = 0;
   }
   Unresolved(ident_t fileName, ref_t reference, _Module* module, size_t row, size_t col)
   {
      this->fileName = fileName;
      this->reference = reference;
      this->module = module;
      this->row = row;
      this->col = col;
   }
};

typedef List<Unresolved> Unresolveds;

// --- Compiler class ---
class Compiler : public _Compiler
{
public:
   struct Parameter
   {
      int    offset;
      ref_t  class_ref;
      ref_t  subj_ref;
      int    size;

      Parameter()
      {
         offset = -1;
         subj_ref = 0;
         class_ref = 0;
         size = 0;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->subj_ref = 0;
         this->class_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = 0;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref, ref_t class_ref)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = class_ref;
         this->size = 0;
      }
      Parameter(int offset, ref_t subj_ref, ref_t class_ref, int size)
      {
         this->offset = offset;
         this->subj_ref = subj_ref;
         this->class_ref = class_ref;
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
      irInvalid,
      irObsolete
   };

   enum ObjectKind
   {
      okUnknown = 0,
   
      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference, extraparam - class reference
      okConstantClass,                // param - reference, extraparam - class reference
      okLiteralConstant,              // param - reference 
      okWideLiteralConstant,          // param - reference 
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference, extraparam - imm argument
      okLongConstant,                 // param - reference 
      okRealConstant,                 // param - reference 
      okMessageConstant,              // param - reference 
      okExtMessageConstant,           // param - reference 
      okSignatureConstant,            // param - reference 
      okVerbConstant,                 // param - reference 
      okArrayConst,
      okField,                        // param - field offset, extraparam - class reference
      okStaticField,                  // param - reference
      okFieldAddress,                 // param - field offset, extraparam - class reference
      okOuter,                        // param - field offset, extraparam - class reference
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okLocal,                        // param - local / out parameter offset, extraparam : class reference
      okParam,                        // param - parameter offset, extraparam = -1 (is stack safe) / 0
      okParamField,
      okSubject,                      // param - parameter offset
      okThisParam,                    // param - parameter offset, extraparam = -1 (stack allocated) / -2 (primitive array)
      okNil,
      okSuper,
      okLocalAddress,                 // param - local offset, extraparam - class reference
      okParams,                       // param - local offset
      okBlockLocal,                   // param - local offset
      okConstantRole,                 // param - role reference

      okExternal,
      okInternal,
   };
   
   struct ObjectInfo
   {
      ObjectKind kind;
      ref_t      param;
      ref_t      extraparam;
      ref_t      type;
   
      ObjectInfo()
      {
         this->kind = okUnknown;
         this->param = 0;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind)
      {
         this->kind = kind;
         this->param = 0;
         this->extraparam = 0;
         this->type = 0;
      }
//      ObjectInfo(ObjectKind kind, ObjectInfo copy)
//      {
//         this->kind = kind;
//         this->param = copy.param;
//         this->extraparam = copy.extraparam;
//         this->type = copy.type;
//      }
      ObjectInfo(ObjectKind kind, ref_t param)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = 0;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->type = 0;
      }
      ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam, ref_t type)
      {
         this->kind = kind;
         this->param = param;
         this->extraparam = extraparam;
         this->type = type;
      }
   };

   typedef Map<ident_t, ref_t>            ForwardMap;
   typedef MemoryMap<ident_t, Parameter>  LocalMap;
//   typedef MemoryMap<int, ref_t>          RoleMap;   
   typedef Map<ref_t, SubjectMap*>        ExtensionMap;

private:
   // - ModuleScope -
   struct ModuleScope : _CompilerScope
   {
      _ProjectManager* project;
      _Module*       debugModule;

      ident_t        sourcePath;
      ref_t          sourcePathRef;

      // default namespaces
      List<ident_t> defaultNs;
      ForwardMap    forwards;       // forward declarations

      // symbol hints
      Map<ref_t, ref_t> constantHints;

      // extensions
      SubjectMap        extensionHints; 
      ExtensionMap      extensions;

      // type hints
      MessageMap        attributes;

      // action hints
      SubjectMap        actionHints;

      ref_t packageReference;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
      int  warningMask;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(SNode identifier);

      ref_t mapReference(ident_t reference, bool existing = false);
      ref_t mapAttribute(ident_t reference, bool existing);

      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      void raiseError(const char* message, int row, int col, ident_t terminal);
      void raiseWarning(int level, const char* message, int row, int col, ident_t terminal);

      void raiseError(const char* message, SNode terminal);
      void raiseWarning(int level, const char* message, SNode terminal);

      bool checkReference(ident_t referenceName);

      ref_t resolveIdentifier(ident_t name);

      ref_t mapNewAttribute(ident_t terminal);

      // NOTE : the function returns 0 for implicit subjects
      // in any case output is set (for explicit one - the namespace is copied as well)
      ref_t mapAttribute(SNode terminal, IdentifierString& output);
      ref_t mapAttribute(SNode terminal, bool explicitOnly = true);
      ref_t resolveAttributeRef(ident_t name, bool explicitOnly = true);

      ref_t mapTerminal(SNode terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      virtual _Module* loadReferenceModule(ref_t& reference);

      ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
      virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
      {
         return loadClassInfo(info, module->resolveReference(reference), headerOnly);
      }
      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

      _Memory* loadAttributeInfo(ref_t reference/*, _Module* &argModule*/)
      {
         return loadAttributeInfo(module->resolveSubject(reference)/*, argModule*/);
      }
      _Memory* loadAttributeInfo(ident_t attribute/*, _Module* &argModule*/);

      void loadAttributes(_Module* module);
      void loadExtensions(_Module* module);
      void loadActions(_Module* module);

      void saveAttribute(ref_t attrRef, ref_t classReference, bool internalType);
      bool saveExtension(ref_t message, ref_t type, ref_t role);
      void saveAction(ref_t message, ref_t reference);

      void validateReference(SNode terminal, ref_t reference);

      ref_t getBaseLazyExpressionClass();

      void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);

      void loadModuleInfo(_Module* extModule)
      {
         loadAttributes(extModule);
         loadExtensions(extModule);
         loadActions(extModule);
      }

      ref_t mapNestedExpression();

      ModuleScope(_ProjectManager* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved);
   };

   // - Scope -
   struct Scope
   {
      enum ScopeLevel
      {
         slClass,
         slSymbol,
         slMethod,
         slCode,
         slOwnerClass,
         slTemplate,
      };

      ModuleScope* moduleScope;
      Scope*       parent;

      void raiseError(const char* message, SNode terminal)
      {
         moduleScope->raiseError(message, terminal);
      }
      void raiseWarning(int level, const char* message, SNode terminal)
      {
         moduleScope->raiseWarning(level, message, terminal);
      }

      ObjectInfo mapObject(SNode terminal)
      {
         ObjectInfo object = mapTerminal(terminal.findChild(lxTerminal).identifier());
         if (object.kind == okUnknown) {
            return moduleScope->mapObject(terminal);
         }
         else return object;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier)
      {
         if (parent) {
            return parent->mapTerminal(identifier);
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

      virtual ref_t mapSubject(SNode terminal, IdentifierString& output)
      {
         if (parent) {
            return parent->mapSubject(terminal, output);
         }
         else return moduleScope->mapAttribute(terminal, output);
      }

      virtual ref_t mapSubject(SNode terminal, bool implicitOnly = true)
      {
         if (parent) {
            return parent->mapSubject(terminal, implicitOnly);
         }
         else return moduleScope->mapAttribute(terminal, implicitOnly);
      }

      virtual int getSourcePathRef()
      {
         if (parent) {
            return parent->getSourcePathRef();
         }
         else return 0;
      }

      Scope(ModuleScope* moduleScope)
      {
         this->parent = NULL;
         this->moduleScope = moduleScope;
      }
      Scope(Scope* parent)
      {
         this->parent = parent;
         this->moduleScope = parent->moduleScope;
      }
   };

   // - SourceScope -
   struct SourceScope : public Scope
   {
      CommandTape    tape;
      ref_t          reference;

      SourceScope(ModuleScope* parent, ref_t reference);
   };

   // - ClassScope -
   struct ClassScope : public SourceScope
   {
      ClassInfo   info;
      ref_t       extensionMode;

      virtual ObjectInfo mapTerminal(ident_t identifier);

      void compileClassAttribute(SyntaxTree::Node hint);

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
         MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false), 0);
         metaWriter.Memory()->trim(0);
         info.save(&metaWriter);
      }

      bool include(ref_t message)
      {
         // check if the method is inhreited and update vmt size accordingly
         ClassInfo::MethodMap::Iterator it = info.methods.getIt(message);
         if (it.Eof()) {
            info.methods.add(message, true);
         
            return true;
         }
         else {
            (*it) = true;
         
            return false;
         }
      }

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      bool  constant;
//      bool  preloaded;
      ref_t typeRef;      

      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slSymbol) {
            return this;
         }
         else return Scope::getScope(level);
      }

      SymbolScope(ModuleScope* parent, ref_t reference);
   };

   // - MethodScope -
   struct MethodScope : public Scope
   {
      ref_t        message;
      LocalMap     parameters;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      bool         withOpenArg;
      bool         stackSafe;
      bool         classEmbeddable;
      bool         generic;
//      bool         sealed;

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getReturningType(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope->info.methodHints.get(ClassInfo::Attribute(message, maType));
      }

      ref_t getClassFlags(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.flags : 0;
      }
      ref_t getClassRef(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->reference : 0;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      MethodScope(ClassScope* parent);
   };

   // - ActionScope -
   struct ActionScope : public MethodScope
   {
      ActionScope(ClassScope* parent);

      virtual ObjectInfo mapTerminal(ident_t identifier);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      // scope local variables
      LocalMap     locals;
      int          level;

      // scope stack allocation
      int          reserved;  // allocated for the current statement
      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

      void mapLocal(ident_t local, int level/*, ref_t type*/)
      {
         locals.add(local, Parameter(level/*, type*/));
      }
      void mapLocal(ident_t local, int level, ref_t type, size_t class_ref, int size)
      {
         locals.add(local, Parameter(level, type, class_ref, size));
      }

      void freeSpace()
      {
         reserved = saved;
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

      int getMessageID()
      {
         MethodScope* scope = (MethodScope*)getScope(slMethod);

         return scope ? scope->message : 0;
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

      CodeScope(SymbolScope* parent);
      CodeScope(MethodScope* parent);
      CodeScope(CodeScope* parent);
   };

   // - InlineClassScope -
   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         int        reference;
         bool       preserved;
         ObjectInfo outerObject;

         Outer()
         {
            reference = -1;
            preserved = false;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
            this->preserved = false;
         }
      };

      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();

      bool markAsPresaved(ObjectInfo object);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapTerminal(ident_t identifier);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   // --- TemplateScope ---
   struct TemplateScope : ClassScope
   {
      ref_t       templateRef;
      ForwardMap  parameters;
      SubjectMap  subjects;
      bool        classMode;
      int         sourceRef;

      virtual ref_t mapSubject(SNode terminal, IdentifierString& output)
      {
         ident_t name = terminal.findChild(lxTerminal).identifier();
         ref_t parameter = parameters.get(name);
         if (parameter != 0) {
            ref_t subjRef = subjects.get(parameter);
            output.append(moduleScope->module->resolveSubject(subjRef));

            return subjRef;
         }
         else return Scope::mapSubject(terminal, output);
      }

      virtual ref_t mapSubject(SNode terminal, bool implicitOnly = true)
      {
         ident_t identifier = terminal.findChild(lxTerminal).identifier();

         ref_t parameter = parameters.get(identifier);
         if (parameter != 0) {
            return subjects.get(parameter);
         }
         else return Scope::mapSubject(terminal, implicitOnly);
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slTemplate) {
            return this;
         }
         else if (level == slClass && classMode) {
            return this;
         }
         else return parent->getScope(level);
      }

      virtual int getSourcePathRef()
      {
         return sourceRef;
      }

      void loadParameters(SNode node);

      void generateClassName(bool newName = false);

      TemplateScope(ClassScope* parent)
         : ClassScope(parent->moduleScope, parent->reference)
      {
         this->templateRef = 0;
         this->parent = parent;
         this->info.header.flags = 0;
         this->classMode = false;
         this->sourceRef = -1;
      }
      TemplateScope(Scope* parent, ref_t attrRef)
         : ClassScope(parent->moduleScope, 0)
      {
         this->parent = parent;
         this->templateRef = attrRef;
         this->classMode = false;
         this->sourceRef = -1;
      }
      TemplateScope(ModuleScope* moduleScope, ref_t attrRef)
         : ClassScope(moduleScope, 0)
      {
         this->parent = NULL;
         this->templateRef = attrRef;
         this->classMode = false;
         this->sourceRef = -1;
      }
   };

   struct WarningScope
   {
      ident_t terminal;

      int warningMask;
      int col;
      int row;

      void raise(ModuleScope& scope, int level, ident_t message, SNode node)
      {
         if (test(warningMask, level)) {
            if (col != 0) {
               scope.raiseWarning(level, message, row, col, terminal);
            }
            else if(node != lxNone)
               scope.raiseWarning(level, message, node);
         }            
      }

      WarningScope(int mask)
      {
         warningMask = mask;
         col = row = 0;
         terminal = NULL;
      }
      WarningScope()
      {
         warningMask = 0;
         col = row = 0;
         terminal = NULL;
      }
   };

   _CompilerLogic*  _logic;

   ByteCodeWriter _writer;

   MessageMap     _verbs;                            // list of verbs
   MessageMap     _operators;                        // list of operators

   int            _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeIdleBreakpoints(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);
   
   void insertDebugStep(SNode& node, int stepType)
   {
      node.insertNode(lxBreakpoint, stepType);
   }
   void appendDebugStep(SNode& node, int stepType)
   {
      node.appendNode(lxBreakpoint, stepType);
   }
   void setDebugStep(SNode& node, int stepType)
   {
      node.set(lxBreakpoint, stepType);
   }

//   void raiseWarning(ModuleScope& scope, SNode node, ident_t message, int warningLevel, int warningMask, bool triggered = true);
//
//   void appendObjectInfo(CodeScope& scope, ObjectInfo object);
   void insertMessage(SNode node, ModuleScope& scope, ref_t messageRef);
   ref_t mapAttribute(SNode attribute, Scope& scope, int& attrValue);
   ref_t mapAttribute(SNode attribute, ModuleScope& scope);
   void initialize(Scope& scope, MethodScope& methodScope);

   int checkMethod(ModuleScope& scope, ref_t reference, ref_t message)
   {
      bool dummy1 = false;
      ref_t dummy2 = 0;

      return _logic->checkMethod(scope, reference, message, dummy1, dummy2);
   }

   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);

   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(SNode node, ModuleScope& scope, ident_t reference, ref_t message);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

   void declareParameterDebugInfo(SNode node, MethodScope& scope, bool withThis, bool withSelf);

   bool copyTemplate(SNode node, Scope& scope, ref_t attrRef, SNode attributeNode);

   void compileParentDeclaration(SNode baseNode, ClassScope& scope, ref_t parentRef, bool ignoreSealed = false);
   void compileParentDeclaration(SNode node, ClassScope& scope);
   void compileFieldDeclarations(SNode member, ClassScope& scope); 

   void compileSymbolAttributes(SNode node, SymbolScope& scope, SNode rootNode);
   void compileSymbolAttributes(SNode node, SymbolScope& scope)
   {
      compileSymbolAttributes(node, scope, node);
   }
   void compileClassAttributes(SNode node, ClassScope& scope, SNode rootNode);
   void compileLocalAttributes(SNode hints, CodeScope& scope, ObjectInfo& variable, int& size);
   void compileFieldAttributes(SNode hints, ClassScope& scope, SNode rootNode);
   void compileMethodAttributes(SNode hints, MethodScope& scope, SNode rootNode);
   void declareVMT(SNode member, ClassScope& scope);
   void declareTemplateMethods(SNode node, ClassScope& scope);

   ref_t mapMessage(SNode node, CodeScope& scope, size_t& count/*, bool& argsUnboxing*/);

   void compileSwitch(SNode node, CodeScope& scope);
   void compileVariable(SNode node, CodeScope& scope);

   ObjectInfo compileClosure(SNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileClosure(SNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
   ObjectInfo compileCollection(SNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileCollection(SNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);

   ObjectInfo compileMessageReference(SNode objectNode, CodeScope& scope, int mode);
   void setTerminal(SNode& terminal, CodeScope& scope, ObjectInfo object, int mode);

   ObjectInfo compileTerminal(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(SNode objectNode, CodeScope& scope, int mode);

   ObjectInfo compileOperator(SNode node, CodeScope& scope, int mode, int operator_id);
   ObjectInfo compileOperator(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileBranchingOperator(SNode& node, CodeScope& scope, int mode, int operator_id);

   ObjectInfo compileMessageParameters(SNode node, CodeScope& scope);   // returns an info of the first operand

   ObjectInfo compileMessage(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileMessage(SNode node, CodeScope& scope, ObjectInfo target, int messageRef, int mode);
   ObjectInfo compileExtensionMessage(SNode node, CodeScope& scope, ObjectInfo role/*, int mode*/);

   ObjectInfo compileNewOperator(SNode node, CodeScope& scope/*, int mode*/);
   ObjectInfo compileAssigning(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileExtension(SNode node, CodeScope& scope, int mode = 0);
   ObjectInfo compileExpression(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileRetExpression(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigningExpression(SNode assigning, CodeScope& scope, int mode = 0);

   ObjectInfo compileBranching(SNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);

   void compileTrying(SNode node, CodeScope& scope);
   void compileAltOperation(SNode node, CodeScope& scope);
   void compileLoop(SNode node, CodeScope& scope);
   void compileThrow(SNode node, CodeScope& scope, int mode);
//   void compileTry(DNode node, CodeScope& scope);
//   void compileLock(DNode node, CodeScope& scope);

   void compileExternalArguments(SNode node, CodeScope& scope/*, ExternalScope& externalScope*/);

   int allocateStructure(bool bytearray, int& allocatedSize, int& reserved);
   int allocateStructure(SNode node, int& size);
   bool allocateStructure(CodeScope& scope, int size, bool bytearray, ObjectInfo& exprOperand);

   ObjectInfo compileExternalCall(SNode node, CodeScope& scope, int mode);
   ObjectInfo compileInternalCall(SNode node, CodeScope& scope, ref_t message, ObjectInfo info);

   void compileConstructorResendExpression(SNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
   void compileConstructorDispatchExpression(SNode node, CodeScope& scope);
   void compileResendExpression(SNode node, CodeScope& scope);
   void compileDispatchExpression(SNode node, CodeScope& scope);

   ObjectInfo compileCode(SNode node, CodeScope& scope);

   void declareArgumentList(SNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(SNode node, MethodScope& scope);
   bool declareActionScope(SNode& node, ClassScope& scope, SNode argNode, ActionScope& methodScope, int mode, bool alreadyDeclared);

   void declareSingletonClass(SNode node, ClassScope& scope, SNode hints);
   void compileSingletonClass(SNode member, ClassScope& scope, SNode hints);

   void declareSingletonAction(ClassScope& scope, SNode objNode);

   void compileActionMethod(SNode member, MethodScope& scope);
   void compileLazyExpressionMethod(SNode member, MethodScope& scope);
   void compileDispatcher(SNode node, MethodScope& scope, bool withGenericMethods = false);

   void compileMethod(SNode node, MethodScope& scope);
   void compileDefaultConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
   void compileDynamicDefaultConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope);
   void compileConstructor(SNode node, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef = 0);
//   void compileEmbeddableConstructor(DNode node, SyntaxWriter& writer, MethodScope& scope, ClassScope& classClassScope);

//   void compilePreloadedCode(SymbolScope& scope);
   void compileSymbolCode(ClassScope& scope);
//   void compileVirtualDispatchMethod(SyntaxWriter& writer, MethodScope& scope, LexicalType target, int argument = 0);

   void compileAction(SNode node, ClassScope& scope, SNode argNode, int mode, bool alreadyDeclared = false);
   void compileNestedVMT(SNode node, InlineClassScope& scope);

   void compileVMT(SNode node, ClassScope& scope);
   void compileTemplateMethods(SNode node, ClassScope& scope);

//   void declareVirtualMethods(ClassScope& scope);

   ref_t generateTemplate(SNode attribute, TemplateScope& scope);

   void generateClassField(ClassScope& scope, SyntaxTree::Node node, bool singleField);
   void generateClassStaticField(ClassScope& scope, SNode current);   

   void generateClassFlags(ClassScope& scope, SyntaxTree::Node root);
   void generateClassFields(ClassScope& scope, SyntaxTree::Node root, bool singleField);
   void generateMethodAttributes(ClassScope& scope, SyntaxTree::Node node, ref_t& message);
   void generateMethodDeclarations(ClassScope& scope, SNode node, bool hideDuplicates, bool closed);
   void generateClassDeclaration(SNode node, ClassScope& scope, bool closed);

   void generateClassImplementation(SNode node, ClassScope& scope);

   void compileClassDeclaration(SNode node, ClassScope& scope);
   void compileClassImplementation(SNode node, ClassScope& scope);
   void compileClassClassDeclaration(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(SNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(SNode node, SymbolScope& scope);
   void compileSymbolImplementation(SNode node, SymbolScope& scope);
   bool compileSymbolConstant(SNode node, SymbolScope& scope, ObjectInfo retVal);
   void compileIncludeModule(SNode node, ModuleScope& scope);
   void declareSubject(SNode member, ModuleScope& scope);
   void compileSubject(SNode member, ModuleScope& scope);

   void compileDeclarations(SNode member, ModuleScope& scope);
   void compileImplementations(SNode member, ModuleScope& scope);
   void compileIncludeSection(SNode& node, ModuleScope& scope);

   bool validate(_ProjectManager& project, _Module* module, int reference);

   ObjectInfo typecastObject(SNode node, CodeScope& scope, ref_t subjectRef, ObjectInfo object);
   ObjectInfo assignResult(CodeScope& scope, SNode& node, ref_t targetRef, ref_t targetType = 0);

   bool convertObject(SNode node, CodeScope& scope, ref_t targetRef, ObjectInfo source);

   void optimizeAssigning(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
   void optimizeExtCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope, int mode);
   void optimizeInternalCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope, int mode);
   void optimizeCall(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope);
   void optimizeOp(ModuleScope& scope, SyntaxTree::Node node, WarningScope& warningScope, int mode);
//   void optimizeNewOp(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);

   void optimizeBoxing(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode);
//   void optimizeTypecast(ModuleScope& scope, SyntaxTree::Node node, int warningLevel, int mode);
   void optimizeArgUnboxing(ModuleScope& scope, SNode node, WarningScope& warningScope);
   void optimizeNestedExpression(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode);
   void optimizeSyntaxNode(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode);
   void optimizeSyntaxExpression(ModuleScope& scope, SNode node, WarningScope& warningScope, int mode = 0);
   void optimizeClassTree(SNode node, ClassScope& scope, WarningScope& warningScope);
   void optimizeSymbolTree(SNode node, SourceScope& scope, int warningMask);

   void defineEmbeddableAttributes(ClassScope& scope, SyntaxTree::Node node);

   void createPackageInfo(_Module* module, _ProjectManager& project);

   void compileModule(SNode node, ModuleScope& scope);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   void compileModule(_ProjectManager& project, ident_t file, SNode node, ModuleInfo& moduleInfo, Unresolveds& unresolveds);

   ModuleInfo createModule(ident_t name, _ProjectManager& project, bool withDebugInfo);

   void validateUnresolved(Unresolveds& unresolveds, _ProjectManager& project);

   // _Compiler interface implementation
   virtual void injectVirtualReturningMethod(SNode node, ident_t variable);
   virtual void injectBoxing(_CompilerScope& scope, SNode node, LexicalType boxingType, int argument, ref_t targetClassRef);
   virtual void injectLocalBoxing(SNode node, int size);
   virtual void injectConverting(SNode node, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef);
   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject);
   virtual void generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef);
   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader);

   Compiler(_CompilerLogic* logic);
};

} // _ELENA_

#endif // compilerH
