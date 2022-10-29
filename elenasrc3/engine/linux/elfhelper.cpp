//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ELFHelper.
//		Supported platforms: Linux I386
//                                                  (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elfhelper.h"
#include <elf.h>

using namespace elena_lang;

constexpr unsigned int ELF_HEADER_SIZE = 0x34;
constexpr unsigned int ELF64_HEADER_SIZE = 0x40;

// --- ELFHelper ---

addr_t ELFHelper :: findEntryPoint(path_t path)
{
   FileReader reader(path, FileRBMode, FileEncoding::Raw, false);
   if (reader.eof())
      return INVALID_ADDR;

#if __i386__
   Elf32_Ehdr header;
   reader.read(&header, ELF_HEADER_SIZE);
#else
   Elf64_Ehdr header;
   reader.read(&header, ELF64_HEADER_SIZE);
#endif

   return header.e_entry;
}

bool ELFHelper :: seekRODataSegment(StreamReader& reader, addr_t& rvaAddress)
{
#if __i386__
   Elf32_Ehdr header;
   Elf32_Phdr ph_header;
   reader.read(&header, ELF_HEADER_SIZE);
#else
   Elf64_Ehdr header;
   Elf64_Phdr ph_header;
   reader.read(&header, ELF64_HEADER_SIZE);
#endif

   printf("seekRODataSegment %x\n", header.e_ident[0]);
   printf("%x\n", header.e_ident[1]);
   printf("%x\n", header.e_ident[2]);
   printf("%x\n", header.e_ident[3]);

   int count = header.e_phnum;
   while (count > 0) {
      reader.read(&ph_header, header.e_phentsize);

      if (ph_header.p_type == PT_LOAD && ph_header.p_flags == PF_R) {
         rvaAddress = ph_header.p_vaddr;

         return true;
      }

      count--;
   }

   return false;
}