//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitcompiler.h"
#include "langcommon.h"
#include "core.h"

// --- JITCompiler ---

using namespace elena_lang;

CodeGenerator _codeGenerators[256] =
{
   loadNop, compileBreakpoint, loadNop, loadOp, loadOp, loadOp, loadOp, loadOp,
   loadOp, loadOp, loadOp, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadROp, loadFrameDispOp, loadLenOp, loadIndexOp, loadROp, loadROp, loadNop, loadNop,
   loadMOp, loadNOp, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNOp, compileClose, loadIndexOp, loadIndexOp, loadNOp, loadNOp, loadNOp, loadNOp,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadFrameDispOp, loadFrameIndexOp, loadStackIndexOp, loadStackIndexOp, loadStackIndexOp, loadFieldIndexOp, loadNop, loadNop,
   loadFrameIndexOp, loadStackIndexOp, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadCallROp, loadVMTIndexOp, compileJump, compileJeq, compileJne, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadROp, loadNop, loadIOp, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadFrameIndexOp, loadStackIndexOp, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,
   loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop, loadNop,

   loadDPNOp, loadDPNOp, loadDPNOp, loadDPNOp, loadDPNOp, loadDPNOp, loadDPROp, loadNewOp,
   loadNop, loadNop, loadNop, loadNop, loadVMTROp, loadMROp, loadRROp, loadRROp,

   compileOpen, loadStackIndexROp, compileOpen, loadStackIndexFrameIndexOp, loadNewOp, loadNewNOp, loadStackIndexIndexOp, loadCreateNOp,
   loadNop, loadFrameIndexROp, compileDispatchMR, compileDispatchMR, loadVMTROp, loadMROp, loadCallOp, loadNop,
};

// preloaded gc routines
constexpr int coreVariableNumber = 2;
constexpr ref_t coreVariables[coreVariableNumber] =
{
   CORE_GC_TABLE, CORE_EH_TABLE
};

constexpr int coreConstantNumber = 4;
constexpr ref_t coreConstants[coreConstantNumber] =
{
   // NOTE: SYSTEM_ENV should be the last one to add correctly extra fields: GCMGSize, GCYGSize
   CORE_TOC, VOIDOBJ, VOIDPTR, SYSTEM_ENV
};

// preloaded gc routines
constexpr int coreFunctionNumber = 3;
constexpr ref_t coreFunctions[coreFunctionNumber] =
{
   INVOKER, GC_ALLOC, EXCEPTION_HANDLER
};

// preloaded bc commands
constexpr size_t bcCommandNumber = 63;
constexpr ByteCode bcCommands[bcCommandNumber] =
{
   ByteCode::MovEnv, ByteCode::SetR, ByteCode::SetDP, ByteCode::CloseN, ByteCode::AllocI,
   ByteCode::FreeI, ByteCode::SaveDP, ByteCode::StoreFI, ByteCode::OpenIN, ByteCode::XStoreSIR,
   ByteCode::OpenHeaderIN, ByteCode::CallExtR, ByteCode::MovSIFI, ByteCode::PeekFI, ByteCode::Load,
   ByteCode::SaveSI, ByteCode::CallR, ByteCode::Quit, ByteCode::MovM, ByteCode::CallVI,
   ByteCode::StoreSI, ByteCode::Redirect, ByteCode::NewIR, ByteCode::XFlushSI, ByteCode::Copy,
   ByteCode::NewNR, ByteCode::CallMR, ByteCode::VCallMR, ByteCode::DispatchMR, ByteCode::CopyDPN,
   ByteCode::IAddDPN, ByteCode::ISubDPN, ByteCode::IMulDPN, ByteCode::IDivDPN, ByteCode::PeekSI,
   ByteCode::Len, ByteCode::NLen, ByteCode::XMovSISI, ByteCode::CmpR, ByteCode::VJumpMR,
   ByteCode::JumpMR, ByteCode::CmpFI, ByteCode::CmpSI, ByteCode::SelEqRR, ByteCode::XDispatchMR,
   ByteCode::ICmpN, ByteCode::SelLtRR, ByteCode::XAssignI, ByteCode::GetI, ByteCode::PeekR,
   ByteCode::StoreR, ByteCode::Class, ByteCode::NSaveDPN, ByteCode::Save, ByteCode::AndN,
   ByteCode::ReadN, ByteCode::WriteN, ByteCode::CreateNR, ByteCode::Throw, ByteCode::XHookDPR,
   ByteCode::CmpN, ByteCode::MovN, ByteCode::XNewNR
};

