//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

#include "langcommon.h"

using namespace elena_lang;

typedef ByteCodeWriter::TapeScope TapeScope;

//inline void storeNode(BuildNode node)
//{
//   DynamicUStr target;
//
//   BuildTreeSerializer::save(node, target);
//}

//inline void testNodes(BuildNode node)
//{
//   BuildNode current = node.firstChild();
//   while (current != BuildKey::None) {
//      testNodes(current);
//
//      current = current.nextNode();
//   }
//}

inline BuildKey operator | (const BuildKey& l, const BuildKey& r)
{
   return (BuildKey)((uint32_t)l | (uint32_t)r);
}

inline BuildKey operator & (const BuildKey& l, const BuildKey& r)
{
   return (BuildKey)((uint32_t)l & (uint32_t)r);
}

inline BuildKey operator ~ (BuildKey arg1)
{
   return (BuildKey)(~static_cast<unsigned int>(arg1));
}

inline bool testMask(BuildKey key, BuildKey mask)
{
   return (key & mask) == mask;
}

inline bool isAssignOp(int operatorId)
{
   switch (operatorId) {
      case ADD_ASSIGN_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         return true;
      default:
         return false;
   }
}

void openFrame(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   if (tapeScope.classMode) {
      for (int i = 0; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XFlushSI, i);
      }
   }

   int reservedManaged = tapeScope.reserved;
   int reservedUnmanaged = tapeScope.reservedN;

   tape.newLabel();
   tape.write(ByteCode::OpenIN, reservedManaged, reservedUnmanaged);
}

void extOpenFrame(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int reservedManaged = tapeScope.reserved;
   int reservedUnmanaged = tapeScope.reservedN;

   tape.newLabel();
   tape.write(ByteCode::ExtOpenIN, reservedManaged, reservedUnmanaged);
}

void closeFrame(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int reservedUnmanaged = tapeScope.reservedN;

   tape.setLabel();
   tape.write(ByteCode::CloseN, reservedUnmanaged);

   if (node.arg.value) {
      for (int i = 0; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XRefreshSI, i);
      }
   }
}

void close_ext_frame(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int reservedUnmanaged = tapeScope.reservedN;

   tape.setLabel();
   if (tapeScope.scope->ptrSize == 8) {
      tape.write(ByteCode::LLoad);
   }
   else tape.write(ByteCode::Load);

   tape.write(ByteCode::ExtCloseN, reservedUnmanaged);
}

void nilReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, 0);
}

void terminatorReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, -1);
}

void symbolCall(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::CallR, node.arg.reference | mskSymbolRef);
}

void classReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskVMTRef);
}

void sendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int vmtIndex = node.findChild(BuildKey::VMTIndex).arg.value;

   bool variadicOp = (node.arg.reference & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE;

   pos_t argCount = getArgCount(node.arg.reference);
   if (!variadicOp && (int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = argCount; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);
   tape.write(ByteCode::CallVI, vmtIndex);
}

//void resendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
//{
//   if (node.arg.reference)
//      tape.write(ByteCode::MovM, node.arg.reference);
//
//   int vmtIndex = node.findChild(BuildKey::VMTIndex).arg.value;
//   tape.write(ByteCode::CallVI, vmtIndex);
//}

void strongResendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::CallMR, node.arg.reference, targetRef | mskVMTRef);
}

void redirectOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   if (node.arg.reference)
      tape.write(ByteCode::MovM, node.arg.reference);

   int vmtIndex = node.findChild(BuildKey::VMTIndex).arg.value;
   tape.write(ByteCode::JumpVI, vmtIndex);
}

void directCallOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   bool variadicOp = (node.arg.reference & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE;

   pos_t argCount = getArgCount(node.arg.reference);
   if (!variadicOp && (int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = argCount; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);
   tape.write(ByteCode::CallMR, node.arg.reference, targetRef | mskVMTRef);
}

void semiDirectCallOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   bool variadicOp = (node.arg.reference & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE;
   bool indexTableMode = node.existChild(BuildKey::IndexTableMode);

   pos_t argCount = getArgCount(node.arg.reference);
   if (!variadicOp && (int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = argCount; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);

   // if it is an indexed call
   if (indexTableMode)
      tape.write(ByteCode::AltMode);

   tape.write(ByteCode::VCallMR, node.arg.reference, targetRef | mskVMTRef);
}

void exit(CommandTape& tape, BuildNode&, TapeScope&)
{
   tape.write(ByteCode::Quit);
}

void ext_exit(CommandTape& tape, BuildNode&, TapeScope&)
{
   tape.write(ByteCode::XQuit);
}

void savingInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreSI, node.arg.value);
}

void assigningLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreFI, node.arg.value);
}

void copyingLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   switch (n) {
      case 1:
         // bload
         // save dp:arg
         tape.write(ByteCode::BLoad);
         tape.write(ByteCode::SaveDP, node.arg.value);
         break;
      case 2:
         // wload
         // save dp:arg
         tape.write(ByteCode::WLoad);
         tape.write(ByteCode::SaveDP, node.arg.value);
         break;
      case 4:
         // load
         // save dp:arg
         tape.write(ByteCode::Load);
         tape.write(ByteCode::SaveDP, node.arg.value);
         break;
      default:
         tape.write(ByteCode::StoreSI, 0);
         tape.write(ByteCode::CopyDPN, node.arg.value, n);
         break;
   }
}

void copyingToAcc(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;
   switch (n) {
      case 1:
         tape.write(ByteCode::BCopy);
         break;
      case 2:
         tape.write(ByteCode::WCopy);
         break;
      default:
         tape.write(ByteCode::Copy, n);
         break;
   }
}

void copyingToAccExact(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   tape.write(ByteCode::Copy, n);
}

void intCopyingToAccField(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int value = node.findChild(BuildKey::Value).arg.value;

   if (!node.arg.value) {
      tape.write(ByteCode::XSaveN, value);
   }
   else tape.write(ByteCode::XSaveDispN, node.arg.value, value);
}

void copyingLocalArr(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;
   if (n) {
      tape.write(ByteCode::DCopy, n);
   }
   else tape.write(ByteCode::DTrans);
}

void copyingToLocalArr(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   assert(n > 0);

   tape.write(ByteCode::DCopyDPN, node.arg.reference, n);
}

void assignToStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::StackIndex).arg.value;

   tape.write(ByteCode::MovSIFI, n, node.arg.value);
}

void copyingToAccField(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   tape.write(ByteCode::XCopyON, node.arg.value, n);
}

void copyingAccField(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   tape.write(ByteCode::XWriteON, node.arg.value, n);
}

void getLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::PeekFI, node.arg.value);
}

void localReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetFP, node.arg.value);
}

void getArgument(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::PeekSI, node.arg.value);
}

void getLocalAddress(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetDP, node.arg.value);
}

void creatingClass(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t typeRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::NewIR, node.arg.value, typeRef | mskVMTRef);
}

void creatingStruct(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t typeRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::NewNR, node.arg.value, typeRef ? typeRef | mskVMTRef : 0);
}

void openStatement(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::Statement };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void closeStatement(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::EndOfStatement };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void addingBreakpoint(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   BuildNode row = node.findChild(BuildKey::Row);
   BuildNode col = node.findChild(BuildKey::Column);

   DebugLineInfo symbolInfo = { DebugSymbol::Breakpoint, col.arg.value, row.arg.value };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));

   tape.write(ByteCode::Breakpoint);
}

void addVirtualBreakpoint(CommandTape& tape, BuildNode&, TapeScope& tapeScope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::VirtualBreakpoint };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));

   tape.write(ByteCode::Breakpoint);
}

void intLiteral(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskIntLiteralRef);
}

void longLiteral(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskLongLiteralRef);
}

void realLiteral(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskRealLiteralRef);
}

void mssgLiteral(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskMssgLiteralRef);
}

void mssgNameLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskMssgNameLiteralRef);
}

void propNameLiteral(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskPropNameLiteralRef);
}

void extMssgLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskExtMssgLiteralRef);
}

void stringLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskLiteralRef);
}

void wideLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskWideLiteralRef);
}

void charLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskCharacterRef);
}

void constant(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskConstant);
}

void distrConstant(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskDistrTypeListRef);
}

void constantArray(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskConstArray);
}

void procedure_ref(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskProcedureRef);
}

void externalvar_ref(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskExternalRef);
}

void goingToEOP(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   //gotoEnd(tape, baFirstLabel);
   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
}

void allocatingStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::AllocI, node.arg.value);
}

void freeingStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::FreeI, node.arg.value);
}

void savingNInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Load);
   tape.write(ByteCode::SaveSI, node.arg.value);
}

void load_long_index(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LLoadDP, node.arg.value);
}

void save_long_index(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LSaveDP, node.arg.value);
}

void savingLInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LLoad);
   tape.write(ByteCode::LSaveSI, node.arg.value);
}

void extCallOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int arg2 = node.findChild(BuildKey::Count).arg.value;
   // HOTFIX : special case - long call
   if (node.existChild(BuildKey::LongMode))
      arg2 |= 0x80000000;

   tape.write(ByteCode::CallExtR, node.arg.reference | mskExternalRef, arg2);
}

void savingIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SaveDP, node.arg.value);
}

void savingLongIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LSaveDP, node.arg.value);
}

void savingFloatIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetDP, node.arg.value);
   tape.write(ByteCode::XFSave);
}

void loadingIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LoadDP, node.arg.value);
}

void dispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;
   bool altMode = node.existChild(BuildKey::IndexTableMode);
   if (message) {
      // if it is a multi-method dispatcher
      if (altMode)
         tape.write(ByteCode::AltMode);

      tape.write(ByteCode::DispatchMR, message, node.arg.reference | mskConstArray);
   }
   // otherwise it is generic dispatcher
   else tape.write(ByteCode::Redirect);
}

void xdispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;

   assert(node.arg.reference);

   tape.write(ByteCode::XDispatchMR, message, node.arg.reference | mskConstArray);
}

void genericDispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;

   tape.write(ByteCode::XRedirectM, message);
}

void intRealOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::PeekSI);
      tape.write(ByteCode::Load);
      tape.write(ByteCode::SetDP, targetOffset);
      tape.write(ByteCode::FSave);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FAddDPN, targetOffset, 8);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FSubDPN, targetOffset, 8);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FMulDPN, targetOffset, 8);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FDivDPN, targetOffset, 8);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void intLongOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::PeekSI);
      tape.write(ByteCode::Load);
      tape.write(ByteCode::ConvL);
      tape.write(ByteCode::LSaveDP, targetOffset);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 8);
         break;
      case SUB_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 8);
         break;
      case MUL_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 8);
         break;
      case DIV_OPERATOR_ID:
         tape.write(ByteCode::IDivDPN, targetOffset, 8);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 8);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 8);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 8);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void real_int_xop(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   tape.write(ByteCode::SetDP, targetOffset);

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FIAdd, 8);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FISub, 8);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FIMul, 8);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FIDiv, 8);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void real_int_op(CommandTape& tape, BuildNode& node, TapeScope& scope)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   real_int_xop(tape, node, scope);
}

void realOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FAddDPN, targetOffset, 8);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FSubDPN, targetOffset, 8);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FMulDPN, targetOffset, 8);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::FDivDPN, targetOffset, 8);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void intOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 4);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 4);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 4);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 4);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IDivDPN, targetOffset, 4);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 4);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 4);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 4);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::IShlDPN, targetOffset, 4);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::IShrDPN, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void intOpWithConst(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;
   int sourceOffset = node.findChild(BuildKey::Source).arg.value;
   int value = node.findChild(BuildKey::Value).arg.value;

   // loaddpn
   tape.write(ByteCode::LoadDP, sourceOffset);

   switch (operatorId) {
      case ADD_OPERATOR_ID:
         tape.write(ByteCode::AddN, value);
         break;
      case SUB_OPERATOR_ID:
         tape.write(ByteCode::SubN, value);
         break;
      case MUL_OPERATOR_ID:
         tape.write(ByteCode::MulN, value);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::AndN, value);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::OrN, value);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::Shl, value);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::Shr, value);
         break;
      default:
         throw InternalError(errFatalError);
   }

   // savedpn
   tape.write(ByteCode::SaveDP, targetOffset);
}

