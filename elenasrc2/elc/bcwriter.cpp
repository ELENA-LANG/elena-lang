//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

//#define FULL_OUTOUT_INFO 1

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

using namespace _ELENA_;

constexpr auto STACKOP_MODE      = 0x0001;
constexpr auto BOOL_ARG_EXPR     = 0x0002;
constexpr auto NOBREAKPOINTS     = 0x0004;

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
   tape.write(bcAllocI, 1);
   tape.write(bcStoreSI, 0);

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

void ByteCodeWriter :: declareIdleMethod(CommandTape& tape, mssg_t message, ref_t sourcePathRef)
{
   // method-begin:
   tape.write(blBegin, bsMethod, message);

   if (sourcePathRef != INVALID_REF)
      tape.write(bdSourcePath, sourcePathRef);
}

void ByteCodeWriter :: declareAbstractMethod(CommandTape& tape, mssg_t message)
{
   // method-begin:
   tape.write(blBegin, bsAbstractMethod, message);
}

void ByteCodeWriter :: declareMethod(CommandTape& tape, mssg_t message, ref_t sourcePathRef, int reserved, int allocated,
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
         tape.write(bcOpen);
         tape.write(bcReserve, reserved << 2);   // NOTE : converting to actual value
      }
      else tape.write(bcOpen);

      if (withPresavedMessage) {
         tape.write(bcSaveF, -4);
      }

      if (allocated > 0) {
         allocateStack(tape, allocated);

         // NOTE : self should be automatically saved
         tape.write(bcStoreFI, 1);
      }

   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: excludeFrame(CommandTape& tape)
{
   tape.write(bcExclude);
}

void ByteCodeWriter :: includeFrame(CommandTape& tape, bool withThreadSafeNop)
{
   tape.write(bcInclude);
   if (withThreadSafeNop)
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
   tape.write(includeFrame ? bdIntLocal : bdIntLocalPtr, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(includeFrame ? bdLongLocal : bdLongLocalPtr, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(includeFrame ? bdRealLocal : bdRealLocalPtr, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level)
{
   tape.write(bdByteArrayLocal, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level)
{
   tape.write(bdShortArrayLocal, writeString(localName), level);
}

void ByteCodeWriter :: declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level)
{
   tape.write(bdIntArrayLocal, writeString(localName), level);
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
   if (count > 0)
      tape.write(bcAllocI, count);
}

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

   //tape.write(bcRe tack);
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
      tape.write(bcAllocI, 1);
      tape.write(bcStoreSI, 0);
      generateCodeBlock(tape, finallyNode, scope, false);
      tape.write(bcPeekSI, 0);
      tape.write(bcFreeI, 1);

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

void ByteCodeWriter :: openFrame(CommandTape& tape, int reserved)
{
   tape.write(bcOpen);
   tape.write(bcReserve, reserved << 2); // NOTE : converting to actual value
}

void ByteCodeWriter :: newFrame(CommandTape& tape, int reserved, int allocated, bool withPresavedMessage, bool withPresavedAcc)
{
   //   open 1
   //   pusha
   if (reserved > 0) {
      // to include new frame header
      tape.write(bcOpen);
      tape.write(bcReserve, reserved << 2); // NOTE : converting to actual value
   }
   else tape.write(bcOpen);

   if (withPresavedMessage) {
      tape.write(bcSaveF, -4);
   }

   allocateStack(tape, allocated);
   if (withPresavedAcc) {
      tape.write(bcStoreFI, 1);
   }
}

void ByteCodeWriter :: closeFrame(CommandTape& tape, int reserved)
{
   if (reserved > 0) {
      tape.write(bcRestore, reserved << 2); // NOTE : converting to actual value
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
   else tape.write(bcNewN, 0, size);
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
//      case lxExtMessageConstant:
//         return mskExtMessage;
      case lxSubjectConstant:
         return mskMessageName;
      case lxConstantList:
         return mskConstArray;
      default:
         return mskConstantRef;
   }
}

void ByteCodeWriter :: unboxArgList(CommandTape& tape, int fixedLen/*, bool arrayMode*/)
{
//   if (arrayMode) {
//      // pushn -1
//      // len
//      // labNext:
//      // dec 1
//      // push
//      // elsen labNext
//      tape.write(bcPushN, -1);
//      tape.write(bcCount);
//      tape.newLabel();
//      tape.setLabel(true);
//      tape.write(bcDec, 1);
//      tape.write(bcPush);
//      tape.write(bcElseN, baCurrentLabel, 0);
//      tape.releaseLabel();
//   }
//   else {
      // pusha
      // movn   0
      // labSearch:
      // peek
      // inc    1
      // elser labSearch -1
      // popa
      
      // inc fixedLen
      // allocd
      // dec fixedLen

      // pusha
      // movsip  fixedLen + 1
      // labNextParam:
      // dec     1
      // xtrans
      // elsen   labNextParam, 0
      // pop

      tape.write(bcPushA);
      tape.write(bcMovN, 0);
      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcPeek);
      tape.write(bcInc, 1);
      tape.write(bcElseR, baCurrentLabel, -1);
      tape.releaseLabel();
      tape.write(bcPopA);

      if (fixedLen > 0) {
         tape.write(bcInc, fixedLen);
         tape.write(bcAllocD);
         tape.write(bcDec, fixedLen);
      }
      else tape.write(bcAllocD);
      
      tape.write(bcPushA);
      tape.write(bcMovSIP, fixedLen + 1);

      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcDec, 1);
      tape.write(bcXTrans);
      tape.write(bcElseN, baCurrentLabel, 0);

      tape.write(bcPop);
      tape.releaseLabel();
//   }
}

void ByteCodeWriter :: popObject(CommandTape& tape, LexicalType sourceType)
{
   switch (sourceType) {
      case lxResult:
         // popa
         tape.write(bcPopA);
         break;
   }
}

void ByteCodeWriter :: releaseStack(CommandTape& tape, int count)
{
   // NOTE : the actual size of released stack can be more than count
   // so it should be used only in pair with alloci / xalloci

   // freei n
   if (count >= 1)
      tape.write(bcFreeI, count);
}

void ByteCodeWriter :: releaseArg(CommandTape& tape)
{
   tape.write(bcPop);
}

void ByteCodeWriter :: releaseArgList(CommandTape& tape)
{
   // pusha
   // pushsip 1
   // movn    0
   // labSearch:
   // peek
   // inc     1
   // elser   labSearch -1
   // pop
   // popa
   // freed

   tape.write(bcPushA);
   tape.write(bcPushSIP, 1);
   tape.write(bcMovN, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcPeek);
   tape.write(bcInc, 1);
   tape.write(bcElseR, baCurrentLabel, -1);
   tape.releaseLabel();
   tape.write(bcPop);
   tape.write(bcPopA);
   tape.write(bcFreeD);
}

void ByteCodeWriter :: setSubject(CommandTape& tape, ref_t subject)
{
   // movv subj
   tape.write(bcMovV, getAction(subject));
}

void ByteCodeWriter :: resendDirectResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message, bool sealedMode)
{
   // jumprm r, m
   if (sealedMode) {
      tape.write(bcJumpRM, reference | mskVMTMethodAddress, message);
   }
   else tape.write(bcVJumpRM, reference | mskVMTEntryOffset, message);
}

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message/*, bool invokeMode, bool withValidattion*/)
{
   // validate
   // callrm r, m

//   if(withValidattion)
//      tape.write(bcValidate);

   tape.write(bcCallRM, reference | mskVMTMethodAddress, message);
}

void ByteCodeWriter :: callVMTResolvedMethod(CommandTape& tape, ref_t reference, mssg_t message/*, bool invokeMode*/)
{
   // vcallrm r, m

   tape.write(bcVCallRM, reference | mskVMTEntryOffset, message);
}

void ByteCodeWriter :: doGenericHandler(CommandTape& tape)
{
   // bsredirect

   tape.write(bcBSRedirect);
}

void ByteCodeWriter :: changeMessageCounter(CommandTape& tape, int arg, int paramCount, int flags)
{
   // ; change param count
   // loadf -4
   // and ~PARAM_MASK
   // orn OPEN_ARG_COUNT

   tape.write(bcLoadF, arg);
   tape.write(bcAnd, ~ARG_MASK);
   tape.write(bcOr, paramCount | flags);
}

void ByteCodeWriter :: unboxMessage(CommandTape& tape)
{
   // ; copy the call stack
   // mcount
   // inc     1
   // allocd
   //
   // movsip  0
   // pushfip -1
   // dec     1
   // xsetr   -1
   // ifn     labSkip, 0
   // labNextParam:
   // dec     1
   // xtrans
   // elsen   labNextParam, 0
   // labSkip:
   // pop

   tape.write(bcMCount);
   tape.write(bcInc, 1);
   tape.write(bcAllocD);

   tape.write(bcMovSIP, 0);
   tape.write(bcPushFIP, -1);
   tape.write(bcDec, 1);
   tape.write(bcXSetR, -1);
   tape.newLabel();
   tape.write(bcIfN, baCurrentLabel, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcDec, 1);
   tape.write(bcXTrans);
   tape.write(bcElseN, baCurrentLabel, 0);
   tape.releaseLabel();

   tape.setLabel();
   tape.write(bcPop);
}

