//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

using namespace _ELENA_;

typedef SyntaxTree::Node SNode;

// check if the node contains only the simple nodes
bool isSimpleObjectExpression(SNode node, bool ignoreFields = false)
{
   if (node == lxNone)
      return true;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression) {
         if (!isSimpleObjectExpression(current, ignoreFields))
            return false;
      }
      else if (ignoreFields && (current.type == lxField || current.type == lxFieldAddress)) {
         // ignore fields if required
      }
      else if (!test(current.type, lxSimpleMask))
         return false;

      current = current.nextNode();
   }

   return true;
}

// --- Auxiliary declareVariable ---

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

ref_t ByteCodeWriter :: writeSourcePath(_Module* debugModule, ident_t path)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      ref_t sourceRef = debugStringWriter.Position();

      debugStringWriter.writeLiteral(path);

      return sourceRef;
   }
   else return 0;
}

ref_t ByteCodeWriter :: writeMessage(_Module* debugModule, _Module* module, MessageMap& verbs, ref_t message)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      ref_t sourceRef = debugStringWriter.Position();

      ref_t subjectRef, verb;
      int paramCount;
      decodeMessage(message, subjectRef, verb, paramCount);

      IdentifierString name(retrieveKey(verbs.start(), verb, DEFAULT_STR));
      if (subjectRef != 0) {
         name.append('&');
         name.append(module->resolveSubject(subjectRef));
      }
      name.append('[');
      name.appendInt(paramCount);
      name.append(']');

      debugStringWriter.writeLiteral(name);

      return sourceRef;
   }
   else return 0;
}

void ByteCodeWriter :: declareSymbol(CommandTape& tape, ref_t reference/*, CodeType scope*/)
{
   // symbol-begin:
   tape.write(blBegin, bsSymbol, reference);
}

void ByteCodeWriter :: declareStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // symbol-begin:

   // aloadr static
   // elser procedure-end
   // acopyr ref
   // pusha

   tape.newLabel();     // declare symbol-end label

   tape.write(blBegin, bsSymbol, staticReference);
   tape.write(bcALoadR, staticReference | mskStatSymbolRef);
   tape.write(bcElseR, baCurrentLabel, 0);
   tape.write(bcACopyR, staticReference | mskLockVariable);
   tape.write(bcPushA);

   tryLock(tape);
   declareTry(tape);

   // check if the symbol was not created while in the lock
   // aloadr static
   tape.write(bcALoadR, staticReference | mskStatSymbolRef);
   jumpIfNotEqual(tape, 0, true);
}

void ByteCodeWriter :: declareClass(CommandTape& tape, ref_t reference)
{
   // class-begin:
	tape.write(blBegin, bsClass, reference);
}

void ByteCodeWriter :: declareIdleMethod(CommandTape& tape, ref_t message)
{
   // method-begin:
   tape.write(blBegin, bsMethod, message);
}

