//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

using namespace _ELENA_;

// check if the node contains only the simple nodes

bool isSimpleObject(SNode node, bool ignoreFields = false)
{
   if (test(node.type, lxObjectMask)) {
      if (node == lxExpression) {
         if (!isSimpleObjectExpression(node, ignoreFields))
            return false;
      }
      else if (ignoreFields && (node.type == lxField || node.type == lxFieldAddress)) {
         // ignore fields if required
      }
      else if (!test(node.type, lxSimpleMask))
         return false;
   }

   return true;
}

bool _ELENA_::isSimpleObjectExpression(SNode node, bool ignoreFields)
{
   if (node == lxNone)
      return true;

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (!isSimpleObject(current, ignoreFields))
         return false;

      current = current.nextNode();
   }

   return true;
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

void ByteCodeWriter :: declareInitializer(CommandTape& tape, ref_t reference)
{
   // symbol-begin:
   tape.write(blBegin, bsInitializer, reference);
}

void ByteCodeWriter :: declareSymbol(CommandTape& tape, ref_t reference, ref_t sourcePathRef)
{
   // symbol-begin:
   tape.write(blBegin, bsSymbol, reference);

   if (sourcePathRef != (size_t)-1)
      tape.write(bdSourcePath, sourcePathRef);
}

void ByteCodeWriter :: declareStaticSymbol(CommandTape& tape, ref_t staticReference, ref_t sourcePathRef)
{
   // symbol-begin:

   // aloadr static
   // elser procedure-end
   // acopyr ref
   // pusha

   tape.newLabel();     // declare symbol-end label

   if (sourcePathRef != (size_t)-1)
      tape.write(blBegin, bsSymbol, staticReference);

   tape.write(bdSourcePath, sourcePathRef);

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

void ByteCodeWriter :: declareIdleMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef)
{
   // method-begin:
   tape.write(blBegin, bsMethod, message);

   if (sourcePathRef != (size_t)-1)
      tape.write(bdSourcePath, sourcePathRef);
}

