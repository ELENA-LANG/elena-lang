//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class body
//		Supported platforms: Linux ARM64
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elfarmlinker64.h"
#include "elfcommon.h"
#include "elf.h"

constexpr auto ARM_SECTION_ALIGNMENT = 0x10000;

constexpr auto ARM_INTERPRETER64_PATH = "/lib/ld-linux-aarch64.so.1";

using namespace elena_lang;

// --- ElfARM64Linker ---

ustr_t ElfARM64Linker :: getInterpreter()
{
   return ARM_INTERPRETER64_PATH;
}

int ElfARM64Linker :: getMachine()
{
   return EM_AARCH64;
}

void ElfARM64Linker :: prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize)
{
   image.sectionAlignment = ARM_SECTION_ALIGNMENT;
   image.flags = 2;

   Elf64Linker::prepareElfImage(provider, image, headerSize);
}