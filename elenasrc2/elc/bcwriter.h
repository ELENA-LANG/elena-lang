//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                              (C)2005-2019, by Alexei Rakov
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

//   struct ExternalScope
//   {
//      struct ParamInfo
//      {
//         int offset;
//
//         ParamInfo()
//         {
//            offset = 0;
//         }
//      };
//      
//      int               frameSize;
//      Stack<ParamInfo>  operands;
//   
//      ExternalScope()
//         : operands(ParamInfo())
//      {
//         frameSize = 0;
//      }
//   };

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

   void writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings);
   void writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t className, int flags);
   void writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t symbolName);
   void writeProcedureDebugInfo(Scope& scope, ref_t sourceRef);
   void writeCodeDebugInfo(Scope& scope, ref_t sourceRef);
   void writeDebugInfoStopper(MemoryWriter* debug);

   void writeProcedure(ByteCodeIterator& it, Scope& scope);
   void writeVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope);
   void writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, bool appendMode);
   void writeClass(ref_t reference, ByteCodeIterator& it, _ModuleScope& scope);

//   void declareInitializer(CommandTape& tape, ref_t reference);
   void declareClass(CommandTape& tape, ref_t reference);
   void declareSymbol(CommandTape& tape, ref_t reference, ref_t sourcePathRef);
//   void declareStaticSymbol(CommandTape& tape, ref_t staticReference, ref_t sourcePathRef);
   void declareIdleMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef);
   void declareMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef, int reserved, int allocated/*, bool withPresavedMessage*/, 
      bool withNewFrame = true);
//   void declareExternalBlock(CommandTape& tape);
//   void excludeFrame(CommandTape& tape);
//   void includeFrame(CommandTape& tape);
//   void declareVariable(CommandTape& tape, int value);
   void allocateStack(CommandTape& tape, int count);
//   int declareLoop(CommandTape& tape, bool threadFriendly);  // thread friendly means the loop contains safe point
   void declareThenBlock(CommandTape& tape);
   void declareThenElseBlock(CommandTape& tape);
   void declareElseBlock(CommandTape& tape);
//   void declareSwitchBlock(CommandTape& tape);
//   void declareSwitchOption(CommandTape& tape);
//   void declareTry(CommandTape& tape);
//   int declareSafeTry(CommandTape& tape);
//   void declareCatch(CommandTape& tape);
//   void declareSafeCatch(CommandTape& tape, SyntaxTree::Node finallyNode, int retLabel);
//   void doCatch(CommandTape& tape);
//   void declareAlt(CommandTape& tape);

   void declareLocalInfo(CommandTape& tape, ident_t localName, int level);
//   void declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className);
//   void declareSelfStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className);
   void declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level);
   void declareSelfInfo(CommandTape& tape, int level);
   void declareMessageInfo(CommandTape& tape, ident_t message);
   void declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType);
   void declareBlock(CommandTape& tape);

   void newFrame(CommandTape& tape, int reserved, int allocated/*, bool withPresavedMessage*/);
   void newStructure(CommandTape& tape, int size, ref_t reference);
   //void newDynamicStructure(CommandTape& tape, int itemSize);

   void newObject(CommandTape& tape, int fieldCount, ref_t reference);
//   void newVariable(CommandTape& tape, ref_t reference, LexicalType field, ref_t argument = 0);
   void newDynamicObject(CommandTape& tape, ref_t reference);
//   void copyDynamicObject(CommandTape& tape, bool unsafeMode, bool swapMode);

   void popObject(CommandTape& tape, LexicalType sourceType);

//   void copyBase(CommandTape& tape, int size);
//   void loadBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument, int mode);
//   void loadFieldExpressionBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument, int mode);
//   void initBase(CommandTape& tape, int fieldCount);
   void clearObject(CommandTape& tape, int fieldCount);
   void clearDynamicObject(CommandTape& tape);
   //   void initDynamicObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);
//   void saveBase(CommandTape& tape, bool directOperation, LexicalType sourceType, ref_t sourceArgument = 0);
//   void saveStructBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument, int size);
//   void loadIndex(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);
//   void loadInternalReference(CommandTape& tape, ref_t reference);
//
//   void boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing = false);
//   void boxField(CommandTape& tape, int offset, int size, ref_t vmtReference);
//   void boxArgList(CommandTape& tape, ref_t vmtReference);
//   void unboxArgList(CommandTape& tape, bool arrayMode);
//   void unboxLocal(CommandTape& tape, SNode larg, SNode rarg);

   void releaseStack(CommandTape& tape, int count = 1);
//   void releaseArgList(CommandTape& tape);
//
//   void setSubject(CommandTape& tape, ref_t subject);
//
//   void callMethod(CommandTape& tape, int vmtOffset, int paramCount);
   void callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message/*, bool invokeMode, bool withValidattion = true*/);
//   void callInitMethod(CommandTape& tape, ref_t reference, ref_t message, bool withValidattion = true);
   void callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message/*, bool invokeMode*/);

   void doMultiDispatch(CommandTape& tape, ref_t operationList, ref_t message);
   void doSealedMultiDispatch(CommandTape& tape, ref_t operationList, ref_t message);