void ByteCodeWriter :: declareMethod(CommandTape& tape, ref_t message, ref_t sourcePathRef, int reserved, int allocated, bool withPresavedMessage, bool withNewFrame)
{
   // method-begin:
   //   { pope }?
   //   open
   //   { reserve }?
   //   pusha
   tape.write(blBegin, bsMethod, message);

   if (sourcePathRef != (size_t)-1)
      tape.write(bdSourcePath, sourcePathRef);

   if (withPresavedMessage)
      tape.write(bcPopE);

   if (withNewFrame) {
      if (reserved > 0) {
         // to include new frame header
         tape.write(bcOpen, 3 + reserved);
         tape.write(bcReserve, reserved);
      }
      else tape.write(bcOpen, 1);

      tape.write(bcPushA);

      if (withPresavedMessage)
         saveSubject(tape);

      if (allocated > 0) {
         tape.write(bcInit, allocated);
         tape.write(bcAllocStack, allocated);
      }
   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
{
   tape.write(blDeclare, bsBranch);
}

void ByteCodeWriter :: excludeFrame(CommandTape& tape)
{
   tape.write(bcExclude);
   tape.write(bcAllocStack, 1);
}

void ByteCodeWriter :: includeFrame(CommandTape& tape)
{
   tape.write(bcInclude);
   tape.write(bcSNop);
   tape.write(bcFreeStack, 1);
}

void ByteCodeWriter :: declareStructInfo(CommandTape& tape, ident_t localName, int level, ident_t className)
{
   if (!emptystr(localName)) {
      tape.write(bdStruct, writeString(localName), level);
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

   tape.write(bcResetStack);
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
   tape.write(bcAllocStack, 3);
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
      saveSubject(tape);

   if (allocated > 0) {
      tape.write(bcInit, allocated);
      tape.write(bcAllocStack, allocated);
   }      
}

void ByteCodeWriter :: closeFrame(CommandTape& tape)
{
   // close
   tape.write(bcClose);
}

void ByteCodeWriter :: newDynamicStructure(CommandTape& tape, int itemSize)
{
   if (itemSize == 4) {
      // ncreate
      tape.write(bcNCreate);
   }
   else if (itemSize == 2) {
      // wcreate
      tape.write(bcWCreate);
   }
   else {
      if (itemSize != 1) {
         // muln itemSize
         tape.write(bcMulN, itemSize);
      }
      // bcreate
      tape.write(bcBCreate);
   }
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

void ByteCodeWriter :: initDynamicObject(CommandTape& tape, LexicalType sourceType, ref_t sourceArgument)
{
   tape.write(bcBCopyA);
   tape.write(bcCount);

   loadObject(tape, sourceType, sourceArgument);

   tape.write(bcDCopy, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcXSet);
   tape.write(bcNext, baCurrentLabel);
   tape.releaseLabel();

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
      case lxLocal:
      case lxThisLocal:
         //case lxBoxableLocal:
         // bloadfi param
         tape.write(bcBLoadFI, sourceArgument, bpFrame);
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
      case lxFieldAddress:
         // bloadfi 1
         tape.write(bcBLoadFI, 1, bpFrame);
         break;
      case lxStaticField:
         // bloadr ref
         tape.write(bcBLoadR, sourceArgument | mskStatSymbolRef);
         break;
   }
}

void ByteCodeWriter :: loadInternalReference(CommandTape& tape, ref_t reference)
{
   // acopyr reference
   tape.write(bcACopyR, reference | mskInternalRef);
}

void ByteCodeWriter :: assignBaseTo(CommandTape& tape, LexicalType target)
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
      case -1:
      case -2:
      case -4:
         tape.write(bcCopy);
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

void ByteCodeWriter :: boxField(CommandTape& tape, int offset, int size, ref_t vmtReference)
{
   // bcopya
   // newn vmt, size
   // bswap
   tape.write(bcBCopyA);
   tape.write(bcNewN, vmtReference | mskVMTRef, size);
   tape.write(bcBSwap);

   copyStructureField(tape, offset, 0, size);

   assignBaseTo(tape, lxResult);
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
   // dec               ; to skip trailing nil
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

   tape.write(bcDec);
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

void ByteCodeWriter :: popObject(CommandTape& tape, LexicalType sourceType)
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

void ByteCodeWriter :: unboxMessage(CommandTape& tape)
{
   // ; copy the call stack
   // bcopyf -2
   // dcopycount
   // 
   // inc
   // pushn 0
   // labNextParam:
   // get
   // pusha
   // dec
   // elsen labNextParam 0
   // ; change param count
   // dcopyfi - 1
   // orn 0Fh
   // ecopyd

   tape.write(bcBCopyF, -2);
   tape.write(bcDCopyCount);
   tape.write(bcInc);
   tape.write(bcPushN, 0);
   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcGet);
   tape.write(bcPushA);
   tape.write(bcDec);
   tape.write(bcElseN, baCurrentLabel, 0);
   tape.releaseLabel();
   tape.write(bcDLoadFI, -1);
   tape.write(bcOrN, OPEN_ARG_COUNT);
   tape.write(bcECopyD);
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

void ByteCodeWriter :: callCore(CommandTape& tape, ref_t functionReference, int paramCount)
{
   // callextr ref
   tape.write(bcCallExtR, functionReference | mskNativeCodeRef, paramCount);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef)
{
   // ifr then-end, r
   if (comparingRef == 0) {
      tape.write(bcIfR, baCurrentLabel, 0);
   }
   else tape.write(bcIfR, baCurrentLabel, comparingRef | mskConstantRef);
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

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
   tape.write(bcFreeStack, 3);
}

void ByteCodeWriter :: endAlt(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
   tape.write(bcFreeStack, 3);
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

void ByteCodeWriter :: endExternalBlock(CommandTape& tape,  bool idle)
{
   if (!idle)
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

void ByteCodeWriter :: endInitializer(CommandTape& tape)
{
   // symbol-end:
   tape.write(blEnd, bsInitializer);
}

void ByteCodeWriter :: endStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // finally block - should free the lock if the exception was thrown
   declareCatch(tape);

   tape.write(bcBCopyA);
   tape.write(bcPopA);
   freeLock(tape);
   tape.write(bcPushB);

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

void ByteCodeWriter :: writeProcedureDebugInfo(Scope& scope, ident_t path)
{
   ref_t sourceRef = scope.debugStrings->Position();
   ident_t defaultPath = scope.defaultNameRef != -1 ? (const char*)scope.debugStrings->Memory()->get(scope.defaultNameRef) : NULL;

   if (!emptystr(path) && !path.compare(defaultPath)) {
      scope.debugStrings->writeLiteral(path);
   }
   else sourceRef = scope.defaultNameRef;

   DebugLineInfo symbolInfo(dsProcedure, 0, 0, 0);
   symbolInfo.addresses.source.nameRef = sourceRef;

   scope.debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
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

void ByteCodeWriter :: writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule,
   int sourcePathRef, bool appendMode)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(module->mapSection(reference | mskSymbolRef, false));

   Scope scope;
   //scope.codeStrings = strings;
   scope.code = &codeWriter;
   scope.appendMode = appendMode;

   // create debug info if debugModule available
   if (debugModule) {
      // initialize debug info writer
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debugStrings = &debugStringWriter;
      scope.debug = &debugWriter;
      scope.defaultNameRef = sourcePathRef;

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

void ByteCodeWriter :: save(CommandTape& tape, _Module* module, _Module* debugModule, int sourcePathRef)
{
   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin) {
         ref_t reference = (*it).additional;
         if ((*it).Argument() == bsClass) {
            writeClass(reference, ++it, module, debugModule, sourcePathRef);
         }
         else if ((*it).Argument() == bsSymbol) {
            writeSymbol(reference, ++it, module, debugModule, sourcePathRef, false);
         }
         else if ((*it).Argument() == bsInitializer) {
            writeSymbol(reference, ++it, module, debugModule, sourcePathRef, true);
         }
      }
      it++;
   }
}

void ByteCodeWriter :: writeClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, int sourcePathRef)
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

   // reset VMT length
   info.header.count = 0;
   for (ClassInfo::MethodMap::Iterator m_it = info.methods.start(); !m_it.Eof(); m_it++) {
      //NOTE : ingnore private methods
      if (getVerb(m_it.key()) != PRIVATE_MESSAGE_ID)
         info.header.count++;
   }

   vmtWriter.write((void*)&info.header, sizeof(ClassHeader));  // header

   Scope scope;
   //scope.codeStrings = strings;
   scope.code = &codeWriter;
   scope.vmt = &vmtWriter;

   // create debug info if debugModule available
   if (debugModule) {
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debugStrings = &debugStringWriter;
      scope.debug = &debugWriter;
      scope.defaultNameRef = sourcePathRef;

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
   if (*it == bdSourcePath) {
      if (scope.debug)
         writeProcedureDebugInfo(scope, (const char*)_strings.get((*it).Argument()));

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
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsStructPtr, 0);
            it++;
            writeInfo(scope, dsStructInfo, (const char*)_strings.get((*it).Argument()));
            break;
         case bdStructSelf:
            writeLocal(scope, (const char*)_strings.get((*it).Argument()), (*it).additional, dsLocalPtr, frameLevel);
            it++;
            writeInfo(scope, dsStructInfo, (const char*)_strings.get((*it).Argument()));
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
   else if (target == lxFieldAddress) {
      loadBase(tape, target, 0);

      // nsave
      tape.write(bcNSave);

      tape.write(bcACopyB);
   }
}

void ByteCodeWriter :: saveReal(CommandTape& tape, LexicalType target, int argument)
{
   if (target == lxLocalAddress) {
      // bcopyf param
      // rsave
      tape.write(bcBCopyF, argument);
      tape.write(bcRSave);
   }
   else if (target == lxLocal || target == lxThisLocal/* || target == lxBoxableLocal*/) {
      // bloadfi param
      // rsave
      tape.write(bcBLoadFI, argument, bpFrame);
      tape.write(bcRSave);
   }
   else if (target == lxFieldAddress) {
      // push 0
      // push 0
      // bcopys 0
      // rsave
      // bloadfi 1
      // pope
      // dcopy target.param
      // bwrite
      // pope
      // dcopy target.param+4
      // bwrite
      // popi 2
      tape.write(bcPushN);
      tape.write(bcPushN);
      tape.write(bcBCopyS, 0);
      tape.write(bcRSave);
      tape.write(bcBLoadFI, 1, bpFrame);
      tape.write(bcPopE);
      tape.write(bcDCopy, argument);
      tape.write(bcBWrite);
      tape.write(bcPopE);
      tape.write(bcDCopy, argument + 4);
      tape.write(bcBWrite);
      tape.write(bcPopI, 2);
   }
}

void ByteCodeWriter :: loadIndex(CommandTape& tape, LexicalType target, ref_t sourceArgument)
{
   if (target == lxResult) {
      tape.write(bcNLoad);
   }
   else if (target == lxConstantInt) {
      tape.write(bcDCopy, sourceArgument);
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
   else if (target == lxLocal) {
      // bloadfi param
      // ncopy
      tape.write(bcBLoadFI, offset, bpFrame);
      tape.write(bcNCopy);
   }
   else if (target == lxField) {
      // bcopya
      // aloadfi param
      // aloadai param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadFI, 1, bpFrame);
      tape.write(bcALoadAI, offset);
      tape.write(bcNCopyB);
   }
   else if (target == lxStaticField) {
      // bcopya
      // aloadr param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadR, offset | mskStatSymbolRef);
      tape.write(bcNCopyB);
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
   else if (target == lxLocal) {
      // bloadfi param
      // ncopy
      tape.write(bcBLoadFI, offset, bpFrame);
      tape.write(bcNCopy);
   }
   else if (target == lxField) {
      // bcopya
      // aloadfi param
      // aloadai param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadFI, 1, bpFrame);
      tape.write(bcALoadAI, offset);
      tape.write(bcNCopyB);
   }
   else if (target == lxStaticField) {
      // bcopya
      // aloadr param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadR, offset | mskStatSymbolRef);
      tape.write(bcNCopyB);
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
   else if (target == lxLocal) {
      // bloadfi param
      // ncopy
      tape.write(bcBLoadFI, offset, bpFrame);
      tape.write(bcNCopy);
   }
   else if (target == lxField) {
      // bcopya
      // aloadfi param
      // aloadai param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadFI, 1, bpFrame);
      tape.write(bcALoadAI, offset);
      tape.write(bcNCopyB);
   }
   else if (target == lxStaticField) {
      // bcopya
      // aloadr param
      // ncopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadR, offset | mskStatSymbolRef);
      tape.write(bcNCopyB);
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
   else if (target == lxLocal) {
      // bloadfi param
      // lcopy
      tape.write(bcBLoadFI, offset, bpFrame);
      tape.write(bcLCopy);
   }
   else if (target == lxField) {
      // bcopya
      // aloadfi param
      // aloadai param
      // lcopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadFI, 1, bpFrame);
      tape.write(bcALoadAI, offset);
      tape.write(bcLCopyB);
   }
   else if (target == lxStaticField) {
      // bcopya
      // aloadr param
      // lcopyb
      tape.write(bcBCopyA);
      tape.write(bcALoadR, offset | mskStatSymbolRef);
      tape.write(bcLCopyB);
   }
}

void ByteCodeWriter :: assignStruct(CommandTape& tape, LexicalType target, int offset, int size)
{
   if (target == lxFieldAddress) {
      // bloadfi 1
      tape.write(bcBLoadFI, 1);

      copyStructureField(tape, 0, offset, size);
   }
   else if (target == lxLocalAddress) {
      // bcopyf param
      tape.write(bcBCopyF, offset);

      copyStructure(tape, 0, size);
   }
   //else if (target == lxLocalAddress) {
   //   // bloadfi param
   //   tape.write(bcBLoadFI, offset, bpFrame);

   //   copyStructure(tape, 0, size);
   //}
}

void ByteCodeWriter :: copyStructureField(CommandTape& tape, int sour_offset, int dest_offset, int size)
{
   if (size == 4) {
      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {
         // nloadi sour_offset
         // nsavei dest_offset
         tape.write(bcNLoadI, sour_offset >> 2);
         tape.write(bcNSaveI, dest_offset >> 2);
      }
      else {
         // dcopy sour_offset
         // bread
         // dcopy dest_offset
         // bwrite
         tape.write(bcDCopy, sour_offset);
         tape.write(bcBRead);
         tape.write(bcDCopy, dest_offset);
         tape.write(bcBWrite);
      }
   }
   else if (size == 8) {
      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {
         // nloadi sour_offset
         // nsavei dest_offset
         tape.write(bcNLoadI, sour_offset >> 2);
         tape.write(bcNSaveI, dest_offset >> 2);
         // nloadi sour_offset + 1
         // nsavei dest_offset + 1
         tape.write(bcNLoadI, (sour_offset >> 2) + 1);
         tape.write(bcNSaveI, (dest_offset >> 2) + 1);
      }
      else {
         // dcopy sour_offset
         // bread
         // dcopy dest_offset
         // bwrite
         tape.write(bcDCopy, sour_offset);
         tape.write(bcBRead);
         tape.write(bcDCopy, dest_offset);
         tape.write(bcBWrite);
         // dcopy sour_offset + 4
         // bread
         // dcopy dest_offset + 4
         // bwrite
         tape.write(bcDCopy, sour_offset + 4);
         tape.write(bcBRead);
         tape.write(bcDCopy, dest_offset + 4);
         tape.write(bcBWrite);

      }

   }
   else if (size == 1) {
      // dcopy sour_offset
      // breadb
      // dcopy dest_offset
      // bwriteb
      tape.write(bcDCopy, sour_offset);
      tape.write(bcBReadB);
      tape.write(bcDCopy, dest_offset);
      tape.write(bcBWriteB);
   }
   else if ((size & 3) == 0) {
      if ((sour_offset & 3) == 0 && (dest_offset & 3) == 0) {

         // pushn size
         // dcopy 0

         // labNext:
         // addn sour_offset
         // nread
         // addn dest_offset-sour_offset
         // nwrite
         // addn -dest_offset
         // eloadsi 0
         // next labNext
         // pop

         tape.write(bcPushN, size >> 2);
         tape.write(bcDCopy, 0);
         tape.newLabel();
         tape.setLabel(true);

         if (sour_offset != 0)
            tape.write(bcAddN, sour_offset >> 2);

         tape.write(bcNRead);
         tape.write(bcAddN, (dest_offset >> 2) - (sour_offset >> 2));
         tape.write(bcNWrite);

         if (dest_offset != 0)
            tape.write(bcAddN, -(dest_offset >> 2));

         tape.write(bcELoadSI, 0);
         tape.write(bcNext, baCurrentLabel);
         tape.releaseLabel();
         tape.write(bcPop);
      }
      else {
         // pushn size
         // dcopy 0

         // labNext:
         // addn sour_offset
         // bread
         // addn dest_offset-sour_offset
         // bwrite
         // addn 3-dest_offset
         // eloadsi 0
         // next labNext
         // pop

         tape.write(bcPushN, size);
         tape.write(bcDCopy, 0);
         tape.newLabel();
         tape.setLabel(true);

         if (sour_offset != 0)
            tape.write(bcAddN, sour_offset);

         tape.write(bcNRead);
         tape.write(bcAddN, dest_offset - sour_offset);
         tape.write(bcNWrite);

         if (dest_offset != 0)
            tape.write(bcAddN, 3-dest_offset);

         tape.write(bcELoadSI, 0);
         tape.write(bcNext, baCurrentLabel);
         tape.releaseLabel();
         tape.write(bcPop);
      }
   }
   else {
      // pushn size
      // dcopy 0

      // labNext:
      // addn sour_offset
      // breadb
      // addn dest_offset-sour_offset
      // bwriteb
      // addn -dest_offset
      // eloadsi 0
      // next labNext
      // pop

      tape.write(bcPushN, size);
      tape.write(bcDCopy, 0);
      tape.newLabel();
      tape.setLabel(true);

      if (sour_offset != 0)
         tape.write(bcAddN, sour_offset);

      tape.write(bcBReadB);
      tape.write(bcAddN, dest_offset - sour_offset);
      tape.write(bcBWriteB);

      if (dest_offset != 0)
         tape.write(bcAddN, - dest_offset);

      tape.write(bcELoadSI, 0);
      tape.write(bcNext, baCurrentLabel);
      tape.releaseLabel();
      tape.write(bcPop);
   }
}

void ByteCodeWriter :: copyStructure(CommandTape& tape, int offset, int size)
{
   // if it is alinged
   if ((offset & 3) == 0 && (size & 3) == 0) {
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

////void ByteCodeWriter :: invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef)
////{
////   // pushr trueRef
////   // ifr labEnd falseRef
////   // acopyr falseRef
////   // asavesi 0
////   // labEnd:
////   // popa
////
////   tape.newLabel();
////
////   tape.write(bcPushR, trueRef | mskConstantRef);
////   tape.write(bcIfR, baCurrentLabel, falseRef | mskConstantRef);
////   tape.write(bcACopyR, falseRef | mskConstantRef);
////   tape.write(bcASaveSI);
////   tape.setLabel();
////   tape.write(bcPopA);
////}

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
      // Note read / write operator is used for bitwise operations
      case WRITE_MESSAGE_ID:
         // nload
         // not
         // inc
         // nshift
         tape.write(bcNLoad);
         tape.write(bcNot);
         tape.write(bcInc);
         tape.write(bcNShift);
         break;
      // Note read / write operator is used for bitwise operations
      case READ_MESSAGE_ID:
         // nload
         // nshift
         tape.write(bcNLoad);
         tape.write(bcNShift);
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
      case SET_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id, int immArg)
{
   switch (operator_id) {
      // Note read / write operator is used for bitwise operations
      case WRITE_MESSAGE_ID:
         // nload
         // shiftn -immArg
         // nsave
         tape.write(bcNLoad);
         tape.write(bcShiftN, -immArg);
         tape.write(bcNSave);
         break;
      // Note read / write operator is used for bitwise operations
      case READ_MESSAGE_ID:
         // nload
         // shiftn immArg
         // nsave
         tape.write(bcNLoad);
         tape.write(bcShiftN, immArg);
         tape.write(bcNSave);
         break;
      case ADD_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcAddN, immArg);
         tape.write(bcNSave);
         break;
      case SUB_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcAddN, -immArg);
         tape.write(bcNSave);
         break;
      case MUL_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcMulN, immArg);
         tape.write(bcNSave);
         break;
      case AND_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcAndN, immArg);
         tape.write(bcNSave);
         break;
      case OR_MESSAGE_ID:
         tape.write(bcNLoad);
         tape.write(bcOrN, immArg);
         tape.write(bcNSave);
         break;
      case SET_MESSAGE_ID:
         tape.write(bcDCopy, immArg);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doFieldIntOperation(CommandTape& tape, int operator_id, int offset, int immArg)
{
   switch (operator_id) {
         // Note read / write operator is used for bitwise operations
      case WRITE_MESSAGE_ID:
         // dcopy offset
         // bread
         // eswap
         // shiftn -immArg
         // eswap
         // bwrite
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcShiftN, -immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
         // Note read / write operator is used for bitwise operations
      case READ_MESSAGE_ID:
         // dcopy offset
         // bread
         // eswap
         // shiftn immArg
         // eswap
         // bwrite
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcShiftN, immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case ADD_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcAddN, immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case SUB_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcAddN, -immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case MUL_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcMulN, immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case AND_MESSAGE_ID:
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcAndN, immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case OR_MESSAGE_ID:
         tape.write(bcDCopy, offset);
         tape.write(bcBRead);
         tape.write(bcESwap);
         tape.write(bcOrN, immArg);
         tape.write(bcESwap);
         tape.write(bcBWrite);
         break;
      case SET_MESSAGE_ID:
         tape.write(bcECopy, immArg);
         tape.write(bcDCopy, offset);
         tape.write(bcBWrite);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doLongOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      // Note read / write operator is used for bitwise operations
      case WRITE_MESSAGE_ID:
         // nload
         // not
         // inc
         // lshift
         tape.write(bcNLoad);
         tape.write(bcNot);
         tape.write(bcInc);
         tape.write(bcLShift);
         break;
      // Note read / write operator is used for bitwise operations
      case READ_MESSAGE_ID:
         // nload
         // lshift
         tape.write(bcNLoad);
         tape.write(bcLShift);
         break;
      case ADD_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
         tape.write(bcLAdd);
         break;
      case SUB_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
         tape.write(bcLSub);
         break;
      case MUL_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         tape.write(bcLMul);
         break;
      case DIV_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
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
      case APPEND_MESSAGE_ID:
         tape.write(bcRAdd);
         break;
      case SUB_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
         tape.write(bcRSub);
         break;
      case MUL_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         tape.write(bcRMul);
         break;
      case DIV_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
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
      // NOTE : read operator is used to define the array length
      case READ_MESSAGE_ID:
         // len
         // nsave
         tape.write(bcLen);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doArgArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // bcopya
         // get
         tape.write(bcBCopyA);
         tape.write(bcGet);
         break;
      case SET_REFER_MESSAGE_ID:
         // xset
         tape.write(bcXSet);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doIntArrayOperation(CommandTape& tape, int operator_id)
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
         // nloade
         // nwrite
         tape.write(bcNLoadE);
         tape.write(bcNWrite);
         break;
      // NOTE : read operator is used to define the array length
      case READ_MESSAGE_ID:
         // nlen
         // nsave
         tape.write(bcNLen);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doByteArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // breadb
         // dcopye
         // nsave
         tape.write(bcBReadB);
         tape.write(bcDCopyE);
         tape.write(bcNSave);
         break;
      case SET_REFER_MESSAGE_ID:
         // nloade
         // bwriteb
         tape.write(bcNLoadE);
         tape.write(bcBWriteB);
         break;
      // NOTE : read operator is used to define the array length
      case READ_MESSAGE_ID:
         // blen
         // nsave
         tape.write(bcBLen);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doBinaryArrayOperation(CommandTape& tape, int operator_id, int itemSize)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         if (itemSize == 4) {
            // nread
            // dcopye
            // nsave
            tape.write(bcNRead);
            tape.write(bcDCopyE);
            tape.write(bcNSave);
         }
         else if (itemSize == 8) {
            // shiftn -3
            // bread
            // nwritei 0
            // addn 4
            // bread
            // nwritei 1
            tape.write(bcShiftN, -3);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 0);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 1);
         }
         else if (itemSize == 12) {
            // muln 12
            // bread
            // nwritei 0
            // addn 4
            // bread
            // nwritei 1
            // addn 4
            // bread
            // nwritei 2
            tape.write(bcMulN, 12);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 0);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 1);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 2);
         }
         else if (itemSize == 16) {
            // shiftn -4
            // bread
            // nwritei 0
            // addn 4
            // bread
            // nwritei 1
            // addn 4
            // bread
            // nwritei 2
            // addn 4
            // bread
            // nwritei 3
            tape.write(bcShiftN, -4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 0);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 1);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 2);
            tape.write(bcAddN, 4);
            tape.write(bcBRead);
            tape.write(bcNWriteI, 3);
         }
         else if ((itemSize & 3) == 0) {
            // muln itemSize

            // pushd
            // pushn 0

            // labNext:
            // dloadsi 1
            // bread
            // addn 4
            // dsavesi 1
            // dloadsi 0
            // nwrite
            // addn 4
            // dsavesi 0
            // lessn itemSize labNext
            // popi 2

            tape.newLabel();
            tape.write(bcMulN, itemSize);
            tape.write(bcPushD);
            tape.write(bcPushN, 0);
            tape.setLabel(true);
            tape.write(bcDLoadSI, 1);
            tape.write(bcBRead);
            tape.write(bcAddN, 4);
            tape.write(bcDSaveSI, 1);
            tape.write(bcDLoadSI, 0);
            tape.write(bcNWrite);
            tape.write(bcAddN, 4);
            tape.write(bcDSaveSI, 0);
            tape.write(bcLessN, baCurrentLabel, itemSize);
            tape.write(bcPopI, 2);
            tape.releaseLabel();
         }
         else {
            // muln itemSize

            // pushd
            // pushn 0

            // labNext:
            // dloadsi 1
            // breadb
            // addn 1
            // dsavesi 1
            // dloadsi 0
            // bwriteb
            // addn 1
            // dsavesi 0
            // lessn itemSize labNext
            // popi 2

            tape.newLabel();
            tape.write(bcMulN, itemSize);
            tape.write(bcPushD);
            tape.write(bcPushN, 0);
            tape.setLabel(true);
            tape.write(bcDLoadSI, 1);
            tape.write(bcBReadB);
            tape.write(bcAddN, 1);
            tape.write(bcDSaveSI, 1);
            tape.write(bcDLoadSI, 0);
            tape.write(bcBWriteB);
            tape.write(bcAddN, 1);
            tape.write(bcDSaveSI, 0);
            tape.write(bcLessN, baCurrentLabel, itemSize);
            tape.write(bcPopI, 2);
            tape.releaseLabel();
         }
         break;
      case SET_REFER_MESSAGE_ID:
         if (itemSize == 4) {
            // nloade
            // nwrite
            tape.write(bcNLoadE);
            tape.write(bcNWrite);
            break;
         }
         else if (itemSize == 8) {
            // shiftn -3
            // nreadi 0
            // bwrite
            // addn 4
            // nreadi 1
            // bwrite
            tape.write(bcShiftN, -3);
            tape.write(bcNReadI, 0);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 1);
            tape.write(bcBWrite);
         }
         else if (itemSize == 12) {
            // muln 12
            // nreadi 0
            // bwrite
            // addn 4
            // nreadi 1
            // bwrite
            // addn 4
            // nreadi 2
            // bwrite
            tape.write(bcMulN, 12);
            tape.write(bcNReadI, 0);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 1);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 2);
            tape.write(bcBWrite);
         }
         else if (itemSize == 16) {
            // shiftn -4
            // nreadi 0
            // bwrite
            // addn 4
            // nreadi 1
            // bwrite
            // addn 4
            // nreadi 2
            // bwrite
            // addn 4
            // nreadi 3
            // bwrite
            tape.write(bcShiftN, -4);
            tape.write(bcNReadI, 0);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 1);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 2);
            tape.write(bcBWrite);
            tape.write(bcAddN, 4);
            tape.write(bcNReadI, 3);
            tape.write(bcBWrite);
         }
         else if ((itemSize & 3) == 0) {
            // muln itemSize

            // pushn 0
            // pushd

            // dloadsi 1
            // labNext:
            // bread
            // addn 4
            // dsavesi 1
            // dloadsi 0
            // nwrite
            // addn 4
            // dsavesi 0
            // dloadsi 1
            // lessn itemSize labNext
            // popi 2

            tape.newLabel();
            tape.write(bcMulN, itemSize);
            tape.write(bcPushN, 0);
            tape.write(bcPushD);
            tape.write(bcDLoadSI, 1);
            tape.setLabel(true);
            tape.write(bcBRead);
            tape.write(bcAddN, 4);
            tape.write(bcDSaveSI, 1);
            tape.write(bcDLoadSI, 0);
            tape.write(bcNWrite);
            tape.write(bcAddN, 4);
            tape.write(bcDSaveSI, 0);
            tape.write(bcDLoadSI, 1);
            tape.write(bcLessN, baCurrentLabel, itemSize);
            tape.write(bcPopI, 2);
            tape.releaseLabel();
         }
         else {
            // muln itemSize

            // pushn 0
            // pushd

            // dloadsi 1
            // labNext:
            // breadb
            // addn 1
            // dsavesi 1
            // dloadsi 0
            // bwriteb
            // addn 1
            // dsavesi 0
            // dloadsi 1
            // lessn itemSize labNext
            // popi 2

            tape.newLabel();
            tape.write(bcMulN, itemSize);
            tape.write(bcPushN, 0);
            tape.write(bcPushD);
            tape.write(bcDLoadSI, 1);
            tape.setLabel(true);
            tape.write(bcBReadB);
            tape.write(bcAddN, 1);
            tape.write(bcDSaveSI, 1);
            tape.write(bcDLoadSI, 0);
            tape.write(bcBWriteB);
            tape.write(bcAddN, 1);
            tape.write(bcDSaveSI, 0);
            tape.write(bcDLoadSI, 1);
            tape.write(bcLessN, baCurrentLabel, itemSize);
            tape.write(bcPopI, 2);
            tape.releaseLabel();
         }
         break;
      // NOTE : read operator is used to define the array length
      case READ_MESSAGE_ID:
         // blen
         // divn itemSize
         // nsave
         tape.write(bcBLen);
         if (itemSize == 4) {
            tape.write(bcShiftN, 2);
         }
         else if (itemSize == 8) {
            tape.write(bcShiftN, 3);
         }
         else if (itemSize == 16) {
            tape.write(bcShiftN, 4);
         }
         else tape.write(bcDivN, itemSize);
         tape.write(bcNSave);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doShortArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // wread
         // dcopye
         // nsave
         tape.write(bcWRead);
         tape.write(bcDCopyE);
         tape.write(bcNSave);
         break;
      case SET_REFER_MESSAGE_ID:
         // nloade
         // wwrite
         tape.write(bcNLoadE);
         tape.write(bcWWrite);
         break;
      // NOTE : read operator is used to define the array length
      case READ_MESSAGE_ID:
         // wlen
         // nsave
         tape.write(bcWLen);
         tape.write(bcNSave);
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
   // elsen labWait

   int labWait = tape.newLabel();
   tape.setLabel(true);
   tape.write(bcSNop);
   tape.write(bcTryLock);
   tape.write(bcElseN, labWait, 0);
   tape.releaseLabel();
}

