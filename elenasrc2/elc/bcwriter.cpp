//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

using namespace _ELENA_;

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

void ByteCodeWriter :: declareModule(_Module* debugModule, const wchar16_t* path)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      _sourceRef = debugStringWriter.Position();

      debugStringWriter.writeWideLiteral(path);
   }
}

void ByteCodeWriter :: declareSymbol(CommandTape& tape, ref_t reference/*, CodeType scope*/)
{  
   // symbol-begin:
   tape.write(blBegin, bsSymbol, reference);
}

void ByteCodeWriter :: declareStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // symbol-begin:

   // accloadr static
   // then procedure-end
   // open

   tape.newLabel();     // declare symbol-end label

   tape.write(blBegin, bsSymbol, staticReference);
   tape.write(bcAccLoadR, staticReference | mskStatSymbolRef);
   tape.write(bcThen, baCurrentLabel);
}

void ByteCodeWriter :: declareClass(CommandTape& tape, ref_t reference)
{
   // class-begin:
	tape.write(blBegin, bsClass, reference);
}

void ByteCodeWriter :: declareMethod(CommandTape& tape, ref_t message, bool withNewFrame)
{
   // method-begin:
   //   init
   //   open
   tape.write(blBegin, bsMethod, message);
   if (withNewFrame) {
      tape.write(bcInit);
      tape.write(bcOpen, 1);
   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: declareGenericAction(CommandTape& tape, ref_t genericMessage, ref_t message)
{
   tape.newLabel();     // declare error
   tape.newLabel();     // declare exit point

   // method-begin:
   //   mccelse labEnd message
   //   init
   //   open

   tape.write(blBegin, bsMethod, genericMessage);
   tape.write(bcMccElse, baFirstLabel, message);
   tape.write(bcInit);
   tape.write(bcOpen, 1);
}

void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
{
   tape.write(bcPushSelf);
   tape.write(blDeclare, bsBranch);
}

void ByteCodeWriter :: exclude(CommandTape& tape)
{
   tape.write(bcExclude);
}

void ByteCodeWriter :: declareLocalInfo(CommandTape& tape, const wchar_t* localName, int level)
{
   tape.write(bdLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalIntInfo(CommandTape& tape, const wchar_t* localName, int level)
{
   tape.write(bdIntLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalLongInfo(CommandTape& tape, const wchar_t* localName, int level)
{
   tape.write(bdLongLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalRealInfo(CommandTape& tape, const wchar_t* localName, int level)
{
   tape.write(bdRealLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareSelfInfo(CommandTape& tape, int level)
{
   tape.write(bdSelf, -2, level);
}

void ByteCodeWriter :: declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType)
{
   tape.write(bcBreakpoint);

   tape.write(bdBreakpoint, stepType, row);
   tape.write(bdBreakcoord, disp, length);
}

void ByteCodeWriter :: declareStatement(CommandTape& tape)
{
   tape.write(blStatement);
}

void ByteCodeWriter :: declareArgumentList(CommandTape& tape, int count)
{
   // reserve n
   tape.write(bcReserve, count);
}

void ByteCodeWriter :: declareVariable(CommandTape& tape, ref_t nilReference)
{
   // pushr nil
   tape.write(bcPushR, nilReference | mskConstantRef);
}

void ByteCodeWriter :: declareVariable(CommandTape& tape)
{
   // pushn 0
   tape.write(bcPushN, 0);
}

int ByteCodeWriter :: declareLabel(CommandTape& tape)
{
   return tape.newLabel();
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
   tape.write(bcCopyFPI, bsBranch);
}

void ByteCodeWriter :: declareTry(CommandTape& tape)
{
   tape.newLabel();                  // declare end-label
   tape.newLabel();                  // declare alternative-label

   // accloadsi
   // hook labAlt

   tape.write(bcAccLoadSI);
   tape.write(bcHook, baCurrentLabel);
}

void ByteCodeWriter :: declareCatch(CommandTape& tape)
{
   //   unhook
   //   jump labEnd
   // labErr:
   //   unhook
   //   pushacc

   tape.write(bcUnhook);
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
   tape.write(bcUnhook);
   tape.write(bcPushAcc);
}

void ByteCodeWriter :: declarePrimitiveCatch(CommandTape& tape)
{
   int labEnd = tape.newLabel();

   // then labEnd
   tape.write(bcThen, labEnd);
}

void ByteCodeWriter :: newSelf(CommandTape& tape)
{
   //   init
   tape.write(bcInit);
}

void ByteCodeWriter :: newFrame(CommandTape& tape)
{
   //   init
   //   open
   tape.write(bcInit);
   tape.write(bcOpen, 1);
}

void ByteCodeWriter :: newStructure(CommandTape& tape, int size, ref_t reference)
{
   // createn size, vmt

   tape.write(bcCreateN, size, reference | mskVMTRef);
}

void ByteCodeWriter :: newObject(CommandTape& tape, int fieldCount, ref_t reference, ref_t nilReference)
{
   // create fieldCount, vmt
   // iaccfillr fieldCount, nil

   tape.write(bcCreate, fieldCount, reference | mskVMTRef);

   switch (fieldCount) {
      case 0:
         break;
      case 1:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         break;
      case 2:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 1, nilReference | mskConstantRef);
         break;
      case 3:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 2, nilReference | mskConstantRef);
         break;
      case 4:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 3, nilReference | mskConstantRef);
         break;
      case 5:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 3, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 4, nilReference | mskConstantRef);
         break;
      case 6:
         tape.write(bcIAccCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 3, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 4, nilReference | mskConstantRef);
         tape.write(bcIAccCopyR, 5, nilReference | mskConstantRef);
         break;
      default:
         tape.write(bcIAccFillR, fieldCount, nilReference | mskConstantRef);
         break;
   }  
}

void ByteCodeWriter :: newDynamicObject(CommandTape& tape, ref_t reference, int sizeOffset)
{
   // accloadsi offs
   // accloadacci 0
   // acccreate size, vmt

   tape.write(bcAccLoadSI, sizeOffset);
   tape.write(bcAccLoadAccI);
   tape.write(bcAccCreate, reference | mskVMTRef);
}

void ByteCodeWriter :: newDynamicStructure(CommandTape& tape, int size, ref_t reference, int sizeOffset, int permanentSize)
{
   // accloadsi offs
   // accloadacci 0
   // accaddn permanentSize
   // acccreaten size, vmt

   tape.write(bcAccLoadSI, sizeOffset);
   tape.write(bcAccLoadAccI);

   if (permanentSize > 0)
      tape.write(bcAccAddN, permanentSize);

   tape.write(bcAccCreateN, size, reference | mskVMTRef);
}

inline ref_t defineConstantMask(ObjectType type)
{
   switch(type) {
      case otClass:
         return mskVMTRef;
      case otLiteral:
         return mskLiteralRef;
      case otInt:
         return mskInt32Ref;
      case otLong:
         return mskInt64Ref;
      case otReal:
         return mskRealRef;
      case otMessage:
         return mskMessage;
      case otSignature:
         return mskSignature;
      case otControl:
      default:
         return mskConstantRef;
   }
}

void ByteCodeWriter :: pushObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okExternal:
         // ignore virtual symbols
         break;
      case okSymbol:
         tape.write(bcEvalR, object.reference | mskSymbolRef);
         tape.write(bcPushAcc);
         break;
      case okConstant:
         // pushr reference
         tape.write(bcPushR, object.reference | defineConstantMask(object.type));
         break;
      case okLocal:
         if (object.type == otParams) {
            // pushfpi n
            tape.write(bcPushFPI, object.reference);
         }
         // pushfi index
         else tape.write(bcPushFI, object.reference);
         break;
      case okBlockLocal:
         // pushbi index
         tape.write(bcPushBI, object.reference);
         tape.write(bcAllocStack, 1);
         break;
      case okCurrent:
         // pushsi index
         tape.write(bcPushSI, object.reference);
         break;
      case okRegister:
         // pushacc
         tape.write(bcPushAcc);
         break;
      case okVSelf:
//      case otVNext:
         // pushfi -(param_count + 1)  ; nagative means relative to the previous stack frame
         tape.write(bcPushFI, object.reference);
         break;
      case okSelf:
      case okSuper:
         // pushself
         tape.write(bcPushSelf);
         break;
      case okField:
      case okOuter:
         // pushselfi offset
         if ((int)object.reference < 0) {
            tape.write(bcPushSelf);
         }
         else tape.write(bcPushSelfI, object.reference);
         break;
      case okOuterField:
         // accloadselfi index
         // loadacc offset
         tape.write(bcAccLoadSelfI, object.reference);
         tape.write(bcPushAccI, object.extraparam);
         break;
      case okCurrentMessage:
         // pushmcc
         tape.write(bcPushMcc);      
         break;
      case okLocalAddress:
         // x_pushfpi n
         tape.write(bcXPushFPI, object.reference);
         break;
   }
}

void ByteCodeWriter :: swapObject(CommandTape& tape, ObjectKind kind, int offset)
{
   if (kind == okRegister) {
      tape.write(bcAccSwapSI, offset);
   }
   else if (kind == okCurrent) {
      tape.write(bcSwapSI, offset);
   }
}

void ByteCodeWriter :: saveObjectLength(CommandTape& tape, int size, int modificator, int offset)
{
   // getlen size
   // accaddn modificator
   // x_accsavefi offset

   tape.write(bcGetLen, size);

   if (modificator != 0)
      tape.write(bcAccAddN, modificator);

   tape.write(bcXAccSaveFI, offset);
}

void ByteCodeWriter :: setObject(CommandTape& tape, int value)
{
   // acccopyn
   tape.write(bcAccCopyN, value);
}

void ByteCodeWriter :: loadObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okSymbol:
         tape.write(bcEvalR, object.reference | mskSymbolRef);
         break;
      case okConstant:
         // acccopyr r
         tape.write(bcAccCopyR, object.reference | defineConstantMask(object.type));
         break;
      case okSelf:
         // acccopyself
         tape.write(bcAccCopySelf);
         break;
      case okCurrent:
         // accloadsi index
         tape.write(bcAccLoadSI, object.reference);
         break;
      case okLocal:
      case okVSelf:
         if (object.type == otParams) {
            // acccopyfpi index
            tape.write(bcAccCopyFPI, object.reference);
         }
         // accloadfi index
         else tape.write(bcAccLoadFI, object.reference);
         break;
      case okRegisterField:
         // accloadacci   
         tape.write(bcAccLoadAccI, object.reference);
         break;
      case okField:
      case okOuter:
         // accloadselfi / acccopyself
         if ((int)object.reference < 0) {
            tape.write(bcAccCopySelf);
         }
         else tape.write(bcAccLoadSelfI, object.reference);
         break;
      case okLocalAddress:
         // x_acccopyfpi n
         tape.write(bcXAccCopyFPI, object.reference);
         break;
   }
}

// light-weight assigning, it is presumed both source and target belongs to the same generation or source is older
void ByteCodeWriter :: assignLocalObject(CommandTape& tape, int offset)
{
   // pop2acci offset
   tape.write(bcXPopAccI, offset);
}

void ByteCodeWriter :: moveLocalObject(CommandTape& tape, int index)
{
   // popsi index
   tape.write(bcPopSI, index);
}

void ByteCodeWriter :: saveObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okLocal:
         // accsavefi index
         tape.write(bcAccSaveFI, object.reference);
         break;
      case okBlockLocal:
         // accsavebi index
         tape.write(bcAccSaveBI, object.reference);
         break;
      case okCurrent:
         // accsavesi index
         tape.write(bcAccSaveSI, object.reference);
         break;
      case okField:
         // accsaveselfi index
         tape.write(bcAccSaveSelfI, object.reference);  
         break;
      case okLocalAddress:
         // pushacc
         // x_acccopyfpi
         // x_popacci 
         tape.write(bcPushAcc);
         tape.write(bcXAccCopyFPI, object.reference);
         tape.write(bcXPopAccI);
         break;
   }
}