void ByteCodeWriter :: resend(CommandTape& tape)
{
   // jumpvi 0
   tape.write(bcJumpVI);
}

void ByteCodeWriter :: callExternal(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed)
{
   int flags = (argsToBeFreed ? baReleaseArgs : 0) | baExternalCall;

   // callextr ref, flags
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount | flags);
}

void ByteCodeWriter :: callLongExternal(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed)
{
   int flags = (argsToBeFreed ? baReleaseArgs : 0) | baExternalCall | baLongCall;

   // callextr ref
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount | flags);
}

void ByteCodeWriter :: callCore(CommandTape& tape, ref_t functionReference, int paramCount, bool argsToBeFreed)
{
   int flags = (argsToBeFreed ? baReleaseArgs : 0);

   // callextr ref, flags
   tape.write(bcCallExtR, functionReference | mskNativeCodeRef, paramCount | flags);
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

void ByteCodeWriter :: exitMethod(CommandTape& tape, int reserved, bool withFrame)
{
   // labExit:
   //   restore reserved / nop
   //   close
   //   quit
   // end

   // NOTE : index register should not be modified
   tape.setLabel();
   if (withFrame) {
      if (reserved > 0) {
         tape.write(bcRestore, reserved << 2); // NOTE : converting to actual value
      }
      tape.write(bcClose);
   }

   tape.write(bcQuit);
}

void ByteCodeWriter :: endMethod(CommandTape& tape, int reserved, bool withFrame)
{
   exitMethod(tape, reserved, withFrame);

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

   tape.write(bcSwap);
   freeLock(tape);

   // throw
   tape.write(bcThrow);

   endCatch(tape);

   tape.write(bcSwap);
   freeLock(tape);
   tape.write(bcPeekSI, 0);
   tape.write(bcFreeI, 1);

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

   DebugLineInfo info;
   info.symbol = symbol;
   info.addresses.local.nameRef = scope.debugStrings->Position();
   info.addresses.local.level = level;

   scope.debugStrings->writeLiteral(localName);
   scope.debug->write((char*)&info, sizeof(DebugLineInfo));

   if (level < 0) {
      DebugLineInfo frameInfo;
      frameInfo.symbol = dsFrameOffset;
      frameInfo.addresses.offset.disp = frameLevel;

      scope.debug->write((char*)&frameInfo, sizeof(DebugLineInfo));
   }
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

void ByteCodeWriter :: writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings,
   _Module* module)
{
   bool structure = test(info.header.flags, elStructureRole);
   int remainingSize = info.size;

   ClassInfo::FieldMap::Iterator it = info.fields.start();
   while (!it.Eof()) {
      if (!emptystr(it.key())) {
         DebugLineInfo symbolInfo(dsField, 0, 0, 0);
         ref_t typeRef = 0;

         symbolInfo.addresses.field.nameRef = debugStrings->Position();
         if (structure) {
            int nextOffset = getNextOffset(it);
            if (nextOffset == -1) {
               symbolInfo.addresses.field.size = remainingSize;
            }
            else symbolInfo.addresses.field.size = nextOffset - *it;

            remainingSize -= symbolInfo.addresses.field.size;

            typeRef = info.fieldTypes.get(*it).value1;
         }

         debugStrings->writeLiteral(it.key());

         writer->write((void*)&symbolInfo, sizeof(DebugLineInfo));

         if (typeRef && !isPrimitiveRef(typeRef)) {
            DebugLineInfo typeInfo;
            typeInfo.symbol = dsFieldInfo;
            typeInfo.addresses.source.nameRef = debugStrings->Position();

            ident_t className = module->resolveReference(typeRef);
            if (isTemplateWeakReference(className) || !isWeakReference(className)) {
               // HOTFIX : save weak template-based class name directly
               debugStrings->writeLiteral(className);
            }
            else {
               IdentifierString fullName(module->Name(), className);

               debugStrings->writeLiteral(fullName.c_str());
            }

            writer->write((char*)&typeInfo, sizeof(DebugLineInfo));
         }
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

   pos_t position = debugStrings->Position();
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

   pos_t position = debugStrings->Position();

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
      writeFieldDebugInfo(info, &debugWriter, &debugStringWriter, compilerScope.module);

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
            else if ((*it).Argument() == bsAbstractMethod) {
               scope.vmt->writeDWord((*it).additional);                     // Message ID
               scope.vmt->writeDWord(INVALID_REF);                          // NOTE : -1 for an abstract entry

               writeAbstractProcedure(++it, scope);
            }
            break;
      };
      it++;
   }
   // save the real section size
   (*scope.vmt->Memory())[classPosition - 4] = scope.vmt->Position() - classPosition;
}

void ByteCodeWriter :: writeAbstractProcedure(ByteCodeIterator& it, Scope& scope)
{
   // go to the end
   int level = 1;
   while (!it.Eof() && level > 0) {
      // save command
      switch (*it) {
         case blBegin:
            level++;
            break;
         case blEnd:
            level--;
            break;
      }
      if (level == 0)
         break;
      it++;
   }

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

   int frameLevel = 0;
   int level = 1;
   while (!it.Eof() && level > 0) {
      // save command
      switch (*it) {
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
         case blEnd:
            level--;
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
         case bdIntLocalPtr:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntLocalPtr, 0);
            break;
         case bdIntLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntLocal, frameLevel);
            break;
         case bdLongLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsLongLocal, frameLevel);
            break;
         case bdLongLocalPtr:
            writeLocal(scope, (const char*)(const char*)_strings.get((*it).Argument()), (*it).additional, dsLongLocalPtr, 0);
            break;
         case bdRealLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsRealLocal, frameLevel);
            break;
         case bdRealLocalPtr:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsRealLocalPtr, 0);
            break;
         case bdByteArrayLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsByteArrayLocalPtr, 0);
            break;
         case bdShortArrayLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsShortArrayLocalPtr, 0);
            break;
         case bdIntArrayLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsIntArrayLocalPtr, 0);
            break;
         case bdParamsLocal:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsParamsLocal, frameLevel);
            break;
         case bdMessage:
            writeMessageInfo(scope, dsMessage, (const char*)_strings.get((*it).additional));
            break;
         case bdStruct:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsStructPtr, 0);

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
         case bcReserve:
            frameLevel = (*it).argument >> 2;
            (*it).save(scope.code);
            break;
         case bcIfR:
         case bcElseR:
         case bcElseD:
         case bcIf:
         case bcIfCount:
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
         case bcIfHeap:
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

//void ByteCodeWriter :: saveIntConstant(CommandTape& tape, LexicalType target, int targetArg, int value)
//{
//   if (target == lxLocalAddress) {
//      // xsavef arg, value
//
//      tape.write(bcXSaveF, targetArg << 2, value);
//   }
//   else throw InternalError("Not yet implemented");
//}

