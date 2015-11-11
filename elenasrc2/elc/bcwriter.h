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
   //         bool       out;
   //         ref_t      subject;
   //         ObjectInfo info;
         int size;
         int offset;
   //
         ParamInfo()
         {
   //            subject = 0;
            size = 0;
            offset = 0;
   //            out = false;
         }
      };
      //
      //      struct OutputInfo
      //      {
      //         int        subject;
      //         int        offset;
      //         ObjectInfo target;
      //
      //         OutputInfo()
      //         {
      //            offset = subject = 0;
      //         }
      //      };
      
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
   void writeBreakpoint(ByteCodeIterator& it, MemoryWriter* debug);

   void writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings);
   void writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t className, int flags);
   void writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t symbolName);
   void writeProcedureDebugInfo(MemoryWriter* writer, ref_t sourceNameRef);
   void writeDebugInfoStopper(MemoryWriter* debug);

   void writeProcedure(ByteCodeIterator& it, Scope& scope);
   void writeVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope);
//   void writeAction(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule);
   void writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef);
//   void writeClassHandler(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule);
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
//   void declareSwitchBlock(CommandTape& tape);
//   void declareSwitchOption(CommandTape& tape);
   void declareTry(CommandTape& tape);
   void declareCatch(CommandTape& tape);
//   void declareAlt(CommandTape& tape);
////   void declarePrimitiveCatch(CommandTape& tape);

   void declareLocalInfo(CommandTape& tape, ident_t localName, int level);
   void declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
   void declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame);
//   void declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level);
   void declareSelfInfo(CommandTape& tape, int level);
   void declareMessageInfo(CommandTape& tape, ref_t nameRef);
   void declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType);
////   void removeLastBreakpoint(CommandTape& tape);
////   void declareStatement(CommandTape& tape);
   void declareBlock(CommandTape& tape);
//
//   void tryEmbeddable(CommandTape& tape);
//   void endEmbeddable(CommandTape& tape);

   void newFrame(CommandTape& tape);
   void newStructure(CommandTape& tape, int size, ref_t reference);
   void newDynamicStructure(CommandTape& tape, int itemSize);
   void newDynamicWStructure(CommandTape& tape);
   void newDynamicNStructure(CommandTape& tape);

   void newObject(CommandTape& tape, int fieldCount, ref_t reference);
//   void newVariable(CommandTape& tape, ref_t reference, ObjectInfo field);
   void newDynamicObject(CommandTape& tape);

//   void loadPrimitive(CommandTape& tape, ref_t reference);
//   void loadStatic(CommandTape& tape, ref_t reference);
//   void loadObject(CommandTape& tape, ObjectInfo object);
//   void pushObject(CommandTape& tape, ObjectInfo object);
//   void saveObject(CommandTape& tape, ObjectInfo object);
   void popObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);
//   void exchange(CommandTape& tape, ObjectInfo object);

   void copyBase(CommandTape& tape, int size);
   void loadBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);
   void initBase(CommandTape& tape, int fieldCount);
   void initObject(CommandTape& tape, int fieldCount, LexicalType sourceType, ref_t sourceArgument = 0);
   void saveBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument = 0);

   void boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing = false);
////   void boxArgList(CommandTape& tape, ref_t vmtReference);
////   void unboxArgList(CommandTape& tape);
//
   void releaseObject(CommandTape& tape, int count = 1);
//   void releaseArgList(CommandTape& tape);
//
//   void setMessage(CommandTape& tape, ref_t message);
//   void setSubject(CommandTape& tape, ref_t subject);

   void callMethod(CommandTape& tape, int vmtOffset, int paramCount);
   void callRoleMessage(CommandTape& tape, int paramCount);
   void callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message, bool withValidattion = true);
   void callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
//   void typecast(CommandTape& tape);

//   void doGenericHandler(CommandTape& tape);
   void resend(CommandTape& tape);
//   void resend(CommandTape& tape, ObjectInfo object, int dispatchIndex = 0);
//   void resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
   void callExternal(CommandTape& tape, ref_t functionReference, int paramCount);

////   int declareLabel(CommandTape& tape);
//   void jumpIfEqual(CommandTape& tape, ref_t ref);
   void jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, bool jumpToEnd = false);