void ByteCodeWriter::freeLock(CommandTape& tape)
{
   // freelock
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

//inline bool existNode(SNode node, LexicalType type)
//{
//   return SyntaxTree::findChild(node, type) == type;
//}

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
      case lxSignatureConstant:
         return mskSignature;
      case lxVerbConstant:
         return mskVerb;
      case lxConstantList:
         return mskConstArray;
      default:
         return mskConstantRef;
   }
}

bool ByteCodeWriter :: translateBreakpoint(CommandTape& tape, SNode node)
{
   if (node != lxNone) {
      // try to find the terminal symbol
      SNode terminal = node;
      while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
         terminal = terminal.firstChild(lxObjectMask);
      }

      if (terminal == lxNone) {
         terminal = node.findNext(lxObjectMask);
         while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
            terminal = terminal.firstChild(lxObjectMask);
         }
         // HOTFIX : use idle node 
         if (terminal == lxNone && node.nextNode() == lxIdle) {
            terminal = node.nextNode();
            while (terminal != lxNone && terminal.findChild(lxRow) != lxRow) {
               terminal = terminal.firstChild(lxObjectMask);
            }
         }
      }

      if (terminal != lxNone) {
         declareBreakpoint(tape,
            terminal.findChild(lxRow).argument,
            terminal.findChild(lxCol).argument - 1,
            terminal.findChild(lxLength).argument, node.argument);
      }

      return true;
   }
   else return false;
}

