//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86jitcompiler.h"
#include "bytecode.h"

using namespace _ELENA_;

// --- ELENA Object constants ---
const int gcPageSize       = 0x0010;           // a heap page size constant

// --- ELENA built-in routines
#define CORE_EXCEPTION_TABLE 0x0001
#define CORE_GC_TABLE        0x0002
#define CORE_GC_SIZE         0x0003
#define CORE_STAT_COUNT      0x0004
#define CORE_STATICROOT      0x0005
#define CORE_VM_TABLE        0x0006

#define GC_ALLOC             0x10001
#define HOOK                 0x10010
#define LOADCLASSNAME        0x10011
//#define GC_REALLOC         0x10006
//#define GC_TERMINATOR      0x10007

//#define VM_GET_CLASSNAME   0x30001

// preloaded gc routines
const int coreVariableNumber = 2;
const int coreVariables[coreVariableNumber] =
{
   CORE_EXCEPTION_TABLE, CORE_GC_TABLE
};

// preloaded gc routines
const int coreFunctionNumber = 3;
const int coreFunctions[coreFunctionNumber] =
{
   GC_ALLOC, HOOK, LOADCLASSNAME,
   /*GC_REALLOC,
   CORE_INIT_ROUTINE, CORE_SET_ROUTINE, CORE_OPENFRAME, CORE_CLOSEFRAME, CORE_ALLOC_ROUTINE, CORE_OBJALLOC_ROUTINE,
   THREAD_INIT_ROUTINE, CORE_SEND_ROUTINE, THREAD_CLOSE_ROUTINE, THREAD_OPEN_SAFEREGION, THREAD_CLOSE_SAFEREGION, THREAD_WAIT,
   CORE_NEWFRAME, CORE_ENDFRAME, CORE_OBJREALLOC_ROUTINE, CORE_GETVMT, CORE_GETCOUNT, CORE_GETSIZE, CORE_COPYWSTR, CORE_COPYDUMP,
   CORE_VM_GETCLASSNAME, CORE_VM_GETCLASSREF, CORE_VM_GETSYMBOLREF, CORE_TESTFLAG, CORE_VM_INTERPRETE, CORE_VM_GETEROR*/
};

// preloaded gc commands
const int gcCommandNumber = 51;
const ByteCommand gcCommands[gcCommandNumber] =
{
   bcBSRedirect, bcALoadSI, bcACallVI, bcOpen, bcBCopyA,
   bcSwapSI, bcALoadFI, bcASaveSI, bcASaveFI, bcClose,
/*   bcIAccFillR, */bcCreateN, bcPopBI, bcCreate,
   bcALoadBI, bcPushAI, bcCallExtR, bcXPushF, /*bcPushSPI, */
   bcHook, bcPopAI, bcXPopAI, bcInclude, bcExclude, 
   bcThrow, bcUnhook, /*bcRethrow, bcAccCreate, */bcMLoadSI, 
   bcMLoadFI, bcMSaveParams, bcDLoadSI, bcDSaveSI, 
   bcDAddSI, bcDSubSI, bcDLoadFI, bcDSaveFI, //bcJumpAcc, bcAccFillR,
   bcMLoadAI, bcMQuit, bcAJumpVI, bcASaveBI, bcXCallRM, 
/*   bcRCallN, */bcGet, bcSet, bcASwapSI, 
   bcSCallVI, bcMAddAI, bcRestore, bcGetLen, bcNBox,
   /*   bcMccReverse,*/ bcAXSetR, bcWSTest, bcTest, bcBSTest,
   bcBox
};

const int gcExtensionNumber = 56;
const FunctionCode gcExtensions[gcExtensionNumber] =
{
   fnCopy, fnReserve, fnSave, fnSetLen, fnLoad, 
   fnEqual, fnLess, fnNotGreater, fnAdd, fnSub, 
   fnMul, fnDiv, fnAnd, fnOr, fnXor, 
   fnShift, fnCreate, fnCopyInt, fnNot, fnCopyStr,
   fnCopyLong, fnCopyReal, fnGetLen, fnIndexOfStr, fnDeleteStr,
   fnLoad, fnGetAt, fnSetAt, fnLoadStr, fnAddStr,
   fnAddInt, fnSubInt, fnMulInt, fnDivInt, fnAddLong,
   fnSubLong, fnMulLong, fnDivLong, fnInc, fnCopyBuf,
   fnIndexOf, fnIndexOfWord, fnSetWord, fnGetWord, fnGetBuf,
   fnGetInt, fnSetInt, fnRndNew, fnRndNext, fnLn,
   fnExp, fnAbs, fnRound, fnSetBuf, fnLoadName,
   fnGetLenZ
};