////   void jumpIfNotEqualN(CommandTape& tape, int value);
//   void jump(CommandTape& tape, bool previousLabel = false);
//
//   void throwCurrent(CommandTape& tape);

   void tryLock(CommandTape& tape);
   void freeLock(CommandTape& tape);

   void gotoEnd(CommandTape& tape, PseudoArg label);

   void selectByIndex(CommandTape& tape, ref_t r1, ref_t r2);
   void selectByAcc(CommandTape& tape, ref_t r1, ref_t r2);

//   void freeVirtualStack(CommandTape& tape, int count);

   void insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size);
   void updateStackAlloc(ByteCodeIterator it, int size);

//   ByteCodeIterator insertCommand(ByteCodeIterator it, CommandTape& tape, ByteCode command, int argument);
//
//   void trimTape(ByteCodeIterator it, CommandTape& tape);

////   void setLabel(CommandTape& tape);
   void endCatch(CommandTape& tape);
//   void endAlt(CommandTape& tape);
//   void endPrimitiveCatch(CommandTape& tape);
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
////   void exitStaticSymbol(CommandTape& tape, ref_t staticReference);
////   void endSwitchOption(CommandTape& tape);
////   void endSwitchBlock(CommandTape& tape);
//
////   void copy(CommandTape& tape);
   void assignBaseTo(CommandTape& tape, LexicalType target, int offset = 0);

   void assignInt(CommandTape& tape, LexicalType target, int offset);
   void assignLong(CommandTape& tape, LexicalType target, int offset);
//   void assignShort(CommandTape& tape, LexicalType target, int offset);
//   void assignByte(CommandTape& tape, LexicalType target, int offset);
   void saveInt(CommandTape& tape, LexicalType target, int argument);
//   void loadInt(CommandTape& tape, ObjectInfo target);
//   void saveReal(CommandTape& tape, ObjectInfo target);
//   void copyInt(CommandTape& tape, int offset);
//   void copyShort(CommandTape& tape, int offset);
//   void copyStructure(CommandTape& tape, int offset, int size);
//   void copySubject(CommandTape& tape);
   void saveSubject(CommandTape& tape);
//   void loadSymbolReference(CommandTape& tape, ref_t reference);
//   void saveIntConstant(CommandTape& tape, int value);
   void invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef);
   void doIntOperation(CommandTape& tape, int operator_id);
   void doLongOperation(CommandTape& tape, int operator_id);
   void doRealOperation(CommandTape& tape, int operator_id);
//   //void doLiteralOperation(CommandTape& tape, int operator_id);
//   void doArrayOperation(CommandTape& tape, int operator_id);
//   void doIntArrayOperation(CommandTape& tape, int operator_id);

   void translateBreakpoint(CommandTape& tape, SyntaxTree::Node node);

   void pushObject(CommandTape& tape, LexicalType type, ref_t argument = 0);
   void loadObject(CommandTape& tape, LexicalType type, ref_t argument = 0);
   void saveObject(CommandTape& tape, LexicalType type, ref_t argument);

   void saveExternalParameters(CommandTape& tape, ExternalScope& externalScope);
   void unboxCallParameters(CommandTape& tape, SyntaxTree::Node node);

//   void copyObject(CommandTape& tape, LexicalType type, ref_t size, size_t offset);

   void pushObject(CommandTape& tape, SyntaxTree::Node node);
   void loadObject(CommandTape& tape, SyntaxTree::Node node);

   void translateBoolOperation(CommandTape& tape, SyntaxTree::Node node);
   void translateNilOperation(CommandTape& tape, SyntaxTree::Node node);
   void translateOperation(CommandTape& tape, SyntaxTree::Node node);

   void translateResendingExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateExternalArguments(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope);
   void translateExternalCall(CommandTape& tape, SyntaxTree::Node node);
   void translateCall(CommandTape& tape, SyntaxTree::Node node);

   void translateLooping(CommandTape& tape, SyntaxTree::Node node);
   void translateBranching(CommandTape& tape, SyntaxTree::Node node);
   void translateAssigningExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateReturnExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateCallExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateBoxingExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateNestedExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateStructExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateObjectExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateExpression(CommandTape& tape, SyntaxTree::Node node);
   void translateCodeBlock(CommandTape& tape, SyntaxTree::Node node);

   void translateTree(CommandTape& tape, MemoryDump& dump);

   void save(CommandTape& tape, _Module* module, _Module* debugModule, ref_t sourceRef);
};

} // _ELENA_

#endif // bcwriterH
