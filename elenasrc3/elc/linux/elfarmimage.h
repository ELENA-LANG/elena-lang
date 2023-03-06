//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive ELF Image class declaration
//       supported platform: ARM64
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFPPCIMAGE_H
#define ELFPPCIMAGE_H

#include "elfimage.h"

namespace elena_lang
{
   // --- ElfARM64ImageFormatter ---
   class ElfARM64ImageFormatter : public Elf64ImageFormatter
   {
      ElfARM64ImageFormatter(ForwardResolverBase* resolver)
         : Elf64ImageFormatter(resolver)
      {
      }

      int getRelocationType() override;

      void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, 
         RelocationMap& importMapping) override;

      void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) override;
      pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) override;

      void fixSection(MemoryBase* section, AddressSpace& map) override;
      void fixImportSection(MemoryBase* section, AddressSpace& map) override;

   public:
      static ElfARM64ImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static ElfARM64ImageFormatter instance(resolver);

         return instance;
      }
   };
}

#endif