void ByteCodeWriter :: pushObject(CommandTape& tape, LexicalType type, ref_t argument)
{
   switch (type)
   {
      case lxSymbolReference:
         tape.write(bcCallR, argument | mskSymbolRef);
         tape.write(bcPushA);
         break;
      case lxConstantString:
      case lxConstantWideStr:
      case lxConstantClass:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxExtMessageConstant:
      case lxSignatureConstant:
      case lxVerbConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcPushR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxThisLocal:
         //case lxBoxableLocal:
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
      case lxStaticField:
         // aloadr r
         // pusha
         tape.write(bcALoadR, argument | mskStatSymbolRef);
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
   switch (type) {
      case lxSymbolReference:
         tape.write(bcCallR, argument | mskSymbolRef);
         break;
      case lxConstantString:
      case lxConstantWideStr:
      case lxConstantClass:
      case lxConstantSymbol:
      case lxConstantChar:
      case lxConstantInt:
      case lxConstantLong:
      case lxConstantReal:
      case lxMessageConstant:
      case lxExtMessageConstant:
      case lxSignatureConstant:
      case lxVerbConstant:
      case lxConstantList:
         // pushr reference
         tape.write(bcACopyR, argument | defineConstantMask(type));
         break;
      case lxLocal:
      case lxThisLocal:
//      //case lxBoxableLocal:
         // aloadfi index
         tape.write(bcALoadFI, argument, bpFrame);
         break;
      case lxCurrent:
         // aloadsi index
         tape.write(bcALoadSI, argument);
         break;
//      case lxCurrentField:
//         // aloadsi index
//         // aloadai 0
//         tape.write(bcALoadSI, argument);
//         tape.write(bcALoadAI, 0);
//         break;
      case lxNil:
         // acopyr 0
         tape.write(bcACopyR, argument);
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
      case lxStaticField:
         // aloadr r
         tape.write(bcALoadR, argument | mskStatSymbolRef);
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
      case lxThisLocal:
      //case lxBoxableLocal:
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
      case lxStaticField:
         // asaver arg
         tape.write(bcASaveR, argument | mskStatSymbolRef);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: loadObject(CommandTape& tape, SNode node)
{
   loadObject(tape, node.type, node.argument);
}

void ByteCodeWriter::pushObject(CommandTape& tape, SNode node)
{
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

void ByteCodeWriter :: generateNewOperation(CommandTape& tape, SyntaxTree::Node node)
{
   generateExpression(tape, node);
   loadIndex(tape, lxResult);

   if (node.argument != 0) {
      int size = node.findChild(lxSize).argument;

      loadObject(tape, lxConstantClass, node.argument);
      if (size < 0) {
         newDynamicStructure(tape, -size);
      }
      else if (size == 0) {
         newDynamicObject(tape);
         initDynamicObject(tape, lxNil);
      }
   }
   else {
      loadObject(tape, lxThisLocal, 1);
      // HOTFIX: -1 indicates the stack is not consumed by the constructor
      callMethod(tape, 1, -1);
   }
}

void ByteCodeWriter :: generateArrOperation(CommandTape& tape, SyntaxTree::Node node)
{
   bool lenMode = node.argument == READ_MESSAGE_ID;
   bool setMode = node.argument == SET_REFER_MESSAGE_ID;
   bool assignMode = node != lxArrOp || node != lxArgArrOp;

   SNode larg, rarg, rarg2;
   assignOpArguments(node, larg, rarg, rarg2);

   if (rarg == lxExpression)
      rarg = rarg.findSubNodeMask(lxObjectMask);

   bool largSimple = isSimpleObject(larg);
   bool rargSimple = isSimpleObject(rarg);
   bool rarg2Simple = isSimpleObject(rarg2);
   bool immIndex = rarg == lxConstantInt;

   if (setMode) {
      generateObjectExpression(tape, larg);
      loadBase(tape, lxResult);

      if (!rargSimple || !rarg2Simple) {
         tape.write(bcPushB);
      }

      if (!rarg2Simple) {
         generateObjectExpression(tape, rarg2);
         pushObject(tape, lxResult);
      }

      if (immIndex) {
         int index = rarg.findChild(lxIntValue).argument;

         loadIndex(tape, rarg.type, index);
      }
      else {
         generateObjectExpression(tape, rarg);
         loadIndex(tape, lxResult);
      }

      if (!rarg2Simple) {
         popObject(tape, lxResult);
      }
      else generateObjectExpression(tape, rarg2);

      if (!rargSimple || !rarg2Simple) {
         tape.write(bcPopB);
      }
   }
   else if (lenMode) {
      generateObjectExpression(tape, rarg);
      loadBase(tape, lxResult);

      generateObjectExpression(tape, larg);
   }
   else {
      if (assignMode && (!largSimple || !rargSimple)) {
         tape.write(bcPushB);
      }

      if (!largSimple) {
         generateObjectExpression(tape, larg);
         pushObject(tape, lxResult);
      }

      if (immIndex) {
         int index = rarg.findChild(lxIntValue).argument;

         loadIndex(tape, rarg.type, index);
      }
      else {
         generateObjectExpression(tape, rarg);
         loadIndex(tape, lxResult);
      }

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
      case lxByteArrOp:
         doByteArrayOperation(tape, node.argument);

         if (node.argument == REFER_MESSAGE_ID)
            assignBaseTo(tape, lxResult);
         break;
      case lxShortArrOp:
         doShortArrayOperation(tape, node.argument);

         if (node.argument == REFER_MESSAGE_ID)
            assignBaseTo(tape, lxResult);
         break;
      case lxBinArrOp:
         doBinaryArrayOperation(tape, node.argument, node.findChild(lxSize).argument);

         if (node.argument == REFER_MESSAGE_ID)
            assignBaseTo(tape, lxResult);
         break;
      case lxArrOp:
         doArrayOperation(tape, node.argument);
         break;
      case lxArgArrOp:
         doArgArrayOperation(tape, node.argument);
         break;
   }
}

void ByteCodeWriter :: generateOperation(CommandTape& tape, SyntaxTree::Node node)
{
   int operation = node.argument;
   bool assignMode = false;
   bool selectMode = false;
   bool invertSelectMode = false;
   bool invertMode = false;
   bool immOp = false;
   int  level = 0;

   switch (node.argument) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
      case READ_MESSAGE_ID:
      case WRITE_MESSAGE_ID:
         immOp = true;
         assignMode = true;
         break;
      case DIV_MESSAGE_ID:
         assignMode = true;
         break;
      case EQUAL_MESSAGE_ID:
         selectMode = true;
         break;
      case NOTEQUAL_MESSAGE_ID:
         invertSelectMode = true;
         break;
      case LESS_MESSAGE_ID:
         invertMode = true;
         selectMode = true;
         break;
      case GREATER_MESSAGE_ID:
         selectMode = true;
         operation = LESS_MESSAGE_ID;
         break;
      case SET_MESSAGE_ID:
         immOp = true;
         break;
      case APPEND_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
         immOp = true;
         break;
   }

   SNode larg;
   SNode rarg;
   if (invertMode) {
      assignOpArguments(node, rarg, larg);
   }
   else assignOpArguments(node, larg, rarg);

   if (larg == lxExpression)
      larg = larg.findSubNodeMask(lxObjectMask);
   if (rarg == lxExpression)
      rarg = rarg.findSubNodeMask(lxObjectMask);

   bool largSimple = isSimpleObject(larg);
   bool rargSimple = isSimpleObject(rarg);
   bool rargConst = immOp && (rarg == lxConstantInt);

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

   // if operation result is assigned to the same variable
   bool targetSet = false;
   if (assignMode) {
      //SNode parent = node.parentNode();
      //while (parent == lxExpression)
      //   parent = parent.parentNode();

      //if (parent == lxAssigning) {
      //   SNode target = parent.findSubNodeMask(lxObjectMask);
      //   if (target.type == larg.type && target.argument == larg.argument)
      //      targetSet = true;
      //}
   }

   if (targetSet) {
      assignBaseTo(tape, lxResult);
   }
   else {
      if (!largSimple) {
         loadObject(tape, lxCurrent, level - (assignMode ? 2 : 1));
      }
      else generateObjectExpression(tape, larg);

      if (assignMode) {
         if (node.type == lxIntOp && !rargConst) {
            copyBase(tape, 4);
         }
         else if (node.type == lxLongOp || node == lxRealOp) {
            copyBase(tape, 8);
         }
      }
      else loadBase(tape, lxResult);
   }

   if (!rargSimple) {
      popObject(tape, lxResult);
      level--;
   }
   else if (!rargConst)
      generateObjectExpression(tape, rarg);

   if (node.type == lxIntOp) {
      if (rargConst) {
         SNode immArg = rarg.findChild(lxIntValue);
         if (larg == lxFieldAddress && larg.argument > 0) {
            doFieldIntOperation(tape, operation, larg.argument, immArg.argument);
         }
         else doIntOperation(tape, operation, immArg.argument);
      }
      else doIntOperation(tape, operation);
   }
   else if (node == lxLongOp) {
      doLongOperation(tape, operation);
   }
   else if (node == lxRealOp) {
      doRealOperation(tape, operation);
   }

   if (selectMode) {
      selectByIndex(tape,
         node.findChild(lxElseValue).argument,
         node.findChild(lxIfValue).argument);
   }
   else if (invertSelectMode) {
      selectByIndex(tape,
         node.findChild(lxIfValue).argument,
         node.findChild(lxElseValue).argument);
   }
   else assignBaseTo(tape, lxResult);

   if (larg == lxLocalUnboxing) {
      SNode assignNode = larg.findChild(lxAssigning);
      assignOpArguments(assignNode, larg, rarg);

      loadBase(tape, rarg.type, 0);

      if (assignNode.argument == 4) {
         assignInt(tape, lxFieldAddress, rarg.argument);
      }
      else if (assignNode.argument == 2) {
         assignLong(tape, lxFieldAddress, rarg.argument);
      }
      else assignStruct(tape, lxFieldAddress, rarg.argument, assignNode.argument);
   }

   releaseObject(tape, level);
}

//void ByteCodeWriter :: generateBoolOperation(CommandTape& tape, SyntaxTree::Node node)
//{
//   generateExpression(tape, node);
//
//   SNode ifParam = SyntaxTree::findChild(node, lxIfValue);
//   SNode elseParam = SyntaxTree::findChild(node, lxElseValue);
//
//   if (node.argument == NOT_MESSAGE_ID) {
//      invertBool(tape, ifParam.argument, elseParam.argument);
//   }
//}

void ByteCodeWriter :: generateNilOperation(CommandTape& tape, SyntaxTree::Node node)
{
   generateExpression(tape, node);

   SNode ifParam = node.findChild(lxIfValue);
   SNode elseParam = node.findChild(lxElseValue);

   if (node.argument == EQUAL_MESSAGE_ID) {
      selectByAcc(tape, elseParam.argument, ifParam.argument);
   }
}

void ByteCodeWriter :: generateExternalArguments(CommandTape& tape, SNode node, ExternalScope& externalScope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExtInteranlRef) {
      }
      else if (current == lxIntExtArgument || current == lxExtArgument) {
         SNode object = current.findSubNodeMask(lxObjectMask);
         if (test(object.type, lxObjectMask)) {
            if (!isSimpleObject(object, true)) {
               ExternalScope::ParamInfo param;

               generateObjectExpression(tape, object);
               pushObject(tape, lxResult);
               param.offset = ++externalScope.frameSize;

               externalScope.operands.push(param);
            }
         }
      }
      current = current.nextNode();
   }
}

int ByteCodeWriter :: saveExternalParameters(CommandTape& tape, SyntaxTree::Node node, ExternalScope& externalScope)
{
   int paramCount = 0;

   // save function parameters
   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   SNode current = node.lastChild();
   while (current != lxNone) {
      if (current == lxExtInteranlRef) {
         // HOTFIX : ignore call operation
         SNode ref = current.findSubNode(lxInternalRef);
         loadInternalReference(tape, ref.argument);
         pushObject(tape, lxResult);

         paramCount++;
      }
      else if (current == lxIntExtArgument || current == lxExtArgument) {
         SNode object = current.findSubNodeMask(lxObjectMask);
         SNode value;
         if (object == lxConstantInt)
            value = object;

         if (!isSimpleObject(object, true)) {
            loadObject(tape, lxBlockLocal, (*out_it).offset);

            out_it++;
         }
         else if (current != lxIntExtArgument || value == lxNone) {
            generateObjectExpression(tape, object);
         }

         if (current == lxIntExtArgument) {
            // optimization : use the numeric constant directly
            if (value == lxConstantInt) {
               declareVariable(tape, value.findChild(lxIntValue).argument);
            }
            else if (object.type == lxFieldAddress) {
               if (testany(object.argument, 3)) {
                  // dcopy index
                  // bread
                  // pushe
                  tape.write(bcDCopy, object.argument);
                  tape.write(bcBRead);
                  tape.write(bcPushE);
               }
               else pushObject(tape, lxResultField, object.argument >> 2);
            }
            else pushObject(tape, lxResultField);
         }
         else if (current == lxExtArgument) {
            pushObject(tape, lxResult);
         }
         paramCount++;
      }

      current = current.prevNode();
   }

   return paramCount;
}

void ByteCodeWriter :: generateExternalCall(CommandTape& tape, SNode node)
{
   SNode bpNode = node.findChild(lxBreakpoint);
   if (bpNode != lxNone) {
      translateBreakpoint(tape, bpNode);

      declareBlock(tape);
   }

   bool apiCall = (node == lxCoreAPICall);

   // compile argument list
   ExternalScope externalScope;
   declareExternalBlock(tape);

   generateExternalArguments(tape, node, externalScope);

   // save function parameters
   int paramCount = saveExternalParameters(tape, node, externalScope);

   // call the function
   if (apiCall) {
      // if it is an API call
      // simply release parameters from the stack
      // without setting stack pointer directly - due to optimization
      callCore(tape, node.argument, externalScope.frameSize);

      endExternalBlock(tape, true);
      releaseObject(tape, paramCount);
   }
   else {
      callExternal(tape, node.argument, externalScope.frameSize);

      endExternalBlock(tape);
   }

   if (bpNode != lxNone)
      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
}

ref_t ByteCodeWriter :: generateCall(CommandTape& tape, SNode callNode)
{
   SNode bpNode = callNode.findChild(lxBreakpoint);
   if (bpNode != lxNone) {
      translateBreakpoint(tape, bpNode);

      declareBlock(tape);
   }

   SNode overridden = callNode.findChild(lxOverridden);
   if (overridden != lxNone) {
      generateExpression(tape, overridden);
   }
   else tape.write(bcALoadSI, 0);

   // copym message
   ref_t message = callNode.argument;
   SNode msg = callNode.findChild(lxOvreriddenMessage);
   if (msg != lxNone)
      message = msg.argument;

   tape.write(bcCopyM, message);

   SNode target = callNode.findChild(lxCallTarget);
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

   return message;
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
   bool openArg = false;

   int paramCount = 0;
   int presavedCount = 0;

   // analizing a sub tree
   SNode current = node.firstChild();
   while (current != lxNone) {
      SNode member = current;
      if (current == lxExpression) {
         member = current.firstChild(lxObjectMask);
      }

      if (current == lxArgUnboxing) {
         argUnboxMode = true;
         generateExpression(tape, current);
         unboxArgList(tape);
      }
      else if (test(member.type, lxObjectMask)) {
         if (member.type == lxLocalUnboxing)
            unboxMode = true;

         paramCount++;
      }

      // presave the boxed arguments if required
      if (member == lxUnboxing) {
         generateObjectExpression(tape, member);
         pushObject(tape, lxResult);
         presavedCount++;
         unboxMode = true;
      }
      // presave the nested object if outer operation is required
      else if (member == lxNested && member.existChild(lxOuterMember, lxCode)) {
         generateObjectExpression(tape, member);
         pushObject(tape, lxResult);
         presavedCount++;
         unboxMode = true;
         directMode = false;
      }

      if (member == lxExpression && !isSimpleObjectExpression(member, true)) {
         // ignore nested expression
      }
      else if (test(member.type, lxCodeScopeMask) || member == lxResult)
         directMode = false;

      current = current.nextNode();
   }

   if (!argUnboxMode && isOpenArg(node.argument)) {
      // NOTE : do not add trailing nil for result of unboxing operation
      pushObject(tape, lxNil);
      openArg = true;
   }      

   if (!directMode && (paramCount > 1 || unboxMode)) {
      declareArgumentList(tape, paramCount);
   }
   // if message has no arguments - direct mode is allowed
   else directMode = true;

   size_t counter = countChildren(node);
   size_t index = 0;
   for (size_t i = 0; i < counter; i++) {
      // get parameters in reverse order if required
      current = getChild(node, directMode ? counter - i - 1 : i);
      if (current == lxExpression) {
         current = current.firstChild(lxObjectMask);
      }

      if (current == lxArgUnboxing) {
         // argument list is already unboxed
      }
      else if (test(current.type, lxObjectMask)) {
         if (current == lxUnboxing) {
            SNode tempLocal = current.findChild(lxTempLocal);
            if (tempLocal == lxNone) {
               loadObject(tape, lxCurrent, paramCount + presavedCount - 1);
               presavedCount--;
            }
            else loadObject(tape, lxLocal, tempLocal.argument);
         }
         else if (current == lxNested && current.existChild(lxOuterMember, lxCode)) {
            loadObject(tape, lxCurrent, paramCount + presavedCount - 1);
            presavedCount--;
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
   else if (openArg) {
      // clear open argument list, including trailing nil
      releaseObject(tape, paramCount);
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

      if (current == lxExpression)
         current = current.firstChild(lxObjectMask);

      if (current == lxUnboxing) {
         SNode target = current.firstChild(lxObjectMask);
         SNode tempLocal = current.findChild(lxTempLocal);
         if (tempLocal != lxNone) {
            loadObject(tape, lxLocal, tempLocal.argument);
         }
         else popObject(tape, lxResult);

         if (current.argument != 0) {
            if (target == lxExpression)
               target = target.firstChild(lxObjectMask);

            tape.write(bcPushB);
            if (target == lxAssigning) {
               // unboxing field address
               SNode larg, rarg;
               assignOpArguments(target, larg, rarg);

               target = rarg;
            }

            if (target == lxFieldAddress) {
               if (current.argument == 4) {
                  assignInt(tape, lxFieldAddress, target.argument);
               }
               else if (current.argument == 2) {
                  assignLong(tape, lxFieldAddress, target.argument);
               }
               else assignStruct(tape, lxFieldAddress, target.argument, current.argument);
            }
            else {
               loadBase(tape, target.type, target.argument);
               copyBase(tape, current.argument);
            }

            tape.write(bcPopB);
         }
         else {
            loadObject(tape, lxResultField);
            saveObject(tape, target.type, target.argument);
         }
      }
      else if (current == lxLocalUnboxing) {
         SNode assignNode = current.findChild(lxAssigning);
         SNode larg;
         SNode rarg;

         assignOpArguments(assignNode, larg, rarg);

         tape.write(bcPushB);
         loadObject(tape, larg.type, larg.argument);
         loadBase(tape, rarg.type, 0);

         if (assignNode.argument == 4) {
            assignInt(tape, lxFieldAddress, rarg.argument);
         }
         else if (assignNode.argument == 2) {
            assignLong(tape, lxFieldAddress, rarg.argument);
         }
         else assignStruct(tape, lxFieldAddress, rarg.argument, assignNode.argument);

         tape.write(bcPopB);
      }
      else if (current == lxNested) {
         bool unboxing = false;
         SNode member = current.firstChild();
         while (member != lxNone) {
            if (member == lxOuterMember) {
               unboxing = true;

               SNode target = member.firstChild(lxObjectMask);

               // load outer field
               loadObject(tape, lxCurrent, 0);
               loadObject(tape, lxResultField, member.argument);

               // save to the original variable
               if (target.type == lxBoxing) {
                  SNode localNode = target.firstChild(lxObjectMask);

                  tape.write(bcPushB);
                  loadBase(tape, localNode.type, localNode.argument);
                  if (target.argument != 0) {
                     copyBase(tape, target.argument);
                  }
                  else tape.write(bcCopy);

                  tape.write(bcPopB);
               }
               else saveObject(tape, target.type, target.argument);
            }
            else if (member == lxCode) {
               unboxing = true;

               generateCodeBlock(tape, member);
            }

            member = member.nextNode();
         }
         if (unboxing)
            releaseObject(tape);
      }

      index++;
   }

   assignBaseTo(tape, lxResult);
}

void ByteCodeWriter :: generateReturnExpression(CommandTape& tape, SNode node)
{
   if (translateBreakpoint(tape, node.findSubNode(lxBreakpoint))) {
      declareBlock(tape);
      generateExpression(tape, node);
      declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
   }
   else generateExpression(tape, node);

   gotoEnd(tape, baFirstLabel);
}

void ByteCodeWriter :: generateThrowExpression(CommandTape& tape, SNode node)
{
   generateExpression(tape, node);

   pushObject(tape, lxResult);
   throwCurrent(tape);


   gotoEnd(tape, baFirstLabel);
}

void ByteCodeWriter :: generateBoxing(CommandTape& tape, SNode node)
{
   SNode target = node.findChild(lxTarget);

   if (node == lxArgBoxing) {
      boxArgList(tape, target.argument);
   }
   else if (node.argument == 0) {
      newVariable(tape, target.argument, lxResult);
   }
   else boxObject(tape, node.argument, target.argument, node != lxCondBoxing);

   SNode temp = node.findChild(lxTempLocal);
   if (temp != lxNone) {
      saveObject(tape, lxLocal, temp.argument);
   }
}

void ByteCodeWriter :: generateFieldBoxing(CommandTape& tape, SyntaxTree::Node node, int offset)
{
   SNode target = node.findChild(lxTarget);

   boxField(tape, offset, node.argument, target.argument);
}

void ByteCodeWriter :: generateBoxingExpression(CommandTape& tape, SNode node)
{
   SNode expr = node.firstChild(lxObjectMask);
   if (expr == lxFieldAddress && expr.argument > 0) {
      loadObject(tape, expr);
      generateFieldBoxing(tape, node, expr.argument);
   }
   else {
      generateExpression(tape, node);
      generateBoxing(tape, node);
   }
}

void ByteCodeWriter :: generateAssigningExpression(CommandTape& tape, SyntaxTree::Node node)
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
         else if (child == lxExpression) {
            translateBreakpoint(tape, child.findChild(lxBreakpoint));

            source = child.findSubNodeMask(lxObjectMask);
         }
         else source = child;
      }

      child = child.nextNode();
   }

   if (test(source.type, lxPrimitiveOpMask) && (IsExprOperator(source.argument) || (source.argument == REFER_MESSAGE_ID && source.type != lxArrOp && source.type != lxArgArrOp) ||
      (IsShiftOperator(source.argument) && (source.type == lxIntOp || source.type == lxLongOp))))
   {
      if (target == lxCreatingStruct) {
         generateObjectExpression(tape, target);
         loadBase(tape, lxResult);
      }
      else loadBase(tape, target.type, target.argument);

      generateObjectExpression(tape, source);
   }
   else {
      generateObjectExpression(tape, source);

      if (source == lxExternalCall || source == lxStdExternalCall || source == lxCoreAPICall) {
         if (node.argument == 4) {
            saveInt(tape, target.type, target.argument);
         }
         else if (node.argument == 8) {
            saveReal(tape, target.type, target.argument);
         }
      }
      else if (target == lxFieldExpression || target == lxExpression) {
         SNode arg1, arg2;

         assignOpArguments(target, arg1, arg2);
         loadBase(tape, arg1.type, arg1.argument);
         saveBase(tape, false, lxResult, arg2.argument);
      }
      else if (size != 0) {
         if (source == lxFieldAddress) {
            loadBase(tape, target.type, target.argument);
            if (target == lxFieldAddress) {
               copyStructureField(tape, source.argument, target.argument, size);
            }
            else if (size == 4) {
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
            else assignStruct(tape, target.type, target.argument, size);

            assignBaseTo(tape, lxResult);
         }
      }
      else {
         // if assinging the result of primitive assigning operation
         // it should be boxed before
         if (source == lxAssigning && source.argument > 0) {
            generateBoxing(tape, source);
         }

         saveObject(tape, target.type, target.argument);
      }
   }
}

void ByteCodeWriter :: generateExternFrame(CommandTape& tape, SyntaxTree::Node node)
{
   excludeFrame(tape);

   generateCodeBlock(tape, node);

   includeFrame(tape);
}

void ByteCodeWriter :: generateTrying(CommandTape& tape, SyntaxTree::Node node)
{
   bool first = true;

   declareTry(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (test(current.type, lxObjectMask)) {
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
      if (test(current.type, lxExprMask)) {
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
   declareLoop(tape, true);

   //declareBlock(tape);

   SNode current = node.firstChild();
   bool repeatMode = true;
   while (current != lxNone) {
      if (current == lxElse) {
         jumpIfEqual(tape, current.argument);

         generateCodeBlock(tape, current.findSubNode(lxCode));

         repeatMode = false;
      }
      else if (test(current.type, lxObjectMask)) {
         declareBlock(tape);
         generateObjectExpression(tape, current);
         declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
      }

      current = current.nextNode();
   }

   if (repeatMode)
      jumpIfEqual(tape, 0);

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
      if (current == lxAssigning) {
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
   bool switchBranching = node.argument == -1;

   if (switchBranching) {
      // labels already declared in the case of switch
   }
   else if (node.existChild(lxElse)) {
      declareThenElseBlock(tape);
   }
   else declareThenBlock(tape);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIf) {
         jumpIfNotEqual(tape, current.argument);

         //declareBlock(tape);
         generateCodeBlock(tape, current.findSubNode(lxCode));
      }
      else if (current == lxIfNot) {
         jumpIfEqual(tape, current.argument);

         //declareBlock(tape);
         generateCodeBlock(tape, current.findSubNode(lxCode));
      }
      else if (current == lxElse) {
         declareElseBlock(tape);

         //declareBlock(tape);
         generateCodeBlock(tape, current.findSubNode(lxCode));
      }
      else if (test(current.type, lxObjectMask))
         generateObjectExpression(tape, current);

      current = current.nextNode();
   }

   if(!switchBranching)
      endThenBlock(tape);
}

void ByteCodeWriter :: generateNestedExpression(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = node.findChild(lxTarget);

   // presave all the members which could create new objects
   SNode current = node.lastChild();
   while (current != lxNone) {
      if (current.type == lxMember || current.type == lxOuterMember) {
         if (!isSimpleObjectExpression(current)) {
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
      if (current.type == lxMember || current.type == lxOuterMember) {
         if (!isSimpleObjectExpression(current)) {
            popObject(tape, lxResult);
         }
         else generateExpression(tape, current);

         saveBase(tape, true, lxResult, current.argument);
      }

      current = current.nextNode();
   }

   assignBaseTo(tape, lxResult);
   
   SNode callNode = node.findChild(lxOvreriddenMessage);
   if (callNode != lxNone) {
      // call implicit constructor
      callResolvedMethod(tape, target.argument, callNode.argument);
   }   
}

void ByteCodeWriter :: generateStructExpression(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = node.findChild(lxTarget);

   newStructure(tape, node.argument, target.argument);
}

void ByteCodeWriter :: generateResendingExpression(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = node.findChild(lxTarget);
   if (node.argument == 0) {
      SNode message = node.findChild(lxMessage);
      if (message.argument == encodeMessage(0, DISPATCH_MESSAGE_ID, OPEN_ARG_COUNT)) {
         // if it is open argument dispatching
         pushObject(tape, lxCurrentMessage);
         tape.write(bcOpen, 1);

         unboxMessage(tape);
         callResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument, false);

         closeFrame(tape);
         popObject(tape, lxCurrentMessage);
         tape.write(bcEQuit);
      }
      else {
         pushObject(tape, lxCurrentMessage);
         setSubject(tape, message.argument);
         resendResolvedMethod(tape, target.argument, target.findChild(lxMessage).argument);
      }
   }
   else {
      SNode current = node.firstChild();
      while (current != lxNone) {
         if (current == lxNewFrame) {
            // new frame
            newFrame(tape, 0, 0, false);

            // save message
            pushObject(tape, lxCurrentMessage);

            generateExpression(tape, current);

            // restore message
            popObject(tape, lxCurrentMessage);

            // close frame
            closeFrame(tape);
         }
         else if (current == lxExpression) {
            generateExpression(tape, current);
         }

         current = current.nextNode();
      }

      if (target.argument != 0) {
         resendResolvedMethod(tape, target.argument, node.argument);
      }
      else resend(tape);
   }
}

void ByteCodeWriter :: generateObjectExpression(CommandTape& tape, SNode node)
{
   switch (node.type)
   {
      case lxExpression:
      case lxLocalUnboxing:
      case lxFieldExpression:
      case lxAltExpression:
         generateExpression(tape, node);
         break;
////      case lxTypecasting:
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
      case lxCoreAPICall:
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
      case lxStruct:
         generateStructExpression(tape, node);
         break;
      case lxNested:
         generateNestedExpression(tape, node);
         break;
//      case lxBoolOp:
//         generateBoolOperation(tape, node);
//         break;
      case lxNilOp:
         generateNilOperation(tape, node);
         break;
      case lxIntOp:
      case lxLongOp:
      case lxRealOp:
         generateOperation(tape, node);
         break;
      case lxIntArrOp:
      case lxByteArrOp:
      case lxShortArrOp:
      case lxArrOp:
      case lxBinArrOp:
      case lxArgArrOp:
         generateArrOperation(tape, node);
         break;
      case lxNewOp:
         generateNewOperation(tape, node);
         break;
      case lxResending:
         generateResendingExpression(tape, node);
         break;
      case lxDispatching:
         generateDispatching(tape, node);
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
      case lxCreatingClass:
      case lxCreatingStruct:
         generateCreating(tape, node);
         break;
      //case lxBreakpoint:
      //   translateBreakpoint(tape, node);
      //   break;
      case lxCode:
         generateCodeBlock(tape, node);
         break;
      default:
         loadObject(tape, node);
         break;
   }
}

void ByteCodeWriter :: generateExpression(CommandTape& tape, SNode node)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      //if (current == lxReleasing) {
      //   releaseObject(tape, current.argument);
      //}
      /*else */if (test(current.type, lxObjectMask)) {
         generateObjectExpression(tape, current);
      }
      else generateDebugInfo(tape, current);

      current = current.nextNode();
   }
}

void ByteCodeWriter :: generateBinary(CommandTape& tape, SyntaxTree::Node node, int offset)
{
   loadObject(tape, lxLocalAddress, offset + 2);
   saveIntConstant(tape, 0x800000 + node.argument);
}

void ByteCodeWriter :: generateDebugInfo(CommandTape& tape, SyntaxTree::Node current)
{
   LexicalType type = current.type;
   switch (type)
   {
      case lxVariable:
         declareLocalInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            current.findChild(lxLevel).argument);
         break;
      case lxIntVariable:
         declareLocalIntInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      case lxLongVariable:
         declareLocalLongInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      case lxReal64Variable:
         declareLocalRealInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            current.findChild(lxLevel).argument, /*SyntaxTree::existChild(current, lxFrameAttr)*/false);
         break;
      case lxMessageVariable:
         declareMessageInfo(tape, current.identifier());
         break;
      //case lxParamsVariable:
      //   declareLocalParamsInfo(tape,
      //      current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
      //      current.findChild(lxLevel).argument);
      //   break;
      case lxBytesVariable:
      {
         int level = current.findChild(lxLevel).argument;
         
         generateBinary(tape, current, level);
         declareLocalByteArrayInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            level, false);
         break;
      }
      case lxShortsVariable:
      {
         int level = current.findChild(lxLevel).argument;
         
         generateBinary(tape, current, level);
         declareLocalShortArrayInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            level, false);
         break;
      }
      case lxIntsVariable:
      {
         int level = current.findChild(lxLevel).argument;
         
         generateBinary(tape, current, level);
         
         declareLocalIntArrayInfo(tape,
            current.findChild(lxIdentifier, lxPrivate).identifier(),
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
            current.findChild(lxIdentifier, lxPrivate).identifier(),
            level, current.findChild(lxClassName).identifier());
         break;
      }
   }
}

