//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86COMPILER_H
#define X86COMPILER_H

#include "jitcompiler.h"

namespace elena_lang
{
   // --- X86JITCompiler ---
   class X86JITCompiler : public JITCompiler32
   {
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         JITSettings settings) override;

   public:
      void writeImm9(MemoryWriter* writer, int value, int type) override;
      void writeImm12(MemoryWriter* writer, int value, int type) override;

      void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) override;

      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter) override;

      X86JITCompiler()
         : JITCompiler32()
      {
         
      }
   };
}

#endif