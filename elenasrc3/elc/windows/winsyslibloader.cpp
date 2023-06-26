//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader declaration.
//		Supported platforms: x86, x86-64
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "winsyslibloader.h"

#include <windows.h>

using namespace elena_lang;

// --- WinSysLibraryLoader ---

WinSysLibraryLoader :: WinSysLibraryLoader(path_t libraryPath)
{
   _handle = LoadLibraryW(libraryPath);
}

void* WinSysLibraryLoader :: loadFunction(const char* name)
{
   return GetProcAddress((HINSTANCE)_handle, name);
}
