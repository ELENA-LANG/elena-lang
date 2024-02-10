//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader declaration.
//		Supported platforms: Linux
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELF_SYSIBLOADER_H
#define ELF_SYSIBLOADER_H

#include "clicommon.h"

namespace elena_lang
{

// --- ElfSysLibraryLoader ---
class ElfSysLibraryLoader : public SysLibraryLoaderBase
{
   void* _handle;

public:
   void* loadFunction(const char* name) override;

   ElfSysLibraryLoader(path_t libraryPath);
};
   
}

#endif
