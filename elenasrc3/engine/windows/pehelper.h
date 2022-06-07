//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA PEHelper.
//		Supported platforms: x86
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PEHELPER_H
#define PEHELPER_H

namespace elena_lang
{

class PEHelper
{
public:
   static addr_t findEntryPoint(path_t path);

   static bool seekSection(StreamReader& reader, const char* name, addr_t& rvaAddress);
   static bool seekSection64(StreamReader& reader, const char* name, addr_t& rvaAddress);
};

} // _ELENA_

#endif // pehelperH
