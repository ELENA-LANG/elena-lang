//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: x86-64
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86_64COMPILER_H
#define X86_64COMPILER_H

#include "jitcompiler.h"

#if defined(__x86_64__)

constexpr int EXT_OFFSET = 64;

#else

constexpr int EXT_OFFSET = 104;

#endif

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
         ProcessSettings& settings,
         bool virtualMode) override;

      friend void x86_64loadCallOp(JITCompilerScope* scope);
      friend void x86_64compileStackOp(JITCompilerScope* scope);
      friend void x86_64compileOpenIN(JITCompilerScope* scope);
      friend void x86_64compileExtOpenIN(JITCompilerScope* scope);
      friend void x86_64compileXOpenIN(JITCompilerScope* scope);

   public:
      static PlatformSettings getSettings()
      {
         return { 2, 2, 16, 32, 8, 8 };
      }

      int calcFrameOffset(int argument, bool extMode) override
      {
         // NOTE : for the external frame we have to store all nonvolatile registers (rsi, rdi, rbx, r12, r13, r14, r15)
         return (extMode ? EXT_OFFSET : 8) + (argument > 0 ? align(argument + 16, 16) : 0);
      }

      void writeImm9(MemoryWriter* writer, int value, int type) override;
      void writeImm12(MemoryWriter* writer, int value, int type) override;

      void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) override;
      void alignJumpAddress(MemoryWriter&) override
      {
         // must be implemented
      }

      // NOTE that LabelHelperBase argument should be overridden inside the CPU compiler
      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase*) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase*) override;

      X86_64JITCompiler(bool withStackShadow)
         : JITCompiler64()
      {
         _constants.dataOffset = 0x0C;
         _constants.unframedOffset = 1;
         _constants.stackShadowMode = withStackShadow;
      }
   };

   void x86_64loadCallOp(JITCompilerScope* scope);
   void x86_64compileStackOp(JITCompilerScope* scope);
   void x86_64compileOpenIN(JITCompilerScope* scope);
   void x86_64compileExtOpenIN(JITCompilerScope* scope);
   void x86_64compileXOpenIN(JITCompilerScope* scope);
}

#endif