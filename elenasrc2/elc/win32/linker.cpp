//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: Win32
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "linker.h"
#include "errors.h"

#include <windows.h>

#include <time.h>

#define MAJOR_OS           0x05
#define MINOR_OS           0x00

#define FILE_ALIGNMENT     0x200
#define SECTION_ALIGNMENT  0x1000
#define IMAGE_BASE         0x00400000

#define TEXT_SECTION       ".text"
#define RDATA_SECTION      ".rdata"
#define DATA_SECTION       ".data"
#define BSS_SECTION        ".bss"
#define IMPORT_SECTION     ".import"
#define TLS_SECTION        ".tls"
#define DEBUG_SECTION      ".debug"

#ifndef IMAGE_SIZEOF_NT_OPTIONAL_HEADER
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER 224
#endif

using namespace _ELENA_;

inline int getSize(Section* section)
{
   return section ? section->Length() : 0;
}

// --- Reallocate ---

ref_t reallocate(ref_t pos, ref_t key, ref_t disp, void* map)
{
   int base = ((ImageBaseMap*)map)->base + key & ~mskAnyRef;

   switch(key & mskImageMask) {
      case mskCodeRef:
         return ((ImageBaseMap*)map)->code + base + disp;
      case mskRelCodeRef:
         return (key & ~mskAnyRef) + disp - pos - 4;
      case mskRDataRef:
         return ((ImageBaseMap*)map)->rdata + base + disp;
      case mskStatRef:
         return ((ImageBaseMap*)map)->stat + base + disp;
      case mskDataRef:
         return ((ImageBaseMap*)map)->bss + base + disp;
      case mskTLSRef:
         return ((ImageBaseMap*)map)->tls + base + disp;
      case mskImportRef:
      {
         int address = ((ImageBaseMap*)map)->base + ((ImageBaseMap*)map)->importMapping.get(key);

         return ((ImageBaseMap*)map)->import + address + disp;
      }
      case mskDebugRef:
         return ((ImageBaseMap*)map)->debug + base + disp;
      default:
         return disp;
   }
}

ref_t reallocateImport(ref_t, ref_t key, ref_t disp, void* map)
{
   if ((key & mskImageMask)==mskImportRef) {
      return ((ImageBaseMap*)map)->import + disp + ((ImageBaseMap*)map)->importMapping.get(key);
   }
   else return disp;
}

// --- Linker ---

void Linker :: mapImage(ImageInfo& info)
{
   info.map.base = info.project->IntSetting(opImageBase, IMAGE_BASE);

   int alignment = info.project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   info.map.code = 0x1000;               // code section should always be first
   info.map.rdata = align(info.map.code + getSize(info.image->getTextSection()), alignment);
   info.map.bss = align(info.map.rdata + getSize(info.image->getRDataSection()), alignment);
   info.map.stat = align(info.map.bss + getSize(info.image->getBSSSection()), alignment);
   info.map.tls = align(info.map.stat + getSize(info.image->getStatSection()), alignment);
   info.map.import = align(info.map.tls + getSize(info.image->getTLSSection()), alignment);
   info.map.debug = align(info.map.import + getSize(info.image->getImportSection()), alignment);

   info.imageSize = align(info.map.debug + getSize(info.image->getDebugSection()), alignment);
}

int Linker :: fillImportTable(ImageInfo& info)
{
   int count = 0;

   ReferenceMap::Iterator it = info.image->getExternalIt();
   while (!it.Eof()) {
      ident_t external = it.key();

      ident_t function = external + StringHelper::findLast(external, '.') + 1;

      IdentifierString dll(external + getlength(DLL_NAMESPACE) + 1, function - (external + getlength(DLL_NAMESPACE)) - 2);

      ReferenceMap* functions = info.importTable.get(dll);
      if (functions==NULL) {
         functions = new ReferenceMap(0);

         info.importTable.add(dll, functions);
      }
      functions->add(function, *it);

      it++;
      count++;
   }
   return count;
}

