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

ref_t ByteCodeWriter :: writeSourcePath(_Module* debugModule, const tchar_t* path)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      ref_t sourceRef = debugStringWriter.Position();

      debugStringWriter.writeWideLiteral(path);

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
   //   open
   //   pushb
   //   bcopya
   tape.write(blBegin, bsMethod, message);
   if (withNewFrame) {
      tape.write(bcOpen, 1);
      tape.write(bcPushB);
      tape.write(bcBCopyA);
   }
   tape.newLabel();     // declare exit point
}

void ByteCodeWriter :: declareGenericAction(CommandTape& tape, ref_t genericMessage, ref_t message)
{
   tape.newLabel();     // declare error
   tape.newLabel();     // declare exit point

   // method-begin:
   //   melse labEnd message
   //   open
   //   pushb
   //   bcopya

   tape.write(blBegin, bsMethod, genericMessage);
   tape.write(bcMElse, baFirstLabel, message);
   tape.write(bcOpen, 1);
   tape.write(bcPushB);
   tape.write(bcBCopyA);
}

void ByteCodeWriter :: declareExternalBlock(CommandTape& tape)
{
   tape.write(bcPushB);
   tape.write(blDeclare, bsBranch);
}

void ByteCodeWriter :: exclude(CommandTape& tape, int& level)
{
   tape.write(bcExclude);
   level += 2;
}

void ByteCodeWriter :: declareLocalInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalIntInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdIntLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalLongInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdLongLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalRealInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdRealLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalParamsInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdParamsLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareSelfInfo(CommandTape& tape, int level)
{
   tape.write(bdSelf, -2, level);
}

void ByteCodeWriter :: declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType)
{
   // prevent breakpoint duplication
   if (*tape.end() != bdBreakcoord && *tape.end() != bcBreakpoint) {
      tape.write(bcBreakpoint);

      tape.write(bdBreakpoint, stepType, row);
      tape.write(bdBreakcoord, disp, length);
   }
}

void ByteCodeWriter :: declareStatement(CommandTape& tape)
{
   tape.write(blStatement);
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

void ByteCodeWriter :: declareVariable(CommandTape& tape, ref_t nilReference)
{
   // pushr nil
   tape.write(bcPushR, nilReference | mskConstantRef);
}

//void ByteCodeWriter :: declareVariable(CommandTape& tape)
//{
//   // pushn 0
//   tape.write(bcPushN, 0);
//}

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
   //   open 1
   //   pushb
   //   bcopya
   tape.write(bcOpen, 1);
   tape.write(bcPushB);
   tape.write(bcBCopyA);
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
   //   axcopyr r
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
         tape.write(bcDCopy, 0);
         tape.newLabel();
         tape.setLabel(true);
         tape.write(bcAXCopyR, nilReference | mskConstantRef);
         tape.write(bcNext, baCurrentLabel, fieldCount);
         tape.releaseLabel();
         break;
   }
}

void ByteCodeWriter :: newDynamicObject(CommandTape& tape, ref_t reference, int sizeOffset, ref_t nilReference)
{
   //   aloadsi offs
   //   dloadai 0
   //   acopyr vmt
   //   refcreate

   //   delse labEnd
   // labNext:
   //   ddec
   //   axsetr r
   //   dthen labNext
   // labEnd:

   tape.write(bcALoadSI, sizeOffset);
   tape.write(bcDLoadAI);
   tape.write(bcACopyR, reference | mskVMTRef);
   tape.write(bcFunc, fnRefCreate);

   tape.newLabel();
   tape.newLabel();
   tape.write(bcDElse, baPreviousLabel);
   tape.setLabel(true);
   tape.write(bcDDec);
   tape.write(bcAXCopyR, nilReference | mskConstantRef);
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
   tape.write(bcFunc, fnBSCreate);
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
   tape.write(bcFunc, fnWSCreate);
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

//void ByteCodeWriter :: pushObject(ByteCodeIterator bookmark, CommandTape& tape, ObjectInfo object)
//{
//   if (object.kind == okLocal) {
//      tape.insert(bookmark, ByteCommand(bcPushFI, object.reference));
//   }
//}

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
            tape.write(bcPushF, object.reference, bpFrame);
         }
         // pushfi index
         else tape.write(bcPushFI, object.reference, bpFrame);
         break;
      case okBlockLocal:
         // pushfi index
         tape.write(bcPushFI, object.reference, bpBlock);
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
      case okRegisterField:
         // pushai reference
         tape.write(bcPushAI, object.reference);
         break;
      case okVSelf:
