//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Linux
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFLINKER_H
#define ELFLINKER_H

#include "clicommon.h"

namespace elena_lang
{
   // --- ElfExecutableImage ---
   struct ElfExecutableImage
   {
      unsigned int    sectionAlignment;
      unsigned int    fileAlignment;
      unsigned int    flags;
      bool            withDebugInfo;

      AddressSpace    addressMap;

      ImageSections   imageSections;

      ElfExecutableImage(bool withDebugInfo)
         : imageSections(ImageSections())
      {
         this->fileAlignment = this->sectionAlignment = 0;
         this->flags = 0;
         this->withDebugInfo = withDebugInfo;
      }
   };

   // --- ElfLinker ---
   class ElfLinker : public LinkerBase
   {
   protected:
      ImageFormatter* _imageFormatter;

      virtual unsigned short calcPHLength(ElfExecutableImage& image);
      virtual unsigned int calcHeaderSize() = 0;

      virtual void writeELFHeader(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) = 0;
      virtual void writePHTable(ElfExecutableImage& image, FileWriter* file, unsigned short ph_num) = 0;
      virtual void writeInterpreter(FileWriter* file) = 0;

      void writeSection(FileWriter* file, Section* section);
      void writeSegments(ElfExecutableImage& image, FileWriter* file);

      bool createExecutable(ElfExecutableImage& image, path_t exePath);
      bool createDebugFile(ImageProviderBase& provider, ElfExecutableImage& image, path_t debugFilePath);

      virtual void prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize);

   public:
      void run(ProjectBase& project, ImageProviderBase& code) override;

      ElfLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : LinkerBase(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