// command table
void (*commands[0x100])(int opcode, x86JITScope& scope) =
{
   &compileNop, &compileBreakpoint, &compilePushSelf, &compilePop, &compileNop, &compileMccPush, &compileMccCopyVerb, &loadOneByteOp,
   &compileNop, &compileMccCopySubj, &compilePushAcc, &compilePopAcc, &compileAccLoadSelf, &compileMccPop, &loadOneByteOp, &compileNop,

   &compileNop, &loadOneByteLOp, &loadOneByteLOp, &compileIndexDec, &compilePopSelf, &loadOneByteLOp, &compileNop, &compileQuit,
   &loadOneByteOp, &loadOneByteOp, &compileIndexInc, &loadOneByteLOp, &compileALoadD, &loadOneByteOp, &loadOneByteOp, &loadOneByteOp,

   &compileReserve, &compilePush, &compilePush, &compileLoadField, &loadIndexOp, &compileNop, &compilePushF, &compileNop,
   &compileNop, &loadFPOp, &compilePushS, &compileNop, &compileNop, &compilePushFPI, &compileXPushFPI, &compileNop,

   &compilePopN, &loadIndexOp, &compilePopFI, &loadIndexOp, &compilePopSI, &loadIndexOp, &compileNop, &compileNop,
   &compileNop, &compileQuitN, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &loadFunction, &loadCode, &loadVMTIndexOp, &compileCallR, &compileNop, &compileNop, &compileNop, &loadIndexOp,
   &loadIndexOp, &loadFPOp, &compileNop, &compileNop, &compileNop, &loadIndexOp, &compileMSet, &compileMAdd,

   &loadIndexOp, &loadIndexOp, &loadFPOp, &compileAccLoadR, &loadFPOp, &loadIndexOp, &compileDCopyI, &compileDCopyAI,
   &compileNop, &compileDAddAI, &compileDSubAI, &loadIndexOp, &loadIndexOp, &loadFPOp, &compileNop, &compileNop,

   &compileNop, &loadIndexOp, &compileNop, &loadIndexOp, &loadFPOp, &compileAccSaveR, &compileNop, &compileDSaveAI,
   &compileNop, &compileNop, &compileNop, &compileNop, &loadIndexOp, &loadIndexOp, &loadROp, &compileNop,

   &compileNop, &compileNop, &compileSPSetF, &compileNop, &compileNop, &compileAccCopySPI, &compileNop, &compileNop,
   &compileAccSetR, &compileAccSetN, &compileAccCopyAccI, &compileXAccCopyFPI, &compileAccCopyFPI, &compileNop, &compileAAdd, &compileAMul,

   &compileNop, &compileNop, &loadIndexOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileOpen, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileJump, &loadVMTIndexOp, &compileNop, &compileNop, &compileNop, &compileNop, &compileHook, &compileDElse,
   &compileDThen, &compileAElse, &compileAThen, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileBoxN, &loadROp, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,

   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadROp, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &loadIndexOp, &compileNop,

   &loadExtensions, &loadExtensions, &loadExtensions, &loadExtensions, &loadExtensions, &loadExtensions, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileNop, &compileTest, &compileTest, &compileTest,

   &compileElseR, &compileThenR, &compileMElse, &compileMThen, &compileNop, &compileNop, &compileElseSI, &compileThenSI,
   &compileNop, &compileNop, &compileMElseAccI, &compileMThenAccI, &compileElseFlag, &compileThenFlag, &compileNop, &compileNext,

   &compileCreate, &compileCreateN, &compileIAccCopyR, &compileNop, &compileNop, &compileNop, &compileNop, &compileNop,
   &compileNop, &compileNop, &compileNop, &compileNop, &compileCallSI, &compileNop, &compileInvokeVMT, &compileNop,
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

////inline void compileJumpY(x86JITScope& scope, int label, bool forwardJump, bool shortJump, x86Helper::x86JumpType prefix)
////{
////   scope.code->writeWord(0xDB85);
////   if (!forwardJump) {
////      scope.lh.writeJxxBack(prefix, label);
////   }
////   else {
////      // if it is forward jump, try to predict if it is short
////      if (shortJump) {
////         scope.lh.writeShortJxxForward(label, prefix);
////      }
////      else scope.lh.writeJxxForward(label, prefix);
////   }
////}

inline void compileJumpIf(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JNZ);
}

inline void compileJumpIfNot(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jz   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JZ);
}

//inline void compileJumpAbove(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
//{
//   // ja   lbEnding
////   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JA);
//}

inline void compileJumpGreater(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jg   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JG);
}

inline void compileJumpLess(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JL);
}

inline void compileJumpLessOrEqual(x86JITScope& scope, int label, bool forwardJump, bool shortJump)
{
   // jl   lbEnding
   compileJumpX(scope, label, forwardJump, shortJump, x86Helper::JUMP_TYPE_JLE);
}

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
         scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      }
      else {
         //if ((key & mskAnyRef) == mskLinkerConstant) {
         //   scope.code->writeDWord(scope.helper->getLinkerConstant(key & ~mskAnyRef));
         //}
         /*else */scope.helper->writeReference(*writer, key, *(int*)(code + offset));
      }

      relocation += 2;
      count--;
   }
   writer->seekEOF();
}

inline void _ELENA_::writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
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
         scope.compiler->writePreloadedReference(scope, key, position, offset, code);
      }
      else scope.writeReference(*writer, key, *(int*)(code + offset));

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadExtensions(int opcode, x86JITScope& scope)
{
   MemoryWriter* writer = scope.code;

   char* code = (char*)scope.compiler->_extensions[opcode - bcFunc][scope.argument];
   size_t position = writer->Position();
   size_t length = *(size_t*)(code - 4);

   // simply copy correspondent inline code>	elc.exe!_ELENA_::loadExtensions(int opcode, _ELENA_::x86JITScope & scope) Line 349	C++

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
         scope.compiler->writePreloadedReference(scope, key, position, offset, code);
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
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadOneByteLOp(int opcode, x86JITScope& scope)
{
   char* code = (char*)scope.compiler->_inlines[opcode];

   // simply copy correspondent inline code
   scope.code->write(code, *(int*)(code - 4));
}

void _ELENA_::loadROp(int opcode, x86JITScope& scope)
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
         scope.writeReference(*scope.code, scope.argument, 0);
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

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
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

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
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

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
         if (scope.argument < 0) {
            scope.code->writeDWord(scope.prevFSPOffs - (scope.argument << 2));
         }
         else scope.code->writeDWord(-(scope.argument << 2));
      }
      else writePreloadedReference(scope, relocation[0], position, relocation[1], code);

      relocation += 2;
      count--;
   }
   scope.code->seekEOF();
}

