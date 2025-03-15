//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class declaration
//		Supported platforms: MacOS 64
//                                              (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef MACHOLINKER64_H
#define MACHOLINKER64_H

#include "macholinker.h"

namespace elena_lang
{
   // --- ElfLinker ---
   class MachOLinker64 : public MachOLinker
   {
   protected:

   public:
      MachOLinker64(ErrorProcessorBase* errorProcessor/*, ImageFormatter* imageFormatter*/)
         : MachOLinker(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