void Linker :: createImportTable(ImageInfo& info)
{
   size_t count = fillImportTable(info);
   Section* import = info.image->getImportSection();

   MemoryWriter writer(import);

   // reference to the import section
   ref_t importRef = (count + 1) | mskImportRef;
   info.map.importMapping.add(importRef, 0);

   MemoryWriter tableWriter(import);
   writer.writeBytes(0, (count + 1) * 20);               // fill import table
   MemoryWriter fwdWriter(import);
   writer.writeBytes(0, (info.importTable.Count()+count)*4);  // fill forward table
   MemoryWriter lstWriter(import);
   writer.writeBytes(0, (info.importTable.Count()+count)*4);  // fill import list

   ImportTable::Iterator dll = info.importTable.start();
   while (!dll.Eof()) {
      tableWriter.writeRef(importRef, lstWriter.Position());              // OriginalFirstThunk
      tableWriter.writeDWord((int)time(NULL));                            // TimeDateStamp
      tableWriter.writeDWord(-1);                                         // ForwarderChain
      tableWriter.writeRef(importRef, import->Length());                  // Name

      const char* dllName = dll.key();
      writer.write(dllName, getlength(dllName));
      writer.writeChar('.');
      writer.writeLiteral("dll");

      tableWriter.writeRef(importRef, fwdWriter.Position());              // ForwarderChain

      // fill OriginalFirstThunk & ForwarderChain
      ReferenceMap::Iterator fun = (*dll)->start();
      while (!fun.Eof()) {
         info.map.importMapping.add(*fun, fwdWriter.Position());

         fwdWriter.writeRef(importRef, import->Length());
         lstWriter.writeRef(importRef, import->Length());

         writer.writeWord(1);                                             // Hint (not used)
         writer.writeLiteral(fun.key());

         fun++;
      }
      lstWriter.writeDWord(0);                                            // mark end of chains
      fwdWriter.writeDWord(0);

      dll++;
   }
}

void Linker :: fixImage(ImageInfo& info)
{
   Section* text = info.image->getTextSection();
   Section* rdata = info.image->getRDataSection();
   Section* bss = info.image->getBSSSection();
   Section* stat = info.image->getStatSection();
   Section* tls = info.image->getTLSSection();
   Section* import = info.image->getImportSection();

  // fix up text reallocate
   text->fixupReferences(&info.map, reallocate);

  // fix up rdata section
   rdata->fixupReferences(&info.map, reallocate);

  // fix up bss section
   bss->fixupReferences(&info.map, reallocate);

  // fix up stat section
   stat->fixupReferences(&info.map, reallocate);

  // fix up tls section
   tls->fixupReferences(&info.map, reallocate);

  // fix up import section
   import->fixupReferences(&info.map, reallocateImport);

  // fix up debug info if enabled
   if (info.withDebugInfo) {
      Section* debug = info.image->getDebugSection();

      debug->fixupReferences(&info.map, reallocate);
   }
}

int Linker :: countSections(Image* image)
{
   int count = 0;

   if (getSize(image->getTextSection()))
      count++;

   if (getSize(image->getRDataSection()))
      count++;

   if (getSize(image->getBSSSection()))
      count++;

   if (getSize(image->getStatSection()))
      count++;

   if (getSize(image->getTLSSection()))
      count++;

   if (getSize(image->getImportSection()))
      count++;

   if (getSize(image->getDebugSection()))
      count++;

   return count;
}

void Linker :: writeDOSStub(Project* project, FileWriter* file)
{
   Path stubPath;
   Path::loadPath(stubPath, project->StrSetting(opAppPath));
   Path::combinePath(stubPath, "winstub.ex_");
   
   FileReader stub(stubPath, L"rb", feRaw, false);

   if (stub.isOpened()) {
      file->read(&stub, stub.Length());
   }
   else project->raiseError(errInvalidFile, IdentifierString(stubPath));
}

void Linker :: writeHeader(FileWriter* file, short characteristics, int sectionCount)
{
   IMAGE_FILE_HEADER   header;

   header.Machine = IMAGE_FILE_MACHINE_I386; // !! machine type may be different;
   header.NumberOfSections = (short)sectionCount;
   header.TimeDateStamp = (int)time(NULL);
   header.SizeOfOptionalHeader = IMAGE_SIZEOF_NT_OPTIONAL_HEADER;
   header.Characteristics = characteristics;
   header.Characteristics |= IMAGE_FILE_32BIT_MACHINE;
   header.Characteristics |= IMAGE_FILE_LOCAL_SYMS_STRIPPED;
   header.Characteristics |= IMAGE_FILE_LINE_NUMS_STRIPPED;
   header.PointerToSymbolTable = 0;
   header.NumberOfSymbols = 0;

   file->write(&header, IMAGE_SIZEOF_FILE_HEADER);
}