void _ELENA_::loadFunction(int opcode, x86JITScope& scope)
{
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

void _ELENA_::loadCode(int opcode, x86JITScope& scope)
{
   // if it is a symbol reference
   if ((scope.argument & mskAnyRef) == mskSymbolRef) {
      // if embedded symbol mode is on
      if (scope.embeddedSymbols) {
         SectionInfo  info = scope.getSection(scope.argument);
         MemoryReader reader(info.section);

         scope.compiler->embedSymbol(*scope.helper, reader, *scope.code, info.module);
      }
      // otherwise treat like calling a symbol
      else compileCallR(bcCallR, scope);
   }
   else {
      // otherwise a primitive code
      SectionInfo   info = scope.getSection(scope.argument);
      MemoryWriter* writer = scope.code;

      // override module
      scope.module = info.module;

      char*  code = (char*)info.section->get(0);
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

         scope.writeReference(*writer, key, *(int*)(code + offset));

         relocation += 2;
         count--;
      }
      // clear module overriding
      scope.module = NULL;
      scope.code->seekEOF();
   }
}

void _ELENA_::compileNop(int opcode, x86JITScope& scope)
{
   // nop command is used to indicate possible label
   // fix the label if it exists
   if (scope.lh.checkLabel(scope.tape->Position() - 1)) {
      scope.lh.fixLabel(scope.tape->Position() - 1);
   }
   // or add the label
   else scope.lh.setLabel(scope.tape->Position() - 1);
}

void _ELENA_::compileBreakpoint(int opcode, x86JITScope& scope)
{
   if (scope.debugMode)
      scope.helper->addBreakpoint(scope.code->Position());
}

