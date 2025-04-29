//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive ELF Image class implementation
//       supported platform: I386, AMD64
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "elfimage.h"
#include "elfcommon.h"

#include "x86relocation.h"

#include <elf.h>

using namespace elena_lang;

// --- ElfImageFormatter ---

pos_t ElfImageFormatter :: fillImportTable(AddressMap::Iterator it, ElfData& elfData)
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

      if (!functionName.startsWith("##")) {
         elfData.functions.add(functionName, (ref_t)*it);
         count++;
      }
      else elfData.variables.add(functionName + 2, (ref_t)*it); 

      ustr_t lib = elfData.libraries.retrieve<ustr_t>(*dll, [](ustr_t item, ustr_t current)
      {
         return item.compare(current);
      });
      if (lib.empty()) {
         elfData.libraries.add((*dll).clone());
      }

      ++it;
   }

   return count;
}

void ElfImageFormatter :: mapImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections, pos_t sectionAlignment,
   pos_t fileAlignment, ElfData& elfData)
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
   MemoryBase* tls = provider.getTLSSection();

   // === address space mapping ===

   // --- code ---
   map.code = map.headerSize;               // code section should always be first
   map.codeSize = text->length() + map.headerSize;
   map.entryPoint += map.code;

   sectionSize = fileSize = align(map.codeSize, fileAlignment);

   sections.headers.add(ImageSectionHeader::get(nullptr, 0, ImageSectionHeader::SectionType::Text,
      sectionSize, fileSize));

   sections.items.add(sections.items.count() + 1, { text, true });

   // --- rodata segment ---
   sectionOffset = align(sectionSize, sectionAlignment);
   fileOffset = align(fileSize, fileAlignment);
   // NOTE : due to loader requirement, adjust offset
   sectionOffset += (fileOffset & (sectionAlignment - 1));

   // adata & mdata
   map.dataSize = adata->length();
   map.adata = sectionOffset;

   map.dataSize += mdata->length();
   map.mdata = map.adata + adata->length();

   map.dataSize += mbdata->length();
   map.mbdata = map.mdata + mdata->length();

   fileSize = sectionSize = align(adata->length() + mdata->length() + mbdata->length(), fileAlignment);

   // rdata
   map.dataSize += rdata->length();
   map.rdata = map.adata + fileSize;

   fileSize += align(rdata->length(), fileAlignment);
   sectionSize += align(rdata->length(), fileAlignment);

   sections.headers.add(ImageSectionHeader::get(nullptr, map.adata, ImageSectionHeader::SectionType::RData,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count(), { adata, false });
   sections.items.add(sections.headers.count(), { mdata, false });
   sections.items.add(sections.headers.count(), { mbdata, true });
   sections.items.add(sections.headers.count(), { rdata, true });

   // --- data segment ---
   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);
   fileOffset = align(fileOffset + fileSize, fileAlignment);
   // NOTE : due to loader requirement, adjust offset
   sectionOffset += (fileOffset & (sectionAlignment - 1));

   map.importSize = import->length();
   map.import = sectionOffset;

   fileSize = sectionSize = align(map.importSize, fileAlignment);

   map.dataSize += data->length();
   map.data = map.import + fileSize;

   map.dictionary.add(elfDynamicOffset, fileOffset + fileSize + elfData.dynamicOffset);
   map.dictionary.add(elfDynamicVAddress, sectionOffset + fileSize + elfData.dynamicOffset);
   map.dictionary.add(elfDynamicSize, elfData.dynamicSize);

   fileSize += align(data->length(), fileAlignment);
   sectionSize += align(data->length(), fileAlignment);

   if (tls->length() > 0) {
      map.tls = map.data + align(data->length(), fileAlignment);
      map.dataSize += align(tls->length(), fileAlignment);

      sectionSize += align(tls->length(), fileAlignment);
      fileSize += align(tls->length(), fileAlignment);

      map.stat = map.tls + tls->length();
   }
   else map.stat = map.data + data->length();

   map.dataSize += stat->length();

   sectionSize += align(stat->length(), fileAlignment);

   sections.headers.add(ImageSectionHeader::get(nullptr, map.import, ImageSectionHeader::SectionType::Data,
      sectionSize, fileSize));

   sections.items.add(sections.headers.count(), { import, true });
   sections.items.add(sections.headers.count(), { data, true });

   if (tls->length() > 0) {
      map.dictionary.add(elfTLSSize, tls->length());

      sections.items.add(sections.headers.count(), { tls, true });
   }

   sectionOffset = align(sectionOffset + sectionSize, sectionAlignment);
   fileOffset = align(fileOffset + fileSize, fileAlignment);
   // NOTE : due to loader requirement, adjust offset
   sectionOffset += (fileOffset & (sectionAlignment - 1));
}

