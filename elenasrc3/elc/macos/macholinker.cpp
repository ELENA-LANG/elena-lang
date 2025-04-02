//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker.h"

using namespace elena_lang;

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

   for (auto command_it = image.commands.start(); !command_it.eof(); ++command_it) {
      writeCommand(image, &executable, *command);
   }   

   return false; // !! temporal
}

void MachOLinker :: prepareMachOImage(MachOExecutableImage& image)
{
   if (!image.sectionAlignment)
      image.sectionAlignment = SECTION_ALIGNMENT;

   if (!image.fileAlignment)
      image.fileAlignment = FILE_ALIGNMENT;

   _imageFormatter->prepareImage(provider, image.addressMap, image.imageSections,
      image.sectionAlignment,
      image.fileAlignment,
      image.withDebugInfo);
}

LinkResult MachOLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType, path_t exeExtension)
{
   MachOExecutableImage image(withDebugMode);

   prepareMachOImage(/*provider, */image/*, calcHeaderSize()*/);

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