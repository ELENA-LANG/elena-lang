//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                              (C)2005-2014, by Alexei Rakov
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

   // aloadr static
   // athen procedure-end

   tape.newLabel();     // declare symbol-end label

   tape.write(blBegin, bsSymbol, staticReference);
   tape.write(bcALoadR, staticReference | mskStatSymbolRef);
   tape.write(bcAThen, baCurrentLabel);
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

void ByteCodeWriter :: declareMethod(CommandTape& tape, ref_t message, bool withNewFrame)
{
   // method-begin:
   //   pushb
   //   bcopya
   //   open
   tape.write(blBegin, bsMethod, message);
   if (withNewFrame) {
      tape.write(bcPushB);
      tape.write(bcBCopyA);
      tape.write(bcOpen, 1);
   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: declareGenericAction(CommandTape& tape, ref_t genericMessage, ref_t message)
{
   tape.newLabel();     // declare error
   tape.newLabel();     // declare exit point

   // method-begin:
   //   melse labEnd message
   //   pushb
   //   bcopya
   //   open

   tape.write(blBegin, bsMethod, genericMessage);
   tape.write(bcMElse, baFirstLabel, message);
   tape.write(bcPushB);
   tape.write(bcBCopyA);
   tape.write(bcOpen, 1);
}

void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
{
   tape.write(bcPushB);
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

void ByteCodeWriter :: declarePrimitiveVariable(CommandTape& tape, int value)
{
   // pushn  value
   tape.write(bcPushN, value);
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
   //   unhook
   //   pusha

   tape.write(bcUnhook);
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
   tape.write(bcUnhook);
   tape.write(bcPushA);
}

void ByteCodeWriter :: declarePrimitiveCatch(CommandTape& tape)
{
   int labEnd = tape.newLabel();

   // athen labEnd
   tape.write(bcAThen, labEnd);
}

void ByteCodeWriter :: newSelf(CommandTape& tape)
{
   //   pushb
   //   bcopya
   tape.write(bcPushB);
   tape.write(bcBCopyA);
}

void ByteCodeWriter :: newFrame(CommandTape& tape)
{
   //   pushb
   //   bcopya
   //   open 1
   tape.write(bcPushB);
   tape.write(bcBCopyA);
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

   //   dcopy 0                       |   { iaxcopyr i, r }n
   // labNext:
   //   axsetr r
   //   next labNext fieldCount

   tape.write(bcCreate, fieldCount, reference | mskVMTRef);

   switch (fieldCount) {
      case 0:
         break;
      case 1:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         break;
      case 2:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 1, nilReference | mskConstantRef);
         break;
      case 3:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 2, nilReference | mskConstantRef);
         break;
      case 4:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 3, nilReference | mskConstantRef);
         break;
      case 5:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 3, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 4, nilReference | mskConstantRef);
         break;
      case 6:
         tape.write(bcIAXCopyR, 0, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 1, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 2, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 3, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 4, nilReference | mskConstantRef);
         tape.write(bcIAXCopyR, 5, nilReference | mskConstantRef);
         break;
      default:
         tape.write(bcDCopy, fieldCount);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcAXSetR, nilReference | mskConstantRef);
         tape.write(bcNext, baCurrentLabel, fieldCount);
         break;
   }  
}

void ByteCodeWriter :: newDynamicObject(CommandTape& tape, ref_t reference, int sizeOffset, ref_t nilReference)
{
   //   aloadsi offs
   //   dloadai 0
   //   acopyr vmt
   //   bscreate

   //   delse labEnd
   // labNext:
   //   ddec
   //   axsetr r
   //   dthen labNext
   // labEnd:

   tape.write(bcALoadSI, sizeOffset);
   tape.write(bcDLoadAI);
   tape.write(bcACopyR, reference | mskVMTRef);
   tape.write(bcFunc, fnCreate);

   tape.newLabel();
   tape.newLabel();
   tape.write(bcDElse, baPreviousLabel);
   tape.setLabel(true);
   tape.write(bcDDec);
   tape.write(bcAXSetR, nilReference | mskConstantRef);
   tape.write(bcDThen, baCurrentLabel);
   tape.releaseLabel();
   tape.setLabel();
}