void Linker :: writeNTHeader(ImageInfo& info, FileWriter* file, ref_t tls_directory)
{
   IMAGE_OPTIONAL_HEADER   header;

   header.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
   header.MajorLinkerVersion = 1;                                              // not used
   header.MinorLinkerVersion = 0;
   header.SizeOfCode = getSize(info.image->getTextSection());
   header.SizeOfInitializedData = getSize(info.image->getRDataSection()) + getSize(info.image->getImportSection())/* + getSize(image.getTLSSection())*/;
   header.SizeOfUninitializedData = getSize(info.image->getBSSSection()) + getSize(info.image->getStatSection());

   header.AddressOfEntryPoint = info.map.code + info.entryPoint;
   header.BaseOfCode = info.map.code;
   header.BaseOfData = info.map.rdata;

   header.ImageBase = info.project->IntSetting(opImageBase, IMAGE_BASE);
   header.SectionAlignment = info.project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);
   header.FileAlignment = info.project->IntSetting(opFileAlignment, FILE_ALIGNMENT);

   header.MajorOperatingSystemVersion = MAJOR_OS;
   header.MinorOperatingSystemVersion = MINOR_OS;
   header.MajorImageVersion = 0;                                               // not used
   header.MinorImageVersion = 0;
   header.MajorSubsystemVersion = MAJOR_OS;                                    // set for Win 4.0
   header.MinorSubsystemVersion = MINOR_OS;
   #ifndef mingw49
   header.Win32VersionValue = 0;                                               // ??
   #endif

   header.SizeOfImage = info.imageSize;
   header.SizeOfHeaders = info.headerSize;
   header.CheckSum = 0;                                                        // For EXE file
   switch (info.project->IntSetting(opPlatform, ptWin32Console) & mtUIMask)
   {
      case mtGUI:
         header.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
         break;
      case mtCUI:
      default:
         header.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
         break;
   }

   header.DllCharacteristics = 0;                                              // For EXE file
   header.LoaderFlags = 0;                                                     // not used

   header.SizeOfStackReserve = info.project->IntSetting(opSizeOfStackReserv, 0x100000); // !! explicit constant name
   header.SizeOfStackCommit = info.project->IntSetting(opSizeOfStackCommit, 0x1000);    // !! explicit constant name
   header.SizeOfHeapReserve = info.project->IntSetting(opSizeOfHeapReserv, 0x100000);   // !! explicit constant name
   header.SizeOfHeapCommit = info.project->IntSetting(opSizeOfHeapCommit, 0x10000);     // !! explicit constant name

   header.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
   for (unsigned long i = 0 ; i < header.NumberOfRvaAndSizes ; i++) {
      header.DataDirectory[i].VirtualAddress = 0;
      header.DataDirectory[i].Size = 0;
   }
   //if (_sections.exist(IMPORT_SECTION)) { // !! temporal
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = info.map.import;
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = getSize(info.image->getImportSection());
   //}
   
   // IMAGE_DIRECTORY_ENTRY_TLS
   if (tls_directory != 0xFFFFFFFF) {
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = info.map.rdata + tls_directory;
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = getSize(info.image->getTLSSection());
   }
   file->write((char*)&header, IMAGE_SIZEOF_NT_OPTIONAL_HEADER);
}

void Linker :: writeSectionHeader(FileWriter* file, const char* name, Section* section, int& tblOffset, int alignment, int sectionAlignment,
                                  int vaddress, int characteristics)
{
   IMAGE_SECTION_HEADER header;

   strncpy((char*)header.Name, name, 8);
   header.Misc.VirtualSize = align(section->Length(), sectionAlignment);
   header.SizeOfRawData = align(section->Length(), alignment);
   header.PointerToRawData = tblOffset;

   header.PointerToRelocations = 0;
   header.PointerToLinenumbers = 0;
   header.NumberOfRelocations = 0;
   header.NumberOfLinenumbers = 0;

   header.VirtualAddress = vaddress;
   header.Characteristics = characteristics;

   file->write((char*)&header, IMAGE_SIZEOF_SECTION_HEADER);

   tblOffset += header.SizeOfRawData;
}

void Linker :: writeBSSSectionHeader(FileWriter* file, const char* name, size_t size, int sectionAlignment,
                                  int vaddress, int characteristics)
{
   IMAGE_SECTION_HEADER header;

   strncpy((char*)header.Name, name, 8);
   header.Misc.VirtualSize = align(size, sectionAlignment);
   header.SizeOfRawData = 0;
   header.PointerToRawData = 0;

   header.PointerToRelocations = 0;
   header.PointerToLinenumbers = 0;
   header.NumberOfRelocations = 0;
   header.NumberOfLinenumbers = 0;

   header.VirtualAddress = vaddress;
   header.Characteristics = characteristics;

   file->write((char*)&header, IMAGE_SIZEOF_SECTION_HEADER);
}

void Linker :: writeSection(FileWriter* file, Section* section, int alignment)
{
   MemoryReader reader(section);
   file->read(&reader, section->Length());
   file->align(alignment);
}

