//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader.
//		Supported platforms: x86
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "syslibloader.h"

using namespace _ELENA_;

#include <windows.h>

// --- SysLibraryLoader ---

SysLibraryLoader :: SysLibraryLoader(path_t libraryPath)
{
   _handle = LoadLibraryW(libraryPath);
}

void* SysLibraryLoader :: loadFunction(const char* name)
{
   return GetProcAddress((HINSTANCE)_handle, name);
}
