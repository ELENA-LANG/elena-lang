//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive MachO Image class declaration
//       supported platform: ARM64
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOPPCIMAGE_H
#define MACHOPPCIMAGE_H

#include "machoimage.h"

namespace elena_lang
{
   // --- MachOARM64ImageFormatter ---
   class MachOARM64ImageFormatter : public MachO64ImageFormatter
   {
      MachOARM64ImageFormatter(ForwardResolverBase* resolver)
         : MachO64ImageFormatter(resolver)
      {
      }

      //int getRelocationType() override;

      //void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, 
      //   RelocationMap& importMapping) override;

      //void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) override;
      //pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) override;

      //void fixSection(MemoryBase* section, AddressSpace& map) override;
      //void fixImportSection(MemoryBase* section, AddressSpace& map) override;

   public:
      static MachOARM64ImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static MachOARM64ImageFormatter instance(resolver);

         return instance;
      }
   };
}

#endif