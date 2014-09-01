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

// Compiler optimization flags
//const int optDirectConstant = 0x00000001;
//const int optJumps          = 0x00000002;

// --- Compiler class ---
class Compiler
{
protected:
   struct Parameter
   {
      int        offset;
      ref_t      sign_ref;
      bool       out; 

      Parameter()
      {
         offset = -1;
         sign_ref = 0;
         out = false;
      }
      Parameter(int offset)
      {
         this->offset = offset;
         this->sign_ref = 0;
         this->out = false;
      }
      Parameter(int offset, ref_t sign_ref)
      {
         this->offset = offset;
         this->sign_ref = sign_ref;
         this->out = false;
      }
      Parameter(int offset, ref_t sign_ref, bool out)
      {
         this->offset = offset;
         this->sign_ref = sign_ref;
         this->out = out;
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

   struct Unresolved
   {
      const tchar_t* fileName;
      ref_t          reference;
      _Module*       module;
      size_t         row;
      size_t         col;           // virtual column

      Unresolved()
      {
         reference = 0;
      }
      Unresolved(const tchar_t* fileName, ref_t reference, _Module* module, size_t row, size_t col)
      {
         this->fileName = fileName;
         this->reference = reference;
         this->module = module;
         this->row = row;
         this->col = col;
      }
   };

   typedef Map<const wchar16_t*, ref_t, false>     ForwardMap;
   typedef Map<const wchar16_t*, Parameter, false> LocalMap;
   typedef Map<ref_t, ref_t>                       SubjectMap;
   typedef List<Unresolved>                        Unresolveds;
   typedef Map<ref_t, SubjectMap*>                 ExtensionMap;
   typedef Map<ref_t, int>                         SizeMap;

   // - ModuleScope -
   struct ModuleScope
   {
      Project*       project;
      _Module*       module;
      _Module*       debugModule;

      const tchar_t* sourcePath;
      ref_t          sourcePathRef;

      // default namespaces
      List<const wchar16_t*> defaultNs;
      ForwardMap             forwards;

      // symbol hints
      Map<ref_t, ObjectKind> symbolHints;

      // extensions
      SubjectMap             extensionHints; 
      ExtensionMap           extensions;

      // type hints
      SubjectMap             typeHints;
      SubjectMap             synonymHints;
      SizeMap                sizeHints;

      // cached references
      ref_t nilReference;
      ref_t trueReference;
      ref_t falseReference;

      // cached subjects
      // should not be referred directly ;
      // appropriate get function should be used instead
      ref_t intType, longType, realType, literalType;
      ref_t boolType, actionType, paramsType;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
//      bool warnOnAnonymousSignature;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectInfo mapObject(TerminalInfo identifier);

      ref_t mapReference(const wchar16_t* reference, bool existing = false);

      ObjectInfo mapReferenceInfo(const wchar16_t* reference, bool existing = false);

      ref_t mapConstantReference(const char* name)
      {
         ConstantIdentifier wsName(name);

         return module->mapReference(wsName);
      }

      ref_t mapSubject(const wchar16_t* name);

      ref_t mapSubject(const char* name)
      {
         IdentifierString wsName(name);

         return mapSubject(wsName);
      }

      bool defineForward(const wchar16_t* forward, const wchar16_t* referenceName, bool constant)
      {
         ObjectInfo info = mapReferenceInfo(referenceName, false);

         if (constant)
            defineConstantSymbol(info.param);

         return forwards.add(forward, info.param, true);
      }
//      bool defineForward(const wchar16_t* forward, const char* referenceName, bool constant)
//      {
//         IdentifierString wsReferenceName(referenceName);
//
//         return defineForward(forward, wsReferenceName, constant);
//      }

      void compileForwardHints(DNode hints, bool& constant);

      void defineConstantSymbol(ref_t reference)
      {
         symbolHints.add(reference, okConstantSymbol, true);
      }

      void defineIntConstant(ref_t reference)
      {
         symbolHints.add(reference, okIntConstant, true);
      }

      void defineLiteralConstant(ref_t reference)
      {
         symbolHints.add(reference, okLiteralConstant, true);
      }

      void raiseError(const char* message, TerminalInfo terminal);
      void raiseWarning(const char* message, TerminalInfo terminal);

      bool checkReference(const wchar16_t* referenceName);

      ref_t resolveIdentifier(const wchar16_t* name);

      ref_t mapType(const wchar16_t* terminal);
      ref_t mapType(TerminalInfo terminal, bool& out);

      ref_t mapTerminal(TerminalInfo terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      ref_t loadClassInfo(_Module* &classModule, ClassInfo& info, const wchar16_t* vmtName);
      ref_t loadClassInfo(ClassInfo& info, const wchar16_t* vmtName);
      
      bool checkTypeMethod(ref_t type_ref, ref_t message, bool& found);

      void loadTypes(_Module* module);
      void loadExtensions(TerminalInfo terminal, _Module* module);

      bool saveType(ref_t type_ref, ref_t wrapper_ref = 0, int size = 0);
      bool saveSynonym(ref_t type_ref, ref_t synonym_ref);
      bool saveExtension(ref_t message, ref_t type, ref_t role);

      void validateReference(TerminalInfo terminal, ref_t reference);
//      void validateForwards();

      ref_t getBoolType();
      ref_t getIntType();
      ref_t getLongType();
      ref_t getRealType();
      ref_t getLiteralType();
      ref_t getParamsType();
      ref_t getActionType();

      ref_t getClassType(ref_t reference);

      void init(_Module* module, _Module* debugModule);

      ModuleScope(Project* project, const tchar_t* sourcePath, Unresolveds* forwardsUnresolved);
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

      void raiseWarning(const char* message, TerminalInfo terminal)
      {
         moduleScope->raiseWarning(message, terminal);
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
      ref_t     extensionTypeRef;

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

      void save(ByteCodeWriter& writer)
      {
         // save class meta data
         MemoryWriter metaWriter(moduleScope->module->mapSection(reference | mskMetaRDataRef, false));
         info.save(&metaWriter);

         // create byte code sections
         writer.compile(tape, moduleScope->module, moduleScope->debugModule, moduleScope->sourcePathRef);
      }

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
      ref_t typeRef;

      void compileHints(DNode hints, bool& constant);

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
      bool      withBreakHandler;
//      bool      withCustomVerb;
      int       masks;              // used for ecode optimization
      int       reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int       rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well

      int compileHints(DNode hints);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
      }

      ref_t getClassType()
      {
         return ((ClassScope*)parent)->info.header.typeRef;
      }

      void include();

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

      void mapLocal(const wchar16_t* local, int level, ref_t type)
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

      ref_t getClassType()
      {
         MethodScope* methodScope = (MethodScope*)getScope(Scope::slMethod);

         return methodScope->getClassType();
      }

//      //bool testMode(MethodScope::Mode mode)
//      //{
//      //   MethodScope* scope = (MethodScope*)getScope(slMethod);
//
//      //   return scope ? scope->testMode(mode) : false;
//      //}

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

         return scope ? scope->extensionTypeRef : 0;
      }

      void compileLocalHints(DNode hints, ref_t& type, int& size);

      CodeScope(SourceScope* parent);
      CodeScope(MethodScope* parent);
//      CodeScope(MethodScope* parent, CodeType type);
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

      Map<const wchar16_t*, Outer> outers;

      Outer mapSelf();

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slClass) {
            return this;
         }
         else return Scope::getScope(level);
      }

//      void mapOuterField(const wchar16_t* name, ObjectInfo object)
//      {
//         int offset = info.fields.Count();
//
//         info.fields.add(name, offset);
//         outers.add(name, Outer(offset, object));
//      }
//
      virtual ObjectInfo mapObject(TerminalInfo identifier);

