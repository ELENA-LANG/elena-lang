//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: I64
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "amd64jitcompiler.h"
#include "bytecode.h"

#pragma warning(disable : 4100)

using namespace _ELENA_;

// --- ELENA Object constants ---
//const int gcPageSize = 0x0010;           // a heap page size constant
const int elObjectOffset = 0x0010;           // object header / offset constant
// --- ELENA CORE built-in routines
//#define GC_ALLOC             0x10001
//#define HOOK                 0x10010
//#define INIT_RND             0x10012
#define INIT                 0x10013
#define NEWFRAME             0x10014
//#define INIT_ET              0x10015
//#define ENDFRAME             0x10016
//#define RESTORE_ET           0x10017
//#define OPENFRAME            0x10019
//#define CLOSEFRAME           0x1001A
//#define NEWTHREAD            0x1001B
//#define CLOSETHREAD          0x1001C
#define EXIT                 0x1001D
//#define CALC_SIZE            0x1001F
//#define GET_COUNT            0x10020
//#define THREAD_WAIT          0x10021
//#define NEW_HEAP             0x10025
//#define BREAK                0x10026
#define PREPARE              0x10027
//#define EXPAND_HEAP          0x10028
//#define EXITTHREAD           0x1002A
//#define NEW_EVENT            0x10101

//#define CORE_EXCEPTION_TABLE 0x20001
//#define CORE_GC_TABLE        0x20002
#define CORE_GC_SIZE         0x20003
#define CORE_STAT_COUNT      0x20004
#define CORE_STATICROOT      0x20005
//#define CORE_TLS_INDEX       0x20007
//#define CORE_THREADTABLE     0x20008
#define CORE_OS_TABLE        0x20009

// preloaded gc routines
const int coreVariableNumber = /*3*/1;
const int coreVariables[coreVariableNumber] =
{
   /*CORE_EXCEPTION_TABLE, CORE_GC_TABLE, */CORE_OS_TABLE
};

// preloaded gc routines
const int coreFunctionNumber = /*22*/4;
const int coreFunctions[coreFunctionNumber] =
{
/*   NEW_HEAP, EXPAND_HEAP, BREAK, GC_ALLOC, HOOK, INIT_RND, */INIT, NEWFRAME, //INIT_ET, ENDFRAME, RESTORE_ET,
/*   OPENFRAME, CLOSEFRAME, NEWTHREAD, CLOSETHREAD, */EXIT, //CALC_SIZE, GET_COUNT,
/*   THREAD_WAIT, EXITTHREAD, */PREPARE//, NEW_EVENT
};

// preloaded gc commands
const int gcCommandNumber = /*138*/14;
const int gcCommands[gcCommandNumber] =
{
   bcALoadSI, /*bcACallVI, */bcOpen, //bcBCopyA, bcPackage,
   bcALoadFI, /*bcASaveSI, bcASaveFI, */bcClose, //bcMIndex,
//   bcNewN, bcNew, bcASwapSI, bcXIndexRM, bcESwap,
   bcALoadBI, /*bcPushAI, bcCallExtR, bcPushF, bcBSRedirect,*/
//   bcHook, bcThrow, bcUnhook, bcClass, bcACallVD,
//   bcDLoadSI, bcDSaveSI, bcDLoadFI, bcDSaveFI, bcELoadSI,
/*   bcEQuit, bcAJumpVI, bcASaveBI, */bcXCallRM, //bcESaveSI,
//   bcGet, bcSet, bcXSet, bcACallI, bcBReadB,
   bcRestore, //bcLen, bcIfHeap, bcFlag, bcNCreate,
/*   bcBLoadFI, */bcReserve, /*bcAXSaveBI, */bcBLoadSI, //bcBWriteB,
/*   bcNEqual, bcNLess, */bcNCopy, bcNAdd, //bcBSwapSI,
//   bcNSub, bcNMul, bcNDiv, bcNLoadE, bcDivN,
/*   bcWLen, */bcNSave, bcNLoad, //bcWCreate, bcCopy,
//   bcBCreate, bcBWrite, bcBLen, bcBReadW, bcXLen,
//   bcBRead, bcBSwap, bcDSwapSI, bcESwapSI, bcSNop,
//   bcNAnd, bcNOr, bcNXor, bcTryLock, bcFreeLock,
//   bcLCopy, bcLEqual, bcLLess, bcLAdd,
//   bcLSub, bcLMul, bcLDiv, bcLAnd, bcLOr,
//   bcLXor, bcNShift, bcNNot, bcLShift,
//   bcLNot, bcRCopy, bcRSave, bcREqual, bcBSaveSI,
//   bcRLess, bcRAdd, bcRSub, bcRMul, bcRDiv,
//   bcCreate, bcExclude, bcDCopyR, bcInclude,
//   bcSelectR, bcNext, bcXSelectR, bcCount,
/*   bcRAbs, bcRExp, bcRInt, */bcValidate//,
//   bcRLn, bcRRound, bcRSin, bcRCos, bcRArcTan,
//   bcAddress, bcBWriteW, bcRLoad, bcXJumpRM, bcNLen,
//   bcNRead, bcNWrite, bcNLoadI, bcNSaveI, bcELoadFI,
//   bcESaveFI, bcWRead, bcWWrite, bcNWriteI,
//   bcNCopyB, bcLCopyB, bcCopyB, bcNReadI
};