void ByteCodeWriter :: newByteArray(CommandTape& tape, ref_t reference, int sizeOffset)
{
   // aloadsi offs
   // dloadai 0
   // acopyr vmt
   // bscreate

   tape.write(bcALoadSI, sizeOffset);
   tape.write(bcDLoadAI);
   tape.write(bcACopyR, reference | mskVMTRef);
   tape.write(bcBSFunc, fnCreate);
}

void ByteCodeWriter :: newWideLiteral(CommandTape& tape, ref_t reference, int sizeOffset)
{
   // aloadsi offs
   // dloadai 0
   // acopyr vmt
   // wscreate

   tape.write(bcALoadSI, sizeOffset);
   tape.write(bcDLoadAI);
   tape.write(bcACopyR, reference | mskVMTRef);
   tape.write(bcWSFunc, fnCreate);
}

inline ref_t defineConstantMask(ObjectType type)
{
   switch(type) {
      case otClass:
         return mskVMTRef;
      case otLiteral:
         return mskLiteralRef;
      case otInt:
      case otIntVar:
         return mskInt32Ref;
      case otLong:
      case otLongVar:
         return mskInt64Ref;
      case otReal:
      case otRealVar:
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

void ByteCodeWriter :: pushObject(ByteCodeIterator bookmark, CommandTape& tape, ObjectInfo object)
{
   if (object.kind == okLocal) {
      tape.insert(bookmark, ByteCommand(bcPushFI, object.reference));
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
         tape.write(bcPushA);
         break;
      case okConstant:
         // pushr reference
         tape.write(bcPushR, object.reference | defineConstantMask(object.type));
         break;
      case okLocal:
         if (object.type == otParams) {
            // pushf i
            tape.write(bcPushF, object.reference);
         }
         // pushfi index
         else tape.write(bcPushFI, object.reference);
         break;
      case okBlockLocal:
         // pushblocki index
         tape.write(bcPushBlockI, object.reference);
         tape.write(bcAllocStack, 1);
         break;
      case okCurrent:
         // pushsi index
         tape.write(bcPushSI, object.reference);
         break;
      case okRegister:
         // pusha
         tape.write(bcPushA);
         break;
      case okVSelf:
//      case otVNext:
         // pushfi -(param_count + 1)  ; nagative means relative to the previous stack frame
         tape.write(bcPushFI, object.reference);
         break;
      case okSelf:
      case okSuper:
         // pushb
         tape.write(bcPushB);
         break;
      case okField:
      case okOuter:
         // pushbi offset / pushb
         if ((int)object.reference < 0) {
            tape.write(bcPushB);
         }
         else tape.write(bcPushBI, object.reference);
         break;
      case okOuterField:
         // aloadbi index
         // pushai offset
         tape.write(bcALoadBI, object.reference);
         tape.write(bcPushAI, object.extraparam);
         break;
      case okBlockOuterField:
         // accloadblocki n
         // pushai offset

         tape.write(bcALoadBlockI, object.reference);
         tape.write(bcPushAI, object.extraparam);
         break;
      case okCurrentMessage:
         // pushmcc
         tape.write(bcPushM);      
         break;
      case okLocalAddress:
         // xpushf n
         tape.write(bcXPushF, object.reference);
         break;
      case okBlockLocalAddress:
         // pushblockpi n
         tape.write(bcPushBlockPI, object.reference);
         break;
   }
}

void ByteCodeWriter :: swapObject(CommandTape& tape, ObjectKind kind, int offset)
{
   if (kind == okRegister) {
      tape.write(bcASwapSI, offset);
   }
   else if (kind == okCurrent) {
      tape.write(bcSwapSI, offset);
   }
}

//void ByteCodeWriter :: copyPrimitiveValue(CommandTape& tape, int value)
//{
//   // acopyn
//   tape.write(bcACopyN, value);
//}

void ByteCodeWriter :: loadObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okSymbol:
         tape.write(bcEvalR, object.reference | mskSymbolRef);
         break;
      case okConstant:
         // acccopyr r
         tape.write(bcACopyR, object.reference | defineConstantMask(object.type));
         break;
      case okSelf:
         // acopyb
         tape.write(bcACopyB);
         break;
      case okCurrent:
         // aloadsi index
         tape.write(bcALoadSI, object.reference);
         break;
      case okLocal:
      case okVSelf:
         if (object.type == otParams) {
            // acopyf index
            tape.write(bcACopyF, object.reference);
         }
         // aloadfi index
         else tape.write(bcALoadFI, object.reference);
         break;
      case okRegisterField:
         // aloadai   
         tape.write(bcALoadAI, object.reference);
         break;
      case okField:
      case okOuter:
         // aloadbi / acopyb
         if ((int)object.reference < 0) {
            tape.write(bcACopyB);
         }
         else tape.write(bcALoadBI, object.reference);
         break;
      case okOuterField:
         // aloadbi index
         // aloadai index
         tape.write(bcALoadBI, object.reference);
         tape.write(bcALoadAI, object.extraparam);
         break;
      case okLocalAddress:
         // axccopyf n
         tape.write(bcAXCopyF, object.reference);
         break;
      case okBlockLocal:
         // aloadblocki n
         tape.write(bcALoadBlockI, object.reference);
         break;
      case okBlockLocalAddress:
         // aloadblockpi n
         tape.write(bcALoadBlockPI, object.reference);
         break;
   }
}

