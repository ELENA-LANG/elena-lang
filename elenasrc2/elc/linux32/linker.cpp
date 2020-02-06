//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: Linux32
//                                              (C)2015-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "linker.h"
#include "errors.h"

#include <elf.h>
#include <limits.h>
#include <sys/stat.h>

#define MAGIC_NUMBER "\x07F""ELF"

#define FILE_ALIGNMENT     0x0010
#define SECTION_ALIGNMENT  0x1000
#define IMAGE_BASE         0x08048000
#define HEADER_SIZE        0x0200

#define ELF_HEADER_SIZE    0x34
#define ELF_PH_SIZE        0x20

#define INTERPRETER_PATH   "/lib/ld-linux.so.2"

using namespace _ELENA_;

inline int getSize(Section* section)
{
   return section ? section->Length() : 0;
}

// --- Reallocate ---

ref_t reallocate(ref_t pos, ref_t key, ref_t disp, void* map)
{
   int base = ((ImageBaseMap*)map)->base + (key & ~mskAnyRef);

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
//      case mskTLSRef:
//         return ((ImageBaseMap*)map)->tls + base + disp;
      case mskImportRef:
      {
         int address = ((ImageBaseMap*)map)->base + ((ImageBaseMap*)map)->importMapping.get(key);

         return ((ImageBaseMap*)map)->import + address + disp;
      }
      case mskMetaRef:
         // HOTFIX : mskDebugRef is used for the message table reference
         switch (key) {
            case mskMessageTableRef:
               return ((ImageBaseMap*)map)->mdata + ((ImageBaseMap*)map)->base;
            case mskMetaAttributes:
               return ((ImageBaseMap*)map)->adata + ((ImageBaseMap*)map)->base;
         }
      default:
         return disp;
   }
}

ref_t reallocateImport(ref_t pos, ref_t key, ref_t disp, void* map)
{
   if ((key & mskImageMask)==mskImportRef) {
      int base = ((ImageBaseMap*)map)->base + ((ImageBaseMap*)map)->importMapping.get(key);

      return base + ((ImageBaseMap*)map)->import + disp;
   }
   else if ((key & mskImageMask)==mskRDataRef) {
      int base = ((ImageBaseMap*)map)->base + (key & ~mskAnyRef);

      return ((ImageBaseMap*)map)->rdata + base + disp;
   }
   else if ((key & mskImageMask)==mskCodeRef) {
      int base = ((ImageBaseMap*)map)->base + (key & ~mskAnyRef);

      return ((ImageBaseMap*)map)->code + base + disp;
   }
   else return disp;
}

// --- Linker ---

void Linker32 :: mapImage(ImageInfo& info)
{
   info.map.base = info.project->IntSetting(opImageBase, IMAGE_BASE);

   int alignment = info.project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   info.ph_length = 4; // header + text + rdata + data

   if (info.dynamic > 0) {
      info.ph_length += 2; // if import table is not empty, append interpreter / dynamic
   }

   // define section sizes
   int adataSize = align(getSize(info.image->getADataSection()), 4);
   int mdataSize = align(getSize(info.image->getMDataSection()), 4);
   int rdataSize = align(getSize(info.image->getRDataSection()), 4);
   int importSize = getSize(info.image->getImportSection());
   int statSize = align(getSize(info.image->getStatSection()), 4);
   int bssSize = align(getSize(info.image->getBSSSection()), 4);

   info.headerSize = align(HEADER_SIZE, FILE_ALIGNMENT);
   info.textSize = align(getSize(info.image->getTextSection()), FILE_ALIGNMENT);
   info.rdataSize = align(adataSize + mdataSize + rdataSize, FILE_ALIGNMENT);
   info.importSize = align(importSize, FILE_ALIGNMENT);
   info.bssSize = align(statSize + bssSize, FILE_ALIGNMENT);

   // text segment
   info.map.code = info.headerSize;               // code section should always be first

   // rodata segment
   info.map.adata = align(info.map.code + getSize(info.image->getTextSection()), alignment);
   // due to loader requirement, adjust offset
   info.map.adata += ((info.headerSize + info.textSize) & (alignment - 1));

   info.map.mdata = info.map.adata + adataSize;
   info.map.rdata = info.map.mdata + mdataSize;

   // data segment
   info.map.import = align(info.map.rdata + rdataSize, alignment);
   // due to loader requirement, adjust offset
   if (info.importSize != 0)
      info.map.import += ((info.headerSize + info.textSize + info.rdataSize) & (alignment - 1));

   info.map.stat = align(info.map.import + importSize, FILE_ALIGNMENT);
   info.map.bss = info.map.stat + statSize;

/*
   info.map.tls = align(info.map.stat + getSize(info.image->getStatSection()), alignment);
   info.imageSize = align(info.map.debug + getSize(info.image->getDebugSection()), alignment);
*/
}