//      case otVNext:
         // pushfi -(param_count + 1)  ; nagative means relative to the previous stack frame
         tape.write(bcPushFI, object.reference, bpFrame);
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
         // aloadfi n
         // pushai offset

         tape.write(bcALoadFI, object.reference, bpBlock);
         tape.write(bcPushAI, object.extraparam);
         break;
      case okCurrentMessage:
         // pushmcc
         tape.write(bcPushM);
         break;
      case okLocalAddress:
         // pushf n
         tape.write(bcPushF, object.reference);
         break;
      case okBlockLocalAddress:
         // pushblockpi n
         tape.write(bcPushF, object.reference, bpBlock);
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

////void ByteCodeWriter :: copyPrimitiveValue(CommandTape& tape, int value)
////{
////   // acopyn
////   tape.write(bcACopyN, value);
////}

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
            tape.write(bcACopyF, object.reference, bpFrame);
         }
         // aloadfi index
         else tape.write(bcALoadFI, object.reference, bpFrame);
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
         // acopyf n
         tape.write(bcACopyF, object.reference);
         break;
      case okBlockLocal:
         // aloadfi n
         tape.write(bcALoadFI, object.reference, bpBlock);
         break;
      case okBlockLocalAddress:
         // acopyf n
         tape.write(bcACopyF, object.reference, bpBlock);
         break;
   }
}

//void ByteCodeWriter :: moveLocalObject(CommandTape& tape, int index)
//{
//   // popsi index
//   tape.write(bcPopSI, index);
//}

void ByteCodeWriter :: saveRegister(CommandTape& tape, ObjectInfo object, int fieldOffset)
{
   switch (object.kind) {
      case okLocal:
      case okVSelf:
         // ialoadfi
         tape.write(bcIAXLoadFI, object.reference, fieldOffset, bpFrame);
         break;
      case okCurrent:
         // ailoadsi
         tape.write(bcIAXLoadSI, object.reference, fieldOffset);
         break;
      case okField:
      case okOuter:
         // ailoadbi
         tape.write(bcIAXLoadBI, object.reference, fieldOffset);
         break;
      case okSelf:
         // ailoadb
         tape.write(bcIAXLoadB, fieldOffset);
         break;
   }
}

void ByteCodeWriter :: saveObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      //case okIndex:
      //   // dxcopya
      //   tape.write(bcDXCopyA);
      //   break;
      case okLocal:
         // asavefi index
         tape.write(bcASaveFI, object.reference, bpFrame);
         break;
      case okBlockLocal:
         // accsavefi index
         tape.write(bcASaveFI, object.reference, bpBlock);
         break;
      case okCurrent:
         // asavesi index
         tape.write(bcASaveSI, object.reference);
         break;
      case okField:
//      case okOuter:
         // asavebi index
         tape.write(bcASaveBI, object.reference);
         break;
      case okLocalAddress:
         // pusha
         // acopyf
         // xpopai
         tape.write(bcPushA);
         tape.write(bcACopyF, object.reference);
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
   tape.write(bcFunc, fnRefGetLenZ);
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
   tape.write(bcFunc, fnRefGetLenZ);
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
   // popa

   // ; released

   // labNext:
   // pop
   // ddec
   // dthen labNext

   tape.newLabel();

   tape.write(bcPushA);
   tape.write(bcACopyS, 1);
   tape.write(bcFunc, fnRefGetLenZ);
   tape.write(bcPopA);

   tape.setLabel(true);

   tape.write(bcPop);
   tape.write(bcDDec);
   tape.write(bcDThen, baCurrentLabel);

   tape.releaseLabel();
}

