//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger class header
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LDBG_COMMON_H
#define LDBG_COMMON_H

namespace elena_lang
{
   // --- DebugProcessException ---
   struct DebugProcessException
   {
      int   code;
      addr_t address;
   };
}

#endif // LDBG_COMMON_H