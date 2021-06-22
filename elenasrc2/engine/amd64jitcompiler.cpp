//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86-64
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

// !! NOTE : ELENA 64 will not maintain 16 byte stack alignment due to the current call conventions 
//           and some language features (like a command tape). 
//           The alignment will be maintained only for external operations (inside exclude / include brackets)

// !! NOTE : R15 register must be preserved

#include "elena.h"
// --------------------------------------------------------------------------
#include "amd64jitcompiler.h"
#include "bytecode.h"
#include "core.h"

//#pragma warning(disable : 4100)

using namespace _ELENA_;

// preloaded gc routines
const int coreVariableNumber = 2;
const int coreVariables[coreVariableNumber] =
{
   CORE_GC_TABLE, CORE_EH_TABLE
};

const int coreStaticNumber = 2;
const int coreStatics[coreStaticNumber] =
{
   VOIDOBJ, VOIDPTR
};

// preloaded env routines
const int envFunctionNumber = 1;
const int envFunctions[envFunctionNumber] =
{
   INVOKER
};

// preloaded gc routines
const int coreFunctionNumber = 7;
const int coreFunctions[coreFunctionNumber] =
{
   GC_ALLOC, HOOK, INIT_RND,
   CALC_SIZE, GET_COUNT,
   THREAD_WAIT, GC_ALLOCPERM
};

// preloaded gc commands
const int gcCommandNumber = 167;
const int gcCommands[gcCommandNumber] =
{
   bcLoadEnv, bcCallExtR, bcSaveSI, bcBSRedirect, bcOpen,
   bcReserve, bcPushSIP, bcStoreSI, bcPeekSI, bcThrow,
   bcCallVI, bcClose, bcNew, bcFillRI, bcCallRM,
   bcPeekFI, bcStoreFI, bcAllocI, bcJumpRM, bcVCallRM,
   bcMTRedirect, bcJumpVI, bcXMTRedirect, bcRestore, bcPushF,
   bcCopyF, bcCopyFI, bcNAddF, bcCopyToF, bcCopyToFI,
   bcNSubF, bcNMulF, bcNDivF, bcPushAI, bcGetI,
   bcSetI, bcCopyToAI, bcCreate, bcFillR, bcXSetI,
   bcXSetFI, bcClass, bcXSaveF, bcLen, bcSave,
   bcSelect, bcNEqual, bcNLess, bcSNop, bcCreateN,
   bcSaveF, bcTryLock, bcLoad, bcHook, bcUnhook,
   bcFlag, bcFreeLock, bcGet, bcNShlF, bcNShrF,
   bcMovN, bcCloneF, bcInc, bcRead, bcExclude,
   bcInclude, bcCopyTo, bcReadToF, bcXWrite, bcDiv,
   bcLoadFI, bcEqual, bcNAndF, bcNOrF, bcNXorF,
   bcCoalesce, bcCoalesceR, bcXSelectR, bcLAddF, bcLSubF,
   bcLMulF, bcLDivF, bcLShlF, bcLShrF, bcLAndF,
   bcLOrF, bcLXorF, bcLEqual, bcLLess, bcSaveI,
   bcLoadI, bcRAddF, bcRSubF, bcRMulF, bcRDivF,
   bcREqual, bcRLess, bcRSet, bcRSave, bcRSaveN,
   bcRIntF, bcRLoad, bcClone, bcAddF, bcSubF,
   bcAddress, bcLoadSI, bcLoadVerb, bcSetV, bcCount,
   bcSet, bcPushVerb, bcPush, bcMQuit, bcStoreV,
   bcRExp, bcRSin, bcRArcTan, bcRLn, bcRRound,
   bcRInt, bcRCos, bcMove, bcMoveTo, bcCopyAI,
   bcXSaveAI, bcXSave, bcSaveFI, bcXLoad, bcXAddF,
   bcMul, bcXOr, bcPeek, bcSwap, bcXCreate,
   bcIfHeap, bcEqualFI, bcLoadF, bcRSaveL, bcRAbs,
   bcCallI, bcIfCount, bcSub, bcSwapD, bcXSet,
   bcMIndex, bcParent, bcCheckSI, bcLSave,
   bcRAddNF, bcRSubNF, bcRMulNF, bcRDivNF, bcXRSaveF,
   bcXRedirect, bcXVRedirect, bcVJumpRM, bcAllocN, bcXNew,
   bcXSaveSI, bcAllocD, bcXSetR, bcXTrans, bcLLoad,
   bcFreeD, bcStore, bcMovFIPD,
};

const int gcCommandExNumber = 58;
const int gcCommandExs[gcCommandExNumber] =
{
   bcMTRedirect + 0x100, bcXMTRedirect + 0x100,
   bcMTRedirect + 0x200, bcXMTRedirect + 0x200,
   bcMTRedirect + 0xC00, bcXMTRedirect + 0xC00,
   bcCreateN + 0x100, bcCreateN + 0x200, bcCreateN + 0x300, bcCreateN + 0x400,
   bcXWrite + 0x100, bcXWrite + 0x200, bcXWrite + 0x300, bcXWrite + 0x400,
   bcReadToF + 0x100, bcReadToF + 0x200, bcReadToF + 0x300, bcReadToF + 0x400,
   bcCopyTo + 0x100, bcCopyTo + 0x200, bcCopyTo + 0x300, bcCopyTo + 0x400,
   bcCopyToF + 0x100, bcCopyToF + 0x200, bcCopyToF + 0x300, bcCopyToF + 0x400,
   bcCopyToFI + 0x100, bcCopyToFI + 0x200, bcCopyToFI + 0x300, bcCopyToFI + 0x400,
   bcCopyFI + 0x100, bcCopyFI + 0x200, bcCopyFI + 0x300, bcCopyFI + 0x400,
   bcCopyF + 0x100, bcCopyF + 0x200, bcCopyF + 0x300, bcCopyF + 0x400,
   bcCopyAI + 0x100, bcCopyAI + 0x200, bcCopyAI + 0x300, bcCopyAI + 0x400,
   bcCopyToAI + 0x100, bcCopyToAI + 0x200, bcCopyToAI + 0x300, bcCopyToAI + 0x400,
   bcMove + 0x100, bcMove + 0x200, bcMove + 0x300, bcMove + 0x400,
   bcMoveTo + 0x100, bcMoveTo + 0x200, bcMoveTo + 0x300, bcMoveTo + 0x400,
   bcCallExtR + 0x100, bcCallExtR + 0x200, bcCallExtR + 0x300,
   bcXSaveSI + 0x100  
};

