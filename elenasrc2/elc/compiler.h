//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "project.h"
#include "parser.h"
#include "bcwriter.h"

namespace _ELENA_
{

// --- Compiler class ---
class Compiler
{
public:
   struct Parameter
   {
      int        offset;
      bool       stackAllocated;
      union {
         ref_t   sign_ref;   // if not stack allocated - contains type reference
         ref_t   class_ref;  // if stack allocated - contains class reference
      };

      Parameter()
      {
         offset = -1;
         sign_ref = 0;
         stackAllocated = false;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->sign_ref = 0;
         stackAllocated = false;
      }
      Parameter(int offset, ref_t sign_ref)
      {
         this->offset = offset;
         this->sign_ref = sign_ref;
         stackAllocated = false;
      }
      Parameter(int offset, ref_t ref, bool stackAllocated)
      {
         this->offset = offset;
         this->stackAllocated = stackAllocated;
         if (stackAllocated) {
            this->class_ref = ref;
         }
         else this->sign_ref = ref;
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

   enum MethodHint
   {
      tpMask       = 0x0F,

      tpUnknown    = 0x00,
      tpSealed     = 0x01,
      tpClosed     = 0x02,
      tpNormal     = 0x03,
      tpDispatcher = 0x04,
      tpStackSafe  = 0x10,
      tpEmbeddable = 0x20,
      tpGeneric    = 0x40,
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

   enum ObjectKind
   {
      okUnknown = 0,
   
      okObject,                       // param - class reference
      okSymbol,                       // param - reference
      okConstantSymbol,               // param - reference, extraparam - class reference
      okConstantClass,                // param - reference, extraparam - class reference
      okLiteralConstant,              // param - reference 
      okCharConstant,                 // param - reference
      okIntConstant,                  // param - reference 
      okLongConstant,                 // param - reference 
      okRealConstant,                 // param - reference 
      okMessageConstant,              // param - reference 
      okSignatureConstant,            // param - reference 
      okVerbConstant,                 // param - reference 
      okField,                        // param - field offset
      okFieldAddress,                 // param - field offset
      okOuter,                        // param - field offset
      okOuterField,                   // param - field offset, extraparam - outer field offset
      okLocal,                        // param - local / out parameter offset, extraparam : -1 indicates boxable / class reference for constructor call
      okParam,                        // param - parameter offset
      okSubject,                      // param - parameter offset
      okSubjectDispatcher,
      okThisParam,                    // param - parameter offset
      okNil,
      okSuper,
      okLocalAddress,                  // param - local offset, extraparam - class reference
      okParams,                        // param - local offset
      okBlockLocal,                    // param - local offset
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
      ObjectInfo(ObjectKind kind, ObjectInfo copy)
      {
         this->kind = kind;
         this->param = copy.param;
         this->extraparam = copy.extraparam;
         this->type = copy.type;
      }
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

   typedef Map<ident_t, ref_t, false>     ForwardMap;
   typedef Map<ident_t, Parameter, false> LocalMap;
   typedef Map<ref_t, ref_t>              SubjectMap;
   typedef List<Unresolved>               Unresolveds;
   typedef Map<ref_t, SubjectMap*>        ExtensionMap;

private:
   // - ModuleScope -
   struct ModuleScope
   {
      Project*       project;
      _Module*       module;
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
      ForwardMap        types;
      SubjectMap        typeHints;

      // cached references
      ref_t superReference;
      ref_t intReference;
      ref_t longReference;
      ref_t realReference;
      ref_t literalReference;
      ref_t charReference;
      ref_t trueReference;
      ref_t falseReference;
      ref_t paramsReference;
      ref_t signatureReference;
      ref_t arrayReference;

      ref_t boolType;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(TerminalInfo identifier);

      ref_t mapReference(ident_t reference, bool existing = false);

      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

      void defineConstantSymbol(ref_t reference, ref_t classReference)
      {
         constantHints.add(reference, classReference);
      }

      void raiseError(const char* message, TerminalInfo terminal);
      void raiseWarning(const char* message, TerminalInfo terminal);
      void raiseWarning(const char* message, int row, int col);

      bool checkReference(ident_t referenceName);

      ref_t resolveIdentifier(ident_t name);

      ref_t mapNewType(ident_t terminal);

      ref_t mapType(TerminalInfo terminal);

      ref_t mapSubject(TerminalInfo terminal, IdentifierString& output, bool strongOnly = false);
      ref_t mapSubject(ident_t name)
      {
         IdentifierString wsName(name);
         return module->mapSubject(wsName, false);
      }

      ref_t mapTerminal(TerminalInfo terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
      ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false)
      {
         return loadClassInfo(info, module->resolveReference(reference), headerOnly);
      }
      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

      int defineStructSize(ref_t classReference, bool& variable);
      int defineStructSize(ref_t classReference)
      {
         bool dummy = false;

         return defineStructSize(classReference, dummy);
      }

      int defineTypeSize(ref_t type_ref, ref_t& class_ref, bool& variable);
      int defineTypeSize(ref_t type_ref)
      {
         ref_t dummy1;
         bool dummy2;

         return defineTypeSize(type_ref, dummy1, dummy2);
      }
      int defineTypeSize(ref_t type_ref, ref_t& class_ref)
      {
         bool dummy2;

         return defineTypeSize(type_ref, class_ref, dummy2);
      }

      int checkMethod(ref_t reference, ref_t message, bool& found, ref_t& outputType);
      int checkMethod(ref_t reference, ref_t message)
      {
         bool dummy;
         ref_t dummyRef;
         return checkMethod(reference, message, dummy, dummyRef);
      }

      void loadTypes(_Module* module);
      void loadExtensions(TerminalInfo terminal, _Module* module);

      void saveType(ref_t type_ref, ref_t classReference, bool internalType);
      bool saveExtension(ref_t message, ref_t type, ref_t role);

      void validateReference(TerminalInfo terminal, ref_t reference);

      ref_t getBaseFunctionClass(int paramCount);
      ref_t getBaseIndexFunctionClass(int paramCount);
      ref_t getBaseLazyExpressionClass();

      int getClassFlags(ref_t reference);

      bool checkIfCompatible(ref_t typeRef, ref_t classRef);

      void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly);

      ModuleScope(Project* project, ident_t sourcePath, _Module* module, _Module* debugModule, Unresolveds* forwardsUnresolved);
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
         slOwnerClass
      };

      ModuleScope* moduleScope;
      Scope*       parent;
      int          warningMask;

      void raiseError(const char* message, TerminalInfo terminal)
      {
         moduleScope->raiseError(message, terminal);
      }

      void raiseWarning(int level, const char* message, TerminalInfo terminal)
      {
         if (test(warningMask, level))
            moduleScope->raiseWarning(message, terminal);
      }
      void raiseWarning(int level, const char* message, int row, int col)
      {
         if (test(warningMask, level))
            moduleScope->raiseWarning(message, row, col);
      }

      virtual ObjectInfo mapObject(TerminalInfo identifier)
      {
         if (parent) {
            return parent->mapObject(identifier);
         }
         else return moduleScope->mapObject(identifier);
      }

      virtual Scope* getScope(ScopeLevel level)
      {
         if (parent) {
            return parent->getScope(level);
         }
         else return NULL;
      }

      Scope(ModuleScope* moduleScope)
      {
         this->parent = NULL;
         this->moduleScope = moduleScope;
         this->warningMask = moduleScope->project->getWarningMask();
      }
      Scope(Scope* parent)
      {
         this->parent = parent;
         this->moduleScope = parent->moduleScope;
         this->warningMask = parent->warningMask;
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
      ClassInfo info;

      virtual ObjectInfo mapObject(TerminalInfo identifier);

      void compileClassHints(DNode hints);
      void compileFieldHints(DNode hints, int& size, ref_t& type);

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

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      MemoryDump syntaxTree;

      bool  constant;
      ref_t typeRef;

      void compileHints(DNode hints);

      virtual ObjectInfo mapObject(TerminalInfo identifier);

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
      MemoryDump   syntaxTree;
      CommandTape* tape;

      ref_t        message;
      LocalMap     parameters;
      int          reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int          rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      bool         withOpenArg;
      bool         stackSafe;

      void compileHints(DNode hints);
      void compileWarningHints(DNode hints);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      void setClassFlag(int flag)
      {
         ((ClassScope*)parent)->info.header.flags = ((ClassScope*)parent)->info.header.flags | flag;
      }

      int getClassFlag()
      {
         return ((ClassScope*)parent)->info.header.flags;
      }
      
      ref_t getReturningType() const
      {
         return ((ClassScope*)parent)->info.methodHints.get(ClassInfo::Attribute(message, maType));
      }

      bool isSealed() const
      {
         int hint = ((ClassScope*)parent)->info.methodHints.get(ClassInfo::Attribute(message, maHint));

         return (hint & tpMask) == tpSealed;
      }

      bool isEmbeddable() const
      {
         int hint = ((ClassScope*)parent)->info.methodHints.get(ClassInfo::Attribute(message, maHint));

         return test(hint, tpEmbeddable);
      }

      bool isGeneric() const
      {
         int hint = ((ClassScope*)parent)->info.methodHints.get(ClassInfo::Attribute(message, maHint));

         return test(hint, tpGeneric);
      }

      bool include();

      virtual ObjectInfo mapObject(TerminalInfo identifier);

      MethodScope(ClassScope* parent);
   };

   // - ActionScope -
   struct ActionScope : public MethodScope
   {
      virtual ObjectInfo mapObject(TerminalInfo identifier);

      ActionScope(ClassScope* parent);
   };

   // - CodeScope -
   struct CodeScope : public Scope
   {
      SyntaxWriter* writer;
      int           rootBookmark;

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

      void mapLocal(ident_t local, int level, ref_t type)
      {
         locals.add(local, Parameter(level, type));
      }

      void mapLocal(ident_t local, int level, ref_t ref, bool stackAllocated)
      {
         locals.add(local, Parameter(level, ref, stackAllocated));
      }

      int newSpace(size_t size)
      {
         int retVal = reserved;

         reserved += size;

         // the offset should include frame header offset
         return -2 - retVal;
      }

      void freeSpace()
      {
         reserved = saved;
      }

      virtual ObjectInfo mapObject(TerminalInfo identifier);

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

      ref_t getExtensionType()
      {
         ClassScope* scope = (ClassScope*)getScope(slClass);

         return scope ? scope->info.extensionTypeRef : 0;
      }

      void compileLocalHints(DNode hints, ref_t& type, int& size, ref_t& classReference);

      CodeScope(SymbolScope* parent, SyntaxWriter* writer);
      CodeScope(MethodScope* parent, SyntaxWriter* writer);
      CodeScope(CodeScope* parent);
   };

   // - InlineClassScope -

   struct InlineClassScope : public ClassScope
   {
      struct Outer
      {
         int        reference;
         ObjectInfo outerObject;

         Outer()
         {
            reference = -1;
         }
         Outer(int reference, ObjectInfo outerObject)
         {
            this->reference = reference;
            this->outerObject = outerObject;
         }
      };

      Map<ident_t, Outer>     outers;
      ClassInfo::FieldTypeMap outerFieldTypes;

      Outer mapSelf();

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

      virtual ObjectInfo mapObject(TerminalInfo identifier);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   ByteCodeWriter _writer;
   Parser         _parser;

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
   
   void recordDebugStep(CodeScope& scope, TerminalInfo terminal, int stepType)
   {
      if (terminal != nsNone) {
         scope.writer->newNode(lxBreakpoint, stepType);
         scope.writer->appendNode(lxRow, terminal.row);
         scope.writer->appendNode(lxCol, terminal.disp);
         scope.writer->appendNode(lxLength, terminal.length);
         scope.writer->closeNode();
      }
   }
   void recordDebugVirtualStep(CodeScope& scope, int stepType)
   {
      scope.writer->newNode(lxBreakpoint, stepType);
      scope.writer->closeNode();
   }

   void appendObjectInfo(CodeScope& scope, ObjectInfo object);

   bool checkIfCompatible(CodeScope& scope, ref_t typeRef, ObjectInfo object);
   ref_t resolveObjectReference(CodeScope& scope, ObjectInfo object);

   ref_t mapNestedExpression(CodeScope& scope);
   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(DNode node, ModuleScope& scope, CommandTape* tape, ident_t reference);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

   void declareParameterDebugInfo(MethodScope& scope, bool withThis, bool withSelf);

   void compileParentDeclaration(DNode node, ClassScope& scope);
   InheritResult compileParentDeclaration(ref_t parentRef, ClassScope& scope, bool ignoreSealed = false);

   bool writeBoxing(TerminalInfo terminal, CodeScope& scope, ObjectInfo& object, ref_t targetTypeRef, int mode);

   ref_t mapMessage(DNode node, CodeScope& scope, size_t& count, bool& argsUnboxing);
   ref_t mapMessage(DNode node, CodeScope& scope, size_t& count)
   {
      bool dummy = false;
      return mapMessage(node, scope, count, dummy);
   }

   void compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue);
   void compileVariable(DNode node, CodeScope& scope, DNode hints);

   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);

   ObjectInfo compileMessageReference(DNode objectNode, CodeScope& scope);
   void writeTerminal(TerminalInfo terminal, CodeScope& scope, ObjectInfo object);

   ObjectInfo compileTerminal(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(DNode objectNode, CodeScope& scope, int mode);

   int mapOperandType(CodeScope& scope, ObjectInfo operand);
   int mapVarOperandType(CodeScope& scope, ObjectInfo operand);

   ObjectInfo compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id);
   ObjectInfo compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id);