void ByteCodeWriter :: saveLength(CommandTape& tape, LexicalType target, int targetArg, int value)
{
   if (target == lxLocalAddress) {
      // xsavef arg, value

      tape.write(bcXSaveLenF, targetArg << 2, value);
   }
   else throw InternalError("Not yet implemented");
}

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
         tape.write(bcNAddF, localOffset << 2);
         break;
      case SUB_OPERATOR_ID:
         // nsubf i
         tape.write(bcNSubF, localOffset << 2);
         break;
      case MUL_OPERATOR_ID:
         // nmulf i
         tape.write(bcNMulF, localOffset << 2);
         break;
      case DIV_OPERATOR_ID:
         // ndivf i
         tape.write(bcNDivF, localOffset << 2);
         break;
      case SHIFTL_OPERATOR_ID:
         // nshlf i
         tape.write(bcNShlF, localOffset << 2);
         break;
      case SHIFTR_OPERATOR_ID:
         // nshrf i
         tape.write(bcNShrF, localOffset << 2);
         break;
      case BAND_OPERATOR_ID:
         // nandf i
         tape.write(bcNAndF, localOffset << 2);
         break;
      case BOR_OPERATOR_ID:
         // norf i
         tape.write(bcNOrF, localOffset << 2);
         break;
      case BXOR_OPERATOR_ID:
         // nxorf i
         tape.write(bcNXorF, localOffset << 2);
         break;
      case NEGATIVE_OPERATOR_ID:
         tape.write(bcXSaveF, localOffset << 2, 0);
         tape.write(bcNSubF, localOffset << 2);
         break;
      case BINVERTED_OPERATOR_ID:
         tape.write(bcLoad);
         tape.write(bcNot);
         tape.write(bcSaveF, localOffset << 2);
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
         tape.write(bcXSaveF, localOffset << 2, immValue);
         break;
      case BINVERTED_OPERATOR_ID:
         // xsavef i, v
         tape.write(bcXSaveF, localOffset << 2, ~immValue);
         break;
      case ADD_OPERATOR_ID:
         // xaddf i, v
         tape.write(bcXAddF, localOffset << 2, immValue);
         break;
      case SUB_OPERATOR_ID:
         // xaddf i, -v
         tape.write(bcXAddF, localOffset << 2, -immValue);
         break;
      case MUL_OPERATOR_ID:
         // loadf
         // mul v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcMul, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case DIV_OPERATOR_ID:
         // loadf
         // div v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcDiv, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case SHIFTL_OPERATOR_ID:
         // loadf
         // shl v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcShl, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case SHIFTR_OPERATOR_ID:
         // loadf
         // shr v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcShr, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case BAND_OPERATOR_ID:
         // loadf
         // and v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcAnd, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case BOR_OPERATOR_ID:
         // loadf
         // or v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcOr, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      case BXOR_OPERATOR_ID:
         // loadf
         // xor v
         // savef
         tape.write(bcLoadF, localOffset << 2);
         tape.write(bcXOr, immValue);
         tape.write(bcSaveF, localOffset << 2);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: doRealIntOperation(CommandTape& tape, int operator_id, int localOffset, int immValue)
{
   switch (operator_id) {
      case SET_OPERATOR_ID:
         // xrsavef i, v
         tape.write(bcXRSaveF, localOffset << 2, immValue);
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
         tape.write(bcLAddF, localOffset << 2);
         break;
      case SUB_OPERATOR_ID:
         // lsubf i
         tape.write(bcLSubF, localOffset << 2);
         break;
      case MUL_OPERATOR_ID:
         // lmulf i
         tape.write(bcLMulF, localOffset << 2);
         break;
      case DIV_OPERATOR_ID:
         // ldivf i
         tape.write(bcLDivF, localOffset << 2);
         break;
      case SHIFTL_OPERATOR_ID:
         // lshlf i
         tape.write(bcLShlF, localOffset << 2);
         break;
      case SHIFTR_OPERATOR_ID:
         // lshrf i
         tape.write(bcLShrF, localOffset << 2);
         break;
      case BAND_OPERATOR_ID:
         // landf i
         tape.write(bcLAndF, localOffset << 2);
         break;
      case BOR_OPERATOR_ID:
         // lorf i
         tape.write(bcLOrF, localOffset << 2);
         break;
      case BXOR_OPERATOR_ID:
         // lxorf i
         tape.write(bcLXorF, localOffset << 2);
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
         tape.write(bcRAddF, localOffset << 2);
         break;
      case SUB_OPERATOR_ID:
         // rsubf i
         tape.write(bcRSubF, localOffset << 2);
         break;
      case MUL_OPERATOR_ID:
         // rmulf i
         tape.write(bcRMulF, localOffset << 2);
         break;
      case DIV_OPERATOR_ID:
         // rdivf i
         tape.write(bcRDivF, localOffset << 2);
         break;
      default:
         throw InternalError("not yet implemente"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: doRealIntOperation(CommandTape& tape, int operator_id, int localOffset)
{
   switch (operator_id) {
   case ADD_OPERATOR_ID:
      // raddnf i
      tape.write(bcRAddNF, localOffset << 2);
      break;
   case SUB_OPERATOR_ID:
      // rsubnf i
      tape.write(bcRSubNF, localOffset << 2);
      break;
   case MUL_OPERATOR_ID:
      // rmulnf i
      tape.write(bcRMulNF, localOffset << 2);
      break;
   case DIV_OPERATOR_ID:
      // rdivnf i
      tape.write(bcRDivNF, localOffset << 2);
      break;
   default:
      throw InternalError("not yet implemente"); // !! temporal
      break;
   }
}

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
         // pop
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
         tape.write(bcPop);
         tape.write(bcSaveF, argument << 2);

         break;
//      case SET_REFER_OPERATOR_ID:
//         // xset
//         tape.write(bcXSet);
//         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doArgArrayOperation(CommandTape& tape, int operator_id, int, int immValue)
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
               tape.write(bcSaveF, target << 2);
               break;
            case 2:
               // read
               // and 0FFFFh
               // savef target
               tape.write(bcRead);
               tape.write(bcAnd, 0xFFFF);
               tape.write(bcSaveF, target << 2);
               break;
            case 4:
               // read
               // savef target
               tape.write(bcRead);
               tape.write(bcSaveF, target << 2);
               break;
            default:
               // NOTE : operations with the stack should always be aligned for efficiency
               // readtof target itemSize / 4
               tape.write(bcReadToF, target << 2, align(itemSize, 4) >> 2);
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
         tape.write(bcSaveF, target << 2);
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
         tape.write(bcSaveF, target << 2);
         break;
   }
}

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

inline SNode getChild(SNode node, pos_t index)
{
   SNode current = node.firstChild(lxObjectMask);

   while (index > 0 && current != lxNone) {
      current = current.nextNode(lxObjectMask);

      index--;
   }

   return current;
}

////inline bool existNode(SNode node, LexicalType type)
////{
////   return SyntaxTree::findChild(node, type) == type;
////}
//
//inline size_t countChildren(SNode node)
//{
//   size_t counter = 0;
//   SNode current = node.firstChild(lxObjectMask);
//
//   while (current != lxNone) {
//      current = current.nextNode(lxObjectMask);
//
//      counter++;
//   }
//
//   return counter;
//}

void ByteCodeWriter :: translateBreakpoint(CommandTape& tape, SNode node, FlowScope& scope/*, bool ignoreBranching*/)
{
   if (!scope.debugBlockStarted) {
      declareBlock(tape);

      scope.debugBlockStarted = true;
   }

   declareBreakpoint(tape,
      node.findChild(lxRow).argument,
      node.findChild(lxCol).argument - 1,
      node.findChild(lxLength).argument, node.argument);
}

void ByteCodeWriter :: pushObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int)
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
//      case lxExtMessageConstant:
      case lxSubjectConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcPushR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
         // pushfi index
         tape.write(bcPushFI, argument);
         break;
      case lxClassRef:
         // class
         tape.write(bcClass);
         tape.write(bcPushA);
         break;
      case lxLocalAddress:
         // pushf n
         tape.write(bcPushF, argument << 2);
         break;
      case lxBlockLocalAddr:
         // pushf n
         tape.write(bcPushFIP, argument);
         break;
      case lxCurrent:
         // pushsi index
         tape.write(bcPushSI, argument);
         break;
      case lxField:
         // pushai index
         tape.write(bcPushAI, argument);
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
////      case lxBlockLocal:
////         // pushfi index
////         tape.write(bcPushFI, argument, bpBlock);
////         break;
      case lxNil:
         // pushn 0
         tape.write(bcPushR, 0);
         break;
//      case lxStopper:
//         // pushn -1
//         tape.write(bcPushN, -1);
//         break;
      case lxResult:
         // pusha
         tape.write(bcPushA);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: pushIntConstant(CommandTape& tape, int value)
{
   tape.write(bcPushN, value);
}

void ByteCodeWriter :: loadIntValue(CommandTape& tape)
{
   // load

   tape.write(bcLoad);
}

void ByteCodeWriter :: loadObject(CommandTape& tape, LexicalType type, ref_t argument, FlowScope& scope, int)
{
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
//      case lxExtMessageConstant:
      case lxSubjectConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcMovR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
         tape.write(bcPeekFI, argument);
         break;
      case lxCurrent:
         // peeksi index
         tape.write(bcPeekSI, argument);
         break;
      case lxNil:
         // acopyr 0
         tape.write(bcMovR, argument);
         break;
      case lxField:
         // geti index
         tape.write(bcGetI, argument);
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
      case lxLocalAddress:
         // acopyf n
         tape.write(bcMovF, argument << 2);
         break;
      case lxClassRef:
         // class
         tape.write(bcClass);
         break;
      case lxBlockLocalAddr:
         // acopyf n
         tape.write(bcMovFIP, argument);
         break;
      case lxResult:
         break;
      default:
         throw InternalError("Not yet implemented"); // !! temporal
         return;
   }

   scope.acc.type = type;
   scope.acc.arg = argument;
}

void ByteCodeWriter :: saveObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type) {
      case lxLocal:
      case lxSelfLocal:
      case lxTempLocal:
         // storefi index
         tape.write(bcStoreFI, argument);
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
         break;
      default:
         throw InternalError("Not yet implemented"); // !! temporal
         break;
   }
}