void ByteCodeWriter :: declareMethod(CommandTape& tape, ref_t message, bool withPresavedMessage, bool withNewFrame)
{
   // method-begin:
   //   { pope }?
   //   open
   //   pusha
   tape.write(blBegin, bsMethod, message);

   if (withPresavedMessage)
      tape.write(bcPopE);

   if (withNewFrame) {
      tape.write(bcOpen, 1);
      tape.write(bcPushA);
   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
{
   // exclude
   tape.write(blDeclare, bsBranch);
}

void ByteCodeWriter :: excludeFrame(CommandTape& tape)
{
   tape.write(bcExclude);
   tape.write(bcAllocStack, 1);
}

void ByteCodeWriter :: declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className)
{
   if (!emptystr(localName)) {
      tape.write(bdStruct, (ref_t)localName, level);
      tape.write(bdLocalInfo, (ref_t)className);
   }      
}

void ByteCodeWriter :: declareLocalInfo(CommandTape& tape, ident_t localName, int level)
{
   if (!emptystr(localName))
      tape.write(bdLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalIntInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdIntLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalLongInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdLongLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalRealInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdRealLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalByteArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdByteArrayLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalShortArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdShortArrayLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalIntArrayInfo(CommandTape& tape, ident_t localName, int level, bool includeFrame)
{
   tape.write(bdIntArrayLocal, (ref_t)localName, level, includeFrame ? bpFrame : bpNone);
}

void ByteCodeWriter :: declareLocalParamsInfo(CommandTape& tape, ident_t localName, int level)
{
   tape.write(bdParamsLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareSelfInfo(CommandTape& tape, int level)
{
   tape.write(bdSelf, 0, level);
}

void ByteCodeWriter :: declareMessageInfo(CommandTape& tape, ref_t nameRef)
{
   tape.write(bdMessage, 0, nameRef);
}

void ByteCodeWriter :: declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType)
{
   tape.write(bcBreakpoint);

   tape.write(bdBreakpoint, stepType, row);
   tape.write(bdBreakcoord, disp, length);
}

void ByteCodeWriter :: declareBlock(CommandTape& tape)
{
   tape.write(blBlock);
}

void ByteCodeWriter :: declareArgumentList(CommandTape& tape, int count)
{
   // { pushn 0 } n
   for(int i = 0 ; i < count ; i++)
      tape.write(bcPushN, 0);
}

void ByteCodeWriter :: declareVariable(CommandTape& tape, int value)
{
   // pushn  value
   tape.write(bcPushN, value);
}

int ByteCodeWriter :: declareLoop(CommandTape& tape/*, bool threadFriendly*/)
{
   // loop-begin

   tape.newLabel();                 // declare loop start label
   tape.setLabel(true);

   int end = tape.newLabel();       // declare loop end label

//   // snop
//   if (threadFriendly)
//      tape.write(bcSNop);

   return end;
}

void ByteCodeWriter :: declareThenBlock(CommandTape& tape, bool withStackControl)
{
   if (withStackControl)
      tape.write(blDeclare, bsBranch);  // mark branch-level

   tape.newLabel();                  // declare then-end label
}

void ByteCodeWriter :: declareThenElseBlock(CommandTape& tape)
{
   tape.write(blDeclare, bsBranch);  // mark branch-level
   tape.newLabel();                  // declare end label
   tape.newLabel();                  // declare else label
}

void ByteCodeWriter :: declareElseBlock(CommandTape& tape)
{
   //   jump end
   // labElse
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();

   tape.write(bcResetStack);
}

void ByteCodeWriter :: declareSwitchBlock(CommandTape& tape)
{
   tape.write(blDeclare, bsBranch);  // mark branch-level
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
   tape.write(bcSCopyF, bsBranch);
}

void ByteCodeWriter :: declareTry(CommandTape& tape)
{
   tape.newLabel();                  // declare end-label
   tape.newLabel();                  // declare alternative-label

   // hook labAlt

   tape.write(bcHook, baCurrentLabel);
}

void ByteCodeWriter :: declareCatch(CommandTape& tape)
{
   //   unhook
   //   jump labEnd
   // labErr:
   //   popa
   //   flag
   //   andn elMessage
   //   ifn labSkip
   //   nload
   //   ecopyd
   //   aloadsi 0
   //   acallvi 0
   // labSkip:
   //   unhook

   tape.write(bcUnhook);
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();

   tape.newLabel();

   // HOT FIX: to compensate the unpaired pop
   tape.write(bcAllocStack, 1);
   tape.write(bcPopA);
   tape.write(bcFlag);
   tape.write(bcAndN, elMessage);
   tape.write(bcIfN, baCurrentLabel, 0);
   tape.write(bcNLoad);
   tape.write(bcECopyD);
   tape.write(bcALoadSI, 0);
   tape.write(bcACallVI, 0);

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

void ByteCodeWriter :: newFrame(CommandTape& tape)
{
   //   open 1
   //   pusha
   tape.write(bcOpen, 1);
   tape.write(bcPushA);
}

void ByteCodeWriter :: closeFrame(CommandTape& tape)
{
   // close
   tape.write(bcClose);
}

void ByteCodeWriter :: newDynamicStructure(CommandTape& tape, int itemSize)
{
   if (itemSize != 1) {
      // muln itemSize
      tape.write(bcMulN, itemSize);
   }
   // bcreate
   tape.write(bcBCreate);
}

void ByteCodeWriter :: newDynamicWStructure(CommandTape& tape)
{
   // wcreate
   tape.write(bcWCreate);
}

void ByteCodeWriter :: newDynamicNStructure(CommandTape& tape)
{
   // ncreate
   tape.write(bcNCreate);
}

void ByteCodeWriter :: newStructure(CommandTape& tape, int size, ref_t reference)
{
   // newn size, vmt

   tape.write(bcNewN, reference | mskVMTRef, size);
}

void ByteCodeWriter :: newObject(CommandTape& tape, int fieldCount, ref_t reference)
{
   //   new fieldCount, vmt

   tape.write(bcNew, reference | mskVMTRef, fieldCount);
}

void ByteCodeWriter :: initObject(CommandTape& tape, int fieldCount, LexicalType sourceType, ref_t sourceArgument)
{
   tape.write(bcBCopyA);

   loadObject(tape, sourceType, sourceArgument);
   initBase(tape, fieldCount);

   tape.write(bcACopyB);
}

void ByteCodeWriter :: newVariable(CommandTape& tape, ref_t reference, LexicalType field, ref_t argument)
{
   loadBase(tape, field, argument);
   newObject(tape, 1, reference);
   tape.write(bcBSwap);
   tape.write(bcAXSaveBI, 0);
   tape.write(bcACopyB);
}

void ByteCodeWriter :: newDynamicObject(CommandTape& tape)
{
   // create
   tape.write(bcCreate);
}

void ByteCodeWriter :: initBase(CommandTape& tape, int fieldCount)
{
   //   dcopy 0                  |   { axsavebi i }n
   //   ecopy fieldCount
   // labNext:
   //   xset
   //   next labNext
   switch (fieldCount) {
      case 0:
         break;
      case 1:
         tape.write(bcAXSaveBI, 0);
         break;
      case 2:
         tape.write(bcAXSaveBI, 0);
         tape.write(bcAXSaveBI, 1);
         break;
      case 3:
         tape.write(bcAXSaveBI, 0);
         tape.write(bcAXSaveBI, 1);
         tape.write(bcAXSaveBI, 2);
         break;
      case 4:
         tape.write(bcAXSaveBI, 0);
         tape.write(bcAXSaveBI, 1);
         tape.write(bcAXSaveBI, 2);
         tape.write(bcAXSaveBI, 3);
         break;
      case 5:
         tape.write(bcAXSaveBI, 0);
         tape.write(bcAXSaveBI, 1);
         tape.write(bcAXSaveBI, 2);
         tape.write(bcAXSaveBI, 3);
         tape.write(bcAXSaveBI, 4);
         break;
      case 6:
         tape.write(bcAXSaveBI, 0);
         tape.write(bcAXSaveBI, 1);
         tape.write(bcAXSaveBI, 2);
         tape.write(bcAXSaveBI, 3);
         tape.write(bcAXSaveBI, 4);
         tape.write(bcAXSaveBI, 5);
         break;
      default:
         tape.write(bcDCopy, 0);
         tape.write(bcECopy, fieldCount);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcXSet);
         tape.write(bcNext, baCurrentLabel);
         tape.releaseLabel();
         break;
   }
}

void ByteCodeWriter :: loadBase(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument)
{
   switch (sourceType) {
      case lxCurrent:
         // bloadsi param
         tape.write(bcBLoadSI, sourceArgument);
         break;
      case lxLocalAddress:
         // bcopyf n
         tape.write(bcBCopyF, sourceArgument);
         break;
      case lxResult:
         // bcopya
         tape.write(bcBCopyA);
         break;
      case lxField:
         // pusha
         // bloadfi 1
         // aloadbi
         // bcopya
         // popa
         tape.write(bcPushA);
         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcALoadBI, sourceArgument);
         tape.write(bcBCopyA);
         tape.write(bcPopA);
         break;
   }
}

void ByteCodeWriter :: assignBaseTo(CommandTape& tape, LexicalType target, int offset)
{
   switch (target) {
      case lxResult:
         // acopyb
         tape.write(bcACopyB);
         break;
   }
}

void ByteCodeWriter :: copyBase(CommandTape& tape, int size)
{
   switch (size) {
      case 1:
      case 2:
      case 4:
         tape.write(bcNCopy);
         break;
      case 8:
         tape.write(bcLCopy);
         break;
      default:
         // dcopy 0
         // ecopy count / 4
         // pushe
         // labCopy:
         // esavesi 0
         // nread
         // nwrite
         // eloadsi
         // next labCopy
         // pop
         tape.write(bcDCopy);
         tape.write(bcECopy, size >> 2);
         tape.write(bcPushE);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcESaveSI);
         tape.write(bcNRead);
         tape.write(bcNWrite);
         tape.write(bcELoadSI);
         tape.write(bcNext, baCurrentLabel);
         tape.releaseLabel();
         tape.write(bcPop);         
         break;
   }
}

void ByteCodeWriter :: saveBase(CommandTape& tape, bool directOperation, LexicalType sourceType, ref_t sourceArgument)
{
   switch (sourceType) {
      case lxResult:
         if (directOperation) {
            // axsavebi
            tape.write(bcAXSaveBI, sourceArgument);
         }
         else {
            // asavebi
            tape.write(bcASaveBI, sourceArgument);
         }
         break;
   }
}

void ByteCodeWriter :: boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing)
{
   // ifheap labSkip
   // bcopya
   // newn vmt, size
   // copyb
   // labSkip:

   if (!alwaysBoxing) {
      tape.newLabel();
      tape.write(bcIfHeap, baCurrentLabel);
   }

   if (size == -4) {
      tape.write(bcNLen);
      tape.write(bcBCopyA);
      tape.write(bcACopyR, vmtReference | mskVMTRef);
      tape.write(bcNCreate);
      tape.write(bcCopyB);
   }
   else if (size == -2) {
      tape.write(bcWLen);
      tape.write(bcBCopyA);
      tape.write(bcACopyR, vmtReference | mskVMTRef);
      tape.write(bcWCreate);
      tape.write(bcCopyB);
   }
   else if (size < 0) {
      tape.write(bcBLen);
      tape.write(bcBCopyA);
      tape.write(bcACopyR, vmtReference | mskVMTRef);
      tape.write(bcBCreate);
      tape.write(bcCopyB);
   }
   else {
      tape.write(bcBCopyA);
      tape.write(bcNewN, vmtReference | mskVMTRef, size);

      if (size >0 && size <= 4) {
         tape.write(bcNCopyB);
      }
      else if (size == 8) {
         tape.write(bcLCopyB);
      }
      else tape.write(bcCopyB);
   }

   if (!alwaysBoxing)
      tape.setLabel();
}

void ByteCodeWriter :: boxArgList(CommandTape& tape, ref_t vmtReference)
{
   // bcopya
   // dcopy 0
   // labSearch:
   // get
   // inc
   // elser labSearch
   // acopyr vmt
   // create

   // pusha
   // xlen
   // dcopy 0
   // labCopy:
   // get
   // bswapsi 0
   // xset
   // bswapsi 0
   // next labCopy
   // popa

   tape.write(bcBCopyA);
   tape.write(bcDCopy, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcGet);
   tape.write(bcInc);
   tape.write(bcElseR, baCurrentLabel, 0);
   tape.releaseLabel();

   tape.write(bcACopyR, vmtReference | mskVMTRef);
   tape.write(bcCreate);

   tape.write(bcPushA);
   tape.write(bcXLen);
   tape.write(bcDCopy, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcGet);
   tape.write(bcBSwapSI);
   tape.write(bcXSet);
   tape.write(bcBSwapSI);
   tape.write(bcNext, baCurrentLabel);
   tape.releaseLabel();

   tape.write(bcPopA);
}

void ByteCodeWriter :: unboxArgList(CommandTape& tape)
{
   // bcopya
   // dcopy 0
   // labSearch:
   // get
   // inc
   // elser labSearch
   // ecopyd
   // dec
   // pushn 0

   // labNext:
   // dec
   // get
   // pusha
   // elsen labNext 0

   tape.write(bcBCopyA);
   tape.write(bcDCopy, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcGet);
   tape.write(bcInc);
   tape.write(bcElseR, baCurrentLabel, 0);
   tape.releaseLabel();
   tape.write(bcECopyD);
   tape.write(bcDec);
   tape.write(bcPushN, 0);

   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcDec);
   tape.write(bcGet);
   tape.write(bcPushA);
   tape.write(bcElseN, baCurrentLabel, 0);
   tape.releaseLabel();
}

void ByteCodeWriter :: popObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument)
{
   switch (sourceType) {
      case lxResult:
         // popa
         tape.write(bcPopA);
         break;
      case lxCurrentMessage:
         // pope
         tape.write(bcPopE);
         break;
   }
}

void ByteCodeWriter :: freeVirtualStack(CommandTape& tape, int count)
{
   tape.write(bcFreeStack, count);
}

void ByteCodeWriter :: releaseObject(CommandTape& tape, int count)
{
   // popi n
   if (count == 1) {
      tape.write(bcPop);
   }
   else if (count > 1)
      tape.write(bcPopI, count);
}

void ByteCodeWriter :: releaseArgList(CommandTape& tape)
{
   // bcopya
   // labSearch:
   // popa
   // elser labSearch
   // acopyb

   tape.write(bcBCopyA);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcPopA);
   tape.write(bcElseR, baCurrentLabel, 0);
   tape.releaseLabel();
   tape.write(bcACopyB);
}

