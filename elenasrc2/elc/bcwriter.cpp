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

ref_t ByteCodeWriter :: writeMessage(_Module* debugModule, _Module* module, MessageMap& verbs, ref_t message)
{
   if (debugModule != NULL) {
      MemoryWriter debugStringWriter(debugModule->mapSection(DEBUG_STRINGS_ID, false));

      ref_t sourceRef = debugStringWriter.Position();

      ref_t subjectRef;
      int verb, paramCount;
      decodeMessage(message, subjectRef, verb, paramCount);

      IdentifierString name(retrieveKey(verbs.start(), verb, DEFAULT_STR));
      name.append('&');
      name.append(module->resolveSubject(subjectRef));
      name.append('[');
      name.appendInt(paramCount);
      name.append(']');

      debugStringWriter.writeWideLiteral(name);

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
   tape.write(bcElseR, baCurrentLabel, 0);
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
   tape.write(blDeclare, bsBranch);
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

void ByteCodeWriter :: declareLocalByteArrayInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdByteArrayLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalShortArrayInfo(CommandTape& tape, const wchar16_t* localName, int level)
{
   tape.write(bdShortArrayLocal, (ref_t)localName, level);
}

void ByteCodeWriter :: declareLocalParamsInfo(CommandTape& tape, const wchar16_t* localName, int level)
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

void ByteCodeWriter :: declarePrimitiveVariable(CommandTape& tape, int value)
{
   // pushn  value
   tape.write(bcPushN, value);
}

//int ByteCodeWriter :: declareLabel(CommandTape& tape)
//{
//   return tape.newLabel();
//}

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

void ByteCodeWriter :: declarePrimitiveCatch(CommandTape& tape)
{
   int labEnd = tape.newLabel();

   // elsen 0 labEnd
   tape.write(bcElseN, labEnd, 0);
}

void ByteCodeWriter :: newFrame(CommandTape& tape)
{
   //   open 1
   //   pusha
   tape.write(bcOpen, 1);
   tape.write(bcPushA);
}

void ByteCodeWriter :: newDynamicStructure(CommandTape& tape)
{
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
         tape.write(bcBCopyA);
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

inline ref_t defineConstantMask(ObjectKind type)
{
   switch(type) {
      case okConstantClass:
         return mskVMTRef;
      case okLiteralConstant:
         return mskLiteralRef;
      case okIntConstant:
         return mskInt32Ref;
      case okLongConstant:
         return mskInt64Ref;
      case okRealConstant:
         return mskRealRef;
      case okMessageConstant:
         return mskMessage;
      case okSignatureConstant:
         return mskSignature;
      case okVerbConstant:
         return mskVerb;
//      case okSymbolReference:
//         return mskSymbolLoaderRef;
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
         tape.write(bcEvalR, object.param | mskSymbolRef);
         tape.write(bcPushA);
         break;
      case okConstantSymbol:
      case okConstantClass:
      case okConstant:
      case okLiteralConstant:
      case okIntConstant:
      case okLongConstant:
      case okRealConstant:
      case okMessageConstant:
      case okSignatureConstant:
      case okVerbConstant:
//      case okSymbolReference:
         // pushr reference
         tape.write(bcPushR, object.param | defineConstantMask(object.kind));
         break;
      case okParams:
         // pushf i
         tape.write(bcPushF, object.param, bpFrame);
         break;
      case okLocal:
      case okParam:
      case okThisParam:
      case okOutputParam:
         // pushfi index
         tape.write(bcPushFI, object.param, bpFrame);
         break;
      case okSuper:
         // pushfi 1
         tape.write(bcPushFI, 1, bpFrame);
         break;
      case okBlockLocal:
         // pushfi index
         tape.write(bcPushFI, object.param, bpBlock);
         break;
      case okCurrent:
         // pushsi index
         tape.write(bcPushSI, object.param);
         break;
      case okAccumulator:
         // pusha
         tape.write(bcPushA);
         break;
      case okAccField:
         // pushai reference
         tape.write(bcPushAI, object.param);
         break;
      case okField:
      case okOuter:
         // aloadfi 1
         // pushai offset / pusha
         tape.write(bcALoadFI, 1, bpFrame);
         if ((int)object.param < 0) {
            tape.write(bcPushA);
         }
         else tape.write(bcPushAI, object.param);
         break;
      case okOuterField:
         // aloadfi 1
         // aloadai index
         // pushai offset
         tape.write(bcALoadFI, 1, bpFrame);
         tape.write(bcALoadAI, object.param);
         tape.write(bcPushAI, object.extraparam);
         break;
//      case okBlockOuterField:
//         // aloadfi n
//         // pushai offset
//
//         tape.write(bcALoadFI, object.reference, bpBlock);
//         tape.write(bcPushAI, object.extraparam);
//         break;
      case okExtraRegister:
         // pushe
         tape.write(bcPushE);
         break;
      case okIndexAccumulator:
         // pushd
         tape.write(bcPushD);
         break;
      case okLocalAddress:
         // pushf n
         tape.write(bcPushF, object.param);
         break;
//      case okBlockLocalAddress:
//         // pushblockpi n
//         tape.write(bcPushF, object.param, bpBlock);
//         break;
   }
}

void ByteCodeWriter :: loadBase(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okCurrent:
         // bloadsi param
         tape.write(bcBLoadSI, object.param);
         break;
      case okLocalAddress:
         // bcopyf n
         tape.write(bcBCopyF, object.param);
         break;
      case okAccumulator:
         // bcopya
         tape.write(bcBCopyA);
         break;
   }
}

void ByteCodeWriter :: loadObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okSymbol:
         tape.write(bcEvalR, object.param | mskSymbolRef);
         break;
      case okConstantSymbol:
      case okConstantClass:
      case okConstant:
      case okLiteralConstant:
      case okIntConstant:
      case okLongConstant:
      case okRealConstant:
      case okMessageConstant:
      case okSignatureConstant:
      case okVerbConstant:
//      case okSymbolReference:
         // acccopyr r
         tape.write(bcACopyR, object.param | defineConstantMask(object.kind));
         break;
      case okCurrent:
         // aloadsi index
         tape.write(bcALoadSI, object.param);
         break;
      case okParams:
         // acopyf index
         tape.write(bcACopyF, object.param, bpFrame);
         break;
      case okLocal:
      case okParam:
      case okThisParam:
      case okOutputParam:
         // aloadfi index
         tape.write(bcALoadFI, object.param, bpFrame);
         break;
      case okAccField:
         // aloadai
         tape.write(bcALoadAI, object.param);
         break;
      case okField:
      case okOuter:
         // bloadfi 1
         // aloadbi / acopyb
         tape.write(bcBLoadFI, 1, bpFrame);
         if ((int)object.param < 0) {
            tape.write(bcACopyB);
         }
         else tape.write(bcALoadBI, object.param);
         break;
      case okOuterField:
         // aloadfi 1
         // aloadai param
         // aloadai extra
         tape.write(bcALoadFI, 1, bpFrame);
         tape.write(bcALoadAI, object.param);
         tape.write(bcALoadAI, object.extraparam);
         break;
      case okLocalAddress:
         // acopyf n
         tape.write(bcACopyF, object.param);
         break;
      case okBlockLocal:
         // aloadfi n
         tape.write(bcALoadFI, object.param, bpBlock);
         break;
//      case okBlockLocalAddress:
//         // acopyf n
//         tape.write(bcACopyF, object.param, bpBlock);
//         break;
      case okBase:
         // acopyb
         tape.write(bcACopyB);
         break;
   }
}

