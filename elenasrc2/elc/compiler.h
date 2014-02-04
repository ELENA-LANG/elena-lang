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
const int optIdleFrame         = 0x00000003;

// --- Compiler class ---
class Compiler
{
protected:
   struct Parameter
   {
      ref_t      reference;
      ObjectType type;

      Parameter()
      {
         reference = -1;
         type = otNone;
      }
      Parameter(ref_t reference, ObjectType type)
      {
         this->reference = reference;
         this->type = type;
      }
   };

////   typedef Map<const wchar16_t*, const wchar16_t*, false> NamespaceMaskMap;
   typedef Map<const wchar16_t*, ref_t, false>          ForwardMap;
   typedef Map<const wchar16_t*, Parameter, false>      LocalMap;
////   typedef Map<const wchar16_t*, ref_t>                 SubjectMap;

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
      const _path_t* fileName;
      ref_t          reference;
      _Module*       module;
      size_t         row;
      size_t         col;           // virtual column

      Unresolved()
      {
         reference = 0;
      }
      Unresolved(const _path_t* fileName, ref_t reference, _Module* module, size_t row, size_t col)
      {
         this->fileName = fileName;
         this->reference = reference;
         this->module = module;
         this->row = row;
         this->col = col;
      }
   };

   typedef List<Unresolved> Unresolveds;

   // - ModuleScope -
   struct ModuleScope
   {
      Project*       project;
      _Module*       module;
      _Module*       debugModule;

      const _path_t* sourcePath;

      // default namespaces
      List<const wchar16_t*>  defaultNs;
      ForwardMap              forwards;

      // symbol hints
      Map<ref_t, ObjectKind>  symbolHints;

      // cached values
      ref_t nilReference;
      ref_t trueReference;
      ref_t falseReference;
      ref_t controlReference;

      ref_t shortSubject;
      ref_t intSubject;
      ref_t longSubject;
      ref_t realSubject;
      ref_t handleSubject;
      ref_t wideStrSubject;
      ref_t dumpSubject;
      ref_t lengthSubject;
      ref_t lengthOutSubject;
      ref_t indexSubject;
      ref_t arraySubject;
      ref_t byteSubject;

      ref_t whileSignRef;
      ref_t untilSignRef;

      // warning mapiing
      bool warnOnUnresolved;
      bool warnOnWeakUnresolved;
//      bool warnOnAnonymousSignature;

      // list of references to the current module which should be checked after the project is compiled
      Unresolveds* forwardsUnresolved;

      ObjectType mapSubjectType(ref_t subjRef);
      ObjectType mapSubjectType(TerminalInfo identifier, bool& out);

      ObjectInfo mapObject(TerminalInfo identifier);

      ObjectInfo mapReference(const wchar16_t* reference, bool existing = false);

      ref_t mapConstantReference(const char* name)
      {
         IdentifierString wsName(name);

         return module->mapReference(wsName);
      }

      ref_t mapSubject(const wchar16_t* name, bool& output)
      {
         if (ConstantIdentifier::compare(name, "out'", 4)) {
            output = true;
            return module->mapSubject(name + 4, false);
         }
         else return module->mapSubject(name, false);
      }

      ref_t mapSubject(const wchar16_t* name)
      {
         return module->mapSubject(name, false);
      }

      ref_t mapSubject(const char* name)
      {
         IdentifierString wsName(name);

         return module->mapSubject(wsName, false);
      }

//      bool defineMask(const wchar16_t* mask, const wchar16_t* reference)
//      {
//         return symbolNs.aliases.add(mask, reference, true);
//      }

      bool defineForward(const wchar16_t* forward, const wchar16_t* referenceName, bool constant)
      {
         ObjectInfo info = mapReference(referenceName, false);

         if (constant)
            defineConstant(info.reference);

         return forwards.add(forward, info.reference, true);
      }
//      bool defineForward(const wchar16_t* forward, const char* referenceName, bool constant)
//      {
//         IdentifierString wsReferenceName(referenceName);
//
//         return defineForward(forward, wsReferenceName, constant);
//      }

      void compileForwardHints(DNode hints, bool& constant);

      void defineConstant(ref_t reference)
      {
         symbolHints.add(reference, okConstant, true);
      }

      void raiseError(const char* message, TerminalInfo terminal);
      void raiseWarning(const char* message, TerminalInfo terminal);

      ref_t resolveIdentifier(const wchar16_t* name);
////      ref_t resolveSubject(const wchar16_t* subject);
////      ref_t resolvePrivateSubject(const wchar16_t* subject);

////      ref_t mapMessageSubject(TerminalInfo terminal);

      bool checkGlobalReference(const wchar16_t* referenceName);
      ref_t mapTerminal(TerminalInfo terminal, bool existing = false);

      ObjectInfo defineObjectInfo(ref_t reference, bool checkState = false);

      ref_t loadClassInfo(ClassInfo& info, const wchar16_t* vmtName);

      void validateReference(TerminalInfo terminal, ref_t reference);
//      void validateForwards();

      void init(_Module* module, _Module* debugModule, const _path_t* sourcePath);

      ModuleScope(Project* project, Unresolveds* forwardsUnresolved);
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

      virtual ObjectInfo mapObject(TerminalInfo identifier);

      void compileHints(DNode hints);
      void compileFieldHints(DNode hints, int offset);
      void compileFieldSizeHint(DNode hints, size_t& size);

      int getFieldSizeHint();
      int getFieldSizeHint(TerminalInfo value);

      void setTypeHints(DNode hintValue);

      int getClassType()
      {
         return info.header.flags & elDebugMask;
      }

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
         writer.flush(tape, moduleScope->module, moduleScope->debugModule);
      }

      ClassScope(ModuleScope* parent, ref_t reference);
   };

   // - SymbolScope -
   struct SymbolScope : public SourceScope
   {
//      const wchar16_t* param;
//
//      void compileHints(DNode hints);

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
      Parameter outParameter;       // used in out-assignment
      bool      withBreakHandler;
      bool      withCustomVerb;
      int       masks;              // used for ecode optimization
      int       reserved;           // defines inter-frame stack buffer (excluded from GC frame chain)
      int       rootToFree;         // by default is 1, for open argument - contains the list of normal arguments as well

//      int compileHints(DNode hints);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slMethod) {
            return this;
         }
         else return parent->getScope(level);
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

      // scope breakpoint label
      int          breakLabel;

      // scope stack allocation
      int          reserved;  // allocated for the current statement
      int          saved;     // permanently allocated

      int newLocal()
      {
         level++;

         return level;
      }

      void mapLocal(const wchar16_t* local, int level, ObjectType type)
      {
         locals.add(local, Parameter(level, type));
      }

      virtual ObjectInfo mapObject(TerminalInfo identifier);

      virtual Scope* getScope(ScopeLevel level)
      {
         if (level == slCode) {
            return this;
         }
         else return parent->getScope(level);
      }

