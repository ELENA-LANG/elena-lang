//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86jitcompiler.h"
#include "bytecode.h"

using namespace _ELENA_;

// --- ELENA Object constants ---
constexpr int gcPageSize         = 0x0010;           // a heap page size constant
constexpr int elObjectOffset     = 0x0008;           // object header / offset constant

// --- ELENA CORE built-in routines
constexpr int GC_ALLOC           = 0x10001;
constexpr int HOOK               = 0x10010;
constexpr int INIT_RND           = 0x10012;
constexpr int ENDFRAME           = 0x10016;
constexpr int CALC_SIZE          = 0x1001F;
constexpr int GET_COUNT          = 0x10020;
constexpr int THREAD_WAIT        = 0x10021;
constexpr int BREAK              = 0x10026;
constexpr int EXPAND_HEAP        = 0x10028;

constexpr int CORE_GC_TABLE      = 0x20002;
constexpr int CORE_STATICROOT    = 0x20005;
constexpr int CORE_TLS_INDEX     = 0x20007;
constexpr int CORE_THREADTABLE   = 0x20008;
constexpr int CORE_OS_TABLE      = 0x20009;
constexpr int CORE_MESSAGE_TABLE = 0x2000A;
constexpr int CORE_EH_TABLE      = 0x2000B;
constexpr int SYSTEM_ENV         = 0x2000C;
constexpr int VOID               = 0x2000D;
constexpr int VOIDPTR            = 0x2000E;

// preloaded gc routines
const int coreVariableNumber = 3;
const int coreVariables[coreVariableNumber] =
{
   CORE_GC_TABLE, CORE_OS_TABLE, CORE_EH_TABLE
};

const int coreStaticNumber = 2;
const int coreStatics[coreStaticNumber] =
{
   VOID, VOIDPTR
};

// preloaded gc routines
const int coreFunctionNumber = 9;
const int coreFunctions[coreFunctionNumber] =
{
   BREAK, EXPAND_HEAP, GC_ALLOC, HOOK, INIT_RND, ENDFRAME,
   CALC_SIZE, GET_COUNT,
   THREAD_WAIT
};

// preloaded gc commands
const int gcCommandNumber = /*160*/23;
const int gcCommands[gcCommandNumber] =
{
   bcLoadEnv, bcCallExtR, bcSaveSI, bcBSRedirect, bcOpen,
   bcReserve, bcPushS, bcStoreSI, bcPeekSI, bcThrow,
   bcCallVI, bcClose, bcNew, bcFill, bcCallRM,
   bcPeekFI, bcStoreFI, bcAllocI, bcJumpRM, bcVCallRM,
   bcMTRedirect, bcJumpVI, bcXMTRedirect,
   //bcBCopyA, bcParent,
//   bcMIndex,
//   bcASwapSI, bcXIndexRM, bcESwap,
//   bcALoadBI, bcPushAI, bcPushF, ,
//   bcHook, bcUnhook, bcClass, bcACallVD,
//   bcDLoadSI, bcDLoadFI, bcDSaveFI, bcELoadSI,
//   bcEQuit, bcASaveBI, bcESaveSI,
//   bcGet, bcSet, bcXSet, bcACallI, bcBReadB,
//   bcRestore, bcLen, bcIfHeap, bcFlag, bcNCreate,
//   bcBLoadFI, bcAXSaveBI, bcBLoadSI, bcBWriteB,
//   bcNEqual, bcNLess, bcNCopy, bcNAdd, bcBSwapSI,
//   bcNSub, bcNMul, bcNDiv, bcNLoadE, bcDivN,
//   bcWLen, bcNSave, bcNLoad, bcWCreate, bcCopy,
//   bcBCreate, bcBWrite, bcBLen, bcBReadW, bcXLen,
//   bcBRead, bcBSwap, bcDSwapSI, bcESwapSI, bcSNop,
//   bcNAnd, bcNOr, bcNXor, bcTryLock, bcFreeLock,
//   bcLCopy, bcLEqual, bcLLess, bcLAdd, bcRethrow,
//   bcLSub, bcLMul, bcLDiv, bcLAnd, bcLOr,
//   bcLXor, bcNShiftL, bcNNot, bcLShiftL, bcLShiftR,
//   bcLNot, bcRCopy, bcRSave, bcREqual, bcBSaveSI,
//   bcRLess, bcRAdd, bcRSub, bcRMul, bcRDiv,
//   bcCreate, bcExclude, bcDCopyR, bcInclude,
//   bcSelectR, bcNext, bcXSelectR, bcCount,
//   bcRAbs, bcRExp, bcRInt, bcValidate, ,
//   bcRLn, bcRRound, bcRSin, bcRCos, bcRArcTan,
//   bcAddress, bcBWriteW, bcRLoad, bcNLen,
//   bcNRead, bcNWrite, bcNLoadI, bcNSaveI, bcELoadFI,
//   bcESaveFI, bcWRead, bcWWrite, bcNWriteI,
//   bcNCopyB, bcLCopyB, bcCopyB, bcNReadI, bcInit,
//   bcCheck, bcDCopyVerb, bcXCopy,
//   bcSaveFI, bcAddFI, bcSubFI, bcNShiftR, bcLSave,
//   bcSelect, bcEqualR, bcBLoadAI, bcAndE, bcDMoveVerb,
//   bcEOrN, bcNewI, bcACopyAI
};

const int gcCommandExNumber = /*6*/0;
//const int gcCommandExs[gcCommandExNumber] =
//{
//   bcNop // !! temporal
//
////   bcMTRedirect + 0x100, bcXMTRedirect + 0x100,
////   bcMTRedirect + 0x200, bcXMTRedirect + 0x200,
////   bcMTRedirect + 0xC00, bcXMTRedirect + 0xC00
//};

