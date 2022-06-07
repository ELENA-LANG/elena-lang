//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux 32
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFLINKER32_H
#define ELFLINKER32_H

#include "elflinker.h"

namespace elena_lang
{
   // --- Elf32Linker ---
   class Elf32Linker : public ElfLinker
   {
   protected:
      virtual int getMachine() = 0;
      unsigned int calcHeaderSize() override;

      void writeELFHeader(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) override;
      void writePHTable(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) override;
      void writeInterpreter(FileWriter* file) override;

   public:
      Elf32Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : ElfLinker(errorProcessor, imageFormatter)
      {
      }
   };

   // --- ElfI386Linker ---
   class ElfI386Linker : public Elf32Linker
   {
   protected:
      int getMachine() override;

   public:
      ElfI386Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : Elf32Linker(errorProcessor, imageFormatter)
      {
      }
   };
}

#endif
