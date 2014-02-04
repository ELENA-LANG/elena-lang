//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: Win32
//                                              (C)2005-2012, by Alexei Rakov
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
//#define DEBUG_SECTION      ".debug"

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
      default:
         return disp;
   }
}

ref_t reallocateImport(ref_t pos, ref_t key, ref_t disp, void* map)
{
   if ((key & mskImageMask)==mskImportRef) {
      return ((ImageBaseMap*)map)->import + disp + ((ImageBaseMap*)map)->importMapping.get(key);
   }
   else return disp;
}

// --- Linker ---

void Linker :: mapImage(Image& image)
{
   _map.base = _project->IntSetting(opImageBase, IMAGE_BASE);

   int alignment = _project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   _map.code = 0x1000;               // code section should always be first
   _map.rdata = align(_map.code + getSize(image.getTextSection()), alignment);
   _map.bss = align(_map.rdata + getSize(image.getRDataSection()), alignment);
   _map.stat = align(_map.bss + getSize(image.getBSSSection()), alignment);
   _map.tls = align(_map.stat + getSize(image.getStatSection()), alignment);
   _map.import = align(_map.tls + getSize(image.getTLSSection()), alignment);

   _imageSize = align(_map.import + getSize(image.getImportSection()), alignment);
}

int Linker :: fillImportTable(Image& image)
{
   int count = 0;

   ReferenceMap::Iterator it = image.getExternalIt();
   while (!it.Eof()) {
      String<wchar_t, MAX_PATH> external(it.key());

      const wchar16_t* function = external + external.findLast('.') + 1;
      Path dll(external + getlength(DLL_NAMESPACE) + 1, function - (external + getlength(DLL_NAMESPACE)) - 2);

      ReferenceMap* functions = _importTable.get(dll);
      if (functions==NULL) {
         functions = new ReferenceMap(0);

         _importTable.add(dll, functions);
      }
      functions->add(function, *it);

      it++;
      count++;
   }
   return count;
}

void Linker :: createImportTable(Image& image)
{
   ConstantIdentifier dllExt("dll");

   size_t count = fillImportTable(image);
   Section* import = image.getImportSection();

   MemoryWriter writer(import);

   // reference to the import section
   ref_t importRef = (count + 1) | mskImportRef;
   _map.importMapping.add(importRef, 0);

   MemoryWriter tableWriter(import);
   writer.writeBytes(0, (count + 1) * 20);               // fill import table
   MemoryWriter fwdWriter(import);
   writer.writeBytes(0, (_importTable.Count()+count)*4);  // fill forward table
   MemoryWriter lstWriter(import);
   writer.writeBytes(0, (_importTable.Count()+count)*4);  // fill import list

   ImportTable::Iterator dll = _importTable.start();
   while (!dll.Eof()) {
      tableWriter.writeRef(importRef, lstWriter.Position());              // OriginalFirstThunk
      tableWriter.writeDWord((int)time(NULL));                            // TimeDateStamp
      tableWriter.writeDWord(-1);                                         // ForwarderChain
      tableWriter.writeRef(importRef, import->Length());                  // Name
      const wchar16_t* dllname = dll.key();
      if (!Path::checkExtension(dllname, dllExt)) {
         writer.writeAsciiLiteral(dllname, getlength(dllname));
         writer.writeChar('.');
         writer.writeAsciiLiteral(dllExt);
      }
      else writer.writeAsciiLiteral(dllname);
      tableWriter.writeRef(importRef, fwdWriter.Position());              // ForwarderChain

      // fill OriginalFirstThunk & ForwarderChain
      ReferenceMap::Iterator fun = (*dll)->start();
      while (!fun.Eof()) {
         _map.importMapping.add(*fun, fwdWriter.Position());

         fwdWriter.writeRef(importRef, import->Length());
         lstWriter.writeRef(importRef, import->Length());

         writer.writeWord(1);                                             // Hint (not used)
         writer.writeAsciiLiteral(fun.key());

         fun++;
      }
      lstWriter.writeDWord(0);                                            // mark end of chains
      fwdWriter.writeDWord(0);

      dll++;
   }
}