void ByteCodeWriter::setSubject(CommandTape& tape, ref_t subject)
{
   // setsubj subj
   tape.write(bcSetSubj, subject);
}

void ByteCodeWriter :: callMethod(CommandTape& tape, int vmtOffset, int paramCount)
{
   // acallvi offs

   tape.write(bcACallVI, vmtOffset);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: callRoleMessage(CommandTape& tape, int paramCount)
{
   // acallvi 0
   tape.write(bcACallVI);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message)
{
   // xjumprm r, m

   tape.write(bcXJumpRM, reference | mskVMTMethodAddress, message);
}

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message, bool withValidattion)
{
   // validate
   // xcallrm r, m

   if(withValidattion)
      tape.write(bcValidate);

   tape.write(bcXCallRM, reference | mskVMTMethodAddress, message);

   tape.write(bcFreeStack, 1 + getParamCount(message));
}

void ByteCodeWriter :: callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message)
{
   // xindexrm r, m
   // acallvd

   tape.write(bcXIndexRM, reference | mskVMTEntryOffset, message);
   tape.write(bcACallVD);

   tape.write(bcFreeStack, 1 + getParamCount(message));
}

void ByteCodeWriter :: doGenericHandler(CommandTape& tape)
{
   // bsredirect

   tape.write(bcBSRedirect);
}

void ByteCodeWriter :: resend(CommandTape& tape)
{
   // ajumpvi 0
   tape.write(bcAJumpVI);
}

void ByteCodeWriter :: callExternal(CommandTape& tape, ref_t functionReference, int paramCount)
{
   // callextr ref
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef)
{
   // ifr then-end, r
   tape.write(bcIfR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef, bool jumpToEnd)
{
   // elser then-end, r
   if (comparingRef == 0) {
      tape.write(bcElseR, jumpToEnd ? baFirstLabel : baCurrentLabel, 0);
   }
   else tape.write(bcElseR, jumpToEnd ? baFirstLabel : baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: throwCurrent(CommandTape& tape)
{
   // throw
   tape.write(bcThrow);
}

void ByteCodeWriter :: gotoEnd(CommandTape& tape, PseudoArg label)
{
   // jump labEnd
   tape.write(bcJump, label);
}

void ByteCodeWriter :: insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   // exclude code should follow open command
   it++;

   // reserve

   tape.insert(it, ByteCommand(bcReserve, size));
}

void ByteCodeWriter :: updateStackAlloc(ByteCodeIterator it, int size)
{
   while (*it != bcReserve)  {
      it++;
   }

   (*it).argument += size;
}

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
}

void ByteCodeWriter :: endAlt(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
}

void ByteCodeWriter :: endThenBlock(CommandTape& tape, bool withStackControl)
{
   // then-end:
   //  scopyf  branch-level

   tape.setLabel();

   if (withStackControl) {
      tape.write(bcSCopyF, bsBranch);
      tape.write(blEnd, bsBranch);
   }
      
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

void ByteCodeWriter :: endExternalBlock(CommandTape& tape)
{
   tape.write(bcSCopyF, bsBranch);
   tape.write(blEnd, bsBranch);
}

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

void ByteCodeWriter :: endStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // finally block - should free the lock if the exception was thrown
   declareCatch(tape);

   tape.write(bcBCopyA);
   tape.write(bcPopA);
   freeLock(tape);
   tape.write(bcACopyB);

   // throw
   tape.write(bcThrow);

   endCatch(tape);

   tape.write(bcBCopyA);
   tape.write(bcPopA);
   freeLock(tape);
   tape.write(bcACopyB);

   // HOTFIX : contains no symbol ending tag, to correctly place an expression end debug symbol
   // asaver static
   tape.write(bcASaveR, staticReference | mskStatSymbolRef);
   tape.setLabel();

   // symbol-end:
   tape.write(blEnd, bsSymbol);
}

void ByteCodeWriter :: writeProcedureDebugInfo(MemoryWriter* debug, ref_t sourceNameRef)
{
   DebugLineInfo symbolInfo(dsProcedure, 0, 0, 0);
   symbolInfo.addresses.source.nameRef = sourceNameRef;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
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
      scope.debugStrings->writeLiteral(SELF_VAR);

      level -= frameLevel;
   }
   else scope.debugStrings->writeLiteral(THIS_VAR);

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

void ByteCodeWriter :: writeMessageInfo(Scope& scope, DebugSymbol symbol, ref_t nameRef)
{
   if (!scope.debug)
      return;

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

   debugStrings->writeLiteral(className);

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
   IdentifierString bookmark("#", symbolName);
   debugModule->mapPredefinedReference(bookmark, debug->Position());

   ref_t position = debugStrings->Position();

   debugStrings->writeLiteral(symbolName);

   DebugLineInfo symbolInfo(dsSymbol, 0, 0, 0);
   symbolInfo.addresses.symbol.nameRef = position;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(module->mapSection(reference | mskSymbolRef, false));

   Scope scope;
   scope.sourceRef = sourceRef;
   scope.code = &codeWriter;

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

void ByteCodeWriter :: save(CommandTape& tape, _Module* module, _Module* debugModule, ref_t sourceRef)
{
   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin) {
         ref_t reference = (*it).additional;
         if ((*it).Argument() == bsClass) {
            writeClass(reference, ++it, module, debugModule, sourceRef);
         }
         else if ((*it).Argument() == bsSymbol) {
            writeSymbol(reference, ++it, module, debugModule, sourceRef);
         }
      }
      it++;
   }
}

void ByteCodeWriter :: writeClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(module->mapSection(reference | mskClassRef, false));

   // initialize vmt section writers
   MemoryWriter vmtWriter(module->mapSection(reference | mskVMTRef, false));

   vmtWriter.writeDWord(0);                              // save size place holder
   size_t classPosition = vmtWriter.Position();

   // copy class meta data header + vmt size
   MemoryReader reader(module->mapSection(reference | mskMetaRDataRef, true));
   ClassInfo info;
   info.load(&reader);

   info.header.count = info.methods.Count(); // set VMT length

   vmtWriter.writeDWord(info.classClassRef);                   // vmt class reference

   vmtWriter.write((void*)&info.header, sizeof(ClassHeader));  // header

   Scope scope;
   scope.sourceRef = sourceRef;
   scope.code = &codeWriter;
   scope.vmt = &vmtWriter;

   // create debug info if debugModule available
   if (debugModule) {
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debugStrings = &debugStringWriter;
      scope.debug = &debugWriter;

     // save class debug info
      writeClassDebugInfo(debugModule, &debugWriter, &debugStringWriter, module->resolveReference(reference & ~mskAnyRef), info.header.flags);
      writeFieldDebugInfo(info, &debugWriter, &debugStringWriter);

      writeVMT(classPosition, it, scope);

      writeDebugInfoStopper(&debugWriter);
   }
   else writeVMT(classPosition, it, scope);
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
   if (scope.debug)
      writeProcedureDebugInfo(scope.debug, scope.sourceRef);

   scope.code->writeDWord(0);                                // write size place holder
   size_t procPosition = scope.code->Position();

   Map<int, int> labels;
   Map<int, int> fwdJumps;
   Stack<int>    stackLevels;                          // scope stack levels

   int frameLevel = 0;
   int level = 1;
   int stackLevel = 0;
   while (!it.Eof() && level > 0) {
      // calculate stack level
      if(*it == bcAllocStack) {
         stackLevel += (*it).argument;
      }
      else if (*it == bcResetStack) {
         stackLevel = stackLevels.peek();
      }
      else if (ByteCodeCompiler::IsPush(*it)) {
         stackLevel++;            
      }
      else if (ByteCodeCompiler::IsPop(*it) || *it == bcFreeStack) {
         stackLevel -= (*it == bcPopI || *it == bcFreeStack) ? (*it).argument : 1;

         // clear previous stack level bookmarks when they are no longer valid
         while (stackLevels.Count() > 0 && stackLevels.peek() > stackLevel)
            stackLevels.pop();
      }

      // save command
      switch (*it) {
         case bcFreeStack:
         case bcAllocStack:
         case bcResetStack:
         case bcNone:
         case bcNop:
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
         case blDeclare:
            if ((*it).Argument() == bsBranch) {
               stackLevels.push(stackLevel);
            }
            break;
         case blEnd:
            if ((*it).Argument() == bsBranch) {
               stackLevels.pop();
            }
            else level--;
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
         case bdSelf:
            writeSelf(scope, (*it).additional, frameLevel);
            break;
         case bdLocal:
            writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, frameLevel);
            break;
         case bdIntLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsIntLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsIntLocalPtr, 0);
            break;
         case bdLongLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsLongLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsLongLocalPtr, 0);
            break;
         case bdRealLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsRealLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsRealLocalPtr, 0);
            break;
         case bdByteArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsByteArrayLocal, frameLevel);
            }
            // else it is a primitive variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsByteArrayLocalPtr, 0);
            break;
         case bdShortArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsShortArrayLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsShortArrayLocalPtr, 0);
            break;
         case bdIntArrayLocal:
            if ((*it).predicate == bpFrame) {
               // if it is a variable containing reference to the primitive value
               writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsIntArrayLocal, frameLevel);
            }
            // else it is a primitice variable
            else writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsIntArrayLocalPtr, 0);
            break;
         case bdParamsLocal:
            writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsParamsLocal, frameLevel);
            break;
         case bdMessage:
            writeMessageInfo(scope, dsMessage, (*it).additional);
            break;
         case bdStruct:
            writeLocal(scope, (ident_t)(*it).Argument(), (*it).additional, dsStructPtr, 0);
            it++;
            writeInfo(scope, dsStructInfo, (ident_t)(*it).Argument());
            break;
         case bcOpen:
            frameLevel = (*it).argument;
            stackLevel = 0;
            (*it).save(scope.code);
            break;
         case bcPushFI:
         case bcPushF:
         case bcALoadFI:
         case bcASaveFI:
         case bcACopyF:
         case bcBLoadFI:
         case bcDLoadFI:
         case bcDSaveFI:
         case bcELoadFI:
         case bcESaveFI:
            (*it).save(scope.code, true);
            if ((*it).predicate == bpBlock) {
               scope.code->writeDWord(stackLevels.peek() + (*it).argument);
            }
            else if ((*it).predicate == bpFrame && (*it).argument < 0) {
               scope.code->writeDWord((*it).argument - frameLevel);
            }
            else scope.code->writeDWord((*it).argument);
            break;
         case bcSCopyF:
            (*it).save(scope.code, true);
            if ((*it).argument == bsBranch) {
               stackLevel = stackLevels.peek();
            }
            else stackLevel = (*it).additional;

            scope.code->writeDWord(stackLevel);
            break;
         case bcIfR:
         case bcElseR:
         case bcIfB:
         case bcElseB:
         case bcIf:
         case bcElse:
         case bcLess:
         case bcNotLess:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcIfM:
         case bcElseM:
         case bcNext:
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

