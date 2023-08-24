//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

#include "langcommon.h"

using namespace elena_lang;

typedef ByteCodeWriter::TapeScope TapeScope;

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
   int vmtIndex = node.findChild(BuildKey::Index).arg.value;

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

void resendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   if (node.arg.reference)
      tape.write(ByteCode::MovM, node.arg.reference);

   int vmtIndex = node.findChild(BuildKey::Index).arg.value;
   tape.write(ByteCode::CallVI, vmtIndex);
}

void strongResendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::CallMR, node.arg.reference, targetRef | mskVMTRef);
}

void redirectOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   if (node.arg.reference)
      tape.write(ByteCode::MovM, node.arg.reference);

   int vmtIndex = node.findChild(BuildKey::Index).arg.value;
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

   pos_t argCount = getArgCount(node.arg.reference);
   if (!variadicOp && (int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = argCount; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);
   tape.write(ByteCode::VCallMR, node.arg.reference, targetRef | mskVMTRef);
}

void exit(CommandTape& tape, BuildNode& node, TapeScope& scope)
{
   tape.write(ByteCode::Quit);
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

void copyingLocalArr(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;
   if (n) {
      tape.write(ByteCode::DCopy, n);
   }
   else tape.write(ByteCode::DTrans);
}

void assignToStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Index).arg.value;

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

   tape.write(ByteCode::NewNR, node.arg.value, typeRef | mskVMTRef);
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

void intLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskIntLiteralRef);
}

void longLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskLongLiteralRef);
}

void realLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskRealLiteralRef);
}

void mssgLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskMssgLiteralRef);
}

void mssgNameLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskMssgNameLiteralRef);
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

void constantArray(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskConstArray);
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

void savingLInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LLoad);
   tape.write(ByteCode::LSaveSI, node.arg.value);
}

void extCallOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::CallExtR, node.arg.reference | mskExternalRef, node.findChild(BuildKey::Count).arg.value);
}

void savingIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SaveDP, node.arg.value);
}

void savingLongIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LSaveDP, node.arg.value);
}

void loadingIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::LoadDP, node.arg.value);
}

void dispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;
   if (message) {
      // if it is a multi-method dispatcher
      tape.write(ByteCode::DispatchMR, message, node.arg.reference | mskConstArray);
   }
   // otherwise it is generic dispatcher
   else tape.write(ByteCode::Redirect);
}

void xdispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;

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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::PeekSI);
      tape.write(ByteCode::Load);
      tape.write(ByteCode::SetDP, targetOffset);
      tape.write(ByteCode::FSave);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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

void realIntOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);

      tape.write(ByteCode::SetDP, targetOffset);
   }

   switch (node.arg.value) {
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

void realOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 4);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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

void uintOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 4);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   switch (node.arg.value) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void byteOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 1);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 2);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   if (!isAssignOp(node.arg.value)) {
      tape.write(ByteCode::CopyDPN, targetOffset, 8);
      tape.write(ByteCode::XMovSISI, 0, 1);
   }

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   switch (node.arg.value) {
      case BNOT_OPERATOR_ID:
         tape.write(ByteCode::INotDPN, targetOffset, 8);
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   int size = node.findChild(BuildKey::Size).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   int size = node.findChild(BuildKey::Size).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
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
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

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
   else tape.write(ByteCode::Jump, PseudoArg::CurrentLabel);
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
   int index = node.findChild(BuildKey::Index).arg.value;

   // add    n:index
   // dalloc
   // set    sp:index
   // sub    n:(index + 1)
   // alloc  i:1
   // store  sp:0
   // set    fp:arg
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

void loadingSubject(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int index = node.findChild(BuildKey::Index).arg.value;
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

void freeStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Neg);
   tape.write(ByteCode::DAlloc);
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

ByteCodeWriter::Saver commands[] =
{
   nullptr, openFrame, closeFrame, nilReference, symbolCall, classReference, sendOp, exit,
   savingInStack, assigningLocal, getLocal, creatingClass, openStatement, closeStatement, addingBreakpoint, addingBreakpoint,

   creatingStruct, intLiteral, stringLiteral, goingToEOP, getLocalAddress, copyingLocal, allocatingStack, freeingStack,
   savingNInStack, extCallOp, savingIndex, directCallOp, dispatchOp, intOp, byteArraySOp, copyingToAcc,

   getArgument, nullptr, directResend, resendOp, xdispatchOp, boolSOp, intCondOp, charLiteral,
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
   intRealOp, realIntOp
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
      case BuildKey::VirtualBreakoint:
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

inline BuildNode getNextNode(BuildNode node)
{
   node = node.nextNode();
   switch (node.key) {
      case BuildKey::VirtualBreakoint:
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

      // modify the tree to exclude double copying
      byRefArg.setArgumentValue(copyOp.arg.value);
      opNode.setArgumentValue(copyOp.arg.value);
      copyOp.setKey(BuildKey::Idle);

      return true;
   }

   return false;
}

inline bool intCopying(BuildNode lastNode)
{
   int size = lastNode.findChild(BuildKey::Size).arg.value;
   if (size == 4) {
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
   int size = lastNode.findChild(BuildKey::Size).arg.value;
   if (size != 4)
      return false;

   BuildNode targetNode = lastNode;
   BuildNode tempNode = getPrevious(targetNode);
   BuildNode opNode = getPrevious(tempNode);
   BuildNode savingOp2 = getPrevious(opNode);
   BuildNode intNode = getPrevious(savingOp2);
   BuildNode valueNode = intNode.findChild(BuildKey::Value);
   BuildNode savingOp1 = getPrevious(intNode);

   int tempTarget = opNode.findChild(BuildKey::Index).arg.value;
   if (tempTarget != tempNode.arg.value)
      return false;

   switch (opNode.arg.value) {
      case ADD_OPERATOR_ID:
      case SUB_OPERATOR_ID:
         savingOp1.setKey(BuildKey::Copying);
         savingOp1.setArgumentValue(targetNode.arg.value);
         savingOp1.appendChild(BuildKey::Size, 4);
         intNode.setKey(BuildKey::AddingInt);
         intNode.setArgumentValue(targetNode.arg.value);
         if (opNode.arg.value == SUB_OPERATOR_ID) {
            // revert the value
            valueNode.setArgumentValue(-valueNode.arg.value);
         }
         savingOp2.setKey(BuildKey::Idle);
         opNode.setKey(BuildKey::Idle);
         tempNode.setKey(BuildKey::Idle);
         targetNode.setKey(BuildKey::Idle);
         break;
      case MUL_OPERATOR_ID:
         savingOp1.setKey(BuildKey::LoadingAccToIndex);
         savingOp1.setArgumentValue(0);
         intNode.setKey(BuildKey::IndexOp);
         intNode.setArgumentValue(opNode.arg.value);
         savingOp2.setKey(BuildKey::SavingIndex);
         savingOp2.setArgumentValue(targetNode.arg.value);
         opNode.setKey(BuildKey::Idle);
         tempNode.setKey(BuildKey::Idle);
         targetNode.setKey(BuildKey::Idle);
         break;
      default:
         return false;
   }

   return true;
}

inline bool intOpWithConsts2(BuildNode lastNode)
{
   BuildNode copyingOp = lastNode;
   BuildNode targetNode2 = getPrevious(copyingOp);
   BuildNode stackOp = getPrevious(targetNode2);
   BuildNode tempOp = getPrevious(stackOp);
   BuildNode assigningOp = getPrevious(tempOp);
   BuildNode createOp = getPrevious(assigningOp);
   BuildNode intOp = getPrevious(createOp);
   BuildNode stackOp2 = getPrevious(intOp);
   BuildNode intLiteralOp = getPrevious(stackOp2);
   BuildNode stackOp3 = getPrevious(intLiteralOp);
   BuildNode sourceOp = getPrevious(stackOp3);

   BuildNode valueNode = intLiteralOp.findChild(BuildKey::Value);
   BuildNode intOpIndex = intOp.findChild(BuildKey::Index);

   if (targetNode2.arg.reference != assigningOp.arg.reference)
      return false;

   if (tempOp.arg.reference != intOpIndex.arg.reference)
      return false;

   // !! temporal - exclude div
   if (intOp.arg.value == DIV_OPERATOR_ID)
      return false;

   tempOp.setKey(BuildKey::LoadingIndex);
   tempOp.setArgumentReference(sourceOp.arg.reference);

   stackOp.setKey(BuildKey::IndexOp);
   stackOp.setArgumentValue(intOp.arg.value);
   stackOp.appendChild(BuildKey::Value, valueNode.arg.value);
   copyingOp.setKey(BuildKey::SavingIndexToAcc);

   intOp.setKey(BuildKey::Idle);
   stackOp2.setKey(BuildKey::Idle);
   intLiteralOp.setKey(BuildKey::Idle);
   stackOp3.setKey(BuildKey::Idle);
   sourceOp.setKey(BuildKey::Idle);

   return true;
}

inline bool assignIntOpWithConsts(BuildNode lastNode)
{
   BuildNode opNode = lastNode;
   BuildNode savingOp = getPrevious(opNode);
   BuildNode intNode = getPrevious(savingOp);
   BuildNode valueNode = intNode.findChild(BuildKey::Value);

   int tempTarget = opNode.findChild(BuildKey::Index).arg.value;

   switch (opNode.arg.value) {
      case ADD_ASSIGN_OPERATOR_ID:
      case SUB_ASSIGN_OPERATOR_ID:
         intNode.setKey(BuildKey::AddingInt);
         intNode.setArgumentValue(tempTarget);
         if (opNode.arg.value == SUB_ASSIGN_OPERATOR_ID) {
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
   BuildNode indexNode = opNode.findChild(BuildKey::Index);

   if (tempNode.arg.value != indexNode.arg.value)
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

   indexNode.setArgumentValue(copyingNode.arg.value);

   tempNode.setKey(BuildKey::Idle);
   copyingNode.setKey(BuildKey::Idle);

   return true;
}

ByteCodeWriter::Transformer transformers[] =
{
   nullptr, duplicateBreakpoints, doubleAssigningByRefHandler, intCopying, intOpWithConsts, assignIntOpWithConsts,
   boxingInt, nativeBranchingOp, intConstBranchingOp, doubleAssigningConverting, doubleAssigningIntRealOp,
   intOpWithConsts2
};

// --- ByteCodeWriter ---

ByteCodeWriter :: ByteCodeWriter(LibraryLoaderBase* loader)
{
   _commands = commands;
   _loader = loader;
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
         DebugLineInfo classInfo = { DebugSymbol::ClassInfo };

         classInfo.addresses.source.nameRef = scope.debugStrings->position();
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

   tape.import(importInfo.module, importInfo.section, true, scope.moduleScope);
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

void ByteCodeWriter::saveShortCircuitOp(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   int endLabel = tape.newLabel();

   ref_t trueRef = node.findChild(BuildKey::TrueConst).arg.reference;
   ref_t falseRef = node.findChild(BuildKey::FalseConst).arg.reference;

   BuildNode lnode = node.findChild(BuildKey::Tape);
   BuildNode rnode = lnode.nextNode();

   saveTape(tape, lnode, tapeScope, paths, tapeOptMode, false);

   switch (node.arg.reference) {
      case AND_OPERATOR_ID:
         tape.write(ByteCode::CmpR, falseRef | mskVMTRef);
         break;
      case OR_OPERATOR_ID:
         tape.write(ByteCode::CmpR, trueRef | mskVMTRef);
         break;
   }

//   tape.write(ByteCode::BreakLabel); // !! temporally, to prevent if-optimization
   tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);

   saveTape(tape, rnode, tapeScope, paths, tapeOptMode, false);

   tape.setLabel();
}

void ByteCodeWriter :: saveLoop(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
   ReferenceMap& paths, bool tapeOptMode)
{
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

void ByteCodeWriter :: saveCatching(CommandTape& tape, BuildNode node, TapeScope& tapeScope, 
   ReferenceMap& paths, bool tapeOptMode)
{
   int retLabel = tape.newLabel();                 // declare ret-end-label
   tape.newLabel();                                // declare end-label  
   tape.newLabel();                                // declare alternative-label

   tape.write(ByteCode::XHookDPR, node.arg.value, PseudoArg::CurrentLabel, mskLabelRef);

   retLabel = tape.exchangeFirstsLabel(retLabel);

   BuildNode tryNode = node.findChild(BuildKey::Tape);
   BuildNode catchNode = tryNode.nextNode(BuildKey::Tape);
   BuildNode finallyNode = catchNode.nextNode(BuildKey::Tape);
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

   // finally-block
   if (finallyNode != BuildKey::None)
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);

   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
   // ===========================

   // catchLabel:
   tape.setLabel();

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

   saveTape(tape, catchNode, tapeScope, paths, tapeOptMode, false);

   // eos:
   tape.setLabel();
   tape.releaseLabel(); // release ret-end-label

   // finally-block
   if (finallyNode != BuildKey::None)
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);
}

void ByteCodeWriter :: saveFinally(CommandTape& tape, BuildNode node, TapeScope& tapeScope,
   ReferenceMap& paths, bool tapeOptMode)
{
   int retLabel = tape.newLabel();                 // declare ret-end-label
   tape.newLabel();                                // declare end-label  
   tape.newLabel();                                // declare alternative-label

   tape.write(ByteCode::XHookDPR, node.arg.value, PseudoArg::CurrentLabel, mskLabelRef);

   retLabel = tape.exchangeFirstsLabel(retLabel);

   BuildNode tryNode = node.findChild(BuildKey::Tape);
   BuildNode finallyNode = tryNode.nextNode(BuildKey::Tape);
   BuildNode index = node.findChild(BuildKey::Index);

   saveTape(tape, tryNode, tapeScope, paths, tapeOptMode, false);

   // unhook
   tape.write(ByteCode::Unhook);

   if (finallyNode != BuildKey::None)
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);

   // jump
   tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

   // === exit redirect block ===
   // restore the original ret label and return the overridden one
   retLabel = tape.exchangeFirstsLabel(retLabel);

   // ret-end-label:
   tape.setPredefinedLabel(retLabel);

   // unhook
   tape.write(ByteCode::Unhook);

   // finally-block
   if (finallyNode != BuildKey::None)
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);

   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
   // ===========================

   // catchLabel:
   tape.setLabel();

   // tstflg elMessage
   // jeq labSkip
   // load
   // peeksi 0
   // callvi 0   
   // labSkip:
   // unhook
   // store fp:index

   tape.newLabel();
   tape.write(ByteCode::TstFlag, elMessage);
   tape.write(ByteCode::Jeq, PseudoArg::CurrentLabel);
   tape.write(ByteCode::Load);
   tape.write(ByteCode::PeekSI);
   tape.write(ByteCode::CallVI);
   tape.setLabel();
   tape.write(ByteCode::Unhook);
   tape.write(ByteCode::StoreFI, index.arg.value);

   // finally-block
   if (finallyNode != BuildKey::None)
      saveTape(tape, finallyNode, tapeScope, paths, tapeOptMode, false);

   // peek fp:index
   // throw
   tape.write(ByteCode::PeekFI, index.arg.value);
   tape.write(ByteCode::Throw);

   // eos:
   tape.setLabel();
   tape.releaseLabel(); // release ret-end-label
}

void ByteCodeWriter :: saveSwitchOption(CommandTape& tape, BuildNode node, TapeScope& tapeScope, ReferenceMap& paths, bool tapeOptMode)
{
   tape.newLabel();

   // NOTE : loopMode is set to true due to current implementation, so the branching will use an existing label
   saveTape(tape, node, tapeScope, paths, tapeOptMode, true);

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
   tape.newLabel();
   tape.newLabel();

   tape.write(ByteCode::XHookDPR, node.arg.value, PseudoArg::CurrentLabel, mskLabelRef);

   BuildNode tryNode = node.findChild(BuildKey::Tape);
   saveTape(tape, tryNode, tapeScope, paths, tapeOptMode, false);

   // unhook
   tape.write(ByteCode::Unhook);

   // jump
   tape.write(ByteCode::Jump, PseudoArg::PreviousLabel);

   // catchLabel:
   tape.setLabel();

   // unhook
   tape.write(ByteCode::Unhook);

   BuildNode catchNode = tryNode.nextNode(BuildKey::Tape);
   saveTape(tape, catchNode, tapeScope, paths, tapeOptMode, false);

   // eos:
   tape.setLabel();
}

inline void saveDebugSymbol(DebugSymbol symbol, int offset, ustr_t name, TapeScope& tapeScope)
{
   DebugLineInfo info = { symbol };
   info.addresses.local.offset = offset;
   info.addresses.local.nameRef = tapeScope.scope->debugStrings->position();

   tapeScope.scope->debugStrings->writeString(name);

   tapeScope.scope->debug->write(&info, sizeof(info));
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
            saveDebugSymbol(DebugSymbol::Local, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::VariableAddress:
            saveDebugSymbol(DebugSymbol::LocalAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntVariableAddress:
            saveDebugSymbol(DebugSymbol::IntLocalAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::UIntVariableAddress:
            saveDebugSymbol(DebugSymbol::UIntLocalAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ByteArrayAddress:
            saveDebugSymbol(DebugSymbol::ByteArrayAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ShortArrayAddress:
            saveDebugSymbol(DebugSymbol::ShortArrayAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntArrayAddress:
            saveDebugSymbol(DebugSymbol::IntArrayAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::LongVariableAddress:
            saveDebugSymbol(DebugSymbol::LongLocalAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::RealVariableAddress:
            saveDebugSymbol(DebugSymbol::RealLocalAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
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
      DebugLineInfo classInfo = { DebugSymbol::ClassInfo };
      classInfo.addresses.source.nameRef= tapeScope.scope->debugStrings->position();
      tapeScope.scope->debug->write(&classInfo, sizeof(classInfo));
      tapeScope.scope->debugStrings->writeString(className);
   }
}

void ByteCodeWriter :: saveParameterInfo(CommandTape& tape, BuildNode node, TapeScope& tapeScope)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Parameter:
            saveParameterDebugSymbol(DebugSymbol::Parameter, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::IntParameterAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::LongParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::LongParameterAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::RealParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::RealParameterAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ParameterAddress:
            saveParameterDebugSymbol(DebugSymbol::ParameterAddress, current.findChild(BuildKey::Index).arg.value, current.identifier(),
               tapeScope, current.findChild(BuildKey::ClassName).identifier());
            break;
         case BuildKey::ByteArrayParameter:
            saveDebugSymbol(DebugSymbol::ByteArrayParameter, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::ShortArrayParameter:
            saveDebugSymbol(DebugSymbol::ShortArrayParameter, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
            break;
         case BuildKey::IntArrayParameter:
            saveDebugSymbol(DebugSymbol::IntArrayParameter, current.findChild(BuildKey::Index).arg.value, current.identifier(), tapeScope);
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
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::VariableInfo:
            // declaring variables / setting array size
            saveVariableInfo(tape, current, tapeScope);
            break;
         case BuildKey::ParameterInfo:
            // declaring variables / setting array size
            saveParameterInfo(tape, current, tapeScope);
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
            break;
         case BuildKey::IntBranchOp:
         case BuildKey::IntConstBranchOp:
         case BuildKey::RealBranchOp:
            saveNativeBranching(tape, current, tapeScope, paths, tapeOptMode, loopMode);
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
         case BuildKey::Path:
         case BuildKey::Idle:
            // ignore path node
            break;
         default:
            _commands[(int)current.key](tape, current, tapeScope);
            break;
      }
   
      current = current.nextNode();
   }
}

void ByteCodeWriter :: saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList, 
   ReferenceMap& paths, bool tapeOptMode, bool threadFriendly)
{
   auto section = moduleScope->mapSection(node.arg.reference | mskSymbolRef, false);
   MemoryWriter writer(section);

   Scope scope = { nullptr, &writer, moduleScope, nullptr, nullptr, minimalArgList };

   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      pos_t sourcePathRef = savePath(node.findChild(BuildKey::Path), scope, paths);

      openSymbolDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef));
      saveProcedure(node, scope, false, sourcePathRef, paths, tapeOptMode, threadFriendly);
      endDebugInfo(scope);
   }
   else saveProcedure(node, scope, false, INVALID_POS, paths, tapeOptMode, threadFriendly);
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
      case BuildKey::IntBranchOp:
         return false;
      case BuildKey::OpenStatement:
      case BuildKey::EndStatement:
      case BuildKey::VirtualBreakoint:
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
      case BuildKey::Tape:
         return true;
      default:
         return false;
   }
}

bool ByteCodeWriter :: matchTriePatterns(BuildNode node)
{
   BuildPatterns matchedOnes;
   BuildPatterns nextOnes;

   BuildPatterns* matched = &matchedOnes;
   BuildPatterns* followers = &nextOnes;
   bool           reversed = false;

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

      matched->add({ &_btTransformer.trie });
      followers->clear();

      for (auto it = matched->start(); !it.eof(); ++it) {
         auto pattern = *it;

         for (auto child_it = pattern.Children(); !child_it.eof(); ++child_it) {
            auto currentPattern = child_it.Node();
            auto currentPatternValue = currentPattern.Value();

            if (currentPatternValue.match(current)) {
               if (currentPatternValue.patternId && transformers[currentPatternValue.patternId](current))
                  return true;

               followers->add(currentPattern);
            }
         }
      }

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

      current = current.nextNode();
   }

   return false;
}

void ByteCodeWriter :: optimizeBuildTree(BuildNode node)
{
   if (!_btTransformer.loaded)
      return;

   bool applied = true;
   while (applied) {
      applied = false;

      applied = matchTriePatterns(node);
   }
}

void ByteCodeWriter :: saveProcedure(BuildNode node, Scope& scope, bool classMode, pos_t sourcePathRef, 
   ReferenceMap& paths, bool tapeOptMode, bool threadFriendly)
{
   optimizeBuildTree(node.findChild(BuildKey::Tape));

   if (scope.moduleScope->debugModule)
      openMethodDebugInfo(scope, sourcePathRef);

   TapeScope tapeScope = {
      &scope,
      node.findChild(BuildKey::Reserved).arg.value,
      node.findChild(BuildKey::ReservedN).arg.value,
      classMode,
      threadFriendly
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

void ByteCodeWriter :: saveVMT(BuildNode node, Scope& scope, pos_t sourcePathRef, 
   ReferenceMap& paths, bool tapeOptMode, bool threadFriendly)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      if (current == BuildKey::Method) {
         pos_t methodSourcePathRef = sourcePathRef;

         MethodEntry entry = { current.arg.reference, scope.code->position() };
         scope.vmt->write(&entry, sizeof(MethodEntry));

         BuildNode pathNode = current.findChild(BuildKey::Path);
         if (pathNode != BuildKey::None)
            methodSourcePathRef = savePath(pathNode, scope, paths);

         saveProcedure(current, scope, true, methodSourcePathRef, paths, tapeOptMode, threadFriendly);
      }
      else if (current == BuildKey::AbstractMethod) {
         MethodEntry entry = { current.arg.reference, INVALID_POS };
         scope.vmt->write(&entry, sizeof(MethodEntry));
      }

      current = current.nextNode();
   }
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
   ReferenceMap& paths, bool tapeOptMode, bool threadFriendly)
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

   // reset VMT length
   info.header.count = 0;
   for (auto m_it = info.methods.start(); !m_it.eof(); ++m_it) {
      auto m_info = *m_it;

      //NOTE : ingnore statically linked and predefined methods
      if (!test(m_it.key(), STATIC_MESSAGE) && !test(m_info.hints, (ref_t)MethodHint::Predefined))
         info.header.count++;
   }

   vmtWriter.write(&info.header, sizeof(ClassHeader));  // header

   Scope scope = { &vmtWriter, &codeWriter, moduleScope, nullptr, nullptr, minimalArgList };
   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      pos_t sourcePath = savePath(node.findChild(BuildKey::Path), scope, paths);

      openClassDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef), info.header.flags);
      saveFieldDebugInfo(scope, info);
      saveVMT(node, scope, sourcePath, paths, tapeOptMode, threadFriendly);
      endDebugInfo(scope);
   }
   else saveVMT(node, scope, INVALID_POS, paths, tapeOptMode, threadFriendly);

   pos_t size = vmtWriter.position() - classPosition;
   vmtSection->write(classPosition - 4, &size, sizeof(size));

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
   int minimalArgList, bool tapeOptMode, bool threadFriendly)
{
   ReferenceMap paths(INVALID_POS);

   BuildNode node = tree.readRoot();
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Symbol:
            saveSymbol(current, moduleScope, minimalArgList, paths, tapeOptMode, threadFriendly);
            break;
         case BuildKey::Class:
            saveClass(current, moduleScope, minimalArgList, paths, tapeOptMode, threadFriendly);
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

   _btTransformer.trie.load(&reader);
   _btTransformer.loaded = true;
}

void ByteCodeWriter :: loadByteCodeRules(MemoryDump* dump)
{
   MemoryReader reader(dump);

   _bcTransformer.trie.load(&reader);
   _bcTransformer.loaded = true;
}