void Linker :: fixImage(Image& image)
{
   Section* text = image.getTextSection();
   Section* rdata = image.getRDataSection();
   Section* bss = image.getBSSSection();
   Section* stat = image.getStatSection();
   Section* tls = image.getTLSSection();
   Section* import = image.getImportSection();

  // fix up text section
   text->fixupReferences(&_map, reallocate);

  // fix up rdata section
   rdata->fixupReferences(&_map, reallocate);

  // fix up bss section
   bss->fixupReferences(&_map, reallocate);

  // fix up stat section
   stat->fixupReferences(&_map, reallocate);

  // fix up tls section
   tls->fixupReferences(&_map, reallocate);

  // fix up import section
   import->fixupReferences(&_map, reallocateImport);

  // fix up debug info if enabled
   if (_withDebugInfo) {
      Section* debug = image.getDebugSection();

      debug->fixupReferences(&_map, reallocate);
   }
}

int Linker :: countSections(Image& image)
{
   int count = 0;

   if (getSize(image.getTextSection()))
      count++;

   if (getSize(image.getRDataSection()))
      count++;

   if (getSize(image.getBSSSection()))
      count++;

   if (getSize(image.getStatSection()))
      count++;

   if (getSize(image.getTLSSection()))
      count++;

   if (getSize(image.getImportSection()))
      count++;

   return count;
}