void ByteCodeWriter :: generateCodeBlock(CommandTape& tape, SyntaxTree::Node node)
{
   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      LexicalType type = current.type;
      switch (type)
      {
         case lxExpression:
            if (translateBreakpoint(tape, current.findChild(lxBreakpoint))) {
               declareBlock(tape);
               generateExpression(tape, current);
               declareBreakpoint(tape, 0, 0, 0, dsVirtualEnd);
            }
            else generateExpression(tape, current);
            break;
         case lxReturning:
            generateObjectExpression(tape, current);
            break;
         case lxExternFrame:
            generateExternFrame(tape, current);
            break;
//         case lxReleasing:
//            releaseObject(tape, current.argument);
//            break;
         case lxBinarySelf:
            declareSelfStructInfo(tape, THIS_VAR, current.argument,
               current.findChild(lxClassName).identifier());
            break;
         case lxBreakpoint:
            translateBreakpoint(tape, current);
            break;
         case lxVariable:
         case lxIntVariable:
         case lxLongVariable:
         case lxReal64Variable:
         case lxMessageVariable:
         case lxParamsVariable:
         case lxBytesVariable:
         case lxShortsVariable:
         case lxIntsVariable:
         case lxBinaryVariable:
            generateDebugInfo(tape, current);
            break;
         default:
            generateObjectExpression(tape, current);
            break;
      }
      current = current.nextNode();
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
      CommandTape::import(*it, scope.sour, scope.dest);
      it++;
   }
}

