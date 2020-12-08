//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win64 ELENA System Routines
//
//                                              (C)2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"

using namespace _ELENA_;

void SystemRoutineProvider::InitCriticalStruct(CriticalStruct* header, uintptr_t criticalHandler)
{
   //uintptr_t previousHeader = 0;

   //// ; set SEH handler / frame / stack pointers
   //__asm {
   //   mov  rax, header
   //   mov  rcx, fs: [0]
   //   mov  previousHeader, rcx
   //   mov  fs : [0] , rax
   //}

   //header->previousStruct = previousHeader;
   //header->handler = criticalHandler;
}
