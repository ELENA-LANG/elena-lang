//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef bcwriterH
#define bcwriterH 1

#include "bytecode.h"
#include "syntaxtree.h"

namespace _ELENA_
{

// --- ByteCodeWriter class ---
class ByteCodeWriter
{
   struct Scope
   {
      ref_t         sourceRef;
      MemoryWriter* vmt;
      MemoryWriter* code;
      MemoryWriter* debug;
      MemoryWriter* debugStrings;

      Scope()
      {
         vmt = code = NULL;
         debug = debugStrings = NULL;
         sourceRef = 0;
      }
   };

   struct ExternalScope
   {
      struct ParamInfo
      {
         int size;
         int offset;

         ParamInfo()
         {
            size = 0;
            offset = 0;
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
   void writeMessageInfo(Scope& scope, DebugSymbol symbol, ref_t nameRef);
   void writeInfo(Scope& scope, DebugSymbol symbol, ident_t className);
   void writeBreakpoint(ByteCodeIterator& it, MemoryWriter* debug);

   void writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings);
   void writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t className, int flags);
   void writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t symbolName);
   void writeProcedureDebugInfo(MemoryWriter* writer, ref_t sourceNameRef);
   void writeDebugInfoStopper(MemoryWriter* debug);

   void writeProcedure(ByteCodeIterator& it, Scope& scope);
   void writeVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope);
   void writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef);
   void writeClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef);