void ByteCodeWriter :: generateDispatching(CommandTape& tape, SyntaxTree::Node node)
{
   if (node.argument != 0) {
      pushObject(tape, lxCurrentMessage);
      setSubject(tape, node.argument);
      doGenericHandler(tape);
      popObject(tape, lxCurrentMessage);
   }
   else doGenericHandler(tape);

   generateExpression(tape, node);
}

void ByteCodeWriter :: generateCreating(CommandTape& tape, SyntaxTree::Node node)
{
   SNode target = node.findChild(lxTarget);

   int size = node.argument;
   if (node == lxCreatingClass) {
      if (size < 0) {
         loadObject(tape, lxConstantClass, target.argument);
         newDynamicObject(tape);
         initDynamicObject(tape, lxNil);
      }
      else {
         newObject(tape, size, target.argument);
         initObject(tape, size, lxNil);
      }
   }
   else if (node == lxCreatingStruct) {
      if (size < 0) {
         loadObject(tape, lxConstantClass, target.argument);
         switch (size) {
            case -1:
               newDynamicStructure(tape, 1);
               break;
            case -2:
               newDynamicWStructure(tape);
               break;
            case -4:
               newDynamicNStructure(tape);
               break;
            default:
               newDynamicStructure(tape, -size);
               break;
         }
      }
      else newStructure(tape, size, target.argument);
   }
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
               current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
               current.findChild(lxLevel).argument);
            break;
         case lxSelfVariable:
            declareSelfInfo(tape, current.argument);
            break;
         case lxIntVariable:
            declareLocalIntInfo(tape,
               current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
               current.findChild(lxLevel).argument, true);
         case lxLongVariable:
            declareLocalLongInfo(tape,
               current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
               current.findChild(lxLevel).argument, true);
         case lxReal64Variable:
            declareLocalRealInfo(tape,
               current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
               current.findChild(lxLevel).argument, true);
            break;
         case lxParamsVariable:
            declareLocalParamsInfo(tape,
               current.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier(),
               current.findChild(lxLevel).argument);
            break;
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: generateMethod(CommandTape& tape, SyntaxTree::Node node)
{
   int reserved = node.findChild(lxReserved).argument;
   int allocated = node.findChild(lxAllocated).argument;
   int paramCount = node.findChild(lxParamCount).argument;
   ref_t sourcePathRef = node.findChild(lxSourcePath).argument;

   bool withNewFrame = false;
   bool open = false;
   bool exit = false;
   SyntaxTree::Node current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxCalling:
            if (current.argument == -1) {
               if (!open) {
                  open = true;

                  declareMethod(tape, node.argument, sourcePathRef, 0, 0, false, false);
               }
               // HOTFIX: -1 indicates the stack is not consumed by the constructor
               callMethod(tape, 1, -1);
            }
            break;
         case lxImporting:
         case lxCreatingClass:
         case lxCreatingStruct:
            if (!open)
               declareIdleMethod(tape, node.argument, sourcePathRef);

            if (current == lxImporting) {
               importCode(tape, *imports.get(current.argument - 1), true);
            }
            else generateCreating(tape, current);
            break;
         case lxNewFrame:
            withNewFrame = true;
            if (!open) {
               declareMethod(tape, node.argument, sourcePathRef, reserved, allocated, current.argument == -1);
               open = true;
            }
            else newFrame(tape, reserved, allocated, current.argument == -1);

            generateMethodDebugInfo(tape, node);   // HOTFIX : debug info should be declared inside the frame body
            generateCodeBlock(tape, current);
            break;
         case lxDispatching:
            exit = true;
            if (!open)
               declareIdleMethod(tape, node.argument, sourcePathRef);

            generateDispatching(tape, current);
            break;
         default:
            if (test(current.type, lxExprMask)) {
               if (!open) {
                  open = true;

                  declareMethod(tape, node.argument, sourcePathRef, 0, 0, false, false);

                  generateMethodDebugInfo(tape, node);   // HOTFIX : debug info should be declared inside the frame body
                  //if (messageRef != -1)
                  //   declareMessageInfo(tape, messageRef);
               }

               generateObjectExpression(tape, current);
            }
      }

      current = current.nextNode();
   }
   if (!open) {
      if (!exit)
         exitMethod(tape, paramCount, reserved, false);

      endIdleMethod(tape);
   }
   else endMethod(tape, paramCount, reserved, withNewFrame);
}