// command table
void(*commands[0x100])(int opcode, AMD64JITScope& scope) =
{
   &compileNop, &compileBreakpoint, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compilePushA, &compileNop, &compileACopyB, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadOneByteLOp, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &loadOneByteLOp, &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &loadOneByteLOp,
   &loadOneByteLOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileDCopy, &compileNop, &loadIndexOpX, &compileNop, &loadFPOpX, &loadIndexOpX, &compileNop, &compileNop,
   &compileOpen, &compileQuitN, &compileNop, &compileBCopyF, &compileACopyF, &compileNop, &compileACopyR, &compileMCopy,

   &compileNop, &compileNop, &compileNop, &compileCallR, &compileNop, &loadFunction, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadIndexOpX,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &loadIndexOpX, &compileNop, &compileNop, &compileNop, &compileNop, &loadIndexOpX, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileDAddN, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileInvokeVMT, &compileNop,
};

//// --- x86JITCompiler commands ---
//
//inline void compileJump(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jmp   lbEnding
//   if (!forwardJump) {
//      scope.lh.writeJmpBack(label);
//   }
//   else {
//      // if it is forward jump, try to predict if it is short
//      if (shortJump) {
//         scope.lh.writeShortJmpForward(label);
//      }
//      else scope.lh.writeJmpForward(label);
//   }
//}
//
//inline void compileJumpX(x86JITScope& scope, int label, bool forwardJump, bool shortJump, x86Helper::x86JumpType prefix)
//{
//   if (!forwardJump) {
//      scope.lh.writeJxxBack(prefix, label);
//   }
//   else {
//      // if it is forward jump, try to predict if it is short
//      if (shortJump) {
//         scope.lh.writeShortJxxForward(label, prefix);
//      }
//      else scope.lh.writeJxxForward(label, prefix);
//   }
//}
//
//inline void compileJumpIf(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jnz   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JNZ);
//}
//
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
////inline void compileJumpGreater(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
////{
////   // jg   lbEnding
////   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JG);
////}
//
//inline void compileJumpLess(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // jl   lbEnding
//   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JL);
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