//void ByteCodeWriter :: moveLocalObject(CommandTape& tape, int index)
//{
//   // popsi index
//   tape.write(bcPopSI, index);
//}

void ByteCodeWriter :: saveObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okLocal:
         // asavefi index
         tape.write(bcASaveFI, object.reference);
         break;
      case okBlockLocal:
         // accsaveblocki index
         tape.write(bcASaveBlockI, object.reference);
         break;
      case okCurrent:
         // asavesi index
         tape.write(bcASaveSI, object.reference);
         break;
      case okField:
         // asavebi index
         tape.write(bcASaveBI, object.reference);  
         break;
      case okLocalAddress:
         // pusha
         // axcopyf
         // xpopai 
         tape.write(bcPushA);
         tape.write(bcAXCopyF, object.reference);
         tape.write(bcXPopAI);
         break;
   }
}

void ByteCodeWriter :: selectObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okCurrent:
         // acopys 0
         // aloadd
         tape.write(bcACopyS);
         tape.write(bcALoadD);
         break;
   }
}

void ByteCodeWriter :: boxObject(CommandTape& tape, int size, ref_t vmtReference, bool registerMode)
{
   // dcopy n
   tape.write(bcDCopy, size);
   if (registerMode){
      // boxn 
      tape.write(bcNBox, vmtReference);
   }  
   else {
      // popa
      // boxn 
      // pusha

      tape.write(bcPopA);
      tape.write(bcNBox, vmtReference);
      tape.write(bcPushA);
   }
}

void ByteCodeWriter :: boxArgList(CommandTape& tape, ref_t vmtReference)
{
   // rfgetlenz
   // box vmt
   tape.write(bcFunc, fnGetLenZ);
   tape.write(bcBox, vmtReference);
}

void ByteCodeWriter :: unboxArgList(CommandTape& tape)
{
   // pushn 0
   // rfgetlenz
   // unbox
   // dinc
   // acopys 0
   // aloadd
   tape.write(bcPushN);
   tape.write(bcFunc, fnGetLenZ);
   tape.write(bcUnbox);
   tape.write(bcDInc);
   tape.write(bcACopyS);
   tape.write(bcALoadD);
}

void ByteCodeWriter :: popObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okRegister:
         // popa
         tape.write(bcPopA);      
         break;
      // used for light-weight field assigning
      // source should be created after target
      case okRegisterField:
         // xpopai
         tape.write(bcXPopAI, object.reference);
         break;
      case okLocal:
         // popfi index
         tape.write(bcPopFI, object.reference);
         break;
      case okField:
         // popbi index
         tape.write(bcPopBI, object.reference);  
         break;
      case okOuterField:
         // aloadbi index
         // popai offset
         tape.write(bcALoadBI, object.reference);
         tape.write(bcPopAI, object.extraparam);
         break;
      case okCurrentMessage:
         // popm
         tape.write(bcPopM);      
         break;
   }
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
   // pusha
   // acopys 1
   // rfgetlenz

   // ; released

   // labNext:
   // pop
   // ddec
   // dthen labNext
   // popa

   tape.newLabel();

   tape.write(bcPushA);
   tape.write(bcACopyS, 1);
   tape.write(bcFunc, fnGetLenZ);

   tape.setLabel(true);

   tape.write(bcPop);
   tape.write(bcDDec);
   tape.write(bcDThen, baCurrentLabel);
   tape.write(bcPopA);

   tape.releaseLabel();
}

