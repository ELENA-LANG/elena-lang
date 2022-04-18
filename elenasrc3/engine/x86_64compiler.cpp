//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86-64
//                                              (C)2021-2022 by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86_64compiler.h"
#include "x86helper.h"
#include "langcommon.h"

using namespace elena_lang;

constexpr auto OverloadsCount = 5;
const Pair<ByteCode, CodeGenerator, ByteCode::None, nullptr> Overloads[OverloadsCount] =
{
   { ByteCode::CallExtR, x86_64loadCallOp},
   { ByteCode::FreeI, x86_64compileStackOp},
   { ByteCode::AllocI, x86_64compileStackOp},
   { ByteCode::OpenIN, x86_64compileOpenIN},
   { ByteCode::OpenHeaderIN, x86_64compileOpenIN},
};

inline void x86_64AllocStack(int args, MemoryWriter* code)
{
   // sub esp, arg
   if (args < 0x80) {
      code->writeByte(0x48);
      code->writeWord(0xEC83);
      code->writeByte(args << 3);
   }
   else {
      code->writeByte(0x48);
      code->writeWord(0xEC81);
      code->writeDWord(args << 3);
   }
}

inline void x86_64FreeStack(int args, MemoryWriter* code)
{
   // add rsp, arg
   if (args < 0x80) {
      code->writeByte(0x48);
      code->writeWord(0xC483);
      code->writeByte(args << 3);
   }
   else {
      code->writeByte(0x48);
      code->writeWord(0xC481);
      code->writeDWord(args << 3);
   }
}

void elena_lang::x86_64loadCallOp(JITCompilerScope* scope)
{
   MemoryWriter* writer = scope->codeWriter;

   int argsToFree = 0;
   switch (scope->command.arg2) {
      case 0:
         argsToFree = 4;
         x86_64AllocStack(argsToFree, writer);
         break;
      case 1:
         argsToFree = 3;
         x86_64AllocStack(argsToFree, writer);
         break;
      case 2:
         argsToFree = 2;
         x86_64AllocStack(argsToFree, writer);
         break;
      case 3:
         argsToFree = 1;
         x86_64AllocStack(argsToFree, writer);
         break;
      default:
         // to make compiler happy
         break;
   }

   void* code = ((X86_64JITCompiler*)scope->compiler)->_inlines[0][scope->code()];
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
      if (entries->reference == RELPTR32_1) {
         ((X86_64JITCompiler*)scope->compiler)->writeArgAddress(scope, scope->command.arg1, 0, mskRelRef32);
      }
      //else writeCoreReference();

      entries++;
      count--;
   }
   writer->seekEOF();

   if (argsToFree > 0) {
      x86_64FreeStack(argsToFree, writer);
   }
}

void elena_lang::x86_64compileStackOp(JITCompilerScope* scope)
{
   // NOTE : stack should be aligned to 16 bytes
   scope->command.arg1 = align(scope->command.arg1, 2);

   elena_lang::loadIndexOp(scope);
}

void elena_lang::x86_64compileOpenIN(JITCompilerScope* scope)
{
   // NOTE : stack should be aligned to 16 bytes
   scope->command.arg1 = align(scope->command.arg1, 2);
   scope->command.arg2 = align(scope->command.arg2, 16);

   elena_lang::compileOpen(scope);
}

// --- X86_64JITCompiler ---

void X86_64JITCompiler :: prepare(
   LibraryLoaderBase* loader, 
   ImageProviderBase* imageProvider, 
   ReferenceHelperBase* helper,
   LabelHelperBase*,
   JITSettings _settings)
{
   _constants.inlineMask = mskCodeRelRef32;

   // override code generators
   auto commands = codeGenerators();

   for (size_t i = 0; i < OverloadsCount; i++)
      commands[(int)Overloads[i].value1] = Overloads[i].value2;

   X86LabelHelper lh;
   JITCompiler64::prepare(loader, imageProvider, helper, &lh, _settings);
}

void X86_64JITCompiler :: writeImm9(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void X86_64JITCompiler :: writeImm12(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void X86_64JITCompiler :: alignCode(MemoryWriter& writer, pos_t alignment, bool isText)
{
   writer.align(alignment, isText ? 0x90 : 0x00);
}

void X86_64JITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   X86LabelHelper lh;

   JITCompiler::compileProcedure(helper, bcReader, codeWriter, &lh);

   alignCode(codeWriter, 0x08, true);
}

void X86_64JITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   X86LabelHelper lh;

   JITCompiler64::compileSymbol(helper, bcReader, codeWriter, &lh);

   alignCode(codeWriter, 0x08, true);
}