void ByteCodeWriter :: saveInt(CommandTape& tape, LexicalType target, int argument)
{
   if (target == lxLocalAddress) {
      // bcopyf param
      // nsave
      tape.write(bcBCopyF, argument);
      tape.write(bcNSave);

      tape.write(bcACopyB);
   }
}

void ByteCodeWriter :: loadIndex(CommandTape& tape, LexicalType target, ref_t sourceArgument)
{
   if (target == lxResult) {
      tape.write(bcNLoad);
   }
}

void ByteCodeWriter :: assignInt(CommandTape& tape, LexicalType target, int offset)
{
   if (target == lxFieldAddress) {

      if (offset == 0) {
         // bloadfi 1
         // ncopy

         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcNCopy);
      }
      else {
         // nload
         // ecopyd
         // bloadfi 1
         // dcopy target.param
         // bwrite

         tape.write(bcNLoad);
         tape.write(bcECopyD);
         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcDCopy, offset);
         tape.write(bcBWrite);
      }
   }
   else if (target == lxLocalAddress) {
      // bcopyf param
      // ncopy
      tape.write(bcBCopyF, offset);
      tape.write(bcNCopy);
   }
}

void ByteCodeWriter :: assignShort(CommandTape& tape, LexicalType target, int offset)
{
   if (target == lxFieldAddress) {
      // nload
      // ecopyd
      // bloadfi 1
      // dcopy target.param
      // bwritew
      tape.write(bcNLoad);
      tape.write(bcECopyD);
      tape.write(bcBLoadFI, 1, bpFrame);
      tape.write(bcDCopy, offset);
      tape.write(bcBWriteW);
   }
   else if (target == lxLocalAddress) {
      // bcopyf param
      // ncopy
      tape.write(bcBCopyF, offset);
      tape.write(bcNCopy);
   }
}

void ByteCodeWriter :: assignByte(CommandTape& tape, LexicalType target, int offset)
{
   if (target == lxFieldAddress) {
      // nload
      // ecopyd
      // bloadfi 1
      // dcopy target.param
      // bwriteb

      tape.write(bcNLoad);
      tape.write(bcECopyD);
      tape.write(bcBLoadFI, 1, bpFrame);
      tape.write(bcDCopy, offset);
      tape.write(bcBWriteB);
   }
   else if (target == lxLocalAddress) {
      // bcopyf param
      // ncopy
      tape.write(bcBCopyF, offset);
      tape.write(bcNCopy);
   }
}

void ByteCodeWriter :: assignLong(CommandTape& tape, LexicalType target, int offset)
{
   if (target == lxFieldAddress) {
      // bloadfi 1
      tape.write(bcBLoadFI, 1, bpFrame);

      if (offset == 0) {
         // lcopy

         tape.write(bcLCopy);
      }
      else {
         // dcopy 0
         // bread
         // dcopy prm
         // bwrite
         // dcopy 4
         // bread
         // dcopy prm + 4
         // bwrite
         tape.write(bcDCopy, 0);
         tape.write(bcBRead);
         tape.write(bcDCopy, offset);
         tape.write(bcBWrite);
         tape.write(bcDCopy, 4);
         tape.write(bcBRead);
         tape.write(bcDCopy, offset + 4);
         tape.write(bcBWrite);
      }
   }
   else if (target == lxLocalAddress) {
      // bcopyf param
      // lcopy
      tape.write(bcBCopyF, offset);
      tape.write(bcLCopy);
   }
}

void ByteCodeWriter :: copyStructure(CommandTape& tape, int offset, int size)
{
   // if it is alinged
   if ((offset & 3) == 0 && (size & 3) == 3) {
      if (size == 8) {
         // nloadi offset
         // nsavei 0
         // nloadi offset + 1
         // nsavei 1
         tape.write(bcNLoadI, offset);
         tape.write(bcNSaveI, 0);
         tape.write(bcNLoadI, offset + 1);
         tape.write(bcNSaveI, 1);
      }
      else if (size == 12) {
         // nloadi offset
         // nsavei 0
         // nloadi offset + 1
         // nsavei 1
         // nloadi offset + 2
         // nsavei 2
         tape.write(bcNLoadI, offset);
         tape.write(bcNSaveI, 0);
         tape.write(bcNLoadI, offset + 1);
         tape.write(bcNSaveI, 1);
         tape.write(bcNLoadI, offset + 2);
         tape.write(bcNSaveI, 2);
      }
      else if (size == 16) {
         // nloadi offset
         // nsavei 0
         // nloadi offset + 1
         // nsavei 1
         // nloadi offset + 2
         // nsavei 2
         // nloadi offset + 3
         // nsavei 3
         tape.write(bcNLoadI, offset);
         tape.write(bcNSaveI, 0);
         tape.write(bcNLoadI, offset + 1);
         tape.write(bcNSaveI, 1);
         tape.write(bcNLoadI, offset + 2);
         tape.write(bcNSaveI, 2);
         tape.write(bcNLoadI, offset + 3);
         tape.write(bcNSaveI, 3);
      }
      else {
         // dcopy 0
         // ecopy count / 4
         // pushe
         // labCopy:
         // esavesi 0
         // addn (offset / 4)
         // nread
         // addn -offset
         // nwrite
         // eloadsi
         // next labCopy
         // pop

         tape.write(bcDCopy);
         tape.write(bcECopy, size >> 2);
         tape.write(bcPushE);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcESaveSI);
         tape.write(bcAddN, offset >> 2);
         tape.write(bcNRead);
         tape.write(bcAddN, -(offset >> 2));
         tape.write(bcNWrite);
         tape.write(bcELoadSI);
         tape.write(bcNext, baCurrentLabel);
         tape.releaseLabel();
         tape.write(bcPop);
      }
   }
   else {
      // dcopy 0
      // ecopy count
      // pushe
      // labCopy:
      // esavesi 0
      // addn offset
      // breadb
      // addn -offset
      // bwriteb
      // eloadsi 0
      // next labCopy
      // pop

      tape.write(bcDCopy);
      tape.write(bcECopy, size);
      tape.write(bcPushE);
      tape.newLabel();
      tape.setLabel(true);
      tape.write(bcESaveSI);
      tape.write(bcAddN, offset);
      tape.write(bcBReadB);
      tape.write(bcAddN, -offset);
      tape.write(bcBWriteB);
      tape.write(bcELoadSI);
      tape.write(bcNext, baCurrentLabel);
      tape.releaseLabel();
      tape.write(bcPop);
   }
}

void ByteCodeWriter :: copyInt(CommandTape& tape, int offset)
{
   if (offset != 0) {
      // dcopy index
      // bread
      // dcopye

      tape.write(bcDCopy, offset);
      tape.write(bcBRead);
      tape.write(bcDCopyE);
   }
   else {
      // nload
      tape.write(bcNLoad);
   }

   // nsave
   // acopyb

   tape.write(bcNSave);
   tape.write(bcACopyB);
}

