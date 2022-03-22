//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Win64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef NTLINKER64_H
#define NTLINKER64_H

#include "ntlinker.h"

namespace elena_lang
{
   class Win64NtLinker : public WinNtLinker
   {
   protected:
      void prepareNtImage(ImageProviderBase& provider, WinNtExecutableImage& image) override;

      void writeNtHeader(WinNtExecutableImage& image, FileWriter& file) override;

   public:
      Win64NtLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : WinNtLinker(errorProcessor, imageFormatter)
      {         
      }
   };

}

#endif