void ByteCodeWriter :: boxObject(CommandTape& tape, int size, ref_t vmtReference, bool registerMode)
{
   if (registerMode){
      // accboxn 
      tape.write(bcAccBoxN, size, vmtReference);
   }  
   else {
      // popacc
      // accboxn 
      // pushacc

      tape.write(bcPopAcc);
      tape.write(bcAccBoxN, size, vmtReference);
      tape.write(bcPushAcc);
   }
}

void ByteCodeWriter :: popObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okCurrent:
         // popacc
         tape.write(bcPopAcc);      
         break;
      case okLocal:
         // popfi index
         tape.write(bcPopFI, object.reference);
         break;
      case okField:
         // popselfi index
         tape.write(bcPopSelfI, object.reference);  
         break;
      case okOuterField:
         // accloadselfi index
         // popacci offset
         tape.write(bcAccLoadSelfI, object.reference);
         tape.write(bcPopAccI, object.extraparam);
         break;
      case okCurrentMessage:
         // popmcc
         tape.write(bcPopMcc);      
         break;
   }
}

void ByteCodeWriter :: releaseObject(CommandTape& tape, int count)
{
   // popn n
   if (count == 1) {      
      tape.write(bcPop);      
   }
   else if (count > 1)
      tape.write(bcPopN, count);
}

