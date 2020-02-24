//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

using namespace _ELENA_;

constexpr auto STACKOP_MODE      = 0x0001;
constexpr auto BOOL_ARG_EXPR     = 0x0002;

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

inline bool isSubOperation(SNode node)
{
   if (node == lxExpression) {
      return isSubOperation(node.firstChild(lxObjectMask));
   }
   else return test(node.type, lxOpScopeMask);
}

//// check if the node contains only the simple nodes
//
//bool isSimpleObject(SNode node, bool ignoreFields = false)
//{
//   if (test(node.type, lxObjectMask)) {
//      if (node == lxExpression) {
//         if (!isSimpleObjectExpression(node, ignoreFields))
//            return false;
//      }
//      else if (ignoreFields && (node.type == lxField || node.type == lxFieldAddress)) {
//         // ignore fields if required
//      }
//      else if (!test(node.type, lxSimpleMask))
//         return false;
//   }
//
//   return true;
//}
//
//bool _ELENA_::isSimpleObjectExpression(SNode node, bool ignoreFields)
//{
//   if (node == lxNone)
//      return true;
//
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (!isSimpleObject(current, ignoreFields))
//         return false;
//
//      current = current.nextNode();
//   }
//
//   return true;
//}

// --- Auxiliary  ---

void fixJumps(_Memory* code, int labelPosition, Map<int, int>& jumps, int label)
{
   Map<int, int>::Iterator it = jumps.start();
   while (!it.Eof()) {
      if (it.key() == label) {
         (*code)[*it] = labelPosition - *it - 4;
      }
      it++;
   }
}

// --- ByteCodeWriter ---

int ByteCodeWriter :: writeString(ident_t path)
{
   MemoryWriter writer(&_strings);

   int position = writer.Position();

   writer.writeLiteral(path);

   return position;
}

pos_t ByteCodeWriter :: writeSourcePath(_Module* debugModule, ident_t path)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      pos_t sourceRef = debugStringWriter.Position();

      debugStringWriter.writeLiteral(path);

      return sourceRef;
   }
   else return 0;
}

void ByteCodeWriter :: declareInitializer(CommandTape& tape, ref_t reference)
{
   // symbol-begin:
   tape.write(blBegin, bsInitializer, reference);
}

void ByteCodeWriter :: declareSymbol(CommandTape& tape, ref_t reference, ref_t sourcePathRef)
{
   // symbol-begin:
   tape.write(blBegin, bsSymbol, reference);

   if (sourcePathRef != INVALID_REF)
      tape.write(bdSourcePath, sourcePathRef);
}

void ByteCodeWriter :: declareStaticSymbol(CommandTape& tape, ref_t staticReference, ref_t sourcePathRef)
{
   // symbol-begin:

   // peekr static
   // elser procedure-end
   // movr ref
   // pusha

   tape.newLabel();     // declare symbol-end label

   if (sourcePathRef != INVALID_REF)
      tape.write(blBegin, bsSymbol, staticReference);

   tape.write(bdSourcePath, sourcePathRef);

   tape.write(bcPeekR, staticReference | mskStatSymbolRef);
   tape.write(bcElseR, baCurrentLabel, 0);
   tape.write(bcMovR, staticReference | mskLockVariable);
   tape.write(bcPushA);

   tryLock(tape);
   declareTry(tape);

   // check if the symbol was not created while in the lock
   // peekr static
   tape.write(bcPeekR, staticReference | mskStatSymbolRef);
   jumpIfNotEqual(tape, 0, true, true);
}

void ByteCodeWriter :: declareClass(CommandTape& tape, ref_t reference)
{
   // class-begin:
	tape.write(blBegin, bsClass, reference);
}

void ByteCodeWriter :: declareIdleMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef)
{
   // method-begin:
   tape.write(blBegin, bsMethod, message);

   if (sourcePathRef != INVALID_REF)
      tape.write(bdSourcePath, sourcePathRef);
}

void ByteCodeWriter :: declareMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef, int reserved, int allocated, 
   bool withPresavedMessage, bool withNewFrame)
{
   // method-begin:
   //   { pope }?
   //   open
   //   { reserve }?
   //   pusha
   tape.write(blBegin, bsMethod, message);

   if (sourcePathRef != INVALID_REF)
      tape.write(bdSourcePath, sourcePathRef);

   if (withPresavedMessage)
      tape.write(bcPopD);

   if (withNewFrame) {
      if (reserved > 0) {
         // to include new frame header
         tape.write(bcOpen, 3 + reserved);
         tape.write(bcReserve, reserved);
      }
      else tape.write(bcOpen, 1);

      tape.write(bcPushA);

      if (withPresavedMessage)
         tape.write(bcSaveF, -2);

      if (allocated > 0) {
         allocateStack(tape, allocated);
      }
   }
   tape.newLabel();     // declare exit point
}

//void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
//{
//   tape.write(blDeclare, bsBranch);
//}

void ByteCodeWriter :: excludeFrame(CommandTape& tape)
{
   tape.write(bcExclude);
}

void ByteCodeWriter :: includeFrame(CommandTape& tape)
{
   tape.write(bcInclude);
   tape.write(bcSNop);
}

void ByteCodeWriter :: declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className)
{
   if (!emptystr(localName)) {
      tape.write(bdStruct, writeString(localName), level);
      if (!emptystr(className))
         tape.write(bdLocalInfo, writeString(className));
   }
}

void ByteCodeWriter :: declareSelfStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className)
{
   if (!emptystr(localName)) {
      tape.write(bdStructSelf, writeString(localName), level);
      tape.write(bdLocalInfo, writeString(className));
   }
}

void ByteCodeWriter :: declareLocalInfo(CommandTape& tape, ident_t localName, int level)
{
   if (!emptystr(localName))
      tape.write(bdLocal, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdIntLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdLongLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdRealLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdByteArrayLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdShortArrayLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdIntArrayLocal, writeString(localName), level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level)
{
   tape.write(bdParamsLocal, writeString(localName), level);
}

void ByteCodeWriter :: declareSelfInfo(CommandTape& tape, int level)
{
   tape.write(bdSelf, 0, level);
}

void ByteCodeWriter :: declareMessageInfo(CommandTape& tape, ident_t message)
{
   MemoryWriter writer(&_strings);

   tape.write(bdMessage, 0, writer.Position());
   writer.writeLiteral(message);
}

void ByteCodeWriter :: declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType)
{
   if (disp >= 0 || stepType == dsVirtualEnd) {
      tape.write(bcBreakpoint);

      tape.write(bdBreakpoint, stepType, row);
      tape.write(bdBreakcoord, disp, length);
   }
}

void ByteCodeWriter :: declareBlock(CommandTape& tape)
{
   tape.write(blBlock);
}

void ByteCodeWriter :: allocateStack(CommandTape& tape, int count)
{
   // NOTE : due to implementation the prefered way to push 0, is to use pushr nil
   // NOTE : { pushr }n is preferred over alloci, for small n
   switch (count) {
      case 1:
         tape.write(bcPushR);
         break;
      case 2:
         tape.write(bcPushR);
         tape.write(bcPushR);
         break;
      case 3:
         tape.write(bcPushR);
         tape.write(bcPushR);
         tape.write(bcPushR);
         break;
      case 4:
         tape.write(bcPushR);
         tape.write(bcPushR);
         tape.write(bcPushR);
         tape.write(bcPushR);
         break;
      default:
         tape.write(bcAllocI, count);
         break;
   }
}

//void ByteCodeWriter :: declareVariable(CommandTape& tape, int value)
//{
//   // pushn  value
//   tape.write(bcPushN, value);
//}

int ByteCodeWriter :: declareLoop(CommandTape& tape, bool threadFriendly)
{
   // loop-begin

   tape.newLabel();                 // declare loop start label
   tape.setLabel(true);

   int end = tape.newLabel();       // declare loop end label

   if (threadFriendly)
      // snop
      tape.write(bcSNop);

   return end;
}

void ByteCodeWriter :: declareThenBlock(CommandTape& tape)
{
   tape.newLabel();                  // declare then-end label
}

void ByteCodeWriter :: declareThenElseBlock(CommandTape& tape)
{
   tape.newLabel();                  // declare end label
   tape.newLabel();                  // declare else label
}

void ByteCodeWriter :: declareElseBlock(CommandTape& tape)
{
   //   jump end
   // labElse
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();

   //tape.write(bcResetStack);
}

void ByteCodeWriter :: declareSwitchBlock(CommandTape& tape)
{
   tape.newLabel();                  // declare end label
}

void ByteCodeWriter :: declareSwitchOption(CommandTape& tape)
{
   tape.newLabel();                  // declare next option
}

void ByteCodeWriter :: endSwitchOption(CommandTape& tape)
{
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
}

void ByteCodeWriter :: endSwitchBlock(CommandTape& tape)
{
   tape.setLabel();
}

void ByteCodeWriter :: declareTry(CommandTape& tape)
{
   tape.newLabel();                  // declare end-label
   tape.newLabel();                  // declare alternative-label

   // hook labAlt

   tape.write(bcHook, baCurrentLabel);
}

int ByteCodeWriter :: declareSafeTry(CommandTape& tape)
{
   int label = tape.newLabel();                  // declare ret-end-label
   tape.newLabel();                  // declare end-label
   tape.newLabel();                  // declare alternative-label

   // hook labAlt

   tape.write(bcHook, baCurrentLabel);

   return tape.exchangeFirstsLabel(label);
}

void ByteCodeWriter :: endTry(CommandTape& tape)
{
   //   unhook
   tape.write(bcUnhook);
}

void ByteCodeWriter :: declareSafeCatch(CommandTape& tape, SyntaxTree::Node finallyNode, int retLabel, FlowScope& scope)
{
   //   jump labEnd
   tape.write(bcJump, baPreviousLabel);

   // restore the original ret label and return the overridden one
   retLabel = tape.exchangeFirstsLabel(retLabel);

   if (finallyNode != lxNone) {
      // tryRet:
      tape.setPredefinedLabel(retLabel);
      tape.write(bcUnhook);

      // generate finally
      pushObject(tape, lxResult, 0, scope, 0);
      generateCodeBlock(tape, finallyNode, scope);
      popObject(tape, lxResult);

      gotoEnd(tape, baFirstLabel);
   }

   // labErr:
   tape.setLabel();
}

void ByteCodeWriter :: declareCatch(CommandTape& tape)
{
   //   jump labEnd
   // labErr:

   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
}

void ByteCodeWriter :: doCatch(CommandTape& tape)
{
   //   popa
   //   flag
   //   and elMessage
   //   ifn labSkip
   //   load
   //   peeksi 0
   //   callvi 0
   // labSkip:
   //   unhook

   tape.newLabel();

   // HOT FIX: to compensate the unpaired pop
   tape.write(bcPopA);
   tape.write(bcFlag);
   tape.write(bcAnd, elMessage);
   tape.write(bcIfN, baCurrentLabel, 0);
   tape.write(bcLoad);
   tape.write(bcPeekSI, 0);
   tape.write(bcCallVI, 0);

   tape.setLabel();

   tape.write(bcUnhook);
}

void ByteCodeWriter :: declareAlt(CommandTape& tape)
{
   //   unhook
   //   jump labEnd
   // labErr:
   //   unhook

   tape.write(bcUnhook);
   tape.write(bcJump, baPreviousLabel);

   tape.setLabel();

   tape.write(bcUnhook);
}

void ByteCodeWriter :: newFrame(CommandTape& tape, int reserved, int allocated, bool withPresavedMessage)
{
   //   open 1
   //   pusha
   if (reserved > 0) {
      // to include new frame header
      tape.write(bcOpen, 3 + reserved);
      tape.write(bcReserve, reserved);
   }
   else tape.write(bcOpen, 1);

   tape.write(bcPushA);

   if (withPresavedMessage)
      tape.write(bcSaveF, -2);

   if (allocated > 0) {
      allocateStack(tape, allocated);
   }
}

void ByteCodeWriter :: closeFrame(CommandTape& tape, int reserved)
{
   if (reserved > 0) {
      tape.write(bcRestore, 2 + reserved);
   }
   // close
   tape.write(bcClose);
}

void ByteCodeWriter :: newDynamicStructure(CommandTape& tape, int itemSize, ref_t reference)
{
   tape.write(bcCreateN, reference | mskVMTRef, itemSize);
}

void ByteCodeWriter :: newStructure(CommandTape& tape, int size, ref_t reference)
{
   if (reference) {
      // newn size, vmt
      tape.write(bcNewN, reference | mskVMTRef, size);
   }
   else tape.write(bcNew, 0, size);
}

void ByteCodeWriter :: newObject(CommandTape& tape, int fieldCount, ref_t reference)
{
   //   new fieldCount, vmt
   if (reference) {
      tape.write(bcNew, reference | mskVMTRef, fieldCount);
   }
   else tape.write(bcNew, 0, fieldCount);
}

void ByteCodeWriter :: clearObject(CommandTape& tape, int fieldCount)
{
   if (fieldCount > 0) {
      // fillri 0, fieldCount
      tape.write(bcFillRI, 0, fieldCount);
   }
}

void ByteCodeWriter :: clearDynamicObject(CommandTape& tape)
{
   // fillr 0
   tape.write(bcFillR, 0);
}

//void ByteCodeWriter :: initDynamicObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument)
//{
//   tape.write(bcBCopyA);
//   tape.write(bcCount);
//
//   loadObject(tape, sourceType, sourceArgument, 0);
//
//   tape.write(bcDCopy, 0);
//   tape.newLabel();
//   tape.setLabel(true);
//   tape.write(bcXSet);
//   tape.write(bcNext, baCurrentLabel);
//   tape.releaseLabel();
//
//   tape.write(bcACopyB);
//}
//
//void ByteCodeWriter :: newVariable(CommandTape& tape, ref_t reference, LexicalType field, ref_t argument)
//{
//   loadBase(tape, field, argument, 0);
//   newObject(tape, 1, reference);
//   tape.write(bcBSwap);
//   tape.write(bcAXSaveBI, 0);
//   tape.write(bcACopyB);
//}

void ByteCodeWriter :: newDynamicObject(CommandTape& tape, ref_t reference)
{
   // create
   tape.write(bcCreate, reference | mskVMTRef);
}

inline ref_t defineConstantMask(LexicalType type)
{
   switch (type) {
      case lxClassSymbol:
         return mskVMTRef;
      case lxConstantString:
         return mskLiteralRef;
      case lxConstantWideStr:
         return mskWideLiteralRef;
      case lxConstantChar:
         return mskCharRef;
      case lxConstantInt:
         return mskInt32Ref;
      case lxConstantLong:
         return mskInt64Ref;
      case lxConstantReal:
         return mskRealRef;
      case lxMessageConstant:
         return mskMessage;
      case lxExtMessageConstant:
         return mskExtMessage;
      case lxSubjectConstant:
         return mskMessageName;
      case lxConstantList:
         return mskConstArray;
      default:
         return mskConstantRef;
   }
}


void ByteCodeWriter :: unboxArgList(CommandTape& tape, bool arrayMode)
{
   if (arrayMode) {
      // pushn -1
      // len
      // labNext:
      // dec 1
      // push
      // elsen labNext
      tape.write(bcPushN, -1);
      tape.write(bcCount);
      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcDec, 1);
      tape.write(bcPush);
      tape.write(bcElseN, baCurrentLabel, 0);
      tape.releaseLabel();
   }
   else {
      // pusha
      // movn 0
      // labSearch:
      // peek
      // inc 1
      // elser labSearch -1
      // popa

      // dec 1
      // pushn -1

      // labNext:
      // dec 1
      // push
      // elsen labNext 0

      tape.write(bcPushA);
      tape.write(bcMovN, 0);
      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcPeek);
      tape.write(bcInc, 1);
      tape.write(bcElseR, baCurrentLabel, -1);
      tape.releaseLabel();
      tape.write(bcPopA);
      tape.write(bcDec, 1);
      tape.write(bcPushN, -1);

      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcDec, 1);
      tape.write(bcPush);
      tape.write(bcElseN, baCurrentLabel, 0);
      tape.releaseLabel();
   }
}

void ByteCodeWriter :: popObject(CommandTape& tape, LexicalType sourceType)
{
   switch (sourceType) {
      case lxResult:
         // popa
         tape.write(bcPopA);
         break;
//      case lxCurrentMessage:
//         // pope
//         tape.write(bcPopE);
//         break;
   }
}

void ByteCodeWriter :: releaseStack(CommandTape& tape, int count)
{
   // freei n
   //if (count == 1) {
   //   tape.write(bcPop);
   //}
   //else if (count > 1)
      tape.write(bcFreeI, count);
}

void ByteCodeWriter :: releaseArgList(CommandTape& tape)
{
   // pusha
   // labSearch:
   // popa
   // swap
   // elser labSearch -1
   // popa

   tape.write(bcPushA);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcPopA);
   tape.write(bcSwap);
   tape.write(bcElseR, baCurrentLabel, -1);
   tape.releaseLabel();
   tape.write(bcPopA);
}

void ByteCodeWriter :: setSubject(CommandTape& tape, ref_t subject)
{
   // movv subj
   tape.write(bcMovV, getAction(subject));
}

//void ByteCodeWriter :: callMethod(CommandTape& tape, int vmtOffset, int paramCount)
//{
//   // acallvi offs
//
//   tape.write(bcACallVI, vmtOffset);
//   tape.write(bcFreeStack, 1 + paramCount);
//}

void ByteCodeWriter :: resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message)
{
   // jumprm r, m

   tape.write(bcJumpRM, reference | mskVMTMethodAddress, message);
}

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message/*, bool invokeMode, bool withValidattion*/)
{
//   // validate
//   // callrm r, m
//
//   int freeArg;
//   if (invokeMode) {
//      tape.write(bcPop);
//      freeArg = getParamCount(message);
//   }
//   else freeArg = getParamCount(message) + 1;
//
//   if(withValidattion)
//      tape.write(bcValidate);

   tape.write(bcCallRM, reference | mskVMTMethodAddress, message);

//   tape.write(bcFreeStack, freeArg);
}

//void ByteCodeWriter :: callInitMethod(CommandTape& tape, ref_t reference, ref_t message, bool withValidattion)
//{
//   // validate
//   // xcallrm r, m
//
//   if (withValidattion)
//      tape.write(bcValidate);
//
//   tape.write(bcXCallRM, reference | mskVMTMethodAddress, message);
//
//   tape.write(bcFreeStack, getParamCount(message));
//}

void ByteCodeWriter :: callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message/*, bool invokeMode*/)
{
//   int freeArg;
//   if (invokeMode) {
//      tape.write(bcPop);
//      freeArg = getParamCount(message);
//   }
//   else freeArg = getParamCount(message) + 1;

   // vcallrm r, m

   tape.write(bcVCallRM, reference | mskVMTEntryOffset, message);
}

void ByteCodeWriter :: doGenericHandler(CommandTape& tape)
{
   // bsredirect

   tape.write(bcBSRedirect);
}

void ByteCodeWriter :: changeMessageCounter(CommandTape& tape, int paramCount, int flags)
{
   // ; change param count
   // loadfi - 1
   // and ~PARAM_MASK
   // orn OPEN_ARG_COUNT

   tape.write(bcLoadFI, -1);
   tape.write(bcAnd, ~ARG_MASK);
   tape.write(bcOr, paramCount | flags);
}

void ByteCodeWriter :: unboxMessage(CommandTape& tape)
{
   // ; copy the call stack
   // mcount
   //
   // pushn 0
   // labNextParam:
   // movf -2
   // push
   // dec 1
   // elsen labNextParam 0

   tape.write(bcMCount);
   tape.write(bcPushN, -1);
   tape.write(bcMovF, -2);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcPush);
   tape.write(bcDec, 1);
   tape.write(bcElseN, baCurrentLabel, 0);
   tape.releaseLabel();
}

void ByteCodeWriter :: resend(CommandTape& tape)
{
   // jumpvi 0
   tape.write(bcJumpVI);
}

void ByteCodeWriter :: callExternal(CommandTape& tape, ref_t functionReference/*, int paramCount*/)
{
   // callextr ref
   tape.write(bcCallExtR, functionReference | mskImportRef/*, paramCount*/);
}

void ByteCodeWriter :: callCore(CommandTape& tape, ref_t functionReference/*, int paramCount*/)
{
   // callextr ref
   tape.write(bcCallExtR, functionReference | mskNativeCodeRef/*, paramCount*/);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef, bool referenceMode)
{
   if (!referenceMode) {
      tape.write(bcLoad);
      tape.write(bcIfN, baCurrentLabel, comparingRef);
   }
   // ifr then-end, r
   else if (comparingRef == 0) {
      tape.write(bcIfR, baCurrentLabel, 0);
   }
   else tape.write(bcIfR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfLess(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcLoad);
   tape.write(bcLessN, baCurrentLabel, comparingRef);
}

void ByteCodeWriter :: jumpIfNotLess(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcLoad);
   tape.write(bcNotLessN, baCurrentLabel, comparingRef);
}

void ByteCodeWriter :: jumpIfGreater(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcLoad);
   tape.write(bcGreaterN, baCurrentLabel, comparingRef);
}

void ByteCodeWriter :: jumpIfNotGreater(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcLoad);
   tape.write(bcNotGreaterN, baCurrentLabel, comparingRef);
}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, bool referenceMode, bool jumpToEnd)
{
   if (!referenceMode) {
      tape.write(bcLoad);
      tape.write(bcElseN, jumpToEnd ? baFirstLabel : baCurrentLabel, comparingRef);
   }
   // elser then-end, r
   else if (comparingRef == 0) {
      tape.write(bcElseR, jumpToEnd ? baFirstLabel : baCurrentLabel, 0);
   }
   else tape.write(bcElseR, jumpToEnd ? baFirstLabel : baCurrentLabel, comparingRef | mskConstantRef);
}

//////void ByteCodeWriter :: throwCurrent(CommandTape& tape)
//////{
//////   // throw
//////   tape.write(bcThrow);
//////}