void elena_lang :: writeCoreReference(JITCompilerScope* scope, ref_t reference/*, pos_t position*/,
   pos_t disp, void* code)
{
   // references should be already preloaded
   ref_t mask = reference & mskAnyRef;
   switch (mask) {
      case mskCodeRef32:
      case mskDataRef32:
      case mskMDataRef32:
         scope->helper->writeVAddress32(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskCodeRef64:
      case mskDataRef64:
      case mskMDataRef64:
         scope->helper->writeVAddress64(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskCodeRelRef32:
      case mskDataRelRef32:
         scope->helper->writeRelAddress32(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskImportRef64:
         scope->helper->writeVAddress64(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos64_t*)((char*)code + disp), mask);
         break;
      case mskRDataRef32:
         scope->helper->writeVAddress32(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskRDataRef64:
         scope->helper->writeVAddress64(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskDataRef32Lo:
      case mskRDataRef32Lo:
      case mskMDataRef32Lo:
      case mskCodeRef32Lo:
         scope->helper->writeVAddress32Lo(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskDataRef32Hi:
      case mskRDataRef32Hi:
      case mskMDataRef32Hi:
      case mskCodeRef32Hi:
         scope->helper->writeVAddress32Hi(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskDataDisp32Hi:
      case mskRDataDisp32Hi:
      case mskCodeDisp32Hi:
         scope->helper->writeDisp32Hi(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      case mskDataDisp32Lo:
      case mskRDataDisp32Lo:
      case mskCodeDisp32Lo:
         scope->helper->writeDisp32Lo(*scope->codeWriter->Memory(), scope->codeWriter->position(),
            (addr_t)scope->compiler->_preloaded.get(reference & ~mskAnyRef) & ~mskAnyRef,
            *(pos_t*)((char*)code + disp), mask);
         break;
      default:
         // to make compiler happy
         break;
   }
}

inline int getFPOffset(int argument, int argOffset)
{
   return -(argument - (argument < 0 ? argOffset : 0));
}

void elena_lang :: loadNop(JITCompilerScope* scope)
{
   // nop command is used to indicate possible label
   // fix the label if it exists
   pos_t position = scope->tapeReader->position() - 1;
   if (scope->lh->checkLabel(position)) {
      scope->lh->fixLabel(position, *scope->codeWriter);
   }
   // or add the label
   else scope->lh->setLabel(position, *scope->codeWriter);
}

void elena_lang :: loadLOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = scope->compiler->_inlines[0][scope->code()];
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);
}

void elena_lang :: allocateCode(JITCompilerScope* scope, void* code)
{
   MemoryWriter* writer = scope->codeWriter;
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   writer->writeBytes(0, length);
}

void elena_lang :: loadCode(JITCompilerScope* scope, void* code)
{
   MemoryWriter* writer = scope->codeWriter;

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);

      writeCoreReference(scope, entries->reference, entries->offset, code);

      entries++;
      count--;
   }
   writer->seek(position + length);
}

void elena_lang :: loadOp(JITCompilerScope* scope)
{
   loadCode(scope, scope->compiler->_inlines[0][scope->code()]);
}

void* elena_lang :: retrieveCode(JITCompilerScope* scope)
{
   void* code = nullptr;
   switch (scope->command.arg1) {
      case 0:
         code = scope->compiler->_inlines[1][scope->code()];
         break;
      case 1:
         code = scope->compiler->_inlines[2][scope->code()];
         break;
      case 2:
         code = scope->compiler->_inlines[3][scope->code()];
         break;
      case 3:
         code = scope->compiler->_inlines[4][scope->code()];
         break;
      case 4:
         code = scope->compiler->_inlines[5][scope->code()];
         break;
      default:
         code = scope->compiler->_inlines[0][scope->code()];
         break;
   }
   return code;
}

void* elena_lang :: retrieveIndexRCode(JITCompilerScope* scope)
{
   size_t index = 0;
   switch (scope->command.arg1) {
      case 0:
         index = 1;
         break;
      case 1:
         index = 2;
         break;
      case 2:
         index = 3;
         break;
      case 3:
         index = 4;
         break;
      case 4:
         index = 5;
        break;
      default:
         break;
   }
   if (scope->command.arg2 == 0)
      index += 5;

   return scope->compiler->_inlines[index][scope->code()];
}

void* elena_lang::retrieveCodeWithNegative(JITCompilerScope* scope)
{
   arg_t arg = scope->command.arg1;

   void* code = nullptr;
   switch (arg) {
      case 0:
         code = scope->compiler->_inlines[1][scope->code()];
         break;
      case 1:
         code = scope->compiler->_inlines[2][scope->code()];
         break;
      case 2:
         code = scope->compiler->_inlines[3][scope->code()];
         break;
      case 3:
         code = scope->compiler->_inlines[4][scope->code()];
         break;
      default:
         if (arg < 0) {
            code = scope->compiler->_inlines[5][scope->code()];
         }
         else code = scope->compiler->_inlines[0][scope->code()];
         break;
   }
   return code;
}

void elena_lang :: loadIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(scope->command.arg1 << scope->constants->indexPower);
            break;
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case ARG16_1:
            writer->writeWord((unsigned short)scope->command.arg1 << scope->constants->indexPower);
            break;
         case INV_ARG16_1:
            writer->writeWord((unsigned short)-(scope->command.arg1 << scope->constants->indexPower));
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer,
               scope->command.arg1 << scope->constants->indexPower,
               0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadNOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case NARG16_1:
            scope->compiler->writeImm16(writer, scope->command.arg1, 0);
            break;
         case NARG12_1:
            scope->compiler->writeImm12(writer, scope->command.arg1, 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadFieldIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCodeWithNegative(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   int arg1 = 0;
   if (scope->command.arg1 < 0) {
      arg1 = (scope->command.arg1 << scope->constants->indexPower) - scope->constants->vmtSize;
   }
   else arg1 = scope->command.arg1 << scope->constants->indexPower;

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
      case ARG32_1:
         writer->writeDWord(arg1);
         break;
      case NARG_1:
         writer->writeDWord(arg1);
         break;
      case ARG16_1:
         writer->writeWord((unsigned short)arg1);
         break;
      case ARG12_1:
         scope->compiler->writeImm12(writer, arg1, 0);
         break;
      case INV_ARG12_1:
         scope->compiler->writeImm12(writer, -arg1, 0);
         break;
      default:
         // to make compiler happy
         break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadStackIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   arg_t arg1 = scope->stackOffset + scope->command.arg1;
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(arg1 << scope->constants->indexPower);
            break;
         case NARG_1:
            writer->writeDWord(arg1);
            break;
         case ARG16_1:
            writer->writeWord((unsigned short)arg1 << scope->constants->indexPower);
            break;
         case INV_ARG16_1:
            writer->writeWord((unsigned short)-(arg1 << scope->constants->indexPower));
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, arg1 << scope->constants->indexPower, 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadVMTIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   arg_t addrOffs = ((scope->command.arg1 << 1) + 1) << scope->constants->indexPower;
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(addrOffs);
            break;
         case NARG_1:
            writer->writeDWord(addrOffs);
            break;
         case ARG16_1:
            writer->writeWord((unsigned short)addrOffs);
            break;
         case INV_ARG16_1:
            writer->writeWord((unsigned short)-addrOffs);
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, addrOffs, 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadFrameIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCodeWithNegative(scope);
   arg_t arg1 = scope->command.arg1;

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(getFPOffset(
               scope->command.arg1 << scope->constants->indexPower, scope->frameOffset));
            break;
         case ARG16_1:
            writer->writeWord((unsigned short)getFPOffset(
               scope->command.arg1 << scope->constants->indexPower, scope->frameOffset));
            break;
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer,
               getFPOffset(arg1 << scope->constants->indexPower, scope->frameOffset),
               0);
            break;
         case INV_ARG12_1:
            scope->compiler->writeImm12(writer,
               -getFPOffset(arg1 << scope->constants->indexPower, scope->frameOffset),
               0);
            break;
         case ARG9_1:
            scope->compiler->writeImm9(writer,
               getFPOffset(scope->command.arg1 << scope->constants->indexPower, scope->frameOffset),
               0);
            break;
         default:
            // to make compiler happy
            //writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadFrameDispOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG16_1:
            writer->writeWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         case ARG9_1:
            scope->compiler->writeImm9(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadIOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveICode(scope, scope->command.arg1);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case NARG16_1:
         case ARG16_1:
            scope->compiler->writeImm16(writer, (short)scope->command.arg1, 0);
            break;
         case NARG12_1:
            scope->compiler->writeImm12(writer, (short)scope->command.arg1, 0);
            break;
         default:
            //writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadLenOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = nullptr;
   switch (scope->command.arg1) {
      case 1:
         code = scope->compiler->_inlines[1][scope->code()];
         break;
      case 2:
         code = scope->compiler->_inlines[2][scope->code()];
         break;
      case 4:
         code = scope->compiler->_inlines[3][scope->code()];
         break;
      case 8:
         code = scope->compiler->_inlines[4][scope->code()];
         break;
      default:
         code = scope->compiler->_inlines[0][scope->code()];
         break;
   }

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case NARG16_1:
         case ARG16_1:
            scope->compiler->writeImm16(writer, (short)scope->command.arg1, 0);
            break;
         default:
            //writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));
   ref_t arg = scope->command.arg1;

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case PTR32_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskRef32);
            break;
         case PTR64_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskRef64);
            break;
         case DISP32HI_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskDisp32Hi);
            break;
         case DISP32LO_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskDisp32Lo);
            break;
         case XDISP32HI_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskXDisp32Hi);
            break;
         case XDISP32LO_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskXDisp32Lo);
            break;
         case PTR32HI_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Lo);
            break;
         }
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadRROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));
   ref_t arg = scope->command.arg1;
   ref_t arg2 = scope->command.arg2;

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case PTR32_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskRef32);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskRef32);
            break;
         case PTR64_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskRef64);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskRef64);
            break;
         case DISP32HI_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskDisp32Hi);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskDisp32Lo);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskDisp32Lo);
            break;
         case XDISP32HI_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskXDisp32Hi);
            break;
         case XDISP32HI_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskXDisp32Hi);
            break;
         case XDISP32LO_1:
            scope->compiler->writeArgAddress(scope, arg, 0, mskXDisp32Lo);
            break;
         case XDISP32LO_2:
            scope->compiler->writeArgAddress(scope, arg2, 0, mskXDisp32Lo);
            break;
         case PTR32HI_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Hi);
            break;
         }
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Lo);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadMOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));
   ref_t arg = scope->command.arg1;

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeRef(scope->helper->importMessage(arg));
            break;
         case ARG32HI_1:
            scope->compiler->writeImm16(writer, scope->helper->importMessage(arg) >> 16, 0);
            break;
         case ARG32LO_1:
            scope->compiler->writeImm16(writer, scope->helper->importMessage(arg) & 0xFFFF, 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadCallOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = nullptr;
   switch (scope->command.arg2) {
       case 0:
           code = scope->compiler->_inlines[1][scope->code()];
           break;
       case 1:
           code = scope->compiler->_inlines[2][scope->code()];
           break;
       case 2:
           code = scope->compiler->_inlines[3][scope->code()];
           break;
       case 3:
           code = scope->compiler->_inlines[4][scope->code()];
           break;
       case 4:
          code = scope->compiler->_inlines[5][scope->code()];
          break;
       default:
           code = scope->compiler->_inlines[0][scope->code()];
           break;
   }

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      if (entries->reference == PTR32_1) {
         scope->compiler->writeArgAddress(scope, scope->command.arg1, 0, mskRef32);
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadCallROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = scope->compiler->_inlines[0][scope->code()];

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case PTR32_1:
            scope->compiler->writeArgAddress(scope, scope->command.arg1, 0, mskRef32);
            break;
         case RELPTR32_1:
            scope->compiler->writeArgAddress(scope, scope->command.arg1, 0, mskRelRef32);
            break;
         case DISP32HI_1:
            scope->compiler->writeArgAddress(scope, scope->command.arg1, 0, mskDisp32Hi);
            break;
         case DISP32LO_1:
            scope->compiler->writeArgAddress(scope, scope->command.arg1, 0, mskDisp32Lo);
            break;
         case PTR32HI_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_1:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg1, disp, mskRef32Lo);
            break;
         }
         default:
            //else writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadStackIndexROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveIndexRCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   arg_t arg1 = scope->stackOffset + scope->command.arg1;
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(arg1 << scope->constants->indexPower);
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, scope->command.arg1 << scope->constants->indexPower, 0);
            break;
         case ARG16_1:
            scope->compiler->writeImm16(writer, scope->command.arg1 << scope->constants->indexPower, 0);
            break;
         case PTR32_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            }
            else writer->writeDWord(0);
            break;
         case PTR64_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            }
            else writer->writeQWord(0);
            break;
         case DISP32HI_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         case DISP32LO_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         case XDISP32HI_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Hi);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         case XDISP32LO_2:
            if (scope->command.arg2) {
               scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Lo);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         case PTR32HI_2:
         {
            if (scope->command.arg2) {
               short disp = *(short*)((char*)code + entries->offset);
               scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         }
         case PTR32LO_2:
         {
            if (scope->command.arg2) {
               short disp = *(short*)((char*)code + entries->offset);
               scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            }
            else scope->compiler->writeImm16(writer, 0, 0);
            break;
         }
         default:
             writeCoreReference(scope, entries->reference, entries->offset, code);
             break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadFrameIndexROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(getFPOffset(
               scope->command.arg1 << scope->constants->indexPower, scope->frameOffset));
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case XDISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Hi);
            break;
         case XDISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadIndexNOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = nullptr;
   if (scope->command.arg2 == 0) {
      switch (scope->command.arg1) {
         case 0:
            code = scope->compiler->_inlines[1][scope->code()];
            break;
         case 1:
            code = scope->compiler->_inlines[2][scope->code()];
            break;
         case 2:
            code = scope->compiler->_inlines[3][scope->code()];
            break;
         case 3:
            code = scope->compiler->_inlines[4][scope->code()];
            break;
         default:
            code = scope->compiler->_inlines[6][scope->code()];
            break;
      }
   }
   else {
      switch (scope->command.arg1) {
         case 0:
            code = scope->compiler->_inlines[5][scope->code()];
            break;
         default:
            code = scope->compiler->_inlines[0][scope->code()];
            break;
      }
   }

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(scope->command.arg1 << scope->constants->indexPower);
            break;
         case INV_ARG16_1:
            writer->writeWord(-(scope->command.arg1 << scope->constants->indexPower));
            break;
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case NARG_2:
            writer->writeDWord(scope->command.arg2);
            break;
         case NARG12_2:
            scope->compiler->writeImm12(writer, scope->command.arg2, 0);
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, scope->command.arg1 << scope->constants->indexPower, 0);
            break;
         case ARG16_1:
            scope->compiler->writeImm16(writer, scope->command.arg1 << scope->constants->indexPower, 0);
            break;
         case NARG16_2:
            scope->compiler->writeImm16(writer, scope->command.arg2, 0);
            break;
         case INV_NARG16_2:
            scope->compiler->writeImm16(writer, scope->command.arg2, INV_ARG);
            break;
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadStackIndexFrameIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   arg_t arg1 = scope->stackOffset + scope->command.arg1;
   arg_t arg2 = getFPOffset(scope->command.arg2 << scope->constants->indexPower, scope->frameOffset);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(arg1 << scope->constants->indexPower);
            break;
         case ARG32_2:
            writer->writeDWord(arg2);
            break;
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadStackIndexIndexOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   int arg1 = scope->command.arg1;
   int arg2 = scope->command.arg2;

   void* code = scope->compiler->_inlines[0][scope->code()];;
   if (arg1 == 0 && arg2 == 1) {
      code = scope->compiler->_inlines[5][scope->code()];
   }
   else if (arg1 == 1 && arg2 == 0) {
      code = scope->compiler->_inlines[6][scope->code()];
   }
   else if (arg1 == 0) {
      code = scope->compiler->_inlines[1][scope->code()];
   }
   else if (arg2 == 0) {
      code = scope->compiler->_inlines[2][scope->code()];
   }
   else if (arg1 == 1) {
      code = scope->compiler->_inlines[3][scope->code()];
   }
   else if (arg2 == 1) {
      code = scope->compiler->_inlines[4][scope->code()];
   }

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // HOTFIX : for some CPU the unframed stack already contains the returning address,
   // which should be taken into account
   arg1 += scope->stackOffset;
   arg2 += scope->stackOffset;

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(arg1 << scope->constants->indexPower);
            break;
         case ARG32_2:
            writer->writeDWord(arg2 << scope->constants->indexPower);
            break;
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadNewOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(scope->compiler->calcTotalSize(scope->command.arg1));
            break;
         case ARG16_1:
            scope->compiler->writeImm16(writer, (short)scope->compiler->calcTotalSize(scope->command.arg1), 0);
            break;
         case NARG_1:
            writer->writeDWord(scope->command.arg1);
            break;
         case NARG16_1:
            scope->compiler->writeImm16(writer, (short)scope->command.arg1, 0);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadNewNOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   int narg = scope->command.arg1 | scope->constants->structMask;

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(scope->compiler->calcTotalStructSize(scope->command.arg1));
            break;
         case ARG16_1:
            scope->compiler->writeImm16(writer, (short)scope->compiler->calcTotalStructSize(scope->command.arg1), 0);
            break;
         case NARG_1:
            writer->writeDWord(narg);
            break;
         case NARG16_1:
            scope->compiler->writeImm16(writer, (short)narg, 0);
            break;
         case NARGHI_1:
            scope->compiler->writeImm16(writer, (short)(narg >> 16), 0);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadCreateNOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveCode(scope);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   int narg = scope->command.arg1;

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case NARG_1:
            writer->writeDWord(narg);
            break;
         case NARG16_1:
            scope->compiler->writeImm16(writer, (short)narg, 0);
            break;
         case NARGHI_1:
            scope->compiler->writeImm16(writer, (short)(narg >> 16), 0);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang :: loadMROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = scope->compiler->_inlines[0][scope->code()];

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case PTR32_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskRef32);
            break;
         case RELPTR32_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskRelRef32);
            break;
         case DISP32HI_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskDisp32Lo);
            break;
         case XDISP32HI_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskXDisp32Hi);
            break;
         case XDISP32LO_2:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               0, scope->helper->importMessage(scope->command.arg1), mskXDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               disp, scope->helper->importMessage(scope->command.arg1), mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodAddress,
               disp, scope->helper->importMessage(scope->command.arg1), mskRef32Lo);
            break;
         }
         default:
            //else writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadVMTROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = scope->compiler->_inlines[0][scope->code()];

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodOffset,
               0, scope->helper->importMessage(scope->command.arg1), mskRef32);
            break;
         case ARG12_1:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodOffset,
               0, scope->helper->importMessage(scope->command.arg1), mskRef32Lo12);
            break;
         case ARG16_1:
            scope->compiler->writeVMTMethodArg(scope, scope->command.arg2 | mskVMTMethodOffset,
               0, scope->helper->importMessage(scope->command.arg1), mskRef32Lo);
            break;
         default:
            //else writeCoreReference();
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