void ByteCodeWriter :: copyShort(CommandTape& tape, int offset)
{
   // dcopy index
   // breadw
   // dcopye
   // nsave
   // acopyb

   tape.write(bcDCopy, offset);
   tape.write(bcBReadW);
   tape.write(bcDCopyE);
   tape.write(bcAndN, 0xFFFF);
   tape.write(bcNSave);
   tape.write(bcACopyB);
}

void ByteCodeWriter :: saveIntConstant(CommandTape& tape, int value)
{
   // bcopya
   // dcopy value
   // nsave

   tape.write(bcBCopyA);
   tape.write(bcDCopy, value);
   tape.write(bcNSave);
}

void ByteCodeWriter :: invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef)
{
   // pushr trueRef
   // ifr labEnd falseRef
   // acopyr falseRef
   // asavesi 0
   // labEnd:
   // popa

   tape.newLabel();

   tape.write(bcPushR, trueRef | mskConstantRef);
   tape.write(bcIfR, baCurrentLabel, falseRef | mskConstantRef);
   tape.write(bcACopyR, falseRef | mskConstantRef);
   tape.write(bcASaveSI);
   tape.setLabel();
   tape.write(bcPopA);
}

void ByteCodeWriter :: saveSubject(CommandTape& tape)
{
   // dcopysubj
   // pushd

   tape.write(bcDCopySubj);
   tape.write(bcPushD);
}

void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case WRITE_MESSAGE_ID:
         tape.write(bcNCopy);
         break;
      case ADD_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
         tape.write(bcNAdd);
         break;
      case SUB_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
         tape.write(bcNSub);
         break;
      case MUL_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         tape.write(bcNMul);
         break;
      case DIV_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
         tape.write(bcNDiv);
         break;
      case AND_MESSAGE_ID:
         tape.write(bcNAnd);
         break;
      case OR_MESSAGE_ID:
         tape.write(bcNOr);
         break;
      case XOR_MESSAGE_ID:
         tape.write(bcNXor);
         break;
      case EQUAL_MESSAGE_ID:
         tape.write(bcNEqual);         
         break;
      case LESS_MESSAGE_ID:
         tape.write(bcNLess);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doLongOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case WRITE_MESSAGE_ID:
         tape.write(bcLCopy);
         break;
      case ADD_MESSAGE_ID:
         tape.write(bcLAdd);
         break;
      case SUB_MESSAGE_ID:
         tape.write(bcLSub);
         break;
      case MUL_MESSAGE_ID:
         tape.write(bcLMul);
         break;
      case DIV_MESSAGE_ID:
         tape.write(bcLDiv);
         break;
      case AND_MESSAGE_ID:
         tape.write(bcLAnd);
         break;
      case OR_MESSAGE_ID:
         tape.write(bcLOr);
         break;
      case XOR_MESSAGE_ID:
         tape.write(bcLXor);
         break;
      case EQUAL_MESSAGE_ID:
         tape.write(bcLEqual);
         break;
      case LESS_MESSAGE_ID:
         tape.write(bcLLess);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doRealOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case WRITE_MESSAGE_ID:
         tape.write(bcLCopy);
         break;
      case ADD_MESSAGE_ID:
         tape.write(bcRAdd);
         break;
      case SUB_MESSAGE_ID:
         tape.write(bcRSub);
         break;
      case MUL_MESSAGE_ID:
         tape.write(bcRMul);
         break;
      case DIV_MESSAGE_ID:
         tape.write(bcRDiv);
         break;
      case EQUAL_MESSAGE_ID:
         tape.write(bcREqual);
         break;
      case LESS_MESSAGE_ID:
         tape.write(bcRLess);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // bcopya
         // get
         tape.write(bcBCopyA);
         tape.write(bcGet);
         break;
      case SET_REFER_MESSAGE_ID:
         // set
         tape.write(bcSet);
         break;
      default:
         break;
   }
}

void ByteCodeWriter::doIntArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // nread
         // dcopye
         // nsave
         tape.write(bcNRead);
         tape.write(bcDCopyE);
         tape.write(bcNSave);
         break;
      case SET_REFER_MESSAGE_ID:
         // ecopyd
         // nload
         // nwrite
         tape.write(bcECopyD);
         tape.write(bcNLoad);
         tape.write(bcNWrite);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: selectByIndex(CommandTape& tape, ref_t r1, ref_t r2)
{
   tape.write(bcSelectR, r1 | mskConstantRef, r2 | mskConstantRef);
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
   // elser labWait

   int labWait = tape.newLabel();
   tape.setLabel(true);
   tape.write(bcSNop);
   tape.write(bcTryLock);
   tape.write(bcElseR, labWait, 0);
   tape.releaseLabel();
}

void ByteCodeWriter::freeLock(CommandTape& tape)
{
   // freelock index
   tape.write(bcFreeLock);
}

inline SNode getChild(SNode node, size_t index)
{
   SNode current = node.firstChild();

   while (index > 0 && current != lxNone) {
      current = current.nextNode();

      index--;
   }

   return current;
}

inline bool existNode(SNode node, LexicalType type)
{
   return SyntaxTree::findChild(node, type) == type;
}

inline size_t countChildren(SNode node)
{
   size_t counter = 0;
   SNode current = node.firstChild();

   while (current != lxNone) {
      current = current.nextNode();

      counter++;
   }

   return counter;
}

inline ref_t defineConstantMask(LexicalType type)
{
   switch(type) {
      case lxConstantClass:
         return mskVMTRef;
      case lxConstantString:
         return mskLiteralRef;
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
      case lxSignatureConstant:
         return mskSignature;
      case lxVerbConstant:
         return mskVerb;
      default:
         return mskConstantRef;
   }
}

void ByteCodeWriter :: translateBreakpoint(CommandTape& tape, SNode node)
{
   if (node == lxBreakpoint) {
      declareBreakpoint(tape, 
         SyntaxTree::findChild(node, lxRow).argument, 
         SyntaxTree::findChild(node, lxCol).argument,
         SyntaxTree::findChild(node, lxLength).argument, node.argument);
   }
}

void ByteCodeWriter :: pushObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type)
   {
      case lxSymbol:
         tape.write(bcCallR, argument | mskSymbolRef);
         tape.write(bcPushA);
         break;
      case lxConstantString:
      case lxConstantClass:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxSignatureConstant:
      case lxVerbConstant:
         // pushr reference
         tape.write(bcPushR, argument | defineConstantMask(type));
         break;
      case lxLocal:
         // pushfi index
         tape.write(bcPushFI, argument, bpFrame);
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
         // aloadfi 1
         // pushai offset / pusha
         tape.write(bcALoadFI, 1, bpFrame);
         if ((int)argument < 0) {
            tape.write(bcPushA);
         }
         else tape.write(bcPushAI, argument);
         break;
      case lxBlockLocal:
         // pushfi index
         tape.write(bcPushFI, argument, bpBlock);
         break;
      case lxNil:
         // pushn 0
         tape.write(bcPushN, 0);
         break;
      case lxResult:
         // pusha
         tape.write(bcPushA);
         break;
      case lxResultField:
         // pushai reference
         tape.write(bcPushAI, argument);
         break;
      case lxCurrentMessage:
         // pushe
         tape.write(bcPushE);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: loadObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type)
   {
      case lxSymbol:
         tape.write(bcCallR, argument | mskSymbolRef);
         break;
      case lxConstantString:
      case lxConstantClass:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxSignatureConstant:
      case lxVerbConstant:
            // pushr reference
         tape.write(bcACopyR, argument | defineConstantMask(type));
         break;
      case lxLocal:
         // aloadfi index
         tape.write(bcALoadFI, argument, bpFrame);
         break;
      case lxCurrent:
         // aloadsi index
         tape.write(bcALoadSI, argument);
         break;
      case lxNil:
         // acopyr 0
         tape.write(bcACopyR);
         break;
      case lxField:
         // bloadfi 1
         // aloadbi / acopyb
         tape.write(bcBLoadFI, 1, bpFrame);
         if ((int)argument < 0) {
            tape.write(bcACopyB);
         }
         else tape.write(bcALoadBI, argument);
         break;
      case lxFieldAddress:
         // aloadfi 1
         tape.write(bcALoadFI, 1, bpFrame);
         break;
      case lxBlockLocal:
         // aloadfi n
         tape.write(bcALoadFI, argument, bpBlock);
         break;
      case lxLocalAddress:
         // acopyf n
         tape.write(bcACopyF, argument);
         break;
      case lxBlockLocalAddr:
         // acopyf n
         tape.write(bcACopyF, argument, bpFrame);
         break;
      case lxResultField:
         // aloadai
         tape.write(bcALoadAI, argument);
         break;
      case lxInternalCall:
         tape.write(bcCallR, argument | mskInternalRef);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: saveObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type)
   {
      case lxLocal:
      //case lxParam:
         // asavefi index
         tape.write(bcASaveFI, argument, bpFrame);
         break;
      case lxCurrent:
         // asavesi index
         tape.write(bcASaveSI, argument);
         break;
      case lxField:
         // bloadfi 1
         // asavebi index
         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcASaveBI, argument);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: loadObject(CommandTape& tape, SNode node)
{
   translateBreakpoint(tape, SyntaxTree::findChild(node, lxBreakpoint));

   loadObject(tape, node.type, node.argument);
}

