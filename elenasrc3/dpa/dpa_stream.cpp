//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Debugger Adapater
//
//		This file contains the DPA I/O class implementations
// 
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "dpa_stream.h"

using namespace dpa;

ContentReader :: ~ContentReader()
{

}

bool ContentReader :: isOpen()
{
   return false;
}

void ContentReader :: close()
{

}