inline void* elena_lang::retrieveICode(JITCompilerScope* scope, int arg)
{
   switch (arg) {
      case 1:
         return scope->compiler->_inlines[1][scope->code()];
      case 2:
         return scope->compiler->_inlines[2][scope->code()];
      case 4:
         return scope->compiler->_inlines[3][scope->code()];
      case 8:
         return scope->compiler->_inlines[4][scope->code()];
      default:
         return scope->compiler->_inlines[0][scope->code()];
   }
}

inline void* elena_lang :: retrieveRCode(JITCompilerScope* scope, int arg)
{
   switch (arg) {
      case 0:
         return scope->compiler->_inlines[1][scope->code()];
      default:
         return scope->compiler->_inlines[0][scope->code()];
   }
}

void elena_lang::loadDPNOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveICode(scope, scope->command.arg2);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG16_1:
            writer->writeWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         case ARG9_1:
            scope->compiler->writeImm9(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         case NARG_2:
            scope->compiler->writeImm32(writer, scope->command.arg2);
            break;
         case NARG16_2:
            scope->compiler->writeImm16(writer, scope->command.arg2, 0);
            break;
         case NARG12_2:
            scope->compiler->writeImm12(writer, scope->command.arg2, 0);
            break;
         default:
            // to make compiler happy
            break;
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();
}

