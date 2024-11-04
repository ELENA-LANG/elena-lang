//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker base class body
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "ntimage.h"
#include <ctime>

#include "x86relocation.h"

using namespace elena_lang;

constexpr auto TEXT_SECTION   = ".text";
constexpr auto MDATA_SECTION  = ".mdata";
constexpr auto RDATA_SECTION  = ".rdata";
constexpr auto IMPORT_SECTION = ".import";
constexpr auto BSS_SECTION    = ".bss";
constexpr auto DATA_SECTION   = ".data";
constexpr auto TLS_SECTION    = ".tls";

// --- WinNtImageFormatter ---

pos_t WinNtImageFormatter :: fillImportTable(ImportTable& importTable, AddressMap::Iterator it)
{
   pos_t count = 0;
   while (!it.eof()) {
      ustr_t referenceName = it.key();
      size_t index = referenceName.findLast('.');
      IdentifierString dll(referenceName, index);
      ustr_t functionName = referenceName + index + 1;

      if ((*dll).compare(RT_FORWARD)) {
         ustr_t resolvedName = _resolver->resolveExternal(*dll);
         if (!resolvedName.empty()) {
            dll.copy(resolvedName);
         }
      }

      ReferenceMap* map = importTable.get(*dll);
      if (!map) {
         map = new ReferenceMap(0);

         importTable.add(*dll, map);
      }
      map->add(functionName, (ref_t)*it);

      ++it;
      count++;
   }

   return count;
}

void WinNtImageFormatter :: mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
   pos_t sectionAlignment, pos_t fileAlignment)
{
   MemoryBase* text = provider.getTextSection();
   MemoryBase* adata = provider.getADataSection();
   MemoryBase* mdata = provider.getMDataSection();
   MemoryBase* mbdata = provider.getMBDataSection();
   MemoryBase* rdata = provider.getRDataSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* stat = provider.getStatSection();
   MemoryBase* import = provider.getImportSection();
   MemoryBase* tls = provider.getTLSSection();

   pos_t    size = 0;
   pos_t    offset = 0x1000;

   // === address space mapping ===

   // --- code ---
   size = text->length();
   map.code = offset;
   map.codeSize = align(size, fileAlignment);
   map.entryPoint += map.code;

   sections.headers.add(ImageSectionHeader::get(TEXT_SECTION, map.code, ImageSectionHeader::SectionType::Text,
      align(size, sectionAlignment),
      align(size, fileAlignment)));

   sections.items.add(sections.items.count() + 1, { text, true });

   offset = align(offset + size, sectionAlignment);

   // --- adata & mdata ---
   map.adata = offset;
   size = adata->length();
   map.mdata = offset + size;
   size += mdata->length();
   map.mbdata = offset + size;
   size += mbdata->length();
   map.dataSize += align(size, fileAlignment);

   sections.headers.add(ImageSectionHeader::get(MDATA_SECTION, map.adata, 
      ImageSectionHeader::SectionType::RData,
      align(size, sectionAlignment),
      align(size, fileAlignment)));

   sections.items.add(sections.items.count() + 1, { adata, false });
   sections.items.add(sections.items.count() + 1, { mdata, false });
   sections.items.add(sections.items.count() + 1, { mbdata, true });

   offset = align(offset + size, sectionAlignment);

   // --- rdata ---
   size = rdata->length();
   map.rdata = offset;
   map.dataSize += align(size, fileAlignment);

   sections.headers.add(ImageSectionHeader::get(RDATA_SECTION, map.rdata, 
      ImageSectionHeader::SectionType::RData,
      align(size, sectionAlignment),
      align(size, fileAlignment)));

   sections.items.add(sections.items.count() + 1, { rdata, true });

   offset = align(offset + size, sectionAlignment);

   // --- import ---
   map.import = offset;
   map.importSize = size = import->length();

   sections.headers.add(ImageSectionHeader::get(IMPORT_SECTION, map.import, 
      ImageSectionHeader::SectionType::Data,
      align(size, sectionAlignment),
      align(size, fileAlignment)));

   sections.items.add(sections.items.count() + 1, { import, true });

   offset = align(offset + size, sectionAlignment);

   // --- bss ---
   size = data->length();
   map.data = offset;
   map.unintDataSize = align(size, fileAlignment);

   sections.headers.add(ImageSectionHeader::get(BSS_SECTION, map.data, 
      ImageSectionHeader::SectionType::Data,
      align(size, sectionAlignment), 0));

   offset = align(offset + size, sectionAlignment);

   // --- stat ---
   size = stat->length();
   if (size > 0) {
      map.stat = offset;
      map.unintDataSize += align(size, fileAlignment);

      sections.headers.add(ImageSectionHeader::get(DATA_SECTION, map.stat,
         ImageSectionHeader::SectionType::Data,
         align(size, sectionAlignment), 0));
   }

   offset = align(offset + size, sectionAlignment);

   // --- tls ---
   size = tls->length();
   if (size > 0) {
      map.tls = offset;
      map.dataSize += align(size, fileAlignment);

      sections.headers.add(ImageSectionHeader::get(TLS_SECTION, map.tls,
         ImageSectionHeader::SectionType::Data,
         align(size, sectionAlignment),
         align(size, fileAlignment)));

      sections.items.add(sections.items.count() + 1, { tls, true });
   }

   offset = align(offset + size, sectionAlignment);

   map.imageSize = offset;
}