   ref_t compileMessageParameters(DNode node, CodeScope& scope/*, bool stacksafe*/);

   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object);
   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode);
   ObjectInfo compileExtensionMessage(DNode node, CodeScope& scope, ObjectInfo object, ObjectInfo role/*, int mode*/);

   ObjectInfo compileOperations(DNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExpression(DNode node, CodeScope& scope, ref_t targetType, int mode);
   ObjectInfo compileRetExpression(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigningExpression(DNode node, DNode assigning, CodeScope& scope, ObjectInfo target, int mode = 0);

   ObjectInfo compileBranching(DNode thenNode, CodeScope& scope/*, ObjectInfo target, int verb, int subCodinteMode*/);

   void compileLoop(DNode node, CodeScope& scope);
   void compileThrow(DNode node, CodeScope& scope, int mode);
   void compileTry(DNode node, CodeScope& scope);
   void compileLock(DNode node, CodeScope& scope);

   void compileExternalArguments(DNode node, CodeScope& scope/*, ExternalScope& externalScope*/);

   void reserveSpace(CodeScope& scope, int size);
   bool allocateStructure(CodeScope& scope, int mode, ObjectInfo& exprOperand/*, bool presavedAccumulator = false*/);

   ObjectInfo compileExternalCall(DNode node, CodeScope& scope, ident_t dllName, int mode);
   ObjectInfo compileInternalCall(DNode node, CodeScope& scope, ObjectInfo info);

   void compileConstructorResendExpression(DNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
   void compileConstructorDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape);
   void compileResendExpression(DNode node, CodeScope& scope, CommandTape* tape);
   void compileDispatchExpression(DNode node, CodeScope& scope, CommandTape* tape);

   ObjectInfo compileCode(DNode node, CodeScope& scope);

   void declareArgumentList(DNode node, MethodScope& scope, DNode hints);
   ref_t declareInlineArgumentList(DNode node, MethodScope& scope);
   bool declareActionScope(DNode& node, ClassScope& scope, DNode argNode, ActionScope& methodScope, bool alreadyDeclared);
   void declareVMT(DNode member, ClassScope& scope, Symbol methodSymbol, bool closed);

   void declareSingletonClass(DNode member, ClassScope& scope, bool closed);
   void compileSingletonClass(DNode member, ClassScope& scope);

   void declareSingletonAction(ClassScope& scope, ActionScope& methodScope);

   void compileImportCode(DNode node, CodeScope& scope, ref_t message, ident_t function, CommandTape* tape);

   void compileActionMethod(DNode member, MethodScope& scope);
   void compileLazyExpressionMethod(DNode member, MethodScope& scope);
   void compileDispatcher(DNode node, MethodScope& scope, bool withGenericMethods = false);
   void compileMethod(DNode node, MethodScope& scope, bool genericMethod);
   void compileDefaultConstructor(MethodScope& scope, ClassScope& classClassScope);
   void compileDynamicDefaultConstructor(MethodScope& scope, ClassScope& classClassScope);
   void compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, ref_t embeddedMethodRef = 0);
   void compileEmbeddableConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope);

