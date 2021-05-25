//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef bcwriterH
#define bcwriterH 1

#include "bytecode.h"
#include "compilercommon.h"
//#include "syntaxtree.h"

namespace _ELENA_
{

// --- ByteCodeWriter class ---
class ByteCodeWriter
{
   struct Scope
   {
      MemoryWriter* vmt;
      MemoryWriter* code;
      MemoryWriter* debug;
      MemoryWriter* debugStrings;
      bool          appendMode;

      Scope()
      {
         vmt = code = NULL;
         debug = debugStrings = NULL;
         appendMode = false;
      }
   };

   struct ImportScope
   {
      _Memory*    section; 
      _Module*    sour;
      _Module*    dest;

      ImportScope()
      {
         section = NULL;
         sour = NULL;
         dest = NULL;
      }
      ImportScope(_Memory* section, _Module* sour, _Module* dest)
      {
         this->section = section;
         this->sour = sour;
         this->dest = dest;
      }
   };

   struct RegScope
   {
      LexicalType type;
      ref_t       arg;

      RegScope()
      {
         type = lxNone;
         arg = 0;
      }
   };

   struct FlowScope
   {
      RegScope acc;
      bool     debugBlockStarted;

      void clear()
      {
         acc.type = lxNone;
      }

      FlowScope()
      {
         debugBlockStarted = false;
      }
   };

   List<ImportScope> imports;
   MemoryDump _strings; // NOTE : all literal constants are copied into this temporal buffer

   ByteCode peekNext(ByteCodeIterator it)
   {
      it++;

      return (*it).code;
   }

   ByteCode peekPrevious(ByteCodeIterator it)
   {
      it--;

      return (*it).code;
   }

   void writeNewStatement(MemoryWriter* debug);
   void writeNewBlock(MemoryWriter* debug);
   void writeSelf(Scope& scope, int level, int frameLevel);
   void writeLocal(Scope& scope, ident_t localName, int level, int frameLevel);
   void writeLocal(Scope& scope, ident_t localName, int level, DebugSymbol symbol, int frameLevel);
   void writeMessageInfo(Scope& scope, DebugSymbol symbol, ident_t message);
   void writeInfo(Scope& scope, DebugSymbol symbol, ident_t className);
   void writeBreakpoint(ByteCodeIterator& it, MemoryWriter* debug);

