#pragma GCC visibility push(hidden)

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper implementation.
//		Supported platforms: Linux I386
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elfhelper.h"
#include <elf.h>

#define ELF_HEADER_SIZE    0x34

using namespace _ELENA_;

size_t ELFHelper :: findEntryPoint(path_t path)
{
   FileReader reader(path, feRaw, false);
   if (reader.Eof())
      return (size_t)-1;

   // !! hard-coded offset
   reader.seek(0x18u);

   size_t entry = 0;
   reader.readDWord(entry);

   return entry;
}

bool ELFHelper :: seekRODataSegment(StreamReader& reader, size_t& address)
{
   Elf32_Ehdr header;

   reader.read(&header, ELF_HEADER_SIZE);

   int count = header.e_phnum;
   Elf32_Phdr ph_header;

   while (count > 0) {
      reader.read(&ph_header, header.e_phentsize);

      if (ph_header.p_type == PT_LOAD && ph_header.p_flags == PF_R) {
         address = ph_header.p_vaddr;

         return true;
      }

      count--;
   }

   return false;
}