//void ByteCodeWriter :: generateTemplateMethods(CommandTape& tape, SNode root)
//{
//   SyntaxTree::Node current = root.firstChild();
//   while (current != lxNone) {
//      if (current == lxClassMethod) {
//         generateMethod(tape, current);
//
//         // HOTFIX : compile nested template methods
//         generateTemplateMethods(tape, current);
//      }
//      else if (current == lxIdle) {
//         // HOTFIX : analize nested template methods
//         generateTemplateMethods(tape, current);
//      }
//      else if (current == lxTemplate) {
//         generateTemplateMethods(tape, current);
//      }
//
//      current = current.nextNode();
//   }
//}

void ByteCodeWriter :: generateClass(CommandTape& tape, SNode root)
{
   declareClass(tape, root.argument);
   SyntaxTree::Node current = root.firstChild();
   while (current != lxNone) {
      if (current == lxClassMethod) {
         generateMethod(tape, current);
      }
      current = current.nextNode();
   }

   endClass(tape);
}

void ByteCodeWriter :: generateSymbol(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument)
{
   declareSymbol(tape, reference, (size_t)-1);
   loadObject(tape, type, argument);
   endSymbol(tape);
}

void ByteCodeWriter :: generateInitializer(CommandTape& tape, ref_t reference, LexicalType type, ref_t argument)
{
   declareInitializer(tape, reference);
   loadObject(tape, type, argument);
   endInitializer(tape);
}

