//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86-64
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86_64COMPILER_H
#define X86_64COMPILER_H

#include "jitcompiler.h"

namespace elena_lang
{
   // --- X86_64JITCompiler --
   class X86_64JITCompiler : public JITCompiler64
   {
   protected:
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings,
         bool virtualMode) override;

      friend void x86_64loadCallOp(JITCompilerScope* scope);
      friend void x86_64compileStackOp(JITCompilerScope* scope);
      friend void x86_64compileOpenIN(JITCompilerScope* scope);

   public:
      void writeImm9(MemoryWriter* writer, int value, int type) override;
      void writeImm12(MemoryWriter* writer, int value, int type) override;

      void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) override;

      // NOTE that LabelHelperBase argument should be overridden inside the CPU compiler
      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase*) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase*) override;

      X86_64JITCompiler()
         : JITCompiler64()
      {
         _constants.dataOffset = 0x0C;
         _constants.unframedOffset = 1;
      }
   };

   void x86_64loadCallOp(JITCompilerScope* scope);
   void x86_64compileStackOp(JITCompilerScope* scope);
   void x86_64compileOpenIN(JITCompilerScope* scope);
}

#endif