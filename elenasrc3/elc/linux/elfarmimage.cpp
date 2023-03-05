//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive ELF Image class implementation
//       supported platform: ARM64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "elfarmimage.h"
#include "armrelocation.h"

#include <elf.h>

// --- ElfARM64ImageFormatter ---

void ElfARM64ImageFormatter :: fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment,
   RelocationMap& importMapping)
{
   pos_t count = fillImportTable(provider.externals(), elfData);

   MemoryBase* code = provider.getTextSection();
   MemoryBase* data = provider.getDataSection();
   MemoryBase* import = provider.getImportSection();

   MemoryWriter dynamicWriter(data);
   dynamicWriter.align(fileAlignment, 0);

   elfData.dynamicOffset = dynamicWriter.position();

   // reference to GOT
   ref_t importRef = (count + 1) | mskImportRef64;
   importMapping.add(importRef, 0);

   // reserve got table
   MemoryWriter gotWriter(import);
   gotWriter.writeQWord(0);
   gotWriter.writeQWord(0);
   gotWriter.writeQWord(0);
   pos_t gotStart = gotWriter.position();
   gotWriter.writeBytes(0, count * 8);
   gotWriter.seek(gotStart);

   // reserve relocation table
   MemoryWriter reltabWriter(import);
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

   // code writer
   MemoryWriter codeWriter(code);

   // should be aligned to 4 byte border
   pos_t aligned = elena_lang::align(codeWriter.position(), 16);
   while (codeWriter.position() < aligned)
      codeWriter.writeDWord(0xd503201F);

   pos_t pltStartEntry = codeWriter.position();
   writePLTStartEntry(codeWriter, (count + 1) | mskImportRef, gotStart);

   int relocateType = getRelocationType();
   long long symbolIndex = 1;
   int pltIndex = 1;
   for (auto fun = elfData.functions.start(); !fun.eof(); ++fun) {
      pos_t gotPosition = gotWriter.position();

      ref_t funRef = *fun & ~mskAnyRef;
      importMapping.add(funRef | mskImportRef64, gotPosition);

      pos_t strIndex = strWriter.position() - strOffset;

      // symbol table entry
      symtabWriter.writeDWord(strIndex);
      symtabWriter.writeByte(0x12);
      symtabWriter.writeByte(0x60);
      symtabWriter.writeWord(SHN_UNDEF);
      symtabWriter.writeQWord(0);
      symtabWriter.writeQWord(0);

      // relocation table entry
      pos_t relPosition = reltabWriter.position() - reltabOffset;
      reltabWriter.writeQReference(importRef, gotPosition);
      reltabWriter.writeQWord((symbolIndex << 32) + relocateType);
      reltabWriter.writeQWord(0);

      // string table entry
      strWriter.writeString(fun.key());

      // plt entry
      writePLTEntry(codeWriter, 0, funRef | mskImportRef, 0, pltIndex);
      gotWriter.writeQReference(mskCodeRef64, pltStartEntry);

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
   dynamicWriter.writeQReference(importRef, /*gotStart*/0);

   dynamicWriter.writeQWord(DT_PLTRELSZ);
   dynamicWriter.writeQWord(count * 24);

   dynamicWriter.writeQWord(DT_PLTREL);
   dynamicWriter.writeQWord(DT_RELA);

   dynamicWriter.writeQWord(DT_JMPREL);
   dynamicWriter.writeQReference(importRef, reltabOffset);

   dynamicWriter.writeQWord(0);
   dynamicWriter.writeQWord(0);

   dynamicWriter.align(fileAlignment, 0);

   elfData.dynamicSize = dynamicWriter.position() - elfData.dynamicOffset;
}

int ElfARM64ImageFormatter :: getRelocationType()
{
   return R_AARCH64_JUMP_SLOT;
}

void ElfARM64ImageFormatter:: fixSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, arm64relocate);
}

void ElfARM64ImageFormatter :: fixImportSection(MemoryBase* section, AddressSpace& map)
{
   // !! temporally
   dynamic_cast<Section*>(section)->fixupReferences<AddressSpace*>(&map, arm64relocateElf64Import);
}

void ElfARM64ImageFormatter :: writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t)
{
   // stp x16, x30, [sp, #-16]!
   codeWriter.writeDWord(0xa9bf7bf0);
   // adrp x16, (GOT+16)
   codeWriter.writeDReference(gotReference | mskRelRef32Hi4k, 0x90000010);
   // ldr x17, [x16, #PLT_GOT+0x10]
   codeWriter.writeDReference(gotReference | mskRef32Lo12_8, 0xf9400211 + (2 << 10)/*0xf9400a11*/);
   // add x16, x16,#PLT_GOT+0x10
   codeWriter.writeDReference(gotReference | mskRef32Lo12, 0x91000210 + (0x10 << 10)/*0x91004210*/);
   /* br x17  */
   codeWriter.writeDWord(0xd61f0220);
   /* nop */
   codeWriter.writeDWord(0xd503201F);
   /* nop */
   codeWriter.writeDWord(0xd503201F);
   /* nop */
   codeWriter.writeDWord(0xd503201F);
}

pos_t ElfARM64ImageFormatter :: writePLTEntry(MemoryWriter& codeWriter, pos_t, ref_t funReference, pos_t, 
   int)
{
   //pos_t position = codeWriter.position();

   // adrp x16, PLTGOT + n * 8
   codeWriter.writeDReference(funReference | mskRelRef32Hi4k, 0x90000010);
   // ldr x17, [x16, PLTGOT + n * 8]
   codeWriter.writeDReference(funReference | mskRef32Lo12_8, 0xf9400211/*0xf9400a11*/);
   // add x16, x16, :lo12:PLTGOT + n * 8
    codeWriter.writeDReference(funReference | mskRef32Lo12, 0x91000210/*0x91004210*/);
   // br x17
   codeWriter.writeDWord(0xd61f0220);

   return 0;
}

