//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains Common MachO types
//                                              (C)2025, by Aleksey Rakov
//                                              (C)2026, by Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef MACHOCOMMON_H
#define MACHOCOMMON_H

#include <mach-o/loader.h>

namespace elena_lang
{
   constexpr auto SECTION_ALIGNMENT = 0x1000;
   constexpr auto ARM64_SECTION_ALIGNMENT = 0x4000;
   constexpr auto FILE_ALIGNMENT = 0x0010;

   constexpr auto __PAGEZERO_SEGMENT = "__PAGEZERO";
   constexpr auto __TEXT_SEGMENT = "__TEXT";
   constexpr auto __DATA_CONST_SEGMENT = "__DATA_CONST";
   constexpr auto __DATA_SEGMENT = "__DATA";
   constexpr auto __LINKEDIT_SEGMENT = "__LINKEDIT";

   constexpr addr_t MACHO64_IMAGE_BASE = 0x100000000;

   constexpr int MacOS_11_0_0 = 0x000B0000;

   constexpr size_t MaxLoadCommandPath = 256;

   struct Command
   {
      MemoryDump image;

      template<class T> T* as()
      {
         return static_cast<T*>(image.get(0));
      }

      void* bytes() const
      {
         return image.get(0);
      }

      pos_t size() const
      {
         auto command = static_cast<load_command*>(image.get(0));

         return command ? command->cmdsize : 0;
      }

      Command(pos_t capacity)
         : image(capacity)
      {
         image.writeBytes(0, 0, capacity);
      }
   };

   typedef List<Command*, freeobj> Commands;

   constexpr int PROT_R = 1;
   constexpr int PROT_W = 2;
   constexpr int PROT_X = 4;
}

#endif