// command table
void (*commands[0x100])(int opcode, x86JITScope& scope) =
{
   &compileNop, &compileBreakpoint, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteOp,
   &compileNop, &compileNop, &compilePushA, &compilePopA, &compileNop, &compileNop, &loadOneByteOp, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteLOp, &compileNop, &compileQuit,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &loadOneByteOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &loadFPOp, &loadIndexOp, &compileNop, &compileNop,
   &compileOpen, &compileQuitN, &compileNop, &compileNop, &compileNop, &compileNop, &compileSetR, &compileMCopy,

   &compileJump, &loadVMTIndexOp, &loadVMTIndexOp, &compileCallR, &compileNop, &loadFunction, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compilePush, &compileNop, &compileNop, &compileNop, &compilePushFI, &compileNop,
   &compileNop, &compileNop, &compilePushSI, &loadIndexOp, &compileNop, &compileNop, &loadIndexOp, &loadIndexOp,

   &compileNop, &compileNop, &compileNop, &loadIndexOp, &loadFPOp, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compilePopN, &compileAllocI, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileMTRedirect, &compileMTRedirect, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileCreate, &compileCreateN, &compileFill, &compileNop, &compileInvokeVMTOffset, & compileInvokeVMT, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileElseR, &compileNop, &compileNop, &compileInvokeVMT, &compileNop,

   //   &compileNop, &compileBreakpoint, &compilePushB, &compilePop, &loadOneByteOp, &compilePushE, &loadMTOp, &loadOneByteOp,
//   &compileDCopyCount, &compileOr, &compilePushA, &compilePopA, &compileACopyB, &compilePopE, &loadOneByteOp, &compileDSetVerb,
//
//   &compileNot, &loadOneByteLOp, &loadOneByteLOp, &compileIndexDec, &compilePopB, &loadOneByteLOp, &compileDSub, &compileQuit,
//   &loadOneByteOp, &loadOneByteOp, &compileIndexInc, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &compileDAdd, &loadOneByteOp,
//
//   &compileECopyD, &compileDCopyE, &compilePushD, &compilePopD, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,
//   &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteLOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,
//
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop,
//
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteOp,
//
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop,
//   &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &loadOneByteOp,
//
//   &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//   &loadOneByteLOp, &loadOneByteOp, &compileNop, &compileNop, &loadOneByteOp, &loadOneByteOp, &compileNop, &loadOneByteOp,
//
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop,
//
//   &loadOneByteLOp, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//   &loadOneByteLOp, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
//
//   &compileDCopy, &compileECopy, &loadIndexOp, &compileALoadR, &loadFPOp, &loadIndexOp, &compileIfHeap, &compileBCopyS,
//   &compileOpen, &compileQuitN, &compileBCopyR, &compileBCopyF, &compileACopyF, &compileACopyS, &compileACopyR, &compileMCopy,
//
//   &compileJump, &loadVMTIndexOp, &loadVMTIndexOp, &compileCallR, &compileJumpN, &loadFunction, &compileHook, &compileHook,
//   &loadIndexOp, &compileLessE, &compileNotLessE, &compileIfB, &compileElseB, &compileIfE, &compileElseE, &compileNext,
//
//   &compilePush, &loadFPOp, &compilePush, &loadIndexOp, &loadIndexOp, &loadFPOp, &compilePushFI, &loadFPOp,
//   &loadIndexOp, &loadFPOp, &compilePushS, &loadIndexOp, &loadIndexOp, &compilePushF, &loadIndexOp, &loadIndexOp,
//
//   &loadIndexOp, &loadIndexOp, &loadIndexOp, &loadIndexOp, &loadFPOp, &loadIndexOp, &loadIndexOp, &loadIndexOp,
//   &loadFPOp, &loadIndexOp, &loadIndexOp, &loadIndexOp, &compileASaveR, &compileALoadAI, &loadIndexOp, &loadIndexOp,
//
//   &compilePopN, &loadIndexOp, &compileSCopyF, &compileSetVerb, &compileDShiftN, &compileDAndN, &compileDAddN, &compileDOrN,
//   &compileEAddN, &compileDShiftN, &compileDMulN, &loadOneByteLOp, &compileBLoadR, &compileInit, &loadROp, &loadIndexOp,
//
//   &loadNOp, &compileCreateI, & loadIndexOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
//   &compileMTRedirect, &compileMTRedirect, &compileGreaterN, &compileGreaterN, &compileLessN, &loadFNOp, &loadFNOp, &loadFNOp,
//
//   &compileCreate, &compileCreateN, &compileNop, &compileSelectR, &compileInvokeVMTOffset, &compileInvokeVMT, &compileSelectR, &compileLessN,
//   &compileIfM, &compileElseM, &compileIfR, &compileElseR, &compileIfN, &compileElseN, &compileInvokeVMT, &compileNop
};

// --- x86JITCompiler commands ---

inline void compileJump(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jmp   lbEnding
   if (!forwardJump) {
      scope.lh.writeJmpBack(label);
   }
   else {
      // if it is forward jump, try to predict if it is short
      if (shortJump) {
         scope.lh.writeShortJmpForward(label);
      }
      else scope.lh.writeJmpForward(label);
   }
}

inline void compileJumpX(x86JITScope& scope, int label, bool forwardJump, bool shortJump, x86Helper::x86JumpType prefix)
{
   if (!forwardJump) {
      scope.lh.writeJxxBack(prefix, label);
   }
   else {
      // if it is forward jump, try to predict if it is short
      if (shortJump) {
         scope.lh.writeShortJxxForward(label, prefix);
      }
      else scope.lh.writeJxxForward(label, prefix);
   }
}

inline void compileJumpIf(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jnz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JNZ);
}

//inline void compileJumpIfNot(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jz   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JZ);
//}
//
//inline void compileJumpAbove(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // ja   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JA);
//}
//
//inline void compileJumpBelow(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jb   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JB);
//}
//
//inline void compileJumpLess(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jl   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JL);
//}
//
//inline void compileJumpNotLess(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jl   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JGE);
//}
//
//inline void compileJumpGreater(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jl   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JG);
//}
//
//inline void compileJumpNotGreater(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jl   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JLE);
//}
//
//inline void compileJumpLessOrEqual(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jle   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JLE);
//}
//
//inline void compileJumpGreaterOrEqual(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jge   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JGE);
//}

void _ELENA_::loadCoreOp(x86JITScope& scope, char* code)
{
   MemoryWriter* writer = scope.code;

   if (code==NULL)
      throw InternalError("Cannot load core command");

   size_t position = writer->Position();
   size_t length = *(size_t*)(code - 4);

   writer->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + offset);

      if ((key & mskTypeMask) == mskPreloaded) {
         scope.compiler->writeCoreReference(scope, key, position, offset, code);
      }
      else {
         scope.helper->writeReference(*writer, key, *(int*)(code + offset), scope.module);
      }

      relocation += 2;
      count--;
   }
   writer->seekEOF();
}

