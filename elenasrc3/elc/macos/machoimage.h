//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive MachO Image class declaration
//       supported platform: ARM64
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOIMAGE_H
#define MACHOIMAGE_H

namespace elena_lang
{
   //typedef List<ustr_t, freeUStr> LibraryList;

   // --- MachOImageFormatter ---
   class MachOImageFormatter : public ImageFormatter
   {
   protected:
      ForwardResolverBase* _resolver;

      //struct ElfData
      //{
      //   ReferenceMap functions;
      //   LibraryList  libraries;

      //   pos_t dynamicOffset;
      //   pos_t dynamicSize;

      //   ElfData()
      //      : functions(0), libraries(nullptr)
      //   {
      //      dynamicOffset = dynamicSize = 0;
      //   }
      //};

      MachOImageFormatter(ForwardResolverBase* resolver)
      {
         _resolver = resolver;
      }

      //virtual pos_t fillImportTable(AddressMap::Iterator it, ElfData& elfData);
      //virtual void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, RelocationMap& importMapping) = 0;

      //virtual void fixSection(MemoryBase* section, AddressSpace& map) = 0;
      //virtual void fixImportSection(MemoryBase* section, AddressSpace& map) = 0;

      //void mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections, pos_t sectionAlignment,
      //  pos_t fileAlignment, ElfData& elfData);
      //void fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo);

   public:
      void prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
         pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo) override;
    };

   // --- MachO64ImageFormatter ---
   class MachO64ImageFormatter : public MachOImageFormatter
   {
   protected:
      MachO64ImageFormatter(ForwardResolverBase* resolver)
         : MachOImageFormatter(resolver)
      {

      }

      //virtual void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) = 0;
      //virtual pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) = 0;

      //virtual int getRelocationType() = 0;

      //void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, RelocationMap& importMapping) override;
   public:
   };
}

#endif
