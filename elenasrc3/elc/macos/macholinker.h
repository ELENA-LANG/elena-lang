//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: MacOS
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOLINKER_H
#define MACHOLINKER_H

#include "clicommon.h"

namespace elena_lang
{
   // --- ElfLinker ---
   class MachOLinker : public LinkerBase
   {
   protected:
      virtual void writeMachOHeader() = 0;

      bool createExecutable(ElfExecutableImage& image, path_t exePath);

   public:
      LinkResult run(ProjectBase& project, ImageProviderBase& code, PlatformType uiType,
         path_t exeExtension) override;

      MachOLinker(ErrorProcessorBase* errorProcessor/*, ImageFormatter* imageFormatter*/)
         : LinkerBase(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