void ByteCodeWriter :: saveObject(CommandTape& tape, SNode node)
{
   /*if (node == lxExpression) {
      saveObject(tape, node.findSubNodeMask(lxObjectMask));
   }
   else */saveObject(tape, node.type, node.argument);
}

void ByteCodeWriter :: loadObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   if (test(node.type, lxOpScopeMask)) {
      generateObject(tape, node, scope, mode);
   }
   else loadObject(tape, node.type, node.argument, scope, mode);
}

//void ByteCodeWriter :: pushObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
//{
//   pushObject(tape, node.type, node.argument, scope, mode);
//}

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

void __FASTCALL assignOpArguments(SNode node, SNode& larg, SNode& rarg, SNode& rarg2)
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
         releaseArg(tape);
      }
      else if (size == 0) {
         newDynamicObject(tape, node.argument);
         clearDynamicObject(tape);
         releaseArg(tape);
      }
      else throw InternalError("not yet implemented"); // !! temporal
   }
   else throw InternalError("not yet implemented"); // !! temporal

   scope.clear();
}

void ByteCodeWriter :: generateArrOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int)
{
   bool freeStack = false;
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
         freeStack = true;
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
      //else if (larg.type == lxSeqExpression) {
      //   if (!immIndex) {
      //      generateObject(tape, larg, scope, STACKOP_MODE);
      //      generateObject(tape, rarg, scope);
      //      tape.write(bcLoad);
      //      tape.write(bcPopA);
      //   }
      //   else generateObject(tape, larg, scope);
      //}
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

   if (freeStack)
      releaseArg(tape);
}

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

      if (rargObj.firstChild() == lxBreakpoint && !test(mode, NOBREAKPOINTS)) {
         translateBreakpoint(tape, rargObj.firstChild(), scope);
      }

      doIntOperation(tape, node.argument, largObj.argument, rargVal);
   }
   else if (rargImm && node.type == lxRealIntOp && node.argument == SET_OPERATOR_ID) {
      int rargVal = rargObj.findChild(lxIntValue).argument;

      doRealIntOperation(tape, node.argument, largObj.argument, rargVal);
   }
   else {
      generateObject(tape, rarg, scope);

      if (node.type == lxIntOp) {
         doIntOperation(tape, node.argument, largObj.argument);
      }
      else if (node == lxLongOp) {
         doLongOperation(tape, node.argument, largObj.argument);
      }
      else if (node == lxRealOp) {
         doRealOperation(tape, node.argument, largObj.argument);
      }
      else if (node == lxRealIntOp) {
         doRealIntOperation(tape, node.argument, largObj.argument);
      }
   }

}

void ByteCodeWriter :: generateBoolOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int)
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

   releaseArg(tape);
}

void ByteCodeWriter :: generateBoolLogicOperation(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int mode)
{
   SNode larg;
   SNode rarg;
   assignOpArguments(node, larg, rarg);

   ref_t trueRef = node.findChild(lxIfValue).argument | mskConstantRef;
   ref_t falseRef = node.findChild(lxElseValue).argument | mskConstantRef;

   if (node.argument == INVERTED_OPERATOR_ID) {
      // NOTE : the second argument is dummy for unary operator
      generateObject(tape, larg, scope, BOOL_ARG_EXPR);
      tape.write(bcPushR, trueRef);
      tape.write(bcEqual);
      tape.write(bcSelect, trueRef, falseRef);
      tape.write(bcPop);
   }
   else {
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
            releaseArg(tape);
            break;
      }
   }
   else throw new InternalError("Not yet implemented"); // temporal
}

int ByteCodeWriter :: saveExternalParameters(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool idleMode)
{
   int paramCount = 0;

   SNode current = node.firstChild();
   int i = 0;
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (!idleMode) {
            if (current == lxExtIntConst) {
               tape.write(bcXSaveSI, i, current.firstChild(lxObjectMask).findChild(lxIntValue).argument);
            }
            else if (current == lxExtIntArg) {
               loadObject(tape, current.firstChild(lxObjectMask), scope);
               loadIntValue(tape);
               tape.write(bcSaveSI, i);
            }
            else {
               generateObject(tape, current, scope);
               saveObject(tape, lxCurrent, i);
            }
         }

         paramCount++;
      }

      current = current.nextNode();
      i++;
   }

   return paramCount;
}

void ByteCodeWriter :: generateExternalCall(CommandTape& tape, SNode node, FlowScope& scope)
{
   bool apiCall = (node == lxCoreAPICall);
   bool cleaned = (node == lxStdExternalCall);

   // alloc space for parameters
   int paramCount = saveExternalParameters(tape, node, scope, true);
   tape.write(bcAllocI, paramCount);

   // save function parameters
   paramCount = saveExternalParameters(tape, node, scope, false);

   // call the function
   if (apiCall) {
      // if it is an API call
      // simply release parameters from the stack
      // without setting stack pointer directly - due to optimization
      callCore(tape, node.argument, paramCount, !cleaned);
   }
   else if (node.existChild(lxLongMode)) {
      callLongExternal(tape, node.argument, paramCount, !cleaned);
   }
   else {
      callExternal(tape, node.argument, paramCount, !cleaned);
   }
}

void ByteCodeWriter :: generateCall(CommandTape& tape, SNode callNode)
{
   // copym message
   mssg_t message = callNode.argument;

   tape.write(bcMovM, message);

   SNode target = callNode.findChild(lxCallTarget);
   if (callNode == lxDirectCalling) {
      callResolvedMethod(tape, target.argument, callNode.argument);
   }
   else if (callNode == lxSDirectCalling) {
      callVMTResolvedMethod(tape, target.argument, callNode.argument);
   }
   else {
      int vmtOffset = callNode == lxCalling_1 ? 1 : 0;

      // callvi vmtOffset
      tape.write(bcCallVI, vmtOffset);
   }
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

   releaseStack(tape, paramCount);
}

//void ByteCodeWriter :: generateInlineArgCallExpression(CommandTape& tape, SNode node, FlowScope& scope)
//{
//   SNode larg;
//   SNode rarg;
//   assignOpArguments(node, larg, rarg);
//   if (rarg == lxExpression)
//      rarg = rarg.findSubNodeMask(lxObjectMask);
//
//   generateInlineArgCall(tape, larg, rarg, node.argument, scope);
//
//   scope.clear();
//}
//
//inline void setInlineArgMessage(CommandTape& tape, int message)
//{
//   // count
//   tape.write(bcCount);
//   if (!test(message, FUNCTION_MESSAGE)) {
//      // inc 1
//      tape.write(bcInc, 1);
//   }
//
//   // movv m
//   tape.write(bcMovV, getAction(message));
//   if (test(message, FUNCTION_MESSAGE)) {
//      // or SPECIAL_MESSAGE
//      tape.write(bcOr, FUNCTION_MESSAGE);
//   }
//}
//
//void ByteCodeWriter :: generateInlineArgCall(CommandTape& tape, SNode larg, SNode rarg, int message, FlowScope& scope)
//{
//   if (larg == lxExpression)
//      larg = larg.findSubNodeMask(lxObjectMask);
//
//   tape.newLabel(); // declare end label
//   tape.newLabel(); // declare variadic label
//
//   generateObject(tape, rarg, scope);
//
//   // count
//   tape.write(bcCount);
//
//   // greatern labVariadic ARG_COUNT
//   tape.write(bcGreaterN, baCurrentLabel, ARG_COUNT);
//
//   // labNext:
//   tape.newLabel();
//   tape.setLabel(true);
//   // dec
//   tape.write(bcDec, 1);
//   tape.write(bcPush);
//   // elsen labNext 0
//   tape.write(bcElseN, baCurrentLabel, 0);
//   tape.releaseLabel();
//
//   if (!isSubOperation(larg)) {
//      setInlineArgMessage(tape, message);
//
//      loadObject(tape, larg.type, larg.argument, scope, 0);
//      if (!test(message, FUNCTION_MESSAGE))
//         tape.write(bcPushA);
//   }
//   else {
//      if (!isSubOperation(rarg)) {
//         generateObject(tape, larg, scope, STACKOP_MODE);
//
//         loadObject(tape, rarg.type, rarg.argument, scope, 0);
//         setInlineArgMessage(tape, message);
//
//         if (!test(message, FUNCTION_MESSAGE)) {
//            tape.write(bcPeekSI, 0);
//         }
//         else tape.write(bcPopA);
//      }
//      else {
//         tape.write(bcPushN, 0);
//         tape.write(bcPushA);
//         generateObject(tape, larg, scope);
//         tape.write(bcStoreSI, 1);
//         tape.write(bcPopA);
//
//         setInlineArgMessage(tape, message);
//         if (!test(message, FUNCTION_MESSAGE)) {
//            tape.write(bcPeekSI, 0);
//         }
//         else tape.write(bcPopA);
//      }
//   }
//
//   // callvi 0
//   tape.write(bcCallVI, 0);
//   // jmp labEnd
//   tape.write(bcJump, baPreviousLabel);
//
//   // labVariadic:
//   tape.setLabel();
//
//   // pushn -1
//   tape.write(bcPushN, -1);
//
//   // labNext2:
//   tape.newLabel();
//   tape.setLabel(true);
//
//   // dec 1
//   tape.write(bcDec, 1);
//   // push
//   tape.write(bcPush);
//   // elsen labNext2 0
//   tape.write(bcElseN, baCurrentLabel, 0);
//   tape.releaseLabel();
//
//   // ; push target
//   generateObject(tape, larg, scope);
//   if (!test(message, FUNCTION_MESSAGE))
//      tape.write(bcPushA);
//
//   // movm
//   tape.write(bcMovM, encodeMessage(getAction(message), 1, VARIADIC_MESSAGE));
//
//   if (test(message, FUNCTION_MESSAGE)) {
//      // or SPECIAL_MESSAGE
//      tape.write(bcOr, FUNCTION_MESSAGE);
//   }
//
//   // callvi 0
//   tape.write(bcCallVI, 0);
//
//   releaseArgList(tape);
//
//   // labEnd
//   tape.setLabel();
//}