void ByteCodeWriter :: setMessage(CommandTape& tape, ref_t message)
{
   // mcopy message
   tape.write(bcMCopy, message);
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

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message)
{
   // xcallrm r, m

   tape.write(bcXCallRM, reference | mskVMTEntryOffset, message);

   tape.write(bcFreeStack, 1 + getParamCount(message));
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

void ByteCodeWriter :: resend(CommandTape& tape)
{
   // ajumpvi 0
   // throw
   tape.write(bcAJumpVI);
   tape.write(bcThrow);
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
   // callextr ref
   tape.write(bcCallExtR, functionReference | mskImportRef, paramCount);
}

//void ByteCodeWriter :: callAPI(CommandTape& tape, ref_t reference, bool embedded, int paramCount)
//{
//   // callr / evalr ref
//
//   tape.write(embedded ? bcEvalR : bcCallR, reference | mskNativeCodeRef);
//   tape.write(bcFreeStack, paramCount);
//}

void ByteCodeWriter :: executeFunction(CommandTape& tape, ObjectInfo target, FunctionCode code)
{
   tape.write(bcFunc, code);
}

//void ByteCodeWriter :: compare(CommandTape& tape, ref_t trueRetVal, ref_t falseRetVal)
//{
//   int labEnd = tape.newLabel();
//   int labFalse = tape.newLabel();
//
//   // popa
//   // aelsesi labFalse 0
//   // acopyr true
//   // jump labEnd
//   // labFalse:
//   // acopyr false
//   // labEnd:
//   // pop
//
//   tape.write(bcPopA);
//   tape.write(bcAElseSI, baCurrentLabel, 0);
//   tape.write(bcACopyR, trueRetVal | mskConstantRef);
//   tape.write(bcJump, labEnd);
//   tape.setLabel();
//   tape.write(bcACopyR, falseRetVal | mskConstantRef);
//   tape.setLabel();
//   tape.write(bcPop);
//}

void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ref_t comparingRef)
{
   // aelser then-end, r
   tape.write(bcAElseR, baCurrentLabel, comparingRef | mskConstantRef);
}

//void ByteCodeWriter :: jumpIfEqual(CommandTape& tape, ObjectInfo object)
//{
//   // aelser then-end, r
//   tape.write(bcAElseR, baCurrentLabel, object.reference | defineConstantMask(object.type));
//}

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef)
{
   // athenr then-end, r
   tape.write(bcAThenR, baCurrentLabel, comparingRef | mskConstantRef);
}

void ByteCodeWriter :: jumpIfNotEqualN(CommandTape& tape, int value)
{
   // dthenn then-end, value
   tape.write(bcDElseN, baCurrentLabel, value);
}

////void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ObjectInfo object)
////{
////   // thenr then-end, r
////   tape.write(bcThenR, baCurrentLabel, object.reference | defineConstantMask(object.type));
////}

void ByteCodeWriter :: jump(CommandTape& tape, bool previousLabel)
{
   // jump label
   tape.write(bcJump, previousLabel ? baPreviousLabel : baCurrentLabel);
}

////void ByteCodeWriter :: setArrayItem(CommandTape& tape)
////{
////   // accloadacci
////   // set
////   // acccopyself
////   tape.write(bcAccLoadAccI);
////   tape.write(bcSet);
////   tape.write(bcAccCopySelf);
////}

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
   // exclude code should follow open command
   it++;

   // reserve

   tape.insert(it, ByteCommand(bcReserve, size));
}

void ByteCodeWriter :: updateStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   while (*it != bcReserve)  {
      it++;
   }

   (*it).argument += size;
}

bool ByteCodeWriter :: checkIfFrameUsed(ByteCodeIterator it)
{
   while (*it != blBegin || ((*it).argument != bsMethod))  {
      switch(*it) {
         case bcPushFI:
         case bcMSaveParams:
         case bcPushF:
         case bcPopFI:
         case bcMLoadFI:
         case bcDLoadFI:
         case bcDSaveFI:
         case bcALoadFI:
         case bcASaveFI:
         case bcSCopyF:
         case bcACopyF:
         case bcUnhook:
         case bcHook:
            return true;
      }

      it--;
   }
   return false;
}

//bool ByteCodeWriter :: checkIfBaseUsed(ByteCodeIterator it)
//{
//   while (*it != blBegin || ((*it).argument != bsMethod))  {
//      switch(*it) {
//         case bcGet:
//         case bcSet:
//         case bcPushBI:
//         case bcPopBI:
//         case bcASaveBI:
//         case bcIAXLoadB:
//            return true;
//      }
//
//      it--;
//   }
//   return false;
//}