void _ELENA_::loadCoreOp(AMD64JITScope& scope, char* code)
{
   MemoryWriter* writer = scope.code;

   if (code == NULL)
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

inline void _ELENA_::writeCoreReference(AMD64JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   // references should be already preloaded
   /*if ((reference & mskAnyRef) == mskPreloadRelCodeRef) {
      scope.helper->writeReference(*scope.code,
         scope.compiler->_preloaded.get(reference & ~mskAnyRef), true, *(int*)(code + offset));

      scope.lh.addFixableJump(offset + position, (*scope.code->Memory())[offset + position]);
   }
   else */scope.helper->writeReference(*scope.code,
      scope.compiler->_preloaded.get(reference & ~mskAnyRef), false, *(int*)(code + offset));
}

//void _ELENA_::loadOneByteOp(int opcode, x86JITScope& scope)
//{
//   MemoryWriter* writer = scope.code;
//
//   char* code = (char*)scope.compiler->_inlines[opcode];
//   size_t position = writer->Position();
//   size_t length = *(size_t*)(code - 4);
//
//   // simply copy correspondent inline code
//   writer->write(code, *(int*)(code - 4));
//
//   // resolve section references
//   int count = *(int*)(code + length);
//   int* relocation = (int*)(code + length + 4);
//   int key, offset;
//   while (count > 0) {
//      key = relocation[0];
//      offset = relocation[1];
//
//      // locate relocation position
//      writer->seek(position + relocation[1]);
//
//      if ((key & mskTypeMask) == mskPreloaded) {
//         scope.compiler->writeCoreReference(scope, key, position, offset, code);
//      }
//      else scope.writeReference(*writer, key, *(int*)(code + offset));
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}
//
//void _ELENA_::loadNOp(int opcode, x86JITScope& scope)
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
//      if (relocation[0] == -1) {
//         scope.code->writeDWord(scope.argument);
//      }
//      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::loadOneByteLOp(int opcode, AMD64JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];

   // simply copy correspondent inline code
   scope.code->write(code, *(pos_t*)(code - 4));
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
//      if (relocation[0] == -1) {
//         scope.writeReference(*scope.code, scope.argument, 0);
//      }
//      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::loadIndexOpX(int opcode, AMD64JITScope& scope)
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
         scope.code->writeDWord(scope.argument << 3);
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

//void _ELENA_::loadVMTIndexOp(int opcode, x86JITScope& scope)
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
//      if (relocation[0] == -1) {
//         scope.code->writeDWord((scope.argument << 3) + 4);
//      }
//      else writeCoreReference(scope, relocation[0], position, relocation[1], code);
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}
//
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

void _ELENA_::loadFPOpX(int opcode, AMD64JITScope& scope)
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
         scope.code->writeDWord(-(scope.argument << 3));
      }
      else writeCoreReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFunction(int opcode, AMD64JITScope& scope)
{
   // if it is internal native call
   switch (scope.argument & mskAnyRef) {
      case mskNativeCodeRef:
      case mskPreloadCodeRef:
         compileCallR(opcode, scope);
         return;
   }

//   MemoryWriter* writer = scope.code;
//
//   char*  code = (char*)scope.compiler->_inlines[opcode];
//   size_t position = scope.code->Position();
//   size_t length = *(size_t*)(code - 4);
//
//   // simply copy correspondent inline code
//   writer->write(code, length);
//
//   // resolve section references
//   int count = *(int*)(code + length);
//   int* relocation = (int*)(code + length + 4);
//   int key, offset;
//   while (count > 0) {
//      key = relocation[0];
//      offset = relocation[1];
//
//      // locate relocation position
//      writer->seek(position + relocation[1]);
//
//      // !! temporal, more optimal way should be used
//      if (relocation[0] == -1) {
//         scope.writeReference(*writer, scope.argument, *(int*)(code + offset));
//      }
//      //else if ((key & mskTypeMask) == mskPreloaded) {
//      //   scope.compiler->writePreloadedReference(scope, key, position, offset, code);
//      //}
//      else scope.writeReference(*writer, key, *(int*)(code + offset));
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
}

