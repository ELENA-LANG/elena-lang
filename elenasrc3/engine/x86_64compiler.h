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

// ; the distance from the frame pointer built by extopen back to the first argument, and
// ; it is a property of the target ABI, not of the host. On Windows the four register
// ; arguments are spilled into the home area above the return address, on the System V
// ; ABI extopen pushes rcx and rdx below it, which puts the first argument 32 bytes lower
constexpr int EXT_OFFSET_MS = 104;
constexpr int EXT_OFFSET_SYSV = 72;

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
         return (extMode ? _extOffset : 8) + (argument > 0 ? align(argument + 16, 16) : 0);
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

      X86_64JITCompiler(bool msABI = true)
         : JITCompiler64()
      {
         _extOffset = msABI ? EXT_OFFSET_MS : EXT_OFFSET_SYSV;

         _constants.dataOffset = 0x0C;
         _constants.unframedOffset = 1;
      }

   private:
      int _extOffset;
   };

   void x86_64loadCallOp(JITCompilerScope* scope);
   void x86_64compileStackOp(JITCompilerScope* scope);
   void x86_64compileOpenIN(JITCompilerScope* scope);
   void x86_64compileExtOpenIN(JITCompilerScope* scope);
   void x86_64compileXOpenIN(JITCompilerScope* scope);
}

#endif