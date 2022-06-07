//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: Win32
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef NTLINKER32_H
#define NTLINKER32_H

#include "ntlinker.h"

namespace elena_lang
{

class Win32NtLinker : public WinNtLinker
{
protected:
   void prepareNtImage(ImageProviderBase& provider, WinNtExecutableImage& image) override;

   void writeNtHeader(WinNtExecutableImage& image, FileWriter& file) override;

public:
   Win32NtLinker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
      : WinNtLinker(errorProcessor, imageFormatter)
   {
   }
};

}

#endif // LINKER32_H