////void _ELENA_::loadCode(int opcode, x86JITScope& scope)
////{
////   // if it is a symbol reference
////   if ((scope.argument & mskAnyRef) == mskSymbolRef) {
////      compileCallR(bcCallR, scope);
////   }
////   else {
////      // otherwise a primitive code
////      SectionInfo   info = scope.getSection(scope.argument);
////      MemoryWriter* writer = scope.code;
////
////      // override module
////      scope.module = info.module;
////
////      char*  code = (char*)info.section->get(0);
////      size_t position = scope.code->Position();
////      size_t length = *(size_t*)(code - 4);
////
////      // simply copy correspondent inline code
////      writer->write(code, length);
////
////      // resolve section references
////      int count = *(int*)(code + length);
////      int* relocation = (int*)(code + length + 4);
////      int key, offset;
////      while (count > 0) {
////         key = relocation[0];
////         offset = relocation[1];
////
////         // locate relocation position
////         writer->seek(position + relocation[1]);
////
////         scope.writeReference(*writer, key, *(int*)(code + offset));
////
////         relocation += 2;
////         count--;
////      }
////      // clear module overriding
////      scope.module = NULL;
////      scope.code->seekEOF();
////   }
////}

void _ELENA_::compileNop(int, AMD64JITScope& scope)
{
//   // nop command is used to indicate possible label
//   // fix the label if it exists
//   if (scope.lh.checkLabel(scope.tape->Position() - 1)) {
//      scope.lh.fixLabel(scope.tape->Position() - 1);
//   }
//   // or add the label
//   else scope.lh.setLabel(scope.tape->Position() - 1);
}

void _ELENA_::compileBreakpoint(int, AMD64JITScope& scope)
{
   if (scope.withDebugInfo)
      scope.helper->addBreakpoint(scope.code->Position());
}

//void _ELENA_::compilePush(int opcode, x86JITScope& scope)
//{
//   // push constant | reference
//   scope.code->writeByte(0x68);
//   if (opcode == bcPushR && scope.argument != 0) {
//      scope.writeReference(*scope.code, scope.argument, 0);
//   }
//   else scope.code->writeDWord(scope.argument);
//}
//
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
//
//void _ELENA_::compilePushS(int, x86JITScope& scope)
//{
//   // push [esp+offset]
//   scope.code->writeWord(0xB4FF);
//   scope.code->writeByte(0x24);
//   scope.code->writeDWord(scope.argument << 2);
//}
//
//void _ELENA_::compileJump(int, x86JITScope& scope)
//{
//   ::compileJump(scope, scope.tape->Position() + scope.argument, (scope.argument > 0), (__abs(scope.argument) < 0x10));
//}
//
//void _ELENA_::compileHook(int opcode, x86JITScope& scope)
//{
//   if (scope.argument < 0) {
//      scope.lh.writeLoadBack(scope.tape->Position() + scope.argument);
//   }
//   else scope.lh.writeLoadForward(scope.tape->Position() + scope.argument);
//
//   loadOneByteOp(opcode, scope);
//}

void _ELENA_::compileOpen(int opcode, AMD64JITScope& scope)
{
   loadOneByteLOp(opcode, scope);

   //scope.prevFSPOffs += (scope.argument << 2);
}

//void _ELENA_::compileQuit(int, x86JITScope& scope)
//{
//   scope.code->writeByte(0xC3);
//}

void _ELENA_::compileQuitN(int, AMD64JITScope& scope)
{
   scope.code->writeByte(0xC2);
   scope.code->writeWord((unsigned short)(scope.argument << 3));
}

