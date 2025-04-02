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

   struct SegmentCommand_64 : public Command
   {  
      char       segname[16];   /* segment name */

      uint64_t   vmaddr;        /* memory address of this segment */

      uint64_t   vmsize;        /* memory size of this segment */

      uint64_t   fileoff;       /* file offset of this segment */

      uint64_t   filesize;      /* amount to map from the file */

      vm_prot_t  maxprot;       /* maximum VM protection */

      vm_prot_t  initprot;      /* initial VM protection */

      uint32_t   nsects;        /* number of sections in segment */

      uint32_t   flags;         /* flags */
   };

   // --- MachOLinker64 ---
   class MachOLinker64 : public MachOLinker
   {
   protected:
      unsigned long getMagicNumber() override
      {
         return MH_MAGIC_64;
      }

      Command* createSegmentCommand(ImageSectionHeader& header, pos_t& fileOffset) override;

      void writeMachOHeader(MachOExecutableImage& image, FileWriter* file) override;

   public:
      MachOLinker64(ErrorProcessorBase* errorProcessor/*, ImageFormatter* imageFormatter*/)
         : MachOLinker(errorProcessor)
      {
         _imageFormatter = imageFormatter;
      }
   };
}

#endif