inline void _ELENA_::writeCoreReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   // references should be already preloaded
   if ((reference & mskAnyRef) == mskPreloadRelCodeRef) {
      scope.helper->writeReference(*scope.code,
         scope.compiler->_preloaded.get(reference & ~mskAnyRef), true, *(int*)(code + offset));

      scope.lh.addFixableJump(offset + position, (*scope.code->Memory())[offset + position]);
   }
   else scope.helper->writeReference(*scope.code,
      scope.compiler->_preloaded.get(reference & ~mskAnyRef), false, *(int*)(code + offset));
}

void _ELENA_::loadOneByteOp(int opcode, x86JITScope& scope)
{
   MemoryWriter* writer = scope.code;

   char* code = (char*)scope.compiler->_inlines[opcode];
   size_t position = writer->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   writer->write(code, *(int*)(code - 4));

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + relocation[1]);

      if ((key & mskTypeMask) == mskPreloaded) {
         scope.compiler->writeCoreReference(scope, key, position, offset, code);
      }
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(scope.argument);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(scope.argument << 2);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

//void _ELENA_::loadFNOp(int opcode, x86JITScope& scope)
//{
//   int arg2 = scope.tape->getDWord();
//
//   char*  code = (char*)scope.compiler->_inlines[opcode];
//   size_t position = scope.code->Position();
//   size_t length = *(size_t*)(code - 4);
//
//   // simply copy correspondent inline code
//   scope.code->write(code, length);
//
//   // resolve section references
//   int count = *(int*)(code + length);
//   int* relocation = (int*)(code + length + 4);
//   while (count > 0) {
//      // locate relocation position
//      scope.code->seek(position + relocation[1]);
//
//      if (relocation[0] == -1) {
//         scope.code->writeDWord(-(scope.argument << 2));
//      }
//      else if (relocation[0] == -2) {
//         scope.code->writeDWord(arg2);
//      }
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::loadOneByteLOp(int opcode, x86JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];

   // simply copy correspondent inline code
   scope.code->write(code, *(int*)(code - 4));
}