// command table
void (*commands[0x100])(int opcode, I64JITScope& scope) =
{
   &compileNop, &compileBreakpoint, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,
   &compileDCopyCount, &loadOneByteOp, &compilePushA, &compilePopA, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,

   &compileNot, &compileOpen, &compilePop, &loadOneByteOp, &loadOneByteOp, &loadOneByteLOp, &loadOneByteOp, &compileQuit,
   &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,

   &loadOneByteOp, &loadOneByteOp, &compilePushD, &compilePopD, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,
   &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,

   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteOp, &loadOneByteLOp, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteOp,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &compileNop,

   &loadOneByteLOp, &loadOneByteLOp, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp,
   &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &loadOneByteOp,

   &loadFOp, &loadFOp, &loadFOp, &loadFOp, &loadFOp, &loadFPOp, &compileNop, &compileNop,
   &compileNop, &compileNop, &loadNOp, &loadNOp, &loadNOpX, &loadN4OpX, &loadFOp, &loadFOp,

   &loadNOp, &loadIndexOp, &compileXRedirect, &compileXRedirect, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &loadFOp, &loadFOp, &loadFOp, &loadFOp,
   &loadFOp, &loadFOp, &loadFOp, &loadFOp, &compileNop, &loadFOp, &compileNop, &compileNop,

   &loadFOp, &loadFOp, &loadFOp, &loadFPOp, &loadFPOp, &loadFOp, &loadFOp, &loadFOp,
   &loadFOp, &loadFOp, &compileNop, &compileNop, &compileNop, &compileNop, &loadFOp, &compileNop,

   &compileDec, &loadIndexOp, &compileRestore, &compileALoadR, &loadFPOp, &loadIndexOp, &compileIfHeap, &loadIndexOp,
   &compileNop, &compileQuitN, &loadROp, &loadROp, &compileACopyF, &compileACopyS, &compileSetR, &compileMCopy,

   &compileJump, &loadVMTIndexOp, &loadVMTIndexOp, &compileCallR, &compileJumpN, &compileSetFrame, &compileHook, &compileHook,
   &loadIndexOp, &compileNop, &compileNotLessE, &compileNotGreaterE, &compileElseD, &compileIfE, &compileElseE, &compileIfCount,

   &compilePush, &loadNOp, &compilePush, &loadFPOp, &loadIndexOp, &loadFOp, &compilePushFI, &loadFPOp,
   &loadIndexOp, &loadFOp, &compilePushSI, &loadIndexOp, &loadFPOp, &compilePushF, &loadSPOp, &compileReserve,

   &loadIndexOp, &compileACopyF, &compilePushF, &loadIndexOp, &loadFPOp, &loadFOp, &loadFOp, &loadROp,
   &loadFOp, &loadFOp, &loadIndexOp, &loadIndexOp, &compileASaveR, &loadNOp, &loadFOp, &loadNOp,

   &compilePopN, &compileAllocI, &loadROp, &compileMovV, &compileDShiftN, &compileDAndN, &loadNOp, &compileDOrN,
   &loadROp, &compileDShiftN, &compileSaveLen, &compileInvokeVMTOffset, &loadIndexNOp, &loadIndexN4OpX, &loadNNOpX, &loadNNOpX,

   &loadFN4OpX, &compileDynamicCreateN, &loadFPIndexOp, &loadIndexN4OpX, &loadFPN4OpX, &loadFN4OpX, &loadFPN4OpX, &loadFN4OpX,
   &compileMTRedirect, &compileMTRedirect, &compileGreaterN, &compileGreaterN, &compileLessN, &loadFNOp, &loadFNOp, &loadFNOp,

   &compileCreate, &compileCreateN, &compileFill, &compileSelectR, &compileInvokeVMTOffset, &compileInvokeVMT, &compileSelectR, &compileLessN,
   &compileCreateN, &loadIndexNOp, &compileIfR, &compileElseR, &compileIfN, &compileElseN, &compileInvokeVMT, &loadFunction,
};

constexpr int FPOffset = 0xC;

// --- I64JITCompiler commands ---

inline void compileJump(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
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

inline void compileJumpX(I64JITScope& scope, int label, bool forwardJump, bool shortJump, AMD64Helper::AMD64JumpType prefix)
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

inline void compileJumpIf(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jnz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JNZ);
}

inline void compileJumpIfNot(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JZ);
}

////inline void compileJumpAbove(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
////{
////   // ja   lbEnding
////   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JA);
////}
////
////inline void compileJumpBelow(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
////{
////   // jb   lbEnding
////   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JB);
////}

inline void compileJumpGreater(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jg   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JG);
}

inline void compileJumpLess(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JL);
}

inline void compileJumpNotLess(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jnl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JGE);
}

inline void compileJumpNotGreater(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JLE);
}

inline void compileJumpLessOrEqual(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jle   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JLE);
}

inline void compileJumpGreaterOrEqual(I64JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jge   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, AMD64Helper::JUMP_TYPE_JGE);
}

void _ELENA_::compileJump(int, I64JITScope& scope)
{
   ::compileJump(scope, scope.tape->Position() + scope.argument, (scope.argument > 0), (__abs(scope.argument) < 0x10));
}

void _ELENA_::loadCoreOp(I64JITScope& scope, char* code)
{
   MemoryWriter* writer = scope.code;

   if (code == NULL)
      throw InternalError("Cannot load core command");

   pos_t position = writer->Position();
   pos_t length = *(pos_t*)(code - 4);

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
         //if ((key & mskAnyRef) == mskLinkerConstant) {
         //   scope.code->writeDWord(scope.helper->getLinkerConstant(key & ~mskAnyRef));
         //}
         /*else */scope.helper->writeReference(*writer, key, *(int*)(code + offset), scope.module);
      }

      relocation += 2;
      count--;
   }
   writer->seekEOF();
}