      InlineClassScope(CodeScope* owner, ref_t reference);
   };

   struct ExternalScope
   {
      struct ParamInfo
      {
         ref_t      subject;
         ObjectInfo info;
         bool       output;

         ParamInfo()
         {
            subject = 0;
            output = false;
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

   // optimization flags
   int _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

////   void loadOperators(MessageMap& operators);

   void recordStep(CodeScope& scope, TerminalInfo terminal, int stepType)
   {
      if (terminal != nsNone) {
         _writer.declareBreakpoint(*scope.tape, terminal.row, terminal.disp, terminal.length, stepType);
      }
   }

////   //ref_t mapQualifiedMessage(TerminalInfo terminal, ModuleScope* scope, int defaultVerb);

   ref_t mapNestedExpression(CodeScope& scope, int mode, ref_t& typeRef);

////   void saveInlineField(CommandTape& tape, Map<const wchar16_t*, Compiler::InlineClassScope::Outer>::Iterator& outer_it)
////   {
////      ObjectInfo info = (*outer_it).outerObject;
////
////      outer_it++;
////      if (!outer_it.Eof())
////         saveInlineField(tape, outer_it);
////
////      _writer.pushObject(tape, info);
////   }

   void importCode(DNode node, ModuleScope& scope, CommandTape* tape, const wchar16_t* reference);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef);

////   void assignBranchExpression(DNode node, TerminalInfo target, CodeScope& scope, ObjectInfo currentInfo, bool& assigned);

   void declareParameterDebugInfo(MethodScope& scope, CommandTape* tape, bool withThis, bool withSelf);

   ObjectInfo compileTypecast(CodeScope& scope, ObjectInfo target, size_t type_ref, bool& enforced, int mode);

   void compileParentDeclaration(DNode node, ClassScope& scope);
   InheritResult compileParentDeclaration(ref_t parentRef, ClassScope& scope);

   ObjectInfo saveObject(CodeScope& scope, ObjectInfo object, int mode);