//void _ELENA_::compileNext(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // test upper boundary
//   loadOneByteLOp(opcode, scope);
//
//   // try to use short jump if offset small (< 0x10?)
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
//   // try to use short jump if offset small (< 0x10?)
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
//   // cmp eax, [ebx]
//   // ja short label
//   // cmp eax, esp
//   // jb short label
//
//   scope.code->writeWord(0x033B);
//   // try to use short jump if offset small (< 0x10?)
//   compileJumpAbove(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//
//   scope.code->writeWord(0xC43B);
//   compileJumpBelow(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileCreate(int opcode, x86JITScope& scope)
//{
//   // HOT FIX : reverse the argument order
//   ref_t vmtRef = scope.argument;
//   scope.argument = scope.tape->getDWord();
//
//   scope.argument <<= 2;
//
//   // mov ecx, #gc_page + (length - 1)
//   scope.code->writeByte(0xB9);
//   scope.code->writeDWord(align(scope.argument + scope.objectSize, gcPageSize));
//
//   //NOTE : empty length should be equal to 800000h
//   // due to current GC algorithm
//   if (scope.argument == 0)
//      scope.argument = 0x800000;
//
//   loadNOp(opcode, scope);
//
//   // set vmt reference
//   // mov [eax-4], vmt
//   scope.code->writeWord(0x40C7);
//   scope.code->writeByte(0xFC);
//   scope.writeReference(*scope.code, vmtRef, 0);
//}
//
//void _ELENA_::compileCreateN(int opcode, x86JITScope& scope)
//{
//   // HOT FIX : reverse the argument order
//   ref_t vmtRef = scope.argument;
//   scope.argument = scope.tape->getDWord();
//
//   int size = align(scope.argument + scope.objectSize, gcPageSize);
//
//   scope.argument |= 0x800000;  // mark object as a binary structure
//
//                                // mov  ecx, #gc_page + (size - 1)
//   scope.code->writeByte(0xB9);
//   scope.code->writeDWord(size);
//
//   loadNOp(opcode, scope);
//
//   // set vmt reference
//   // mov [eax-4], vmt
//   scope.code->writeWord(0x40C7);
//   scope.code->writeByte(0xFC);
//   scope.writeReference(*scope.code, vmtRef, 0);
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

void _ELENA_::compileACopyR(int, AMD64JITScope& scope)
{
   // mov eax, r
   scope.code->writeByte(0xB8);
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

void _ELENA_::compileDCopy(int, AMD64JITScope& scope)
{
   // mov ebx, i
   scope.code->writeByte(0xBB);
   scope.code->writeDWord(scope.argument);
}

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

void _ELENA_::compileDAddN(int, AMD64JITScope& scope)
{
   // add ebx, n
   scope.code->writeWord(0xC381);
   scope.code->writeDWord(scope.argument);
}

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
//void _ELENA_::compileDCopyVerb(int, x86JITScope& scope)
//{
//   // mov ebx, ecx
//   // and ebx, VERB_MASK
//   scope.code->writeWord(0xD98B);
//   scope.code->writeWord(0xE381);
//   scope.code->writeDWord(VERB_MASK | MESSAGE_MASK);
//}
//
//void _ELENA_::compileDCopyCount(int, x86JITScope& scope)
//{
//   // mov ebx, ecx
//   // and ebx, VERB_MASK
//   scope.code->writeWord(0xD98B);
//   scope.code->writeWord(0xE381);
//   scope.code->writeDWord(PARAM_MASK);
//}
//
//void _ELENA_::compileDCopySubj(int, x86JITScope& scope)
//{
//   // mov ebx, ecx
//   // and ebx, VERB_MASK
//   scope.code->writeWord(0xD98B);
//   scope.code->writeWord(0xE381);
//   scope.code->writeDWord(SIGN_MASK | MESSAGE_MASK);
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
//void _ELENA_::compileBLoadR(int, x86JITScope& scope)
//{
//   // mov edi, [r]
//   scope.code->writeWord(0x3D8B);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}

void _ELENA_::compilePushA(int, AMD64JITScope& scope)
{
   // push rax
   scope.code->writeByte(0x50);
}

//void _ELENA_::compilePushFI(int, x86JITScope& scope)
//{
//   scope.code->writeWord(0xB5FF);
//   // push [ebp-level*4]
//   scope.code->writeDWord(-(scope.argument << 2));
//}
//
//void _ELENA_::compilePushF(int, x86JITScope& scope)
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
//
//void _ELENA_::compilePushE(int, x86JITScope& scope)
//{
//   // push ecx
//   scope.code->writeByte(0x51);
//}
//
//void _ELENA_::compilePushD(int, x86JITScope& scope)
//{
//   // push ebx
//   scope.code->writeByte(0x53);
//}

void _ELENA_::compileCallR(int, AMD64JITScope& scope)
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
//
//void _ELENA_::compilePopA(int, x86JITScope& scope)
//{
//   // pop eax
//   scope.code->writeByte(0x58);
//}

void _ELENA_ :: compileMCopy(int, AMD64JITScope& scope)
{
   // mov ecx, message
   scope.code->writeByte(0x48);
   scope.code->writeByte(0xB9);
   scope.code->writeQWord(scope.resolveMessage(scope.argument));
}

//void _ELENA_::compilePopN(int, x86JITScope& scope)
//{
//   // add esp, arg
//   scope.code->writeWord(0xC481);
//   scope.code->writeDWord(scope.argument << 2);
//
//   //// lea esp, [esp + level * 4]
//   //x86Helper::leaRM32disp(
//   //   scope.code, x86Helper::otESP, x86Helper::otESP, scope.argument << 2);
//}

void _ELENA_::compileACopyB(int, AMD64JITScope& scope)
{
   // mov rax, rdi
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xF889);
}

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