void ByteCodeWriter::pushObject(CommandTape& tape, SNode node)
{
   translateBreakpoint(tape, SyntaxTree::findChild(node, lxBreakpoint));

   pushObject(tape, node.type, node.argument);
}

void assignOpArguments(SNode node, SNode& larg, SNode& rarg)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         if (larg == lxNone) {
            larg = current;
         }
         else rarg = current;
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

void ByteCodeWriter :: generateArrOperation(CommandTape& tape, SyntaxTree::Node node)
{
   bool setMode = node.argument == SET_REFER_MESSAGE_ID;
   bool assignMode = node != lxArrOp;

   SNode larg, rarg, rarg2;
   assignOpArguments(node, larg, rarg, rarg2);   

   bool largSimple = isSimpleObjectExpression(larg);
   bool rargSimple = isSimpleObjectExpression(rarg);
   bool rarg2Simple = isSimpleObjectExpression(rarg2);

   if (setMode) {
      generateObjectExpression(tape, larg);
      loadBase(tape, lxResult);

      if (assignMode && (!largSimple || !rarg2Simple)) {
         tape.write(bcPushB);
      }

      if (!rargSimple) {
         generateObjectExpression(tape, rarg);
         pushObject(tape, lxResult);
      }

      generateObjectExpression(tape, rarg2);
      loadIndex(tape, lxResult);

      if (!rargSimple) {
         popObject(tape, lxResult);
      }
      else generateObjectExpression(tape, rarg);

      if (!rargSimple || !rarg2Simple) {
         tape.write(bcPopB);
      }
   }
   else {
      if (assignMode && (!largSimple || !rargSimple)) {
         tape.write(bcPushB);
      }

      if (!largSimple) {
         generateObjectExpression(tape, larg);
         pushObject(tape, lxResult);
      }

      generateObjectExpression(tape, rarg);
      loadIndex(tape, lxResult);

      if (!largSimple) {
         popObject(tape, lxResult);
      }
      else generateObjectExpression(tape, larg);

      if (assignMode && (!largSimple || !rargSimple)) {
         tape.write(bcPopB);
      }
   }

   switch (node.type)
   {
      case lxIntArrOp:
         doIntArrayOperation(tape, node.argument);

         if (node.argument == REFER_MESSAGE_ID)
            assignBaseTo(tape, lxResult);
         break;
      case lxArrOp:
         doArrayOperation(tape, node.argument);
         break;
   }   
}

void ByteCodeWriter :: generateOperation(CommandTape& tape, SyntaxTree::Node node)
{
   int operation = node.argument;
   bool assignMode = false;
   bool selectMode = false;
   bool invertMode = false;
   int  level = 0;

   switch (node.argument) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         assignMode = true;
         break;
      case LESS_MESSAGE_ID:
         invertMode = true;
      case EQUAL_MESSAGE_ID:
         selectMode = true;
         break;
      case GREATER_MESSAGE_ID:
         selectMode = true;
         operation = LESS_MESSAGE_ID;
         break;
   }

   SNode larg;
   SNode rarg;
   if (invertMode) {
      assignOpArguments(node, rarg, larg);
   }
   else assignOpArguments(node, larg, rarg);

   bool largSimple = isSimpleObjectExpression(larg);
   bool rargSimple = isSimpleObjectExpression(rarg);

   if (!largSimple) {
      if (assignMode) {
         tape.write(bcPushB);
         level++;
      }

      generateObjectExpression(tape, larg);
      pushObject(tape, lxResult);
      level++;
   }

   if (!rargSimple) {
      if (level == 0 && assignMode) {
         tape.write(bcPushB);
         level++;
      }

      generateObjectExpression(tape, rarg);
      pushObject(tape, lxResult);
      level++;
   }

   if (level > 0 && assignMode)
      loadBase(tape, lxCurrent, level - 1);

   if (!largSimple) {
      loadObject(tape, lxCurrent, level - (assignMode ? 2 : 1));
   }
   else generateObjectExpression(tape, larg);

   if (assignMode) {
      if (node.type == lxIntOp) {
         copyBase(tape, 4);
      }
      else if (node.type == lxLongOp || node == lxRealOp) {
         copyBase(tape, 8);
      }
   }
   else loadBase(tape, lxResult);

   if (!rargSimple) {
      popObject(tape, lxResult);
      level--;
   }
   else generateObjectExpression(tape, rarg);

   if (node.type == lxIntOp) {
      doIntOperation(tape, operation);
   }
   else if (node == lxLongOp) {
      doLongOperation(tape, operation);
   }
   else if (node == lxRealOp) {
      doRealOperation(tape, operation);
   }

   if (selectMode) {
      selectByIndex(tape,
         SyntaxTree::findChild(node, lxElseValue).argument,
         SyntaxTree::findChild(node, lxIfValue).argument);
   }
   else assignBaseTo(tape, lxResult);

   releaseObject(tape, level);
}

void ByteCodeWriter :: generateBoolOperation(CommandTape& tape, SyntaxTree::Node node)
{
   generateExpression(tape, node);

   SNode ifParam = SyntaxTree::findChild(node, lxIfValue);
   SNode elseParam = SyntaxTree::findChild(node, lxElseValue);

   if (node.argument == NOT_MESSAGE_ID) {
      invertBool(tape, ifParam.argument, elseParam.argument);
   }      
}

void ByteCodeWriter :: generateNilOperation(CommandTape& tape, SyntaxTree::Node node)
{
   generateExpression(tape, node);

   SNode ifParam = SyntaxTree::findChild(node, lxIfValue);
   SNode elseParam = SyntaxTree::findChild(node, lxElseValue);

   if (node.argument == EQUAL_MESSAGE_ID) {
      selectByAcc(tape, elseParam.argument, ifParam.argument);
   }
}

void ByteCodeWriter :: generateExternalArguments(CommandTape& tape, SNode node, ExternalScope& externalScope)
{
   ref_t functionRef = 0;

   SNode arg = node.firstChild();
   while (arg != lxNone) {
      ExternalScope::ParamInfo param;

      SNode object = arg.firstChild();
      if (test(arg.type, lxObjectMask)) {
         generateObjectExpression(tape, object);
         pushObject(tape, lxResult);
         param.offset = ++externalScope.frameSize;

         if (arg == lxIntExtArgument) {
            param.size = 4;
         }
         else param.size = -1;

         externalScope.operands.push(param);
      }

      arg = arg.nextNode();
   }
}

void ByteCodeWriter:: saveExternalParameters(CommandTape& tape, ExternalScope& externalScope)
{
   // save function parameters
   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   while (!out_it.Eof()) {
      if ((*out_it).size == 4) {
         loadObject(tape, lxBlockLocal, (*out_it).offset);
         pushObject(tape, lxResultField, 0);
      }
      else {
         loadObject(tape, lxBlockLocal, (*out_it).offset);
         pushObject(tape, lxResult);
      }

      out_it++;
   }
}

void ByteCodeWriter :: generateExternalCall(CommandTape& tape, SNode node)
{
   // compile argument list
   ExternalScope externalScope;
   
   declareExternalBlock(tape);

   bool stdCall = (node == lxStdExternalCall);
   generateExternalArguments(tape, node, externalScope);

   // exclude stack if necessary
   excludeFrame(tape);

   // save function parameters
   saveExternalParameters(tape, externalScope);
   
   // call the function
   callExternal(tape, node.argument, externalScope.frameSize);
   
   if (!stdCall)
      releaseObject(tape, externalScope.operands.Count());

   endExternalBlock(tape);
}

void ByteCodeWriter ::generateCall(CommandTape& tape, SNode callNode)
{
   SNode bpNode = SyntaxTree::findChild(callNode, lxBreakpoint);
   if (bpNode != lxNone) {
      translateBreakpoint(tape, bpNode);

      declareBlock(tape);
   }

   SNode overridden = SyntaxTree::findChild(callNode, lxOverridden);
   if (overridden != lxNone) {
      generateExpression(tape, overridden);
   }
   else tape.write(bcALoadSI, 0);

   // copym message
   SNode msg = SyntaxTree::findChild(callNode, lxMessage);
   if (msg != lxNone) {
      tape.write(bcCopyM, msg.argument);
   }
   else tape.write(bcCopyM, callNode.argument);

   SNode target = SyntaxTree::findChild(callNode, lxTarget);
   if (callNode == lxDirectCalling) {
      callResolvedMethod(tape, target.argument, callNode.argument);
   }
   else if (callNode == lxSDirctCalling) {
      callVMTResolvedMethod(tape, target.argument, callNode.argument);
   }
   else {
      // acallvi offs
      tape.write(bcACallVI, 0);
      tape.write(bcFreeStack, 1 + getParamCount(callNode.argument));
   }

   if (bpNode != lxNone)
      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
}

