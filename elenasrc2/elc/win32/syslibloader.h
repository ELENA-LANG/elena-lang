//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA DLL loader declaration.
//		Supported platforms: x86
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef win32dllLoaderH
#define win32dllLoaderH 1

namespace _ELENA_
{

class SysLibraryLoader
{
   void* _handle;

public:
   void* loadFunction(const char* name);

   SysLibraryLoader(path_t libraryPath);
};

}

#endif // win32dllLoaderH