//   void doGenericHandler(CommandTape& tape);
//   void unboxMessage(CommandTape& tape);
//   void changeMessageCounter(CommandTape& tape, int paramCount, int flags);
   void resend(CommandTape& tape);
   void resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
//   void callExternal(CommandTape& tape, ref_t functionReference, int paramCount);
//   void callCore(CommandTape& tape, ref_t functionReference, int paramCount);
//
//   void jumpIfLess(CommandTape& tape, ref_t ref);
//   void jumpIfNotLess(CommandTape& tape, ref_t ref);
//   void jumpIfGreater(CommandTape& tape, ref_t ref);
//   void jumpIfNotGreater(CommandTape& tape, ref_t ref);
//   void jumpIfEqual(CommandTape& tape, ref_t ref, bool referenceMode);
   void jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, /*bool referenceMode, */bool jumpToEnd = false);

//////   void throwCurrent(CommandTape& tape);
//
//   void tryLock(CommandTape& tape);
//   void freeLock(CommandTape& tape);

   void gotoEnd(CommandTape& tape, PseudoArg label);

//   void selectByIndex(CommandTape& tape, ref_t r1, ref_t r2);
//   void selectByAcc(CommandTape& tape, ref_t r1, ref_t r2);
//
//   void freeVirtualStack(CommandTape& tape, int count);
//
//   void endTry(CommandTape& tape);
//   void endCatch(CommandTape& tape);
//   void endSafeCatch(CommandTape& tape);
//   void endAlt(CommandTape& tape);
   void endThenBlock(CommandTape& tape);
//   void endLoop(CommandTape& tape);
//   void endLoop(CommandTape& tape, ref_t comparingRef);
//   void endExternalBlock(CommandTape& tape, bool idle = false);
   void exitMethod(CommandTape& tape, int count, int reserved, bool withFrame = true);
   void endMethod(CommandTape& tape, int argCount, int reserved, bool withFrame = true);
   void endIdleMethod(CommandTape& tape);
   void endClass(CommandTape& tape);
   void endSymbol(CommandTape& tape);
//   void endInitializer(CommandTape& tape);
//   void endStaticSymbol(CommandTape& tape, ref_t staticReference);
//   void endSwitchOption(CommandTape& tape);
//   void endSwitchBlock(CommandTape& tape);
//   void closeFrame(CommandTape& tape);
//
//   void assignBaseTo(CommandTape& tape, LexicalType target);
//
//   void assignInt(CommandTape& tape, LexicalType target, int offset, bool& accRequired);
//   void assignLong(CommandTape& tape, LexicalType target, int offset, bool& accRequired);
//   void assignShort(CommandTape& tape, LexicalType target, int offset, bool& accRequired);
//   void assignByte(CommandTape& tape, LexicalType target, int offset, bool& accRequired);
//   void assignStruct(CommandTape& tape, LexicalType target, int offset, int size);
//   void saveInt(CommandTape& tape, LexicalType target, int argument);
//   void saveReal(CommandTape& tape, LexicalType target, int argument);
//   void saveLong(CommandTape& tape, LexicalType target, int argument);
//   void copyInt(CommandTape& tape, int offset);
//   void copyShort(CommandTape& tape, int offset);
//   void copyByte(CommandTape& tape, int offset);
//   void copyStructure(CommandTape& tape, int offset, int size);
//   void copyStructureField(CommandTape& tape, int sour_offset, int dest_offset, int size);
//   void saveSubject(CommandTape& tape);
//   void saveIntConstant(CommandTape& tape, int value);
////////   void invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef);
   void doIntOperation(CommandTape& tape, int operator_id, int localOffset, FlowScope& scope);
//   void doIntOperation(CommandTape& tape, int operator_id);
//   void doIntOperation(CommandTape& tape, int operator_id, int immArg);
//   void doIndexOperation(CommandTape& tape, int operator_id);
//   void doIndexOperation(CommandTape& tape, int operator_id, int immArg);
//   void doIntDirectOperation(CommandTape& tape, int operator_id, int immArg, int indexArg);
//   void doFieldIntOperation(CommandTape& tape, int operator_id, int offset, int immArg);
//   void doLongOperation(CommandTape& tape, int operator_id);
//   void doRealOperation(CommandTape& tape, int operator_id);
//   void doRealOperation(CommandTape& tape, int operator_id, int immArg);
//   void doArrayOperation(CommandTape& tape, int operator_id);
//   void doArgArrayOperation(CommandTape& tape, int operator_id);
//   void doIntArrayOperation(CommandTape& tape, int operator_id);
//   void doByteArrayOperation(CommandTape& tape, int operator_id);
//   void doShortArrayOperation(CommandTape& tape, int operator_id);
//   void doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize);

   void translateBreakpoint(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope/*, bool ignoreBranching*/);

   void copyToFieldAddress(CommandTape& tape, int size, int argument);
   void copyToLocalAddress(CommandTape& tape, int size, int argument);
   void copyToLocal(CommandTape& tape, int size, int argument);
   void copyFromLocalAddress(CommandTape& tape, int size, int argument);

   void saveObject(CommandTape& tape, LexicalType type, ref_t argument);
   void saveObject(CommandTape& tape, SNode node);