void ByteCodeWriter :: saveBase(CommandTape& tape, ObjectInfo object, int fieldOffset)
{
   switch (object.kind) {
      case okLocal:
      case okParam:
      case okThisParam:
         // aloadfi 1
         // axsavebi
         tape.write(bcALoadFI, object.param, bpFrame);
         tape.write(bcAXSaveBI, fieldOffset);
         break;
      case okCurrent:
         // aloadsi
         // axsavebi
         tape.write(bcALoadSI, object.param);
         tape.write(bcAXSaveBI, fieldOffset);
         break;
      case okAccumulator:
         // axsavebi
         tape.write(bcAXSaveBI, fieldOffset);
         break;
      case okField:
      case okOuter:
         // aloadfi 1
         // aloadai
         // axsavebi
         tape.write(bcALoadFI, 1, bpFrame);
         tape.write(bcALoadAI, object.param);
         tape.write(bcAXSaveBI, fieldOffset);
         break;
   }
}

void ByteCodeWriter :: saveObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okLocal:
      case okThisParam:
         // asavefi index
         tape.write(bcASaveFI, object.param, bpFrame);
         break;
      case okBlockLocal:
         // asavefi index
         tape.write(bcASaveFI, object.param, bpBlock);
         break;
      case okCurrent:
         // asavesi index
         tape.write(bcASaveSI, object.param);
         break;
      case okOutputParam:
         // bloadfi index
         // axsavebi 0
         tape.write(bcBLoadFI, object.param, bpFrame);
         tape.write(bcAXSaveBI);
         break;
      case okField:
      case okOuter:
         // bloadfi 1
         // asavebi index
         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcASaveBI, object.param);
         break;
      case okOuterField:
         // bcopya
         // aloadfi 1
         // aloadai param
         // bswap
         // asavebi extra
         tape.write(bcBCopyA);
         tape.write(bcALoadFI, 1, bpFrame);
         tape.write(bcALoadAI, object.param);
         tape.write(bcBSwap);
         tape.write(bcASaveBI, object.extraparam);
         break;
   }
}

