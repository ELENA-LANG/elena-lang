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
   constexpr auto DEBUG_IDENTIFIER_LEN = 0x0300;

   typedef int threadid_t;

   // --- DebugProcessException ---
   struct DebugProcessException
   {
      int   code;
      addr_t address;
   };

   // --- DebugProcessBase ---
   class DebugProcessBase
   {
   public:
      virtual bool readDump(addr_t address, char* s, pos_t length) = 0;

      virtual void addStep(addr_t address, void* current) = 0;

      virtual void setBreakpoint(addr_t address, bool withStackLevelControl) = 0;

      virtual ~DebugProcessBase() = default;
   };
}

#endif // LDBG_COMMON_H