void Linker :: writeDOSStub(FileWriter* file)
{
   Path stubPath(_project->StrSetting(opAppPath), "winstub.ex_");
   FileReader stub(stubPath, L"rb", feRaw, false);

   if (stub.isOpened()) {
      file->read(&stub, stub.Length());
   }
   else _project->raiseError(errInvalidFile, (const wchar16_t*)stubPath);
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

void Linker :: writeNTHeader(Image& image, FileWriter* file, ref_t tls_directory)
{
   IMAGE_OPTIONAL_HEADER   header;

   header.Magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
   header.MajorLinkerVersion = 1;                                              // not used
   header.MinorLinkerVersion = 0;
   header.SizeOfCode = getSize(image.getTextSection());
   header.SizeOfInitializedData = getSize(image.getRDataSection()) + getSize(image.getImportSection())/* + getSize(image.getTLSSection())*/;
   header.SizeOfUninitializedData = getSize(image.getBSSSection()) + getSize(image.getStatSection());

   header.AddressOfEntryPoint = _map.code + _entryPoint;
   header.BaseOfCode = _map.code;
   header.BaseOfData = _map.rdata;

   header.ImageBase = _project->IntSetting(opImageBase, IMAGE_BASE);
   header.SectionAlignment = _project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);
   header.FileAlignment = _project->IntSetting(opFileAlignment, FILE_ALIGNMENT);

   header.MajorOperatingSystemVersion = MAJOR_OS;
   header.MinorOperatingSystemVersion = MINOR_OS;
   header.MajorImageVersion = 0;                                               // not used
   header.MinorImageVersion = 0;
   header.MajorSubsystemVersion = MAJOR_OS;                                    // set for Win 4.0
   header.MinorSubsystemVersion = MINOR_OS;
   #ifndef mingw49
   header.Win32VersionValue = 0;                                               // ??
   #endif

   header.SizeOfImage = _imageSize;
   header.SizeOfHeaders = _headerSize;
   header.CheckSum = 0;                                                        // For EXE file
   switch (_project->IntSetting(opApplicationType, stConsole))
   {
      case stGUI:
         header.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
         break;
      case stConsole:
      default:
         header.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
         break;
   }

   header.DllCharacteristics = 0;                                              // For EXE file
   header.LoaderFlags = 0;                                                     // not used

   header.SizeOfStackReserve = _project->IntSetting(opSizeOfStackReserv, 0x100000); // !! explicit constant name
   header.SizeOfStackCommit = _project->IntSetting(opSizeOfStackCommit, 0x1000);    // !! explicit constant name
   header.SizeOfHeapReserve = _project->IntSetting(opSizeOfHeapReserv, 0x100000);   // !! explicit constant name
   header.SizeOfHeapCommit = _project->IntSetting(opSizeOfHeapCommit, 0x10000);     // !! explicit constant name

   header.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
   for (unsigned long i = 0 ; i < header.NumberOfRvaAndSizes ; i++) {
      header.DataDirectory[i].VirtualAddress = 0;
      header.DataDirectory[i].Size = 0;
   }
   //if (_sections.exist(IMPORT_SECTION)) { // !! temporal
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = _map.import;
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = getSize(image.getImportSection());
   //}
   
   // IMAGE_DIRECTORY_ENTRY_TLS
   if (tls_directory != 0xFFFFFFFF) {
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = _map.rdata + tls_directory;
      header.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = getSize(image.getTLSSection());
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

void Linker :: writeSections(Image& image, FileWriter* file)
{
   int tblOffset = _headerSize;
   int alignment = _project->IntSetting(opFileAlignment, FILE_ALIGNMENT);
   int sectionAlignment = _project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   writeSectionHeader(file, TEXT_SECTION, image.getTextSection(), tblOffset, alignment, sectionAlignment,
      _map.code, IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ);

   int rdataSize = getSize(image.getRDataSection());
   if (rdataSize > 0) {
      writeSectionHeader(file, RDATA_SECTION, image.getRDataSection(), tblOffset, alignment, sectionAlignment,
         _map.rdata, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ);
   }
   int bssSize = getSize(image.getBSSSection());
   if (bssSize > 0) {
      writeBSSSectionHeader(file, BSS_SECTION, bssSize, sectionAlignment,
         _map.bss, IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   int statSize = getSize(image.getStatSection());
   if (statSize > 0) {
      writeBSSSectionHeader(file, DATA_SECTION, statSize, sectionAlignment,
         _map.stat, IMAGE_SCN_CNT_UNINITIALIZED_DATA |IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   int tlsSize = getSize(image.getTLSSection());
   if (tlsSize > 0) {
      writeSectionHeader(file, TLS_SECTION, image.getTLSSection(), tblOffset, alignment, sectionAlignment,
         _map.tls, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);
   }

   writeSectionHeader(file, IMPORT_SECTION, image.getImportSection(), tblOffset, alignment, sectionAlignment,
      _map.import, IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE);

   file->align(alignment);

   writeSection(file, image.getTextSection(), alignment);

   if (rdataSize > 0)
      writeSection(file, image.getRDataSection(), alignment);

   if (tlsSize > 0)
      writeSection(file, image.getTLSSection(), alignment);

   writeSection(file, image.getImportSection(), alignment);
}

bool Linker :: createExecutable(Image& image, const _path_t* exePath, ref_t tls_directory)
{
   // create a full path (including none existing directories)
   Path dirPath;
   dirPath.copyPath(exePath);
   Path::create(NULL, exePath);

   FileWriter executable(exePath, feRaw, false);

   if (!executable.isOpened())
      return false;

   writeDOSStub(&executable);

   executable.writeDWord((int)IMAGE_NT_SIGNATURE);

   int sectionCount = countSections(image);

   _headerSize = executable.Length();
   _headerSize += IMAGE_SIZEOF_FILE_HEADER + IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

   _headerSize += IMAGE_SIZEOF_SECTION_HEADER * sectionCount;
   _headerSize = align(_headerSize, _project->IntSetting(opFileAlignment, FILE_ALIGNMENT));

   writeHeader(&executable, IMAGE_FILE_EXECUTABLE_IMAGE, sectionCount);
   writeNTHeader(image, &executable, tls_directory);
   writeSections(image, &executable);

   return true;
}

bool Linker :: createDebugFile(Image& image, const _path_t* debugFilePath)
{
   FileWriter	debugFile(debugFilePath, feRaw, false);

   if (!debugFile.isOpened())
      return false;

   Section*	debugInfo = image.getDebugSection();

   // signature
   debugFile.write(DEBUG_MODULE_SIGNATURE, strlen(DEBUG_MODULE_SIGNATURE));

   // save entry point
   ref_t imageBase = _project->IntSetting(opImageBase, IMAGE_BASE);
   ref_t entryPoint = _map.code + _map.base + image.getDebugEntryPoint();

   debugFile.writeDWord(entryPoint);

   // save DebugInfo
   MemoryReader reader(debugInfo);
   debugFile.read(&reader, debugInfo->Length());

   return true;
}

void Linker :: run(Image& image, ref_t tls_directory)
{
   _entryPoint = image.getEntryPoint();

   createImportTable(image);
   mapImage(image);
   fixImage(image);

   const wchar16_t* path = _project->StrSetting(opTarget);

   if (emptystr(path))
      _project->raiseError(errEmptyTarget);

   if (!createExecutable(image, path, tls_directory))
      _project->raiseError(errCannotCreate, path);

   if (_withDebugInfo) {
      Path debugPath(path);
      debugPath.changeExtension(ConstantIdentifier("dn"));

      if (!createDebugFile(image, debugPath))
         _project->raiseError(errCannotCreate, (const wchar16_t*)debugPath);
   }
}
