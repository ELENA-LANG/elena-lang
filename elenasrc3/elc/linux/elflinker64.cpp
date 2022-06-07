//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class body
//		Supported platforms: Linux 64
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elflinker64.h"
#include "elfcommon.h"
#include "elf.h"

#define MAGIC_NUMBER "\x07F""ELF"

constexpr unsigned int HEADER_INTERPRETER_SIZE  = 0x200;
constexpr unsigned int ELF64_HEADER_SIZE        = 0x40;
constexpr unsigned int ELF64_PH_SIZE            = 0x38;

constexpr auto INTERPRETER64_PATH = "/lib64/ld-linux-x86-64.so.2";

using namespace elena_lang;

// --- Elf64Linker ---

ustr_t Elf64Linker :: getInterpreter()
{
   return INTERPRETER64_PATH;
}

unsigned int Elf64Linker :: calcHeaderSize()
{
   return HEADER_INTERPRETER_SIZE;
}

void Elf64Linker :: writeELFHeader(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num)
{
   Elf64_Ehdr header;

   // e_ident
   memset(header.e_ident, 0, EI_NIDENT);
   memcpy(header.e_ident, MAGIC_NUMBER, 4);
   header.e_ident[EI_CLASS] = ELFCLASS64;
   header.e_ident[EI_DATA] = ELFDATA2LSB;
   header.e_ident[EI_VERSION] = EV_CURRENT;

   header.e_type = ET_EXEC;
   header.e_machine = getMachine();
   header.e_version = EV_CURRENT;

   header.e_entry = image.addressMap.imageBase + image.addressMap.entryPoint;
   header.e_phoff = ELF64_HEADER_SIZE;
   header.e_shoff = 0;
   header.e_flags = image.flags;
   header.e_ehsize = ELF64_HEADER_SIZE;
   header.e_phentsize = ELF64_PH_SIZE;
   header.e_phnum = ph_num;
   header.e_shentsize = 0;
   header.e_shnum = 0;
   header.e_shstrndx = SHN_UNDEF;

   file->write((char*)&header, ELF64_HEADER_SIZE);
}

void Elf64Linker :: writePHTable(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num)
{
   Elf64_Phdr ph_header;

   // Program header
   ph_header.p_type = PT_PHDR;
   ph_header.p_offset = ELF64_HEADER_SIZE;
   ph_header.p_vaddr = image.addressMap.imageBase + ELF64_HEADER_SIZE;
   ph_header.p_paddr = image.addressMap.imageBase + ELF64_HEADER_SIZE;
   ph_header.p_filesz = ph_num * ELF64_PH_SIZE;
   ph_header.p_memsz = ph_num * ELF64_PH_SIZE;
   ph_header.p_flags = PF_R;
   ph_header.p_align = 8;
   file->write(&ph_header, ELF64_PH_SIZE);

   // Program loader
   ph_header.p_type = PT_INTERP;
   ph_header.p_offset = ELF64_HEADER_SIZE + ph_num * ELF64_PH_SIZE;
   ph_header.p_vaddr = ph_header.p_paddr = image.addressMap.imageBase + ELF64_HEADER_SIZE + ph_num * ELF64_PH_SIZE;
   ph_header.p_memsz = ph_header.p_filesz = getlength(getInterpreter()) + 1;
   ph_header.p_flags = PF_R;
   ph_header.p_align = 1;
   file->write((char*)&ph_header, ELF64_PH_SIZE);

   unsigned int offset = 0;
   for (auto it = image.imageSections.headers.start(); !it.eof(); ++it) {
      ImageSectionHeader info = *it;

      ph_header.p_type = PT_LOAD;
      ph_header.p_offset = offset;
      ph_header.p_vaddr = image.addressMap.imageBase + info.vaddress;
      ph_header.p_paddr = image.addressMap.imageBase + info.vaddress;
      ph_header.p_align = image.sectionAlignment;
      ph_header.p_memsz = ph_header.p_filesz = info.fileSize;

      switch (info.type) {
         case ImageSectionHeader::SectionType::Text:
            ph_header.p_flags = PF_R + PF_X;
            break;
         case ImageSectionHeader::SectionType::RData:
            ph_header.p_flags = PF_R;
            break;
         case ImageSectionHeader::SectionType::Data:
            ph_header.p_flags = PF_R + PF_W;
            break;
         default:
            break;
      }

      file->write((char*)&ph_header, ELF64_PH_SIZE);

      offset += ph_header.p_filesz;
   }

   // Dynamic
   pos_t dynamicOffset = image.addressMap.dictionary.get(elfDynamicOffset);
   pos_t dynamicVAddress = image.addressMap.dictionary.get(elfDynamicVAddress);
   pos_t dynamicSize = image.addressMap.dictionary.get(elfDynamicSize);

   ph_header.p_type = PT_DYNAMIC;
   ph_header.p_offset = dynamicOffset;
   ph_header.p_paddr = ph_header.p_vaddr = image.addressMap.imageBase + dynamicVAddress;
   ph_header.p_filesz = ph_header.p_memsz = align(dynamicSize, 8);
   ph_header.p_flags = PF_R + PF_W;
   ph_header.p_align = 8;
   file->write((char*)&ph_header, ELF64_PH_SIZE);
}

void Elf64Linker :: writeInterpreter(FileWriter* file)
{
   file->writeString(getInterpreter(), getlength_pos(getInterpreter()) + 1);
}

// --- ElfAmd64Linker ---

int ElfAmd64Linker :: getMachine()
{
   return EM_X86_64;
}