//   void saveObjectIfChanged(CommandTape& tape, LexicalType type, ref_t argument, int checkLocal, int mode);
//
//   int saveExternalParameters(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope);
//   void unboxCallParameters(CommandTape& tape, SyntaxTree::Node node);
//   void unboxCallParameter(CommandTape& tape, SNode current);

   void pushObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode);
   void pushObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
   void loadObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode);
   void loadObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);

   SyntaxTree::Node loadFieldExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);

//   void saveUnboxingVar(CommandTape& tape, SNode member, bool& accTrarget, bool& accPresaving, int& presavedCount);
//   void loadUnboxingVar(CommandTape& tape, SNode current, int paramCount, int& presavedCount);
//
//   void generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset);
//
//   void generateBoolOperation(CommandTape& tape, SyntaxTree::Node node, int mode);
//   void generateNilOperation(CommandTape& tape, SyntaxTree::Node node);
   void generateOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode);
//   void generateArrOperation(CommandTape& tape, SyntaxTree::Node node, int mode);
   void generateNewArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);

   void generateResendingExpression(CommandTape& tape, SyntaxTree::Node node);
//   void generateDispatching(CommandTape& tape, SyntaxTree::Node node);
//   void generateResending(CommandTape& tape, SyntaxTree::Node node);
   void generateMultiDispatching(CommandTape& tape, SyntaxTree::Node node, ref_t message);
//   void generateYieldDispatch(CommandTape& tape, SyntaxTree::Node node);
//   void generateYieldReturn(CommandTape& tape, SyntaxTree::Node node);
//   void generateYieldStop(CommandTape& tape, SyntaxTree::Node node);
//   void generateExternalArguments(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope);
//   void generateExternalCall(CommandTape& tape, SyntaxTree::Node node);
//   void generateInternalCall(CommandTape& tape, SyntaxTree::Node node);
   /*ref_t*/void generateCall(CommandTape& tape, SyntaxTree::Node node/*, int paramCount, int presavedCount*/);

//   void generateExternFrame(CommandTape& tape, SyntaxTree::Node node);
//   void generateTrying(CommandTape& tape, SyntaxTree::Node node);
//   void generateAlt(CommandTape& tape, SyntaxTree::Node node);
//   void generateLooping(CommandTape& tape, SyntaxTree::Node node);
   void generateBranching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateSwitching(CommandTape& tape, SyntaxTree::Node node);
   void generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateCopyingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
//   void generateCopying(CommandTape& tape, SyntaxTree::Node node, int mode = 0);
   void generateReturnExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCallExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateInlineArgCallExpression(CommandTape& tape, SyntaxTree::Node node);
//   void generateInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message);
//   void generateVariadicInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message);
//   void generateBoxing(CommandTape& tape, SyntaxTree::Node node);
//   void generateFieldBoxing(CommandTape& tape, SyntaxTree::Node node, int offset);
//   void generateBoxingExpression(CommandTape& tape, SyntaxTree::Node node, int mode = 0);
   void generateInitializingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
//   void generateStructExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateObject(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode = 0);
   void generateDebugInfo(CommandTape& tape, SyntaxTree::Node current);
   void generateCodeBlock(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);
   void generateCreating(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope);

   void generateMethod(CommandTape& tape, SyntaxTree::Node node, ref_t sourcePathRef);
   void generateMethodDebugInfo(CommandTape& tape, SyntaxTree::Node node);

   void importCode(CommandTape& tape, ImportScope& scope, bool withBreakpoints);

////   void generateTemplateMethods(CommandTape& tape, SNode root);

public:
   pos_t writeSourcePath(_Module* debugModule, ident_t path);
   int writeString(ident_t path);

   void generateClass(CommandTape& tape, SNode root, ref_t reference, pos_t sourcePathBookmark, bool(*cond)(LexicalType));
//   void generateInitializer(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument);
//   void generateInitializer(CommandTape& tape, ref_t reference, SNode root);
   void generateSymbol(CommandTape& tape, SNode root/*, bool isStatic*/, pos_t sourcePathBookmark);
//   void generateConstantList(SNode node, _Module* module, ref_t reference);
//   void generateConstantMember(MemoryWriter& writer, LexicalType type, ref_t argument);

   void saveTape(CommandTape& tape, _ModuleScope& scope);

   int registerImportInfo(_Memory* section, _Module* sour, _Module* dest)
   {
      imports.add(ImportScope(section, sour, dest));

      return imports.Count();
   }
//   void clear()
//   {
//      _strings.clear();
//      imports.clear();
//   }
};

//bool isSimpleObjectExpression(SyntaxTree::Node node, bool ignoreFields = false);

} // _ELENA_

#endif // bcwriterH
