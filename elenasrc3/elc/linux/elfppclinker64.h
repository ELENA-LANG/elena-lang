//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux PPC64le
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFPPCLINKER64_H
#define ELFPPCLINKER64_H

#include "elflinker64.h"

namespace elena_lang
{
   // --- ElfPPC64leLinker ---
   class ElfPPC64leLinker : public Elf64Linker
   {
   protected:
      int getMachine() override;

      ustr_t getInterpreter() override;

      void prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize) override;

   public:
      ElfPPC64leLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : Elf64Linker(errorProcessor, imageFormatter)
      {

      }
   };
}

#endif