void ByteCodeWriter :: generateInternalCall(CommandTape& tape, SNode node)
{
   int paramCount = 0;

   // analizing a sub tree
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         paramCount++;
      }

      current = current.nextNode();
   }

   declareArgumentList(tape, paramCount);

   int index = 0;
   current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
         paramCount++;
      }

      if (test(current.type, lxObjectMask)) {
         generateObjectExpression(tape, current);

         saveObject(tape, lxCurrent, index);
         index++;
      }

      current = current.nextNode();
   }

   loadObject(tape, node);
   freeVirtualStack(tape, paramCount);
}

void ByteCodeWriter :: generateCallExpression(CommandTape& tape, SNode node)
{
   bool directMode = true;
   bool argUnboxMode = false;
   bool unboxMode = false;

   int paramCount = 0;
   int presavedCount = 0;

   // analizing a sub tree
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxArgUnboxing) {
         argUnboxMode = true;
         generateExpression(tape, current);
         unboxArgList(tape);
      }
      else if (test(current.type, lxObjectMask)) {
         paramCount++;
      }
    
      // presave the boxed arguments if required
      if (current == lxUnboxing) {
         generateObjectExpression(tape, current);
         pushObject(tape, lxResult);
         presavedCount++;
         unboxMode = true;
      }

      if (current == lxExpression && !isSimpleObjectExpression(current, true)) {
         // ignore nested expression
      }
      else if (test(current.type, lxExpressionMask) || current == lxResult) 
         directMode = false;
   
      current = current.nextNode();
   }

   if (!directMode && paramCount > 1) {
      declareArgumentList(tape, paramCount);
   }
   // if message has no arguments - direct mode is allowed
   else directMode = true;

   size_t counter = countChildren(node);
   size_t index = 0;   
   for (int i = 0; i < counter; i++) {
      // get parameters in reverse order if required
      current = getChild(node, directMode ? counter - i - 1 : i);

      if (current == lxArgUnboxing) {
         // argument list is already unboxed
      }
      else if (test(current.type, lxObjectMask)) {
         if (current == lxUnboxing) {
            SNode tempLocal = SyntaxTree::findChild(current, lxTempLocal);
            if (tempLocal == lxNone) {
               loadObject(tape, lxCurrent, paramCount + presavedCount - 1);
               presavedCount--;
            }
            else loadObject(tape, lxLocal, tempLocal.argument);
         }
         else generateObjectExpression(tape, current);

         if (directMode) {
            pushObject(tape, lxResult);
         }
         else saveObject(tape, lxCurrent, index);

         index++;
      }
   }

   generateCall(tape, node);

   if (argUnboxMode) {
      releaseArgList(tape);
      releaseObject(tape);
   }
   else if (paramCount > getParamCount(node.argument) + 1) {
      //   int  spaceToRelease = callStack.oargUnboxing ? -1 : (callStack.parameters.Count() - getParamCount(messageRef) - 1);
      releaseObject(tape, paramCount - getParamCount(node.argument) - 1);
   }

   // unbox the arguments
   if(unboxMode)
      unboxCallParameters(tape, node);
}

void ByteCodeWriter :: unboxCallParameters(CommandTape& tape, SyntaxTree::Node node)
{
   loadBase(tape, lxResult);

   size_t counter = countChildren(node);
   size_t index = 0;
   while (index < counter) {
      // get parameters in reverse order if required
      SNode current = getChild(node, counter - index - 1);
      if (current == lxUnboxing) {
         SNode target = SyntaxTree::findMatchedChild(current, lxObjectMask);
         SNode tempLocal = SyntaxTree::findChild(current, lxTempLocal);
         if (tempLocal != lxNone) {
            loadObject(tape, lxLocal, tempLocal.argument);
         }
         else popObject(tape, lxResult);

         if (target == lxLocalAddress) {
            tape.write(bcPushB);
            loadBase(tape, target.type, target.argument);
            copyBase(tape, current.argument);
            tape.write(bcPopB);
         }
         else if (target == lxLocal) {
            loadObject(tape, lxResultField);
            saveObject(tape, target.type, target.argument);
         }
         else if (target == lxFieldAddress) {
            tape.write(bcPushB);

            loadBase(tape, target.type, target.argument);
            if (current.argument == 4) {
               copyInt(tape, target.argument);
            }
            else if (current.argument == 2) {
               copyShort(tape, target.argument);
            }
            else copyStructure(tape, target.argument, current.argument);

            tape.write(bcPopB);
         }
      }

      index++;
   }

   assignBaseTo(tape, lxResult);
}

void ByteCodeWriter :: generateReturnExpression(CommandTape& tape, SNode node)
{
   generateExpression(tape, node);

   gotoEnd(tape, baFirstLabel);
}

void ByteCodeWriter :: generateThrowExpression(CommandTape& tape, SNode node)
{
   generateExpression(tape, node);

   pushObject(tape, lxResult);
   throwCurrent(tape);

   gotoEnd(tape, baFirstLabel);
}

void ByteCodeWriter :: generateBoxingExpression(CommandTape& tape, SNode node)
{
   generateExpression(tape, node);

   SNode target = SyntaxTree::findChild(node, lxTarget);

   if (node == lxArgBoxing) {
      boxArgList(tape, target.argument);
   }
   else if (node.argument == 0) {
      newVariable(tape, target.argument, lxResult);
   }
   else boxObject(tape, node.argument, target.argument, node != lxCondBoxing);

   SNode temp = SyntaxTree::findChild(node, lxTempLocal);
   if (temp != lxNone) {
      saveObject(tape, lxLocal, temp.argument);
   }
}

void ByteCodeWriter ::generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node)
{
   int size = node.argument;

   SNode target;
   SNode source;

   SNode child = node.firstChild();
   while (child != lxNone) {
      if (test(child.type, lxObjectMask)) {
         if (target == lxNone) {
            target = child;
         }
         else source = child;
      }

      child = child.nextNode();
   }

   if (test(source.type, lxPrimitiveOpMask) && (IsExprOperator(source.argument) || (source.type == lxIntArrOp && source.argument == REFER_MESSAGE_ID))) {
      loadBase(tape, target.type, target.argument);

      generateObjectExpression(tape, source);
   }
   else {
      generateObjectExpression(tape, source);

      if (source == lxExternalCall || source == lxStdExternalCall) {
         if (target == lxLocalAddress) {
            if (node.argument == 4) {
               saveInt(tape, target.type, target.argument);
            }
         }
      }
      else if (target == lxExpression) {
         SNode arg1, arg2;

         assignOpArguments(target, arg1, arg2);
         loadBase(tape, arg1.type, arg1.argument);
         saveBase(tape, false, lxResult, arg2.argument);
      }
      else if (size != 0) {
         if (source == lxFieldAddress) {
            loadBase(tape, target.type, target.argument);
            if (size == 4) {
               copyInt(tape, source.argument);
            }
            else if (size == 2) {
               copyShort(tape, source.argument);
            }
            else copyStructure(tape, source.argument, size);
         }
         else {
            if (size == 4) {
               assignInt(tape, target.type, target.argument);
            }
            else if (size == 2) {
               assignShort(tape, target.type, target.argument);
            }
            else if (size == 1) {
               assignByte(tape, target.type, target.argument);
            }
            else if (size == 8) {
               assignLong(tape, target.type, target.argument);
            }
         }
      }
      else saveObject(tape, target.type, target.argument);
   }
}

void ByteCodeWriter ::generateLocking(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target;
   SNode block;
   assignOpArguments(node, target, block);

   generateObjectExpression(tape, target);
   pushObject(tape, lxResult);

   tryLock(tape);
   declareTry(tape);

   generateCodeBlock(tape, block);

   popObject(tape, lxResult);
   freeLock(tape);

   // finally block - should free the lock if the exception was thrown
   declareCatch(tape);

   popObject(tape, lxResult);
   freeLock(tape);

   throwCurrent(tape);

   endCatch(tape);

}

void ByteCodeWriter :: generateTrying(CommandTape& tape, SyntaxTree::Node node)
{
   bool first = true;

   declareTry(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxExpressionMask)) {
         generateObjectExpression(tape, current);

         if (first) {
            declareCatch(tape);

            // ...

            first = false;
         }
      }
      current = current.nextNode();
   }

   endCatch(tape);
}

