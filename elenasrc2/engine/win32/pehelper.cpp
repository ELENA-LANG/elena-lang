//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper implementation.
//		Supported platforms: x86
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "pehelper.h"
#include <windows.h>

using namespace _ELENA_;

size_t PEHelper :: findEntryPoint(path_t path)
{
   FileReader reader(path, feRaw, false);
   if (reader.Eof())
      return (size_t)-1;

   // !! hard-coded offset
   reader.seek(0xC8);

   size_t entry = 0;
   reader.readDWord(entry);

   return entry + 0x400000;
}

bool PEHelper :: seekSection(StreamReader& reader, char* name, size_t& address)
{
   size_t base = reader.Position();

   IMAGE_DOS_HEADER dosHeader;
   reader.read(&dosHeader, sizeof(IMAGE_DOS_HEADER));

   //Check for Valid DOS file
   if(dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
      return false;

   IMAGE_NT_HEADERS ntHeader;
   reader.seek(base + dosHeader.e_lfanew);
   reader.read(&ntHeader, sizeof(IMAGE_NT_HEADERS));
   //Check if valid PE file  
   if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
      return false;

   reader.seek(base + dosHeader.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + ntHeader.FileHeader.SizeOfOptionalHeader);
   for (int i = 0 ; i < ntHeader.FileHeader.NumberOfSections ; i++) {
      IMAGE_SECTION_HEADER header;

      reader.read(&header, sizeof(IMAGE_SECTION_HEADER));
      if (StringHelper::compare((char*)header.Name, name))
      {
         address = header.VirtualAddress + 0x400000;

         reader.seek(base + header.VirtualAddress);

         return true;
      }
   }

   return false;
}