//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT-X linker class.
//		Supported platforms: PPC64le
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PPC64COMPILER_H
#define PPC64COMPILER_H

#include "jitcompiler.h"
#include "ppchelper.h"

namespace elena_lang
{
   // --- PPC64leJITCompiler --
   class PPC64leJITCompiler : public JITCompiler64
   {
   protected:
      void prepare(
         LibraryLoaderBase* loader, 
         ImageProviderBase* imageProvider, 
         ReferenceHelperBase* helper,
         LabelHelperBase* lh,
         ProcessSettings& settings,
         bool virtualMode) override;

      friend void PPC64loadCallOp(JITCompilerScope* scope);
      friend void PPC64compileOpenIN(JITCompilerScope* scope);
      friend void PPC64compileExtOpenIN(JITCompilerScope* scope);
      //friend void x86_64compileFreeI(JITCompilerScope* scope);

   public:
      static JITCompilerSettings getSettings()
      {
         return { 2, 2, 16, 32 };
      }

      void writeImm9(MemoryWriter* writer, int value, int type) override;
      void writeImm12(MemoryWriter* writer, int value, int type) override;
      void writeImm16Hi(MemoryWriter* writer, int value, int type) override
      {
         writeImm16(writer, PPCHelper::getHiAdjusted(value), type);
      }

      void resolveLabelAddress(MemoryWriter* writer, ref_t mask, pos_t position, bool virtualMode) override;

      void alignCode(MemoryWriter& writer, pos_t alignment, bool isText) override;
      void alignJumpAddress(MemoryWriter& writer) override
      {
         // must be implemented
      }

      void compileProcedure(ReferenceHelperBase* helper, MemoryReader& bcReader, 
         MemoryWriter& codeWriter, LabelHelperBase* lh) override;
      void compileSymbol(ReferenceHelperBase* helper, MemoryReader& bcReader, MemoryWriter& codeWriter, 
         LabelHelperBase* lh) override;

      PPC64leJITCompiler()
         : JITCompiler64()
      {
         _constants.mediumForm = _constants.extendedForm = 0x7FFF;
      }
   };

   void PPC64loadCallOp(JITCompilerScope* scope);
   void PPC64compileOpenIN(JITCompilerScope* scope);
   void PPC64compileExtOpenIN(JITCompilerScope* scope);
   //void x86_64compileFreeI(JITCompilerScope* scope);
}

#endif