void ByteCodeWriter :: setMessage(CommandTape& tape, ref_t message)
{
   // mcopy message
   tape.write(bcMCopy, message);
}

void ByteCodeWriter :: callDispatcher(CommandTape& tape, int targetOffset, int paramCount)
{
   // aloadsi n
   // acallvi 0

   tape.write(bcALoadSI, targetOffset);
   tape.write(bcACallVI);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: typecast(CommandTape& tape, size_t subject_it)
{
   // aloadsi 0
   // mcopy get&subject
   // acallvi 0

   tape.write(bcALoadSI);
   tape.write(bcMCopy, encodeMessage(subject_it, GET_MESSAGE_ID, 0));
   tape.write(bcACallVI, 0);
   tape.write(bcFreeStack, 1);
}

void ByteCodeWriter :: callMethod(CommandTape& tape, int vmtOffset, int paramCount)
{
   // acallvi offs

   tape.write(bcACallVI, vmtOffset);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: callRoleMessage(CommandTape& tape, ref_t classRef, int paramCount)
{
   // acopyr ref
   // acallvi 0
   tape.write(bcACopyR, classRef | mskConstantRef);
   tape.write(bcACallVI);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: callRoleMessage(CommandTape& tape, int paramCount)
{
   // acallvi 0
   tape.write(bcACallVI);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: callResolvedSelfMessage(CommandTape& tape, ref_t classRef, ref_t message)
{
   int paramCount = getParamCount(message);

   // acopyb
   // xcallrm ref, m

   tape.write(bcACopyB);
   tape.write(bcXCallRM, classRef | mskVMTEntryOffset, message);
   tape.write(bcFreeStack, 1 + paramCount);
}

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message, int targetOffset)
{
   // aloadsi targetOffset
   // xcallrm r, m

   tape.write(bcALoadSI, targetOffset);
   tape.write(bcXCallRM, reference | mskVMTEntryOffset, message);

   tape.write(bcFreeStack, 1 + getParamCount(message));
}

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t classRef, ref_t messageRef)
{
   // acopyr ref
   // xcallrm rref, m
   tape.write(bcACopyR, classRef | mskConstantRef);
   tape.write(bcXCallRM, classRef | mskVMTRef, messageRef);
   tape.write(bcFreeStack, 1 + getParamCount(messageRef));
}

void ByteCodeWriter :: dispatchVerb(CommandTape& tape, int verb, int dispatcherOffset, int targetOffset)
{
   // mcopy verb
   // aloadsi targetOffset
   // scallvi dispatcherOffset, 1

   tape.write(bcMCopy, verb);
   tape.write(bcALoadSI, targetOffset);
   tape.write(bcSCallVI, dispatcherOffset, 1);
   tape.write(bcFreeStack, 2);
}

void ByteCodeWriter :: dispatchVerb(CommandTape& tape, int verb, int dispatcherOffset)
{
   // mcopy verb
   // scallvi dispatcherOffset, 1

   tape.write(bcMCopy, verb);
   tape.write(bcSCallVI, dispatcherOffset, 1);
   tape.write(bcFreeStack, 2);
}

void ByteCodeWriter :: extendObject(CommandTape& tape, ObjectInfo info)
{
   // bsredirect
   tape.write(bcBSRedirect);

   switch(info.kind) {
      case okSymbol:
         // pushm
         // evalr reference
         // popm
         tape.write(bcPushM);
         tape.write(bcEvalR, info.reference | mskSymbolRef);
         tape.write(bcPopM);
         break;
      case okConstant:
         // acopyr r
         tape.write(bcACopyR, info.reference | defineConstantMask(info.type));
         break;
      case okField:
         // aloadai i
         tape.write(bcALoadAI, info.reference);
         break;
   }
   // ajumpvi 0
   tape.write(bcAJumpVI);
}

void ByteCodeWriter :: resend(CommandTape& tape, ObjectInfo info)
{
   switch(info.kind) {
      case okConstant:
         // acopyr r
         tape.write(bcACopyR, info.reference | defineConstantMask(info.type));
         break;
      case okField:
         // aloadai i
         tape.write(bcALoadAI, info.reference);
         break;
      case okRegister:
         // asavesi 1
         tape.write(bcASaveSI, 1);
         break;
   }
   // ajumpvi 0
   // throw
   tape.write(bcAJumpVI);
   tape.write(bcThrow);
}

void ByteCodeWriter :: redirectVerb(CommandTape& tape, ref_t message)
{
   // mcopy
   // ajumpvi
   tape.write(bcMCopy, message);
   tape.write(bcAJumpVI);
}

void ByteCodeWriter :: callBack(CommandTape& tape, int sign_id)
{
   // madd subject_id
   // ajumpvi 0

   tape.write(bcMAdd, sign_id);
   tape.write(bcAJumpVI);
}

void ByteCodeWriter :: callExternal(CommandTape& tape, ref_t functionReference, int paramCount)
{
   // callextr ref, n
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount);
}

//void ByteCodeWriter :: callAPI(CommandTape& tape, ref_t reference, bool embedded, int paramCount)
//{
//   // callr / evalr ref
//
//   tape.write(embedded ? bcEvalR : bcCallR, reference | mskNativeCodeRef);
//   tape.write(bcFreeStack, paramCount);
//}

void ByteCodeWriter :: executeFunction(CommandTape& tape, ObjectInfo target, ObjectInfo lparam, FunctionCode code)
{
   if (lparam.kind == okCurrent) {
      // nload / lload / rload / ...
      // pop
      executeFunction(tape, target, fnLoad);
      tape.write(bcPop);
   }

   switch (target.type)
   {
      case otInt:
      case otIntVar:
      case otLength:
      case otIndex:
         tape.write(bcNFunc, code);
         break;
      case otLong:
      case otLongVar:
         tape.write(bcLFunc, code);
         break;
      case otReal:
      case otRealVar:
         tape.write(bcRFunc, code);
         break;
      case otLiteral:
         tape.write(bcWSFunc, code);
         break;
      case otByteArray:
         tape.write(bcBSFunc, code);
         break;
   }
}

void ByteCodeWriter :: executeFunction(CommandTape& tape, ObjectInfo target, FunctionCode code)
{
   switch (target.type)
   {
      case otInt:
      case otIntVar:
      case otLength:
      case otIndex:
         tape.write(bcNFunc, code);
         break;
      case otLong:
      case otLongVar:
         tape.write(bcLFunc, code);
         break;
      case otReal:
      case otRealVar:
         tape.write(bcRFunc, code);
         break;
      case otLiteral:
         tape.write(bcWSFunc, code);
         break;
      case otByteArray:
         tape.write(bcBSFunc, code);
         break;
   }
}

void ByteCodeWriter :: compare(CommandTape& tape, ref_t trueRetVal, ref_t falseRetVal)
{
   int labEnd = tape.newLabel();
   int labFalse = tape.newLabel();

   // popa
   // aelsesi labFalse 0
   // acopyr true
   // jump labEnd
   // labFalse:
   // acopyr false
   // labEnd:
   // pop

   tape.write(bcPopA);
   tape.write(bcAElseSI, baCurrentLabel, 0);
   tape.write(bcACopyR, trueRetVal | mskConstantRef);
   tape.write(bcJump, labEnd);
   tape.setLabel();
   tape.write(bcACopyR, falseRetVal | mskConstantRef);
   tape.setLabel();
   tape.write(bcPop);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef)
{
   // aelser then-end, r
   tape.write(bcAElseR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ObjectInfo object)
{
   // aelser then-end, r
   tape.write(bcAElseR, baCurrentLabel, object.reference | defineConstantMask(object.type));
}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef)
{
   // athenr then-end, r
   tape.write(bcAThenR, baCurrentLabel, comparingRef | mskConstantRef);
}

//void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ObjectInfo object)
//{
//   // thenr then-end, r
//   tape.write(bcThenR, baCurrentLabel, object.reference | defineConstantMask(object.type));
//}

void ByteCodeWriter :: jump(CommandTape& tape, bool previousLabel)
{
   // jump label
   tape.write(bcJump, previousLabel ? baPreviousLabel : baCurrentLabel);
}

//void ByteCodeWriter :: setArrayItem(CommandTape& tape)
//{
//   // accloadacci
//   // set
//   // acccopyself
//   tape.write(bcAccLoadAccI);
//   tape.write(bcSet);
//   tape.write(bcAccCopySelf);
//}

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
   // popb
   tape.write(bcPopB);
}