void ByteCodeWriter :: gotoEnd(CommandTape& tape, PseudoArg label)
{
   // jump labEnd
   tape.write(bcJump, label);
}

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
}

void ByteCodeWriter :: endSafeCatch(CommandTape& tape)
{
   // labEnd

   tape.setLabel();

   tape.releaseLabel(); // retCatch is aleady set
}

void ByteCodeWriter :: endAlt(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
}

void ByteCodeWriter :: endThenBlock(CommandTape& tape)
{
   // then-end:
   //  scopyf  branch-level

   tape.setLabel();
}

void ByteCodeWriter :: endLoop(CommandTape& tape)
{
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
   tape.releaseLabel();
}

void ByteCodeWriter :: endLoop(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcIfR, baPreviousLabel, comparingRef | mskConstantRef);

   tape.setLabel();
   tape.releaseLabel();
}

//void ByteCodeWriter :: endExternalBlock(CommandTape& tape,  bool idle)
//{
//   if (!idle)
//      tape.write(bcSCopyF, bsBranch);
//
//   tape.write(blEnd, bsBranch);
//}

void ByteCodeWriter :: exitMethod(CommandTape& tape, int count, int reserved, bool withFrame)
{
   // labExit:
   //   restore reserved / nop
   //   close
   //   quitn n / quit
   // end

   tape.setLabel();
   if (withFrame) {
      if (reserved > 0) {
         tape.write(bcRestore, 2 + reserved);
      }
      tape.write(bcClose);
   }

   if (count > 0) {
      tape.write(bcQuitN, count);
   }
   else tape.write(bcQuit);
}

void ByteCodeWriter :: endMethod(CommandTape& tape, int count, int reserved, bool withFrame)
{
   exitMethod(tape, count, reserved, withFrame);

   tape.write(blEnd, bsMethod);
}

void ByteCodeWriter :: endIdleMethod(CommandTape& tape)
{
   // end

   tape.write(blEnd, bsMethod);
}

void ByteCodeWriter :: endClass(CommandTape& tape)
{
   // end:
   tape.write(blEnd, bsClass);
}

void ByteCodeWriter :: endSymbol(CommandTape& tape)
{
   // symbol-end:
   tape.write(blEnd, bsSymbol);
}

void ByteCodeWriter :: endInitializer(CommandTape& tape)
{
   // symbol-end:
   tape.write(blEnd, bsInitializer);
}

void ByteCodeWriter :: endStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // finally block - should free the lock if the exception was thrown
   endTry(tape);
   declareCatch(tape);
   doCatch(tape);

   freeLock(tape);
   releaseStack(tape);
   tape.write(bcPushA);

   // throw
   tape.write(bcThrow);

   endCatch(tape);

   freeLock(tape);
   releaseStack(tape);

   // HOTFIX : contains no symbol ending tag, to correctly place an expression end debug symbol
   // storer static
   tape.write(bcStoreR, staticReference | mskStatSymbolRef);
   tape.setLabel();

   // symbol-end:
   tape.write(blEnd, bsSymbol);
}

void ByteCodeWriter :: writeProcedureDebugInfo(Scope& scope, ref_t sourceRef)
{
   DebugLineInfo symbolInfo(dsProcedure, 0, 0, 0);
   symbolInfo.addresses.source.nameRef = sourceRef;

   scope.debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeCodeDebugInfo(Scope& scope, ref_t sourceRef)
{
   if (scope.debug) {
      DebugLineInfo symbolInfo(dsCodeInfo, 0, 0, 0);
      symbolInfo.addresses.source.nameRef = sourceRef;

      scope.debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
   }
}

void ByteCodeWriter :: writeNewStatement(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsStatement, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeNewBlock(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsVirtualBlock, 0, 0, -1);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(Scope& scope, ident_t localName, int level, int frameLevel)
{
   writeLocal(scope, localName, level, dsLocal, frameLevel);
}

void ByteCodeWriter :: writeInfo(Scope& scope, DebugSymbol symbol, ident_t className)
{
   if (!scope.debug)
      return;

   DebugLineInfo info;
   info.symbol = symbol;
   info.addresses.source.nameRef = scope.debugStrings->Position();

   scope.debugStrings->writeLiteral(className);
   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSelf(Scope& scope, int level, int frameLevel)
{
   if (!scope.debug)
      return;

   DebugLineInfo info;
   info.symbol = dsLocal;
   info.addresses.local.nameRef = scope.debugStrings->Position();

   if (level < 0) {
      scope.debugStrings->writeLiteral(GROUP_VAR);

      level -= frameLevel;
   }
   else scope.debugStrings->writeLiteral(SELF_VAR);

   info.addresses.local.level = level;

   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(Scope& scope, ident_t localName, int level, DebugSymbol symbol, int frameLevel)
{
   if (!scope.debug)
      return;

   if (level < 0) {
      level -= frameLevel;
   }

   DebugLineInfo info;
   info.symbol = symbol;
   info.addresses.local.nameRef = scope.debugStrings->Position();
   info.addresses.local.level = level;

   scope.debugStrings->writeLiteral(localName);
   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeMessageInfo(Scope& scope, DebugSymbol symbol, ident_t message)
{
   if (!scope.debug)
      return;

   ref_t nameRef = scope.debugStrings->Position();
   scope.debugStrings->writeLiteral(message);

   DebugLineInfo info;
   info.symbol = symbol;
   info.addresses.local.nameRef = nameRef;

   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeBreakpoint(ByteCodeIterator& it, MemoryWriter* debug)
{
   // reading breakpoint coordinate
   DebugLineInfo info;

   info.col = 0;
   info.length = 0;
   info.symbol = (DebugSymbol)(*it).Argument();
   info.row = (*it).additional - 1;
   if (peekNext(it) == bdBreakcoord) {
      it++;

      info.col = (*it).argument;
      info.length = (*it).additional;
   }
   // saving breakpoint
   debug->write((char*)&info, sizeof(DebugLineInfo));
}

inline int getNextOffset(ClassInfo::FieldMap::Iterator it)
{
   it++;

   return it.Eof() ? -1 : *it;
}

void ByteCodeWriter :: writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings)
{
   bool structure = test(info.header.flags, elStructureRole);
   int remainingSize = info.size;

   ClassInfo::FieldMap::Iterator it = info.fields.start();
   while (!it.Eof()) {
      if (!emptystr(it.key())) {
         DebugLineInfo symbolInfo(dsField, 0, 0, 0);

         symbolInfo.addresses.field.nameRef = debugStrings->Position();
         if (structure) {
            int nextOffset = getNextOffset(it);
            if (nextOffset == -1) {
               symbolInfo.addresses.field.size = remainingSize;
            }
            else symbolInfo.addresses.field.size = nextOffset - *it;

            remainingSize -= symbolInfo.addresses.field.size;
         }

         debugStrings->writeLiteral(it.key());

         writer->write((void*)&symbolInfo, sizeof(DebugLineInfo));
      }
      it++;
   }
}

void ByteCodeWriter :: writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings,
                                           ident_t className, int flags)
{
   // put place holder if debug section is empty
   if (debug->Position() == 0)
   {
      debug->writeDWord(0);
   }

   IdentifierString bookmark(className);
   debugModule->mapPredefinedReference(bookmark, debug->Position());

   ref_t position = debugStrings->Position();
   if (isWeakReference(className)) {
      IdentifierString fullName(debugModule->Name(), className);

      debugStrings->writeLiteral(fullName.c_str());
   }
   else debugStrings->writeLiteral(className);

   DebugLineInfo symbolInfo(dsClass, 0, 0, 0);
   symbolInfo.addresses.symbol.nameRef = position;
   symbolInfo.addresses.symbol.flags = flags;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, ident_t symbolName)
{
   // put place holder if debug section is empty
   if (debug->Position() == 0)
   {
      debug->writeDWord(0);
   }

   // map symbol debug info, starting the symbol with # to distinsuish from class
   NamespaceName ns(symbolName);
   IdentifierString bookmark(ns, "'#", symbolName + ns.Length() + 1);
   debugModule->mapPredefinedReference(bookmark, debug->Position());

   ref_t position = debugStrings->Position();

   debugStrings->writeLiteral(symbolName);

   DebugLineInfo symbolInfo(dsSymbol, 0, 0, 0);
   symbolInfo.addresses.symbol.nameRef = position;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, bool appendMode)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(module->mapSection(reference | mskSymbolRef, false));

   Scope scope;
   scope.code = &codeWriter;
   scope.appendMode = appendMode;

   // create debug info if debugModule available
   if (debugModule) {
      // initialize debug info writer
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debugStrings = &debugStringWriter;
      scope.debug = &debugWriter;

      // save symbol debug line info
      writeSymbolDebugInfo(debugModule, &debugWriter, &debugStringWriter, module->resolveReference(reference & ~mskAnyRef));

      writeProcedure(it, scope);

      writeDebugInfoStopper(&debugWriter);
   }
   else writeProcedure(it, scope);
}

void ByteCodeWriter :: writeDebugInfoStopper(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsEnd, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: saveTape(CommandTape& tape, _ModuleScope& scope)
{
   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin) {
         ref_t reference = (*it).additional;
         if ((*it).Argument() == bsClass) {
            writeClass(reference, ++it, scope);
         }
         else if ((*it).Argument() == bsSymbol) {
            writeSymbol(reference, ++it, scope.module, scope.debugModule, false);
         }
         else if ((*it).Argument() == bsInitializer) {
            writeSymbol(reference, ++it, scope.module, scope.debugModule, true);
         }
      }
      it++;
   }
}

void ByteCodeWriter :: writeClass(ref_t reference, ByteCodeIterator& it, _ModuleScope& compilerScope)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(compilerScope.mapSection(reference | mskClassRef, false));

   // initialize vmt section writers
   MemoryWriter vmtWriter(compilerScope.mapSection(reference | mskVMTRef, false));

   // initialize attribute section writers
   MemoryWriter attrWriter(compilerScope.mapSection(reference | mskAttributeRef, false));

   vmtWriter.writeDWord(0);                              // save size place holder
   size_t classPosition = vmtWriter.Position();

   // copy class meta data header + vmt size
   MemoryReader reader(compilerScope.mapSection(reference | mskMetaRDataRef, true));
   ClassInfo info;
   info.load(&reader);

   // reset VMT length
   info.header.count = 0;
   for (auto m_it = info.methods.start(); !m_it.Eof(); m_it++) {
      //NOTE : ingnore statically linked methods
      if (!test(m_it.key(), STATIC_MESSAGE))
         info.header.count++;
   }

   vmtWriter.write((void*)&info.header, sizeof(ClassHeader));  // header

   Scope scope;
   //scope.codeStrings = strings;
   scope.code = &codeWriter;
   scope.vmt = &vmtWriter;

   // create debug info if debugModule available
   if (compilerScope.debugModule) {
      MemoryWriter debugWriter(compilerScope.debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(compilerScope.debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debugStrings = &debugStringWriter;
      scope.debug = &debugWriter;

     // save class debug info
      writeClassDebugInfo(compilerScope.debugModule, &debugWriter, &debugStringWriter, compilerScope.module->resolveReference(reference & ~mskAnyRef), info.header.flags);
      writeFieldDebugInfo(info, &debugWriter, &debugStringWriter);

      writeVMT(classPosition, it, scope);

      writeDebugInfoStopper(&debugWriter);
   }
   else writeVMT(classPosition, it, scope);

   // save Static table
   info.staticValues.write(&vmtWriter);

   // save attributes
   info.mattributes.write(&attrWriter);
}

void ByteCodeWriter :: writeVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope)
{
   while (!it.Eof() && (*it) != blEnd) {
      switch (*it)
      {
         case blBegin:
            // create VMT entry
            if ((*it).Argument() == bsMethod) {
               scope.vmt->writeDWord((*it).additional);                     // Message ID
               scope.vmt->writeDWord(scope.code->Position());               // Method Address

               writeProcedure(++it, scope);
            }
            break;
      };
      it++;
   }
   // save the real section size
   (*scope.vmt->Memory())[classPosition - 4] = scope.vmt->Position() - classPosition;
}

void ByteCodeWriter :: writeProcedure(ByteCodeIterator& it, Scope& scope)
{
   if (*it == bdSourcePath) {
      if (scope.debug)
         writeProcedureDebugInfo(scope, (*it).argument);

      it++;
   }
   else if (scope.debug)
      writeProcedureDebugInfo(scope, NULL);

   size_t procPosition = 4;
   if (!scope.appendMode || scope.code->Position() == 0) {
      scope.code->writeDWord(0);                                // write size place holder
      procPosition = scope.code->Position();
   }

   Map<int, int> labels;
   Map<int, int> fwdJumps;
   //Stack<int>    stackLevels;                          // scope stack levels

   int frameLevel = 0;
   int level = 1;
   //int stackLevel = 0;
   while (!it.Eof() && level > 0) {
      // calculate stack level
      //if(*it == bcAllocStack) {
      //   stackLevel += (*it).argument;
      //}
      //else if (*it == bcResetStack) {
      //   stackLevel = stackLevels.peek();
      //}
      //else if (ByteCodeCompiler::IsPush(*it)) {
      //   stackLevel++;
      //}
      //else if (ByteCodeCompiler::IsPop(*it) || *it == bcFreeStack) {
      //   stackLevel -= (/**it == bcPopI || */*it == bcFreeStack) ? (*it).argument : 1;

      //   // clear previous stack level bookmarks when they are no longer valid
      //   while (stackLevels.Count() > 0 && stackLevels.peek() > stackLevel)
      //      stackLevels.pop();
      //}

      // save command
      switch (*it) {
         //case bcFreeStack:
         //case bcAllocStack:
         //case bcResetStack:
         case bcNone:
         case bcNop:
         case blBreakLabel:
            // nop in command tape is ignored (used in replacement patterns)
            break;
         case blBegin:
            level++;
            break;
         case blLabel:
            fixJumps(scope.code->Memory(), scope.code->Position(), fwdJumps, (*it).argument);
            labels.add((*it).argument, scope.code->Position());

            // JIT compiler interprets nop command as a label mark
            scope.code->writeByte(bcNop);

            break;
         //case blDeclare:
         //   if ((*it).Argument() == bsBranch) {
         //      stackLevels.push(stackLevel);
         //   }
         //   break;
         case blEnd:
            /*if ((*it).Argument() == bsBranch) {
               stackLevels.pop();
            }
            else */level--;
            break;
         case blStatement:
            // generate debug exception only if debug info enabled
            if (scope.debug)
               writeNewStatement(scope.debug);

            break;
         case blBlock:
            // generate debug exception only if debug info enabled
            if (scope.debug)
               writeNewBlock(scope.debug);

            break;
         case bcBreakpoint:
            // generate debug exception only if debug info enabled
            if (scope.debug) {
               (*it).save(scope.code);

               if(peekNext(it) == bdBreakpoint)
                  writeBreakpoint(++it, scope.debug);
            }
            break;
         case bdSourcePath:
            writeCodeDebugInfo(scope, (*it).argument);
            break;
         case bdSelf:
            writeSelf(scope, (*it).additional, frameLevel);
            break;
         case bdLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, frameLevel);
            break;
         case bdIntLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntLocalPtr, 0);
            break;
         case bdLongLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsLongLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (const char*)(const char*)_strings.get((*it).Argument()), (*it).additional, dsLongLocalPtr, 0);
            break;
         case bdRealLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsRealLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsRealLocalPtr, 0);
            break;
         case bdByteArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsByteArrayLocal, frameLevel);
            }
            // else it is a primitive variable
            else writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsByteArrayLocalPtr, 0);
            break;
         case bdShortArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsShortArrayLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsShortArrayLocalPtr, 0);
            break;
         case bdIntArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntArrayLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntArrayLocalPtr, 0);
            break;
         case bdParamsLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsParamsLocal, frameLevel);
            break;
         case bdMessage:
            writeMessageInfo(scope, dsMessage, (const char*)_strings.get((*it).additional));
            break;
         case bdStruct:
            /*if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsStructPtr, frameLevel);
            }
            else */writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsStructPtr, 0);

            if (peekNext(it) == bdLocalInfo) {
               it++;
               writeInfo(scope, dsStructInfo, (const char*)_strings.get((*it).Argument()));
            }
            break;
         case bdStructSelf:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsLocalPtr, frameLevel);
            if (peekNext(it) == bdLocalInfo) {
               it++;
               writeInfo(scope, dsStructInfo, (const char*)_strings.get((*it).Argument()));
            }
            break;
         case bcOpen:
            frameLevel = (*it).argument;
            //stackLevel = 0;
            (*it).save(scope.code);
            break;
         case bcPeekFI:
         case bcPushFI:
         case bcStoreFI:
         case bcXSetFI:
         case bcCopyToFI:
         case bcCopyFI:
         case bcPushF:
         case bcMovF:
         case bcCloneF:
         case bcReadToF:
         //case bcAddF:
         //case bcSubF:
         //case bcMulF:
         //case bcDivF:
         //case bcCopyToF:
         //case bcCopyF:
         //case bcBCopyF:
         //case bcBLoadFI:
         case bcLoadFI:
         case bcSaveF:
         case bcSaveFI:
         //case bcELoadFI:
         //case bcESaveFI:
            (*it).save(scope.code, true);
            //if ((*it).predicate == bpBlock) {
            //   scope.code->writeDWord(stackLevels.peek() + (*it).argument);
            //}
            /*else */if ((*it).predicate == bpFrame && (*it).argument < 0) {
               scope.code->writeDWord((*it).argument - frameLevel);
            }
            else scope.code->writeDWord((*it).argument);

            (*it).saveAditional(scope.code);
            break;
         //case bcSCopyF:
         //   (*it).save(scope.code, true);
         //   if ((*it).argument == bsBranch) {
         //      stackLevel = stackLevels.peek();
         //   }
         //   else stackLevel = (*it).additional;

         //   scope.code->writeDWord(stackLevel);
         //   break;
         case bcIfR:
         case bcElseR:
         //case bcIfB:
         case bcElseD:
         case bcIf:
         case bcElse:
         //case bcLess:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcNotLess:
         case bcNotGreater:         
         case bcNotLessN:
         case bcGreaterN:
         case bcNotGreaterN:
         //case bcIfM:
         //case bcElseM:
         //case bcNext:
         case bcJump:
         case bcHook:
         case bcAddress:
         //case bcIfHeap:
            (*it).save(scope.code, true);

            if ((*it).code > MAX_DOUBLE_ECODE)
               scope.code->writeDWord((*it).additional);

            // if forward jump, it should be resolved later
            if (!labels.exist((*it).argument)) {
               fwdJumps.add((*it).argument, scope.code->Position());
               // put jump offset place holder
               scope.code->writeDWord(0);
            }
            // if backward jump
            else scope.code->writeDWord(labels.get((*it).argument) - scope.code->Position() - 4);

            break;
         case bdBreakpoint:
         case bdBreakcoord:
            break; // bdBreakcoord & bdBreakpoint should be ingonored if they are not paired with bcBreakpoint
         default:
            (*it).save(scope.code);
            break;
      }
      if (level == 0)
         break;
      it++;
   }
   // save the real procedure size
   (*scope.code->Memory())[procPosition - 4] = scope.code->Position() - procPosition;

   // add debug end line info
   if (scope.debug)
      writeDebugInfoStopper(scope.debug);
}