void byteOpWithConst(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;
   int sourceOffset = node.findChild(BuildKey::Source).arg.value;
   int value = node.findChild(BuildKey::Value).arg.value;

   // loaddpn
   tape.write(ByteCode::LoadDP, sourceOffset);

   switch (operatorId) {
      case ADD_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::AddN, value);
         break;
      case SUB_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::SubN, value);
         break;
      case MUL_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::MulN, value);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::AndN, value);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::OrN, value);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::Shl, value);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::AndN, 0xFF);
         tape.write(ByteCode::Shr, value);
         break;
      default:
         throw InternalError(errFatalError);
   }

   // savedpn
   tape.write(ByteCode::SaveDP, targetOffset);
}

void uintOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 4);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 4);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 4);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 4);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::UDivDPN, targetOffset, 4);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 4);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 4);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 4);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::IShlDPN, targetOffset, 4);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::IShrDPN, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void intSOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;
   switch (operatorId) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 4);
         break;
      case NEGATE_OPERATOR_ID:
         tape.write(ByteCode::PeekSI);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::Neg);
         tape.write(ByteCode::SaveDP, targetOffset);
         break;
      case INC_OPERATOR_ID:
         tape.write(ByteCode::NAddDPN, targetOffset, 1);
         break;
      case DEC_OPERATOR_ID:
         tape.write(ByteCode::NAddDPN, targetOffset, -1);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void byteOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 1);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 1);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 1);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 1);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IDivDPN, targetOffset, 1);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 1);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 1);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 1);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::IShlDPN, targetOffset, 1);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::IShrDPN, targetOffset, 1);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void byteSOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 1);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void shortOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 2);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 2);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 2);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 2);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IDivDPN, targetOffset, 2);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 2);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 2);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 2);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::IShlDPN, targetOffset, 2);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::IShrDPN, targetOffset, 2);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void shortSOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 2);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void nilCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand
   tape.write(ByteCode::CmpR, 0);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
   default:
      assert(false);
      break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void realCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::FCmpN, 4);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void intCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::ICmpN, 4);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case GREATER_OPERATOR_ID:
         opCode = ByteCode::SelGrRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      case NOTGREATER_OPERATOR_ID:
         opCode = ByteCode::SelGrRR;
         inverted = true;
         break;
      case NOTLESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void uintCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelULtRR;
         break;
      case EQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 4);
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 4);
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void uint8CondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelULtRR;
         break;
      case EQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 1);
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 1);
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}


void uint16CondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelULtRR;
         break;
      case EQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 2);
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         tape.write(ByteCode::ICmpN, 2);
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void byteCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   assert(trueRef);
   assert(falseRef);

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::ICmpN, 1);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void shortCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::ICmpN, 2);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void longOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   if (!isAssignOp(operatorId)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case ADD_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IAddDPN, targetOffset, 8);
         break;
      case SUB_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::ISubDPN, targetOffset, 8);
         break;
      case MUL_OPERATOR_ID:
      case MUL_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IMulDPN, targetOffset, 8);
         break;
      case DIV_OPERATOR_ID:
      case DIV_ASSIGN_OPERATOR_ID:
         tape.write(ByteCode::IDivDPN, targetOffset, 8);
         break;
      case BAND_OPERATOR_ID:
         tape.write(ByteCode::IAndDPN, targetOffset, 8);
         break;
      case BOR_OPERATOR_ID:
         tape.write(ByteCode::IOrDPN, targetOffset, 8);
         break;
      case BXOR_OPERATOR_ID:
         tape.write(ByteCode::IXorDPN, targetOffset, 8);
         break;
      case SHL_OPERATOR_ID:
         tape.write(ByteCode::IShlDPN, targetOffset, 8);
         break;
      case SHR_OPERATOR_ID:
         tape.write(ByteCode::IShrDPN, targetOffset, 8);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void longSOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 8);
         break;
      case NEGATE_OPERATOR_ID:
         tape.write(ByteCode::PeekSI);
         tape.write(ByteCode::LLoad);
         tape.write(ByteCode::LNeg);
         tape.write(ByteCode::LSaveDP, targetOffset);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void longCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::ICmpN, 8);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void longIntCondOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   bool inverted = false;
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   // NOTE : sp[0] - loperand, sp[1] - roperand
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::Load);
   tape.write(ByteCode::ConvL);
   tape.write(ByteCode::PeekSI, 0);
   tape.write(ByteCode::XLCmp, 8);

   ByteCode opCode = ByteCode::None;
   switch (node.arg.value) {
      case LESS_OPERATOR_ID:
         opCode = ByteCode::SelLtRR;
         break;
      case EQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         break;
      case NOTEQUAL_OPERATOR_ID:
         opCode = ByteCode::SelEqRR;
         inverted = true;
         break;
      default:
         assert(false);
         break;
   }

   if (!inverted) {
      tape.write(opCode, trueRef | mskVMTRef, falseRef | mskVMTRef);
   }
   else tape.write(opCode, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void byteArraySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::NLen, 1);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void vargSOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         // nsave      dp : tmp, -1
         // labNext :
         // nadd       dp : tmp, 1
         // peek       sp : 0
         // load       dp : tmp
         // xget
         // cmp        terminator
         // jne        labNext
         // nadd       dp : tmp, 1

         tape.write(ByteCode::NSaveDPN, targetOffset, -1);
         tape.newLabel();     // declare symbol-end label
         tape.setLabel(true);
         tape.write(ByteCode::NAddDPN, targetOffset, 1);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::LoadDP, targetOffset);
         tape.write(ByteCode::XGet);
         tape.write(ByteCode::CmpR, -1);
         tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
         tape.releaseLabel();
         tape.write(ByteCode::NAddDPN, targetOffset, 1);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void shortArraySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::NLen, 2);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void bynaryArraySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;
   int size = node.findChild(BuildKey::Size).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::NLen, size);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void objArraySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::Len);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void objArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   switch (node.arg.value) {
      case SET_INDEXER_OPERATOR_ID:
         // load
         // peek sp:1
         // assign
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::XMovSISI, 0, 1);
         tape.write(ByteCode::Assign);
         break;
      case INDEX_OPERATOR_ID:
         // peek sp:1
         // load
         // xget
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::XGet);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void byteArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         // load
         // peek sp:1
         // write 1
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::WriteN, 1);
         break;
      case INDEX_OPERATOR_ID:
         // peek sp:1
         // load
         // setdp index
         // read 1
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::SetDP, targetOffset);
         tape.write(ByteCode::BRead);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void binaryArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;
   int size = node.findChild(BuildKey::Size).arg.value;

   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         // load
         // peek sp:1
         // write n:1
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::WriteN, size);
         break;
      case INDEX_OPERATOR_ID:
         // peek sp:1
         // load
         // setdp index
         // read n:1
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::SetDP, targetOffset);
         tape.write(ByteCode::ReadN, size);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void shortArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         // load
         // peek sp:1
         // write n:1
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::WriteN, 2);
         break;
      case INDEX_OPERATOR_ID:
         // peek sp:1
         // load
         // setdp index
         // read n:1
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::SetDP, targetOffset);
         tape.write(ByteCode::WRead);
         break;
   default:
      throw InternalError(errFatalError);
   }
}

void intArraySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::NLen, 4);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void intArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.arg.value;
   int operatorId = node.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case SET_INDEXER_OPERATOR_ID:
         // load
         // peek sp:1
         // write n:1
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::WriteN, 4);
         break;
      case INDEX_OPERATOR_ID:
         // peek sp:1
         // load
         // setdp index
         // read n:1
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::Load);
         tape.write(ByteCode::SetDP, targetOffset);
         tape.write(ByteCode::ReadN, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void copyingItem(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int targetOffset = node.findChild(BuildKey::Value).arg.value;

   tape.write(ByteCode::MovN, targetOffset);
   tape.write(ByteCode::WriteN, node.arg.value);
}

void directResend(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   assert(targetRef != 0);

   tape.write(ByteCode::JumpMR, node.arg.reference, targetRef | mskVMTRef);
}

void semiDirectResend(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   assert(targetRef != 0);

   tape.write(ByteCode::VJumpMR, node.arg.reference, targetRef | mskVMTRef);
}

void boolSOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   tape.write(ByteCode::CmpR, trueRef | mskVMTRef);
   tape.write(ByteCode::SelEqRR, falseRef | mskVMTRef, trueRef | mskVMTRef);
}

void nilOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   switch (node.arg.value) {
      case ISNIL_OPERATOR_ID:
         tape.write(ByteCode::Coalesce);
         break;
      default:
         break;
   }
}

void assignSPField(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   // !! temporally - assigni should be used instead
   tape.write(ByteCode::AssignI, node.arg.value);
}

void swapSPField(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::XSwapSI, node.arg.value);
}

void accSwapSPField(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SwapSI, node.arg.value);
}

void getField(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   // !! temporally - assigni should be used instead
   tape.write(ByteCode::GetI, node.arg.value);
}

void staticVarOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::PeekR, node.arg.reference | mskStaticVariable);
}

void threadVarOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::PeekTLS, node.arg.reference | mskTLSVariable);
}

void staticBegin(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.newLabel();     // declare symbol-end label
   tape.write(ByteCode::PeekR, node.arg.reference | mskStaticVariable);
   tape.write(ByteCode::CmpR, 0);
   tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
}

void staticEnd(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::StoreR, node.arg.reference | mskStaticVariable);

   tape.setLabel();
}

void threadVarBegin(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.newLabel();     // declare symbol-end label
   tape.write(ByteCode::PeekTLS, node.arg.reference | mskTLSVariable);
   tape.write(ByteCode::CmpR, 0);
   tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
}

void threadVarEnd(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::StoreTLS, node.arg.reference | mskTLSVariable);

   tape.setLabel();
}

void classOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   switch (node.arg.value) {
      case CLASS_OPERATOR_ID:
         tape.write(ByteCode::Class);
         break;
      default:
         assert(false);
         break;
   }
}

void newArrayOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t typeRef = node.arg.reference;
   int n = node.findChild(BuildKey::Size).arg.value;
   if (n < 0) {
      tape.write(ByteCode::CreateNR, -n, typeRef | mskVMTRef);
   }
   else if (n == 0) {
      tape.write(ByteCode::CreateR, typeRef | mskVMTRef);
   }
   else assert(false);
}

void fillOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   if (node.arg.reference) {
      tape.write(ByteCode::FillIR, node.arg.reference);
   }
   else tape.write(ByteCode::XFillR);
}

void refParamAssigning(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // store si:0
   // peekfi
   // xassign 0
   tape.write(ByteCode::StoreSI, 0);
   tape.write(ByteCode::PeekFI, node.arg.value);
   tape.write(ByteCode::XAssignI, 0);
}

void assignImmediateAccField(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::XAssignI, node.arg.value);
}

void conversionOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   switch (node.arg.reference) {
      case INT8_32_CONVERSION:
         tape.write(ByteCode::BLoad);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::Save);
         break;
      case INT16_32_CONVERSION:
         tape.write(ByteCode::WLoad);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::Save);
         break;
      case INT32_64_CONVERSION:
         tape.write(ByteCode::Load);
         tape.write(ByteCode::ConvL);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::LSave);
         break;
      case INT32_FLOAT64_CONVERSION:
         tape.write(ByteCode::Load);
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::FSave);
         break;
      default:
         break;
   }
}

void breakOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, 0);

   int eolLabel = 0;
   if (tapeScope.loopLabels.count() > 0) {
      auto loopOp = tapeScope.loopLabels.peek();

      eolLabel = loopOp.value2;
   }

   if (eolLabel > 0) {
      tape.write(ByteCode::Jump, eolLabel);
   }
   else tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
}

void continueOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int bolLabel = 0;
   if (tapeScope.loopLabels.count() > 0) {
      auto loopOp = tapeScope.loopLabels.peek();

      bolLabel = loopOp.value1;
   }

   if (bolLabel > 0) {
      tape.write(ByteCode::Jump, bolLabel);
   }
}

void loadingBynaryLen(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::NLen, node.arg.value);
}

void loadArgCount(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // mlen
   // add    n:1
   tape.write(ByteCode::MLen);
   if (node.arg.value > 0)
      tape.write(ByteCode::AddN, node.arg.value);
}

void incIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::AddN, node.arg.value);
}

void unboxingMessage(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int index = node.findChild(BuildKey::Value).arg.value;

   // add    n:index
   // dalloc
   // set    sp:index
   // sub    n:(index + 1)
   // alloc  i:1
   // store  sp:0

   // swap   sp:0
   // dtrans
   // swap   sp:0
   // set    r:-1
   // swap   sp:0
   // xassign
   // free   i:1

   tape.write(ByteCode::AddN, index);
   tape.write(ByteCode::DAlloc);
   tape.write(ByteCode::SetSP, index);
   tape.write(ByteCode::SubN, 1 + index);
   tape.write(ByteCode::AllocI, 1);
   tape.write(ByteCode::StoreSI, 0);
   tape.write(ByteCode::SetFP, node.arg.value);
   tape.write(ByteCode::SwapSI);
   tape.write(ByteCode::DTrans);
   tape.write(ByteCode::SwapSI);
   tape.write(ByteCode::SetR, -1);
   tape.write(ByteCode::SwapSI);
   tape.write(ByteCode::XAssign);
   tape.write(ByteCode::FreeI, 1);
   tape.write(ByteCode::XRefreshSI, 0);
   tape.write(ByteCode::XRefreshSI, 1);
}

void unboxingAndCallMessage(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int index = node.findChild(BuildKey::Value).arg.value;
   int length = node.findChild(BuildKey::Length).arg.value;
   int temp = node.findChild(BuildKey::TempVar).arg.value;
   mssg_t message = node.findChild(BuildKey::Message).arg.value;

   // nsave  dp:tmp, 0
   // load   dp:length
   // add    n:index
   // dalloc
   // set    sp:index
   //
   // alloc  i:2
   // store  sp:1

   // nadd   dp:length, -1
   // labStart:
   // load   dp:tmp
   // xcmp   dp:length
   // jeq    labEnd

   // set    fp:arg
   // xget
   // store  sp:0
   // mov    m:typeMessage
   // call   vt:0
   // store  sp:0

   // load   dp:tmp
   // peek   sp:1
   // xassign

   // nadd   dp:tmp, 1
   // jump   labStart
   // labEnd:

   // xstore sp:0, -1
   // peek   sp:1
   // xassign
   // free   i:2

   tape.write(ByteCode::NSaveDPN, temp, 0);
   tape.write(ByteCode::LoadDP, length);
   tape.write(ByteCode::AddN, index);
   tape.write(ByteCode::DAlloc);
   tape.write(ByteCode::SetSP, index);
   tape.write(ByteCode::AllocI, 2);
   tape.write(ByteCode::StoreSI, 1);
   tape.write(ByteCode::NAddDPN, length, -1);

   tape.newLabel();     // labStart
   tape.setLabel(true);
   tape.write(ByteCode::LoadDP, temp);
   tape.write(ByteCode::XCmpDP, length);
   tape.newLabel();     // labEnd
   tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);

   tape.write(ByteCode::SetFP, node.arg.value);
   tape.write(ByteCode::XGet);
   tape.write(ByteCode::StoreSI, 0);
   tape.write(ByteCode::MovM, message);
   tape.write(ByteCode::CallVI, 0);
   tape.write(ByteCode::StoreSI, 0);
   tape.write(ByteCode::XRefreshSI, 1);  // NOTE :  sp[1] is not refreshed after the operation

   tape.write(ByteCode::LoadDP, temp);
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::XAssign);

   tape.write(ByteCode::NAddDPN, temp, 1);
   tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);
   tape.setLabel();
   tape.releaseLabel();

   tape.write(ByteCode::XStoreSIR, 0, -1);
   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::XAssign);
   tape.write(ByteCode::FreeI, 2);
   tape.write(ByteCode::XRefreshSI, 0);  // NOTE :  sp[0] is not refreshed
   tape.write(ByteCode::XRefreshSI, 1);  // NOTE :  sp[1] is not refreshed
}

void loadingSubject(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int index = node.findChild(BuildKey::StackAddress).arg.value;
   bool mixedMode = node.findChild(BuildKey::Special).arg.value != 0;

   if (mixedMode) {
      tape.newLabel();     // lab2
      tape.newLabel();     // lab1

      // load dp:index
      // tst  FUNCTION_MESSAGE
      // jne  lab1
      // mov mmsg:arg + 1
      // jmp  lab2
      // lab1:
      // mov mmsg:arg
      // lab2:
      tape.write(ByteCode::LoadDP, index);
      tape.write(ByteCode::TstN, FUNCTION_MESSAGE);
      tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
      tape.write(ByteCode::MovM, node.arg.value + 1);
      tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);
      tape.setLabel();
      tape.write(ByteCode::MovM, node.arg.value);
      tape.setLabel();
   }
   else {
      // mov mmsg:arg
      tape.write(ByteCode::MovM, node.arg.value);
   }

   // set dp:index
   // loadv
   tape.write(ByteCode::SetDP, index);
   tape.write(ByteCode::LoadV);

   if ((node.arg.value & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      // or n:VARIADIC_MESSAGE
      tape.write(ByteCode::OrN, VARIADIC_MESSAGE);
   }
}

void peekArgument(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::PeekSI, node.arg.value);
}

void staticAssigning(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreR, node.arg.value | mskStaticVariable);
}

void threadVarAssigning(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreTLS, node.arg.value | mskTLSVariable);
}

void freeStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::DFree);
}


inline void load_ext_arg(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::XLoadArgFI, node.arg.value);
}

inline void savingInt(CommandTape& tape, BuildNode& node, TapeScope&)
{
   BuildNode value = node.findChild(BuildKey::Value);

   tape.write(ByteCode::NSaveDPN, node.arg.value, value.arg.value);
}

inline void loadingAccToIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Load);
}

inline void loadingAccToLongIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LLoad);
}

inline void savingIndexToAcc(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Save);
}

inline void addingInt(CommandTape& tape, BuildNode& node, TapeScope&)
{
   BuildNode value = node.findChild(BuildKey::Value);

   tape.write(ByteCode::NAddDPN, node.arg.value, value.arg.value);
}

inline void indexOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   BuildNode value = node.findChild(BuildKey::Value);

   switch (node.arg.value) {
      case ADD_OPERATOR_ID:
         tape.write(ByteCode::AddN, value.arg.value);
         break;
      case SUB_OPERATOR_ID:
         tape.write(ByteCode::SubN, value.arg.value);
         break;
      case MUL_OPERATOR_ID:
         tape.write(ByteCode::MulN, value.arg.value);
         break;
      default:
         assert(false);
         break;
   }
}

inline void loadingStackDump(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   // store sp:0
   // len
   // shr ptr_size_order
   // neg
   // xset dp:0
   // neg
   // dcopy ptr_size

   tape.write(ByteCode::StoreSI);
   tape.write(ByteCode::NLen, tapeScope.scope->ptrSize);
   tape.write(ByteCode::SetDP, -tapeScope.scope->ptrSize);
   tape.write(ByteCode::DCopy, tapeScope.scope->ptrSize);
   tape.write(ByteCode::PeekSI);
}

inline void savingStackDump(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   // store sp:0
   // nlen ptr_size_order
   // set dp:0
   // dcopy ptr_size

   tape.write(ByteCode::StoreSI);
   tape.write(ByteCode::NLen, tapeScope.scope->ptrSize);
   tape.write(ByteCode::SetDP, -tapeScope.scope->ptrSize);
   tape.write(ByteCode::SwapSI);
   tape.write(ByteCode::DCopy, tapeScope.scope->ptrSize);
   tape.write(ByteCode::PeekSI);
}

inline void includeFrame(CommandTape& tape, bool withThreadSafeNop)
{
   if (withThreadSafeNop)
      tape.write(ByteCode::SNop);

   tape.write(ByteCode::Include);
}

inline void excludeFrame(CommandTape& tape)
{
   tape.write(ByteCode::Exclude);
}

// build code table
ByteCodeWriter::Saver commands[] =
{
   nullptr, openFrame, closeFrame, nilReference, symbolCall, classReference, sendOp, exit,
   savingInStack, assigningLocal, getLocal, creatingClass, openStatement, closeStatement, addingBreakpoint, addingBreakpoint,

   creatingStruct, intLiteral, stringLiteral, goingToEOP, getLocalAddress, copyingLocal, allocatingStack, freeingStack,
   savingNInStack, extCallOp, savingIndex, directCallOp, dispatchOp, intOp, byteArraySOp, copyingToAcc,

   getArgument, nullptr, directResend, /*resendOp*/nullptr, xdispatchOp, boolSOp, intCondOp, charLiteral,
   assignSPField, getField, staticBegin, staticEnd, classOp, byteArrayOp, newArrayOp, swapSPField,

   mssgLiteral, accSwapSPField, redirectOp, shortArraySOp, wideLiteral, byteOp, shortOp, byteCondOp,
   shortCondOp, copyingToAccField, copyingAccField, localReference, refParamAssigning, staticVarOp, loadingIndex, nilOp,

   intSOp, byteSOp, shortSOp, longLiteral, longOp, longSOp, longCondOp, realLiteral,
   realOp, realCondOp, addVirtualBreakpoint, conversionOp, semiDirectResend, nilCondOp, assignToStack, assignImmediateAccField,

   genericDispatchOp, bynaryArraySOp, binaryArrayOp, shortArrayOp, breakOp, constant, objArrayOp, intArrayOp,
   intArraySOp, objArraySOp, copyingLocalArr, extMssgLiteral, loadingBynaryLen, unboxingMessage, loadingSubject, peekArgument,

   terminatorReference, copyingItem, savingLongIndex, longIntCondOp, constantArray, staticAssigning, savingLInStack, uintCondOp,
   uintOp, mssgNameLiteral, vargSOp, loadArgCount, incIndex, freeStack, fillOp, strongResendOp,

   copyingToAccExact, savingInt, addingInt, loadingAccToIndex, indexOp, savingIndexToAcc, continueOp, semiDirectCallOp,
   intRealOp, real_int_op, copyingToLocalArr, loadingStackDump, savingStackDump, savingFloatIndex, intCopyingToAccField, intOpWithConst,

   uint8CondOp, uint16CondOp, intLongOp, distrConstant, unboxingAndCallMessage, threadVarOp, threadVarAssigning, threadVarBegin,
   threadVarEnd, load_long_index, save_long_index, real_int_xop, extOpenFrame, load_ext_arg, close_ext_frame, ext_exit,

   procedure_ref, loadingAccToLongIndex, externalvar_ref, byteOpWithConst, propNameLiteral
};