public:
   ref_t writeSourcePath(_Module* debugModule, ident_t path);
   ref_t writeMessage(_Module* debugModule, _Module* module, MessageMap& verbs, ref_t message);

   void declareClass(CommandTape& tape, ref_t reference);
   void declareSymbol(CommandTape& tape, ref_t reference);
   void declareStaticSymbol(CommandTape& tape, ref_t staticReference);
   void declareIdleMethod(CommandTape& tape, ref_t message);
   void declareMethod(CommandTape& tape, ref_t message, bool withPresavedMessage, bool withNewFrame = true);
   void declareExternalBlock(CommandTape& tape);
   void excludeFrame(CommandTape& tape);
   void declareVariable(CommandTape& tape, int value);
   void declareArgumentList(CommandTape& tape, int count);
   int declareLoop(CommandTape& tape/*, bool threadFriendly*/);  // thread friendly means the loop contains safe point
   void declareThenBlock(CommandTape& tape, bool withStackControl = true);
   void declareThenElseBlock(CommandTape& tape);
   void declareElseBlock(CommandTape& tape);
   void declareSwitchBlock(CommandTape& tape);
   void declareSwitchOption(CommandTape& tape);
   void declareTry(CommandTape& tape);
   void declareCatch(CommandTape& tape);
   void declareAlt(CommandTape& tape);

   void declareLocalInfo(CommandTape& tape, ident_t localName, int level);
   void declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className);
   void declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level);
   void declareSelfInfo(CommandTape& tape, int level);
   void declareMessageInfo(CommandTape& tape, ref_t nameRef);
   void declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType);
   void declareBlock(CommandTape& tape);

   void newFrame(CommandTape& tape);
   void newStructure(CommandTape& tape, int size, ref_t reference);
   void newDynamicStructure(CommandTape& tape, int itemSize);
   void newDynamicWStructure(CommandTape& tape);
   void newDynamicNStructure(CommandTape& tape);

   void newObject(CommandTape& tape, int fieldCount, ref_t reference);
   void newVariable(CommandTape& tape, ref_t reference, LexicalType field, ref_t argument = 0);
   void newDynamicObject(CommandTape& tape);

   void popObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);

   void copyBase(CommandTape& tape, int size);
   void loadBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);
   void initBase(CommandTape& tape, int fieldCount);
   void initObject(CommandTape& tape, int fieldCount, LexicalType sourceType, ref_t sourceArgument = 0);
   void saveBase(CommandTape& tape, bool directOperation, LexicalType sourceType, ref_t sourceArgument = 0);
   void loadIndex(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);

   void boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing = false);
   void boxArgList(CommandTape& tape, ref_t vmtReference);
   void unboxArgList(CommandTape& tape);

   void releaseObject(CommandTape& tape, int count = 1);
   void releaseArgList(CommandTape& tape);

   void setSubject(CommandTape& tape, ref_t subject);

   void callMethod(CommandTape& tape, int vmtOffset, int paramCount);
   void callRoleMessage(CommandTape& tape, int paramCount);
   void callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message, bool withValidattion = true);
   void callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);

   void doGenericHandler(CommandTape& tape);
   void resend(CommandTape& tape);
   void resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
   void callExternal(CommandTape& tape, ref_t functionReference, int paramCount);

   void jumpIfEqual(CommandTape& tape, ref_t ref);
   void jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, bool jumpToEnd = false);

   void throwCurrent(CommandTape& tape);

   void tryLock(CommandTape& tape);
   void freeLock(CommandTape& tape);

   void gotoEnd(CommandTape& tape, PseudoArg label);

   void selectByIndex(CommandTape& tape, ref_t r1, ref_t r2);
   void selectByAcc(CommandTape& tape, ref_t r1, ref_t r2);

   void freeVirtualStack(CommandTape& tape, int count);

   void insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size);
   void updateStackAlloc(ByteCodeIterator it, int size);

   void endCatch(CommandTape& tape);
   void endAlt(CommandTape& tape);
   void endThenBlock(CommandTape& tape, bool withStackContro = true);
   void endLoop(CommandTape& tape);
   void endLoop(CommandTape& tape, ref_t comparingRef);
   void endExternalBlock(CommandTape& tape);
   void exitMethod(CommandTape& tape, int count, int reserved, bool withFrame = true);
   void endMethod(CommandTape& tape, int paramCount, int reserved, bool withFrame = true);
   void endIdleMethod(CommandTape& tape);
   void endClass(CommandTape& tape);
   void endSymbol(CommandTape& tape);
   void endStaticSymbol(CommandTape& tape, ref_t staticReference);
   void endSwitchOption(CommandTape& tape);
   void endSwitchBlock(CommandTape& tape);
   void closeFrame(CommandTape& tape);

   void assignBaseTo(CommandTape& tape, LexicalType target, int offset = 0);

   void assignInt(CommandTape& tape, LexicalType target, int offset);
   void assignLong(CommandTape& tape, LexicalType target, int offset);
   void assignShort(CommandTape& tape, LexicalType target, int offset);
   void assignByte(CommandTape& tape, LexicalType target, int offset);
   void saveInt(CommandTape& tape, LexicalType target, int argument);
   void copyInt(CommandTape& tape, int offset);
   void copyShort(CommandTape& tape, int offset);
   void copyStructure(CommandTape& tape, int offset, int size);
   void saveSubject(CommandTape& tape);
   void saveIntConstant(CommandTape& tape, int value);
   void invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef);
   void doIntOperation(CommandTape& tape, int operator_id);
   void doLongOperation(CommandTape& tape, int operator_id);
   void doRealOperation(CommandTape& tape, int operator_id);
   void doArrayOperation(CommandTape& tape, int operator_id);
   void doIntArrayOperation(CommandTape& tape, int operator_id);

   void translateBreakpoint(CommandTape& tape, SyntaxTree::Node node);

   void pushObject(CommandTape& tape, LexicalType type, ref_t argument = 0);
   void loadObject(CommandTape& tape, LexicalType type, ref_t argument = 0);
   void saveObject(CommandTape& tape, LexicalType type, ref_t argument);

   void saveExternalParameters(CommandTape& tape, ExternalScope& externalScope);
   void unboxCallParameters(CommandTape& tape, SyntaxTree::Node node);

   void pushObject(CommandTape& tape, SyntaxTree::Node node);
   void loadObject(CommandTape& tape, SyntaxTree::Node node);

   void generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset);

   void generateBoolOperation(CommandTape& tape, SyntaxTree::Node node);
   void generateNilOperation(CommandTape& tape, SyntaxTree::Node node);
   void generateOperation(CommandTape& tape, SyntaxTree::Node node);
   void generateArrOperation(CommandTape& tape, SyntaxTree::Node node);

   void generateResendingExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateExternalArguments(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope);
   void generateExternalCall(CommandTape& tape, SyntaxTree::Node node);
   void generateInternalCall(CommandTape& tape, SyntaxTree::Node node);
   void generateCall(CommandTape& tape, SyntaxTree::Node node);

   void generateLocking(CommandTape& tape, SyntaxTree::Node node);
   void generateTrying(CommandTape& tape, SyntaxTree::Node node);
   void generateAlt(CommandTape& tape, SyntaxTree::Node node);
   void generateLooping(CommandTape& tape, SyntaxTree::Node node);
   void generateBranching(CommandTape& tape, SyntaxTree::Node node);
   void generateSwitching(CommandTape& tape, SyntaxTree::Node node);
   void generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateReturnExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateThrowExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateCallExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateBoxingExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateNestedExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateStructExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateObjectExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateExpression(CommandTape& tape, SyntaxTree::Node node);
   void generateCodeBlock(CommandTape& tape, SyntaxTree::Node node);

   void generateTree(CommandTape& tape, MemoryDump& dump);

   void save(CommandTape& tape, _Module* module, _Module* debugModule, ref_t sourceRef);
};

} // _ELENA_

#endif // bcwriterH