//void ByteCodeWriter :: saveInt(CommandTape& tape, LexicalType target, int argument)
//{
//   if (target == lxLocalAddress) {
//      // bcopyf param
//      // nsave
//      tape.write(bcBCopyF, argument);
//      tape.write(bcNSave);
//   }
//   else if (target == lxFieldAddress) {
//      loadBase(tape, target, 0, 0);
//
//      // nsave
//      tape.write(bcNSave);
//   }
//}
//
//void ByteCodeWriter :: saveReal(CommandTape& tape, LexicalType target, int argument)
//{
//   if (target == lxLocalAddress) {
//      // bcopyf param
//      // rsave
//      tape.write(bcBCopyF, argument);
//      tape.write(bcRSave);
//   }
//   else if (target == lxLocal || target == lxSelfLocal/* || target == lxBoxableLocal*/) {
//      // bloadfi param
//      // rsave
//      tape.write(bcBLoadFI, argument, bpFrame);
//      tape.write(bcRSave);
//   }
//   else if (target == lxFieldAddress) {
//      // push 0
//      // push 0
//      // bcopys 0
//      // rsave
//      // bloadfi 1
//      // pope
//      // dcopy target.param
//      // bwrite
//      // pope
//      // dcopy target.param+4
//      // bwrite
//      // popi 2
//      tape.write(bcPushN);
//      tape.write(bcPushN);
//      tape.write(bcBCopyS, 0);
//      tape.write(bcRSave);
//      tape.write(bcBLoadFI, 1, bpFrame);
//      tape.write(bcPopE);
//      tape.write(bcDCopy, argument);
//      tape.write(bcBWrite);
//      tape.write(bcPopE);
//      tape.write(bcDCopy, argument + 4);
//      tape.write(bcBWrite);
//      tape.write(bcPopI, 2);
//   }
//}
//
//void ByteCodeWriter :: saveLong(CommandTape& tape, LexicalType target, int argument)
//{
//   if (target == lxLocalAddress) {
//      // bcopyf param
//      // lsave
//      tape.write(bcBCopyF, argument);
//      tape.write(bcLSave);
//   }
//}
//
//void ByteCodeWriter :: loadIndex(CommandTape& tape, LexicalType target, ref_t sourceArgument)
//{
//   if (target == lxResult) {
//      tape.write(bcNLoad);
//   }
//   else if (target == lxConstantInt) {
//      tape.write(bcDCopy, sourceArgument);
//   }
//}
//
//void ByteCodeWriter :: assignInt(CommandTape& tape, LexicalType target, int offset, bool& accRequired)
//{
//   if (target == lxFieldAddress) {
//
//      if (offset == 0) {
//         // bloadfi 1
//         // ncopy
//
//         tape.write(bcBLoadFI, 1, bpFrame);
//         tape.write(bcNCopy);
//      }
//      else if ((offset & 3) == 0) {
//         // nload
//         // bloadfi 1
//         // nsavei offset / 4
//         tape.write(bcNLoad);
//         tape.write(bcBLoadFI, 1, bpFrame);
//         tape.write(bcNSaveI, offset >> 2);
//      }
//      else {
//         // nload
//         // ecopyd
//         // bloadfi 1
//         // dcopy target.param
//         // bwrite
//
//         tape.write(bcNLoad);
//         tape.write(bcECopyD);
//         tape.write(bcBLoadFI, 1, bpFrame);
//         tape.write(bcDCopy, offset);
//         tape.write(bcBWrite);
//      }
//   }
//   else if (target == lxResultFieldIndex) {
//      // bswap
//      // acopyai
//      // bswap
//      // ncopy
//      tape.write(bcBSwap);
//      tape.write(bcACopyAI, offset);
//      tape.write(bcBSwap);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxLocalAddress) {
//      // bcopyf param
//      // ncopy
//      tape.write(bcBCopyF, offset);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxLocal) {
//      // bloadfi param
//      // ncopy
//      tape.write(bcBLoadFI, offset, bpFrame);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxField) {
//      // bcopya
//      // aloadfi param
//      // aloadai param
//      // ncopyb
//      tape.write(bcBCopyA);
//      tape.write(bcALoadFI, 1, bpFrame);
//      tape.write(bcALoadAI, offset);
//      tape.write(bcNCopyB);
//
//      accRequired = false;
//   }
//   else if (target == lxStaticField) {
//      if (offset > 0) {
//         // bcopya
//         // aloadr param
//         // ncopyb
//         tape.write(bcBCopyA);
//         tape.write(bcALoadR, offset | mskStatSymbolRef);
//         tape.write(bcNCopyB);
//
//         accRequired = false;
//      }
//   }
//}
//
//void ByteCodeWriter :: assignShort(CommandTape& tape, LexicalType target, int offset, bool& accRequired)
//{
//   if (target == lxFieldAddress) {
//      // nload
//      // ecopyd
//      // bloadfi 1
//      // dcopy target.param
//      // bwritew
//      tape.write(bcNLoad);
//      tape.write(bcECopyD);
//      tape.write(bcBLoadFI, 1, bpFrame);
//      tape.write(bcDCopy, offset);
//      tape.write(bcBWriteW);
//   }
//   else if (target == lxLocalAddress) {
//      // bcopyf param
//      // ncopy
//      tape.write(bcBCopyF, offset);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxLocal) {
//      // bloadfi param
//      // ncopy
//      tape.write(bcBLoadFI, offset, bpFrame);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxField) {
//      // bcopya
//      // aloadfi param
//      // aloadai param
//      // ncopyb
//      tape.write(bcBCopyA);
//      tape.write(bcALoadFI, 1, bpFrame);
//      tape.write(bcALoadAI, offset);
//      tape.write(bcNCopyB);
//
//      accRequired = false;
//   }
//   else if (target == lxStaticField) {
//      if (offset > 0) {
//         // bcopya
//         // aloadr param
//         // ncopyb
//         tape.write(bcBCopyA);
//         tape.write(bcALoadR, offset | mskStatSymbolRef);
//         tape.write(bcNCopyB);
//
//         accRequired = false;
//      }
//   }
//}
//
//void ByteCodeWriter :: assignByte(CommandTape& tape, LexicalType target, int offset, bool& accRequired)
//{
//   if (target == lxFieldAddress) {
//      // nload
//      // ecopyd
//      // bloadfi 1
//      // dcopy target.param
//      // bwriteb
//
//      tape.write(bcNLoad);
//      tape.write(bcECopyD);
//      tape.write(bcBLoadFI, 1, bpFrame);
//      tape.write(bcDCopy, offset);
//      tape.write(bcBWriteB);
//   }
//   else if (target == lxLocalAddress) {
//      // bcopyf param
//      // ncopy
//      tape.write(bcBCopyF, offset);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxLocal) {
//      // bloadfi param
//      // ncopy
//      tape.write(bcBLoadFI, offset, bpFrame);
//      tape.write(bcNCopy);
//   }
//   else if (target == lxField) {
//      // bcopya
//      // aloadfi param
//      // aloadai param
//      // ncopyb
//      tape.write(bcBCopyA);
//      tape.write(bcALoadFI, 1, bpFrame);
//      tape.write(bcALoadAI, offset);
//      tape.write(bcNCopyB);
//
//      accRequired = false;
//   }
//   else if (target == lxStaticField) {
//      if (offset > 0) {
//         // bcopya
//         // aloadr param
//         // ncopyb
//         tape.write(bcBCopyA);
//         tape.write(bcALoadR, offset | mskStatSymbolRef);
//         tape.write(bcNCopyB);
//
//         accRequired = false;
//      }
//   }
//}
//
//void ByteCodeWriter :: assignLong(CommandTape& tape, LexicalType target, int offset, bool& accRequired)
//{
//   if (target == lxFieldAddress) {
//      // bloadfi 1
//      tape.write(bcBLoadFI, 1, bpFrame);
//
//      if (offset == 0) {
//         // lcopy
//
//         tape.write(bcLCopy);
//      }
//      else if ((offset & 3) == 0) {
//         // nloadi 0
//         // nsavei offset / 4
//         // nloadi 1
//         // nsavei (offset + 1) / 4
//         tape.write(bcNLoadI, 0);
//         tape.write(bcNSaveI, offset >> 2);
//         tape.write(bcNLoadI, 1);
//         tape.write(bcNSaveI, (offset >> 2) + 1);
//      }
//      else {
//         // dcopy 0
//         // bread
//         // dcopy prm
//         // bwrite
//         // dcopy 4
//         // bread
//         // dcopy prm + 4
//         // bwrite
//         tape.write(bcDCopy, 0);
//         tape.write(bcBRead);
//         tape.write(bcDCopy, offset);
//         tape.write(bcBWrite);
//         tape.write(bcDCopy, 4);
//         tape.write(bcBRead);
//         tape.write(bcDCopy, offset + 4);
//         tape.write(bcBWrite);
//      }
//   }
//   else if (target == lxLocalAddress) {
//      // bcopyf param
//      // lcopy
//      tape.write(bcBCopyF, offset);
//      tape.write(bcLCopy);
//   }
//   else if (target == lxLocal) {
//      // bloadfi param
//      // lcopy
//      tape.write(bcBLoadFI, offset, bpFrame);
//      tape.write(bcLCopy);
//   }
//   else if (target == lxField) {
//      // bcopya
//      // aloadfi param
//      // aloadai param
//      // lcopyb
//      tape.write(bcBCopyA);
//      tape.write(bcALoadFI, 1, bpFrame);
//      tape.write(bcALoadAI, offset);
//      tape.write(bcLCopyB);
//
//      accRequired = false;
//   }
//   else if (target == lxStaticField) {
//      if (offset > 0) {
//         // bcopya
//         // aloadr param
//         // lcopyb
//         tape.write(bcBCopyA);
//         tape.write(bcALoadR, offset | mskStatSymbolRef);
//         tape.write(bcLCopyB);
//
//         accRequired = false;
//      }
//   }
//}
//
//void ByteCodeWriter :: assignStruct(CommandTape& tape, LexicalType target, int offset, int size)
//{
//   if (target == lxFieldAddress) {
//      // bloadfi 1
//      tape.write(bcBLoadFI, 1);
//
//      copyStructureField(tape, 0, offset, size);
//   }
//   else if (target == lxLocalAddress) {
//      // bcopyf param
//      tape.write(bcBCopyF, offset);
//
//      copyStructure(tape, 0, size);
//   }
//   else if (target == lxResultFieldIndex) {
//      // bswap
//      // acopyai
//      // bswap
//      // ncopy
//      tape.write(bcBSwap);
//      tape.write(bcACopyAI, offset);
//      tape.write(bcBSwap);
//
//      copyStructure(tape, 0, size);
//   }
//   else if (target == lxLocal) {
//      // bloadfi param
//      tape.write(bcBLoadFI, offset, bpFrame);
//
//      copyStructure(tape, 0, size);
//   }
//   //else if (target == lxLocalAddress) {
//   //   // bloadfi param
//   //   tape.write(bcBLoadFI, offset, bpFrame);
//
//   //   copyStructure(tape, 0, size);
//   //}
//}
//
//void ByteCodeWriter :: copyStructureField(CommandTape& tape, int sour_offset, int dest_offset, int size)
//{
//   if (size == 4) {
//      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {
//         // nloadi sour_offset
//         // nsavei dest_offset
//         tape.write(bcNLoadI, sour_offset >> 2);
//         tape.write(bcNSaveI, dest_offset >> 2);
//      }
//      else {
//         // dcopy sour_offset
//         // bread
//         // dcopy dest_offset
//         // bwrite
//         tape.write(bcDCopy, sour_offset);
//         tape.write(bcBRead);
//         tape.write(bcDCopy, dest_offset);
//         tape.write(bcBWrite);
//      }
//   }
//   else if (size == 8) {
//      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {
//         // nloadi sour_offset
//         // nsavei dest_offset
//         tape.write(bcNLoadI, sour_offset >> 2);
//         tape.write(bcNSaveI, dest_offset >> 2);
//         // nloadi sour_offset + 1
//         // nsavei dest_offset + 1
//         tape.write(bcNLoadI, (sour_offset >> 2) + 1);
//         tape.write(bcNSaveI, (dest_offset >> 2) + 1);
//      }
//      else {
//         // dcopy sour_offset
//         // bread
//         // dcopy dest_offset
//         // bwrite
//         tape.write(bcDCopy, sour_offset);
//         tape.write(bcBRead);
//         tape.write(bcDCopy, dest_offset);
//         tape.write(bcBWrite);
//         // dcopy sour_offset + 4
//         // bread
//         // dcopy dest_offset + 4
//         // bwrite
//         tape.write(bcDCopy, sour_offset + 4);
//         tape.write(bcBRead);
//         tape.write(bcDCopy, dest_offset + 4);
//         tape.write(bcBWrite);
//
//      }
//
//   }
//   else if (size == 1) {
//      // dcopy sour_offset
//      // breadb
//      // dcopy dest_offset
//      // bwriteb
//      tape.write(bcDCopy, sour_offset);
//      tape.write(bcBReadB);
//      tape.write(bcDCopy, dest_offset);
//      tape.write(bcBWriteB);
//   }
//   else if ((size & 3) == 0) {
//      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {
//
//         // pushn size
//         // dcopy 0
//
//         // labNext:
//         // addn sour_offset
//         // nread
//         // addn dest_offset-sour_offset
//         // nwrite
//         // addn -dest_offset
//         // eloadsi 0
//         // next labNext
//         // pop
//
//         tape.write(bcPushN, size >> 2);
//         tape.write(bcDCopy, 0);
//         tape.newLabel();
//         tape.setLabel(true);
//
//         if (sour_offset != 0)
//            tape.write(bcAddN, sour_offset >> 2);
//
//         tape.write(bcNRead);
//         tape.write(bcAddN, (dest_offset >> 2) - (sour_offset >> 2));
//         tape.write(bcNWrite);
//
//         if (dest_offset != 0)
//            tape.write(bcAddN, -(dest_offset >> 2));
//
//         tape.write(bcELoadSI, 0);
//         tape.write(bcNext, baCurrentLabel);
//         tape.releaseLabel();
//         tape.write(bcPop);
//      }
//      else {
//         // pushn size
//         // dcopy 0
//
//         // labNext:
//         // addn sour_offset
//         // bread
//         // addn dest_offset-sour_offset
//         // bwrite
//         // addn 3-dest_offset
//         // eloadsi 0
//         // next labNext
//         // pop
//
//         tape.write(bcPushN, size);
//         tape.write(bcDCopy, 0);
//         tape.newLabel();
//         tape.setLabel(true);
//
//         if (sour_offset != 0)
//            tape.write(bcAddN, sour_offset);
//
//         tape.write(bcNRead);
//         tape.write(bcAddN, dest_offset - sour_offset);
//         tape.write(bcNWrite);
//
//         if (dest_offset != 0)
//            tape.write(bcAddN, 3-dest_offset);
//
//         tape.write(bcELoadSI, 0);
//         tape.write(bcNext, baCurrentLabel);
//         tape.releaseLabel();
//         tape.write(bcPop);
//      }
//   }
//   else {
//      // pushn size
//      // dcopy 0
//
//      // labNext:
//      // addn sour_offset
//      // breadb
//      // addn dest_offset-sour_offset
//      // bwriteb
//      // addn -dest_offset
//      // eloadsi 0
//      // next labNext
//      // pop
//
//      tape.write(bcPushN, size);
//      tape.write(bcDCopy, 0);
//      tape.newLabel();
//      tape.setLabel(true);
//
//      if (sour_offset != 0)
//         tape.write(bcAddN, sour_offset);
//
//      tape.write(bcBReadB);
//      tape.write(bcAddN, dest_offset - sour_offset);
//      tape.write(bcBWriteB);
//
//      if (dest_offset != 0)
//         tape.write(bcAddN, - dest_offset);
//
//      tape.write(bcELoadSI, 0);
//      tape.write(bcNext, baCurrentLabel);
//      tape.releaseLabel();
//      tape.write(bcPop);
//   }
//}
//
//void ByteCodeWriter :: copyStructure(CommandTape& tape, int offset, int size)
//{
//   // if it is alinged
//   if ((offset & 3) == 0 && (size & 3) == 0) {
//      if (size == 8) {
//         // nloadi offset
//         // nsavei 0
//         // nloadi offset + 1
//         // nsavei 1
//         tape.write(bcNLoadI, (offset >> 2));
//         tape.write(bcNSaveI, 0);
//         tape.write(bcNLoadI, (offset >> 2) + 1);
//         tape.write(bcNSaveI, 1);
//      }
//      else if (size == 12) {
//         // nloadi offset
//         // nsavei 0
//         // nloadi offset + 1
//         // nsavei 1
//         // nloadi offset + 2
//         // nsavei 2
//         tape.write(bcNLoadI, (offset >> 2));
//         tape.write(bcNSaveI, 0);
//         tape.write(bcNLoadI, (offset >> 2) + 1);
//         tape.write(bcNSaveI, 1);
//         tape.write(bcNLoadI, (offset >> 2) + 2);
//         tape.write(bcNSaveI, 2);
//      }
//      else if (size == 16) {
//         // nloadi offset
//         // nsavei 0
//         // nloadi offset + 1
//         // nsavei 1
//         // nloadi offset + 2
//         // nsavei 2
//         // nloadi offset + 3
//         // nsavei 3
//         tape.write(bcNLoadI, (offset >> 2));
//         tape.write(bcNSaveI, 0);
//         tape.write(bcNLoadI, (offset >> 2) + 1);
//         tape.write(bcNSaveI, 1);
//         tape.write(bcNLoadI, (offset >> 2) + 2);
//         tape.write(bcNSaveI, 2);
//         tape.write(bcNLoadI, (offset >> 2) + 3);
//         tape.write(bcNSaveI, 3);
//      }
//      else {
//         // dcopy 0
//         // ecopy count / 4
//         // pushe
//         // labCopy:
//         // esavesi 0
//         // addn (offset / 4)
//         // nread
//         // addn -offset
//         // nwrite
//         // eloadsi
//         // next labCopy
//         // pop
//
//         tape.write(bcDCopy);
//         tape.write(bcECopy, size >> 2);
//         tape.write(bcPushE);
//         tape.newLabel();
//         tape.setLabel(true);
//         tape.write(bcESaveSI);
//         tape.write(bcAddN, offset >> 2);
//         tape.write(bcNRead);
//         tape.write(bcAddN, -(offset >> 2));
//         tape.write(bcNWrite);
//         tape.write(bcELoadSI);
//         tape.write(bcNext, baCurrentLabel);
//         tape.releaseLabel();
//         tape.write(bcPop);
//      }
//   }
//   else {
//      // dcopy 0
//      // ecopy count
//      // pushe
//      // labCopy:
//      // esavesi 0
//      // addn offset
//      // breadb
//      // addn -offset
//      // bwriteb
//      // eloadsi 0
//      // next labCopy
//      // pop
//
//      tape.write(bcDCopy);
//      tape.write(bcECopy, size);
//      tape.write(bcPushE);
//      tape.newLabel();
//      tape.setLabel(true);
//      tape.write(bcESaveSI);
//      tape.write(bcAddN, offset);
//      tape.write(bcBReadB);
//      tape.write(bcAddN, -offset);
//      tape.write(bcBWriteB);
//      tape.write(bcELoadSI);
//      tape.write(bcNext, baCurrentLabel);
//      tape.releaseLabel();
//      tape.write(bcPop);
//   }
//}
//
//void ByteCodeWriter :: copyInt(CommandTape& tape, int offset)
//{
//   if (offset != 0) {
//      // dcopy index
//      // bread
//      // dcopye
//
//      tape.write(bcDCopy, offset);
//      tape.write(bcBRead);
//      tape.write(bcDCopyE);
//   }
//   else {
//      // nload
//      tape.write(bcNLoad);
//   }
//
//   // nsave
//   tape.write(bcNSave);
//}
//
//void ByteCodeWriter :: copyShort(CommandTape& tape, int offset)
//{
//   // dcopy index
//   // breadw
//   // dcopye
//   // nsave
//
//   tape.write(bcDCopy, offset);
//   tape.write(bcBReadW);
//   tape.write(bcDCopyE);
//   tape.write(bcNSave);
//}
//
//void ByteCodeWriter :: copyByte(CommandTape& tape, int offset)
//{
//   // dcopy index
//   // breadb
//   // dcopye
//   // nsave
//
//   tape.write(bcDCopy, offset);
//   tape.write(bcBReadB);
//   tape.write(bcDCopyE);
//   tape.write(bcNSave);
//}

void ByteCodeWriter :: saveIntConstant(CommandTape& tape, LexicalType target, int targetArg, int value)
{
   if (target == lxLocalAddress) {
      // xsavef arg, value

      tape.write(bcXSaveF, targetArg, value, bpFrame);
   }
   else throw InternalError("Not yet implemented");

//   // bcopya
//   // dcopy value
//   // nsave
//
//   tape.write(bcBCopyA);
//   tape.write(bcDCopy, value);
//   tape.write(bcNSave);
}

////////void ByteCodeWriter :: invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef)
////////{
////////   // pushr trueRef
////////   // ifr labEnd falseRef
////////   // acopyr falseRef
////////   // asavesi 0
////////   // labEnd:
////////   // popa
////////
////////   tape.newLabel();
////////
////////   tape.write(bcPushR, trueRef | mskConstantRef);
////////   tape.write(bcIfR, baCurrentLabel, falseRef | mskConstantRef);
////////   tape.write(bcACopyR, falseRef | mskConstantRef);
////////   tape.write(bcASaveSI);
////////   tape.setLabel();
////////   tape.write(bcPopA);
////////}
//
//void ByteCodeWriter :: saveSubject(CommandTape& tape)
//{
//   // dcopyverb
//   // pushd
//
//   tape.write(bcDCopyVerb);
//   tape.write(bcPushD);
//}
//
//void ByteCodeWriter :: doRealOperation(CommandTape& tape, int operator_id, int immArg)
//{
//   switch (operator_id) {
//      case SET_OPERATOR_ID:
//         tape.write(bcDCopy, immArg);
//         tape.write(bcRCopy);
//         tape.write(bcRSave);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doIntDirectOperation(CommandTape& tape, int operator_id, int immArg, int indexArg)
//{
//   switch (operator_id) {
//      case ADD_OPERATOR_ID:
//         tape.write(bcDLoadFI, indexArg);
//         tape.write(bcAddN, immArg);
//         tape.write(bcNSave);
//         break;
//      case APPEND_OPERATOR_ID:
//         tape.write(bcAddFI, indexArg, immArg);
//         break;
//      case SUB_OPERATOR_ID:
//         tape.write(bcDLoadFI, indexArg);
//         tape.write(bcAddN, -immArg);
//         tape.write(bcNSave);
//         break;
//      case REDUCE_OPERATOR_ID:
//         tape.write(bcSubFI, indexArg, immArg);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcDLoadFI, indexArg);
//         tape.write(bcMulN, immArg);
//         tape.write(bcNSave);
//         break;
//      //case DIV_MESSAGE_ID:
//      //   tape.write(bcNDiv);
//      //   break;
//      //case AND_MESSAGE_ID:
//      //   tape.write(bcNAnd);
//      //   break;
//      //case OR_MESSAGE_ID:
//      //   tape.write(bcNOr);
//      //   break;
//      //case XOR_MESSAGE_ID:
//      //   tape.write(bcNXor);
//      //   break;
//      //case EQUAL_MESSAGE_ID:
//      //   tape.write(bcNEqual);
//      //   break;
//      //case LESS_MESSAGE_ID:
//      //   tape.write(bcNLess);
//      //   break;
//      case SET_OPERATOR_ID:
//         tape.write(bcSaveFI, indexArg, immArg);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doIndexOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case SHIFTL_OPERATOR_ID:
//         // nshiftl
//         tape.write(bcNShiftL);
//         break;
//         // Note read / write operator is used for bitwise operations
//      case SHIFTR_OPERATOR_ID:
//         // nshiftr
//         tape.write(bcNShiftR);
//         break;
//      case SET_OPERATOR_ID:
//         tape.write(bcNSave);
//         break;
//   }
//}
//
//void ByteCodeWriter :: doIndexOperation(CommandTape& tape, int operator_id, int immArg)
//{
//   switch (operator_id) {
//      // Note read / write operator is used for bitwise operations
//      case SHIFTL_OPERATOR_ID:
//         // shiftln immArg
//         // nsave
//         tape.write(bcShiftLN, immArg);
//         tape.write(bcNSave);
//         break;
//         // Note read / write operator is used for bitwise operations
//      case SHIFTR_OPERATOR_ID:
//         // shiftrn immArg
//         // nsave
//         tape.write(bcShiftRN, immArg);
//         tape.write(bcNSave);
//         break;
//      case ADD_OPERATOR_ID:
//      case APPEND_OPERATOR_ID:
//         tape.write(bcAddN, immArg);
//         tape.write(bcNSave);
//         break;
//      case SUB_OPERATOR_ID:
//      case REDUCE_OPERATOR_ID:
//         tape.write(bcAddN, -immArg);
//         tape.write(bcNSave);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcMulN, immArg);
//         tape.write(bcNSave);
//         break;
//      case AND_OPERATOR_ID:
//         tape.write(bcAndN, immArg);
//         tape.write(bcNSave);
//         break;
//      case OR_OPERATOR_ID:
//         tape.write(bcOrN, immArg);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}