void ByteCodeWriter :: commentFrame(ByteCodeIterator it)
{
   // !! temporally commented

   //// make sure the frame is not used in the code
   //if (checkIfFrameUsed(it))
   //   return;

   //while (*it != blBegin || ((*it).argument != bsMethod))  {
   //   // comment operations with stack
   //   switch(*it) {
   //      case bcOpen:
   //      case bcClose:
   //      case bdSelf:
   //      case bdLocal:
   //      case bdIntLocal:
   //      case bdLongLocal:
   //      case bdRealLocal:
   //      case bdParamsLocal:
   //         (*it).code = bcNop;
   //   }

   //   it--;
   //}
}

//void ByteCodeWriter :: commentBase(ByteCodeIterator it)
//{
//   // make sure the frame is not used in the code
//   if (checkIfBaseUsed(it))
//      return;
//
//   while (*it != blBegin || ((*it).argument != bsMethod))  {
//      // comment operations with stack
//      switch(*it) {
//         case bcPushB:
//         case bcPopB:
//         case bcBCopyA:
//            (*it).code = bcNop;
//      }
//
//      it--;
//   }
//}

void ByteCodeWriter :: nextCatch(CommandTape& tape)
{
   //   athen labEnd
   //   aloadsi 0

   tape.write(bcAThen, baCurrentLabel);
   tape.write(bcALoadSI);
}

void ByteCodeWriter :: setLabel(CommandTape& tape)
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

void ByteCodeWriter :: endExternalBlock(CommandTape& tape, bool safeMode)
{
   if (safeMode)
      tape.write(bcInclude);

   tape.write(bcSCopyF, bsBranch);
   tape.write(bcPopB);
}

