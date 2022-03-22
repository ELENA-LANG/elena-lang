//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Executive Linker class body
//		Supported platforms: Linux PPC64le
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elfppclinker64.h"
#include "elfcommon.h"
#include "elf.h"

constexpr auto PPC_SECTION_ALIGNMENT = 0x10000;

constexpr auto PPC_INTERPRETER64_PATH = "/lib64/ld64.so.2";

using namespace elena_lang;

// --- ElfPPC64leLinker ---

ustr_t ElfPPC64leLinker :: getInterpreter()
{
   return PPC_INTERPRETER64_PATH;
}

int ElfPPC64leLinker :: getMachine()
{
   return EM_PPC64;
}

void ElfPPC64leLinker :: prepareElfImage(ImageProviderBase& provider, ElfExecutableImage& image, unsigned int headerSize)
{
   image.sectionAlignment = PPC_SECTION_ALIGNMENT;
   image.flags = 2;

   Elf64Linker::prepareElfImage(provider, image, headerSize);
}