void ByteCodeWriter :: setMessage(CommandTape& tape, ref_t message)
{
   // mcccopym message
   tape.write(bcMccCopyM, message);
}

void ByteCodeWriter :: sendMessage(CommandTape& tape, int targetOffset, int paramCount)
{
   // accloadsi n
   // callacc 0

   tape.write(bcAccLoadSI, targetOffset);
   tape.write(bcCallAcc);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: sendMessage(CommandTape& tape, int paramCount)
{
   // callacc 0

   tape.write(bcCallAcc);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: sendDirectMessage(CommandTape& tape, int vmtOffset, int paramCount)
{
   // callacc offs

   tape.write(bcCallAcc, vmtOffset);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: sendRoleMessage(CommandTape& tape, ref_t classRef, int paramCount)
{
   // acccopyr ref
   // callacc 0
   tape.write(bcAccCopyR, classRef | mskConstantRef);
   tape.write(bcCallAcc);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: sendRoleMessage(CommandTape& tape, int paramCount)
{
   // callacc 0
   tape.write(bcCallAcc);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: sendSelfMessage(CommandTape& tape, ref_t classRef, ref_t message)
{
   int paramCount = getParamCount(message);

   // acccopyself
   // rcallm ref, m

   tape.write(bcAccCopySelf);
   tape.write(bcRCallM, classRef | mskVMTEntryOffset, message);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: invokeMessage(CommandTape& tape, ref_t reference, ref_t message, int targetOffset)
{
   // accloadsi targetOffset
   // rcallm r, m

   tape.write(bcAccLoadSI, targetOffset);
   tape.write(bcRCallM, reference | mskVMTEntryOffset, message);

//   // we need to place dummy breakpoint to cope with possible branching (due to debugger step-over implementation)
//   declareDummyBreakpoint(tape, dsVirtualEnd);

   tape.write(bcFreeStack, 1 + getParamCount(message));
}

void ByteCodeWriter :: invokeConstantRoleMessage(CommandTape& tape, ref_t classRef, ref_t messageRef)
{
   // acccopyr ref
   // rcallm rref, m
   tape.write(bcAccCopyR, classRef | mskConstantRef);
   tape.write(bcRCallM, classRef | mskVMTRef, messageRef);
   tape.write(bcFreeStack, 1 + getParamCount(messageRef));
}

void ByteCodeWriter :: dispatchVerb(CommandTape& tape, int verb, int dispatcherOffset, int targetOffset)
{
   // mcccopym verb
   // accloadsi targetOffset
   // callsi dispatcherOffset, 1

   tape.write(bcMccCopyM, verb);
   tape.write(bcAccLoadSI, targetOffset);
   tape.write(bcCallSI, dispatcherOffset, 1);
   tape.write(bcFreeStack, 2);
}

void ByteCodeWriter :: extendObject(CommandTape& tape, ObjectInfo info)
{
   // bsredirect
   tape.write(bcBSRedirect);

   switch(info.kind) {
      case okSymbol:
         // pushmcc
         // evalr reference
         // popmcc
         tape.write(bcPushMcc);
         tape.write(bcEvalR, info.reference | mskSymbolRef);
         tape.write(bcPopMcc);
         break;
      case okConstant:
         // acccopyr r
         tape.write(bcAccCopyR, info.reference | defineConstantMask(info.type));
         break;
      case okField:
         // acccloadacci i
         tape.write(bcAccLoadAccI, info.reference);
         break;
   }
   // jumpaccn 0
   tape.write(bcJumpAccN);
}

void ByteCodeWriter :: resend(CommandTape& tape, ObjectInfo info)
{
   switch(info.kind) {
      case okConstant:
         // acccopyr r
         tape.write(bcAccCopyR, info.reference | defineConstantMask(info.type));
         break;
      case okField:
         // acccloadacci i
         tape.write(bcAccLoadAccI, info.reference);
         break;
   }
   // bsredirect
   // throw
   tape.write(bcBSRedirect);
   tape.write(bcThrow);
}

void ByteCodeWriter :: redirectVerb(CommandTape& tape, ref_t message)
{
   // mcccopym
   // jumpaccn
   tape.write(bcMccCopyM, message);
   tape.write(bcJumpAccN);
}

void ByteCodeWriter :: callBack(CommandTape& tape, int sign_id)
{
   // mccaddm subject_id
   // jumpaccn 0

   tape.write(bcMccAddM, sign_id);
   tape.write(bcJumpAccN);
}

void ByteCodeWriter :: callExternal(CommandTape& tape, ref_t functionReference, int paramCount)
{
   // callextr ref, n
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount);
}

void ByteCodeWriter :: callAPI(CommandTape& tape, ref_t reference, bool embedded, int paramCount)
{
   // callr / evalr ref

   tape.write(embedded ? bcEvalR : bcCallR, reference | mskNativeCodeRef);
   tape.write(bcFreeStack, paramCount);
}

void ByteCodeWriter :: compare(CommandTape& tape, ref_t trueRetVal, ref_t falseRetVal)
{
   int labEnd = tape.newLabel();
   int labFalse = tape.newLabel();

   // popacc
   // elsesi labFalse 0
   // acccopyr true
   // jump labEnd
   // labFalse:
   // acccopyr false
   // labEnd:
   // pop

   tape.write(bcPopAcc);
   tape.write(bcElseSI, baCurrentLabel, 0);
   tape.write(bcAccCopyR, trueRetVal | mskConstantRef);
   tape.write(bcJump, labEnd);
   tape.setLabel();
   tape.write(bcAccCopyR, falseRetVal | mskConstantRef);
   tape.setLabel();
   tape.write(bcPop);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef)
{
   // elser then-end, r
   tape.write(bcElseR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ObjectInfo object)
{
   // elser then-end, r
   tape.write(bcElseR, baCurrentLabel, object.reference | defineConstantMask(object.type));
}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef)
{
   // thenr then-end, r
   tape.write(bcThenR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ObjectInfo object)
{
   // thenr then-end, r
   tape.write(bcThenR, baCurrentLabel, object.reference | defineConstantMask(object.type));
}

void ByteCodeWriter :: jump(CommandTape& tape, bool previousLabel)
{
   // jump label
   tape.write(bcJump, previousLabel ? baPreviousLabel : baCurrentLabel);
}

void ByteCodeWriter :: setArrayItem(CommandTape& tape)
{
   // accloadacci
   // set
   // acccopyself
   tape.write(bcAccLoadAccI);
   tape.write(bcSet);
   tape.write(bcAccCopySelf);
}

void ByteCodeWriter :: getArrayItem(CommandTape& tape)
{
   // accloadacci
   // get
   // popacc

   tape.write(bcAccLoadAccI);
   tape.write(bcGet);
   tape.write(bcPopAcc);
}

void ByteCodeWriter :: throwCurrent(CommandTape& tape)
{
   // throw
   tape.write(bcThrow);
}

void ByteCodeWriter :: breakLoop(CommandTape& tape, int label)
{
   // jump break_handler
   tape.write(bcJump, label);
}

void ByteCodeWriter :: gotoEnd(CommandTape& tape, PseudoArg label)
{
   // jump labEnd
   tape.write(bcJump, label);
}

void ByteCodeWriter :: releaseSelf(CommandTape& tape)
{
   // popself
   tape.write(bcPopSelf);      
}

void ByteCodeWriter :: insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   // exclude
   // reserve
   // include

   it--;

   tape.insert(it, ByteCommand(bcExclude));
   tape.insert(it, ByteCommand(bcReserve, size));
   tape.insert(it, ByteCommand(bcInclude));
}

void ByteCodeWriter :: updateStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   while (*it != bcReserve)  {
      it--;
   }

   (*it).argument += size;
}

bool ByteCodeWriter :: checkIfFrameUsed(ByteCodeIterator it)
{
   while (*it != blBegin || ((*it).argument != bsMethod))  {
      switch(*it) {
         case bcPushFI:
         case bcMccCopyPrmFI:
         case bcXPushFPI:
         case bcPushFPI:
         case bcPopFI:
         case bcMccCopyFI:
         case bcIncFI:
         case bcAccLoadFI:
         case bcAccSaveFI:
         case bcXAccSaveFI:
         case bcCopyFPI:
         case bcAccCopyFPI:
         case bcXAccCopyFPI:
         case bcAccGetFI:
         case bcUnhook:
         case bcHook:
            return true;
      }

      it--;
   }
   return false;
}

void ByteCodeWriter :: commentFrame(ByteCodeIterator it)
{
   // make sure the frame is not used in the code
   if (checkIfFrameUsed(it))
      return;

   while (*it != blBegin || ((*it).argument != bsMethod))  {
      // comment operations with stack
      switch(*it) {
         case bcOpen:
         case bcClose:
         case bdSelf:
         case bdLocal:
         case bdIntLocal:
         case bdLongLocal:
         case bdRealLocal:
            (*it).code = bcNop;
      }

      it--;
   }
}

void ByteCodeWriter :: nextCatch(CommandTape& tape)
{
   //   then labEnd
   //   accloadsi 0

   tape.write(bcThen, baCurrentLabel);
   tape.write(bcAccLoadSI);
}

void ByteCodeWriter :: endLabel(CommandTape& tape)
{
   tape.setLabel();   
}

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   //    popacc
   //    throw
   // labEnd
   //    pop

   tape.write(bcPopAcc);
   tape.write(bcThrow);
   tape.setLabel();
   tape.write(bcPop);
}

void ByteCodeWriter :: endPrimitiveCatch(CommandTape& tape)
{
   // labEnd
   tape.setLabel();
}

void ByteCodeWriter :: endThenBlock(CommandTape& tape, bool withStackControl)
{
   // then-end:
   //  copyfpi  branch-level

   tape.setLabel();

   if (withStackControl)
      tape.write(bcCopyFPI, bsBranch);
}

void ByteCodeWriter :: endLoop(CommandTape& tape)
{
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
   tape.releaseLabel();
}

void ByteCodeWriter :: endExternalBlock(CommandTape& tape)
{
   tape.write(bcCopyFPI, bsBranch);
   tape.write(bcPopSelf);
}

void ByteCodeWriter :: exitGenericAction(CommandTape& tape, int count, int reserved)
{
   // labEnd:
   //   close
   //   popself
   //   restore reserved
   //   quitn n
   // labErr:
   //   throw
   // end

   tape.setLabel();
   tape.write(bcClose);
   tape.write(bcPopSelf);

   if (reserved > 0) {
      tape.write(bcRestore, reserved);
   }

   if (count > 0) {
      tape.write(bcQuitN, count);
   }
   else tape.write(bcQuit);
   tape.setLabel();

   tape.write(bcThrow);
}

void ByteCodeWriter :: endGenericAction(CommandTape& tape, int count, int reserved)
{
   exitGenericAction(tape, count, reserved);

   tape.write(blEnd, bsMethod);
}

void ByteCodeWriter :: exitMethod(CommandTape& tape, int count, int reserved, bool withFrame)
{
   // labExit:
   //   close
   //   popself
   //   restore reserved / nop
   //   quitn n / quit
   // end

   tape.setLabel();
   if (withFrame) {
      tape.write(bcClose);
      tape.write(bcPopSelf);
   }

   if (reserved > 0) {
      tape.write(bcRestore, reserved);
   }

   if (count > 0) {
      tape.write(bcQuitN, count);
   }
   else {
      tape.write(bcQuit);
   }
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
   // accsaver static
   // procedure-ending:

   tape.write(bcAccSaveR, staticReference | mskStatSymbolRef);
   tape.setLabel();
   tape.write(blEnd, bsSymbol);
}

void ByteCodeWriter :: writeProcedureDebugInfo(MemoryWriter* debug, ref_t sourceNameRef)
{
   DebugLineInfo symbolInfo(dsProcedure, 0, 0, 0);
   symbolInfo.addresses.source.nameRef = sourceNameRef;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSelfLocal(MemoryWriter* debug, MemoryWriter* debugStrings, int level)
{
   if (!debug)
      return;

   DebugLineInfo info;

   info.symbol = dsBase;
   info.addresses.local.level = level;

   debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeNewStatement(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsStatement, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(MemoryWriter* debug, MemoryWriter* debugStrings, const wchar16_t* localName, int level, int frameLevel)
{
   if (!debug)
      return;

   DebugLineInfo info;

   if (level < 0)
      level -= frameLevel;

   info.symbol = dsLocal;
   info.addresses.local.nameRef = debugStrings->Position();
   info.addresses.local.level = level;


   debugStrings->writeWideLiteral(localName);
   debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(MemoryWriter* debug, MemoryWriter* debugStrings, const wchar16_t* localName, int level, DebugSymbol symbol, int frameLevel)
{
   if (!debug)
      return;

   if (level < 0)
      level -= frameLevel;

   DebugLineInfo info;
   info.symbol = symbol;
   info.addresses.local.nameRef = debugStrings->Position();
   info.addresses.local.level = level;

   debugStrings->writeWideLiteral(localName);

   debug->write((char*)&info, sizeof(DebugLineInfo));
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

void ByteCodeWriter :: writeFieldDebugInfo(ClassInfo::FieldMap& fields, MemoryWriter* writer, MemoryWriter* debugStrings)
{
   ClassInfo::FieldMap::Iterator it = fields.start();
   while (!it.Eof()) {
      if (!emptystr(it.key())) {
         DebugLineInfo symbolInfo(dsField, 0, 0, 0);

         symbolInfo.addresses.symbol.nameRef = debugStrings->Position();
         debugStrings->writeWideLiteral(it.key());

         writer->write((void*)&symbolInfo, sizeof(DebugLineInfo));
      }
      it++;
   }
}

void ByteCodeWriter :: writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings,
                                           const wchar16_t* className, int flags)
{
   // put place holder if debug section is empty
   if (debug->Position() == 0)
   {
      debug->writeDWord(0);
   }

   IdentifierString bookmark(className);
   debugModule->mapPredefinedReference(bookmark, debug->Position());

   ref_t position = debugStrings->Position();

   debugStrings->writeWideLiteral(className);

   DebugLineInfo symbolInfo(dsClass, 0, 0, 0);
   symbolInfo.addresses.symbol.nameRef = position;
   symbolInfo.addresses.symbol.flags = flags;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, const wchar16_t* symbolName)
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

   debugStrings->writeWideLiteral(symbolName);

   DebugLineInfo symbolInfo(dsSymbol, 0, 0, 0);
   symbolInfo.addresses.symbol.nameRef = position;

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(module->mapSection(reference | mskSymbolRef, false));

   // create debug info if debugModule available
   if (debugModule) {
      // initialize debug info writer
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      // save symbol debug line info
      writeSymbolDebugInfo(debugModule, &debugWriter, &debugStringWriter, module->resolveReference(reference & ~mskAnyRef));

      writeProcedure(it, &codeWriter, &debugWriter, &debugStringWriter);

      writeDebugInfoStopper(&debugWriter);
   }
   else writeProcedure(it, &codeWriter);
}

void ByteCodeWriter :: writeDebugInfoStopper(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsEnd, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: flush(CommandTape& tape, _Module* module, _Module* debugModule)
{
   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin) {
         ref_t reference = (*it).additional;
         if ((*it).Argument() == bsClass) {
            writeClass(reference, ++it, module, debugModule);
         }
         else if ((*it).Argument() == bsSymbol) {
            writeSymbol(reference, ++it, module, debugModule);
         }
//         else if ((*it).Argument() == bsHandler) {
//            writeClassHandler(reference, ++it, module, debugModule);
//         }
//         else if ((*it).Argument() == bsAction) {
//            writeAction(reference, ++it, module, debugModule);
//         }
      }
      it++;
   }
}

void ByteCodeWriter :: writeClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule)
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

   vmtWriter.writeDWord(info.classClassRef);                      // vmt class reference

   vmtWriter.write((void*)&info.header, sizeof(ClassHeader));  // header

   // create debug info if debugModule available
   if (debugModule) {
      MemoryWriter debugWriter(debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

     // save class debug info
      writeClassDebugInfo(debugModule, &debugWriter, &debugStringWriter, module->resolveReference(reference & ~mskAnyRef), info.header.flags);
      writeFieldDebugInfo(info.fields, &debugWriter, &debugStringWriter);

      writeVMT(classPosition, &vmtWriter, &codeWriter, it, &debugWriter, &debugStringWriter);

      writeDebugInfoStopper(&debugWriter);
   }
   else writeVMT(classPosition, &vmtWriter, &codeWriter, it, NULL, NULL);
}

void ByteCodeWriter :: writeVMT(size_t classPosition, MemoryWriter* vmtWriter, MemoryWriter* codeWriter, ByteCodeIterator& it, MemoryWriter* debug, MemoryWriter* debugStrings)
{
   while (!it.Eof() && (*it) != blEnd) {
      switch (*it)
      {
         case blBegin:
            // create VMT entry
            if ((*it).Argument() == bsMethod) {
               vmtWriter->writeDWord((*it).additional);                     // Message ID
               vmtWriter->writeDWord(codeWriter->Position());               // Method Address

               writeProcedure(++it, codeWriter, debug, debugStrings);
            }
            break;
      };
      it++;
   }
   // save the real section size
   (*vmtWriter->Memory())[classPosition - 4] = vmtWriter->Position() - classPosition;
}

void ByteCodeWriter :: writeProcedure(ByteCodeIterator& it, MemoryWriter* code, MemoryWriter* debug, MemoryWriter* debugStrings)
{
   if (debug)
      writeProcedureDebugInfo(debug, _sourceRef);

   code->writeDWord(0);                                // write size place holder
   size_t procPosition = code->Position();

   Map<int, int> labels;
   Map<int, int> fwdJumps;
   Stack<int>    stackLevels;                          // scope stack levels

   int level = 1;
   int stackLevel = 0;
   int frameLevel = 0;
   while (!it.Eof() && level > 0) {
      // find out if it is a push / pop command

      int hibyte = *it & 0xFFFFFFF0;
      if (hibyte == 0) {
         if (*it == bcPushAcc || *it == bcPushSelf || *it == bcPushMcc)
            hibyte = bcReserve;
         else if (*it == bcPop || *it == bcPopAcc || *it == bcPopMcc)
            hibyte = bcPopN;
      }

      // calculate stack level
      // if it is a push command
      if(*it == bcAllocStack) {
         stackLevel += (*it).argument;
      }
      else if (hibyte == bcReserve) {
         stackLevel += (*it == bcReserve) ? (*it).argument : 1;
      }
      // if it is a pop command
      else if (hibyte == bcPopN || *it == bcFreeStack || *it == bcPopSelf) {
         stackLevel -= (*it == bcPopN || *it == bcFreeStack) ? (*it).argument : 1;

         // clear previous stack level bookmarks when they are no longer valid
         while (stackLevels.Count() > 0 && stackLevels.peek() > stackLevel)
            stackLevels.pop();
      }

      // save command
      switch (*it) {
//         case blHint:
         case bcFreeStack:
         case bcAllocStack:
         case bcNone:
         case bcNop:
            // nop in command tape is ignored (used in replacement patterns)
            break;
         case blBegin:
            level++;
            break;
         case blLabel:
            fixJumps(code->Memory(), code->Position(), fwdJumps, (*it).argument);
            labels.add((*it).argument, code->Position());

            // JIT compiler interprets nop command as a label mark
            code->writeByte(bcNop);

            break;
         case blDeclare:
            if ((*it).Argument() == bsBranch) {
               stackLevels.push(stackLevel);
            }
            break;
         case blEnd:
            level--;
            break;
         case blStatement:
            // generate debug exception only if debug info enabled
            if (debug)
               writeNewStatement(debug);

            break;
         case bdLocal:
            writeLocal(debug, debugStrings, (const wchar16_t*)(*it).Argument(), (*it).additional, frameLevel);
            break;
         case bdIntLocal:
            writeLocal(debug, debugStrings, (const wchar16_t*)(*it).Argument(), (*it).additional, dsIntLocal, frameLevel);
            break;
         case bdLongLocal:
            writeLocal(debug, debugStrings, (const wchar16_t*)(*it).Argument(), (*it).additional, dsLongLocal, frameLevel);
            break;
         case bdRealLocal:
            writeLocal(debug, debugStrings, (const wchar16_t*)(*it).Argument(), (*it).additional, dsRealLocal, frameLevel);
            break;
         case bdSelf:
            writeSelfLocal(debug, debugStrings, (*it).additional - frameLevel);
            break;
         case bcBreakpoint:
            // generate debug exception only if debug info enabled
            if (debug) {
               (*it).save(code);

               if(peekNext(it) == bdBreakpoint)
                  writeBreakpoint(++it, debug);
            }
            break;
         case bcOpen:
            frameLevel = (*it).argument;
            stackLevel = 0;
            (*it).save(code);
            break;
         case bcPushBI:
         case bcAccLoadBI:
         case bcAccSaveBI:
         case bcPushBPI:
            (*it).save(code, true);
            code->writeDWord(stackLevels.peek() + (*it).argument);
            break;
         case bcCopyFPI:
            (*it).save(code, true);
            if ((*it).argument == bsBranch) {
               stackLevel = stackLevels.peek();
            }
            else stackLevel = (*it).additional;

            code->writeDWord(stackLevel);
            break;
         case bcElseR:
         case bcThenR:
//         case bcElseN:
//         case bcThenN:
         case bcMccElse:
         case bcMccThen:
         case bcElse:
         case bcThen:
         case bcElseSI:
         case bcThenSI:
//         case bcMccElseSI:
//         case bcMccThenSI:
         case bcMccElseAccI:
         case bcMccThenAccI:
//         case bcElseLocal:
         case bcThenFlag:
         case bcElseFlag:
         case bcMccElseAcc:
         case bcMccThenAcc:
         case bcJump:
         case bcHook:
            (*it).save(code, true);
            
            if ((*it).code >= 0xE0)
               code->writeDWord((*it).additional);

            // if forward jump, it should be resolved later
            if (!labels.exist((*it).argument)) {
               fwdJumps.add((*it).argument, code->Position());
               // put jump offset place holder
               code->writeDWord(0);
            }
            // if backward jump
            else code->writeDWord(labels.get((*it).argument) - code->Position() - 4);

            break;
         case bdBreakpoint:
         case bdBreakcoord:
            break; // bdBreakcoord & bdBreakpoint should be ingonored if they are not paired with bcBreakpoint         
         default:
            (*it).save(code);
            break;
      }
      if (level == 0)
         break;
      it++;
   }
   // save the real procedure size
   (*code->Memory())[procPosition - 4] = code->Position() - procPosition;

   // add debug end line info
   if (debug)
      writeDebugInfoStopper(debug);
}

void ByteCodeWriter :: saveOutputResult(CommandTape& tape, int destOffset, int sourOffset)
{
   // pushbi sour
   // accloadbi dest
   // x_popacci

   tape.write(bcPushBI, sourOffset);
   tape.write(bcAccLoadBI, destOffset);
   tape.write(bcXPopAccI);
}

void ByteCodeWriter :: saveIntParam(CommandTape& tape, ObjectInfo param)
{
   switch (param.kind) {
      case okLocal:
         // pushfi n
         tape.write(bcPushFI, param.reference);
         break;
      case okBlockLocal:
         // pushbi n
         tape.write(bcPushBI, param.reference);
         break;
      case okOuterField:
         // accloadbi n
         // pushacci 0

         tape.write(bcAccLoadBI, param.reference);
         tape.write(bcPushAccI);
         break;
      case okLocalAddress:
         // pushfpi n

         tape.write(bcXPushFPI, param.reference);
         break;
      case okBlockLocalAddress:
         // pushbspi n

         tape.write(bcPushBPI, param.reference);
         break;
      case okCurrent:
         // pushspi n

         tape.write(bcPushSPI, param.reference);
   }
}

void ByteCodeWriter :: saveLiteralParam(CommandTape& tape, ObjectInfo param, ref_t functionRef)
{
   if (param.kind == okLocal) {
      // accloadfi n
      // eval save_widestr

      tape.write(bcAccLoadFI, param.reference);
      tape.write(bcEvalR, functionRef | mskNativeCodeRef);
   }
   else if (param.kind == okBlockLocal) {
      // accloadbi n
      // eval save_widestr

      tape.write(bcAccLoadBI, param.reference);
      tape.write(bcEvalR, functionRef | mskNativeCodeRef);
   }
}

void ByteCodeWriter :: loadLiteralParam(CommandTape& tape, ref_t offset, ref_t functionRef)
{
   // accloadbi n
   // eval load_widestr

   tape.write(bcAccLoadBI, offset);
   tape.write(bcEvalR, functionRef | mskNativeCodeRef);
}
