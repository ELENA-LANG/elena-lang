//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Win32 Linker class body
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "ntlinker32.h"

#if defined(__unix__)

#include "windows/ntdeclaration.h"

#else

#include <windows.h>

#endif

#ifndef IMAGE_SIZEOF_NT_OPTIONAL_HEADER
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER 224
#endif

#define MAJOR_OS           0x05
#define MINOR_OS           0x00

using namespace elena_lang;

// --- Win32NtLinker ---

void Win32NtLinker :: prepareNtImage(ImageProviderBase& provider, WinNtExecutableImage& image, PlatformType uiType)
{
   WinNtLinker::prepareNtImage(provider, image, uiType);

   image.machine = IMAGE_FILE_MACHINE_I386;
   image.optionalHeaderSize = IMAGE_SIZEOF_NT_OPTIONAL_HEADER;
   image.characteristics |= IMAGE_FILE_32BIT_MACHINE;

   image.headerSize += IMAGE_SIZEOF_NT_OPTIONAL_HEADER;
   image.stackInfo.reserve = provider.getStackReserved();

   // !! temporal
   image.stackInfo.commit = 0x1000;
   image.heapInfo.commit = 0x1000;
   image.heapInfo.reserve = 0x100000;
}

void Win32NtLinker :: writeNtHeader(WinNtExecutableImage& image, FileWriter& file)
{
   IMAGE_OPTIONAL_HEADER32   header = {};

   header.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
   header.MajorLinkerVersion = 1;
   header.MinorLinkerVersion = 0;
   header.SectionAlignment = image.sectionAlignment;
   header.FileAlignment = image.fileAlignment;

   header.SizeOfCode = align(image.addressSpace.codeSize, header.FileAlignment);
   header.SizeOfInitializedData = align(image.addressSpace.dataSize, header.FileAlignment);
   header.SizeOfUninitializedData = align(image.addressSpace.unintDataSize, header.FileAlignment);

   header.AddressOfEntryPoint = image.addressSpace.entryPoint;
   header.BaseOfCode = image.addressSpace.code;
   header.BaseOfData = image.addressSpace.rdata;

   header.ImageBase = (DWORD)image.addressSpace.imageBase;

   header.MajorOperatingSystemVersion = MAJOR_OS;
   header.MinorOperatingSystemVersion = MINOR_OS;
   header.MajorImageVersion = 0;                                               // not used
   header.MinorImageVersion = 0;
   header.MajorSubsystemVersion = MAJOR_OS;                                    // set for Win 4.0
   header.MinorSubsystemVersion = MINOR_OS;
   header.Win32VersionValue = 0;

   header.SizeOfImage = image.addressSpace.imageSize;
   header.SizeOfHeaders = align(image.headerSize, header.FileAlignment);
   header.CheckSum = 0;                                                        // For EXE file
   header.Subsystem = image.subsystem;

   header.DllCharacteristics = 0;                                              // For EXE file
   header.LoaderFlags = 0;                                                     // not used

   header.SizeOfStackReserve = image.stackInfo.reserve;
   header.SizeOfStackCommit = image.stackInfo.commit;
   header.SizeOfHeapReserve = image.heapInfo.reserve;
   header.SizeOfHeapCommit = image.heapInfo.commit;

   header.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
   for (unsigned long i = 0; i < header.NumberOfRvaAndSizes; i++) {
      header.DataDirectory[i].VirtualAddress = 0;
      header.DataDirectory[i].Size = 0;
   }
   //if (getSize(info.image->getImportSection()) > 0) { // !! temporal
   header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = image.addressSpace.import;
   header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = image.addressSpace.importSize;
   //}

   // IMAGE_DIRECTORY_ENTRY_TLS
   if (image.addressSpace.tlsDirectory != 0xFFFFFFFF) {
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = image.addressSpace.rdata + image.addressSpace.tlsDirectory;
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = image.addressSpace.tlsSize;
   }
   file.write(&header, IMAGE_SIZEOF_NT_OPTIONAL_HEADER);
}
