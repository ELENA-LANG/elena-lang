//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive MachO Image class implementation
// 
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "machoimage.h"
#include "machocommon.h"

#include "x86relocation.h"

using namespace elena_lang;

// --- MachOImageFormatter ---

void MachOImageFormatter :: mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections, pos_t sectionAlignment,
   pos_t fileAlignment/*, ElfData& elfData*/)
{
   pos_t fileOffset = 0, sectionOffset = 0;
   pos_t sectionSize = 0, fileSize = 0;

   MemoryBase* text = provider.getTextSection();
   MemoryBase* rdata = provider.getRDataSection();
   MemoryBase* adata = provider.getADataSection();
   MemoryBase* mdata = provider.getMDataSection();
   MemoryBase* mbdata = provider.getMBDataSection();
   MemoryBase* import = provider.getImportSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* stat = provider.getStatSection();

   // === address space mapping ===

   sectionSize = (pos_t)map.imageBase;
   sections.headers.add(ImageSectionHeader::get(__PAGEZERO_SEGMENT, 0, ImageSectionHeader::SectionType::Data,
      sectionSize, 0));

   // --- __TEXT (code) ---
   sectionOffset = 0;

   map.code = map.headerSize;               // code section should always be first
   map.codeSize = text->length() + map.headerSize;
   map.entryPoint += map.code;

   fileOffset = map.code + align(map.codeSize, fileAlignment);
   sectionSize = fileSize = align(fileOffset, sectionAlignment);

   sections.headers.add(ImageSectionHeader::get(__TEXT_SEGMENT, sectionOffset, ImageSectionHeader::SectionType::Text,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count(), { text, true });

   // --- __DATA_CONST (adata & mdata & mbdata & rdata) ---
   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);

   map.dataSize = adata->length();
   map.adata = sectionOffset;

   map.dataSize += mdata->length();
   map.mdata = map.adata + adata->length();

   map.dataSize += mbdata->length();
   map.mbdata = map.mdata + mdata->length();

   fileOffset = align(map.mbdata + mbdata->length(), fileAlignment);

   map.dataSize += rdata->length();
   map.rdata = fileOffset;

   fileOffset += align(rdata->length(), fileAlignment);
   sectionSize = fileSize = align(fileOffset - sectionOffset, sectionAlignment);

   sections.headers.add(ImageSectionHeader::get(__DATA_CONST_SEGMENT, sectionOffset, ImageSectionHeader::SectionType::Data,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count(), { adata, false });
   sections.items.add(sections.headers.count(), { mdata, false });
   sections.items.add(sections.headers.count(), { mbdata, true });
   sections.items.add(sections.headers.count(), { rdata, true });

   // --- __DATA (data & stat) segment ---
   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);
   
   map.importSize = import->length();
   map.import = sectionOffset;

   fileSize = sectionSize = align(map.importSize, fileAlignment);

   map.dataSize += data->length();
   map.data = map.import + fileSize;

   map.dataSize += stat->length();
   map.stat = map.data + align(data->length(), fileAlignment);

   fileSize += align(data->length(), fileAlignment) + align(stat->length(), fileAlignment);
   sectionSize = fileSize = align(fileSize, sectionAlignment);

   sections.headers.add(ImageSectionHeader::get(__DATA_SEGMENT, sectionOffset, ImageSectionHeader::SectionType::Data,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count(), { import, true });
   sections.items.add(sections.headers.count(), { data, true });
   sections.items.add(sections.headers.count(), { stat, true });

   // --- __LINKEDIT segment ---
   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);
   map.imageSize = sectionOffset;

   sections.headers.add(ImageSectionHeader::get(__LINKEDIT_SEGMENT, sectionOffset, ImageSectionHeader::SectionType::RData,
      0, 0));
}

void MachOImageFormatter :: prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
   pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo)
{
   //MachOData data;
   //fillElfData(provider, elfData, fileAlignment, map.importMapping);

   mapImage(provider, map, sections, sectionAlignment, fileAlignment/*, data*/);

   fixImage(provider, map, withDebugInfo);
}

void MachOImageFormatter :: fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo)
{
   fixSection(provider.getTextSection(), map);
   fixSection(provider.getRDataSection(), map);
   fixSection(provider.getDataSection(), map);
   fixSection(provider.getADataSection(), map);
   fixSection(provider.getMDataSection(), map);
   fixSection(provider.getMBDataSection(), map);
   fixImportSection(provider.getImportSection(), map);

   // fix up debug info if enabled
   if (withDebugInfo) {
      fixSection(provider.getTargetDebugSection(), map);
   }
}

// --- MachOAmd64ImageFormatter ---

void MachOAmd64ImageFormatter :: fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocate64);
}

void MachOAmd64ImageFormatter :: fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally commented out
//   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocateElf64Import);
}