int Linker32 :: fillImportTable(ImageInfo& info)
{
   int count = 0;

   ReferenceMap::Iterator it = info.image->getExternalIt();
   while (!it.Eof()) {
      String<char, PATH_MAX> external(it.key());

      int dotPos = ident_t(external).findLast('.') + 1;

      ident_t function = external + dotPos;

      IdentifierString dll(external + getlength(DLL_NAMESPACE) + 1, getlength(external) - getlength(DLL_NAMESPACE) - getlength(function) - 2);
      if (dll.ident().compare(RTDLL_FORWARD)) {
         dll.copy(info.project->resolvePrimitive(RTDLL_FORWARD));
      }

      info.functions.add(function.clone(), *it);
      if (!retrieve(info.libraries.start(), dll.ident(), (char*)NULL)) {
          info.libraries.add(dll.clone());
      }

      it++;
      count++;
   }
   return count;
}

void Linker32 :: createImportData(ImageInfo& info)
{
   int adataSize = align(getSize(info.image->getADataSection()), 4);
   int mdataSize = align(getSize(info.image->getMDataSection()), 4);

   size_t count = fillImportTable(info);
   if (count == 0)
      return;

   Section* import = info.image->getImportSection();

   // dynamic table
   MemoryWriter dynamicWriter(info.image->getRDataSection());
   dynamicWriter.align(FILE_ALIGNMENT, 0);

   // HOTFIX : rdata is located after adata and mdta
   info.dynamic = dynamicWriter.Position() + adataSize + mdataSize;

   // reference to GOT
   ref_t importRef = (count + 1) | mskImportRef;
   info.map.importMapping.add(importRef, 0);

   // reserve got table
   MemoryWriter gotWriter(import);
   gotWriter.writeRef(mskRDataRef, info.dynamic);
   gotWriter.writeDWord(0);
   gotWriter.writeDWord(0);
   size_t gotStart = gotWriter.Position();
   gotWriter.writeBytes(0, count * 4);
   gotWriter.seek(gotStart);

   // reserve relocation table
   MemoryWriter reltabWriter(import);
   size_t reltabOffset = reltabWriter.Position();
   reltabWriter.writeBytes(0, count * 8);
   reltabWriter.seek(reltabOffset);

   // reserve symbol table
   MemoryWriter symtabWriter(import);
   size_t symtabOffset = symtabWriter.Position();
   symtabWriter.writeBytes(0, (count + 1) * 16);
   symtabWriter.seek(symtabOffset + 16);

   // string table
   MemoryWriter strWriter(import);
   int strOffset = strWriter.Position();
   strWriter.writeChar('\0');

   // code writer
   MemoryWriter codeWriter(info.image->getTextSection());
   writePLTStartEntry(codeWriter, importRef);

   ImportReferences::Iterator fun = info.functions.start();
   int symbolIndex = 1;
   int pltIndex = 1;
   while (!fun.Eof()) {
      int gotPosition = gotWriter.Position();

      // map import reference
      info.map.importMapping.add(*fun, gotWriter.Position());

      int strIndex = strWriter.Position() - strOffset;

      // symbol table entry
      symtabWriter.writeDWord(strIndex);
      symtabWriter.writeDWord(0);
      symtabWriter.writeDWord(0);
      symtabWriter.writeDWord(0x12);

      // relocation table entry
      size_t relPosition = reltabWriter.Position() - reltabOffset;
      reltabWriter.writeRef(importRef, gotPosition);
      reltabWriter.writeDWord((symbolIndex << 8) + R_386_JMP_SLOT);

      // string table entry
      strWriter.writeLiteral(fun.key());

      // got / plt entry
      ref_t position = writePLTEntry(codeWriter, relPosition, importRef, gotPosition, pltIndex);
      gotWriter.writeRef(mskCodeRef, position);

      fun++;
      symbolIndex++;
      pltIndex++;
   }

   // write dynamic segment

   // write libraries needed to be loaded
   List<char*>::Iterator dll = info.libraries.start();
   while (!dll.Eof()) {
      dynamicWriter.writeDWord(DT_NEEDED);
      dynamicWriter.writeDWord(strWriter.Position() - strOffset);

      strWriter.writeLiteral(*dll);

      dll++;
   }
   strWriter.writeChar('\0');
   int strLength = strWriter.Position() - strOffset;

   dynamicWriter.writeDWord(DT_STRTAB);
   dynamicWriter.writeRef(importRef, strOffset);

   dynamicWriter.writeDWord(DT_SYMTAB);
   dynamicWriter.writeRef(importRef, symtabOffset);

   dynamicWriter.writeDWord(DT_STRSZ);
   dynamicWriter.writeDWord(strLength);

   dynamicWriter.writeDWord(DT_SYMENT);
   dynamicWriter.writeDWord(0x10);

   dynamicWriter.writeDWord(DT_PLTGOT);
   dynamicWriter.writeRef(importRef, 0);

   dynamicWriter.writeDWord(DT_PLTRELSZ);
   dynamicWriter.writeDWord(count * 8);

   dynamicWriter.writeDWord(DT_PLTREL);
   dynamicWriter.writeDWord(DT_REL);

   dynamicWriter.writeDWord(DT_JMPREL);
   dynamicWriter.writeRef(importRef, reltabOffset);

   dynamicWriter.writeDWord(DT_REL);
   dynamicWriter.writeRef(importRef, reltabOffset);

   dynamicWriter.writeDWord(DT_RELSZ);
   dynamicWriter.writeDWord(count * 8);

   dynamicWriter.writeDWord(DT_RELENT);
   dynamicWriter.writeDWord(8);

   dynamicWriter.writeDWord(0);
   dynamicWriter.writeDWord(0);

   // write interpreter path
   dynamicWriter.align(FILE_ALIGNMENT, 0);

   // HOTFIX : rdata is located after adata and mdta
   info.interpreter = dynamicWriter.Position() + adataSize + mdataSize;
   dynamicWriter.writeLiteral(INTERPRETER_PATH, getlength(INTERPRETER_PATH) + 1);
}

