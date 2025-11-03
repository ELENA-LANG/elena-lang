//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux ARM64
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOARMLINKER64_H
#define MACHOARMLINKER64_H

#include "macholinker64.h"

namespace elena_lang
{
   // --- MachOARM64Linker ---
   class MachOARM64Linker : public MachOLinker64
   {
   protected:
      CPUType getCPUType() override
      {
         return CPUType::AARCH64;
      }

      CPUSubType getCPUSubType() override
      {
         return CPUSubType::ARM_ALL;
      }

   public:
      MachOARM64Linker(ErrorProcessorBase* errorProcessor)
         : MachOLinker64(errorProcessor)
      {

      }
   };
}

#endif
