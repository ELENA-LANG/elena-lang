//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: ARM64
//                                              (C)2021-2022 by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "arm64compiler.h"
#include "armhelper.h"

using namespace elena_lang;

constexpr auto OverloadsCount = 3;
const Pair<ByteCode, CodeGenerator, ByteCode::None, nullptr> Overloads[OverloadsCount] =
{
   { ByteCode::CallExtR, ARM64loadCallOp},
   { ByteCode::OpenIN, ARM64compileOpenIN},
   { ByteCode::ExtOpenIN, ARM64compileOpenIN},
};

void elena_lang::ARM64loadCallOp(JITCompilerScope* scope)
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
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[1][scope->code()];
         break;
      case 1:
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[2][scope->code()];
         break;
      case 2:
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[3][scope->code()];
         break;
      case 3:
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[4][scope->code()];
         break;
      case 4:
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[5][scope->code()];
         break;
      default:
         code = ((ARM64JITCompiler*)scope->compiler)->_inlines[0][scope->code()];
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
         case PTR32HI_1:
         {
            ((ARM64JITCompiler*)scope->compiler)->writeArgAddress(scope, scope->command.arg1, 0, mskRef32Hi);
            break;
         }
         case PTR32LO_1:
         {
            ((ARM64JITCompiler*)scope->compiler)->writeArgAddress(scope, scope->command.arg1, 0, mskRef32Lo);
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

//   if (argsToFree > 0) {
//      x86_64FreeStack(argsToFree, writer);
//   }
}

void elena_lang::ARM64compileOpenIN(JITCompilerScope* scope)
{
   // NOTE : stack should be aligned to 16 bytes
   scope->command.arg1 = align(scope->command.arg1, 2);
   scope->command.arg2 = align(scope->command.arg2, 16);

   elena_lang::compileOpen(scope);
}

// --- ARM64JITCompiler ---

void ARM64JITCompiler :: writeImm9(MemoryWriter* writer, int imm, int type)
{
   switch (type) {
      case 0:
         writer->maskDWord((imm & 0x1FF) << 12);
         break;
      default:
         break;
   }
}

void ARM64JITCompiler :: writeImm12(MemoryWriter* writer, int imm, int type)
{
   switch (type) {
      case 0:
         writer->maskDWord((imm & 0xFFF) << 10);
         break;
      default:
         break;
   }
}

void ARM64JITCompiler :: writeImm16(MemoryWriter* writer, int imm, int type)
{
   switch (type) {
      case INV_ARG:
         writer->maskDWord((-imm & 0xFFFF) << 5);
         break;
      default:
         writer->maskDWord((imm & 0xFFFF) << 5);
         break;
   }
}

void ARM64JITCompiler :: prepare(
   LibraryLoaderBase* loader, 
   ImageProviderBase* imageProvider, 
   ReferenceHelperBase* helper,
   LabelHelperBase*,
   JITSettings settings,
   bool virtualMode)
{
   //_inlineMask = mskCodeRelRef32;

   // override code generators
   auto commands = codeGenerators();

   for (size_t i = 0; i < OverloadsCount; i++)
      commands[(int)Overloads[i].value1] = Overloads[i].value2;

   ARMLabelHelper labelHelper;
   JITCompiler64::prepare(loader, imageProvider, helper, &labelHelper, settings, virtualMode);
}

void ARM64JITCompiler :: alignCode(MemoryWriter& writer, pos_t alignment, bool isText)
{
   if (isText) {
      // should be aligned to 4 byte border
      pos_t aligned = elena_lang::align(writer.position(), alignment & ~3);
      while (writer.position() < aligned)
         writer.writeDWord(0xd503201F);

      //0x1f, 0x20, 0x03, 0xd5,	/* nop */
   }
   else writer.align(alignment, 0x00);
}

void ARM64JITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   ARMLabelHelper labelHelper;
   JITCompiler::compileProcedure(helper, bcReader, codeWriter, &labelHelper);

   alignCode(codeWriter, 0x08, true);
}

void ARM64JITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   ARMLabelHelper labelHelper;
   JITCompiler64::compileSymbol(helper, bcReader, codeWriter, &labelHelper);

   alignCode(codeWriter, 0x08, true);
}

void ARM64JITCompiler :: resolveLabelAddress(MemoryWriter* writer, ref_t mask, pos_t position, bool virtualMode)
{
   pos_t offset = writer->position();
   switch (mask) {
      case mskRef32Hi:
      {
         offset >>= 16;

         MemoryBase::maskDWord(writer->Memory(), position, (offset & 0xFFFF) << 5);
         writer->Memory()->addReference(mskCodeRef32Hi, position);

         break;
      }
      case mskRef32Lo:
      {
         offset &= 0xFFFF;

         MemoryBase::maskDWord(writer->Memory(), position, (offset & 0xFFFF) << 5);
         writer->Memory()->addReference(mskCodeRef32Lo, position);

         break;
      }
      default:
         JITCompiler::resolveLabelAddress(writer, mask, position, virtualMode);
         break;
   }
}