void ByteCodeWriter :: generateCallExpression(CommandTape& tape, SNode node, FlowScope& scope)
{
   bool functionMode = test(node.argument, FUNCTION_MESSAGE);
   bool argUnboxMode = false;

   pos_t argCount = 0;
   pos_t extraArgs = 0;

   // analizing a sub tree
   SNode current = node.firstChild(lxObjectMask);
   bool directOrder = false;
   while (current != lxNone) {
      if (current == lxResult)
         directOrder = true;

      //SNode argNode = current;
      //if (argNode == lxExpression)
      //   argNode = current.findSubNodeMask(lxObjectMask);

      if (current == lxArgArray) {
         argUnboxMode = true;
      }
      else argCount++;

      current = current.nextNode(lxObjectMask);
   }

   // the function target can be loaded at the end
   int startIndex = 0;
   if (functionMode) {
      startIndex = 1;
   }

   bool openArg = !argUnboxMode && isOpenArg(node.argument);
   if (openArg)
      extraArgs = 1;

   if (argUnboxMode) {
      current = node.findChild(lxArgArray);

      generateExpression(tape, current, scope);
      unboxArgList(tape/*, argNode.argument != 0*/, argCount - startIndex + extraArgs);
   }
   else tape.write(bcAllocI, argCount - startIndex + extraArgs);   

   if (directOrder) {
      for (pos_t i = startIndex; i != argCount; i++) {
         // get parameters in reverse order if required
         current = getChild(node, i);

         generateObject(tape, current, scope);
         saveObject(tape, lxCurrent, i - startIndex);
      }
   }
   else
   {
      for (int i = argCount - 1; i >= startIndex; i--) {
         // get parameters in reverse order if required
         current = getChild(node, i);

         generateObject(tape, current, scope);
         saveObject(tape, lxCurrent, i - startIndex);
      }
   }

   if (!argUnboxMode && isOpenArg(node.argument)) {
      // NOTE : do not add trailing nil for result of unboxing operation
      tape.write(bcXSaveSI, argCount - startIndex, -1);
   }

   if (functionMode) {
      generateObject(tape, getChild(node, 0), scope);
   }
   else tape.write(bcPeekSI, 0);

   generateCall(tape, node/*, paramCount, presavedCount*/);
   
   if (argUnboxMode) {
      releaseArgList(tape);
   }
   else tape.write(bcFreeI, argCount - startIndex + extraArgs);

   scope.clear();
}

void ByteCodeWriter :: generateReturnExpression(CommandTape& tape, SNode node, FlowScope& scope)
{
   generateExpression(tape, node, scope/*, ACC_REQUIRED*/);

   gotoEnd(tape, baFirstLabel);
}

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
   tape.write(bcCopyF, argument << 2, size >> 2);
}

void ByteCodeWriter :: copyToLocalAddress(CommandTape& tape, int size, int argument)
{
   // stack operations are always 4-byte aligned
   size = align(size, 4);

   tape.write(bcCopyToF, argument << 2, size >> 2);
}

//void ByteCodeWriter :: saveToLocal(CommandTape& tape, int size, int argument)
//{
//   if (size == 4) {
//      // savef arg
//      tape.write(bcSaveFI, argument);
//   }
//   else throw InternalError("not yet implemente"); // !! temporal
//}

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
   tape.write(bcSaveF, argument << 2);
}

void ByteCodeWriter :: loadFieldAddress(CommandTape& tape, int size, int argument)
{
   if (size == 4) {
      // xload arg
      tape.write(bcXLoad, argument);
   }
   else if (size == 3) {
      // xload arg
      // and 0FFFFFFh
      tape.write(bcXLoad, argument);
      tape.write(bcAnd, 0x0FFFFFF);
   }
   else if (size == 2) {
      // xload arg
      // and 0FFFFh
      tape.write(bcXLoad, argument);
      tape.write(bcAnd, 0x0FFFF);
   }
   else if (size == 1) {
      // xload arg
      // and 0FFh
      tape.write(bcXLoad, argument);
      tape.write(bcAnd, 0x0FF);
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

void ByteCodeWriter :: copyToLocal(CommandTape& tape, int size, int argument)
{
   // stack operations are always 4-byte aligned
   size = align(size, 4);

   // if it is a dword aligned
   tape.write(bcCopyToFI, argument, size >> 2);
}

inline bool isAligned(int size)
{
   return (size & 3) == 0;
}

void ByteCodeWriter :: saveFieldExpression(CommandTape& tape, SNode dstObj, SNode source, int size, FlowScope& scope)
{
   SNode fieldNode = loadFieldExpression(tape, dstObj, scope, true);
   if (fieldNode.compare(lxFieldAddress, lxField)) {
      generateObject(tape, source, scope, STACKOP_MODE | NOBREAKPOINTS);
      loadFieldExpression(tape, dstObj, scope, false);
      if (fieldNode == lxField) {
         loadObject(tape, fieldNode, scope);

         copyToFieldAddress(tape, size, 0);
      }
      else copyToFieldAddress(tape, size, fieldNode.argument);
      releaseArg(tape);
   }
   else if (fieldNode.compare(lxSelfLocal, lxTempLocal)) {
      if (isAligned(size)) {
         loadObject(tape, source, scope, NOBREAKPOINTS);
         copyToLocal(tape, size, fieldNode.argument);
      }
      else {
         generateObject(tape, source, scope, STACKOP_MODE | NOBREAKPOINTS);
         generateObject(tape, dstObj, scope, 0);
         copyToFieldAddress(tape, size, 0);
         releaseArg(tape);
      }
   }
   else throw InternalError("not yet implemente"); // !! temporal
}

//void ByteCodeWriter :: saveIndexToFieldExpression(CommandTape& tape, SNode dstObj, SNode srcObj, FlowScope& scope)
//{
//   SNode fieldNode = loadFieldExpression(tape, dstObj, scope, true);
//   if (fieldNode == lxFieldAddress) {
//      loadFieldExpression(tape, dstObj, scope, false);
//
//      tape.write(bcPushA);
//      loadObject(tape, srcObj, scope);  // NOTE : it should load the index
//      tape.write(bcPopA);
//
//      if ((fieldNode.argument & 3) == 0) {
//         tape.write(bcSaveI, fieldNode.argument >> 2);
//      }
//      else tape.write(bcXSave, fieldNode.argument);
//   }
//   else if (fieldNode == lxSelfLocal) {
//      loadFieldExpression(tape, dstObj, scope, false);
//
//      loadObject(tape, srcObj, scope);  // NOTE : it should load the index
//      loadObject(tape, fieldNode, scope);
//      tape.write(bcSave);
//   }
//   else throw InternalError("not yet implemente"); // !! temporal
//}
//
//void ByteCodeWriter :: saveIndexToObject(CommandTape& tape, SNode dstObj, SNode srcObj, FlowScope& scope, int size)
//{
//   generateObject(tape, dstObj, scope, STACKOP_MODE);
//   generateObject(tape, srcObj, scope);
//
//   tape.write(bcPopA);
//   if (size == 4) {
//      tape.write(bcSave);
//   }
//   else throw InternalError("not yet implemente"); // !! temporal
//}

void ByteCodeWriter :: copyExpression(CommandTape& tape, SNode source, SNode dstObj, int size, FlowScope& scope/*, bool condCopying*/)
{
   if (dstObj.compare(lxLocal, lxTempLocal, lxSelfLocal)) {
      loadObject(tape, source, scope);
      // no need for this optimization for dword?
      //if (condCopying && size > 4) {
      //   tape.newLabel();
      //   // equalfi i
      //   // ifn labNext 1
      //   tape.write(bcEqualFI, dstObj.argument);
      //   tape.write(bcIfN, baCurrentLabel, 1);
      //   copyToLocal(tape, size, dstObj.argument);
      //   tape.setLabel();
      //}
      /*else */copyToLocal(tape, size, dstObj.argument);
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

   SNode srcObj = /*source == lxExpression ? source.findSubNodeMask(lxObjectMask) : */source;
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
               loadFieldAddress(tape, 4, fieldNode.argument);
            }
            else {
               loadObject(tape, fieldNode, scope);
               tape.write(bcLoad);
            }
            saveToLocalAddress(tape, node.argument, dstObj.argument);
         }
         else {
            generateObject(tape, target, scope, STACKOP_MODE | NOBREAKPOINTS);
            loadFieldExpression(tape, srcObj, scope, false);
            if (fieldNode == lxFieldAddress) {
               copyFieldAddress(tape, node.argument, fieldNode.argument, scope);
            }
            else {
               loadObject(tape, fieldNode, scope);
               copyFieldAddress(tape, node.argument, 0, scope);
            }

            releaseArg(tape);
         }
      }
      else copyExpression(tape, source, dstObj, node.argument, scope/*, node == lxCondCopying*/);
   }
   else copyExpression(tape, source, dstObj, node.argument, scope/*, node == lxCondCopying*/);
}

