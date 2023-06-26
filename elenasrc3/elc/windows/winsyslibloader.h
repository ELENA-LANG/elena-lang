//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader declaration.
//		Supported platforms: x86, x86-64
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN_SYSIBLOADER_H
#define WIN_SYSIBLOADER_H

#include "clicommon.h"

namespace elena_lang
{

// --- WinSysLibraryLoader ---
class WinSysLibraryLoader : public SysLibraryLoaderBase
{
   void* _handle;

public:
   void* loadFunction(const char* name) override;

   WinSysLibraryLoader(path_t libraryPath);
};
   
}

#endif