void WinNtImageFormatter :: fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo)
{
   fixSection(provider.getTextSection(), map);
   fixSection(provider.getRDataSection(), map);
   fixSection(provider.getADataSection(), map);
   fixSection(provider.getMDataSection(), map);
   fixSection(provider.getMBDataSection(), map);
   fixImportSection(provider.getImportSection(), map);
   fixSection(provider.getTLSSection(), map);

   // fix up debug info if enabled
   if (withDebugInfo) {
      fixSection(provider.getTargetDebugSection(), map);
   }
}

void WinNtImageFormatter :: prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
                                         pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo)
{
   createTLSSection(provider, map);

   createImportSection(provider, map.importMapping);

   mapImage(provider, map, sections, sectionAlignment, fileAlignment);

   fixImage(provider, map, withDebugInfo);
}

// --- Win32NtImageFormatter ---

void Win32NtImageFormatter :: createTLSSection(ImageProviderBase& provider, AddressSpace& map)
{
   MemoryBase* tlsSection = provider.getTLSSection();
   if (tlsSection && tlsSection->length() > 0) {
      pos_t tls_variable = (pos_t)provider.getTLSVariable();

      // map IMAGE_TLS_DIRECTORY
      MemoryWriter rdataWriter(provider.getRDataSection());
      map.tlsDirectory = rdataWriter.position();
      map.tlsSize = tlsSection->length();

      // create IMAGE_TLS_DIRECTORY
      rdataWriter.writeDReference(mskTLSRef32, 0);              // StartAddressOfRawData
      rdataWriter.writeDReference(mskTLSRef32, map.tlsSize);    // EndAddressOfRawData
      rdataWriter.writeDReference(mskDataRef32, tls_variable);  // AddressOfIndex
      rdataWriter.writeDWord(0);                                // AddressOfCallBacks
      rdataWriter.writeDWord(0);                                // SizeOfZeroFill
      rdataWriter.writeDWord(0);                                // Characteristics
   }
}

void Win32NtImageFormatter :: createImportSection(ImageProviderBase& provider, RelocationMap& importMapping)
{
   ImportTable importTable(nullptr);
   pos_t count = fillImportTable(importTable, provider.externals());

   MemoryBase* import = provider.getImportSection();

   MemoryWriter writer(import);

   ref_t importRef = (count + 1) | mskImportRef32;
   importMapping.add(importRef, 0);

   MemoryWriter tableWriter(import);
   writer.writeBytes(0, (count + 1) * 20);                       // fill import table
   MemoryWriter fwdWriter(import);
   writer.writeBytes(0, (importTable.count() + count) * 4);      // fill forward table
   MemoryWriter lstWriter(import);
   writer.writeBytes(0, (importTable.count() + count) * 4);      // fill import list

   for (auto dll = importTable.start(); !dll.eof(); ++dll) {
      tableWriter.writeDReference(importRef, lstWriter.position()); // OriginalFirstThunk
      tableWriter.writeDWord((pos_t)time(nullptr));                      // TimeDateStamp 
      tableWriter.writeDWord(-1);                                   // ForwarderChain
      tableWriter.writeDReference(importRef, import->length());     // Name

      ustr_t dllName = dll.key();
      writer.writeString(dllName, dllName.length_pos());
      writer.writeString(".dll");

      tableWriter.writeDReference(importRef, fwdWriter.position()); // ForwarderChain

      // fill OriginalFirstThunk & ForwarderChain
      for (auto fun = (*dll)->start(); !fun.eof(); ++fun) {
         ref_t funRef = *fun & ~mskAnyRef;
         importMapping.add(funRef | mskImportRef32, fwdWriter.position());

         fwdWriter.writeDReference(importRef, import->length());
         lstWriter.writeDReference(importRef, import->length());

         writer.writeWord(1);
         writer.writeString(fun.key());
      }
      lstWriter.writeDWord(0);
      fwdWriter.writeDWord(0);
   }
}

