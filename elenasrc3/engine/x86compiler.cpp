//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                                 (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86compiler.h"
#include "x86helper.h"
#include "langcommon.h"

using namespace elena_lang;

void X86JITCompiler :: prepare(
   LibraryLoaderBase* loader, 
   ImageProviderBase* imageProvider, 
   ReferenceHelperBase* helper,
   LabelHelperBase*,
   ProcessSettings& settings,
   bool virtualMode)
{
   X86LabelHelper lh;

   _constants.inlineMask = mskCodeRef32;

   JITCompiler32::prepare(loader, imageProvider, helper, &lh, settings, virtualMode);
}

void X86JITCompiler :: writeImm9(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void X86JITCompiler :: writeImm12(MemoryWriter* writer, int value, int type)
{
   throw InternalError(errNotImplemented);
}

void X86JITCompiler :: alignCode(MemoryWriter& writer, pos_t alignment, bool isText)
{
   writer.align(alignment, isText ? 0x90 : 0x00);
}

void X86JITCompiler :: alignJumpAddress(MemoryWriter& writer)
{
   int bytesToFill = 0x10 - (writer.position() & 0xF);
   switch (bytesToFill) {
      case 1:
         writer.writeByte(0x90);
         break;
      case 2:
         writer.writeWord(0x9066);
         break;
      case 3:
         writer.writeByte(0x0F);
         writer.writeWord(0x1F);
         break;
      case 4:
         writer.writeByte(0x0F);
         writer.writeWord(0x401F);
         writer.writeByte(0);
         break;
      case 5:
         writer.writeByte(0x0F);
         writer.writeWord(0x441F);
         writer.writeWord(0);
         break;
      case 6:
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x441F);
         writer.writeWord(0);
         break;
      case 7:
         writer.writeByte(0x0F);
         writer.writeWord(0x801F);
         writer.writeDWord(0);
         break;
      case 8:
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 9:
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 10:
         writer.writeByte(0x90);

         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 11:
         writer.writeWord(0x9066);

         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 12:
         writer.writeByte(0x0F);
         writer.writeWord(0x1F);

         writer.writeByte(0x0F);
         writer.writeWord(0x1F);
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 13:
         writer.writeByte(0x0F);
         writer.writeWord(0x401F);
         writer.writeByte(0);

         writer.writeByte(0x0F);
         writer.writeWord(0x1F);
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 14:
         writer.writeByte(0x0F);
         writer.writeWord(0x441F);
         writer.writeWord(0);

         writer.writeByte(0x0F);
         writer.writeWord(0x1F);
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      case 15:
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x441F);
         writer.writeWord(0);

         writer.writeByte(0x0F);
         writer.writeWord(0x1F);
         writer.writeByte(0x66);
         writer.writeByte(0x0F);
         writer.writeWord(0x841F);
         writer.writeDWord(0);
         writer.writeByte(0);
         break;
      default:
         break;
   }
}

void X86JITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   X86LabelHelper lh;

   JITCompiler::compileProcedure(helper, bcReader, codeWriter, &lh);

   alignCode(codeWriter, 0x04, true);
}

void X86JITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
   MemoryWriter& codeWriter, LabelHelperBase*)
{
   X86LabelHelper lh;

   JITCompiler32::compileSymbol(helper, bcReader, codeWriter, &lh);

   alignCode(codeWriter, 0x04, true);
}