void _ELENA_::compilePush(int opcode, x86JITScope& scope)
{
   // push constant | reference
   scope.code->writeByte(0x68);
   if (opcode == bcPushR) {
      scope.writeReference(*scope.code, scope.argument, 0);
   }
   else scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileMccPush(int opcode, x86JITScope& scope)
{
   // push edx
   scope.code->writeByte(0x52);
}

void _ELENA_::compileMccPop(int opcode, x86JITScope& scope)
{
   // pop edx
   scope.code->writeByte(0x5A);
}

void _ELENA_::compileSPSetF(int opcode, x86JITScope& scope)
{
   // lea esp, [ebp - level * 4]

   x86Helper::leaRM32disp(scope.code, x86Helper::otESP, x86Helper::otEBP, -(scope.argument << 2));
}

void _ELENA_::compileAccCopyAccI(int opcode, x86JITScope& scope)
{
   // mov eax, [eax + __arg * 4]

   scope.code->writeWord(0x808B);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compilePushS(int opcode, x86JITScope& scope)
{
   // push [esp+offset]
   scope.code->writeWord(0xB4FF);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileJump(int opcode, x86JITScope& scope)
{
   ::compileJump(scope, scope.tape->Position() + scope.argument, (scope.argument > 0), (__abs(scope.argument) < 0x10));
}

void _ELENA_::compileHook(int opcode, x86JITScope& scope)
{
   scope.lh.writeLoadForward(scope.tape->Position() + scope.argument);
   loadOneByteOp(opcode, scope);
}

//void _ELENA_::compileJumpR(int opcode, x86JITScope& scope)
//{
//   //// mov ecx, reference
//   //// jmp ecx
//
//   //scope.code->writeByte(0xB9);
//   //scope.writeReference(*scope.code, scope.argument, 0);
//   //scope.code->writeWord(0xE1FF);
//}

void _ELENA_::compilePopSI(int opcode, x86JITScope& scope)
{
   // pop ecx
   // mov [esp+(level - 1)*4], ecx

   scope.code->writeByte(0x59);
   x86Helper::movMR32disp(scope.code, x86Helper::otESP, x86Helper::otECX, (scope.argument - 1) << 2);
}

void _ELENA_::compileOpen(int opcode, x86JITScope& scope)
{
   loadOneByteLOp(opcode, scope);

   scope.prevFSPOffs += (scope.argument << 2);
}

void _ELENA_::compileQuit(int opcode, x86JITScope& scope)
{
   scope.code->writeByte(0xC3);
}

void _ELENA_::compileQuitN(int opcode, x86JITScope& scope)
{
   scope.code->writeByte(0xC2);
   scope.code->writeWord(scope.argument << 2);
}

void _ELENA_::compileAThen(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test eax, eax
   scope.code->writeWord(0xC085);

  // try to use short jump if offset small (< 0x10?)
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileAElse(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test eax, eax
   scope.code->writeWord(0xC085);

  // try to use short jump if offset small (< 0x10?)
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileTest(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test low boundary
  // cmp esi, 0
   scope.code->writeWord(0xFE81);
   scope.code->writeDWord(0);
  // try to use short jump if offset small (< 0x10?)
   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));

  // test upper boundary
   loadOneByteLOp(opcode, scope);
  // try to use short jump if offset small (< 0x10?)
   compileJumpLessOrEqual(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileDThen(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test esi, esi
   scope.code->writeWord(0xF685);

  // try to use short jump if offset small (< 0x10?)
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileDElse(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.argument;

  // test esi, esi
   scope.code->writeWord(0xF685);

  // try to use short jump if offset small (< 0x10?)
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

//void _ELENA_::compileElseLocal(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp [eax-0x0C], -1
//   scope.code->writeWord(0x7881);
//   scope.code->writeByte(0xF4);
//   scope.code->writeDWord(-1);
//
//  // try to use short jump if offset small (< 0x10?)
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}

void _ELENA_::compileMThen(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();
   int message = scope.resolveMessage(scope.argument);

   // cmp edx, message
   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(message);

   // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileMElse(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();
   int message = scope.resolveMessage(scope.argument);

   // cmp edx, message
   scope.code->writeWord(0xFA81);
   scope.code->writeDWord(message);

  // try to use short jump if offset small (< 0x10?)
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

//void _ELENA_::compileMThenSI(int opcode, x86JITScope& scope)
//{
//   //int jumpOffset = scope.tape->getDWord();
//
//   //// cmp edx, [esp-i]
//   //scope.code->writeWord(0x943B);
//   //scope.code->writeByte(0x24);
//   //scope.code->writeDWord(scope.argument << 2);
//
//   //// try to use short jump if offset small (< 0x10?)
//   //compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}
//
//void _ELENA_::compileMElseSI(int opcode, x86JITScope& scope)
//{
//  // int jumpOffset = scope.tape->getDWord();
//
//  // // cmp edx, [eax+i]
//  //// scope.code->writeWord(0x943B);
//  //// scope.code->writeByte(0x24);
//  //// scope.code->writeDWord(scope.argument << 2);
//
//  //// try to use short jump if offset small (< 0x10?)
//  // compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
//}

void _ELENA_::compileMThenAccI(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, [eax+i]
   scope.code->writeWord(0x903B);
   scope.code->writeDWord(scope.argument << 2);

   // try to use short jump if offset small (< 0x10?)
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileMElseAccI(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp edx, [eax+i]
   scope.code->writeWord(0x903B);
   scope.code->writeDWord(scope.argument << 2);

   // try to use short jump if offset small (< 0x10?)
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (__abs(jumpOffset) < 0x10));
}

void _ELENA_::compileCreate(int opcode, x86JITScope& scope)
{
   scope.argument <<= 2;

   // mov  ebx, #gc_page + (length - 1)
   scope.code->writeByte(0xBB);
   scope.code->writeDWord(align(scope.argument + scope.objectSize, gcPageSize));

   loadNOp(opcode, scope);

   // set vmt reference
   // mov [eax-4], vmt
   scope.code->writeWord(0x40C7);
   scope.code->writeByte(0xFC);
   scope.writeReference(*scope.code, scope.tape->getDWord(), 0);
}

void _ELENA_::compileCreateN(int opcode, x86JITScope& scope)
{
   int size = align(scope.argument + scope.objectSize, gcPageSize);

   scope.argument = -scope.argument;  // mark object as a binary structure

   // mov  ebx, #gc_page + (size - 1)
   scope.code->writeByte(0xBB);
   scope.code->writeDWord(size);

   loadNOp(opcode, scope);

   // set vmt reference
   // mov [eax-4], vmt
   scope.code->writeWord(0x40C7);
   scope.code->writeByte(0xFC);
   scope.writeReference(*scope.code, scope.tape->getDWord(), 0);
}

//void _ELENA_::compileGetLen(int opcode, x86JITScope& scope)
//{
//   loadFPOp(opcode, scope);
//
//   if (scope.argument <= 1) {
//   }
//   else if (scope.argument == 2) {
//      // shr eax, 1
//      scope.code->writeWord(0xE8D1);
//   }
//   else if (scope.argument == 4) {
//      // shr eax, 2
//      scope.code->writeWord(0xE8C1);
//      scope.code->writeByte(0x02);
//   }
//   else {
//      // mov  ebx, nn
//      // idiv ebx
//      scope.code->writeByte(0xBB);
//      scope.code->writeDWord(scope.argument);
//      scope.code->writeWord(0xFBF7);
//   }
//}

void _ELENA_::compileBoxN(int opcode, x86JITScope& scope)
{
   loadROp(opcode, scope);
}

//void _ELENA_::compileAccCreate(int opcode, x86JITScope& scope)
//{
//   // shl eax, 2
//   scope.code->writeWord(0xE0C1);
//   scope.code->writeByte(2);
//
//   loadNOp(opcode, scope);
//
//   // set vmt reference
//   // mov [eax-4], vmt
//   scope.code->writeWord(0x40C7);
//   scope.code->writeByte(0xFC);
//   scope.writeReference(*scope.code, scope.argument, 0);
//}
//
//void _ELENA_::compileIAccFillR(int opcode, x86JITScope& scope)
//{
//   ref_t reference = scope.tape->getDWord();
//
//   scope.code->writeByte(0xBB);
//   scope.writeReference(*scope.code, reference, 0);
//
//   loadIndexOp(opcode, scope);
//}

void _ELENA_::compileAccSetR(int opcode, x86JITScope& scope)
{
   // mov eax, r
   scope.code->writeByte(0xB8);
   scope.writeReference(*scope.code, scope.argument, 0);
}

void _ELENA_::compileDCopyI(int opcode, x86JITScope& scope)
{
   // mov esi, i
   scope.code->writeByte(0xBE);
   scope.code->writeDWord(scope.argument);

}

void _ELENA_::compileDCopyAI(int opcode, x86JITScope& scope)
{
   // mov esi, [eax + arg]
   scope.code->writeWord(0xB08B);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileDAddAI(int opcode, x86JITScope& scope)
{
   // add esi, [eax + arg]
   scope.code->writeWord(0xB003);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileDSubAI(int opcode, x86JITScope& scope)
{
   // sub esi, [eax + arg]
   scope.code->writeWord(0xB02B);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileDSaveAI(int opcode, x86JITScope& scope)
{
   // mov [eax + arg], esi
   scope.code->writeWord(0xB089);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileALoadD(int opcode, x86JITScope& scope)
{
   // mov eax, [eax + esi*4]
   scope.code->writeWord(0x048B);
   scope.code->writeByte(0xB0);
}

void _ELENA_::compileAccSetN(int opcode, x86JITScope& scope)
{
   // mov eax, n
   scope.code->writeByte(0xB8);
   scope.code->writeDWord(scope.argument);
}

void _ELENA_::compileAccLoadR(int opcode, x86JITScope& scope)
{
   // mov eax, [r]
   scope.code->writeByte(0xA1);
   scope.writeReference(*scope.code, scope.argument, 0);
}

void _ELENA_::compilePushAcc(int opcode, x86JITScope& scope)
{
   // push eax
   scope.code->writeByte(0x50);
}

void _ELENA_::compilePushF(int opcode, x86JITScope& scope)
{
   scope.code->writeWord(0xB5FF);
   if (scope.argument < 0) {
      // push [ebp + prev_bsp - level * 4]
      scope.code->writeDWord(scope.prevFSPOffs - (scope.argument << 2));
   }
   else {
      // push [ebp-level*4]
      scope.code->writeDWord(-(scope.argument << 2));
   }
}

void _ELENA_:: compileXPushFPI(int opcode, x86JITScope& scope)
{
   // invert index
   scope.argument = -scope.argument;

   loadIndexOp(opcode, scope);
}

void _ELENA_:: compilePushFPI(int opcode, x86JITScope& scope)
{
   // invert index
   if (scope.argument < 0) {
      scope.argument = scope.prevFSPOffs - (scope.argument << 2);
   }
   else scope.argument = -(scope.argument << 2);   

   loadNOp(bcXPushF, scope);
}

void _ELENA_::compilePushSelf(int opcode, x86JITScope& scope)
{
   // push edi
   scope.code->writeByte(0x57);
}

void _ELENA_::compileLoadField(int opcode, x86JITScope& scope)
{
   // push [edi + offset * 4]
   scope.code->writeWord(0xB7FF);
   scope.code->writeDWord(scope.argument << 2);
}

void _ELENA_::compileCallR(int opcode, x86JITScope& scope)
{
   // call symbol
   scope.code->writeByte(0xE8);
   scope.writeReference(*scope.code, scope.argument | mskRelCodeRef, 0);
}

void _ELENA_::compilePopFI(int opcode, x86JITScope& scope)
{
   // pop ecx
   scope.code->writeByte(0x59);

   if (scope.argument < 0) {
      // mov [ebp + prev_bsp - level * 4], ecx
      x86Helper::movMR32disp(scope.code, x86Helper::otEBP, x86Helper::otECX, scope.prevFSPOffs - (scope.argument << 2));
   }
   else {
      // mov [ebp-level*4], ecx
      x86Helper::movMR32disp(scope.code, x86Helper::otEBP, x86Helper::otECX, -(scope.argument << 2));
   }
}

void _ELENA_::compilePop(int opcode, x86JITScope& scope)
{
   // pop ecx
   scope.code->writeByte(0x59);
}

void _ELENA_::compilePopAcc(int opcode, x86JITScope& scope)
{
   // pop eax
   scope.code->writeByte(0x58);
}

//void _ELENA_::compileXMSet(int opcode, x86JITScope& scope)
//{
//   // and edx, PARAM_MASK
//   // or  edx, message
//   scope.code->writeWord(0xE281);
//   scope.code->writeDWord(PARAM_MASK);
//   scope.code->writeByte(0xBA);
//   scope.code->writeDWord(scope.resolveMessage(scope.argument));
//}

void _ELENA_::compileMSet(int opcode, x86JITScope& scope)
{
   // mov edx, message
   scope.code->writeByte(0xBA);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compileMAdd(int opcode, x86JITScope& scope)
{
   // or edx, message
   scope.code->writeWord(0xCA81);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compileAAdd(int opcode, x86JITScope& scope)
{
   // add eax, arg1
   scope.code->writeByte(0x05);
   scope.code->writeDWord(scope.resolveMessage(scope.argument));
}

void _ELENA_::compileAMul(int opcode, x86JITScope& scope)
{
   // imul eax, arg1
   if (scope.argument == 2) {
      scope.code->writeWord(0xE0C1);
      scope.code->writeByte(0x01);
   }
   else if (scope.argument == 4) {
      scope.code->writeWord(0xE0C1);
      scope.code->writeByte(0x02);
   }
   else {
      scope.code->writeWord(0xC069);
      scope.code->writeDWord(scope.resolveMessage(scope.argument));
   }
}

void _ELENA_::compilePopN(int opcode, x86JITScope& scope)
{
   // lea esp, [esp + level * 4]

   x86Helper::leaRM32disp(
      scope.code, x86Helper::otESP, x86Helper::otESP, scope.argument << 2);
}

//void _ELENA_::compileSendVMTR(int opcode, x86JITScope& scope)
//{
//   loadROp(opcode, scope);
//}

void _ELENA_::compileAccLoadSelf(int opcode, x86JITScope& scope)
{
   // mov eax, edi
   scope.code->writeWord(0xF889);
}

void _ELENA_::compileAccSaveR(int opcode, x86JITScope& scope)
{
   // mov [ref], eax

   scope.code->writeWord(0x0589);
   scope.writeReference(*scope.code, scope.argument, 0);
}

//void _ELENA_::compileCallVMT(int opcode, x86JITScope& scope)
//{
//   // mov ecx, offset
//   scope.code->writeByte(0xB9);
//   scope.code->writeDWord(scope.tape->getDWord() << 3);
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
//      if (relocation[0]==-1) {
//         // resolve message offset
//         scope.writeReference(*scope.code, scope.argument, 0);
//      }
//
//      relocation += 2;
//      count--;
//   }
//   scope.code->seekEOF();
//}

void _ELENA_::compileInvokeVMT(int opcode, x86JITScope& scope)
{
   int message = scope.resolveMessage(scope.tape->getDWord());

   // mov edx, message
   scope.code->writeByte(0xBA);
   scope.code->writeDWord(message);

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

void _ELENA_::compileIAccCopyR(int opcode, x86JITScope& scope)
{
   ref_t reference = scope.tape->getDWord();

   // mov [eax + arg], r
   scope.code->writeWord(0x40C7);
   scope.code->writeByte(scope.argument << 2);
   scope.writeReference(*scope.code, reference, 0);
}

//void _ELENA_::compileIAccCopyN(int opcode, x86JITScope& scope)
//{
//   ref_t value = scope.tape->getDWord();
//
//   // mov ebx, n
//   // mov [eax + arg], ebx
//   scope.code->writeByte(0xBB);
//   scope.code->writeDWord(value);
//   scope.code->writeWord(0x9889);
//   scope.code->writeDWord(scope.argument << 2);
//}

void _ELENA_::compileAccCopySPI(int opcode, x86JITScope& scope)
{
   // lea eax, [esp + index]
   x86Helper::leaRM32disp(                     
      scope.code, x86Helper::otEAX, x86Helper::otESP, scope.argument << 2);
}

//void _ELENA_::compileRedirect(int opcode, x86JITScope& scope)
//{
//   // jmp [eax]
//   scope.code->writeWord(0x20FF);
//}

void _ELENA_::compileElseR(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, r
   // jnz lab

   scope.code->writeByte(0x3D);
   scope.writeReference(*scope.code, scope.argument, 0);
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileNext(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // add esi, 1
   // cmp esi, __arg1
   // jl  lab

   scope.code->writeWord(0xC683);
   scope.code->writeByte(1);
   scope.code->writeWord(0xFE81);
   scope.code->writeDWord(scope.argument);

   compileJumpLess(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileThenR(int opcode, x86JITScope& scope)
{
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, r
   // jz lab

   scope.code->writeByte(0x3D);
   scope.writeReference(*scope.code, scope.argument, 0);
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

//void _ELENA_::compileElseN(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp eax, n
//   // jz lab
//
//   scope.code->writeByte(0x3D);
//   scope.code->writeDWord(scope.argument);
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileThenN(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.tape->getDWord();
//
//   // cmp eax, n
//   // jnz lab
//
//   scope.code->writeByte(0x3D);
//   scope.code->writeDWord(scope.argument);
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}

void _ELENA_::compileElseSI(int opcode, x86JITScope& scope)
{
   int index = scope.argument;
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, [esp+index]
   // jz lab

   scope.code->writeWord(0x843B);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(index << 2);
   //NOTE: due to compileJumpX implementation - compileJumpIf is called
   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileThenSI(int opcode, x86JITScope& scope)
{
   int index = scope.argument;
   int jumpOffset = scope.tape->getDWord();

   // cmp eax, [esp+index]
   // jnz lab

   scope.code->writeWord(0x843B);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(index << 2);
   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

//void _ELENA_::compileMccElseAcc(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp eax, edx
//   // jz lab
//
//   scope.code->writeWord(0xC23B);
//   //NOTE: due to compileJumpX implementation - compileJumpIf is called
//   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}
//
//void _ELENA_::compileMccThenAcc(int opcode, x86JITScope& scope)
//{
//   int jumpOffset = scope.argument;
//
//   // cmp eax, [esp+index]
//   // jnz lab
//
//   scope.code->writeWord(0xC23B);
//   //NOTE: due to compileJumpX implementation - compileJumpIfNot is called
//   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
//}

void _ELENA_::compileElseFlag(int opcode, x86JITScope& scope)
{
   int flag = scope.argument;
   int jumpOffset = scope.tape->getDWord();

   // mov  ebx, [eax-4]
   // test [ebx-8], f
   // jz lab

   scope.code->writeWord(0x588B);
   scope.code->writeByte(0xFC);
   scope.code->writeWord(0x43F7);
   scope.code->writeByte(0xF8);
   scope.code->writeDWord(flag);

   compileJumpIfNot(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileThenFlag(int opcode, x86JITScope& scope)
{
   int flag = scope.argument;
   int jumpOffset = scope.tape->getDWord();

   // mov  ebx, [eax-4]
   // test [ebx-8], f
   // jnz lab

   scope.code->writeWord(0x588B);
   scope.code->writeByte(0xFC);
   scope.code->writeWord(0x43F7);
   scope.code->writeByte(0xF8);
   scope.code->writeDWord(flag);

   compileJumpIf(scope, scope.tape->Position() + jumpOffset, (jumpOffset > 0), (jumpOffset < 0x10));
}

void _ELENA_::compileReserve(int opcode, x86JITScope& scope)
{
   for(int i = 0 ; i < scope.argument ; i++) {
      // push 0
      scope.code->writeWord(0x006A);
   }
}

//void _ELENA_::compileAccInc(int opcode, x86JITScope& scope)
//{
//   // add eax, index
//   scope.code->writeByte(0x05);
//   scope.code->writeDWord(scope.argument << 2);
//}

void _ELENA_::compileIndexInc(int opcode, x86JITScope& scope)
{
   // add esi, 1
   scope.code->writeWord(0xC683);
   scope.code->writeByte(1);
}

void _ELENA_::compileIndexDec(int opcode, x86JITScope& scope)
{
   // sub esi, 1
   scope.code->writeWord(0xEE83);
   scope.code->writeByte(1);
}

//void _ELENA_::compileAccCopyM(int opcode, x86JITScope& scope)
//{
//   //// mov eax, m
//   //scope.code->writeByte(0xB8);
//   //scope.code->writeDWord(scope.resolveMessage(scope.argument));
//}

void _ELENA_::compileMccCopyVerb(int opcode, x86JITScope& scope)
{
   // and edx, VERB_MASK
   scope.code->writeWord(0xE281);
   scope.code->writeDWord(VERB_MASK | MESSAGE_MASK);
}

void _ELENA_::compileMccCopySubj(int opcode, x86JITScope& scope)
{
   // and edx, ~VERB_MASK
   scope.code->writeWord(0xE281);
   scope.code->writeDWord(~VERB_MASK);
}

//void _ELENA_::compileMccCopyAcc(int opcode, x86JITScope& scope)
//{
//   // mov edx, eax
//   scope.code->writeWord(0xD08B);
//}

void _ELENA_::compilePopSelf(int opcode, x86JITScope& scope)
{
   // pop edi
   scope.code->writeByte(0x5F);
}

//void _ELENA_::compileNWrite(int opcode, x86JITScope& scope)
//{
//   if (scope.argument == 4) {
//      // mov ebx, [eax]
//      // mov [edi], ebx
//      scope.code->writeWord(0x188B);
//      scope.code->writeWord(0x1F89);
//   }
//}
//
//void _ELENA_::compileWriteAcc(int opcode, x86JITScope& scope)
//{
//   // mov [edi], eax
//   scope.code->writeWord(0x0789);
//}

void _ELENA_::compileCallSI(int opcode, x86JITScope& scope)
{
   // mov ebx, [esp+index]
   scope.code->writeWord(0x9C8B);
   scope.code->writeByte(0x24);
   scope.code->writeDWord(scope.argument << 2);

   // HOTFIX: reload the argument
   scope.argument = scope.tape->getDWord();
   loadVMTIndexOp(opcode, scope);
}

//void _ELENA_::compileXAccSaveFI(int opcode, x86JITScope& scope)
//{
//   // invert index
//   scope.argument = -scope.argument;
//
//   loadIndexOp(bcAccSaveFI, scope);
//}

void _ELENA_::compileXAccCopyFPI(int opcode, x86JITScope& scope)
{
   // lea eax, [ebp+nn]
   scope.code->writeWord(0x858D);
   scope.code->writeDWord(-(scope.argument << 2));
}

void _ELENA_::compileAccCopyFPI(int opcode, x86JITScope& scope)
{
   // lea eax, [ebp+nn]
   if (scope.argument < 0) {
      scope.code->writeWord(0x858D);
      scope.code->writeDWord(scope.prevFSPOffs - (scope.argument << 2));
   }
   else {
      scope.code->writeWord(0x858D);
      scope.code->writeDWord(-(scope.argument << 2));
   }
}

// --- x86JITScope ---

x86JITScope :: x86JITScope(MemoryReader* tape, MemoryWriter* code, _ReferenceHelper* helper, x86JITCompiler* compiler, bool embeddedSymbols)
   : lh(code)
{
   this->tape = tape;
   this->code = code;
   this->helper = helper;
   this->compiler = compiler;
   this->debugMode = compiler->isDebugMode();
   this->objectSize = helper ? helper->getLinkerConstant(lnObjectSize) : 0;
   this->embeddedSymbols = embeddedSymbols;
   this->module = NULL;

   this->prevFSPOffs = 0;
}

// --- x86JITCompiler ---

x86JITCompiler :: x86JITCompiler(bool debugMode, bool embeddedSymbolMode)
{
   _debugMode = debugMode;
   _embeddedSymbolMode = embeddedSymbolMode;
}

void x86JITCompiler :: alignCode(MemoryWriter* writer, int alignment, bool code)
{
   writer->align(VA_ALIGNMENT, code ? 0x90 : 0x00);
}

void x86JITCompiler :: writePreloadedReference(x86JITScope& scope, ref_t reference, int position, int offset, char* code)
{
   if (!_preloaded.exist(reference& ~mskAnyRef)) {
      MemoryWriter writer(scope.code->Memory());

      _preloaded.add(reference & ~mskAnyRef, scope.helper->getVAddress(writer, mskCodeRef));

      // due to optimization section must be ROModule::ROSection instance
      SectionInfo info = scope.helper->getPredefinedSection(reference & ~mskAnyRef);
      // separate scoep should be used to prevent overlapping
      x86JITScope newScope(NULL, &writer, scope.helper, this, _embeddedSymbolMode);
      loadCoreOp(newScope, info.section ? (char*)info.section->get(0) : NULL);
   }
   _ELENA_::writePreloadedReference(scope, reference, position, offset, code);
}

void x86JITCompiler :: prepareCoreData(_ReferenceHelper& helper, _Memory* data, _Memory* rdata, _Memory* sdata)
{
   MemoryWriter writer(data);
   MemoryWriter rdataWriter(rdata);
   MemoryWriter sdataWriter(sdata);

   x86JITScope scope(NULL, &writer, &helper, this, _embeddedSymbolMode);
   for (int i = 0 ; i < coreVariableNumber ; i++) {
      if (!_preloaded.exist(coreFunctions[i])) {
         _preloaded.add(coreVariables[i], helper.getVAddress(writer, mskDataRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getPredefinedSection(coreVariables[i]);
         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }

   // GC SIZE Table
   _preloaded.add(CORE_GC_SIZE, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnGCSize));
   rdataWriter.writeDWord(helper.getLinkerConstant(lnYGRatio));

   // load GC static root
   _preloaded.add(CORE_STATICROOT, helper.getVAddress(sdataWriter, mskStatRef));

   // STAT COUNT
   _preloaded.add(CORE_STAT_COUNT, helper.getVAddress(rdataWriter, mskRDataRef));
   rdataWriter.writeDWord(0);
}

void x86JITCompiler :: prepareVMData(_ReferenceHelper& helper, _Memory* data)
{
   MemoryWriter writer(data);

   // VM TABLE
   _preloaded.add(CORE_VM_TABLE, helper.getVAddress(writer, mskDataRef));

   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_Instance));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_LoadSymbol));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_LoadName));
   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_Interprete));
//   writer.writeDWord(helper.getLinkerConstant(lnVMAPI_GetLastError));
}

void x86JITCompiler :: prepareCore(_ReferenceHelper& helper, _Memory* code)
{
   MemoryWriter writer(code);
   x86JITScope scope(NULL, &writer, &helper, this, _embeddedSymbolMode);

   for (int i = 0 ; i < coreFunctionNumber ; i++) {
      if (!_preloaded.exist(coreFunctions[i])) {
         _preloaded.add(coreFunctions[i], helper.getVAddress(writer, mskCodeRef));

         // due to optimization section must be ROModule::ROSection instance
         SectionInfo info = helper.getPredefinedSection(coreFunctions[i]);
         loadCoreOp(scope, info.section ? (char*)info.section->get(0) : NULL);
      }
   }
}

void x86JITCompiler :: prepareCommandSet(_ReferenceHelper& helper, _Memory* code)
{
   MemoryWriter writer(code);
   x86JITScope scope(NULL, &writer, NULL, this, _embeddedSymbolMode);

   // preload vm commands
   scope.helper = &helper;
   for (int i = 0 ; i < gcCommandNumber ; i++) {
      SectionInfo info = helper.getPredefinedSection(gcCommands[i]);

      // due to optimization section must be ROModule::ROSection instance
      _inlines[gcCommands[i]] = (char*)info.section->get(0);
   }

   // preload vm extension commands
   for (int i = 0 ; i < gcExtensionNumber ; i++) {
      SectionInfo info =  helper.getPredefinedSection(0x10000 | gcExtensions[i]);
      if (info.section) {
         // due to optimization section must be ROModule::ROSection instance
         _extensions[0][gcExtensions[i]] = (char*)info.section->get(0);
      }
   }
   for (int type = 1 ; type < 8 ; type++) {
      for (int i = 0 ; i < gcExtensionNumber ; i++) {
         SectionInfo info =  helper.getPredefinedSection((type << 12) | gcExtensions[i]);
         if (info.section) {
            // due to optimization section must be ROModule::ROSection instance
            _extensions[type][gcExtensions[i]] = (char*)info.section->get(0);
         }
      }
   }
}

void x86JITCompiler :: setStaticRootCounter(_JITLoader* loader, int counter, bool virtualMode)
{
   if (virtualMode) {
      _Memory* data = loader->getTargetSection(mskRDataRef);

      size_t offset = ((size_t)_preloaded.get(CORE_STAT_COUNT) & ~mskAnyRef);
      (*data)[offset] = (counter << 2);
   }
   else {
 	   size_t offset = (size_t)_preloaded.get(CORE_STAT_COUNT);
 	   *(int*)offset = (counter << 2);
   }
}

void* x86JITCompiler :: getPreloadedReference(ref_t reference)
{
   return (void*)_preloaded.get(reference);
}

////void x86JITCompiler :: compileTLS(_JITLoader* loader/*, bool virtualMode*/)
////{
////   MemoryWriter dataWriter(loader->getTargetSection(mskDataRef));
////
////   // reserve space for TLS index
////   int position = dataWriter.Position();
////   allocateVariable(dataWriter);
////
////   // map TLS index
////   ConstantIdentifier tlsKey(TLS_KEY);
////
////   loader->mapReference(tlsKey, (void*)(position | mskDataRef), mskDataRef);
////   _preloaded.add(CORE_TLS_INDEX, (void*)(position | mskDataRef));
////
////   // allocate tls section
////   MemoryWriter tlsWriter(loader->getTargetSection(mskTLSRef));
////   tlsWriter.writeDWord(0);   // frame pointer
////   tlsWriter.writeDWord(0);   // syncronization event
////   tlsWriter.writeDWord(0);   // thread flags
////
////   // map IMAGE_TLS_DIRECTORY
////   MemoryWriter rdataWriter(loader->getTargetSection(mskRDataRef));
////   loader->mapReference(tlsKey, (void*)(rdataWriter.Position() | mskRDataRef), mskRDataRef);
////
////   // create IMAGE_TLS_DIRECTORY
////   rdataWriter.writeRef(mskTLSRef, 0);          // StartAddressOfRawData
////   rdataWriter.writeRef(mskTLSRef, 12);         // EndAddressOfRawData
////   rdataWriter.writeRef(mskDataRef, position);  // AddressOfIndex
////   rdataWriter.writeDWord(0);                   // AddressOfCallBacks
////   rdataWriter.writeDWord(0);                   // SizeOfZeroFill
////   rdataWriter.writeDWord(0);                   // Characteristics
////}

inline void compileTape(MemoryReader& tapeReader, int endPos, x86JITScope& scope)
{
   unsigned char code = 0;
   while(tapeReader.Position() < endPos) {
      // read bytecode + arguments
      code = tapeReader.getByte();
      // preload an argument if a command requires it
      if (code >= 0x20) {
         scope.argument = tapeReader.getDWord();
      }
      commands[code](code, scope);
   }
}

void x86JITCompiler :: embedSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter, _Module* module)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);
   scope.debugMode = false;   // embedded symbol does not provide a debug info
   scope.module = module;

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);
}

void x86JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

//   // ; copy the parameter from the previous frame to simulate embedded symbol
//   // push [esp+4]
//   codeWriter.writeDWord(0x042474FF);

   compileTape(tapeReader, endPos, scope);

   // ; copy the parameter to the accumulator to simulate embedded symbol
   // ; exit the procedure
   // ret
   codeWriter.writeByte(0xC3);  
}

void x86JITCompiler :: compileProcedure(_ReferenceHelper& helper, MemoryReader& tapeReader, MemoryWriter& codeWriter)
{
   x86JITScope scope(&tapeReader, &codeWriter, &helper, this, _embeddedSymbolMode);
   scope.prevFSPOffs = 4;

   size_t codeSize = tapeReader.getDWord();
   size_t endPos = tapeReader.Position() + codeSize;

   compileTape(tapeReader, endPos, scope);
}

void x86JITCompiler :: loadNativeCode(_BinaryHelper& helper, MemoryWriter& writer, _Module* binary, _Memory* section)
{
   size_t position = writer.Position();

   writer.write(section->get(0), section->Length());

   // resolve section references
   _ELENA_::RelocationMap::Iterator it(section->getReferences());
   while (!it.Eof()) {
      int arg = *it;
      writer.seek(arg + position);

      const wchar16_t* reference = binary->resolveReference(it.key() & ~mskAnyRef);

      helper.writeReference(writer, reference, it.key() & mskAnyRef);

      it++;
   }
   writer.seekEOF();
}
