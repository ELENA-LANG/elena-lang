//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains the main body of the win32 / win64 
//    ELENA Debugger Adapter
//
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ldbg_session.h"

using namespace elena_lang;

int main()
{
   DPASessionWrapper session;

   session.prepare();

   return 0;
}