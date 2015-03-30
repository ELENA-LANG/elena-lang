//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class.
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerH
#define compilerH

#include "project.h"
#include "parser.h"
#include "bcwriter.h"

namespace _ELENA_
{

//// Compiler optimization flags
////const int optDirectConstant = 0x00000001;
////const int optJumps          = 0x00000002;

// --- Compiler class ---
class Compiler
{
protected:
   struct Parameter
   {
      int        offset;
      ref_t      sign_ref;
      bool       output;

      Parameter()
      {
         offset = -1;
         sign_ref = 0;
         output = false;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->sign_ref = 0;
         this->output = false;
      }
      Parameter(int offset, ref_t sign_ref)
      {
         this->offset = offset;
         this->sign_ref = sign_ref;
         this->output = false;
      }
      Parameter(int offset, ref_t sign_ref, bool output)
      {
         this->offset = offset;
         this->sign_ref = sign_ref;
         this->output = output;
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

   enum MethodType
   {
      tpUnknown = 0,
      tpSealed  = 1,
      tpClosed  = 2,
      tpNormal  = 3
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

   typedef Map<ident_t, ref_t, false>     ForwardMap;
   typedef Map<ident_t, Parameter, false> LocalMap;
   typedef Map<ref_t, ref_t>              SubjectMap;
   typedef List<Unresolved>               Unresolveds;
   typedef Map<ref_t, SubjectMap*>        ExtensionMap;

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
      Map<ref_t, ObjectKind> symbolHints;

      // extensions
      SubjectMap             extensionHints; 
      ExtensionMap           extensions;

      // type hints
      ForwardMap             types;
      SubjectMap             typeHints;

      // cached references
      ref_t superReference;
      ref_t nilReference;
      ref_t intReference;
      ref_t longReference;
      ref_t realReference;
      ref_t literalReference;
      ref_t charReference;
      ref_t trueReference;
      ref_t falseReference;
      ref_t paramsReference;
      ref_t signatureReference;

      ref_t boolType;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
//      bool warnOnAnonymousSignature;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(TerminalInfo identifier);

      ref_t mapReference(ident_t reference, bool existing = false);

      ObjectInfo mapReferenceInfo(ident_t reference, bool existing = false);

      bool defineForward(ident_t forward, ident_t referenceName, bool constant)
      {
         ObjectInfo info = mapReferenceInfo(referenceName, false);

         if (constant) {
            ObjectInfo constantInfo = defineObjectInfo(info.param, true);
            // try to correctly define typed constant
            if (constantInfo.kind == okSymbol && constantInfo.extraparam != 0) {
               ref_t typeClass = resolveStrongType(constantInfo.extraparam);
               if (typeClass == intReference) {
                  defineIntConstant(info.param);
               }
               else if (typeClass == longReference) {
                  defineLongConstant(info.param);
               }
               else if (typeClass == realReference) {
                  defineRealConstant(info.param);
               }
               else if (typeClass == literalReference) {
                  defineLiteralConstant(info.param);
               }
               else if (typeClass == charReference) {
                  defineCharConstant(info.param);
               }
               defineConstantSymbol(info.param);
            }
            else defineConstantSymbol(info.param);
         }

         return forwards.add(forward, info.param, true);
      }

      void compileForwardHints(DNode hints, bool& constant);

      void defineConstantSymbol(ref_t reference)
      {
         symbolHints.add(reference, okConstantSymbol, true);
      }

      void defineIntConstant(ref_t reference)
      {
         symbolHints.add(reference, okIntConstant, true);
      }

      void defineLongConstant(ref_t reference)
      {
         symbolHints.add(reference, okLongConstant, true);
      }

      void defineRealConstant(ref_t reference)
      {
         symbolHints.add(reference, okRealConstant, true);
      }

      void defineLiteralConstant(ref_t reference)
      {
         symbolHints.add(reference, okLiteralConstant, true);
      }

      void defineCharConstant(ref_t reference)
      {
         symbolHints.add(reference, okCharConstant, true);
      }

      void raiseError(const char* message, TerminalInfo terminal);
      void raiseWarning(int level, const char* message, TerminalInfo terminal);

      bool checkReference(ident_t referenceName);

      ref_t resolveIdentifier(ident_t name);

      ref_t mapNewType(ident_t terminal);

      ref_t mapType(TerminalInfo terminal);

      ref_t mapSubject(TerminalInfo terminal, IdentifierString& output);
      ref_t mapSubject(ident_t name)
      {
         IdentifierString wsName(name);
         return module->mapSubject(wsName, false);
      }

      ref_t mapTerminal(TerminalInfo terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      ref_t loadClassInfo(_Module* &classModule, ClassInfo& info, ident_t vmtName, bool headerOnly = false);
      ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false);
      ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbol);

      ref_t resolveStrongType(ref_t type_ref);

      int defineStructSize(ref_t classReference);
      int defineTypeSize(ref_t type_ref, ref_t& class_ref);
      int defineTypeSize(ref_t type_ref)
      {
         ref_t dummy1;

         return defineTypeSize(type_ref, dummy1);
      }

      MethodType checkTypeMethod(ref_t type_ref, ref_t message, bool& found);
      MethodType checkTypeMethod(ref_t type_ref, ref_t message)
      {
         bool dummy;
         return checkTypeMethod(type_ref, message, dummy);
      }
      MethodType checkMethod(ref_t reference, ref_t message, bool& found, ref_t& outputType);
      MethodType checkMethod(ref_t reference, ref_t message)
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
//      void validateForwards();

      ref_t getBaseFunctionClass(int paramCount);
      ref_t getBaseIndexFunctionClass(int paramCount);
      ref_t getBaseLazyExpressionClass();

      int getClassFlags(ref_t reference);

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

      void raiseError(const char* message, TerminalInfo terminal)
      {
         moduleScope->raiseError(message, terminal);
      }

      void raiseWarning(int level, const char* message, TerminalInfo terminal)
      {
         moduleScope->raiseWarning(level, message, terminal);
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

//      SourceScope(Scope* parent);
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
         info.save(&metaWriter);
      }

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
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
      ref_t     message;
      LocalMap  parameters;
      int       reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int       rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well
      bool      withOpenArg;

      int compileHints(DNode hints);

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
      CommandTape* tape;

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

      ref_t getClassParentRefId(bool ownerClass = true)
      {
         ClassScope* scope = (ClassScope*)getScope(ownerClass ? slOwnerClass : slClass);

         return scope ? scope->info.header.parentRef : 0;
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

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
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

      Map<ident_t, Outer> outers;

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

   struct ExternalScope
   {
      struct ParamInfo
      {
         bool       out;
         ref_t      subject;
         ObjectInfo info;
         int        size;

         ParamInfo()
         {
            subject = 0;
            size = 0;
            out = false;
         }
      };

      struct OutputInfo
      {
         int        subject;
         int        offset;
         ObjectInfo target;

         OutputInfo()
         {
            offset = subject = 0;
         }
      };

      int               frameSize;
      Stack<ParamInfo>  operands;

      ExternalScope()
         : operands(ParamInfo())
      {
         frameSize = 0;
      }
   };

   ByteCodeWriter _writer;
   Parser         _parser;

   MessageMap     _verbs;                            // list of verbs
   MessageMap     _operators;                        // list of operators
//   MessageMap       _obsolete;                       // list of obsolete messages

//   int _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   void recordStep(CodeScope& scope, TerminalInfo terminal, int stepType)
   {
      if (terminal != nsNone) {
         _writer.declareBreakpoint(*scope.tape, terminal.row, terminal.disp, terminal.length, stepType);
      }
   }

   ref_t mapNestedExpression(CodeScope& scope, int mode);
   ref_t mapExtension(CodeScope& scope, ref_t messageRef, ObjectInfo target);

   void importCode(DNode node, ModuleScope& scope, CommandTape* tape, ident_t reference);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef, bool ignoreSealed);

   void declareParameterDebugInfo(MethodScope& scope, CommandTape* tape, bool withThis, bool withSelf);

   ObjectInfo compileTypecast(CodeScope& scope, ObjectInfo target, size_t type_ref, bool& enforced, int mode);

   void compileParentDeclaration(DNode node, ClassScope& scope);
   InheritResult compileParentDeclaration(ref_t parentRef, ClassScope& scope, bool ignoreSealed = false);

   ObjectInfo saveObject(CodeScope& scope, ObjectInfo object, int mode);

   void releaseOpenArguments(CodeScope& scope, size_t spaceToRelease);

   bool checkIfBoxingRequired(CodeScope& scope, ObjectInfo object, ref_t argType, int mode);
   ObjectInfo boxObject(CodeScope& scope, ObjectInfo object, bool& boxed);
   ObjectInfo boxStructureField(CodeScope& scope, ObjectInfo field, ObjectInfo thisObject, int mode = 0);

   ref_t mapMessage(DNode node, CodeScope& scope, size_t& paramCount, int& mode);
   ref_t mapMessage(DNode node, CodeScope& scope, size_t& paramCount)
   {
      int dummy = 0;
      return mapMessage(node, scope, paramCount, dummy);
   }

   void compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue);
   void compileAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo);
   void compileContentAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo, ObjectInfo object);
   void compileVariable(DNode node, CodeScope& scope, DNode hints);

   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);

   void compileMessageParameter(DNode& arg, TerminalInfo& subject, CodeScope& scope, ref_t type_ref, int mode, size_t& count);

   void compileDirectMessageParameters(DNode node, CodeScope& scope, int mode);
   void compilePresavedMessageParameters(DNode node, CodeScope& scope, int mode, size_t& stackToFree);
   void compileUnboxedMessageParameters(DNode node, CodeScope& scope, int mode, int count, size_t& stackToFree);

   ObjectInfo compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo object, ref_t message, int paramCount, int& mode, size_t& spaceToRelease);
   ref_t compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo& object, int& mode, size_t& spaceToRelease);

   ObjectInfo compileMessageReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileTerminal(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(DNode objectNode, CodeScope& scope, int mode);

   int mapInlineOperandType(ModuleScope& moduleScope, ObjectInfo operand);
   int mapInlineTargetOperandType(ModuleScope& moduleScope, ObjectInfo operand);

   bool compileInlineArithmeticOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, int mode);
   bool compileInlineVarArithmeticOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, int mode);
   bool compileInlineComparisionOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, bool& invertMode);
   bool compileInlineReferOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo roperand2, ObjectInfo& result);

   ObjectInfo compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id);

   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode);
   ObjectInfo compileExtensionMessage(DNode& node, DNode& roleNode, CodeScope& scope, ObjectInfo object, ObjectInfo role, int mode);
   ObjectInfo compileEvalMessage(DNode& node, CodeScope& scope, ObjectInfo object, int mode);

   ObjectInfo compileOperations(DNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileTypecastExpression(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExpression(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileRetExpression(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileAssigningExpression(DNode node, DNode assigning, CodeScope& scope, ObjectInfo target);

   ObjectInfo compileBranching(DNode thenNode, CodeScope& scope, ObjectInfo target, int verb, int subCodinteMode);

   void compileLoop(DNode node, CodeScope& scope, int mode);
   void compileThrow(DNode node, CodeScope& scope, int mode);

   void compileExternalArguments(DNode node, CodeScope& scope, ExternalScope& externalScope);
   void saveExternalParameters(CodeScope& scope, ExternalScope& externalScope);

   void reserveSpace(CodeScope& scope, int size);
   bool allocateStructure(CodeScope& scope, int mode, ObjectInfo& exprOperand, bool presavedAccumulator = false);

   ObjectInfo compilePrimitiveCatch(DNode node, CodeScope& scope);
   ObjectInfo compileExternalCall(DNode node, CodeScope& scope, ident_t dllName, int mode);
   ObjectInfo compileInternalCall(DNode node, CodeScope& scope, ObjectInfo info, int mode);

   void compileConstructorResendExpression(DNode node, CodeScope& scope, ClassScope& classClassScope, bool& withFrame);
   void compileConstructorDispatchExpression(DNode node, CodeScope& scope, ClassScope& classClassScope);
   void compileResendExpression(DNode node, CodeScope& scope);
   void compileDispatchExpression(DNode node, CodeScope& scope);

   ObjectInfo compileCode(DNode node, CodeScope& scope, int mode = 0);

   void declareArgumentList(DNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(DNode node, MethodScope& scope);
   void declareVMT(DNode member, ClassScope& scope, Symbol methodSymbol, bool closed);

   void declareSingletonClass(DNode member, ClassScope& scope, bool closed);
   void compileSingletonClass(DNode member, ClassScope& scope);

   void compileImportMethod(DNode node, ClassScope& scope, ref_t message, ident_t function);
   void compileImportMethod(DNode node, CodeScope& scope, ref_t message, ident_t function, int mode);

   void compileActionMethod(DNode member, MethodScope& scope, int mode);
   void compileLazyExpressionMethod(DNode member, MethodScope& scope);
   void compileDispatcher(DNode node, MethodScope& scope, bool withGenericMethods = false);
   void compileMethod(DNode node, MethodScope& scope, int mode);
   void compileDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);
   void compileDynamicDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);
   void compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, int mode);

   void compileSymbolCode(ClassScope& scope);

   void compileAction(DNode node, InlineClassScope& scope, DNode argNode);
   void compileNestedVMT(DNode node, InlineClassScope& scope);

   void compileVMT(DNode member, ClassScope& scope);

   void compileFieldDeclarations(DNode& member, ClassScope& scope);
   void compileClassDeclaration(DNode node, ClassScope& scope, DNode hints);
   void compileClassImplementation(DNode node, ClassScope& scope);
   void compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileClassClassImplementation(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints, bool isStatic);
   void compileSymbolImplementation(DNode node, SymbolScope& scope, DNode hints, bool isStatic);
   void compileIncludeModule(DNode node, ModuleScope& scope, DNode hints);
   void compileForward(DNode node, ModuleScope& scope, DNode hints);
   void compileType(DNode& member, ModuleScope& scope, DNode hints);

   void compileDeclarations(DNode member, ModuleScope& scope);
   void compileImplementations(DNode member, ModuleScope& scope);
   void compileIncludeSection(DNode& node, ModuleScope& scope);

   virtual void compileModule(DNode node, ModuleScope& scope);

   void compile(ident_t source, MemoryDump* buffer, ModuleScope& scope);

   bool validate(Project& project, _Module* module, int reference);
   void validateUnresolved(Unresolveds& unresolveds, Project& project);

public:
//   void setOptFlag(int flag)
//   {
//      _optFlag |= flag;
//   }

   void loadRules(StreamReader* optimization);

   bool run(Project& project);

   Compiler(StreamReader* syntax);
};

} // _ELENA_

#endif // compilerH