//void ByteCodeWriter :: generateIndexLoadingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope&, int)
//{
//   SNode source = node.findSubNodeMask(lxObjectMask);
//   if (source == lxLocalAddress) {
//      tape.write(bcLoadF, source.argument << 2);
//   }
//   else throw InternalError("Not yet implemented"); // !! temporal
//}
//
//void ByteCodeWriter :: generateIndexSavingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope&, int)
//{
//   SNode target = node.findSubNodeMask(lxObjectMask);
//   if (target == lxLocalAddress) {
//      tape.write(bcSaveF, target.argument << 2);
//   }
//   else throw InternalError("Not yet implemented"); // !! temporal
//}

void ByteCodeWriter :: generateSavingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   SNode dstObj = target == lxExpression ? target.findSubNodeMask(lxObjectMask) : target;

   //if (srcObj == lxLocalAddress) {
   //   loadObject(tape, target, scope);
   //   copyFromLocalAddress(tape, node.argument, srcObj.argument);
   //}
   /*else *//*if (dstObj.compare(lxLocal, lxTempLocal, lxSelfLocal)) {
      // !! never used???
      loadObject(tape, source, scope);
      saveToLocal(tape, 4, dstObj.argument);
   }
   else */if (dstObj == lxLocalAddress) {
      // NOTE : it should be the external operation
      loadObject(tape, source, scope);

      if (node.argument == 8) {
         // HOTFIX : to support external op returning long
         tape.write(bcMovF, dstObj.argument << 2);
         tape.write(node == lxFloatSaving ? bcRSave : bcLSave);
         scope.clear();
      }
      else saveToLocalAddress(tape, node.argument, dstObj.argument);
   }
   //else if (dstObj == lxFieldExpression) {
   //   saveIndexToFieldExpression(tape, dstObj, srcObj, scope);
   //}
   //else if (dstObj == lxCreatingStruct) {
   //   saveIndexToObject(tape, dstObj, srcObj, scope, node.argument);
   //}
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
   releaseArg(tape);
}

void ByteCodeWriter :: generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, int)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   if (isSubOperation(target)) {
      if (target == lxFieldExpression) {
         generateObject(tape, source, scope, STACKOP_MODE);

         //if (node == lxCondAssigning) {
         //   tape.newLabel();
         //   // equalfi node.argument
         //   // ifn labNext 1

         //   tape.write(bcEqualFI, node.argument);
         //   tape.write(bcIfN, baCurrentLabel, 1);

         //   SNode fieldNode = loadFieldExpression(tape, target, scope, false);
         //   saveObject(tape, fieldNode);

         //   tape.setLabel();
         //}
         //else {
            SNode fieldNode = loadFieldExpression(tape, target, scope, false);
            saveObject(tape, fieldNode);
         //}

         releaseArg(tape);
      }
      else throw InternalError("not yet implemente"); // !! temporal
   }
   else {
      loadObject(tape, source, scope);
//
//      if (node == lxCondAssigning) {
//         tape.newLabel();
//         // equalfi node.argument
//         // ifn labNext 1
//
//         tape.write(bcEqualFI, node.argument);
//         tape.write(bcIfN, baCurrentLabel, 1);
//
//         saveObject(tape, target);
//
//         tape.setLabel();
//      }
      /*else */saveObject(tape, target);
   }
}