void ByteCodeWriter :: doIntBoolOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id)
   {
      case EQUAL_OPERATOR_ID:
         tape.write(bcNEqual);
         break;
      case LESS_OPERATOR_ID:
         tape.write(bcNLess);
         break;
   }
}

void ByteCodeWriter :: doLongBoolOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id)
   {
      case EQUAL_OPERATOR_ID:
         tape.write(bcLEqual);
         break;
      case LESS_OPERATOR_ID:
         tape.write(bcLLess);
         break;
   }
}

void ByteCodeWriter :: doRealBoolOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id)
   {
      case EQUAL_OPERATOR_ID:
         tape.write(bcREqual);
         break;
      case LESS_OPERATOR_ID:
         tape.write(bcRLess);
         break;
   }
}

void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id, int localOffset)
{
   switch (operator_id) {
      case ADD_OPERATOR_ID:
         // naddf i
         tape.write(bcNAddF, localOffset);
         break;
      case SUB_OPERATOR_ID:
         // nsubf i
         tape.write(bcNSubF, localOffset);
         break;
      case MUL_OPERATOR_ID:
         // nmulf i
         tape.write(bcNMulF, localOffset);
         break;
      case DIV_OPERATOR_ID:
         // ndivf i
         tape.write(bcNDivF, localOffset);
         break;
      case SHIFTL_OPERATOR_ID:
         // nshlf i
         tape.write(bcNShlF, localOffset);
         break;
      case SHIFTR_OPERATOR_ID:
         // nshrf i
         tape.write(bcNShrF, localOffset);
         break;
      case AND_OPERATOR_ID:
         // nandf i
         tape.write(bcNAndF, localOffset);
         break;
      case OR_OPERATOR_ID:
         // norf i
         tape.write(bcNOrF, localOffset);
         break;
      case XOR_OPERATOR_ID:
         // nxorf i
         tape.write(bcNXorF, localOffset);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id, int localOffset, int immValue)
{
   switch (operator_id) {
      case SET_OPERATOR_ID:
         // xsavef i, v
         tape.write(bcXSaveF, localOffset, immValue);
         break;
      case ADD_OPERATOR_ID:
         // xaddf i, v
         tape.write(bcXAddF, localOffset, immValue);
         break;
      case SUB_OPERATOR_ID:
         // xaddf i, -v
         tape.write(bcXAddF, localOffset, -immValue);
         break;
      case MUL_OPERATOR_ID:
         // loadf
         // mul v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcMul, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case DIV_OPERATOR_ID:
         // loadf
         // div v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcDiv, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case SHIFTL_OPERATOR_ID:
         // loadf
         // shl v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcShl, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case SHIFTR_OPERATOR_ID:
         // loadf
         // shr v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcShr, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case AND_OPERATOR_ID:
         // loadf
         // and v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcAnd, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case OR_OPERATOR_ID:
         // loadf
         // or v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcOr, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      case XOR_OPERATOR_ID:
         // loadf
         // xor v
         // savef
         tape.write(bcLoadFI, localOffset);
         tape.write(bcXOr, immValue);
         tape.write(bcSaveF, localOffset);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: doLongOperation(CommandTape& tape, int operator_id, int localOffset)
{
   switch (operator_id) {
      case ADD_OPERATOR_ID:
         // laddf i
         tape.write(bcLAddF, localOffset);
         break;
      case SUB_OPERATOR_ID:
         // lsubf i
         tape.write(bcLSubF, localOffset);
         break;
      case MUL_OPERATOR_ID:
         // lmulf i
         tape.write(bcLMulF, localOffset);
         break;
      case DIV_OPERATOR_ID:
         // ldivf i
         tape.write(bcLDivF, localOffset);
         break;
      case SHIFTL_OPERATOR_ID:
         // lshlf i
         tape.write(bcLShlF, localOffset);
         break;
      case SHIFTR_OPERATOR_ID:
         // lshrf i
         tape.write(bcLShrF, localOffset);
         break;
      case AND_OPERATOR_ID:
         // landf i
         tape.write(bcLAndF, localOffset);
         break;
      case OR_OPERATOR_ID:
         // lorf i
         tape.write(bcLOrF, localOffset);
         break;
      case XOR_OPERATOR_ID:
         // lxorf i
         tape.write(bcLXorF, localOffset);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: doRealOperation(CommandTape& tape, int operator_id, int localOffset)
{
   switch (operator_id) {
      case ADD_OPERATOR_ID:
         // raddf i
         tape.write(bcRAddF, localOffset);
         break;
      case SUB_OPERATOR_ID:
         // rsubf i
         tape.write(bcRSubF, localOffset);
         break;
      case MUL_OPERATOR_ID:
         // rmulf i
         tape.write(bcRMulF, localOffset);
         break;
      case DIV_OPERATOR_ID:
         // rdivf i
         tape.write(bcRDivF, localOffset);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

//void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      // Note read / write operator is used for bitwise operations
//      case SHIFTL_OPERATOR_ID:
//         // nload
//         // nshiftl
//         tape.write(bcNLoad);
//         tape.write(bcNShiftL);
//         break;
//      // Note read / write operator is used for bitwise operations
//      case SHIFTR_OPERATOR_ID:
//         // nload
//         // nshiftr
//         tape.write(bcNLoad);
//         tape.write(bcNShiftR);
//         break;
//      case ADD_OPERATOR_ID:
//      case APPEND_OPERATOR_ID:
//         tape.write(bcNAdd);
//         break;
//      case SUB_OPERATOR_ID:
//      case REDUCE_OPERATOR_ID:
//         tape.write(bcNSub);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcNMul);
//         break;
//      case DIV_OPERATOR_ID:
//         tape.write(bcNDiv);
//         break;
//      case AND_OPERATOR_ID:
//         tape.write(bcNAnd);
//         break;
//      case OR_OPERATOR_ID:
//         tape.write(bcNOr);
//         break;
//      case XOR_OPERATOR_ID:
//         tape.write(bcNXor);
//         break;
//      case EQUAL_OPERATOR_ID:
//         tape.write(bcNEqual);
//         break;
//      case LESS_OPERATOR_ID:
//         tape.write(bcNLess);
//         break;
//      case SET_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id, int immArg)
//{
//   switch (operator_id) {
//      // Note read / write operator is used for bitwise operations
//      case SHIFTL_OPERATOR_ID:
//         // nload
//         // shiftln immArg
//         // nsave
//         tape.write(bcNLoad);
//         tape.write(bcShiftLN, immArg);
//         tape.write(bcNSave);
//         break;
//      // Note read / write operator is used for bitwise operations
//      case SHIFTR_OPERATOR_ID:
//         // nload
//         // shiftrn immArg
//         // nsave
//         tape.write(bcNLoad);
//         tape.write(bcShiftRN, immArg);
//         tape.write(bcNSave);
//         break;
//      case APPEND_OPERATOR_ID:
//         // acc is not set for this operation
//         tape.write(bcACopyB);
//      case ADD_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcAddN, immArg);
//         tape.write(bcNSave);
//         break;
//      case REDUCE_OPERATOR_ID:
//         // acc is not set for this operation
//         tape.write(bcACopyB);
//      case SUB_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcAddN, -immArg);
//         tape.write(bcNSave);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcMulN, immArg);
//         tape.write(bcNSave);
//         break;
//      case AND_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcAndN, immArg);
//         tape.write(bcNSave);
//         break;
//      case OR_OPERATOR_ID:
//         tape.write(bcNLoad);
//         tape.write(bcOrN, immArg);
//         tape.write(bcNSave);
//         break;
//      case SET_OPERATOR_ID:
//         tape.write(bcDCopy, immArg);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doFieldIntOperation(CommandTape& tape, int operator_id, int offset, int immArg)
//{
//   switch (operator_id) {
//         // Note read / write operator is used for bitwise operations
//      case SHIFTL_OPERATOR_ID:
//         // dcopy offset
//         // bread
//         // eswap
//         // shiftln immArg
//         // eswap
//         // bwrite
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcShiftLN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//         // Note read / write operator is used for bitwise operations
//      case SHIFTR_OPERATOR_ID:
//         // dcopy offset
//         // bread
//         // eswap
//         // shiftrn immArg
//         // eswap
//         // bwrite
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcShiftRN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case ADD_OPERATOR_ID:
//      case APPEND_OPERATOR_ID:
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcAddN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case SUB_OPERATOR_ID:
//      case REDUCE_OPERATOR_ID:
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcAddN, -immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcMulN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case AND_OPERATOR_ID:
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcAndN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case OR_OPERATOR_ID:
//         tape.write(bcDCopy, offset);
//         tape.write(bcBRead);
//         tape.write(bcESwap);
//         tape.write(bcOrN, immArg);
//         tape.write(bcESwap);
//         tape.write(bcBWrite);
//         break;
//      case SET_OPERATOR_ID:
//         if ((offset & 3) == 0) {
//            tape.write(bcDCopy, immArg);
//            tape.write(bcNSaveI, offset >> 2);
//         }
//         else {
//            tape.write(bcECopy, immArg);
//            tape.write(bcDCopy, offset);
//            tape.write(bcBWrite);
//         }
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doRealOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case SHIFTL_OPERATOR_ID:
//         tape.write(bcLCopy);
//         break;
//      case ADD_OPERATOR_ID:
//      case APPEND_OPERATOR_ID:
//         tape.write(bcRAdd);
//         break;
//      case SUB_OPERATOR_ID:
//      case REDUCE_OPERATOR_ID:
//         tape.write(bcRSub);
//         break;
//      case MUL_OPERATOR_ID:
//         tape.write(bcRMul);
//         break;
//      case DIV_OPERATOR_ID:
//         tape.write(bcRDiv);
//         break;
//      case EQUAL_OPERATOR_ID:
//         tape.write(bcREqual);
//         break;
//      case LESS_OPERATOR_ID:
//         tape.write(bcRLess);
//         break;
//      case SET_OPERATOR_ID:
//         tape.write(bcRSave);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doArrayOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case REFER_OPERATOR_ID:
//         // bcopya
//         // get
//         tape.write(bcBCopyA);
//         tape.write(bcGet);
//         break;
//      case SET_REFER_OPERATOR_ID:
//         // set
//         tape.write(bcSet);
//         break;
//      // NOTE : read operator is used to define the array length
//      case SHIFTR_OPERATOR_ID:
//         // len
//         // nsave
//         tape.write(bcLen);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}

void ByteCodeWriter :: doArgArrayOperation(CommandTape& tape, int operator_id, int argument)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         // get
         tape.write(bcGet);
         break;
      case LEN_OPERATOR_ID:
         // pusha
         // movn 0
         // labSearch
         // peeksi 0
         // get
         // inc 1
         // elser -1 labSearch
         // freei 1
         // savef arg
         tape.write(bcPushA);
         tape.write(bcMovN, 0);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcPeekSI, 0);
         tape.write(bcGet);
         tape.write(bcInc, 1);
         tape.write(bcElseR, baCurrentLabel, -1);
         tape.releaseLabel();
         tape.write(bcFreeI, 1);
         tape.write(bcSaveF, argument);

         break;
//      case SET_REFER_OPERATOR_ID:
//         // xset
//         tape.write(bcXSet);
//         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doArgArrayOperation(CommandTape& tape, int operator_id, int argument, int immValue)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         // geti imm
         tape.write(bcGetI, immValue);
         break;
      //      case SET_REFER_OPERATOR_ID:
      //         // xset
      //         tape.write(bcXSet);
      //         break;
      default:
         break;
   }
}

//void ByteCodeWriter :: doIntArrayOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case REFER_OPERATOR_ID:
//         // nread
//         // dcopye
//         // nsave
//         tape.write(bcNRead);
//         tape.write(bcDCopyE);
//         tape.write(bcNSave);
//         break;
//      case SET_REFER_OPERATOR_ID:
//         // nloade
//         // nwrite
//         tape.write(bcNLoadE);
//         tape.write(bcNWrite);
//         break;
//      //case SETNIL_REFER_MESSAGE_ID:
//      //   // ecopy 0
//      //   // nwrite
//      //   tape.write(bcECopy, 0);
//      //   tape.write(bcNWrite);
//      //   break;
//      //// NOTE : read operator is used to define the array length
//      case SHIFTR_OPERATOR_ID:
//         // nlen
//         // nsave
//         tape.write(bcNLen);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}
//
//void ByteCodeWriter :: doByteArrayOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case REFER_OPERATOR_ID:
//         // breadb
//         // dcopye
//         // nsave
//         tape.write(bcBReadB);
//         tape.write(bcDCopyE);
//         tape.write(bcNSave);
//         break;
//      case SET_REFER_OPERATOR_ID:
//         // nloade
//         // bwriteb
//         tape.write(bcNLoadE);
//         tape.write(bcBWriteB);
//         break;
//      // NOTE : read operator is used to define the array length
//      case SHIFTR_OPERATOR_ID:
//         // blen
//         // nsave
//         tape.write(bcBLen);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}

void ByteCodeWriter :: doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize, int target, int immValue)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         // movn immValue
         tape.write(bcMovN, immValue);
         doBinaryArrayOperation(tape, operator_id, itemSize, target);
         break;
      case SET_REFER_OPERATOR_ID:
         if ((itemSize & 3) == 0 && (immValue & 3) == 0) {
            // copytoai itemSize
            tape.write(bcCopyToAI, immValue >> 2, itemSize >> 2);
         }
         else {
            // movn immValue
            tape.write(bcMovN, immValue);
            doBinaryArrayOperation(tape, operator_id, itemSize, target);
         }
         break;
   }
}

void ByteCodeWriter :: doArrayImmOperation(CommandTape& tape, int operator_id, int immValue)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         // geti imm
         tape.write(bcGetI, immValue);
         break;
      case SET_REFER_OPERATOR_ID:
         // seti imm
         tape.write(bcSetI, immValue);
         break;
   }
}

inline void divIndex(CommandTape& tape, int itemSize)
{
   if (itemSize == 128) {
      tape.write(bcShr, 7);
   }
   else if (itemSize == 64) {
      tape.write(bcShr, 6);
   }
   else if (itemSize == 32) {
      tape.write(bcShr, 5);
   }
   else if (itemSize == 16) {
      tape.write(bcShr, 4);
   }
   else if (itemSize == 8) {
      tape.write(bcShr, 3);
   }
   else if (itemSize == 4) {
      tape.write(bcShr, 2);
   }
   else if (itemSize == 2) {
      tape.write(bcShr, 1);
   }
   else if (itemSize == 1) {
   }
   else tape.write(bcDiv, itemSize);

}

void ByteCodeWriter :: doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize, int target)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         switch (itemSize) {
            case 1:
               // read
               // and 0FFh
               // savef target
               tape.write(bcRead);
               tape.write(bcAnd, 0xFF);
               tape.write(bcSaveF, target);
               break;
            case 2:
               // read
               // and 0FFFFh
               // savef target
               tape.write(bcRead);
               tape.write(bcAnd, 0xFFFF);
               tape.write(bcSaveF, target);
               break;
            case 4:
               // read
               // savef target
               tape.write(bcRead);
               tape.write(bcSaveF, target);
               break;
            default:
               // NOTE : operations with the stack should always be aligned for efficiency
               // readtof target itemSize / 4
               tape.write(bcReadToF, target, align(itemSize, 4) >> 2);
               break;
         }
         break;
      case SET_REFER_OPERATOR_ID:
         if ((itemSize & 3) == 0) {
            // copyto itemSize
            tape.write(bcCopyTo, itemSize >> 2);
         }
         // xwrite itemSize
         else tape.write(bcXWrite, itemSize);
         break;
      case LEN_OPERATOR_ID:
         // len
         // div itemSize
         // savef
         tape.write(bcLen);
         divIndex(tape, itemSize);
         tape.write(bcSaveF, target);
         break;
   }
}

void ByteCodeWriter :: doArrayOperation(CommandTape& tape, int operator_id, int target)
{
   switch (operator_id) {
      case REFER_OPERATOR_ID:
         // get
         tape.write(bcGet);
         break;
      case SET_REFER_OPERATOR_ID:
         // set
         tape.write(bcSet);
         break;
      case LEN_OPERATOR_ID:
         // count
         // savef target
         tape.write(bcCount);
         tape.write(bcSaveF, target);
         break;
   }
}

//void ByteCodeWriter :: doShortArrayOperation(CommandTape& tape, int operator_id)
//{
//   switch (operator_id) {
//      case REFER_OPERATOR_ID:
//         // wread
//         // dcopye
//         // nsave
//         tape.write(bcWRead);
//         tape.write(bcDCopyE);
//         tape.write(bcNSave);
//         break;
//      case SET_REFER_OPERATOR_ID:
//         // nloade
//         // wwrite
//         tape.write(bcNLoadE);
//         tape.write(bcWWrite);
//         break;
//      // NOTE : read operator is used to define the array length
//      case SHIFTR_OPERATOR_ID:
//         // wlen
//         // nsave
//         tape.write(bcWLen);
//         tape.write(bcNSave);
//         break;
//      default:
//         break;
//   }
//}

void ByteCodeWriter :: selectByIndex(CommandTape& tape, ref_t r1, ref_t r2)
{
   tape.write(bcSelect, r1 | mskConstantRef, r2 | mskConstantRef);
}

void ByteCodeWriter :: selectByAcc(CommandTape& tape, ref_t r1, ref_t r2)
{
   tape.write(bcXSelectR, r1 | mskConstantRef, r2 | mskConstantRef);
}

void ByteCodeWriter :: tryLock(CommandTape& tape)
{
   // labWait:
   // snop
   // trylock
   // elsen labWait

   int labWait = tape.newLabel();
   tape.setLabel(true);
   tape.write(bcSNop);
   tape.write(bcTryLock);
   tape.write(bcElseN, labWait, 0);
   tape.releaseLabel();
}

void ByteCodeWriter :: freeLock(CommandTape& tape)
{
   // freelock
   tape.write(bcFreeLock);
}

inline SNode getChild(SNode node, size_t index)
{
   SNode current = node.firstChild(lxObjectMask);

   while (index > 0 && current != lxNone) {
      current = current.nextNode(lxObjectMask);

      index--;
   }

   return current;
}

//inline bool existNode(SNode node, LexicalType type)
//{
//   return SyntaxTree::findChild(node, type) == type;
//}

inline size_t countChildren(SNode node)
{
   size_t counter = 0;
   SNode current = node.firstChild(lxObjectMask);

   while (current != lxNone) {
      current = current.nextNode(lxObjectMask);

      counter++;
   }

   return counter;
}

void ByteCodeWriter :: translateBreakpoint(CommandTape& tape, SNode node, FlowScope& scope/*, bool ignoreBranching*/)
{
   SNode terminal = node.nextNode(lxTerminalMask);
   if (terminal == lxNone) {
      SNode current = node.nextNode();
      if (current == lxType)
         terminal = current.firstChild(lxTerminalMask);
   }

   if (terminal != lxNone) {
      if (!scope.debugBlockStarted) {
         declareBlock(tape);

         scope.debugBlockStarted = true;
      }

      declareBreakpoint(tape,
         terminal.findChild(lxRow).argument,
         terminal.findChild(lxCol).argument - 1,
         terminal.findChild(lxLength).argument, node.argument);
   }
   //else declareBreakpoint(tape, 0, 0, 0, node.argument);

 //  if (node != lxNone) {
//      // try to find the terminal symbol
//      SNode terminal = node;
//      while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
//         terminal = terminal.firstChild(lxObjectMask);
//      }
//
//      if (terminal == lxNone) {
//         terminal = node.findNext(lxObjectMask);
//         while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
//            terminal = terminal.firstChild(lxObjectMask);
//         }
//         // HOTFIX : use idle node
//         if (terminal == lxNone && node.nextNode() == lxIdle) {
//            terminal = node.nextNode();
//            while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
//               terminal = terminal.firstChild(lxObjectMask);
//            }
//         }
//      }
//
//      if (terminal != lxNone) {
//      }
//
//      node = lxIdle; // comment breakpoint out to prevent duplicate compilation
//
//      if (ignoreBranching) {
//         // generate debug step scope only if requiered
//         SNode next = node.nextNode();
//
//         switch (node.nextNode()) {
//            case lxDirectCalling:
//            case lxSDirectCalling:
//            case lxCalling:
//            case lxInlineArgCall:
//            case lxNone:
//               return true;
//            default:
//               return false;
//         }
//      }
//      else return true;
  // }
  // else return false;
}

