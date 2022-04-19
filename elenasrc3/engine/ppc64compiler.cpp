//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: PPC64le
//                                              (C)2021-2022 by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "ppc64compiler.h"

using namespace elena_lang;

constexpr auto OverloadsCount = 3;
const Pair<ByteCode, CodeGenerator, ByteCode::None> Overloads[OverloadsCount] =
{
   { ByteCode::CallExtR, PPC64loadCallOp},
   { ByteCode::OpenIN, PPC64compileOpenIN},
   { ByteCode::OpenHeaderIN, PPC64compileOpenIN},
};

//inline void x86_64AllocStack(int args, MemoryWriter* code)
//{
//   // sub esp, arg
//   if (args < 0x80) {
//      code->writeByte(0x48);
//      code->writeWord(0xEC83);
//      code->writeByte(args << 3);
//   }
//   else {
//      code->writeByte(0x48);
//      code->writeWord(0xEC81);
//      code->writeDWord(args << 3);
//   }
//}
//
//inline void x86_64FreeStack(int args, MemoryWriter* code)
//{
//   // add rsp, arg
//   if (args < 0x80) {
//      code->writeByte(0x48);
//      code->writeWord(0xC483);
//      code->writeByte(args << 3);
//   }
//   else {
//      code->writeByte(0x48);
//      code->writeWord(0xC481);
//      code->writeDWord(args << 3);
//   }
//}

void elena_lang::PPC64loadCallOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

//   int argsToFree = 0;
//   switch (scope->command.arg2) {
//      case 0:
//         argsToFree = 4;
//         x86_64AllocStack(argsToFree, writer);
//         break;
//      case 1:
//         argsToFree = 3;
//         x86_64AllocStack(argsToFree, writer);
//         break;
//      case 2:
//         argsToFree = 2;
//         x86_64AllocStack(argsToFree, writer);
//         break;
//      case 3:
//         argsToFree = 1;
//         x86_64AllocStack(argsToFree, writer);
//         break;
//   }

   void* code = nullptr;
   switch (scope->command.arg2) {
      case 0:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[1][scope->code()];
         break;
      case 1:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[2][scope->code()];
         break;
      case 2:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[3][scope->code()];
         break;
      case 3:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[4][scope->code()];
         break;
      case 4:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[5][scope->code()];
         break;
      default:
         code = ((PPC64leJITCompiler*)scope->compiler)->_inlines[0][scope->code()];
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
         case DISP32HI_1:
            ((PPC64leJITCompiler*)scope->compiler)->writeArgAddress(scope, scope->command.arg1, 0, mskDisp32Hi);
            break;
         case DISP32LO_1:
            ((PPC64leJITCompiler*)scope->compiler)->writeArgAddress(scope, scope->command.arg1, 0, mskDisp32Lo);
            break;
         default:
            writeCoreReference(scope, entries->reference, entries->offset, code);
            break;
      }

      entries++;
      count--;
   }
   writer->seekEOF();

//   if (argsToFree > 0) {
//      x86_64FreeStack(argsToFree, writer);
//   }
}

void elena_lang::PPC64compileOpenIN(JITCompilerScope* scope)
{
   // NOTE : stack should be aligned to 16 bytes
   scope->command.arg1 = align(scope->command.arg1, 2);
   scope->command.arg2 = align(scope->command.arg2, 16);

   elena_lang::compileOpen(scope);
}

// --- PPC64leJITCompiler ---

void PPC64leJITCompiler :: writeImm9(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void PPC64leJITCompiler :: writeImm12(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void PPC64leJITCompiler:: prepare(
   LibraryLoaderBase* loader, 
   ImageProviderBase* imageProvider, 
   ReferenceHelperBase* helper,
   LabelHelperBase*,
   JITSettings settings)
{
   //_inlineMask = mskCodeRelRef32;

   // override code generators
   auto commands = codeGenerators();

   for (size_t i = 0; i < OverloadsCount; i++)
      commands[(int)Overloads[i].value1] = Overloads[i].value2;

   PPCLabelHelper labelHelper;
   JITCompiler64::prepare(loader, imageProvider, helper, &labelHelper, settings);
}

void PPC64leJITCompiler:: alignCode(MemoryWriter& writer, pos_t alignment, bool isText)
{
   if (isText) {
      // should be aligned to 4 byte border
      pos_t aligned = elena_lang::align(writer.position(), alignment & ~3);
      while (writer.position() < aligned)
         writer.writeDWord(0x60000000);
   }
   else writer.align(alignment, 0x00);
}

void PPC64leJITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter, 
   LabelHelperBase*)
{
   PPCLabelHelper labelHelper;

   JITCompiler::compileProcedure(helper, bcReader, codeWriter, &labelHelper);

   alignCode(codeWriter, 0x08, true);
}

void PPC64leJITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   PPCLabelHelper labelHelper;

   JITCompiler64::compileSymbol(helper, bcReader, codeWriter, &labelHelper);

   alignCode(codeWriter, 0x08, true);
}
