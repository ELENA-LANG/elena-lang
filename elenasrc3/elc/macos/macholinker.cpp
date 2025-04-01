//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Executive Linker class implementation
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "macholinker.h"

using namespace elena_lang;

bool MachOLinker :: createExecutable(ElfExecutableImage& image, path_t exePath)
{
   if (exePath.empty())
      _errorProcessor->raiseInternalError(errEmptyTarget);

   if (!PathUtil::recreatePath(/*nullptr, */exePath))
      return false;

   FileWriter executable(exePath, FileEncoding::Raw, false);
   if (!executable.isOpen())
      return false;

   writeMachOHeader(/*image, &executable, ph_length*/);

   return false; // !! temporal
}

LinkResult MachOLinker :: run(ProjectBase& project, ImageProviderBase& provider, PlatformType, path_t exeExtension)
{
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