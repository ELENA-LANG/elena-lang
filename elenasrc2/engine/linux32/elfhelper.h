//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA ELFHelper.
//		Supported platforms: Linux I386
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elfhelperH
#define elfhelperH 1

namespace _ELENA_
{

class ELFHelper
{
public:
   static size_t findEntryPoint(path_t path);

   static bool seekRDataSegment(StreamReader& reader, size_t& rvaAddress);
   static bool seekDebugSegment(StreamReader& reader, size_t& rvaAddress);
};

} // _ELENA_

#endif // elfhelperH