void ByteCodeWriter :: pushObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode)
{
   switch (type)
   {
      case lxSymbolReference:
         tape.write(bcCallR, argument | mskSymbolRef);
         tape.write(bcPushA);

         scope.clear();
         break;
      case lxConstantString:
      case lxConstantWideStr:
      case lxClassSymbol:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxExtMessageConstant:
      case lxSubjectConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcPushR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
//      case lxBoxableLocal:
         // pushfi index
         tape.write(bcPushFI, argument, bpFrame);
         break;
      case lxClassRef:
         // class
         tape.write(bcClass);
         tape.write(bcPushA);
         break;
      case lxLocalAddress:
         // pushf n
         tape.write(bcPushF, argument);
         break;
      case lxBlockLocalAddr:
         // pushf n
         tape.write(bcPushF, argument, bpFrame);
         break;
      case lxCurrent:
         // pushsi index
         tape.write(bcPushSI, argument);
         break;
      case lxField:
         // pushai index
//         if ((int)argument < 0) {
//            tape.write(bcPushA);
//         }
         /*else */tape.write(bcPushAI, argument);
         break;
      case lxStaticConstField:
//         if ((int)argument > 0) {
//            // aloadr r
//            // pusha
//            tape.write(bcALoadR, argument | mskStatSymbolRef);
//            tape.write(bcPushA);
//         }
//         else {
         loadObject(tape, type, argument, scope, 0);
         tape.write(bcPushA);
//            // aloadai -offset
//            // pusha
//            tape.write(bcALoadAI, argument);
//            tape.write(bcPushA);
//         }
         break;
      case lxStaticField:
//         if ((int)argument > 0) {
            // peekr r
            // pusha
            loadObject(tape, type, argument, scope, 0);
            tape.write(bcPushA);
//         }
//         else {
//            // aloadai -offset
//            // aloadai 0
//            // pusha
//            tape.write(bcALoadAI, argument);
//            tape.write(bcALoadAI, 0);
//            tape.write(bcPushA);
//         }
         break;
//      case lxBlockLocal:
//         // pushfi index
//         tape.write(bcPushFI, argument, bpBlock);
//         break;
      case lxNil:
         // pushn 0
         tape.write(bcPushR, 0);
         break;
      case lxStopper:
         // pushn -1
         tape.write(bcPushN, -1);
         break;
      case lxResult:
         // pusha
         tape.write(bcPushA);
         break;
//      case lxResultField:
//         // pushai reference
//         tape.write(bcPushAI, argument);
//         break;
//      case lxCurrentMessage:
//         // pushe
//         tape.write(bcPushE);
//         break;
      default:
         break;
   }
}

void ByteCodeWriter :: pushIntConstant(CommandTape& tape, int value)
{
   tape.write(bcPushN, value);
}

void ByteCodeWriter :: pushIntValue(CommandTape& tape)
{
   // pushai 0

   tape.write(bcPushAI, 0);
}

void ByteCodeWriter :: loadObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int mode)
{
//   bool basePresaved = test(mode, BASE_PRESAVED);

   if (scope.acc.type == type && scope.acc.arg == argument) {
      // if the agument is already in the register - do nothing
      return;
   }

   switch (type) {
      case lxSymbolReference:
         tape.write(bcCallR, argument | mskSymbolRef);
         type = lxNone; // acc content is undefined
         scope.clear();
         break;
      case lxConstantString:
      case lxConstantWideStr:
      case lxClassSymbol:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxExtMessageConstant:
      case lxSubjectConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcMovR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
////      //case lxBoxableLocal:
         // aloadfi index
         tape.write(bcPeekFI, argument, bpFrame);
         break;
      case lxCurrent:
         // peeksi index
         tape.write(bcPeekSI, argument);
         break;
//////      case lxCurrentField:
//////         // aloadsi index
//////         // aloadai 0
//////         tape.write(bcALoadSI, argument);
//////         tape.write(bcALoadAI, 0);
//////         break;
      case lxNil:
         // acopyr 0
         tape.write(bcMovR, argument);
         break;
      case lxField:
         // geti index
//         if ((int)argument < 0) {
//            tape.write(bcACopyB);
//         }
         /*else */tape.write(bcGetI, argument);
         scope.clear();
         return;
      case lxStaticConstField:
//         if ((int)argument > 0) {
//            // aloadr r
//            tape.write(bcALoadR, argument | mskStatSymbolRef);
//         }
//         else {
            // geti -offset
            tape.write(bcGetI, argument);
            //         }
         scope.clear();
         return;
      case lxStaticField:
//         if ((int)argument > 0) {
            // peekr r
            tape.write(bcPeekR, argument | mskStatSymbolRef);
//         }
//         else {
//            // aloadai -offset
//            // aloadai 0
//            tape.write(bcALoadAI, argument);
//            tape.write(bcALoadAI, 0);
//         }
         scope.clear();
         return;
//      case lxFieldAddress:
//         // aloadfi 1
//         tape.write(bcALoadFI, 1, bpFrame);
//         break;
//      case lxBlockLocal:
//         // aloadfi n
//         tape.write(bcALoadFI, argument, bpBlock);
//         break;
      case lxLocalAddress:
         // acopyf n
         tape.write(bcMovF, argument);
         break;
      case lxClassRef:
         // class
         tape.write(bcClass);
         break;
      case lxBlockLocalAddr:
         // acopyf n
         tape.write(bcMovF, argument, bpFrame);
         break;
//      case lxResultField:
//         // aloadai
//         tape.write(bcALoadAI, argument);
//         break;
//      case lxResultFieldIndex:
//         // acopyai
//         tape.write(bcACopyAI, argument);
//         break;
//      case lxClassRefField:
//         // pushb
//         // bloadfi 1
//         // class
//         // popb
//         if (basePresaved)
//            tape.write(bcPushB);
//
//         tape.write(bcBLoadFI, 1, bpFrame);
//         tape.write(bcClass);
//
//         if (basePresaved)
//            tape.write(bcPopB);
//         break;
      case lxResult:
         break;
      default:
         throw InternalError("Not yet implemented"); // !! temporal
         return;
   }

   scope.acc.type = type;
   scope.acc.arg = argument;
}

//void ByteCodeWriter :: saveObjectIfChanged(CommandTape& tape, LexicalType type, ref_t argument, int checkLocal, int mode)
//{
//   bool basePresaved = test(mode, BASE_PRESAVED);
//
//   // bloadfi checkLocal
//   if (basePresaved)
//      tape.write(bcPushB);
//
//   loadBase(tape, lxLocal, checkLocal, ACC_PRESAVED);
//
//   // ifb labSkip
//   /*int labSkip = */tape.newLabel();
//   tape.write(bcIfB, baCurrentLabel);
//
//   saveObject(tape, type, argument);
//
//   // labSkip:
//   tape.setLabel();
//
//   if (basePresaved)
//      tape.write(bcPopB);
//}

void ByteCodeWriter :: saveObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type)
   {
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
//      //case lxBoxableLocal:
         // storefi index
         tape.write(bcStoreFI, argument, bpFrame);
         break;
      case lxCurrent:
         // storesi index
         tape.write(bcStoreSI, argument);
         break;
      case lxField:
         // set index
         tape.write(bcSetI, argument);
         break;
      case lxStaticField:
//         if ((int)argument > 0) {
            // storer arg
            tape.write(bcStoreR, argument | mskStatSymbolRef);
//         }
//         else {
//            // pusha
//            // aloadai -offset
//            // bcopya
//            // popa
//            // axsavebi 0
//            tape.write(bcPushA);
//            tape.write(bcALoadAI, argument);
//            tape.write(bcBCopyA);
//            tape.write(bcPopA);
//            tape.write(bcAXSaveBI, 0);
//         }
         break;
//      //case lxLocalReference:
//      //   // bcopyf param
//      //   // axsavebi 0
//      //   tape.write(bcBCopyF, argument);
//      //   tape.write(bcAXSaveBI, 0);
//      //   break;
//      case lxBaseField:
//         tape.write(bcASaveBI, argument);
//         break;
      default:
         break;
   }
}

void ByteCodeWriter :: saveObject(CommandTape& tape, SNode node)
{
   if (node == lxExpression) {
      saveObject(tape, node.findSubNodeMask(lxObjectMask));
   }
   else saveObject(tape, node.type, node.argument);
}

void ByteCodeWriter :: loadObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   if (test(node.type, lxOpScopeMask)) {
      generateObject(tape, node, scope);
   }
   else loadObject(tape, node.type, node.argument, scope, mode);

//   if (node.type == lxLocalAddress && test(mode, EMBEDDABLE_EXPR)) {
//      SNode implicitNode = node.findChild(lxImplicitCall);
//      if (implicitNode != lxNone)
//         callInitMethod(tape, implicitNode.findChild(lxTarget).argument, implicitNode.argument, false);
//   }
}

void ByteCodeWriter :: pushObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   pushObject(tape, node.type, node.argument, scope, mode);
}

void assignOpArguments(SNode node, SNode& larg, SNode& rarg)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (larg == lxNone) {
            larg = current;
         }
         else {
            rarg = current;
            break;
         }
      }

      current = current.nextNode();
   }
}

void assignOpArguments(SNode node, SNode& larg, SNode& rarg, SNode& rarg2)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (larg == lxNone) {
            larg = current;
         }
         else if (rarg == lxNone) {
            rarg = current;
         }
         else rarg2 = current;
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: generateNewArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   generateObject(tape, node.firstChild(lxObjectMask), scope, STACKOP_MODE);

   if (node.argument != 0) {
      int size = node.findChild(lxSize).argument;
//
//      if ((int)node.argument < 0) {
//         //HOTFIX : recognize primitive object
//         loadObject(tape, lxNil, 0, 0);
//      }
//      else loadObject(tape, lxClassSymbol, node.argument, 0);

      if (size < 0) {
         newDynamicStructure(tape, -size, node.argument);
         releaseStack(tape);
      }
      else if (size == 0) {
         newDynamicObject(tape, node.argument);
         clearDynamicObject(tape);
         releaseStack(tape);
      }
      else throw InternalError("not yet implemented"); // !! temporal
   }
   else throw InternalError("not yet implemented"); // !! temporal
//   //else {
//   //   loadObject(tape, lxSelfLocal, 1);
//   //   // HOTFIX: -1 indicates the stack is not consumed by the constructor
//   //   callMethod(tape, 1, -1);
//   //}

   scope.clear();
}

void ByteCodeWriter :: generateArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   int freeArgs = 0;
   bool lenMode = node.argument == LEN_OPERATOR_ID;
   bool setMode = (node.argument == SET_REFER_OPERATOR_ID/* || node.argument == SETNIL_REFER_MESSAGE_ID*/);
   bool immIndex = false;
//   //bool assignMode = node != lxArrOp/* && node != lxArgArrOp*/;

   int argument = 0;

   SNode larg, rarg, rarg2;
   assignOpArguments(node, larg, rarg, rarg2);

   if (larg == lxExpression)
      larg = larg.findSubNodeMask(lxObjectMask);
   if (rarg == lxExpression)
      rarg = rarg.findSubNodeMask(lxObjectMask);

   if (lenMode) {
      if (larg == lxLocalAddress) {
         argument = larg.argument;

         generateObject(tape, rarg, scope);
      }
      else throw InternalError("Not yet implemented"); // !! temporal
   }
   else {
      immIndex = rarg == lxConstantInt;

      if (setMode) {
         generateObject(tape, rarg2, scope, STACKOP_MODE);
         freeArgs = 1;
      }
      else if (rarg2 == lxLocalAddress) {
         argument = rarg2.argument;
      }
      // !! temporal
      else if (node.type == lxIntArrOp) {
         throw InternalError("Not yet implemented"); // !! temporal
      }

      if (!test(larg.type, lxOpScopeMask)) {
         if (!immIndex) {
            generateObject(tape, rarg, scope);
            tape.write(bcLoad);
         }

         loadObject(tape, larg.type, larg.argument, scope, 0);
      }
      else throw InternalError("Not yet implemented"); // !! temporal
   }

   if (immIndex) {
      int immValue = rarg.findChild(lxIntValue).argument;
      switch (node.type) {
         case lxIntArrOp:
            doBinaryArrayOperation(tape, node.argument, 4, argument, immValue * 4);
            break;
         case lxByteArrOp:
            doBinaryArrayOperation(tape, node.argument, 1, argument, immValue);
            break;
         case lxShortArrOp:
            doBinaryArrayOperation(tape, node.argument, 2, argument, immValue * 2);
            break;
         case lxBinArrOp:
         {
            int size = node.findChild(lxSize).argument;
            doBinaryArrayOperation(tape, node.argument, size, argument, immValue * size);
            break;
         }
         case lxArrOp:
            doArrayImmOperation(tape, node.argument, immValue);
            break;
         case lxArgArrOp:
            doArgArrayOperation(tape, node.argument, argument, immValue);
            break;
      }
   }
   else {
      switch (node.type) {
         case lxIntArrOp:
            if (!lenMode) {
               // shl 2
               tape.write(bcShl, 2);
            }
            doBinaryArrayOperation(tape, node.argument, 4, argument);
            break;
         case lxByteArrOp:
            doBinaryArrayOperation(tape, node.argument, 1, argument);
            break;
         case lxShortArrOp:
            if (!lenMode) {
               // shl 1
               tape.write(bcShl, 1);
            }
            doBinaryArrayOperation(tape, node.argument, 2, argument);
            break;
         case lxBinArrOp:
         {
            int size = node.findChild(lxSize).argument;
            if (!lenMode) {
               switch (size) {
                  case 1:
                     break;
                  case 2:
                     // shl 1
                     tape.write(bcShl, 1);
                     break;
                  case 4:
                     // shl 2
                     tape.write(bcShl, 2);
                     break;
                  case 8:
                     // shl 3
                     tape.write(bcShl, 3);
                     break;
                  case 16:
                     // shl 4
                     tape.write(bcShl, 4);
                     break;
                  case 32:
                     // shl 5
                     tape.write(bcShl, 5);
                     break;
                  case 64:
                     // shl 6
                     tape.write(bcShl, 6);
                     break;
                  case 128:
                     // shl 6
                     tape.write(bcShl, 128);
                     break;
                  default:
                     break;
               }
            }
            doBinaryArrayOperation(tape, node.argument, size, argument);
            break;
         }
         case lxArrOp:
            doArrayOperation(tape, node.argument, argument);
            break;
         case lxArgArrOp:
            doArgArrayOperation(tape, node.argument, argument);
            break;
      }
   }
   scope.clear();

   if (freeArgs > 0)
      releaseStack(tape, freeArgs);
}

//void ByteCodeWriter :: unboxLocal(CommandTape& tape, SNode larg, SNode rarg)
//{
//   SNode assignNode = larg.findChild(lxAssigning);
//   assignOpArguments(assignNode, larg, rarg);
//
//   loadBase(tape, rarg.type, 0, 0);
//
//   bool dummy = false;
//   if (assignNode.argument == 4) {
//      assignInt(tape, lxFieldAddress, rarg.argument, dummy);
//   }
//   else if (assignNode.argument == 2) {
//      assignLong(tape, lxFieldAddress, rarg.argument, dummy);
//   }
//   else assignStruct(tape, lxFieldAddress, rarg.argument, assignNode.argument);
//}

void ByteCodeWriter :: generateOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   SNode larg;
   SNode rarg;
   assignOpArguments(node, larg, rarg);

   SNode largObj = larg == lxExpression ? larg.findSubNodeMask(lxObjectMask) : larg;
   if (largObj != lxLocalAddress)
      throw new InternalError("Not yet implemented"); // temporal

   SNode rargObj = rarg == lxExpression ? rarg.findSubNodeMask(lxObjectMask) : rarg;

   bool rargImm = rargObj == lxConstantInt;
   if (rargImm && node.type == lxIntOp) {
      int rargVal = rargObj.findChild(lxIntValue).argument;

      doIntOperation(tape, node.argument, largObj.argument, rargVal);
   }
   else {
      loadObject(tape, rarg, scope);

      if (node.type == lxIntOp) {
         doIntOperation(tape, node.argument, largObj.argument);
      }
      else if (node == lxLongOp) {
         doLongOperation(tape, node.argument, largObj.argument);
      }
      else if (node == lxRealOp) {
         doRealOperation(tape, node.argument, largObj.argument);
      }
   }

}

void ByteCodeWriter :: generateBoolOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   int operation = node.argument;
   bool invertSelectMode = false;
   bool invertMode = false;

   switch (node.argument) {
      case NOTEQUAL_OPERATOR_ID:
         invertSelectMode = true;
         break;
      case GREATER_OPERATOR_ID:
         invertMode = true;
         operation = LESS_OPERATOR_ID;
         break;
   }

   SNode larg;
   SNode rarg;
   if (invertMode) {
      assignOpArguments(node, rarg, larg);
   }
   else assignOpArguments(node, larg, rarg);

   generateObject(tape, rarg, scope, STACKOP_MODE);
   generateObject(tape, larg, scope);

   if (node.type == lxIntBoolOp) {
      doIntBoolOperation(tape, operation);
   }
   else if (node.type == lxLongBoolOp) {
      doLongBoolOperation(tape, operation);
   }
   else if (node.type == lxRealBoolOp) {
      doRealBoolOperation(tape, operation);
   }

   if (invertSelectMode) {
      selectByIndex(tape,
         node.findChild(lxIfValue).argument,
         node.findChild(lxElseValue).argument);
   }
   else {
      selectByIndex(tape,
         node.findChild(lxElseValue).argument,
         node.findChild(lxIfValue).argument);
   }

   scope.clear();

   releaseStack(tape);
}

void ByteCodeWriter :: generateBoolLogicOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   SNode larg;
   SNode rarg;
   assignOpArguments(node, larg, rarg);

   ref_t trueRef = node.findChild(lxIfValue).argument | mskConstantRef;
   ref_t falseRef = node.findChild(lxElseValue).argument | mskConstantRef;

   if (!test(mode, BOOL_ARG_EXPR))
      tape.newLabel();

   generateObject(tape, larg, scope, BOOL_ARG_EXPR);

   switch (node.argument) {
      case AND_OPERATOR_ID:
         tape.write(blBreakLabel); // !! temporally, to prevent if-optimization
         tape.write(bcIfR, baCurrentLabel, falseRef);
         break;
      case OR_OPERATOR_ID:
         tape.write(blBreakLabel); // !! temporally, to prevent if-optimization
         tape.write(bcIfR, baCurrentLabel, trueRef);
         break;
   }

   generateObject(tape, rarg, scope, BOOL_ARG_EXPR);

   if (!test(mode, BOOL_ARG_EXPR))
      tape.setLabel();
}

inline bool isConstant(LexicalType type)
{
   switch (type) {
      case _ELENA_::lxNil:
      case _ELENA_::lxClassSymbol:
      case _ELENA_::lxConstantSymbol:
      case _ELENA_::lxConstantInt:
      case _ELENA_::lxConstantString:
      case _ELENA_::lxConstantList:
      case _ELENA_::lxConstantWideStr:
      case _ELENA_::lxConstantChar:
         return true;
   default:
      return false;
   }
}

void ByteCodeWriter :: generateNilOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   if (node.argument == EQUAL_OPERATOR_ID) {
      SNode larg;
      SNode rarg;
      assignOpArguments(node, larg, rarg);
      if (larg == lxExpression)
         larg = larg.findSubNodeMask(lxObjectMask);
      if (rarg == lxExpression)
         rarg = rarg.findSubNodeMask(lxObjectMask);

      if (larg == lxNil) {
         generateObject(tape, rarg, scope);
      }
      else if (rarg == lxNil) {
         generateObject(tape, larg, scope);
      }
      else throw new InternalError("Not yet implemented"); // temporal
      //else generateExpression(tape, node, ACC_REQUIRED); // ?? is this code reachable

      SNode ifParam = node.findChild(lxIfValue);
      SNode elseParam = node.findChild(lxElseValue);

      selectByAcc(tape, elseParam.argument, ifParam.argument);
   }
   else if (node.argument == ISNIL_OPERATOR_ID) {

      SNode larg;
      SNode rarg;
      assignOpArguments(node, larg, rarg);

      if (rarg == lxExpression)
         rarg = rarg.findSubNodeMask(lxObjectMask);

      switch (rarg.type) {
         case lxConstantChar:
            //case lxConstantClass:
         case lxConstantInt:
         case lxConstantLong:
         case lxConstantList:
         case lxConstantReal:
         case lxConstantString:
         case lxConstantWideStr:
         case lxConstantSymbol:
            loadObject(tape, larg, scope);
            tape.write(bcCoalesceR, rarg.argument | defineConstantMask(rarg.type));
            break;
         case lxNil:
            loadObject(tape, larg, scope);
            tape.write(bcCoalesceR, 0);
            break;
         default:
            generateObject(tape, rarg, scope, STACKOP_MODE);
            loadObject(tape, larg, scope);
            tape.write(bcCoalesce);
            releaseStack(tape);
            break;
      }
   }
   else throw new InternalError("Not yet implemented"); // temporal
}

//void ByteCodeWriter :: generateExternalArguments(CommandTape& tape, SNode node, ExternalScope& externalScope)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      if (current == lxExtInteranlRef) {
//      }
//      else if (current == lxIntExtArgument || current == lxExtArgument) {
//         SNode object = current.findSubNodeMask(lxObjectMask);
//         if (test(object.type, lxObjectMask)) {
//            if (!isSimpleObject(object, true)) {
//               ExternalScope::ParamInfo param;
//
//               generateObject(tape, object, ACC_REQUIRED);
//               pushObject(tape, lxResult);
//               param.offset = ++externalScope.frameSize;
//
//               externalScope.operands.push(param);
//            }
//         }
//      }
//      current = current.nextNode();
//   }
//}

int ByteCodeWriter :: saveExternalParameters(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   int paramCount = 0;

//   // save function parameters
//   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   SNode current = node.lastChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         generateObject(tape, current, scope, STACKOP_MODE);

         paramCount++;
      }