void Linker32 :: fixImage(ImageInfo& info)
{
   Section* text = info.image->getTextSection();
   Section* rdata = info.image->getRDataSection();
   Section* adata = info.image->getADataSection();
   Section* mdata = info.image->getMDataSection();
   Section* bss = info.image->getBSSSection();
   Section* stat = info.image->getStatSection();
   Section* import = info.image->getImportSection();
//   Section* tls = info.image->getTLSSection();

  // fix up text reallocate
   text->fixupReferences(&info.map, reallocate);

  // fix up rdata section
   rdata->fixupReferences(&info.map, reallocate);

   // fix up mdata section
   mdata->fixupReferences(&info.map, reallocate);

   // fix up adata section
   adata->fixupReferences(&info.map, reallocate);

  // fix up bss section
   bss->fixupReferences(&info.map, reallocate);

  // fix up stat section
   stat->fixupReferences(&info.map, reallocate);

//  // fix up tls section
//   tls->fixupReferences(&info.map, reallocate);

  // fix up import section
   import->fixupReferences(&info.map, reallocateImport);

  // fix up debug info if enabled
   if (info.withDebugInfo) {
      Section* debug = info.image->getDebugSection();

      debug->fixupReferences(&info.map, reallocate);
   }
}

void Linker32 :: writeSection(FileWriter* file, Section* section, int alignment)
{
   MemoryReader reader(section);
   file->read(&reader, section->Length());
   file->align(alignment);
}

