//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper implementation.
//		Supported platforms: x86
//                                              (C)2005-2019, by Alexei Rakov
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
   //reader.seek(0xC8u);
   //
   //ref_t entry = 0;
   //reader.readDWord(entry);
   //
   //return entry + 0x400000;

   IMAGE_DOS_HEADER dosh;
   reader.read(&dosh, sizeof(IMAGE_DOS_HEADER));
   reader.seek((pos_t)dosh.e_lfanew);
   
   IMAGE_NT_HEADERS inth;
   reader.read(&inth, sizeof(IMAGE_NT_HEADERS));
   
   return inth.OptionalHeader.AddressOfEntryPoint + inth.OptionalHeader.ImageBase;
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
   reader.seek((size_t)(base + dosHeader.e_lfanew));
   reader.read(&ntHeader, sizeof(IMAGE_NT_HEADERS));
   //Check if valid PE file  
   if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
      return false;

   reader.seek((size_t)(base + dosHeader.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + ntHeader.FileHeader.SizeOfOptionalHeader));
   for (int i = 0 ; i < ntHeader.FileHeader.NumberOfSections ; i++) {
      IMAGE_SECTION_HEADER header;

      reader.read(&header, sizeof(IMAGE_SECTION_HEADER));
      if (ident_t((char*)header.Name).compare(name))
      {
         address = header.VirtualAddress + 0x400000;

         reader.seek((size_t)(base + header.VirtualAddress));

         return true;
      }
   }

   return false;
}

bool PEHelper :: seekSection64(StreamReader& reader, char* name, size_t& address)
{
   size_t base = reader.Position();

   IMAGE_DOS_HEADER dosHeader;
   reader.read(&dosHeader, sizeof(IMAGE_DOS_HEADER));

   //Check for Valid DOS file
   if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
      return false;

   IMAGE_NT_HEADERS ntHeader;
   reader.seek((size_t)(base + dosHeader.e_lfanew));
   reader.read(&ntHeader, sizeof(IMAGE_NT_HEADERS64));
   //Check if valid PE file  
   if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
      return false;

   reader.seek((size_t)(base + dosHeader.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + ntHeader.FileHeader.SizeOfOptionalHeader));
   for (int i = 0; i < ntHeader.FileHeader.NumberOfSections; i++) {
      IMAGE_SECTION_HEADER header;

      reader.read(&header, sizeof(IMAGE_SECTION_HEADER));
      if (ident_t((char*)header.Name).compare(name))
      {
         address = header.VirtualAddress + 0x400000;

         reader.seek((size_t)(base + header.VirtualAddress));

         return true;
      }
   }

   return false;
}