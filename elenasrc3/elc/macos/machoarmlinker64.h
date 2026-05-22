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
      void prepareMachOImage(ForwardResolverBase* resolver, ImageProviderBase& provider, MachOExecutableImage& image) override
      {
         image.sectionAlignment = ARM64_SECTION_ALIGNMENT;

         MachOLinker64::prepareMachOImage(resolver, provider, image);
      }

      cpu_type_t getCPUType() override
      {
         return CPU_TYPE_ARM64;
      }

      cpu_subtype_t getCPUSubType() override
      {
         return CPU_SUBTYPE_ARM64_ALL;
      }

   public:
      MachOARM64Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : MachOLinker64(errorProcessor, imageFormatter)
      {

      }
   };
}

#endif