void Linker32 :: writeELFHeader(ImageInfo& info, FileWriter* file)
{
   Elf32_Ehdr header;

   // e_ident
   memset(header.e_ident, 0, EI_NIDENT);
   memcpy(header.e_ident, MAGIC_NUMBER, 4);
   header.e_ident[EI_CLASS] = ELFCLASS32;
   header.e_ident[EI_DATA]  = ELFDATA2LSB;
   header.e_ident[EI_VERSION]  = EV_CURRENT;

   header.e_type = ET_EXEC;
   header.e_machine = EM_386;
   header.e_version = EV_CURRENT;
   header.e_entry = info.map.base + info.map.code + info.entryPoint;
   header.e_phoff = ELF_HEADER_SIZE;
   header.e_shoff = 0;
   header.e_flags = 0;
   header.e_ehsize = 0;
   header.e_phentsize = ELF_PH_SIZE;
   header.e_phnum = info.ph_length;
   header.e_shentsize = 0x28;
   header.e_shnum = 0;
   header.e_shstrndx = SHN_UNDEF;

   file->write((char*)&header, ELF_HEADER_SIZE);
}

void Linker32 :: writePHTable(ImageInfo& info, FileWriter* file)
{
   int alignment = info.project->IntSetting(opSectionAlignment, SECTION_ALIGNMENT);

   Elf32_Phdr ph_header;

   // Program header
   ph_header.p_type = PT_PHDR;
   ph_header.p_offset = ELF_HEADER_SIZE;
   ph_header.p_vaddr = info.map.base + ELF_HEADER_SIZE;
   ph_header.p_paddr = info.map.base + ELF_HEADER_SIZE;
   ph_header.p_filesz = info.ph_length * ELF_PH_SIZE;
   ph_header.p_memsz = info.ph_length * ELF_PH_SIZE;
   ph_header.p_flags = PF_R;
   ph_header.p_align = 4;
   file->write((char*)&ph_header, ELF_PH_SIZE);

   if (info.interpreter > 0) {
      // Interpreter
      ph_header.p_type = PT_INTERP;
      ph_header.p_offset = info.textSize + info.headerSize + info.interpreter;
      ph_header.p_paddr = ph_header.p_vaddr = info.map.base + info.map.rdata + info.interpreter;
      ph_header.p_memsz = ph_header.p_filesz = getlength(INTERPRETER_PATH) + 1;
      ph_header.p_flags = PF_R;
      ph_header.p_align = 1;
      file->write((char*)&ph_header, ELF_PH_SIZE);
   }

   // Text Segment
   ph_header.p_type = PT_LOAD;
   ph_header.p_offset = 0;
   ph_header.p_vaddr = info.map.base;
   ph_header.p_paddr = info.map.base;
   ph_header.p_memsz = ph_header.p_filesz = info.headerSize + info.textSize;
   ph_header.p_flags = PF_R + PF_X;
   ph_header.p_align = alignment;
   file->write((char*)&ph_header, ELF_PH_SIZE);

   // RData Segment
   ph_header.p_type = PT_LOAD;
   ph_header.p_offset = info.headerSize + info.textSize;
   ph_header.p_vaddr = info.map.base + info.map.adata;
   ph_header.p_paddr = info.map.base + info.map.adata;
   ph_header.p_memsz = ph_header.p_filesz = info.rdataSize;
   ph_header.p_flags = PF_R;
   ph_header.p_align = alignment;
   file->write((char*)&ph_header, ELF_PH_SIZE);

   // Data Segment
   ph_header.p_type = PT_LOAD;
   if (info.importSize != 0) {
      ph_header.p_offset = info.headerSize + info.textSize + info.rdataSize;
   }
   else ph_header.p_offset = 0;
   ph_header.p_paddr = ph_header.p_vaddr = info.map.base + info.map.import;
   ph_header.p_memsz = info.importSize + info.bssSize;
   ph_header.p_filesz = info.importSize;
   ph_header.p_flags = PF_R + PF_W;
   ph_header.p_align = alignment;
   file->write((char*)&ph_header, ELF_PH_SIZE);

  if (info.dynamic > 0) {
      // Dynamic
      ph_header.p_type = PT_DYNAMIC;
      ph_header.p_offset = info.headerSize + info.textSize + info.dynamic;
      ph_header.p_paddr = ph_header.p_vaddr = info.map.base + info.map.rdata + info.dynamic;
      ph_header.p_filesz = ph_header.p_memsz = info.rdataSize - info.dynamic;
      ph_header.p_flags = PF_R;
      ph_header.p_align = 8;
      file->write((char*)&ph_header, ELF_PH_SIZE);
   }
}


