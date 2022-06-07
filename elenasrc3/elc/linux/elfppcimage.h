//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive ELF Image class declaration
//       supported platform: PPC64le
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFPPCIMAGE_H
#define ELFPPCIMAGE_H

#include "elfimage.h"

namespace elena_lang
{
   // --- ElfPPC64leImageFormatter ---
   class ElfPPC64leImageFormatter : public Elf64ImageFormatter
   {
      ElfPPC64leImageFormatter(ForwardResolverBase* resolver)
         : Elf64ImageFormatter(resolver)
      {
      }

      int getRelocationType() override;

      void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, 
         RelocationMap& importMapping) override;

      void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) override;
      pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) override;

      void fixSection(Section* section, AddressSpace& map) override;
      void fixImportSection(Section* section, AddressSpace& map) override;

   public:
      static ElfPPC64leImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static ElfPPC64leImageFormatter instance(resolver);

         return instance;
      }
   };
}

#endif