void Win32NtImageFormatter::fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocate);
}

void Win32NtImageFormatter::fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocateImport);
}

// --- Win64NtImageFormatter ---

void Win64NtImageFormatter :: createTLSSection(ImageProviderBase& provider, AddressSpace& map)
{
   MemoryBase* tlsSection = provider.getTLSSection();
   if (tlsSection && tlsSection->length() > 0) {
      pos_t tls_variable = (pos_t)provider.getTLSVariable();

      // map IMAGE_TLS_DIRECTORY
      MemoryWriter rdataWriter(provider.getRDataSection());
      map.tlsDirectory = rdataWriter.position();
      map.tlsSize = tlsSection->length();

      // create IMAGE_TLS_DIRECTORY
      rdataWriter.writeQReference(mskTLSRef64, 0);              // StartAddressOfRawData
      rdataWriter.writeQReference(mskTLSRef64, map.tlsSize);    // EndAddressOfRawData
      rdataWriter.writeQReference(mskDataRef64, tls_variable);  // AddressOfIndex
      rdataWriter.writeQWord(0);                                // AddressOfCallBacks
      rdataWriter.writeDWord(0);                                // SizeOfZeroFill
      rdataWriter.writeDWord(0);                                // Characteristics
   }
}

void Win64NtImageFormatter :: createImportSection(ImageProviderBase& provider, RelocationMap& importMapping)
{
   ImportTable importTable(nullptr);
   pos_t count = fillImportTable(importTable, provider.externals());

   MemoryBase* import = provider.getImportSection();

   MemoryWriter writer(import);

   ref_t importRef = (count + 1) | mskImportRef32;
   importMapping.add(importRef, 0);

   MemoryWriter tableWriter(import);
   writer.writeBytes(0, (count + 1) * 20);                      // fill import table
   MemoryWriter fwdWriter(import);
   writer.writeBytes(0, (importTable.count() + count) * 8);     // fill forward table
   MemoryWriter lstWriter(import);
   writer.writeBytes(0, (importTable.count() + count) * 8);     // fill import list

   for (auto dll = importTable.start(); !dll.eof(); ++dll) {
      tableWriter.writeDReference(importRef, lstWriter.position()); // OriginalFirstThunk
      tableWriter.writeDWord((pos_t)time(nullptr));                     // TimeDateStamp 
      tableWriter.writeDWord(-1);                                  // ForwarderChain
      tableWriter.writeDReference(importRef, import->length());     // Name

      ustr_t dllName = dll.key();
      writer.writeString(dllName, dllName.length_pos());
      writer.writeString(".dll");

      tableWriter.writeDReference(importRef, fwdWriter.position()); // ForwarderChain

      // fill OriginalFirstThunk & ForwarderChain
      for (auto fun = (*dll)->start(); !fun.eof(); ++fun) {
         ref_t funRef = *fun & ~mskAnyRef;
         importMapping.add(funRef | mskImportRelRef32, fwdWriter.position());

         fwdWriter.writeQReference(importRef, import->length());
         lstWriter.writeQReference(importRef, import->length());

         writer.writeWord(1);
         writer.writeString(fun.key());
      }
      lstWriter.writeQWord(0);
      fwdWriter.writeQWord(0);
   }
}

void Win64NtImageFormatter::fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocate64);
}

void Win64NtImageFormatter::fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocateImport);
}