//      if (current == lxExtInteranlRef) {
//         // HOTFIX : ignore call operation
//         SNode ref = current.findSubNode(lxInternalRef);
//         loadInternalReference(tape, ref.argument);
//         pushObject(tape, lxResult);
//
//         paramCount++;
//      }
//      else if (current == lxIntExtArgument || current == lxExtArgument) {
//         SNode object = current.findSubNodeMask(lxObjectMask);
//         SNode value;
//         if (object == lxConstantInt)
//            value = object;
//
//         if (!isSimpleObject(object, true)) {
//            loadObject(tape, lxBlockLocal, (*out_it).offset, 0);
//
//            out_it++;
//         }
//         else if (current != lxIntExtArgument || value == lxNone) {
//            generateObject(tape, object);
//         }
//
//         if (current == lxIntExtArgument) {
//            // optimization : use the numeric constant directly
//            if (value == lxConstantInt) {
//               declareVariable(tape, value.findChild(lxIntValue).argument);
//            }
//            else if (object.type == lxFieldAddress) {
//               if (testany(object.argument, 3)) {
//                  // dcopy index
//                  // bread
//                  // pushe
//                  tape.write(bcDCopy, object.argument);
//                  tape.write(bcBRead);
//                  tape.write(bcPushE);
//               }
//               else pushObject(tape, lxResultField, object.argument >> 2);
//            }
//            else pushObject(tape, lxResultField);
//         }
//         else if (current == lxExtArgument) {
//            pushObject(tape, lxResult);
//         }
//         paramCount++;
//      }

      current = current.prevNode();
   }

   return paramCount;
}

void ByteCodeWriter :: generateExternalCall(CommandTape& tape, SNode node, FlowScope& scope)
{
//   SNode bpNode = node.findChild(lxBreakpoint);
//   if (bpNode != lxNone) {
//      translateBreakpoint(tape, bpNode, false);
//
//      declareBlock(tape);
//   }

   bool apiCall = (node == lxCoreAPICall);
   bool cleaned = (node == lxStdExternalCall);

   // compile argument list
   //ExternalScope externalScope;
  // declareExternalBlock(tape);

//   generateExternalArguments(tape, node, externalScope);

   // save function parameters
   int paramCount = saveExternalParameters(tape, node, scope);

   // call the function
   if (apiCall) {
//      // if it is an API call
//      // simply release parameters from the stack
//      // without setting stack pointer directly - due to optimization
      callCore(tape, node.argument/*, externalScope.frameSize*/);

//      endExternalBlock(tape, true);
//
   }
   else {
      callExternal(tape, node.argument/*, externalScope.frameSize*/);

  //    endExternalBlock(tape);
   }

   if (!cleaned)
      releaseStack(tape, paramCount);

//   if (bpNode != lxNone)
//      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
}

/*ref_t*/void ByteCodeWriter :: generateCall(CommandTape& tape, SNode callNode/*, int paramCount, int presavedCount*/)
{
//   SNode bpNode = callNode.findChild(lxBreakpoint);
//   if (bpNode != lxNone) {
//      translateBreakpoint(tape, bpNode, false);
//
//      declareBlock(tape);
//   }

   // copym message
   ref_t message = callNode.argument;
//   SNode msg = callNode.findChild(lxOvreriddenMessage);
//   if (msg != lxNone)
//      message = msg.argument;

   tape.write(bcMovM, message);

//   bool invokeMode = test(message, SPECIAL_MESSAGE);

   SNode target = callNode.findChild(lxCallTarget);
   if (callNode == lxDirectCalling) {
      callResolvedMethod(tape, target.argument, callNode.argument/*, invokeMode*/);
   }
   else if (callNode == lxSDirectCalling) {
      callVMTResolvedMethod(tape, target.argument, callNode.argument/*, invokeMode*/);
   }
//   else if (invokeMode) {
//      // pop
//      // acallvi offs
//      tape.write(bcPop);
//      tape.write(bcACallVI, 0);
//      tape.write(bcFreeStack, getParamCount(callNode.argument));
//   }
   else {
      int vmtOffset = callNode == lxCalling_1 ? 1 : 0;

      // callvi vmtOffset
      tape.write(bcCallVI, vmtOffset);
   }

//   if (bpNode != lxNone)
//      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);

   //return message;
}

void ByteCodeWriter :: generateInternalCall(CommandTape& tape, SNode node, FlowScope& scope)
{
   int paramCount = 0;

   // analizing a sub tree
   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      paramCount++;

      current = current.nextNode(lxObjectMask);
   }

   allocateStack(tape, paramCount);

   int index = 0;
   current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      generateObject(tape, current, scope);

      saveObject(tape, lxCurrent, index);
      index++;

      current = current.nextNode(lxObjectMask);
   }

   tape.write(bcCallR, node.argument | mskInternalRef);
   scope.clear();
}

//void ByteCodeWriter :: generateInlineArgCallExpression(CommandTape& tape, SNode node)
//{
//   SNode larg;
//   SNode rarg;
//   assignOpArguments(node, larg, rarg);
//   if (rarg == lxExpression)
//      rarg = rarg.findSubNodeMask(lxObjectMask);
//
//   generateInlineArgCall(tape, larg, rarg, node.argument);
//}
//
//void ByteCodeWriter :: generateInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message)
//{
//   tape.newLabel(); // declare end label
//   tape.newLabel(); // declare long label
//
//   if (isSimpleObject(rarg)) {
//      loadBase(tape, rarg.type, rarg.argument, 0);
//   }
//   else {
//      generateObject(tape, rarg, ACC_REQUIRED);
//      loadBase(tape, lxResult, 0, 0);
//   }
//
//   // count
//   tape.write(bcCount);
//   // dcopye
//   tape.write(bcDCopyE);
//
//   // greatern labVariadic ARG_COUNT
//   tape.write(bcGreaterN, baCurrentLabel, ARG_COUNT);
//
//   // labNext:
//   tape.newLabel();
//   tape.setLabel(true);
//   // dec
//   tape.write(bcDec);
//   // get
//   tape.write(bcGet);
//   // pusha
//   tape.write(bcPushA);
//   // elsen labNext 0
//   tape.write(bcElseN, baCurrentLabel, 0);
//   tape.releaseLabel();
//
//   // ; push target
//   generateObject(tape, larg, ACC_REQUIRED);
//   pushObject(tape, lxResult);
//
//   // setverb m
//   tape.write(bcSetVerb, getAction(message));
//
//   if (test(message, SPECIAL_MESSAGE)) {
//      // eorn SPECIAL_MESSAGE
//      tape.write(bcEOrN, SPECIAL_MESSAGE);
//      releaseObject(tape);
//   }
//
//   // acallvi 0
//   tape.write(bcACallVI, 0);
//   // jmp labEnd
//   tape.write(bcJump, baPreviousLabel);
//
//   // labVariadic:
//   tape.setLabel();
//
//   // pushn 0
//   tape.write(bcPushR, 0);
//
//   // labNext2:
//   tape.newLabel();
//   tape.setLabel(true);
//
//   // dec
//   tape.write(bcDec);
//   // get
//   tape.write(bcGet);
//   // pusha
//   tape.write(bcPushA);
//   // elsen labNext2 0
//   tape.write(bcElseN, baCurrentLabel, 0);
//   tape.releaseLabel();
//
//   // ; push target
//   generateObject(tape, larg, ACC_REQUIRED);
//   pushObject(tape, lxResult);
//
//   // copym
//   tape.write(bcCopyM, encodeMessage(getAction(message), 1, VARIADIC_MESSAGE));
//
//   if (test(message, SPECIAL_MESSAGE)) {
//      // eorn SPECIAL_MESSAGE
//      tape.write(bcEOrN, SPECIAL_MESSAGE);
//      releaseObject(tape);
//   }
//
//   // acallvi 0
//   tape.write(bcACallVI, 0);
//
//   releaseArgList(tape);
//
//   // labEnd
//   tape.setLabel();
//}
//
//void ByteCodeWriter :: generateVariadicInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message)
//{
//}
//
//void ByteCodeWriter :: loadUnboxingVar(CommandTape& tape, SNode current, int paramCount, int& presavedCount)
//{
//   SNode tempLocal = current.findChild(lxTempLocal);
//   if (tempLocal == lxNone) {
//      loadObject(tape, lxCurrent, paramCount + presavedCount - 1, 0);
//      presavedCount--;
//   }
//   else loadObject(tape, lxLocal, tempLocal.argument, 0);
//}
//
//void ByteCodeWriter :: saveUnboxingVar(CommandTape& tape, SNode member, bool& accTarget, bool& accPresaving, int& presavedCount)
//{
//   if (accTarget) {
//      pushObject(tape, lxResult);
//      presavedCount++;
//      accPresaving = true;
//   }
//
//   generateObject(tape, member, ACC_REQUIRED);
//   pushObject(tape, lxResult);
//   presavedCount++;
//}

void ByteCodeWriter :: generateCallExpression(CommandTape& tape, SNode node, FlowScope& scope)
{
   bool functionMode = test(node.argument, FUNCTION_MESSAGE);
   bool directMode = true;
   bool argUnboxMode = false;
   bool openArg = false;

   int argCount = 0;

   // analizing a sub tree
   SNode current = node.firstChild(lxObjectMask);
   // check if the message target can be used directly
   bool isFirstDirect = !isSubOperation(current) && current != lxResult;
   while (current != lxNone) {
      SNode argNode = current;
      if (argNode == lxExpression)
         argNode = current.findSubNodeMask(lxObjectMask);

      if (argNode == lxArgArray) {
         argUnboxMode = true;
         generateExpression(tape, argNode, scope);
         unboxArgList(tape, argNode.argument != 0);
      }
      else {
         argCount++;

         if (isSubOperation(current) || current == lxResult)
            directMode = false;
      }

      current = current.nextNode(lxObjectMask);
   }

   if (!argUnboxMode && isOpenArg(node.argument)) {
      // NOTE : do not add trailing nil for result of unboxing operation
      pushObject(tape, lxStopper, 0, scope, 0);
      openArg = true;
   }

   if ((argCount == 2 && isFirstDirect) || argCount == 1) {
      // if message target can be used directly or it has no arguments - direct mode is allowed
      directMode = true;
   }
   else if (!directMode) {
      allocateStack(tape, functionMode && isFirstDirect ? argCount - 1 : argCount);
   }

   // the function target can be loaded at the end
   size_t startIndex = 0, diff = 1;
   if (functionMode && isFirstDirect) {
      startIndex = 1;
      diff = 0;
   }

   for (size_t i = startIndex; i < argCount; i++) {
      // get parameters in reverse order if required
      current = getChild(node, directMode ? argCount - i - diff : i);
      if (current == lxExpression)
         current = current.findSubNodeMask(lxObjectMask);

      if (current == lxArgArray) {
         // argument list is already unboxed
      }
      else if (!directMode) {
         generateObject(tape, current, scope);
         saveObject(tape, lxCurrent, i - startIndex);
      }
      else generateObject(tape, current, scope, STACKOP_MODE);
   }

   if (functionMode) {
      // load a function target
      if (startIndex == 0) {
         popObject(tape, lxResult);
      }
      else generateObject(tape, getChild(node, 0), scope);
   }
   else tape.write(bcPeekSI, 0);

   generateCall(tape, node/*, paramCount, presavedCount*/);

   if (argUnboxMode) {
      releaseArgList(tape);
   }
   else if (openArg) {
      //// clear open argument list, including trailing nil and subtracting normal arguments
      //if (test(node.argument, SPECIAL_MESSAGE)) {
      //   // HOTFIX : self is not in the stack
      //   releaseObject(tape, paramCount - getParamCount(node.argument) + 1);
      //}
      /*else */releaseStack(tape, argCount - getArgCount(node.argument));
   }

   scope.clear();
}

//void ByteCodeWriter :: unboxCallParameter(CommandTape& tape, SNode current)
//{
//   SNode target = current.firstChild(lxObjectMask);
//   SNode tempLocal = current.findChild(lxTempLocal);
//   if (tempLocal != lxNone) {
//      loadObject(tape, lxLocal, tempLocal.argument, 0);
//   }
//   else popObject(tape, lxResult);
//
//   if (current.argument != 0) {
//      if (target == lxExpression)
//         target = target.firstChild(lxObjectMask);
//
//      tape.write(bcPushB);
//      if (target == lxAssigning) {
//         // unboxing field address
//         SNode larg, rarg;
//         assignOpArguments(target, larg, rarg);
//
//         target = rarg;
//      }
//
//      if (target == lxFieldAddress) {
//         bool dummy = false;
//         if (current.argument == 4) {
//            assignInt(tape, lxFieldAddress, target.argument, dummy);
//         }
//         else if (current.argument == 2) {
//            assignLong(tape, lxFieldAddress, target.argument, dummy);
//         }
//         else assignStruct(tape, lxFieldAddress, target.argument, current.argument);
//      }
//      else {
//         loadBase(tape, target.type, target.argument, 0);
//         copyBase(tape, current.argument);
//      }
//
//      tape.write(bcPopB);
//   }
//   else {
//      loadObject(tape, lxResultField, 0, 0);
//      saveObject(tape, target.type, target.argument);
//   }
//}
//
//void ByteCodeWriter :: unboxCallParameters(CommandTape& tape, SyntaxTree::Node node)
//{
//   loadBase(tape, lxResult, 0, 0);
//
//   size_t counter = countChildren(node);
//   size_t index = 0;
//   while (index < counter) {
//      // get parameters in reverse order if required
//      SNode current = getChild(node, counter - index - 1);
//
//      if (current == lxExpression)
//         current = current.firstChild(lxObjectMask);
//
//      if (current == lxUnboxing) {
//         unboxCallParameter(tape, current);
//      }
//      else if (current == lxOverridden && current.existChild(lxUnboxing)) {
//         unboxCallParameter(tape, current.findChild(lxUnboxing));
//      }
//      else if (current == lxLocalUnboxing) {
//         SNode assignNode = current.findChild(lxAssigning);
//         SNode larg;
//         SNode rarg;
//
//         assignOpArguments(assignNode, larg, rarg);
//
//         tape.write(bcPushB);
//         loadObject(tape, larg.type, larg.argument, 0);
//         loadBase(tape, rarg.type, 0, ACC_PRESAVED);
//
//         bool dummy = false;
//         if (assignNode.argument == 4) {
//            assignInt(tape, lxFieldAddress, rarg.argument, dummy);
//         }
//         else if (assignNode.argument == 2) {
//            assignLong(tape, lxFieldAddress, rarg.argument, dummy);
//         }
//         else assignStruct(tape, lxFieldAddress, rarg.argument, assignNode.argument);
//
//         tape.write(bcPopB);
//      }
//      else if (current == lxNested) {
//         bool unboxing = false;
//         SNode member = current.firstChild();
//         while (member != lxNone) {
//            if (member == lxOuterMember) {
//               unboxing = true;
//
//               SNode target = member.firstChild(lxObjectMask);
//               SNode checkLocal = member.firstChild(lxCheckLocal);
//
//               // load outer field
//               loadObject(tape, lxCurrent, 0, 0);
//               loadObject(tape, lxResultField, member.argument, 0);
//
//               // save to the original variable
//               if (target.type == lxBoxing) {
//                  SNode localNode = target.firstChild(lxObjectMask);
//
//                  tape.write(bcPushB);
//                  loadBase(tape, localNode.type, localNode.argument, ACC_PRESAVED);
//                  if (target.argument != 0) {
//                     copyBase(tape, target.argument);
//                  }
//                  else tape.write(bcCopy);
//
//                  tape.write(bcPopB);
//               }
//               else if (checkLocal == lxCheckLocal) {
//                  saveObjectIfChanged(tape, target.type, target.argument, checkLocal.argument, 0);
//               }
//               else saveObject(tape, target.type, target.argument);
//            }
//            else if (member == lxCode) {
//               unboxing = true;
//
//               generateCodeBlock(tape, member);
//            }
//
//            member = member.nextNode();
//         }
//         if (unboxing)
//            releaseObject(tape);
//      }
//
//      index++;
//   }
//
//   assignBaseTo(tape, lxResult);
//}

void ByteCodeWriter :: generateReturnExpression(CommandTape& tape, SNode node, FlowScope& scope)
{
   generateExpression(tape, node, scope/*, ACC_REQUIRED*/);

   gotoEnd(tape, baFirstLabel);
}

//////void ByteCodeWriter :: generateThrowExpression(CommandTape& tape, SNode node)
//////{
//////   generateExpression(tape, node, ACC_REQUIRED);
//////
//////   pushObject(tape, lxResult);
//////   throwCurrent(tape);
//////
//////
//////   gotoEnd(tape, baFirstLabel);
//////}
//
//void ByteCodeWriter :: generateBoxing(CommandTape& tape, SNode node)
//{
//   SNode target = node.findChild(lxTarget);
//   if (isPrimitiveRef(target.argument))
//      throw InternalError("Invalid boxing target");
//
//   if (node == lxArgBoxing) {
//      boxArgList(tape, target.argument);
//   }
//   else if (node.argument == 0) {
//      SNode attr = node.findChild(lxBoxableAttr);
//      if (attr.argument == INVALID_REF) {
//         // HOTFIX : to recognize a primitive array boxing
//         tape.write(bcLen);
//         loadBase(tape, lxResult, 0, 0);
//         loadObject(tape, lxClassSymbol, target.argument, 0);
//         newDynamicObject(tape);
//         copyDynamicObject(tape, true, true);
//      }
//      else newVariable(tape, target.argument, lxResult);
//   }
//   else boxObject(tape, node.argument, target.argument, node != lxCondBoxing);
//
//   SNode temp = node.findChild(lxTempLocal);
//   if (temp != lxNone) {
//      saveObject(tape, lxLocal, temp.argument);
//   }
//}
//
//void ByteCodeWriter :: generateFieldBoxing(CommandTape& tape, SyntaxTree::Node node, int offset)
//{
//   SNode target = node.findChild(lxTarget);
//
//   boxField(tape, offset, node.argument, target.argument);
//}
//
//void ByteCodeWriter :: generateBoxingExpression(CommandTape& tape, SNode node, int mode)
//{
//   SNode expr = node.firstChild(lxObjectMask);
//   if (expr == lxFieldAddress && expr.argument > 0) {
//      loadObject(tape, expr);
//      generateFieldBoxing(tape, node, expr.argument);
//   }
//   else {
//      generateExpression(tape, node, mode | ACC_REQUIRED);
//      generateBoxing(tape, node);
//   }
//}
//
//inline bool isAssignOp(SNode source)
//{
//   return test(source.type, lxPrimitiveOpMask) && (IsExprOperator(source.argument) || (source.argument == REFER_OPERATOR_ID && source.type != lxArrOp && source.type != lxArgArrOp) ||
//      (IsShiftOperator(source.argument) && (source.type == lxIntOp || source.type == lxLongOp)));
//}

SyntaxTree::Node ByteCodeWriter :: loadFieldExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool idleMode)
{
   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      SNode objNode = current;
      if (objNode == lxExpression)
         objNode = current.findSubNodeMask(lxObjectMask);

      SNode nextNode = current.nextNode(lxObjectMask);

      if (nextNode != lxNone) {
         if (!idleMode)
            loadObject(tape, current, scope);
      }
      else return objNode;

      current = nextNode;
   }

   return current;
}

void ByteCodeWriter :: copyToFieldAddress(CommandTape& tape, int size, int argument)
{
   if ((size & 3) == 0 && (argument & 3) == 0) {
      // if it is a dword aligned
      tape.write(bcCopyToAI, argument >> 2, size >> 2);
   }
   else {
      tape.write(bcMoveTo, argument, size);
   }
}

void ByteCodeWriter :: copyFieldAddress(CommandTape& tape, int size, int argument, FlowScope& scope)
{
   if ((size & 3) == 0 && (argument & 3) == 0) {
      // if it is a dword aligned
      tape.write(bcCopyAI, argument >> 2, size >> 2);
   }
   else if (size == 2) {
      tape.write(bcXLoad, argument);
      tape.write(bcAnd, 0xFFFF);
      tape.write(bcPeekSI, 0);
      tape.write(bcSave);
      scope.clear();
   }
   else if (size == 1) {
      tape.write(bcXLoad, argument);
      tape.write(bcAnd, 0xFF);
      tape.write(bcPeekSI, 0);
      tape.write(bcSave);
      scope.clear();
   }
   else {
      tape.write(bcMove, argument, size);
   }
}

void ByteCodeWriter :: copyFromLocalAddress(CommandTape& tape, int size, int argument)
{
   // stack operations are always 4-byte aligned
   size = align(size, 4);

   // if it is a dword aligned
   tape.write(bcCopyF, argument, size >> 2);
}

void ByteCodeWriter :: copyToLocalAddress(CommandTape& tape, int size, int argument)
{
   // stack operations are always 4-byte aligned
   size = align(size, 4);

   tape.write(bcCopyToF, argument, size >> 2);
}