void ByteCodeWriter :: insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   // exclude
   // reserve
   // include

   it--;
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
         case bcMSaveParams:
         case bcXPushF:
         case bcPushF:
         case bcPopFI:
         case bcMLoadFI:
         case bcDLoadFI:
         case bcDSaveFI:
         case bcALoadFI:
         case bcASaveFI:
         //case bcXAccSaveFI:
         case bcSCopyF:
         case bcACopyF:
         case bcAXCopyF:
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
   //   athen labEnd
   //   aloadsi 0

   tape.write(bcAThen, baCurrentLabel);
   tape.write(bcALoadSI);
}

void ByteCodeWriter :: endLabel(CommandTape& tape)
{
   tape.setLabel();   
}

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   //    popa
   //    throw
   // labEnd

   tape.write(bcPopA);
   tape.write(bcThrow);
   tape.setLabel();
}

void ByteCodeWriter :: endPrimitiveCatch(CommandTape& tape)
{
   // labEnd
   tape.setLabel();
}

void ByteCodeWriter :: endThenBlock(CommandTape& tape, bool withStackControl)
{
   // then-end:
   //  scopyf  branch-level

   tape.setLabel();

   if (withStackControl)
      tape.write(bcSCopyF, bsBranch);
}

void ByteCodeWriter :: endLoop(CommandTape& tape)
{
   tape.write(bcJump, baPreviousLabel);
   tape.setLabel();
   tape.releaseLabel();
}