   void writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings, _Module* module);
   void writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t className, int flags);
   void writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t symbolName);
   void writeProcedureDebugInfo(Scope& scope, ref_t sourceRef);
   void writeCodeDebugInfo(Scope& scope, ref_t sourceRef);
   void writeDebugInfoStopper(MemoryWriter* debug);

   void writeProcedure(ByteCodeIterator& it, Scope& scope);
   void writeAbstractProcedure(ByteCodeIterator& it, Scope& scope);
   void writeVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope);
   void writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, bool appendMode);
   void writeClass(ref_t reference, ByteCodeIterator& it, _ModuleScope& scope);

   void declareInitializer(CommandTape& tape, ref_t reference);
   void declareClass(CommandTape& tape, ref_t reference);
   void declareSymbol(CommandTape& tape, ref_t reference, ref_t sourcePathRef);
   void declareStaticSymbol(CommandTape& tape, ref_t staticReference, ref_t sourcePathRef);
   void declareIdleMethod(CommandTape& tape, mssg_t message, ref_t sourcePathRef);
   void declareAbstractMethod(CommandTape& tape, mssg_t message);
   void declareMethod(CommandTape& tape, mssg_t message, ref_t sourcePathRef, int reserved, int allocated,
      bool withPresavedMessage, bool withNewFrame = true);
   void excludeFrame(CommandTape& tape);
   void includeFrame(CommandTape& tape, bool withThreadSafeNop);
   void allocateStack(CommandTape& tape, int count);
   int declareLoop(CommandTape& tape, bool threadFriendly);  // thread friendly means the loop contains safe point
   void declareThenBlock(CommandTape& tape);
   void declareThenElseBlock(CommandTape& tape);
   void declareElseBlock(CommandTape& tape);
   void declareSwitchBlock(CommandTape& tape);
   void declareSwitchOption(CommandTape& tape);
   void declareTry(CommandTape& tape);
   int declareSafeTry(CommandTape& tape);
   void declareCatch(CommandTape& tape);
   void declareSafeCatch(CommandTape& tape, SyntaxTree::Node finallyNode, int retLabel, FlowScope& scope);
   void doCatch(CommandTape& tape);
   void declareAlt(CommandTape& tape);

   void declareLocalInfo(CommandTape& tape, ident_t localName, int level);
   void declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className);
   void declareSelfStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className);
   void declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level);
   void declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level);
   void declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level);
   void declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level);
   void declareSelfInfo(CommandTape& tape, int level);
   void declareMessageInfo(CommandTape& tape, ident_t message);
   void declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType);
   void declareBlock(CommandTape& tape);

   void openFrame(CommandTape& tape, int reserved);
   void newFrame(CommandTape& tape, int reserved, int allocated, bool withPresavedMessage, bool withPresavedAcc);
   void newStructure(CommandTape& tape, int size, ref_t reference);
   void newDynamicStructure(CommandTape& tape, int itemSize, ref_t reference);

   void newObject(CommandTape& tape, int fieldCount, ref_t reference);
   void newDynamicObject(CommandTape& tape, ref_t reference);

   void popObject(CommandTape& tape, LexicalType sourceType);

   void clearObject(CommandTape& tape, int fieldCount);
   void clearDynamicObject(CommandTape& tape);
   void unboxArgList(CommandTape& tape, int fixedLen/*, bool arrayMode*/);

   void releaseStack(CommandTape& tape, int count);
   void releaseArg(CommandTape& tape);
   void releaseArgList(CommandTape& tape);

   void setSubject(CommandTape& tape, ref_t subject);

   void callResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message/*, bool invokeMode, bool withValidattion = true*/);
   void callVMTResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message/*, bool invokeMode*/);

   void doMultiDispatch(CommandTape& tape, ref_t operationList, mssg_t message);
   void doSealedMultiDispatch(CommandTape& tape, ref_t operationList, mssg_t message);
   void doGenericHandler(CommandTape& tape);
   void unboxMessage(CommandTape& tape);
   void changeMessageCounter(CommandTape& tape, int arg, int paramCount, int flags);
   void resend(CommandTape& tape);
   void resendDirectResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message, bool sealedMode);
   void callExternal(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed);
   void callLongExternal(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed);
   void callCore(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed);

   void jumpIfLess(CommandTape& tape, ref_t ref);
   void jumpIfNotLess(CommandTape& tape, ref_t ref);
   void jumpIfGreater(CommandTape& tape, ref_t ref);
   void jumpIfNotGreater(CommandTape& tape, ref_t ref);
   void jumpIfEqual(CommandTape& tape, ref_t ref, bool referenceMode);
   void jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, bool referenceMode, bool jumpToEnd = false);

   void tryLock(CommandTape& tape);
   void freeLock(CommandTape& tape);

   void gotoEnd(CommandTape& tape, PseudoArg label);

   void selectByIndex(CommandTape& tape, ref_t r1, ref_t r2);
   void selectByAcc(CommandTape& tape, ref_t r1, ref_t r2);

   void endTry(CommandTape& tape);
   void endCatch(CommandTape& tape);
   void endSafeCatch(CommandTape& tape);
   void endAlt(CommandTape& tape);
   void endThenBlock(CommandTape& tape);
   void endLoop(CommandTape& tape);
   void endLoop(CommandTape& tape, ref_t comparingRef);
   void exitMethod(CommandTape& tape, int reserved, bool withFrame = true);
   void endMethod(CommandTape& tape, int reserved, bool withFrame = true);
   void endIdleMethod(CommandTape& tape);
   void endClass(CommandTape& tape);
   void endSymbol(CommandTape& tape);
   void endInitializer(CommandTape& tape);
   void endStaticSymbol(CommandTape& tape, ref_t staticReference);
   void endSwitchOption(CommandTape& tape);
   void endSwitchBlock(CommandTape& tape);
   void closeFrame(CommandTape& tape, int reserved);

//   void saveIntConstant(CommandTape& tape, LexicalType target, int targetArg, int value);
   void saveLength(CommandTape& tape, LexicalType target, int targetArg, int value);
   void doIntOperation(CommandTape& tape, int operator_id, int localOffset);
   void doIntOperation(CommandTape& tape, int operator_id, int localOffset, int immValue);
   void doIntBoolOperation(CommandTape& tape, int operator_id);
   void doLongOperation(CommandTape& tape, int operator_id, int localOffset);
   void doLongBoolOperation(CommandTape& tape, int operator_id);
   void doRealOperation(CommandTape& tape, int operator_id, int localOffset);
   void doRealIntOperation(CommandTape& tape, int operator_id, int localOffset);
   void doRealIntOperation(CommandTape& tape, int operator_id, int localOffset, int immValue);
   void doRealBoolOperation(CommandTape& tape, int operator_id);
   void doArgArrayOperation(CommandTape& tape, int operator_id, int rargument);
   void doArgArrayOperation(CommandTape& tape, int operator_id, int rargument, int immValue);
   void doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize, int target);
   void doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize, int target, int immValue);
   void doArrayImmOperation(CommandTape& tape, int operator_id, int target);
   void doArrayOperation(CommandTape& tape, int operator_id, int immValue);

   void translateBreakpoint(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope/*, bool ignoreBranching*/);

   void loadFieldAddress(CommandTape& tape, int size, int argument);
   void copyFieldAddress(CommandTape& tape, int size, int argument, FlowScope& scope);
   void copyToFieldAddress(CommandTape& tape, int size, int argument);
   void copyToLocalAddress(CommandTape& tape, int size, int argument);
   void saveToLocalAddress(CommandTape& tape, int size, int argument);
