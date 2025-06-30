//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class body
//		Supported platforms: MacOS 64
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker64.h"

using namespace elena_lang;

// --- MachOLinker64 ---

void MachOLinker64 :: writeMachOHeader(MachOExecutableImage& image, FileWriter* file)
{
   MachOHeader_64 header = {};

   header.magic = getMagicNumber();
   header.cputype = getCPUType();
   header.cpusubtype = getCPUSubType();
   header.filetype = FILE_EXECUTABLE;
   header.ncmds = image.commands.count();
   header.sizeofcmds = image.totalCommandSize;
   header.flags = image.flags;

   file->write((char*)&header, sizeof(MachOHeader_64));
}

Command* MachOLinker64 :: createSegmentCommand(ImageSectionHeader& header, pos_t& fileOffset)
{
   auto command = new SegmentCommand_64();

   command->commandType = Command_Segment_64;
   command->commandSize = sizeof(SegmentCommand_64);
   strncpy(command->segname, header.name.str(), header.name.length() + 1);
   command->vmaddr = header.vaddress;
   command->vmsize = header.memorySize;
   command->fileoff = fileOffset;
   command->filesize = fileSize;
   switch (header.type) {
      case ImageSectionHeader::SectionType::Text:
         command->initprot = command->maxprot = PROT_X | PROT_R;         
         break;
      case ImageSectionHeader::SectionType::Data:
      case ImageSectionHeader::SectionType::UninitializedData:
         command->initprot = command->maxprot = PROT_R | PROT_W;
         break;
      case ImageSectionHeader::SectionType::RData:
         command->initprot = command->maxprot = PROT_R;
         break;
      default:
        assert(false);
        break;
   }
   command->nsects = 0;
   command->flags = 0;

   fileOffset += command->filesize;

   return command;
}