inline bool duplicateBreakpoints(BuildNode lastNode)
{
   BuildNode prevNode = lastNode.prevNode();
   while (prevNode != BuildKey::Breakpoint) {
      switch (prevNode.key) {
         case BuildKey::OpenStatement:
         case BuildKey::EndStatement:
            prevNode.setKey(BuildKey::Idle);
            break;
         default:
            break;
      }
      prevNode = prevNode.prevNode();
   }
   if (prevNode == BuildKey::Breakpoint) {
      prevNode.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline BuildNode getPrevious(BuildNode node)
{
   node = node.prevNode();
   switch (node.key) {
      case BuildKey::VirtualBreakpoint:
      case BuildKey::Breakpoint:
      case BuildKey::EOPBreakpoint:
      case BuildKey::EndStatement:
      case BuildKey::OpenStatement:
      case BuildKey::Idle:
         return getPrevious(node);
      default:
         return node;
   }
}

inline void setChild(BuildNode node, BuildKey childKey, ref_t childArg)
{
   BuildNode child = node.findChild(childKey);
   if (child.key == childKey) {
      child.setArgumentReference(childArg);
   }
   else node.appendChild(childKey, childArg);
}

inline BuildNode getNextNode(BuildNode node)
{
   node = node.nextNode();
   switch (node.key) {
      case BuildKey::VirtualBreakpoint:
      case BuildKey::Breakpoint:
      case BuildKey::EOPBreakpoint:
      case BuildKey::EndStatement:
      case BuildKey::OpenStatement:
      case BuildKey::Idle:
         return getNextNode(node);
      default:
         return node;
   }
}


inline bool scanFrameForLocalAddresses(BuildNode& current, BuildKey destKey, int local)
{
   bool callOp = false;
   int localCounter = 0;

   while (current != BuildKey::CloseFrame) {
      switch (current.key) {
         case BuildKey::LocalAddress:
            if (current.arg.value == local) {
               if (callOp && getNextNode(current).key == destKey) {
                  return localCounter == 1;
               }

               localCounter++;
            }
            break;
         case BuildKey::CallOp:
         case BuildKey::DirectCallOp:
            if (!callOp) {
               callOp = true;
            }
            else return false;
            break;
         case BuildKey::None:
            return false;
         default:
            break;
      }

      current = current.nextNode();
   }

   return false;
}

inline bool doubleAssigningByRefHandler(BuildNode lastNode)
{
   // OPTTMIZATION CASE : ByRefHandler can directly pass the variable as byref arg
   lastNode.setKey(BuildKey::Idle);

   // scan the tree and find all local addresses used in the operation
   BuildNode opNode = lastNode.nextNode();
   if(scanFrameForLocalAddresses(opNode, BuildKey::Copying, lastNode.arg.value)) {
      BuildNode byRefArg = BuildTree::gotoNode(lastNode, BuildKey::LocalAddress, lastNode.arg.value);
      BuildNode copyOp = opNode.nextNode();
      BuildNode prevCopyOp = getPrevious(opNode);

      // check if it is unboxing op
      if (prevCopyOp.key != BuildKey::Copying || prevCopyOp.arg.reference != lastNode.arg.reference)
         prevCopyOp = {};

      // modify the tree to exclude double copying
      byRefArg.setArgumentValue(copyOp.arg.value);
      opNode.setArgumentValue(copyOp.arg.value);
      copyOp.setKey(BuildKey::Idle);
      if (prevCopyOp.key == BuildKey::Copying)
         prevCopyOp.setArgumentValue(copyOp.arg.value);

      return true;
   }

   return false;
}

inline bool intCopying(BuildNode lastNode)
{
   int size = lastNode.findChild(BuildKey::Size).arg.value;
   if (size == 4 || size == 1 || size == 2) {
      BuildNode constNode = lastNode.prevNode();
      int value = constNode.findChild(BuildKey::Value).arg.value;

      lastNode.appendChild(BuildKey::Value, value);
      lastNode.setKey(BuildKey::SavingInt);

      constNode.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool intOpWithConsts(BuildNode lastNode)
{
   BuildNode opNode = lastNode;
   BuildNode savingOp2 = getPrevious(opNode);
   BuildNode intNode = getPrevious(savingOp2);
   BuildNode valueNode = intNode.findChild(BuildKey::Value);
   BuildNode savingOp1 = getPrevious(intNode);
   BuildNode sourceNode = getPrevious(savingOp1);

   BuildKey constOp = BuildKey::None;
   int size = 0;
   switch (opNode.key) {
      case BuildKey::IntOp:
         size = 4;
         constOp = BuildKey::IntConstOp;
         break;
      case BuildKey::ByteOp:
         size = 1;
         constOp = BuildKey::ByteConstOp;
         break;
      default:
         assert(false);
         break;
   }

   int tempTarget = opNode.arg.value;
   int operatorId = opNode.findChild(BuildKey::OperatorId).arg.value;
   switch (operatorId) {
      case ADD_OPERATOR_ID:
      case SUB_OPERATOR_ID:
         savingOp1.setKey(BuildKey::Copying);
         savingOp1.setArgumentValue(tempTarget);
         savingOp1.appendChild(BuildKey::Size, size);
         intNode.setKey(BuildKey::AddingInt);
         intNode.setArgumentValue(tempTarget);
         if (operatorId == SUB_OPERATOR_ID) {
            // revert the value
            valueNode.setArgumentValue(-valueNode.arg.value);
         }
         savingOp2.setKey(BuildKey::Idle);
         opNode.setKey(BuildKey::Idle);
         break;
      case MUL_OPERATOR_ID:
         savingOp1.setKey(BuildKey::LoadingAccToIndex);
         savingOp1.setArgumentValue(0);
         intNode.setKey(BuildKey::IndexOp);
         intNode.setArgumentValue(operatorId);
         savingOp2.setKey(BuildKey::SavingIndex);
         savingOp2.setArgumentValue(tempTarget);
         opNode.setKey(BuildKey::Idle);
         break;
      case BAND_OPERATOR_ID:
      case BOR_OPERATOR_ID:
      case SHL_OPERATOR_ID:
      case SHR_OPERATOR_ID:
         setChild(intNode, BuildKey::Source, sourceNode.arg.value);
         setChild(intNode, BuildKey::OperatorId, operatorId);

         intNode.setKey(constOp);
         intNode.setArgumentValue(tempTarget);

         opNode.setKey(BuildKey::Idle);
         savingOp2.setKey(BuildKey::Idle);
         savingOp1.setKey(BuildKey::Idle);
         sourceNode.setKey(BuildKey::Idle);
         break;
      default:
         return false;
   }

   return true;
}

inline bool optIntOpWithConsts(BuildNode lastNode)
{
   BuildNode copyNode = lastNode;
   BuildNode sourNode = getPrevious(copyNode);
   BuildNode opNode = getPrevious(sourNode);
   BuildNode sourceCopyNode = getPrevious(opNode);

   if (sourNode.arg.reference == opNode.arg.reference && sourNode.arg.reference == sourceCopyNode.arg.reference) {
      opNode.setArgumentReference(copyNode.arg.reference);
      sourceCopyNode.setArgumentReference(copyNode.arg.reference);

      copyNode.setKey(BuildKey::Idle);
      sourNode.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool doubleCopyingIntOp(BuildNode lastNode)
{
   BuildNode copyingOp = lastNode;
   BuildNode tempLocalOp = getPrevious(copyingOp);
   BuildNode savingIndexOp = getPrevious(tempLocalOp);

   int size = copyingOp.findChild(BuildKey::Size).arg.value;

   if (tempLocalOp.arg.reference == savingIndexOp.arg.reference && size == 4) {
      savingIndexOp.setArgumentReference(copyingOp.arg.reference);

      copyingOp.setKey(BuildKey::Idle);
      tempLocalOp.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool assignIntOpWithConsts(BuildNode lastNode)
{
   BuildNode opNode = lastNode;
   BuildNode savingOp = getPrevious(opNode);
   BuildNode intNode = getPrevious(savingOp);
   BuildNode valueNode = intNode.findChild(BuildKey::Value);

   int tempTarget = opNode.arg.value;
   int operatorId = opNode.findChild(BuildKey::OperatorId).arg.value;

   switch (operatorId) {
      case ADD_ASSIGN_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         intNode.setKey(BuildKey::AddingInt);
         intNode.setArgumentValue(tempTarget);
         if (operatorId == SUB_ASSIGN_OPERATOR_ID) {
            // revert the value
            valueNode.setArgumentValue(-valueNode.arg.value);
         }
         savingOp.setKey(BuildKey::Idle);
         opNode.setKey(BuildKey::Idle);
         break;
      default:
         return false;
   }

   return true;
}

inline bool boxingInt(BuildNode lastNode)
{
   BuildNode copyNode = lastNode;
   BuildNode localNode = getPrevious(copyNode);
   BuildNode savingOp = getPrevious(localNode);
   BuildNode localAddrOp = getPrevious(savingOp);
   BuildNode assigningOp = getPrevious(localAddrOp);

   if (assigningOp.arg.value != localNode.arg.value)
      return false;

   localAddrOp.setKey(BuildKey::LoadingIndex);
   localAddrOp.setArgumentValue(localAddrOp.arg.value);

   savingOp.setKey(BuildKey::SavingIndexToAcc);

   localNode.setKey(BuildKey::Idle);
   copyNode.setKey(BuildKey::Idle);

   return true;
}

inline bool nativeBranchingOp(BuildNode lastNode)
{
   BuildNode branchNode = lastNode;
   BuildNode localNode = getPrevious(branchNode);
   BuildNode assignNode = getPrevious(localNode);
   BuildNode nativeOpNode = getPrevious(assignNode);

   if (localNode.arg.value != assignNode.arg.value)
      return false;

   int op = nativeOpNode.arg.value;
   switch (nativeOpNode.arg.value) {
      case EQUAL_OPERATOR_ID:
      case NOTEQUAL_OPERATOR_ID:
      case LESS_OPERATOR_ID:
      case GREATER_OPERATOR_ID:
      case NOTGREATER_OPERATOR_ID:
      case NOTLESS_OPERATOR_ID:
         break;
      default:
         return false;
   }

   if (branchNode.arg.value == ELSE_OPERATOR_ID) {
      // invert comparison if required
      switch (op) {
         case EQUAL_OPERATOR_ID:
            op = NOTEQUAL_OPERATOR_ID;
            break;
         case NOTEQUAL_OPERATOR_ID:
            op = EQUAL_OPERATOR_ID;
            break;
         case LESS_OPERATOR_ID:
            op = NOTLESS_OPERATOR_ID;
            break;
         case GREATER_OPERATOR_ID:
            op = NOTGREATER_OPERATOR_ID;
            break;
         case NOTGREATER_OPERATOR_ID:
            op = GREATER_OPERATOR_ID;
            break;
         case NOTLESS_OPERATOR_ID:
            op = LESS_OPERATOR_ID;
            break;
         default:
            break;
      }
   }

   switch (nativeOpNode.key) {
      case BuildKey::IntCondOp:
         branchNode.setKey(BuildKey::IntBranchOp);
         break;
      case BuildKey::RealCondOp:
         branchNode.setKey(BuildKey::RealBranchOp);
         break;
      case BuildKey::NilCondOp:
         branchNode.setKey(BuildKey::NilRefBranchOp);
         break;
      default:
         break;
   }

   branchNode.setArgumentValue(op);

   localNode.setKey(BuildKey::Idle);
   assignNode.setKey(BuildKey::Idle);
   nativeOpNode.setKey(BuildKey::Idle);

   return true;
}

inline bool intConstBranchingOp(BuildNode lastNode)
{
   BuildNode branchNode = lastNode;
   BuildNode savingOp2 = getPrevious(branchNode);
   BuildNode intNode = getPrevious(savingOp2);
   BuildNode savingOp1 = getPrevious(intNode);
   BuildNode localOp = getPrevious(savingOp1);

   BuildNode valueNode = intNode.findChild(BuildKey::Value);

   branchNode.appendChild(BuildKey::Value, valueNode.arg.value);
   branchNode.setKey(BuildKey::IntConstBranchOp);

   savingOp2.setKey(BuildKey::Idle);
   intNode.setKey(BuildKey::Idle);
   savingOp1.setKey(BuildKey::Idle);

   localOp.setKey(BuildKey::LoadingIndex);

   return true;
}

inline bool doubleAssigningConverting(BuildNode lastNode)
{
   BuildNode copyingNode = lastNode;
   BuildNode tempNode = getPrevious(copyingNode);
   BuildNode conversionOp = getPrevious(tempNode);
   BuildNode sourceNode = getPrevious(conversionOp);
   BuildNode savingOp1 = getPrevious(sourceNode);
   BuildNode temp2Node = getPrevious(savingOp1);

   int size = 0;
   switch (conversionOp.arg.value) {
      case INT16_32_CONVERSION:
      case INT8_32_CONVERSION:
         size = 4;
         break;
      case INT32_64_CONVERSION:
      case INT32_FLOAT64_CONVERSION:
         size = 8;
         break;
      default:
         assert(false);
         break;
   }

   if (tempNode.arg.value != temp2Node.arg.value)
      return false;

   int copySize = copyingNode.findChild(BuildKey::Size).arg.value;
   if (copySize != size)
      return false;

   temp2Node.setArgumentValue(copyingNode.arg.value);

   tempNode.setKey(BuildKey::Idle);
   copyingNode.setKey(BuildKey::Idle);

   return true;
}

inline bool doubleAssigningIntRealOp(BuildNode lastNode)
{
   BuildNode copyingNode = lastNode;
   BuildNode tempNode = getPrevious(copyingNode);
   BuildNode opNode = getPrevious(tempNode);

   if (tempNode.arg.value != opNode.arg.value)
      return false;

   int copySize = copyingNode.findChild(BuildKey::Size).arg.value;
   switch (opNode.key) {
      case BuildKey::IntRealOp:
      case BuildKey::RealIntOp:
         if (copySize != 8)
            return false;
         break;
      default:
         break;
   }

   opNode.setArgumentValue(copyingNode.arg.value);

   tempNode.setKey(BuildKey::Idle);
   copyingNode.setKey(BuildKey::Idle);

   return true;
}

inline bool inplaceCallOp(BuildNode lastNode)
{
   BuildNode markNode = getPrevious(lastNode);
   BuildNode callNode = getPrevious(markNode);
   BuildNode classNode = getPrevious(callNode);

   if (classNode == BuildKey::ClassReference && getArgCount(callNode.arg.reference) == 0
      && getArgCount(markNode.arg.reference) == 1)
   {
      int targetOffset = lastNode.arg.value;

      classNode.setKey(BuildKey::LocalAddress);
      classNode.setArgumentValue(targetOffset);

      markNode.setKey(callNode.key);
      markNode.setArgumentReference(markNode.arg.reference);
      setChild(markNode, BuildKey::Type, callNode.findChild(BuildKey::Type).arg.reference);

      callNode.setKey(BuildKey::SavingInStack);
      callNode.setArgumentValue(0);

      lastNode.setKey(BuildKey::Idle);

      return true;
   }
   else if (classNode == BuildKey::ClassReference && getArgCount(callNode.arg.reference) == 1
      && getArgCount(markNode.arg.reference) == 2)
   {
      BuildNode savingNode = getPrevious(classNode);
      if (savingNode == BuildKey::SavingInStack && savingNode.arg.value == 0) {
         savingNode.setArgumentValue(1);
      }
      else return false;

      int targetOffset = lastNode.arg.value;

      classNode.setKey(BuildKey::LocalAddress);
      classNode.setArgumentValue(targetOffset);

      markNode.setKey(callNode.key);
      markNode.setArgumentReference(markNode.arg.reference);
      setChild(markNode, BuildKey::Type, callNode.findChild(BuildKey::Type).arg.reference);

      callNode.setKey(BuildKey::SavingInStack);
      callNode.setArgumentValue(0);

      lastNode.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool inplaceCallOp2(BuildNode lastNode)
{
   BuildNode localNode = getPrevious(lastNode);
   BuildNode savingNode = getPrevious(localNode);
   BuildNode markNode = getPrevious(savingNode);
   BuildNode callNode = getPrevious(markNode);
   BuildNode classNode = getPrevious(callNode);

   if (classNode == BuildKey::ClassReference && getArgCount(callNode.arg.reference) == 0
      && getArgCount(markNode.arg.reference) == 1)
   {
      int targetOffset = localNode.arg.value;

      classNode.setKey(BuildKey::Local);
      classNode.setArgumentValue(targetOffset);

      markNode.setKey(callNode.key);
      markNode.setArgumentReference(markNode.arg.reference);
      setChild(markNode, BuildKey::Type, callNode.findChild(BuildKey::Type).arg.reference);

      callNode.setKey(BuildKey::SavingInStack);
      callNode.setArgumentValue(0);

      savingNode.setKey(BuildKey::Idle);
      localNode.setKey(BuildKey::Idle);
      lastNode.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool intConstAssigning(BuildNode lastNode)
{
   BuildNode localNode = getPrevious(lastNode);
   BuildNode savingNode = getPrevious(localNode);
   BuildNode intNode = getPrevious(savingNode);

   BuildNode value = intNode.findChild(BuildKey::Value);

   if (lastNode == BuildKey::CopyingToAccExact) {
      int size = lastNode.findChild(BuildKey::Size).arg.value;
      if (size != 4)
         return false;

      lastNode.setKey(BuildKey::IntCopyingToAccField);
      lastNode.setArgumentValue(0);
   }
   else if (lastNode == BuildKey::CopyingToAccField) {
      int size = lastNode.findChild(BuildKey::Size).arg.value;
      if (size != 4)
         return false;

      lastNode.setKey(BuildKey::IntCopyingToAccField);
   }
   else return false;

   setChild(lastNode, BuildKey::Value, value.arg.value);

   intNode.setKey(BuildKey::Idle);
   savingNode.setKey(BuildKey::Idle);

   return true;
}

ByteCodeWriter::Transformer transformers[] =
{
   nullptr, duplicateBreakpoints, doubleAssigningByRefHandler, intCopying, intOpWithConsts, assignIntOpWithConsts,
   boxingInt, nativeBranchingOp, intConstBranchingOp, doubleAssigningConverting, doubleAssigningIntRealOp,
   doubleCopyingIntOp, inplaceCallOp, intConstAssigning, inplaceCallOp2, optIntOpWithConsts
};

// --- ByteCodeWriter ---

ByteCodeWriter :: ByteCodeWriter(LibraryLoaderBase* loader, bool threadFriendly)
{
   _commands = commands;
   _loader = loader;
   _threadFriendly = true;
}

void ByteCodeWriter :: openSymbolDebugInfo(Scope& scope, ustr_t symbolName)
{
   if (scope.debug->position() == 0)
      scope.debug->writeDWord(0);

   // map symbol debug info, ending with #sym to distinsuish from class
   IdentifierString bookmark(symbolName, "#sym");
   scope.moduleScope->debugModule->mapPredefinedReference(*bookmark, scope.debug->position());

   pos_t namePosition = scope.debugStrings->position();

   if (isWeakReference(symbolName)) {
      IdentifierString fullName(scope.moduleScope->debugModule->name(), symbolName);

      scope.debugStrings->writeString(*fullName);
   }
   else scope.debugStrings->writeString(symbolName);

   DebugLineInfo symbolInfo = { DebugSymbol::Symbol };
   symbolInfo.addresses.source.nameRef = namePosition;

   scope.debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: openClassDebugInfo(Scope& scope, ustr_t className, ref_t flags)
{
   if (scope.debug->position() == 0)
      scope.debug->writeDWord(0);

   scope.moduleScope->debugModule->mapPredefinedReference(className, scope.debug->position());

   pos_t namePosition = scope.debugStrings->position();

   if (isWeakReference(className)) {
      IdentifierString fullName(scope.moduleScope->debugModule->name(), className);

      scope.debugStrings->writeString(*fullName);
   }
   else scope.debugStrings->writeString(className);

   DebugLineInfo symbolInfo = { DebugSymbol::Class };
   symbolInfo.addresses.classSource.nameRef = namePosition;
   symbolInfo.addresses.classSource.flags = flags;

   scope.debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: saveFieldDebugInfo(Scope& scope, ClassInfo& info)
{
   bool isStruct = test(info.header.flags, elStructureRole);

   for (auto it = info.fields.start(); !it.eof(); ++it) {
      DebugLineInfo fieldInfo = { isStruct ? DebugSymbol::FieldAddress : DebugSymbol::Field };
      fieldInfo.addresses.field.nameRef = scope.debugStrings->position();
      fieldInfo.addresses.field.offset = (*it).offset;

      scope.debugStrings->writeString(it.key());
      scope.debug->write(&fieldInfo, sizeof(DebugLineInfo));

      ref_t typeRef = (*it).typeInfo.typeRef;
      if (typeRef && isStruct) {
         DebugLineInfo classInfo = { DebugSymbol::FieldInfo };

         classInfo.addresses.info.nameRef = scope.debugStrings->position();
         if (isStruct) {
            auto next_it = it;
            ++next_it;

            if (next_it.eof()) {
               classInfo.addresses.info.size = info.size - (*it).offset;
            }
            else classInfo.addresses.info.size = (*next_it).offset - (*it).offset;
         }

         if (!isPrimitiveRef(typeRef)) {
            ustr_t typeName = scope.moduleScope->module->resolveReference(typeRef);
            if (isWeakReference(typeName)) {
               IdentifierString fullName(scope.moduleScope->module->name());
               fullName.append(typeName);

               scope.debugStrings->writeString(*fullName);
            }
            else scope.debugStrings->writeString(typeName);
         }
         scope.debug->write(&classInfo, sizeof(DebugLineInfo));
      }
   }
}

void ByteCodeWriter :: openMethodDebugInfo(Scope& scope, pos_t sourcePathRef)
{
   DebugLineInfo symbolInfo = { DebugSymbol::Procedure };
   symbolInfo.addresses.source.nameRef = sourcePathRef;

   scope.debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: endDebugInfo(Scope& scope)
{
   DebugLineInfo endInfo = { DebugSymbol::End };

   scope.debug->write(&endInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: importTree(CommandTape& tape, BuildNode node, Scope& scope)
{
   ustr_t referenceName = scope.moduleScope->resolveFullName(node.arg.reference);
   SectionInfo importInfo;
   if (isWeakReference(referenceName)) {
      importInfo = _loader->getSection(ReferenceInfo(scope.moduleScope->module, referenceName), mskProcedureRef, 0, false);
   }
   else importInfo = _loader->getSection(ReferenceInfo(referenceName), mskProcedureRef, 0, false);

   tape.import(importInfo.module, importInfo.section, true, scope.moduleScope->module);
}

void ByteCodeWriter :: saveBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode, bool loopMode)
{
   bool ifElseMode = node.arg.value == IF_ELSE_OPERATOR_ID;
   if (ifElseMode) {
      tape.newLabel();
   }

   if (!loopMode)
      tape.newLabel();

   switch (node.arg.value) {
      case IF_OPERATOR_ID:
      case IF_ELSE_OPERATOR_ID:
         tape.write(ByteCode::CmpR, node.findChild(BuildKey::Const).arg.reference | mskVMTRef);
         tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
         break;
      case ELSE_OPERATOR_ID:
         tape.write(ByteCode::CmpR, node.findChild(BuildKey::Const).arg.reference | mskVMTRef);
         tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
         break;
      default:
         assert(false);
         break;
   }

   BuildNode tapeNode = node.findChild(BuildKey::Tape);
   saveTape(tape, tapeNode, tapeScope, paths, tapeOptMode);

   if (ifElseMode) {
      // HOTFIX : inject virtual breakpoint to fine-tune a debugger
      BuildNode idleNode = {};
      addVirtualBreakpoint(tape, idleNode, tapeScope);

      tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

      tape.setLabel();
      saveTape(tape, tapeNode.nextNode(), tapeScope, paths, tapeOptMode);

      tape.setLabel();
   }
   else {
      if (!loopMode)
         tape.setLabel();
   }
}

void ByteCodeWriter :: saveTernaryOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   BuildNode lnodeNode = node.findChild(BuildKey::Tape);
   BuildNode rnodeNode = lnodeNode.nextNode();

   tape.write(ByteCode::StoreSI, 1);

   saveTape(tape, lnodeNode, tapeScope, paths, tapeOptMode);
   tape.write(ByteCode::StoreSI);

   saveTape(tape, rnodeNode, tapeScope, paths, tapeOptMode);
   tape.write(ByteCode::SwapSI, 1);

   tape.write(ByteCode::CmpR, node.findChild(BuildKey::Const).arg.reference | mskVMTRef);

   tape.write(ByteCode::PeekSI, 1);
   tape.write(ByteCode::XPeekEq);
}

void ByteCodeWriter :: saveNativeBranching(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode, bool loopMode)
{
   BuildNode tapeNode = node.findChild(BuildKey::Tape);

   bool ifElseMode = tapeNode.nextNode() == BuildKey::Tape;
   if (ifElseMode) {
      tape.newLabel();
   }

   if (!loopMode)
      tape.newLabel();

   switch (node.key) {
      case BuildKey::IntBranchOp:
         // NOTE : sp[0] - loperand, sp[1] - roperand
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::ICmpN, 4);
         break;
      case BuildKey::RealBranchOp:
         // NOTE : sp[0] - loperand, sp[1] - roperand
         tape.write(ByteCode::PeekSI, 1);
         tape.write(ByteCode::FCmpN, 8);
         break;
      case BuildKey::IntConstBranchOp:
      {
         BuildNode valueNode = node.findChild(BuildKey::Value);

         tape.write(ByteCode::CmpN, valueNode.arg.value);
         break;
      }
      case BuildKey::NilRefBranchOp:
         // NOTE : sp[0] - loperand
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::CmpR);
         break;
      default:
         assert(false);
         break;
   }

   switch (node.arg.value) {
      case EQUAL_OPERATOR_ID:
         tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);
         break;
      case NOTEQUAL_OPERATOR_ID:
         tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
         break;
      case LESS_OPERATOR_ID:
         tape.write(ByteCode::Jge, PseudoArg::CurrentLabel);
         break;
      case GREATER_OPERATOR_ID:
         // should be inverted
         tape.write(ByteCode::Jle, PseudoArg::CurrentLabel);
         break;
      case NOTGREATER_OPERATOR_ID:
         // should be inverted
         tape.write(ByteCode::Jgr, PseudoArg::CurrentLabel);
         break;
      case NOTLESS_OPERATOR_ID:
         // should be inverted
         tape.write(ByteCode::Jlt, PseudoArg::CurrentLabel);
         break;
      default:
         assert(false);
         break;
   }

   saveTape(tape, tapeNode, tapeScope, paths, tapeOptMode);

   if (ifElseMode) {
      // HOTFIX : inject virtual breakpoint to fine-tune a debugger
      BuildNode idleNode = {};
      addVirtualBreakpoint(tape, idleNode, tapeScope);

      tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

      tape.setLabel();
      saveTape(tape, tapeNode.nextNode(), tapeScope, paths, tapeOptMode);

      tape.setLabel();
   }
   else {
      if (!loopMode)
         tape.setLabel();
   }
}

void ByteCodeWriter :: saveShortCircuitOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();

   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   BuildNode lnode = node.findChild(BuildKey::Tape);
   BuildNode rnode = lnode.nextNode();

   if (rnode != BuildKey::None) {
      saveTape(tape, lnode, tapeScope, paths, tapeOptMode, false);
   }
   else rnode = lnode;

   ByteCode jmpCode = ByteCode::Jeq;
   switch (node.arg.reference) {
      case AND_OPERATOR_ID:
         tape.write(ByteCode::CmpR, falseRef | mskVMTRef);
         break;
      case OR_OPERATOR_ID:
         tape.write(ByteCode::CmpR, trueRef | mskVMTRef);
         break;
      case ISNIL_OPERATOR_ID:
         tape.write(ByteCode::CmpR, 0);
         jmpCode = ByteCode::Jne;
         break;
   }

//   tape.write(ByteCode::BreakLabel); // !! temporally, to prevent if-optimization
   tape.write(jmpCode, PseudoArg::CurrentLabel);

   saveTape(tape, rnode, tapeScope, paths, tapeOptMode, false);

   tape.setLabel();
}

void ByteCodeWriter :: saveLoop(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   tape.write(ByteCode::XNop);
   int startLabel = tape.newLabel();
   tape.setLabel(true);
   int eopLabel = tape.newLabel();

   if (tapeScope.threadFriendly)
      // snop
      tape.write(ByteCode::SNop);

   tapeScope.loopLabels.push({ startLabel, eopLabel });

   saveTape(tape, node, tapeScope, paths, tapeOptMode, true);

   tape.write(ByteCode::Jump, startLabel);

   tapeScope.loopLabels.pop();

   tape.setLabel();
   tape.releaseLabel();
}

void ByteCodeWriter :: openTryBlock(CommandTape& tape, TryContextInfo& tryInfo, bool virtualMode)
{
   if (!virtualMode) {
      tryInfo.retLabel = tape.newLabel();             // declare ret-end-label
      tryInfo.endLabel = tape.newLabel();             // declare end-label
      tryInfo.altLabel = tape.newLabel();             // declare alternative-label
   }
   else {
      tryInfo.retLabel = tape.renewLabel(tryInfo.retLabel); // declare ret-end-label
      tryInfo.endLabel = tape.renewLabel(tryInfo.endLabel); // declare end-label
      tryInfo.altLabel = tape.renewLabel(tryInfo.altLabel); // declare alternative-label
   }

   tape.write(ByteCode::XHookDPR, tryInfo.ptr, tryInfo.altLabel, mskLabelRef);

   tryInfo.retLabel = tape.exchangeFirstsLabel(tryInfo.retLabel);
}

// NOTE : closing is true for the actual try-catch end, and false - for try_exclude
void ByteCodeWriter :: closeTryBlock(CommandTape& tape, TryContextInfo& tryInfo, bool virtualMode,
   TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   BuildNode finallyNode = tryInfo.catchMode ? tryInfo.catchNode.nextNode(BuildKey::Tape) : tryInfo.catchNode;

   // unhook
   tape.write(ByteCode::Unhook);

   if (!tryInfo.catchMode) {
      // for try-finally statement, the finnaly code must be called here
      if (!virtualMode && finallyNode != BuildKey::None) {
         tape.write(ByteCode::StoreFI, tryInfo.index);
         saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);
         tape.write(ByteCode::PeekFI, tryInfo.index);
      }
   }

   // jump
   tape.write(ByteCode::Jump, tryInfo.endLabel);

   // === exit redirect block ===
   // restore the original ret label and return the overridden one
   tryInfo.retLabel = tape.exchangeFirstsLabel(tryInfo.retLabel);

   // ret-end-label:
   tape.setPredefinedLabel(tryInfo.retLabel);

   // unhook
   tape.write(ByteCode::Unhook);

   // finally-block
   if (finallyNode != BuildKey::None) {      
      tape.write(ByteCode::StoreFI, tryInfo.index);
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);
      tape.write(ByteCode::PeekFI, tryInfo.index);
   }

   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
   // ===========================

   // catchLabel:
   if (virtualMode) {
      tape.setPredefinedLabel(tryInfo.altLabel);
   }
   else tape.setLabel();

   // tstflg elMessage
   // jeq labSkip
   // load
   // peeksi 0
   // callvi 0
   // labSkip:
   // unhook
   tape.newLabel();
   tape.write(ByteCode::TstFlag, elMessage);
   tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
   tape.write(ByteCode::Load);
   tape.write(ByteCode::PeekSI);
   tape.write(ByteCode::CallVI);
   tape.setLabel();
   tape.write(ByteCode::Unhook);

   if (tryInfo.catchMode) {
      saveTape(tape, tryInfo.catchNode, tapeScope, paths, tapeOptMode, false);

      // eos:
      if (virtualMode) {
         tape.setPredefinedLabel(tryInfo.endLabel);
      }
      else {
         tape.setLabel();
         tape.releaseLabel(); // release ret-end-label
      }

      // finally-block
      if (finallyNode != BuildKey::None) {
         tape.write(ByteCode::StoreFI, tryInfo.index);
         saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);
         tape.write(ByteCode::PeekFI, tryInfo.index);
      }

      if (virtualMode)
         tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
   }
   else {
      // finally-block
      if (finallyNode != BuildKey::None) {
         // store fp:index
         // <finally>
         // peek fp:index

         tape.write(ByteCode::StoreFI, tryInfo.index);
         saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);
         tape.write(ByteCode::PeekFI, tryInfo.index);
      }

      // throw
      tape.write(ByteCode::Throw);

      // eos:
      if (virtualMode) {
         tape.setPredefinedLabel(tryInfo.endLabel);
      }
      else {
         tape.setLabel();
         tape.releaseLabel(); // release ret-end-label
      }
   }
}

void ByteCodeWriter::includeTryBlocks(CommandTape& tape, TapeScope& tapeScope)
{
   if (tapeScope.tryContexts.count() == 1) {
      auto info = tapeScope.tryContexts.pop();

      openTryBlock(tape, info, true);

      tapeScope.tryContexts.push(info);
   }
   else if (tapeScope.tryContexts.count() > 1) {
      TryContexts temp({ });
      while (tapeScope.tryContexts.count() > 0) {
         temp.push(tapeScope.tryContexts.pop());
      }
      while (temp.count() > 0) {
         auto info = temp.pop();

         openTryBlock(tape, info, true);

         tapeScope.tryContexts.push(info);
      }
   }
}

void ByteCodeWriter :: excludeTryBlocks(CommandTape& tape,
   TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   if (tapeScope.tryContexts.count() == 1) {
      auto info = tapeScope.tryContexts.pop();

      closeTryBlock(tape, info, true, tapeScope, paths, tapeOptMode);

      tapeScope.tryContexts.push(info);
   }
   else if (tapeScope.tryContexts.count() > 1) {
      TryContexts temp({});
      while (temp.count() > 0) {
         auto info = tapeScope.tryContexts.pop();

         closeTryBlock(tape, info, true, tapeScope, paths, tapeOptMode);

         temp.push(info);
      }
      while (temp.count() > 0) {
         tapeScope.tryContexts.push(temp.pop());
      }
   }
}

void ByteCodeWriter :: saveCatching(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   BuildNode tryNode = node.findChild(BuildKey::Tape);

   TryContextInfo blockInfo = { true };
   blockInfo.catchNode = tryNode.nextNode(BuildKey::Tape);   
   blockInfo.index = node.findChild(BuildKey::StackIndex).arg.value;
   blockInfo.ptr = node.arg.value;

   openTryBlock(tape, blockInfo, false);

   tapeScope.tryContexts.push(blockInfo);

   saveTape(tape, tryNode, tapeScope, paths, tapeOptMode, false);

   blockInfo = tapeScope.tryContexts.pop();

   closeTryBlock(tape, blockInfo, false, tapeScope, paths, tapeOptMode);
}

void ByteCodeWriter :: saveFinally(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   BuildNode tryNode = node.findChild(BuildKey::Tape);

   TryContextInfo blockInfo = { false };
   blockInfo.catchNode = tryNode.nextNode(BuildKey::Tape);
   blockInfo.index = node.findChild(BuildKey::StackIndex).arg.value;
   blockInfo.ptr = node.arg.value;

   openTryBlock(tape, blockInfo, false);

   tapeScope.tryContexts.push(blockInfo);

   saveTape(tape, tryNode, tapeScope, paths, tapeOptMode, false);

   blockInfo = tapeScope.tryContexts.pop();

   closeTryBlock(tape, blockInfo, false, tapeScope, paths, tapeOptMode);
}

void ByteCodeWriter :: saveSwitchOption(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();

   // NOTE : loopMode is set to true due to current implementation, so the branching will use an existing label
   saveTape(tape, node, tapeScope, paths, tapeOptMode, true);

   // HOTFIX : inject virtual breakpoint to fine-tune a debugger
   BuildNode idleNode = {};
   addVirtualBreakpoint(tape, idleNode, tapeScope);

   tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

   tape.setLabel();
}

void ByteCodeWriter :: saveSwitching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::SwitchOption:
            saveSwitchOption(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::ElseOption:
            saveTape(tape, current, tapeScope, paths, tapeOptMode);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }

   tape.setLabel();
}

void ByteCodeWriter :: saveAlternate(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   int retLabel = tape.newLabel();                 // declare ret-end-label
   tape.newLabel();
   tape.newLabel();

   tape.write(ByteCode::XHookDPR, node.arg.value, PseudoArg::CurrentLabel, mskLabelRef);

   retLabel = tape.exchangeFirstsLabel(retLabel);

   BuildNode tryNode = node.findChild(BuildKey::Tape);
   saveTape(tape, tryNode, tapeScope, paths, tapeOptMode, false);

   // unhook
   tape.write(ByteCode::Unhook);

   // jump
   tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

   // === exit redirect block ===
   // restore the original ret label and return the overridden one
   retLabel = tape.exchangeFirstsLabel(retLabel);

   // ret-end-label:
   tape.setPredefinedLabel(retLabel);

   // unhook
   tape.write(ByteCode::Unhook);

   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
   // ===========================

   // catchLabel:
   tape.setLabel();

   // unhook
   tape.write(ByteCode::Unhook);

   BuildNode catchNode = tryNode.nextNode(BuildKey::Tape);
   saveTape(tape, catchNode, tapeScope, paths, tapeOptMode, false);

   // eos:
   tape.setLabel();
   tape.releaseLabel(); // release ret-end-label
}

void ByteCodeWriter :: saveStackCondOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();

   tape.write(ByteCode::TstStck);
   tape.write(ByteCode::Jne, PseudoArg::CurrentLabel);

   saveTape(tape, node, tapeScope, paths, tapeOptMode);

   tape.setLabel();
}

void ByteCodeWriter :: saveYielding(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();

   tape.write(ByteCode::XLabelDPR, node.arg.value, PseudoArg::CurrentLabel, mskLabelRef);

   BuildNode tapeNode = node.findChild(BuildKey::Tape);
   saveTape(tape, tapeNode, tapeScope, paths, tapeOptMode);

   tape.setLabel();
}

void ByteCodeWriter :: saveYieldDispatch(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   // get i:0
   // cmp r:0
   // jeq labStart
   // xjump
   // labStart:

   tape.newLabel();

   tape.write(ByteCode::GetI);
   tape.write(ByteCode::CmpR);
   tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
   tape.write(ByteCode::XJump);
   tape.setLabel();
}

inline void saveDebugSymbol(DebugSymbol symbol, int offset, ustr_t name, TapeScope& tapeScope,
   ustr_t className = nullptr)
{
   DebugLineInfo info = { symbol };
   info.addresses.local.offset = offset;
   info.addresses.local.nameRef = tapeScope.scope->debugStrings->position();

   tapeScope.scope->debugStrings->writeString(name);

   tapeScope.scope->debug->write(&info, sizeof(info));

   if (!emptystr(className)) {
      DebugLineInfo classInfo = { DebugSymbol::LocalInfo };
      classInfo.addresses.source.nameRef = tapeScope.scope->debugStrings->position();
      tapeScope.scope->debug->write(&classInfo, sizeof(classInfo));
      tapeScope.scope->debugStrings->writeString(className);
   }
}

void ByteCodeWriter :: saveVariableInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::BinaryArray:
            // setting size
            tape.write(ByteCode::NSaveDPN, current.arg.value + 4, current.findChild(BuildKey::Size).arg.value);
            break;
         case BuildKey::Variable:
            saveDebugSymbol(DebugSymbol::Local, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::VariableAddress:
            saveDebugSymbol(DebugSymbol::LocalAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(),
               tapeScope, current.findChild(BuildKey::ClassName).identifier());
            break;
         case BuildKey::IntVariableAddress:
            saveDebugSymbol(DebugSymbol::IntLocalAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::UIntVariableAddress:
            saveDebugSymbol(DebugSymbol::UIntLocalAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ByteArrayAddress:
            saveDebugSymbol(DebugSymbol::ByteArrayAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ShortArrayAddress:
            saveDebugSymbol(DebugSymbol::ShortArrayAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntArrayAddress:
            saveDebugSymbol(DebugSymbol::IntArrayAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::LongVariableAddress:
            saveDebugSymbol(DebugSymbol::LongLocalAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::RealVariableAddress:
            saveDebugSymbol(DebugSymbol::RealLocalAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

inline void saveParameterDebugSymbol(DebugSymbol symbol, int offset, ustr_t name, TapeScope& tapeScope, ustr_t className = nullptr)
{
   DebugLineInfo info = { symbol };
   info.addresses.local.offset = offset;
   info.addresses.local.nameRef = tapeScope.scope->debugStrings->position();
   tapeScope.scope->debug->write(&info, sizeof(info));

   tapeScope.scope->debugStrings->writeString(name);

   DebugLineInfo frameInfo = { DebugSymbol::FrameInfo };
   frameInfo.addresses.offset.disp = tapeScope.reservedN;
   tapeScope.scope->debug->write(&frameInfo, sizeof(frameInfo));

   if (!emptystr(className)) {
      DebugLineInfo classInfo = { DebugSymbol::ParameterInfo };
      classInfo.addresses.source.nameRef= tapeScope.scope->debugStrings->position();
      tapeScope.scope->debug->write(&classInfo, sizeof(classInfo));
      tapeScope.scope->debugStrings->writeString(className);
   }
}

void ByteCodeWriter :: saveArgumentsInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Parameter:
            saveParameterDebugSymbol(DebugSymbol::Parameter, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::IntParameterAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::LongParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::LongParameterAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::RealParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::RealParameterAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::ParameterAddress, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(),
               tapeScope, current.findChild(BuildKey::ClassName).identifier());
            break;
         case BuildKey::ByteArrayParameter:
            saveDebugSymbol(DebugSymbol::ByteArrayParameter, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ShortArrayParameter:
            saveDebugSymbol(DebugSymbol::ShortArrayParameter, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntArrayParameter:
            saveDebugSymbol(DebugSymbol::IntArrayParameter, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::RealArrayParameter:
            saveDebugSymbol(DebugSymbol::RealArrayParameter, current.findChild(BuildKey::StackIndex).arg.value, current.identifier(), tapeScope);
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: saveMethodInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope)
{
   DebugLineInfo methodInfo = { DebugSymbol::MessageInfo };
   methodInfo.addresses.source.nameRef = tapeScope.scope->debugStrings->position();

   tapeScope.scope->debugStrings->writeString(node.identifier());
   tapeScope.scope->debug->write(&methodInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: saveExternOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   excludeFrame(tape);

   saveTape(tape, node, tapeScope, paths, tapeOptMode);

   includeFrame(tape, tapeScope.threadFriendly);
}

void ByteCodeWriter :: saveTape(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode, bool loopMode)
{
   bool weakLoop = loopMode;

   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::VariableInfo:
            // declaring variables / setting array size
            saveVariableInfo(tape, current, tapeScope);
            break;
         case BuildKey::ArgumentsInfo:
            // declaring variables / setting array size
            saveArgumentsInfo(tape, current, tapeScope);
            break;
         case BuildKey::MethodName:
            // declaring variables / setting array size
            saveMethodInfo(tape, current, tapeScope);
            break;
         case BuildKey::Import:
            tape.write(ByteCode::ImportOn);
            importTree(tape, current, *tapeScope.scope);
            tape.write(ByteCode::ImportOff);
            break;
         case BuildKey::BranchOp:
            saveBranching(tape, current, tapeScope, paths, tapeOptMode, loopMode);
            weakLoop = false;
            break;
         case BuildKey::IntBranchOp:
         case BuildKey::IntConstBranchOp:
         case BuildKey::RealBranchOp:
         case BuildKey::NilRefBranchOp:
            saveNativeBranching(tape, current, tapeScope, paths, tapeOptMode, loopMode);
            weakLoop = false;
            break;
         case BuildKey::LoopOp:
            saveLoop(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::CatchOp:
            saveCatching(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::FinalOp:
            saveFinally(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::AltOp:
            saveAlternate(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::Switching:
            saveSwitching(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::ExternOp:
            saveExternOp(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::ShortCircuitOp:
            saveShortCircuitOp(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::StackCondOp:
            saveStackCondOp(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::YieldingOp:
            saveYielding(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::YieldDispatch:
            saveYieldDispatch(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::TernaryOp:
            saveTernaryOp(tape, current, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::ExcludeTry:
            excludeTryBlocks(tape, tapeScope, paths, tapeOptMode);
            break;
         case BuildKey::IncludeTry:
            includeTryBlocks(tape, tapeScope);
            break;
         case BuildKey::Path:
         case BuildKey::InplaceCall:
            // ignore special nodes
            break;
         case BuildKey::Idle:
            break;
         default:
            _commands[(int)current.key](tape, current, tapeScope);
            break;
      }

      current = current.nextNode();
   }

   if (weakLoop) {
      tape.write(ByteCode::CmpR, 0);
      tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
   }
}

void ByteCodeWriter :: saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList,
   int ptrSize, ReferenceMap& paths, bool tapeOptMode)
{
   ref_t mask = node.key == BuildKey::Procedure ? mskProcedureRef : mskSymbolRef;

   auto section = moduleScope->mapSection(node.arg.reference | mask, false);
   MemoryWriter writer(section);

   Scope scope = { nullptr, &writer, moduleScope, nullptr, nullptr, minimalArgList, ptrSize };

   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      pos_t sourcePathRef = savePath(node.findChild(BuildKey::Path), scope, paths);

      openSymbolDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef));
      saveProcedure(node, scope, false, sourcePathRef, paths, tapeOptMode);
      endDebugInfo(scope);
   }
   else saveProcedure(node, scope, false, INVALID_POS, paths, tapeOptMode);
}

bool ByteCodeWriter :: applyRules(CommandTape& tape)
{
   if (!_bcTransformer.loaded)
      return false;

   if (_bcTransformer.apply(tape)) {
      while (_bcTransformer.apply(tape));

      return true;
   }
   return false;
}

void ByteCodeWriter :: optimizeTape(CommandTape& tape)
{
   bool modified = true;
   while (modified) {
      modified = false;

      // optimize unused and idle jumps
      while (CommandTape::optimizeJumps(tape)) {
         modified = true;
      }

      // optimize byte codes using optimization rules
      while (applyRules(tape)) {
         modified = true;
      }
   }
}

inline bool isNonOperational(BuildKey key)
{
   switch (key) {
      case BuildKey::ByRefOpMark:
      case BuildKey::InplaceCall:
      case BuildKey::IntBranchOp:
      case BuildKey::CatchOp:
      case BuildKey::AltOp:
      case BuildKey::LoopOp:
      case BuildKey::ShortCircuitOp:
         return false;
      case BuildKey::OpenStatement:
      case BuildKey::EndStatement:
      case BuildKey::VirtualBreakpoint:
      case BuildKey::EOPBreakpoint:
         // NOTE : to simplify the patterns, ignore open / end statement commands
         return true;
      default:
         return (key > BuildKey::MaxOperationalKey);
   }
}

inline bool isNested(BuildKey key)
{
   switch (key) {
      case BuildKey::LoopOp:
      case BuildKey::CatchOp:
      case BuildKey::AltOp:
      case BuildKey::Switching:
      case BuildKey::ExternOp:
      case BuildKey::ShortCircuitOp:
      case BuildKey::BranchOp:
      case BuildKey::FinalOp:
      case BuildKey::IntBranchOp:
      case BuildKey::IntConstBranchOp:
      case BuildKey::RealBranchOp:
      case BuildKey::YieldingOp:
      case BuildKey::SwitchOption:
      case BuildKey::ElseOption:
      case BuildKey::StackCondOp:
      case BuildKey::TernaryOp:
      case BuildKey::Tape:
         return true;
      default:
         return false;
   }
}

void ByteCodeWriter :: saveProcedure(BuildNode node, Scope& scope, bool classMode, pos_t sourcePathRef,
   ReferenceMap& paths, bool tapeOptMode)
{
   _btAnalyzer.proceed(node.findChild(BuildKey::Tape));
   _btTransformer.proceed(node.findChild(BuildKey::Tape));

   if (scope.moduleScope->debugModule)
      openMethodDebugInfo(scope, sourcePathRef);

   TapeScope tapeScope = {
      &scope,
      node.findChild(BuildKey::Reserved).arg.value,
      node.findChild(BuildKey::ReservedN).arg.value,
      classMode,
      _threadFriendly
   };

   CommandTape tape;
   saveTape(tape, node.findChild(BuildKey::Tape), tapeScope, paths, tapeOptMode);

   // optimize
   if (tapeOptMode)
      optimizeTape(tape);

   MemoryWriter* code = scope.code;
   pos_t sizePlaceholder = code->position();
   code->writePos(0);

   // create byte code sections
   tape.saveTo(code);

   pos_t endPosition = code->position();
   pos_t size = endPosition - sizePlaceholder - sizeof(pos_t);

   code->seek(sizePlaceholder);
   code->writePos(size);
   code->seek(endPosition);

   if (scope.moduleScope->debugModule)
      endDebugInfo(scope);
}

void ByteCodeWriter :: saveVMT(ClassInfo& info, BuildNode node, Scope& scope, pos_t sourcePathRef,
   ReferenceMap& paths, bool tapeOptMode, IndexedMessages& indexedMessages)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      if (current == BuildKey::Method) {
         pos_t methodSourcePathRef = sourcePathRef;

         auto methodInfo = info.methods.get(current.arg.reference);
         if (MethodInfo::checkHint(methodInfo, MethodHint::Indexed))
            indexedMessages.add(current.arg.reference);

         MethodEntry entry = { current.arg.reference, scope.code->position(), methodInfo.outputRef };
         scope.vmt->write(&entry, sizeof(MethodEntry));

         BuildNode pathNode = current.findChild(BuildKey::Path);
         if (pathNode != BuildKey::None)
            methodSourcePathRef = savePath(pathNode, scope, paths);

         saveProcedure(current, scope, true, methodSourcePathRef, paths, tapeOptMode);
      }
      else if (current == BuildKey::AbstractMethod) {
         auto methodInfo = info.methods.get(current.arg.reference);
         if (MethodInfo::checkHint(methodInfo, MethodHint::Indexed))
            indexedMessages.add(current.arg.reference);

         MethodEntry entry = { current.arg.reference, INVALID_POS };
         scope.vmt->write(&entry, sizeof(MethodEntry));
      }

      current = current.nextNode();
   }

}

void ByteCodeWriter :: saveIndexTable(Scope& scope, IndexedMessages& indexedMessages)
{
   // saving indexes terminated with 0
   for (pos_t i = 0; i < indexedMessages.count_pos(); i++)
      scope.vmt->writeDWord(indexedMessages[i]);

   scope.vmt->writeDWord(0);
}

pos_t ByteCodeWriter :: savePath(BuildNode node, Scope& scope, ReferenceMap& paths)
{
   if (node == BuildKey::None)
      return INVALID_POS;

   pos_t sourcePathRef = paths.get(node.identifier());
   if (sourcePathRef == INVALID_POS) {
      sourcePathRef = scope.debugStrings->position();
      scope.debugStrings->writeString(node.identifier());

      paths.add(node.identifier(), sourcePathRef);
   }

   return sourcePathRef;
}

void ByteCodeWriter :: saveClass(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList,
   int ptrSize, ReferenceMap& paths, bool tapeOptMode)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(moduleScope->mapSection(node.arg.reference | mskClassRef, false));

   auto vmtSection = moduleScope->mapSection(node.arg.reference | mskVMTRef, false);
   MemoryWriter vmtWriter(vmtSection);

   vmtWriter.writeDWord(0);                     // save size place holder
   pos_t classPosition = vmtWriter.position();

   // copy class meta data header + vmt size
   MemoryReader metaReader(moduleScope->mapSection(node.arg.reference | mskMetaClassInfoRef, true));
   ClassInfo info;
   info.load(&metaReader);

   // reset VMT & HMT lengths
   info.header.count = 0;
   info.header.indexCount = 0;
   for (auto m_it = info.methods.start(); !m_it.eof(); ++m_it) {
      auto m_info = *m_it;

      //NOTE : ingnore statically linked and predefined methods
      if (!test(m_it.key(), STATIC_MESSAGE) && !test(m_info.hints, (ref_t)MethodHint::Predefined))
         info.header.count++;

      //NOTE : count indexed methods
      if (MethodInfo::checkHint(m_info, MethodHint::Indexed))
         info.header.indexCount++;
   }

   vmtWriter.write(&info.header, sizeof(ClassHeader));  // header

   Scope scope = { &vmtWriter, &codeWriter, moduleScope, nullptr, nullptr, minimalArgList, ptrSize };
   IndexedMessages indexedMessages;
   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      pos_t sourcePath = savePath(node.findChild(BuildKey::Path), scope, paths);

      openClassDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef), info.header.flags);
      saveFieldDebugInfo(scope, info);
      saveVMT(info, node, scope, sourcePath, paths, tapeOptMode, indexedMessages);
      endDebugInfo(scope);
   }
   else saveVMT(info, node, scope, INVALID_POS, paths, tapeOptMode, indexedMessages);

   pos_t size = vmtWriter.position() - classPosition;
   vmtSection->write(classPosition - 4, &size, sizeof(size));

   saveIndexTable(scope, indexedMessages);

   ClassInfo::saveStaticFields(&vmtWriter, info.statics);

   CachedList<ref_t, 4> globalAttributes;
   for (auto it = info.attributes.start(); !it.eof(); ++it) {
      auto key = it.key();
      if (!testany(info.header.flags, elClassClass | elAbstract)
         && (key.value2 == ClassAttribute::RuntimeLoadable))
      {
         globalAttributes.add((unsigned int)ClassAttribute::RuntimeLoadable);
      }
      else if (key.value2 == ClassAttribute::Initializer) {
         ref_t symbolRef = *it;

         globalAttributes.add((unsigned int)ClassAttribute::Initializer);
         globalAttributes.add(symbolRef);
      }
      else if (key.value2 == ClassAttribute::ExtOverloadList) {
         globalAttributes.add((unsigned int)ClassAttribute::ExtOverloadList);
         globalAttributes.add(key.value1);
         globalAttributes.add(*it);
      }
   }

   vmtWriter.writePos(globalAttributes.count_pos() * sizeof(unsigned int));
   for (pos_t i = 0; i < globalAttributes.count_pos(); i++) {
      vmtWriter.writeDWord(globalAttributes.get(i));
   }
}

void ByteCodeWriter :: save(BuildTree& tree, SectionScopeBase* moduleScope,
   int minimalArgList, int ptrSize, bool tapeOptMode)
{
   ReferenceMap paths(INVALID_POS);

   BuildNode node = tree.readRoot();
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Symbol:
         case BuildKey::Procedure:
            saveSymbol(current, moduleScope, minimalArgList, ptrSize, paths, tapeOptMode);
            break;
         case BuildKey::Class:
            saveClass(current, moduleScope, minimalArgList, ptrSize, paths, tapeOptMode);
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: loadBuildTreeRules(MemoryDump* dump)
{
   MemoryReader reader(dump);

   _btAnalyzer.load(reader);
}

void ByteCodeWriter :: loadBuildTreeXRules(MemoryDump* dump)
{
   MemoryReader reader(dump);

   _btTransformer.load(reader);
}

void ByteCodeWriter :: loadByteCodeRules(MemoryDump* dump)
{
   MemoryReader reader(dump);

   _bcTransformer.trie.load(&reader);
   _bcTransformer.loaded = true;
}

// --- ByteCodeWriter::BuildTreeTransformerBase ---

void ByteCodeWriter::BuildTreeTransformerBase :: load(StreamReader& reader)
{
   _btPatterns.trie.load(&reader);
   _btPatterns.loaded = true;
}

void ByteCodeWriter::BuildTreeTransformerBase :: proceed(BuildNode node)
{
   if (!_btPatterns.loaded)
      return;

   bool applied = true;
   while (applied) {
      applied = false;

      applied = matchTriePatterns(node);
   }
}

bool ByteCodeWriter::BuildTreeTransformerBase :: matchTriePatterns(BuildNode node)
{
   BuildPatterns matchedOnes;
   BuildPatterns nextOnes;

   BuildPatterns* matched = &matchedOnes;
   BuildPatterns* followers = &nextOnes;
   bool           reversed = false;

   BuildNode previous = {};
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      if (isNested(current.key)) {
         // NOTE : analize nested command
         if (matchTriePatterns(current))
            return true;
      }
      if (isNonOperational(current.key)) {
         // NOTE : ignore non-operational commands)
         current = current.nextNode();

         continue;
      }

      matched->add({ &_btPatterns.trie });
      followers->clear();

      if (matchBuildKey(matched, followers, current, previous))
         return true;

      if (reversed) {
         reversed = false;
         followers = &nextOnes;
         matched = &matchedOnes;
      }
      else {
         reversed = true;
         matched = &nextOnes;
         followers = &matchedOnes;
      }

      previous = current;
      current = current.nextNode();
   }

   // check the remaining patterns
   return matchBuildKey(matched, followers, current, previous);
}

bool ByteCodeWriter::BuildTreeTransformerBase :: matchBuildKey(BuildPatterns* matched, BuildPatterns* followers, BuildNode current, BuildNode previous)
{
   for (auto it = matched->start(); !it.eof(); ++it) {
      BuildPatternArg args = (*it).args;
      auto pattern = (*it).node;

      for (auto child_it = pattern.Children(); !child_it.eof(); ++child_it) {
         auto currentPattern = child_it.Node();
         auto currentPatternValue = currentPattern.Value();

         if (currentPatternValue.match(current, args)) {
            if (currentPatternValue.key == BuildKey::Match) {
               if (transform(currentPattern, previous, args))
                  return true;
            }

            followers->add({ currentPattern, args });
         }
      }
   }

   return false;
}

// --- ByteCodeWriter::BuildTreeAnalyzer ---

bool ByteCodeWriter::BuildTreeAnalyzer :: transform(BuildCodeTrieNode matchNode, BuildNode current, BuildPatternArg&)
{
   auto matchNodeValue = matchNode.Value();
   int patternId = matchNodeValue.argValue;
   if (matchNodeValue.key == BuildKey::Match && patternId) {
      return transformers[patternId](current);
   }

   return false;
}

// --- ByteCodeWriter::BuildTreeOptimizer ---

bool ByteCodeWriter::BuildTreeOptimizer :: transform(BuildCodeTrieNode matchNode, BuildNode current, BuildPatternArg& args)
{
   BuildCodeTrieNode replacement = matchNode.FirstChild();

   BuildPattern pattern = replacement.Value();
   while (pattern.key != BuildKey::None) {
      // skip meta commands (except label)
      while (isNonOperational(current.key))
         current = current.prevNode();

      switch (pattern.argType) {
         case BuildPatternType::Set:
            if (pattern.argValue == 1) {
               current.setArgumentValue(args.arg1);
            }
            else current.setArgumentValue(args.arg2);
            break;
         case BuildPatternType::MatchArg:
            current.setArgumentValue(pattern.argValue);
            break;
         default:
            break;
      }

      current.setKey(pattern.key);

      current = current.prevNode();

      replacement = replacement.FirstChild();
      pattern = replacement.Value();
   }

   return true;
}
