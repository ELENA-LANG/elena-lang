//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: Linux
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elflinker.h"
#include "elfcommon.h"

#include <elf.h>
#include <sys/stat.h>

using namespace elena_lang;

// --- ElfLinker ---

unsigned short ElfLinker :: calcPHLength(ElfExecutableImage& image)
{
   return image.imageSections.headers.count() + 3; // sections + HDR + interpreter + dynamic
}

void ElfLinker :: writeSection(FileWriter* file, Section* section)
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

void ElfLinker :: prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize)
{
   if (!image.sectionAlignment)
      image.sectionAlignment = SECTION_ALIGNMENT;

   image.fileAlignment = FILE_ALIGNMENT;

   image.addressMap.imageBase = IMAGE_BASE;
   image.addressMap.headerSize += align(headerSize, image.fileAlignment);

   _imageFormatter->prepareImage(provider, image.addressMap, image.imageSections, image.sectionAlignment, image.fileAlignment);
}

void ElfLinker :: run(ProjectBase& project, ImageProviderBase& provider)
{
   ElfExecutableImage image;

   image.addressMap.entryPoint = (pos_t)provider.getEntryPoint();
   prepareElfImage(provider, image, calcHeaderSize());

   PathString exePath(project.PathSetting(ProjectOption::TargetPath));

   if (!createExecutable(image, *exePath)) {
      _errorProcessor->raisePathError(errCannotCreate, project.PathSetting(ProjectOption::TargetPath));
   }

   chmod(*exePath, S_IXOTH | S_IXUSR | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
}