//      int getClassType()
//      {
//         ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);
//
//         return classScope->getClassType();
//      }

      //bool testMode(MethodScope::Mode mode)
      //{
      //   MethodScope* scope = (MethodScope*)getScope(slMethod);

      //   return scope ? scope->testMode(mode) : false;
      //}

//      int getMessageID()
//      {
//         MethodScope* scope = (MethodScope*)getScope(slMethod);
//
//         return scope ? scope->message : 0;
//      }

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

      void compileLocalHints(DNode hints, ObjectType& type, int& size);

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

      Map<const wchar_t*, Outer> outers;

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
      Stack<OutputInfo> output;

      ExternalScope()
         : operands(ParamInfo()), output(OutputInfo())
      {
         frameSize = 0;
      }
   };

   ByteCodeWriter _writer;
   Parser         _parser;

   MessageMap     _verbs;                            // list of verbs
   MessageMap     _operators;                        // list of operators
//   MessageMap       _obsolete;                       // list of obsolete messages

   Unresolveds      _unresolveds;

   // optimization flags
   int _optFlag;

   // optimization rules
   TransformTape _rules;

   // optmimization routines
   bool applyRules(CommandTape& tape);
   bool optimizeJumps(CommandTape& tape);
   void optimizeTape(CommandTape& tape);

   void loadOperators(MessageMap& operators);

   void recordStep(CodeScope& scope, TerminalInfo terminal, int stepType)
   {
      if (terminal != nsNone) {
         _writer.declareBreakpoint(*scope.tape, terminal.row, terminal.disp, terminal.length, stepType);
      }
   }