void elena_lang::loadDPROp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = retrieveRCode(scope, scope->command.arg2);

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            writer->writeDWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG16_1:
            writer->writeWord(getFPOffset(scope->command.arg1, scope->constants->dataOffset));
            break;
         case ARG12_1:
            scope->compiler->writeImm12(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         case ARG9_1:
            scope->compiler->writeImm9(writer, getFPOffset(scope->command.arg1,
               scope->constants->dataOffset), 0);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case XDISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Hi);
            break;
         case XDISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskXDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();
}

inline void loadPreloaded(JITCompilerScope& scope, LibraryLoaderBase* loader, size_t length,
   const ref_t* functions, JITCompiler::PreloadedMap& map, Map<ref_t, pos_t>& positions,
   ref_t mask, bool declarating)
{
   for (size_t i = 0; i < length; i++) {
      // due to optimization section must be ROModule::ROSection instance
      auto info = loader->getCoreSection(functions[i], false);
      if (!info.section)
         throw InternalError(errCommandSetAbsent, functions[i]);

      if (declarating) {
         if (!map.exist(functions[i])) {
            map.add(functions[i], (void*)scope.helper->calculateVAddress(*scope.codeWriter, mask));
            positions.add(functions[i], scope.codeWriter->position());

            allocateCode(&scope, info.section->get(0));
         }
      }
      else {
         pos_t position = positions.get(functions[i]);
         scope.codeWriter->seek(position);

         loadCode(&scope, info.section->get(0));
      }
   }
}

inline void loadInline(ref_t index, void* inlines[][0x100], LibraryLoaderBase* loader)
{
   for (size_t i = 0; i < bcCommandNumber; i++) {
      auto info = loader->getCoreSection((ref_t)bcCommands[i] | (index << 8), false);
      if (info.section) {
         // due to optimization section must be ROModule::ROSection instance
         inlines[index][(ref_t)bcCommands[i]] = info.section->get(0);
      }
      else if (inlines[0][(ref_t)bcCommands[i]] != nullptr) {
         inlines[index][(ref_t)bcCommands[i]] = inlines[0][(ref_t)bcCommands[i]];
      }
      else throw InternalError(errCommandSetAbsent, (int)bcCommands[i]);
   }
}

void elena_lang::compileClose(JITCompilerScope* scope)
{
   if (scope->command.arg1 > 0)
      scope->command.arg1 += scope->constants->dataHeader;

   loadNOp(scope);
}

void elena_lang::compileOpen(JITCompilerScope* scope)
{
   scope->frameOffset = scope->compiler->calcFrameOffset(scope->command.arg2);
   scope->stackOffset = 0;

   loadIndexNOp(scope);
}

void elena_lang::compileBreakpoint(JITCompilerScope* scope)
{
   if (scope->withDebugInfo)
      scope->helper->addBreakpoint(*scope->codeWriter);
}

void elena_lang::compileJump(JITCompilerScope* scope)
{
   pos_t label = scope->tapeReader->position() + scope->command.arg1;

   if (scope->command.arg1 < 0) {
      // if it is a back jump
      scope->lh->writeJumpBack(label, *scope->codeWriter);
   }
   else if(scope->command.arg1 > 0) {
      scope->lh->writeJumpForward(label, *scope->codeWriter, scope->command.arg1);
   }
}

void elena_lang::compileJeq(JITCompilerScope* scope)
{
   pos_t label = scope->tapeReader->position() + scope->command.arg1;

   if (scope->command.arg1 < 0) {
      // if it is a back jump
      scope->lh->writeJeqBack(label, *scope->codeWriter);
   }
   else if (scope->command.arg1 > 0) {
      scope->lh->writeJeqForward(label, *scope->codeWriter, scope->command.arg1);
   }
}

