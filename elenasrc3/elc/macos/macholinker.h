//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOLINKER_H
#define MACHOLINKER_H

#include "clicommon.h"
#include "machocommon.h"

namespace elena_lang
{
   struct MachOImportInfo
   {
      IdentifierString library;
      IdentifierString symbol;
      pos_t            slotOffset;
      int              libraryOrdinal;
      ref_t            reference;

      MachOImportInfo(ustr_t library, ustr_t symbol, pos_t slotOffset, int libraryOrdinal, ref_t reference)
      {
         this->library.copy(library);
         this->symbol.copy(symbol);
         this->slotOffset = slotOffset;
         this->libraryOrdinal = libraryOrdinal;
         this->reference = reference;
      }
   };

   typedef List<MachOImportInfo*, freeobj> MachOImportList;
   typedef List<ustr_t, freeUStr>          MachOLibraryList;
   typedef List<pos_t>                     MachORebaseList;

   struct MachOExecutableImage
   {
      unsigned int    sectionAlignment;
      unsigned int    fileAlignment;
      bool            withDebugInfo;
      int             flags;
      pos_t           stackReserved;
      pos_t           codeSignatureOffset;
      pos_t           codeSignatureSize;
      pos_t           codeSignaturePageSize;
      pos_t           linkEditOffset;
      pos_t           linkEditSize;
      pos_t           dyldFixupsOffset;
      pos_t           dyldFixupsSize;
      pos_t           rebaseInfoOffset;
      pos_t           rebaseInfoSize;
      pos_t           bindInfoOffset;
      pos_t           bindInfoSize;
      pos_t           exportsTrieOffset;
      pos_t           exportsTrieSize;
      pos_t           symtabOffset;
      pos_t           symtabCount;
      pos_t           definedSymbolCount;
      pos_t           indirectSymtabOffset;
      pos_t           indirectSymtabCount;
      pos_t           stringTableOffset;
      pos_t           stringTableSize;
      pos_t           functionStartsOffset;
      pos_t           functionStartsSize;
      pos_t           dataInCodeOffset;
      pos_t           dataInCodeSize;
      IdentifierString codeSignatureIdentifier;

      AddressSpace    addressMap;

      ImageSections   imageSections;
      MachOImportList imports;
      MachOLibraryList importLibraries;
      MachORebaseList rebaseOffsets;

      int             totalCommandSize;
      Commands        commands;

      MachOExecutableImage(bool withDebugInfo)
         : imageSections({}), imports(nullptr), importLibraries(nullptr), rebaseOffsets(0), commands(nullptr)
      {
         this->fileAlignment = this->sectionAlignment = 0;
         this->flags = 0;
         this->stackReserved = 0;
         this->codeSignatureOffset = 0;
         this->codeSignatureSize = 0;
         this->codeSignaturePageSize = 0;
         this->linkEditOffset = 0;
         this->linkEditSize = 0;
         this->dyldFixupsOffset = 0;
         this->dyldFixupsSize = 0;
         this->rebaseInfoOffset = 0;
         this->rebaseInfoSize = 0;
         this->bindInfoOffset = 0;
         this->bindInfoSize = 0;
         this->exportsTrieOffset = 0;
         this->exportsTrieSize = 0;
         this->symtabOffset = 0;
         this->symtabCount = 0;
         this->definedSymbolCount = 0;
         this->indirectSymtabOffset = 0;
         this->indirectSymtabCount = 0;
         this->stringTableOffset = 0;
         this->stringTableSize = 0;
         this->functionStartsOffset = 0;
         this->functionStartsSize = 0;
         this->dataInCodeOffset = 0;
         this->dataInCodeSize = 0;
         this->withDebugInfo = withDebugInfo;
         this->totalCommandSize = 0;
      }
   };

   // --- MachOLinker ---
   class MachOLinker : public LinkerBase
   {
   protected:
      ImageFormatter* _imageFormatter;

      virtual unsigned long getMagicNumber() = 0;

      virtual cpu_type_t getCPUType() = 0;
      virtual cpu_subtype_t getCPUSubType() = 0;

      virtual Command* createSegmentCommand(ImageSectionHeader& header, int headerIndex,
         ImageSections& sections, pos_t& fileOffset, addr_t imageBase) = 0;

      virtual void prepareMachOImage(ForwardResolverBase* resolver, ImageProviderBase& provider, MachOExecutableImage& image);
      virtual void prepareCommands(MachOExecutableImage& image);
      void addCommand(MachOExecutableImage& image, Command* command);

      virtual void writeMachOHeader(MachOExecutableImage& image, StreamWriter* file) = 0;
      virtual void writeSegments(MachOExecutableImage& image, StreamWriter* file);
      void writeSection(StreamWriter* file, MemoryBase* section);
      void writeLinkEditData(MachOExecutableImage& image, StreamWriter* file);
      void writeCodeSignature(MachOExecutableImage& image, MemoryDump& executable, StreamWriter* file);

      bool createExecutable(MachOExecutableImage& image, path_t exePath);

   public:
      LinkResult run(ProjectBase& project, ImageProviderBase& code, PlatformType osType, PlatformType uiType,
         path_t exeExtension) override;

      MachOLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : LinkerBase(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
