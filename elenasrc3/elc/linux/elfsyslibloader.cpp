//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader declaration.
//		Supported platforms: Linux
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elfsyslibloader.h"
#include <dlfcn.h>

using namespace elena_lang;

// --- ElfSysLibraryLoader ---

ElfSysLibraryLoader :: ElfSysLibraryLoader(path_t libraryPath)
{
   _handle = dlopen(libraryPath.str(), RTLD_LAZY);
}

void* ElfSysLibraryLoader :: loadFunction(const char* name)
{
   return dlsym(_handle, name);
}
