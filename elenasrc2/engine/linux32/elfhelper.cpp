#pragma GCC visibility push(hidden)

//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper implementation.
//		Supported platforms: Linux I386
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elfhelper.h"
#include <elf.h>
//#include <windows.h>

#define ELF_HEADER_SIZE    0x34

using namespace _ELENA_;

bool ELFHelper :: seekDebugSegment(StreamReader& reader, size_t& address)
{
   Elf32_Ehdr header;

   reader.read(&header, ELF_HEADER_SIZE);

   int count = header.e_phnum;
   Elf32_Phdr ph_header;
   bool skipRData = true;

   while (count > 0) {
      reader.read(&ph_header, header.e_phentsize);

      if (ph_header.p_type == PT_LOAD && ph_header.p_flags == PF_R) {
         if (!skipRData) {
            address = ph_header.p_vaddr;

            return true;
         }
         else skipRData = false;
      }

      count--;
   }

   return false;
}
