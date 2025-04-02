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
   struct MachOHeader_64
   {
      uint32_t       magic;

      CPUType        cputype;

      CPUSubType     cpusubtype;

      uint32_t       filetype;

      uint32_t       ncmds;

      uint32_t       sizeofcmds;

      uint32_t       flags;

      uint32_t       reserved;
   };


   // --- MachOLinker64 ---
   class MachOLinker64 : public MachOLinker
   {
   protected:
      unsigned long getMagicNumber() override
      {
         return MH_MAGIC_64;
      }

      void writeMachOHeader(MachOExecutableImage& image, FileWriter* file) override;
      void writeCommand(MachOExecutableImage& image, FileWriter* file, Command* command) override;

   public:
      MachOLinker64(ErrorProcessorBase* errorProcessor/*, ImageFormatter* imageFormatter*/)
         : MachOLinker(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