inline void _ELENA_::writeCoreReference(I64JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   // references should be already preloaded
   if ((reference & mskAnyRef) == mskPreloadRelCodeRef) {
      scope.helper->writeRelVAddress(*scope.code,
         scope.compiler->_preloaded.get(reference & ~mskAnyRef), mskRelCodeRef, *(int*)(code + offset));

      scope.lh.addFixableJump(offset + position, (*scope.code->Memory())[offset + position]);
   }
   else if ((reference & mskAnyRef) == mskPreloadRelDataRef) {
      scope.helper->writeRelVAddress(*scope.code,
         scope.compiler->_preloaded.get(reference & ~mskAnyRef), mskNativeRelDataRef, *(int*)(code + offset));
   }
   else scope.helper->writeVAddress(*scope.code,
      scope.compiler->_preloaded.get(reference & ~mskAnyRef), *(int*)(code + offset));
}


inline void loadCoreData(_ReferenceHelper& helper, I64JITScope& dataScope, ref_t reference)
{
   // due to optimization section must be ROModule::ROSection instance
   SectionInfo info = helper.getCoreSection(reference);
   dataScope.module = info.module;

   loadCoreOp(dataScope, info.section ? (char*)info.section->get(0) : NULL);
}

inline void loadRoutines(int functionNumber, const int* functions, I64JITScope& scope,
   IntFixedMap<lvaddr_t>& preloaded)
{
   for (int i = 0; i < functionNumber; i++) {
      if (!preloaded.exist(functions[i])) {

         preloaded.add(functions[i], scope.helper->getVAddress(*scope.code, mskCodeRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = scope.helper->getCoreSection(functions[i]);
         scope.module = info.module;

         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }
}

void _ELENA_::loadOneByteOp(int opcode, I64JITScope& scope)
{
   MemoryWriter* writer = scope.code;

   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = writer->Position();
   pos_t length = *(pos_t*)(code - 4);

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

void _ELENA_::loadOneByteLOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];

   // simply copy correspondent inline code
   scope.code->write(code, *(pos_t*)(code - 4));
}


void _ELENA_::loadROp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         if (scope.argument == 0) {
            scope.code->writeDWord(0);
         }
         else if (scope.argument == -1) {
            scope.code->writeDWord(INVALID_REF);
         }
         else scope.writeReference(*scope.code, scope.argument, 0);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadIndexOp(int opcode, I64JITScope& scope)
{
   char*  code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}


void _ELENA_::loadIndexNOp(int opcode, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord();

   char* code = (char*)scope.compiler->_inlines[opcode];
   if (arg2 == -1) {
      for (int i = 0; i < gcCommandExNumber; i++) {
         if (gcCommandExs[i] == opcode + 0x100) {
            code = (char*)scope.compiler->_inlineExs[i];
            break;
         }
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadIndexN4OpX(int opcode, I64JITScope& scope, int prefix)
{
   int arg2 = scope.tape->getDWord();
   char* code = NULL;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadIndexN4OpX(int opcode, I64JITScope& scope)
{
   pos_t pos = scope.tape->Position();
   int arg2 = scope.tape->getDWord();
   scope.tape->seek(pos);

   switch (arg2) {
      case 1:
         loadIndexN4OpX(opcode, scope, 0x100);
         break;
      case 2:
         loadIndexN4OpX(opcode, scope, 0x200);
         break;
      case 3:
         loadIndexN4OpX(opcode, scope, 0x300);
         break;
      case 4:
         loadIndexN4OpX(opcode, scope, 0x400);
         break;
      default:
         loadIndexNOp(opcode, scope);
         break;
   }
}

void _ELENA_::loadVMTIndexOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord((scope.argument << 4) + 8);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

inline int getFPOffset(int argument, int argOffset)
{
   return -(argument - (argument < 0 ? argOffset : 0));
}

void _ELENA_::loadFNOp(int opcode, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord();

   loadFNOp(opcode, scope, arg2);
}

void _ELENA_::loadFNOp(int opcode, I64JITScope& scope, int arg2)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument, FPOffset));
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFPOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadSPOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument, FPOffset));
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFN4OpX(int opcode, I64JITScope& scope, int prefix)
{
   int arg2 = scope.tape->getDWord();
   char* code = nullptr;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument, FPOffset));
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFN4OpX(int opcode, I64JITScope& scope)
{
   pos_t pos = scope.tape->Position();
   int arg2 = scope.tape->getDWord();
   scope.tape->seek(pos);

   switch (arg2) {
   case 1:
      loadFN4OpX(opcode, scope, 0x100);
      break;
   case 2:
      loadFN4OpX(opcode, scope, 0x200);
      break;
   case 3:
      loadFN4OpX(opcode, scope, 0x300);
      break;
   case 4:
      loadFN4OpX(opcode, scope, 0x400);
      break;
   default:
      loadFNOp(opcode, scope);
      break;
   }
}

void _ELENA_::loadFPIndexOp(int opcode, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord();

   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2 << 3);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFPN4OpX(int opcode, I64JITScope& scope, int prefix)
{
   int arg2 = scope.tape->getDWord();
   char* code = nullptr;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFPN4Op(int opcode, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord();
   char* code = (char*)scope.compiler->_inlines[opcode];

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFPN4OpX(int opcode, I64JITScope& scope)
{
   pos_t pos = scope.tape->Position();
   int arg2 = scope.tape->getDWord();
   scope.tape->seek(pos);

   switch (arg2) {
      case 1:
         loadFPN4OpX(opcode, scope, 0x100);
         break;
      case 2:
         loadFPN4OpX(opcode, scope, 0x200);
         break;
      case 3:
         loadFPN4OpX(opcode, scope, 0x300);
         break;
      case 4:
         loadFPN4OpX(opcode, scope, 0x400);
         break;
      default:
         loadFPN4Op(opcode, scope);
         break;
   }
}

void _ELENA_::loadNOpX(int opcode, I64JITScope& scope, int prefix)
{
   char* code = nullptr;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

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
      else if (relocation[0] == (CORE_MESSAGE_TABLE | mskPreloadDataRef)) {
         scope.helper->writeMTReference(*scope.code);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNOpX(int opcode, I64JITScope& scope)
{
   switch (scope.argument) {
   case 1:
      loadNOpX(opcode, scope, 0x100);
      break;
   case 2:
      loadNOpX(opcode, scope, 0x200);
      break;
   case 4:
      loadNOpX(opcode, scope, 0x300);
      break;
   case 8:
      loadNOpX(opcode, scope, 0x400);
      break;
   default:
      loadNOp(opcode, scope);
      break;
   }
}

void _ELENA_::loadN4OpX(int opcode, I64JITScope& scope)
{
   switch (scope.argument) {
      case 1:
         loadNOpX(opcode, scope, 0x100);
         break;
      case 2:
         loadNOpX(opcode, scope, 0x200);
         break;
      case 3:
         loadNOpX(opcode, scope, 0x300);
         break;
      case 4:
         loadNOpX(opcode, scope, 0x400);
         break;
      default:
         loadNOp(opcode, scope);
         break;
   }
}

void _ELENA_::loadMTOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

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

void _ELENA_::loadMTOpX(int opcode, I64JITScope& scope, int prefix)
{
   char* code = NULL;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

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

void freeStack64(int args, MemoryWriter* code)
{
   // add esp, arg
   if (args < 0x80) {
      code->writeByte(0x48);
      code->writeWord(0xC483);
      code->writeByte(args);
   }
   else {
      code->writeByte(0x48);
      code->writeWord(0xC481);
      code->writeDWord(args);
   }
}

void _ELENA_::compilePop(int, I64JITScope& scope)
{
   freeStack64(8, scope.code);
}

void _ELENA_::compilePopN(int, I64JITScope& scope)
{
   if (scope.argument)
      freeStack64(align(scope.argument << 3, 16), scope.code);
}

void _ELENA_::loadFunction(int opcode, I64JITScope& scope)
{
   int flags = scope.tape->getDWord();

   int argsToFree = 0;
   if (test(flags, baReleaseArgs))
      argsToFree = flags & baCallArgsMask;

   // if it is internal native call
   switch (scope.argument & mskAnyRef) {
      case mskNativeCodeRef:
      case mskPreloadCodeRef:
         compileCallR(opcode, scope);

         if (argsToFree)
            freeStack64(argsToFree << 3, scope.code);

         return;
   }

   MemoryWriter* writer = scope.code;

   char*  code = (char*)scope.compiler->_inlines[opcode];
   int prefix = 0;
   if (test(flags, baExternalCall)) {
      switch (flags & baCallArgsMask) {
         case 1:
            prefix = 0x100;
            argsToFree += 3;
            break;
         case 2:
            prefix = 0x200;
            argsToFree += 2;
            break;
         case 3:
            prefix = 0x300;
            argsToFree += 1;
            break;
         default:
            break;
      }
   }

   if (prefix) {
      for (int i = 0; i < gcCommandExNumber; i++) {
         if (gcCommandExs[i] == opcode + prefix) {
            code = (char*)scope.compiler->_inlineExs[i];
            break;
         }
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

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
      if (relocation[0] == -1) {
         scope.writeReference(*writer, scope.argument | mskRelImportRef, *(int*)(code + offset));
      }
      //else if ((key & mskTypeMask) == mskPreloaded) {
      //   scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      //}
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();

   if (argsToFree)
      freeStack64(align(argsToFree << 3, 16), scope.code);
}

void _ELENA_::loadNOp(int opcode, I64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(scope.argument << 3);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNNOp(int opcode, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord();

   char* code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNNOpX(int opcode, I64JITScope& scope, int prefix)
{
   int arg2 = scope.tape->getDWord();

   char* code = NULL;
   for (int i = 0; i < gcCommandExNumber; i++) {
      if (gcCommandExs[i] == opcode + prefix) {
         code = (char*)scope.compiler->_inlineExs[i];
         break;
      }
   }

   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         scope.code->writeDWord(scope.argument);
      }
      else if (relocation[0] == -2) {
         scope.code->writeDWord(arg2);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadNNOpX(int opcode, I64JITScope& scope)
{
   pos_t pos = scope.tape->Position();
   int arg2 = scope.tape->getDWord();
   scope.tape->seek(pos);

   switch (arg2) {
      case 1:
         loadNNOpX(opcode, scope, 0x100);
         break;
      case 2:
         loadNNOpX(opcode, scope, 0x200);
         break;
      case 4:
         loadNNOpX(opcode, scope, 0x300);
         break;
      case 8:
         loadNNOpX(opcode, scope, 0x400);
         break;
      default:
         loadNNOp(opcode, scope);
         break;
   }
}

void _ELENA_::compileNop(int, I64JITScope& scope)
{
   // nop command is used to indicate possible label
   // fix the label if it exists
   if (scope.lh.checkLabel(scope.tape->Position() - 1)) {
      scope.lh.fixLabel(scope.tape->Position() - 1);
   }
   // or add the label
   else scope.lh.setLabel(scope.tape->Position() - 1);
}

void _ELENA_::compileBreakpoint(int, I64JITScope& scope)
{
   if (scope.withDebugInfo)
      scope.helper->addBreakpoint(scope.code->Position());
}

void _ELENA_::compilePush(int opcode, I64JITScope& scope)
{
   //if (opcode == bcPushR && scope.bigAddressMode && scope.argument) {
   //   scope.code->writeByte(??);
   //   scope.writeReference(*scope.code, scope.argument, 0);
   //   scope.code->writeDWord(0);
   //}
   //else {
      // push constant | reference
   if (scope.argument == -1)
      scope.argument = -1;

      scope.code->writeByte(0x68);
      if (opcode == bcPushR && scope.argument != 0) {
         scope.writeReference(*scope.code, scope.argument, 0);
      }
      else scope.code->writeDWord(scope.argument);
//   }
}

void _ELENA_::compilePopD(int, I64JITScope& scope)
{
   // pop rdx
   scope.code->writeByte(0x5A);
}

void _ELENA_::compileHook(int opcode, I64JITScope& scope)
{
   if (scope.argument < 0) {
      scope.lh.writeLoadBack(scope.tape->Position() + scope.argument);
   }
   else scope.lh.writeLoadForward(scope.tape->Position() + scope.argument);

   loadOneByteOp(opcode, scope);
}

void _ELENA_::compileQuit(int, I64JITScope& scope)
{
   scope.code->writeByte(0xC3);
}

void _ELENA_::compileQuitN(int, I64JITScope& scope)
{
   scope.code->writeByte(0xC2);
   scope.code->writeWord((unsigned short)(scope.argument << 3));
}

void _ELENA_::compileMTRedirect(int op, I64JITScope& scope)
{
   scope.extra_arg = scope.tape->getDWord();
   scope.extra_arg2 = getArgCount(scope.extra_arg);

   int startArg = 1;
   if (test(scope.extra_arg, FUNCTION_MESSAGE)) {
      startArg = 0;
      scope.extra_arg2++;
   }
   //   else scope.extra_arg = 4;

      // ; lea  rax, [rsp + offs]
   AMD64Helper::leaRM64disp(scope.code, AMD64Helper::otRAX, AMD64Helper::otRSP, startArg << 3);

   if ((scope.extra_arg & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      loadMTOpX(op, scope, 0xC00);
   }
   else {
      switch (getArgCount(scope.extra_arg) - startArg) {
      case 1:
         loadMTOpX(op, scope, 0x100);
         break;
      case 2:
         loadMTOpX(op, scope, 0x200);
         break;
      default:
         loadMTOp(op, scope);
         break;
      }
   }
}

void _ELENA_::compileCreate(int opcode, I64JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   scope.argument <<= 3;
   int length = scope.argument;

   // __arg1 = #gc_page + (length - 1)
   scope.argument = align(scope.argument + scope.objectSize, gcPageSize64);

   loadNOp(opcode, scope);

   // NOTE : empty length should be equal to 800000h
   // due to current GC algorithm
   if (length == 0)
      length = elStructMask64;

   // mov dword ptr [rbx-elPageSizeOffset], length
   scope.code->writeWord(0x43C7);
   scope.code->writeByte((unsigned char)-elPageSizeOffset64);
   scope.code->writeDWord(length);

   if (vmtRef) {
      // ; set vmt reference
      // mov [rbx-elPageVMTOffset], vmt
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x43C7);
      scope.code->writeByte((unsigned char)-elPageVMTOffset64);
      scope.writeReference(*scope.code, vmtRef, 0);
   }
}

void _ELENA_::compileCreateN(int, I64JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord();

   int length = scope.argument | elStructMask64; // mark object as a binary structure

   // __arg1 = #gc_page + (length - 1)
   scope.argument = align(scope.argument + scope.objectSize, gcPageSize64);

   loadNOp(bcNew, scope);

   // mov dword ptr [rbx-elPageSizeOffset], length
   scope.code->writeWord(0x43C7);
   scope.code->writeByte((unsigned char)-elPageSizeOffset64);
   scope.code->writeDWord(length);

   if (vmtRef) {
      // ; set vmt reference
      // mov [rbx-elPageVMTOffset], vmt
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x43C7);
      scope.code->writeByte((unsigned char)-elPageVMTOffset64);
      scope.writeReference(*scope.code, vmtRef, 0);
   }
}

void _ELENA_::compileDynamicCreateN(int opcode, I64JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t vmtRef = scope.argument;
   scope.argument = scope.tape->getDWord(); // item size

   switch (scope.argument) {
   case 1:
      loadNOpX(opcode, scope, 0x100);
      break;
   case 2:
      loadNOpX(opcode, scope, 0x200);
      break;
   case 4:
      loadNOpX(opcode, scope, 0x300);
      break;
   case 8:
      loadNOpX(opcode, scope, 0x400);
      break;
   default:
      loadNOp(opcode, scope);
      break;
   }

   if (vmtRef) {
      // ; set vmt reference
      // mov [rbx-elVMT], vmt
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x43C7);
      scope.code->writeByte((unsigned char)(-elPageVMTOffset64));
      scope.writeReference(*scope.code, vmtRef, 0);
   }
}

void _ELENA_::compileSelectR(int opcode, I64JITScope& scope)
{
   // HOT FIX : reverse the argument order
   ref_t r1 = scope.argument;
   scope.argument = scope.tape->getDWord();

   if (opcode == bcSelect) {
      // mov ebx, r1
      scope.code->writeByte(0xBB);
      scope.writeReference(*scope.code, r1, 0);
   }
   else {
      // mov  eax, r1
      scope.code->writeByte(0xB8);
      scope.writeReference(*scope.code, r1, 0);
   }

   loadROp(opcode, scope);
}


void _ELENA_::compileSetR(int, I64JITScope& scope)
{
   // mov rbx, r
   if (scope.argument == -1 || !scope.argument) {
      scope.code->writeByte(0xBB);
      scope.code->writeDWord((pos_t)scope.argument);
   }
   else if (scope.bigAddressMode) {
      scope.code->writeByte(0x48);
      scope.code->writeByte(0xBB);
      scope.writeReference(*scope.code, scope.argument, 0);
      scope.code->writeDWord(0);
   }
   else {
      scope.code->writeByte(0xBB);
      scope.writeReference(*scope.code, scope.argument, 0);
   }
}

void _ELENA_::compileDAndN(int, I64JITScope& scope)
{
   // and edx, mask
   scope.code->writeWord(0xE281);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileDec(int, I64JITScope& scope)
{
   // sub rdx, n
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xEA81);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileDCopyCount(int, I64JITScope& scope)
{
   // and rdx, ARG_MASK
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xE281);
   scope.code->writeDWord(ARG_MASK);
}

inline void writeDisp32(I64JITScope& scope, pos_t disp)
{
   switch (scope.argument & mskAnyRef) {
   case mskStatSymbolRef:
      scope.writeReference(*scope.code, scope.argument | mskStatSymbolRelRef, disp);
      break;
   default:
      scope.writeReference(*scope.code, scope.argument, disp);
      break;
   }
}

void _ELENA_::compileASaveR(int, I64JITScope& scope)
{
   // mov [ref], rbx
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x1D89);
   writeDisp32(scope, 0);
}

void _ELENA_::compileALoadR(int, I64JITScope& scope)
{
   // mov rbx, [r]
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x1D8B);
   writeDisp32(scope, 0);
}

void _ELENA_::compilePushA(int, I64JITScope& scope)
{
   // push ebx
   scope.code->writeByte(0x53);
}

void _ELENA_::compilePushFI(int, I64JITScope& scope)
{
   scope.code->writeWord(0xB5FF);
   // push [rbp-level*4]
   scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
}

void _ELENA_::compilePushSI(int, I64JITScope& scope)
{
   // push [rsp+offset]
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xB4FF);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(scope.argument << 3);
}

void _ELENA_::compilePushF(int op, I64JITScope& scope)
{
   if (op == bcPushFIP) {
      scope.argument = getFPOffset(scope.argument << 3, scope.frameOffset);
   }
   else scope.argument = getFPOffset(scope.argument, FPOffset);

   loadNOp(bcPushF, scope);
}

void _ELENA_::compilePushD(int, I64JITScope& scope)
{
   // push rdx
   scope.code->writeByte(0x52);
}

void _ELENA_::compileCallR(int, I64JITScope& scope)
{
   // call symbol
   scope.code->writeByte(0xE8);
   scope.writeReference(*scope.code, scope.argument | mskRelCodeRef, 0);
}

void _ELENA_::compilePopA(int, I64JITScope& scope)
{
   // pop rbx
   scope.code->writeByte(0x5B);
}

void _ELENA_::compileIfE(int, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp dword ptr[rbx], edx
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x1339);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfCount(int opcode, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   loadOneByteOp(opcode, scope);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileElseE(int, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp dword ptr[rbx], edx
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x1339);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileDOrN(int, I64JITScope& scope)
{
   // or edx, mask
   scope.code->writeWord(0xCA81);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileXRedirect(int op, I64JITScope& scope)
{
   // should be always used for a function message
   int startArg = 0;

   // ; lea  rax, [rsp + offs]
   AMD64Helper::leaRM64disp(scope.code, AMD64Helper::otRAX, AMD64Helper::otRSP, startArg << 3);

   // HOTFIX : to adjust the command for loadMTOp
   scope.extra_arg = scope.argument << 3;

   loadMTOp(op, scope);
}

void _ELENA_::compileInvokeVMTOffset(int opcode, I64JITScope& scope)
{
   mssg_t message = scope.resolveMessage(scope.tape->getDWord());

   char*  code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         // resolve message offset
         scope.writeReference(*scope.code, scope.argument | mskVMTEntryOffset, message);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::compileFill(int opcode, I64JITScope& scope)
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
      // mov [rbx], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x0389);
   }
   else if (scope.argument == 2) {
      // mov [rbx], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x0389);
      // mov [rbx+8], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(8);
   }
   else if (scope.argument == 3) {
      // mov [rbx], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x0389);
      // mov [rbx+8], eax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(8);
      // mov [rbx+16], eax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(16);
   }
   else if (scope.argument == 4) {
      // mov [rbx], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x0389);
      // mov [rbx+8], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(8);
      // mov [rbx+16], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(16);
      // mov [rbx+24], rax
      scope.code->writeByte(0x48);
      scope.code->writeWord(0x4389);
      scope.code->writeByte(24);
   }
   else loadNOp(opcode, scope);
}

void _ELENA_::compileInvokeVMT(int opcode, I64JITScope& scope)
{
   ref_t message = /*fromMessage64(*/scope.resolveMessage(scope.tape->getDWord())/*)*/;

   char*  code = (char*)scope.compiler->_inlines[opcode];
   pos_t position = scope.code->Position();
   pos_t length = *(pos_t*)(code - 4);

   // simply copy correspondent inline code
   scope.code->write(code, length);

   // resolve section references
   int count = *(int*)(code + length);
   int* relocation = (int*)(code + length + 4);
   while (count > 0) {
      // locate relocation position
      scope.code->seek(position + relocation[1]);

      if (relocation[0] == -1) {
         // resolve message offset
         scope.writeReference(*scope.code, (scope.argument & ~mskAnyRef) | mskVMTMethodAddress, message);
      }

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::compileACopyS(int, I64JITScope& scope)
{
   // lea ebx, [esp + index]
   AMD64Helper::leaRM64disp(
      scope.code, AMD64Helper::otEBX, AMD64Helper::otRSP, scope.argument << 3);
}

void _ELENA_::compileIfHeap(int opcode, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // load bottom boundary
   loadOneByteOp(opcode, scope);

   // jz short label
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileIfR(int, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp rbx, r
   // jz lab

   scope.code->writeByte(0x48);
   scope.code->writeWord(0xFB81);
   // HOTFIX : support zero references
   if (scope.argument == 0) {
      scope.code->writeDWord(0);
   }
   else if (scope.argument == -1) {
      scope.code->writeDWord(INVALID_REF);
   }
   else scope.writeReference(*scope.code, scope.argument, 0);
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileElseD(int, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp rdx, [rsp]
   scope.code->writeByte(0x48);
   scope.code->writeByte(0x3B);
   scope.code->writeWord(0x2414);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileElseR(int, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp rbx, r
   // jz lab

   scope.code->writeByte(0x48);
   scope.code->writeWord(0xFB81);
   // HOTFIX : support zero references
   if (scope.argument == -1) {
      scope.code->writeDWord((pos_t)-1);
   }
   else if (scope.argument == 0) {
      scope.code->writeDWord(0);
   }
   else scope.writeReference(*scope.code, scope.argument, 0);

   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileIfN(int, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, n
   // jz lab

   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(scope.argument);
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileElseN(int, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, n
   // jnz lab

   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(scope.argument);
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileNotLessE(int, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp dword ptr[ebx], edx
   scope.code->writeWord(0x1339);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpGreaterOrEqual(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileNotGreaterE(int, I64JITScope& scope)
{
   int jumpOffset = scope.argument;

   // cmp dword ptr[ebx], edx
   scope.code->writeWord(0x1339);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpLessOrEqual(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileGreaterN(int op, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, n
   // jz lab
   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(scope.argument);
   if (op == bcGreaterN) {
      compileJumpGreater(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
   }
   else compileJumpNotGreater(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileLessN(int op, I64JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, n
   // jz lab
   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(scope.argument);
   if (op == bcLessN) {
      compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
   }
   else compileJumpNotLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileJumpN(int, I64JITScope& scope)
{
   // jmp [rbx+i]
   scope.code->writeWord(0x63FF);
   scope.code->writeByte(scope.argument << 3);
}

void _ELENA_::compileACopyF(int op, I64JITScope& scope)
{
   // lea ebx, [ebp+nn]
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x9D8D);
   if (op == bcMovFIP) {
      scope.code->writeDWord(getFPOffset(scope.argument << 3, scope.frameOffset));
   }
   else scope.code->writeDWord(getFPOffset(scope.argument, FPOffset));
}

void _ELENA_::compileMovV(int, I64JITScope& scope)
{
   // and edx, ARG_MASK | ACTION_MASK
   // or  edx, m
   scope.code->writeWord(0xE281);
   scope.code->writeDWord(ARG_MASK | ACTION_MASK);
   scope.code->writeWord(0xCA81);
   scope.code->writeDWord(scope.resolveMessage(encodeAction(scope.argument)));
}

void _ELENA_::compileNot(int, I64JITScope& scope)
{
   // not rdx
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xD2F7);
}

void _ELENA_::compileDShiftN(int op, I64JITScope& scope)
{
   if (op == bcShl) {
      // shl edx, n
      scope.code->writeWord(0xE2C1);
      scope.code->writeByte((unsigned char)scope.argument);
   }
   else {
      // shr edx, n
      scope.code->writeWord(0xEAC1);
      scope.code->writeByte((unsigned char)scope.argument);
   }
}

void _ELENA_::compileAllocI(int opcode, I64JITScope& scope)
{
   scope.argument = align(scope.argument, 2);

   switch (scope.argument) {
      case 0:
         break;
      case 1:
         scope.code->writeByte(0x68);
         scope.code->writeDWord(0);
         break;
      case 2:
         scope.code->writeByte(0x68);
         scope.code->writeDWord(0);
         scope.code->writeByte(0x68);
         scope.code->writeDWord(0);
         break;
      default:
      {
         // sub rsp, __arg1 * 4
         int arg = scope.argument << 3;
         if (arg < 0x80) {
            scope.code->writeByte(0x48);
            scope.code->writeWord(0xEC83);
            scope.code->writeByte(scope.argument << 3);
         }
         else {
            scope.code->writeByte(0x48);
            scope.code->writeWord(0xEC81);
            scope.code->writeDWord(scope.argument << 3);
         }

         loadNOp(opcode, scope);
         break;
      }
   }
}

void _ELENA_::compileMCopy(int, I64JITScope& scope)
{
   // mov edx, message
   scope.code->writeByte(0xBA);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compileRestore(int op, I64JITScope& scope)
{
   scope.argument += 16; // include EIP & EBP 

   scope.argument = align(scope.argument, 16);

   loadNOp(op, scope);
}

void _ELENA_::compileOpen(int opcode, I64JITScope& scope)
{
   loadOneByteLOp(opcode, scope);

   scope.frameOffset = 8;
}

void _ELENA_::compileSetFrame(int, I64JITScope& scope)
{
   scope.frameOffset = 8 + align(scope.argument + 16, 16);
}

void _ELENA_::compileReserve(int op, I64JITScope& scope)
{
   scope.argument = align(scope.argument, 16);

   // include raw data frame header
   scope.frameOffset += align(scope.argument + 16, 16);

   loadNOp(op, scope);
}

void _ELENA_::compileSaveLen(int, I64JITScope& scope)
{
   int arg2 = scope.tape->getDWord() | elStructMask64;

   loadFNOp(bcXSaveF, scope, arg2);
}

// --- AMD64JITScope ---

I64JITScope :: I64JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, 
   I64JITCompiler* compiler, bool bigAddressMode)
   : lh(code)
{
   this->tape = tape;
   this->code = code;
   this->helper = helper;
   this->compiler = compiler;
   this->withDebugInfo = compiler->isWithDebugInfo();
   this->objectSize = helper ? helper->getLinkerConstant(lnObjectSize) : 0;
   this->module = nullptr;
   this->frameOffset = 0;
   this->bigAddressMode = bigAddressMode;
   this->argument = 0;
   this->extra_arg = 0;
   this->extra_arg2 = 0;
}

void I64JITScope::writeReference(MemoryWriter& writer, ref_t reference, pos_t disp)
{
   // HOTFIX : mskLockVariable used to fool trylock opcode, adding virtual offset
   if ((reference & mskAnyRef) == mskLockVariable) {
      disp += compiler->getObjectHeaderSize();
   }

   helper->writeReference(writer, reference, disp, module);
}

//void I64JITScope :: writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp)
//{
//   helper->writeXReference(writer, reference, disp, module);
//}

// --- I64JITCompiler ---

I64JITCompiler :: I64JITCompiler(bool debugMode, bool bigAddressMode)
   : _inlineExs(nullptr, gcCommandExNumber << 2)
{
   _debugMode = debugMode;
   _bigAddressMode = bigAddressMode;
}

pos_t I64JITCompiler:: getObjectHeaderSize() const
{
   return elObjectOffset64;
}

bool I64JITCompiler:: isWithDebugInfo() const
{
   // in the current implementation, debug info (i.e. debug section)
   // is always generated (to be used by RTManager)
   return true;
}

void I64JITCompiler:: alignCode(MemoryWriter* writer, int alignment, bool code)
{
   writer->align(alignment, code ? 0x90 : 0x00);
}

void I64JITCompiler :: writeCoreReference(I64JITScope& scope, ref_t reference, pos_t position, int offset, char* code)
{
   if (!_preloaded.exist(reference& ~mskAnyRef)) {
      MemoryWriter writer(scope.code->Memory());

      _preloaded.add(reference & ~mskAnyRef, scope.helper->getVAddress(writer, mskCodeRef));

      // due to optimization section must be ROModule::ROSection instance
      SectionInfo info = scope.helper->getCoreSection(reference & ~mskAnyRef);
      // separate scope should be used to prevent overlapping
      I64JITScope newScope(NULL, &writer, scope.helper, this, _bigAddressMode);
      newScope.module = info.module;

      loadCoreOp(newScope, info.section ? (char*)info.section->get(0) : NULL);
   }
   _ELENA_::writeCoreReference(scope, reference, position, offset, code);
}

void I64JITCompiler :: prepareCore(_ReferenceHelper& helper, _JITLoader* loader)
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
   I64JITScope dataScope(NULL, &dataWriter, &helper, this, _bigAddressMode);
   for (int i = 0; i < coreVariableNumber; i++) {
      if (!_preloaded.exist(coreVariables[i])) {
         _preloaded.add(coreVariables[i], helper.getVAddress(dataWriter, mskDataRef));

         loadCoreData(helper, dataScope, coreVariables[i]);
      }
   }

   // MESSAGE TABLE POINTER
   _preloaded.add(CORE_MESSAGE_TABLE, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeQWord(0);

   // load GC static root
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // STAT COUNT
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // HOTFIX : preload invoker
   I64JITScope scope(NULL, &codeWriter, &helper, this, _bigAddressMode);
   loadRoutines(envFunctionNumber, envFunctions, scope, _preloaded);

   // SYSTEM_ENV
   I64JITScope rdataScope(NULL, &rdataWriter, &helper, this, _bigAddressMode);
   _preloaded.add(SYSTEM_ENV, helper.getVAddress(rdataWriter, mskRDataRef));
   loadCoreData(helper, rdataScope, SYSTEM_ENV);
   // NOTE : the table is tailed with GCMGSize,GCYGSize,GCPERMSize,MaxThread
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCMGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCYGSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCPERMSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnThreadCount));

   // resolve reference to SYSTEM_ENV at rdata header
   rdata->addReference((ref_t)_preloaded.get(SYSTEM_ENV), 0);

   // preloaded core static variables
   for (int i = 0; i < coreStaticNumber; i++) {
      if (!_preloaded.exist(coreStatics[i])) {
         _preloaded.add(coreStatics[i], helper.getVAddress(rdataWriter, mskRDataRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getCoreSection(coreStatics[i]);
         rdataScope.module = info.module;

         loadCoreOp(rdataScope, info.section ? (char*)info.section->get(0) : nullptr);
      }
   }

   // preloaded core functions
   loadRoutines(coreFunctionNumber, coreFunctions, scope, _preloaded);

   // preload vm commands
   for (int i = 0; i < gcCommandNumber; i++) {
      SectionInfo info = helper.getCoreSection(gcCommands[i]);

      // due to optimization section must be ROModule::ROSection instance
      _inlines[gcCommands[i]] = (char*)info.section->get(0);
   }

   // preload vm exended commmands
   for (int i = 0; i < gcCommandExNumber; i++) {
      SectionInfo info = helper.getCoreSection(gcCommandExs[i]);
      _inlineExs.add(gcCommandExs[i], info.section->get(0));
   }
}

void I64JITCompiler :: setStaticRootCounter(_JITLoader* loader, pos_t counter, bool virtualMode)
{
   if (virtualMode) {
      _Memory* data = loader->getTargetSection(mskRDataRef);

      ref_t offset = ((ref_t)_preloaded.get(SYSTEM_ENV) & ~mskAnyRef);
      (*data)[offset] = (counter << 3);
   }
   else {
      lvaddr_t offset = _preloaded.get(SYSTEM_ENV);
      *(pos_t*)offset = (counter << 3);
   }
}

lvaddr_t I64JITCompiler :: getPreloadedReference(ref_t reference)
{
   return _preloaded.get(reference);
}

void I64JITCompiler :: allocateThreadTable(_JITLoader* loader, int maxThreadNumber)
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
   loader->mapReference(ReferenceInfo(GC_THREADTABLE), (position | mskDataRef), (ref_t)mskDataRef);
   _preloaded.add(CORE_THREADTABLE, position | mskDataRef);
}

int I64JITCompiler:: allocateTLSVariable(_JITLoader* loader)
{
   MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskDataRef));

   // reserve space for TLS index
   pos_t position = dataWriter.Position();
   allocateVariable(dataWriter);

   // map TLS index
   loader->mapReference(ReferenceInfo(TLS_KEY), position | mskDataRef, (ref_t)mskDataRef);
   _preloaded.add(CORE_TLS_INDEX, position | mskDataRef);

   return position;
}

inline void compileTape(MemoryReader& tapeReader, size_t endPos, I64JITScope& scope)
{
   unsigned char code = 0;
   while (tapeReader.Position() < endPos) {
      code = tapeReader.getByte();
      // preload an argument if a command requires it
      if (code > MAX_SINGLE_ECODE) {
         scope.argument = tapeReader.getDWord();
      }
      commands[code](code, scope);
   }
}

void I64JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   I64JITScope scope(&tapeReader, &codeWriter, &helper, this, _bigAddressMode);

   pos_t codeSize = tapeReader.getDWord();
   pos_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   // ; copy the parameter to the accumulator to simulate embedded symbol
   // ; exit the procedure
   // ret
   codeWriter.writeByte(0xC3);

   alignCode(&codeWriter, 0x08, true);
}

void I64JITCompiler :: compileProcedure(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   I64JITScope scope(&tapeReader, &codeWriter, &helper, this, _bigAddressMode);

   pos_t codeSize = tapeReader.getDWord();
   pos_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   alignCode(&codeWriter, 0x08, true);
}

//void I64JITCompiler :: loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section)
//{
//   //size_t position = writer.Position();
//
//   //writer.write(section->get(0), section->Length());
//
//   //// resolve section references
//   //_ELENA_::RelocationMap::Iterator it(section->getReferences());
//   //while (!it.Eof()) {
//   //   int arg = *it;
//   //   writer.seek(arg + position);
//
//   //   ident_t reference = binary->resolveReference(it.key() & ~mskAnyRef);
//
//   //   helper.writeReference(writer, reference, it.key() & mskAnyRef);
//
//   //   it++;
//   //}
//   //writer.seekEOF();
//}

void I64JITCompiler :: generateSymbolCall(MemoryDump& tape, lvaddr_t address)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeByte(bcCallR);
   ecodes.writeDWord(address | mskCodeRef);
}

void I64JITCompiler :: generateProgramEnd(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeByte(bcQuit);

   JITCompiler64::generateProgramEnd(tape);
}

int I64JITCompiler :: allocateVMTape(_JITLoader* loader, void* tape, pos_t length)
{
   MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskRDataRef));

   // reserve space for TLS index
   pos_t position = dataWriter.Position();

   dataWriter.write(tape, length);

   // map VMTape
   loader->mapReference(ReferenceInfo(TAPE_KEY), position | mskRDataRef, (ref_t)mskRDataRef);

   return position;
}

void I64JITCompiler :: setTLSKey(lvaddr_t ptr)
{
   _preloaded.add(CORE_TLS_INDEX, ptr);
}

void I64JITCompiler :: setThreadTable(lvaddr_t ptr)
{
   _preloaded.add(CORE_THREADTABLE, ptr);
}

void I64JITCompiler :: setEHTable(lvaddr_t ptr)
{
   _preloaded.add(CORE_EH_TABLE, ptr);
}

void I64JITCompiler :: setGCTable(lvaddr_t ptr)
{
   _preloaded.add(CORE_GC_TABLE, ptr);
}

void I64JITCompiler :: setVoidParent(_JITLoader* loader, lvaddr_t ptr, bool virtualMode)
{
   if (virtualMode) {
      ref_t offset = ((ref_t)_preloaded.get(VOIDOBJ) & ~mskAnyRef);

      _Memory* rdata = loader->getTargetSection((ref_t)mskRDataRef);

      rdata->addReference((ref_t)ptr, offset);
   }
   else {
      lvaddr_t offset = _preloaded.get(VOIDOBJ);
      *(lvaddr_t*)offset = ptr;
   }
}

lvaddr_t I64JITCompiler :: getInvoker()
{
   return _preloaded.get(INVOKER);
}