void Linker :: writeSections(ImageInfo& info, FileWriter* file)
{
   int tblOffset = info.headerSize;
   int alignment = info.project->IntSetting(opFileAlignment, FILE_ALIGNMENT);
   int sectionAlignment = info.project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   // text section header
   writeSectionHeader(file, TEXT_SECTION, info.image->getTextSection(), tblOffset, alignment, sectionAlignment,
      info.map.code, IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ);

   // rdata section header
   int rdataSize = getSize(info.image->getRDataSection());
   if (rdataSize > 0) {
      writeSectionHeader(file, RDATA_SECTION, info.image->getRDataSection(), tblOffset, alignment, sectionAlignment,
         info.map.rdata, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);
   }

   // bss section header
   int bssSize = getSize(info.image->getBSSSection());
   if (bssSize > 0) {
      writeBSSSectionHeader(file, BSS_SECTION, bssSize, sectionAlignment,
         info.map.bss, IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   // stat section header
   int statSize = getSize(info.image->getStatSection());
   if (statSize > 0) {
      writeBSSSectionHeader(file, DATA_SECTION, statSize, sectionAlignment,
         info.map.stat, IMAGE_SCN_CNT_UNINITIALIZED_DATA |IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   // tls section header
   int tlsSize = getSize(info.image->getTLSSection());
   if (tlsSize > 0) {
      writeSectionHeader(file, TLS_SECTION, info.image->getTLSSection(), tblOffset, alignment, sectionAlignment,
         info.map.tls, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   // import section header
   writeSectionHeader(file, IMPORT_SECTION, info.image->getImportSection(), tblOffset, alignment, sectionAlignment,
      info.map.import, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);

   // debug section header 
   int debugSize = getSize(info.image->getDebugSection());
   if (debugSize > 0) {
      writeSectionHeader(file, DEBUG_SECTION, info.image->getDebugSection(), tblOffset, alignment, sectionAlignment,
         info.map.debug, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);
   }

   file->align(alignment);

   // text section
   writeSection(file, info.image->getTextSection(), alignment);

   // rdata section
   if (rdataSize > 0)
      writeSection(file, info.image->getRDataSection(), alignment);

   // tls section
   if (tlsSize > 0)
      writeSection(file, info.image->getTLSSection(), alignment);

   // import section
   writeSection(file, info.image->getImportSection(), alignment);

   // debug section
   if (debugSize > 0) {
      writeSection(file, info.image->getDebugSection(), alignment);
   }
}

bool Linker :: createExecutable(ImageInfo& info, path_t exePath, ref_t tls_directory)
{
   // create a full path (including none existing directories)
   Path dirPath;
   dirPath.copySubPath(exePath);
   Path::create(NULL, exePath);

   FileWriter executable(exePath, feRaw, false);

   if (!executable.isOpened())
      return false;

   writeDOSStub(info.project, &executable);

   executable.writeDWord((int)IMAGE_NT_SIGNATURE);

   int sectionCount = countSections(info.image);

   info.headerSize = executable.Length();
   info.headerSize += IMAGE_SIZEOF_FILE_HEADER + IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

   info.headerSize += IMAGE_SIZEOF_SECTION_HEADER * sectionCount;
   info.headerSize = align(info.headerSize, info.project->IntSetting(opFileAlignment, FILE_ALIGNMENT));

   writeHeader(&executable, IMAGE_FILE_EXECUTABLE_IMAGE, sectionCount);
   writeNTHeader(info, &executable, tls_directory);
   writeSections(info, &executable);

   return true;
}

void Linker :: run(Project& project, Image& image, ref_t tls_directory)
{
   ImageInfo info(&project, &image);

   info.entryPoint = image.getEntryPoint();

   createImportTable(info);
   mapImage(info);
   fixImage(info);

   Path path;
   Path::loadPath(path, project.StrSetting(opTarget));

   if (emptystr(path))
      throw InternalError(errEmptyTarget);

   if (!createExecutable(info, path, tls_directory))
      project.raiseError(errCannotCreate, project.StrSetting(opTarget));
}

void Linker :: prepareTLS(Image& image, int tls_variable, ref_t& tls_directory)
{
   // allocate tls section
   MemoryWriter tlsWriter(image.getTLSSection());
   tlsWriter.writeDWord(0);   // stack frame pointer
   tlsWriter.writeDWord(0);   // stack bottom pointer
   tlsWriter.writeDWord(0);   // catch address
   tlsWriter.writeDWord(0);   // catch stack level
   tlsWriter.writeDWord(0);   // catch stack frame
   tlsWriter.writeDWord(0);   // syncronization event
   tlsWriter.writeDWord(0);   // thread flags

   // map IMAGE_TLS_DIRECTORY
   MemoryWriter rdataWriter(image.getRDataSection());
   tls_directory = rdataWriter.Position();

   // create IMAGE_TLS_DIRECTORY
   int tlsLength = tlsWriter.Position();
   rdataWriter.writeRef((ref_t)mskTLSRef, 0);              // StartAddressOfRawData
   rdataWriter.writeRef((ref_t)mskTLSRef, tlsLength);      // EndAddressOfRawData
   rdataWriter.writeRef((ref_t)mskDataRef, tls_variable);  // AddressOfIndex
   rdataWriter.writeDWord(0);                       // AddressOfCallBacks
   rdataWriter.writeDWord(0);                       // SizeOfZeroFill
   rdataWriter.writeDWord(0);                       // Characteristics
}