void _ELENA_::compileInvokeVMTOffset(int opcode, AMD64JITScope& scope)
{
//   int message = scope.resolveMessage(scope.tape->getDWord());
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
//         // resolve message offset
//         scope.writeReference(*scope.code, scope.argument | mskVMTEntryOffset, message);
//      }
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
}

void _ELENA_::compileInvokeVMT(int opcode, AMD64JITScope& scope)
{
   ref_t message = fromMessage64(scope.resolveMessage(scope.tape->getDWord()));

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
         // resolve message offset
         scope.writeReference(*scope.code, (scope.argument & ~mskAnyRef) | mskVMTMethodAddress, message);
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
//      scope.code->writeDWord(-1);
//   }
//   else scope.writeReference(*scope.code, scope.argument, 0);
//   //NOTE: due to compileJumpX implementation - compileJumpIf is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileElseR(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp eax, r
//   // jz lab
//
//   scope.code->writeByte(0x3D);
//   // HOTFIX : support zero references
//   if (scope.argument != 0) {
//      scope.writeReference(*scope.code, scope.argument, 0);
//   }
//   else scope.code->writeDWord(0);
//
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
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
//void _ELENA_::compileLessN(int, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp ebx, n
//   // jz lab
//
//   scope.code->writeWord(0xFB81);
//   scope.code->writeDWord(scope.argument);
//   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileSetVerb(int, x86JITScope& scope)
//{
//   // and ecx, VERB_MASK
//   // or  ecx, m
//   scope.code->writeWord(0xE181);
//   scope.code->writeDWord(~VERB_MASK);
//   scope.code->writeWord(0xC981);
//   scope.code->writeDWord(scope.argument);
//}
//
//void _ELENA_::compileSetSubj(int, x86JITScope& scope)
//{
//   // and ecx, SUBJ_MASK
//   // or  ecx, m
//   scope.code->writeWord(0xE181);
//   scope.code->writeDWord(~SIGN_MASK);
//   scope.code->writeWord(0xC981);
//   scope.code->writeDWord(scope.resolveMessage(scope.argument));
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

void _ELENA_::compileBCopyF(int, AMD64JITScope& scope)
{
   // lea rdi, [rbp+nn]
   scope.code->writeByte(0x48);
   scope.code->writeWord(0xBD8D);
   scope.code->writeDWord(-(scope.argument << 3));
}

//void _ELENA_::compileBCopyS(int, x86JITScope& scope)
//{
//   // lea edi, [esp+nn]
//   scope.code->writeWord(0xBC8D);
//   scope.code->writeByte(0x24);
//   scope.code->writeDWord(-(scope.argument << 2));
//}

void _ELENA_::compileACopyF(int, AMD64JITScope& scope)
{
   // lea eax, [ebp+nn]
   scope.code->writeByte(0x48);
   scope.code->writeWord(0x858D);
   scope.code->writeDWord(-(scope.argument << 3));
}

//void _ELENA_::compileNot(int, x86JITScope& scope)
//{
//   // not ebx
//   scope.code->writeWord(0xD3F7);
//}
//
//void _ELENA_::compileDShiftN(int, x86JITScope& scope)
//{
//   if (scope.argument < 0) {
//      // shl ebx, n
//      scope.code->writeWord(0xE3C1);
//      scope.code->writeByte((unsigned char)-scope.argument);
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

// --- AMD64JITScope ---

AMD64JITScope :: AMD64JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, AMD64JITCompiler* compiler)
//   : lh(code)
{
   this->tape = tape;
   this->code = code;
   this->helper = helper;
   this->compiler = compiler;
   this->withDebugInfo = compiler->isWithDebugInfo();
   //this->objectSize = helper ? helper->getLinkerConstant(lnObjectSize) : 0;
   this->module = NULL;
}

