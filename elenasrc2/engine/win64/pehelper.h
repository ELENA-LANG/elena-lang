//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper.
//		Supported platforms: x86
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef pehelperH
#define pehelperH 1

namespace _ELENA_
{

class PEHelper
{
public:
   static size_t findEntryPoint(path_t path);

   static bool seekSection(StreamReader& reader, char* name, size_t& rvaAddress);
   static bool seekSection64(StreamReader& reader, char* name, size_t& rvaAddress);
};

} // _ELENA_

#endif // pehelperH