void ByteCodeWriter :: generateAlt(CommandTape& tape, SyntaxTree::Node node)
{
   bool first = true;

   declareTry(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxExpressionMask)) {
         generateObjectExpression(tape, current);

         if (first) {
            declareAlt(tape);

            first = false;
         }
      }
      current = current.nextNode();
   }

   endAlt(tape);
}

void ByteCodeWriter :: generateLooping(CommandTape& tape, SyntaxTree::Node node)
{
   declareLoop(tape/*, true*/);

   //declareBlock(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIf) {
         jumpIfNotEqual(tape, current.argument);
         
         generateCodeBlock(tape, current);
      }
      else if (current == lxElse) {
         declareElseBlock(tape);

         generateCodeBlock(tape, current);
      }
      else if (test(current.type, lxObjectMask)) {
         declareBlock(tape);
         generateObjectExpression(tape, current);
         declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
      }
         
      current = current.nextNode();
   }

   //declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);

   if (node.argument != 0) {
      endLoop(tape, node.argument);
   }
   else endLoop(tape);
}

void ByteCodeWriter :: generateSwitching(CommandTape& tape, SyntaxTree::Node node)
{
   declareSwitchBlock(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxVariable) {
         generateObjectExpression(tape, current);
      }
      else if (current == lxOption) {
         declareSwitchOption(tape);

         generateExpression(tape, current);

         endSwitchOption(tape);
      }
      else if (current == lxElse) {
         generateObjectExpression(tape, current);
      }

      current = current.nextNode();
   }

   endSwitchBlock(tape);
}

void ByteCodeWriter :: generateBranching(CommandTape& tape, SyntaxTree::Node node)
{
   if (SyntaxTree::existChild(node, lxElse)) {
      declareThenElseBlock(tape);
   }
   else declareThenBlock(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIf) {
         jumpIfNotEqual(tape, current.argument);

         //declareBlock(tape);
         generateCodeBlock(tape, current);
      }
      else if (current == lxElse) {
         declareElseBlock(tape);

         //declareBlock(tape);
         generateCodeBlock(tape, current);
      }
      else if (test(current.type, lxObjectMask))
         generateObjectExpression(tape, current);

      current = current.nextNode();
   }

   endThenBlock(tape);
}

void ByteCodeWriter :: generateNestedExpression(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = SyntaxTree::findChild(node, lxTarget);

   // presave all the members which could create new objects
   SNode current = node.lastChild();
   while (current != lxNone) {
      if (current.type == lxMember) {
         if (!isSimpleObjectExpression(current, true)) {
            generateExpression(tape, current);
            pushObject(tape, lxResult);
         }
      }

      current = current.prevNode();
   }

   newObject(tape, node.argument, target.argument);

   loadBase(tape, lxResult);

   current = node.firstChild();
   while (current != lxNone) {
      if (current.type == lxMember) {
         if (!isSimpleObjectExpression(current, true)) {
            popObject(tape, lxResult);
         }
         else generateExpression(tape, current);

         saveBase(tape, true, lxResult, current.argument);
      }

      current = current.nextNode();
   }

   assignBaseTo(tape, lxResult);
}

void ByteCodeWriter :: generateStructExpression(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = SyntaxTree::findChild(node, lxTarget);

   newStructure(tape, node.argument, target.argument);
}

void ByteCodeWriter :: generateResendingExpression(CommandTape& tape, SyntaxTree::Node node)
{
   // new frame
   newFrame(tape);

   // save message
   pushObject(tape, lxCurrentMessage);

   generateExpression(tape, node);

   // restore message
   popObject(tape, lxCurrentMessage);

   // close frame
   closeFrame(tape);

   resend(tape);
}

void ByteCodeWriter :: generateObjectExpression(CommandTape& tape, SNode node)
{
   switch (node.type)
   {
      case lxExpression:
         generateExpression(tape, node);
         break;
      case lxTypecasting:
      case lxCalling:
      case lxDirectCalling:
      case lxSDirctCalling:
         generateCallExpression(tape, node);
         break;
      case lxTrying:
         generateTrying(tape, node);
         break;
      case lxAlt:
         generateAlt(tape, node);
         break;
      case lxReturning:
         generateReturnExpression(tape, node);
         break;
      case lxThrowing:
         generateThrowExpression(tape, node);
         break;
      case lxStdExternalCall:
      case lxExternalCall:
         generateExternalCall(tape, node);
         break;
      case lxInternalCall:
         generateInternalCall(tape, node);
         break;
      case lxBoxing:
      case lxCondBoxing:
      case lxArgBoxing:
      case lxUnboxing:
         generateBoxingExpression(tape, node);
         break;
      case lxAssigning:
         generateAssigningExpression(tape, node);
         break;
      case lxBranching:
         generateBranching(tape, node);
         break;
      case lxSwitching:
         generateSwitching(tape, node);
         break;
      case lxLooping:
         generateLooping(tape, node);
         break;
      case lxLocking:
         generateLocking(tape, node);
         break;
      case lxStruct:
         generateStructExpression(tape, node);
         break;
      case lxNested:
         generateNestedExpression(tape, node);
         break;
      case lxBoolOp:
         generateBoolOperation(tape, node);
         break;
      case lxNilOp:
         generateNilOperation(tape, node);
         break;
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
         generateOperation(tape, node);
         break;
      case lxIntArrOp:
      case lxArrOp:
         generateArrOperation(tape, node);
         break;
      case lxResending:
         generateResendingExpression(tape, node);
         break;
      case lxVariable:
         generateExpression(tape, node);
         pushObject(tape, lxResult);
         break;
      case lxIf:
         jumpIfNotEqual(tape, node.argument);
         generateCodeBlock(tape, node);
         break;
      case lxElse:
         if (node.argument != 0)
            jumpIfEqual(tape, node.argument);

         generateCodeBlock(tape, node);
         break;
      default:
         loadObject(tape, node);
         break;
   }
}

void ByteCodeWriter :: generateExpression(CommandTape& tape, SNode node)
{
   SNode child = node.firstChild();
   while (child != lxNone) {
      if (test(child.type, lxObjectMask)) {
         generateObjectExpression(tape, child);
      }
      else if (child == lxVariable) {
         generateObjectExpression(tape, child);
      }
      else if (child == lxReleasing) {
         releaseObject(tape, child.argument);
      }

      child = child.nextNode();
   }      
}

void ByteCodeWriter :: generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset)
{
   loadObject(tape, lxLocalAddress, offset + 2);
   saveIntConstant(tape, -node.argument);
}

void ByteCodeWriter :: generateCodeBlock(CommandTape& tape, SyntaxTree::Node node)
{
   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      LexicalType type = current.type;
      switch (type)
      {
         case lxExpression:
            declareBlock(tape);
            generateExpression(tape, current);
            declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
            break;
         case lxAssigning:
         case lxReturning:
            declareBlock(tape);
            generateObjectExpression(tape, current);
            declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
            break;
         case lxBreakpoint:
            translateBreakpoint(tape, current);
            break;
         case lxVariable:
            declareVariable(tape, current.argument);
            declareLocalInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               SyntaxTree::findChild(current, lxLevel).argument);
            break;
         case lxIntVariable:
            declareLocalIntInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               SyntaxTree::findChild(current, lxLevel).argument, false);
            break;
         case lxLongVariable:
            declareLocalLongInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               SyntaxTree::findChild(current, lxLevel).argument, false);
            break;
         case lxReal64Variable:
            declareLocalRealInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               SyntaxTree::findChild(current, lxLevel).argument, false);
            break;
         case lxBytesVariable:
         {
            int level = SyntaxTree::findChild(current, lxLevel).argument;

            generateBinary(tape, current, level);
            declareLocalByteArrayInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               level, false);
            break;
         }
         case lxShortsVariable:
         {
            int level = SyntaxTree::findChild(current, lxLevel).argument;

            generateBinary(tape, current, level);
            declareLocalShortArrayInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               level, false);
            break;
         }
         case lxIntsVariable:
         {
            int level = SyntaxTree::findChild(current, lxLevel).argument;

            generateBinary(tape, current, level);
            declareLocalIntArrayInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               level, false);
            break;
         }
         case lxBinaryVariable:
         {
            int level = SyntaxTree::findChild(current, lxLevel).argument;
            
            // HOTFIX : only for dynamic objects
            if (current.argument != 0)
               generateBinary(tape, current, level);

            declareStructInfo(tape,
               (ident_t)SyntaxTree::findChild(current, lxTerminal).argument,
               level, (ident_t)SyntaxTree::findChild(current, lxClassName).argument);
            break;
         }
         case lxReleasing:
            releaseObject(tape, current.argument);
            break;
         default:
            generateObjectExpression(tape, current);
            break;
      }
      current = current.nextNode();
   }
}

void ByteCodeWriter :: generateTree(CommandTape& tape, MemoryDump& dump)
{
   SyntaxTree reader(&dump);

   generateCodeBlock(tape, reader.readRoot());
}