void ByteCodeWriter :: boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing)
{
   // ifheap labSkip
   // bcopya
   // newn vmt, size
   // bswap
   // copy
   // acopyb
   // labSkip:

   if (!alwaysBoxing) {
      tape.newLabel();
      tape.write(bcIfHeap, baCurrentLabel);
   }

   tape.write(bcBCopyA);
   tape.write(bcNewN, vmtReference | mskVMTRef, size);
   tape.write(bcBSwap);

   if (size >0 && size <= 4) {
      tape.write(bcNLoad);
      tape.write(bcNSave);
   }
   else tape.write(bcCopy);

   tape.write(bcACopyB);

   if (!alwaysBoxing)
      tape.setLabel();
}

void ByteCodeWriter :: boxArgList(CommandTape& tape, ref_t vmtReference)
{
   // dcopy 0
   // bcopya
   // ecopyd
   // xseek
   // acopyr vmt
   // create
   // bswap
   // xclone
   // acopyb

   tape.write(bcDCopy, 0);
   tape.write(bcECopyD);
   tape.write(bcXSeek);
   tape.write(bcBCopyA);
   tape.write(bcACopyR, vmtReference);
   tape.write(bcCreate);
   tape.write(bcBSwap);
   tape.write(bcXClone);
   tape.write(bcACopyB);
}

void ByteCodeWriter :: unboxArgList(CommandTape& tape)
{
   // dcopy 0
   // bcopya
   // ecopyd
   // xseek
   // ecopyd
   // pushn 0

   // labNext:
   // dec
   // get
   // pusha
   // elsen labNext 0

   // dcopye
   // acopys 0
   // inc
   // aload

   tape.write(bcDCopy, 0);
   tape.write(bcBCopyA);
   tape.write(bcECopyD);
   tape.write(bcXSeek);
   tape.write(bcECopyD);
   tape.write(bcPushN, 0);

   tape.newLabel();
   tape.setLabel(true);
   tape.write(bcDec);
   tape.write(bcGet);
   tape.write(bcPushA);
   tape.write(bcElseN, baCurrentLabel, 0);
   tape.releaseLabel();

   tape.write(bcDCopyE);
   tape.write(bcACopyS);
   tape.write(bcInc);
   tape.write(bcALoad);
}