void ByteCodeWriter :: exitGenericAction(CommandTape& tape, int count, int reserved)
{
   // labEnd:
   //   bloadfi 1
   //   restore reserved
   //   close
   //   quitn n
   // labErr:
   //   throw
   // end

   tape.setLabel();
   tape.write(bcBLoadFI, 1);
   if (reserved > 0) {
      tape.write(bcRestore, reserved + 2);
   }
   tape.write(bcClose);

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
   //   bloadfi 1
   //   restore reserved / nop
   //   close
   //   quitn n / quit
   // end

   tape.setLabel();
   if (withFrame) {
      tape.write(bcBLoadFI, 1);
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

void ByteCodeWriter :: writeSelfLocal(Scope& scope, int level)
{
   if (!scope.debug)
      return;

   DebugLineInfo info;

   info.symbol = dsBase;
   info.addresses.local.level = level;

   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeNewStatement(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsStatement, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeNewBlock(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsVirtualBlock, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(Scope& scope, const wchar16_t* localName, int level, int frameLevel)
{
   writeLocal(scope, localName, level, dsLocal, frameLevel);
}

void ByteCodeWriter :: writeLocal(Scope& scope, const wchar16_t* localName, int level, DebugSymbol symbol, int frameLevel)
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

   scope.debugStrings->writeWideLiteral(localName);
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

void ByteCodeWriter :: compileSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef)
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

      compileProcedure(it, scope);

      writeDebugInfoStopper(&debugWriter);
   }
   else compileProcedure(it, scope);
}

void ByteCodeWriter :: writeDebugInfoStopper(MemoryWriter* debug)
{
   DebugLineInfo symbolInfo(dsEnd, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: compile(CommandTape& tape, _Module* module, _Module* debugModule, ref_t sourceRef)
{
   ByteCodeIterator it = tape.start();
   while (!it.Eof()) {
      if (*it == blBegin) {
         ref_t reference = (*it).additional;
         if ((*it).Argument() == bsClass) {
            compileClass(reference, ++it, module, debugModule, sourceRef);
         }
         else if ((*it).Argument() == bsSymbol) {
            compileSymbol(reference, ++it, module, debugModule, sourceRef);
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

void ByteCodeWriter :: compileClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef)
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
      writeFieldDebugInfo(info.fields, &debugWriter, &debugStringWriter);

      compileVMT(classPosition, it, scope);

      writeDebugInfoStopper(&debugWriter);
   }
   else compileVMT(classPosition, it, scope);
}

void ByteCodeWriter :: compileVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope)
{
   while (!it.Eof() && (*it) != blEnd) {
      switch (*it)
      {
         case blBegin:
            // create VMT entry
            if ((*it).Argument() == bsMethod) {
               scope.vmt->writeDWord((*it).additional);                     // Message ID
               scope.vmt->writeDWord(scope.code->Position());               // Method Address

               compileProcedure(++it, scope);
            }
            break;
      };
      it++;
   }
   // save the real section size
   (*scope.vmt->Memory())[classPosition - 4] = scope.vmt->Position() - classPosition;
}

void ByteCodeWriter :: compileProcedure(ByteCodeIterator& it, Scope& scope)
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
   bool importMode = false;
   while (!it.Eof() && level > 0) {
      if (!importMode) {
         // find out if it is a push / pop command
         int hibyte = *it & 0xFFFFFFF0;
         if (hibyte == 0) {
            if (*it == bcPushA || *it == bcPushB || *it == bcPushM)
               hibyte = bcPushN;
            else if (*it == bcPop || *it == bcPopA || *it == bcPopM)
               hibyte = bcPopI;
         }

         // calculate stack level
         // if it is a push command
         if(*it == bcAllocStack) {
            stackLevel += (*it).argument;
         }
         else if (hibyte == bcPushN) {
            stackLevel += /*(*it == bcPushN) ? (*it).argument : */1;
         }
         // if it is a pop command
         else if (hibyte == bcPopI || *it == bcFreeStack || *it == bcPopB) {
            stackLevel -= (*it == bcPopI || *it == bcFreeStack) ? (*it).argument : 1;

            // clear previous stack level bookmarks when they are no longer valid
            while (stackLevels.Count() > 0 && stackLevels.peek() > stackLevel)
               stackLevels.pop();
         }
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
            // for import code, turn off stack auto tracking
            if ((*it).argument == bsImport)
               importMode = true;
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
            level--;
            // for import code, turn off stack auto tracking
            if ((*it).argument == bsImport)
               importMode = false;
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
         case bdLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, frameLevel);
            break;
         case bdIntLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsIntLocal, frameLevel);
            break;
         case bdLongLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsLongLocal, frameLevel);
            break;
         case bdRealLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsRealLocal, frameLevel);
            break;
         case bdParamsLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsParamsLocal, frameLevel);
            break;
         case bdSelf:
            writeSelfLocal(scope, (*it).additional - frameLevel);
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
            (*it).save(scope.code, true);
            if ((*it).predicate == bpBlock) {
               scope.code->writeDWord(stackLevels.peek() + (*it).argument);
            }
            else if ((*it).predicate == bpFrame && (*it).argument < 0) {
               scope.code->writeDWord((*it).argument - frameLevel);
            }
            else scope.code->writeDWord((*it).argument);
            break;
         case bcIAXLoadFI:
            (*it).save(scope.code, true);
            if ((*it).predicate == bpBlock) {
               scope.code->writeDWord(stackLevels.peek() + (*it).argument);
            }
            else if ((*it).predicate == bpFrame && (*it).argument < 0) {
               scope.code->writeDWord((*it).argument - frameLevel);
            }
            else scope.code->writeDWord((*it).argument);
            scope.code->writeDWord((*it).additional);
            break;
         case bcSCopyF:
            (*it).save(scope.code, true);
            if ((*it).argument == bsBranch) {
               stackLevel = stackLevels.peek();
            }
            else stackLevel = (*it).additional;

            scope.code->writeDWord(stackLevel);
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
         case bcMElseAI:
         case bcMThenAI:
         case bcTestFlag:
         case bcElseFlag:
         case bcJump:
         case bcHook:
         case bcNext:
            (*it).save(scope.code, true);

            if ((*it).code >= 0xE0)
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

void ByteCodeWriter :: copyInt(CommandTape& tape)
{
   //  nsave
   tape.write(bcFunc, fnNSave);
}

void ByteCodeWriter :: copyLong(CommandTape& tape, ObjectInfo target)
{
   //  lsave
   tape.write(bcFunc, fnLSave);
}

void ByteCodeWriter :: copyIntToLong(CommandTape& tape, ObjectInfo target)
{
   //  lsaveint
   tape.write(bcFunc, fnLSaveInt);
}

void ByteCodeWriter :: saveStr(CommandTape& tape, bool onlyAllocate)
{
   tape.write(bcFunc, onlyAllocate ? fnWSReserve : fnWSSave);
}

//void ByteCodeWriter :: saveDump(CommandTape& tape, bool onlyAllocate)
//{
//   tape.write(bcFunc, onlyAllocate ? fnBSReserve : fnBSSave);
//}

void ByteCodeWriter :: saveActionPtr(CommandTape& tape)
{
   // pushb
   // bcopya
   // loadclass
   // popb
   // pushai 1

   tape.write(bcPushB);
   tape.write(bcBCopyA);
   tape.write(bcFunc, fnLoadClass);
   tape.write(bcPopB);
   tape.write(bcPushAI, 1);
}

void ByteCodeWriter :: setStrLength(CommandTape& tape, ObjectInfo target)
{
   // dloadai
   // aloadblocki
   // wssetlength
   tape.write(bcDLoadAI);

   loadObject(tape, target);

   tape.write(bcFunc, fnWSSetLen);
}

void ByteCodeWriter :: setDumpLength(CommandTape& tape, ObjectInfo target)
{
   // dloadai
   // aloadblocki
   // asetsize
   tape.write(bcDLoadAI);

   loadObject(tape, target);

   tape.write(bcFunc, fnBSSetLen);
}

void ByteCodeWriter :: loadStr(CommandTape& tape)
{
   tape.write(bcFunc, fnWSLoad);
}

//void ByteCodeWriter :: loadDump(CommandTape& tape, ObjectInfo source)
//{
//   loadObject(tape, source);
//   tape.write(bcFunc, fnBSLoad);
//}

void ByteCodeWriter :: loadLiteralLength(CommandTape& tape, ObjectInfo target)
{
   // wsgetlen
   // <load>
   // dxsaveai 0

   tape.write(bcFunc, fnWSGetLen);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: loadByteArrayLength(CommandTape& tape, ObjectInfo target)
{
   // bsgetlen
   // <load>
   // dxsaveai 0

   tape.write(bcFunc, fnBSGetLen);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: loadArrayLength(CommandTape& tape, ObjectInfo target)
{
   // getlen
   // <load>
   // dxsaveai 0

   tape.write(bcGetLen);
   loadObject(tape, target);
   tape.write(bcDSaveAI);
}

void ByteCodeWriter :: loadParamsLength(CommandTape& tape, ObjectInfo target)
{
   // rfgetlenz
   // <load>
   // dxsaveai 0

   tape.write(bcFunc, fnRefGetLenZ);
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

void ByteCodeWriter :: getObjectItem(CommandTape& tape, ObjectInfo target)
{
   if (target.type == otLiteral) {
      // pusha
      // aloadsi 2
      // dloadai 0
      // aloadsi 1
      // wsgetat
      // popa
      tape.write(bcPushA);
      tape.write(bcALoadSI, 2);
      tape.write(bcDLoadAI);
      tape.write(bcALoadSI, 1);
      tape.write(bcFunc, fnWSGetAt);
      tape.write(bcPopA);
   }
   else {
      // aloadsi 1
      // dloadai 0
      // aloadsi 0
      // aloadd
      tape.write(bcALoadSI, 1);
      tape.write(bcDLoadAI);
      tape.write(bcALoadSI);
      tape.write(bcALoadD);
   }
}

void ByteCodeWriter :: setObjectItem(CommandTape& tape, ObjectInfo target)
{
   // aloadsi 1
   // pushb
   // dloadai 0
   // aloadsi 1
   // bcopya
   // aloadsi 3
   // set
   // popb
   tape.write(bcALoadSI, 1);
   tape.write(bcPushB);
   tape.write(bcDLoadAI);
   tape.write(bcALoadSI, 1);
   tape.write(bcBCopyA);
   tape.write(bcALoadSI, 3);
   tape.write(bcSet);
   tape.write(bcPopB);
}
