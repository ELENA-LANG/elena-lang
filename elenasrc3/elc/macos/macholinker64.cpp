//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class body
//		Supported platforms: MacOS 64
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker64.h"

using namespace elena_lang;

static pos_t countSegmentSections(ImageSections& sections, int headerIndex)
{
   pos_t counter = 0;
   for (auto it = sections.items.start(); !it.eof(); ++it) {
      if (it.key() == headerIndex && (*it).section != nullptr && (*it).section->length() > 0)
         counter++;
   }

   return counter;
}

static void fillSectionCommand(section_64& command, ImageSectionHeader& header, ImageItem& item, addr_t imageBase)
{
   strncpy(command.sectname, item.name, 16);
   strncpy(command.segname, header.name.str(), 16);

   command.addr = imageBase + item.fileOffset;
   command.size = item.section->length();
   command.offset = item.fileOffset;
   command.align = (item.name && strcmp(item.name, "__import") == 0)
      ? 3
      : ((header.type == ImageSectionHeader::SectionType::Text
         || (item.name && (strcmp(item.name, "__adata") == 0 || strcmp(item.name, "__mdata") == 0
            || strcmp(item.name, "__mbdata") == 0 || strcmp(item.name, "__const") == 0))) ? 2 : 0);
   command.reloff = 0;
   command.nreloc = 0;
   if (header.type == ImageSectionHeader::SectionType::Text && item.name && strcmp(item.name, "__text") == 0) {
      command.flags = S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS;
   }
   else if (item.name && strcmp(item.name, "__import") == 0) {
      command.flags = S_NON_LAZY_SYMBOL_POINTERS;
   }
   else command.flags = 0;
   command.reserved1 = 0;
   command.reserved2 = 0;
   command.reserved3 = 0;
}

// --- MachOLinker64 ---

void MachOLinker64 :: writeMachOHeader(MachOExecutableImage& image, StreamWriter* file)
{
   mach_header_64 header = {};

   header.magic = getMagicNumber();
   header.cputype = getCPUType();
   header.cpusubtype = getCPUSubType();
   header.filetype = MH_EXECUTE;
   header.ncmds = image.commands.count();
   header.sizeofcmds = image.totalCommandSize;
   header.flags = image.flags;

   file->write((char*)&header, sizeof(mach_header_64));
}

Command* MachOLinker64 :: createSegmentCommand(ImageSectionHeader& header, int headerIndex,
   ImageSections& sections, pos_t& fileOffset, addr_t imageBase)
{
   pos_t sectionCounter = countSegmentSections(sections, headerIndex);
   pos_t commandSize = sizeof(segment_command_64) + sectionCounter * sizeof(section_64);

   auto command = new Command(commandSize);
   auto segment = command->as<segment_command_64>();
   auto section = reinterpret_cast<section_64*>((char*)segment + sizeof(segment_command_64));

   segment->cmd = LC_SEGMENT_64;
   segment->cmdsize = commandSize;
   strncpy(segment->segname, header.name.str(), header.name.length() + 1);
   if (header.name.compare(__PAGEZERO_SEGMENT)) {
      segment->vmaddr = 0;
      segment->vmsize = imageBase;
      segment->fileoff = 0;
      segment->filesize = 0;
      segment->initprot = segment->maxprot = 0;
   }
   else {
      segment->vmaddr = imageBase + header.vaddress;
      segment->vmsize = header.memorySize;
      segment->fileoff = header.name.compare(__TEXT_SEGMENT) ? 0 : header.vaddress;
      segment->filesize = header.fileSize;
      switch (header.type) {
         case ImageSectionHeader::SectionType::Text:
            segment->initprot = segment->maxprot = PROT_X | PROT_R;
            break;
         case ImageSectionHeader::SectionType::RData:
            segment->initprot = segment->maxprot = PROT_R;
            break;
         case ImageSectionHeader::SectionType::Data:
         case ImageSectionHeader::SectionType::UninitializedData:
            segment->initprot = segment->maxprot = PROT_R | PROT_W;
            break;
         default:
           assert(false);
           break;
      }

      fileOffset = segment->fileoff + segment->filesize;
   }
   segment->nsects = sectionCounter;
   segment->flags = header.name.compare(__DATA_CONST_SEGMENT) ? SG_READ_ONLY : 0;

   pos_t sectionIndex = 0;
   for (auto it = sections.items.start(); !it.eof(); ++it) {
      if (it.key() == headerIndex && (*it).section != nullptr && (*it).section->length() > 0) {
         fillSectionCommand(section[sectionIndex++], header, *it, imageBase);
      }
   }

   return command;
}
