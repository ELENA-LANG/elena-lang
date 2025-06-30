//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker.h"

using namespace elena_lang;

void MachOLinker :: writeSection(FileWriter* file, MemoryBase* section)
{
   if (section != nullptr) {
      MemoryReader reader(section);
      file->copyFrom(&reader, section->length());
   }
}


void MachOLinker :: writeSegments(MachOExecutableImage& image, FileWriter* file)
{
   for (auto it = image.imageSections.items.start(); !it.eof(); ++it) {
      writeSection(file, (*it).section);
      if ((*it).isAligned)
         file->align(image.fileAlignment);
   }
   file->align(image.fileAlignment);
}

bool MachOLinker :: createExecutable(MachOExecutableImage& image, path_t exePath)
{
   if (exePath.empty())
      _errorProcessor->raiseInternalError(errEmptyTarget);

   if (!PathUtil::recreatePath(/*nullptr, */exePath))
      return false;

   FileWriter executable(exePath, FileEncoding::Raw, false);
   if (!executable.isOpen())
      return false;

   writeMachOHeader(image, &executable/*, ph_length */);

   // write commands
   for (auto command_it = image.commands.start(); !command_it.eof(); ++command_it) {
      Command* command = *command_it;

      executable->write((char*)command, command->commandSize);
   }   

   // write sections
   writeSegments(image, file);

   return true;
}

void MachOLinker :: prepareCommands(MachOExecutableImage& image)
{
   pos_t fileOffset = 0;
   for (auto it = image.addressMap.sections.headers.start(); !it.eof(); ++it) {
      ImageSectionHeader header = *it;

      Command* command = createSegmentCommand(header, fileOffset);
      image.commands.add(command);

      image.totalCommandSize += command->commandSize;
   }
}

void MachOLinker :: prepareMachOImage(ImageProviderBase& provider, MachOExecutableImage& image)
{
   image.flags |= Flags_NoUndefs;
   //image.flags |= Flags_DyldLink;
   //image.flags |= Flags_TwoLevel;

   //NoUndefs, DyldLink, TwoLevel, PIE

   if (!image.sectionAlignment)
      image.sectionAlignment = SECTION_ALIGNMENT;

   if (!image.fileAlignment)
      image.fileAlignment = FILE_ALIGNMENT;

   image.addressMap.headerSize = SECTION_ALIGNMENT;

   _imageFormatter->prepareImage(provider, image.addressMap, image.imageSections,
      image.sectionAlignment,
      image.fileAlignment,
      image.withDebugInfo);

   prepareCommands(image);
}

LinkResult MachOLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType osType, PlatformType, path_t)
{
   MachOExecutableImage image(withDebugMode);

   prepareMachOImage(provider, image/*, calcHeaderSize()*/);

   PathString exePath(project.PathSetting(ProjectOption::TargetPath));
   exePath.changeExtension(exeExtension);

   if (!createExecutable(image, *exePath)) {
      _errorProcessor->raisePathError(errCannotCreate, project.PathSetting(ProjectOption::TargetPath));
   }

   //if (withDebugMode) {
   //   PathString debugFilePath(*exePath);
   //   debugFilePath.changeExtension("dn");

   //   if (!createDebugFile(provider, image, *debugFilePath)) {
   //      _errorProcessor->raisePathError(errCannotCreate, *debugFilePath);
   //   }
   //}

   // !! temporal stub
   return {};
}