void ByteCodeWriter :: generateExternFrame(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   excludeFrame(tape);

   generateCodeBlock(tape, node.findChild(lxCode), scope, false);

   includeFrame(tape, true);
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
            generateCodeBlock(tape, current, scope, false);
         }
         else generateObject(tape, current, scope);

         endTry(tape);
         if (finallyNode != lxNone) {
            scope.clear();

            // generate finally
            tape.write(bcAllocI, 1);
            tape.write(bcStoreSI, 0);
            generateCodeBlock(tape, finallyNode, scope, false);
            tape.write(bcPeekSI, 0);
            tape.write(bcFreeI, 1);
         }
         declareSafeCatch(tape, finallyNode, retLabel, scope);
         doCatch(tape);
         if (finallyNode != lxNone) {
            scope.clear();

            // generate finally
            tape.write(bcAllocI, 1);
            tape.write(bcStoreSI, 0);
            generateCodeBlock(tape, finallyNode, scope, false);
            tape.write(bcPeekSI, 0);
            tape.write(bcFreeI, 1);
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

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxIfN) {
         jumpIfNotEqual(tape, current.argument, false);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxIfNotN) {
         jumpIfEqual(tape, current.argument, false);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxLessN) {
         jumpIfNotLess(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxNotLessN) {
         jumpIfLess(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxGreaterN) {
         jumpIfNotGreater(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

         repeatMode = false;
      }
      else if (current == lxNotGreaterN) {
         jumpIfGreater(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode), scope, false);

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

      if (current == lxOption) {
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
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxIfNot:
         case lxIfNotN:
            jumpIfEqual(tape, current.argument, current == lxIfNot);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxLessN:
            jumpIfNotLess(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxNotLessN:
            jumpIfLess(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxGreaterN:
            jumpIfNotGreater(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxNotGreaterN:
            jumpIfGreater(tape, current.argument);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         case lxElse:
            declareElseBlock(tape);

            //declareBlock(tape);
            generateCodeBlock(tape, current.findChild(lxCode), scope, true);
            break;
         default:
            generateObject(tape, current, scope);
            if (scope.debugBlockStarted) {
               declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
               scope.debugBlockStarted = false;
            }
            break;
      }

      current = current.nextNode(lxObjectMask);
   }

   if(!switchBranching)
      endThenBlock(tape);

   scope.clear();
}

//inline SNode goToNode(SNode current, LexicalType type)
//{
//   while (current != lxNone && current != type)
//      current = current.nextNode();
//
//   return current;
//}

void ByteCodeWriter :: generateCloningExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode target;
   SNode source;
   assignOpArguments(node, target, source);

   if (target == lxExpression)
      target = target.findSubNodeMask(lxObjectMask);

   if (source == lxNone) {
      generateObject(tape, target, scope, STACKOP_MODE);

      tape.write(bcXCreate, node.findChild(lxType).argument | mskVMTRef);

      tape.write(bcClone);
      releaseArg(tape);
   }
   else {
      //if (source == lxExpression)
      //   source = source.findSubNodeMask(lxObjectMask);

      if (source == lxBlockLocalAddr) {
         //   if (source.firstChild() == lxBreakpoint) {
         //      translateBreakpoint(tape, source.firstChild(), scope);
         //   }

         generateObject(tape, target, scope);

         tape.write(bcPushFIP, source.argument);
         tape.write(bcClone);
         tape.write(bcPop);
      }
      else if (source == lxLocalAddress) {
         //   if (source.firstChild() == lxBreakpoint) {
         //      translateBreakpoint(tape, source.firstChild(), scope);
         //   }

         generateObject(tape, target, scope);

         tape.write(bcCloneF, source.argument << 2);
      }
      else if (source.compare(lxLocal, lxTempLocal)) {
         if (source.firstChild() == lxBreakpoint) {
            translateBreakpoint(tape, source.firstChild(), scope);
         }

         generateObject(tape, source, scope, STACKOP_MODE);
         generateObject(tape, target, scope);

         tape.write(bcClone);
         releaseArg(tape);
      }
      else throw InternalError("Not yet implemented"); // !! temporal
   }
}

void ByteCodeWriter :: generateInitializingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode objNode = node.findSubNodeMask(lxObjectMask);

   SNode current = node.findChild(lxMember);
   int size = objNode.findChild(lxSize).argument;
   bool createMode = objNode == lxCreatingClass;
   bool structMode = objNode == lxCreatingStruct;
   bool onlyLocals = createMode;
   size_t counter = 0;
   while (current != lxNone) {
      if (current == lxMember) {
         counter++;

         SNode memberNode = current.findSubNodeMask(lxObjectMask);
         if (!memberNode.compare(lxLocal, lxSelfLocal)) {
            onlyLocals = false;
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
            else {
               tape.write(createMode ? bcXSetI : bcSetI, current.argument);

               createMode = true; // HOTFIX : we have to mark the source object only once
            }
            releaseArg(tape);
         }
         current = current.nextNode();
      }
   }
   else {
      if (createMode && counter == objNode.argument) {
         // HOTFIX : if the closure is initialized in-place - no need to initialize its fields with zeros
         generateCreating(tape, objNode, scope, false);
      }
      else generateObject(tape, objNode, scope);

      current = node.findChild(lxMember);
      while (current != lxNone) {
         if (current == lxMember) {
            SNode memberNode = current.findSubNodeMask(lxObjectMask);

            tape.write(bcXSetFI, memberNode.argument, current.argument);
         }
         current = current.nextNode();
      }
   }
}

void ByteCodeWriter :: generateResendingExpression(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   bool genericResending = node == lxGenericResending;

   SNode target = node.findChild(lxCallTarget);
   if (!genericResending && node.argument == 0) {
      SNode message = node.findChild(lxMessage);
      if (isOpenArg(message.argument)) {
         // if it is open argument dispatching
         openFrame(tape, 1);
         tape.write(bcAllocI, 1);

         tape.write(bcSaveF, -4);
         tape.write(bcStoreFI, 1);

         unboxMessage(tape);
         changeMessageCounter(tape, -4, 2, VARIADIC_MESSAGE);
         tape.write(bcPeekFI, 1);

         callResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument/*, false, false*/);

         closeFrame(tape, 1);
         tape.write(bcQuit);
      }
      else {
         tape.write(bcPushD);
         setSubject(tape, message.argument);

         resendDirectResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument, true);
      }
   }
   else {
      SNode current = node.firstChild();
      while (current != lxNone) {
         if (current == lxNewFrame) {
            int allocated = current.findChild(lxAllocated).argument;
            int reserved = current.findChild(lxReserved).argument;

            // new frame
            newFrame(tape, reserved, allocated, false, true);

            if (genericResending) {
               // save message
               tape.write(bcSaveF, -4);

               generateExpression(tape, current, scope);

               // restore message
               tape.write(bcLoadF, -4);
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

      if (target.argument != 0 && (node.type == lxDirectResending || node.type == lxSDirectResending)) {
         resendDirectResolvedMethod(tape, target.argument, node.argument, node.type == lxDirectResending);
      }
      else resend(tape);
   }
}

void ByteCodeWriter :: generateCondBoxing(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode step1;
   SNode step2;
   assignOpArguments(node, step1, step2);

   SNode tempLocal = step1.findSubNodeMask(lxObjectMask);
   if (step2 == lxCopying) {
      SNode local = step2.firstChild(lxObjectMask).nextNode(lxObjectMask);

      tape.newLabel();
      generateObject(tape, local, scope);
      tape.write(bcIfHeap, baCurrentLabel);

      generateObject(tape, step1, scope);
      generateObject(tape, step2, scope);
   }
   else if (step2 == lxNone) {
      SNode cloneNode = step1.findChild(lxCloning);
      if (cloneNode != lxNone) {
         SNode local = cloneNode.firstChild(lxObjectMask);

         tape.newLabel();
         generateObject(tape, local, scope);
         tape.write(bcIfHeap, baCurrentLabel);

         generateObject(tape, step1, scope);
      }
      else throw InternalError("Not yet implemented"); // !! temporal
   }
   else throw InternalError("Not yet implemented"); // !! temporal

   scope.clear();
   generateObject(tape, tempLocal, scope);
   tape.setLabel();

   if (tempLocal.compare(lxLocal, lxTempLocal)) {
      saveObject(tape, tempLocal.type, tempLocal.argument);
   }
   else throw InternalError("Not yet implemented"); // !! temporal
}

void ByteCodeWriter :: generateCondUnBoxing(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   SNode current = node.firstChild(lxObjectMask);

   SNode step1;
   SNode step2;
   assignOpArguments(node, step1, step2);

   SNode local = step1.firstChild(lxObjectMask);
   tape.newLabel();
   generateObject(tape, local, scope);
   //scope.clear();

   tape.write(bcIfHeap, baCurrentLabel);

   generateObject(tape, current, scope);
   generateObject(tape, local, scope);

   tape.setLabel();
}

void ByteCodeWriter :: generateObject(CommandTape& tape, SNode node, FlowScope& scope, int mode)
{
   if (node.firstChild() == lxBreakpoint && !test(mode, NOBREAKPOINTS)) {
      translateBreakpoint(tape, node.firstChild(), scope);

      mode |= NOBREAKPOINTS;
   }

   bool stackOp = test(mode, STACKOP_MODE);

   switch (node.type)
   {
      case lxExpression:
      case lxFieldExpression:
      case lxSeqExpression:
         generateExpression(tape, node, scope, (mode & ~STACKOP_MODE) | NOBREAKPOINTS);
         break;
//      case lxNestedSeqExpression:
//         tape.write(bcPushA);
//         generateExpression(tape, node, scope, mode & ~STACKOP_MODE);
//         tape.write(bcPopA);
//         break;
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
//      case lxCondAssigning:
         generateAssigningExpression(tape, node, scope);
         break;
      case lxCopying:
//      case lxCondCopying:
         generateCopyingExpression(tape, node, scope, mode);
         break;
      case lxSaving:
      case lxFloatSaving:
         generateSavingExpression(tape, node, scope);
         break;
//      case lxIndexSaving:
//         generateIndexSavingExpression(tape, node, scope);
//         break;
//      case lxIndexLoading:
//         generateIndexLoadingExpression(tape, node, scope);
//         break;
//      case lxInlineArgCall:
//         generateInlineArgCallExpression(tape, node, scope);
//         break;
      case lxImplicitJump:
         resendDirectResolvedMethod(tape, node.findChild(lxCallTarget).argument, node.argument, true);
         break;
      case lxTrying:
         generateTrying(tape, node, scope);
         break;
      case lxAlt:
         generateAlt(tape, node, scope);
         break;
      case lxCoreAPICall:
      case lxStdExternalCall:
      case lxExternalCall:
         generateExternalCall(tape, node, scope);
         break;
      case lxInternalCall:
         generateInternalCall(tape, node, scope);
         break;
      case lxBranching:
         generateBranching(tape, node, scope);
         break;
      case lxSwitching:
         generateSwitching(tape, node, scope);
         break;
      case lxLooping:
         generateLooping(tape, node, scope);
         break;
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
      case lxRealIntOp:
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
      case lxDirectResending:
      case lxSDirectResending:
      case lxGenericResending:
         generateResendingExpression(tape, node, scope);
         break;
////      case lxIf:
////         jumpIfNotEqual(tape, node.argument, true);
////         generateCodeBlock(tape, node);
////         break;
      case lxElse:
         if (node.argument != 0)
            jumpIfEqual(tape, node.argument, true);

         generateCodeBlock(tape, node, scope, false);
         break;
      case lxCreatingClass:
      case lxCreatingStruct:
         generateCreating(tape, node, scope, true);
         break;
//      case lxYieldReturning:
//         generateYieldReturn(tape, node, scope);
//         break;
//      case lxBoxableExpression:
//      case lxArgBoxableExpression:
//      case lxCondBoxableExpression:
//      case lxPrimArrBoxableExpression:
//         throw InternalError("Unboxed expression");
//         break;
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
//      case lxExtIntArg:
//         if (stackOp) {
//            loadObject(tape, node.firstChild(lxObjectMask), scope);
//            pushIntValue(tape);
//            stackOp = false;
//         }
//         else throw InternalError("Not yet implemented"); // !! temporal
//         break;
      case lxCode:
         generateCodeBlock(tape, node, scope, false);
         break;
      case lxCondBoxing:
         generateCondBoxing(tape, node, scope);
         break;
      case lxCondUnboxing:
         generateCondUnBoxing(tape, node, scope);
         break;
      case lxTempBinaryArray:
         generateBinary(tape, node, node.findChild(lxLevel).argument);
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
   if (node.firstChild() == lxBreakpoint && !test(mode, NOBREAKPOINTS)) {
      translateBreakpoint(tape, node.firstChild(), scope);
   }

   SNode current = node.firstChild(lxObjectMask);
   while (current != lxNone) {
      if (current == lxExternFrame) {
         generateExternFrame(tape, current, scope);
      }
      else generateObject(tape, current, scope, mode);

      current = current.nextNode(lxObjectMask);
   }
}

void ByteCodeWriter :: generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset)
{
   saveLength(tape, lxLocalAddress, offset + 1, node.argument);
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
            current.findChild(lxIdentifier).identifier(), level);
         break;
      }
      case lxShortsVariable:
      {
         int level = current.findChild(lxLevel).argument;

         generateBinary(tape, current, level);
         declareLocalShortArrayInfo(tape,
            current.findChild(lxIdentifier).identifier(), level);
         break;
      }
      case lxIntsVariable:
      {
         int level = current.findChild(lxLevel).argument;

         generateBinary(tape, current, level);

         declareLocalIntArrayInfo(tape,
            current.findChild(lxIdentifier).identifier(), level);
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
   }
}

void ByteCodeWriter :: generateCodeBlock(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool ignoreEOP)
{
   SNode current = node.firstChild(/*lxStatementMask*/);
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
         case lxBinarySelf:
            declareSelfStructInfo(tape, SELF_VAR, current.argument,
               current.findChild(lxClassName).identifier());
            break;
         case lxVariable:
         case lxIntVariable:
         case lxLongVariable:
         case lxReal64Variable:
         case lxParamsVariable:
         case lxBytesVariable:
         case lxShortsVariable:
         case lxIntsVariable:
         case lxBinaryVariable:
            generateDebugInfo(tape, current);
            break;
//         case lxYieldDispatch:
//            generateYieldDispatch(tape, current, scope);
//            break;
         case lxEOP:
            if (!ignoreEOP) {
               scope.debugBlockStarted = true;
               if (current.firstChild() == lxBreakpoint)
                  translateBreakpoint(tape, current.findChild(lxBreakpoint), scope);
               scope.debugBlockStarted = false;
            }
            break;
//         case lxCode:
//         case lxCodeExpression:
//            // HOTFIX : nested code, due to resend implementation
//            generateCodeBlock(tape, current, scope);
//            break;
//         default:
//            generateObject(tape, current, scope);
//            break;
      }
      current = current.nextNode(/*lxStatementMask*/);
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

void ByteCodeWriter :: doMultiDispatch(CommandTape& tape, ref_t operationList, mssg_t message)
{
   tape.write(bcMTRedirect, operationList | mskConstArray, message);
}

void ByteCodeWriter :: doSealedMultiDispatch(CommandTape& tape, ref_t operationList, mssg_t message)
{
   tape.write(bcXMTRedirect, operationList | mskConstArray, message);
}

void ByteCodeWriter :: generateMultiDispatching(CommandTape& tape, SyntaxTree::Node node, mssg_t message)
{
   if (node.type == lxSealedMultiDispatching) {
      doSealedMultiDispatch(tape, node.argument, message);
   }
   else doMultiDispatch(tape, node.argument, message);
}

void ByteCodeWriter :: generateDispatching(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
{
   doGenericHandler(tape);
   //if (node.argument != 0) {
   //   // if it is a generic dispatcher with the custom target
   //   tape.write(bcPushD);
   //   setSubject(tape, node.argument);
   //   doGenericHandler(tape);
   //   tape.write(bcPopD);
   //}

   generateExpression(tape, node, scope);
}

void ByteCodeWriter :: generateCreating(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope, bool fillMode)
{
   SNode target = node.findChild(lxType);

   int size = node.argument;
   if (node == lxCreatingClass) {
      newObject(tape, size, target.argument);
      if (fillMode)
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

//void ByteCodeWriter :: generateYieldDispatch(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
//{
//   generateExpression(tape, node, scope);
//
//   // ; jump if state is set
//   // load
//   // ifn labStart 0
//   // nop
//   // ajumpi 0
//   // labStart:
//
//   tape.newLabel();
//
//   tape.write(bcLoad);
//   tape.write(bcIfN, baCurrentLabel, 0);
//   tape.write(bcNop);
//   tape.write(bcJumpI, 0);
//   tape.setLabel();
//}
//
////void ByteCodeWriter :: generateYieldStop(CommandTape& tape, SyntaxTree::Node node)
////{
////   // ; clear state
////   // aloadfi 1
////   // bloadai index
////   // dcopy 0
////   // nsave
////
////   tape.write(bcALoadFI, 1, bpFrame);
////   tape.write(bcBLoadAI, node.argument);
////   tape.write(bcDCopy, 0);
////   tape.write(bcNSave);
////}
//
//void ByteCodeWriter :: generateYieldReturn(CommandTape& tape, SyntaxTree::Node node, FlowScope& scope)
//{
//   // address labNext
//   // peekfi 1
//   // geti index
//   // save
//
//   // <expr>
//
//   // jump labReturn
//   // labNext:
//
//   tape.newLabel();
//   tape.write(bcAddress, baCurrentLabel);
//   tape.write(bcPeekFI, 1);
//   tape.write(bcGetI, node.argument);
//   tape.write(bcSave);
//
//   scope.clear();
//
//   generateExpression(tape, node, scope);
//
//   gotoEnd(tape, baFirstLabel);
//
//   tape.setLabel();
//}

void ByteCodeWriter :: generateMethod(CommandTape& tape, SyntaxTree::Node node, ref_t sourcePathRef/*, bool extStackEvenMode*/)
{
   FlowScope scope;
   bool open = false;
   bool withNewFrame = false;
   bool exit = false;
   bool exitLabel = true;
//   scope.stackEvenMode = extStackEvenMode;

//   int argCount = node.findChild(lxArgCount).argument;
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
               newFrame(tape, reserved, allocated, current.argument == -1, true);
               if (!exitLabel) {
                  tape.newLabel();     // declare exit point
               }
            }
            exitLabel = true;
            generateMethodDebugInfo(tape, node);   // HOTFIX : debug info should be declared inside the frame body
            generateCodeBlock(tape, current, scope, false);
            break;
         case lxNoBody:
            declareAbstractMethod(tape, node.argument);
            open = true;
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
      exitMethod(tape, reserved, false);

      endIdleMethod(tape);
   }
   else if (!exit) {
      if (!exitLabel)
         tape.newLabel();     // declare exit point

      endMethod(tape, reserved, withNewFrame);
   }
   else endIdleMethod(tape);
}

void ByteCodeWriter :: generateClass(_ModuleScope& scope, CommandTape& tape, SNode root, ref_t reference, pos_t sourcePathRef,
   bool(*cond)(LexicalType)/*, bool stackEvenMode*/)
{
#ifdef FULL_OUTOUT_INFO
   // info
   ident_t name = scope.module->resolveReference(reference);
   scope.printInfo("class %s", name);
#endif // FULL_OUTOUT_INFO

   declareClass(tape, reference);
   SyntaxTree::Node current = root.firstChild();
   while (current != lxNone) {
      if (cond(current.type)) {
#ifdef FULL_OUTOUT_INFO
         // info
         int x = 0;
         scope.printMessageInfo("method %s", current.argument);
#endif // FULL_OUTOUT_INFO

         generateMethod(tape, current, sourcePathRef/*, stackEvenMode*/);
      }
      current = current.nextNode();
   }

   endClass(tape);

   //tape.clearArguments();
}

void ByteCodeWriter :: generateInitializer(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument)
{
   FlowScope scope;

   declareInitializer(tape, reference);
   loadObject(tape, type, argument, scope, 0);
   endInitializer(tape);
}

void ByteCodeWriter :: generateInitializer(CommandTape& tape, ref_t reference, SNode root)
{
   FlowScope scope;

   declareInitializer(tape, reference);
   generateCodeBlock(tape, root, scope, false);
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

   SNode exprNode = root.firstChild();
   if (exprNode == lxNewFrame) {
      int reserved = root.findChild(lxReserved).argument;
      int allocated = root.findChild(lxAllocated).argument;

      newFrame(tape, reserved, allocated, false, false);
      generateExpression(tape, exprNode, scope);
      closeFrame(tape, reserved);
   }
   else generateExpression(tape, root, scope);

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
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantList:
      case lxConstantReal:
      case lxConstantString:
      case lxConstantWideStr:
      case lxConstantSymbol:
      case lxClassSymbol:
      case lxMessageConstant:
      //case lxExtMessageConstant:
      case lxSubjectConstant:
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
      if (current == lxMember) {
         SNode object = current.firstChild(lxObjectMask);

         generateConstantMember(writer, object.type, object.argument);
      }

      current = current.nextNode();
   }

   // add vmt reference
   if (target != lxNone)
      writer.Memory()->addReference(target.argument | mskVMTRef, (pos_t)-4);
}