void ByteCodeWriter :: generateSymbol(CommandTape& tape, SNode root, bool isStatic)
{
   SNode sourceNode = root.findChild(lxSourcePath);

   if (isStatic) {
      declareStaticSymbol(tape, root.argument, sourceNode.argument);
   }
   else declareSymbol(tape, root.argument, sourceNode.argument);

   generateCodeBlock(tape, root);

   if (isStatic) {
      endStaticSymbol(tape, root.argument);
   }
   else endSymbol(tape);
}

void ByteCodeWriter :: generateConstantList(SNode node, _Module* module, ref_t reference)
{
   SNode target = node.findChild(lxTarget);
   MemoryWriter writer(module->mapSection(reference | mskRDataRef, false));
   SNode current = node.firstChild();
   while (current != lxNone) {
      SNode object = current.findSubNodeMask(lxObjectMask);
      switch (object.type) {
         case lxConstantChar:
         case lxConstantClass:
         case lxConstantInt:
         case lxConstantLong:
         case lxConstantList:
         case lxConstantReal:
         case lxConstantString:
         case lxConstantWideStr:
         case lxConstantSymbol:
            writer.writeRef(object.argument | defineConstantMask(object.type), 0);
            break;
         case lxNil:
            writer.writeDWord(0);
            break;
      }
      current = current.nextNode();
   }

   // add vmt reference
   if (target != lxNone)
      writer.Memory()->addReference(target.argument | mskVMTRef, (pos_t)-4);
}