void ByteCodeWriter :: saveToLocal(CommandTape& tape, int size, int argument)
{
   if (size == 4) {
      // savef arg
      tape.write(bcSaveFI, argument, bpFrame);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: saveToLocalAddress(CommandTape& tape, int size, int argument)
{
   switch (size) {
      case 1:
         tape.write(bcAnd, 0xFF);
         break;
      case 2:
         tape.write(bcAnd, 0xFFFF);
         break;
      case 4:
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
   }

   // savef arg
   tape.write(bcSaveF, argument);
}

void ByteCodeWriter :: loadFieldAddress(CommandTape& tape, int size, int argument)
{
   if (size == 4) {
      // xload arg
      tape.write(bcXLoad, argument);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: copyToLocal(CommandTape& tape, int size, int argument)
{
   // stack operations are always 4-byte aligned
   size = align(size, 4);

   // if it is a dword aligned
   tape.write(bcCopyToFI, argument, size >> 2, bpFrame);
}

inline bool isAligned(int size)
{
   return (size & 3) == 0;
}

void ByteCodeWriter :: saveFieldExpression(CommandTape& tape, SNode dstObj, SNode source, int size, FlowScope& scope)
{
   SNode fieldNode = loadFieldExpression(tape, dstObj, scope, true);
   if (fieldNode == lxFieldAddress) {
      generateObject(tape, source, scope, STACKOP_MODE);
      loadFieldExpression(tape, dstObj, scope, false);
      copyToFieldAddress(tape, size, fieldNode.argument);
      releaseStack(tape);
   }
   else if (fieldNode.compare(lxSelfLocal, lxTempLocal)) {
      if (isAligned(size)) {
         loadObject(tape, source, scope);
         copyToLocal(tape, size, fieldNode.argument);
      }
      else {
         generateObject(tape, source, scope, STACKOP_MODE);
         generateObject(tape, dstObj, scope, 0);
         copyToFieldAddress(tape, size, 0);
         releaseStack(tape);
      }
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: saveIndexToFieldExpression(CommandTape& tape, SNode dstObj, SNode srcObj, FlowScope& scope)
{
   SNode fieldNode = loadFieldExpression(tape, dstObj, scope, true);
   if (fieldNode == lxFieldAddress) {
      loadFieldExpression(tape, dstObj, scope, false);

      tape.write(bcPushA);
      loadObject(tape, srcObj, scope);  // NOTE : it should load the index
      tape.write(bcPopA);

      if ((fieldNode.argument & 3) == 0) {
         tape.write(bcSaveI, fieldNode.argument >> 2);
      }
      else tape.write(bcXSave, fieldNode.argument);
   }
   else if (fieldNode == lxSelfLocal) {
      loadFieldExpression(tape, dstObj, scope, false);

      loadObject(tape, srcObj, scope);  // NOTE : it should load the index
      loadObject(tape, fieldNode, scope);
      tape.write(bcSave);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: copyExpression(CommandTape& tape, SNode source, SNode dstObj, int size, FlowScope& scope)
{
   if (dstObj.compare(lxLocal, lxTempLocal, lxSelfLocal)) {
      loadObject(tape, source, scope);
      copyToLocal(tape, size, dstObj.argument);
   }
   else if (dstObj == lxLocalAddress) {
      loadObject(tape, source, scope);
      copyToLocalAddress(tape, size, dstObj.argument);
   }
   else if (dstObj == lxFieldExpression) {
      saveFieldExpression(tape, dstObj, source, size, scope);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: generateCopyingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   SNode srcObj = source == lxExpression ? source.findSubNodeMask(lxObjectMask) : source;
   SNode dstObj = target == lxExpression ? target.findSubNodeMask(lxObjectMask) : target;

   if (srcObj == lxLocalAddress && dstObj != lxFieldExpression) {
      loadObject(tape, target, scope);
      copyFromLocalAddress(tape, node.argument, srcObj.argument);
   }
   else if (srcObj == lxFieldExpression && dstObj != lxFieldExpression) {
      SNode fieldNode = loadFieldExpression(tape, srcObj, scope, true);
      if (fieldNode.compare(lxFieldAddress, lxSelfLocal)) {
         if (dstObj == lxLocalAddress && node.argument <= 4) {
            loadFieldExpression(tape, srcObj, scope, false);
            if (fieldNode == lxFieldAddress) {
               loadFieldAddress(tape, node.argument, fieldNode.argument);
            }
            else {
               loadObject(tape, fieldNode, scope);
               tape.write(bcLoad);
            }
            saveToLocalAddress(tape, node.argument, dstObj.argument);
         }
         else {
            generateObject(tape, target, scope, STACKOP_MODE);
            loadFieldExpression(tape, srcObj, scope, false);
            if (fieldNode == lxFieldAddress) {
               copyFieldAddress(tape, node.argument, fieldNode.argument, scope);
            }
            else {
               loadObject(tape, fieldNode, scope);
               copyFieldAddress(tape, node.argument, 0, scope);
            }

            releaseStack(tape);
         }
      }
      else copyExpression(tape, source, dstObj, node.argument, scope);
   }
   else copyExpression(tape, source, dstObj, node.argument, scope);
}

void ByteCodeWriter :: generateSavingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   SNode srcObj = source == lxExpression ? source.findSubNodeMask(lxObjectMask) : source;
   SNode dstObj = target == lxExpression ? target.findSubNodeMask(lxObjectMask) : target;

   //if (srcObj == lxLocalAddress) {
   //   loadObject(tape, target, scope);
   //   copyFromLocalAddress(tape, node.argument, srcObj.argument);
   //}
   /*else */if (dstObj.compare(lxLocal, lxTempLocal, lxSelfLocal)) {
      loadObject(tape, source, scope);
      saveToLocal(tape, 4, dstObj.argument);
   }
   else if (dstObj == lxLocalAddress) {
      loadObject(tape, source, scope); // NOTE : it should load the index
      saveToLocalAddress(tape, 4, dstObj.argument);
   }
   else if (dstObj == lxFieldExpression) {
      saveIndexToFieldExpression(tape, dstObj, srcObj, scope);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: generateByRefAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   generateObject(tape, source, scope, STACKOP_MODE);
   loadObject(tape, target, scope);
   saveObject(tape, lxField, 0);
   releaseStack(tape);
}

void ByteCodeWriter :: generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
//   if (test(mode, BASE_PRESAVED))
//      tape.write(bcPushB);
//
//   bool accRequiered = test(mode, ACC_REQUIRED);
//
//   int size = node.argument;

   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   if (isSubOperation(target)) {
      if (target == lxFieldExpression) {
         generateObject(tape, source, scope, STACKOP_MODE);

         SNode fieldNode = loadFieldExpression(tape, target, scope, false);
         saveObject(tape, fieldNode);

         releaseStack(tape);
      }
      else throw InternalError("not yet implemente"); // !! temporal
   }
   else {
      loadObject(tape, source, scope);
      saveObject(tape, target);
   }

//   if (source == lxExpression)
//      source = source.findSubNodeMask(lxObjectMask);
//
//   //SNode child = node.firstChild();
//   //while (child != lxNone) {
//   //   if (test(child.type, lxObjectMask)) {
//   //      if (target == lxNone) {
//   //         target = child;
//   //      }
//   //      else if (child == lxExpression) {
//   //         translateBreakpoint(tape, child.findChild(lxBreakpoint), false);
//
//   //         source = child.findSubNodeMask(lxObjectMask);
//   //      }
//   //      else source = child;
//   //   }
//
//   //   child = child.nextNode();
//   //}
//
//
//   if (isAssignOp(source)) {
//      int opMode = ASSIGN_MODE;
//      if (accRequiered)
//         opMode |= ACC_REQUIRED;
//
//   //   if (target == lxCreatingStruct) {
//   //      generateObject(tape, target, ACC_REQUIRED);
//   //      loadBase(tape, lxResult);
//   //   }
//      /*else */generateObject(tape, source, opMode);
//   }
//   else {
//      generateObject(tape, source, ACC_REQUIRED);
//
//      if (source == lxExternalCall || source == lxStdExternalCall || source == lxCoreAPICall) {
//         if (node.argument == 4) {
//            saveInt(tape, target.type, target.argument);
//         }
//         else if (node.argument == 8) {
//            if (node.findSubNode(lxFPUTarget) == lxFPUTarget) {
//               saveReal(tape, target.type, target.argument);
//            }
//            else saveLong(tape, target.type, target.argument);
//         }
//
//         if (accRequiered)
//            assignBaseTo(tape, lxResult);
//      }
//      else if (target == lxFieldExpression || target == lxExpression) {
//         SNode arg1, arg2;
//         assignOpArguments(target, arg1, arg2);
//         if (arg1.type == lxFieldExpression) {
//            SNode arg3, arg4;
//            assignOpArguments(arg1, arg3, arg4);
//            loadBase(tape, arg3.type, arg3.argument, ACC_PRESAVED);
//            loadFieldExpressionBase(tape, arg4.type, arg4.argument, ACC_PRESAVED);
//         }
//         else loadBase(tape, arg1.type, arg1.argument, ACC_PRESAVED);
//         if (arg2 == lxStaticField) {
//            saveBase(tape, false, arg2.type, arg2.argument);
//         }
//         else saveBase(tape, false, lxResult, arg2.argument);
//      }
//      else if (size != 0) {
//         if (source == lxFieldAddress) {
//            loadBase(tape, target.type, target.argument, ACC_PRESAVED);
//            if (target == lxFieldAddress) {
//               copyStructureField(tape, source.argument, target.argument, size);
//            }
//            else if (size == 4) {
//               copyInt(tape, source.argument);
//            }
//            else if (size == 2) {
//               copyShort(tape, source.argument);
//            }
//            else if (size == 1) {
//               copyByte(tape, source.argument);
//            }
//            else copyStructure(tape, source.argument, size);
//         }
//         else {
//            if (size == 4) {
//               assignInt(tape, target.type, target.argument, accRequiered);
//            }
//            else if (size == 2) {
//               assignShort(tape, target.type, target.argument, accRequiered);
//            }
//            else if (size == 1) {
//               assignByte(tape, target.type, target.argument, accRequiered);
//            }
//            else if (size == 8) {
//               assignLong(tape, target.type, target.argument, accRequiered);
//            }
//            else assignStruct(tape, target.type, target.argument, size);
//         }
//
//         if (accRequiered)
//            assignBaseTo(tape, lxResult);
//      }
//      else if (node.existChild(lxByRefTarget) && size == 0) {
//         // HOTFIX : to support boxed by ref operation
//         loadBase(tape, target.type, target.argument, ACC_PRESAVED);
//         saveObject(tape, lxBaseField, 0);
//      }
//      else {
//   //      // if assinging the result of primitive assigning operation
//   //      // it should be boxed before
//   //      if (source == lxAssigning && source.argument > 0) {
//   //         generateBoxing(tape, source);
//   //      }
//
//         saveObject(tape, target.type, target.argument);
//      }
//   }
//
//   if (test(mode, BASE_PRESAVED))
//      tape.write(bcPopB);
}

//void ByteCodeWriter :: generateCopying(CommandTape& tape, SyntaxTree::Node node, int mode)
//{
//   int size = node.argument << 2;
//   if (!size)
//      return;
//
//   if (test(mode, BASE_PRESAVED))
//      tape.write(bcPushB);
//
//   bool accRequiered = test(mode, ACC_REQUIRED);
//
//   SNode target;
//   SNode source;
//   assignOpArguments(node, target, source);
//
//   if (source == lxExpression)
//      source = source.findSubNodeMask(lxObjectMask);
//
//   generateObject(tape, source, ACC_REQUIRED);
//
//   if (target == lxFieldExpression || target == lxExpression) {
//      SNode arg1, arg2;
//      assignOpArguments(target, arg1, arg2);
//      if (arg1.type == lxFieldExpression) {
//         SNode arg3, arg4;
//         assignOpArguments(arg1, arg3, arg4);
//         loadBase(tape, arg3.type, arg3.argument, ACC_PRESAVED);
//         loadFieldExpressionBase(tape, arg4.type, arg4.argument, ACC_PRESAVED);
//      }
//      else loadBase(tape, arg1.type, arg1.argument, ACC_PRESAVED);
//
//      if (size == 4) {
//         assignInt(tape, arg2, arg2.argument, accRequiered);
//      }
//      else assignStruct(tape, arg2, arg2.argument, size);
//   }
//   else {
//      if (target == lxSeqExpression) {
//         pushObject(tape, lxResult);
//         generateObject(tape, target);
//         loadBase(tape, lxResult, 0, 0);
//         popObject(tape, lxResult);
//      }
//      else loadBase(tape, target.type, target.argument, ACC_PRESAVED);
//
//      if (size == 4) {
//         copyInt(tape, 0);
//      }
//      else copyStructure(tape, 0, size);
//
//   }
//
//   if (accRequiered)
//      assignBaseTo(tape, lxResult);
//
//   if (test(mode, BASE_PRESAVED))
//      tape.write(bcPopB);
//}

void ByteCodeWriter :: generateExternFrame(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   excludeFrame(tape);

   generateCodeBlock(tape, node, scope);

   includeFrame(tape);
}

void ByteCodeWriter :: generateTrying(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   bool first = true;

   int retLabel = declareSafeTry(tape);

   SNode finallyNode = node.findChild(lxFinalblock);
   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      scope.clear();

      if (first) {
         if (current == lxCode) {
            generateCodeBlock(tape, current, scope);
         }
         else generateObject(tape, current, scope);

         endTry(tape);
         if (finallyNode != lxNone) {
            scope.clear();

            // generate finally
            pushObject(tape, lxResult, 0, scope, 0);
            generateCodeBlock(tape, finallyNode, scope);
            popObject(tape, lxResult);
         }
         declareSafeCatch(tape, finallyNode, retLabel, scope);
         doCatch(tape);
         if (finallyNode != lxNone) {
            scope.clear();

            // generate finally
            pushObject(tape, lxResult, 0, scope, 0);
            generateCodeBlock(tape, finallyNode, scope);
            popObject(tape, lxResult);
         }

         // ...

         first = false;
      }
      else generateObject(tape, current, scope);

      current = current.nextNode(lxObjectMask);
   }

   endSafeCatch(tape);

   scope.clear();
}

void ByteCodeWriter :: generateAlt(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   bool first = true;

   declareTry(tape);

   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      scope.clear();

      generateObject(tape, current, scope);

      if (first) {
         declareAlt(tape);

         first = false;
      }
      current = current.nextNode(lxObjectMask);
   }

   endAlt(tape);
   scope.clear();
}

void ByteCodeWriter :: generateLooping(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   declareLoop(tape, true);

   SNode current = node.firstChild();
   bool repeatMode = true;
   while (current != lxNone) {
      // HOTFIX : clear the previous register value - it doesn't work for the second loop
      scope.clear();

      if (current == lxElse) {
         jumpIfEqual(tape, current.argument, true);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxIfN) {
         jumpIfNotEqual(tape, current.argument, false);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxIfNotN) {
         jumpIfEqual(tape, current.argument, false);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxLessN) {
         jumpIfNotLess(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxNotLessN) {
         jumpIfLess(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxGreaterN) {
         jumpIfNotGreater(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (current == lxNotGreaterN) {
         jumpIfGreater(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope);

         repeatMode = false;
      }
      else if (test(current.type, lxObjectMask)) {
         scope.debugBlockStarted = false;
         generateObject(tape, current, scope);

         if (scope.debugBlockStarted) {
            declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
            scope.debugBlockStarted = false;
         }
      }

      current = current.nextNode();
   }

   if (repeatMode)
      jumpIfEqual(tape, 0, true);

   if (node.argument != 0) {
      endLoop(tape, node.argument);
   }
   else endLoop(tape);

   scope.clear();
}

void ByteCodeWriter :: generateSwitching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   declareSwitchBlock(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      scope.clear();

      if (current == lxAssigning) {
         generateObject(tape, current, scope);
      }
      else if (current == lxOption) {
         declareSwitchOption(tape);

         generateExpression(tape, current, scope);

         endSwitchOption(tape);
      }
      else if (current == lxElse) {
         generateObject(tape, current, scope);
      }

      current = current.nextNode();
   }

   endSwitchBlock(tape);
   scope.clear();
}

void ByteCodeWriter :: generateBranching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   bool switchBranching = node.argument == -1;

   if (switchBranching) {
      // labels already declared in the case of switch
   }
   else if (node.existChild(lxElse)) {
      declareThenElseBlock(tape);
   }
   else declareThenBlock(tape);

   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      scope.clear();

      switch (current.type) {
         case lxIf:
         case lxIfN:
            jumpIfNotEqual(tape, current.argument, current == lxIf);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxIfNot:
         case lxIfNotN:
            jumpIfEqual(tape, current.argument, current == lxIfNot);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxLessN:
            jumpIfNotLess(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxNotLessN:
            jumpIfLess(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxGreaterN:
            jumpIfNotGreater(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxNotGreaterN:
            jumpIfGreater(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         case lxElse:
            declareElseBlock(tape);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findSubNode(lxCode), scope);
            break;
         default:
//            if (test(current.type, lxObjectMask)) {
//               //HOTFIX : breakpoint should be generated here for better debugging
//               declareBlock(tape);
               generateObject(tape, current, scope);
//               declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
//            }
            break;
      }

      current = current.nextNode(lxObjectMask);
   }

   if(!switchBranching)
      endThenBlock(tape);

   scope.clear();
}

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

void ByteCodeWriter :: generateCloningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   if (source == lxExpression)
      source = source.findSubNodeMask(lxObjectMask);

   if (target == lxExpression)
      target = target.findSubNodeMask(lxObjectMask);

   if (source.compare(lxLocalAddress, lxBlockLocalAddr)) {
      if (source.firstChild() == lxBreakpoint) {
         translateBreakpoint(tape, source.firstChild(), scope);
      }

      if (target == lxCreatingStruct && target.argument == -1) {
         generateObject(tape, source, scope, STACKOP_MODE);

         tape.write(bcXCreate, target.findChild(lxType).argument | mskVMTRef);

         tape.write(bcClone, source.argument, bpFrame);
         releaseStack(tape);
      }
      else {
         generateObject(tape, target, scope);

         tape.write(bcCloneF, source.argument, bpFrame);
      }
   }
   else if (source.compare(lxLocal, lxTempLocal)) {
      if (source.firstChild() == lxBreakpoint) {
         translateBreakpoint(tape, source.firstChild(), scope);
      }

      generateObject(tape, source, scope, STACKOP_MODE);

      if (target == lxCreatingStruct && target.argument == -1) {
         tape.write(bcXCreate, target.findChild(lxType).argument | mskVMTRef);
      }
      else generateObject(tape, target, scope);

      tape.write(bcClone, source.argument, bpFrame);
      releaseStack(tape);
   }
   else throw InternalError("Not yet implemented"); // !! temporal
}

void ByteCodeWriter :: generateInitializingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode objNode = node.findSubNodeMask(lxObjectMask);

   SNode current = node.findChild(lxMember);
   int size = objNode.findChild(lxSize).argument;
   bool createMode = objNode == lxCreatingClass;
   bool structMode = objNode == lxCreatingStruct;
   bool onlyLocals = createMode;
   while (current != lxNone) {
      if (current == lxMember) {
         SNode memberNode = current.findSubNodeMask(lxObjectMask);
         if (!memberNode.compare(lxLocal, lxSelfLocal)) {
            onlyLocals = false;
            break;
         }
      }
      current = current.nextNode();
   }

   if (!onlyLocals) {
      current = node.lastChild();
      while (current != lxNone) {
         if (current == lxMember) {
            SNode memberNode = current.firstChild(lxObjectMask);

            generateObject(tape, memberNode, scope, STACKOP_MODE);
         }
         current = current.prevNode();
      }
      generateObject(tape, objNode, scope);
      current = node.findChild(lxMember);
      while (current != lxNone) {
         if (current == lxMember) {
            //SNode memberNode = current.firstChild(lxObjectMask);
            if (structMode) {
               copyToFieldAddress(tape, size, current.argument * size);
            }
            else tape.write(createMode ? bcXSetI : bcSetI, current.argument);
            releaseStack(tape);
         }
         current = current.nextNode();
      }
   }
   else {
      generateObject(tape, objNode, scope);
      current = node.findChild(lxMember);
      while (current != lxNone) {
         if (current == lxMember) {
            SNode memberNode = current.findSubNodeMask(lxObjectMask);
   
            tape.write(bcXSetFI, memberNode.argument, current.argument, bpFrame);
         }
         current = current.nextNode();
      }
   }
}

//void ByteCodeWriter :: generateStructExpression(CommandTape& tape, SyntaxTree::Node node)
//{
//   SNode target = node.findChild(lxTarget);
//   int itemSize = node.findChild(lxSize).argument;
//
//   // presave all the members which could create new objects
//   SNode current = node.lastChild();
//   bool withMembers = false;
//   while (current != lxNone) {
//      if (current.type == lxMember || current.type == lxOuterMember) {
//         withMembers = true;
//         if (!isSimpleObjectExpression(current)) {
//            generateExpression(tape, current, ACC_REQUIRED);
//            pushObject(tape, lxResult);
//         }
//      }
//
//      current = current.prevNode();
//   }
//
//   newStructure(tape, node.argument, target.argument);
//
//   if (withMembers) {
//      loadBase(tape, lxResult, 0, 0);
//
//      current = node.firstChild();
//      while (current != lxNone) {
//         if (current.type == lxMember/* || current.type == lxOuterMember*/) {
//            if (!isSimpleObjectExpression(current)) {
//               popObject(tape, lxResult);
//            }
//            else generateExpression(tape, current, ACC_REQUIRED);
//
//            saveStructBase(tape, lxResult, current.argument, itemSize);
//         }
//
//         current = current.nextNode();
//      }
//
//      assignBaseTo(tape, lxResult);
//   }
//
//   SNode callNode = node.findChild(lxOvreriddenMessage);
//   while (callNode != lxNone) {
//      ref_t messageTarget = callNode.findChild(lxTarget).argument;
//      if (!messageTarget)
//         messageTarget = target.argument;
//
//      // call implicit constructor
//      callInitMethod(tape, messageTarget, callNode.argument, false);
//
//      callNode = goToNode(callNode.nextNode(), lxOvreriddenMessage);
//   }
//}

void ByteCodeWriter :: generateResendingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   bool genericResending = node == lxGenericResending;

   SNode target = node.findChild(lxCallTarget);
   if (!genericResending && node.argument == 0) {
      SNode message = node.findChild(lxMessage);
      if (isOpenArg(message.argument)) {
         // if it is open argument dispatching
         tape.write(bcPushD);
         tape.write(bcOpen, 1);
         tape.write(bcPushA);

         unboxMessage(tape);
         changeMessageCounter(tape, 2, VARIADIC_MESSAGE);
         loadObject(tape, lxLocal, 1, scope, 0);

         callResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument/*, false, false*/);

         closeFrame(tape, 0);
         tape.write(bcPopD);
         tape.write(bcMQuit);
      }
      else {
         tape.write(bcPushD);
         setSubject(tape, message.argument);

         resendResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument);
      }
   }
   else {
      SNode current = node.firstChild();
      while (current != lxNone) {
         if (current == lxNewFrame) {
            int reserved = current.findChild(lxReserved).argument;

            // new frame
            newFrame(tape, 1, reserved, false);

            if (genericResending) {
               // save message
               tape.write(bcSaveF, -2);
               
               generateExpression(tape, current, scope);

               // restore message
               tape.write(bcLoadFI, -2);
            }
            else generateExpression(tape, current, scope);

            // close frame
            closeFrame(tape, 1);
         }
         else if (test(current.type, lxObjectMask)) {
            generateObject(tape, current, scope);
         }

         current = current.nextNode();
      }

      if (!genericResending)
         tape.write(bcMovM, node.argument);

//      if (target.argument != 0) {
//         resendResolvedMethod(tape, target.argument, node.argument);
//      }
      /*else */resend(tape);
   }
}

void ByteCodeWriter :: generateObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   if (node.firstChild() == lxBreakpoint) {
      translateBreakpoint(tape, node.firstChild(), scope);
   }

   bool stackOp = test(mode, STACKOP_MODE);

   switch (node.type)
   {
      case lxExpression:
//      case lxLocalUnboxing:
      case lxFieldExpression:
//      case lxAltExpression:
      case lxSeqExpression:
         generateExpression(tape, node, scope, mode & ~STACKOP_MODE);
         break;
      case lxCalling_0:
      case lxCalling_1:
      case lxDirectCalling:
      case lxSDirectCalling:
         generateCallExpression(tape, node, scope);
         break;
      case lxByRefAssigning:
         generateByRefAssigningExpression(tape, node, scope);
         break;
      case lxAssigning:
         generateAssigningExpression(tape, node, scope);
         break;
      case lxCopying:
         generateCopyingExpression(tape, node, scope);
         break;
      case lxSaving:
         generateSavingExpression(tape, node, scope);
         break;
         //      case lxInlineArgCall:
//         generateInlineArgCallExpression(tape, node);
//         break;
//      //case lxImplicitCall:
//      //   callInitMethod(tape, node.findChild(lxTarget).argument, node.argument, false);
//      //   break;
      case lxImplicitJump:
         resendResolvedMethod(tape, node.findChild(lxCallTarget).argument, node.argument);
         break;
      case lxTrying:
         generateTrying(tape, node, scope);
         break;
      case lxAlt:
         generateAlt(tape, node, scope);
         break;
////      case lxReturning:
////         generateReturnExpression(tape, node);
////         break;
////      case lxThrowing:
////         generateThrowExpression(tape, node);
////         break;
      case lxCoreAPICall:
      case lxStdExternalCall:
      case lxExternalCall:
         generateExternalCall(tape, node, scope);
         break;
      case lxInternalCall:
         generateInternalCall(tape, node, scope);
         break;
//      case lxBoxing:
//      case lxCondBoxing:
//      case lxArgBoxing:
//      case lxUnboxing:
//         generateBoxingExpression(tape, node, mode);
//         break;
//      case lxCopying:
//         generateCopying(tape, node, mode);
//         break;
      case lxBranching:
         generateBranching(tape, node, scope);
         break;
      case lxSwitching:
         generateSwitching(tape, node, scope);
         break;
      case lxLooping:
         generateLooping(tape, node, scope);
         break;
//      case lxStruct:
//         generateStructExpression(tape, node);
//         break;
      case lxInitializing:
         generateInitializingExpression(tape, node, scope);
         break;
      case lxCloning:
         generateCloningExpression(tape, node, scope);
         break;
      case lxBoolOp:
         generateBoolLogicOperation(tape, node, scope, mode & ~STACKOP_MODE);
         break;
      case lxNilOp:
         generateNilOperation(tape, node, scope);
         break;
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
         generateOperation(tape, node, scope, mode & ~STACKOP_MODE);
         break;
      case lxIntBoolOp:
      case lxLongBoolOp:
      case lxRealBoolOp:
         generateBoolOperation(tape, node, scope, mode & ~STACKOP_MODE);
         break;
      case lxIntArrOp:
      case lxByteArrOp:
      case lxShortArrOp:
      case lxArrOp:
      case lxBinArrOp:
      case lxArgArrOp:
         generateArrOperation(tape, node, scope, mode);
         break;
      case lxNewArrOp:
         generateNewArrOperation(tape, node, scope);
         break;
      case lxResending:
      case lxGenericResending:
         generateResendingExpression(tape, node, scope);
         break;
//      case lxDispatching:
//         generateDispatching(tape, node);
//         break;
//      case lxIf:
//         jumpIfNotEqual(tape, node.argument, true);
//         generateCodeBlock(tape, node);
//         break;
      case lxElse:
         if (node.argument != 0)
            jumpIfEqual(tape, node.argument, true);

         generateCodeBlock(tape, node, scope);
         break;
      case lxCreatingClass:
      case lxCreatingStruct:
         generateCreating(tape, node, scope);
         break;
      case lxYieldReturning:
         generateYieldReturn(tape, node, scope);
         break;
      case lxBoxableExpression:
      case lxArgBoxableExpression:
         throw InternalError("Unboxed expression");
         break;
      case lxReturning:
         generateReturnExpression(tape, node, scope);
         break;
      case lxExtIntConst:
         if (stackOp) {
            pushIntConstant(tape, node.findChild(lxIntValue).argument);
            stackOp = false;
         }
         else throw InternalError("Not yet implemented"); // !! temporal
         break;
      case lxExtIntArg:
         if (stackOp) {
            loadObject(tape, node.firstChild(lxObjectMask), scope);
            pushIntValue(tape);
            stackOp = false;
         }
         else throw InternalError("Not yet implemented"); // !! temporal
         break;
      case lxCodeExpression:
         generateCodeBlock(tape, node, scope);
         break;
      default:
         if (stackOp) {
            pushObject(tape, node.type, node.argument, scope, mode);
            stackOp = false;
         }
         else loadObject(tape, node.type, node.argument, scope, mode);
         break;
   }

   if (stackOp)
      pushObject(tape, lxResult, 0, scope, mode);
}

void ByteCodeWriter :: generateExpression(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      if (current == lxExternFrame) {
         generateExternFrame(tape, current, scope);
      }
      else generateObject(tape, current, scope, mode);
//      else if (current == lxTapeArgument) {
//         SNode nextNode = current.nextNode();
//
//         nextNode.setArgument(nextNode.argument + tape.resolveArgument(current.argument));
//         //setAssignsize(node);
//      }
//      else generateDebugInfo(tape, current);

      current = current.nextNode(lxObjectMask);
   }
}

void ByteCodeWriter :: generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset)
{
   saveIntConstant(tape, lxLocalAddress, offset + 2, 0x800000 + node.argument);
}

void ByteCodeWriter :: generateDebugInfo(CommandTape& tape, SyntaxTree::Node current)
{
   LexicalType type = current.type;
   switch (type)
   {
      case lxVariable:
         declareLocalInfo(tape,
            current.firstChild(lxTerminalMask).identifier(),
            current.findChild(lxLevel).argument);
         break;
      case lxIntVariable:
         declareLocalIntInfo(tape,
            current.findChild(lxIdentifier/*, lxPrivate*/).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      case lxLongVariable:
         declareLocalLongInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      case lxReal64Variable:
         declareLocalRealInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      //case lxMessageVariable:
      //   declareMessageInfo(tape, current.identifier());
      //   break;
      case lxParamsVariable:
         declareLocalParamsInfo(tape,
            current.firstChild(lxTerminalMask).identifier(),
            current.findChild(lxLevel).argument);
         break;
      case lxBytesVariable:
      {
         int level = current.findChild(lxLevel).argument;

         generateBinary(tape, current, level);
         declareLocalByteArrayInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            level, false);
         break;
      }
      case lxShortsVariable:
      {
         int level = current.findChild(lxLevel).argument;

         generateBinary(tape, current, level);
         declareLocalShortArrayInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            level, false);
         break;
      }
      case lxIntsVariable:
      {
         int level = current.findChild(lxLevel).argument;

         generateBinary(tape, current, level);

         declareLocalIntArrayInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            level, false);
         break;
      }
      case lxBinaryVariable:
      {
         int level = current.findChild(lxLevel).argument;

         // HOTFIX : only for dynamic objects
         if (current.argument != 0)
            generateBinary(tape, current, level);

         declareStructInfo(tape,
            current.findChild(lxIdentifier).identifier(),
            level, current.findChild(lxClassName).identifier());
         break;
      }
//      case lxBreakpoint:
//         translateBreakpoint(tape, current, false);
//         break;
   }
}