void ElfImageFormatter :: fixImage(ImageProviderBase& provider, AddressSpace& map, bool withDebugInfo)
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

void ElfImageFormatter :: prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
   pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo)
{
   ElfData elfData;
   fillElfData(provider, elfData, fileAlignment, map.importMapping);

   mapImage(provider, map, sections, sectionAlignment, fileAlignment, elfData);

   fixImage(provider, map, withDebugInfo);
}

// --- Elf32ImageFormatter ---

void Elf32ImageFormatter :: fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment,
   RelocationMap& importMapping)
{
   pos_t count = fillImportTable(provider.externals(), elfData);

   MemoryBase* code = provider.getTextSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* import = provider.getImportSection();

   MemoryWriter dynamicWriter(data);
   dynamicWriter.align(FILE_ALIGNMENT, 0);

   elfData.dynamicOffset = dynamicWriter.position();

   // reference to GOT
   ref_t importRef = (count + 1) | mskImportRef32;
   importMapping.add(importRef, 0);

   // reserve got table
   MemoryWriter gotWriter(import);
   gotWriter.writeDReference(mskDataRef32, elfData.dynamicOffset);
   gotWriter.writeDWord(0); // reserved for run-time linker
   gotWriter.writeDWord(0);
   pos_t gotStart = gotWriter.position();
   gotWriter.writeBytes(0, count * 4);
   gotWriter.seek(gotStart);

   // reserve relocation table
   MemoryWriter reltabWriter(import);
   pos_t reltabOffset = reltabWriter.position();
   reltabWriter.writeBytes(0, count * 8);
   reltabWriter.seek(reltabOffset);

   // reserve symbol table
   MemoryWriter symtabWriter(import);
   pos_t symtabOffset = symtabWriter.position();
   symtabWriter.writeBytes(0, (count + 1) * 16);
   symtabWriter.seek(symtabOffset + 16);

   // string table
   MemoryWriter strWriter(import);
   pos_t strOffset = strWriter.position();
   strWriter.writeChar('\0');

   // code writer
   MemoryWriter codeWriter(code);
   writePLTStartEntry(codeWriter, importRef, 0);

   int relocateType = getRelocationType();
   int symbolIndex = 1;
   int pltIndex = 1;
   for(auto fun = elfData.functions.start(); !fun.eof(); ++fun) {
      pos_t gotPosition = gotWriter.position();

      ref_t funRef = *fun & ~mskAnyRef;
      importMapping.add(funRef | mskImportRef32, gotPosition);

      pos_t strIndex = strWriter.position() - strOffset;

      // symbol table entry
      symtabWriter.writeDWord(strIndex);
      symtabWriter.writeDWord(0);
      symtabWriter.writeDWord(0);
      symtabWriter.writeDWord(0x12);

      // relocation table entry
      pos_t relPosition = reltabWriter.position() - reltabOffset;
      reltabWriter.writeDReference(importRef, gotPosition);
      reltabWriter.writeDWord((symbolIndex << 8) + relocateType);

      // string table entry
      strWriter.writeString(fun.key());

      // got / plt entry
      pos_t position = writePLTEntry(codeWriter, relPosition, importRef, gotPosition, pltIndex);
      gotWriter.writeDReference(mskCodeRef32, position);

      symbolIndex++;
      pltIndex++;
   }

   // write dynamic segment

   // write libraries needed to be loaded
   auto dll = elfData.libraries.start();
   while (!dll.eof()) {
      dynamicWriter.writeDWord(DT_NEEDED);
      dynamicWriter.writeDWord(strWriter.position() - strOffset);

      strWriter.writeString(*dll);

      ++dll;
   }
   strWriter.writeChar('\0');
   pos_t strLength = strWriter.position() - strOffset;

   dynamicWriter.writeDWord(DT_STRTAB);
   dynamicWriter.writeDReference(importRef, strOffset);

   dynamicWriter.writeDWord(DT_SYMTAB);
   dynamicWriter.writeDReference(importRef, symtabOffset);

   dynamicWriter.writeDWord(DT_STRSZ);
   dynamicWriter.writeDWord(strLength);

   dynamicWriter.writeDWord(DT_SYMENT);
   dynamicWriter.writeDWord(16);

   dynamicWriter.writeDWord(DT_PLTGOT);
   dynamicWriter.writeDReference(importRef, /*gotStart*/0);

   dynamicWriter.writeDWord(DT_PLTRELSZ);
   dynamicWriter.writeDWord(count * 8);

   dynamicWriter.writeDWord(DT_PLTREL);
   dynamicWriter.writeDWord(DT_REL);

   dynamicWriter.writeDWord(DT_JMPREL);
   dynamicWriter.writeDReference(importRef, reltabOffset);

   dynamicWriter.writeDWord(DT_REL);
   dynamicWriter.writeDReference(importRef, reltabOffset);

   dynamicWriter.writeDWord(DT_RELSZ);
   dynamicWriter.writeDWord(count * 8);

   dynamicWriter.writeDWord(DT_RELENT);
   dynamicWriter.writeDWord(8);

   dynamicWriter.writeDWord(0);
   dynamicWriter.writeDWord(0);

   dynamicWriter.align(FILE_ALIGNMENT, 0);

   elfData.dynamicSize = dynamicWriter.position() - elfData.dynamicOffset;
}