void AMD64JITScope::writeReference(MemoryWriter& writer, ref_t reference, size_t disp)
{
   // HOTFIX : mskLockVariable used to fool trylock opcode, adding virtual offset
   if ((reference & mskAnyRef) == mskLockVariable) {
      disp += compiler->getObjectHeaderSize();
   }

   helper->writeReference(writer, reference, disp, module);
}

void AMD64JITScope :: writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp)
{
   helper->writeXReference(writer, reference, disp, module);
}

// --- AMD64JITCompiler ---

AMD64JITCompiler :: AMD64JITCompiler(bool debugMode)
{
   _debugMode = debugMode;
}

size_t AMD64JITCompiler :: getObjectHeaderSize() const
{
   return elObjectOffset;
}

bool AMD64JITCompiler :: isWithDebugInfo() const
{
   // in the current implementation, debug info (i.e. debug section)
   // is always generated (to be used by RTManager)
   return true;
}

void AMD64JITCompiler :: alignCode(MemoryWriter* writer, int alignment, bool code)
{
   writer->align(alignment, code ? 0x90 : 0x00);
}

void AMD64JITCompiler :: writeCoreReference(AMD64JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   if (!_preloaded.exist(reference& ~mskAnyRef)) {
      MemoryWriter writer(scope.code->Memory());

      _preloaded.add(reference & ~mskAnyRef, scope.helper->getVAddress(writer, mskCodeRef));

      // due to optimization section must be ROModule::ROSection instance
      SectionInfo info = scope.helper->getCoreSection(reference & ~mskAnyRef);
      // separate scope should be used to prevent overlapping
      AMD64JITScope newScope(NULL, &writer, scope.helper, this);
      newScope.module = info.module;

      loadCoreOp(newScope, info.section ? (char*)info.section->get(0) : NULL);
   }
   _ELENA_::writeCoreReference(scope, reference, position, offset, code);
}