void elena_lang::compileJne(JITCompilerScope* scope)
{
   pos_t label = scope->tapeReader->position() + scope->command.arg1;

   if (scope->command.arg1 < 0) {
      // if it is a back jump
      scope->lh->writeJneBack(label, *scope->codeWriter);
   }
   else if (scope->command.arg1 > 0) {
      scope->lh->writeJneForward(label, *scope->codeWriter, scope->command.arg1);
   }
}

void elena_lang::compileDispatchMR(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   void* code = scope->compiler->_inlines[0][scope->code()];

   pos_t position = writer->position();
   pos_t length = *(pos_t*)((char*)code - sizeof(pos_t));

   // simply copy correspondent inline code
   writer->write(code, length);

   bool functionMode = test((unsigned int)scope->command.arg1, FUNCTION_MESSAGE);
   int startArg = (functionMode ? 0 : 1) << scope->constants->indexPower;
   int argCount = (scope->command.arg1 & ARG_MASK) + (functionMode ? 1 : 0);

   // resolve section references
   pos_t count = *(pos_t*)((char*)code + length);
   RelocationEntry* entries = (RelocationEntry*)((char*)code + length + sizeof(pos_t));
   while (count > 0) {
      // locate relocation position
      writer->seek(position + entries->offset);
      switch (entries->reference) {
         case ARG32_1:
            scope->compiler->writeImm32(writer, scope->helper->importMessage(scope->command.arg1));
            break;
         case NARG_1:
            scope->compiler->writeImm32(writer, argCount);
            break;
         case NARG16_1:
            scope->compiler->writeImm16(writer, argCount, 0);
            break;
         case PTR32_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef32);
            break;
         case PTR64_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskRef64);
            break;
         case DISP32HI_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Hi);
            break;
         case DISP32LO_2:
            scope->compiler->writeArgAddress(scope, scope->command.arg2, 0, mskDisp32Lo);
            break;
         case PTR32HI_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Hi);
            break;
         }
         case PTR32LO_2:
         {
            short disp = *(short*)((char*)code + entries->offset);
            scope->compiler->writeArgAddress(scope, scope->command.arg2, disp, mskRef32Lo);
            break;
         }
         case NARG_2:
            scope->compiler->writeImm32(writer, startArg);
            break;
         case NARG16_2:
            scope->compiler->writeImm16(writer, startArg, 0);
            break;
         case NARG12_2:
            scope->compiler->writeImm12(writer, startArg, 0);
            break;
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();

}

// --- JITCompiler ---

JITCompilerScope :: JITCompilerScope(ReferenceHelperBase* helper, JITCompiler* compiler, LabelHelperBase* lh,
   MemoryWriter* writer, MemoryReader* tapeReader, JITConstants* constants)
{
   this->helper = helper;
   this->compiler = compiler;
   this->lh = lh;
   this->codeWriter = writer;
   this->tapeReader = tapeReader;
   this->constants = constants;

   this->stackOffset = constants->unframedOffset;
   this->frameOffset = 0;
   this->withDebugInfo = compiler->isWithDebugInfo();
}

// --- JITCompiler ---

void JITCompiler :: loadCoreRoutines(
   LibraryLoaderBase* loader,
   ImageProviderBase* imageProvider,
   ReferenceHelperBase* helper,
   LabelHelperBase* lh,
   JITSettings settings,
   Map<ref_t, pos_t>& positions, bool declareMode)
{
   // preload core data
   MemoryBase* code = imageProvider->getTextSection();
   MemoryBase* rdata = imageProvider->getRDataSection();
   MemoryBase* data = imageProvider->getDataSection();

   MemoryWriter codeWriter(code);
   MemoryWriter rdataWriter(rdata);
   MemoryWriter dataWriter(data);

   // preload variables
   JITCompilerScope dataScope(helper, this, lh, &dataWriter, nullptr, &_constants);
   loadPreloaded(
      dataScope, loader, coreVariableNumber, coreVariables,
      _preloaded, positions, _constants.inlineMask, declareMode);

   // preload core constants
   JITCompilerScope rdataScope(helper, this, lh, &rdataWriter, nullptr, &_constants);
   loadPreloaded(rdataScope, loader, coreConstantNumber, coreConstants,
      _preloaded, positions, _constants.inlineMask, declareMode);
   // NOTE : SYSTEM_ENV table is tailed with GCMGSize,GCYGSize,GCPERMSize,MaxThread
   rdataWriter.writeDWord(settings.mgSize);
   rdataWriter.writeDWord(settings.ygSize);

   // preload core functions
   JITCompilerScope scope(helper, this, lh, &codeWriter, nullptr, &_constants);
   loadPreloaded(scope, loader, coreFunctionNumber, coreFunctions, _preloaded, positions,
      _constants.inlineMask, declareMode);
}

void JITCompiler :: prepare(
   LibraryLoaderBase* loader,
   ImageProviderBase* imageProvider,
   ReferenceHelperBase* helper,
   LabelHelperBase* lh,
   JITSettings settings)
{
   Map<ref_t, pos_t> positions(0u);
   loadCoreRoutines(loader, imageProvider, helper, lh, settings, positions, true);
   loadCoreRoutines(loader, imageProvider, helper, lh, settings, positions, false);

   // preload vm commands
   for (ref_t i = 0; i < NumberOfInlines; i++) {
      loadInline(i, _inlines, loader);
   }
}

CodeGenerator* JITCompiler :: codeGenerators()
{
   return _codeGenerators;
}

void JITCompiler :: writeArgAddress(JITCompilerScope* scope, arg_t arg, pos_t offset, ref_t addressMask)
{
   scope->helper->writeReference(*scope->codeWriter->Memory(), scope->codeWriter->position(),
      (ref_t)arg, offset, addressMask);
}

void JITCompiler :: writeVMTMethodArg(JITCompilerScope* scope, arg_t arg, pos_t offset, mssg_t message, ref_t addressMask)
{
   scope->helper->writeVMTMethodReference(*scope->codeWriter->Memory(), scope->codeWriter->position(),
      (ref_t)arg, offset, message, addressMask);
}

void JITCompiler :: compileTape(ReferenceHelperBase* helper, MemoryReader& bcReader, pos_t endPos,
   MemoryWriter& codeWriter, LabelHelperBase* lh)
{
   CodeGenerator*   generators = codeGenerators();
   JITCompilerScope scope(helper, this, lh, &codeWriter, &bcReader, &_constants);

   while (bcReader.position() < endPos) {
      ByteCodeUtil::read(bcReader, scope.command);

      generators[(unsigned char)scope.command.code](&scope);
   }
}

void JITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader,
   MemoryWriter& codeWriter, LabelHelperBase* lh)
{
   pos_t codeSize = bcReader.getPos();
   pos_t endPos = bcReader.position() + codeSize;

   compileTape(helper, bcReader, endPos, codeWriter, lh);
}

void JITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader,
   MemoryWriter& codeWriter, LabelHelperBase* lh)
{
   pos_t codeSize = bcReader.getPos();
   pos_t endPos = bcReader.position() + codeSize;

   compileTape(helper, bcReader, endPos, codeWriter, lh);
}

// --- JITCompiler32 ---

inline void insertVMTEntry32(VMTEntry32* entries, pos_t count, pos_t index)
{
   for (pos_t i = count; i > index; i--) {
      entries[i] = entries[i - 1];
   }
}

