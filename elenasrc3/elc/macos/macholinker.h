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
#include "machocommon.h"

namespace elena_lang
{
   struct MachOExecutableImage
   {
      unsigned int    sectionAlignment;
      unsigned int    fileAlignment;
      bool            withDebugInfo;
      int             flags;

      AddressSpace    addressMap;

      ImageSections   imageSections;

      int             totalCommandSize;
      Commands        commands;

      MachOExecutableImage(bool withDebugInfo)
         : imageSections({}), commands(nullptr)
      {
         this->fileAlignment = this->sectionAlignment = 0;
         this->flags = 0;
         this->withDebugInfo = withDebugInfo;
         this->totalCommandSize = 0;
      }
   };

   // --- MachOLinker ---
   class MachOLinker : public LinkerBase
   {
   protected:
      ImageFormatter* _imageFormatter;

      virtual unsigned long getMagicNumber() = 0;

      virtual CPUType getCPUType() = 0;
      virtual CPUSubType getCPUSubType() = 0;

      virtual Command* createSegmentCommand(ImageSectionHeader& header, pos_t& fileOffset) = 0;

      virtual void prepareMachOImage(ImageProviderBase& provider, MachOExecutableImage& image);
      virtual void prepareCommands(MachOExecutableImage& image);

      virtual void writeMachOHeader(MachOExecutableImage& image, FileWriter* file) = 0;
      virtual void writeSegments(MachOExecutableImage& image, FileWriter* file);
      void writeSection(FileWriter* file, MemoryBase* section);

      bool createExecutable(MachOExecutableImage& image, path_t exePath);

   public:
      LinkResult run(ProjectBase& project, ImageProviderBase& code, PlatformType osType, PlatformType uiType,
         path_t exeExtension) override;

      MachOLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : LinkerBase(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
