//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: ARM64
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ARM64COMPILER_H
#define ARM64COMPILER_H

#include "jitcompiler.h"

namespace elena_lang
{
   // --- ARM64JITCompiler --
   class ARM64JITCompiler : public JITCompiler64
   {
   protected:
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         JITSettings settings) override;

      friend void ARM64loadCallOp(JITCompilerScope* scope);
      friend void ARM64compileOpenIN(JITCompilerScope* scope);

   public:
      void writeImm9(MemoryWriter* writer, int value, int type) override;
      void writeImm12(MemoryWriter* writer, int value, int type) override;
      void writeImm16(MemoryWriter* writer, int value, int type) override;

      void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) override;

      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) override;

      ARM64JITCompiler()
         : JITCompiler64()
      {         
      }
   };

   void ARM64loadCallOp(JITCompilerScope* scope);
   void ARM64compileOpenIN(JITCompilerScope* scope);
}

#endif