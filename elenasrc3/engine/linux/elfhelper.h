//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ELFHelper.
//		Supported platforms: Linux I386
//                                                  (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELFHELPER_H
#define ELFHELPER_H

namespace elena_lang
{

class ELFHelper
{
public:
   static addr_t findEntryPoint(path_t path);

   static bool seekRODataSegment(StreamReader& reader, addr_t& rvaAddress);
};

}

#endif // elfhelperH