// --- ElfI386ImageFormatter ---

int ElfI386ImageFormatter :: getRelocationType()
{
   return R_386_JMP_SLOT;
}

void ElfI386ImageFormatter :: fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocate);
}

void ElfI386ImageFormatter :: fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocateElfImport);
}

void ElfI386ImageFormatter :: writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t)
{
   codeWriter.writeWord(0x35FF);
   codeWriter.writeDReference(gotReference, 4);
   codeWriter.writeWord(0x25FF);
   codeWriter.writeDReference(gotReference, 8);
   codeWriter.writeDWord(0);
}

pos_t ElfI386ImageFormatter :: writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gotOffset, int entryIndex)
{
   codeWriter.writeWord(0x25FF);
   codeWriter.writeDReference(gotReference, gotOffset);

   pos_t position = codeWriter.position();

   codeWriter.writeByte(0x68);
   codeWriter.writeDWord(symbolIndex);
   codeWriter.writeByte(0xE9);
   codeWriter.writeDWord(0x10*(-1-entryIndex));

   return position;
}

// --- Elf64ImageFormatter ---

void Elf64ImageFormatter :: fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment,
   RelocationMap& importMapping)
{
   pos_t count = fillImportTable(provider.externals(), elfData);
   pos_t global_count = /*elfData.variables.count()*/0;

   MemoryBase* code = provider.getTextSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* import = provider.getImportSection();

   MemoryWriter dynamicWriter(data);
   dynamicWriter.align(FILE_ALIGNMENT, 0);

   elfData.dynamicOffset = dynamicWriter.position();

   // reference to GOT
   ref_t importRef = (count + 1) | mskImportRef64;
   ref_t importRelRef = (count + 1) | mskImportRelRef32;
   importMapping.add(importRef, 0);

   // reserve got table
   MemoryWriter gotWriter(import);
   //pos_t gotStartVar = gotWriter.position();
   //gotWriter.writeBytes(0, global_count * 8);
   pos_t gotPltPos = gotWriter.position();
   gotWriter.writeQReference(mskDataRef64, elfData.dynamicOffset);
   gotWriter.writeQWord(0);   // reserved for run-time linker
   gotWriter.writeQWord(0);
   pos_t gotStart = gotWriter.position();
   gotWriter.writeBytes(0, count * 8);
   gotWriter.seek(gotStart);
   
   // reserve relocation table
   MemoryWriter reltabWriter(import);
   //pos_t relGlobalOffset = reltabWriter.position();
   //reltabWriter.writeBytes(0, global_count * 24);
   pos_t reltabOffset = reltabWriter.position();
   reltabWriter.writeBytes(0, count * 24);
   reltabWriter.seek(reltabOffset);
      
   // reserve symbol table
   MemoryWriter symtabWriter(import);
   pos_t symtabOffset = symtabWriter.position();
   symtabWriter.writeBytes(0, (count + 1) * 24);
   symtabWriter.seek(symtabOffset + 24);

   // string table
   MemoryWriter strWriter(import);
   pos_t strOffset = strWriter.position();
   strWriter.writeChar('\0');

   //// globals
   //if (global_count > 0) {
   //   gotWriter.seek(gotStartVar);
   //   reltabWriter.seek(relGlobalOffset);

   //   int globalRelocateType = /*getGlobalRelocationType()*/6;
   //   for (auto glob = elfData.variables.start(); !glob.eof(); ++glob) {
   //      printf("%s\n", glob.key().str());

   //      pos_t gotPosition = gotWriter.position();

   //      ref_t globalRef = *glob & ~mskAnyRef;
   //      importMapping.add(globalRef | mskImportRelRef32, gotPosition);

   //      pos_t strIndex = strWriter.position() - strOffset;

   //      // relocation table entry
   //      reltabWriter.writeQReference(importRef, gotPosition);
   //      reltabWriter.writeDWord(globalRelocateType);
   //      reltabWriter.writeDWord(strIndex);
   //      reltabWriter.writeQWord(0);

   //      // string table entry
   //      strWriter.writeString(glob.key());

   //      gotWriter.writeQWord(0);
   //   }
   //}

   // code writer
   MemoryWriter codeWriter(code);
   writePLTStartEntry(codeWriter, importRelRef, /*gotPltPos*/0);

   // functions
   //gotWriter.seek(gotStart);
   //reltabWriter.seek(reltabOffset);
   int relocateType = getRelocationType();
   long long symbolIndex = 1;
   int pltIndex = 1;
   for (auto fun = elfData.functions.start(); !fun.eof(); ++fun) {
      pos_t gotPosition = gotWriter.position();

      ref_t funRef = *fun & ~mskAnyRef;
      importMapping.add(funRef | mskImportRelRef32, gotPosition);

      pos_t strIndex = strWriter.position() - strOffset;

      // symbol table entry
      symtabWriter.writeDWord(strIndex);
      symtabWriter.writeByte(0x12);
      symtabWriter.writeByte(0);
      symtabWriter.writeWord(SHN_UNDEF);
      symtabWriter.writeQWord(0);
      symtabWriter.writeQWord(0);

      // relocation table entry
      reltabWriter.writeQReference(importRef, gotPosition);
      reltabWriter.writeQWord((symbolIndex << 32) + relocateType);
      reltabWriter.writeQWord(0);

      // string table entry
      strWriter.writeString(fun.key());

      // got / plt entry
      pos_t position = writePLTEntry(codeWriter, /*relPosition*/pltIndex - 1, importRelRef, gotPosition, pltIndex);
      gotWriter.writeQReference(mskCodeRef64, position);

      symbolIndex++;
      pltIndex++;
   }

   // write dynamic segment

   // write libraries needed to be loaded
   auto dll = elfData.libraries.start();
   while (!dll.eof()) {
      dynamicWriter.writeQWord(DT_NEEDED);
      dynamicWriter.writeQWord(strWriter.position() - strOffset);

      strWriter.writeString(*dll);

      ++dll;
   }
   strWriter.writeChar('\0');
   pos_t strLength = strWriter.position() - strOffset;

   dynamicWriter.writeQWord(DT_STRTAB);
   dynamicWriter.writeQReference(importRef, strOffset);

   dynamicWriter.writeQWord(DT_SYMTAB);
   dynamicWriter.writeQReference(importRef, symtabOffset);

   dynamicWriter.writeQWord(DT_STRSZ);
   dynamicWriter.writeQWord(strLength);

   dynamicWriter.writeQWord(DT_SYMENT);
   dynamicWriter.writeQWord(24);

   dynamicWriter.writeQWord(DT_PLTGOT);
   dynamicWriter.writeQReference(importRef, /*gotPltPos*/0);

   dynamicWriter.writeQWord(DT_PLTRELSZ);
   dynamicWriter.writeQWord(count * 24);

   dynamicWriter.writeQWord(DT_PLTREL);
   dynamicWriter.writeQWord(DT_RELA);

   dynamicWriter.writeQWord(DT_JMPREL);
   dynamicWriter.writeQReference(importRef, reltabOffset);

#if defined(__FreeBSD__)
   dynamicWriter.writeQWord(DT_RELA);
   dynamicWriter.writeQReference(importRef, /*relGlobalOffset*/0);

   dynamicWriter.writeQWord(DT_RELASZ);
   dynamicWriter.writeQWord(/*global_count * 24*/0);
#else
   assert(global_count == 0); // !! temporally globals are supported only for FreeBSD

   dynamicWriter.writeQWord(DT_RELA);
   dynamicWriter.writeQReference(importRef, reltabOffset);

   dynamicWriter.writeQWord(DT_RELASZ);
   dynamicWriter.writeQWord(count * 24);
#endif

   dynamicWriter.writeQWord(DT_RELAENT);
   dynamicWriter.writeQWord(24);

   dynamicWriter.writeQWord(0);
   dynamicWriter.writeQWord(0);

   dynamicWriter.align(fileAlignment, 0);

   elfData.dynamicSize = dynamicWriter.position() - elfData.dynamicOffset;
}

