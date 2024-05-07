//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker base class declaration
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef NTLINKER_H
#define NTLINKER_H

#include "clicommon.h"

namespace elena_lang
{

   struct WinNtExecutableImage
   {
      struct AllocationInfo
      {
         pos_t commit;
         pos_t reserve;

         AllocationInfo()
         {
            commit = reserve = 0;
         }
      };

      unsigned short  characteristics;

      unsigned short  machine;
      unsigned short  subsystem;

      pos_t           optionalHeaderSize;
      pos_t           headerSize;
      pos_t           imageSize;

      unsigned int    sectionAlignment;
      unsigned int    fileAlignment;
      bool            withDebugInfo;

      AllocationInfo  stackInfo;
      AllocationInfo  heapInfo;

      AddressSpace    addressSpace;

      ImageSections   imageSections;

      WinNtExecutableImage(bool withDebugInfo)
      {
         characteristics = 0;
         subsystem = machine = 0;
         headerSize = optionalHeaderSize = imageSize  = 0;
         sectionAlignment = fileAlignment = 0;

         this->withDebugInfo = withDebugInfo;
      }
   };

   class WinNtLinker : public LinkerBase
   {
   protected:
      ImageFormatter* _imageFormatter;

      virtual void prepareNtImage(ImageProviderBase& provider, WinNtExecutableImage& image, PlatformType uiType);

      void writeDOSStub(path_t appPath, FileWriter& file);
      void writeExecutableHeader(WinNtExecutableImage& image, FileWriter& file);
      virtual void writeNtHeader(WinNtExecutableImage& image, FileWriter& file) = 0;
      void writeSections(WinNtExecutableImage& image, FileWriter& file);

      bool createExecutable(WinNtExecutableImage& image, path_t exePath, path_t basePath);
      bool createDebugFile(ImageProviderBase& provider, WinNtExecutableImage& image, path_t debugFilePath);

   public:
      LinkResult run(ProjectBase& project, ImageProviderBase& code, PlatformType uiType, path_t exeExtension) override;

      WinNtLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : LinkerBase(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };

}

#endif // NTLINKER_H