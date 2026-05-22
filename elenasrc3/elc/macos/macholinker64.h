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
   // --- MachOLinker64 ---
   class MachOLinker64 : public MachOLinker
   {
   protected:
      unsigned long getMagicNumber() override
      {
         return MH_MAGIC_64;
      }

      Command* createSegmentCommand(ImageSectionHeader& header, int headerIndex,
         ImageSections& sections, AddressSpace& addressMap) override;

      void writeMachOHeader(MachOExecutableImage& image, StreamWriter* file) override;

   public:
      MachOLinker64(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : MachOLinker(errorProcessor, imageFormatter)
      {
      }
   };

   // --- MachOAmd64Linker ---
   class MachOAmd64Linker : public MachOLinker64
   {
   protected:
      cpu_type_t getCPUType() override
      {
         return CPU_TYPE_X86;
      }

      cpu_subtype_t getCPUSubType() override
      {
         return CPU_SUBTYPE_X86_ALL;
      }

   public:
      MachOAmd64Linker(ErrorProcessorBase* errorProcessor, ImageFormatter* imageFormatter)
         : MachOLinker64(errorProcessor, imageFormatter)
      {

      }
   };
}

#endif