// --- ElfAmd64ImageFormatter ---

int ElfAmd64ImageFormatter:: getRelocationType()
{
#if defined(__FreeBSD__)
   return R_X86_64_JMP_SLOT;
#else
   return R_X86_64_JUMP_SLOT;
#endif
}

void ElfAmd64ImageFormatter :: fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocate64);
}

void ElfAmd64ImageFormatter :: fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, relocateElf64Import);
}

void ElfAmd64ImageFormatter :: writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t gotPlt)
{
   codeWriter.writeWord(0x35FF);
   codeWriter.writeDReference(gotReference, gotPlt + 8);
   codeWriter.writeWord(0x25FF);
   codeWriter.writeDReference(gotReference, gotPlt + 16);
#if defined(__FreeBSD__)
   codeWriter.writeDWord(0x401F0F);
#else
   codeWriter.writeDWord(0);
#endif
}

pos_t ElfAmd64ImageFormatter :: writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gotOffset, int entryIndex)
{
   codeWriter.writeWord(0x25FF);
   codeWriter.writeDReference(gotReference, gotOffset);

   pos_t position = codeWriter.position();

   codeWriter.writeByte(0x68);
   codeWriter.writeDWord(symbolIndex);
   codeWriter.writeByte(0xE9);
   codeWriter.writeDWord(0x10 * (-1 - entryIndex));

   return position;
}
