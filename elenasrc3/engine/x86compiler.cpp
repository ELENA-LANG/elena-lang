//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                                 (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "x86compiler.h"
#include "langcommon.h"

using namespace elena_lang;

void X86JITCompiler :: prepare(
   LibraryLoaderBase* loader, 
   ImageProviderBase* imageProvider, 
   ReferenceHelperBase* helper,
   JITSettings settings)
{
   _constants.inlineMask = mskCodeRef32;

   JITCompiler32::prepare(loader, imageProvider, helper, settings);
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

void X86JITCompiler :: compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter)
{
   JITCompiler::compileProcedure(helper, bcReader, codeWriter);

   alignCode(codeWriter, 0x04, true);
}

void X86JITCompiler :: compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter)
{
   JITCompiler32::compileSymbol(helper, bcReader, codeWriter);

   alignCode(codeWriter, 0x04, true);
}