void ByteCodeWriter :: endExternalBlock(CommandTape& tape)
{
   tape.write(bcSCopyF, bsBranch);
   tape.write(bcPopB);
}

void ByteCodeWriter :: exitGenericAction(CommandTape& tape, int count, int reserved)
{
   // labEnd:
   //   close
   //   popb
   //   restore reserved
   //   quitn n
   // labErr:
   //   throw
   // end

   tape.setLabel();
   tape.write(bcClose);
   tape.write(bcPopB);

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
   //   popb
   //   restore reserved / nop
   //   quitn n / quit
   // end

   tape.setLabel();
   if (withFrame) {
      tape.write(bcClose);
      tape.write(bcPopB);
   }

   if (reserved > 0) {
      tape.write(bcRestore, reserved);
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
   // asaver static
   // procedure-ending:

   tape.write(bcASaveR, staticReference | mskStatSymbolRef);
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
         /*else */if ((*it).Argument() == bsSymbol) {
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
         if (*it == bcPushA || *it == bcPushB || *it == bcPushM)
            hibyte = bcReserve;
         else if (*it == bcPop || *it == bcPopA || *it == bcPopM)
            hibyte = bcPopI;
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
      else if (hibyte == bcPopI || *it == bcFreeStack || *it == bcPopB) {
         stackLevel -= (*it == bcPopI || *it == bcFreeStack) ? (*it).argument : 1;

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
         case bcPushBlockI:
         case bcALoadBlockI:
         case bcASaveBlockI:
         case bcPushBlockPI:
         case bcALoadBlockPI:
            (*it).save(code, true);
            code->writeDWord(stackLevels.peek() + (*it).argument);
            break;
         case bcSCopyF:
            (*it).save(code, true);
            if ((*it).argument == bsBranch) {
               stackLevel = stackLevels.peek();
            }
            else stackLevel = (*it).additional;

            code->writeDWord(stackLevel);
            break;
         case bcAElseR:
         case bcAThenR:
         case bcWSTest:
         case bcBSTest:
         case bcTest:
         case bcDElseN:
         case bcDThenN:
         case bcMElse:
         case bcMThen:
         case bcMElseVerb:
         case bcMThenVerb:
         case bcAElse:
         case bcAThen:
         case bcDElse:
         case bcDThen:
         case bcAElseSI:
         case bcAThenSI:
////         case bcMccElseSI:
////         case bcMccThenSI:
         case bcMElseAI:
         case bcMThenAI:
////         case bcElseLocal:
         case bcTestFlag:
         case bcElseFlag:
//         case bcMccElseAcc:
//         case bcMccThenAcc:
         case bcJump:
         case bcHook:
         case bcNext:
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

void ByteCodeWriter :: copyInt(CommandTape& tape, ObjectInfo sour, ObjectInfo target)
{
   //  <push target>
   //  nsave
   //  popi 1

   loadObject(tape, sour);
   pushObject(tape, target);
   tape.write(bcNFunc, fnSave);
   tape.write(bcPopI, 1);
}

void ByteCodeWriter :: copyInt(CommandTape& tape, ObjectInfo target)
{
   //  <push>
   //  nsave
   //  popi 1

   pushObject(tape, target);
   tape.write(bcNFunc, fnSave);
   tape.write(bcPopI, 1);
}

void ByteCodeWriter :: copyLong(CommandTape& tape, ObjectInfo target)
{
   //  <push>
   //  lsave
   //  popi 1

   pushObject(tape, target);
   tape.write(bcLFunc, fnSave);
   tape.write(bcPopI, 1);
}

void ByteCodeWriter :: saveStr(CommandTape& tape, bool onlyAllocate)
{
   tape.write(bcWSFunc, onlyAllocate ? fnReserve : fnSave);
}

void ByteCodeWriter :: saveDump(CommandTape& tape, bool onlyAllocate)
{
   tape.write(bcBSFunc, onlyAllocate ? fnReserve : fnSave);
}

void ByteCodeWriter :: setStrLength(CommandTape& tape, ObjectInfo target)
{
   // dloadai
   // aloadblocki
   // wssetlength
   tape.write(bcDLoadAI);

   loadObject(tape, target);

   tape.write(bcWSFunc, fnSetLen);
}

void ByteCodeWriter :: setDumpLength(CommandTape& tape, ObjectInfo target)
{
   // dloadai
   // aloadblocki
   // asetsize
   tape.write(bcDLoadAI);

   loadObject(tape, target);

   tape.write(bcBSFunc, fnSetLen);
}

void ByteCodeWriter :: loadStr(CommandTape& tape, ObjectInfo source)
{
   loadObject(tape, source);
   tape.write(bcWSFunc, fnLoad);
}

void ByteCodeWriter :: loadDump(CommandTape& tape, ObjectInfo source)
{
   loadObject(tape, source);
   tape.write(bcBSFunc, fnLoad);
}

void ByteCodeWriter :: loadLiteralLength(CommandTape& tape, ObjectInfo target)
{
   // wsgetlen
   // <load>
   // dxsaveai 0

   tape.write(bcWSFunc, fnGetLen);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: loadByteArrayLength(CommandTape& tape, ObjectInfo target)
{
   // bsgetlen
   // <load>
   // dxsaveai 0

   tape.write(bcBSFunc, fnGetLen);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: loadParamsLength(CommandTape& tape, ObjectInfo target)
{
   // rfgetlenz
   // <load>
   // dxsaveai 0

   tape.write(bcFunc, fnGetLenZ);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: getArrayItem(CommandTape& tape)
{
   // pusha
   // aloadsi 1
   // dloadai 0
   // popa
   // aloadd
   tape.write(bcPushA);
   tape.write(bcALoadSI, 1);
   tape.write(bcDLoadAI);
   tape.write(bcPopA);
   tape.write(bcALoadD);
}

void ByteCodeWriter :: getLiteralItem(CommandTape& tape, ObjectInfo target)
{
   // aswapsi 0
   // dloadai
   // popa
   // push <object>
   // wsgetat

   tape.write(bcASwapSI);
   tape.write(bcDLoadAI);
   tape.write(bcPopA);
   pushObject(tape, target);
   tape.write(bcWSFunc, fnGetAt);
}