void JITCompiler32 :: prepare(
   LibraryLoaderBase* loader,
   ImageProviderBase* imageProvider,
   ReferenceHelperBase* helper,
   LabelHelperBase* lh,
   JITSettings settings)
{
   _constants.indexPower = 2;
   _constants.dataOffset = 4;
   _constants.dataHeader = 8;
   _constants.structMask = elStructMask32;
   _constants.vmtSize = elVMTClassOffset32;

   JITCompiler::prepare(loader, imageProvider, helper, lh, settings);
}

pos_t JITCompiler32 :: getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed)
{
   if (emptyNotAllowed && statSection->length() == 0)
      MemoryBase::writeDWord(statSection, 0, 0);

   return statSection->length() >> 2;
}

void JITCompiler32 :: compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length)
{
   writer.writeDWord(length);

   while (length > 0) {
      ref_t memberRef = reader.getRef();

      writer.writeDWord(0);
      helper->writeReference(*writer.Memory(), writer.position() - 4, memberRef, 0, mskRef32);

      --length;
   }
}

void JITCompiler32 :: allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength,
   pos_t staticLength)
{
   // create VMT static table
   vmtWriter.writeBytes(0, staticLength << 2);

   alignCode(vmtWriter, _constants.alignmentVA, false);

   // create VMT header:
   VMTHeader32 header = { 0, flags, 0, vmtLength };

   vmtWriter.write(&header, sizeof(VMTHeader32));

   pos_t position = vmtWriter.position();
   pos_t vmtSize = 0;
   if (test(flags, elStandartVMT))
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTEntry32);

   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

pos_t JITCompiler32 :: getVMTLength(void* targetVMT)
{
   // get the parent vmt size
   VMTHeader32* header = (VMTHeader32*)((uintptr_t)targetVMT - elVMTClassOffset32);

   return header->count;
}

addr_t JITCompiler32 :: findMethodAddress(void* entries, mssg_t message)
{
   // return the method address
   // if the vmt entry was not resolved, SEND_MESSAGE routine should be used (the first method entry)
   VMTHeader32* header = (VMTHeader32*)((uintptr_t)entries - elVMTClassOffset32);

   pos_t address = ((VMTEntry32*)entries)[0].address;
   for (pos_t i = 0; i < header->count; i++) {
      if (((VMTEntry32*)entries)[i].message == message) {
         address = ((VMTEntry32*)entries)[i].address;
         break;
      }
   }

   return (addr_t)address;
}

pos_t JITCompiler32 :: findMethodOffset(void* entries, mssg_t message)
{
   VMTHeader32* header = (VMTHeader32*)((uintptr_t)entries - elVMTClassOffset64);
   pos_t offset = 0;
   for (pos_t i = 0; i < header->count; i++) {
      if (((VMTEntry32*)entries)[i].message == message) {
         offset = i * sizeof(VMTEntry32);
         break;
      }
   }

   return offset + 4;
}

pos_t JITCompiler32 :: copyParentVMT(void* parentVMT, void* targetVMT)
{
   if (parentVMT) {
      // get the parent vmt size
      VMTHeader32* header = (VMTHeader32*)((uintptr_t)parentVMT - elVMTClassOffset32);

      // get the parent entry array
      VMTEntry32* parentEntries = (VMTEntry32*)parentVMT;

      // copy parent VMT
      for (pos_t i = 0; i < header->count; i++) {
         ((VMTEntry32*)targetVMT)[i] = parentEntries[i];
      }

      return header->count;
   }
   else return 0;
}

void JITCompiler32 :: allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length,
   bool structMode, bool virtualMode)
{
   alignCode(writer, _constants.alignmentVA, false);

   if (vmtAddress == INVALID_ADDR) {
      writer.writeDWord(0);
   }
   else if (virtualMode) {
      writer.writeDReference((ref_t)vmtAddress | mskRef32, 0);
   }
   else writer.writeDWord((pos_t)vmtAddress);

   if (structMode) {
      writer.writeDWord(length | elStructMask32);
   }
   else writer.writeDWord(length << 2);
}

void JITCompiler32 :: addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount)
{
   VMTEntry32* entries = (VMTEntry32*)targetVMT;

   pos_t index = 0;

   // find the message entry
   while (index < entryCount && (entries[index].message < message))
      index++;

   if (index < entryCount) {
      if (entries[index].message != message) {
         insertVMTEntry32(entries, entryCount, index);
         entryCount++;
      }
   }
   else entryCount++;

   entries[index].message = message;
   entries[index].address = (pos_t)codeAddress;
}

void JITCompiler32 :: updateVMTHeader(MemoryWriter& vmtWriter, addr_t parentAddress, addr_t classClassAddress,
   ref_t flags, pos_t count, FieldAddressMap& staticValues, bool virtualMode)
{
   pos_t position = vmtWriter.position();

   vmtWriter.seek(position - sizeof(VMTHeader32));
   VMTHeader32 header = { 0 };
   header.flags = flags;
   header.count = count;
   if (!virtualMode) {
      header.parentRef = (pos_t)parentAddress;
      header.classRef = (pos_t)classClassAddress;
   }

   vmtWriter.write(&header, sizeof(VMTHeader32));

   if (virtualMode) {
      MemoryBase* image = vmtWriter.Memory();

      if (parentAddress) {
         vmtWriter.seek(position - sizeof(VMTHeader32) + VMTHeader32ParentRefOffs);
         vmtWriter.writeDReference((ref_t)parentAddress | mskRef32, 0);
      }
      if (classClassAddress) {
         vmtWriter.seek(position - sizeof(VMTHeader32) + VMTHeader32ClassRefOffs);
         vmtWriter.writeDReference((ref_t)classClassAddress | mskRef32, 0);
      }

      pos_t entryPosition = position;
      for (pos_t i = 0; i < count; i++) {
         image->addReference(mskCodeRef32, entryPosition + 4);

         entryPosition += 8;
      }
   }

   // settings static values
   for (auto it = staticValues.start(); !it.eof(); ++it) {
      vmtWriter.seek(position - sizeof(VMTHeader32) + it.key() * 4);
      if (virtualMode) {
         vmtWriter.writeDReference(*it | mskRef32, 0);
      }
      else vmtWriter.writeDWord((pos_t)*it);
   }

   vmtWriter.seek(position);
}

pos_t JITCompiler32 :: addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, ustr_t actionName,
   ref_t weakActionRef, ref_t signature)
{
   pos_t actionRef = messageWriter.position() >> 3;

   // weak action ref for strong one or the same ref
   if (weakActionRef) {
      messageWriter.writeRef(weakActionRef);
   }
   else messageWriter.writeRef(0u);

   // signature or action name for weak message
   if (signature) {
      messageWriter.writeDReference(mskMBDataRef32 | signature, 0u);
   }
   else if (actionName.empty()) {
      messageWriter.writeRef(0u);
   }
   else {
      messageWriter.writeDReference(mskMBDataRef32 | messageBodyWriter.position(), 0u);

      messageBodyWriter.writeString(actionName, actionName.length_pos() + 1);
      messageBodyWriter.align(4, 0);
   }

   return actionRef;
}