void AMD64JITCompiler :: prepareCore(_ReferenceHelper& helper, _JITLoader* loader)
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

   AMD64JITScope dataScope(NULL, &dataWriter, &helper, this);
   for (int i = 0; i < coreVariableNumber; i++) {
      if (!_preloaded.exist(coreVariables[i])) {
         _preloaded.add(coreVariables[i], helper.getVAddress(dataWriter, mskDataRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getCoreSection(coreVariables[i]);
         dataScope.module = info.module;

         loadCoreOp(dataScope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // GC SIZE Table
   _preloaded.add(CORE_GC_SIZE, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeQWord(helper.getLinkerConstant(lnGCMGSize));
   rdataWriter.writeQWord(helper.getLinkerConstant(lnGCYGSize));
   rdataWriter.writeQWord(helper.getLinkerConstant(lnThreadCount));

   // load GC static root
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // STAT COUNT
   _preloaded.add(CORE_STAT_COUNT, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeQWord(0);

   dataWriter.writeQWord(helper.getLinkerConstant(lnVMAPI_Instance));

   AMD64JITScope scope(NULL, &codeWriter, &helper, this);
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
}

void AMD64JITCompiler :: setStaticRootCounter(_JITLoader* loader, size_t counter, bool virtualMode)
{
   //if (virtualMode) {
   //   _Memory* data = loader->getTargetSection(mskRDataRef);

   //   size_t offset = ((size_t)_preloaded.get(CORE_STAT_COUNT) & ~mskAnyRef);
   //   (*data)[offset] = (counter << 2);
   //}
   //else {
   //   size_t offset = (size_t)_preloaded.get(CORE_STAT_COUNT);
   //   *(int*)offset = (counter << 2);
   //}
}

void* AMD64JITCompiler :: getPreloadedReference(ref_t reference)
{
   return (void*)_preloaded.get(reference);
}

void AMD64JITCompiler :: allocateThreadTable(_JITLoader* loader, int maxThreadNumber)
{
   //// get target image & resolve virtual address
   //MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskDataRef));

   //// size place holder
   //dataWriter.writeDWord(0);

   //// reserve space for the thread table
   //int position = dataWriter.Position();
   //allocateArray(dataWriter, maxThreadNumber);

   //// map thread table
   //loader->mapReference(GC_THREADTABLE, (void*)(position | mskDataRef), (ref_t)mskDataRef);
   //_preloaded.add(CORE_THREADTABLE, (void*)(position | mskDataRef));
}

int AMD64JITCompiler :: allocateTLSVariable(_JITLoader* loader)
{
   //MemoryWriter dataWriter(loader->getTargetSection((ref_t)mskDataRef));

   //// reserve space for TLS index
   //int position = dataWriter.Position();
   //allocateVariable(dataWriter);

   //// map TLS index
   //loader->mapReference(TLS_KEY, (void*)(position | mskDataRef), (ref_t)mskDataRef);
   //_preloaded.add(CORE_TLS_INDEX, (void*)(position | mskDataRef));

   //return position;

   return 0;
}

inline void compileTape(MemoryReader& tapeReader, size_t endPos, AMD64JITScope& scope)
{
   unsigned char code = 0;
   while (tapeReader.Position() < endPos) {
      // read bytecode + arguments
      code = tapeReader.getByte();
      // preload an argument if a command requires it
      if (code > MAX_SINGLE_ECODE) {
         scope.argument = tapeReader.getDWord();
      }
      commands[code](code, scope);
   }
}

void AMD64JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   AMD64JITScope scope(&tapeReader, &codeWriter, &helper, this);

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   // ; copy the parameter to the accumulator to simulate embedded symbol
   // ; exit the procedure
   // ret
   codeWriter.writeByte(0xC3);

   alignCode(&codeWriter, 0x08, true);
}

void AMD64JITCompiler :: compileProcedure(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   AMD64JITScope scope(&tapeReader, &codeWriter, &helper, this);

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);

   alignCode(&codeWriter, 0x08, true);
}

void AMD64JITCompiler :: loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section)
{
   //size_t position = writer.Position();

   //writer.write(section->get(0), section->Length());

   //// resolve section references
   //_ELENA_::RelocationMap::Iterator it(section->getReferences());
   //while (!it.Eof()) {
   //   int arg = *it;
   //   writer.seek(arg + position);

   //   ident_t reference = binary->resolveReference(it.key() & ~mskAnyRef);

   //   helper.writeReference(writer, reference, it.key() & mskAnyRef);

   //   it++;
   //}
   //writer.seekEOF();
}

void AMD64JITCompiler :: generateProgramStart(MemoryDump& tape)
{
   JITCompiler64::generateProgramStart(tape);

   MemoryWriter ecodes(&tape);
   ecodes.writeByte(bcCallExtR);
   ecodes.writeDWord(PREPARE | mskPreloadCodeRef);

   ecodes.writeByte(bcCallExtR);
   ecodes.writeDWord(INIT | mskPreloadCodeRef);

   ecodes.writeByte(bcCallExtR);
   ecodes.writeDWord(NEWFRAME | mskPreloadCodeRef);
}

void AMD64JITCompiler :: generateSymbolCall(MemoryDump& tape, void* address)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeByte(bcCallR);
   ecodes.writeDWord((size_t)address | mskCodeRef);
}

void AMD64JITCompiler :: generateProgramEnd(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeByte(bcCallExtR);
   ecodes.writeDWord(mskPreloadCodeRef | EXIT);

   JITCompiler64::generateProgramEnd(tape);
}