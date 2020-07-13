//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader.
//		Supported platforms: Linux x86
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syslibloader.h"
#include <dlfcn.h>

using namespace _ELENA_;

// --- SysLibraryLoader ---

SysLibraryLoader :: SysLibraryLoader(path_t libraryPath)
{
   _handle = dlopen (libraryPath.c_str(), RTLD_LAZY);
}

void* SysLibraryLoader :: loadFunction(const char* name)
{
   return dlsym(_handle, name);
}