pos_t JITCompiler32 :: addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode)
{
   pos_t position = writer.position();

   targetMask = mskRef32;

   if (vmtAddress != INVALID_ADDR) {
      if (virtualMode) {
         writer.writeDReference((ref_t)vmtAddress | targetMask, 0);
      }
      else writer.writeDWord((unsigned int)vmtAddress);
   }
   else writer.writeDWord(0);

   return position;
}

int JITCompiler32 :: calcTotalSize(int numberOfFields)
{
   return align((numberOfFields << 2) + elObjectOffset32, gcPageSize32);
}

int JITCompiler32 :: calcTotalStructSize(int size)
{
   return align(size + elObjectOffset32, gcPageSize32);
}

void JITCompiler32 :: addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode)
{
   if (!virtualMode) {
      MemoryBase* image = codeWriter.Memory();

      addr_t address = (addr_t)image->get(codeWriter.position());

      writer.writeDWord((unsigned int)address);
   }
   else writer.writeDReference(mskCodeRef32, codeWriter.position());
}

void JITCompiler32 :: addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode)
{
   if (!virtualMode) {
      writer.writeDWord((unsigned int)vaddress);
   }
   else writer.writeDReference((ref_t)vaddress | mskRef32, 0);
}

void JITCompiler32 :: writeInt32(MemoryWriter& writer, unsigned value)
{
   writer.writeDWord(value);
}

void JITCompiler32 :: writeLiteral(MemoryWriter& writer, ustr_t value)
{
   writer.writeString(value, value.length_pos() + 1);
   writer.align(4, 0);
}

void JITCompiler32 :: writeChar32(MemoryWriter& writer, ustr_t value)
{
   size_t len = 1;
   unic_c ch = 0;
   StrConvertor::copy(&ch, value, getlength(value), len);

   writer.writeDWord(ch);
   writer.align(4, 0);
}

void JITCompiler32 :: writeMessage(MemoryWriter& writer, mssg_t message)
{
   writer.writeDWord(message);
}

void JITCompiler32 :: writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo)
{
   MemoryBase* section = sectionInfo->section;

   pos_t position = writer.position();
   pos_t length = section->length();

   writer.write(section->get(0), length);
   writer.align(4, 0);

   for (auto it = RelocationMap::Iterator(section->getReferences()); !it.eof(); ++it) {
      pos_t imageOffset = *it + position;

      if (*it == (pos_t)-4) {
         // skip VMT reference
      }
      else helper->writeSectionReference(writer.Memory(), imageOffset, it.key(), sectionInfo, *it, mskRef32);
   }
}

void JITCompiler32 :: writeVariable(MemoryWriter& writer)
{
   writer.writeDWord(0);
}

void JITCompiler32 :: updateEnvironment(MemoryBase* rdata, pos_t staticCounter, bool virtualMode)
{
   void* env = _preloaded.get(SYSTEM_ENV);
   if (virtualMode) {
#if defined _M_X64 || __x86_64__ || __PPC64__ || __aarch64__
      int64_t tmp = (int64_t)env;

      MemoryBase::writeDWord(rdata, (static_cast<ref_t>(tmp) & ~mskAnyRef), staticCounter);
#else
      MemoryBase::writeDWord(rdata, ((ref_t)env & ~mskAnyRef), staticCounter);
#endif
   }
   else {
      *(int64_t*)env = staticCounter;
   }
}

// --- JITCompiler64 ---

inline void insertVMTEntry64(VMTEntry64* entries, pos_t count, pos_t index)
{
   for (pos_t i = count; i > index; i--) {
      entries[i] = entries[i - 1];
   }
}

void JITCompiler64 :: prepare(
   LibraryLoaderBase* loader,
   ImageProviderBase* imageProvider,
   ReferenceHelperBase* helper,
   LabelHelperBase* lh,
   JITSettings settings)
{
   _constants.indexPower = 3;
   _constants.dataOffset = 8;
   _constants.dataHeader = 16;
   _constants.structMask = elStructMask64;
   _constants.vmtSize = elVMTClassOffset64;

   JITCompiler::prepare(loader, imageProvider, helper, lh, settings);
}

void JITCompiler64 :: compileMetaList(ReferenceHelperBase* helper, MemoryReader& reader, MemoryWriter& writer, pos_t length)
{
   writer.writeQWord(length);

   while (length > 0) {
      ref_t memberRef = reader.getRef();

      writer.writeQWord(0);
      helper->writeReference(*writer.Memory(), writer.position() - 8, memberRef, 0, mskRef64);

      --length;
   }
}

void JITCompiler64 :: allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticLength)
{
   // create VMT static table
   vmtWriter.writeBytes(0, staticLength << 3);

   alignCode(vmtWriter, _constants.alignmentVA, false);

   // create VMT header:
   VMTHeader64 header = { 0, flags, 0, vmtLength };

   vmtWriter.write(&header, sizeof(header));

   pos_t position = vmtWriter.position();
   pos_t vmtSize = 0;
   if (test(flags, elStandartVMT))
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTEntry64);

   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

pos_t JITCompiler64 :: getStaticCounter(MemoryBase* statSection, bool emptyNotAllowed)
{
   if (emptyNotAllowed && statSection->length() == 0)
      MemoryBase::writeQWord(statSection, 0, 0);

   return statSection->length() >> 3;
}

pos_t JITCompiler64 :: getVMTLength(void* targetVMT)
{
   VMTHeader64* header = (VMTHeader64*)((uintptr_t)targetVMT - elVMTClassOffset64);

   return (pos_t)header->count;
}

addr_t JITCompiler64 :: findMethodAddress(void* entries, mssg_t message)
{
   // return the method address
   // if the vmt entry was not resolved, SEND_MESSAGE routine should be used (the first method entry)
   VMTHeader64* header = (VMTHeader64*)((uintptr_t)entries - elVMTClassOffset64);

   pos64_t address = ((VMTEntry64*)entries)[0].address;
   for (pos64_t i = 0; i < header->count; i++) {
      if (((VMTEntry64*)entries)[i].message == message) {
         address = ((VMTEntry64*)entries)[i].address;
         break;
      }
   }

   return (addr_t)address;
}

pos_t JITCompiler64 :: findMethodOffset(void* entries, mssg_t message)
{
   VMTHeader64* header = (VMTHeader64*)((uintptr_t)entries - elVMTClassOffset64);
   pos_t offset = 0;
   for (pos64_t i = 0; i < header->count; i++) {
      if (((VMTEntry64*)entries)[i].message == message) {
         offset = (pos_t)i * sizeof(VMTEntry64);
         break;
      }
   }

   return offset + 8;
}

pos_t JITCompiler64 :: copyParentVMT(void* parentVMT, void* targetVMT)
{
   if (parentVMT) {
      // get the parent vmt size
      VMTHeader64* header = (VMTHeader64*)((uintptr_t)parentVMT - elVMTClassOffset64);

      // get the parent entry array
      VMTEntry64* parentEntries = (VMTEntry64*)parentVMT;

      // copy parent VMT
      for (pos_t i = 0; i < header->count; i++) {
         ((VMTEntry64*)targetVMT)[i] = parentEntries[i];
      }

      return (pos_t)header->count;
   }
   else return 0;
}