//   //ref_t mapQualifiedMessage(TerminalInfo terminal, ModuleScope* scope, int defaultVerb);

   ref_t mapNestedExpression(CodeScope& scope, int mode);

//   void saveInlineField(CommandTape& tape, Map<const wchar16_t*, Compiler::InlineClassScope::Outer>::Iterator& outer_it)
//   {
//      ObjectInfo info = (*outer_it).outerObject;
//
//      outer_it++;
//      if (!outer_it.Eof())
//         saveInlineField(tape, outer_it);
//
//      _writer.pushObject(tape, info);
//   }

   void importCode(DNode node, ModuleScope& scope, CommandTape* tape, const wchar16_t* reference);

   InheritResult inheritClass(ClassScope& scope, ref_t parentRef);

////   void assignBranchExpression(DNode node, TerminalInfo target, CodeScope& scope, ObjectInfo currentInfo, bool& assigned);

   void declareParameterDebugInfo(MethodScope& scope, CommandTape* tape, bool withSelf);

   ObjectInfo compileTypecast(CodeScope& scope, ObjectInfo target, size_t subject_id);

   void compileParentDeclaration(DNode node, ClassScope& scope);
   InheritResult compileParentDeclaration(ref_t parentRef, ClassScope& scope);

   ObjectInfo saveObject(CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compilePrimitiveLength(CodeScope& scope, ObjectInfo objectInfo, int target);
   ObjectInfo compilePrimitiveLengthOut(CodeScope& scope, ObjectInfo objectInfo, int target);
   ObjectInfo boxObject(CodeScope& scope, ObjectInfo object, int mode);

   ref_t mapMessage(DNode node, CodeScope& scope, size_t& paramCount, int& mode);
   ref_t mapMessage(DNode node, CodeScope& scope, size_t& paramCount)
   {
      int dummy = 0;
      return mapMessage(node, scope, paramCount, dummy);
   }

   void compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue);
   void compileAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo);
   void compileStackAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo, ObjectInfo object);
   void compileVariable(DNode node, CodeScope& scope, DNode hints);

   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, int mode);
   ObjectInfo compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileCollection(DNode objectNode, CodeScope& scope, int mode, ref_t vmtReference);
   ObjectInfo compileGetProperty(DNode member, CodeScope& scope, int mode, ref_t vmtReference);
   ObjectInfo compileGetProperty(DNode member, CodeScope& scope, int mode);

   void compileMessageParameter(DNode& arg, CodeScope& scope, const wchar16_t* subject, int mode, size_t& count);

   void compileDirectMessageParameters(DNode node, CodeScope& scope, int mode);
   void compilePresavedMessageParameters(DNode node, CodeScope& scope, int mode, size_t& stackToFree);
   void compileUnboxedMessageParameters(DNode node, CodeScope& scope, int mode, int count, size_t& stackToFree);

   ref_t compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo object, int mode, size_t& spaceToRelease);

   ObjectInfo compileMessageReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileSignatureReference(DNode objectNode, CodeScope& scope, int mode);
   ObjectInfo compileTerminal(DNode node, CodeScope& scope, int mode);   
   ObjectInfo compileObject(DNode objectNode, CodeScope& scope, int mode);

   ObjectInfo compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id);

   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode);
   ObjectInfo compileEvalMessage(DNode& node, CodeScope& scope, ObjectInfo object, int mode);

   ObjectInfo compileOperations(DNode node, CodeScope& scope, ObjectInfo target, int mode);
   ObjectInfo compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode);
   ObjectInfo compileExpression(DNode node, CodeScope& scope, int mode);
   ObjectInfo compileRetExpression(DNode node, CodeScope& scope, int mode);

   ObjectInfo compileControlVirtualExpression(DNode node, CodeScope& scope, ObjectInfo info, int mode);
   ObjectInfo compileBranching(DNode thenNode, CodeScope& scope, int verb, int subCodinteMode);

   void compileLoop(DNode node, CodeScope& scope, int mode);
   void compileThrow(DNode node, CodeScope& scope, int mode);
   void compileBreak(DNode node, CodeScope& scope, int mode);
   void compileBreakHandler(CodeScope& scope, int mode);

   void compileExternalArguments(DNode node, CodeScope& scope, ExternalScope& externalScope);
   void saveExternalParameters(CodeScope& scope, ExternalScope& externalScope);
   void reserveExternalOutputParameters(CodeScope& scope, ExternalScope& externalScope);
   void reserveExternalLiteralParameters(CodeScope& scope, ExternalScope& externalScope);

   bool allocatePrimitiveObject(CodeScope& scope, int mode, ObjectInfo& exprOperand);
   FunctionCode definePrimitiveOperationCode(CodeScope& scope, int operator_id, ObjectInfo& result, ObjectInfo loperand, ObjectInfo roperand, int mode);

   ObjectInfo compilePrimitiveOperator(DNode& node, CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, int mode);
   ObjectInfo compilePrimitiveCatch(DNode node, CodeScope& scope);
   ObjectInfo compileExternalCall(DNode node, CodeScope& scope, const wchar16_t* dllName, int mode);

   void compileResend(DNode node, CodeScope& scope);
   void compileMessageDispatch(DNode node, CodeScope& scope);

   void compileEndStatement(DNode node, CodeScope& scope);

   void compileCode(DNode node, CodeScope& scope, int mode = 0);

   void declareArgumentList(DNode node, MethodScope& scope);
   ref_t declareInlineArgumentList(DNode node, MethodScope& scope);

   void compileDispatcher(DNode node, CodeScope& scope);
   void compileTransmitor(DNode node, CodeScope& scope);
   void compileMethod(DNode node, MethodScope& scope/*, DNode hints*/);
   void compileDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);
   void compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints);

   void compileSymbolCode(ClassScope& scope);

   void compileActionVMT(DNode node, InlineClassScope& scope, DNode argNode);
   void compileNestedVMT(DNode node, InlineClassScope& scope);

   void compileAction(DNode member, MethodScope& scope, ref_t actionMessage);
   void compileInlineAction(DNode member, MethodScope& scope, ref_t actionMessage);
   void compileVMT(DNode member, ClassScope& scope);

   void compileFieldDeclarations(DNode& member, ClassScope& scope);
   void compileClassDeclaration(DNode node, ClassScope& scope, DNode hints);
   void compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope);
   void compileSymbolDeclaration(DNode node, SymbolScope& scope, /*DNode hints, */bool isStatic);
   void compileIncludeModule(DNode node, ModuleScope& scope, DNode hints);
   void compileForward(DNode node, ModuleScope& scope, DNode hints);

   void compileDeclarations(DNode& member, ModuleScope& scope);
   void compileIncludeSection(DNode& node, ModuleScope& scope);

   virtual void compileModule(DNode node, ModuleScope& scope);

   void compile(const _path_t* source, MemoryDump* buffer, ModuleScope& scope);

   bool validate(Project& project, _Module* module, int reference);
   void validateUnresolved(Project& project);

   void createModuleInfo(ModuleScope& scope, const wchar16_t* path, bool withDebugInfo, Map<const wchar16_t*, ModuleInfo>& modules);

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