void ByteCodeWriter :: popObject(CommandTape& tape, ObjectInfo object)
{
   switch (object.kind) {
      case okAccumulator:
         // popa
         tape.write(bcPopA);
         break;
      case okBase:
         // popb
         tape.write(bcPopB);
         break;
      case okExtraRegister:
         // pope
         tape.write(bcPopE);
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
   // bcopya
   // acopys 1
   // bswap
   // dcopy 0
   // ecopyd
   // xseek

   // ; released

   // labNext:
   // pop
   // dec
   // elsen 0 labNext

   tape.newLabel();

   tape.write(bcBCopyA);
   tape.write(bcACopyS, 1);
   tape.write(bcBSwap);
   tape.write(bcDCopy, 0);
   tape.write(bcECopyD);
   tape.write(bcXSeek);

   tape.setLabel(true);

   tape.write(bcPop);
   tape.write(bcDec);
   tape.write(bcElseN, baCurrentLabel, 0);

   tape.releaseLabel();
}

void ByteCodeWriter :: setMessage(CommandTape& tape, ref_t message)
{
   // copym message
   tape.write(bcCopyM, message);
}

void ByteCodeWriter :: setSubject(CommandTape& tape, ref_t subject)
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

void ByteCodeWriter :: callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message)
{
   // xcallrm r, m

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

void ByteCodeWriter :: typecast(CommandTape& tape)
{
   //  setverb  GET_MESSAGE_ID
   //  pusha
   //  acallvi  0

   tape.write(bcSetVerb, encodeVerb(GET_MESSAGE_ID));
   tape.write(bcPushA);
   tape.write(bcACallVI, 0);
   tape.write(bcFreeStack, 1);
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

void ByteCodeWriter :: resend(CommandTape& tape, ObjectInfo object, int dispatchIndex)
{
   switch (object.kind) {
      case okSymbol:
         tape.write(bcPushE);
         tape.write(bcEvalR, object.param | mskSymbolRef);
         tape.write(bcPopE);
         break;
      case okConstantSymbol:
      case okConstantClass:
      case okConstant:
      case okLiteralConstant:
      case okIntConstant:
      case okLongConstant:
      case okRealConstant:
      case okMessageConstant:
      case okSignatureConstant:
      case okVerbConstant:
//      case okSymbolReference:
         // acccopyr r
         tape.write(bcACopyR, object.param | defineConstantMask(object.kind));
         break;
      case okField:
         // bcopya
         // aloadbi
         tape.write(bcBCopyA);
         tape.write(bcALoadBI, object.param);
         break;
   }

   // ajumpvi 0
   tape.write(bcAJumpVI, dispatchIndex);
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

void ByteCodeWriter :: jumpIfNotEqual(CommandTape& tape, ref_t comparingRef)
{
   // elser then-end, r
   tape.write(bcElseR, baCurrentLabel, comparingRef | mskConstantRef);
}

//void ByteCodeWriter :: jumpIfNotEqualN(CommandTape& tape, int value)
//{
//   //// dthenn then-end, value
//   //tape.write(bcDElseN, baCurrentLabel, value);
//}

void ByteCodeWriter :: jump(CommandTape& tape, bool previousLabel)
{
   // jump label
   tape.write(bcJump, previousLabel ? baPreviousLabel : baCurrentLabel);
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

void ByteCodeWriter :: updateStackAlloc(ByteCodeIterator it, CommandTape& tape, int size)
{
   while (*it != bcReserve)  {
      it++;
   }

   (*it).argument += size;
}

//void ByteCodeWriter :: setLabel(CommandTape& tape)
//{
//   tape.setLabel();
//}

void ByteCodeWriter :: endCatch(CommandTape& tape)
{
   // labEnd

   tape.setLabel();
}

void ByteCodeWriter :: endAlt(CommandTape& tape)
{
   // labEnd
   // pop

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

void ByteCodeWriter :: endLoop(CommandTape& tape, ref_t comparingRef)
{
   tape.write(bcIfR, baPreviousLabel, comparingRef | mskConstantRef);

   tape.setLabel();
   tape.releaseLabel();
}

void ByteCodeWriter :: endExternalBlock(CommandTape& tape)
{
   tape.write(bcSCopyF, bsBranch);
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

void ByteCodeWriter :: exitStaticSymbol(CommandTape& tape, ref_t staticReference)
{
   // asaver static

   tape.write(bcASaveR, staticReference | mskStatSymbolRef);
   tape.setLabel();
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
   DebugLineInfo symbolInfo(dsVirtualBlock, 0, 0, 0);

   debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: writeLocal(Scope& scope, const wchar16_t* localName, int level, int frameLevel)
{
   writeLocal(scope, localName, level, dsLocal, frameLevel);
}

void ByteCodeWriter :: writeSelf(Scope& scope, int level, int frameLevel)
{
   if (!scope.debug)
      return;

   DebugLineInfo info;
   info.symbol = dsLocal;
   info.addresses.local.nameRef = scope.debugStrings->Position();

   if (level < 0) {
      scope.debugStrings->writeWideLiteral(SELF_VAR);

      level -= frameLevel;
   }
   else scope.debugStrings->writeWideLiteral(THIS_VAR);

   info.addresses.local.level = level;

   scope.debug->write((char*)&info, sizeof(DebugLineInfo));
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
   while (!it.Eof() && level > 0) {
      // calculate stack level
      if(*it == bcAllocStack) {
         stackLevel += (*it).argument;
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
         case bdSelf:
            writeSelf(scope, (*it).additional, frameLevel);
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
         case bdByteArrayLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsByteArrayLocal, frameLevel);
            break;
         case bdShortArrayLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsShortArrayLocal, frameLevel);
            break;
         case bdParamsLocal:
            writeLocal(scope, (const wchar16_t*)(*it).Argument(), (*it).additional, dsParamsLocal, frameLevel);
            break;
         case bdMessage:
            writeMessageInfo(scope, dsMessage, (*it).additional);
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

void ByteCodeWriter :: saveInt(CommandTape& tape, ObjectInfo target)
{
   if (target.kind == okLocalAddress) {
      // bcopyf param
      // nsave
      tape.write(bcBCopyF, target.param);
      tape.write(bcNSave);
   }
   else if (target.kind == okLocal || target.kind == okOutputParam) {
      // bloadfi param
      // nsave
      tape.write(bcBLoadFI, target.param, bpFrame);
      tape.write(bcNSave);
   }
   else if (target.kind == okFieldAddress) {
      // ecopyd
      // bloadfi 1
      // dcopy target.param
      // bwrite
      tape.write(bcECopyD);
      tape.write(bcBLoadFI, 1, bpFrame);
      tape.write(bcDCopy, target.param);
      tape.write(bcBWrite);
   }   
}

void ByteCodeWriter :: assignInt(CommandTape& tape, ObjectInfo target)
{
   if (target.kind == okFieldAddress) {

      if (target.param == 0) {
         // bloadfi 1
         // ncopy

         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcNCopy);
      }
      // if it is aligned
      else if (align(target.param, 4) == target.param) {
         // bloadfi 1
         // dcopy target.param / 4
         // ecopy 2
         // ninsert

         tape.write(bcBLoadFI, 1, bpFrame);
         tape.write(bcDCopy, target.param >> 2);
         tape.write(bcECopy, 1);
         tape.write(bcNInsert);
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
         tape.write(bcDCopy, target.param);
         tape.write(bcBWrite);
      }
   }
   else if (target.kind == okLocal || target.kind == okOutputParam) {
      // bloadfi param
      // ncopy
      tape.write(bcBLoadFI, target.param, bpFrame);
      tape.write(bcNCopy);
   }
   else if (target.kind == okBase) {
      // ncopy
      tape.write(bcNCopy);
   }
}

void ByteCodeWriter :: assignShort(CommandTape& tape, ObjectInfo target)
{
   if (target.kind == okFieldAddress) {
      // nload
      // ecopyd
      // bloadfi 1
      // dcopy target.param / 2
      // wwrite

      tape.write(bcNLoad);
      tape.write(bcECopyD);
      tape.write(bcBLoadFI, 1, bpFrame);
      tape.write(bcDCopy, target.param >> 1);
      tape.write(bcWWrite);
   }
   else if (target.kind == okLocal || target.kind == okOutputParam) {
      // bloadfi param
      // ncopy
      tape.write(bcBLoadFI, target.param, bpFrame);
      tape.write(bcNCopy);
   }
   else if (target.kind == okBase) {
      // ncopy
      tape.write(bcNCopy);
   }
}

void ByteCodeWriter :: assignLong(CommandTape& tape, ObjectInfo target)
{
   if (target.kind == okFieldAddress) {
      // bloadfi 1
      tape.write(bcBLoadFI, 1, bpFrame);

      if (target.param == 0) {
         // lcopy

         tape.write(bcLCopy);
      }
      // if it is aligned
      else if (align(target.param, 4) == target.param) {
         // dcopy target.param / 4
         // ecopy 2
         // ninsert

         tape.write(bcDCopy, target.param >> 2);
         tape.write(bcECopy, 2);
         tape.write(bcNInsert);
      }
      else {
         // dcopy target.param
         // ecopy 8
         // insert

         tape.write(bcDCopy, target.param);
         tape.write(bcECopy, 8);
         tape.write(bcInsert);
      }
   }
   else if (target.kind == okLocal || target.kind == okOutputParam) {
      // bloadfi param
      // lcopy

      tape.write(bcBLoadFI, target.param, bpFrame);
      tape.write(bcLCopy);
   }
   else if (target.kind == okBase) {
      // lcopy
      tape.write(bcLCopy);
   }
}

void ByteCodeWriter :: copyStructure(CommandTape& tape, int offset, int size)
{
   // if it is alinged
   if ((offset & 3) == 0 && (size & 3) == 3) {
      // dcopy index / 4
      // ecopy count / 4
      // nsubcopy

      tape.write(bcDCopy, offset >> 2);
      tape.write(bcECopy, size >> 2);
      tape.write(bcNSubCopy);
   }
   else {
      // dcopy index
      // ecopy count
      // subcopy

      tape.write(bcDCopy, offset);
      tape.write(bcECopy, size);
      tape.write(bcSubCopy);
   }
}

void ByteCodeWriter :: copyInt(CommandTape& tape, int offset)
{
   // dcopy index
   // bread
   // dcopye
   // nsave
   // acopyb

   tape.write(bcDCopy, offset);
   tape.write(bcBRead);
   tape.write(bcDCopyE);
   tape.write(bcNSave);
   tape.write(bcACopyB);
}

void ByteCodeWriter :: copyShort(CommandTape& tape, int offset)
{
   if ((offset & 1) == 0) {
      // dcopy index / 2
      // wread
      // dcopye
      // nsave

      tape.write(bcDCopy, offset >> 1);
      tape.write(bcWRead);
      tape.write(bcDCopyE);
      tape.write(bcNSave);
   }
   else {
      // dcopy index
      // bread
      // dcopye
      // andn 0FFFFh
      // nsave
      tape.write(bcDCopy, offset);
      tape.write(bcBRead);
      tape.write(bcDCopyE);
      tape.write(bcAndN, 0xFFFF);
      tape.write(bcNSave);
   }
   // acopyb
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

void ByteCodeWriter :: copySubject(CommandTape& tape)
{
   // dcopysubj
   tape.write(bcDCopySubj);
}

void ByteCodeWriter :: doIntOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case ADD_MESSAGE_ID:
         tape.write(bcNAdd);
         break;
      case SUB_MESSAGE_ID:
         tape.write(bcNSub);
         break;
      case MUL_MESSAGE_ID:
         tape.write(bcNMul);
         break;
      case DIV_MESSAGE_ID:
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

void ByteCodeWriter :: doLiteralOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case EQUAL_MESSAGE_ID:
         tape.write(bcWEqual);
         break;
      case LESS_MESSAGE_ID:
         tape.write(bcWLess);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: doArrayOperation(CommandTape& tape, int operator_id)
{
   switch (operator_id) {
      case REFER_MESSAGE_ID:
         // nload
         // get
         tape.write(bcNLoad);
         tape.write(bcGet);
         break;
      default:
         break;
   }
}

void ByteCodeWriter :: selectConstant(CommandTape& tape, ref_t r1, ref_t r2)
{
   tape.write(bcSelectR, r1 | mskConstantRef, r2 | mskConstantRef);
}

//void ByteCodeWriter :: loadSymbolReference(CommandTape& tape, ref_t reference)
//{
//   // acopyr reference
//   tape.write(bcACopyR, reference | mskSymbolRef);
//}
