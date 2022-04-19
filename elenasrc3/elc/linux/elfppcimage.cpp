//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive ELF Image class implementation
//       supported platform: PPC64le
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "elfppcimage.h"
#include "ppcrelocation.h"

#include <elf.h>

// --- ElfPPC64leImageFormatter ---

void ElfPPC64leImageFormatter :: fillElfData(ImageProviderBase& provider, ElfData& elfData, pos_t fileAlignment,
   RelocationMap& importMapping)
{
   pos_t count = fillImportTable(provider.externals(), elfData);

   Section* code = provider.getTextSection();
   Section* data = provider.getDataSection();
   Section* import = provider.getImportSection();

   MemoryWriter dynamicWriter(data);
   dynamicWriter.align(fileAlignment, 0);

   elfData.dynamicOffset = dynamicWriter.position();

   // reference to GOT
   ref_t importRef = (count + 1) | mskImportRef64;
   ref_t importRelRef = (count + 1) | mskImportRelRef32;
   importMapping.add(importRef, 0);

   // reserve got table
   MemoryWriter gotWriter(import);
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
   writePLTStartEntry(codeWriter, importRelRef, gotStart);
   pos_t pltFirstEntry = codeWriter.position();

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
      //pos_t relPosition = reltabWriter.position() - reltabOffset;
      reltabWriter.writeQReference(importRef, gotPosition);
      reltabWriter.writeQWord((symbolIndex << 32) + relocateType);
      reltabWriter.writeQWord(0);

      // string table entry
      strWriter.writeString(fun.key());

      // plt entry
      writePLTEntry(codeWriter, 0, 0, 0, pltIndex);
      //gotWriter.writeDWord();

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

   //dynamicWriter.writeQWord(DT_RELA);
   //dynamicWriter.writeQReference(importRef, reltabOffset);

   //dynamicWriter.writeQWord(DT_RELASZ);
   //dynamicWriter.writeQWord(count * 24);

   //dynamicWriter.writeQWord(DT_RELAENT);
   //dynamicWriter.writeQWord(24);

   // NOTE : points to 32 bytes before the .glink lazy link symbol resolver stubs
   dynamicWriter.writeQWord(DT_PPC64_GLINK);
   dynamicWriter.writeQReference(mskCodeRef64, pltFirstEntry - 0x20);

   dynamicWriter.writeQWord(DT_PPC64_OPT);
   dynamicWriter.writeQWord(1);

   dynamicWriter.writeQWord(0);
   dynamicWriter.writeQWord(0);

   dynamicWriter.align(fileAlignment, 0);

   elfData.dynamicSize = dynamicWriter.position() - elfData.dynamicOffset;
}

int ElfPPC64leImageFormatter :: getRelocationType()
{
   return R_PPC64_JMP_SLOT;
}

void ElfPPC64leImageFormatter :: fixSection(Section* section, AddressSpace& map)
{
   section->fixupReferences<AddressSpace*>(&map, ppc64relocate);
}

void ElfPPC64leImageFormatter :: fixImportSection(Section* section, AddressSpace& map)
{
   section->fixupReferences<AddressSpace*>(&map, ppc64relocateElf64Import);
}

void ElfPPC64leImageFormatter :: writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference, pos_t disp)
{
   codeWriter.writeQReference(gotReference, disp - 0x1C);

   // ; r12 holds the address of the res_N stub for the target routine      

   // ; Determine addressability.This sequence works for both PIC
   // ;     #and non - PIC code and does not rely on presence of the TOC pointer.
   // mflr r0
   codeWriter.writeDWord(0x7c0802a6);
   // bcl 20, 31, 1f
   codeWriter.writeDWord(0x429F0005);
   // mflr r11
   codeWriter.writeDWord(0x7d6802a6);
   // mtlr r0
   codeWriter.writeDWord(0x7c0803a6);
   // ; Compute.plt section index from entry point address in r12
   // ; .plt section index is placed into r0 as argument to the resolver
   // ld  r0, -10h(r11)
   codeWriter.writeDWord(0xe80bfff0);
   // sub r12, r12, r11
   codeWriter.writeDWord(0x7d8b6050);
   // add r11,r0,r11
   codeWriter.writeDWord(0x7d605a14);
   // subi r0, r12, 44,
   codeWriter.writeDWord(0x380cffd4);
   // ; Load resolver addressand DSO identifier from the
   // ; first two doublewords of the PLT
   // ld r12, 0(r11)
   codeWriter.writeDWord(0xe98b0000);
   // rldicl r0, r0, 62, 4
   codeWriter.writeDWord(0x7800F084);
   // ; Branch to resolver
   // mtctr r12
   codeWriter.writeDWord(0x7d8903a6);
   // ld r11, 8(r11)
   codeWriter.writeDWord(0xe96b0008);
   // bctr
   codeWriter.writeDWord(0x4e800420);
}

pos_t ElfPPC64leImageFormatter :: writePLTEntry(MemoryWriter& codeWriter, pos_t symbolIndex, ref_t gotReference, pos_t gotOffset, int entryIndex)
{
   // NOTE : .glink lazy link symbol resolver stub
   pos_t position = codeWriter.position();

   int offs = -52 - ((entryIndex - 1) * 4);

   // b PLTStartEntry
   codeWriter.writeDWord(0x4B000000 | ((unsigned int)offs & 0xFFFFFF));

   return position;
}