void ByteCodeWriter :: generateCodeBlock(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode current = node.firstChild(lxStatementMask);
   while (current != lxNone) {
      LexicalType type = current.type;
      switch (type)
      {
         case lxExpression:
         case lxSeqExpression:
            scope.debugBlockStarted = false;
            generateExpression(tape, current, scope);

            if (scope.debugBlockStarted) {
               declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
               scope.debugBlockStarted = false;
            }
            break;
         case lxReturning:
            scope.debugBlockStarted = false;
            generateReturnExpression(tape, current, scope);

            if (scope.debugBlockStarted) {
               declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
               scope.debugBlockStarted = false;
            }
            break;
//         case lxExternFrame:
//            generateExternFrame(tape, current);
//            break;
////         case lxReleasing:
////            releaseObject(tape, current.argument);
////            break;
         case lxBinarySelf:
            declareSelfStructInfo(tape, SELF_VAR, current.argument,
               current.findChild(lxClassName).identifier());
            break;
//         case lxBreakpoint:
//            translateBreakpoint(tape, current, false);
//            break;
         case lxVariable:
         case lxIntVariable:
         case lxLongVariable:
         case lxReal64Variable:
//////         case lxMessageVariable:
         case lxParamsVariable:
         case lxBytesVariable:
         case lxShortsVariable:
         case lxIntsVariable:
         case lxBinaryVariable:
            generateDebugInfo(tape, current);
            break;
         case lxYieldDispatch:
            generateYieldDispatch(tape, current, scope);
            break;
         case lxEOP:
            scope.debugBlockStarted = true;
            if (current.firstChild() == lxBreakpoint)
               translateBreakpoint(tape, current.findChild(lxBreakpoint), scope);
            scope.debugBlockStarted = false;
            break;
         case lxCode:
         case lxCodeExpression:
            // HOTFIX : nested code, due to resend implementation
            generateCodeBlock(tape, current, scope);
            break;
         default:
            generateObject(tape, current, scope);
            break;
      }
      current = current.nextNode(lxStatementMask);
   }
}

void ByteCodeWriter :: importCode(CommandTape& tape, ImportScope& scope, bool withBreakpoints)
{
   ByteCodeIterator it = tape.end();

   tape.import(scope.section, true, withBreakpoints);

   // goes to the first imported command
   it++;

   // import references
   while (!it.Eof()) {
      CommandTape::importReference(*it, scope.sour, scope.dest);
      it++;
   }
}

void ByteCodeWriter :: doMultiDispatch(CommandTape& tape, ref_t operationList, ref_t message)
{
   tape.write(bcMTRedirect, operationList | mskConstArray, message);
}

void ByteCodeWriter :: doSealedMultiDispatch(CommandTape& tape, ref_t operationList, ref_t message)
{
   tape.write(bcXMTRedirect, operationList | mskConstArray, message);
}

void ByteCodeWriter :: generateMultiDispatching(CommandTape& tape, SyntaxTree::Node node, ref_t message)
{
   if (node.type == lxSealedMultiDispatching) {
      doSealedMultiDispatch(tape, node.argument, message);
   }
   else doMultiDispatch(tape, node.argument, message);

//   SNode current = node.findChild(lxDispatching, /*lxResending, */lxCalling);
//   switch (current.type) {
//      case lxDispatching:
//         generateResending(tape, current);
//         break;
//      //case lxResending:
//      //   // if there is an ambiguity with open argument list handler
//      //   tape.write(bcCopyM, current.findChild(lxOvreriddenMessage).argument);
//      //   generateResendingExpression(tape, current);
//      //   break;
//      case lxCalling:
//         // if it is a multi-method conversion
//         generateCallExpression(tape, current);
//         break;
//      default:
//         break;
//   }
}

//void ByteCodeWriter :: generateResending(CommandTape& tape, SyntaxTree::Node node)
//{
//   if (node.argument != 0) {
//      tape.write(bcCopyM, node.argument);
//
//      SNode target = node.findChild(lxTarget);
//      if (target == lxTarget) {
//         resendResolvedMethod(tape, target.argument, node.argument);
//      }
//      else resend(tape);
//   }
//}

void ByteCodeWriter :: generateDispatching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   if (node.argument != 0) {
      // if it is a generic dispatcher with the custom target
      tape.write(bcPushD);
      setSubject(tape, node.argument);
      doGenericHandler(tape);
      tape.write(bcPopD);
   }
   doGenericHandler(tape);

   generateExpression(tape, node, scope);
}

void ByteCodeWriter :: generateCreating(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode target = node.findChild(lxType);

   int size = node.argument;
   if (node == lxCreatingClass) {
      newObject(tape, size, target.argument);
      clearObject(tape, size);
   }
   else if (node == lxCreatingStruct) {
      if (size < 0) {
         // NOTE : in normal case this code should never be reached
         throw InternalError("Invalid creation operation");
      }
      else newStructure(tape, size, target.argument);
   }

   scope.clear();
}

void ByteCodeWriter :: generateMethodDebugInfo(CommandTape& tape, SyntaxTree::Node node)
{
   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxMessageVariable:
            declareMessageInfo(tape, current.identifier());
            break;
         case lxVariable:
            declareLocalInfo(tape,
               current.firstChild(lxTerminalMask).identifier(),
               current.findChild(lxLevel).argument);
            break;
         case lxSelfVariable:
            declareSelfInfo(tape, current.argument);
            break;
         case lxBinarySelf:
            declareSelfStructInfo(tape, SELF_VAR, current.argument,
               current.findChild(lxClassName).identifier());
            break;
         case lxIntVariable:
            declareLocalIntInfo(tape,
               current.firstChild(lxTerminalMask).identifier(),
               current.findChild(lxLevel).argument, true);
         case lxLongVariable:
            declareLocalLongInfo(tape,
               current.firstChild(lxTerminalMask).identifier(),
               current.findChild(lxLevel).argument, true);
         case lxReal64Variable:
            declareLocalRealInfo(tape,
               current.firstChild(lxTerminalMask).identifier(),
               current.findChild(lxLevel).argument, true);
            break;
         case lxParamsVariable:
            declareLocalParamsInfo(tape,
               current.firstChild(lxTerminalMask).identifier(),
               current.findChild(lxLevel).argument);
            break;
         case lxBinaryVariable:
         {
            int level = current.findChild(lxLevel).argument;

            // HOTFIX : only for dynamic objects
            if (current.argument != 0)
               generateBinary(tape, current, level);

            declareStructInfo(tape,
               current.findChild(lxIdentifier).identifier(),
               level, current.findChild(lxClassName).identifier());
            break;
         }
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: generateYieldDispatch(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   generateExpression(tape, node, scope);
   
   // ; jump if state is set
   // load
   // ifn labStart 0
   // nop
   // ajumpi 0
   // labStart:

   tape.newLabel();

   tape.write(bcLoad);
   tape.write(bcIfN, baCurrentLabel, 0);
   tape.write(bcNop);
   tape.write(bcJumpI, 0);
   tape.setLabel();
}

//void ByteCodeWriter :: generateYieldStop(CommandTape& tape, SyntaxTree::Node node)
//{
//   // ; clear state
//   // aloadfi 1
//   // bloadai index
//   // dcopy 0
//   // nsave
//
//   tape.write(bcALoadFI, 1, bpFrame);
//   tape.write(bcBLoadAI, node.argument);
//   tape.write(bcDCopy, 0);
//   tape.write(bcNSave);
//}

void ByteCodeWriter :: generateYieldReturn(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   // address labNext
   // peekfi 1
   // geti index
   // save

   // <expr>

   // jump labReturn
   // labNext:

   tape.newLabel();
   tape.write(bcAddress, baCurrentLabel);
   tape.write(bcPeekFI, 1, bpFrame);
   tape.write(bcGetI, node.argument);
   tape.write(bcSave);

   scope.clear();

   generateExpression(tape, node, scope);

   gotoEnd(tape, baFirstLabel);

   tape.setLabel();
}

void ByteCodeWriter :: generateMethod(CommandTape& tape, SyntaxTree::Node node, ref_t sourcePathRef)
{
   FlowScope scope;
   bool open = false;
   bool withNewFrame = false;
   bool exit = false;
   bool exitLabel = true;

   int argCount = node.findChild(lxArgCount).argument;
   int reserved = node.findChild(lxReserved).argument;
   int allocated = node.findChild(lxAllocated).argument;

   ref_t methodSourcePathRef = node.findChild(lxSourcePath).argument;
   if (methodSourcePathRef)
      sourcePathRef = methodSourcePathRef;

   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxDispatching:
            exit = true;
            if (!open) {
               open = true;
               exitLabel = false;
               declareIdleMethod(tape, node.argument, sourcePathRef);
            }
            generateDispatching(tape, current, scope);
            break;
         case lxMultiDispatching:
         case lxSealedMultiDispatching:
            if (!open) {
               declareIdleMethod(tape, node.argument, sourcePathRef);
               exitLabel = false;
               open = true;
            }

            generateMultiDispatching(tape, current, node.argument);
            break;
         //         case lxYieldStop:
         //            generateYieldStop(tape, current);
         //            break;
         //         case lxNil:
         //            // idle body;
                  // open = true;
         //            declareIdleMethod(tape, node.argument, sourcePathRef);
         //            break;
         case lxImporting:
            if (!open) {
               open = true;

               declareIdleMethod(tape, node.argument, sourcePathRef);
            }
            importCode(tape, *imports.get(current.argument - 1), true);
            exit = true; // NOTE : the imported code should already contain an exit command
            break;
         case lxNewFrame:
            withNewFrame = true;
            if (!open) {
               declareMethod(tape, node.argument, sourcePathRef, reserved, allocated, current.argument == -1);
               open = true;
            }
            else {
               newFrame(tape, reserved, allocated, current.argument == -1);
               if (!exitLabel) {
                  tape.newLabel();     // declare exit point
               }
            }
            exitLabel = true;
            generateMethodDebugInfo(tape, node);   // HOTFIX : debug info should be declared inside the frame body
            generateCodeBlock(tape, current, scope);
            break;
         default:
            if (test(current.type, lxObjectMask)) {
               if (!open) {
                  open = true;
                  exitLabel = false;

                  declareIdleMethod(tape, node.argument, sourcePathRef);
               }

               generateObject(tape, current, scope);
            }
            break;
      }

      current = current.nextNode();
   }

   if (!open) {
      declareIdleMethod(tape, node.argument, sourcePathRef);

      tape.newLabel();     // declare exit point
      exitMethod(tape, argCount, reserved, false);

      endIdleMethod(tape);
   }
   else if (!exit) {
      if (!exitLabel)
         tape.newLabel();     // declare exit point

      endMethod(tape, argCount, reserved, withNewFrame);
   }
   else endIdleMethod(tape);
}

//////void ByteCodeWriter :: generateTemplateMethods(CommandTape& tape, SNode root)
//////{
//////   SyntaxTree::Node current = root.firstChild();
//////   while (current != lxNone) {
//////      if (current == lxClassMethod) {
//////         generateMethod(tape, current);
//////
//////         // HOTFIX : compile nested template methods
//////         generateTemplateMethods(tape, current);
//////      }
//////      else if (current == lxIdle) {
//////         // HOTFIX : analize nested template methods
//////         generateTemplateMethods(tape, current);
//////      }
//////      else if (current == lxTemplate) {
//////         generateTemplateMethods(tape, current);
//////      }
//////
//////      current = current.nextNode();
//////   }
//////}

void ByteCodeWriter :: generateClass(CommandTape& tape, SNode root, ref_t reference, pos_t sourcePathRef, bool(*cond)(LexicalType))
{
   declareClass(tape, reference);
   SyntaxTree::Node current = root.firstChild();
   while (current != lxNone) {
      if (cond(current.type)) {
         generateMethod(tape, current, sourcePathRef);
      }
      current = current.nextNode();
   }

   endClass(tape);

   //tape.clearArguments();
}

//void ByteCodeWriter :: generateInitializer(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument)
//{
//   declareInitializer(tape, reference);
//   loadObject(tape, type, argument, 0);
//   endInitializer(tape);
//}

void ByteCodeWriter :: generateInitializer(CommandTape& tape, ref_t reference, SNode root)
{
   FlowScope scope;

   declareInitializer(tape, reference);
   generateCodeBlock(tape, root, scope);
   endInitializer(tape);
}

void ByteCodeWriter :: generateSymbol(CommandTape& tape, SNode root, bool isStatic, pos_t sourcePathRef)
{
   if (isStatic) {
      declareStaticSymbol(tape, root.argument, sourcePathRef);
   }
   else declareSymbol(tape, root.argument, sourcePathRef);

   FlowScope scope;

   scope.debugBlockStarted = false;
   generateExpression(tape, root, scope);

   if (scope.debugBlockStarted)
      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);

   if (isStatic) {
      endStaticSymbol(tape, root.argument);
   }
   else endSymbol(tape);
}

void ByteCodeWriter :: generateConstantMember(MemoryWriter& writer, LexicalType type, ref_t argument)
{
   switch (type) {
      case lxConstantChar:
      //case lxConstantClass:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantList:
      case lxConstantReal:
      case lxConstantString:
      case lxConstantWideStr:
      case lxConstantSymbol:
         writer.writeRef(argument | defineConstantMask(type), 0);
         break;
      case lxNil:
         writer.writeDWord(0);
         break;
   }
}

void ByteCodeWriter :: generateConstantList(SNode node, _Module* module, ref_t reference)
{
   SNode target = node.findChild(lxType);
   MemoryWriter writer(module->mapSection(reference | mskRDataRef, false));
   SNode current = node.firstChild();
   while (current != lxNone) {
      SNode object = current.findSubNodeMask(lxObjectMask);

      generateConstantMember(writer, object.type, object.argument);

      current = current.nextNode();
   }

   // add vmt reference
   if (target != lxNone)
      writer.Memory()->addReference(target.argument | mskVMTRef, (pos_t)-4);
}