void Linker32 :: writeSegments(ImageInfo& info, FileWriter* file)
{
   // text section
   writeSection(file, info.image->getTextSection(), FILE_ALIGNMENT);

   // rodata section
   writeSection(file, info.image->getADataSection(), 4);
   writeSection(file, info.image->getMDataSection(), 4);
   writeSection(file, info.image->getRDataSection(), FILE_ALIGNMENT);

   // import section
   writeSection(file, info.image->getImportSection(), FILE_ALIGNMENT);
}

bool Linker32 :: createExecutable(ImageInfo& info, const char* exePath/*, ref_t tls_directory*/)
{
   // create a full path (including none existing directories)
   Path dirPath;
   dirPath.copySubPath(exePath);
   Path::create(NULL, exePath);

   FileWriter executable(exePath, feRaw, false);

   if (!executable.isOpened())
      return false;

   writeELFHeader(info, &executable);
   writePHTable(info, &executable);

    int p = executable.Position();

   if (info.headerSize >= executable.Position()) {
      executable.writeBytes(0, info.headerSize - executable.Position());
   }
   else throw InternalError(errFatalLinker);

   writeSegments(info, &executable);

   return true;
}

bool Linker32 :: createDebugFile(ImageInfo& info, const char* debugFilePath)
{
   FileWriter	debugFile(debugFilePath, feRaw, false);

   if (!debugFile.isOpened())
      return false;

   Section* debugInfo = info.image->getDebugSection();

   // signature
   debugFile.write(DEBUG_MODULE_SIGNATURE, strlen(DEBUG_MODULE_SIGNATURE));

   // save entry point
   ref_t imageBase = info.project->IntSetting(opImageBase, IMAGE_BASE);
   ref_t entryPoint = info.map.code + info.map.base + info.image->getDebugEntryPoint();

   debugFile.writeDWord(debugInfo->Length());
   debugFile.writeDWord(entryPoint);

   // save DebugInfo
   MemoryReader reader(debugInfo);
   debugFile.read(&reader, debugInfo->Length());

   return true;
}

void Linker32 :: run(Project& project, Image& image/*, ref_t tls_directory*/)
{
   ImageInfo info(&project, &image);

   info.entryPoint = image.getEntryPoint();

   createImportData(info);
   mapImage(info);
   fixImage(info);

   Path path(project.StrSetting(opTarget));

   if (emptystr(path))
      throw InternalError(errEmptyTarget);

   if (!createExecutable(info, path/*, tls_directory*/))
      project.raiseError(errCannotCreate, path.c_str());

   chmod(path, S_IXOTH | S_IXUSR | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);


   if (info.withDebugInfo) {
      Path debugPath(path);
      debugPath.changeExtension("dn");

      if (!createDebugFile(info, debugPath.c_str())) {
         ident_t target = project.StrSetting(opTarget);

         IdentifierString fileNameArg(target);
         fileNameArg.truncate(target.findLast('.', fileNameArg.Length()));
         fileNameArg.append(".dn");

         project.raiseError(errCannotCreate, fileNameArg.c_str());
      }
   }
}

// --- I386Linker32 ---

void I386Linker32 :: writePLTStartEntry(MemoryWriter& codeWriter, ref_t gotReference)
{
   codeWriter.writeWord(0x35FF);
   codeWriter.writeRef(gotReference, 4);
   codeWriter.writeWord(0x25FF);
   codeWriter.writeRef(gotReference, 8);
   codeWriter.writeDWord(0);
}

size_t I386Linker32 :: writePLTEntry(MemoryWriter& codeWriter, int symbolIndex, ref_t gotReference, int gotOffset, int entryIndex)
{
   codeWriter.writeWord(0x25FF);
   codeWriter.writeRef(gotReference, gotOffset);

   size_t position = codeWriter.Position();

   codeWriter.writeByte(0x68);
   codeWriter.writeDWord(symbolIndex);
   codeWriter.writeByte(0xE9);
   codeWriter.writeDWord(-0x10-(entryIndex * 0x10));

   return position;
}
