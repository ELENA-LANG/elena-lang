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
   header.ncmds = image.commands.count_pos();
   header.sizeofcmds = image.totalCommandSize;
   header.flags = image.flags;

   file->write((char*)&header, sizeof(MachOHeader_64));
}

Command* MachOLinker64 :: createPAGEZEROCommand(MachOExecutableImage& image)
{

}

void MachOLinker64 :: writeCommand(MachOExecutableImage& image, FileWriter* file, Command* command)
{

}
