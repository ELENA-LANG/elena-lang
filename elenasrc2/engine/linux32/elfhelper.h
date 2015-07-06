//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ELFHelper.
//		Supported platforms: Linux I386
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elfhelperH
#define elfhelperH 1

namespace _ELENA_
{

class ELFHelper
{
public:
   static bool seekDebugSegment(StreamReader& reader, size_t& rvaAddress);
};

} // _ELENA_

#endif // elfhelperH
