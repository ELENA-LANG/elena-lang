//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive ELF Image class declaration
//       supported platform: I386, AMD64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFIMAGE_H
#define ELFIMAGE_H

#if defined(__FreeBSD__)

   #define R_X86_64_JUMP_SLOT R_X86_64_JMP_SLOT

#endif

namespace elena_lang
{
   typedef List<ustr_t, freeUStr> LibraryList;

   // --- ElfImageFormatter ---
   class ElfImageFormatter : public ImageFormatter
   {
   protected:
      ForwardResolverBase* _resolver;

      struct ElfData
      {
         ReferenceMap functions;
         ReferenceMap variables;
         LibraryList  libraries;

         pos_t dynamicOffset;
         pos_t dynamicSize;

         ElfData()
            : functions(0), variables(0), libraries(nullptr)
         {
            dynamicOffset = dynamicSize = 0;
         }
      };

      ElfImageFormatter(ForwardResolverBase* resolver)
      {
         _resolver = resolver;
      }

      virtual pos_t fillImportTable(AddressMap::Iterator it, ElfData& elfData);
      virtual void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, RelocationMap& importMapping) = 0;

      virtual void fixSection(MemoryBase* section, AddressSpace& map) = 0;
      virtual void fixImportSection(MemoryBase* section, AddressSpace& map) = 0;

      void mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections, pos_t sectionAlignment,
        pos_t fileAlignment, ElfData& elfData);
      void fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo);

   public:
      void prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
         pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo) override;
    };

   // --- Elf32ImageFormatter ---
   class Elf32ImageFormatter : public ElfImageFormatter
   {
   protected:
      Elf32ImageFormatter(ForwardResolverBase* resolver)
         : ElfImageFormatter(resolver)
      {

      }

      virtual void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) = 0;
      virtual pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) = 0;

      virtual int getRelocationType() = 0;

      void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, RelocationMap& importMapping) override;

   public:

   };

   // --- ElfI386ImageFormatter ---
   class ElfI386ImageFormatter : public Elf32ImageFormatter
   {
      ElfI386ImageFormatter(ForwardResolverBase* resolver)
         : Elf32ImageFormatter(resolver)
      {
      }

      int getRelocationType() override;

      void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) override;
      pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) override;

      void fixSection(MemoryBase* section, AddressSpace& map) override;
      void fixImportSection(MemoryBase* section, AddressSpace& map) override;

   public:
      static ElfI386ImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static ElfI386ImageFormatter instance(resolver);

         return instance;
      }
   };

   // --- Elf64ImageFormatter ---
   class Elf64ImageFormatter : public ElfImageFormatter
   {
   protected:
      Elf64ImageFormatter(ForwardResolverBase* resolver)
         : ElfImageFormatter(resolver)
      {

      }

      virtual void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) = 0;
      virtual pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gofOffset, int entryIndex) = 0;

      virtual int getRelocationType() = 0;

      virtual void writeRELA(MemoryWriter& dynamicWriter, ref_t importRef, pos_t reltabOffset, pos_t count);

      pos_t fillElfHashTable(ElfData& elfData, MemoryBase* image, int nbucket, int nchain);
      void fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment, RelocationMap& importMapping) override;

   public:

   };

   // --- ElfAmd64ImageFormatter ---
   class ElfAmd64ImageFormatter : public Elf64ImageFormatter
   {
   protected:
      ElfAmd64ImageFormatter(ForwardResolverBase* resolver)
         : Elf64ImageFormatter(resolver)
      {
      }

      int getRelocationType() override;

      void writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp) override;
      pos_t writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference,
         pos_t gofOffset, int entryIndex) override;

      void fixSection(MemoryBase* section, AddressSpace& map) override;
      void fixImportSection(MemoryBase* section, AddressSpace& map) override;

   public:
      static ElfAmd64ImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static ElfAmd64ImageFormatter instance(resolver);

         return instance;
      }
   };

   // --- ElfAmd64ImageFormatter ---
   class ElfFreeBSDAmd64ImageFormatter : public ElfAmd64ImageFormatter
   {
      ElfFreeBSDAmd64ImageFormatter(ForwardResolverBase* resolver)
         : ElfAmd64ImageFormatter(resolver)
      {
      }

      int getRelocationType() override;

      void writeRELA(MemoryWriter& dynamicWriter, ref_t importRef, pos_t reltabOffset, pos_t count) override;

   public:
      static ElfFreeBSDAmd64ImageFormatter& getInstance(ForwardResolverBase* resolver)
      {
         static ElfFreeBSDAmd64ImageFormatter instance(resolver);

         return instance;
      }
   };
}

#endif
