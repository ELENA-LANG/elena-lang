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
   //MemoryBase* import = provider.getImportSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* stat = provider.getStatSection();

   // === address space mapping ===

   sectionSize = map.headerSize;
   sections.headers.add(ImageSectionHeader::get(__PAGEZERO_SEGMENT, 0, ImageSectionHeader::SectionType::Data,
      sectionSize, 0));

   // --- __TEXT (code & rdata & adata & mdata & mbdata & rdata) ---
   sectionOffset = sectionSize;

   map.code = map.headerSize;               // code section should always be first
   map.codeSize = text->length();
   map.entryPoint += map.code;

   fileOffset += align(map.codeSize, fileAlignment);

   // adata & mdata & mbdata
   map.dataSize = adata->length();
   map.adata = fileOffset;

   map.dataSize += mdata->length();
   map.mdata = map.adata + adata->length();

   map.dataSize += mbdata->length();
   map.mbdata = map.mdata + mdata->length();

   fileOffset += align(map.dataSize, fileAlignment);

   // rdata
   map.dataSize += rdata->length();
   map.rdata = fileOffset;

   fileOffset += align(rdata->length(), fileAlignment);
   sectionSize = fileSize = fileOffset;

   sections.headers.add(ImageSectionHeader::get(__TEXT_SEGMENT, map.code, ImageSectionHeader::SectionType::Text,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count() + 1, { text, true });
   sections.items.add(sections.headers.count() + 1, { adata, false });
   sections.items.add(sections.headers.count() + 1, { mdata, false });
   sections.items.add(sections.headers.count() + 1, { mbdata, true });
   sections.items.add(sections.headers.count() + 1, { rdata, true });

   // --- __DATA (data & stat) segment ---
   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);
   
   map.dataSize += data->length();
   map.data = sectionOffset;

   map.dataSize += stat->length();
   map.stat = map.data + align(data->length(), fileAlignment);

   fileSize = 0;
   sectionSize = align(stat->length(), fileAlignment) + align(data->length(), fileAlignment);

   sections.headers.add(ImageSectionHeader::get(__DATA_SEGMENT, map.data, ImageSectionHeader::SectionType::Data,
      sectionSize, fileSize));
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
