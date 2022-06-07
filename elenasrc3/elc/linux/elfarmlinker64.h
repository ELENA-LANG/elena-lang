//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux ARM64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFARMLINKER64_H
#define ELFARMLINKER64_H

#include "elflinker64.h"

namespace elena_lang
{
   // --- ElfARM64Linker ---
   class ElfARM64Linker : public Elf64Linker
   {
   protected:
      int getMachine() override;

      ustr_t getInterpreter() override;

      void prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize) override;

   public:
      ElfARM64Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : Elf64Linker(errorProcessor, imageFormatter)
      {

      }
   };
}

#endif