   bool checkIfBoxingRequired(CodeScope& scope, ObjectInfo object, int mode = 0);
   ObjectInfo boxObject(CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo boxStructureField(CodeScope& scope, ObjectInfo object, int mode);

   ref_t mapMessage(DNode node, CodeScope& scope, ObjectInfo object, size_t& paramCount, int& mode);
   ref_t mapMessage(DNode node, CodeScope& scope, ObjectInfo object, size_t& paramCount)
   {
      int dummy = 0;
      return mapMessage(node, scope, object, paramCount, dummy);
   }

   ref_t mapConstantType(ModuleScope& scope, ObjectKind kind);

   void compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue);
   void compileAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo);
   void compileContentAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo, ObjectInfo object);
   void compileVariable(DNode node, CodeScope& scope, DNode hints);

   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);
   ObjectInfo compileGetProperty(DNode member, CodeScope& scope, int mode, ref_t vmtReference);
   ObjectInfo compileGetProperty(DNode member, CodeScope& scope, int mode);

   void compileMessageParameter(DNode& arg, CodeScope& scope, ref_t type_ref, int mode, size_t& count);

   void compileDirectMessageParameters(DNode node, CodeScope& scope, int mode);
   void compilePresavedMessageParameters(DNode node, CodeScope& scope, int mode, size_t& stackToFree);
   void compileUnboxedMessageParameters(DNode node, CodeScope& scope, int mode, int count, size_t& stackToFree);

   ref_t compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo object, int& mode, size_t& spaceToRelease);

   ObjectInfo compileReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileMessageReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileSignatureReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileTerminal(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileObject(DNode objectNode, CodeScope& scope, int mode);

   bool compileInlineArithmeticOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, int mode);
   bool compileInlineComparisionOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result, bool& invertMode);
   bool compileInlineReferOperator(CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, ObjectInfo& result);

   ObjectInfo compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id);

   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode);
   ObjectInfo compileEvalMessage(DNode& node, CodeScope& scope, ObjectInfo object, int mode);

   ObjectInfo compileOperations(DNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExpression(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileRetExpression(DNode node, CodeScope& scope, int mode);

   ObjectInfo compileBranching(DNode thenNode, CodeScope& scope, ObjectInfo target, int verb, int subCodinteMode);

   void compileLoop(DNode node, CodeScope& scope, int mode);
   void compileThrow(DNode node, CodeScope& scope, int mode);
   void compileBreak(DNode node, CodeScope& scope, int mode);
   void compileBreakHandler(CodeScope& scope, int mode);

   void compileExternalArguments(DNode node, CodeScope& scope, ExternalScope& externalScope);
   void saveExternalParameters(CodeScope& scope, ExternalScope& externalScope);

   void reserveSpace(CodeScope& scope, int size);
   bool allocateStructure(CodeScope& scope, int mode, ObjectInfo& exprOperand);

   ObjectInfo compilePrimitiveCatch(DNode node, CodeScope& scope);
   ObjectInfo compileExternalCall(DNode node, CodeScope& scope, const wchar16_t* dllName, int mode);

   void compileResendExpression(DNode node, CodeScope& scope);
   void compileDispatchExpression(DNode node, CodeScope& scope);

   void compileCode(DNode node, CodeScope& scope, int mode = 0);

   void declareArgumentList(DNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(DNode node, MethodScope& scope);

   void compileSpecialMethod(MethodScope& scope);

   void compileImportMethod(DNode node, ClassScope& scope, ref_t message, const char* function);
   void compileImportMethod(DNode node, CodeScope& scope, ref_t message, const wchar16_t* function, int mode);

   void compileDispatcher(DNode node, MethodScope& scope);
   void compileMethod(DNode node, MethodScope& scope, int mode);
   void compileDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);
   void compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);

   void compileSymbolCode(ClassScope& scope);

   void compileActionVMT(DNode node, InlineClassScope& scope, DNode argNode);
   void compileNestedVMT(DNode node, InlineClassScope& scope);

   void compileAction(DNode member, MethodScope& scope);
   void compileExpressionAction(DNode member, MethodScope& scope);
   void compileVMT(DNode member, ClassScope& scope);

   void compileFieldDeclarations(DNode& member, ClassScope& scope);
   void compileClassDeclaration(DNode node, ClassScope& scope, DNode hints);
   void compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(DNode node, SymbolScope& scope, DNode hints, bool isStatic);
   void compileIncludeModule(DNode node, ModuleScope& scope, DNode hints);
   void compileForward(DNode node, ModuleScope& scope, DNode hints);
   void compileType(DNode& member, ModuleScope& scope, DNode hints);

   void compileDeclarations(DNode& member, ModuleScope& scope);
   void compileIncludeSection(DNode& node, ModuleScope& scope);

   virtual void compileModule(DNode node, ModuleScope& scope);

   void compile(const tchar_t* source, MemoryDump* buffer, ModuleScope& scope);

   bool validate(Project& project, _Module* module, int reference);
   void validateUnresolved(Unresolveds& unresolveds, Project& project);

   void createModuleInfo(ModuleScope& scope, const tchar_t* path, bool withDebugInfo, Map<const wchar16_t*, ModuleInfo>& modules);

public:
   void setOptFlag(int flag)
   {
      _optFlag |= flag;
   }

   void loadRules(StreamReader* optimization);

   bool run(Project& project);

   Compiler(StreamReader* syntax);
};

} // _ELENA_

#endif // compilerH