   void compileSymbolCode(ClassScope& scope);

   void compileAction(DNode node, ClassScope& scope, DNode argNode, bool alreadyDeclared = false);
   void compileNestedVMT(DNode node, InlineClassScope& scope);

   void compileVMT(DNode member, ClassScope& scope);

   void compileFieldDeclarations(DNode& member, ClassScope& scope);
   void compileClassDeclaration(DNode node, ClassScope& scope, DNode hints);
   void compileClassImplementation(DNode node, ClassScope& scope);
   void compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints);
   void compileSymbolImplementation(DNode node, SymbolScope& scope, DNode hints, bool isStatic);
   void compileIncludeModule(DNode node, ModuleScope& scope/*, DNode hints*/);
   void compileType(DNode& member, ModuleScope& scope, DNode hints);

   void compileDeclarations(DNode member, ModuleScope& scope);
   void compileImplementations(DNode member, ModuleScope& scope);
   void compileIncludeSection(DNode& node, ModuleScope& scope);

   virtual void compileModule(DNode node, ModuleScope& scope);

   void compile(ident_t source, MemoryDump* buffer, ModuleScope& scope);

   bool validate(Project& project, _Module* module, int reference);
   void validateUnresolved(Unresolveds& unresolveds, Project& project);

   void optimizeAssigning(ModuleScope& scope, SyntaxTree::Node node);   
   void optimizeExtCall(ModuleScope& scope, SyntaxTree::Node node);
   void optimizeInternalCall(ModuleScope& scope, SyntaxTree::Node node);
   void optimizeDirectCall(ModuleScope& scope, SyntaxTree::Node node);
   void optimizeEmbeddableCall(ModuleScope& scope, SyntaxTree::Node& assignNode, SyntaxTree::Node& callNode);
   void optimizeOp(ModuleScope& scope, SyntaxTree::Node node);

   void analizeBoxing(Scope* scope, SyntaxTree::Node node);
   void analizeTypecast(Scope* scope, SyntaxTree::Node node);
   void analizeSyntaxExpression(Scope* scope, SyntaxTree::Node node);
   void analizeSyntaxTree(Scope* scope, MemoryDump& dump);

   bool recognizeEmbeddableGet(MethodScope& scope, SyntaxTree& tree, SyntaxTree::Node node, ref_t& subject);
   void defineEmbeddableAttributes(MethodScope& scope, MemoryDump& dump);

public:
   void loadRules(StreamReader* optimization);
   void turnOnOptimiation(int level)
   {
      _optFlag |= level;
   }

   bool run(Project& project);

   Compiler(StreamReader* syntax);
};

} // _ELENA_

#endif // compilerH