void JITCompiler64 :: addVMTEntry(mssg_t message, addr_t codeAddress, void* targetVMT, pos_t& entryCount)
{
   VMTEntry64* entries = (VMTEntry64*)targetVMT;

   pos_t index = 0;

   // find the message entry
   while (index < entryCount && (entries[index].message < message))
      index++;

   if (index < entryCount) {
      if (entries[index].message != message) {
         insertVMTEntry64(entries, entryCount, index);
         entryCount++;
      }
   }
   else entryCount++;

   entries[index].message = message;
   entries[index].address = codeAddress;
}

void JITCompiler64 :: updateVMTHeader(MemoryWriter& vmtWriter, addr_t parentAddress, addr_t classClassAddress,
   ref_t flags, pos_t count, FieldAddressMap& staticValues, bool virtualMode)
{
   pos_t position = vmtWriter.position();

   vmtWriter.seek(position - sizeof(VMTHeader64));
   VMTHeader64 header = { 0 };
   header.flags = flags;
   header.count = count;
   if (!virtualMode) {
      header.parentRef = parentAddress;
      header.classRef = classClassAddress;
   }

   vmtWriter.write(&header, sizeof(VMTHeader64));

   if (virtualMode) {
      MemoryBase* image = vmtWriter.Memory();

      if (parentAddress) {
         vmtWriter.seek(position - sizeof(VMTHeader64) + VMTHeader64ParentRefOffs);
         vmtWriter.writeQReference((ref_t)parentAddress | mskRef64, 0);
      }
      if (classClassAddress) {
         vmtWriter.seek(position - sizeof(VMTHeader64) + VMTHeader64ClassRefOffs);
         vmtWriter.writeQReference((ref_t)classClassAddress | mskRef64, 0);
      }
      pos_t entryPosition = position;
      for (pos_t i = 0; i < count; i++) {
         image->addReference(mskCodeRef64, entryPosition + 8);

         entryPosition += 16;
      }
   }

   // settings static values
   for (auto it = staticValues.start(); !it.eof(); ++it) {
      vmtWriter.seek(position - sizeof(VMTHeader64) + it.key() * 8);
      if (virtualMode) {
         vmtWriter.writeQReference(*it | mskRef64, 0);
      }
      else vmtWriter.writeQWord(*it);
   }

   vmtWriter.seek(position);
}

pos_t JITCompiler64 :: addActionEntry(MemoryWriter& messageWriter, MemoryWriter& messageBodyWriter, ustr_t actionName,
   ref_t weakActionRef, ref_t signature)
{
   pos_t actionRef = messageWriter.position() >> 4;

   // weak action ref for strong one or the same ref
   if (weakActionRef) {
      messageWriter.writeRef64(weakActionRef);
   }
   else messageWriter.writeRef64(0u);

   // signature or action name for weak message
   if (signature) {
      messageWriter.writeQReference(mskMBDataRef64 | signature, 0u);
   }
   else if (actionName.empty()) {
      messageWriter.writeRef64(0u);
   }
   else {
      messageWriter.writeQReference(mskMBDataRef64 | messageBodyWriter.position(), 0u);

      messageBodyWriter.writeString(actionName, actionName.length_pos() + 1);
      messageBodyWriter.align(8, 0);
   }

   return actionRef;
}

pos_t JITCompiler64 :: addSignatureEntry(MemoryWriter& writer, addr_t vmtAddress, ref_t& targetMask, bool virtualMode)
{
   pos_t position = writer.position();

   targetMask = mskRef64;

   if (vmtAddress != INVALID_ADDR) {
      if (virtualMode) {
         writer.writeQReference((ref_t)vmtAddress | mskRef64, 0);
      }
      else writer.writeQWord(vmtAddress);
   }
   else writer.writeQWord(0);

   return position;
}

void JITCompiler64 :: allocateHeader(MemoryWriter& writer, addr_t vmtAddress, int length,
   bool structMode, bool virtualMode)
{
   alignCode(writer, _constants.alignmentVA, false);

   if (vmtAddress == INVALID_ADDR) {
      writer.writeQWord(0);
   }
   else if (virtualMode) {
      writer.writeQReference((ref_t)vmtAddress | mskRef64, 0);
   }
   else writer.writeQWord(vmtAddress);

   writer.writeDWord(0);
   if (structMode) {
      writer.writeDWord(length | elStructMask64);
   }
   else writer.writeDWord(length << 3);
}

void JITCompiler64 :: addBreakpoint(MemoryWriter& writer, MemoryWriter& codeWriter, bool virtualMode)
{
   if (!virtualMode) {
      MemoryBase* image = codeWriter.Memory();

      writer.writeQWord((unsigned long long)image->get(codeWriter.position()));
   }
   else writer.writeQReference(mskCodeRef64, codeWriter.position());
}

void JITCompiler64 :: addBreakpoint(MemoryWriter& writer, addr_t vaddress, bool virtualMode)
{
   if (!virtualMode) {
      writer.writeQWord(vaddress);
   }
   else writer.writeQReference((ref_t)vaddress | mskRef64, 0);
}

int JITCompiler64 :: calcTotalSize(int numberOfFields)
{
   return align((numberOfFields << 3) + elObjectOffset64, gcPageSize64);
}

int JITCompiler64 :: calcTotalStructSize(int size)
{
   return align(size + elObjectOffset64, gcPageSize64);
}

void JITCompiler64 :: writeInt32(MemoryWriter& writer, unsigned value)
{
   writer.writeDWord(value);
   writer.align(8, 0);
}

void JITCompiler64 :: writeLiteral(MemoryWriter& writer, ustr_t value)
{
   writer.writeString(value, value.length_pos() + 1);
   writer.align(8, 0);
}

void JITCompiler64 :: writeChar32(MemoryWriter& writer, ustr_t value)
{
   size_t len = 1;
   unic_c ch = 0;
   StrConvertor::copy(&ch, value, getlength(value), len);

   writer.writeDWord(ch);
   writer.align(8, 0);
}

void JITCompiler64 :: writeMessage(MemoryWriter& writer, mssg_t value)
{
   writer.writeQWord(value);
}

void JITCompiler64 :: writeCollection(ReferenceHelperBase* helper, MemoryWriter& writer, SectionInfo* sectionInfo)
{
   MemoryBase* section = sectionInfo->section;

   pos_t position = writer.position();
   pos_t length = section->length();

   // object body
   pos_t index = 0;
   while (index < length) {
      writer.writeQWord(MemoryBase::getDWord(section, index));
      index += 4;
   }
   writer.align(8, 0);

   for (auto it = RelocationMap::Iterator(section->getReferences()); !it.eof(); ++it) {
      pos_t imageOffset = ((*it) << 1) + position;

      if (*it == (pos_t)-4) {
         // skip VMT reference
      }
      else helper->writeSectionReference(writer.Memory(), imageOffset, it.key(),
         sectionInfo, *it, mskRef64);
   }
}

void JITCompiler64 :: writeVariable(MemoryWriter& writer)
{
   writer.writeQWord(0);
}

void JITCompiler64 :: updateEnvironment(MemoryBase* rdata, pos_t staticCounter, bool virtualMode)
{
   void* env = _preloaded.get(SYSTEM_ENV);
   if (virtualMode) {
      MemoryBase::writeQWord(rdata, (int64_t)env & ~mskAnyRef, staticCounter);
   }
   else {
      *(int64_t*)env = staticCounter;
   }
}