//   void saveToLocal(CommandTape& tape, int size, int argument);
   void copyToLocal(CommandTape& tape, int size, int argument);
   void copyFromLocalAddress(CommandTape& tape, int size, int argument);

   void saveObject(CommandTape& tape, LexicalType type, ref_t argument);
   void saveObject(CommandTape& tape, SNode node);

   int saveExternalParameters(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool idleMode);

   void pushObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode);
//   void pushObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void loadObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode);
   void loadObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void pushIntConstant(CommandTape& tape, int value);
   void loadIntValue(CommandTape& tape);
   //   void pushIntValue(CommandTape& tape);

//   void saveIndexToFieldExpression(CommandTape& tape, SNode dstObj, SNode source, FlowScope& scope);
//   void saveIndexToObject(CommandTape& tape, SNode dstObj, SNode source, FlowScope& scope, int size);
   void saveFieldExpression(CommandTape& tape, SNode dstObj, SNode source, int size, FlowScope& scope);
   SyntaxTree::Node loadFieldExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool idleMode);

   void copyExpression(CommandTape& tape, SNode source, SNode dstObj, int size, FlowScope& scope/*, bool condCopying*/);

   void generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset);

   void generateBoolLogicOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void generateNilOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void generateBoolOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void generateArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void generateNewArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);

   void generateResendingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateDispatching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateMultiDispatching(CommandTape& tape, SyntaxTree::Node node, mssg_t message);
//   void generateYieldDispatch(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateYieldReturn(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
////   void generateYieldStop(CommandTape& tape, SyntaxTree::Node node);
////   void generateExternalArguments(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope);
   void generateExternalCall(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateInternalCall(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCall(CommandTape& tape, SyntaxTree::Node node);

   void generateExternFrame(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateTrying(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateAlt(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateLooping(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateBranching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateSwitching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateByRefAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCopyingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateSavingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
//   void generateIndexLoadingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
//   void generateIndexSavingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
//   //   void generateCopying(CommandTape& tape, SyntaxTree::Node node, int mode = 0);
   void generateReturnExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCallExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateInlineArgCallExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message, FlowScope& scope);
   void generateInitializingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCloningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateDebugInfo(CommandTape& tape, SyntaxTree::Node current);
   void generateCodeBlock(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool ignoreEOP);
   void generateCreating(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool fillMode);
   void generateCondBoxing(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCondUnBoxing(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);

   void generateMethod(CommandTape& tape, SyntaxTree::Node node, ref_t sourcePathRef/*, bool extStackEvenMode*/);
   void generateMethodDebugInfo(CommandTape& tape, SyntaxTree::Node node);

   void importCode(CommandTape& tape, ImportScope& scope, bool withBreakpoints);

public:
   pos_t writeSourcePath(_Module* debugModule, ident_t path);
   int writeString(ident_t path);

   void generateClass(_ModuleScope& scope, CommandTape& tape, SNode root, ref_t reference, pos_t sourcePathBookmark, 
      bool(*cond)(LexicalType)/*, bool extStackEvenMode*/);
   void generateInitializer(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument);
   void generateInitializer(CommandTape& tape, ref_t reference, SNode root);
   void generateSymbol(CommandTape& tape, SNode root, bool isStatic, pos_t sourcePathBookmark);
   void generateConstantList(SNode node, _Module* module, ref_t reference);
   void generateConstantMember(MemoryWriter& writer, LexicalType type, ref_t argument);

   void saveTape(CommandTape& tape, _ModuleScope& scope);

   int registerImportInfo(_Memory* section, _Module* sour, _Module* dest)
   {
      imports.add(ImportScope(section, sour, dest));

      return imports.Count();
   }

};

//bool isSimpleObjectExpression(SyntaxTree::Node node, bool ignoreFields = false);

} // _ELENA_

#endif // bcwriterH