//void _ELENA_::loadROp(int opcode, x86JITScope& scope)
//{
//   char*  code = (char*)scope.compiler->_inlines[opcode];
//   size_t position = scope.code->Position();
//   size_t length = *(size_t*)(code - 4);
//
//   // simply copy correspondent inline code
//   scope.code->write(code, length);
//
//   // resolve section references
//   int count = *(int*)(code + length);
//   int* relocation = (int*)(code + length + 4);
//   while (count > 0) {
//      // locate relocation position
//      scope.code->seek(position + relocation[1]);
//
//      if (relocation[0]==-1) {
//         if (scope.argument == 0) {
//            scope.code->writeDWord(0);
//         }
//         else scope.writeReference(*scope.code, scope.argument, 0);
//      }
//      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::loadMTOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.writeReference(*scope.code, scope.argument, 0);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(scope.extra_arg);
      }
      else if (relocation[0] == -3) {
         scope.code->writeDWord(scope.extra_arg2);
      }
      else if (relocation[0] == (CORE_MESSAGE_TABLE | mskPreloadDataRef)) {
         scope.helper->writeMTReference(*scope.code);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

//void _ELENA_::loadMTOpX(int opcode, x86JITScope& scope, int prefix)
//{
//   char* code = NULL;
//   for (int i = 0; i < gcCommandExNumber; i++) {
//      if (gcCommandExs[i] == opcode + prefix) {
//         code = (char*)scope.compiler->_inlineExs[i];
//         break;
//      }
//   }
//
//   size_t position = scope.code->Position();
//   size_t length = *(size_t*)(code - 4);
//
//   // simply copy correspondent inline code
//   scope.code->write(code, length);
//
//   // resolve section references
//   int count = *(int*)(code + length);
//   int* relocation = (int*)(code + length + 4);
//   while (count > 0) {
//      // locate relocation position
//      scope.code->seek(position + relocation[1]);
//
//      if (relocation[0] == -1) {
//         scope.writeReference(*scope.code, scope.argument, 0);
//      }
//      else if (relocation[0] == -2) {
//         scope.code->writeDWord(scope.extra_arg);
//      }
//      else if (relocation[0] == (CORE_MESSAGE_TABLE | mskPreloadDataRef)) {
//         scope.helper->writeMTReference(*scope.code);
//      }
//      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::loadIndexOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(scope.argument << 2);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadVMTIndexOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord((scope.argument << 3) + 4);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

////void _ELENA_::loadVMTMIndexOp(int opcode, x86JITScope& scope)
////{
////   char*  code = (char*)scope.compiler->_inlines[opcode];
////   size_t position = scope.code->Position();
////   size_t length = *(size_t*)(code - 4);
////
////   // simply copy correspondent inline code
////   scope.code->write(code, length);
////
////   // resolve section references
////   int count = *(int*)(code + length);
////   int* relocation = (int*)(code + length + 4);
////   while (count > 0) {
////      // locate relocation position
////      scope.code->seek(position + relocation[1]);
////
////      if (relocation[0]==-1) {
////         scope.code->writeDWord(scope.argument << 3);
////      }
////      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
////
////      relocation += 2;
////      count--;
////   }
////   scope.code->seekEOF();
////}

void _ELENA_::loadFPOp(int opcode, x86JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         scope.code->writeDWord(-(scope.argument << 2));
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFunction(int opcode, x86JITScope& scope)
{
   // if it is internal native call
   switch (scope.argument & mskAnyRef) {
      case mskNativeCodeRef:
      case mskPreloadCodeRef:
         compileCallR(opcode, scope);
         return;
   }

   MemoryWriter* writer = scope.code;

   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   int key, offset;
   while (count > 0) {
      key = relocation[0];
      offset = relocation[1];

      // locate relocation position
      writer->seek(position + relocation[1]);

      // !! temporal, more optimal way should be used
      if (relocation[0]==-1) {
         scope.writeReference(*writer, scope.argument, *(int*)(code + offset));
      }
      //else if ((key & mskTypeMask) == mskPreloaded) {
      //   scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      //}
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

//void _ELENA_::loadCode(int opcode, x86JITScope& scope)
//{
//   // if it is a symbol reference
//   if ((scope.argument & mskAnyRef) == mskSymbolRef) {
//      compileCallR(bcCallR, scope);
//   }
//   else {
//      // otherwise a primitive code
//      SectionInfo   info = scope.getSection(scope.argument);
//      MemoryWriter* writer = scope.code;
//
//      // override module
//      scope.module = info.module;
//
//      char*  code = (char*)info.section->get(0);
//      size_t position = scope.code->Position();
//      size_t length = *(size_t*)(code - 4);
//
//      // simply copy correspondent inline code
//      writer->write(code, length);
//
//      // resolve section references
//      int count = *(int*)(code + length);
//      int* relocation = (int*)(code + length + 4);
//      int key, offset;
//      while (count > 0) {
//         key = relocation[0];
//         offset = relocation[1];
//
//         // locate relocation position
//         writer->seek(position + relocation[1]);
//
//         scope.writeReference(*writer, key, *(int*)(code + offset));
//
//         relocation += 2;
//         count--;
//      }
//      // clear module overriding
//      scope.module = NULL;
//      scope.code->seekEOF();
//   }
//}

void _ELENA_::compileNop(int, x86JITScope& scope)
{
   // nop command is used to indicate possible label
   // fix the label if it exists
   if (scope.lh.checkLabel(scope.tape->Position() - 1)) {
      scope.lh.fixLabel(scope.tape->Position() - 1);
   }
   // or add the label
   else scope.lh.setLabel(scope.tape->Position() - 1);
}

void _ELENA_::compileBreakpoint(int, x86JITScope& scope)
{
   if (scope.withDebugInfo)
      scope.helper->addBreakpoint(scope.code->Position());
}

void _ELENA_::compilePush(int opcode, x86JITScope& scope)
{
   // push constant | reference
   scope.code->writeByte(0x68);
   if (opcode == bcPushR && scope.argument != 0) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(scope.argument);
}

//void _ELENA_::compilePopE(int, x86JITScope& scope)
//{
//   // pop ecx
//   scope.code->writeByte(0x59);
//}
//
//void _ELENA_::compilePopD(int, x86JITScope& scope)
//{
//   // pop ebx
//   scope.code->writeByte(0x5B);
//}
//
//void _ELENA_::compileSCopyF(int, x86JITScope& scope)
//{
//   // lea esp, [ebp - level * 4]
//
//   x86Helper::leaRM32disp(scope.code, x86Helper::otESP, x86Helper::otEBP, -(scope.argument << 2));
//}
//
//void _ELENA_::compileALoadAI(int, x86JITScope& scope)
//{
//   // mov eax, [eax + __arg * 4]
//
//   scope.code->writeWord(0x808B);
//   scope.code->writeDWord(scope.argument << 2);
//}

void _ELENA_::compilePushSI(int, x86JITScope& scope)
{
   // push [esp+offset]
   scope.code->writeWord(0xB4FF);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileJump(int, x86JITScope& scope)
{
   ::compileJump(scope, scope.tape->Position() + scope.argument, (scope.argument > 0), (__abs(scope.argument) < 0x10));
}

//void _ELENA_::compileHook(int opcode, x86JITScope& scope)
//{
//   if (scope.argument < 0) {
//      scope.lh.writeLoadBack(scope.tape->Position() + scope.argument);
//   }
//   else scope.lh.writeLoadForward(scope.tape->Position() + scope.argument);
//
//   loadOneByteOp(opcode, scope);
//}

void _ELENA_::compileOpen(int opcode, x86JITScope& scope)
{
   loadOneByteLOp(opcode, scope);

   //scope.prevFSPOffs += (scope.argument << 2);
}

void _ELENA_::compileQuit(int, x86JITScope& scope)
{
   scope.code->writeByte(0xC3);
}

void _ELENA_::compileQuitN(int, x86JITScope& scope)
{
   scope.code->writeByte(0xC2);
   scope.code->writeWord((unsigned short)(scope.argument << 2));
}

//void _ELENA_::compileNext(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//  // test upper boundary
//   loadOneByteLOp(opcode, scope);
//
//  // try to use short jump if offset small (< 0x10?)
//   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileIfE(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp ecx, ebx
//   scope.code->writeWord(0xCB3B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileElseE(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp ecx, ebx
//   scope.code->writeWord(0xCB3B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileLessE(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp ebx, ecx
//   scope.code->writeWord(0xD93B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileNotLessE(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp ebx, ecx
//   scope.code->writeWord(0xD93B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpGreaterOrEqual(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileIfB(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp eax, edi
//   scope.code->writeWord(0xC73B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileElseB(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp eax, edi
//   scope.code->writeWord(0xC73B);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileIfM(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//   int message = scope.resolveMessage(scope.argument);
//
//   // cmp ecx, message
//   scope.code->writeWord(0xF981);
//   scope.code->writeDWord(message);
//
//   // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileElseM(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//   int message = scope.resolveMessage(scope.argument);
//
//   // cmp ecx, message
//   scope.code->writeWord(0xF981);
//   scope.code->writeDWord(message);
//
//  // try to use short jump if offset small (< 0x10?)
//   //NOTE: due to compileJumpX implementation - compileJumpIf is called
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileIfHeap(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // load bottom boundary
//   loadOneByteOp(opcode, scope);
//
//   // jz short label
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}

void _ELENA_::compileFill(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t r = scope.argument;
   scope.argument = scope.tape->getDWord();

   if (r == 0) {
      // xor eax, eax
      scope.code->writeWord(0xC033);
   }
   else {
      // mov eax, r
      scope.code->writeByte(0xB8);
      scope.writeReference(*scope.code, r, 0);
   }

   if (scope.argument == 1) {
      // mov [ebx], eax
      scope.code->writeWord(0x0389);
   }
   else loadNOp(opcode, scope);
}

void _ELENA_::compileCreate(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   scope.argument <<= 2;
   int length = scope.argument;

   // __arg1 = #gc_page + (length - 1)
   scope.argument = align(scope.argument + scope.objectSize, gcPageSize);

   loadNOp(opcode, scope);

   // NOTE : empty length should be equal to 800000h
   // due to current GC algorithm
   if (length == 0)
      length = 0x800000;

   // mov [ebx-8], length
   scope.code->writeWord(0x43C7);
   scope.code->writeByte(0xF8);
   scope.code->writeDWord(length);

   if (vmtRef) {
      // ; set vmt reference
      // mov [ebx-4], vmt
      scope.code->writeWord(0x43C7);
      scope.code->writeByte(0xFC);
      scope.writeReference(*scope.code, vmtRef, 0);
   }
}

void _ELENA_::compileCreateN(int opcode, x86JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   int length = scope.argument | 0x800000; // mark object as a binary structure

   // __arg1 = #gc_page + (length - 1)
   scope.argument = align(scope.argument + scope.objectSize, gcPageSize);

   loadNOp(bcNew, scope);

   // mov [ebx-8], length
   scope.code->writeWord(0x43C7);
   scope.code->writeByte(0xF8);
   scope.code->writeDWord(length);

   if (vmtRef) {
      // ; set vmt reference
      // mov [ebx-4], vmt
      scope.code->writeWord(0x43C7);
      scope.code->writeByte(0xFC);
      scope.writeReference(*scope.code, vmtRef, 0);
   }
}

//void _ELENA_::compileCreateI(int opcode, x86JITScope& scope)
//{
//   int size = align(scope.argument + scope.objectSize, gcPageSize);
//
//   scope.argument |= 0x800000;  // mark object as a binary structure
//
//   // mov  ecx, #gc_page + (size - 1)
//   scope.code->writeByte(0xB9);
//   scope.code->writeDWord(size);
//
//   loadNOp(opcode, scope);
//}
//
//void _ELENA_::compileSelectR(int opcode, x86JITScope& scope)
//{
//   // HOT FIX : reverse the argument order
//   ref_t r1 = scope.argument;
//   scope.argument = scope.tape->getDWord();
//
//   if (opcode == bcSelectR) {
//      // mov  eax, r1
//      scope.code->writeByte(0xB8);
//      scope.writeReference(*scope.code, r1, 0);
//   }
//   else {
//      // mov  edx, r1
//      scope.code->writeByte(0xBA);
//      scope.writeReference(*scope.code, r1, 0);
//   }
//
//   loadROp(opcode, scope);
//}

void _ELENA_::compileSetR(int, x86JITScope& scope)
{
   // mov ebx, r
   scope.code->writeByte(0xBB);
   if (scope.argument == 0) {
      scope.code->writeDWord(0);
   }
   else if (scope.argument == -1) {
      scope.code->writeDWord((pos_t)-1);
   }
   else scope.writeReference(*scope.code, scope.argument, 0);
}

//void _ELENA_::compileBCopyR(int, x86JITScope& scope)
//{
//   // mov edi, r
//   scope.code->writeByte(0xBF);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}
//
//void _ELENA_::compileDCopy(int, x86JITScope& scope)
//{
//   // mov ebx, i
//   scope.code->writeByte(0xBB);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileECopy(int, x86JITScope& scope)
//{
//   // mov ecx, i
//   scope.code->writeByte(0xB9);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileDAndN(int, x86JITScope& scope)
//{
//   // and ebx, mask
//   scope.code->writeWord(0xE381);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileDOrN(int, x86JITScope& scope)
//{
//   // or ebx, mask
//   scope.code->writeWord(0xCB81);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileDAddN(int, x86JITScope& scope)
//{
//   // add ebx, n
//   scope.code->writeWord(0xC381);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileDMulN(int, x86JITScope& scope)
//{
//   // mov  esi, scope.argument
//   scope.code->writeByte(0xBE);
//   scope.code->writeDWord(scope.argument);
//
//   // imul ebx, esi
//   scope.code->writeWord(0xAF0F);
//   scope.code->writeByte(0xDE);
//}
//
//void _ELENA_::compileDSub(int, x86JITScope& scope)
//{
//   // sub ebx, ecx
//   scope.code->writeWord(0xD92B);
//}
//
//void _ELENA_::compileEAddN(int, x86JITScope& scope)
//{
//   // add ecx, n
//   scope.code->writeWord(0xC181);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileDCopyCount(int, x86JITScope& scope)
//{
//   // and ebx, VERB_MASK
//   scope.code->writeWord(0xD98B);
//   scope.code->writeWord(0xE381);
//   scope.code->writeDWord(PARAM_MASK);
//}
//
//void _ELENA_::compileDSetVerb(int, x86JITScope& scope)
//{
//   // and ecx, PARAM_MASK | ACTION_MASK
//   // mov edx, ebx
//   // shl edx, ACTION_ORDER
//   // or  ecx, edx
//   scope.code->writeWord(0xE181);
//   scope.code->writeDWord(PARAM_MASK | ACTION_MASK);
//   scope.code->writeWord(0xD38B);
//   scope.code->writeWord(0xE2C1);
//   scope.code->writeByte(ACTION_ORDER);
//   scope.code->writeWord(0xCA0B);
//}
//
////void _ELENA_::compileLoad(int opcode, x86JITScope& scope)
////{
////   // mov eax, [edi + esi*4]
////   scope.code->writeWord(0x048B);
////   scope.code->writeByte(0xB7);
////}
//
//void _ELENA_::compileALoadR(int, x86JITScope& scope)
//{
//   // mov eax, [r]
//   scope.code->writeByte(0xA1);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}
//
//void _ELENA_ :: compileBLoadR(int, x86JITScope& scope)
//{
//   // mov edi, [r]
//   scope.code->writeWord(0x3D8B);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}

void _ELENA_::compilePushA(int, x86JITScope& scope)
{
   // push ebx
   scope.code->writeByte(0x53);
}

void _ELENA_::compilePushFI(int, x86JITScope& scope)
{
   scope.code->writeWord(0xB5FF);
   // push [ebp-level*4]
   scope.code->writeDWord(-(scope.argument << 2));
}

//void _ELENA_:: compilePushF(int, x86JITScope& scope)
//{
//   scope.argument = -(scope.argument << 2);
//
//   loadNOp(bcPushF, scope);
//}
//
//void _ELENA_::compilePushB(int, x86JITScope& scope)
//{
//   // push edi
//   scope.code->writeByte(0x57);
//}

//void _ELENA_::compilePushE(int, x86JITScope& scope)
//{
//   // push ecx
//   scope.code->writeByte(0x51);
//}

//void _ELENA_::compilePushD(int, x86JITScope& scope)
//{
//   // push ebx
//   scope.code->writeByte(0x53);
//}

void _ELENA_::compileCallR(int, x86JITScope& scope)
{
   // call symbol
   scope.code->writeByte(0xE8);
   scope.writeReference(*scope.code, scope.argument | mskRelCodeRef, 0);
}

//void _ELENA_::compileJumpN(int, x86JITScope& scope)
//{
//   // jmp [eax+i]
//   scope.code->writeWord(0x60FF);
//   scope.code->writeByte(scope.argument << 2);
//}
//
//void _ELENA_::compilePop(int, x86JITScope& scope)
//{
//   // pop edx
//   scope.code->writeByte(0x5A);
//}

void _ELENA_::compilePopA(int, x86JITScope& scope)
{
   // pop ebx
   scope.code->writeByte(0x5B);
}

void _ELENA_::compileMCopy(int, x86JITScope& scope)
{
   // mov edx, message
   scope.code->writeByte(0xBA);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compilePopN(int, x86JITScope& scope)
{
   // add esp, arg
   int arg = scope.argument << 2;
   if (arg < 0x80) {
      scope.code->writeWord(0xC483);
      scope.code->writeByte(scope.argument << 2);
   }
   else {
      scope.code->writeWord(0xC481);
      scope.code->writeDWord(scope.argument << 2);
   }
}

void _ELENA_ :: compileAllocI(int opcode, x86JITScope& scope)
{
   // sub esp, __arg1 * 4
   int arg = scope.argument << 2;
   if (arg < 0x80) {
      scope.code->writeWord(0xEC83);
      scope.code->writeByte(scope.argument << 2);
   }
   else {
      scope.code->writeWord(0xEC81);
      scope.code->writeDWord(scope.argument << 2);
   }

   loadNOp(opcode, scope);
}

//void _ELENA_::compileACopyB(int, x86JITScope& scope)
//{
//   // mov eax, edi
//   scope.code->writeWord(0xF889);
//}
//
//void _ELENA_::compileASaveR(int, x86JITScope& scope)
//{
//   // mov [ref], eax
//
//   scope.code->writeWord(0x0589);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}
//
//void _ELENA_::compileIndexInc(int, x86JITScope& scope)
//{
//   // add ebx, 1
//   scope.code->writeWord(0xC383);
//   scope.code->writeByte(1);
//}
//
//void _ELENA_::compileIndexDec(int, x86JITScope& scope)
//{
//   // sub ebx, 1
//   scope.code->writeWord(0xEB83);
//   scope.code->writeByte(1);
//}

void _ELENA_::compileInvokeVMTOffset(int opcode, x86JITScope& scope)
{
   int message = scope.resolveMessage(scope.tape->getDWord());

   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         // resolve message offset
         scope.writeReference(*scope.code, scope.argument | mskVMTEntryOffset, message);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::compileInvokeVMT(int opcode, x86JITScope& scope)
{
   int message = scope.resolveMessage(scope.tape->getDWord());

   char*  code = (char*)scope.compiler->_inlines[opcode];
   size_t position = scope.code->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0]==-1) {
         // resolve message offset
         scope.writeReference(*scope.code, scope.argument | mskVMTMethodAddress, message);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

//void _ELENA_::compileACopyS(int, x86JITScope& scope)
//{
//   // lea eax, [esp + index]
//   x86Helper::leaRM32disp(
//      scope.code, x86Helper::otEAX, x86Helper::otESP, scope.argument << 2);
//}
//
//void _ELENA_::compileIfR(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp eax, r
//   // jz lab
//
//   scope.code->writeByte(0x3D);
//   // HOTFIX : support zero references
//   if (scope.argument == 0) {
//      scope.code->writeDWord(0);
//   }
//   else if (scope.argument == -1) {
//      scope.code->writeDWord((pos_t)-1);
//   }
//   else scope.writeReference(*scope.code, scope.argument, 0);
//   //NOTE: due to compileJumpX implementation - compileJumpIf is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}

void _ELENA_::compileElseR(int, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp ebx, r
   // jz lab

   scope.code->writeWord(0xFB81);
   // HOTFIX : support zero references
   if (scope.argument != 0) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(0);

   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

//void _ELENA_::compileIfN(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp ebx, n
//   // jz lab
//
//   scope.code->writeWord(0xFB81);
//   scope.code->writeDWord(scope.argument);
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileElseN(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp ebx, n
//   // jnz lab
//
//   scope.code->writeWord(0xFB81);
//   scope.code->writeDWord(scope.argument);
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileLessN(int op, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp ebx, n
//   // jz lab
//   scope.code->writeWord(0xFB81);
//   scope.code->writeDWord(scope.argument);
//   if (op == bcLessN) {
//      compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//   }
//   else compileJumpNotLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileGreaterN(int op, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp ebx, n
//   // jz lab
//   scope.code->writeWord(0xFB81);
//   scope.code->writeDWord(scope.argument);
//   if (op == bcGreaterN) {
//      compileJumpGreater(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//   }
//   else compileJumpNotGreater(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}

void _ELENA_::compileMTRedirect(int op, x86JITScope& scope)
{
   scope.extra_arg = scope.tape->getDWord();
   scope.extra_arg2 = getArgCount(scope.extra_arg);

   int startArg = 1;
   if (test(scope.extra_arg, FUNCTION_MESSAGE)) {
      startArg = 0;
      scope.extra_arg2++;
   }
//   else scope.extra_arg = 4;

   // ; lea  eax, [esp + offs]
   x86Helper::leaRM32disp(scope.code, x86Helper::otEAX, x86Helper::otESP, startArg << 2);

//   if (test(message, VARIADIC_MESSAGE)) {
//      loadMTOpX(op, scope, 0xC00);
//   }
//   else {
      switch (getArgCount(scope.extra_arg) - startArg) {
         //case 1:
         //   loadMTOpX(op, scope, 0x100);
         //   break;
         //case 2:
         //   loadMTOpX(op, scope, 0x200);
         //   break;
         default:
            loadMTOp(op, scope);
            break;
      }
//   }
}

//void _ELENA_::compileSetVerb(int, x86JITScope& scope)
//{
//   // and ecx, PARAM_MASK | ACTION_MASK
//   // or  ecx, m
//   scope.code->writeWord(0xE181);
//   scope.code->writeDWord(PARAM_MASK | ACTION_MASK);
//   scope.code->writeWord(0xC981);
//   scope.code->writeDWord(scope.resolveMessage(encodeAction(scope.argument)));
//}
//
//void _ELENA_::compilePopB(int, x86JITScope& scope)
//{
//   // pop edi
//   scope.code->writeByte(0x5F);
//}
//
//void _ELENA_::compileECopyD(int, x86JITScope& scope)
//{
//   // mov ecx, ebx
//   scope.code->writeWord(0xCB8B);
//}
//
//void _ELENA_::compileDCopyE(int, x86JITScope& scope)
//{
//   // mov ebx, ecx
//   scope.code->writeWord(0xD98B);
//}
//
//void _ELENA_::compileBCopyF(int, x86JITScope& scope)
//{
//   // lea edi, [ebp+nn]
//   scope.code->writeWord(0xBD8D);
//   scope.code->writeDWord(-(scope.argument << 2));
//}
//
//void _ELENA_::compileBCopyS(int, x86JITScope& scope)
//{
//   // lea edi, [esp+nn]
//   scope.code->writeWord(0xBC8D);
//   scope.code->writeByte(0x24);
//   scope.code->writeDWord(-(scope.argument << 2));
//}
//
//void _ELENA_::compileACopyF(int, x86JITScope& scope)
//{
//   // lea eax, [ebp+nn]
//   scope.code->writeWord(0x858D);
//   scope.code->writeDWord(-(scope.argument << 2));
//}
//
//void _ELENA_ :: compileNot(int, x86JITScope& scope)
//{
//   // not ebx
//   scope.code->writeWord(0xD3F7);
//}
//
//void _ELENA_::compileInit(int opcode, x86JITScope& scope)
//{
//   if (scope.argument == 1) {
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//   }
//   else if (scope.argument == 2) {
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//   }
//   else if (scope.argument == 3) {
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//      scope.code->writeByte(0x68);
//      scope.code->writeDWord(0);
//   }
//   else loadNOp(opcode, scope);
//}
//
//void _ELENA_ :: compileDShiftN(int op, x86JITScope& scope)
//{
//   if (op == bcShiftLN) {
//      // shl ebx, n
//      scope.code->writeWord(0xE3C1);
//      scope.code->writeByte((unsigned char)scope.argument);
//   }
//   else {
//      // shr ebx, n
//      scope.code->writeWord(0xEBC1);
//      scope.code->writeByte((unsigned char)scope.argument);
//   }
//}
//
//void _ELENA_::compileDAdd(int, x86JITScope& scope)
//{
//   // add ebx, ecx
//   scope.code->writeWord(0xD903);
//}
//
//void _ELENA_::compileOr(int, x86JITScope& scope)
//{
//   // or ebx, ecx
//   scope.code->writeWord(0xD90B);
//}

// --- x86JITScope ---

x86JITScope :: x86JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler)
   : lh(code)
{
   this->tape = tape;
   this->code = code;
   this->helper = helper;
   this->compiler = compiler;
   this->withDebugInfo = compiler->isWithDebugInfo();
   this->objectSize = helper ? helper->getLinkerConstant(lnObjectSize) : 0;
   this->module = NULL;
   this->extra_arg = 0;
   this->extra_arg2 = 0;

//   this->prevFSPOffs = 0;
}

void x86JITScope :: writeReference(MemoryWriter& writer, ref_t reference, size_t disp)
{
   // HOTFIX : mskLockVariable used to fool trylock opcode, adding virtual offset
   if ((reference & mskAnyRef) == mskLockVariable) {
      disp += compiler->getObjectHeaderSize();
   }

   helper->writeReference(writer, reference, disp, module);
}

// --- x86JITCompiler ---

x86JITCompiler :: x86JITCompiler(bool debugMode)
   : _inlineExs(NULL, gcCommandExNumber << 2)
{
   _debugMode = debugMode;
}

size_t x86JITCompiler :: getObjectHeaderSize() const
{
   return elObjectOffset;
}

bool x86JITCompiler :: isWithDebugInfo() const
{
   // in the current implementation, debug info (i.e. debug section)
   // is always generated (to be used by RTManager)
   return true;
}

void x86JITCompiler :: alignCode(MemoryWriter* writer, int alignment, bool code)
{
   writer->align(alignment, code ? 0x90 : 0x00);
}

void x86JITCompiler :: writeCoreReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   if (!_preloaded.exist(reference& ~mskAnyRef)) {
      MemoryWriter writer(scope.code->Memory());

      _preloaded.add(reference & ~mskAnyRef, scope.helper->getVAddress(writer, mskCodeRef));

      // due to optimization section must be ROModule::ROSection instance
      SectionInfo info = scope.helper->getCoreSection(reference & ~mskAnyRef);
      // separate scope should be used to prevent overlapping
      x86JITScope newScope(NULL, &writer, scope.helper, this);
      newScope.module = info.module;

      loadCoreOp(newScope, info.section ? (char*)info.section->get(0) : NULL);
   }
   _ELENA_::writeCoreReference(scope, reference, position, offset, code);
}

inline void loadCoreData(_ReferenceHelper& helper, x86JITScope& dataScope, ref_t reference)
{
   // due to optimization section must be ROModule::ROSection instance
   SectionInfo info = helper.getCoreSection(reference);
   dataScope.module = info.module;

   loadCoreOp(dataScope, info.section ? (char*)info.section->get(0) : NULL);
}

void x86JITCompiler :: prepareCore(_ReferenceHelper& helper, _JITLoader* loader)
{
   // preload core data
   _Memory* data = loader->getTargetSection((ref_t)mskDataRef);
   _Memory* rdata = loader->getTargetSection((ref_t)mskRDataRef);
   _Memory* sdata = loader->getTargetSection((ref_t)mskStatRef);
   _Memory* code = loader->getTargetSection((ref_t)mskCodeRef);

   MemoryWriter dataWriter(data);
   MemoryWriter rdataWriter(rdata);
   MemoryWriter sdataWriter(sdata);
   MemoryWriter codeWriter(code);

   // preloaded variables
    x86JITScope dataScope(NULL, &dataWriter, &helper, this);
   for (int i = 0; i < coreVariableNumber; i++) {
      if (!_preloaded.exist(coreVariables[i])) {
         _preloaded.add(coreVariables[i], helper.getVAddress(dataWriter, mskDataRef));

         loadCoreData(helper, dataScope, coreVariables[i]);
      }
   }

   // MESSAGE TABLE POINTER
   _preloaded.add(CORE_MESSAGE_TABLE, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeDWord(0);

   // load GC static root
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // SYSTEM_ENV
   x86JITScope rdataScope(NULL, &rdataWriter, &helper, this);
   _preloaded.add(SYSTEM_ENV, helper.getVAddress(rdataWriter, mskRDataRef));
   loadCoreData(helper, rdataScope, SYSTEM_ENV);
   // NOTE : the table is tailed with GCMGSize,GCYGSize,MaxThread and DebugPtr fields
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCMGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCYGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnThreadCount));

   // resolve reference to SYSTEM_ENV at rdata header
   rdata->addReference((ref_t)_preloaded.get(SYSTEM_ENV), 0);

   // lnVMAPI_Instance
   dataWriter.writeDWord(helper.getLinkerConstant(lnVMAPI_Instance)); // ??

   // preloaded core static variables
   for (int i = 0; i < coreStaticNumber; i++) {
      if (!_preloaded.exist(coreStatics[i])) {
         _preloaded.add(coreStatics[i], helper.getVAddress(rdataWriter, mskRDataRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getCoreSection(coreStatics[i]);
         rdataScope.module = info.module;

         loadCoreOp(rdataScope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // preloaded core functions
   x86JITScope scope(NULL, &codeWriter, &helper, this);
   for (int i = 0; i < coreFunctionNumber; i++) {
      if (!_preloaded.exist(coreFunctions[i])) {
         _preloaded.add(coreFunctions[i], helper.getVAddress(codeWriter, mskCodeRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getCoreSection(coreFunctions[i]);
         scope.module = info.module;

         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // preload vm commands
   for (int i = 0; i < gcCommandNumber; i++) {
      SectionInfo info = helper.getCoreSection(gcCommands[i]);

      // due to optimization section must be ROModule::ROSection instance
      _inlines[gcCommands[i]] = (char*)info.section->get(0);
   }

   // preload vm exended commmands
   for (int i = 0; i < gcCommandExNumber; i++) {
      //SectionInfo info = helper.getCoreSection(gcCommandExs[i]);
      //_inlineExs.add(gcCommandExs[i], info.section->get(0));
   }
}

void x86JITCompiler :: setStaticRootCounter(_JITLoader* loader, size_t counter, bool virtualMode)
{
   if (virtualMode) {
      _Memory* data = loader->getTargetSection(mskRDataRef);

      size_t offset = ((size_t)_preloaded.get(SYSTEM_ENV) & ~mskAnyRef);
      (*data)[offset] = (counter << 2);
   }
   else {
 	   size_t offset = (size_t)_preloaded.get(SYSTEM_ENV);
 	   *(int*)offset = (counter << 2);
   }
}

//void x86JITCompiler :: setTLSKey(void* ptr)
//{
//   _preloaded.add(CORE_TLS_INDEX, ptr);
//}
//
//void x86JITCompiler :: setThreadTable(void* ptr)
//{
//   _preloaded.add(CORE_THREADTABLE, ptr);
//}
//
//void x86JITCompiler :: setEHTable(void* ptr)
//{
//   _preloaded.add(CORE_EH_TABLE, ptr);
//}
//
//void x86JITCompiler :: setGCTable(void* ptr)
//{
//   _preloaded.add(CORE_GC_TABLE, ptr);
//}

void* x86JITCompiler :: getPreloadedReference(ref_t reference)
{
   return (void*)_preloaded.get(reference);
}

void x86JITCompiler :: allocateThreadTable(_JITLoader* loader, int maxThreadNumber)
{
   // get target image & resolve virtual address
   MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskDataRef));

   // size place holder
   dataWriter.writeDWord(0);
   int position = dataWriter.Position();
   if (maxThreadNumber > 0) {
      // reserve space for the thread table      
      allocateArray(dataWriter, maxThreadNumber);
   }

   // map thread table
   loader->mapReference(ReferenceInfo(GC_THREADTABLE), (void*)(position | mskDataRef), (ref_t)mskDataRef);
   _preloaded.add(CORE_THREADTABLE, (void*)(position | mskDataRef));
}

int x86JITCompiler :: allocateTLSVariable(_JITLoader* loader)
{
   MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskDataRef));

   // reserve space for TLS index
   int position = dataWriter.Position();
   allocateVariable(dataWriter);

   // map TLS index
   loader->mapReference(ReferenceInfo(TLS_KEY), (void*)(position | mskDataRef), (ref_t)mskDataRef);
   _preloaded.add(CORE_TLS_INDEX, (void*)(position | mskDataRef));

   return position;
}

//int x86JITCompiler :: allocateVMTape(_JITLoader* loader, void* tape, pos_t length)
//{
//   MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskRDataRef));
//
//   // reserve space for TLS index
//   int position = dataWriter.Position();
//
//   dataWriter.write(tape, length);
//
//   // map VMTape
//   loader->mapReference(ReferenceInfo(TAPE_KEY), (void*)(position | mskRDataRef), (ref_t)mskRDataRef);
//
//   return position;
//}

inline void compileTape(MemoryReader& tapeReader, size_t endPos, x86JITScope& scope)
{
   unsigned char code = 0;
   while(tapeReader.Position() < endPos) {
      // read bytecode + arguments
      code = tapeReader.getByte();
      // preload an argument if a command requires it
      if (code > MAX_SINGLE_ECODE) {
         scope.argument = tapeReader.getDWord();
      }
      commands[code](code, scope);
   }
}

void x86JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this);

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   // ; copy the parameter to the accumulator to simulate embedded symbol
   // ; exit the procedure
   // ret
   codeWriter.writeByte(0xC3);

   alignCode(&codeWriter, 0x04, true);
}

void x86JITCompiler :: compileProcedure(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this);
//   scope.prevFSPOffs = 4;

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   alignCode(&codeWriter, 0x04, true);
}

//void x86JITCompiler :: loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section)
//{
//   size_t position = writer.Position();
//
//   writer.write(section->get(0), section->Length());
//
//   // resolve section references
//   _ELENA_::RelocationMap::Iterator it(section->getReferences());
//   while (!it.Eof()) {
//      int arg = *it;
//      writer.seek(arg + position);
//
//      ident_t reference = binary->resolveReference(it.key() & ~mskAnyRef);
//
//      helper.writeReference(writer, reference, it.key() & mskAnyRef);
//
//      it++;
//   }
//   writer.seekEOF();
//}

void x86JITCompiler :: generateSymbolCall(MemoryDump& tape, void* address)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeByte(bcCallR);
   ecodes.writeDWord((size_t)address | mskCodeRef);
}

//void x86JITCompiler :: generateArg(MemoryDump& tape, void* address)
//{
//   MemoryWriter ecodes(&tape);
//
//   ecodes.writeByte(bcPushR);
//   ecodes.writeDWord((size_t)address | mskRDataRef);
//}
//
//void x86JITCompiler :: generateExternalCall(MemoryDump& tape, ref_t functionReference)
//{
//   MemoryWriter ecodes(&tape);
//
//   ecodes.writeByte(bcCallExtR);
//   ecodes.writeDWord(functionReference | mskImportRef);
//}