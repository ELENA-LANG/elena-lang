//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux 64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFLINKER64_H
#define ELFLINKER64_H

#include "elflinker.h"

namespace elena_lang
{
   // --- Elf64Linker ---
   class Elf64Linker : public ElfLinker
   {
   protected:
      virtual int getMachine() = 0;
      unsigned int calcHeaderSize() override;

      void writeELFHeader(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) override;
      void writePHTable(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) override;
      void writeInterpreter(FileWriter* file) override;

      virtual ustr_t getInterpreter();

   public:
      Elf64Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : ElfLinker(errorProcessor, imageFormatter)
      {

      }
   };

   // --- ElfAmd64Linker ---
   class ElfAmd64Linker : public Elf64Linker
   {
   protected:
      int getMachine() override;

   public:
      ElfAmd64Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : Elf64Linker(errorProcessor, imageFormatter)
      {

      }
   };
}

#endif
