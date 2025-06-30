//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: Linux
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elflinker.h"
#include "elfcommon.h"
#include "linux/lnxconsts.h"

#if defined __unix__

#include <elf.h>
#include <sys/stat.h>

#else

#include "elfdeclaration.h"

#endif

using namespace elena_lang;

// --- ElfLinker ---

unsigned short ElfLinker :: calcPHLength(ElfExecutableImage& image)
{
   unsigned short def_ph_count = image.withTLS ? 4 : 3;

   return image.imageSections.headers.count() + def_ph_count; // sections + HDR + interpreter + dynamic
}

void ElfLinker :: writeSection(FileWriter* file, MemoryBase* section)
{
   if (section != nullptr) {
      MemoryReader reader(section);
      file->copyFrom(&reader, section->length());
   }
}

void ElfLinker :: writeSegments(ElfExecutableImage& image, FileWriter* file)
{
   for (auto it = image.imageSections.items.start(); !it.eof(); ++it) {
      writeSection(file, (*it).section);
      if ((*it).isAligned)
         file->align(image.fileAlignment);
   }
   file->align(image.fileAlignment);
}

bool ElfLinker :: createExecutable(ElfExecutableImage& image, path_t exePath)
{
   if (exePath.empty())
      _errorProcessor->raiseInternalError(errEmptyTarget);

   if (!PathUtil::recreatePath(/*nullptr, */exePath))
      return false;

   FileWriter executable(exePath, FileEncoding::Raw, false);
   if (!executable.isOpen())
      return false;

   unsigned short ph_length = calcPHLength(image);
   writeELFHeader(image, &executable, ph_length);
   writePHTable(image, &executable, ph_length);
   writeInterpreter(&executable);

   if (image.addressMap.headerSize >= executable.position()) {
      executable.writeBytes(0, image.addressMap.headerSize - executable.position());
   }
   else _errorProcessor->raiseInternalError(errFatalLinker);

   writeSegments(image, &executable);

   return true;
}

bool ElfLinker :: createDebugFile(ImageProviderBase& provider, ElfExecutableImage& image, path_t debugFilePath)
{
   FileWriter debugWriter(debugFilePath, FileEncoding::Raw, false);
   if (!debugWriter.isOpen())
      return false;

   MemoryBase* debug = provider.getTargetDebugSection();

   // signature - first 8 bytes
   debugWriter.write(DEBUG_MODULE_SIGNATURE, getlength_pos(DEBUG_MODULE_SIGNATURE));
   debugWriter.align(8);

   // save entry point
   addr_t entryPoint = image.addressMap.code + image.addressMap.imageBase + provider.getDebugEntryPoint();

   debugWriter.writePos(debug->length());
   debugWriter.write(&entryPoint, sizeof(addr_t));

   // save breakpoints
   MemoryReader reader(debug);
   debugWriter.copyFrom(&reader, debug->length());

   return true;
}

void ElfLinker :: prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize)
{
   if (!image.sectionAlignment)
      image.sectionAlignment = SECTION_ALIGNMENT;

   image.fileAlignment = FILE_ALIGNMENT;

   image.addressMap.imageBase = IMAGE_BASE;
   image.addressMap.headerSize += align(headerSize, image.fileAlignment);

   _imageFormatter->prepareImage(provider, image.addressMap, image.imageSections,
      image.sectionAlignment,
      image.fileAlignment,
      image.withDebugInfo);

   if (image.addressMap.tls > 0) 
      image.withTLS = true;
}

LinkResult ElfLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType osType, PlatformType, path_t)
{
   bool withDebugMode = project.BoolSetting(ProjectOption::DebugMode, true);
   ElfExecutableImage image(withDebugMode, osType);

   image.addressMap.entryPoint = (pos_t)provider.getEntryPoint();
   prepareElfImage(provider, image, calcHeaderSize());

   PathString exePath(project.PathSetting(ProjectOption::TargetPath));

   if (!createExecutable(image, *exePath)) {
      _errorProcessor->raisePathError(errCannotCreate, project.PathSetting(ProjectOption::TargetPath));
   }

#if defined __unix__

   chmod(*exePath, S_IXOTH | S_IXUSR | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);

#endif

   if (withDebugMode) {
      PathString debugFilePath(*exePath);
      debugFilePath.changeExtension("dn");

      if (!createDebugFile(provider, image, *debugFilePath)) {
         _errorProcessor->raisePathError(errCannotCreate, *debugFilePath);
      }
   }

   return{
      image.addressMap.imageBase + image.addressMap.code,
      image.addressMap.imageBase + image.addressMap.rdata
   };
}
